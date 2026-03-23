/*
 * Copyright (c) @CompanyNameMagicTag. 2022. All rights reserved.
 */

#include "lv_dfx_cmd.h"
#include <unistd.h>
#include <stdlib.h>
#include "lv_refr.h"
#include "lv_snapshot.h"
#include "lv_touch_injector.h"
#include "lv_key_injector.h"
#include "lv_rotate_injector.h"
#include "fcntl.h"
#if LV_USE_DFX_TRACE
#include "lv_dfx_trace.h"
#endif
struct LvDfxCmd {
    char* cmdType;
    void (*func)(int argc, char* argv[]);
};

static struct LvDfxCmd g_lvDfxCmdList[] = {
#if LV_USE_DFX_CMD
#if LV_USE_SNAPSHOT_EXT
    {"screencap", Screencap},
#endif
#if LV_USE_PERF_MONITOR_EXT
    {"printperf", PrintPerf},
    {"showperf", ShowPerf},
    {"hideperf", HidePerf},
#endif
    {"inject_key", InjectKey},
    {"inject_key_lp", InjectKeyLongPress},
    {"inject_click", InjectClick},
    {"inject_drag", InjectDrag},
    {"inject_lp", InjectLongPress},
    {"inject_rotate", InjectRotate},
#if LV_USE_DFX_TRACE
    {"showtime", ShowTime},
#endif // LV_USE_DFX_TRACE
    {"dumpmem", DumpMemory},
    {"mem_monitor", SetMemMonitor},
#endif
};

bool ExecLvglDfxCmd(int argc, char* argv[])
{
    if (argc < 1) {
        LV_LOGE("Invalid Input. argc: %d, argv: %s\n", argc, argv);
        return false;
    }
    for (uint16_t i = 0; i < (sizeof(g_lvDfxCmdList) / sizeof(g_lvDfxCmdList[0])); i++) {
        if (strcmp(g_lvDfxCmdList[i].cmdType, argv[0]) == 0) {
            if (argc == 1) {
                g_lvDfxCmdList[i].func(0, NULL);
            } else {
                g_lvDfxCmdList[i].func(argc - 1, &argv[1]);
            }
            return true;
        }
    }
    LV_LOGE("Cmd not found. argc: %d, argv: %s\n", argc, argv[0]);
    return false;
}

#if LV_USE_DFX_CMD
static char* g_filePath = NULL;
static lv_area_t g_capArea = {0};
#define DEFAULT_FILE_PERMISSION 0666

static bool ConvertToInt(const char* input, uint16_t* value)
{
    if (input == NULL) {
        return false;
    }

    char* end = NULL;
    long temp = strtol(input, &end, 0);
    if ((end == input) || (temp < 0)) {
        LV_LOGE("Invalid input value: %s.", input);
        return false;
    }
    *value = (uint16_t)temp;
    return true;
}

/*********************
 *      PERF
 *********************/
#if LV_USE_PERF_MONITOR_EXT
bool PrintPerf(int argc, char* argv[])
{
    if (argc != 1) {
        return false;
    }
    uint16_t count = 0;
    if (ConvertToInt(argv[0], &count)) {
        LvSetPrintPerfCount(count);
        return true;
    }
    return false;
}

void ShowPerf(int argc, char* argv[])
{
    (void)argc;
    (void)argv;
    LvSetDisplayPerfFlag(true);
}

void HidePerf(int argc, char* argv[])
{
    (void)argc;
    (void)argv;
    LvSetDisplayPerfFlag(false);
}
#endif

/*********************
 *      Screencap
 *********************/
#if LV_USE_SNAPSHOT_EXT
static void SetFilePath(const char* path)
{
    if (path == NULL) {
        return;
    }
    if (g_filePath != NULL) {
        lv_mem_free(g_filePath);
        g_filePath = NULL;
    }
    size_t pathLength = strlen(path);
    char* destPath = (char*)(lv_mem_alloc(pathLength + 1));
    if (destPath == NULL) {
        return;
    }

    if (memcpy_s(destPath, pathLength + 1, path, pathLength) != 0) {
        lv_mem_free((void*)(destPath));
        return;
    }
    destPath[pathLength] = '\0';
    g_filePath = destPath;
}

static bool IsRectValid(lv_area_t area)
{
    return ((area.x2 >= area.x1) && (area.y2 >= area.y1) && ((area.x2 - area.x1 + area.y2 - area.y1) != 0));
}

bool OutputDscToFile(lv_img_dsc_t* dsc, const char* filePath)
{
    unlink(filePath);
    char* absolutePath = realpath(filePath, NULL);
    bool checkOK = true;
    if (absolutePath == NULL) {
        absolutePath = filePath;
        checkOK = false;
    }
    int32_t fd = open(absolutePath, O_RDWR | O_CREAT, DEFAULT_FILE_PERMISSION);
    if (checkOK) {
        free(absolutePath);
        absolutePath = NULL;
    }
    bool ret = true;
    if (fd < 0) {
        LV_LOGE("open file failed.\n");
        ret = false;
    }
    if (ret && (write(fd, dsc->data, dsc->data_size) != (int32_t)dsc->data_size)) {
        LV_LOGE("write failed: data_size = %u.\n", dsc->data_size);
        ret = false;
    }
    if (fd >= 0) {
        close(fd);
    }
    LV_LOGI("OutputDscToFile result: %d\n", ret);
    return ret;
}

static bool HandleCapRect(char* capRect, lv_area_t* capArea)
{
    uint16_t left = 0;
    uint16_t top = 0;
    uint16_t width = 0;
    uint16_t height = 0;
    char* temp = NULL;
    if (!ConvertToInt(strtok_r(capRect, ",", &temp), &left) ||
        !ConvertToInt(strtok_r(NULL, ",", &temp), &top) ||
        !ConvertToInt(strtok_r(NULL, ",", &temp), &width) ||
        !ConvertToInt(strtok_r(NULL, ",", &temp), &height)) {
        LV_LOGE("Please input capture rect in correct format: x,y,w,h.\n");
        return false;
    }
    lv_disp_t* default_disp = lv_disp_get_default();
    lv_coord_t screenWidth = lv_disp_get_hor_res(default_disp);
    lv_coord_t screenHeight = lv_disp_get_ver_res(default_disp);
    if (((left + width) > screenWidth) || ((top + height) > screenHeight)) {
        LV_LOGE("Input rect exceeds the screen size. Screen: width = %d, height = %d.\n",
            screenWidth, screenHeight);
        return false;
    }
    lv_area_t tempArea = {left, top, left + width - 1, top + height - 1};
    if (!IsRectValid(tempArea)) {
        LV_LOGE("Invalid Rect!\n");
        return false;
    }
    capArea->x1 = (lv_coord_t)left;
    capArea->x2 = (lv_coord_t)(left + width - 1);
    capArea->y1 = (lv_coord_t)top;
    capArea->y2 = (lv_coord_t)(top + height - 1);
    return true;
}

/* screencap -c 0,0,100,100 -f /bin/vs/sd0p0/rect.bin */
bool Screencap(int argc, char* argv[])
{
    // 4: argument length; 2: argument index
    if ((argc != 4) || (strcmp(argv[0], "-c") != 0) || (strcmp(argv[2], "-f") != 0)) {
        LV_LOGE("Invalid arg.\n");
        return false;
    }

    char* capRect = argv[1];
    if (!HandleCapRect(capRect, &g_capArea)) {
        LV_LOGE("Handle cap rect failed.\n");
        return false;
    }

    SetFilePath(argv[3]); // 3: index
    return true;
}

static void ResetScreencapParam(void)
{
    lv_mem_free(g_filePath);
    g_filePath = NULL;
    memset_s(&g_capArea, sizeof(lv_area_t), 0, sizeof(lv_area_t));
}

bool ScreencapToFile(void)
{
    if ((g_filePath == NULL) || (lv_area_get_size(&g_capArea) == 0)) {
        return false;
    }

    lv_img_dsc_t* dsc = LvSnapshotByArea(g_capArea);
    if (dsc == NULL) {
        LV_LOGE("LvSnapshotByArea failed.\n");
        ResetScreencapParam();
        return false;
    }

    if (!OutputDscToFile(dsc, g_filePath)) {
        LV_LOGE("OutputDscToFile failed.\n");
        LvFreeBuffer(dsc->data);
        lv_mem_free(dsc);
        ResetScreencapParam();
        return false;
    }
    LvFreeBuffer(dsc->data);
    lv_mem_free(dsc);

    ResetScreencapParam();
    return true;
}
#endif

/*********************
 *      event injector
 *********************/
/* param: x y x1 y1 time */
bool InjectDrag(int argc, char* argv[])
{
    if (argc != 5) { // 5: argument length
        LV_LOGE("Invalid parameter length.\n");
        return false;
    }
    uint16_t x = 0;
    uint16_t y = 0;
    uint16_t x1 = 0;
    uint16_t y1 = 0;
    uint16_t time = 0; // 0: release, 1:pressed
    if (!ConvertToInt(argv[0], &x) || !ConvertToInt(argv[1], &y) || !ConvertToInt(argv[2], &x1) || // 2: index
        !ConvertToInt(argv[3], &y1) || !ConvertToInt(argv[4], &time)) { // 3, 4: index
        LV_LOGE("Invalid Parameter.\n");
        return false;
    }
    if ((x >= lv_disp_get_hor_res(lv_disp_get_default())) || (y >= lv_disp_get_ver_res(lv_disp_get_default())) ||
        (x1 >= lv_disp_get_hor_res(lv_disp_get_default())) || (y1 >= lv_disp_get_ver_res(lv_disp_get_default()))) {
        LV_LOGE("x or y is greater than the screen size.\n");
        return false;
    }

    uint16_t count = time / LV_INDEV_DEF_READ_PERIOD + 1;
    if (count < 3) { // 3: min
        LV_LOGE("DragTime is too short.\n");
        return false;
    }

    int16_t posX = (int16_t)x;
    int16_t posY = (int16_t)y;
    int16_t posX1 = (int16_t)x1;
    int16_t posY1 = (int16_t)y1;
    lv_point_t point = {posX, posY};
    if (posX == posX1) {
        float step = (float)(posY1 - posY) / (count - 1);
        for (uint16_t i = 0; i < count - 1; i++) {
            point.y = (lv_coord_t)(posY + i * step);
            LvTouchInjectorPush(point, LV_INDEV_STATE_PRESSED);
        }
    } else {
        float slope = (float)(posY1 - posY) / (float)(posX1 - posX);
        float xStep = (float)(posX1 - posX) / (count - 1);
        for (uint16_t i = 0; i < count - 1; i++) {
            point.x = (lv_coord_t)(posX + i * xStep);
            point.y = (lv_coord_t)(posY + i * xStep * slope);
            LvTouchInjectorPush(point, LV_INDEV_STATE_PRESSED);
        }
    }
    point.x = posX1;
    point.y = posY1;
    LvTouchInjectorPush(point, LV_INDEV_STATE_RELEASED);
    return true;
}

/* param: x y */
bool InjectClick(int argc, char* argv[])
{
    if (argc != 2) { // 2: argument length
        LV_LOGE("Invalid parameter length.\n");
        return false;
    }
    uint16_t x = 0;
    uint16_t y = 0;
    if (!ConvertToInt(argv[0], &x) || !ConvertToInt(argv[1], &y)) {
        LV_LOGE("Invalid Parameter.\n");
        return false;
    }
    if ((x >= lv_disp_get_hor_res(lv_disp_get_default())) || (y >= lv_disp_get_ver_res(lv_disp_get_default()))) {
        LV_LOGE("x or y is greater than the screen size.\n");
        return false;
    }
    lv_point_t point = {x, y};
    LvTouchInjectorPush(point, LV_INDEV_STATE_PRESSED);
    LvTouchInjectorPush(point, LV_INDEV_STATE_RELEASED);
    return true;
}

/* param: x y */
bool InjectLongPress(int argc, char* argv[])
{
    if (argc != 2) { // 2: argument length
        LV_LOGE("Invalid parameter length.\n");
        return false;
    }
    uint16_t x = 0;
    uint16_t y = 0;
    if (!ConvertToInt(argv[0], &x) || !ConvertToInt(argv[1], &y)) {
        LV_LOGE("Invalid Parameter.\n");
        return false;
    }
    if ((x >= lv_disp_get_hor_res(lv_disp_get_default())) || (y >= lv_disp_get_ver_res(lv_disp_get_default()))) {
        LV_LOGE("x or y is greater than the screen size.\n");
        return false;
    }

    uint16_t count = LV_INDEV_DEF_LONG_PRESS_TIME / LV_INDEV_DEF_READ_PERIOD + 1;
    if (count < 2) { // 2: min
        LV_LOGE("Long press time is too short.\n");
        return false;
    }
    lv_point_t point = {x, y};
    for (uint16_t i = 0; i < count - 1; i++) {
        LvTouchInjectorPush(point, LV_INDEV_STATE_PRESSED);
    }
    LvTouchInjectorPush(point, LV_INDEV_STATE_RELEASED);
    return true;
}

/* param: key [0|1] */
bool InjectKey(int argc, char* argv[])
{
    if (argc != 2) { // 2: argument length
        LV_LOGE("Invalid parameter length.\n");
        return false;
    }
    uint16_t key = 0; // refers to lv_key_t in lv_group.h.
    uint16_t state = 0; // 0: release, 1:pressed
    if (!ConvertToInt(argv[0], &key) || !ConvertToInt(argv[1], &state)) {
        LV_LOGE("Invalid Parameter.\n");
        return false;
    }
    if (state > LV_INDEV_STATE_PRESSED) {
        LV_LOGE("Invalid state.\n");
        return false;
    }
    LvKeyInjectorPush(key, (lv_indev_state_t)state);
    return true;
}

/* param: key */
bool InjectKeyLongPress(int argc, char* argv[])
{
    if (argc != 1) {
        LV_LOGE("Invalid parameter length.\n");
        return false;
    }
    uint16_t key = 0; // refers to lv_key_t in lv_group.h.
    if (!ConvertToInt(argv[0], &key)) {
        LV_LOGE("Invalid Parameter.\n");
        return false;
    }
    uint16_t count = LV_INDEV_DEF_LONG_PRESS_TIME / LV_INDEV_DEF_READ_PERIOD + 1;
    if (count < 2) { // 2: min
        LV_LOGE("Long press time is too short.\n");
        return false;
    }
    for (uint16_t i = 0; i < count - 1; i++) {
        LvKeyInjectorPush(key, LV_INDEV_STATE_PRESSED);
    }
    LvKeyInjectorPush(key, LV_INDEV_STATE_RELEASED);
    return true;
}

/* param: rotate */
bool InjectRotate(int argc, char* argv[])
{
    if (argc != 1) { // 1: argument length
        LV_LOGE("Invalid parameter length.\n");
        return false;
    }

    char* end = NULL;
    long rotate = strtol(argv[0], &end, 0);
    if (end == argv[0]) {
        LV_LOGE("Invalid input value: %s.\n", argv[0]);
        return false;
    }
    LvRotateInjectorPush((int16_t)rotate);
    return true;
}

#if LV_USE_DFX_TRACE
bool ShowTime(int argc, char *argv[])
{
    if (argc == 1) {
        if (strcmp(argv[0], "start") == 0) {
            EnablePeriodTrace(true);
        } else if (strcmp(argv[0], "frame") == 0) {
            EnableFrameTrace(true);
        } else if (strcmp(argv[0], "end") == 0) {
            EnablePeriodTrace(false);
        }
        return true;
    } else {
        LV_LOGE("param err.\n");
        return false;
    }
}
#endif

void DumpMemory(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    LvDumpMem();
}

bool SetMemMonitor(int argc, char *argv[])
{
    if (argc != 1) {
        LV_LOGE("param err.\n");
        return false;
    }
    if (strcmp(argv[0], "0") == 0) {
        LvSetMemMonitor(false);
    } else if (strcmp(argv[0], "1") == 0) {
        LvSetMemMonitor(true);
    } else {
        LV_LOGE("param err.\n");
        return false;
    }
    return true;
}
#endif

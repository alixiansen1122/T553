/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#include "lv_demo_rainbow.h"
#include <stdio.h>
#include "lv_conf.h"
#include "lv_canvas_ext.h"
#include "lv_demo_ext.h"

#if LV_USE_DEMO_RAINBOW

#define RAINBOW_PATH_NUM 16
static int16_t g_pathIds[RAINBOW_PATH_NUM] = {0};
static LvPath g_paths[RAINBOW_PATH_NUM] = {{0}};
static LvPaint g_paints[RAINBOW_PATH_NUM] = {{0}};
static uint32_t g_colors[RAINBOW_PATH_NUM] = {
    0xffA4C400, 0xff1ba1e2, 0xffF472D0, 0xff92A867,
    0xffff0000, 0xffffa500, 0xffffff00, 0xff008000, 0xff0000ff, 0xff4b0082, 0xffee82ee,
    0xff92B8E7, 0xffC9B0B9, 0xffEDD46C, 0xffE4A3A7, 0xffFFFFCC
};
static LvVector2 g_scrollEndDataOffset[RAINBOW_PATH_NUM] = {0};
static lv_coord_t g_scrollStartX = 0;
static lv_coord_t g_scrollStartY = 0;
static uint8_t g_controlXIndex = 2;
static uint8_t g_controlYIndex = 3;
static bool g_isScrollEnabled = false;

static const float CURVE_COEF = 1.5f;
static const float PATH_START_X = 100;
static const float PATH_START_Y = 100;
static const float PATH_END_Y = 350;
static const lv_coord_t PATH_WIDTH = 10;
static const lv_coord_t PATH_GAP = 10;
static const uint8_t FIRST_CONTROL_X_INDEX = 2;
static const uint8_t FIRST_CONTROL_Y_INDEX = 3;
static const uint8_t SECOND_CONTROL_X_INDEX = 4;
static const uint8_t SECOND_CONTROL_Y_INDEX = 5;
static const float FIRST_CONTROL_Y_OFFSET = (PATH_END_Y - PATH_START_Y) / 3;
static const float SECOND_CONTROL_Y_OFFSET = (PATH_END_Y - PATH_START_Y) / 3 * 2;

static bool g_isInited = false;

static void InitPaint(LvPaint* paint, uint32_t color)
{
    paint->style = STROKE_STYLE;
    paint->strokeAttr.capType = CAP_ROUND;
    paint->strokeAttr.shader.color = color;
    paint->strokeAttr.strokeWidth = (uint16_t)PATH_WIDTH;
    paint->isAntialiased = true;
}

static bool InitSinglePath(LvPath* path, float startX)
{
    path->cmdNum = 2; // 2: cmdNum
    path->dataNum = 8; // 8: dataNum
    path->cmd = (uint8_t*)LvAllocBufferForPath(sizeof(uint8_t) * path->cmdNum);
    if (path->cmd == NULL) {
        return false;
    }
    path->data = (float*)LvAllocBufferForPath(sizeof(float) * path->dataNum);
    if (path->data == NULL) {
        LvFreeBuffer(path->cmd);
        path->cmd = NULL;
        return false;
    }
    path->cmd[0] = CMD_MOVE_TO;
    path->cmd[1] = CMD_CUBIC_TO;

    path->data[0] = startX;
    path->data[1] = PATH_START_Y;
    path->data[2] = startX; // 2: index
    path->data[3] = PATH_START_Y + FIRST_CONTROL_Y_OFFSET; // 3: index
    path->data[4] = startX; // 4: index
    path->data[5] = PATH_START_Y + SECOND_CONTROL_Y_OFFSET; // 5: index
    path->data[6] = startX; // 6: index
    path->data[7] = PATH_END_Y; // 7: index
    return true;
}

static bool InitRainbowPaths(void)
{
    for (uint8_t i = 0; i < RAINBOW_PATH_NUM; i++) {
        if (!InitSinglePath(&g_paths[i], PATH_START_X + i * (PATH_WIDTH + PATH_GAP))) {
            return false;
        }
        InitPaint(&g_paints[i], g_colors[i]);
    }
    return true;
}

static void ScrollEndExecCb(void* var, int32_t v)
{
    float ratio = v / 100.0f;
    for (uint8_t i = 0; i < RAINBOW_PATH_NUM; i++) {
        float originX = g_paths[i].data[0];
        float originY = PATH_START_Y + FIRST_CONTROL_Y_OFFSET;
        if (g_controlYIndex != FIRST_CONTROL_Y_INDEX) {
            originY = PATH_START_Y + SECOND_CONTROL_Y_OFFSET;
        }
        g_paths[i].data[g_controlXIndex] = originX + g_scrollEndDataOffset[i].x * ratio;
        g_paths[i].data[g_controlYIndex] = originY + g_scrollEndDataOffset[i].y * ratio;
        LvCanvasExtUpdatePathData((lv_obj_t*)var, &g_paths[i], g_pathIds[i]);
    }
    lv_obj_invalidate(var);
}

static void HandleScrollBeginEvent(lv_obj_t* canvas, const lv_indev_t* indev)
{
    if ((indev == NULL) || (indev->proc.state != LV_INDEV_STATE_PRESSED)) {
        return;
    }
    lv_anim_t* anim = lv_anim_get(canvas, ScrollEndExecCb);
    if ((anim != NULL) && (anim->act_time <= anim->time)) {
        return;
    }

    g_scrollStartX = indev->proc.types.pointer.act_point.x;
    g_scrollStartY = indev->proc.types.pointer.act_point.y;

    if (LV_ABS(g_scrollStartY - (PATH_START_Y + FIRST_CONTROL_Y_OFFSET)) >
        LV_ABS(g_scrollStartY - (PATH_START_Y + SECOND_CONTROL_Y_OFFSET))) {
        g_controlXIndex = SECOND_CONTROL_X_INDEX;
        g_controlYIndex = SECOND_CONTROL_Y_INDEX;
    } else {
        g_controlXIndex = FIRST_CONTROL_X_INDEX;
        g_controlYIndex = FIRST_CONTROL_Y_INDEX;
    }

    g_isScrollEnabled = true;
}

static void HandleScrollEvent(lv_obj_t* canvas, const lv_indev_t* indev)
{
    if (!g_isScrollEnabled) {
        return;
    }
    if (indev == NULL) {
        return;
    }
    lv_point_t vect;
    lv_indev_get_vect(indev, &vect);
    for (uint8_t i = 0; i < RAINBOW_PATH_NUM; i++) {
        g_paths[i].data[g_controlXIndex] = g_paths[i].data[g_controlXIndex] + vect.x * CURVE_COEF;
        g_paths[i].data[g_controlYIndex] = g_paths[i].data[g_controlYIndex] + vect.y * CURVE_COEF;
        LvCanvasExtUpdatePathData(canvas, &g_paths[i], g_pathIds[i]);
    }
    lv_obj_invalidate(canvas);
}

static void HandleScrollEndEvent(lv_obj_t* canvas, const lv_indev_t* indev)
{
    if (!g_isScrollEnabled) {
        return;
    }
    if ((indev != NULL) && (indev->proc.state == LV_INDEV_STATE_PRESSED)) {
        return;
    }

    for (uint8_t i = 0; i < RAINBOW_PATH_NUM; i++) {
        float originX = g_paths[i].data[0];
        float originY = PATH_START_Y + FIRST_CONTROL_Y_OFFSET;
        if (g_controlYIndex != FIRST_CONTROL_Y_INDEX) {
            originY = PATH_START_Y + SECOND_CONTROL_Y_OFFSET;
        }
        g_scrollEndDataOffset[i].x = g_paths[i].data[g_controlXIndex] - originX;
        g_scrollEndDataOffset[i].y = g_paths[i].data[g_controlYIndex] - originY;
    }

    g_isScrollEnabled = false;

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_repeat_count(&a, 0);
    lv_anim_set_exec_cb(&a, ScrollEndExecCb);
    lv_anim_set_var(&a, canvas);
    lv_anim_set_time(&a, 200); // 200: period
    lv_anim_set_values(&a, 100, 0); // 100: start value
    lv_anim_set_path_cb(&a, lv_anim_path_bounce);
    lv_anim_start(&a);
}

static void ScrollEventCb(lv_event_t * e)
{
    lv_obj_t* canvas = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);

    lv_indev_t* indev = lv_indev_get_act();
    if (code == LV_EVENT_SCROLL_BEGIN) {
        HandleScrollBeginEvent(canvas, indev);
    }
    if (code == LV_EVENT_SCROLL) {
        HandleScrollEvent(canvas, indev);
    }
    if (code == LV_EVENT_SCROLL_END) {
        HandleScrollEndEvent(canvas, indev);
    }
}

void LvDemoRainbow(lv_obj_t* par)
{
    lv_obj_t* canvas = LvCanvasExtCreate(par);
    lv_obj_set_style_bg_color(canvas, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(canvas, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_size(canvas, LV_HOR_RES, LV_VER_RES);
    lv_obj_center(canvas);
    lv_obj_set_style_border_width(canvas, 0, 0);
    lv_obj_set_style_radius(canvas, 0, 0);
    lv_obj_add_event_cb(canvas, ScrollEventCb, LV_EVENT_ALL, NULL);

    if (!InitRainbowPaths()) {
        printf("InitRainbowPaths Failed\n");
        return;
    }

    for (uint8_t i = 0; i < RAINBOW_PATH_NUM; i++) {
        g_pathIds[i] = LvCanvasExtBeginPath(canvas, &g_paints[i]);
        LvCanvasExtUpdatePathData(canvas, &g_paths[i], g_pathIds[i]);
        LvCanvasExtDrawPath(canvas);
    }

    lv_obj_add_flag(canvas, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(canvas, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t* obj = lv_obj_create(canvas);
    lv_obj_set_size(obj, LV_HOR_RES + 10, LV_VER_RES + 10); // 10：increased by 10 to enable scroll
    lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN);
    lv_obj_center(obj);
    lv_obj_set_style_border_width(obj, 0, 0);
    lv_obj_set_style_radius(obj, 0, 0);
}

static void Init(lv_obj_t* par)
{
    if (g_isInited) {
        return;
    }
    g_isInited = true;
    LvDemoRainbow(par);
}

static void Deinit(lv_obj_t* par)
{
    if (!g_isInited) {
        return;
    }
    lv_obj_clean(par);
    for (uint8_t i = 0; i < RAINBOW_PATH_NUM; i++) {
        LvFreeBuffer(g_paths[i].cmd);
        LvFreeBuffer((float*)g_paths[i].data);
    }
    g_isInited = false;
}

static LvTestCaseInfo g_testCaseInfo = {
    "Rainbow",
    Init,
    Deinit,
};

void LvRegisterDemoRainbow(void)
{
    RegisterTestCase(&g_testCaseInfo);
}
#endif
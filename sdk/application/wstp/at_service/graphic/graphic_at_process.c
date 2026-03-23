/**
 * Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides gpu sample. \n
 * Author: CompanyName \n
 * History: \n
 * 2022-11-17， Create file. \n
 */
#include "debug_print.h"
#include "securec.h"
#include "gpu_sample.h"
#include "at_cmd_api.h"
#ifdef ENABLE_LVGL
#include "lv_sample.h"
#include "lv_dfx_cmd.h"
#endif
#include "graphic_service_wrapper.h"
#include "pinctrl_porting.h"
#include "common_def.h"
#ifdef ENABLE_UIKIT
#include "sample_ui.h"
#include "sample_util.h"
#include "dfx/dfx_cmd.h"
#endif
#include "lcd_bus.h"
#include "app_at_process.h"
#include "graphic_msg_manager.h"
#include "graphic_tx_process.h"
#include "graphic_at_process.h"

#define DIAG_RESULT_LEN   4
#define ARRAY_COUNT(array) (sizeof(array) / sizeof(array[0]))
#define BSP_MEM_ALLOC(y) osal_kmalloc((y), OSAL_GFP_KERNEL)
#define BSP_MEM_FREE(y) osal_kfree(y)
static bool g_sampleInitialized = false;

diag_ser_header_t g_graphic_diag_header;    // 用于保存diag回复包头内容
bool g_graphic_diag_flag = false;           // 用于控制执行功能后是否回复diag消息

uint32_t graphic_diag_cmd_process(diag_ser_data_t *data)
{
    uint8_t size = sizeof(commu_inter_info_t) + DIAG_RESULT_LEN * sizeof(uint8_t);
    commu_inter_info_t *cmd;

    if (data == NULL) {
        return ERRCODE_FAIL;
    }

    cmd = (commu_inter_info_t *)malloc(size);
    if (cmd == NULL) {
        PRINT("{%s():%d} malloc failed.\r\n", __FUNCTION__, __LINE__);
        return ERRCODE_FAIL;
    }
    (void)memset_s(cmd, size, 0, size);
    cmd->payloadLength = DIAG_RESULT_LEN;
    (void)memcpy_s(&g_graphic_diag_header, sizeof(diag_ser_header_t), &(data->header), sizeof(diag_ser_header_t));
    g_graphic_diag_flag = true;
    graphic_at_msg_send((uint8_t*)cmd, size);
    free(cmd);

    return ERRCODE_SUCC;
}

static int32_t ConvertATParamToSampleParam(uint8_t *inParam, uint16_t paramLen, int32_t *outArgc, char **outArgv)
{
    int ret;
    char *pos;
    uint8_t *strTemp;
    uint32_t paramCnt = 0;
    uint16_t realParamLen = paramLen - GRAPHIC_PARAM_OFFSET;
    strTemp = (uint8_t*)BSP_MEM_ALLOC(realParamLen);
    if (strTemp == NULL) {
        PRINT("{%s():%d} malloc failed.\r\n", __FUNCTION__, __LINE__);
        return ERRCODE_FAIL;
    }

    ret = memset_s(strTemp, realParamLen, 0, realParamLen);
    if (ret != EOK) {
        PRINT("{%s():%d} memset_s failed.\r\n", __FUNCTION__, __LINE__);
        BSP_MEM_FREE(strTemp);
        return ERRCODE_FAIL;
    }

    ret = memcpy_s(strTemp, realParamLen, &inParam[GRAPHIC_PARAM_OFFSET], realParamLen);
    if (ret != EOK) {
        PRINT("{%s():%d} memcpy_s failed.\r\n", __FUNCTION__, __LINE__);
        BSP_MEM_FREE(strTemp);
        return ERRCODE_FAIL;
    }

    pos = strtok((char*)strTemp, " ");
    for (uint32_t i = 0; pos != NULL; i++) {
        outArgv[i] = (char *)BSP_MEM_ALLOC(strlen(pos) + 1);
        if (outArgv[i] == NULL) {
            PRINT("{%s():%d} malloc failed.\r\n", __FUNCTION__, __LINE__);
            BSP_MEM_FREE(strTemp);
            return ERRCODE_FAIL;
        }
        ret = memcpy_s(outArgv[i], strlen(pos) + 1, pos, strlen(pos) + 1);
        if (ret != EOK) {
            PRINT("{%s():%d} memcpy_s failed.\r\n", __FUNCTION__, __LINE__);
            BSP_MEM_FREE(strTemp);
            return ERRCODE_FAIL;
        }
        paramCnt++;
        if (*outArgc >= GRAPHIC_MAX_PARAM_CNT) {
            PRINT("{%s():%d} invalid argc: %d(max: 64).\r\n", __FUNCTION__, __LINE__, *outArgc);
            BSP_MEM_FREE(strTemp);
            return ERRCODE_FAIL;
        }
        pos = strtok(NULL, " ");
    }

    *outArgc = paramCnt;
    BSP_MEM_FREE(strTemp);
    return ERRCODE_SUCC;
}

#ifdef ENABLE_UIKIT
#ifndef SUPPORT_NATIVE_UI
static int32_t UikitSample(commu_inter_info_t* interInfo)
{
    unused(interInfo);
    if (g_sampleInitialized) {
        return ERRCODE_SUCC;
    }
    SampleUi();
    g_sampleInitialized = true;
    return ERRCODE_SUCC;
}

static int32_t UikitSampleRun(commu_inter_info_t* interInfo)
{
    int32_t ret = ERRCODE_SUCC;
    char *argv[GRAPHIC_MAX_PARAM_CNT];
    int32_t argc = 0;
    ret = ConvertATParamToSampleParam(interInfo->payload, interInfo->payloadLength, &argc, argv);
    if (ret != ERRCODE_SUCC) {
        goto FREE;
    }
    if (argc != 1) {
        PRINT("{%s():%d} invalid argc: %d(must be : 1).\r\n", __FUNCTION__, __LINE__, argc);
        goto FREE;
    } else {
        if (!g_sampleInitialized) {
            SampleUi();
            g_sampleInitialized = true;
        }
 
        char* testcase = argv[0];
        PRINT("{%s():%d} SAMPLE [%s] START RUN. \n", __FUNCTION__, __LINE__, testcase);
        StartCaseUi(testcase);
    }
    
FREE:
    for (int32_t i = 0; i < argc; i++) {
        if (argv[i] != NULL) {
            BSP_MEM_FREE(argv[i]);
            argv[i]  = NULL;
        }
    }
    return ret;
}

#endif

/*******************************************************************************
  功能描述		:  计算当前画面截图后文件的hash值，并打印出来。
  参数内容		:  参数包含一个参数(key)，打印在时一同输出。用于关联定位。
*******************************************************************************/
static int32_t UikitHashScreen(commu_inter_info_t* interInfo)
{
    int32_t ret = ERRCODE_SUCC;
    char *argv[GRAPHIC_MAX_PARAM_CNT];
    int32_t argc = 0;
    ret = ConvertATParamToSampleParam(interInfo->payload, interInfo->payloadLength, &argc, argv);
    if (ret != ERRCODE_SUCC) {
        goto FREE;
    }
    if (argc != 1) {
        PRINT("{%s():%d} invalid argc: %d(must be : 1).\r\n", __FUNCTION__, __LINE__, argc);
        goto FREE;
    } else {
        char* key = argv[0];
        CalcSampleUiHash(key);
    }
 
    PRINT("{%s():%d} free args.\r\n", __FUNCTION__, __LINE__);
FREE:
    for (int32_t i = 0; i < argc; i++) {
        if (argv[i] != NULL) {
            BSP_MEM_FREE(argv[i]);
            argv[i]  = NULL;
        }
    }
    return ret;
}

static int32_t UikitDfx(commu_inter_info_t* interInfo)
{
    int32_t ret = ERRCODE_SUCC;
    int32_t argc = 0;
    char *argv[GRAPHIC_MAX_PARAM_CNT];

    ret = ConvertATParamToSampleParam(interInfo->payload, interInfo->payloadLength, &argc, argv);
    if (ret != ERRCODE_SUCC) {
        goto FREE;
    }
    ExecUikitDfxByCmdType(argc, argv);
    PRINT("{%s():%d} %s %s.\r\n", __FUNCTION__, __LINE__, argv[0], (ret == ERRCODE_SUCC) ? "success" : "failure");

FREE:
    for (int32_t i = 0; i < argc; i++) {
        if (argv[i] != NULL) {
            BSP_MEM_FREE(argv[i]);
            argv[i]  = NULL;
        }
    }
    return ret;
}

static const GraphicProcessType g_UikitFuncs[] = {
#ifndef SUPPORT_NATIVE_UI
    { UIKIT_SAMPLE, UikitSample },
    { UIKIT_SAMPLE_RUN, UikitSampleRun },
    { UIKIT_SAMPLE_HASH, UikitHashScreen },
#endif
    { UIKIT_DFX, UikitDfx },
};

static int32_t UikitProcess(commu_inter_info_t* interInfo)
{
    uint8_t uikitCmd;
    int32_t ret = ERRCODE_FAIL;

    uikitCmd = interInfo->payload[GRAPHIC_SCENE_CMD_POS];
    for (uint8_t i = 0; i < ARRAY_COUNT(g_UikitFuncs); i++) {
        if (uikitCmd == g_UikitFuncs[i].id) {
            ret = g_UikitFuncs[i].func(interInfo);
        }
    }

    return ret;
}
#endif

static int32_t GraphicSetFreq(commu_inter_info_t* interInfo)
{
    uint8_t *start = &interInfo->payload[GRAPHIC_PARAM_OFFSET];
    int feq = strtol((char *)start, NULL, 0);
    SetFrequence(feq);
    return ERRCODE_SUCC;
}

#ifdef ENABLE_LVGL
static int32_t LvglDemo(commu_inter_info_t* interInfo)
{
    unused(interInfo);
    LvglSample();
    return ERRCODE_SUCC;
}

static int32_t LvglDfx(commu_inter_info_t* interInfo)
{
    int32_t ret = ERRCODE_SUCC;
    int32_t argc = 0;
    uint8_t *argv[GRAPHIC_MAX_PARAM_CNT];

    ret = ConvertATParamToSampleParam(interInfo->payload, interInfo->payloadLength, &argc, argv);
    if (ret != ERRCODE_SUCC) {
        goto FREE;
    }

    ExecLvglDfxCmd(argc, argv);

FREE:
    for (int32_t i = 0; i < argc; i++) {
        if (argv[i] != NULL) {
            BSP_MEM_FREE(argv[i]);
            argv[i]  = NULL;
        }
    }

    return ret;
}

static const GraphicProcessType g_lvglFuncs[] = {
    { LVGL_SAMPLE, LvglDemo },
    { LVGL_DFX, LvglDfx },
};

static int32_t LvglProcess(commu_inter_info_t* interInfo)
{
    PRINT("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);

    uint8_t cmd;
    int32_t ret = ERRCODE_FAIL;

    cmd = interInfo->payload[GRAPHIC_SCENE_CMD_POS];

    for (uint8_t i = 0; i < ARRAY_COUNT(g_lvglFuncs); i++) {
        if (cmd == g_lvglFuncs[i].id) {
            ret = g_lvglFuncs[i].func(interInfo);
        }
    }

    PRINT("{%s():%d} func out %s.\r\n", __FUNCTION__, __LINE__, (ret == ERRCODE_SUCC) ? "success" : "failure");
    return ret;
}
#endif

static int32_t GPUProcess(commu_inter_info_t* interInfo)
{
    int32_t ret = ERRCODE_SUCC;
    int32_t argc = 0;
    uint8_t *argv[GRAPHIC_MAX_PARAM_CNT];

    ret = ConvertATParamToSampleParam(interInfo->payload, interInfo->payloadLength, &argc, argv);
    if (ret != ERRCODE_SUCC) {
        goto FREE;
    }

    wstp_gpu_sample_process(argc, argv);

FREE:
    for (int32_t i = 0; i < argc; i++) {
        if (argv[i] != NULL) {
            BSP_MEM_FREE(argv[i]);
            argv[i]  = NULL;
        }
    }

    return ret;
}

static int32_t get_lcd_status_process(commu_inter_info_t* inter_info)
{
    int32_t ret = ERRCODE_SUCC;
    uint32_t status = 0;
    lcd_bus_api *bus_api = lcd_get_bus_api();
    unused(inter_info);

#ifdef QSPI_DISPLAY
    while (is_qspi_transfer_done() != true) {
        ;
    }
#endif
    ret = bus_api->bus_read_cmd(0xA, &status, sizeof(uint32_t));
    if (status != 0xdc) { // expectvau
        PRINT("\r\nLcd Status Err(0x%x).\r\n", status);
        return ret;
    }
    PRINT("\r\nLcd Status OK(0x%x).\r\n", status);

    return ret;
}

static const GraphicProcessType g_graphicFuncs[] = {
#ifdef ENABLE_UIKIT
    { GRAPHIC_UIKIT, UikitProcess},
#endif
    { GRAPHIC_SET_FREQ, GraphicSetFreq},
#ifdef ENABLE_LVGL
    { GRAPHIC_LVGL, LvglProcess},
#endif
    { GRAPHIC_GPU, GPUProcess},
    { GRAPHIC_LCD_STATUS, get_lcd_status_process},
};

void graphic_at_process(uint8_t *data, uint32_t data_len)
{
    unused(data_len);
    if (data == NULL) {
        return;
    }

    uint8_t graphicScene;
    int32_t ret = ERRCODE_FAIL;
    commu_inter_info_t *interInfo = (commu_inter_info_t *)(data);
    graphicScene = interInfo->payload[GRAPHIC_GUI_TYPE_POS];
    for (uint32_t i = 0; i < ARRAY_COUNT(g_graphicFuncs); i++) {
        if (graphicScene == g_graphicFuncs[i].id) {
            ret = g_graphicFuncs[i].func(interInfo);
            print_at_cmd_ret(ret);
        }
    }

    // phone remote control, need reply to spp
    if (g_graphic_diag_flag) {
        uint8_t reply_data[DIAG_RESULT_LEN] = {0};
        reply_data[0x0] = 0x80;
        reply_data[0x1] = 0x82;
        reply_data[0x2] = 0;
        reply_data[0x3] = (uint8_t)ret;
        ret = send_spp_diag_msg(g_graphic_diag_flag, &g_graphic_diag_header, reply_data, DIAG_RESULT_LEN);
        PRINT("graphic_at_process send msg to spp %s.\r\n", (ret == ERRCODE_SUCC) ? "success" : "failure");
    }
    g_graphic_diag_flag = false;
}
/**
 * Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides at cmd manager source\n
 * Author: CompanyName \n
 * History: \n
 * 2022-11-17， Create file. \n
 */
#include "debug_print.h"
#include "securec.h"
#include "soc_osal.h"
#include "at_cmd_api.h"
#include "graphic_tx_process.h"
#include "graphic_cmd_manager.h"

#define ARRAY_COUNT(array) (sizeof(array) / sizeof(array[0]))
#define BSP_MEM_ALLOC(y) osal_kmalloc((y), OSAL_GFP_KERNEL)
#define BSP_MEM_FREE(y) osal_kfree(y)
#define GRAPHIC_MAX_PARAM_CNT       64
#define GRAPHIC_MAX_PARAM_LEN       128
#define GRAPHIC_CMD_LEN             4
#define MAX_PARAM_COUNT             128
#define CMD_PAYLOAD_MAX             (MAX_PARAM_COUNT-9)

static uint32_t MCGraphicATCmdHandle(uint8_t *para, uint32_t paraLen, char* strCommonCmd);

/* these commands can be used in end_user version */
static const at_cmd_table_t g_graphic_cmd[] = {
    { "AT^UIKIT_SAMPLE",                 MCGraphicATCmdHandle,  "00 00 00 00" },
    { "AT^UIKIT_DFX",                    MCGraphicATCmdHandle,  "00 01 00 00" },
    { "AT^UIKIT_SAMPLE_RUN",             MCGraphicATCmdHandle,  "00 03 00 00" },
    { "AT^UIKIT_SAMPLE_HASH",            MCGraphicATCmdHandle,  "00 04 00 00" },
    { "AT^GUI_SET_FREQ",                 MCGraphicATCmdHandle,  "01 00 00 00" },
    { "AT^LVGL_SAMPLE",                  MCGraphicATCmdHandle,  "02 00 00 00" },
    { "AT^LVGL_DFX",                     MCGraphicATCmdHandle,  "02 01 00 00" },
    { "AT^GPU_SAMPLE",                   MCGraphicATCmdHandle,  "03 00 00 00" },
    { "AT^GET_LCD_STATUS",               MCGraphicATCmdHandle,  "04 00 00 00" },
};

at_cmd_table_t *get_graphic_cmd_table(uint32_t *cmd_count)
{
    *cmd_count = ARRAY_COUNT(g_graphic_cmd);
    return g_graphic_cmd;
}

static void ConvertCmdToParam(char* strcmd, uint8_t* param)
{
    if (strcmd == NULL) {
        PRINT("{%s():%d} invalid cmd.\r\n", __FUNCTION__, __LINE__);
        return;
    }

    char* p;
    uint8_t* strtemp;
    int len;
    int ret;
    int i;

    len = (strlen(strcmd) + 1) / 3; // 命令序列长度，+1为结束符，/3为每个十六进制数字符加空格长度
    if (len > GRAPHIC_MAX_PARAM_CNT) {
        PRINT("{%s():%d} invalid cmd len: %d.\r\n", __FUNCTION__, __LINE__, len);
        return;
    }

    strtemp = (uint8_t*)BSP_MEM_ALLOC(strlen(strcmd) + 1);
    if (strtemp == NULL) {
        PRINT("{%s():%d} malloc failed.\r\n", __FUNCTION__, __LINE__);
        return;
    }
    ret = memset_s(strtemp, strlen(strcmd) + 1, 0, strlen(strcmd) + 1);
    if (ret != EOK) {
        PRINT("{%s():%d} memset_s failed.\r\n", __FUNCTION__, __LINE__);
        BSP_MEM_FREE(strtemp);
        return;
    }
    ret = memcpy_s(strtemp, strlen(strcmd)+1, strcmd, strlen(strcmd) + 1);
    if (ret != EOK) {
        PRINT("{%s():%d} memcpy_s failed.\r\n", __FUNCTION__, __LINE__);
        BSP_MEM_FREE(strtemp);
        return;
    }

    p = strtok((char*)strtemp, " ");
    for (i = 0; i < len; i++) {
        param[i] = strtol(p, NULL, 16);
        p = strtok(NULL, " ");
    }

    BSP_MEM_FREE(strtemp);
    return;
}

static uint32_t MCGraphicATCmdHandle(uint8_t *para, uint32_t paraLen, char* strCommonCmd)
{
    PRINT("MCGraphicATCmdHandle, paraLen: %d, para: %s\n", paraLen, para);
    uint8_t param[GRAPHIC_MAX_PARAM_LEN] = {0};
    commu_inter_info_t* interInfo = NULL;
    uint32_t infoLen;
    int payloadLen;
    int ret;

    ConvertCmdToParam(strCommonCmd, param);
    payloadLen = GRAPHIC_CMD_LEN + paraLen + 1;
    if (payloadLen > GRAPHIC_MAX_PARAM_LEN || payloadLen < 0) {
        PRINT("{%s():%d} invalid payload len: %d range [0,128].\r\n", __FUNCTION__, __LINE__, payloadLen);
        return ERRCODE_FAIL;
    }

    infoLen = sizeof(commu_inter_info_t) + payloadLen;
    interInfo = (commu_inter_info_t*)BSP_MEM_ALLOC(infoLen);
    if (interInfo == NULL) {
        PRINT("{%s():%d} malloc failed.\r\n", __FUNCTION__, __LINE__);
        return ERRCODE_FAIL;
    }

    ret = memset_s(interInfo, infoLen, 0, infoLen);
    if (ret != EOK) {
        PRINT("{%s():%d} memset_s failed.\r\n", __FUNCTION__, __LINE__);
        BSP_MEM_FREE(interInfo);
        return ERRCODE_FAIL;
    }

    ret = memcpy_s(&param[GRAPHIC_CMD_LEN], payloadLen, para, paraLen);
    if (ret != EOK) {
        PRINT("{%s():%d} memcpy_s failed.\r\n", __FUNCTION__, __LINE__);
        BSP_MEM_FREE(interInfo);
        return ERRCODE_FAIL;
    }
    param[GRAPHIC_CMD_LEN + paraLen] = '\0';

    ret = memcpy_s(&interInfo->payload[0], CMD_PAYLOAD_MAX, &param, payloadLen);
    if (ret != EOK) {
        PRINT("{%s():%d} memcpy_s failed, payloadLen: %d, ret: %d.\r\n", __FUNCTION__, __LINE__, payloadLen, ret);
        BSP_MEM_FREE(interInfo);
        return ERRCODE_FAIL;
    }

    interInfo->payloadLength = payloadLen;
    graphic_at_msg_send((uint8_t *)interInfo, infoLen);
    BSP_MEM_FREE(interInfo);
    return ERRCODE_SUCC;
}

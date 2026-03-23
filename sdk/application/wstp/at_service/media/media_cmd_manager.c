/**
 * Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides at cmd manager source\n
 * Author: CompanyName \n
 * History: \n
 * 2022-11-17， Create file. \n
 */
#include "debug_print.h"
#include "soc_osal.h"
#include "media_tx_process.h"
#include "media_cmd_manager.h"

#define ARRAY_COUNT(array) (sizeof(array) / sizeof(array[0]))
#define BSP_MEM_ALLOC(y) osal_kmalloc((y), OSAL_GFP_KERNEL)
#define BSP_MEM_FREE(y) osal_kfree(y)
#define MEDIA_CMD_LEN 2
#define MEDIA_MAX_PARAM_CNT 64
#define MEDIA_MAX_PARAM_LEN 128

/* 处理AT命令对应命令序列的处理接口，将命令序列字符串strcmd处理成数组param */
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

    len = (strlen(strcmd) + 1) / 3;     //命令序列长度，+1为结束符，/3为每个十六进制数字符加空格长度
    if (len > MEDIA_MAX_PARAM_CNT) {
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

    //处理十六进制命令序列转数字数组
    p = strtok((char*)strtemp, " ");
    for (i = 0; i < len; i++) {
        param[i] = strtol(p, NULL, 16);
        p = strtok(NULL, " ");
    }

    BSP_MEM_FREE(strtemp);
    return;
}

static uint32_t MCMediaATCmdHandle(uint8_t *para, uint32_t paraLen, char* strCommonCmd)
{
    PRINT("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);

    int ret;
    int paramLen;
    uint8_t param[MEDIA_MAX_PARAM_LEN] = {0};

    ConvertCmdToParam(strCommonCmd, param);

    paramLen = MEDIA_CMD_LEN + paraLen + 1;
    if (paramLen >= MEDIA_MAX_PARAM_LEN) {
        PRINT("{%s():%d} param length exceeds limit %d bytes.\r\n", __FUNCTION__, __LINE__, MEDIA_MAX_PARAM_LEN);
        return ERRCODE_FAIL;
    }

    ret = memcpy_s(&param[MEDIA_CMD_LEN], paramLen, para, paraLen);
    if (ret != EOK) {
        PRINT("{%s():%d} memcpy_s failed.\r\n", __FUNCTION__, __LINE__);
        return ERRCODE_FAIL;
    }
    param[MEDIA_CMD_LEN + paraLen] = '\0';

    media_at_msg_send(param, paramLen);
    PRINT("{%s():%d} func out success.\r\n", __FUNCTION__, __LINE__);
    return ERRCODE_SUCC;
}

static const at_cmd_table_t g_media_cmd[] = {
    { "AT^MEDIAMUSICPLAY",                 MCMediaATCmdHandle,  "00 00" },
    { "AT^MEDIAMUSICPAUSE",                MCMediaATCmdHandle,  "00 01" },
    { "AT^MEDIAMUSICRESUME",               MCMediaATCmdHandle,  "00 02" },
    { "AT^MEDIAMUSICSTOP",                 MCMediaATCmdHandle,  "00 03" },
#ifndef MEMORY_MINI
    { "AT^MEDIAMUSICABLUMINFO",            MCMediaATCmdHandle,  "00 04" },
#endif
    { "AT^MEDIASETMUSICVOLUME",            MCMediaATCmdHandle,  "00 05" },
#ifndef MEMORY_MINI
    { "AT^MEDIAGETMUSICVOLUME",            MCMediaATCmdHandle,  "00 06" },
    { "AT^MEDIASETBTSCOVOLUME",            MCMediaATCmdHandle,  "00 07" },
    { "AT^MEDIAGETBTSCOVOLUME",            MCMediaATCmdHandle,  "00 08" },
#endif
    { "AT^MEDIAMUSICSINGLELOOP",           MCMediaATCmdHandle,  "00 09" },
    { "AT^MEDIAMUSICSWITCHSONG",           MCMediaATCmdHandle,  "00 0A" },

    { "AT^MEDIAGETCURRENTMUSICSTATE",      MCMediaATCmdHandle,  "00 0B" },
    { "AT^MEDIANOTIFYPLAY",                MCMediaATCmdHandle,  "00 0C" },
#ifndef MEMORY_MINI
    { "AT^BACKAUDIOPLAY",                  MCMediaATCmdHandle,  "00 0D" },
    { "AT^MEDIAAUDIORECORD",               MCMediaATCmdHandle,  "01 00" },
    { "AT^MEDIAAUDIORECORDSTOP",           MCMediaATCmdHandle,  "01 01" },
    // focus: BT SCO Voice Call, incoming: Alarm Clock
    { "AT^AUDIOSCENEINTERACTION001",       MCMediaATCmdHandle,  "02 01" },
    // focus: Ring, incoming: Alarm Clock
    { "AT^AUDIOSCENEINTERACTION002",       MCMediaATCmdHandle,  "02 02" },
    // focus: Alarm Clock, incoming: BT SCO Voice Call
    { "AT^AUDIOSCENEINTERACTION003",       MCMediaATCmdHandle,  "02 03" },
    // focus: Alarm Clock, incoming: Ring
    { "AT^AUDIOSCENEINTERACTION004",       MCMediaATCmdHandle,  "02 04" },
    // focus: Alarm Clock, incoming: Music
    { "AT^AUDIOSCENEINTERACTION005",       MCMediaATCmdHandle,  "02 05" },
    // focus: Music, incoming: Ring
    { "AT^AUDIOSCENEINTERACTION006",       MCMediaATCmdHandle,  "02 06" },
    // focus: Music, incoming: Alarm Clock
    { "AT^AUDIOSCENEINTERACTION007",       MCMediaATCmdHandle,  "02 07" },
    { "AT^MEDIAVIDEOPLAY",                 MCMediaATCmdHandle,  "03 00" },
    { "AT^MEDIAVIDEOPAUSE",                MCMediaATCmdHandle,  "03 01" },
    { "AT^MEDIAVIDEORESUME",               MCMediaATCmdHandle,  "03 02" },
    { "AT^MEDIAVIDEOSTOP",                 MCMediaATCmdHandle,  "03 03" },
    { "AT^MEDIAVIDEOSINGLELOOP",           MCMediaATCmdHandle,  "03 04" },
    { "AT^MEDIACAMERAPREVIEWSTART",        MCMediaATCmdHandle,  "04 00" },
    { "AT^MEDIACAMERAPREVIEWSTOP",         MCMediaATCmdHandle,  "04 01" },
    { "AT^MEDIAVOICECALLVOLTE",            MCMediaATCmdHandle,  "05 00" },
#endif
    { "AT^MEDIA2DPSINKAEF",                MCMediaATCmdHandle,  "06 00" },
#ifndef MEMORY_MINI
    { "AT^AUDIORECORDER",                  MCMediaATCmdHandle,  "07 00" },
#endif
};

at_cmd_table_t *get_media_cmd_table(uint32_t *cmd_count)
{
    *cmd_count = ARRAY_COUNT(g_media_cmd);
    return g_media_cmd;
}

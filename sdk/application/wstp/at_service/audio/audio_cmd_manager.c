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
#include "at_cmd_api.h"
#include "common_def.h"
#include "audio_tx_process.h"
#include "audio_cmd_manager.h"

#define ARRAY_COUNT(array) (sizeof(array) / sizeof(array[0]))

static uint32_t parse_audio_handle(uint8_t *para, uint32_t para_len, char *str_cmd)
{
    unused(str_cmd);
    if (para == NULL) {
        return ERRCODE_FAIL;
    }
    PRINT("parse_audio_handle in para_len = %d  para = %s\r\n", para_len, para);
    audio_at_msg_send(para, para_len);
    return ERRCODE_SUCC;
}

static const at_cmd_table_t g_audio_cmd[] = {
    { "AT^audio",              parse_audio_handle,         "" },
    { "AT^audio_test",         parse_audio_handle,         "" },
};

at_cmd_table_t *get_audio_cmd_table(uint32_t *cmd_count)
{
    *cmd_count = ARRAY_COUNT(g_audio_cmd);
    return (at_cmd_table_t *)g_audio_cmd;
}

/*
 * Copyright (c) CompanyNameMagicTag 2021-2022. All rights reserved.
 * Description: Media task process
 * Author:
 * Create:
 */
#include "debug_print.h"
#include "securec.h"
#include "at_cmd_api.h"
#include "app_msg_manager.h"
#if (SUPPORT_SAMPLE_AUDIO == FEATURE_ON)
void audio_execute_function(int32_t argc, char *argv[]);
#endif
#include "audio_at_process.h"

#define TEST_SUITE_MAX_ARGC 16

#define audio_log_err_info(str)                                         \
    do {                                                                \
        PRINT("[A][ERROR][audio_at_process][%d]%s\r\n", __LINE__, str); \
    } while (0)

#define audio_log_err_s32(val)                                                   \
    do {                                                                         \
        PRINT("[A][ERROR][audio_at_process][%d]%s=%d\r\n", __LINE__, #val, val); \
    } while (0)

static uint32_t sap_audio_para_to_args(const uint8_t *para_str, int32_t *argc, char *msg_argv[])
{
    int32_t ret;
    int32_t len;
    char *p = NULL;
    char *context = NULL;

    p = strtok_s((char*)para_str, " ", &context);
    for (int32_t i = 0; p != NULL; i++) {
        len = strlen(p) + 1;

        msg_argv[i] = (char*)malloc(len);
        if (msg_argv[i] == NULL) {
            audio_log_err_s32(len);
            audio_log_err_info("Call malloc fail for msg_argv.");
            return ERRCODE_FAIL;
        }

        ret = memcpy_s(msg_argv[i], len, p, len);
        if (ret != EOK) {
            audio_log_err_info("Call memcpy_s fail for msg_argv.");
            return ERRCODE_FAIL;
        }

        (*argc)++;
        if (*argc >= TEST_SUITE_MAX_ARGC) {
            audio_log_err_s32(*argc);
            audio_log_err_s32(TEST_SUITE_MAX_ARGC);
            audio_log_err_info("Param num out of range.");
            return ERRCODE_FAIL;
        }

        p = strtok_s(NULL, " ", &context);
    }

    return ERRCODE_SUCC;
}

static uint32_t sap_audio_at_cmd_handle(const uint8_t *para, uint32_t paraLen)
{
    int32_t argc = 0;
    int32_t ret = ERRCODE_SUCC;
    uint8_t *para_str = NULL;
    char *p = NULL;
    char *msg_argv[TEST_SUITE_MAX_ARGC] = {NULL};

    if (para == NULL) {
        ret = ERRCODE_FAIL;
        goto EXIT_PROC;
    }

    para_str = (uint8_t*)malloc(paraLen + 1); /* AT送过来的是命令序列，不是字符串，所以没有结束符 需要手动加上结束符 */
    if (para_str == NULL) {
        audio_log_err_s32(paraLen);
        audio_log_err_info("Call malloc fail for para_str.");
        ret = ERRCODE_FAIL;
        goto EXIT_PROC;
    }

    ret = memcpy_s(para_str, paraLen, para, paraLen);
    if (ret != EOK) {
        audio_log_err_info("Call memcpy_s fail for para_str.");
        ret = ERRCODE_FAIL;
        goto EXIT_PROC;
    }

    para_str[paraLen] = '\0'; /* 给命令序列加上结束符， 方便下面做字符串处理 */

    ret = sap_audio_para_to_args(para_str, &argc, msg_argv);
    if (ret != ERRCODE_SUCC) {
        audio_log_err_info("Call sap_audio_para_to_args fail.");
        ret = ERRCODE_FAIL;
        goto EXIT_PROC;
    }

#if (SUPPORT_SAMPLE_AUDIO == FEATURE_ON)
    audio_execute_function(argc, msg_argv);
#endif

EXIT_PROC:
    for (int32_t i = 0; i < argc; i++) {
        if (msg_argv[i] != NULL) {
            free(msg_argv[i]);
        }
    }

    if (para_str != NULL) {
        free(para_str);
    }

    return ret;
}

void audio_at_cmd_process(uint8_t *data, uint32_t data_len)
{
    int32_t ret;

    if (data == NULL) {
        return;
    }

    PRINT("audio_at_cmd_process in data_len = %d  data = %s\r\n", data_len, data);
    ret = sap_audio_at_cmd_handle(data, data_len);
    if (ret != ERRCODE_SUCC) {
        audio_log_err_info("Call sap_audio_at_cmd_handle fail.");
        print_at_cmd_ret(ERRCODE_FAIL);
    }

    print_at_cmd_ret(ERRCODE_SUCC);
    return;
}

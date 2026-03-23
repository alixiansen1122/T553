/**
 * Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides ohos sample. \n
 * Author: CompanyName \n
 * History: \n
 * 2022-11-17， Create file. \n
 */
#include "gnss_at_process.h"
#include "stdlib.h"
#include "securec.h"
#include "common_def.h"
#include "gnss_process.h"
#include "errcode.h"
#include "gnss_log.h"

#define PARAM_LENS 4
#define AT_PARA_IDX2 2
#define AT_PARA_IDX3 3
#define HEX_PAIR_LENGTH 2
#define HEX_BASE 16

static uint32_t gnss_init_param_parse(uint8_t *para, uint32_t para_len)
{
    unused(para_len);
    char *ptr = (char *)para;
    char *token = NULL;
    char *next_token = NULL;
    uint32_t idx = 0;
    uint32_t temp[PARAM_LENS] = {0};
    token = strtok_s((char*)ptr, ",", &next_token);
    while (token != NULL) {
        temp[idx++] = strtoul(token, NULL, 10); // 10: 十进制
        token = strtok_s(NULL, ",", &next_token);
    }

    uint8_t param_1, param_2, param_3, param_4;
    param_1 = (uint8_t)temp[0];
    param_2 = (uint8_t)temp[1];
    param_3 = (uint8_t)temp[AT_PARA_IDX2];
    param_4 = (uint8_t)temp[AT_PARA_IDX3];
    errcode_t ret = gnss_proc_msg_init(param_1, param_2, param_3, param_4);
    if (ret == ERRCODE_SUCC) {
        GNSS_LOG_INFO("OK\n");
    } else {
        GNSS_LOG_ERROR("gnss init fail, error code: %#x\n", ret);
        return ret;
    }
    return ret;
}

static uint32_t gnss_start_param_parse(uint8_t *para, uint32_t para_len)
{
    unused(para_len);
    unused(para);
    errcode_t ret = gnss_proc_msg_start();
    if (ret == ERRCODE_SUCC) {
        GNSS_LOG_INFO("OK\n");
    } else {
        GNSS_LOG_ERROR("gnss start fail, error code: %#x\n", ret);
        return ret;
    }
    return ret;
}

static uint32_t gnss_stop_param_parse(uint8_t *para, uint32_t para_len)
{
    unused(para_len);
    unused(para);
    errcode_t ret = gnss_proc_msg_stop();
    if (ret == ERRCODE_SUCC) {
        GNSS_LOG_INFO("OK\n");
    } else {
        GNSS_LOG_ERROR("gnss stop fail, error code: %#x\n", ret);
        return ret;
    }
    return ret;
}

static uint32_t gnss_deinit_param_parse(uint8_t *para, uint32_t para_len)
{
    unused(para_len);
    unused(para);
    errcode_t ret = gnss_proc_msg_deinit();
    if (ret == ERRCODE_SUCC) {
        GNSS_LOG_INFO("OK\n");
    } else {
        GNSS_LOG_ERROR("gnss deinit fail, error code: %#x\n", ret);
        return ret;
    }
    return ret;
}

static uint32_t gnss_hex_param_parse(uint8_t *para, uint32_t para_len)
{
    errcode_t ret = gnss_proc_msg_cfg(para, (uint16_t)para_len);
    if (ret == ERRCODE_SUCC) {
        GNSS_LOG_INFO("OK\n");
    } else {
        GNSS_LOG_ERROR("gnss hex exec fail, error code: %#x\n", ret);
        return ret;
    }
    return ret;
}

static uint32_t gnss_sample_param_parse(uint8_t *para, uint32_t para_len)
{
    unused(para_len);
    char *ptr = (char *)para;
    char *token = NULL;
    char *next_token = NULL;
    uint32_t tmp[PARAM_LENS] = {0};
    token = strtok_s((char*)ptr, ",", &next_token);
    uint8_t index = (uint8_t)strtoul(token, NULL, 10); // 10: 十进制
    errcode_t ret = gnss_proc_msg_sample(index);
    if (ret == ERRCODE_SUCC) {
        GNSS_LOG_INFO("OK\n");
    } else {
        GNSS_LOG_ERROR("gnss sample:%d exec fail, error code: %#x\n", index, ret);
        return ret;
    }
    return ret;
}

static uint32_t gnss_nmea_param_parse(uint8_t *para, uint32_t para_len)
{
    unused(para_len);
    char *ptr = (char *)para;
    char *token = NULL;
    char *next_token = NULL;
    uint32_t tmp[PARAM_LENS] = {0};
    token = strtok_s((char*)ptr, ",", &next_token);
    uint8_t index = (uint8_t)strtoul(token, NULL, 10); // 10: 十进制
    errcode_t ret = gnss_proc_msg_nmea(index);
    if (ret == ERRCODE_SUCC) {
        GNSS_LOG_INFO("OK\n");
    } else {
        GNSS_LOG_ERROR("nmea flag: %d set fail, error code: %#x\n", index, ret);
        return ret;
    }
    return ret;
}

static uint32_t app_gnss_init_cmd(uint8_t *para, uint32_t para_len, char *str_cmd)
{
    unused(str_cmd);
    if (para == NULL) {
        return ERRCODE_FAIL;
    }

    gnss_init_param_parse(para, para_len);
    return ERRCODE_SUCC;
}

static uint32_t app_gnss_start_cmd(uint8_t *para, uint32_t para_len, char *str_cmd)
{
    unused(str_cmd);
    if (para == NULL) {
        return ERRCODE_FAIL;
    }
    gnss_start_param_parse(para, para_len);
    return ERRCODE_SUCC;
}

static uint32_t app_gnss_stop_cmd(uint8_t *para, uint32_t para_len, char *str_cmd)
{
    unused(str_cmd);
    if (para == NULL) {
        return ERRCODE_FAIL;
    }
    gnss_stop_param_parse(para, para_len);
    return ERRCODE_SUCC;
}

static uint32_t app_gnss_deinit_cmd(uint8_t *para, uint32_t para_len, char *str_cmd)
{
    unused(str_cmd);
    if (para == NULL) {
        return ERRCODE_FAIL;
    }
    gnss_deinit_param_parse(para, para_len);
    return ERRCODE_SUCC;
}

static uint32_t app_gnss_hex_cmd(uint8_t *para, uint32_t para_len, char *str_cmd)
{
    unused(str_cmd);
    if ((para == NULL) || (para_len == 0) || (para_len % HEX_PAIR_LENGTH == 0)) {
        return ERRCODE_FAIL;
    }

    uint32_t len = para_len / HEX_PAIR_LENGTH;
    uint8_t data[len];
    uint8_t idx = 0;
    for (uint32_t i = 0; i < para_len; i += HEX_PAIR_LENGTH) {
        char hex_pair[HEX_PAIR_LENGTH + 1] = {para[i], para[i + 1], '\0'};
        char *endptr;
        data[idx++] = strtol(hex_pair, &endptr, HEX_BASE);
        if (*endptr != '\0') {
            return ERRCODE_FAIL;
        }
    }
    gnss_hex_param_parse(data, len);
    return ERRCODE_SUCC;
}

static uint32_t app_gnss_sample_cmd(uint8_t *para, uint32_t para_len, char *str_cmd)
{
    unused(str_cmd);
    if (para == NULL) {
        return ERRCODE_FAIL;
    }
    gnss_sample_param_parse(para, para_len);
    return ERRCODE_SUCC;
}

static uint32_t app_gnss_nmea_cmd(uint8_t *para, uint32_t para_len, char *str_cmd)
{
    unused(str_cmd);
    if (para == NULL) {
        return ERRCODE_FAIL;
    }
    gnss_nmea_param_parse(para, para_len);
    return ERRCODE_SUCC;
}

static uint32_t app_gnss_logsize_cmd(uint8_t *para, uint32_t para_len, char *str_cmd)
{
    unused(para_len);
    char *ptr = (char *)para;
    char *token = NULL;
    char *next_token = NULL;
    uint32_t idx = 0;
    uint32_t temp[PARAM_LENS] = {0};
    token = strtok_s((char*)ptr, ",", &next_token);
    while (token != NULL) {
        temp[idx++] = strtoul(token, NULL, 10); // 10: 十进制
        token = strtok_s(NULL, ",", &next_token);
    }

    errcode_t ret = gnss_proc_log_ctrl_info(temp[0], temp[1]);
    if (ret == ERRCODE_SUCC) {
        GNSS_LOG_INFO("OK\n");
    } else {
        GNSS_LOG_ERROR("gnss logsize set fail, error code: %#x\n", ret);
        return ret;
    }
    return ret;
}

static const at_cmd_table_t g_gnss_cmd[] = {
    { "AT^GNSSINIT", app_gnss_init_cmd,   "" },
    { "AT^GNSSSTART", app_gnss_start_cmd,   "" },
    { "AT^GNSSSTOP", app_gnss_stop_cmd,   "" },
    { "AT^GNSSDEINIT", app_gnss_deinit_cmd,   "" },
    { "AT^GNSSHEX", app_gnss_hex_cmd,   "" },
    { "AT^GNSSSAMPLE", app_gnss_sample_cmd,   "" },
    { "AT^GNSSNMEA", app_gnss_nmea_cmd,   "" },
    { "AT^GNSSLOGSIZE", app_gnss_logsize_cmd,   "" },
};

at_cmd_table_t *get_gnss_cmd_table(uint32_t *cmd_count)
{
    *cmd_count = array_size(g_gnss_cmd);
    return (at_cmd_table_t *)g_gnss_cmd;
}

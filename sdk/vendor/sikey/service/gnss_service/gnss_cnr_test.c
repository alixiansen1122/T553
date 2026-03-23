/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: gnss factory test sample
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <securec.h>
#include <stdint.h>
#include <signal.h>
#include <stdbool.h>
//#include "gnss_device.h"
//#include "gnss_common_proc.h"
#include "gnss_log.h"
#include "gnss_custom_api.h"

#define GNSS_COMMAND_BUFF_LEN   200
#define GNSS_REPORT_BUFF_LEN    2000

static uint8_t g_ftm_cmd_buff[GNSS_COMMAND_BUFF_LEN] = {0};
static GnssMsg             *g_gnss_ftm_msg = (GnssMsg*)g_ftm_cmd_buff;
static gnss_factory_test_config *g_gnss_ftm_cfg = (gnss_factory_test_config*)(g_ftm_cmd_buff + sizeof(GnssMsg));

static uint16_t calc_checksum(uint8_t *in_buff, uint32_t in_len)
{
    uint16_t checksum = 0;
    uint16_t i = 0;
    while (i++ < in_len) {
        checksum += *in_buff;
        in_buff++;
    }
    return checksum;
}

/*
 * 产线测试
 * 1. 使用单音信号，配置频点为1575.62MHz（相对于L1正向偏移200KHz）
 * 2. 使用参考板标定信号，调整单音设备功率，使参考板端侧接收信号在40dBHz
*/
static errcode_t custom_gnss_set_factory_test(void)
{
    GNSS_LOG_DEBUG("[FACTORY DEBUG] enter cfg: set factory test\n");

    (void)memset_s(g_ftm_cmd_buff, sizeof(g_ftm_cmd_buff), 0, sizeof(g_ftm_cmd_buff));

    g_gnss_ftm_cfg->test_item         = 0;
    g_gnss_ftm_cfg->signal_source     = 0;          // 配置信号源模式为单音仪器
    g_gnss_ftm_cfg->satellite_type    = 0;
    g_gnss_ftm_cfg->satellite_svid    = 0;
    g_gnss_ftm_cfg->satellite_doppler = 200000;     // 配置多普勒偏移200000Hz

    g_gnss_ftm_cfg->expected_cn0      = 400;        // CN0预期结果是40dB，上报时会放大10倍，是400
    g_gnss_ftm_cfg->max_cn0_bias      = 20;         // 波动不超过2dB，上报时会放大10倍，是20
    g_gnss_ftm_cfg->max_freq_bias     = 3000;       // 波动不超过不超过3000ppb
    g_gnss_ftm_cfg->max_freq_drift    = 5;          // 配置频漂门限为5ppb/s

    g_gnss_ftm_msg->cmd  = GNSS_FACTORY_TEST_COMMAND;
    g_gnss_ftm_msg->dataLength   = sizeof(gnss_factory_test_config);
    g_gnss_ftm_msg->checkSum = calc_checksum(g_gnss_ftm_msg->data, g_gnss_ftm_msg->dataLength);
    g_gnss_ftm_msg->sequence = 0;

    errcode_t ret = uapi_gnss_config(g_ftm_cmd_buff);
    if (ret != ERRCODE_SUCC) {
        GNSS_LOG_ERROR("[FACTORY ERROR] write factory test cmd failed, error code: %#x\n", ret);
        return ret;
    }

    GNSS_LOG_DEBUG("[FACTORY DEBUG] set factory test mode success\n");
    return ret;
}

void custom_fac_proc_data(uint8_t *buff)
{
    GnssMsg *msg = (GnssMsg*)buff;
    if (msg == NULL) {
        GNSS_LOG_ERROR("[FACTORY ERROR] fac data is null");
        return;
    }

    if (msg->cmd != GNSS_FACTORY_TEST_RESULT) {
        return;
    }

    if (msg->dataLength != sizeof(gnss_factory_test_result)) {
        GNSS_LOG_ERROR("[FACTORY ERROR] ftm message dataLength invalid. expect=%d, report=%u\n",
            sizeof(gnss_factory_test_result), msg->dataLength);
        return;
    }

    gnss_factory_test_result *rst = (gnss_factory_test_result*)(msg->data);
    if (rst->test_item != 0) {
        GNSS_LOG_ERROR("[FACTORY ERROR]  ftm message type invalid. expect=%u, report=%u\n", 0, rst->test_item);
        return;
    }

    bool overview_status_succ = (rst->test_status == 0x01) ? true : false;
    bool satellite_cn0_succ   = (rst->test_status & 0x04) == 0 ? true : false;
    bool freq_bias_succ       = (rst->test_status & 0x08) == 0 ? true : false;
    bool freq_drift_succ      = (rst->test_status & 0x10) == 0 ? true : false;

    // search signal failed, then all test should be failed.
    if (rst->test_status == 0x02) {
        satellite_cn0_succ = false;
        freq_bias_succ     = false;
        freq_drift_succ    = false;
    }

    GNSS_LOG_INFO("gnss factory test        %8s %8s %8s\n", "expect", "report", "result");
    GNSS_LOG_INFO("----------------------------------------------------------\n");
    GNSS_LOG_INFO("overview status        : %8s 0x%06X %8s\n",
        " ", rst->test_status, overview_status_succ ? "success" : "failed");
    GNSS_LOG_INFO("satellite cn0 (dBHz)   : %8.1lf %8.1lf %8s\n",
        // 10.0是因为CN0上报时放大了10倍
        g_gnss_ftm_cfg->expected_cn0 / 10.0, rst->satellite_cn0 / 10.0, satellite_cn0_succ ? "success" : "failed");
    GNSS_LOG_INFO("frequency bias (ppb)   : %8u %8d %8s\n",
        g_gnss_ftm_cfg->max_freq_bias, rst->freq_bias, freq_bias_succ ? "success" : "failed");
    GNSS_LOG_INFO("frequency drift (ppb/s): %8u %8d %8s\n",
        g_gnss_ftm_cfg->max_freq_drift, rst->freq_drift, freq_drift_succ ? "success" : "failed");
    GNSS_LOG_INFO("----------------------------------------------------------\n");
    GNSS_LOG_INFO("+GPS:CN0[%lf,%s],BIAS[%d,%s],DRIFT[%d,%s],RESULT[%s]\n", rst->satellite_cn0 / 10.0, satellite_cn0_succ ?  "PASS" : "FAIL",rst->freq_bias, freq_bias_succ ? "PASS" : "FAIL",rst->freq_drift, freq_drift_succ ? "PASS" : "FAIL",(freq_drift_succ && satellite_cn0_succ && freq_drift_succ)? "PASS" : "FAIL");
}

errcode_t custom_gnss_factory_test(void)
{
    // parse command line arguments
    errcode_t ret = custom_gnss_set_factory_test();
    if (ret != ERRCODE_SUCC) {
        GNSS_LOG_ERROR("[FACTORY ERROR] set factory test error, error code: %#x\n", ret);
        return ret;
    }

    return ret;
}
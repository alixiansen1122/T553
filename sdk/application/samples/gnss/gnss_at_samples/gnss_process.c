/*
 * Copyright (c) @CompanyNameMagicTag 2025-2025. All rights reserved.
 * Description: gnss process AT cmd
 * STRATEGY : NO
 */

#include "gnss_process.h"
#include "common_def.h"
#include "gnss_common_proc.h"
#include "gnss_agnss.h"
#include "gnss_normal.h"
#include "gnss_pgnss.h"
#include "gnss_factory_test.h"
#include "gnss_device.h"
#include "gnss_log.h"

#define MSG_LEN_MAX             128
#define CMD_REQUEST             0
#define CMD_COLD_START_SAMPLE   1
#define CMD_HOT_START_SAMPLE    2
#define CMD_AGNSS_SAMPLE        3
#define CMD_PGNSS_SAMPLE        4
#define CMD_FAC_TEST_SAMPLE     5

typedef errcode_t (*gnss_sample_func)(void);
static void print_sample_list(void);

typedef struct {
    uint16_t id;
    gnss_sample_func func;
    const char* funcName;
} gnss_sample_table;

errcode_t gnss_proc_msg_init(uint8_t nmeaFlag, uint8_t replayFlag, uint8_t resv1, uint8_t resv2)
{
    GNSS_LOG_DEBUG("[GNSS DEBUG]GnssProcMsgInit, nmeaFlag: %u, replayFlag: %u, resv1: %u, resv2: %u\n",
        nmeaFlag, replayFlag, resv1, resv2);
    update_binary_save_flag(replayFlag);
    update_nmea_save_flag(nmeaFlag);
    return gnss_init();
}

errcode_t gnss_proc_msg_start(void)
{
    GNSS_LOG_DEBUG("[GNSS DEBUG]GnssProcMsgStart\n");
    return gnss_start();
}

errcode_t gnss_proc_msg_stop(void)
{
    GNSS_LOG_DEBUG("[GNSS DEBUG]GnssProcMsgStop\n");
    return gnss_stop();
}

errcode_t gnss_proc_msg_deinit(void)
{
    GNSS_LOG_DEBUG("[GNSS DEBUG]GnssProcMsgDeInit\n");
    return gnss_deinit();
}

errcode_t gnss_proc_msg_cfg(uint8_t *data, uint16_t len)
{
    char buff[MSG_LEN_MAX] = {0};
    errno_t ret = memcpy_s(buff, MSG_LEN_MAX, data, len);
    if (ret != EOK) {
        GNSS_LOG_ERROR("[GNSS ERROR] gnss_proc_msg_cfg MEMCPY FAIL\n");
        return ERRCODE_FAIL;
    }
    return uapi_gnss_config(buff);
}

static errcode_t default_func(void)
{
    return ERRCODE_SUCC;
}

errcode_t gnss_proc_log_ctrl_info(uint32_t logNumLimit, uint32_t logSizeLimit)
{
    return update_gnss_log_ctrl_info(logNumLimit, logSizeLimit);
}
 
static const gnss_sample_table g_sample_funcs[] = {
    {CMD_REQUEST,               print_sample_list,  "print_sample_list" },
    {CMD_COLD_START_SAMPLE,     gnss_cold_start,    "gnss_cold_start"   },
    {CMD_HOT_START_SAMPLE,      gnss_hot_start,     "gnss_hot_start"    },
    {CMD_AGNSS_SAMPLE,          gnss_agnss,         "gnss_agnss"        },
    {CMD_PGNSS_SAMPLE,          gnss_pgnss,         "gnss_pgnss"        },
    {CMD_FAC_TEST_SAMPLE,       gnss_factory_test,  "gnss_factory_test" },
};

static void print_sample_list(void)
{
    GNSS_LOG_INFO("####################################gnss sample mode###################################\n"
        "##                                                                                             \n"
        "## Menu:                                                                                       \n");
    for (uint32_t i = 0; i < array_size(g_sample_funcs); i++) {
        GNSS_LOG_INFO("##     %u. %s\n", i, g_sample_funcs[i].funcName);
    }
    GNSS_LOG_INFO("########################################################################################\n\n");
}

errcode_t gnss_proc_msg_sample(uint8_t index)
{
    GNSS_LOG_DEBUG("[GNSS DEBUG]GnssProcMsgSample, index: %u\n", index);
    errcode_t ret = ERRCODE_FAIL;
    for (uint32_t i = 0; i < array_size(g_sample_funcs); i++) {
        if (index == g_sample_funcs[i].id) {
            ret = g_sample_funcs[i].func();
        }
    }
    return ret;
}

errcode_t gnss_proc_msg_nmea(uint8_t index)
{
    GNSS_LOG_DEBUG("[GNSS DEBUG]set gnss nmea print, flag: %u\n", index);
    update_gnss_nmea_print_flag(index);
    return ERRCODE_SUCC;
}
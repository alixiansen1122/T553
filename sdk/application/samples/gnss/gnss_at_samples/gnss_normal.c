/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: gnss normal sample
 * Author:
 * Create:
 */

#include "gnss_normal.h"
#include "gnss_common_proc.h"
#include "gnss_device.h"
#include "gnss_log.h"

errcode_t gnss_cold_start(void)
{
    GNSS_LOG_DEBUG("[GNSS DEBUG] enter gnss cold start sample\n");

    const uint8_t cfgColdStartCmd[] = {0x03, 0x00, 0x03, 0x00, 0x00, 0x00, 0x05, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01};

    errcode_t ret = uapi_gnss_config(cfgColdStartCmd);
    if (ret != ERRCODE_SUCC) {
        GNSS_LOG_ERROR("[GNSS ERROR] gnss configure cold start fail, error code: %#x\n", ret);
    }
    return ret;
}

errcode_t gnss_hot_start(void)
{
    GNSS_LOG_DEBUG("[GNSS DEBUG] enter gnss hot start sample\n");

    const uint8_t cfgHotStartCmd[] = {0x03, 0x00, 0x02, 0x00, 0x00, 0x00, 0x05, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00};

    errcode_t ret = uapi_gnss_config(cfgHotStartCmd);
    if (ret != ERRCODE_SUCC) {
        GNSS_LOG_ERROR("[GNSS ERROR] gnss configure hot start fail, error code: %#x\n", ret);
    }
    return ret;
}
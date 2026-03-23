/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingAboutModel
 * Create: 2025-06-01
 */
#include "bts_br_gap.h"
#if !defined(_WIN32)
#include "nv.h"
#endif
#include "nv_config/include/common.h"
#include "settings/common/SettingCommon.h"
#include "settings/model/SettingAboutModel.h"

namespace OHOS {
constexpr static uint8_t SYS_NAME_LEN = 32;

std::string SettingAboutModel::GetSystemName()
{
#if !defined(_WIN32)
    unsigned char sysName[SYS_NAME_LEN] = "";
    unsigned char sysNameLen = SYS_NAME_LEN;
    if (bluetooth_get_local_name(sysName, &sysNameLen) != ERRCODE_SUCC) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingAboutModel::GetSystemName error");
        return "";
    }
    return std::string((const char*)sysName);
#else
    return std::string("WATCH 2 0322");
#endif
}

std::string SettingAboutModel::GetSystemType()
{
#if !defined(_WIN32)
    sys_product_info_t info = {0};
    uint16_t nvsize = 0;
    errcode_t ret = uapi_nv_read(NV_ID_SYS_PRODUCT_INFO, sizeof(sys_product_info_t), &nvsize, (uint8_t *)&info);
    if (ret != ERRCODE_SUCC) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingAboutModel::GetSystemType error, ret=%u", ret);
        return "";
    }
    if (nvsize < sizeof(sys_product_info_t) || info.size == 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "invalid system type: nvsize=%u, info.size=%u", nvsize, info.size);
        return "";
    }
    return std::string((const char*)info.name);
#else
    return std::string("WATCH-BX9");
#endif
}
}

/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingSystemModel
 * Create: 2025-06-01
 */
#ifndef SETTING_SYSTEM_MODEL_H
#define SETTING_SYSTEM_MODEL_H

#include <cstdint>
#include <mutex>
#include <string>
#ifndef _WIN32
#include "osal_task.h"
#endif
#include "settings/common/SettingUnionItemGroup.h"
#include "settings/common/SettingRefSingleton.h"

namespace OHOS {
class SettingSystemModel : public SettingRefSingleton<SettingSystemModel> {
public:
    SettingSystemModel();
    virtual ~SettingSystemModel();
    std::vector<SettingUnionItemData>& GetSystemUnionData();
    void SystemOperation(const char* operationName);
    // OTA
    bool GetOtaIsUpgrade();
    uint32_t GetOtaPackageSize();
    void StartOtaNativeUpgrade();
    void StopOtaNativeUpgrade();
    uint8_t GetOtaNativeUpgradePercent();
    std::string GetOtaNativeUpgradeMessage();
protected:
    void RestoreJSApplication();
    void RestoreNativeApplication();
    void RestoreKvstore();
    void SetOtaNativeUpgradePercent(uint8_t percent);
    void SetOtaNativeUpgradeMessage(std::string message);
    static int SystemOta(void *data);
    static void UninstallCallback(const uint8_t resultCode, const void *resultMessage);
private:
    std::vector<SettingUnionItemData> systemUnionData_;
    std::mutex otaOSALMtx_;
#ifndef _WIN32
    osal_task* systemOtaTask_ = nullptr;
#endif
    uint8_t otaUpgradePrecent_ = 0;
    std::string otaUpgradeMessage_;
};
} // OHOS
#endif
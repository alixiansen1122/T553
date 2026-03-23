/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingSystemModel
 * Create: 2025-06-01
 */
#include <cstring>
#include <cstdint>
#include <cstdio>
#include <dirent.h>
#include <list>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#if !defined(_WIN32)
#include "tcxo.h"
#include "upg.h"
#include "upg_porting.h"
#include "upg_definitions.h"
#include "upg_common_porting.h"
#endif
#include "dfx_reboot.h"
#include "bundlems_slite_client.h"
#include "abilityms_slite_client.h"
#include "bundle_info.h"
#include "adapter.h"
#ifdef SUPPORT_ALIPAY_SEC
#include "alipay_feature.h"
#endif
#include "UiConfig.h"
#include "wearable_log.h"
#include "sport/SportModel.h"
#include "gfx_utils/list.h"
#include "bts_def.h"
#include "bts_br_gap.h"
#include "flashlight/FlashLightModel.h"
#include "settings/model/SettingSystemModel.h"


namespace OHOS {
#define MODLE_THREAD_STACK_SIZE 4096
static constexpr uint8_t SETTING_SYSTEM_OTA_PERCENT_5 = 5;
static constexpr uint8_t SETTING_SYSTEM_OTA_PERCENT_10 = 10;
static constexpr uint8_t SETTING_SYSTEM_OTA_PERCENT_25 = 25;
static constexpr uint8_t SETTING_SYSTEM_OTA_PERCENT_40 = 40;
static constexpr uint8_t SETTING_SYSTEM_OTA_PERCENT_50 = 50;
static constexpr uint8_t SETTING_SYSTEM_OTA_PERCENT_100 = 100;

SettingSystemModel::SettingSystemModel()
{
    std::vector<SettingUnionItemData> tempUnionData = {{
        PlainTextItemData{"系统和升级"},
        IconTextNavigationItemData{SettingPageId::SYSTEM_OPERATION_PAGE, IMAGE_SYSTEM_LIST_POWEROFF, "关机",
            nullptr, true, SETTING_LIST_DEFAULT_BG_COLOR, SETTING_LIST_DEFAULT_BG_COLOR_OPA},
        IconTextNavigationItemData{SettingPageId::SYSTEM_OPERATION_PAGE, IMAGE_SYSTEM_LIST_RESTART, "重启",
            nullptr, true, SETTING_LIST_DEFAULT_BG_COLOR, SETTING_LIST_DEFAULT_BG_COLOR_OPA},
        IconTextNavigationItemData{SettingPageId::SYSTEM_OPERATION_PAGE, IMAGE_SYSTEM_LIST_RESUME, "恢复出厂",
            nullptr, true, SETTING_LIST_DEFAULT_BG_COLOR, SETTING_LIST_DEFAULT_BG_COLOR_OPA},
        IconTextNavigationItemData{SettingPageId::SYSTEM_OTA_PAGE, IMAGE_SYSTEM_LIST_OTA, "OTA升级",
            nullptr, true, SETTING_LIST_DEFAULT_BG_COLOR, SETTING_LIST_DEFAULT_BG_COLOR_OPA},
    }};
    systemUnionData_.swap(tempUnionData);
}

SettingSystemModel::~SettingSystemModel()
{
#if !defined(_WIN32)
    if (systemOtaTask_ != nullptr) {
        osal_kthread_destroy(systemOtaTask_, 0);
    }
#endif
}

std::vector<SettingUnionItemData>& SettingSystemModel::GetSystemUnionData()
{
    return systemUnionData_;
}

void SettingSystemModel::SystemOperation(const char* operationName)
{
#if !defined(_WIN32)
    if (strcmp(operationName, "关机") == 0) {
        uapi_system_shutdown(0);
    } else if (strcmp(operationName, "重启") == 0) {
        uapi_system_reboot(SYSTEM_SOFT_REBOOT);
    } else if (strcmp(operationName, "恢复出厂") == 0) {
        RestoreJSApplication();
        RestoreNativeApplication();
        RestoreKvstore();
        uapi_system_reboot(SYSTEM_SOFT_REBOOT);
    }
#endif
}

void SettingSystemModel::RestoreJSApplication()
{
#if !defined(_WIN32)
    bool res = bluetooth_factory_reset();
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "bluetooth_factory_reset res:%u.", (uint32_t)res);
#ifdef SUPPORT_ALIPAY_SEC
    (void)uapi_alipay_svr_reset();
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "uapi_alipay_svr_reset process.");
#endif

    BundleInfo *bundleInfos = nullptr;
    int32_t     bundleCount = 0;
    uint32_t    err = ERRCODE_FAIL;

    err = BundleMsClient::GetInstance().GetBundleInfos(0, &bundleInfos, &bundleCount);
    if (err != 0) {
        bundleCount = 0;
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "Get bundleInfos Failed, errno is %d\n", err);
    }

    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "----------------- AppList Start[%d] ------------------", bundleCount);
    for (int32_t i = 0; i<bundleCount; i++) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "App:%03d label:%-20s bundleName:%-20s",
                      i, bundleInfos[i].label, bundleInfos[i].bundleName);
        ElementName* topAbility = AbilityMsClient::GetInstance().GetTopAbility();

        if ((topAbility != nullptr) && (strcmp(topAbility->bundleName, bundleInfos[i].bundleName) == 0)) {
            AbilityMsClient::GetInstance().TerminateAbility(0);
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "terminate %-20s", bundleInfos[i].bundleName);
        }

        InstallParam installParam = {.installLocation = 1, .keepData = false};
        BundleMsClient::GetInstance().Uninstall(bundleInfos[i].bundleName, &installParam,
                                                SettingSystemModel::UninstallCallback);
        FreeElement(topAbility);
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "----------------- AppList End[%d] ------------------", bundleCount);

    if (bundleInfos != nullptr && bundleCount != 0) {
        for (int32_t i = 0; i<bundleCount; i++) {
            ClearBundleInfo(&bundleInfos[i]);
        }
        AdapterFree(bundleInfos);
        bundleInfos = nullptr;
    }
#endif
}

void SettingSystemModel::RestoreNativeApplication()
{
    SportModel::GetInstance().ClearData();
    FlashLightModel::GetInstance().SetDefault();
}

void SettingSystemModel::RestoreKvstore()
{
#if !defined(_WIN32)
#ifndef XTS_SUPPORT
    const char* path = "/user/kvstore";
    DIR* dir = opendir(path);
    if (!dir) return;
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        std::string full_path = std::string(path) + "/" + entry->d_name;
        struct stat statbuf;
        if (lstat(full_path.c_str(), &statbuf) == -1)
            continue;
        if (!S_ISDIR(statbuf.st_mode)) {
            unlink(full_path.c_str());
        }
    }
    closedir(dir);
#endif
#endif
}

void SettingSystemModel::UninstallCallback(const uint8_t resultCode, const void *resultMessage)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "Uninstall %-20s, %u", resultMessage, resultCode);
}

uint32_t SettingSystemModel::GetOtaPackageSize()
{
#if !defined(_WIN32)
    FILE *fp = fopen(upg_get_pkg_file_path(), "rb");
    if (fp == nullptr) {
        return 0;
    }
    fseek(fp, 0L, SEEK_END);
    uint32_t total_size = ftell(fp);
    fclose(fp);

    return total_size;
#else
    return 0;
#endif
}

bool SettingSystemModel::GetOtaIsUpgrade()
{
    std::lock_guard<std::mutex> lock(otaOSALMtx_);
#ifndef _WIN32
    return systemOtaTask_ != nullptr;
#else
    return false;
#endif
}

uint8_t SettingSystemModel::GetOtaNativeUpgradePercent()
{
    std::lock_guard<std::mutex> lock(otaOSALMtx_);
    return otaUpgradePrecent_;
}

std::string SettingSystemModel::GetOtaNativeUpgradeMessage()
{
    std::lock_guard<std::mutex> lock(otaOSALMtx_);
    return otaUpgradeMessage_;
}

void SettingSystemModel::SetOtaNativeUpgradePercent(uint8_t percent)
{
    std::lock_guard<std::mutex> lock(otaOSALMtx_);
    otaUpgradePrecent_ = percent;
}

void SettingSystemModel::SetOtaNativeUpgradeMessage(std::string message)
{
    std::lock_guard<std::mutex> lock(otaOSALMtx_);
    otaUpgradeMessage_ = message;
}

void SettingSystemModel::StartOtaNativeUpgrade()
{
#if !defined(_WIN32)
    systemOtaTask_ = osal_kthread_create(&SettingSystemModel::SystemOta, this,
        "sys_ota", MODLE_THREAD_STACK_SIZE);
    if (systemOtaTask_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "ota osal_wait_init error");
        return;
    }
#endif
}

void SettingSystemModel::StopOtaNativeUpgrade()
{
#if !defined(_WIN32)
    if (systemOtaTask_ != nullptr) {
        osal_kthread_destroy(systemOtaTask_, 0);
        systemOtaTask_ = nullptr;
    }
#endif
}

int SettingSystemModel::SystemOta(void *data)
{
    SettingSystemModel* model = (SettingSystemModel*)data;
    do {
#if !defined(_WIN32)
        upg_prepare_info_t info;
        model->SetOtaNativeUpgradePercent(SETTING_SYSTEM_OTA_PERCENT_5);
        model->SetOtaNativeUpgradeMessage("检查文件...");
        info.package_len = model->GetOtaPackageSize();
        if (info.package_len == 0) {
            model->SetOtaNativeUpgradeMessage("检查文件失败");
            break;
        }
        model->SetOtaNativeUpgradePercent(SETTING_SYSTEM_OTA_PERCENT_10);
        model->SetOtaNativeUpgradeMessage("准备中...");
        errcode_t ret = uapi_upg_prepare(&info);
        if (ret != ERRCODE_SUCC) {
            model->SetOtaNativeUpgradeMessage("准备失败");
            break;
        }
        model->SetOtaNativeUpgradePercent(SETTING_SYSTEM_OTA_PERCENT_25);
        model->SetOtaNativeUpgradeMessage("申请升级...");
        ret = uapi_upg_request_upgrade(false);
        if (ret != ERRCODE_SUCC) {
            model->SetOtaNativeUpgradeMessage("申请升级失败");
            break;
        }
        model->SetOtaNativeUpgradePercent(SETTING_SYSTEM_OTA_PERCENT_40);
        model->SetOtaNativeUpgradeMessage("备份系统...");
        ret = upg_image_backups_update();
        if (ret != ERRCODE_SUCC) {
            model->SetOtaNativeUpgradeMessage("备份失败");
            break;
        }
        model->SetOtaNativeUpgradePercent(SETTING_SYSTEM_OTA_PERCENT_50);
        model->SetOtaNativeUpgradeMessage("升级中...");
        ret = uapi_upg_start();
        if (ret != ERRCODE_SUCC) {
            model->SetOtaNativeUpgradeMessage("升级失败");
            break;
        }
        model->SetOtaNativeUpgradePercent(SETTING_SYSTEM_OTA_PERCENT_100);
        model->SetOtaNativeUpgradeMessage("即将重启...");
        uapi_system_reboot(SYSTEM_SOFT_REBOOT);
#else
        model->SetOtaNativeUpgradePercent(SETTING_SYSTEM_OTA_PERCENT_100);
        model->SetOtaNativeUpgradeMessage("即将重启...");
#endif
    } while (false);
    model->StopOtaNativeUpgrade(); // 停止线程
    return 0;
}
} // OHOS

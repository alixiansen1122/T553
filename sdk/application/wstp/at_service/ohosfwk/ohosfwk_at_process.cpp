/**
 * Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides ohos sample. \n
 * Author: CompanyName \n
 * History: \n
 * 2022-11-17， Create file. \n
 */
#include "stdlib.h"
#include "sys/time.h"
#include "time.h"
#include "common_def.h"
#include "wearable_log.h"
#include "ohosfwk_at_process.h"
#include "bundlems_slite_client.h"
#include "abilityms_slite_client.h"
#include "gt_bundle_manager_service.h"
#include "uiservice/ui_service.h"
#include "sign_mode.h"
#include "NativeAbility.h"
#include "ability_manager.h"
#include "js_fwk_common.h"
#include "global.h"
#include "gt_bundle_manager_service.h"
#include "msg_center_customer.h"
#include "applist/ApplistModel.h"
#include "bundle_manager.h"
#include "js_ability.h"
#include <unistd.h>
#ifdef SUPPORT_AT_LOCATION
#include "location_sample.h"
#include <cmsis_os.h>
#endif
#ifdef SUPPORT_AT_SENSOR
#include "sensor_sample.h"
#endif
#include "monkey/MonkeyModel.h"

#define ARRAY_COUNT(array) (sizeof(array) / sizeof(array[0]))
#define OHOSFWK_MAX_ARGC            8
#define OHOSFWK_MAX_ARGV_LEN        64

#define OHOSFWK_SUCCESS             0
#define OHOSFWK_FAILED              (-1)
#define MAX_PAGE 100
#define ARGC_NUM 2
#define DEFAULT_TIME_ZONE           (8 * 60)
#define MINUTES_PER_HOUR 60
#define SECONDS_PER_MINUTE 60
#define USEC_TO_MILLI           1000
#define TIME_MAX                64
#define DECIMAL                     10
#define TIME_YEAR_OFFSET        70
#define MIN_TIMEZONE_OFFSET (-12 * 60)
#define MAX_TIMEZONE_OFFSET (12 * 60)

enum {
    UNDEFINED_STATE = -1,
    INIT_STATE, // 0
    READY_STATE,
    SHOW_STATE,
    BACKGROUND_STATE,
    DESTROY_STATE,
    END_STATE
};

static void ReceiveCallback(const uint8_t resultCode, const void *resultMessage)
{
    BundleInstallMsg *installMsg = (BundleInstallMsg *)resultMessage;
    if (resultCode == 0) {
        BundleData bundleData;
        memset_s(&bundleData, sizeof(BundleData), 0, sizeof(BundleData));
        strncpy_s(bundleData.bundleName, BUNDLE_NAME_LEN, installMsg->bundleName, BUNDLE_NAME_LEN);

        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "bms resultCode = 0x%x", resultCode);
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "bms bundleName = %s", bundleData.bundleName);

        if (installMsg->installState == BUNDLE_INSTALL_OK) {
            strncpy_s(bundleData.label, BUNDLE_PATH_LEN, installMsg->label, BUNDLE_PATH_LEN);
            strncpy_s(bundleData.smallIconPath, BUNDLE_PATH_LEN, installMsg->smallIconPath, BUNDLE_PATH_LEN);
            strncpy_s(bundleData.bigIconPath, BUNDLE_PATH_LEN, installMsg->bigIconPath, BUNDLE_PATH_LEN);
            SendMsgToUIService(JS_INSTALL_SUCESS_TO_UI, 0, &bundleData, sizeof(BundleData));
        } else if (installMsg->installState == BUNDLE_UNINSTALL_OK) {
            SendMsgToUIService(JS_UNINSTALL_SUCESS_TO_UI, 0, &bundleData, sizeof(BundleData));
        }
    } else {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "bms resultCode = 0x%x", resultCode);
    }
    AdapterFree(installMsg);
}

static int ConvertCmdToParam(uint8_t *para, uint32_t paraLen,
        uint8_t argv[OHOSFWK_MAX_ARGC][OHOSFWK_MAX_ARGV_LEN], uint32_t* argc)
{
    uint32_t pos = 0;
    *argc = 0;

    for (uint32_t i = 0; i < paraLen; i++) {
        if ((para[i] == ' ' || para[i] == '\n') && pos != 0) {
            argv[*argc][pos] = '\0';
            *argc += 1;
            pos = 0;
            if (*argc >= OHOSFWK_MAX_ARGC) {
                return OHOSFWK_FAILED;
            }
        } else {
            argv[*argc][pos] = para[i];
            pos++;
            if (pos >= OHOSFWK_MAX_ARGV_LEN) {
                return OHOSFWK_FAILED;
            }
        }
    }
    if (pos != 0) {
        *argc += 1;
    }

    return OHOSFWK_SUCCESS;
}

static uint32_t OhosfwkATCmdAmExitJsApp(uint8_t *para, uint32_t paraLen, char *strCommonCmd)
{
    unused(para);
    unused(paraLen);
    unused(strCommonCmd);

    ElementName *elementName = OHOS::AbilityMsClient::GetInstance().GetTopAbility();
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "OHOSFWK ATCMD Stop %s\n", elementName->bundleName);
    OHOS::AbilityMsClient::GetInstance().ForceStop(elementName->bundleName);
    FreeElement(elementName);
    return ERRCODE_SUCC;
}

static const char *StateInfoToString(int8_t pageInfo)
{
    switch (pageInfo) {
        case UNDEFINED_STATE:
            return "-1 - UNDEFINED_STATE";
        case INIT_STATE:
            return "0 - INIT_STATE";
        case READY_STATE:
            return "1 - READY_STATE";
        case SHOW_STATE:
            return "2 - SHOW_STATE";
        case BACKGROUND_STATE:
            return "3 - BACKGROUND_STATE";
        case DESTROY_STATE:
            return "4 - DESTROY_STATE";
        case END_STATE:
            return "5 - END_STATE";
        default:
            return "Can not get state!";
    }
}

static uint32_t OhosfwkATCmdAmDfx(uint8_t *para, uint32_t paraLen, char *strCommonCmd)
{
    unused(strCommonCmd);

    uint8_t argv[OHOSFWK_MAX_ARGC][OHOSFWK_MAX_ARGV_LEN] = {0};
    uint32_t argc = 0;
    int ret = ConvertCmdToParam(para, paraLen, argv, &argc);
    if (ret != OHOSFWK_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "OHOSFWK ATCMD Convert Command Failed\n");
        return ERRCODE_FAIL;
    }

    char *info = reinterpret_cast<char *>(argv[0]);
    if (info == NULL) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "failed to get info!\n");
        return ERRCODE_FAIL;
    }

    if (strcmp(info, "dump") == 0) {
        ElementName *elementName = OHOS::AbilityMsClient::GetInstance().GetTopAbility();
        if (elementName == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "failed to get elementName!\n");
            return ERRCODE_FAIL;
        }
        if (elementName->bundleName == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "failed to get bundleName!\n");
            return ERRCODE_FAIL;
        }
        char *currentBundleName = elementName->bundleName;

        BundleInfo bundleInfo;
        ret = OHOS::BundleMsClient::GetInstance().GetBundleInfo(currentBundleName, 0, &bundleInfo);
        if (ret != OHOSFWK_SUCCESS) {
            PRINTK("Native App: %s\n", currentBundleName);
            PRINTK("Can not provide BundleInfo\n");
            return ERRCODE_SUCC;
        }
        int8_t pageInfo = OHOS::ACELite::GetJsPageStateInfo();
        const char *pageInfoLog = StateInfoToString(pageInfo);
        PRINTK("-------------PageState-------------\n");
        PRINTK("label: %s\n", bundleInfo.label);
        PRINTK("bundleName: %s\n", bundleInfo.bundleName);
        PRINTK("pageState: %s\n", pageInfoLog);
        PRINTK("-----------PageState End-----------\n");
    }
    return ERRCODE_SUCC;
}

static uint32_t OhosfwkATCmdBmInstall(uint8_t *para, uint32_t paraLen, char *strCommonCmd)
{
    unused(strCommonCmd);
    uint8_t argv[OHOSFWK_MAX_ARGC][OHOSFWK_MAX_ARGV_LEN] = { 0 };
    uint32_t argc = 0;

    int ret = ConvertCmdToParam(para, paraLen, argv, &argc);
    if (ret != OHOSFWK_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "OHOSFWK ATCMD Convert Command Failed\n");
        return ERRCODE_FAIL;
    }

    if (argc > 2) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "OHOSFWK ATCMD Installing Package Failed\n");
        return ERRCODE_FAIL;
    }
    InstallParam installParam = { .installLocation = 1, .keepData = true };
    if (strcmp("rmdata", (char*)argv[1]) == 0) {
        installParam.keepData = false;
    }
    OHOS::BundleMsClient::GetInstance().Install((char*)argv[0], &installParam, ReceiveCallback);

    return ERRCODE_SUCC;
}

static uint32_t OhosfwkATCmdBmUninstall(uint8_t *para, uint32_t paraLen, char *strCommonCmd)
{
    unused(strCommonCmd);
    uint8_t argv[OHOSFWK_MAX_ARGC][OHOSFWK_MAX_ARGV_LEN] = { 0 };
    uint32_t argc = 0;

    int ret = ConvertCmdToParam(para, paraLen, argv, &argc);
    if (ret != OHOSFWK_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "OHOSFWK ATCMD Convert Command Failed\n");
        return ERRCODE_FAIL;
    }

    if (argc != 1) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "OHOSFWK ATCMD Uninstalling Package Failed\n");
        return ERRCODE_FAIL;
    }

    char *bundleName = (char *)argv[0];
    ElementName* topAbility = OHOS::AbilityMsClient::GetInstance().GetTopAbility();

    if ((topAbility != nullptr) && (strcmp(topAbility->bundleName, bundleName) == 0)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "js app is active, first need terminate");
        int token = 0;
        OHOS::AbilityMsClient::GetInstance().TerminateAbility(token);
    }

    InstallParam installParam = { .installLocation = 1, .keepData = false };
    OHOS::BundleMsClient::GetInstance().Uninstall(bundleName, &installParam, ReceiveCallback);
    FreeElement(topAbility);
    return ERRCODE_SUCC;
}

static uint32_t OhosfwkATCmdBmSet(uint8_t *para, uint32_t paraLen, char *strCommonCmd)
{
    unused(strCommonCmd);
    uint8_t argv[OHOSFWK_MAX_ARGC][OHOSFWK_MAX_ARGV_LEN] = { 0 };
    uint32_t argc = 0;

    int ret = ConvertCmdToParam(para, paraLen, argv, &argc);
    if (ret != OHOSFWK_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "OHOSFWK ATCMD Convert Command Failed\n");
        return ERRCODE_FAIL;
    }

    if (argc != 1) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "OHOSFWK ATCMD Set Parameter Failed\n");
        return ERRCODE_FAIL;
    }

    char *enable = (char *)argv[0];
    if (strcmp(enable, "enable") == 0) {
        OHOS::SignMode::GetInstance().SetSignMode(true);
    } else if (strcmp(enable, "disable") == 0) {
        OHOS::SignMode::GetInstance().SetSignMode(false);
    } else {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "input failed!");
    }
    return ERRCODE_SUCC;
}

static uint32_t OhosfwkATCmdAmChangeNativeSlice(uint8_t *para, uint32_t paraLen, char *strCommonCmd)
{
    unused(strCommonCmd);
    uint8_t argv[OHOSFWK_MAX_ARGC][OHOSFWK_MAX_ARGV_LEN] = { 0 };
    uint32_t argc = 0;
    int ret = ConvertCmdToParam(para, paraLen, argv, &argc);
    if (ret != OHOSFWK_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "OHOSFWK ATCMD Convert Command Failed\n");
        return ERRCODE_FAIL;
    }
    if (argc != 1) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "OHOSFWK ATCMD Set Parameter Failed\n");
        return ERRCODE_FAIL;
    }
    char *stringAppViewId = (char *)argv[0];
    uint16_t AppViewId = static_cast<uint16_t>(std::stoi(stringAppViewId));
    OHOS::NativeAbility::GetInstance().ChangeSlice(AppViewId);
    return ERRCODE_SUCC;
}

static uint32_t OhosfwkATCmdAmStartJsApp(uint8_t *para, uint32_t paraLen, char *strCommonCmd)
{
    unused(strCommonCmd);
    uint8_t argv[OHOSFWK_MAX_ARGC][OHOSFWK_MAX_ARGV_LEN] = { 0 };
    uint32_t argc = 0;

    int ret = ConvertCmdToParam(para, paraLen, argv, &argc);
    if (ret != OHOSFWK_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "OHOSFWK ATCMD Convert Command Failed\n");
        return ERRCODE_FAIL;
    }

    if (argc != 1) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "OHOSFWK ATCMD Set Parameter Failed\n");
        return ERRCODE_FAIL;
    }
    Want *want = new Want();
    memset_s(want, sizeof(Want), 0, sizeof(Want));

    ElementName startElement = {};
    startElement.abilityName = (char *)"default";
    startElement.bundleName = (char *)argv[0];
    startElement.deviceId = nullptr;
    SetWantElement(want, startElement);
    StartAbility(want);
    ClearWant(want);
    delete want;
    return ERRCODE_SUCC;
}

static uint32_t OhosfwkATCmdDfxJsMemoryInfo(uint8_t *para, uint32_t paraLen, char *strCommonCmd)
{
    OHOS::ACELite::DumpPrintMemoryInfo();
    return ERRCODE_SUCC;
}

static uint32_t OhosfwkATCmdBmGetAppInfo(uint8_t *para, uint32_t paraLen, char *strCommonCmd)
{
    unused(strCommonCmd);
    uint8_t argv[OHOSFWK_MAX_ARGC][OHOSFWK_MAX_ARGV_LEN] = { 0 };
    uint32_t argc = 0;

    int ret = ConvertCmdToParam(para, paraLen, argv, &argc);
    if (ret != OHOSFWK_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "OHOSFWK ATCMD Convert Command Failed\n");
        return ERRCODE_FAIL;
    }

    char *bundleName = (char *)argv[0];
    if (bundleName == NULL) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "failed to get bundleName!\n");
        return ERRCODE_FAIL;
    }

    BundleInfo bundleInfo;
    ret = OHOS::BundleMsClient::GetInstance().GetBundleInfo(bundleName, 0, &bundleInfo);
    if (ret != OHOSFWK_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "failed to get bundleInfo!");
    } else {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "\n"
        "-----------------AppInfo----------------\n"
        "bundleName: %s\n"
        "appId: %s\n"
        "label: %s\n"
        "versionCode: %d\n"
        "targetApi: %d\n"
        "isSystemApp: %s\n"
        "compatibleApi: %d\n"
        "versionName: %s\n"
        "vendor: %s\n"
        "numOfModule: %d",
        bundleInfo.bundleName,
        bundleInfo.appId,
        bundleInfo.label,
        bundleInfo.versionCode,
        bundleInfo.targetApi,
        bundleInfo.isSystemApp ? "true" : "false",
        bundleInfo.compatibleApi,
        bundleInfo.versionName,
        bundleInfo.vendor,
        bundleInfo.numOfModule
        );

        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "\n"
        "bigIconPath: %s\n"
        "codePath: %s\n"
        "dataPath: %s\n"
        "--------------AppInfo End---------------\n",
        bundleInfo.bigIconPath,
        bundleInfo.codePath,
        bundleInfo.dataPath
        );
    }
    return ERRCODE_SUCC;
}

static uint32_t OhosfwkATCmdBmGetAppList(uint8_t *para, uint32_t paraLen, char *strCommonCmd)
{
    unused(para);
    unused(paraLen);
    unused(strCommonCmd);

    BundleInfo *bundleInfos = nullptr;
    int32_t count = 0;
    int ret = OHOS::BundleMsClient::GetInstance().GetBundleInfos(0, &bundleInfos, &count);
    if (ret != OHOSFWK_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Get bundleInfos Failed\n");
        return ERRCODE_FAIL;
    }else {
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "\n"
            "----------------- AppList Start --------------------\n");
        for (int32_t i = 0; i < count; i++) {

            WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP,
            "App:%d-----------------\n"
            "label:            %s\n"
            "bundleName:       %s\n",
            i+1,
            bundleInfos[i].bundleName,
            bundleInfos[i].label
            );
        }
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "\n"
            "----------------- AppList End -----------------------\n");
    }
    return ERRCODE_SUCC;
}

void ScanPackagesResultCallback(const uint8_t resultCode, const void* resultMessage)
{
    if (resultCode == 0) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "Scan Packages Succeeded.\n");
        OHOS::ApplistModel::GetInstance().SetInitFlag(false);
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "Set InitFlag false Succeeded\n");
    } else {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Scan Packages Failed.\n");
        if (resultMessage != nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[ERROR] %s\n", reinterpret_cast<const char*>(resultMessage));
        }
    }
}

static uint32_t OhosfwkATCmdChangeLanguage(uint8_t *para, uint32_t paraLen, char *strCommonCmd)
{
    unused(strCommonCmd);

    uint8_t argv[OHOSFWK_MAX_ARGC][OHOSFWK_MAX_ARGV_LEN] = { 0 };
    uint32_t argc = 0;
    int ret = ConvertCmdToParam(para, paraLen, argv, &argc);

    if (ret != OHOSFWK_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "OHOSFWK ATCMD Convert Command Failed\n");
        return ERRCODE_FAIL;
    }

    if (argc != 1) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "OHOSFWK ATCMD Language Setting Failed\n");
        return ERRCODE_FAIL;
    }

    char *languageRegion = (char *)argv[0];
    GLOBAL_ConfigLanguage(languageRegion);

    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "Language changed to %s\n", languageRegion);

    ScanPackages(ScanPackagesResultCallback);

    return ERRCODE_SUCC;
}

static uint32_t OhosfwkATStartNotify(uint8_t *para, uint32_t paraLen, char *strCommonCmd)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "in OhosfwkATStartNotify para:%s\n", para);
    uint8_t argv[OHOSFWK_MAX_ARGC][OHOSFWK_MAX_ARGV_LEN] = {0};
    uint32_t argc = 0;
    int ret = ConvertCmdToParam(para, paraLen, argv, &argc);
    if (ret != OHOSFWK_SUCCESS) {
        return ERRCODE_FAIL;
    }
    uint32_t times = 1; // 1 times
    uint32_t msleepTime = 500; // 500ms
    if (argc == 2) {
        char *strtimes = (char *)argv[1];
        times = atoi(strtimes);
    } else if (argc == 3) {
        char *strtimes = (char *)argv[1];
        times = atoi(strtimes);
        char *strMsleepTime = (char *)argv[2];
        msleepTime = atoi(strMsleepTime);
    } else if (argc < 1) {
        return ERRCODE_FAIL;
    }
    char *topicstr = (char *)argv[0];
    uint16_t topicId = atoi(topicstr);
    times = ((msleepTime * times) > 60000) ? 1 : times; // if time is too long times == 1
    for (uint32_t i = 0; i < times; i++) {
        ret = MsgCenterNotifyProc(topicId, nullptr);
        if (ret != true) {
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "times:%d faild\n", i);
            return ERRCODE_FAIL;
        }
        if (i != times - 1) {
            usleep(1000 * msleepTime);
        }
    }
    return ERRCODE_SUCC;
}

#ifdef SUPPORT_AT_LOCATION
static void *LocationTaskEntry(void *argv)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "PgnssTaskEntry\n");
    int ret = LocationSample((const char *)argv);
    if (ret != 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "OHOSFWK ATCMD Set LocationSample Failed\n");
        return nullptr;
    }
    osDelay(300);  // sleep 300ms
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "PgnssExecuteSamples end\n");
    return nullptr;
}

static uint32_t OhosfwkATCmdLocation(uint8_t *para, uint32_t paraLen, char *strCommonCmd)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "in OhosfwkATCmdLocation para:%s\n", para);
    unused(strCommonCmd);
    uint8_t argv[OHOSFWK_MAX_ARGC][OHOSFWK_MAX_ARGV_LEN] = { 0 };
    uint32_t argc = 0;

    int ret = ConvertCmdToParam(para, paraLen, argv, &argc);
    if (ret != OHOSFWK_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "OHOSFWK ATCMD Convert Command Failed\n");
        return ERRCODE_FAIL;
    }

    if (argc != 1) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "OHOSFWK ATCMD Set Parameter Failed\n");
        return ERRCODE_FAIL;
    }
    osThreadAttr_t taskAttr = {"LocationTaskEntry", 0, NULL, 0, NULL, 0x3000, (osPriority_t)(osPriorityHigh), 0, 0};
    // taskAttr.stack_mem = memalign(16, 0x2000);
    osThreadNew((osThreadFunc_t)LocationTaskEntry, argv[0], &taskAttr);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "LocationTaskEntry end\n");
    return ERRCODE_SUCC;
}
#endif

#ifdef SUPPORT_AT_SENSOR
static uint32_t OhosfwkATCmdSensor(uint8_t *para, uint32_t paraLen, char *strCommonCmd)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "in OhosfwkATCmdSensor para:%s\n", para);
    unused(strCommonCmd);
    uint8_t argv[OHOSFWK_MAX_ARGC][OHOSFWK_MAX_ARGV_LEN] = {0};
    uint32_t argc = 0;

    int ret = ConvertCmdToParam(para, paraLen, argv, &argc);
    if (ret != OHOSFWK_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "OHOSFWK ATCMD Convert Command Failed\n");
        return ERRCODE_FAIL;
    }
    static uint32_t MAX_PARAM_CNT = 5;
    if (argc < 1 || argc > MAX_PARAM_CNT) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "OHOSFWK ATCMD Set Parameter Failed\n");
        return ERRCODE_FAIL;
    }
    uint8_t *inputArgv[MAX_PARAM_CNT];
    for (uint32_t i = 0; i < argc; i++) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "arg :%u argv :%s\n", i, argv[i]);
        size_t argSize = strlen((const char *)argv[i]) + 1;
        inputArgv[i] = (uint8_t *)malloc(argSize);
        if (inputArgv[i] == NULL) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "malloc failed.\r\n");
            continue;
        }
        ret = strncpy_s((char *)inputArgv[i], argSize, (const char *)argv[i], argSize);
        if (ret != EOK) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "trncpy_s failed.\r\n");
            continue;
        }
    }
    ret = SensorSample((const char **)inputArgv, argc);
    if (ret != OHOSFWK_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "OHOSFWK ATCMD SensorSample Failed\n");
    }
    for (uint32_t i = 0; i < argc; i++) {
        if (inputArgv[i] == NULL) {
            continue;
        }
        free(inputArgv[i]);
        inputArgv[i] = NULL;
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "SensorTaskEntry end\n");
    return ret;
}
#endif

#ifdef CONFIG_NATIVEAPP_TEST
static uint32_t OhosfwkATSwitchSlicePage(uint8_t *para, uint32_t paraLen, char *strCommonCmd)
{
    unused(strCommonCmd);
    uint8_t argv[OHOSFWK_MAX_ARGC][OHOSFWK_MAX_ARGV_LEN] = {0};
    uint32_t argc = 0;

    int ret = ConvertCmdToParam(para, paraLen, argv, &argc);
    if (ret != OHOSFWK_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "OhosATTestPageCmd ATCMD Convert Command Failed");
        return ERRCODE_FAIL;
    }
    if (argc != ARGC_NUM) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "OhosATTestPageCmd ATCMD Set Parameter Failed");
        return ERRCODE_FAIL;
    }

    char *sliceStr = (char *)argv[0];
    char *pageStr = (char *)argv[1];
    uint32_t sliceIndex = atoi(sliceStr);
    uint32_t pageIndex = atoi(pageStr);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "OhosATTestPageCmd(), sliceIndex = %[d], pageIndex = %[d]", sliceIndex,
                  pageIndex);
    if (sliceIndex > VIEW_INVALIDE_APP || pageIndex > MAX_PAGE) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "OhosATTestPageCmd sliceIndex/pageIndex too larget!", sliceIndex,
                      pageIndex);
        return ERRCODE_FAIL;
    }
    OHOS::NativeAbility::GetInstance().SwitchSlice(sliceIndex, pageIndex);
    return ERRCODE_SUCC;
}

static uint32_t OhosATMonkeyControl(uint8_t *para, uint32_t paraLen, char *strCommonCmd)
{
    unused(strCommonCmd);
    uint8_t argv[OHOSFWK_MAX_ARGC][OHOSFWK_MAX_ARGV_LEN] = {0};
    uint32_t argc = 0;

    int ret = ConvertCmdToParam(para, paraLen, argv, &argc);
    if (ret != OHOSFWK_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "OhosATMonkeyControl ATCMD Convert Command Failed");
        return ERRCODE_FAIL;
    }
    if (argc != 1) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "OhosATMonkeyControl ATCMD Set Parameter Failed");
        return ERRCODE_FAIL;
    }

    char *monkeyStr = (char *)argv[0];
    uint32_t monkeyControl = atoi(monkeyStr);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "OhosATMonkeyControl(),monkeyControl = %[d]", monkeyControl);
    OHOS::MonkeyModel::GetInstance().SetStart(monkeyControl);
    return ERRCODE_SUCC;
}
#endif

static int32_t GetLocalTimezone(void)
{
    struct tm localTime;
    struct tm gmtTime;
    struct timeval tv = { 0, 0 };

    if (gettimeofday(&tv, NULL)) {
        return 0;
    }

    localtime_r(&tv.tv_sec, &localTime);
    gmtime_r(&tv.tv_sec, &gmtTime);

    return (localTime.tm_hour - gmtTime.tm_hour) * MINUTES_PER_HOUR + (localTime.tm_min - gmtTime.tm_min);
}

static uint32_t GetTimeZone(char *time)
{
    char *ptr = time;
    char *tmp;
    const char* utcStr = "UTC";
    size_t utcStrLen = strlen(utcStr);

    // 判断是否配置时区
    tmp = strchr(ptr, ' ');
    if (tmp == NULL) {
        return DEFAULT_TIME_ZONE; // 未配置时区默认为UTC+8:00
    }
    *tmp = '\0';
    ptr = tmp + 1;
    if (strncmp(ptr, utcStr, utcStrLen) != 0) {
        return DEFAULT_TIME_ZONE;
    }
    ptr += utcStrLen;

    // 时
    tmp = strchr(ptr, ':');
    if (tmp == NULL) { return 0; }
    *tmp = '\0';
    int hour = strtol(ptr, NULL, DECIMAL);
    ptr = tmp + 1;

    // 分
    int min = strtol(ptr, NULL, DECIMAL);
    int offsetMin = hour * MINUTES_PER_HOUR + min;
    if (offsetMin < MIN_TIMEZONE_OFFSET || offsetMin > MAX_TIMEZONE_OFFSET) {
        return DEFAULT_TIME_ZONE;
    }
    return offsetMin;
}

static uint32_t GetSecTime(uint8_t *time, struct timezone *tz)
{
    uint32_t sec;
    struct tm tm = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, nullptr};
    char *ptr = (char *)time;
    char *tmp;

    // 年
    tmp = strchr(ptr, '-');
    if (tmp == NULL) { return 0; }
    *tmp = '\0';
    // 年的计算需要减去1900
    tm.tm_year = strtol(ptr, NULL, DECIMAL) - 1900;
    if (tm.tm_year < TIME_YEAR_OFFSET) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "OhosfwkATCmdTimezoneSet year num must bigger than 1970!");
        return 0;
    }
    ptr = tmp + 1;

    // 月
    tmp = strchr(ptr, '-');
    if (tmp == NULL) { return 0; }
    *tmp = '\0';
    // 月的计算需要减1
    tm.tm_mon = strtol(ptr, NULL, DECIMAL) - 1;
    ptr = tmp + 1;

    // 日
    tmp = strchr(ptr, ' ');
    if (tmp == NULL) { return 0; }
    *tmp = '\0';
    tm.tm_mday = strtol(ptr, NULL, DECIMAL);
    ptr = tmp + 1;

    // 时
    tmp = strchr(ptr, ':');
    if (tmp == NULL) { return 0; }
    *tmp = '\0';
    tm.tm_hour = strtol(ptr, NULL, DECIMAL);
    ptr = tmp + 1;

    // 分
    tmp = strchr(ptr, ':');
    if (tmp == NULL) { return 0; }
    *tmp = '\0';
    tm.tm_min = strtol(ptr, NULL, DECIMAL);
    ptr = tmp + 1;

    // 秒
    tm.tm_sec = strtol(ptr, NULL, DECIMAL);
    sec = (uint32_t)mktime(&tm);

    // 时区
    tz->tz_minuteswest = GetTimeZone(ptr);

    return sec;
}

static uint32_t OhosfwkATCmdTimezoneSet(uint8_t *para, uint32_t paraLen, char *strCommonCmd)
{
    unused(strCommonCmd);
    unused(paraLen);

    struct timeval tv = { 0, 0 };
    struct timezone tz = { 0, 0 };
    tv.tv_sec = GetSecTime(para, &tz);
    int32_t localTimeZone = GetLocalTimezone();

    tv.tv_sec = tv.tv_sec + localTimeZone * SECONDS_PER_MINUTE - (tz.tz_minuteswest * SECONDS_PER_MINUTE);
    if (tv.tv_sec == 0) {
        return ERRCODE_FAIL;
    } else if (tv.tv_sec <= (tz.tz_minuteswest * SECONDS_PER_MINUTE)) {
        tv.tv_sec = 0;
    }

    settimeofday(&tv, &tz);
    return ERRCODE_SUCC;
}

static uint32_t OhosfwkATCmdTimezoneGet(uint8_t *para, uint32_t paraLen, char *strCommonCmd)
{
    unused(strCommonCmd);
    unused(para);
    unused(paraLen);
    char timebuf[TIME_MAX] = { 0 };
    struct tm tm;
    struct timeval tv = { 0, 0 };
    const char* formatStr = " %F %T";

    if (gettimeofday(&tv, NULL)) {
        return ERRCODE_FAIL;
    }

    // 计算本地时间
    localtime_r(&tv.tv_sec, &tm);
    uint32_t millisec = tv.tv_usec / USEC_TO_MILLI;
    strftime(timebuf, TIME_MAX - 1, formatStr + 1, &tm);

    // 计算时区
    int32_t localTimeZone = GetLocalTimezone();
    int32_t offsetMin = localTimeZone % MINUTES_PER_HOUR;
    offsetMin = offsetMin < 0 ? offsetMin + MINUTES_PER_HOUR : offsetMin;
    int32_t offsetHour = localTimeZone / MINUTES_PER_HOUR;
    offsetHour = localTimeZone < 0 && offsetMin != 0 ? offsetHour - 1 : offsetHour;

    if (localTimeZone >= 0) {
        PRINTK("%s:%03d UTC+%d:%02d", timebuf, millisec, offsetHour, offsetMin);
    } else {
        PRINTK("%s:%03d UTC%d:%02d", timebuf, millisec, offsetHour, offsetMin);
    }

    return ERRCODE_SUCC;
}

static const at_cmd_table_t g_ohosfwk_cmd[] = {
    { (char *)"AT^OHOSFWK_AM_EXIT_JSAPP",      OhosfwkATCmdAmExitJsApp, (char *)"00 00 00 00" },
    { (char *)"AT^OHOSFWK_BM_INSTALL",         OhosfwkATCmdBmInstall,   (char *)"00 00 00 01" },
    { (char *)"AT^OHOSFWK_BM_SET",             OhosfwkATCmdBmSet,       (char *)"00 00 00 02" },
    { (char *)"AT^OHOSFWK_BM_UNINSTALL",       OhosfwkATCmdBmUninstall, (char *)"00 00 00 03" },
    { (char *)"AT^OHOSFWK_AM_CHANGENATIVESLICE", OhosfwkATCmdAmChangeNativeSlice, (char *)"00 00 00 04" },
    { (char *)"AT^OHOSFWK_AM_START_JS_APP",    OhosfwkATCmdAmStartJsApp, (char *)"00 00 00 05" },
    { (char *)"AT^OHOSFWK_DFX_JS_MEMORY_INFO",    OhosfwkATCmdDfxJsMemoryInfo, (char *)"00 00 00 06" },
    { (char *)"AT^OHOSFWK_CHANGE_LANGUAGE",    OhosfwkATCmdChangeLanguage, (char *)"00 00 00 07" },
    { (char *)"AT^OHOSFWK_BM_GET_APPINFO",     OhosfwkATCmdBmGetAppInfo, (char *)"00 00 00 10" },
    { (char *)"AT^OHOSFWK_BM_GET_APPLIST",     OhosfwkATCmdBmGetAppList, (char *)"00 00 00 11" },
    { (char *)"AT^OHOSFWK_AM_DFX",             OhosfwkATCmdAmDfx,        (char *)"00 00 00 12" },
	{(char *)"AT^OHOSFWK_MSG_START_NOTIFY", OhosfwkATStartNotify, (char *)"00 00 00 13"},
#ifdef SUPPORT_AT_LOCATION
    { (char *)"AT^OHOSFWK_LOCATION",    OhosfwkATCmdLocation, (char *)"00 00 00 08" },
#endif
#ifdef SUPPORT_AT_SENSOR
    {(char *)"AT^OHOSFWK_SENSOR", OhosfwkATCmdSensor, (char *)"00 00 00 09"},
#endif
#ifdef CONFIG_NATIVEAPP_TEST
   {(char *)"AT^SWITCH_SLICE_PAGE", OhosfwkATSwitchSlicePage, (char *)"00 00 00 13"}, // 界面切换AT指令方便测试使用;
   {(char *)"AT^_MONKEY_CONTROL", OhosATMonkeyControl, (char *)"00 00 00 14"}, // Monkey 开关
#endif
    {(char *)"AT^OHOSFWK_GLOBAL_TIMEZONE_SET", OhosfwkATCmdTimezoneSet, (char *)"00 00 00 15"},
    {(char *)"AT^OHOSFWK_GLOBAL_TIMEZONE_GET", OhosfwkATCmdTimezoneGet, (char *)"00 00 00 16"},
};

at_cmd_table_t *get_ohosfwk_cmd_table(uint32_t *cmd_count)
{
    *cmd_count = ARRAY_COUNT(g_ohosfwk_cmd);
    return (at_cmd_table_t *)g_ohosfwk_cmd;
}

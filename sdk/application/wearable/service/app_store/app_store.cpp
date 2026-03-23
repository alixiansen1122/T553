#include <stdio.h>
#include "securec.h"
#include "app_store.h"
#include "msg_center.h"
#include "msg_center_protocol.h"
#include "wearable_log.h"
#include "adapter.h"
#include "bundle_install_msg.h"
#include "bundlems_slite_client.h"
#include "appexecfwk_errors.h"
#include "uiservice/ui_service.h"

void AppStoreReplyAck(uint8_t cmd_id, uint8_t type, uint32_t ack)
{
    int ret = msg_center_send_data(cmd_id, type, &ack, sizeof(ack));
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_MSG_CENTER, "send replay ret = 0x%x", ret);
}

static void SendMsgToAppStore(CallbackStatus status, const uint8_t resultCode, const void *resultMessage)
{
    BundleInstallMsg *installMsg = (BundleInstallMsg *)resultMessage;
    PkgResultReport report = { 0 };

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_MSG_CENTER, "SendMsgToAppStore resultCode = %u", resultCode);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_MSG_CENTER, "SendMsgToAppStore bundleName = %s", installMsg->bundleName);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_MSG_CENTER, "SendMsgToAppStore installState = %u", installMsg->installState);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_MSG_CENTER, "SendMsgToAppStore installProcess = %u", installMsg->installProcess);
    (void)strncpy_s((char *)report.pkgName, PKG_NAME_MAX_LENGTH, installMsg->bundleName, strlen(installMsg->bundleName));
    if (resultCode == INSTALLING_UNINSTALLING) {
        report.operationResult = installMsg->installProcess;
    } else if (resultCode == ERR_OK) {
        BundleData bundleData;
        memset_s(&bundleData, sizeof(BundleData), 0, sizeof(BundleData));
        strncpy_s(bundleData.bundleName, BUNDLE_NAME_LEN, installMsg->bundleName, BUNDLE_NAME_LEN);
        if (installMsg->installState == BUNDLE_INSTALL_OK) {
            SendMsgToUIService(JS_INSTALL_SUCESS_TO_UI, 0, &bundleData, sizeof(BundleData));
            report.operationResult = INSTALL_OK;
        } else if (installMsg->installState == BUNDLE_UNINSTALL_OK) {
            SendMsgToUIService(JS_UNINSTALL_SUCESS_TO_UI, 0, &bundleData, sizeof(BundleData));
            report.operationResult = UNINSTALL_OK;
        }
    } else if (resultCode ==  ERR_APPEXECFWK_INSTALL_FAILED_EXCEED_MAX_BUNDLE_NUMBER) {
        report.operationResult = INSTALL_OVER_LIMIT;
    } else {
        report.operationResult = (status == INSTALL_CALLBACK) ? INSTALL_FAILED : UNINSTALL_FAILED;
    }
    AdapterFree(installMsg);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_MSG_CENTER, "SendMsgToAppStore operationResult = %u", report.operationResult);
    int ret = msg_center_send_data(MSGCENTER_CMD_APP_STORE, APP_STORE_NOTIFY_PROGRESS, &report, sizeof(PkgResultReport));
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_MSG_CENTER, "notity process send ret = 0x%x", ret);
}

static void InstallCallbackFunc(const uint8_t resultCode, const void *resultMessage)
{
    SendMsgToAppStore(INSTALL_CALLBACK, resultCode, resultMessage);
}

static void UninstallCallbackFunc(const uint8_t resultCode, const void *resultMessage)
{
    SendMsgToAppStore(UNINSTALL_CALLBACK, resultCode, resultMessage);
}

errcode_t AppStorePkgOperation(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    bool ret = false;
    uint8_t tlLen = msg_center_get_tlv_tl_len(usr_data);
    uint16_t payloadLen = usr_len - tlLen;
    if (payloadLen == sizeof(PkgOperationInfo)) {
        PkgOperationInfo *operInfo = (PkgOperationInfo *)msg_center_get_tlv_payload(usr_data);
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_MSG_CENTER, "AppStorePkgOperation operType=%d", operInfo->operationType);
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_MSG_CENTER, "AppStorePkgOperation pkgName=%s", operInfo->pkgName);
        if (operInfo->operationType == PKG_OPERATION_TYPE_INSTALL) {
            uint8_t len = strlen(FS_DIR_APP_STORE) + strlen((char *)operInfo->pkgName) + 1;
            char *pkgFullName = (char *)malloc(len);
            if (pkgFullName != nullptr) {
                (void)sprintf_s(pkgFullName, len, "%s%s", FS_DIR_APP_STORE, (char *)operInfo->pkgName);
                WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "AppStorePkgOperation install pkgFullName=%s", pkgFullName);
                InstallParam installParam = { .installLocation = 1, .keepData = false };
                ret = OHOS::BundleMsClient::GetInstance().Install(pkgFullName, &installParam, InstallCallbackFunc);
                free(pkgFullName);
            }
        } else if (operInfo->operationType == PKG_OPERATION_TYPE_UNINSTALL) {
                StopJsApp((char*)operInfo->pkgName);
                WEARABLE_LOGI(WEARABLE_LOG_MODULE_MSG_CENTER, "AppStorePkgOperation uninstall pkgName=%s", operInfo->pkgName);
                InstallParam installParam = { .installLocation = 1, .keepData = false };
                ret = OHOS::BundleMsClient::GetInstance().Uninstall((char*)operInfo->pkgName, &installParam, UninstallCallbackFunc);
        }
    }
    if (ret) {
        AppStoreReplyAck(cmd_id, 0xff, ERR_MSGCENTER_SUCC);
    } else {
        AppStoreReplyAck(cmd_id, 0xff, ERR_MSGCENTER_PARA_ERR);
    }
    return ERRCODE_FAIL;
}

errcode_t AppStoreGetPkgList(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    BundleInfo *bundleInfos = nullptr;
    int32_t count = 0;
    int32_t ret = ERRCODE_FAIL;

    uint8_t bundleRet = OHOS::BundleMsClient::GetInstance().GetBundleInfos(0, &bundleInfos, &count);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_MSG_CENTER, "GetBundleInfos count = %d bundleRet = 0x%x", count, bundleRet);

    int32_t len = sizeof(uint8_t) + count * sizeof(PkgUnitInfo);
    char* infoList = (char *)malloc(len);
    if (infoList != nullptr) {
        memset_s(infoList, len, 0, len);
        *(uint8_t *)(infoList) = count;
        PkgUnitInfo *unitInfo = (PkgUnitInfo *)(infoList + sizeof(uint8_t));
        for (int32_t i = 0; i < count; i++) {
            (void)strncpy_s((char*)unitInfo[i].pkgName, PKG_NAME_MAX_LENGTH,
                            bundleInfos[i].bundleName, strlen(bundleInfos[i].bundleName));
            (void)strncpy_s((char*)unitInfo[i].versionName, VERSION_NAME_MAX_LENGTH,
                            bundleInfos[i].versionName, strlen(bundleInfos[i].versionName));
            (void)strncpy_s((char*)unitInfo[i].labelName, LABEL_NAME_MAX_LENGTH,
                            bundleInfos[i].label, strlen(bundleInfos[i].label));
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AppStoreGetPkgList pkgName[%d] = %s, versionName[%d] = %s,\
                            labeName[%d] = %s", i, unitInfo[i].pkgName, i, unitInfo[i].versionName, i,
                            unitInfo[i].labelName);
            unitInfo[i].versionCode = bundleInfos[i].versionCode;
        }
        ret = msg_center_send_data(MSGCENTER_CMD_APP_STORE, type, infoList, len);
        free(infoList);
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_MSG_CENTER, "AppStoreGetPkgList ret = 0x%x", ret);
    }
    if (bundleInfos != nullptr) {
        for (uint8_t i = 0; i < count; i++) {
            ClearBundleInfo(bundleInfos + i);
        }
        AdapterFree(bundleInfos);
    }
    return ret;
}

errcode_t AppStoreGetBundleInfo(uint8_t cmd_id, uint8_t type, void *usr_data, uint16_t usr_len)
{
    BundleInfo bundleInfo;
    int32_t ret = ERRCODE_FAIL;

    uint8_t tl_len = msg_center_get_tlv_tl_len(usr_data);
    uint16_t payload_len = usr_len - tl_len;
    uint8_t *tlv_payload = (uint8_t *)msg_center_get_tlv_payload(usr_data);
    char *bundleName = (char *)malloc(payload_len + 1);
    (void)memcpy_s(bundleName, payload_len, tlv_payload, payload_len);
    bundleName[payload_len] = '\0';
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "bundleName = %s", bundleName);

    uint8_t bundleRet = OHOS::BundleMsClient::GetInstance().GetBundleInfo(bundleName, 0, &bundleInfo);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "get bundleInfo, ret = %d", bundleRet);

    int32_t len = sizeof(PkgUnitInfo);
    PkgUnitInfo *info = (PkgUnitInfo *)malloc(len);
    if (info != nullptr) {
        memset_s(info, len, 0, len);
        (void)strncpy_s((char*)info->pkgName, PKG_NAME_MAX_LENGTH, bundleInfo.bundleName,
                        strlen(bundleInfo.bundleName));
        (void)strncpy_s((char*)info->versionName, VERSION_NAME_MAX_LENGTH, bundleInfo.versionName,
                        strlen(bundleInfo.versionName));
        (void)strncpy_s((char*)info->labelName, LABEL_NAME_MAX_LENGTH, bundleInfo.label, strlen(bundleInfo.label));
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AppStoreGetBundleInfo pkgName = %s, versionName = %s, labeName = %s",
                      info->pkgName, info->versionName, info->labelName);
        info->versionCode = bundleInfo.versionCode;
        ret = msg_center_send_data(cmd_id, type, info, len);
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_MSG_CENTER, "AppStoreGetBundleInfo send data ret = 0x%x", ret);
        free(info);
        free(bundleName);
    }

    return ret;
}
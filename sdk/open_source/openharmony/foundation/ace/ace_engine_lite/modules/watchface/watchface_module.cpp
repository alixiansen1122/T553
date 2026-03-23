/*
 * Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
 * Description: watchface JS intarface
 * Author: Software Group
 * Create: 2025-09-5
 */
 
#include <string>
#include "jsi.h"
#include "securec.h"
#include "ace_log.h"
#include "js_async_work.h"
#include "watchface_module.h"
 
namespace OHOS {
namespace ACELite {

bool WatchfaceModule::isListenerRunning_ = false;
bool WatchfaceModule::isTerminate_ = false;

constexpr static uint32_t STEP_SIZE = 512;
constexpr static uint32_t DELAY_TIME = 500;

// 下载状态打桩变量
static uint32_t totalSize = 16 * 1024;
static uint32_t receivedSize = 0;

bool WatchfaceModule::GetDialInfo(uint32_t uuid, WatchDialInfo& watch, JSIValue& dialInfo)
{
    bool isGet = ::GetWatchFaceInfo(uuid, &watch);
    if (isGet) {
        JSI::SetNumberProperty(dialInfo, "uuid", watch.uuid);
        JSI::SetNumberProperty(dialInfo, "install_state", watch.install_state);
        JSI::SetNumberProperty(dialInfo, "is_current_watch", watch.is_current_watch);
        JSI::SetStringProperty(dialInfo, "watch_protocol_version", reinterpret_cast<char*>(watch.watch_protocol_version));
        JSI::SetStringProperty(dialInfo, "watch_version", reinterpret_cast<char*>(watch.watch_version));
        return true;
    } else {
        return false;
    }
}
 
JSIValue WatchfaceModule::SetWatchFace(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    if (argsSize != ARGC_TWO) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Invalid parameter!");
        return JSI::CreateUndefined();
    }

    WATCHFACE_ERRCODE ret = WATCHFACE_SUCCESS;
    JSIValue dialInfo = JSI::CreateObject();
    JSIValue callback = JSI::AcquireValue(args[1]);
    bool isGet = false;

    uint32_t uuid = JSI::ValueToNumber(args[0]);
    bool installed = SetCurrentWatchFace(uuid);
    if (!installed) {
        HILOG_ERROR(HILOG_MODULE_ACE, "SetCurrentWatchFace failed!");
        ret = WATCHFACE_INSTALL_ERROR;
        goto exit;
    }

    WatchDialInfo watch;
    memset_s(&watch, sizeof(WatchDialInfo), 0, sizeof(WatchDialInfo));
    isGet = GetDialInfo(uuid, watch, dialInfo);
    if (!isGet) {
        HILOG_ERROR(HILOG_MODULE_ACE, "GetDialInfo failed!");
        ret = WATCHFACE_GETINFO_ERROR;
    }

exit:
    JSIValue err = JSI::CreateNumber(ret);
    JSIValue argv[ARGC_TWO] = {err, dialInfo};
    JSI::CallFunction(callback, thisVal, argv, ARGC_TWO);
    JSI::ReleaseValueList(err, callback, dialInfo);
    return JSI::CreateUndefined();
}
 
JSIValue WatchfaceModule::GetWatchFaceInfo(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    if (argsSize != ARGC_TWO) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Invalid parameter!");
        return JSI::CreateUndefined();
    }

    WatchDialInfo watch;
    memset_s(&watch, sizeof(WatchDialInfo), 0, sizeof(WatchDialInfo));
    WATCHFACE_ERRCODE ret = WATCHFACE_SUCCESS;

    uint32_t uuid = JSI::ValueToNumber(args[0]);
    JSIValue dialInfo = JSI::CreateObject();
    bool isGet = GetDialInfo(uuid, watch, dialInfo);
    if (!isGet) {
        HILOG_ERROR(HILOG_MODULE_ACE, "GetDialInfo failed!");
        ret = WATCHFACE_GETINFO_ERROR;
    }

    JSIValue err = JSI::CreateNumber(ret);
    JSIValue callback = JSI::AcquireValue(args[1]);
    JSIValue argv[ARGC_TWO] = {err, dialInfo};
    JSI::CallFunction(callback, thisVal, argv, ARGC_TWO);
    JSI::ReleaseValueList(err, callback, dialInfo);

    return JSI::CreateUndefined();
}

// 下载状态打桩函数
static int GetDownloadStatus(DownloadStatus& status)
{
    status.receivedSize = receivedSize;
    status.totalSize = totalSize;
    receivedSize += STEP_SIZE;
    if (receivedSize > totalSize) {
        receivedSize = totalSize;
    }
    status.uuid = 1;
    
    osal_msleep(DELAY_TIME);
    return 0;
}

void WatchfaceModule::FailCallback(void *data)
{
    if (data == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Invalid null argument in FailCallback");
        isListenerRunning_ = false;
        return;
    }

    WatchfaceParams *params = static_cast<WatchfaceParams *>(data);

    JSIValue installInfo = JSI::CreateObject();
    JSIValue err = JSI::CreateNumber(WATCHFACE_GETMSG_ERROR);
    JSIValue callback = params->callback;
    JSIValue thisVal = params->thisVal;

    JSIValue argv[ARGC_TWO] = {err, installInfo};

    if (!isTerminate_) {
        JSI::CallFunction(callback, thisVal, argv, ARGC_TWO);
    }
    JSI::ReleaseValueList(callback, thisVal);
    JSI::ReleaseValueList(err, installInfo);
    delete params;
    isListenerRunning_ = false;
    return;
}

void WatchfaceModule::SuccessCallback(void *data)
{
    if (data == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Invalid null argument in SuccessCallback");
        return;
    }

    WatchfaceParams *params = static_cast<WatchfaceParams *>(data);
    JSIValue callback = params->callback;
    JSIValue thisVal = params->thisVal;

    JSIValue installInfo = JSI::CreateObject();
    JSIValue err = JSI::CreateNumber(WATCHFACE_SUCCESS);
    JSI::SetNumberProperty(installInfo, "process", params->process);
    JSI::SetNumberProperty(installInfo, "install_state", params->installState);

    JSIValue argv[ARGC_TWO] = {err, installInfo};

    if (!isTerminate_) {
        JSI::CallFunction(callback, thisVal, argv, ARGC_TWO);
    }

    JSI::ReleaseValueList(err, installInfo);
    if (params->installState || isTerminate_) {
        JSI::ReleaseValueList(callback, thisVal);
        delete params;
        isListenerRunning_ = false;
    }
    return;
}

static void WatchFaceDownload(void *argument)
{
    if (argument == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Invalid null argument in DownloadListener");
        return;
    }
    WatchfaceParams *params = static_cast<WatchfaceParams *>(argument);

    do {
        DownloadStatus status;
        int ret = GetDownloadStatus(status);
        if (ret != 0) {
            HILOG_ERROR(HILOG_MODULE_ACE, "GetDownloadStatus failed");
            JsAsyncWork::DispatchAsyncWork(WatchfaceModule::FailCallback, static_cast<void *>(params));
            break;
        }

        WatchDialInfo watch;
        if (status.receivedSize == status.totalSize) {
            bool isGet = ::GetWatchFaceInfo(status.uuid, &watch);
            if (!isGet) {
                HILOG_ERROR(HILOG_MODULE_ACE, "GetWatchFaceInfo failed");
                JsAsyncWork::DispatchAsyncWork(WatchfaceModule::FailCallback, static_cast<void *>(params));
                break;
            }
            params->installState = watch.install_state;
        }

        params->process = static_cast<double>(status.receivedSize) / status.totalSize;

        JsAsyncWork::DispatchAsyncWork(WatchfaceModule::SuccessCallback, static_cast<void *>(params));
    } while(!params->installState && !WatchfaceModule::isTerminate_);
}

JSIValue WatchfaceModule::SetDownloadListener(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    if (argsSize != ARGC_ONE) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Invalid parameter!");
        return JSI::CreateUndefined();
    }

    if (isListenerRunning_) {
        HILOG_ERROR(HILOG_MODULE_ACE, "another DownloadListener is running!");
        return JSI::CreateUndefined();
    }

    WatchfaceParams *params = new WatchfaceParams{};
    if (params == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Failed to allocate memory for params!");
        return JSI::CreateUndefined();
    }

    params->thisVal = JSI::AcquireValue(thisVal);
    params->callback = JSI::AcquireValue(args[0]);

    osThreadId_t id;
    osThreadAttr_t attr;
    attr.name = "WatchFaceDownload";
    attr.attr_bits = osThreadDetached;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 0x1000;
    attr.priority = osPriorityNormal;

    isTerminate_ = false;
    isListenerRunning_ = true;
    receivedSize = 0; // 打桩数据初始化
    id = osThreadNew((osThreadFunc_t)WatchFaceDownload, (void *)params, &attr);

    return JSI::CreateUndefined();
}

JSIValue WatchfaceModule::Destroy(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    isTerminate_ = true;
    return JSI::CreateUndefined();
}

void InitWatchFaceModule(JSIValue exports)
{
    JSI::SetModuleAPI(exports, "setWatchFace", WatchfaceModule::SetWatchFace);
    JSI::SetModuleAPI(exports, "getWatchFaceInfo", WatchfaceModule::GetWatchFaceInfo);
    JSI::SetModuleAPI(exports, "setDownloadListener", WatchfaceModule::SetDownloadListener);
    JSI::SetModuleAPI(exports, "destroy", WatchfaceModule::Destroy);
}
}  // namespace ACELite
}  // namespace OHOS
/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <unistd.h>
#include <securec.h>
#include "ace_log.h"
#include "js_async_work.h"
#include "product_adapter.h"
#include "connection_manage.h"
#include "network_listener.h"
#include "network_module.h"

constexpr uint8_t ARGS_NUM_2 = 2;

namespace OHOS {
namespace ACELite {
std::map<uint8_t, NetCallBack *> NetWorkModule::callbackSet;

void NetWorkModule::TriggerCallbackListener(const uint8_t networkType, const uint8_t netConnectState)
{
    auto it = callbackSet.find(networkType);
    if (it != callbackSet.end()) {
        NetCallBack *callback = it->second;
        callback->netParam.state = netConnectState;
        callback->netParam.net_type = networkType;
        if (!JsAsyncWork::DispatchAsyncWork(AsyncExecuteCallback, const_cast<NetCallBack *>(callback))) {
            HILOG_ERROR(HILOG_MODULE_ACE, "dispatch async work failed.");
        }
    }
}

static bool ValidJsValue(JSIValue value)
{
    return (value != nullptr) && (!JSI::ValueIsUndefined(value));
}

static JSIValue GetNetworkStateString(uint8_t type, uint8_t state)
{
    // 网络类型
    const char *networkType;
    switch (type) {
        case NETWORK_TYPE_WIFI:
            networkType = "WIFI";
            break;
        case NETWORK_TYPE_BLUETOOTH:
            networkType = "Bluetooth";
            break;
        case NETWORK_TYPE_BLUETOOTH_LE:
            networkType = "Bluetooth LE";
            break;
        case NETWORK_TYPE_BLUETOOTH_PAN:
            networkType = "Bluetooth PAN";
            break;
        case NETWORK_TYPE_CELL_LTE_CAT1:
            networkType = "Cell CAT1";
            break;
        default:
            networkType = "Unknown Network Type";
            break;
    }

    // 连接状态
    const char *connectionState;
    switch (state) {
        case CONNECTION_STATUS_CONNECTING:
            connectionState = "Connecting";
            break;
        case CONNECTION_STATUS_CONNECTED:
            connectionState = "Connected";
            break;
        case CONNECTION_STATUS_DISCONNECTED:
            connectionState = "Disconnected";
            break;
        default:
            connectionState = "Unknown State";
            break;
    }

    // 静态字符串缓冲区，用于返回结果
    static char description[MAX_STATUS_LENGTH];
    int ret =
        snprintf_s(description, sizeof(description), sizeof(description) - 1, "%s: %s", networkType, connectionState);
    if (ret <= 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "result buff is too long");
    }
    JSIValue result = JSI::CreateObject();
    JSI::SetNumberProperty(result, "type", type);
    JSI::SetNumberProperty(result, "state", state);
    JSI::SetStringProperty(result, "description", description);
    return result;
}

void NetWorkModule::AsyncExecuteCallback(void *arg)
{
    NetCallBack *callback = static_cast<NetCallBack *>(arg);
    if (ValidJsValue(callback->net_connect_callback)) {
        JSIValue result = GetNetworkStateString(callback->netParam.net_type, callback->netParam.state);
        JSIValue argv[ARGC_ONE] = {result};
        JSI::CallFunction(callback->net_connect_callback, callback->net_connect_thisVal, argv, ARGC_ONE);
        JSI::ReleaseValue(result);
    }
}

JSIValue NetWorkModule::RegisterConnectListener(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    uint8_t type;
    JSIValue callbackArg;
    if (argsNum == 1) {
        type = NETWORK_TYPE_BLUETOOTH;
        callbackArg = args[0];
    } else if (argsNum == ARGS_NUM_2) {
        if (args == nullptr || JSI::ValueIsUndefined(args[0])) {
            HILOG_ERROR(HILOG_MODULE_ACE, "Invalid type argument for registering net connection callback");
            return JSI::CreateUndefined();
        }
        type = JSI::ValueToNumber(args[0]);
        callbackArg = args[1];
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE, "Invalid number of arguments for registering net connection callback");
        return JSI::CreateUndefined();
    }
    auto it = callbackSet.find(type);
    if (it != callbackSet.end()) {
        NetCallBack *callback = it->second;
        // 从 map 中移除该回调
        callbackSet.erase(it);
        // 如果找到了对应的回调，先清理旧的回调
        Clean(callback);
    }
    NetCallBack *newCallback = new (std::nothrow) NetCallBack();
    if (newCallback == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Failed to allocate memory for NetCallBack");
        return JSI::CreateUndefined();
    }
    // 保存新的回调
    newCallback->net_connect_thisVal = JSI::AcquireValue(thisVal);
    newCallback->net_connect_callback = JSI::AcquireValue(callbackArg);
    if (newCallback->net_connect_callback == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Failed to register net connection callback");
        delete newCallback;
        return JSI::CreateUndefined();
    }
    newCallback->netParam.net_type = type;
    callbackSet[type] = newCallback;
    ::RegisterConnectListener(type, TriggerCallbackListener);
    return JSI::CreateUndefined();
}

JSIValue NetWorkModule::UnregisterConnectListener(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    if (args == nullptr || argsNum == 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Invalid arguments for unregistering net connection callback");
        return JSI::CreateUndefined();
    }

    uint8_t type;
    if (argsNum < 1) {
        type = NETWORK_TYPE_BLUETOOTH;
    } else {
        if (JSI::ValueIsUndefined(args[0])) {
            HILOG_ERROR(HILOG_MODULE_ACE, "Invalid type argument for unregistering net connection callback");
            return JSI::CreateUndefined();
        }
        type = JSI::ValueToNumber(args[0]);
    }
    auto it = callbackSet.find(type);
    if (it != callbackSet.end()) {
        NetCallBack *callback = it->second;
        callbackSet.erase(it);
        Clean(callback);
        ::UnregisterConnectListener(type, TriggerCallbackListener);
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE, "No callback registered for the given type");
    }

    return JSI::CreateUndefined();
}

JSIValue NetWorkModule::GetConnectState(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    uint8_t net_type = NETWORK_TYPE_UNKNOWN;
    if (argsNum < 1) {
        net_type = NETWORK_TYPE_BLUETOOTH;
    } else {
        net_type = (uint8_t)JSI::ValueToNumber(args[0]);
    }
    uint8_t state = GetNetConnectState(net_type);
    return GetNetworkStateString(net_type, state);
}

void NetWorkModule::Clean(NetCallBack *callbackToClean)
{
    if (callbackToClean == nullptr) {
        return;
    }
    // 释放回调函数资源
    if (callbackToClean->net_connect_callback && JSI::ValueIsFunction(callbackToClean->net_connect_callback)) {
        JSI::ReleaseValue(callbackToClean->net_connect_callback);
    }
    // 释放回调上下文资源
    if (callbackToClean->net_connect_thisVal && !JSI::ValueIsUndefined(callbackToClean->net_connect_thisVal)) {
        JSI::ReleaseValue(callbackToClean->net_connect_thisVal);
    }
    // 删除回调对象
    delete callbackToClean;
}

void NetWorkModule::OnTerminate()
{
    for (auto it = callbackSet.begin(); it != callbackSet.end();) {
        NetCallBack *callback = it->second;
        callbackSet.erase(it);
        // 移除多余的迭代器重置代码
        Clean(callback);
        // 从 map 中移除该回调
        it = callbackSet.begin();
    }
}

void InitNetWorkModule(JSIValue exports)
{
    JSI::SetModuleAPI(exports, "registerConnectListener", NetWorkModule::RegisterConnectListener);
    JSI::SetModuleAPI(exports, "unregisterConnectListener", NetWorkModule::UnregisterConnectListener);
    JSI::SetModuleAPI(exports, "getConnectState", NetWorkModule::GetConnectState);
    JSI::SetOnTerminate(exports, NetWorkModule::OnTerminate);
}

}  // namespace ACELite
}  // namespace OHOS

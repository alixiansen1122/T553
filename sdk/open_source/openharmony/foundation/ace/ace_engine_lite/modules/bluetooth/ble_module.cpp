/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "ble_module.h"
#include "ace_log.h"
#include "bts_br_gap.h"
#include "BluetoothWrapper.h"
#include "securec.h"
#include <algorithm>
#include <cctype>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/time.h>

namespace OHOS {
namespace ACELite {
std::map<std::string, EventCallback *> BleModule::eventCallbacks;
uint8_t BleModule::clientId = 0;
std::vector<BleScanFilter> BleModule::bleScanFilters;
std::vector<ScanResult> BleModule::scanResult;
ScanOptions BleModule::scanOptions;
uint8_t BleModule::advId = NOMAL_ADV_ID;
std::map<uint8_t, uint8_t> BleModule::advertisingMap;
JSIValue BleModule::gattClient = JSI::CreateUndefined();
std::mutex BleModule::connectMapMutex;
std::map<std::string, DeviceInfo> BleModule::deviceMap;
std::mutex BleModule::addrMapMutex;
std::map<std::string, uint8_t> BleModule::addrMap;
std::map<uint16_t, DiscoveryContext> BleModule::discoveryContexts;
std::mutex BleModule::contextMutex;
std::vector<GattOperationContext> BleModule::operationContexts;
std::mutex BleModule::operationMutex;
std::mutex BleModule::scanResultMutex;
bool BleModule::hasOn = false;
std::map<uint16_t, std::vector<GattService>> BleModule::discoveredServicesMap;

std::queue<gap_scan_result_data_t> BleModule::scanDataQueue;
std::mutex BleModule::queueMutex;
std::condition_variable BleModule::queueCond;
bool BleModule::workerThreadRunning = false;
osThreadId_t BleModule::workerThreadId = NULL;

std::string BleModule::GetFirstServerDeviceId(void)
{
    std::lock_guard<std::mutex> lock(connectMapMutex);
    // 查找客户端连接的对端
    auto it = std::find_if(deviceMap.begin(), deviceMap.end(), [](std::pair<const std::string, DeviceInfo> &pair){
        return pair.second.deviceType == BLE_SERVER;
    });
    if (it == deviceMap.end()) {
        return "";
    }
    return it->first;
}

JSIValue BleModule::JSGetMtu(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    std::lock_guard<std::mutex> lock(connectMapMutex);
    // 查找客户端连接的对端
    auto it = std::find_if(deviceMap.begin(), deviceMap.end(), [](std::pair<const std::string, DeviceInfo> &pair){
            return pair.second.deviceType == BLE_SERVER;
    });
    if (it == deviceMap.end()) {
        return JSI::CreateNumber(NOMAL_MTU_VALUE);
    }
    return JSI::CreateNumber(it->second.mtu);
}

static errcode_t RegisterGattc()
{
    bt_uuid_t uuid;
    memset_s(&uuid, sizeof(bt_uuid_t), 0, sizeof(bt_uuid_t));
    uuid.uuid_len = 2;
    uuid.uuid[0] = 0x01;
    uuid.uuid[1] = 0x02;
    errcode_t ret = wrapper_gattc_register_client(&uuid, &BleModule::clientId);
    if (ret != BLE_SUCCESS) {
        HILOG_ERROR(
            HILOG_MODULE_ACE, "gattc register client failed! ret: 0x%x, clientId:%d\n", ret, BleModule::clientId);
    }
    return ret;
}

static errcode_t RegisterClientCallbacks()
{
    gattc_callbacks_t gattc_cb = {
        GattcDiscoverySvcCbk,
        GattcDiscoverySvcResponse,
        GattcDiscoveryCharCbk,
        GattcDiscoveryCharResponse,
        GattcDiscoveryDescCbk,
        GattcDiscoveryDescResponse,
        GattcReadCfmCbk,
        GattcReadByUuidRespone,
        GattcWriteCfmCbk,
        GattcMtuChangedCbk,
        GattcNotificationdCbk,
        GattcIndicationCbk,
    };
    errcode_t ret = wrapper_gattc_register_callbacks(&gattc_cb);
    if (ret != BLE_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE,
            "wrapper_gattc_register_callbacks failed! ret: 0x%x clientId:%u\n",
            ret,
            BleModule::clientId);
        return BLE_OPRATION_FAILED;
    }
    gatts_callbacks_t gatts_cb = {nullptr,
        nullptr,
        nullptr,
        GattsStartServiceCbk,
        GattsStopServiceCbk,
        nullptr,
        GattsReadRequestCbk,
        GattsWriteRequestCbk,
        GattsMtuChangedCbk};
    ret = wrapper_gatts_register_ohos_callbacks(&gatts_cb);
    if (ret != BLE_SUCCESS) {
        HILOG_ERROR(
            HILOG_MODULE_ACE, "gatts_register_callbacks failed! ret: 0x%x clientId:%u\n", ret, BleModule::clientId);
        return BLE_OPRATION_FAILED;
    }
    return BLE_SUCCESS;
}

JSIValue BleModule::CreateGattClientDevice(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    bool findId = false;
    if (argsSize > ARGC_ONE) {
        HILOG_ERROR(HILOG_MODULE_ACE, "gattc_register params_cnt error! need: mode app_uuid!");
        return JSI::CreateUndefined();
    }
    if (!JSI::ValueIsUndefined(gattClient)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "gattClient has created!");
        return gattClient;
    }
    if (argsSize == ARGC_ONE) {
        std::string deviceId = ParseDeviceId(args[0]);
        if (deviceId.empty()) {
            HILOG_ERROR(HILOG_MODULE_ACE, "Invalid parameter!");
            return JSI::CreateNumber(BLE_INVALID_PARAMETER);  // 参数错误
        }
        {
            std::lock_guard<std::mutex> lock(BleModule::connectMapMutex);
            auto it = deviceMap.find(deviceId);
            if (it == deviceMap.end()) {
                DeviceInfo info = {NOMAL_MTU_VALUE, 0, STATE_DISCONNECTED, BLE_SERVER};
                deviceMap[deviceId] = info;
            }
        }
    }
    gattClient = JSI::CreateObject();
    JSIValue connect = JSI::CreateFunction(BleModule::Connect);
    JSIValue disconnect = JSI::CreateFunction(BleModule::DisConnect);
    JSIValue on = JSI::CreateFunction(BleModule::ClientOn);
    JSIValue off = JSI::CreateFunction(BleModule::ClientOff);
    JSIValue close = JSI::CreateFunction(BleModule::ClientClose);
    JSIValue getServices = JSI::CreateFunction(BleModule::GetServices);
    JSIValue readCharacteristicValue = JSI::CreateFunction(BleModule::ReadCharacteristicValue);
    JSIValue writeCharacteristicValue = JSI::CreateFunction(BleModule::WriteCharacteristicValue);
    JSIValue readDescriptorValue = JSI::CreateFunction(BleModule::ReadDescriptorValue);
    JSIValue writeDescriptorValue = JSI::CreateFunction(BleModule::WriteDescriptorValue);
    JSIValue getDeviceName = JSI::CreateFunction(BleModule::GetDeviceName);
    JSIValue getRssiValue = JSI::CreateFunction(BleModule::GetRssiValue);
    JSIValue getDeviceMtu = JSI::CreateFunction(BleModule::GetDeviceMtu);
    JSIValue setCharacteristicChangeIndication = JSI::CreateFunction(BleModule::SetCharacteristicChangeIndication);
    JSIValue setCharacteristicChangeNotification = JSI::CreateFunction(BleModule::SetCharacteristicChangeNotification);
    JSI::SetNamedProperty(gattClient, "connect", connect);
    JSI::SetNamedProperty(gattClient, "disconnect", disconnect);
    JSI::SetNamedProperty(gattClient, "on", on);
    JSI::SetNamedProperty(gattClient, "off", off);
    JSI::SetNamedProperty(gattClient, "close", close);
    JSI::SetNamedProperty(gattClient, "getServices", getServices);
    JSI::SetNamedProperty(gattClient, "writeCharacteristicValue", writeCharacteristicValue);

    JSI::SetNamedProperty(gattClient, "readCharacteristicValue", readCharacteristicValue);
    JSI::SetNamedProperty(gattClient, "readDescriptorValue", readDescriptorValue);
    JSI::SetNamedProperty(gattClient, "writeDescriptorValue", writeDescriptorValue);
    JSI::SetNamedProperty(gattClient, "getDeviceName", getDeviceName);
    JSI::SetNamedProperty(gattClient, "getRssiValue", getRssiValue);
    JSI::SetNamedProperty(gattClient, "getDeviceMtu", getDeviceMtu);
    JSI::SetNamedProperty(gattClient, "setCharacteristicChangeIndication", setCharacteristicChangeIndication);

    JSI::SetNamedProperty(gattClient, "setCharacteristicChangeNotification", setCharacteristicChangeNotification);
    JSPropertyDescriptor descriptor;
    descriptor.getter = JSGetMtu;
    descriptor.setter = nullptr;
    JSI::DefineNamedProperty(gattClient, "Mtu", descriptor);

    if (RegisterGattc() != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Register Gattc failed!");
        JSI::ReleaseValue(gattClient);
        return JSI::CreateUndefined();
    }
    errcode_t ret = RegisterClientCallbacks();
    if (ret != BLE_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Register Gattc callback failed!");
        JSI::ReleaseValue(gattClient);
        return JSI::CreateNumber(ret);
    }
    HILOG_DEBUG(HILOG_MODULE_ACE, "Register Gattc SUCCESS!");
    return gattClient;
}

JSIValue BleModule::Connect(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    if (argsSize > ARGC_ONE) {
        HILOG_ERROR(HILOG_MODULE_ACE, "params_cnt error!");
        return JSI::CreateNumber(BLE_INVALID_PARAMETER);  // 参数错误
    }
    errcode_t ret = 0;
    int32_t cnt = 0;
    ProfileConnectionState connState = STATE_DISCONNECTED;
    std::string deviceId;
    if (argsSize == ARGC_ONE) { // 输入deviceID
        deviceId = ParseDeviceId(args[0]);
        if (deviceId.empty()) {
            HILOG_ERROR(HILOG_MODULE_ACE, "Invalid parameter!");
            return JSI::CreateNumber(BLE_INVALID_PARAMETER);  // 参数错误
        }
        std::lock_guard<std::mutex> lock(BleModule::connectMapMutex);
        auto it = deviceMap.find(deviceId);
        if (it == deviceMap.end() && deviceMap.size() < MAX_DEVICE_NUM) {
            DeviceInfo info = {NOMAL_MTU_VALUE, 0, STATE_DISCONNECTED, BLE_SERVER};
            deviceMap[deviceId] = info;
        } else if (deviceMap.size() >= MAX_DEVICE_NUM) { // 最大支持连接16台设备，包括客户端和服务端
            HILOG_ERROR(HILOG_MODULE_ACE, "The number of devices has reached the maximum limit 16.");
            return JSI::CreateUndefined();
        }
        connState = deviceMap[deviceId].connState;
    } else if (argsSize == 0) {
        // 未输入deviceId时，查找deviceMap中第一个未连接的服务端进行连接
        std::lock_guard<std::mutex> lock(connectMapMutex);
        auto it = std::find_if(deviceMap.begin(), deviceMap.end(), [](std::pair<const std::string, DeviceInfo> &pair) {
            return pair.second.deviceType == BLE_SERVER && pair.second.connState == STATE_DISCONNECTED;
        });
        if (it == deviceMap.end()) {
            HILOG_ERROR(HILOG_MODULE_ACE, "get deviceId failed!");
            return JSI::CreateUndefined();
        }
        deviceId = it->first;
        connState = it->second.connState;
    }
    if (connState == STATE_DISCONNECTED) {
        bd_addr_t bd_addr = {0};
        bd_addr.type = addrMap[deviceId];
        // 地址要小端序 "10:20:30:40:50:60"---->60 50 40 30 20 10
        ret = DecodeUuidFromStr(deviceId.c_str(), bd_addr.addr, &cnt);
        if (ret != BLE_SUCCESS) {
            HILOG_ERROR(HILOG_MODULE_ACE, "Decode Uuid failed! ret: 0x%X\n", ret);
            goto exit;
        }
        ret = gap_ble_connect_remote_device(&bd_addr);
        if (ret != BLE_SUCCESS) {
            HILOG_ERROR(HILOG_MODULE_ACE, "gap_ble_connect_remote_device failed! ret: 0x%X\n", ret);
        }
    } else {
        HILOG_INFO(HILOG_MODULE_ACE, "BLE has connected!\n");
    }
exit:
    return JSI::CreateNumber(ret);
}

static errcode_t ClientDisConnect(const std::string &deviceId, const ProfileConnectionState connState)
{
    errcode_t ret = BLE_SUCCESS;
    int32_t cnt = 0;

    if (connState == STATE_CONNECTED) {
        bd_addr_t bd_addr = {0};
        bd_addr.type = BleModule::addrMap[deviceId];
        // 地址要小端序 "10:20:30:40:50:60"---->60 50 40 30 20 10
        ret = DecodeUuidFromStr(deviceId.c_str(), bd_addr.addr, &cnt);
        if (ret != BLE_SUCCESS) {
            HILOG_ERROR(HILOG_MODULE_ACE, "Decode Uuid failed! ret: 0x%X\n", ret);
            return ret;
        }
        ret = gap_ble_disconnect_remote_device(&bd_addr);
        if (ret != BLE_SUCCESS) {
            HILOG_ERROR(HILOG_MODULE_ACE, "gap_ble_disconnect_remote_device failed! ret: 0x%X\n", ret);
        }
    } else {
        HILOG_INFO(HILOG_MODULE_ACE, "BLE has not connected!\n");
    }
    return ret;
}

JSIValue BleModule::DisConnect(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    if (argsSize > ARGC_ONE) {
        HILOG_ERROR(HILOG_MODULE_ACE, "params_cnt error!");
        return JSI::CreateNumber(BLE_INVALID_PARAMETER);  // 参数错误
    }

    errcode_t ret = 0;
    if (argsSize == 1) {
        std::string deviceId = ParseDeviceId(args[0]);
        if (deviceId.empty()) {
            HILOG_ERROR(HILOG_MODULE_ACE, "Invalid parameter!");
            return JSI::CreateNumber(BLE_INVALID_PARAMETER);  // 参数错误
        }
        std::lock_guard<std::mutex> lock(connectMapMutex);
        auto it = deviceMap.find(deviceId);
        if (it == deviceMap.end()) {
            HILOG_INFO(HILOG_MODULE_ACE, "BLE has not connected!\n");
            return JSI::CreateNumber(ret);
        }
        ret = ClientDisConnect(deviceId, it->second.connState);
        return JSI::CreateNumber(ret);
    }
    std::lock_guard<std::mutex> lock(connectMapMutex);
    for (auto it = deviceMap.begin(); it != deviceMap.end(); it++) {
        if (it->second.deviceType == BLE_CLIENT) {
            continue;
        }
        ret = ClientDisConnect(it->first, it->second.connState);
        if (ret != BLE_SUCCESS) {
            return JSI::CreateNumber(ret);
        }
    }
    return JSI::CreateNumber(ret);
}

JSIValue BleModule::GetDeviceName(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    if (argsSize < ARGC_ONE || argsSize > ARGC_TWO || !JSI::ValueIsFunction(args[0])) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Invalid parameter!");
        return JSI::CreateNumber(BLE_INVALID_PARAMETER);  // 参数错误
    }

    std::string deviceId;
    if (argsSize == ARGC_TWO) {
        deviceId = ParseDeviceId(args[1]);
    } else {
        deviceId = GetFirstServerDeviceId();
    }
    if (deviceId.empty()) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Invalid parameter!");
        return JSI::CreateNumber(BLE_INVALID_PARAMETER);  // 参数错误
    }
    JSIValue callback = JSI::AcquireValue(args[0]);
    JSIValue jsiData = JSI::CreateUndefined();
    errcode_t ret = 0;

    // 检查连接状态
    {
        std::lock_guard<std::mutex> lock(connectMapMutex);
        if (deviceMap.find(deviceId) == deviceMap.end()) {
            ret = BLE_UNSUPPORT;  // 未连接
        } else {
            // 从扫描结果中查找设备名称
            bool found = false;
            for (const auto &result : scanResult) {
                if (result.deviceId == deviceId) {
                    jsiData = JSI::CreateString(result.deviceName.c_str());
                    found = true;
                    break;
                }
            }

            if (!found) {
                ret = BLE_UNSUPPORT;  // 设备未找到
                jsiData = JSI::CreateString("");
            }
        }
    }

    // 执行回调
    JSIValue err = JSI::CreateNumber(ret);
    JSIValue argv[ARGC_TWO] = {err, jsiData};
    JSI::CallFunction(callback, thisVal, argv, ARGC_TWO);

    // 释放资源
    JSI::ReleaseValueList(err, jsiData, callback);
    return JSI::CreateUndefined();
}

JSIValue BleModule::GetRssiValue(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    if (argsSize < ARGC_ONE || argsSize > ARGC_TWO || !JSI::ValueIsFunction(args[0])) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Invalid parameter!");
        return JSI::CreateNumber(BLE_INVALID_PARAMETER);  // 参数错误
    }
    std::string deviceId;
    if (argsSize == ARGC_TWO) {
        deviceId = ParseDeviceId(args[1]);
    } else {
        deviceId = GetFirstServerDeviceId();
    }
    if (deviceId.empty()) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Invalid parameter!");
        return JSI::CreateNumber(BLE_INVALID_PARAMETER);  // 参数错误
    }
    errcode_t ret = 0;
    JSIValue jsiRssi = JSI::CreateUndefined();
    JSIValue callback = JSI::AcquireValue(args[0]);
    ProfileConnectionState connState = STATE_DISCONNECTED;
    {
        std::lock_guard<std::mutex> lock(connectMapMutex);
        auto it = deviceMap.find(deviceId);
        if (it != deviceMap.end()) {
            connState = it->second.connState;
        }
    }
    if (connState == STATE_CONNECTED) {
        for (auto result : scanResult) {
            if (std::strcmp(result.deviceId.c_str(), deviceId.c_str()) == 0) {
                jsiRssi = JSI::CreateNumber(result.rssi);
                break;
            }
        }
    } else {
        ret = BLE_OPRATION_FAILED;
        HILOG_ERROR(HILOG_MODULE_ACE, "BLE Operation failed, check conect state!");
    }

    JSIValue err = JSI::CreateNumber(ret);
    JSIValue argv[ARGC_TWO] = {err, jsiRssi};
    JSI::CallFunction(callback, thisVal, argv, ARGC_TWO);
    JSI::ReleaseValueList(jsiRssi, err, callback);
    return JSI::CreateUndefined();
}

JSIValue BleModule::GetDeviceMtu(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    if (argsSize != ARGC_TWO || !JSI::ValueIsFunction(args[1])) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Invalid parameter!");
        return JSI::CreateNumber(BLE_INVALID_PARAMETER);  // 参数错误
    }

    std::string deviceId = ParseDeviceId(args[0]);
    if (deviceId.empty()) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Invalid parameter!");
        return JSI::CreateNumber(BLE_INVALID_PARAMETER);  // 参数错误
    }
    JSIValue callback = JSI::AcquireValue(args[1]);
    JSIValue jsiData = JSI::CreateUndefined();
    errcode_t ret = 0;

    // 检查连接状态
    {
        std::lock_guard<std::mutex> lock(connectMapMutex);
        auto it = deviceMap.find(deviceId);
        if (it == deviceMap.end()) {
            ret = BLE_UNSUPPORT;  // 未连接
        } else {
            jsiData = JSI::CreateNumber(it->second.mtu);
        }
    }

    // 执行回调
    JSIValue err = JSI::CreateNumber(ret);
    JSIValue argv[ARGC_TWO] = {err, jsiData};
    JSI::CallFunction(callback, thisVal, argv, ARGC_TWO);

    // 释放资源
    JSI::ReleaseValueList(err, jsiData, callback);
    return JSI::CreateUndefined();
}

errcode_t BleModule::ReleaseConnect()
{
    errcode_t ret = 0;
    // 断开客户端所有连接
    std::lock_guard<std::mutex> lock(BleModule::connectMapMutex);
    for (auto it = deviceMap.begin(); it != deviceMap.end();) {
        if (it->second.deviceType == BLE_CLIENT) { // 服务端连接不做处理
            it++;
            continue;
        }
        if (it->second.connState == STATE_CONNECTED) {
            int32_t cnt = 0;
            bd_addr_t bd_addr = {0};
            bd_addr.type = addrMap[it->first];
            DecodeUuidFromStr(it->first.c_str(), bd_addr.addr, &cnt);
            ret = gap_ble_disconnect_remote_device(&bd_addr);
            if (ret != BLE_SUCCESS) {
                HILOG_ERROR(HILOG_MODULE_ACE, "gap_ble_disconnect_remote_device failed! ret: 0x%X\n", ret);
            }
            CleanupConnectionResources(it->second.connId);
        }
        it = deviceMap.erase(it);
    }
    return ret;
}

void BleModule::ReleaseClose()
{
    HILOG_INFO(HILOG_MODULE_ACE, "BleModule::ReleaseClose called");
    
    // 清理设备ID和GATT客户端
    if (!JSI::ValueIsUndefined(gattClient)) {
        JSI::ReleaseValue(gattClient);
        gattClient = JSI::CreateUndefined();
    }

    // 清理特定事件回调
    auto it = eventCallbacks.find("BLECharacteristicChange");
    if (it != eventCallbacks.end()) {
        EventCallback *oldEc = it->second;
        if (oldEc != nullptr) {
            if (!JSI::ValueIsUndefined(oldEc->callback)) {
                JSI::ReleaseValue(oldEc->callback);
                oldEc->callback = JSI::CreateUndefined();
            }
            if (!JSI::ValueIsUndefined(oldEc->context)) {
                JSI::ReleaseValue(oldEc->context);
                oldEc->context = JSI::CreateUndefined();
            }
            delete oldEc;
        }
        eventCallbacks.erase(it);
    }

    it = eventCallbacks.find("BLEConnectionStateChange");
    if (it != eventCallbacks.end()) {
        EventCallback *oldEc = it->second;
        if (oldEc != nullptr) {
            if (!JSI::ValueIsUndefined(oldEc->callback)) {
                JSI::ReleaseValue(oldEc->callback);
                oldEc->callback = JSI::CreateUndefined();
            }
            if (!JSI::ValueIsUndefined(oldEc->context)) {
                JSI::ReleaseValue(oldEc->context);
                oldEc->context = JSI::CreateUndefined();
            }
            delete oldEc;
        }
        eventCallbacks.erase(it);
    }

    // 清理GATT数据
    discoveredServicesMap.clear();

    // 重置MTU和地址映射
    {
        std::lock_guard<std::mutex> lock(addrMapMutex);
        addrMap.clear();
    }

    // 清理发现上下文
    {
        std::lock_guard<std::mutex> lock(contextMutex);
        for (auto& pair : discoveryContexts) {
            auto& ctx = pair.second;
            if (!JSI::ValueIsUndefined(ctx.callback)) {
                JSI::ReleaseValue(ctx.callback);
                ctx.callback = JSI::CreateUndefined();
            }
            if (!JSI::ValueIsUndefined(ctx.thisVal)) {
                JSI::ReleaseValue(ctx.thisVal);
                ctx.thisVal = JSI::CreateUndefined();
            }
        }
        discoveryContexts.clear();
    }

    // 清理操作上下文
    {
        std::lock_guard<std::mutex> lock(operationMutex);
        for (auto& ctx : operationContexts) {
            if (!JSI::ValueIsUndefined(ctx.callback)) {
                JSI::ReleaseValue(ctx.callback);
                ctx.callback = JSI::CreateUndefined();
            }
            if (!JSI::ValueIsUndefined(ctx.thisVal)) {
                JSI::ReleaseValue(ctx.thisVal);
                ctx.thisVal = JSI::CreateUndefined();
            }
        }
        operationContexts.clear();
    }

    HILOG_INFO(HILOG_MODULE_ACE, "BleModule::ReleaseClose completed");
}

JSIValue BleModule::ClientClose(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    errcode_t ret = 0;
    if (JSI::ValueIsUndefined(gattClient)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "ClientClose failed! has not register client!");
        return JSI::CreateNumber(BLE_OPRATION_FAILED);
    }

    ret = ReleaseConnect();
    ReleaseClose();
    
    HILOG_INFO(HILOG_MODULE_ACE, "BleModule::ClientClose completed with result: %d", ret);
    return JSI::CreateNumber(ret);
}

void BleModule::DestroyStopScanAndAdv()
{
    errcode_t ret = 0;
    if (advertisingMap[advId] == STARTED) {
        ret = gap_ble_stop_adv(advId);
        if (ret != 0) {
            HILOG_DEBUG(HILOG_MODULE_ACE, "Destroy stop adv failed!, ret:%X", ret);
        }
    }
    advertisingMap.clear();
    {
        std::lock_guard<std::mutex> lock(scanResultMutex);
        if (workerThreadRunning) {
            scanResult.clear();
            bleScanFilters.clear();
            ret = gap_ble_stop_scan();
            if (ret != 0) {
                HILOG_DEBUG(HILOG_MODULE_ACE, "Destroy stop scan failed!, ret:%X", ret);
            }
        }
        // 停止并清理扫描工作线程和队列
        if (workerThreadRunning) {
            workerThreadRunning = false;
            queueCond.notify_all();

            if (workerThreadId != NULL) {
                osThreadJoin(workerThreadId);
                workerThreadId = NULL;
            }
        }
    }

    // 清空扫描数据队列并释放内存
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        while (!scanDataQueue.empty()) {
            gap_scan_result_data_t scanData = scanDataQueue.front();
            if (scanData.adv_data != nullptr) {
                free(scanData.adv_data);
                scanData.adv_data = nullptr;
            }
            scanDataQueue.pop();
        }
    }
}

void BleModule::Destroy()
{
    HILOG_INFO(HILOG_MODULE_ACE, "BleModule::Destroy called - cleaning up all resources");
    
    // 1. 释放连接资源
    ReleaseConnect();
    
    // 2. 清理事件回调
    for (auto it = eventCallbacks.begin(); it != eventCallbacks.end();) {
        auto currentIt = it;
        ++it;
        EventCallback *ec = currentIt->second;
        if (ec != nullptr) {
            if (!JSI::ValueIsUndefined(ec->callback)) {
                JSI::ReleaseValue(ec->callback);
                ec->callback = JSI::CreateUndefined();
            }
            if (!JSI::ValueIsUndefined(ec->context)) {
                JSI::ReleaseValue(ec->context);
                ec->context = JSI::CreateUndefined();
            }
            delete ec;
        }
        eventCallbacks.erase(currentIt);
    }
    eventCallbacks.clear();
    // 3. 清理广播和扫描
    DestroyStopScanAndAdv();

    // 5. 清理GATT相关数据
    discoveredServicesMap.clear();

    // 6. 清理发现上下文
    {
        std::lock_guard<std::mutex> lock(contextMutex);
        for (auto& pair : discoveryContexts) {
            auto& ctx = pair.second;
            if (!JSI::ValueIsUndefined(ctx.callback)) {
                JSI::ReleaseValue(ctx.callback);
                ctx.callback = JSI::CreateUndefined();
            }
            if (!JSI::ValueIsUndefined(ctx.thisVal)) {
                JSI::ReleaseValue(ctx.thisVal);
                ctx.thisVal = JSI::CreateUndefined();
            }
        }
        discoveryContexts.clear();
    }

    // 7. 清理操作上下文
    {
        std::lock_guard<std::mutex> lock(operationMutex);
        for (auto& ctx : operationContexts) {
            if (!JSI::ValueIsUndefined(ctx.callback)) {
                JSI::ReleaseValue(ctx.callback);
                ctx.callback = JSI::CreateUndefined();
            }
            if (!JSI::ValueIsUndefined(ctx.thisVal)) {
                JSI::ReleaseValue(ctx.thisVal);
                ctx.thisVal = JSI::CreateUndefined();
            }
        }
        operationContexts.clear();
    }

    // 8. 清理其他静态资源
    {
        std::lock_guard<std::mutex> lock(addrMapMutex);
        addrMap.clear();
    }
    {
        std::lock_guard<std::mutex> lock(connectMapMutex);
        deviceMap.clear();
    }

    // 9. 释放gattClient对象
    if (!JSI::ValueIsUndefined(gattClient)) {
        JSI::ReleaseValue(gattClient);
        gattClient = JSI::CreateUndefined();
    }

    ServerDestroy();
    HILOG_INFO(HILOG_MODULE_ACE, "BleModule::Destroy completed");
}

static errcode_t CheckOnArguments(const JSIValue *args, uint8_t argsSize) {
    if (argsSize < ARGC_TWO) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Function parameters are not enough!");
        return BLE_INVALID_PARAMETER;
    }
    return BLE_SUCCESS;
}

static errcode_t ParseEventAndCallback(const JSIValue *args, const JSIValue thisVal, std::string &eventName, JSIValue &callback) {
    JSIValue jsiEventName = JSI::AcquireValue(args[0]);
    if (JSI::ValueIsUndefined(jsiEventName)) {
        return BLE_INVALID_PARAMETER;
    }
    char *eventNameTmp = JSI::ValueToString(jsiEventName);
    JSI::ReleaseValue(jsiEventName);
    if (eventNameTmp == nullptr) {
        return BLE_INVALID_PARAMETER;
    }
    eventName.assign(eventNameTmp);
    JSI::ReleaseString(eventNameTmp);
    if (eventName.empty()) {
        return BLE_INVALID_PARAMETER;
    }

    callback = JSI::AcquireValue(args[1]);
    return BLE_SUCCESS;
}

static void HandleExistingCallback(const std::string &eventName) {
    auto it = BleModule::eventCallbacks.find(eventName);
    if (it != BleModule::eventCallbacks.end()) {
        EventCallback *oldEc = it->second;
        JSI::ReleaseValue(oldEc->callback);
        JSI::ReleaseValue(oldEc->context);
        delete oldEc;
        BleModule::eventCallbacks.erase(it);
    }
}

static errcode_t CreateAndStoreCallback(const std::string &eventName, const JSIValue &callback, const JSIValue &thisVal) {
    EventCallback *ec = new EventCallback;
    if (ec == nullptr) {
        return BLE_OPRATION_FAILED;
    }
    ec->callback = JSI::AcquireValue(callback);
    ec->context = JSI::AcquireValue(thisVal);
    BleModule::eventCallbacks[eventName] = ec;
    JSI::ReleaseValue(callback);
    return BLE_SUCCESS;
}

static errcode_t RegisterBleCallbacks() {
    if (BleModule::hasOn == false) {
        gap_ble_callbacks_t gap_cb = {
            GapBleEnableStackCbk,
            GapBleDisableStackCbk,
            GapBleSetAdvDataCbk,
            GapBleSetAdvParamCbk,
            GapBleSetScanParamCbk,
            GapBleStartAdvCbk,
            GapBleStopAdvCbk,
            GapBleScanResultCbk,
            GapBleConnStateChangeCbk,
            GapBlePariedCompleteCbk,
            GapBleReadRemoteRssiCbk,
            GapBleTerminateAdvCbk,
            GapBleAuthCompeteCbk,
            GapBleConnectParamUpdateCbk,
        };
        errcode_t ret = wrapper_gap_ble_register_callbacks(&gap_cb);
        if (ret != BLE_SUCCESS) {
            HILOG_ERROR(HILOG_MODULE_ACE,
                "wrapper_gap_ble_register_callbacks failed! ret: 0x%x clientId:%u\n",
                ret,
                BleModule::clientId);
            return BLE_OPRATION_FAILED;
        }
        BleModule::hasOn = true;
    }
    return BLE_SUCCESS;
}

JSIValue BleModule::On(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize) {
    errcode_t ret = CheckOnArguments(args, argsSize);
    if (ret != BLE_SUCCESS) {
        return JSI::CreateNumber(ret);
    }
    ret = RegisterBleCallbacks();
    if (ret != BLE_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Register Gap callback failed!");
        JSI::ReleaseValue(gattClient);
        return JSI::CreateNumber(ret);
    }
    std::string eventName;
    JSIValue callback;
    ret = ParseEventAndCallback(args, thisVal, eventName, callback);
    if (ret != BLE_SUCCESS) {
        return JSI::CreateNumber(ret);
    }

    HandleExistingCallback(eventName);

    ret = CreateAndStoreCallback(eventName, callback, thisVal);
    if (ret != BLE_SUCCESS) {
        return JSI::CreateNumber(ret);
    }

    return JSI::CreateNumber(0);
}

JSIValue BleModule::Off(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    errcode_t ret = 0;
    if (argsSize < ARGC_ONE || argsSize > ARGC_THREE) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Function parameters are not enough!");
        return JSI::CreateBoolean(false);
    }
    JSIValue jsiEventName = JSI::AcquireValue(args[0]);
    if (JSI::ValueIsUndefined(jsiEventName)) {
        return JSI::CreateNumber(BLE_INVALID_PARAMETER);
    }
    char *eventName = JSI::ValueToString(jsiEventName);
    JSI::ReleaseValue(jsiEventName);
    if (eventName == nullptr) {
        return JSI::CreateNumber(BLE_INVALID_PARAMETER);
    }
    auto it = eventCallbacks.find(eventName);
    if (it != eventCallbacks.end()) {
        JSI::ReleaseValue(it->second->callback);
        JSI::ReleaseValue(it->second->context);
        delete it->second;
        eventCallbacks.erase(it);
        JSI::ReleaseString(eventName);
        return JSI::CreateBoolean(true);
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE, "Off failed, target not found!");
        JSI::ReleaseString(eventName);
    }
    return JSI::CreateBoolean(false);
}

JSIValue BleModule::ClientOn(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    if (argsSize < ARGC_TWO) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Function parameters are not enough!");
        return JSI::CreateBoolean(false);
    }
    JSIValue jsiEventName = JSI::AcquireValue(args[0]);
    if (JSI::ValueIsUndefined(jsiEventName)) {
        return JSI::CreateNumber(BLE_INVALID_PARAMETER);
    }
    char *eventName = JSI::ValueToString(jsiEventName);
    JSI::ReleaseValue(jsiEventName);
    if (eventName == nullptr) {
        return JSI::CreateNumber(BLE_INVALID_PARAMETER);
    }
    auto it = eventCallbacks.find(eventName);
    if (it != eventCallbacks.end()) {
        EventCallback *oldEc = it->second;
        JSI::ReleaseValue(oldEc->callback);
        JSI::ReleaseValue(oldEc->context);
        delete oldEc;
        eventCallbacks.erase(it);
    }

    JSIValue callback = JSI::AcquireValue(args[1]);

    EventCallback *ec = new EventCallback;
    if (ec != nullptr) {
        ec->callback = JSI::AcquireValue(callback);
        ec->context = JSI::AcquireValue(thisVal);
        eventCallbacks[eventName] = ec;
        JSI::ReleaseString(eventName);
        JSI::ReleaseValue(callback);
        return JSI::CreateBoolean(true);
    } else {
        JSI::ReleaseString(eventName);
        JSI::ReleaseValue(callback);
    }

    return JSI::CreateBoolean(false);
}

JSIValue BleModule::ClientOff(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    if (argsSize < 1) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Function parameters are not enough!");
        return JSI::CreateBoolean(false);
    }
    JSIValue jsiEventName = JSI::AcquireValue(args[0]);
    if (JSI::ValueIsUndefined(jsiEventName)) {
        return JSI::CreateNumber(BLE_INVALID_PARAMETER);
    }
    char *eventName = JSI::ValueToString(jsiEventName);
    JSI::ReleaseValue(jsiEventName);
    if (eventName == nullptr) {
        return JSI::CreateNumber(BLE_INVALID_PARAMETER);
    }
    auto it = eventCallbacks.find(eventName);
    if (it != eventCallbacks.end()) {
        JSI::ReleaseValue(it->second->callback);
        JSI::ReleaseValue(it->second->context);
        delete it->second;
        eventCallbacks.erase(it);
        JSI::ReleaseString(eventName);
        return JSI::CreateBoolean(true);
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE, "Off failed, target not found!");
        JSI::ReleaseString(eventName);
    }
    return JSI::CreateBoolean(false);
}

void InitBleModule(JSIValue exports)
{
    JSI::SetModuleAPI(exports, "createGattServer", BleModule::CreateGattServer);
    JSI::SetModuleAPI(exports, "createGattClientDevice", BleModule::CreateGattClientDevice);
    JSI::SetModuleAPI(exports, "startBLEScan", BleModule::StartBLEScan);
    JSI::SetModuleAPI(exports, "stopBLEScan", BleModule::StopBLEScan);
    JSI::SetModuleAPI(exports, "startAdvertising", BleModule::StartAdvertising);
    JSI::SetModuleAPI(exports, "stopAdvertising", BleModule::StopAdvertising);
    JSI::SetModuleAPI(exports, "on", BleModule::On);
    JSI::SetModuleAPI(exports, "off", BleModule::Off);
    JSI::SetOnTerminate(exports, BleModule::OnTerminate);
}

void BleModule::OnTerminate()
{
    BleModule::Destroy();
}
}  // namespace ACELite
}  // namespace OHOS

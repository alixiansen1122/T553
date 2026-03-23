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
#include "bts_gatt_server.h"
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
void GapBleEnableStackCbk(errcode_t status)
{
    HILOG_DEBUG(HILOG_MODULE_ACE, "EnableBLECallback status: %u\n", status);
}

void GapBleDisableStackCbk(errcode_t status)
{
    HILOG_DEBUG(HILOG_MODULE_ACE, "DisableBLECallback status: %u\n", status);
}

void GapBleSetAdvDataCbk(uint8_t adv_id, errcode_t status)
{
    HILOG_DEBUG(HILOG_MODULE_ACE, "SetAdvDataCallback adv_id: %u status: %u\n", adv_id, status);
}

void GapBleSetAdvParamCbk(uint8_t advId, errcode_t status)
{
    HILOG_DEBUG(HILOG_MODULE_ACE, "SetAdvParamCallback advId: %u status: %u\n", advId, status);
}

void GapBleSetScanParamCbk(errcode_t status)
{
    HILOG_DEBUG(HILOG_MODULE_ACE, "SetScanParameterCallback status: %u\n", status);
}

void BleModule::CheckSubscription(const std::string &type, const JSIValue err, const JSIValue value,
    const JSIValue deviceId)
{
    auto it = eventCallbacks.find(type);
    if (it != eventCallbacks.end() && it->second != nullptr) {
        // 确保callback和context有效
        if (!JSI::ValueIsUndefined(it->second->callback) && !JSI::ValueIsUndefined(it->second->context)) {
            if (type == "connectionStateChange" ||
                type == "BLEConnectionStateChange") {
                JSIValue argv[ARGC_TWO] = {err, value};
                JSI::CallFunction(it->second->callback, it->second->context, argv, ARGC_TWO);
            } else if (type == "BLECharacteristicChange") {
                JSIValue argv[ARGC_THREE] = {err, deviceId, value};
                JSI::CallFunction(it->second->callback, it->second->context, argv, ARGC_THREE);
            } else {
                JSIValue argv[ARGC_ONE] = {value};
                JSI::CallFunction(it->second->callback, it->second->context, argv, ARGC_ONE);
            }
        } else {
            HILOG_ERROR(HILOG_MODULE_ACE, "Invalid callback for event: %s", type.c_str());
        }
    } else {
        HILOG_WARN(HILOG_MODULE_ACE, "No callback registered for event: %s", type.c_str());
    }
    // 释放传入的参数
    if (!JSI::ValueIsUndefined(err)) {
        JSI::ReleaseValue(err);
    }
    if (!JSI::ValueIsUndefined(value)) {
        JSI::ReleaseValue(value);
    }
}

static std::string GetDeviceIdByConnId(uint16_t connId)
{
    std::lock_guard<std::mutex> lock(BleModule::connectMapMutex);
    auto it = std::find_if(BleModule::deviceMap.begin(), BleModule::deviceMap.end(),
        [&](std::pair<const std::string, DeviceInfo> &pair){
            return pair.second.connId == connId;
    });
    if (it == BleModule::deviceMap.end()) {
        return "";
    }
    return it->first;
}

static void CallFuncExecute(void *data)
{
    HILOG_DEBUG(HILOG_MODULE_ACE, "CallFuncExecute enter.\n");
    CallFuncAsyncParams *params = static_cast<CallFuncAsyncParams *>(data);
    if (params == nullptr) {
        return;
    }

    if (params->type.empty()) {
        // 类型为空，直接调用回调函数
        JSIValue callback = params->callback;
        JSIValue context = params->context;
        JSIValue result = params->result;
        JSIValue err = params->err;
        JSIValue argv[ARGC_TWO] = {err, result};
        JSI::CallFunction(callback, context, argv, ARGC_TWO);

        // 释放所有资源
        if (!JSI::ValueIsUndefined(err)) {
            JSI::ReleaseValue(err);
        }
        if (!JSI::ValueIsUndefined(result)) {
            JSI::ReleaseValue(result);
        }
        if (!JSI::ValueIsUndefined(callback)) {
            JSI::ReleaseValue(callback);
        }
        if (!JSI::ValueIsUndefined(context)) {
            JSI::ReleaseValue(context);
        }
    } else {
        // 类型不为空，通过CheckSubscription处理
        JSIValue deviceId = JSI::CreateUndefined();
        if (params->type == "BLECharacteristicChange") {
            JSIValue jsiBleCharacteristic = JSI::CreateObject();
            errcode_t ret = SetJsiBLECharacteristic(jsiBleCharacteristic, params->connId, params->characteristicArgs);
            std::string deviceIdStr = GetDeviceIdByConnId(params->connId);
            deviceId = JSI::CreateString(deviceIdStr.c_str());
            // 释放所有资源
            if (!JSI::ValueIsUndefined(params->err)) {
                JSI::ReleaseValue(params->err);
            }
            if (!JSI::ValueIsUndefined(params->result)) {
                JSI::ReleaseValue(params->result);
            }
            if (ret != BLE_SUCCESS) {
                // 释放资源并处理错误
                JSI::ReleaseValue(jsiBleCharacteristic);
                params->err = JSI::CreateNumber(ret);
                params->result = JSI::CreateUndefined();
            } else {
                params->err = JSI::CreateNumber(ret);
                params->result = jsiBleCharacteristic;
            }
        }
        BleModule::CheckSubscription(params->type, params->err, params->result, deviceId);
        // 释放callback和context
        if (!JSI::ValueIsUndefined(params->callback)) {
            JSI::ReleaseValue(params->callback);
        }
        if (!JSI::ValueIsUndefined(params->context)) {
            JSI::ReleaseValue(params->context);
        }
        if (params->type == "BLECharacteristicChange") {
            if (params->characteristicArgs != nullptr) {
                delete(params->characteristicArgs);
                params->characteristicArgs = nullptr;
            }
        }
    }
    if (params != nullptr) {
        delete params;  // 释放params内存
        params = nullptr;
    }
}

void BleDispatchAsyncWork(const DispatchAsync &asyncParameter)
{
    HILOG_DEBUG(HILOG_MODULE_ACE, "BleDispatchAsyncWork enter.\n");
    // 在主线程中执行JS回调
    CallFuncAsyncParams *params = new CallFuncAsyncParams();
    if (params == nullptr) {
        // 如果分配失败，释放所有传入的资源
        if (!JSI::ValueIsUndefined(asyncParameter.err)) {
            JSI::ReleaseValue(asyncParameter.err);
        }
        if (!JSI::ValueIsUndefined(asyncParameter.result)) {
            JSI::ReleaseValue(asyncParameter.result);
        }
        if (!JSI::ValueIsUndefined(asyncParameter.callback)) {
            JSI::ReleaseValue(asyncParameter.callback);
        }
        if (!JSI::ValueIsUndefined(asyncParameter.thisVal)) {
            JSI::ReleaseValue(asyncParameter.thisVal);
        }
        return;
    }
    params->err = asyncParameter.err;
    params->result = asyncParameter.result;
    params->callback = JSI::AcquireValue(asyncParameter.callback);
    params->context = JSI::AcquireValue(asyncParameter.thisVal); // 增加引用计数
    params->type = asyncParameter.type;
    if (asyncParameter.type == "BLECharacteristicChange") { // BLECharacteristicChange会频繁触发，耗时操作挪到js任务
        params->connId = asyncParameter.connId;
        // characteristicArgs类型中包含stl对象使用拷贝构造，避免悬空指针
        params->characteristicArgs = new BLECharacteristic(asyncParameter.characteristicArgs); 
    }
    if (!JsAsyncWork::DispatchAsyncWork(CallFuncExecute, static_cast<void *>(params))) {
        if (!JSI::ValueIsUndefined(asyncParameter.err)) {
            JSI::ReleaseValue(asyncParameter.err);
        }
        if (!JSI::ValueIsUndefined(asyncParameter.result)) {
            JSI::ReleaseValue(asyncParameter.result);
        }
        if (!JSI::ValueIsUndefined(asyncParameter.callback)) {
            JSI::ReleaseValue(asyncParameter.callback);
        }
        if (!JSI::ValueIsUndefined(params->callback)) {
            JSI::ReleaseValue(params->callback);
        }
        if (!JSI::ValueIsUndefined(asyncParameter.thisVal)) {
            JSI::ReleaseValue(asyncParameter.thisVal);
        }
        if (!JSI::ValueIsUndefined(params->context)) {
            JSI::ReleaseValue(params->context);
        }
        if (asyncParameter.type == "BLECharacteristicChange") {
            if (params->characteristicArgs != nullptr) {
                delete params->characteristicArgs;
                params->characteristicArgs = nullptr;
            }
        }
        if (params != nullptr) {
            delete params;
            params = nullptr;
        }
    }
}

void GapBleStartAdvCbk(uint8_t advId, adv_status_t status)
{
    AdvertisingStateChangeInfo changeInfo;
    memset_s(&changeInfo, sizeof(changeInfo), 0, sizeof(changeInfo));
    changeInfo.advertisingId = advId;
    changeInfo.state = STARTED;
    BleModule::advertisingMap[advId] = STARTED;
    JSIValue result = JSI::CreateObject();
    JSI::SetNumberProperty(result, "advertisingId", changeInfo.advertisingId);
    JSI::SetNumberProperty(result, "state", changeInfo.state);
    JSIValue ret = JSI::CreateNumber(0);
    DispatchAsync asyncParameter;
    memset_s(&asyncParameter, sizeof(DispatchAsync), 0, sizeof(DispatchAsync));
    asyncParameter.err = ret;
    asyncParameter.result = result;
    asyncParameter.callback = JSI::CreateUndefined();
    asyncParameter.thisVal = JSI::CreateUndefined();
    asyncParameter.type = "advertisingStateChange";
    BleDispatchAsyncWork(asyncParameter);
}

void GapBleStopAdvCbk(uint8_t advId, adv_status_t status)
{
    AdvertisingStateChangeInfo changeInfo;
    memset_s(&changeInfo, sizeof(changeInfo), 0, sizeof(changeInfo));
    changeInfo.advertisingId = advId;
    changeInfo.state = STOPPED;
    BleModule::advertisingMap[advId] = STOPPED;
    JSIValue result = JSI::CreateObject();
    JSI::SetNumberProperty(result, "advertisingId", changeInfo.advertisingId);
    JSI::SetNumberProperty(result, "state", changeInfo.state);
    JSIValue ret = JSI::CreateNumber(0);
    DispatchAsync asyncParameter;
    memset_s(&asyncParameter, sizeof(DispatchAsync), 0, sizeof(DispatchAsync));
    asyncParameter.err = ret;
    asyncParameter.result = result;
    asyncParameter.callback = JSI::CreateUndefined();
    asyncParameter.thisVal = JSI::CreateUndefined();
    asyncParameter.type = "advertisingStateChange";
    BleDispatchAsyncWork(asyncParameter);
}

static void BleScanResultFilter(ScanResult &scanResultTmp, gap_scan_result_data_t *scan_result_data)
{
    // 解析广播数据
    BleParsedResult parsedResult = ParseMixedAdvertisingData(scan_result_data->adv_data, scan_result_data->adv_len);

    // 查找是否已存在相同设备
    auto it = std::find_if(
        BleModule::scanResult.begin(), BleModule::scanResult.end(), [&scanResultTmp](const ScanResult &existingResult) {
            return existingResult.deviceId == scanResultTmp.deviceId;
        });

    // 无过滤条件时直接处理
    if (BleModule::bleScanFilters.empty()) {
        UpdateScanResult(scanResultTmp, it, BleModule::scanResult);
        return;
    }

    // 有过滤条件时处理：只要满足任意一个过滤条件的所有要求
    bool matched = false;
    for (auto &bleScanFilter : BleModule::bleScanFilters) {
        // 检查是否满足当前过滤条件的所有要求
        if (ReadMatchFilter(bleScanFilter, parsedResult, scanResultTmp)) {
            matched = true;
            break;  // 满足任一条件即可
        }
    }

    if (matched) {
        UpdateScanResult(scanResultTmp, it, BleModule::scanResult);
    } else {
        HILOG_WARN(HILOG_MODULE_ACE, "[Filter] Device filtered out: %s", scanResultTmp.deviceId.c_str());
    }
}

void WorkerThreadFunc(void *argument)
{
    while (BleModule::workerThreadRunning) {
        gap_scan_result_data_t scanData;
        bool hasData = false;

        {
            std::unique_lock<std::mutex> lock(BleModule::queueMutex);
            BleModule::queueCond.wait(lock, [] {
                return !BleModule::scanDataQueue.empty() || !BleModule::workerThreadRunning;
            });
            
            // 检查是否需要退出
            if (!BleModule::workerThreadRunning) {
                break;
            }

            if (!BleModule::scanDataQueue.empty()) {
                scanData = BleModule::scanDataQueue.front();
                BleModule::scanDataQueue.pop();
                hasData = true;
            }
        }

        if (!hasData) {
            continue;
        }
        // 处理扫描结果
        ScanResult scanResultTmp;
        memset_s(&scanResultTmp, sizeof(ScanResult), 0, sizeof(ScanResult));
        scanResultTmp.rssi = scanData.rssi;
        scanResultTmp.deviceId = FormatMacAddress(scanData.addr.addr);

        if (scanData.adv_data && scanData.adv_len > 0) {
            scanResultTmp.data.assign(scanData.adv_data, scanData.adv_data + scanData.adv_len);
        }

        scanResultTmp.deviceName = FormatDeviceName(&scanData);

        {
            std::lock_guard<std::mutex> lock(BleModule::scanResultMutex);
            BleScanResultFilter(scanResultTmp, &scanData);
        }

        {
            std::lock_guard<std::mutex> lockAddr(BleModule::addrMapMutex);
            BleModule::addrMap[scanResultTmp.deviceId] = scanData.addr.type;
        }

        JSIValue resultArray = JSI::CreateArray(0);
        uint32_t resultCount = 0;

        {
            std::lock_guard<std::mutex> lock(BleModule::scanResultMutex);
            if (!BleModule::scanResult.empty()) {
                resultArray = JSI::CreateArray(BleModule::scanResult.size());
                for (size_t i = 0; i < BleModule::scanResult.size(); i++) {
                    uint8_t *resultDataPtr = nullptr;
                    uint32_t bufferLength = BleModule::scanResult[i].data.size();
                    if (bufferLength == 0) {
                        HILOG_ERROR(HILOG_MODULE_ACE, "bufferLength is 0");
                        continue;
                    }
                    JSIValue dataArray = JSI::CreateArrayBuffer(bufferLength, resultDataPtr);
                    // 检查分配是否成功
                    if (JSI::ValueIsUndefined(dataArray)) {
                        HILOG_ERROR(HILOG_MODULE_ACE, "CreateArrayBuffer failed (returned Undefined)");
                        continue;
                    }
                    if (bufferLength > 0 && resultDataPtr == nullptr) {
                        JSI::ReleaseValueList(dataArray);
                        HILOG_ERROR(HILOG_MODULE_ACE, "CreateArrayBuffer for scan result data failed!");
                        continue;
                    }
                    if (memcpy_s(resultDataPtr, bufferLength, BleModule::scanResult[i].data.data(),
                            bufferLength) != EOK) {
                        JSI::ReleaseValueList(dataArray);
                        HILOG_ERROR(HILOG_MODULE_ACE, "memcpy_s scan result data failed!");
                        continue;
                    }
                    JSIValue jsiResultItem = JSI::CreateObject();
                    JSI::SetStringProperty(jsiResultItem, "deviceId", BleModule::scanResult[i].deviceId.c_str());
                    JSI::SetNumberProperty(jsiResultItem, "rssi", BleModule::scanResult[i].rssi);
                    JSI::SetNamedProperty(jsiResultItem, "data", dataArray);

                    const char *deviceName = BleModule::scanResult[i].deviceName.empty() ? 
                        "" : BleModule::scanResult[i].deviceName.c_str();
                    JSI::SetStringProperty(jsiResultItem, "deviceName", deviceName);
                    JSI::SetBooleanProperty(jsiResultItem, "connectable", true);

                    JSI::SetPropertyByIndex(resultArray, resultCount++, jsiResultItem);
                    JSI::ReleaseValueList(jsiResultItem, dataArray);
                }
            }
        }

        JSIValue ret = JSI::CreateNumber(0);
        DispatchAsync asyncParameter;
        memset_s(&asyncParameter, sizeof(DispatchAsync), 0, sizeof(DispatchAsync));
        asyncParameter.err = ret;
        asyncParameter.result = resultArray;
        asyncParameter.callback = JSI::CreateUndefined();
        asyncParameter.thisVal = JSI::CreateUndefined();
        asyncParameter.type = "BLEDeviceFind";
        BleDispatchAsyncWork(asyncParameter);
        // 释放动态分配的内存
        if (scanData.adv_data) {
            free(scanData.adv_data);
            scanData.adv_data = nullptr;
        }
    }

    // 线程退出前清空队列
    {
        std::lock_guard<std::mutex> lock(BleModule::queueMutex);
        while (!BleModule::scanDataQueue.empty()) {
            gap_scan_result_data_t scanData = BleModule::scanDataQueue.front();
            if (scanData.adv_data) {
                free(scanData.adv_data);
            }
            BleModule::scanDataQueue.pop();
        }
    }

    // 线程退出
    BleModule::workerThreadId = NULL;
}

void GapBleScanResultCbk(gap_scan_result_data_t *scan_result_data)
{
    if (scan_result_data == NULL) {
        HILOG_ERROR(HILOG_MODULE_ACE, "scan_result_data is null");
        return;
    }

    // 检查工作线程是否在运行
    if (!BleModule::workerThreadRunning) {
        HILOG_WARN(HILOG_MODULE_ACE, "Worker thread not running, ignoring scan result");
        return;
    }

    // 深拷贝扫描数据
    gap_scan_result_data_t scanData;
    memset_s(&scanData, sizeof(gap_scan_result_data_t), 0, sizeof(gap_scan_result_data_t));
    scanData.rssi = scan_result_data->rssi;
    scanData.addr = scan_result_data->addr;
    scanData.adv_len = scan_result_data->adv_len;

    if (scan_result_data->adv_data && scan_result_data->adv_len > 0) {
        scanData.adv_data = (uint8_t *)malloc(scan_result_data->adv_len);
        if (scanData.adv_data != nullptr) {
            if (memcpy_s(scanData.adv_data, scan_result_data->adv_len, 
                        scan_result_data->adv_data, scan_result_data->adv_len) != EOK) {
                HILOG_ERROR(HILOG_MODULE_ACE, "memcpy_s failed for adv_data");
                free(scanData.adv_data);
                return;
            }
        } else {
            HILOG_ERROR(HILOG_MODULE_ACE, "Failed to allocate memory for adv_data");
            return;
        }
    }

    // 将数据加入队列
    {
        std::lock_guard<std::mutex> lock(BleModule::queueMutex);
        BleModule::scanDataQueue.push(scanData);
    }
    BleModule::queueCond.notify_one();
}

// 清理连接资源的辅助函数
void BleModule::CleanupConnectionResources(uint16_t conn_id)
{
    {
        std::lock_guard<std::mutex> lock(contextMutex);

        // 1. 清理发现上下文
        auto ctxIt = discoveryContexts.find(conn_id);
        if (ctxIt != discoveryContexts.end()) {
            auto &ctx = ctxIt->second;
            JSI::ReleaseValue(ctx.callback);
            JSI::ReleaseValue(ctx.thisVal);
            discoveryContexts.erase(ctxIt);
        }

        // 2. 清理服务数据
        auto servicesIt = discoveredServicesMap.find(conn_id);
        if (servicesIt != discoveredServicesMap.end()) {
            discoveredServicesMap.erase(servicesIt);
        }

        // 3. 清理操作上下文
        for (auto it = operationContexts.begin(); it != operationContexts.end();) {
            if (it->connId == conn_id) {
                JSI::ReleaseValue(it->callback);
                JSI::ReleaseValue(it->thisVal);
                it = operationContexts.erase(it);
            } else {
                ++it;
            }
        }
    }
}

void GapBleConnStateChangeCbk(uint16_t conn_id, bd_addr_t *addr, gap_ble_conn_state_t conn_state,
    gap_ble_pair_state_t pair_state, gap_ble_disc_reason_t disc_reason)
{
    if (addr == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "BLE Connection State Change - addr: null");
        return;
    }
    std::string deviceId = FormatMacAddress(addr->addr);
    DeviceType deviceType = BLE_SERVER;
    ProfileConnectionState connState = STATE_DISCONNECTED;
    // 处理连接状态变化
    if (conn_state == GAP_BLE_STATE_CONNECTED) {
        // 连接建立
        connState = STATE_CONNECTED;
        {
            std::lock_guard<std::mutex> lock(BleModule::connectMapMutex);
            auto it = BleModule::deviceMap.find(deviceId);
            if (it == BleModule::deviceMap.end() && BleModule::deviceMap.size() < MAX_DEVICE_NUM) {
                // 被动连接，对端为客服端
                DeviceInfo info = {NOMAL_MTU_VALUE, conn_id, STATE_CONNECTED, BLE_CLIENT};
                BleModule::deviceMap[deviceId] = info;
                deviceType = BLE_CLIENT;
            } else if (BleModule::deviceMap.size() >= MAX_DEVICE_NUM) {
                HILOG_ERROR(HILOG_MODULE_ACE, "The number of devices has reached the maximum limit 16.");
                return;
            } else if (it->second.connState == STATE_CONNECTED) {
                HILOG_ERROR(HILOG_MODULE_ACE, "device is connected.");
                return;
            } else {
                // 主动连接，对端为服务端
                auto &deviceInfo = it->second;
                deviceInfo.connState = STATE_CONNECTED;
                deviceInfo.connId = conn_id;
                deviceType = deviceInfo.deviceType;
            }
        }

        HILOG_INFO(HILOG_MODULE_ACE, "Device connected: %s, clientId:%d, connId=%u", deviceId.c_str(),
            BleModule::clientId, conn_id);

        // 请求MTU交换
        if (deviceType == BLE_SERVER) {
            errcode_t ret = gattc_exchange_mtu_req(BleModule::clientId, conn_id, NOMAL_MTU_VALUE);
            if (ret != BLE_SUCCESS) {
                HILOG_ERROR(HILOG_MODULE_ACE, "MTU exchange failed: %X", ret);
            }
        }
    } else if (conn_state == GAP_BLE_STATE_DISCONNECTED) {
        // 连接断开
        {
            std::lock_guard<std::mutex> lock(BleModule::connectMapMutex);
            auto it = BleModule::deviceMap.find(deviceId);
            if (it != BleModule::deviceMap.end()) {
                deviceType = it->second.deviceType;
                BleModule::deviceMap.erase(it);
            }
        }

        connState = STATE_DISCONNECTED;
        {
            std::lock_guard<std::mutex> lock(BleModule::addrMapMutex);
            auto addrIt = BleModule::addrMap.find(deviceId);
            if (addrIt != BleModule::addrMap.end()) {
                BleModule::addrMap.erase(addrIt);
            }
        }
        // 清理该连接的所有资源
        BleModule::CleanupConnectionResources(conn_id);

        HILOG_INFO(HILOG_MODULE_ACE, "Device disconnected: %s, connId=%u, reason=%d", deviceId.c_str(), conn_id,
            disc_reason);
    }
    JSIValue result = JSI::CreateObject();
    uint8_t state = static_cast<int32_t>(connState);
    JSI::SetStringProperty(result, "deviceId", deviceId.c_str());
    JSI::SetNumberProperty(result, "state", state);
    std::string event = deviceType == BLE_SERVER ? "BLEConnectionStateChange" : "connectionStateChange";
    auto clientEventIt = BleModule::eventCallbacks.find(event);
    if (clientEventIt != BleModule::eventCallbacks.end() && clientEventIt->second != nullptr) {
        DispatchAsync asyncParameter;
        memset_s(&asyncParameter, sizeof(DispatchAsync), 0, sizeof(DispatchAsync));
        asyncParameter.err = JSI::CreateNumber(0);
        asyncParameter.result = result;
        asyncParameter.callback = JSI::CreateUndefined();
        asyncParameter.thisVal = JSI::CreateUndefined();
        asyncParameter.type = event;
        BleDispatchAsyncWork(asyncParameter);
    } else {
        // 没有注册回调，释放创建的JSI值
        JSI::ReleaseValueList(result);
    }
}

void GapBlePariedCompleteCbk(uint16_t conn_id, const bd_addr_t *addr, errcode_t status)
{
    HILOG_DEBUG(HILOG_MODULE_ACE, "PairedCompleteCall conn_id:%u status:%u\r\n", conn_id, status);
}

void GapBleTerminateAdvCbk(uint8_t adv_id, adv_status_t status)
{
    AdvertisingStateChangeInfo changeInfo;
    memset_s(&changeInfo, sizeof(changeInfo), 0, sizeof(changeInfo));
    changeInfo.advertisingId = adv_id;
    changeInfo.state = STOPPED;
    BleModule::advertisingMap[adv_id] = STOPPED;
    JSIValue result = JSI::CreateObject();
    JSI::SetNumberProperty(result, "advertisingId", changeInfo.advertisingId);
    JSI::SetNumberProperty(result, "state", changeInfo.state);
    JSIValue ret = JSI::CreateNumber(0);
    DispatchAsync asyncParameter;
    memset_s(&asyncParameter, sizeof(DispatchAsync), 0, sizeof(DispatchAsync));
    asyncParameter.err = ret;
    asyncParameter.result = result;
    asyncParameter.callback = JSI::CreateUndefined();
    asyncParameter.thisVal = JSI::CreateUndefined();
    asyncParameter.type = "advertisingStateChange";
    BleDispatchAsyncWork(asyncParameter);
}

void GapBleAuthCompeteCbk(uint16_t conn_id, const bd_addr_t *addr, errcode_t status,
    const ble_auth_info_evt_t* evt)
{
    HILOG_DEBUG(HILOG_MODULE_ACE, "AuthCompleteCall conn_id:%u status:0x%x\r\n", conn_id, status);
}

void GapBleReadRemoteRssiCbk(uint16_t conn_id, int8_t rssi, errcode_t status)
{
    HILOG_DEBUG(HILOG_MODULE_ACE, "read remote rssi conn_id:%u rssi:%d status:%u\r\n", conn_id, rssi, status);
}

void GapBleConnectParamUpdateCbk(uint16_t conn_id, errcode_t status,
    const gap_ble_conn_param_update_t *param)
{
    HILOG_DEBUG(HILOG_MODULE_ACE, "connect param changed conn_id:%u status:%u", conn_id, status);
}

static void DiscoverDescriptors(uint8_t client_id, uint16_t conn_id, uint16_t char_handle);


// 完成发现后的回调通知
static void PostDiscoveryComplete(DiscoveryData data)
{
    // 从discoveredServicesMap获取组装后的服务数据
    JSIValue result;
    CreateGattServices(&result, BleModule::discoveredServicesMap[data.connId]);

    // 在主线程中执行JS回调
    JSIValue ret = JSI::CreateNumber(0);
    DispatchAsync asyncParameter;
    memset_s(&asyncParameter, sizeof(DispatchAsync), 0, sizeof(DispatchAsync));
    asyncParameter.err = ret;
    asyncParameter.result = result;
    asyncParameter.callback = data.callback;
    asyncParameter.thisVal = data.thisVal;
    asyncParameter.type = "";
    BleDispatchAsyncWork(asyncParameter);
}

// 服务发现回调
void GattcDiscoverySvcCbk(uint8_t client_id, uint16_t conn_id, gattc_discovery_service_result_t *service,
    errcode_t status)
{
    if (status != BLE_SUCCESS || !service) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Service discovery failed, status=%u", status);
        return;
    }
    if (service->start_hdl > service->end_hdl) {
        return;
    }

    std::lock_guard<std::mutex> lock(BleModule::contextMutex);
    auto& gattServices = BleModule::discoveredServicesMap[conn_id];

    // 检查是否已存在相同句柄范围的服务
    bool exists = false;
    for (const auto &svc : gattServices) {
        if (svc.service.start_hdl == service->start_hdl && svc.service.end_hdl == service->end_hdl) {
            exists = true;
            break;
        }
    }

    if (exists) {
        return;
    }

    GattService newService;
    newService.serviceUuid = GetUUidStr(service->uuid);
    newService.isPrimary = true;
    if (memcpy_s(&newService.service, sizeof(gattc_discovery_service_result_t),
        service, sizeof(gattc_discovery_service_result_t)) != EOK) {
        HILOG_DEBUG(HILOG_MODULE_ACE, "memcpy_s new service failed!");
        return;
    }

    gattServices.push_back(newService);
}

DiscoveryContext *BleModule::GetValidDiscoveryContext(uint16_t connId)
{
    auto it = discoveryContexts.find(connId);
    if (it == discoveryContexts.end()) {
        HILOG_ERROR(HILOG_MODULE_ACE, "GetValidDiscoveryContext: context not found (connId=%u)", connId);
        return nullptr;
    }
    return &(it->second);
}

void BleModule::DiscoverCharacteristicsForService(uint8_t client_id, uint16_t conn_id)
{
    uint16_t serviceHandle = 0;
    bool skipService = false;
    bool startDescriptorDiscovery = false;

    {
        std::lock_guard<std::mutex> lock(contextMutex);
        auto ctx = GetValidDiscoveryContext(conn_id);
        if (!ctx)
            return;

        auto &gattServices = discoveredServicesMap[conn_id];
        if (ctx->currentServiceIndex >= gattServices.size()) {
            HILOG_DEBUG(HILOG_MODULE_ACE, "All services processed, will start descriptor discovery");
            startDescriptorDiscovery = true;
        } else {
            GattService &service = gattServices[ctx->currentServiceIndex];
            // 检查服务是否有效
            if (service.service.end_hdl < service.service.start_hdl) {
                skipService = true;
            } else if (service.service.end_hdl - service.service.start_hdl < 1) { // 检查服务是否可能包含特征值
                skipService = true;
            } else {
                serviceHandle = service.service.start_hdl;
            }

            // 增加服务索引（如果需要跳过）
            if (skipService) {
                ctx->currentServiceIndex++;
            }
        }
    }

    if (startDescriptorDiscovery) {
        StartDescriptorDiscovery(client_id, conn_id);
        return;
    }

    if (skipService) {
        DiscoverCharacteristicsForService(client_id, conn_id);
        return;
    }

    gattc_discovery_character_param_t param = {.service_handle = serviceHandle};
    errcode_t ret = gattc_discovery_character(client_id, conn_id, &param);
    if (ret != BLE_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Failed to start char discovery: %d", ret);
        // 在锁内更新索引
        {
            std::lock_guard<std::mutex> lock(contextMutex);
            auto ctx = GetValidDiscoveryContext(conn_id);
            if (ctx)
                ctx->currentServiceIndex++;
        }

        // 递归调用（无锁状态）
        DiscoverCharacteristicsForService(client_id, conn_id);
    }
}

// 服务发现完成回调
void GattcDiscoverySvcResponse(uint8_t client_id, uint16_t conn_id, bt_uuid_t *uuid, errcode_t status)
{
    // 先处理需要加锁的操作
    bool needStartCharDiscovery = false;
    {
        std::lock_guard<std::mutex> lock(BleModule::contextMutex);
        auto ctx = BleModule::GetValidDiscoveryContext(conn_id);
        if (!ctx) {
            HILOG_ERROR(HILOG_MODULE_ACE, "GattcDiscoverySvcResponse: context not found");
            return;
        }
        if (status != BLE_SUCCESS) {
            DiscoveryData data = {conn_id, ctx->callback, ctx->thisVal};
            PostDiscoveryComplete(data);
            HILOG_ERROR(HILOG_MODULE_ACE, "Service discovery completed with error: %d", status);
            return;
        }

        auto &gattServices = BleModule::discoveredServicesMap[conn_id];
        if (gattServices.empty()) {
            HILOG_WARN(HILOG_MODULE_ACE, "No services discovered");
            DiscoveryData data = {conn_id, ctx->callback, ctx->thisVal};
            PostDiscoveryComplete(data);
            return;
        }
        // 按起始句柄排序服务
        std::sort(gattServices.begin(), gattServices.end(),
            [](const GattService &a, const GattService &b) { return a.service.start_hdl < b.service.start_hdl; });

        // 重置服务索引
        ctx->currentServiceIndex = 0;
        needStartCharDiscovery = true;
    }  // 释放锁

    // 在锁外启动特征值发现
    if (needStartCharDiscovery) {
        BleModule::DiscoverCharacteristicsForService(client_id, conn_id);
    }
}

// 特征值发现回调
void GattcDiscoveryCharCbk(uint8_t client_id, uint16_t conn_id,
    gattc_discovery_character_result_t *character, errcode_t status)
{
    if (status != BLE_SUCCESS || !character) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Characteristic discovery failed: %d", status);
        return;
    }
    std::lock_guard<std::mutex> lock(BleModule::contextMutex);
    auto ctx = BleModule::GetValidDiscoveryContext(conn_id);
    if (!ctx)
        return;

    // 检查特征值与服务是否匹配
    GattService &service = BleModule::discoveredServicesMap[conn_id][ctx->currentServiceIndex];
    if (character->declare_handle < service.service.start_hdl ||
        character->declare_handle > service.service.end_hdl) {
        return;
    }

    // 检查是否已存在相同句柄的特征值
    auto &gattCharacteristics = service.characteristics;
    bool exists = false;
    for (const auto &ch : gattCharacteristics) {
        if (ch.character.declare_handle == character->declare_handle) {
            exists = true;
            break;
        }
    }
    if (exists) {
        return;
    }

    BLECharacteristic newChar;
    memset_s(&newChar, sizeof(BLECharacteristic), 0, sizeof(BLECharacteristic));
    newChar.characteristicUuid = GetUUidStr(character->uuid);
    if (memcpy_s(&newChar.character, sizeof(gattc_discovery_character_result_t),
        character, sizeof(gattc_discovery_character_result_t)) != EOK) {
        HILOG_ERROR(HILOG_MODULE_ACE, "memcpy_s new character failed!");
        return;
    }

    GetProperties(character->properties, newChar.properties);
    newChar.serviceUuid = service.serviceUuid;
    newChar.service_start_hdl = service.service.start_hdl;
    newChar.service_end_hdl = service.service.end_hdl;

    gattCharacteristics.push_back(newChar);
}

// 特征值发现完成回调
void GattcDiscoveryCharResponse(uint8_t client_id, uint16_t conn_id, gattc_discovery_character_param_t *param,
    errcode_t status)
{
    bool startNext = false;
    {
        std::lock_guard<std::mutex> lock(BleModule::contextMutex);
        auto ctx = BleModule::GetValidDiscoveryContext(conn_id);
        if (!ctx)
            return;

        // 完成特征发现的服务与当前正常发现的服务是否一致
        GattService &service = BleModule::discoveredServicesMap[conn_id][ctx->currentServiceIndex];
        if (!std::equal(service.service.uuid.uuid, service.service.uuid.uuid + service.service.uuid.uuid_len,
            param->uuid.uuid)) {
            return;
        }
        // 按声明句柄排序特征值
        if (!service.characteristics.empty()) {
            std::sort(service.characteristics.begin(), service.characteristics.end(),
                [](const BLECharacteristic &a, const BLECharacteristic &b) {
                    return a.character.declare_handle < b.character.declare_handle;
                });
        }

        // 即使失败也继续下一个服务
        ctx->currentServiceIndex++;
        startNext = true;
    }

    if (startNext) {
        BleModule::DiscoverCharacteristicsForService(client_id, conn_id);
    }
}

static void FindNextDescriptor(uint32_t &serviceIndex, uint32_t &charIndex, uint16_t &expectedDescCount,
    std::vector<GattService> &gattServices)
{
    // 查找有描述符的特征值
    uint32_t servicesSize = gattServices.size();
    for (; serviceIndex < servicesSize; serviceIndex++) {
        uint32_t charSize = gattServices[serviceIndex].characteristics.size();
        auto &characteristics = gattServices[serviceIndex].characteristics;
        for (; charIndex < charSize; charIndex++) {
            uint16_t descStart = characteristics[charIndex].character.value_handle + 1;
            uint16_t descEnd = characteristics[charIndex].service_end_hdl;
            if (charIndex != charSize - 1) {
                descEnd =
                    min(descEnd, static_cast<uint16_t>(characteristics[charIndex + 1].character.declare_handle - 1));
            }
            if (descStart <= descEnd) {
                expectedDescCount = descEnd - descStart + 1;
                return;
            }
        }
        charIndex = 0;
    }
}

// 开始描述符发现
void BleModule::StartDescriptorDiscovery(uint8_t client_id, uint16_t conn_id)
{
    std::lock_guard<std::mutex> lock(contextMutex);
    auto ctx = GetValidDiscoveryContext(conn_id);
    if (!ctx) {
        HILOG_ERROR(HILOG_MODULE_ACE, "StartDescriptorDiscovery: context not found");
        return;
    }
    auto& gattServices = discoveredServicesMap[conn_id];

    // 重置描述符计数
    ctx->currentServiceIndex = 0;
    ctx->currentCharIndex = 0;
    ctx->receivedDescCount = 0;

    // 查找第一个特征值
    FindNextDescriptor(ctx->currentServiceIndex, ctx->currentCharIndex, ctx->expectedDescCount, gattServices);

    // 如果没有特征值需要处理，直接完成发现
    if (ctx->currentServiceIndex == gattServices.size()) {
        HILOG_WARN(HILOG_MODULE_ACE, "No characteristics with descriptors, completing discovery");
        DiscoveryData data = {conn_id, ctx->callback, ctx->thisVal};
        PostDiscoveryComplete(data);
        return;
    }

    // 触发获取描述符
    auto& gattCharacteristics = gattServices[ctx->currentServiceIndex].characteristics;
    uint16_t char_decl_handle = gattCharacteristics[ctx->currentCharIndex].character.declare_handle;
    DiscoverDescriptors(client_id, conn_id, char_decl_handle);
}

void BleModule::ProcessNextDescriptorDiscovery(uint8_t client_id, uint16_t conn_id)
{
    uint16_t nextCharHandle = 0;
    bool hasMore = false;

    {
        std::lock_guard<std::mutex> lock(contextMutex);
        auto ctx = GetValidDiscoveryContext(conn_id);
        if (!ctx) {
            HILOG_ERROR(HILOG_MODULE_ACE, "ProcessNextDescriptorDiscovery: context not found");
            return;
        }
        if (ctx->receivedDescCount != ctx->expectedDescCount) {
            HILOG_ERROR(HILOG_MODULE_ACE, "Descriptor discovery incomplete");
        }
        auto & gattServices = discoveredServicesMap[conn_id];
        GattService &service = gattServices[ctx->currentServiceIndex];
        BLECharacteristic &character = service.characteristics[ctx->currentCharIndex];

        // 按描述符句柄排序
        std::sort(character.descriptors.begin(), character.descriptors.end(),
            [](const BLEDescriptor &a, const BLEDescriptor &b) {
                return a.descriptor.descriptor_hdl < b.descriptor.descriptor_hdl;
            });

        ctx->currentCharIndex++;
        // 该服务支持的描述符已完成，查找下一个服务的特征值
        FindNextDescriptor(ctx->currentServiceIndex, ctx->currentCharIndex, ctx->expectedDescCount, gattServices);
        ctx->receivedDescCount = 0;

        // 检查是否有更多特征值
        if (ctx->currentServiceIndex != gattServices.size()) {
            nextCharHandle =
                gattServices[ctx->currentServiceIndex].characteristics[ctx->currentCharIndex].character.declare_handle;
            hasMore = true;
        }
    }

    if (hasMore) {
        // 继续处理下一个特征值
        DiscoverDescriptors(client_id, conn_id, nextCharHandle);
    } else {
        // 所有特征值处理完成，检查描述符计数
        std::lock_guard<std::mutex> lock(contextMutex);
        auto ctx = GetValidDiscoveryContext(conn_id);
        if (!ctx) {
            return;
        }

        DiscoveryData data = {conn_id, ctx->callback, ctx->thisVal};
        PostDiscoveryComplete(data);
    }
}

// 描述符发现
static void DiscoverDescriptors(uint8_t client_id, uint16_t conn_id, uint16_t char_decl_handle)
{
    errcode_t ret = gattc_discovery_descriptor(client_id, conn_id, char_decl_handle);
    if (ret != BLE_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Failed to start descriptor discovery for char_handle=0x%04X: %X", 
                   char_decl_handle, ret);
        // 即使失败也推进到下一个特征值
        BleModule::ProcessNextDescriptorDiscovery(client_id, conn_id);
    }
}

// 描述符发现回调
void GattcDiscoveryDescCbk(uint8_t client_id, uint16_t conn_id,
    gattc_discovery_descriptor_result_t* descriptor, errcode_t status)
{
    if (status != BLE_SUCCESS || !descriptor) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Descriptor discovery failed: %d", status);
        return;
    }

    // 处理描述符结果
    {
        std::lock_guard<std::mutex> lock(BleModule::contextMutex);
        auto ctx = BleModule::GetValidDiscoveryContext(conn_id);
        if (!ctx) {
            return;
        }
        GattService &service = BleModule::discoveredServicesMap[conn_id][ctx->currentServiceIndex];
        BLECharacteristic &character = service.characteristics[ctx->currentCharIndex];

        // 检查特征值与描述符是否匹配
        uint16_t endHandle = ctx->currentCharIndex == service.characteristics.size() - 1 ? character.service_end_hdl :
            service.characteristics[ctx->currentCharIndex + 1].character.declare_handle - 1;
        if (descriptor->descriptor_hdl < character.character.declare_handle ||
            descriptor->descriptor_hdl > endHandle) {
            return;
        }
        // 检查是否已存在相同句柄的描述符
        bool exists = false;
        for (const auto &desc : character.descriptors) {
            if (desc.descriptor.descriptor_hdl == descriptor->descriptor_hdl) {
                exists = true;
                break;
            }
        }

        if (!exists) {
            BLEDescriptor newDesc;
            newDesc.descriptorUuid = GetUUidStr(descriptor->uuid);
            if (memcpy_s(&newDesc.descriptor,
                    sizeof(gattc_discovery_descriptor_result_t),
                    descriptor,
                    sizeof(gattc_discovery_descriptor_result_t)) != EOK) {
                HILOG_ERROR(HILOG_MODULE_ACE, "memcpy_s new descriptor failed!");
                return;
            }
            newDesc.serviceUuid = character.serviceUuid;
            newDesc.characteristicUuid = character.characteristicUuid;

            character.descriptors.push_back(newDesc);
            ctx->receivedDescCount++;
        } else {
            HILOG_WARN(HILOG_MODULE_ACE, "Duplicate descriptor found: handle=0x%04X", descriptor->descriptor_hdl);
        }
    }
}

// 描述符发现完成回调
void GattcDiscoveryDescResponse(uint8_t client_id, uint16_t conn_id, uint16_t character_handle,
    errcode_t status)
{
    // 无论成功与否，都继续处理下一个特征值
    BleModule::ProcessNextDescriptorDiscovery(client_id, conn_id);
}

static MatchType FindMatchingContext(const GattOperationContext &ctx, uint16_t conn_id, const gattc_handle_value_t *read_result) {
    if (ctx.connId != conn_id) {
        return MatchType::NONE;
    }

    if (ctx.type == GattOperationContext::READ_CHARACTERISTIC) {
        BLECharacteristic charFound;
        if (BleModule::FindCharacteristic(ctx.charHandle, ctx.serviceUuid, ctx.charUuid, charFound, conn_id)) {
            if (charFound.character.value_handle == read_result->handle) {
                return MatchType::CHARACTERISTIC;
            }
        }
    } else if (ctx.type == GattOperationContext::READ_DESCRIPTOR) {
        BLEDescriptor descFound;
        if (BleModule::FindDescriptor(ctx.descHandle, ctx.serviceUuid, ctx.charUuid, ctx.descUuid, descFound, conn_id)) {
            if (descFound.descriptor.descriptor_hdl == read_result->handle) {
                return MatchType::DESCRIPTOR;
            }
        }
    }

    return MatchType::NONE;
}

static void HandleMatchedContext(const GattOperationContext &ctx, const gattc_handle_value_t *read_result, gatt_status_t status,
    const BLECharacteristic *charFound, const BLEDescriptor *descFound)
{
    JSIValue result = ConstructJSIResult(read_result, charFound, descFound, ctx.serviceUuid, ctx.charUuid, ctx.descUuid);
    JSIValue err = JSI::CreateNumber((status == 0) ? 0 : BLE_SERVICE_STOP);
    DispatchAsync asyncParameter;
    memset_s(&asyncParameter, sizeof(DispatchAsync), 0, sizeof(DispatchAsync));
    asyncParameter.err = err;
    asyncParameter.result = result;
    asyncParameter.callback = ctx.callback;
    asyncParameter.thisVal = ctx.thisVal;
    asyncParameter.type = "";
    BleDispatchAsyncWork(asyncParameter);
}

void GattcReadCfmCbk(uint8_t client_id, uint16_t conn_id, gattc_handle_value_t *read_result, gatt_status_t status)
{
    if (!read_result) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Read confirmation callback: null read_result");
        return;
    }

    std::lock_guard<std::mutex> lock(BleModule::operationMutex);
    auto &contexts = BleModule::operationContexts;

    for (auto it = contexts.begin(); it != contexts.end();) {
        const auto &ctx = *it;
        MatchType matchType = FindMatchingContext(ctx, conn_id, read_result);

        if (matchType != MatchType::NONE) {
            BLECharacteristic charFound;
            BLEDescriptor descFound;
            bool isDescriptor = (matchType == MatchType::DESCRIPTOR);

            if (matchType == MatchType::CHARACTERISTIC) {
                if (!BleModule::FindCharacteristic(ctx.charHandle, ctx.serviceUuid, ctx.charUuid, charFound, conn_id)) {
                    ++it;
                    continue;
                }
            } else if (matchType == MatchType::DESCRIPTOR) {
                if (!BleModule::FindDescriptor(ctx.descHandle, ctx.serviceUuid, ctx.charUuid, ctx.descUuid, descFound, conn_id)) {
                    ++it;
                    continue;
                }
            }

            HandleMatchedContext(ctx, read_result, status, (matchType == MatchType::CHARACTERISTIC) ? &charFound : nullptr,
                                (matchType == MatchType::DESCRIPTOR) ? &descFound : nullptr);

            it = contexts.erase(it);
            return;
        } else {
            ++it;
        }
    }
}

void GattcReadByUuidRespone(uint8_t client_id, uint16_t conn_id,
    gattc_read_req_by_uuid_param_t *param, errcode_t status)
{
    HILOG_DEBUG(HILOG_MODULE_ACE, "[GATTClient]Read by uuid rsp: status: %u client_id: %u conn_id: %u",
        status, client_id, conn_id);
}

void GattcWriteCfmCbk(uint8_t client_id, uint16_t conn_id, uint16_t handle, gatt_status_t status)
{
    std::lock_guard<std::mutex> lock(BleModule::operationMutex);
    auto& contexts = BleModule::operationContexts;
    // 查找匹配的操作上下文
    for (auto it = contexts.begin(); it != contexts.end(); ) {
        const auto& ctx = *it;
        bool isMatch = false;
        // 仅处理当前连接的操作
        if (ctx.connId != conn_id) {
            ++it;
            continue;
        }

        // 匹配特征值写入
        if (ctx.type == GattOperationContext::WRITE_CHARACTERISTIC) {
            BLECharacteristic charFound;
            memset_s(&charFound, sizeof(BLECharacteristic), 0, sizeof(BLECharacteristic));
            if (BleModule::FindCharacteristic(ctx.charHandle, ctx.serviceUuid, ctx.charUuid, charFound, conn_id)) {
                if (charFound.character.value_handle == handle) {
                    isMatch = true;
                }
            }
        } else if (ctx.type == GattOperationContext::WRITE_DESCRIPTOR) {
            BLEDescriptor descFound;
            memset_s(&descFound, sizeof(BLEDescriptor), 0, sizeof(BLEDescriptor));
            if (BleModule::FindDescriptor(ctx.descHandle, ctx.serviceUuid, ctx.charUuid, ctx.descUuid, descFound, conn_id)) {
                if (descFound.descriptor.descriptor_hdl == handle) {
                    isMatch = true;
                }
            }
        }
        if (isMatch) {
            // 回调返回
            JSIValue err = JSI::CreateNumber((status == 0) ? 0 : BLE_SERVICE_STOP);
            DispatchAsync asyncParameter;
            memset_s(&asyncParameter, sizeof(DispatchAsync), 0, sizeof(DispatchAsync));
            asyncParameter.err = err;
            asyncParameter.result = JSI::CreateUndefined();
            asyncParameter.callback = ctx.callback;
            asyncParameter.thisVal = ctx.thisVal;
            asyncParameter.type = "";
            BleDispatchAsyncWork(asyncParameter);
            it = contexts.erase(it);
            return;
        } else {
            ++it;
        }
    }
}

// MTU协商接口触发gattc_exchange_mtu_req mtu(协议范围23-517)
void GattcMtuChangedCbk(uint8_t client_id, uint16_t conn_id, uint16_t mtu_size, errcode_t status)
{
    if (status != 0) {
        return;
    }
    std::lock_guard<std::mutex> lock(BleModule::connectMapMutex);
    for (auto it = BleModule::deviceMap.begin(); it != BleModule::deviceMap.end(); it++) {
        if (it->second.connId == conn_id) {
            it->second.mtu = mtu_size;
            break;
        }
    }
}

void BLECharacteristicCallBackFunc(uint16_t connId, BLECharacteristic &characteristicArgs)
{
    DispatchAsync asyncParameter;
    memset_s(&asyncParameter, sizeof(DispatchAsync), 0, sizeof(DispatchAsync));
    asyncParameter.err = JSI::CreateUndefined();
    asyncParameter.result = JSI::CreateUndefined();
    asyncParameter.callback = JSI::CreateUndefined();
    asyncParameter.thisVal = JSI::CreateUndefined();
    asyncParameter.type = "BLECharacteristicChange";
    asyncParameter.connId = connId;
    asyncParameter.characteristicArgs = characteristicArgs;
    BleDispatchAsyncWork(asyncParameter);
}

void GattcNotificationdCbk(uint8_t client_id, uint16_t connId, gattc_handle_value_t *data, errcode_t status)
{
    if (!data) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Notification callback: null data");
        return;
    }

    std::lock_guard<std::mutex> lock(BleModule::contextMutex);

    // 从discoveredServicesMap获取服务数据
    auto servicesIt = BleModule::discoveredServicesMap.find(connId);
    if (servicesIt == BleModule::discoveredServicesMap.end()) {
        HILOG_ERROR(HILOG_MODULE_ACE, "No services found for notification: conn=%u", connId);
        return;
    }
    // 查找匹配的特征值
    bool found = false;
    BLECharacteristic characteristic;
    for (auto &service : servicesIt->second) {
        for (auto &chara : service.characteristics) {
            if (chara.character.value_handle == data->handle) {
                // 更新特征值数据
                chara.characteristicValue.assign(data->data, data->data + data->data_len);
                characteristic = chara;
                found = true;
                break;
            }
        }
        if (found)
            break;
    }
    if (found) {
        // 触发回调
        BLECharacteristicCallBackFunc(connId, characteristic);
    }
}

void GattcIndicationCbk(uint8_t client_id, uint16_t connId, gattc_handle_value_t *data,
    errcode_t status)
{
    if (!data) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Indication callback: null data");
        return;
    }

    std::lock_guard<std::mutex> lock(BleModule::contextMutex);

    // 从discoveredServicesMap获取服务数据
    auto servicesIt = BleModule::discoveredServicesMap.find(connId);
    if (servicesIt == BleModule::discoveredServicesMap.end()) {
        HILOG_ERROR(HILOG_MODULE_ACE, "No services found for indication: conn=%u", connId);
        return;
    }

    // 查找匹配的特征值
    bool found = false;
    BLECharacteristic characteristic;

    for (auto& service : servicesIt->second) {
        for (auto& chara : service.characteristics) {
            if (chara.character.value_handle == data->handle) {
                // 更新特征值数据
                chara.characteristicValue.assign(data->data, data->data + data->data_len);
                characteristic = chara;
                found = true;
                break;
            }
        }
        if (found) break;
    }
    if (found) {
        // 触发回调
        BLECharacteristicCallBackFunc(connId, characteristic);
    }
}
}  // namespace ACELite
}  // namespace OHOS

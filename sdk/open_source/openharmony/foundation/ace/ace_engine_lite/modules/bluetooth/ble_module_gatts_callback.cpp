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
#include "ble_module_common.h"
#include "bts_gatt_server.h"
#include "ace_log.h"

namespace OHOS {
namespace ACELite {
// 启动服务回调
void GattsStartServiceCbk(uint8_t serverId, uint16_t handle, errcode_t status)
{
    std::lock_guard<std::mutex> lock(BleModule::serviceBuildContextsMutex);
    auto it = BleModule::g_serviceBuildContexts.find(handle);
    if (it == BleModule::g_serviceBuildContexts.end()) {
        HILOG_ERROR(HILOG_MODULE_ACE, "GattsStartServiceCbk: context not found for handle %X", handle);
        return;
    }
    ServiceBuildContext context = it->second;
    BleModule::g_serviceBuildContexts.erase(it);
    // 将 GattService 转换为 JSI 对象
    JSIValue jsiResultService = JSI::CreateUndefined();
    if (status == BLE_SUCCESS && context.resultService != nullptr) {
        jsiResultService = CreateJsiGattService(*context.resultService);
    }

    // 准备异步参数
    DispatchAsync asyncParameter;
    memset_s(&asyncParameter, sizeof(DispatchAsync), 0, sizeof(DispatchAsync));
    asyncParameter.err = JSI::CreateNumber(status);
    asyncParameter.result = jsiResultService;
    asyncParameter.callback = context.callback;
    asyncParameter.thisVal = context.thisVal;
    asyncParameter.type = "";

    // 派发异步工作
    BleDispatchAsyncWork(asyncParameter);

    if (context.originalService) {
        delete context.originalService;
        context.originalService = nullptr;
    }
    if (context.resultService) {
        delete context.resultService;
        context.resultService = nullptr;
    }
}

// 停止服务回调，支持Destroy流程
void GattsStopServiceCbk(uint8_t serverId, uint16_t handle, errcode_t status)
{
    HILOG_DEBUG(HILOG_MODULE_ACE, "GattsStopServiceCbk: serverId=%X, handle=%X, status=%X", 
                serverId, handle, status);
    
    std::lock_guard<std::mutex> lock(BleModule::serviceCleanMutex);
    auto it = BleModule::g_removeServiceContexts.find(handle);
    if (it == BleModule::g_removeServiceContexts.end()) {
        HILOG_ERROR(HILOG_MODULE_ACE, "GattsStopServiceCbk: context not found for handle %X", handle);
        return;
    }

    RemoveServiceContext context = it->second;
    
    // 从上下文中移除
    BleModule::g_removeServiceContexts.erase(it);
    
    if (status != BLE_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "GattsStopServiceCbk: stop service failed: %X", status);
        return;
    }

    HILOG_INFO(HILOG_MODULE_ACE, "GattsStopServiceCbk: service stopped successfully, handle: %X", handle);
    
    // 删除服务前清理特征值映射
    RemoveAllCharacteristicHandlesByService(context.serviceUuid);
    
    // 清理特征值和描述符映射
    for (auto charIt = BleModule::g_handleToCharacteristicMap.begin(); charIt != BleModule::g_handleToCharacteristicMap.end();) {
        if (charIt->second.serviceUuid == context.serviceUuid) {
            charIt = BleModule::g_handleToCharacteristicMap.erase(charIt);
        } else {
            ++charIt;
        }
    }
    
    for (auto descIt = BleModule::g_handleToDescriptorMap.begin(); descIt != BleModule::g_handleToDescriptorMap.end();) {
        if (descIt->second.serviceUuid == context.serviceUuid) {
            descIt = BleModule::g_handleToDescriptorMap.erase(descIt);
        } else {
            ++descIt;
        }
    }
    
    // 停止成功，同步删除服务
    errcode_t deleteRet = gatts_delete_service(BleModule::serverId, handle);
    if (deleteRet != BLE_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "GattsStopServiceCbk: gatts_delete_service failed: %X", deleteRet);
    } else {
        HILOG_INFO(HILOG_MODULE_ACE, "GattsStopServiceCbk: service deleted successfully, handle: %X", handle);
    }
    JSI::ReleaseValue(context.callback);
    JSI::ReleaseValue(context.thisVal);
}

// 创建CharacteristicReadRequest JSI对象
JSIValue CreateJsiCharacteristicReadRequest(const ReadWriteRequestContext& context)
{
    JSIValue jsiRequest = JSI::CreateObject();

    JSI::SetStringProperty(jsiRequest, "deviceId", context.deviceId.c_str());
    JSI::SetNumberProperty(jsiRequest, "transId", context.transId);
    JSI::SetNumberProperty(jsiRequest, "offset", context.offset);
    JSI::SetStringProperty(jsiRequest, "characteristicUuid", context.characteristicUuid.c_str());
    JSI::SetStringProperty(jsiRequest, "serviceUuid", context.serviceUuid.c_str());

    return jsiRequest;
}

// 创建CharacteristicWriteRequest JSI对象
JSIValue CreateJsiCharacteristicWriteRequest(const ReadWriteRequestContext& context)
{
    JSIValue jsiRequest = JSI::CreateObject();

    JSI::SetStringProperty(jsiRequest, "deviceId", context.deviceId.c_str());
    JSI::SetNumberProperty(jsiRequest, "transId", context.transId);
    JSI::SetNumberProperty(jsiRequest, "offset", context.offset);
    JSI::SetBooleanProperty(jsiRequest, "isPrepared", context.isPrepared);
    JSI::SetBooleanProperty(jsiRequest, "needRsp", context.needRsp);
    JSI::SetStringProperty(jsiRequest, "characteristicUuid", context.characteristicUuid.c_str());
    JSI::SetStringProperty(jsiRequest, "serviceUuid", context.serviceUuid.c_str());

    // 设置value数据
    if (!context.value.empty()) {
        uint8_t *valuePtr = nullptr;
        uint32_t valueLen = context.value.size();
        JSIValue jsiValue = JSI::CreateArrayBuffer(valueLen, valuePtr);
        if (valueLen > 0 && valuePtr == nullptr) {
            JSI::ReleaseValue(jsiValue);
            JSI::ReleaseValue(jsiRequest);
            HILOG_ERROR(HILOG_MODULE_ACE, "CreateArrayBuffer for WriteRequest Value failed!");
            return JSI::CreateUndefined();
        }
        if (memcpy_s(valuePtr, valueLen, context.value.data(), context.value.size()) != EOK) {
            JSI::ReleaseValue(jsiValue);
            HILOG_ERROR(HILOG_MODULE_ACE, "memcpy_s WriteRequest Value failed!");
            return JSI::CreateUndefined();
        }
        JSI::SetNamedProperty(jsiRequest, "value", jsiValue);
        JSI::ReleaseValue(jsiValue);
    }
    return jsiRequest;
}

// 创建DescriptorReadRequest JSI对象
JSIValue CreateJsiDescriptorReadRequest(const ReadWriteRequestContext& context)
{
    JSIValue jsiRequest = JSI::CreateObject();

    JSI::SetStringProperty(jsiRequest, "deviceId", context.deviceId.c_str());
    JSI::SetNumberProperty(jsiRequest, "transId", context.transId);
    JSI::SetNumberProperty(jsiRequest, "offset", context.offset);
    JSI::SetStringProperty(jsiRequest, "descriptorUuid", context.descriptorUuid.c_str());
    JSI::SetStringProperty(jsiRequest, "characteristicUuid", context.characteristicUuid.c_str());
    JSI::SetStringProperty(jsiRequest, "serviceUuid", context.serviceUuid.c_str());

    return jsiRequest;
}

// 创建DescriptorWriteRequest JSI对象
JSIValue CreateJsiDescriptorWriteRequest(const ReadWriteRequestContext& context) {
    JSIValue jsiRequest = JSI::CreateObject();

    JSI::SetStringProperty(jsiRequest, "deviceId", context.deviceId.c_str());
    JSI::SetNumberProperty(jsiRequest, "transId", context.transId);
    JSI::SetNumberProperty(jsiRequest, "offset", context.offset);
    JSI::SetBooleanProperty(jsiRequest, "isPrepared", context.isPrepared);
    JSI::SetBooleanProperty(jsiRequest, "needRsp", context.needRsp);
    JSI::SetStringProperty(jsiRequest, "descriptorUuid", context.descriptorUuid.c_str());
    JSI::SetStringProperty(jsiRequest, "characteristicUuid", context.characteristicUuid.c_str());
    JSI::SetStringProperty(jsiRequest, "serviceUuid", context.serviceUuid.c_str());

    // 设置value数据
    if (!context.value.empty()) {
        uint8_t *valuePtr = nullptr;
        JSIValue jsiValue = JSI::CreateArrayBuffer(context.value.size(), valuePtr);
        if (valuePtr != nullptr) {
            if (memcpy_s(valuePtr, context.value.size(), context.value.data(), context.value.size()) == EOK) {
                JSI::SetNamedProperty(jsiRequest, "value", jsiValue);
            }
        }
        JSI::ReleaseValue(jsiValue);
    }

    return jsiRequest;
}

// 通过句柄查找特征值信息
bool FindCharacteristicByHandle(uint16_t handle, CharacteristicInfo& info)
{
    auto it = BleModule::g_handleToCharacteristicMap.find(handle);
    if (it != BleModule::g_handleToCharacteristicMap.end()) {
        info = it->second;
        return true;
    }
    return false;
}

// 通过句柄查找描述符信息
bool FindDescriptorByHandle(uint16_t handle, DescriptorInfo& info)
{
    auto it = BleModule::g_handleToDescriptorMap.find(handle);
    if (it != BleModule::g_handleToDescriptorMap.end()) {
        info = it->second;
        return true;
    }
    return false;
}

std::string GetDeviceIdByConnId(uint16_t connId)
{
    std::lock_guard<std::mutex> lock(BleModule::connectMapMutex);
    // 遍历 connectMap，查找值为 connId 的键（deviceId）
    for (const auto& entry : BleModule::deviceMap) {
        if (entry.second.connId == connId) {
            return entry.first;  // 找到匹配的 deviceId，返回
        }
    }
    // 未找到对应 connId 的 deviceId
    return "";
}

// 协议栈读请求回调
void GattsReadRequestCbk(uint8_t serverId, uint16_t connId, gatts_req_read_cb_t *readCbPara, errcode_t status)
{
    if (readCbPara == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "GattsReadRequestCbk: readCbPara is null");
        return;
    }

    // 获取设备ID
    std::string deviceId = GetDeviceIdByConnId(connId);
    if (deviceId.empty()) {
        HILOG_ERROR(HILOG_MODULE_ACE, "GattsReadRequestCbk: device not found for connId %X", connId);
        return;
    }
    ReadWriteRequestContext context;
    context.deviceId = deviceId;
    context.transId = readCbPara->request_id;
    context.offset = readCbPara->offset;
    context.needRsp = readCbPara->need_rsp;

    HILOG_INFO(HILOG_MODULE_ACE, "GattsReadRequestCbk: connId %X, handle %X, transId %X, offset %X", 
               connId, readCbPara->handle, context.transId, context.offset);

    // 根据句柄判断是特征值读还是描述符读
    CharacteristicInfo charaInfo;
    DescriptorInfo descInfo;
    
    if (FindCharacteristicByHandle(readCbPara->handle, charaInfo)) {
        // 特征值读请求
        context.serviceUuid = charaInfo.serviceUuid;
        context.characteristicUuid = charaInfo.characteristicUuid;

        DispatchAsync asyncParameter;
        memset_s(&asyncParameter, sizeof(DispatchAsync), 0, sizeof(DispatchAsync));
        asyncParameter.err = JSI::CreateUndefined();
        asyncParameter.result = CreateJsiCharacteristicReadRequest(context);
        asyncParameter.callback = JSI::CreateUndefined();
        asyncParameter.thisVal = JSI::CreateUndefined();
        asyncParameter.type = "characteristicRead";

        BleDispatchAsyncWork(asyncParameter);

        HILOG_INFO(HILOG_MODULE_ACE, "GattsReadRequestCbk: characteristic read, service %s, chara %s", 
                   context.serviceUuid.c_str(), context.characteristicUuid.c_str());

    } else if (FindDescriptorByHandle(readCbPara->handle, descInfo)) {
        // 描述符读请求
        context.serviceUuid = descInfo.serviceUuid;
        context.characteristicUuid = descInfo.characteristicUuid;
        context.descriptorUuid = descInfo.descriptorUuid;

        DispatchAsync asyncParameter;
        memset_s(&asyncParameter, sizeof(DispatchAsync), 0, sizeof(DispatchAsync));
        asyncParameter.err = JSI::CreateUndefined();
        asyncParameter.result = CreateJsiDescriptorReadRequest(context);
        asyncParameter.callback = JSI::CreateUndefined();
        asyncParameter.thisVal = JSI::CreateUndefined();
        asyncParameter.type = "descriptorRead";

        BleDispatchAsyncWork(asyncParameter);

        HILOG_INFO(HILOG_MODULE_ACE, "GattsReadRequestCbk: descriptor read, service %s, chara %s, desc %s", 
                   context.serviceUuid.c_str(), context.characteristicUuid.c_str(), context.descriptorUuid.c_str());
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE, "GattsReadRequestCbk: unknown handle %X", readCbPara->handle);
    }
}

// 协议栈写请求回调
void GattsWriteRequestCbk(uint8_t serverId, uint16_t connId, gatts_req_write_cb_t *writeCbPara, errcode_t status)
{
    if (writeCbPara == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "GattsWriteRequestCbk: writeCbPara is null");
        return;
    }

    // 获取设备ID
    std::string deviceId = GetDeviceIdByConnId(connId);
    if (deviceId.empty()) {
        HILOG_ERROR(HILOG_MODULE_ACE, "GattsWriteRequestCbk: device not found for connId %X", connId);
        return;
    }

    ReadWriteRequestContext context;
    context.deviceId = deviceId;
    context.transId = writeCbPara->request_id;
    context.offset = writeCbPara->offset;
    context.needRsp = writeCbPara->need_rsp;
    context.isPrepared = writeCbPara->is_prep;
    // 复制写数据
    if (writeCbPara->length > 0 && writeCbPara->value != nullptr) {
        context.value.assign(writeCbPara->value, writeCbPara->value + writeCbPara->length);
    }

    HILOG_INFO(HILOG_MODULE_ACE, "GattsWriteRequestCbk: connId %X, handle %X, transId %X, offset %X, length %X", 
               connId, writeCbPara->handle, context.transId, context.offset, writeCbPara->length);

    // 根据句柄判断是特征值写还是描述符写
    CharacteristicInfo charaInfo;
    DescriptorInfo descInfo;

    if (FindCharacteristicByHandle(writeCbPara->handle, charaInfo)) {
        // 特征值写请求
        context.serviceUuid = charaInfo.serviceUuid;
        context.characteristicUuid = charaInfo.characteristicUuid;

        DispatchAsync asyncParameter;
        memset_s(&asyncParameter, sizeof(DispatchAsync), 0, sizeof(DispatchAsync));
        asyncParameter.err = JSI::CreateUndefined();
        asyncParameter.result = CreateJsiCharacteristicWriteRequest(context);
        asyncParameter.callback = JSI::CreateUndefined();
        asyncParameter.thisVal = JSI::CreateUndefined();
        asyncParameter.type = "CharacteristicWrite";

        BleDispatchAsyncWork(asyncParameter);

        HILOG_INFO(HILOG_MODULE_ACE, "GattsWriteRequestCbk: characteristic write, service %s, chara %s, needRsp %d", 
                   context.serviceUuid.c_str(), context.characteristicUuid.c_str(), context.needRsp);
    } else if (FindDescriptorByHandle(writeCbPara->handle, descInfo)) {
        // 描述符写请求
        context.serviceUuid = descInfo.serviceUuid;
        context.characteristicUuid = descInfo.characteristicUuid;
        context.descriptorUuid = descInfo.descriptorUuid;

        DispatchAsync asyncParameter;
        memset_s(&asyncParameter, sizeof(DispatchAsync), 0, sizeof(DispatchAsync));
        asyncParameter.err = JSI::CreateUndefined();
        asyncParameter.result = CreateJsiDescriptorWriteRequest(context);
        asyncParameter.callback = JSI::CreateUndefined();
        asyncParameter.thisVal = JSI::CreateUndefined();
        asyncParameter.type = "DescriptorWrite";

        BleDispatchAsyncWork(asyncParameter);

        HILOG_INFO(HILOG_MODULE_ACE, "GattsWriteRequestCbk: descriptor write, service %s, chara %s, desc %s, needRsp %d", 
                   context.serviceUuid.c_str(), context.characteristicUuid.c_str(), context.descriptorUuid.c_str(), context.needRsp);
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE, "GattsWriteRequestCbk: unknown handle %X", writeCbPara->handle);
    }
}

void GattsMtuChangedCbk(uint8_t serverId, uint16_t connId, uint16_t mtu_size, errcode_t status)
{
    if (status == 0) {
        DeviceType deviceType = BLE_CLIENT;
        {
            std::lock_guard<std::mutex> lock(BleModule::connectMapMutex);
            for (auto it = BleModule::deviceMap.begin(); it != BleModule::deviceMap.end(); it++) {
                if (it->second.connId == connId) {
                    it->second.mtu = mtu_size;
                    deviceType = it->second.deviceType;
                    break;
                }
            }
        }

        if (deviceType == BLE_CLIENT) {
            DispatchAsync asyncParameter;
            memset_s(&asyncParameter, sizeof(DispatchAsync), 0, sizeof(DispatchAsync));
            asyncParameter.err = JSI::CreateUndefined();
            asyncParameter.result = JSI::CreateNumber(mtu_size);
            asyncParameter.callback = JSI::CreateUndefined();
            asyncParameter.thisVal = JSI::CreateUndefined();
            asyncParameter.type = "BLEMtuChange";
            BleDispatchAsyncWork(asyncParameter);
        }
    }
}
}  // namespace ACELite
}  // namespace OHOS

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

JSIValue BleModule::GetServices(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    if (argsSize < ARGC_ONE || argsSize > ARGC_TWO || !JSI::ValueIsFunction(args[0])) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Invalid parameter!");
        return JSI::CreateUndefined(); // 参数错误
    }
    int ret = BLE_SUCCESS;

    uint16_t connId = 0;
    std::string deviceId;
    if (argsSize == ARGC_TWO) {
        deviceId = ParseDeviceId(args[1]);
    } else {
        deviceId = GetFirstServerDeviceId();
    }
    {
        std::lock_guard<std::mutex> lock(connectMapMutex);
        if (deviceId.empty() || deviceMap.find(deviceId) == deviceMap.end()) {
            HILOG_ERROR(HILOG_MODULE_ACE, "GetServices failed: not connected to device");
            ret = BLE_OPRATION_FAILED; // 未连接错误码
        } else {
            auto connIt = deviceMap.find(deviceId);
            if (connIt->second.connState != STATE_CONNECTED) {
                HILOG_ERROR(HILOG_MODULE_ACE, "GetServices failed: no connection for device %s", deviceId.c_str());
                ret = BLE_OPRATION_FAILED;
            } else {
                connId = connIt->second.connId;
            }
        }
    }
    if (ret == BLE_SUCCESS) {
        // 初始化上下文（加锁保护）
        {
            std::lock_guard<std::mutex> lock(contextMutex);
            // 清理旧上下文
            if (discoveryContexts.find(connId) != discoveryContexts.end()) {
                auto &oldCtx = discoveryContexts[connId];
                JSI::ReleaseValue(oldCtx.callback);
                JSI::ReleaseValue(oldCtx.thisVal);
                discoveryContexts.erase(connId);
            }

            // 创建新发现上下文
            auto &ctx = discoveryContexts[connId];
            ctx.connId = connId;
            ctx.currentServiceIndex = 0;

            // 保存用户回调
            ctx.callback = JSI::AcquireValue(args[0]);  // 增加引用计数
            ctx.thisVal = JSI::AcquireValue(thisVal);   // 增加引用计数
        }

        // 启动服务发现
        bt_uuid_t uuid = {0};  // 空UUID表示发现所有服务
        HILOG_INFO(HILOG_MODULE_ACE, "gattc_discovery_service clientId:%d, connId=%d", BleModule::clientId, connId);
        ret = gattc_discovery_service(clientId, connId, &uuid);
        if (ret != BLE_SUCCESS) {
            HILOG_ERROR(HILOG_MODULE_ACE, "Failed to start service discovery: 0x%X", ret);
            ret = BLE_READ_FORBID;  // 服务发现启动失败错误码

            // 清理上下文
            std::lock_guard<std::mutex> lock(contextMutex);
            if (discoveryContexts.find(connId) != discoveryContexts.end()) {
                auto &ctx = discoveryContexts[connId];
                JSI::ReleaseValue(ctx.callback);
                JSI::ReleaseValue(ctx.thisVal);
                discoveryContexts.erase(connId);
            }
        }
    }

    // 处理错误回调
    if (ret != BLE_SUCCESS) {
        JSIValue callback = JSI::AcquireValue(args[0]);
        JSIValue err = JSI::CreateNumber(ret);
        JSIValue argv[ARGC_TWO] = {err, JSI::CreateUndefined()};
        JSI::CallFunction(callback, thisVal, argv, ARGC_TWO);
        JSI::ReleaseValueList(err, callback);
    }
    return JSI::CreateUndefined();
}

JSIValue BleModule::ReadCharacteristicValue(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    // 变量声明
    errcode_t ret = 0;
    BLECharacteristic targetChar;
    uint16_t connId = 0;
    BLECharacteristic bleCharacteristicArgs;
    memset_s(&bleCharacteristicArgs, sizeof(BLECharacteristic), 0, sizeof(BLECharacteristic));
    GattOperationContext context;
    memset_s(&context, sizeof(GattOperationContext), 0, sizeof(GattOperationContext));
    uint16_t handle = 0;
    bool contextCreated = false;  // 跟踪上下文是否已创建
    std::string deviceId;

    // 参数校验
    if (argsSize < ARGC_TWO || argsSize > ARGC_THREE || !JSI::ValueIsFunction(args[1])) {
        ret = BLE_INVALID_PARAMETER; // 无效参数
        HILOG_ERROR(HILOG_MODULE_ACE, "ReadCharacteristic: invalid parameter!");
        goto exit;
    }
    // 解析入参
    GetCharacteristic(args, bleCharacteristicArgs, true);
    if (argsSize == ARGC_THREE) {
        deviceId = ParseDeviceId(args[ARGC_TWO]);
    } else {
        deviceId = GetFirstServerDeviceId();
    }
    {
        std::lock_guard<std::mutex> lock(connectMapMutex);
        // 检查设备连接状态
        if (deviceId.empty() || deviceMap.find(deviceId) == deviceMap.end()) {
            ret = BLE_UNSUPPORT;  // 未连接
            HILOG_ERROR(HILOG_MODULE_ACE, "ReadCharacteristic: device not connected");
            goto exit;
        }
        connId = deviceMap[deviceId].connId;
    }

    // 校验特征值是否存在
    if (!FindCharacteristic(bleCharacteristicArgs.character.declare_handle, bleCharacteristicArgs.serviceUuid,
                           bleCharacteristicArgs.characteristicUuid,targetChar, connId)) {
        ret = BLE_UNSUPPORT; // 特征值不存在
        HILOG_ERROR(HILOG_MODULE_ACE, "ReadCharacteristic: Characteristic %s(service %s) not found",
                   bleCharacteristicArgs.characteristicUuid.c_str(),
                   bleCharacteristicArgs.serviceUuid.c_str());
        goto exit;
    }

    // 检查特征值是否支持读取
    if (!targetChar.properties.read) {
        ret = BLE_READ_FORBID; // 无权限
        HILOG_ERROR(HILOG_MODULE_ACE, "ReadCharacteristic: characteristic does not support read");
        goto exit;
    }

    // 从特征值获取句柄
    handle = targetChar.character.value_handle;
    // 保存操作上下文（用于回调匹配）
    context.type = GattOperationContext::READ_CHARACTERISTIC;
    context.connId = connId;
    context.serviceUuid = bleCharacteristicArgs.serviceUuid;
    context.charUuid = bleCharacteristicArgs.characteristicUuid;
    context.charHandle = bleCharacteristicArgs.character.declare_handle;
    context.callback = JSI::AcquireValue(args[1]);  // 仅此一次Acquire
    context.thisVal = JSI::AcquireValue(thisVal);
    contextCreated = true;
    {
        std::lock_guard<std::mutex> lock(operationMutex);
        operationContexts.push_back(context);
    }
    // 发起读取请求，触发GattcReadCfmCbk
    ret = gattc_read_req_by_handle(clientId, connId, handle);
    if (ret != BLE_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "ReadCharacteristic: gattc read failed! %X", ret);
        if (contextCreated) {
            // 清理上下文
            std::lock_guard<std::mutex> lock(operationMutex);
            for (auto it = operationContexts.begin(); it != operationContexts.end(); ++it) {
                if (it->type == GattOperationContext::READ_CHARACTERISTIC &&
                    CompareHexStrings(it->serviceUuid, context.serviceUuid) &&
                    CompareHexStrings(it->charUuid, context.charUuid)) {
                    JSI::ReleaseValueList(it->callback, it->thisVal);
                    operationContexts.erase(it);
                    contextCreated = false;  // 标记已清理
                    break;
                }
            }
        }
        ret = BLE_OPRATION_FAILED;  // 操作失败
        goto exit;
    }

exit:
    // 错误处理：统一释放资源并回调
    if (ret != BLE_SUCCESS) {
        JSIValue callbackObj = JSI::AcquireValue(args[1]);
        if (JSI::ValueIsFunction(callbackObj)) {
            JSIValue err = JSI::CreateNumber(ret);
            JSIValue resultLocal = JSI::CreateUndefined();
            JSIValue argv[ARGC_TWO] = {err, resultLocal};
            JSI::CallFunction(callbackObj, thisVal, argv, ARGC_TWO);
            JSI::ReleaseValueList(err, resultLocal);
        }
        JSI::ReleaseValue(callbackObj);
    }

    return JSI::CreateUndefined();
}

// 检查连接状态函数
static errcode_t CheckConnectionStatus(const std::string& deviceId, uint16_t& connId)
{
    {
        std::lock_guard<std::mutex> lock(BleModule::connectMapMutex);
        if (BleModule::deviceMap.find(deviceId) == BleModule::deviceMap.end()) {
            HILOG_ERROR(HILOG_MODULE_ACE, "WriteCharacteristic: device not connected");
            return BLE_UNSUPPORT;  // 未连接
        }
        connId = BleModule::deviceMap[deviceId].connId;
    }
    return BLE_SUCCESS;
}

// 查找特征值函数
static errcode_t FindTargetCharacteristic(const BLECharacteristic& bleCharacteristicArgs, 
                                         uint16_t connId, BLECharacteristic& targetChar)
{
    if (!BleModule::FindCharacteristic(bleCharacteristicArgs.character.declare_handle,
            bleCharacteristicArgs.serviceUuid, bleCharacteristicArgs.characteristicUuid, 
            targetChar, connId)) {
        HILOG_ERROR(HILOG_MODULE_ACE,
            "WriteCharacteristic: Characteristic %s (service %s) not found",
            bleCharacteristicArgs.characteristicUuid.c_str(),
            bleCharacteristicArgs.serviceUuid.c_str());
        return BLE_UNSUPPORT; // 特征值不存在
    }
    return BLE_SUCCESS;
}

// 检查写入权限函数
static errcode_t CheckWritePermission(const BLECharacteristic& targetChar, GattWriteType writeType)
{
    if ((writeType == WRITE && !targetChar.properties.write) ||
        (writeType == WRITE_NO_RESPONSE && !targetChar.properties.writeNoResponse)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "WriteCharacteristic: write type not supported");
        return BLE_OPRATION_FAILED; // 无权限
    }
    return BLE_SUCCESS;
}

// 准备写入数据函数
static void PrepareWriteData(const BLECharacteristic& bleCharacteristicArgs, 
                            const BLECharacteristic& targetChar, gattc_handle_value_t& attValue)
{
    attValue.handle = targetChar.character.value_handle;
    attValue.data_len = bleCharacteristicArgs.characteristicValue.size();
    attValue.data = const_cast<uint8_t *>(bleCharacteristicArgs.characteristicValue.data());
}

// 创建上下文函数
static errcode_t CreateWriteContext(GattWriteType writeType, uint16_t connId, 
                                   const BLECharacteristic& bleCharacteristicArgs,
                                   const JSIValue thisVal, const JSIValue* args,
                                   GattOperationContext& context, bool& contextCreated)
{
    if (writeType == WRITE) {
        context.type = GattOperationContext::WRITE_CHARACTERISTIC;
        context.connId = connId;
        context.serviceUuid = bleCharacteristicArgs.serviceUuid;
        context.charUuid = bleCharacteristicArgs.characteristicUuid;
        context.charHandle = bleCharacteristicArgs.character.declare_handle;
        context.writeType = writeType;
        context.callback = JSI::AcquireValue(args[ARGC_TWO]);
        context.thisVal = JSI::AcquireValue(thisVal);
        contextCreated = true;

        std::lock_guard<std::mutex> lock(BleModule::operationMutex);
        BleModule::operationContexts.push_back(context);
    }
    return BLE_SUCCESS;
}

// 执行写入操作函数
static errcode_t PerformWriteOperation(GattWriteType writeType, uint16_t connId, 
                                      gattc_handle_value_t& attValue)
{
    return (writeType == WRITE) ? gattc_write_req(BleModule::clientId, connId, &attValue)
                                : gattc_write_cmd(BleModule::clientId, connId, &attValue);
}

// 清理上下文函数
static void CleanupWriteContext(bool contextCreated, GattOperationContext& context)
{
    if (contextCreated) {
        std::lock_guard<std::mutex> lock(BleModule::operationMutex);
        for (auto it = BleModule::operationContexts.begin();
            it != BleModule::operationContexts.end(); ++it) {
            if (it->type == GattOperationContext::WRITE_CHARACTERISTIC && 
                CompareHexStrings(it->serviceUuid, context.serviceUuid) &&
                CompareHexStrings(it->charUuid, context.charUuid)) {
                JSI::ReleaseValueList(it->callback, it->thisVal);
                BleModule::operationContexts.erase(it);
                break;
            }
        }
    }
}

// 执行回调函数
static void ExecuteWriteCallback(bool needContext, const JSIValue thisVal, 
                                const JSIValue* args, errcode_t ret)
{
    if (!needContext) {
        JSIValue callbackObj = JSI::AcquireValue(args[ARGC_TWO]);
        if (JSI::ValueIsFunction(callbackObj)) {
            JSIValue err = JSI::CreateNumber(ret);
            JSI::CallFunction(callbackObj, thisVal, &err, 1);
            JSI::ReleaseValue(err);
        }
        JSI::ReleaseValue(callbackObj);
    }
}

// 主函数
JSIValue BleModule::WriteCharacteristicValue(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    errcode_t ret = BLE_SUCCESS;
    uint16_t connId = 0;
    BLECharacteristic bleCharacteristicArgs;
    GattWriteType writeType = WRITE_NO_RESPONSE;
    BLECharacteristic targetChar;
    gattc_handle_value_t attValue = {0};
    GattOperationContext context;
    memset_s(&context, sizeof(GattOperationContext), 0, sizeof(GattOperationContext));
    bool contextCreated = false;
    bool needContext = false;
    std::string deviceId;

    // 参数校验
    if (!ValidateWriteParameters(argsSize, args)) {
        ret = BLE_INVALID_PARAMETER;
        goto exit;
    }

    // 解析参数
    if (argsSize == ARGC_FOUR) {
        deviceId = ParseDeviceId(args[ARGC_THREE]);
    } else {
        deviceId = GetFirstServerDeviceId();
    }
    if (deviceId.empty()) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Invalid parameter!");
        ret = BLE_INVALID_PARAMETER;
        goto exit;
    }
    ret = ParseWriteArguments(args, bleCharacteristicArgs, writeType);
    if (ret != BLE_SUCCESS) {
        goto exit;
    }

    // 检查连接状态
    ret = CheckConnectionStatus(deviceId, connId);
    if (ret != BLE_SUCCESS) {
        goto exit;
    }

    // 查找特征值
    ret = FindTargetCharacteristic(bleCharacteristicArgs, connId, targetChar);
    if (ret != BLE_SUCCESS) {
        goto exit;
    }

    // 检查写入权限
    ret = CheckWritePermission(targetChar, writeType);
    if (ret != BLE_SUCCESS) {
        goto exit;
    }

    // 准备写入数据
    PrepareWriteData(bleCharacteristicArgs, targetChar, attValue);

    // 创建上下文
    needContext = (writeType == WRITE);
    ret = CreateWriteContext(writeType, connId, bleCharacteristicArgs, thisVal, args, context, contextCreated);
    if (ret != BLE_SUCCESS) {
        goto exit;
    }

    // 执行写入操作
    ret = PerformWriteOperation(writeType, connId, attValue);
    if (ret != BLE_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "WriteCharacteristic failed: %X", ret);
        if (contextCreated) {
            CleanupWriteContext(contextCreated, context);
        }
        ret = BLE_OPRATION_FAILED;
        goto exit;
    }

    // 无响应写入直接回调成功
    ExecuteWriteCallback(needContext, thisVal, args, ret);

exit:
    // 错误处理
    if (ret != BLE_SUCCESS && !needContext) {
        ExecuteWriteCallback(false, thisVal, args, ret);
    }

    return JSI::CreateUndefined();
}

static void GetBLEDescriptor(const JSIValue *args, BLEDescriptor &bleDescriptorArgs, bool isRead)
{
    JSIValue jsiBLEDescriptor = JSI::AcquireValue(args[0]);
    char* serviceUuid = JSI::GetStringProperty(jsiBLEDescriptor, "serviceUuid");
    bleDescriptorArgs.serviceUuid = serviceUuid;
    JSI::ReleaseString(serviceUuid);

    char* characteristicUuid = JSI::GetStringProperty(jsiBLEDescriptor, "characteristicUuid");
    bleDescriptorArgs.characteristicUuid = characteristicUuid;
    JSI::ReleaseString(characteristicUuid);

    char* descriptorUuid = JSI::GetStringProperty(jsiBLEDescriptor, "descriptorUuid");
    bleDescriptorArgs.descriptorUuid = descriptorUuid;
    JSI::ReleaseString(descriptorUuid);

    JSIValue jsiDescriptorHandle = JSI::GetNamedProperty(jsiBLEDescriptor, "descriptorHandle");
    if (!JSI::ValueIsUndefined(jsiDescriptorHandle)) {
        bleDescriptorArgs.descriptor.descriptor_hdl = JSI::ValueToNumber(jsiDescriptorHandle);
        JSI::ReleaseValue(jsiDescriptorHandle);
    } else {
        bleDescriptorArgs.descriptor.descriptor_hdl = BLE_INVALID_DESCRIPTOR_HANDLE;
    }
    if (!isRead) {
        JSIValue jsiDescriptorValue = JSI::GetNamedProperty(jsiBLEDescriptor, "descriptorValue");
        size_t descriptorValueLen = 0;
        if (!JSI::ValueIsUndefined(jsiDescriptorValue)) {
            uint8_t *descriptorValue = JSI::GetArrayBufferInfo(jsiDescriptorValue, descriptorValueLen);
            if (descriptorValue) {
                bleDescriptorArgs.descriptorValue.assign(descriptorValue, descriptorValue + descriptorValueLen);
            }
        }
        JSI::ReleaseValue(jsiDescriptorValue);
    }
    JSI::ReleaseValue(jsiBLEDescriptor);
}

// 根据UUID查找特征值
bool BleModule::FindCharacteristic(const uint16_t charHandle, const std::string &serviceUuid,
    const std::string &charUuid, BLECharacteristic &outChar, uint16_t connId)
{
    std::lock_guard<std::mutex> lock(contextMutex);
    
    // 从全局map中获取服务数据
    auto servicesIt = discoveredServicesMap.find(connId);
    if (servicesIt == discoveredServicesMap.end()) {
        HILOG_ERROR(HILOG_MODULE_ACE, "No services found for connId=%u", connId);
        return false;
    }
    auto &services = discoveredServicesMap[connId];
    for (const auto &service : services) {
        if (!CompareHexStrings(service.serviceUuid, serviceUuid)) {
            continue;
        }
        for (const auto &characteristic : service.characteristics) {
            if (!CompareHexStrings(characteristic.characteristicUuid, charUuid)) {
                continue;
            }
            // 使用卫语句简化条件判断
            if (charHandle == BLE_INVALID_CHARACTERISTIC_HANDLE) {
                outChar = characteristic;
                return true;
            }

            if (characteristic.character.declare_handle == charHandle) {
                outChar = characteristic;
                return true;
            }
        }
    }

    HILOG_ERROR(HILOG_MODULE_ACE, "Characteristic %s not found in service %s (connId=%u)",
        charUuid.c_str(), serviceUuid.c_str(), connId);
    return false;
}

// 根据UUID查找描述符
bool BleModule::FindDescriptor(const uint16_t descHandle, const std::string &serviceUuid, const std::string &charUuid,
    const std::string &descUuid, BLEDescriptor &outDesc, uint16_t connId)
{
    std::lock_guard<std::mutex> lock(contextMutex);

    // 从全局map中获取服务数据
    auto servicesIt = discoveredServicesMap.find(connId);
    if (servicesIt == discoveredServicesMap.end()) {
        HILOG_ERROR(HILOG_MODULE_ACE, "No services found for connId=%u", connId);
        return false;
    }
    
    auto &services = servicesIt->second;
    for (const auto &service : services) {
        if (!CompareHexStrings(service.serviceUuid, serviceUuid)) {
            continue;
        }
        
        for (const auto &characteristic : service.characteristics) {
            if (!CompareHexStrings(characteristic.characteristicUuid, charUuid)) {
                continue;
            }
            
            for (const auto &descriptor : characteristic.descriptors) {
                if (!CompareHexStrings(descriptor.descriptorUuid, descUuid)) {
                    continue;
                }
                
                // 使用卫语句简化条件判断
                if (descHandle == BLE_INVALID_DESCRIPTOR_HANDLE) {
                    outDesc = descriptor;
                    return true;
                }
                
                if (descHandle == descriptor.descriptor.descriptor_hdl) {
                    outDesc = descriptor;
                    return true;
                }
            }
        }
    }

    HILOG_ERROR(HILOG_MODULE_ACE, "Descriptor %s not found in char %s, service %s (connId=%u)",
        descUuid.c_str(), charUuid.c_str(), serviceUuid.c_str(), connId);
    return false;
}

JSIValue BleModule::ReadDescriptorValue(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    errcode_t ret = 0;
    BLEDescriptor targetDesc;
    uint16_t connId = 0;
    BLEDescriptor bleDescriptorArgs;
    GattOperationContext context;
    memset_s(&context, sizeof(GattOperationContext), 0, sizeof(GattOperationContext));
    uint16_t handle = 0;
    bool contextCreated = false;
    std::string deviceId;

    // 参数校验
    if (argsSize < ARGC_TWO || argsSize > ARGC_THREE || !JSI::ValueIsFunction(args[1])) {
        ret = BLE_INVALID_PARAMETER;  // 无效参数
        HILOG_ERROR(HILOG_MODULE_ACE, "ReadDescriptor: invalid parameter");
        goto exit;
    }

    // 解析入参
    GetBLEDescriptor(args, bleDescriptorArgs, true);
    if (argsSize == ARGC_THREE) {
        deviceId = ParseDeviceId(args[ARGC_TWO]);
    } else {
        deviceId = GetFirstServerDeviceId();
    }
    {
        std::lock_guard<std::mutex> lock(connectMapMutex);
        // 检查连接状态
        if (deviceId.empty() || deviceMap.find(deviceId) == deviceMap.end()) {
            ret = BLE_OPRATION_FAILED;  // 未连接
            HILOG_ERROR(HILOG_MODULE_ACE, "ReadDescriptor: device not connected");
            goto exit;
        }
        connId = deviceMap[deviceId].connId;
    }

    // 校验描述符是否存在
    if (!FindDescriptor(bleDescriptorArgs.descriptor.descriptor_hdl, bleDescriptorArgs.serviceUuid,
        bleDescriptorArgs.characteristicUuid, bleDescriptorArgs.descriptorUuid, targetDesc, connId)) {
        ret = BLE_READ_FORBID;  // 描述符不存在
        HILOG_ERROR(HILOG_MODULE_ACE,
            "ReadDescriptor: Descriptor %s (characteristic %s, service %s) not found",
            bleDescriptorArgs.descriptorUuid.c_str(),
            bleDescriptorArgs.characteristicUuid.c_str(),
            bleDescriptorArgs.serviceUuid.c_str());
        goto exit;
    }

    // 获取描述符句柄
    handle = targetDesc.descriptor.descriptor_hdl;

    // 保存操作上下文
    context.type = GattOperationContext::READ_DESCRIPTOR;
    context.connId = connId;
    context.serviceUuid = bleDescriptorArgs.serviceUuid;
    context.charUuid = bleDescriptorArgs.characteristicUuid;
    context.descUuid = bleDescriptorArgs.descriptorUuid;
    context.descHandle = bleDescriptorArgs.descriptor.descriptor_hdl;
    context.callback = JSI::AcquireValue(args[1]);
    context.thisVal = JSI::AcquireValue(thisVal);
    contextCreated = true;

    {
        std::lock_guard<std::mutex> lock(operationMutex);
        operationContexts.push_back(context);
    }

    // 发起读取
    ret = gattc_read_req_by_handle(clientId, connId, handle);
    if (ret != BLE_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "ReadDescriptor failed: %X", ret);

        // 清理上下文
        std::lock_guard<std::mutex> lock(operationMutex);
        for (auto it = operationContexts.begin(); it != operationContexts.end(); ++it) {
            if (it->type == GattOperationContext::READ_DESCRIPTOR && 
                CompareHexStrings(it->serviceUuid, context.serviceUuid) &&
                CompareHexStrings(it->charUuid, context.charUuid) && 
                CompareHexStrings(it->descUuid, context.descUuid)) {
                JSI::ReleaseValueList(it->callback, it->thisVal);
                operationContexts.erase(it);
                contextCreated = false;
                break;
            }
        }
        ret = BLE_OPRATION_FAILED;  // 操作失败
        goto exit;
    }

    // 成功路径：不需要释放任何内容（上下文保留用于异步回调）
    return JSI::CreateUndefined();

exit:
    // 错误处理
    if (ret != BLE_SUCCESS) {
        JSIValue callbackObj = contextCreated ? context.callback : JSI::AcquireValue(args[1]);
        if (JSI::ValueIsFunction(callbackObj)) {
            JSIValue err = JSI::CreateNumber(ret);
            JSIValue resultLocal = JSI::CreateUndefined();
            JSIValue argv[ARGC_TWO] = {err, resultLocal};
            JSI::CallFunction(callbackObj, thisVal, argv, ARGC_TWO);
            JSI::ReleaseValueList(err, resultLocal);
            // 仅释放未转移的引用
            if (!contextCreated) {
                JSI::ReleaseValue(callbackObj);
            }
        }
    }

    return JSI::CreateUndefined();
}

JSIValue BleModule::WriteDescriptorValue(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    errcode_t ret = 0;
    uint16_t connId = 0;
    BLEDescriptor bleDescriptorArgs;
    gattc_handle_value_t attValue = {0};
    GattOperationContext context;
    memset_s(&context, sizeof(GattOperationContext), 0, sizeof(GattOperationContext));
    BLEDescriptor targetDesc;
    bool contextCreated = false;
    std::string deviceId;

    // 参数校验
    if (argsSize < ARGC_TWO || argsSize > ARGC_THREE || !JSI::ValueIsFunction(args[1])) {
        ret = BLE_INVALID_PARAMETER;  // 无效参数
        HILOG_ERROR(HILOG_MODULE_ACE, "WriteDescriptor: invalid parameter");
        goto exit;
    }

    // 解析入参
    GetBLEDescriptor(args, bleDescriptorArgs, false);
    if (argsSize == ARGC_THREE) {
        deviceId = ParseDeviceId(args[ARGC_TWO]);
    } else {
        deviceId = GetFirstServerDeviceId();
    }
    {
        std::lock_guard<std::mutex> lock(connectMapMutex);
        // 检查连接状态
        if (deviceId.empty() || deviceMap.find(deviceId) == deviceMap.end()) {
            ret = BLE_UNSUPPORT;  // 未连接
            HILOG_ERROR(HILOG_MODULE_ACE, "WriteDescriptor: device not connected");
            goto exit;
        }
        connId = deviceMap[deviceId].connId;
    }

    // 校验描述符是否存在
    if (!FindDescriptor(bleDescriptorArgs.descriptor.descriptor_hdl, bleDescriptorArgs.serviceUuid,
        bleDescriptorArgs.characteristicUuid, bleDescriptorArgs.descriptorUuid, targetDesc, connId)) {
        ret = BLE_READ_FORBID;  // 描述符不存在
        HILOG_ERROR(HILOG_MODULE_ACE,
            "WriteDescriptor: Descriptor %s (characteristic %s, service %s) not found",
            bleDescriptorArgs.descriptorUuid.c_str(),
            bleDescriptorArgs.characteristicUuid.c_str(),
            bleDescriptorArgs.serviceUuid.c_str());
        goto exit;
    }

    // 保存操作上下文
    context.type = GattOperationContext::WRITE_DESCRIPTOR;
    context.connId = connId;
    context.serviceUuid = bleDescriptorArgs.serviceUuid;
    context.charUuid = bleDescriptorArgs.characteristicUuid;
    context.descUuid = bleDescriptorArgs.descriptorUuid;
    context.descHandle = bleDescriptorArgs.descriptor.descriptor_hdl;
    context.callback = JSI::AcquireValue(args[1]);  // 仅此一次Acquire
    context.thisVal = JSI::AcquireValue(thisVal);
    contextCreated = true;

    {
        std::lock_guard<std::mutex> lock(operationMutex);
        operationContexts.push_back(context);
    }

    // 构造写入数据
    attValue.handle = targetDesc.descriptor.descriptor_hdl;
    attValue.data_len = bleDescriptorArgs.descriptorValue.size();
    attValue.data = const_cast<uint8_t *>(bleDescriptorArgs.descriptorValue.data());

    // 发起写入
    ret = gattc_write_req(clientId, connId, &attValue);
    if (ret != BLE_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "WriteDescriptor failed: %X", ret);

        // 清理上下文
        std::lock_guard<std::mutex> lock(operationMutex);
        for (auto it = operationContexts.begin(); it != operationContexts.end(); ++it) {
            if (it->type == GattOperationContext::WRITE_DESCRIPTOR && 
                CompareHexStrings(it->serviceUuid, context.serviceUuid) &&
                CompareHexStrings(it->charUuid, context.charUuid) && 
                CompareHexStrings(it->descUuid, context.descUuid)) {
                JSI::ReleaseValueList(it->callback, it->thisVal);
                operationContexts.erase(it);
                contextCreated = false;
                break;
            }
        }
        ret = BLE_OPRATION_FAILED;  // 操作失败
        goto exit;
    }

    // 成功路径：不需要释放任何内容（上下文保留用于异步回调）
    return JSI::CreateUndefined();

exit:
    // 错误处理
    if (ret != BLE_SUCCESS) {
        JSIValue callbackObj = contextCreated ? context.callback : JSI::AcquireValue(args[1]);
        if (JSI::ValueIsFunction(callbackObj)) {
            JSIValue err = JSI::CreateNumber(ret);
            JSI::CallFunction(callbackObj, thisVal, &err, 1);
            JSI::ReleaseValue(err);
            
            // 仅释放未转移的引用
            if (!contextCreated) {
                JSI::ReleaseValue(callbackObj);
            }
        }
    }

    return JSI::CreateUndefined();
}

static errcode_t SetCharacteristicChange(const JSIValue *args, bool indication, uint16_t conn_id,
    JSIValue callback, JSIValue thisVal)
{
    // 解析特征值参数
    BLECharacteristic bleCharacteristicArgs;
    GetCharacteristic(args, bleCharacteristicArgs, false);

    // 获取开关状态
    bool enable = false;
    JSIValue jsiEnable = JSI::AcquireValue(args[1]);
    if (!JSI::ValueIsUndefined(jsiEnable)) {
        enable = JSI::ValueToBoolean(jsiEnable);
        JSI::ReleaseValue(jsiEnable);
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE, "Invalid parameter arg 1!");
        return BLE_INVALID_PARAMETER;
    }

    // 查找特征值
    BLECharacteristic targetChar;
    if (!BleModule::FindCharacteristic(bleCharacteristicArgs.character.declare_handle,
        bleCharacteristicArgs.serviceUuid, bleCharacteristicArgs.characteristicUuid, targetChar, conn_id)) {
        HILOG_ERROR(HILOG_MODULE_ACE,
            "Characteristic not found: %s/%s",
            bleCharacteristicArgs.serviceUuid.c_str(),
            bleCharacteristicArgs.characteristicUuid.c_str());
        return BLE_SERVICE_STOP;  // 特征值未找到
    }
    // 查找CCCD描述符
    uint16_t cccdHandle = 0;
    for (const auto &descriptor : targetChar.descriptors) {
        if (CompareHexStrings(descriptor.descriptorUuid, "2902")) {
            cccdHandle = descriptor.descriptor.descriptor_hdl;
            break;
        }
    }
    if (cccdHandle == 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "CCCD descriptor not found for characteristic: %s",
            targetChar.characteristicUuid.c_str());
        return BLE_SERVICE_STOP;  // 描述符未找到
    }
    // 创建操作上下文
    GattOperationContext context;
    memset_s(&context, sizeof(GattOperationContext), 0, sizeof(GattOperationContext));
    context.type = GattOperationContext::WRITE_DESCRIPTOR;
    context.connId = conn_id;
    context.serviceUuid = bleCharacteristicArgs.serviceUuid;
    context.charUuid = bleCharacteristicArgs.characteristicUuid;
    context.charHandle = bleCharacteristicArgs.character.declare_handle;
    context.descHandle = cccdHandle;
    context.descUuid = "2902";
    context.callback = JSI::AcquireValue(callback);
    context.thisVal = JSI::AcquireValue(thisVal);
    JSI::ReleaseValue(callback);

    // 准备写入数据 - 使用栈内存（安全，因为gattc_write_req会拷贝数据）
    gattc_handle_value_t attValue = {0};
    uint8_t data[ARGC_TWO] = {0};
    GetAttValueData(data, enable, indication);

    attValue.handle = cccdHandle;
    attValue.data_len = sizeof(data);
    attValue.data = data;
    // 保存上下文
    {
        std::lock_guard<std::mutex> lock(BleModule::operationMutex);
        BleModule::operationContexts.push_back(context);
    }
    // 发起写入
    errcode_t ret = gattc_write_req(BleModule::clientId, conn_id, &attValue);
    if (ret != BLE_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "SetCharacteristicChange: write request failed: %X", ret);

        // 清理上下文
        std::lock_guard<std::mutex> lock(BleModule::operationMutex);
        for (auto it = BleModule::operationContexts.begin(); it != BleModule::operationContexts.end(); ++it) {
            if (it->type == GattOperationContext::WRITE_DESCRIPTOR && 
                CompareHexStrings(it->serviceUuid, context.serviceUuid) &&
                CompareHexStrings(it->charUuid, context.charUuid) &&
                CompareHexStrings(it->descUuid, context.descUuid)) {
                BleModule::operationContexts.erase(it);
                break;
            }
        }
        return BLE_OPRATION_FAILED;
    }
    return BLE_SUCCESS;
}

JSIValue BleModule::SetCharacteristicChangeNotification(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    JSIValue callback = JSI::CreateUndefined();
    errcode_t ret = BLE_SUCCESS;
    uint16_t connId = 0;
    std::string deviceId;

    // 参数校验
    if (argsSize < ARGC_THREE || argsSize > ARGC_FOUR || !JSI::ValueIsFunction(args[ARGC_TWO])) {
        ret = BLE_INVALID_PARAMETER;
        goto exit;
    }

    // 获取回调并增加引用
    callback = JSI::AcquireValue(args[ARGC_TWO]);
    if (argsSize == ARGC_FOUR) {
        deviceId = ParseDeviceId(args[ARGC_THREE]);
    } else {
        deviceId = GetFirstServerDeviceId();
    }
    {
        std::lock_guard<std::mutex> lock(connectMapMutex);
        // 检查连接状态
        if (deviceId.empty() || deviceMap.find(deviceId) == deviceMap.end()) {
            ret = BLE_UNSUPPORT;
            goto exit;
        }

        // 设置通知
        connId = deviceMap[deviceId].connId;
    }

    ret = SetCharacteristicChange(args, false, connId, callback, thisVal);    

exit:
    if (ret != BLE_SUCCESS) {
        // 错误处理
        if (JSI::ValueIsFunction(callback)) {
            JSIValue err = JSI::CreateNumber(ret);
            JSIValue argv[1] = {err};
            JSI::CallFunction(callback, thisVal, argv, 1);
            JSI::ReleaseValueList(err, callback);
        } else if (!JSI::ValueIsUndefined(callback)) {
            JSI::ReleaseValue(callback);
        }
    }

    return JSI::CreateUndefined();
}

JSIValue BleModule::SetCharacteristicChangeIndication(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    JSIValue callback = JSI::CreateUndefined();
    errcode_t ret = BLE_SUCCESS;
    uint16_t connId = 0;
    std::string deviceId;

    // 参数校验
    if (argsSize < ARGC_THREE || argsSize > ARGC_FOUR || !JSI::ValueIsFunction(args[ARGC_TWO])) {
        ret = BLE_INVALID_PARAMETER;
        goto exit;
    }

    // 获取回调并增加引用
    callback = JSI::AcquireValue(args[ARGC_TWO]);
    if (argsSize == ARGC_FOUR) {
        deviceId = ParseDeviceId(args[ARGC_THREE]);
    } else {
        deviceId = GetFirstServerDeviceId();
    }
    {
        std::lock_guard<std::mutex> lock(connectMapMutex);
        // 检查连接状态
        if (deviceId.empty() || deviceMap.find(deviceId) == deviceMap.end()) {
            ret = BLE_UNSUPPORT;
            goto exit;
        }

        // 设置指示
        connId = deviceMap[deviceId].connId;
    }

    ret = SetCharacteristicChange(args, true, connId, callback, thisVal);

exit:
    if (ret != BLE_SUCCESS) {
        // 错误处理
        if (JSI::ValueIsFunction(callback)) {
            JSIValue err = JSI::CreateNumber(ret);
            JSIValue argv[1] = {err};
            JSI::CallFunction(callback, thisVal, argv, 1);
            JSI::ReleaseValueList(err, callback);
        } else if (!JSI::ValueIsUndefined(callback)) {
            JSI::ReleaseValue(callback);
        }
    }

    return JSI::CreateUndefined();
}
}  // namespace ACELite
}  // namespace OHOS

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
#include "bts_gatt_server.h"
#include "BluetoothWrapper.h"
#include "securec.h"

namespace OHOS {
namespace ACELite {
JSIValue BleModule::gattServer = JSI::CreateUndefined();
uint8_t BleModule::serverId = 0;
std::map<std::string, uint16_t> BleModule::g_serviceHandleMap;
std::mutex BleModule::serviceBuildContextsMutex;
std::map<uint16_t, ServiceBuildContext> BleModule::g_serviceBuildContexts;
std::map<std::string, uint16_t> BleModule::g_characteristicHandleMap;
std::map<uint16_t, RemoveServiceContext> BleModule::g_removeServiceContexts;
std::map<uint16_t, NotifyCharacteristicContext> BleModule::g_notifyContexts;
std::map<uint16_t, CharacteristicInfo> BleModule::g_handleToCharacteristicMap;
std::map<uint16_t, DescriptorInfo> BleModule::g_handleToDescriptorMap;
std::mutex BleModule::serviceCleanMutex;

#define BT_UUID_16BIT_LEN 2
#define BT_UUID_128BIT_LEN 16

static errcode_t RegisterGatts()
{
    bt_uuid_t uuid;
    memset_s(&uuid, sizeof(bt_uuid_t), 0, sizeof(bt_uuid_t));
    uuid.uuid_len = 2;
    uuid.uuid[0] = 0xFF;
    uuid.uuid[1] = 0x2F;
    errcode_t ret = wrapper_gatts_register_server(&uuid, &BleModule::serverId);
    if (ret != BLE_SUCCESS) {
        HILOG_ERROR(
            HILOG_MODULE_ACE, "gattc register client failed! ret: 0x%x, serverId:%d\n", ret, BleModule::serverId);
    }
    return ret;
}

static errcode_t RegisterServerCallbacks()
{
    gatts_callbacks_t gatts_cb = {nullptr,
        nullptr,
        nullptr,
        GattsStartServiceCbk,
        GattsStopServiceCbk,
        nullptr,
        GattsReadRequestCbk,
        GattsWriteRequestCbk,
        GattsMtuChangedCbk};
    errcode_t ret = wrapper_gatts_register_ohos_callbacks(&gatts_cb);
    if (ret != BLE_SUCCESS) {
        HILOG_ERROR(
            HILOG_MODULE_ACE, "gatts_register_callbacks failed! ret: 0x%x clientId:%u\n", ret, BleModule::clientId);
        return BLE_OPRATION_FAILED;
    }
    return BLE_SUCCESS;
}

JSIValue BleModule::CreateGattServer(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    bool findId = false;
    if (argsSize != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "gatts_register params count error!");
        return JSI::CreateUndefined();
    }
    if (!JSI::ValueIsUndefined(gattServer)) {
        return gattServer;
    }
    gattServer = JSI::CreateObject();
    JSIValue addService = JSI::CreateFunction(BleModule::AddService);
    JSIValue removeService = JSI::CreateFunction(BleModule::RemoveService);
    JSIValue on = JSI::CreateFunction(BleModule::ServerOn);
    JSIValue off = JSI::CreateFunction(BleModule::ServerOff);
    JSIValue close = JSI::CreateFunction(BleModule::ServerClose);
    JSIValue notifyCharacteristicChanged = JSI::CreateFunction(BleModule::NotifyCharacteristicChanged);
    JSI::SetNamedProperty(gattServer, "addService", addService);
    JSI::SetNamedProperty(gattServer, "removeService", removeService);
    JSI::SetNamedProperty(gattServer, "on", on);
    JSI::SetNamedProperty(gattServer, "off", off);
    JSI::SetNamedProperty(gattServer, "close", close);
    JSI::SetNamedProperty(gattServer, "notifyCharacteristicChanged", notifyCharacteristicChanged);
    if (RegisterGatts() != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Register Gatts failed!");
        JSI::ReleaseValue(gattServer);
        return JSI::CreateUndefined();
    }
    if (RegisterServerCallbacks() != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Register Gatts callback failed!");
        JSI::ReleaseValue(gattServer);
        return JSI::CreateUndefined();
    }
    HILOG_DEBUG(HILOG_MODULE_ACE, "Register Gatts SUCCESS!");
    return gattServer;
}

bt_uuid_t ConvertUuid(const std::string& uuidStr) {
    bt_uuid_t uuid;
    memset_s(&uuid, sizeof(bt_uuid_t), 0, sizeof(bt_uuid_t));
    HILOG_DEBUG(HILOG_MODULE_ACE, "ConvertUuid: input uuidStr=%s", uuidStr.c_str());

    // 移除UUID中的连字符
    std::string cleanUuid = uuidStr;
    cleanUuid.erase(std::remove(cleanUuid.begin(), cleanUuid.end(), '-'), cleanUuid.end());
    HILOG_DEBUG(HILOG_MODULE_ACE, "ConvertUuid: cleanUuid=%s, length=%zu", cleanUuid.c_str(), cleanUuid.length());

    if (cleanUuid.length() == 4) { // 16位UUID（2字节）：大端字节序
        uuid.uuid_len = BT_UUID_16BIT_LEN;
        uint16_t uuid16 = static_cast<uint16_t>(std::stoul(cleanUuid, nullptr, 16));
        uuid.uuid[0] = (uuid16 >> 8) & 0xFF;  // 高8位
        uuid.uuid[1] = uuid16 & 0xFF;         // 低8位
        HILOG_DEBUG(HILOG_MODULE_ACE, "ConvertUuid: 16bit uuid, value=0x%04X, bytes=[0x%02X, 0x%02X]", 
                   uuid16, uuid.uuid[0], uuid.uuid[1]);
    } else if (cleanUuid.length() == 32) { // 128位UUID（16字节）：小端字节序
        uuid.uuid_len = BT_UUID_128BIT_LEN;
        std::vector<uint8_t> tempBytes;
        // 步骤1：解析字符串为字节数组（正序）
        for (size_t i = 0; i < cleanUuid.length(); i += 2) {
            std::string byteStr = cleanUuid.substr(i, 2);
            tempBytes.push_back(static_cast<uint8_t>(std::stoul(byteStr, nullptr, BT_UUID_128BIT_LEN)));
        }
        // 步骤2：反转字节数组（小端要求）
        std::reverse(tempBytes.begin(), tempBytes.end());
        // 步骤3：复制到uuid结构体（关键：处理memcpy_s返回值）
        errno_t memcpyRet = memcpy_s(uuid.uuid, BT_UUID_128BIT_LEN, tempBytes.data(), BT_UUID_128BIT_LEN);
        if (memcpyRet != EOK) { // 检查复制结果：EOK为成功（不同平台可能用0表示成功，需确认）
            // 复制失败：标记UUID为无效，记录错误日志
            uuid.uuid_len = 0;
            HILOG_ERROR(HILOG_MODULE_ACE, "ConvertUuid: memcpy_s failed for 128bit uuid, ret=%d", memcpyRet);
        } else {
            HILOG_DEBUG(HILOG_MODULE_ACE, "ConvertUuid: 128bit uuid, reversed bytes set successfully");
        }
    } else {
        uuid.uuid_len = 0; // 无效UUID（长度不匹配）
        HILOG_ERROR(HILOG_MODULE_ACE, "ConvertUuid: invalid uuid length=%zu", cleanUuid.length());
    }

    return uuid;
}

// 属性转换
uint8_t ConvertProperties(const GattProperties& properties) {
    uint8_t props = 0;
    if (properties.read) {
        props |= GATT_CHARACTER_PROPERTY_BIT_READ;
    }
    if (properties.write) {
        props |= GATT_CHARACTER_PROPERTY_BIT_WRITE;
    }
    if (properties.writeNoResponse) {
        props |= GATT_CHARACTER_PROPERTY_BIT_WRITE_NO_RSP;
    }
    if (properties.notify) {
        props |= GATT_CHARACTER_PROPERTY_BIT_NOTIFY;
    }
    if (properties.indicate) {
        props |= GATT_CHARACTER_PROPERTY_BIT_INDICATE;
    }
    return props;
}

// 权限转换
uint8_t ConvertPermissions(const GattProperties& properties) {
    uint8_t perms = 0;
    if (properties.read) {
        perms |= GATT_ATTRIBUTE_PERMISSION_READ;
    }
    if (properties.write || properties.writeNoResponse) {
        perms |= GATT_ATTRIBUTE_PERMISSION_WRITE;
    }
    if (properties.notify || properties.indicate) {
        perms |= GATT_ATTRIBUTE_PERMISSION_READ;
    }
    return perms;
}

// 添加特征值句柄映射
void AddCharacteristicHandleMapping(uint16_t handle, const std::string& serviceUuid, 
                                   const std::string& characteristicUuid, uint16_t valueHandle) {
    CharacteristicInfo info;
    info.serviceUuid = serviceUuid;
    info.characteristicUuid = characteristicUuid;
    info.valueHandle = valueHandle;
    BleModule::g_handleToCharacteristicMap[handle] = info;

    // 同时保存值句柄的映射（对于读请求，协议栈可能使用值句柄）
    if (valueHandle != handle && valueHandle != 0) {
        BleModule::g_handleToCharacteristicMap[valueHandle] = info;
    }

    HILOG_INFO(HILOG_MODULE_ACE, "AddCharacteristicHandleMapping: handle %X, valueHandle %X, service %s, chara %s", 
               handle, valueHandle, serviceUuid.c_str(), characteristicUuid.c_str());
}

// 添加描述符句柄映射
void AddDescriptorHandleMapping(uint16_t handle, const std::string& serviceUuid, 
                               const std::string& characteristicUuid, const std::string& descriptorUuid)
{
    DescriptorInfo info;
    info.serviceUuid = serviceUuid;
    info.characteristicUuid = characteristicUuid;
    info.descriptorUuid = descriptorUuid;
    BleModule::g_handleToDescriptorMap[handle] = info;

    HILOG_INFO(HILOG_MODULE_ACE, "AddDescriptorHandleMapping: handle %X, service %s, chara %s, desc %s", 
               handle, serviceUuid.c_str(), characteristicUuid.c_str(), descriptorUuid.c_str());
}

// CreateResultService 函数
GattService* CreateResultService(const GattService& originalService)
{
    GattService* resultService = new GattService();

    // 拷贝基本字段
    resultService->serviceUuid = originalService.serviceUuid;
    resultService->isPrimary = originalService.isPrimary;
    resultService->service = originalService.service;
    // 深拷贝特征值
    resultService->characteristics.clear();
    for (const auto& chara : originalService.characteristics) {
        BLECharacteristic resultChara;
        // 拷贝特征值基本字段
        resultChara.serviceUuid = chara.serviceUuid;
        resultChara.characteristicUuid = chara.characteristicUuid;
        resultChara.characteristicValue = chara.characteristicValue;
        resultChara.properties = chara.properties;
        resultChara.character = chara.character;
        resultChara.service_start_hdl = chara.service_start_hdl;
        resultChara.service_end_hdl = chara.service_end_hdl;

        // 深拷贝描述符
        resultChara.descriptors.clear();
        for (const auto& desc : chara.descriptors) {
            BLEDescriptor resultDesc;
            resultDesc.serviceUuid = desc.serviceUuid;
            resultDesc.characteristicUuid = desc.characteristicUuid;
            resultDesc.descriptorUuid = desc.descriptorUuid;
            resultDesc.descriptorValue = desc.descriptorValue;
            resultDesc.descriptor = desc.descriptor;

            resultChara.descriptors.push_back(resultDesc);
        }

        resultService->characteristics.push_back(resultChara);
    }

    // 包含的服务（递归），协议层不支持，只做浅拷贝
    resultService->includeServices.clear();
    for (const auto& includeService : originalService.includeServices) {
        GattService resultIncludeService;
        resultIncludeService = includeService;
        resultService->includeServices.push_back(resultIncludeService);
    }
    
    return resultService;
}

void ParseProperties(const JSIValue jsiCharacteristic, GattProperties& properties)
{
    JSIValue jsiProperties = JSI::GetNamedProperty(jsiCharacteristic, "properties");
    if (JSI::ValueIsUndefined(jsiProperties)) {
        return;
    }

    properties.write = JSI::GetBooleanProperty(jsiProperties, "write");
    properties.writeNoResponse = JSI::GetBooleanProperty(jsiProperties, "writeNoResponse");
    properties.read = JSI::GetBooleanProperty(jsiProperties, "read");
    properties.notify = JSI::GetBooleanProperty(jsiProperties, "notify");
    properties.indicate = JSI::GetBooleanProperty(jsiProperties, "indicate");

    JSI::ReleaseValue(jsiProperties);
}

void ParseDescriptor(const JSIValue jsiDescriptor, BLEDescriptor& bleDescriptor)
{
    char* serviceUuid = JSI::GetStringProperty(jsiDescriptor, "serviceUuid");
    bleDescriptor.serviceUuid = serviceUuid;
    JSI::ReleaseString(serviceUuid);

    char* characteristicUuid = JSI::GetStringProperty(jsiDescriptor, "characteristicUuid");
    bleDescriptor.characteristicUuid = characteristicUuid;
    JSI::ReleaseString(characteristicUuid);

    char* descriptorUuid = JSI::GetStringProperty(jsiDescriptor, "descriptorUuid");
    bleDescriptor.descriptorUuid = descriptorUuid;
    JSI::ReleaseString(descriptorUuid);

    // 解析描述符句柄（添加服务时通常为0，由协议栈分配）
    JSIValue jsiDescriptorHandle = JSI::GetNamedProperty(jsiDescriptor, "descriptorHandle");
    if (!JSI::ValueIsUndefined(jsiDescriptorHandle)) {
        bleDescriptor.descriptor.descriptor_hdl = JSI::ValueToNumber(jsiDescriptorHandle);
        JSI::ReleaseValue(jsiDescriptorHandle);
    } else {
        bleDescriptor.descriptor.descriptor_hdl = 0;
    }

    // 解析描述符值
    JSIValue jsiDescriptorValue = JSI::GetNamedProperty(jsiDescriptor, "descriptorValue");
    size_t descriptorValueLen = 0;
    if (!JSI::ValueIsUndefined(jsiDescriptorValue)) {
        uint8_t *descriptorValue = JSI::GetArrayBufferInfo(jsiDescriptorValue, descriptorValueLen);
        if (descriptorValue && descriptorValueLen > 0) {
            bleDescriptor.descriptorValue.assign(descriptorValue, descriptorValue + descriptorValueLen);
        }
        JSI::ReleaseValue(jsiDescriptorValue);
    }
}

void ParseCharacteristic(const JSIValue jsiCharacteristic, BLECharacteristic& bleCharacteristic)
{
    char* serviceUuid = JSI::GetStringProperty(jsiCharacteristic, "serviceUuid");
    bleCharacteristic.serviceUuid = serviceUuid;
    JSI::ReleaseString(serviceUuid);
    char* characteristicUuid = JSI::GetStringProperty(jsiCharacteristic, "characteristicUuid");
    bleCharacteristic.characteristicUuid = characteristicUuid;
    JSI::ReleaseString(characteristicUuid);

    // 解析特征值句柄
    JSIValue jsiCharactHandle = JSI::GetNamedProperty(jsiCharacteristic, "characteristicValueHandle");
    if (!JSI::ValueIsUndefined(jsiCharactHandle)) {
        bleCharacteristic.character.declare_handle = JSI::ValueToNumber(jsiCharactHandle);
        JSI::ReleaseValue(jsiCharactHandle);
    } else {
        bleCharacteristic.character.declare_handle = 0;
    }

    // 解析特征值数据
    JSIValue jsiCharacteristicValue = JSI::GetNamedProperty(jsiCharacteristic, "characteristicValue");
    size_t characteristicValueLen = 0;
    if (!JSI::ValueIsUndefined(jsiCharacteristicValue)) {
        uint8_t *characteristicValue = JSI::GetArrayBufferInfo(jsiCharacteristicValue, characteristicValueLen);
        if (characteristicValue && characteristicValueLen > 0) {
            bleCharacteristic.characteristicValue.assign(characteristicValue, characteristicValue + characteristicValueLen);
        }
        JSI::ReleaseValue(jsiCharacteristicValue);
    }

    // 确保描述符向量正确初始化
    bleCharacteristic.descriptors.clear();

    // 解析描述符数组
    JSIValue jsiDescriptors = JSI::GetNamedProperty(jsiCharacteristic, "descriptors");
    if (!JSI::ValueIsUndefined(jsiDescriptors)) {
        int32_t descriptorsLen = JSI::GetArrayLength(jsiDescriptors);
        HILOG_DEBUG(HILOG_MODULE_ACE, "Found %d descriptors for characteristic %s", 
                   descriptorsLen, bleCharacteristic.characteristicUuid.c_str());

        // 预分配空间
        bleCharacteristic.descriptors.resize(descriptorsLen);

        for (int32_t i = 0; i < descriptorsLen; i++) {
            JSIValue jsiDescriptor = JSI::GetPropertyByIndex(jsiDescriptors, i);
            ParseDescriptor(jsiDescriptor, bleCharacteristic.descriptors[i]);
            JSI::ReleaseValue(jsiDescriptor);
        }
        JSI::ReleaseValue(jsiDescriptors);
    }

    // 解析属性
    ParseProperties(jsiCharacteristic, bleCharacteristic.properties);
}

errcode_t ParseAddServiceArguments(const JSIValue* args, GattService& gattServiceArgs)
{
    JSIValue jsiService = JSI::AcquireValue(args[0]);
    if (JSI::ValueIsUndefined(jsiService)) {
        return BLE_INVALID_PARAMETER;
    }

    // 解析基础服务信息
    char* serviceUuid = JSI::GetStringProperty(jsiService, "serviceUuid");
    gattServiceArgs.serviceUuid = serviceUuid;
    JSI::ReleaseString(serviceUuid);

    JSIValue jsiIsPrimary = JSI::GetNamedProperty(jsiService, "isPrimary");
    if (!JSI::ValueIsUndefined(jsiIsPrimary)) {
        gattServiceArgs.isPrimary = JSI::ValueToBoolean(jsiIsPrimary);
        JSI::ReleaseValue(jsiIsPrimary);
    }

    // 确保特征值向量正确初始化
    gattServiceArgs.characteristics.clear();

    // 解析特征值数组
    JSIValue jsiCharacteristics = JSI::GetNamedProperty(jsiService, "characteristics");
    if (!JSI::ValueIsUndefined(jsiCharacteristics)) {
        int32_t characteristicsLen = JSI::GetArrayLength(jsiCharacteristics);
        HILOG_DEBUG(HILOG_MODULE_ACE, "Found %d characteristics", characteristicsLen);

        // 预分配空间
        gattServiceArgs.characteristics.resize(characteristicsLen);

        for (int32_t i = 0; i < characteristicsLen; i++) {
            JSIValue jsiCharacteristic = JSI::GetPropertyByIndex(jsiCharacteristics, i);
            
            // 确保每个特征值的描述符向量正确初始化
            gattServiceArgs.characteristics[i].descriptors.clear();
            
            ParseCharacteristic(jsiCharacteristic, gattServiceArgs.characteristics[i]);
            JSI::ReleaseValue(jsiCharacteristic);
        }
        JSI::ReleaseValue(jsiCharacteristics);
    }

    JSI::ReleaseValue(jsiService);
    return BLE_SUCCESS;
}

// 创建JSI GattService对象
JSIValue CreateJsiGattService(const GattService& service)
{
    JSIValue jsiService = JSI::CreateObject();

    // 设置基础属性
    JSI::SetStringProperty(jsiService, "serviceUuid", service.serviceUuid.c_str());
    JSI::SetBooleanProperty(jsiService, "isPrimary", service.isPrimary);

    // 创建特征值数组
    JSIValue jsiCharacteristics = JSI::CreateArray(service.characteristics.size());
    for (size_t i = 0; i < service.characteristics.size(); i++) {
        const auto& chara = service.characteristics[i];
        JSIValue jsiChara = JSI::CreateObject();

        JSI::SetStringProperty(jsiChara, "serviceUuid", chara.serviceUuid.c_str());
        JSI::SetStringProperty(jsiChara, "characteristicUuid", chara.characteristicUuid.c_str());
        JSI::SetNumberProperty(jsiChara, "characteristicValueHandle", chara.character.declare_handle);

        // 设置特征值数据
        if (!chara.characteristicValue.empty()) {
            uint8_t *valuePtr = nullptr;
            size_t bufferLength = chara.characteristicValue.size();
            JSIValue jsiValue = JSI::CreateArrayBuffer(bufferLength, valuePtr);

            // 检查分配是否成功
            if (valuePtr == nullptr) {
                HILOG_ERROR(HILOG_MODULE_ACE, "CreateArrayBuffer for characteristic Value failed!");
            } else if (bufferLength > 0) {
                if (memcpy_s(valuePtr, bufferLength, chara.characteristicValue.data(), bufferLength) != EOK) {
                    HILOG_ERROR(HILOG_MODULE_ACE, "memcpy_s characteristic Value failed!");
                } else {
                    JSI::SetNamedProperty(jsiChara, "characteristicValue", jsiValue);
                }
            }
            JSI::ReleaseValue(jsiValue);
        }

        // 创建描述符数组
        JSIValue jsiDescriptors = JSI::CreateArray(chara.descriptors.size());
        for (size_t j = 0; j < chara.descriptors.size(); j++) {
            const auto& desc = chara.descriptors[j];
            JSIValue jsiDesc = JSI::CreateObject();

            JSI::SetStringProperty(jsiDesc, "serviceUuid", desc.serviceUuid.c_str());
            JSI::SetStringProperty(jsiDesc, "characteristicUuid", desc.characteristicUuid.c_str());
            JSI::SetStringProperty(jsiDesc, "descriptorUuid", desc.descriptorUuid.c_str());
            JSI::SetNumberProperty(jsiDesc, "descriptorHandle", desc.descriptor.descriptor_hdl);

            // 设置描述符值
            if (!desc.descriptorValue.empty()) {
                uint8_t *descValuePtr = nullptr;
                size_t descBufferLength = desc.descriptorValue.size();
                JSIValue jsiDescValue = JSI::CreateArrayBuffer(descBufferLength, descValuePtr);

                // 检查分配是否成功
                if (descBufferLength > 0 && descValuePtr == nullptr) {
                    HILOG_ERROR(HILOG_MODULE_ACE, "CreateArrayBuffer for descriptor Value failed!");
                    JSI::ReleaseValue(jsiDescValue);
                } else if (descBufferLength > 0) {
                    if (memcpy_s(descValuePtr, descBufferLength, desc.descriptorValue.data(), descBufferLength) != EOK) {
                        HILOG_ERROR(HILOG_MODULE_ACE, "memcpy_s descriptor Value failed!");
                    }
                    JSI::SetNamedProperty(jsiDesc, "descriptorValue", jsiDescValue);
                }
                JSI::ReleaseValue(jsiDescValue);
            }

            JSI::SetPropertyByIndex(jsiDescriptors, j, jsiDesc);
            JSI::ReleaseValue(jsiDesc);
        }
        JSI::SetNamedProperty(jsiChara, "descriptors", jsiDescriptors);
        JSI::ReleaseValue(jsiDescriptors);

        // 设置属性
        JSIValue jsiProperties = JSI::CreateObject();
        JSI::SetBooleanProperty(jsiProperties, "write", chara.properties.write);
        JSI::SetBooleanProperty(jsiProperties, "writeNoResponse", chara.properties.writeNoResponse);
        JSI::SetBooleanProperty(jsiProperties, "read", chara.properties.read);
        JSI::SetBooleanProperty(jsiProperties, "notify", chara.properties.notify);
        JSI::SetBooleanProperty(jsiProperties, "indicate", chara.properties.indicate);
        JSI::SetNamedProperty(jsiChara, "properties", jsiProperties);
        JSI::ReleaseValue(jsiProperties);

        JSI::SetPropertyByIndex(jsiCharacteristics, i, jsiChara);
        JSI::ReleaseValue(jsiChara);
    }
    JSI::SetNamedProperty(jsiService, "characteristics", jsiCharacteristics);
    JSI::ReleaseValue(jsiCharacteristics);

    // 包含服务（通常为空）
    JSIValue jsiIncludeServices = JSI::CreateArray(0);
    JSI::SetNamedProperty(jsiService, "includeServices", jsiIncludeServices);
    JSI::ReleaseValue(jsiIncludeServices);
    
    return jsiService;
}

void ExecuteAddServiceCallback(const JSIValue thisVal, const JSIValue* args, errcode_t ret)
{
    JSIValue callbackObj = JSI::AcquireValue(args[1]);
    if (JSI::ValueIsFunction(callbackObj)) {
        JSIValue params[2];
        params[0] = JSI::CreateNumber(ret);
        params[1] = JSI::CreateUndefined();
        JSI::CallFunction(callbackObj, thisVal, params, 2);
        JSI::ReleaseValue(params[0]);
    }
    JSI::ReleaseValue(callbackObj);
}

bool ValidateAddServiceParameters(uint8_t argsSize, const JSIValue *args)
{
    if (argsSize < 2) {
        HILOG_ERROR(HILOG_MODULE_ACE, "AddService: insufficient arguments");
        return false;
    }

    if (!JSI::ValueIsObject(args[0])) {
        HILOG_ERROR(HILOG_MODULE_ACE, "AddService: first argument must be GattService object");
        return false;
    }

    if (!JSI::ValueIsFunction(args[1])) {
        HILOG_ERROR(HILOG_MODULE_ACE, "AddService: second argument must be callback function");
        return false;
    }

    return true;
}

JSIValue BleModule::AddService(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    errcode_t ret = BLE_SUCCESS;
    GattService* gattServiceArgs = new GattService();
    uint16_t serviceHandle = 0;

    // 参数校验
    if (!ValidateAddServiceParameters(argsSize, args)) {
        ret = BLE_INVALID_PARAMETER;
        ExecuteAddServiceCallback(thisVal, args, ret);
        delete gattServiceArgs;
        return JSI::CreateUndefined();
    }
    // 解析服务参数
    ret = ParseAddServiceArguments(args, *gattServiceArgs);
    if (ret != BLE_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "AddService: parse arguments failed");
        ExecuteAddServiceCallback(thisVal, args, ret);
        delete gattServiceArgs;
        return JSI::CreateUndefined();
    }

    // 验证服务UUID
    if (gattServiceArgs->serviceUuid.empty()) {
        HILOG_ERROR(HILOG_MODULE_ACE, "AddService: serviceUuid is empty");
        ret = BLE_INVALID_PARAMETER;
        ExecuteAddServiceCallback(thisVal, args, ret);
        delete gattServiceArgs;
        return JSI::CreateUndefined();
    }
    // 创建结果服务
    GattService* resultService = CreateResultService(*gattServiceArgs);

    // 调用底层添加服务操作
    ret = PerformAddServiceOperation(*gattServiceArgs, *resultService, serviceHandle);
    if (ret != BLE_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "AddService: perform operation failed: %X", ret);
        ExecuteAddServiceCallback(thisVal, args, ret);
        delete gattServiceArgs;
        delete resultService;
        return JSI::CreateUndefined();
    }
    // 保存服务映射
    BleModule::g_serviceHandleMap[gattServiceArgs->serviceUuid] = serviceHandle;

    // 保存构建上下文
    ServiceBuildContext context;
    context.originalService = gattServiceArgs;
    context.resultService = resultService;
    context.callback = JSI::AcquireValue(args[1]);
    context.thisVal = JSI::AcquireValue(thisVal);
    context.serviceHandle = serviceHandle;
    {
        std::lock_guard<std::mutex> lock(serviceBuildContextsMutex);
        BleModule::g_serviceBuildContexts[serviceHandle] = context;
    }
    // 启动服务（异步）
    ret = gatts_start_service(serverId, serviceHandle);
    if (ret != BLE_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "AddService: start service failed: %X", ret);
        // 清理上下文
        delete context.originalService;
        delete context.resultService;
        JSI::ReleaseValue(context.callback);
        JSI::ReleaseValue(context.thisVal);
        {
            std::lock_guard<std::mutex> lock(serviceBuildContextsMutex);
            BleModule::g_serviceBuildContexts.erase(serviceHandle);
        }
        BleModule::g_serviceHandleMap.erase(gattServiceArgs->serviceUuid);
        ExecuteAddServiceCallback(thisVal, args, ret);
    }
    return JSI::CreateUndefined();
}

// 生成特征值映射的键
std::string GenerateCharacteristicKey(const std::string& serviceUuid, const std::string& characteristicUuid)
{
    // 移除连字符但不改变大小写，保持原始格式
    std::string cleanServiceUuid = serviceUuid;
    cleanServiceUuid.erase(std::remove(cleanServiceUuid.begin(), cleanServiceUuid.end(), '-'), cleanServiceUuid.end());

    std::string cleanCharaUuid = characteristicUuid;
    cleanCharaUuid.erase(std::remove(cleanCharaUuid.begin(), cleanCharaUuid.end(), '-'), cleanCharaUuid.end());

    // 使用原始大小写，蓝牙UUID规范不区分大小写但实现应保持一致
    return cleanServiceUuid + ":" + cleanCharaUuid;
}

// 添加特征值句柄映射
void AddCharacteristicHandle(const std::string& serviceUuid, const std::string& characteristicUuid, uint16_t charHandle) {
    if (serviceUuid.empty() || characteristicUuid.empty() || charHandle == 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "AddCharacteristicHandle: invalid parameters");
        return;
    }

    std::string key = GenerateCharacteristicKey(serviceUuid, characteristicUuid);
    BleModule::g_characteristicHandleMap[key] = charHandle;
    
    HILOG_INFO(HILOG_MODULE_ACE, "AddCharacteristicHandle: service %s, chara %s, handle %X", 
               serviceUuid.c_str(), characteristicUuid.c_str(), charHandle);
}

// 添加服务基础操作
errcode_t AddServiceBase(const GattService& gattService, GattService& resultService, uint16_t& serviceHandle)
{
    bt_uuid_t serviceUuid = ConvertUuid(gattService.serviceUuid);
    if (serviceUuid.uuid_len == 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "PerformAddServiceOperation: invalid service UUID");
        return BLE_INVALID_PARAMETER;
    }

    HILOG_DEBUG(HILOG_MODULE_ACE, "PerformAddServiceOperation: adding service, UUID: %s", gattService.serviceUuid.c_str());

    // 添加服务
    errcode_t ret = gatts_add_service_sync(BleModule::serverId, &serviceUuid, gattService.isPrimary, &serviceHandle);
    if (ret != BLE_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "AddService: gatts_add_service_sync failed: %X", ret);
        return ret;
    }

    HILOG_DEBUG(HILOG_MODULE_ACE, "PerformAddServiceOperation: service added, handle: %X", serviceHandle);
    resultService.service.start_hdl = serviceHandle;

    // 添加边界检查
    if (gattService.characteristics.size() != resultService.characteristics.size()) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Characteristic size mismatch: gattService=%zu, resultService=%zu", 
                   gattService.characteristics.size(), resultService.characteristics.size());
        return BLE_INVALID_PARAMETER;
    }

    return BLE_SUCCESS;
}

// 为主特征值添加所有描述符
errcode_t AddDescriptorsForCharacteristic(const GattService& gattService, GattService& resultService,
                                         uint16_t serviceHandle, size_t charIndex)
{
    const auto& characteristic = gattService.characteristics[charIndex];

    // 添加描述符边界检查
    if (characteristic.descriptors.size() != resultService.characteristics[charIndex].descriptors.size()) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Descriptor size mismatch for characteristic %zu: original=%zu, result=%zu", 
                   charIndex, characteristic.descriptors.size(), resultService.characteristics[charIndex].descriptors.size());
        resultService.characteristics[charIndex].descriptors.resize(characteristic.descriptors.size());
    }
    // 添加描述符
    for (size_t descIndex = 0; descIndex < characteristic.descriptors.size(); descIndex++) {
        const auto& descriptor = characteristic.descriptors[descIndex];

        HILOG_DEBUG(HILOG_MODULE_ACE, "Adding descriptor %zu for characteristic %zu: %s", 
                   descIndex, charIndex, descriptor.descriptorUuid.c_str());
        // 边界检查
        if (descIndex >= resultService.characteristics[charIndex].descriptors.size()) {
            HILOG_ERROR(HILOG_MODULE_ACE, "Descriptor index out of bounds: %zu >= %zu", 
                       descIndex, resultService.characteristics[charIndex].descriptors.size());
            return BLE_INVALID_PARAMETER;
        }
        gatts_add_desc_info_t descInfo;
        memset_s(&descInfo, sizeof(gatts_add_desc_info_t), 0, sizeof(gatts_add_desc_info_t));
        descInfo.desc_uuid = ConvertUuid(descriptor.descriptorUuid);
        if (descInfo.desc_uuid.uuid_len == 0) {
            HILOG_ERROR(HILOG_MODULE_ACE, "Invalid descriptor UUID: %s", descriptor.descriptorUuid.c_str());
            return BLE_INVALID_PARAMETER;
        }
        descInfo.permissions = ConvertPermissions(characteristic.properties);
        descInfo.value_len = descriptor.descriptorValue.size();

        // 创建数据副本，避免const_cast
        std::vector<uint8_t> valueCopy;
        if (descInfo.value_len > 0) {
            valueCopy = descriptor.descriptorValue;  // 深拷贝
            descInfo.value = valueCopy.data();       // 使用副本的数据
        } else {
            descInfo.value = nullptr;
        }

        uint16_t descHandle = 0;
        HILOG_DEBUG(HILOG_MODULE_ACE, "Calling gatts_add_descriptor_sync, serverId:%d, serviceHandle:%x", 
                   BleModule::serverId, serviceHandle);

        // 在同步调用期间，valueCopy保持有效
        errcode_t ret = gatts_add_descriptor_sync(BleModule::serverId, serviceHandle, &descInfo, &descHandle);
        if (ret != BLE_SUCCESS) {
            HILOG_ERROR(HILOG_MODULE_ACE, "AddService: gatts_add_descriptor_sync failed: %X", ret);
            return ret;
        }
        HILOG_DEBUG(HILOG_MODULE_ACE, "Descriptor added: handle=%x", descHandle);

        // 保存描述符句柄到结果服务
        if (descIndex < resultService.characteristics[charIndex].descriptors.size()) {
            resultService.characteristics[charIndex].descriptors[descIndex].descriptor.descriptor_hdl = descHandle;
            HILOG_DEBUG(HILOG_MODULE_ACE, "Descriptor handle saved: %x", descHandle);
        } else {
            HILOG_ERROR(HILOG_MODULE_ACE, "Cannot save descriptor handle: index out of bounds");
            return BLE_INVALID_PARAMETER;
        }

        AddDescriptorHandleMapping(descHandle, gattService.serviceUuid, 
                                  characteristic.characteristicUuid, descriptor.descriptorUuid);
    }
    
    return BLE_SUCCESS;
}

// 添加单个特征值及其描述符
errcode_t AddCharacteristicWithDescriptors(const GattService& gattService, GattService& resultService, 
                                          uint16_t serviceHandle, size_t charIndex)
{
    const auto& characteristic = gattService.characteristics[charIndex];
    
    HILOG_DEBUG(HILOG_MODULE_ACE, "Adding characteristic %zu: %s", charIndex, characteristic.characteristicUuid.c_str());

    // 边界检查
    if (charIndex >= resultService.characteristics.size()) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Characteristic index out of bounds: %zu >= %zu", 
                   charIndex, resultService.characteristics.size());
        return BLE_INVALID_PARAMETER;
    }

    // 准备特征值信息
    gatts_add_chara_info_t charaInfo;
    memset_s(&charaInfo, sizeof(gatts_add_chara_info_t), 0, sizeof(gatts_add_chara_info_t));
    
    charaInfo.chara_uuid = ConvertUuid(characteristic.characteristicUuid);
    if (charaInfo.chara_uuid.uuid_len == 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Invalid characteristic UUID: %s", characteristic.characteristicUuid.c_str());
        return BLE_INVALID_PARAMETER;
    }

    charaInfo.properties = ConvertProperties(characteristic.properties);
    charaInfo.permissions = ConvertPermissions(characteristic.properties);
    charaInfo.value_len = characteristic.characteristicValue.size();
    if (charaInfo.value_len > 0) {
        charaInfo.value = const_cast<uint8_t*>(characteristic.characteristicValue.data());
    }

    // 添加特征值到协议栈
    gatts_add_character_result_t charaResult;
    memset_s(&charaResult, sizeof(gatts_add_character_result_t), 0, sizeof(gatts_add_character_result_t));
    errcode_t ret = gatts_add_characteristic_sync(BleModule::serverId, serviceHandle, &charaInfo, &charaResult);
    if (ret != BLE_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "AddService: gatts_add_characteristic_sync failed: %X", ret);
        return ret;
    }

    HILOG_DEBUG(HILOG_MODULE_ACE, "Characteristic added: handle=%X, value_handle=%X", 
               charaResult.handle, charaResult.value_handle);

    // 保存特征值句柄到结果服务
    resultService.characteristics[charIndex].character.declare_handle = charaResult.handle;
    resultService.characteristics[charIndex].character.value_handle = charaResult.value_handle;
    
    // 保存特征值句柄映射
    AddCharacteristicHandle(gattService.serviceUuid, characteristic.characteristicUuid, charaResult.value_handle);
    AddCharacteristicHandleMapping(charaResult.handle, gattService.serviceUuid, 
                                  characteristic.characteristicUuid, charaResult.value_handle);

    // 处理描述符
    return AddDescriptorsForCharacteristic(gattService, resultService, serviceHandle, charIndex);
}

// 添加服务流程
errcode_t PerformAddServiceOperation(const GattService& gattService, GattService& resultService, uint16_t& serviceHandle)
{
    errcode_t ret = AddServiceBase(gattService, resultService, serviceHandle);
    if (ret != BLE_SUCCESS) {
        return ret;
    }

    // 添加所有特征值
    for (size_t i = 0; i < gattService.characteristics.size(); i++) {
        ret = AddCharacteristicWithDescriptors(gattService, resultService, serviceHandle, i);
        if (ret != BLE_SUCCESS) {
            return ret;
        }
    }

    HILOG_DEBUG(HILOG_MODULE_ACE, "PerformAddServiceOperation completed successfully");
    return BLE_SUCCESS;
}

// 通过serviceUuid获取服务句柄
uint16_t GetServiceHandleByUuid(const std::string& serviceUuid) {
    auto it = BleModule::g_serviceHandleMap.find(serviceUuid);
    if (it != BleModule::g_serviceHandleMap.end()) {
        return it->second;
    }
    return 0;
}

// 执行删除服务回调
void ExecuteRemoveServiceCallback(const JSIValue thisVal, const JSIValue callback, errcode_t ret)
{
    if (JSI::ValueIsFunction(callback)) {
        JSIValue err = JSI::CreateNumber(ret);
        JSI::CallFunction(callback, thisVal, &err, 1);
        JSI::ReleaseValue(err);
    }
}

// 移除服务下的所有特征值映射
void RemoveAllCharacteristicHandlesByService(const std::string& serviceUuid)
{
    if (serviceUuid.empty()) {
        return;
    }
    
    // 规范化服务UUID用于精确匹配
    std::string cleanServiceUuid = serviceUuid;
    cleanServiceUuid.erase(std::remove(cleanServiceUuid.begin(), cleanServiceUuid.end(), '-'), cleanServiceUuid.end());
    std::transform(cleanServiceUuid.begin(), cleanServiceUuid.end(), cleanServiceUuid.begin(), ::tolower);
    
    HILOG_INFO(HILOG_MODULE_ACE, "RemoveAllCharacteristicHandlesByService: removing handles for service %s", 
               serviceUuid.c_str());
    
    // 精确匹配服务UUID部分
    for (auto it = BleModule::g_characteristicHandleMap.begin(); it != BleModule::g_characteristicHandleMap.end();) {
        // 分割键来获取服务UUID部分
        size_t separatorPos = it->first.find(':');
        if (separatorPos != std::string::npos) {
            std::string keyServiceUuid = it->first.substr(0, separatorPos);
            if (keyServiceUuid == cleanServiceUuid) {
                HILOG_INFO(HILOG_MODULE_ACE, "RemoveAllCharacteristicHandlesByService: removed %s", it->first.c_str());
                it = BleModule::g_characteristicHandleMap.erase(it);
                continue;
            }
        }
        ++it;
    }
}

// 参数验证
bool ValidateRemoveServiceParameters(uint8_t argsSize, const JSIValue *args)
{
    if (argsSize < 2) {
        HILOG_ERROR(HILOG_MODULE_ACE, "RemoveService: insufficient arguments");
        return false;
    }

    if (!JSI::ValueIsString(args[0])) {
        HILOG_ERROR(HILOG_MODULE_ACE, "RemoveService: first argument must be serviceUuid string");
        return false;
    }

    if (!JSI::ValueIsFunction(args[1])) {
        HILOG_ERROR(HILOG_MODULE_ACE, "RemoveService: second argument must be callback function");
        return false;
    }

    return true;
}

// 删除服务
JSIValue BleModule::RemoveService(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    errcode_t ret = BLE_SUCCESS;
    // 参数校验
    if (!ValidateRemoveServiceParameters(argsSize, args)) {
        ret = BLE_INVALID_PARAMETER;
        ExecuteRemoveServiceCallback(thisVal, args[1], ret);
        return JSI::CreateUndefined();
    }

    // 获取serviceUuid
    char *serviceUuidTmp = JSI::ValueToString(args[0]);
    if (serviceUuidTmp == nullptr) {
        ret = BLE_INVALID_PARAMETER;
        ExecuteRemoveServiceCallback(thisVal, args[1], ret);
        return JSI::CreateUndefined();
    }
    std::string serviceUuid(serviceUuidTmp);
    JSI::ReleaseString(serviceUuidTmp);
    if (serviceUuid.empty()) {
        HILOG_ERROR(HILOG_MODULE_ACE, "RemoveService: serviceUuid is empty");
        ret = BLE_INVALID_PARAMETER;
        ExecuteRemoveServiceCallback(thisVal, args[1], ret);
        return JSI::CreateUndefined();
    }

    // 查找服务句柄
    uint16_t serviceHandle = GetServiceHandleByUuid(serviceUuid);
    if (serviceHandle == 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "RemoveService: service not found for uuid: %s", serviceUuid.c_str());
        ret = BLE_SERVICE_NOT_FOUND;
        ExecuteRemoveServiceCallback(thisVal, args[1], ret);
        return JSI::CreateUndefined();
    }

    // 保存删除上下文
    RemoveServiceContext context;
    context.callback = JSI::AcquireValue(args[1]);
    context.thisVal = JSI::AcquireValue(thisVal);
    context.serviceUuid = serviceUuid;
    context.serviceHandle = serviceHandle;
    {
        std::lock_guard<std::mutex> lock(serviceCleanMutex);
        BleModule::g_removeServiceContexts[serviceHandle] = context;

        // 先停止服务（异步）
        ret = gatts_stop_service(serverId, serviceHandle);
        if (ret != BLE_SUCCESS) {
            HILOG_ERROR(HILOG_MODULE_ACE, "RemoveService: gatts_stop_service failed: %X", ret);
            // 清理上下文
            BleModule::g_removeServiceContexts.erase(serviceHandle);
            ExecuteRemoveServiceCallback(thisVal, args[1], ret);
            JSI::ReleaseValue(context.callback);
            JSI::ReleaseValue(context.thisVal);
        }
    }

    return JSI::CreateUndefined();
}

errcode_t BleModule::ReleaseServerConnect()
{
    std::lock_guard<std::mutex> lock(connectMapMutex);
    // 清理连接映射
    for (auto it = deviceMap.begin(); it != deviceMap.end();) {
        if (it->second.deviceType == BLE_CLIENT) {
            HILOG_INFO(HILOG_MODULE_ACE, "ReleaseServerConnect: released connection for device, connId: %X", it->second.connId);
            it = deviceMap.erase(it);
        } else {
            ++it;
        }
    }
    return 0;
}

void BleModule::ReleaseEventCallback(const std::string& eventName)
{
    auto it = eventCallbacks.find(eventName);
    if (it == eventCallbacks.end()) {
        return; // 事件不存在，直接返回
    }

    EventCallback* oldEc = it->second;
    if (oldEc != nullptr) {
        // 释放callback（若不为Undefined）
        if (!JSI::ValueIsUndefined(oldEc->callback)) {
            JSI::ReleaseValue(oldEc->callback);
            oldEc->callback = JSI::CreateUndefined();
        }
        // 释放context（若不为Undefined）
        if (!JSI::ValueIsUndefined(oldEc->context)) {
            JSI::ReleaseValue(oldEc->context);
            oldEc->context = JSI::CreateUndefined();
        }
        delete oldEc; // 释放EventCallback对象
    }

    eventCallbacks.erase(it); // 从map中移除该事件
}

void BleModule::ReleaseServerClose()
{
    HILOG_INFO(HILOG_MODULE_ACE, "BleModule::ReleaseClose called");

    // 调用子函数释放所有事件回调（原重复逻辑由子函数统一处理）
    ReleaseEventCallback("characteristicRead");
    ReleaseEventCallback("characteristicWrite");
    ReleaseEventCallback("descriptorRead");
    ReleaseEventCallback("descriptorWrite");
    ReleaseEventCallback("connectionStateChange");
    ReleaseEventCallback("BLEMtuChange");

    // 重置MTU和地址映射（原有逻辑保留）
    {
        std::lock_guard<std::mutex> lock(addrMapMutex);
        addrMap.clear();
    }

    HILOG_INFO(HILOG_MODULE_ACE, "BleModule::ReleaseClose completed");
}

JSIValue BleModule::ServerClose(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    HILOG_INFO(HILOG_MODULE_ACE, "ServerClose: starting server close");
    
    if (JSI::ValueIsUndefined(gattServer)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "ServerClose: GATT server not registered");
        return JSI::CreateNumber(BLE_OPRATION_FAILED);
    }
    
    // 调用Destroy进行完整清理
    errcode_t ret = ServerDestroy();
    
    HILOG_INFO(HILOG_MODULE_ACE, "ServerClose: completed");
    return JSI::CreateNumber(ret);
}

JSIValue BleModule::ServerOn(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
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

JSIValue BleModule::ServerOff(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
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

// 解析NotifyCharacteristic参数
errcode_t ParseNotifyCharacteristicArguments(const JSIValue* args, NotifyCharacteristicContext& context)
{
    // 解析deviceId
    char *deviceId = JSI::ValueToString(args[0]);
    if (deviceId == nullptr) {
        return BLE_INVALID_PARAMETER;
    }
    context.deviceId = deviceId;
    JSI::ReleaseString(deviceId);
    if (context.deviceId.empty()) {
        return BLE_INVALID_PARAMETER;
    }

    // 解析NotifyCharacteristic对象
    JSIValue jsiNotifyChar = JSI::AcquireValue(args[1]);
    if (JSI::ValueIsUndefined(jsiNotifyChar)) {
        return BLE_INVALID_PARAMETER;
    }

    // 解析serviceUuid
    char* serviceUuid = JSI::GetStringProperty(jsiNotifyChar, "serviceUuid");
    context.serviceUuid = serviceUuid;
    JSI::ReleaseString(serviceUuid);
    if (context.serviceUuid.empty()) {
        JSI::ReleaseValue(jsiNotifyChar);
        return BLE_INVALID_PARAMETER;
    }
    
    // 解析characteristicUuid
    char* characteristicUuid = JSI::GetStringProperty(jsiNotifyChar, "characteristicUuid");
    context.characteristicUuid = characteristicUuid;
    JSI::ReleaseString(characteristicUuid);
    if (context.characteristicUuid.empty()) {
        JSI::ReleaseValue(jsiNotifyChar);
        return BLE_INVALID_PARAMETER;
    }
    
    // 解析confirm
    JSIValue jsiConfirm = JSI::GetNamedProperty(jsiNotifyChar, "confirm");
    if (!JSI::ValueIsUndefined(jsiConfirm)) {
        context.confirm = JSI::ValueToBoolean(jsiConfirm);
        JSI::ReleaseValue(jsiConfirm);
    } else {
        context.confirm = false; // 默认为通知
    }

    // 解析characteristicValue
    JSIValue jsiValue = JSI::GetNamedProperty(jsiNotifyChar, "characteristicValue");
    if (!JSI::ValueIsUndefined(jsiValue)) {
        size_t valueLen = 0;
        uint8_t* valueData = JSI::GetArrayBufferInfo(jsiValue, valueLen);
        if (valueData && valueLen > 0) {
            context.characteristicValue.reserve(valueLen);
            context.characteristicValue.assign(valueData, valueData + valueLen);
        }
        JSI::ReleaseValue(jsiValue);
    } else {
        // 特征值可以为空，但通常应该有数据
        HILOG_WARN(HILOG_MODULE_ACE, "ParseNotifyCharacteristicArguments: characteristicValue is empty");
    }

    JSI::ReleaseValue(jsiNotifyChar);
    return BLE_SUCCESS;
}

// 查找特征值句柄
uint16_t FindCharacteristicHandle(const std::string& serviceUuid, const std::string& characteristicUuid)
{
    if (serviceUuid.empty() || characteristicUuid.empty()) {
        HILOG_ERROR(HILOG_MODULE_ACE, "FindCharacteristicHandle: invalid parameters");
        return 0;
    }

    std::string key = GenerateCharacteristicKey(serviceUuid, characteristicUuid);
    auto it = BleModule::g_characteristicHandleMap.find(key);
    if (it != BleModule::g_characteristicHandleMap.end()) {
        HILOG_INFO(HILOG_MODULE_ACE, "FindCharacteristicHandle: found handle %X for service %s, chara %s", 
                   it->second, serviceUuid.c_str(), characteristicUuid.c_str());
        return it->second;
    }

    HILOG_ERROR(HILOG_MODULE_ACE, "FindCharacteristicHandle: characteristic not found, service: %s, chara: %s", 
                serviceUuid.c_str(), characteristicUuid.c_str());
    return 0;
}

// 执行通知回调
static void ExecuteNotifyCallback(const JSIValue thisVal, const JSIValue callback, errcode_t ret)
{
    if (JSI::ValueIsFunction(callback)) {
        JSIValue err = JSI::CreateNumber(ret);
        JSI::CallFunction(callback, thisVal, &err, 1);
        JSI::ReleaseValue(err);
    }
}

// 参数验证
bool ValidateNotifyCharacteristicParameters(uint8_t argsSize, const JSIValue *args)
{
    if (argsSize < 3) {
        HILOG_ERROR(HILOG_MODULE_ACE, "NotifyCharacteristicChanged: insufficient arguments, expected 3, got %d", argsSize);
        return false;
    }

    // 验证参数有效性
    if (!JSI::ValueIsString(args[0])) {
        HILOG_ERROR(HILOG_MODULE_ACE, "NotifyCharacteristicChanged: invalid deviceId");
        return false;
    }

    if (!JSI::ValueIsObject(args[1])) {
        HILOG_ERROR(HILOG_MODULE_ACE, "NotifyCharacteristicChanged: second argument must be NotifyCharacteristic object");
        return false;
    }

    if (!JSI::ValueIsFunction(args[2])) {
        HILOG_ERROR(HILOG_MODULE_ACE, "NotifyCharacteristicChanged: third argument must be callback function");
        return false;
    }

    return true;
}

bool GetConnIdByDeviceId(const std::string& deviceId, uint16_t *connId)
{
    std::lock_guard<std::mutex> lock(BleModule::connectMapMutex);
    auto it = BleModule::deviceMap.find(deviceId);
    if (it != BleModule::deviceMap.end()) {
        *connId = it->second.connId;
        return true;  // 找到
    }
    *connId = 0;
    return false;  // 未找到
}

// 通知特征值变化
JSIValue BleModule::NotifyCharacteristicChanged(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    errcode_t ret = BLE_SUCCESS;
    NotifyCharacteristicContext context;

    // 参数校验 - 在访问args[2]之前检查
    if (!ValidateNotifyCharacteristicParameters(argsSize, args)) {
        ret = BLE_INVALID_PARAMETER;
        // 只在参数足够时才执行回调
        if (argsSize >= 3) {
            ExecuteNotifyCallback(thisVal, args[2], ret);
        } else {
            HILOG_ERROR(HILOG_MODULE_ACE, "NotifyCharacteristicChanged: insufficient arguments for callback, expected 3, got %d", argsSize);
        }
        return JSI::CreateUndefined();
    }

    // 解析参数
    ret = ParseNotifyCharacteristicArguments(args, context);
    if (ret != BLE_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "NotifyCharacteristicChanged: parse arguments failed");
        // 确保args[2]存在
        ExecuteNotifyCallback(thisVal, args[2], ret);
        return JSI::CreateUndefined();
    }

    // 获取连接ID
    uint16_t connId = 0;
    if (!GetConnIdByDeviceId(context.deviceId, &connId) || connId == 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "NotifyCharacteristicChanged: invalid connection for device: %s, connId:%d", context.deviceId.c_str(), connId);
        ExecuteNotifyCallback(thisVal, args[2], BLE_OPRATION_FAILED);
        return JSI::CreateUndefined();
    }

    // 查找特征值句柄
    uint16_t charHandle = FindCharacteristicHandle(context.serviceUuid, context.characteristicUuid);
    if (charHandle == 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "NotifyCharacteristicChanged: characteristic not found, service: %s, chara: %s", 
                    context.serviceUuid.c_str(), context.characteristicUuid.c_str());
        ExecuteNotifyCallback(thisVal, args[2], BLE_OPRATION_FAILED);
        return JSI::CreateUndefined();
    }

    // 准备通知/指示数据
    gatts_ntf_ind_t ntfInd;
    memset_s(&ntfInd, sizeof(gatts_ntf_ind_t), 0, sizeof(gatts_ntf_ind_t));

    ntfInd.attr_handle = charHandle;
    ntfInd.value_len = context.characteristicValue.size();

    if (ntfInd.value_len > 0) {
        ntfInd.value = context.characteristicValue.data();
    } else {
        // 即使数据为空也要发送，但通常应该有数据
        ntfInd.value = nullptr;
    }

    // 保存回调上下文
    context.callback = JSI::AcquireValue(args[2]);
    context.thisVal = JSI::AcquireValue(thisVal);
    BleModule::g_notifyContexts[connId] = context;
    
    // 调用协议栈发送通知/指示
    ret = gatts_notify_indicate(serverId, connId, &ntfInd);
    if (ret != BLE_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE,
           "NotifyCharacteristicChanged: gatts_notify_indicate failed: %X, connId: %X, handle: %X", ret,
           connId, charHandle);
    } else {
        HILOG_INFO(HILOG_MODULE_ACE,
            "NotifyCharacteristicChanged: success, type: %s, connId: %X, handle: %X",
            context.confirm ? "indicate" : "notify", connId, charHandle);
    }
    
    // 立即执行回调
    ExecuteNotifyCallback(context.thisVal, context.callback, ret);
    
    // 清理资源
    JSI::ReleaseValue(context.callback);
    JSI::ReleaseValue(context.thisVal);
    BleModule::g_notifyContexts.erase(connId);

    return JSI::CreateUndefined();
}

// 清理所有服务构建上下文
static void CleanAllServiceBuildContexts()
{
    for (auto& context : BleModule::g_serviceBuildContexts) {
        delete context.second.originalService;
        delete context.second.resultService;
        JSI::ReleaseValue(context.second.callback);
        JSI::ReleaseValue(context.second.thisVal);
    }
    BleModule::g_serviceBuildContexts.clear();
}

// 清理所有事件回调
static void CleanAllEventCallbacks()
{
    // 使用现有的ReleaseEventCallback函数清理每个事件
    BleModule::ReleaseEventCallback("characteristicRead");
    BleModule::ReleaseEventCallback("characteristicWrite");
    BleModule::ReleaseEventCallback("descriptorRead");
    BleModule::ReleaseEventCallback("descriptorWrite");
    BleModule::ReleaseEventCallback("connectionStateChange");
    BleModule::ReleaseEventCallback("BLEMtuChange");
}

// 停止单个服务（异步）
static void StopServiceAsync(uint16_t serviceHandle, const std::string& serviceUuid)
{
    // 创建删除上下文（使用空回调，因为我们在销毁过程中不需要JS回调）
    RemoveServiceContext context;
    context.serviceHandle = serviceHandle;
    context.serviceUuid = serviceUuid;

    {
        std::lock_guard<std::mutex> lock(BleModule::serviceCleanMutex);
        BleModule::g_removeServiceContexts[serviceHandle] = context;
    }

    // 异步停止服务，会在GattsStopServiceCbk中自动删除服务
    errcode_t ret = gatts_stop_service(BleModule::serverId, serviceHandle);
    if (ret != BLE_SUCCESS) {
        HILOG_WARN(HILOG_MODULE_ACE, "StopServiceAsync: stop service %X failed: %X", serviceHandle, ret);

        // 停止失败，直接清理本地映射
        RemoveAllCharacteristicHandlesByService(serviceUuid);

        // 清理特征值和描述符映射
        for (auto it = BleModule::g_handleToCharacteristicMap.begin(); it != BleModule::g_handleToCharacteristicMap.end();) {
            if (it->second.serviceUuid == serviceUuid) {
                it = BleModule::g_handleToCharacteristicMap.erase(it);
            } else {
                ++it;
            }
        }

        for (auto it = BleModule::g_handleToDescriptorMap.begin(); it != BleModule::g_handleToDescriptorMap.end();) {
            if (it->second.serviceUuid == serviceUuid) {
                it = BleModule::g_handleToDescriptorMap.erase(it);
            } else {
                ++it;
            }
        }

        // 从上下文中移除
        std::lock_guard<std::mutex> lock(BleModule::serviceCleanMutex);
        BleModule::g_removeServiceContexts.erase(serviceHandle);
    } else {
        HILOG_INFO(HILOG_MODULE_ACE, "StopServiceAsync: service %s stop initiated", serviceUuid.c_str());
    }
}

// 主要的Destroy函数实现
errcode_t BleModule::ServerDestroy()
{
    HILOG_INFO(HILOG_MODULE_ACE, "BleModule::Destroy: starting resource cleanup");
    
    errcode_t finalRet = BLE_SUCCESS;
    
    // 1. 异步停止所有服务
    if (!g_serviceHandleMap.empty()) {
        HILOG_INFO(HILOG_MODULE_ACE, "Destroy: stopping %zu services", g_serviceHandleMap.size());

        // 复制服务列表，避免在遍历时修改容器
        std::vector<std::pair<std::string, uint16_t>> services(g_serviceHandleMap.begin(), g_serviceHandleMap.end());

        for (const auto& service : services) {
            const std::string& serviceUuid = service.first;
            uint16_t serviceHandle = service.second;

            HILOG_INFO(HILOG_MODULE_ACE, "Destroy: stopping service %s (handle: %X)", 
                       serviceUuid.c_str(), serviceHandle);

            StopServiceAsync(serviceHandle, serviceUuid);
        }

        // 清空服务映射表，服务会在异步回调中被真正删除
        g_serviceHandleMap.clear();
    }

    // 2. 清理所有构建上下文（这些是未完成的服务添加操作）
    CleanAllServiceBuildContexts();

    // 3. 清理其他资源
    g_characteristicHandleMap.clear();
    g_handleToCharacteristicMap.clear();
    g_handleToDescriptorMap.clear();
    g_notifyContexts.clear();

    // 4. 释放JS对象
    if (!JSI::ValueIsUndefined(gattServer)) {
        JSI::ReleaseValue(gattServer);
        gattServer = JSI::CreateUndefined();
    }

    // 5. 清理事件回调和连接
    CleanAllEventCallbacks();
    ReleaseServerConnect();
    ReleaseServerClose();

    // 6. 重置状态
    hasOn = false;

    HILOG_INFO(HILOG_MODULE_ACE, "BleModule::Destroy: cleanup completed");
    return finalRet;
}
}  // namespace ACELite
}  // namespace OHOS

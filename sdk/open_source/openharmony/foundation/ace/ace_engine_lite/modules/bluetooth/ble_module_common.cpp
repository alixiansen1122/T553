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

#include "ble_module_common.h"
#include "ace_log.h"
#include "securec.h"
#include <algorithm>
#include <cctype>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/time.h>

namespace OHOS {
namespace ACELite {

/* 十六进制字符转数值 */
uint8_t Char2Num(int8_t c)
{
    if (c >= '0' && c <= '9') {
        return (uint8_t)(c - '0');
    } else if (c >= 'A' && c <= 'F') {
        return (uint8_t)(c - 'A' + 10);
    } else if (c >= 'a' && c <= 'f') {
        return (uint8_t)(c - 'a' + 10);
    } else {
        return 0xFF;
    }
}

// 比较Uuid，先去除非16进制字符，再转小写比较
bool CompareHexStrings(const std::string& str1, const std::string& str2)
{
    // 处理第一个字符串
    std::string processed1;
    for (char c : str1) {
        if (isxdigit(c)) { // 检查是否为十六进制字符
            processed1 += tolower(c); // 转换为小写
        }
    }
    
    // 处理第二个字符串
    std::string processed2;
    for (char c : str2) {
        if (isxdigit(c)) { // 检查是否为十六进制字符
            processed2 += tolower(c); // 转换为小写
        }
    }
    
    // 比较处理后的字符串
    return processed1 == processed2;
}

/* 从字符串中解析广播uuid和deviceId，先去除冒号分隔符，再转换成小端序 */
errcode_t DecodeUuidFromStr(const char *uuidStr, uint8_t *uuid, int32_t *cnt)
{
    // 1. 创建过滤后的缓冲区（移除所有非十六进制字符）
    char filtered[32 + 1] = {0}; // 最大32字符 + 结束符
    int32_t filteredIdx = 0;

    // 2. 过滤非十六进制字符
    for (int i = 0; uuidStr[i] != '\0'; i++) {
        // 检查是否为十六进制字符（0-9, a-f, A-F）
        if (std::isxdigit(static_cast<unsigned char>(uuidStr[i]))) {
            // 确保不超过缓冲区大小
            if (filteredIdx >= sizeof(filtered) - 1) {
                HILOG_ERROR(HILOG_MODULE_ACE, "Filtered string exceeds buffer size!");
                return -1;
            }
            filtered[filteredIdx++] = uuidStr[i];
        }
    }
    filtered[filteredIdx] = '\0'; // 添加结束符
    
    // 3. 检查过滤后字符串长度
    size_t filteredLen = strlen(filtered);
    if ((filteredLen > 16 * 2) || (filteredLen % 2 != 0) || filteredLen == 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Invalid filtered uuid length: %zu", filteredLen);
        return -1;
    }

    // 4. 从过滤后的字符串解析UUID（小端序）
    *cnt = 0;
    for (int32_t i = (int32_t)filteredLen - 1; i >= 0; i -= 2) {
        uuid[*cnt] = (Char2Num(filtered[i - 1]) << 4) | Char2Num(filtered[i]);
        (*cnt)++;
    }

    return 0;
}

/* 从MAC地址中解析deviceId，先转大端序，再转字符串并添加冒号 */
std::string FormatMacAddress(const uint8_t addr[BLE_ADDR_LEN])
{
    // 创建临时副本以反转字节序
    uint8_t reversedAddr[BLE_ADDR_LEN];
    std::reverse_copy(addr, addr + BLE_ADDR_LEN, reversedAddr);

    // 十六进制字符映射表
    constexpr char hexChars[] = "0123456789abcdef";

    // 预分配空间
    std::string result;
    result.reserve(sizeof(hexChars));

    // 处理反转后的字节
    for (int i = 0; i < BLE_ADDR_LEN; ++i) {
        // 处理高4位
        result += hexChars[(reversedAddr[i] >> 4) & 0x0F];
        // 处理低4位
        result += hexChars[reversedAddr[i] & 0x0F];

        // 添加分隔符（前5个字节后）
        if (i < 5) {
            result += ':';
        }
    }
    
    return result;
}

errcode_t SetJsiBLECharacteristic(JSIValue jsiBleCharacteristic, uint16_t connId,
    BLECharacteristic *bleCharacteristicArgs)
{
    // 设置服务UUID和特征值UUID
    JSI::SetStringProperty(jsiBleCharacteristic, "serviceUuid", bleCharacteristicArgs->serviceUuid.c_str());
    JSI::SetStringProperty(jsiBleCharacteristic, "characteristicUuid",
        bleCharacteristicArgs->characteristicUuid.c_str());

    // 设置特征值数据
    uint8_t *sticValuePtr = nullptr;
    uint32_t bufferLength = bleCharacteristicArgs->characteristicValue.size();
    JSIValue valueArray = JSI::CreateArrayBuffer(bufferLength, sticValuePtr);
    // 检查分配是否成功
    if (bufferLength > 0 && sticValuePtr == nullptr) {
        JSI::ReleaseValueList(valueArray);
        HILOG_ERROR(HILOG_MODULE_ACE, "CreateArrayBuffer for characteristic Value failed!");
        return BLE_OPRATION_FAILED;
    }
    if (memcpy_s(sticValuePtr, bufferLength, bleCharacteristicArgs->characteristicValue.data(),
        bufferLength) != EOK) {
        JSI::ReleaseValueList(valueArray);
        HILOG_ERROR(HILOG_MODULE_ACE, "memcpy_s characteristic Value failed!");
        return BLE_OPRATION_FAILED;
    }
    JSI::SetNamedProperty(jsiBleCharacteristic, "characteristicValue", valueArray);
    JSI::ReleaseValue(valueArray);

    // 设置描述符
    JSIValue descriptorsArray = JSI::CreateArray(bleCharacteristicArgs->descriptors.size());
    for (uint32_t i = 0; i < bleCharacteristicArgs->descriptors.size(); i++) {
        JSIValue jsiDescriptor = JSI::CreateObject();
        JSI::SetStringProperty(
            jsiDescriptor, "serviceUuid",bleCharacteristicArgs->descriptors[i].serviceUuid.c_str());
        JSI::SetStringProperty(
            jsiDescriptor, "characteristicUuid", bleCharacteristicArgs->descriptors[i].characteristicUuid.c_str());
        JSI::SetStringProperty(
            jsiDescriptor, "descriptorUuid", bleCharacteristicArgs->descriptors[i].descriptorUuid.c_str());

        // 描述符值
        uint32_t descValueBufLen = bleCharacteristicArgs->descriptors[i].descriptorValue.size();
        if (descValueBufLen != 0) {
            uint8_t *descValuePtr = nullptr;
            JSIValue descValue = JSI::CreateArrayBuffer(descValueBufLen, descValuePtr);
            // 检查分配是否成功
            if (descValueBufLen > 0 && descValuePtr == nullptr) {
                JSI::ReleaseValueList(descValue, jsiDescriptor, descriptorsArray, jsiBleCharacteristic);
                HILOG_ERROR(HILOG_MODULE_ACE, "CreateArrayBuffer for descriptor value failed!");
                return BLE_OPRATION_FAILED;
            }
            if (memcpy_s(descValuePtr, descValueBufLen, bleCharacteristicArgs->descriptors[i].descriptorValue.data(),
                    descValueBufLen) != EOK) {
                JSI::ReleaseValueList(descValue, jsiDescriptor, descriptorsArray, jsiBleCharacteristic);
                HILOG_ERROR(HILOG_MODULE_ACE, "memcpy_s descriptor value failed!");
                return BLE_OPRATION_FAILED;
            }
            JSI::SetNamedProperty(jsiDescriptor, "descriptorValue", descValue);
            JSI::ReleaseValue(descValue);
        }
        JSI::SetNumberProperty(
            jsiDescriptor, "descriptorHandle", bleCharacteristicArgs->descriptors[i].descriptor.descriptor_hdl);
        JSI::SetPropertyByIndex(descriptorsArray, i, jsiDescriptor);
        JSI::ReleaseValue(jsiDescriptor);
    }
    JSI::SetNumberProperty(jsiBleCharacteristic, "characteristicValueHandle",
        bleCharacteristicArgs->character.declare_handle);
    JSI::SetNamedProperty(jsiBleCharacteristic, "descriptors", descriptorsArray);
    JSI::ReleaseValue(descriptorsArray);

    // 设置属性
    JSIValue jsiProperties = JSI::CreateObject();
    JSI::SetBooleanProperty(jsiProperties, "write", bleCharacteristicArgs->properties.write);
    JSI::SetBooleanProperty(jsiProperties, "writeNoResponse", bleCharacteristicArgs->properties.writeNoResponse);
    JSI::SetBooleanProperty(jsiProperties, "read", bleCharacteristicArgs->properties.read);
    JSI::SetBooleanProperty(jsiProperties, "notify", bleCharacteristicArgs->properties.notify);
    JSI::SetBooleanProperty(jsiProperties, "indicate", bleCharacteristicArgs->properties.indicate);
    JSI::SetNamedProperty(jsiBleCharacteristic, "properties", jsiProperties);
    JSI::ReleaseValue(jsiProperties);

    // 添加连接ID信息
    JSI::SetNumberProperty(jsiBleCharacteristic, "connId", connId);
    return BLE_SUCCESS;
}

// 通用UUID解析函数
errcode_t ParseUuidList(const uint8_t *data, uint8_t len, uint8_t uuid_size, std::vector<std::string> &uuid_list)
{
    if (uuid_size == 0 || len == 0)
        return BLE_OPRATION_FAILED;

    int count = len / uuid_size;
    for (int i = 0; i < count; i++) {
        const uint8_t *uuid_data = data + (i * uuid_size);
        char uuid_str[37] = {0};
        int ret = 0;

        switch (uuid_size) {
            case 2: {  // 16-bit UUID
                uint16_t uuid = (uuid_data[1] << 8) | uuid_data[0];
                ret = snprintf_s(uuid_str, sizeof(uuid_str), sizeof(uuid_str) - 1, "%04X", uuid);
                if (ret < 0) {
                    HILOG_ERROR(HILOG_MODULE_ACE, "ParseUuidList snprintf_s for UUID failed, ret:%X", ret);
                    return BLE_OPRATION_FAILED;
                }
                break;
            }
            case 4: {  // 32-bit UUID
                uint32_t uuid = (uuid_data[3] << 24) | (uuid_data[2] << 16) | (uuid_data[1] << 8) | uuid_data[0];
                ret = snprintf_s(uuid_str, sizeof(uuid_str), sizeof(uuid_str) - 1, "%08X", uuid);
                if (ret < 0) {
                    HILOG_ERROR(HILOG_MODULE_ACE, "ParseUuidList snprintf_s for UUID failed, ret:%X", ret);
                    return BLE_OPRATION_FAILED;
                }
                break;
            }
            case 16: {  // 128-bit UUID
                ret = snprintf_s(uuid_str, sizeof(uuid_str), sizeof(uuid_str) - 1,
                    "%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X",
                    uuid_data[0], uuid_data[1], uuid_data[2], uuid_data[3], uuid_data[4],
                    uuid_data[5], uuid_data[6], uuid_data[7], uuid_data[8], uuid_data[9],
                    uuid_data[10], uuid_data[11], uuid_data[12], uuid_data[13], uuid_data[14], uuid_data[15]);
                if (ret < 0) {
                    HILOG_ERROR(HILOG_MODULE_ACE, "ParseUuidList snprintf_s for UUID failed, ret:%X", ret);
                    return BLE_OPRATION_FAILED;
                }
                break;
            }
            default:
                HILOG_WARN(HILOG_MODULE_ACE, "Unsupported UUID size: %d", uuid_size);
                return BLE_INVALID_PARAMETER;
        }

        if (ret <= 0) {
            HILOG_ERROR(HILOG_MODULE_ACE, "snprintf_s for UUID failed, size=%d", uuid_size);
        } else {
            uuid_list.push_back(uuid_str);
        }
    }
    return 0;
}

// 标准化UUID格式（移除连字符并转为大写）
std::string NormalizeUuid(const std::string &uuid)
{
    std::string normalized;
    normalized.reserve(uuid.length());

    for (char c : uuid) {
        if (c == '-') continue;
        normalized.push_back(std::toupper(c));
    }

    return normalized;
}

bool CheckUuidMatch(const std::string &uuid, const std::string &filter, const std::string &mask)
{
    // 标准化UUID格式（移除连字符并转为大写）
    std::string normalizedUuid = NormalizeUuid(uuid);
    std::string normalizedFilter = NormalizeUuid(filter);
    std::string normalizedMask = NormalizeUuid(mask);

    // 检查长度是否匹配
    if (normalizedUuid.length() != normalizedFilter.length() || normalizedUuid.length() != normalizedMask.length()) {
        HILOG_ERROR(HILOG_MODULE_ACE, "UUID length mismatch: uuid=%d, filter=%d, mask=%d", normalizedUuid.length(),
            normalizedFilter.length(), normalizedMask.length());
        return false;
    }

    for (size_t i = 0; i < normalizedUuid.length(); i++) {
        char u = normalizedUuid[i];
        char f = normalizedFilter[i];
        char m = normalizedMask[i];

        if (m == 'F') {
            if (u != f) {
                HILOG_ERROR(HILOG_MODULE_ACE, "UUID char mismatch at %d: %c vs %c (mask F)", i, u, f);
                return false;
            }
        } else if (m == '0') {
            // 不比较此位
            continue;
        } else {
            // 处理部分掩码
            int u_val = (u >= 'A') ? (u - 'A' + 10) : (u - '0');
            int f_val = (f >= 'A') ? (f - 'A' + 10) : (f - '0');
            int m_val = (m >= 'A') ? (m - 'A' + 10) : (m - '0');
            if ((u_val & m_val) != (f_val & m_val)) {
                HILOG_ERROR(HILOG_MODULE_ACE, "UUID mask mismatch at %d: u=%d, f=%d, m=%d", i, u_val, f_val, m_val);
                return false;
            }
        }
    }
    return true;
}

std::string GetUUidStr(const bt_uuid_t &uuid)
{
    const char hex_chars[] = "0123456789abcdef";
    std::string result;
    result.reserve(uuid.uuid_len * 2);

    if (uuid.uuid_len == 2) {  // 按ble协议，2字节UUID是大端序，直接按顺序处理
        for (int i = 0; i < 2; ++i) {
            result += hex_chars[(uuid.uuid[i] >> 4) & 0x0F];
            result += hex_chars[uuid.uuid[i] & 0x0F];
        }
    } else if (uuid.uuid_len == 16) { // 按ble协议，16字节UUID是小端序，直接按顺序处理
        for (int i = 0; i < 16; ++i) {
            result += hex_chars[(uuid.uuid[i] >> 4) & 0x0F];
            result += hex_chars[uuid.uuid[i] & 0x0F];
        }
    } else { // 4字节当前没有使用，保留
        return "INVALID_LENGTH";
    }

    return result;
}

void GetAttValueData(uint8_t *dataVec, bool enable, bool indication)
{
    // 开关：小端序，开：0x01 0x00  关：0x00 0x00，根据上面enable传下来进行判断是开还是关；
    // indication使能：小端序，使能：0x02 0x00  去使能：0x00 0x00，根据上面enable传下来进行判断使能还是去使能；
    if (enable) {
        if (indication) {
            dataVec[0] = 0x02;
            dataVec[1] = 0x00;
        } else {
            dataVec[0] = 0x01;
            dataVec[1] = 0x00;
        }
    } else {
        dataVec[0] = 0x00;
        dataVec[1] = 0x00;
    }
}

void GetProperty(const JSIValue jsiCharacteristic, BLECharacteristic &bleCharacteristicArgs)
{
    JSIValue jsiProperties = JSI::GetNamedProperty(jsiCharacteristic, "properties");
    if (JSI::ValueIsUndefined(jsiProperties)) {
        return;
    }
    JSIValue jsiWrite = JSI::GetNamedProperty(jsiProperties, "write");
    if (!JSI::ValueIsUndefined(jsiWrite)) {
       bleCharacteristicArgs.properties.write = JSI::ValueToBoolean(jsiWrite);
       JSI::ReleaseValue(jsiWrite);
    }
    JSIValue jsiWriteNoResponse = JSI::GetNamedProperty(jsiProperties, "writeNoResponse");
    if (!JSI::ValueIsUndefined(jsiWriteNoResponse)) {
       bleCharacteristicArgs.properties.writeNoResponse = JSI::ValueToBoolean(jsiWriteNoResponse);
       JSI::ReleaseValue(jsiWriteNoResponse);
    }
    JSIValue jsiRead = JSI::GetNamedProperty(jsiProperties, "read");
    if (!JSI::ValueIsUndefined(jsiRead)) {
       bleCharacteristicArgs.properties.read = JSI::ValueToBoolean(jsiRead);
       JSI::ReleaseValue(jsiRead);
    }
    JSIValue jsiNotify = JSI::GetNamedProperty(jsiProperties, "notify");
    if (!JSI::ValueIsUndefined(jsiNotify)) {
       bleCharacteristicArgs.properties.notify = JSI::ValueToBoolean(jsiNotify);
       JSI::ReleaseValue(jsiNotify);
    }
    JSIValue jsiIndicate = JSI::GetNamedProperty(jsiProperties, "indicate");
    if (!JSI::ValueIsUndefined(jsiIndicate)) {
       bleCharacteristicArgs.properties.indicate = JSI::ValueToBoolean(jsiIndicate);
       JSI::ReleaseValue(jsiIndicate);
    }
    JSI::ReleaseValue(jsiProperties);
}

// 参数校验函数
bool ValidateWriteParameters(uint8_t argsSize, const JSIValue* args)
{
    const uint8_t MIN_REQUIRED_ARGS = 4; // 最多4个参数
    // 前三个为必选参数，第4个为可选参数，若参数个数小于3或大于4，报错无效参数
    if (argsSize < ARGC_THREE || argsSize > MIN_REQUIRED_ARGS || !JSI::ValueIsFunction(args[ARGC_TWO])) {
        HILOG_ERROR(HILOG_MODULE_ACE, "WriteCharacteristic: invalid parameter!");
        return false;
    }
    return true;
}

std::string ParseDeviceId(const JSIValue args)
{
    std::string deviceId;
    JSIValue jsiDeviceId = JSI::AcquireValue(args);
    char *deviceIdTmp = JSI::ValueToString(jsiDeviceId);
    JSI::ReleaseValue(jsiDeviceId);
    if (deviceIdTmp == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "get deviceId failed!");
        return deviceId;
    }
    deviceId = deviceIdTmp;
    JSI::ReleaseString(deviceIdTmp);
    return deviceId;
}

// 解析参数函数
errcode_t ParseWriteArguments(const JSIValue* args, BLECharacteristic& bleCharacteristicArgs, 
                                    GattWriteType& writeType)
{
    JSIValue jsiWriteType = JSI::AcquireValue(args[1]);
    writeType = static_cast<GattWriteType>(JSI::ValueToNumber(jsiWriteType));
    JSI::ReleaseValue(jsiWriteType);
    
    GetCharacteristic(args, bleCharacteristicArgs, false);
    return BLE_SUCCESS;
}

void GetCharacteristic(const JSIValue *args, BLECharacteristic &bleCharacteristicArgs, bool isRead)
{
    JSIValue jsiCharacteristic = JSI::AcquireValue(args[0]);
    char* serviceUuid = JSI::GetStringProperty(jsiCharacteristic, "serviceUuid");
    bleCharacteristicArgs.serviceUuid = serviceUuid;
    JSI::ReleaseString(serviceUuid);

    char* characteristicUuid = JSI::GetStringProperty(jsiCharacteristic, "characteristicUuid");
    bleCharacteristicArgs.characteristicUuid = characteristicUuid;
    JSI::ReleaseString(characteristicUuid);
    JSIValue jsiCharactHandle = JSI::GetNamedProperty(jsiCharacteristic, "characteristicValueHandle");
    if (!JSI::ValueIsUndefined(jsiCharactHandle)) {
        bleCharacteristicArgs.character.declare_handle = JSI::ValueToNumber(jsiCharactHandle);
        JSI::ReleaseValue(jsiCharactHandle);
    } else {
        bleCharacteristicArgs.character.declare_handle = BLE_INVALID_CHARACTERISTIC_HANDLE;
    }
    if (!isRead) {
        JSIValue jsiCharacteristicValue = JSI::GetNamedProperty(jsiCharacteristic, "characteristicValue");
        size_t characteristicValueLen = 0;
        if (!JSI::ValueIsUndefined(jsiCharacteristicValue)) {
            uint8_t *characteristicValue = JSI::GetArrayBufferInfo(jsiCharacteristicValue, characteristicValueLen);
            if (characteristicValue) {
                bleCharacteristicArgs.characteristicValue.assign(
                    characteristicValue, characteristicValue + characteristicValueLen);
            }
            JSI::ReleaseValue(jsiCharacteristicValue);
        }
        JSIValue jsiDescriptors = JSI::GetNamedProperty(jsiCharacteristic, "descriptors");
        if (!JSI::ValueIsUndefined(jsiDescriptors)) {
            int32_t descriptorsLen = JSI::GetArrayLength(jsiDescriptors);
            bleCharacteristicArgs.descriptors.resize(descriptorsLen);
            for (int32_t i = 0; i < descriptorsLen; i++) {
                JSIValue jsiDescriptor = JSI::GetPropertyByIndex(jsiDescriptors, i);
                char* serviceUuid = JSI::GetStringProperty(jsiDescriptor, "serviceUuid");
                bleCharacteristicArgs.descriptors[i].serviceUuid = serviceUuid;
                JSI::ReleaseString(serviceUuid);

                char* characteristicUuid = JSI::GetStringProperty(jsiDescriptor, "characteristicUuid");
                bleCharacteristicArgs.descriptors[i].characteristicUuid = characteristicUuid;
                JSI::ReleaseString(characteristicUuid);

                char* descriptorUuid = JSI::GetStringProperty(jsiDescriptor, "descriptorUuid");
                bleCharacteristicArgs.descriptors[i].descriptorUuid = descriptorUuid;
                JSI::ReleaseString(descriptorUuid);

                JSIValue jsiDescriptorValue = JSI::GetNamedProperty(jsiDescriptor, "descriptorValue");
                size_t descriptorValueLen = 0;
                if (!JSI::ValueIsUndefined(jsiDescriptorValue)) {
                    uint8_t *descriptorValue = JSI::GetArrayBufferInfo(jsiDescriptorValue, descriptorValueLen);
                    if (descriptorValue) {
                        bleCharacteristicArgs.descriptors[i].descriptorValue.assign(descriptorValue, descriptorValue +
                            descriptorValueLen);
                    }
                    JSI::ReleaseValue(jsiDescriptorValue);
                }
                JSIValue jsiDescriptorHandle = JSI::GetNamedProperty(jsiDescriptor, "descriptorHandle");
                if (!JSI::ValueIsUndefined(jsiDescriptorHandle)) {
                    bleCharacteristicArgs.descriptors[i].descriptor.descriptor_hdl = JSI::ValueToNumber(jsiDescriptorHandle);
                    JSI::ReleaseValue(jsiDescriptorHandle);
                } else {
                    bleCharacteristicArgs.descriptors[i].descriptor.descriptor_hdl = BLE_INVALID_DESCRIPTOR_HANDLE;
                }
                JSI::ReleaseValue(jsiDescriptor);
            }
            JSI::ReleaseValue(jsiDescriptors);
        }
        GetProperty(jsiCharacteristic, bleCharacteristicArgs);
    }
    JSI::ReleaseValue(jsiCharacteristic);
}

void GetProperties(uint8_t properties, GattProperties &props)
{
    props.write = (properties & 0x08) != 0;
    props.writeNoResponse = (properties & 0x04) != 0;
    props.read = (properties & 0x02) != 0;
    props.notify = (properties & 0x10) != 0;
    props.indicate = (properties & 0x20) != 0;
}

void CreateGattServices(JSIValue *rltArray, const std::vector<GattService> &services)
{
    *rltArray = JSI::CreateArray(services.size());
    uint32_t serviceIndex = 0;

    for (const auto &service : services) {
        JSIValue jsiService = JSI::CreateObject();
        JSI::SetStringProperty(jsiService, "serviceUuid", service.serviceUuid.c_str());
        JSI::SetBooleanProperty(jsiService, "isPrimary", service.isPrimary);

        // 处理特征值数组
        JSIValue characteristicsArray = JSI::CreateArray(service.characteristics.size());
        uint32_t charIndex = 0;

        for (const auto &characteristic : service.characteristics) {
            JSIValue jsiChar = JSI::CreateObject();
            JSI::SetStringProperty(jsiChar, "serviceUuid", characteristic.serviceUuid.c_str());
            JSI::SetStringProperty(jsiChar, "characteristicUuid", characteristic.characteristicUuid.c_str());

            // 处理描述符数组
            JSIValue descriptorsArray = JSI::CreateArray(characteristic.descriptors.size());
            uint32_t descIndex = 0;

            for (const auto &descriptor : characteristic.descriptors) {
                JSIValue jsiDesc = JSI::CreateObject();
                JSI::SetStringProperty(jsiDesc, "serviceUuid", descriptor.serviceUuid.c_str());
                JSI::SetStringProperty(jsiDesc, "characteristicUuid", descriptor.characteristicUuid.c_str());
                JSI::SetStringProperty(jsiDesc, "descriptorUuid", descriptor.descriptorUuid.c_str());
                JSI::SetNumberProperty(jsiDesc, "descriptorHandle", descriptor.descriptor.descriptor_hdl);
                JSI::SetPropertyByIndex(descriptorsArray, descIndex++, jsiDesc);
                JSI::ReleaseValue(jsiDesc);
            }

            JSI::SetNamedProperty(jsiChar, "descriptors", descriptorsArray);
            JSI::ReleaseValue(descriptorsArray);

            // 处理特征值属性
            JSIValue properties = JSI::CreateObject();
            JSI::SetBooleanProperty(properties, "write", characteristic.properties.write);
            JSI::SetBooleanProperty(properties, "writeNoResponse", characteristic.properties.writeNoResponse);
            JSI::SetBooleanProperty(properties, "read", characteristic.properties.read);
            JSI::SetBooleanProperty(properties, "notify", characteristic.properties.notify);
            JSI::SetBooleanProperty(properties, "indicate", characteristic.properties.indicate);
            JSI::SetNamedProperty(jsiChar, "properties", properties);
            JSI::ReleaseValue(properties);

            JSI::SetNumberProperty(jsiChar, "characteristicValueHandle", characteristic.character.declare_handle);

            JSI::SetPropertyByIndex(characteristicsArray, charIndex++, jsiChar);
            JSI::ReleaseValue(jsiChar);
        }

        JSI::SetNamedProperty(jsiService, "characteristics", characteristicsArray);
        JSI::ReleaseValue(characteristicsArray);

        // 包含服务（暂不支持）
        JSI::SetNamedProperty(jsiService, "includeServices", JSI::CreateUndefined());

        JSI::SetPropertyByIndex(*rltArray, serviceIndex++, jsiService);
        JSI::ReleaseValue(jsiService);
    }
}

JSIValue ConstructJSIResult(const gattc_handle_value_t *read_result, const BLECharacteristic *charFound, const BLEDescriptor *descFound,
    const std::string &serviceUuid, const std::string &charUuid, const std::string &descUuid)
{
    JSIValue result = JSI::CreateObject();
    JSI::SetStringProperty(result, "serviceUuid", serviceUuid.c_str());
    JSI::SetStringProperty(result, "characteristicUuid", charUuid.c_str());

    uint8_t *resultPtr = nullptr;
    uint32_t bufferLength = read_result->data_len;
    JSIValue valueArray = JSI::CreateArrayBuffer(bufferLength, resultPtr);
    if (bufferLength > 0 && read_result->data != nullptr) {
        if (memcpy_s(resultPtr, bufferLength, const_cast<uint8_t *>(read_result->data), bufferLength) != EOK) {
            HILOG_ERROR(HILOG_MODULE_ACE, "memcpy Value failed!");
            JSI::ReleaseValue(valueArray);
            valueArray = JSI::CreateUndefined();
        }
    }

    if (descFound != nullptr) {
        JSI::SetStringProperty(result, "descriptorUuid", descUuid.c_str());
        JSI::SetNamedProperty(result, "descriptorValue", valueArray);
        JSI::SetNumberProperty(result, "descriptorHandle", read_result->handle);
    } else if (charFound != nullptr) {
        JSIValue descriptorsArray = JSI::CreateArray(charFound->descriptors.size());
        uint32_t descIndex = 0;
        for (const auto &descriptor : charFound->descriptors) {
            JSIValue jsiDescriptor = JSI::CreateObject();
            JSI::SetStringProperty(jsiDescriptor, "serviceUuid", descriptor.serviceUuid.c_str());
            JSI::SetStringProperty(jsiDescriptor, "characteristicUuid", descriptor.characteristicUuid.c_str());
            JSI::SetStringProperty(jsiDescriptor, "descriptorUuid", descriptor.descriptorUuid.c_str());

            uint8_t *descriptorPtr = nullptr;
            uint32_t descValueLen = descriptor.descriptorValue.size();
            if (descValueLen > 0) {
                JSIValue descValueArray = JSI::CreateArrayBuffer(descValueLen, descriptorPtr);
                if (memcpy_s(descriptorPtr, descValueLen, descriptor.descriptorValue.data(), descValueLen) != EOK) {
                    HILOG_ERROR(HILOG_MODULE_ACE, "memcpy descriptor Value failed!");
                    JSI::ReleaseValue(descValueArray);
                    descValueArray = JSI::CreateUndefined();
                }
                JSI::SetNamedProperty(jsiDescriptor, "descriptorValue", descValueArray);
                JSI::ReleaseValue(descValueArray);
            }

            JSI::SetNumberProperty(jsiDescriptor, "descriptorHandle", descriptor.descriptor.descriptor_hdl);
            JSI::SetPropertyByIndex(descriptorsArray, descIndex++, jsiDescriptor);
            JSI::ReleaseValue(jsiDescriptor);
        }

        JSI::SetNamedProperty(result, "descriptors", descriptorsArray);
        JSI::ReleaseValue(descriptorsArray);
        JSI::SetNamedProperty(result, "characteristicValue", valueArray);
        JSI::SetNumberProperty(result, "characteristicValueHandle", read_result->handle - 1);
    }

    JSI::ReleaseValue(valueArray);
    return result;
}

std::string FormatDeviceName(gap_scan_result_data_t *scan_result_data)
{
    char deviceName[32] = {0};  // 32是设备名缓冲区最大长度31加结束符
    
    // 1. 尝试从广播数据获取设备名
    if (scan_result_data->adv_data && scan_result_data->adv_len > 0) {
        const uint8_t *data = scan_result_data->adv_data;
        uint8_t len = scan_result_data->adv_len;
        uint8_t offset = 0;

        while (offset < len) {
            uint8_t field_len = data[offset];
            if (field_len == 0) break;
            if (offset + 1 >= len) break;  // 确保有类型字段

            uint8_t type = data[offset + 1];
            // 0x08: Shortened Local Name, 0x09: Complete Local Name
            if (type == 0x08 || type == 0x09) {
                // 设备名字段从offset+2开始，长度为field_len-1
                uint8_t name_len = field_len - 1;
                if (offset + 2 + name_len > len) break;

                // 使用安全复制
                size_t copy_len = (name_len < sizeof(deviceName) - 1) ? 
                                  name_len : sizeof(deviceName) - 1;
                
                if (memcpy_s(deviceName, sizeof(deviceName), data + offset + 2, copy_len) != EOK) {
                    HILOG_ERROR(HILOG_MODULE_ACE, "memcpy_s for device name failed");
                    return "";
                } else {
                    deviceName[copy_len] = '\0';  // 确保以空字符结尾
                    return deviceName;
                }
            }
            offset += field_len + 1;
        }
    }

    return "";
}

// 解析广播数据
BleParsedResult ParseMixedAdvertisingData(const uint8_t *data, uint8_t len)
{
    BleParsedResult result;
    memset_s(&result, sizeof(BleParsedResult), 0, sizeof(BleParsedResult));
    if (data == nullptr || len == 0) {
        return result;
    }

    uint8_t offset = 0;
    while (offset < len) {
        uint8_t field_len = data[offset];
        if (field_len == 0)
            break;
        if (offset + 1 >= len)
            break;  // 确保有类型字段

        // 检查字段是否越界
        if (offset + field_len + 1 > len) {
            HILOG_WARN(HILOG_MODULE_ACE, "AD field out of bounds: offset=%d, len=%d, field_len=%d", offset,
                len, field_len);
            break;
        }

        uint8_t type = data[offset + 1];
        const uint8_t *value = data + offset + 2;
        uint8_t value_len = field_len - 1;  // 减去类型长度

        // 服务UUID解析
        if (type == 0x02 || type == 0x03) {  // 16-bit UUIDs
            ParseUuidList(value, value_len, 2, result.serviceUuids);
        } else if (type == 0x04 || type == 0x05) {  // 32-bit UUIDs
            ParseUuidList(value, value_len, 4, result.serviceUuids);
        } else if (type == 0x06 || type == 0x07) {  // 128-bit UUIDs
            ParseUuidList(value, value_len, 16, result.serviceUuids);
        } else if (type == 0x14) {  // 16-bit Service Solicitation UUIDs
            ParseUuidList(value, value_len, 2, result.serviceSolicitationUuids);
        } else if (type == 0x15) {  // 128-bit Service Solicitation UUIDs
            ParseUuidList(value, value_len, 16, result.serviceSolicitationUuids);
        } else if (type == 0xFF) { // 制造商数据解析 (0xFF)
            if (value_len >= 2) {
                result.manufactureId = (value[1] << 8) | value[0];  // Little-endian
                if (value_len > 2) {
                    result.manufactureData.assign(value + 2, value + value_len);
                }
            }
        } else if (type == 0x16 && value_len >= 2) { // 服务数据解析 (0x16)
            uint16_t uuid = (value[1] << 8) | value[0];  // Little-endian
            char uuid_str[5] = {0}; // 4字符UUID + 1空字符
            int ret = snprintf_s(uuid_str, sizeof(uuid_str), sizeof(uuid_str) - 1, "%04X", uuid);
            if (ret < 0) {
                HILOG_ERROR(HILOG_MODULE_ACE, "snprintf_s for service data UUID failed");
                return result;
            } else {
                std::vector<uint8_t> serviceData;
                if (value_len > 2) {
                    serviceData.assign(value + 2, value + value_len);
                }
                result.serviceDatas.push_back(std::make_pair(uuid_str, serviceData));
            }
        }

        offset += field_len + 1;  // 移动到下一个字段
    }

    return result;
}

// 检查设备基本信息匹配
bool CheckBasicDeviceMatch(const BleScanFilter& filter, const ScanResult& scanResultTmp)
{
    // 设备ID匹配检查
    if (!filter.deviceId.empty() && filter.deviceId != scanResultTmp.deviceId) {
        HILOG_WARN(HILOG_MODULE_ACE,
            "[Filter] Device ID mismatch: filter=%s, device=%s",
            filter.deviceId.c_str(),
            scanResultTmp.deviceId.c_str());
        return false;
    }

    // 设备名匹配检查
    if (!filter.name.empty() && filter.name != scanResultTmp.deviceName) {
        HILOG_WARN(HILOG_MODULE_ACE,
            "[Filter] Device name mismatch: filter=%s, device=%s",
            filter.name.c_str(),
            scanResultTmp.deviceName.c_str());
        return false;
    }

    return true;
}

// 检查UUID相关匹配
bool CheckUuidMatch(const BleScanFilter& filter, const BleParsedResult& parsedResult)
{
    // 服务UUID匹配检查
    if (!filter.serviceUuid.empty()) {
        bool foundUuid = false;
        for (const auto &uuid : parsedResult.serviceUuids) {
            if (CompareHexStrings(uuid, filter.serviceUuid)) {
                foundUuid = true;
                break;
            }
        }
        if (!foundUuid) {
            HILOG_WARN(
                HILOG_MODULE_ACE, "[Filter] Service UUID not found: filter=%s", filter.serviceUuid.c_str());
            return false;
        }
    }

    // 服务请求UUID匹配检查（带掩码）
    if (!filter.serviceSolicitationUuid.empty()) {
        const std::string &mask = filter.serviceSolicitationUuidMask.empty()
                                      ? "FFFFFFFF-FFFF-FFFF-FFFF-FFFFFFFFFFFF"
                                      : filter.serviceSolicitationUuidMask;

        bool foundSolicitationUuid = false;
        for (const auto &uuid : parsedResult.serviceSolicitationUuids) {
            if (CheckUuidMatch(uuid, filter.serviceSolicitationUuid, mask)) {
                foundSolicitationUuid = true;
                break;
            }
        }
        if (!foundSolicitationUuid) {
            HILOG_WARN(HILOG_MODULE_ACE,
                "[Filter] Service solicitation UUID not found: filter=%s",
                filter.serviceSolicitationUuid.c_str());
            return false;
        }
    }

    return true;
}

// 检查数据匹配（制造商数据和服务数据）
bool CheckDataMatch(const BleScanFilter& filter, const BleParsedResult& parsedResult)
{
    // 制造商数据匹配检查
    if (filter.manufactureId != 0 || !filter.manufactureData.empty()) {
        if (filter.manufactureId != 0 && filter.manufactureId != parsedResult.manufactureId) {
            HILOG_WARN(HILOG_MODULE_ACE,
                "[Filter] Manufacture ID mismatch: filter=0x%04X, device=0x%04X",
                filter.manufactureId,
                parsedResult.manufactureId);
            return false;
        }

        // 使用掩码比较制造商数据
        if (!filter.manufactureData.empty()) {
            const std::vector<uint8_t> &filterData = filter.manufactureData;
            const std::vector<uint8_t> &deviceData = parsedResult.manufactureData;
            const std::vector<uint8_t> &mask = filter.manufactureDataMask;

            if (filterData.size() != deviceData.size()) {
                HILOG_WARN(HILOG_MODULE_ACE, "[Filter] Manufacture data size mismatch");
                return false;
            }

            for (size_t i = 0; i < filterData.size(); i++) {
                uint8_t maskVal = (i < mask.size()) ? mask[i] : 0xFF;
                if ((filterData[i] & maskVal) != (deviceData[i] & maskVal)) {
                    HILOG_WARN(HILOG_MODULE_ACE, "[Filter] Manufacture data mismatch at index %d", i);
                    return false;
                }
            }
        }
    }

    // 服务数据匹配检查
    if (!filter.serviceData.empty()) {
        const std::vector<uint8_t> &filterData = filter.serviceData;
        const std::vector<uint8_t> &mask = filter.serviceDataMask;

        bool foundServiceData = false;
        for (const auto &svcData : parsedResult.serviceDatas) {
            const std::vector<uint8_t> &deviceData = svcData.second;
            if (filterData.size() != deviceData.size()) {
                continue;
            }

            bool dataMatch = true;
            for (size_t i = 0; i < filterData.size(); i++) {
                uint8_t maskVal = (i < mask.size()) ? mask[i] : 0xFF;
                if ((filterData[i] & maskVal) != (deviceData[i] & maskVal)) {
                    dataMatch = false;
                    break;
                }
            }

            if (dataMatch) {
                foundServiceData = true;
                break;
            }
        }

        if (!foundServiceData) {
            HILOG_WARN(HILOG_MODULE_ACE, "[Filter] Service data not found");
            return false;
        }
    }

    return true;
}

// 整合函数: 读取匹配特征值和描述符
bool ReadMatchFilter(const BleScanFilter& filter, const BleParsedResult& parsedResult, 
    const ScanResult& scanResultTmp)
{
    // 检查设备基本信息匹配
    if (!CheckBasicDeviceMatch(filter, scanResultTmp)) {
        return false;
    }

    // 检查UUID相关匹配
    if (!CheckUuidMatch(filter, parsedResult)) {
        return false;
    }

    // 检查数据匹配
    if (!CheckDataMatch(filter, parsedResult)) {
        return false;
    }

    return true;
}

// 更新扫描结果
void UpdateScanResult(const ScanResult &scanResultTmp, std::vector<ScanResult>::iterator existingIt,
    std::vector<ScanResult> &scanResultList)
{
    if (existingIt == scanResultList.end()) {
        // 添加新设备
        scanResultList.push_back(scanResultTmp);
    } else {
        // 更新已有设备信息
        existingIt->deviceName = scanResultTmp.deviceName;
        existingIt->rssi = scanResultTmp.rssi;
        existingIt->data = scanResultTmp.data;
    }
}
}  // namespace ACELite
}  // namespace OHOS

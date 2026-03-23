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
static void ExtractAdvertiseSetting(const JSIValue &jsiSetting, AdvertiseSetting &setting)
{
    JSIValue jsiInterval = JSI::GetNamedProperty(jsiSetting, "interval");
    if (!JSI::ValueIsUndefined(jsiInterval)) {
        setting.interval = JSI::ValueToNumber(jsiInterval);
        JSI::ReleaseValue(jsiInterval);
    }
    JSIValue jsiTxPower = JSI::GetNamedProperty(jsiSetting, "txPower");
    if (!JSI::ValueIsUndefined(jsiTxPower)) {
        setting.txPower = JSI::ValueToNumber(jsiTxPower);
        JSI::ReleaseValue(jsiTxPower);
    }
    JSIValue jsiConnectable = JSI::GetNamedProperty(jsiSetting, "connectable");
    if (!JSI::ValueIsUndefined(jsiConnectable)) {
        setting.connectable = JSI::ValueToBoolean(jsiConnectable);
        JSI::ReleaseValue(jsiConnectable);
    }
}

static void ExtractServiceUuids(const JSIValue &jsiServiceUuids, AdvertiseData &advData)
{
    if (!JSI::ValueIsUndefined(jsiServiceUuids)) {
        uint32_t jsiServiceUuidsLen = JSI::GetArrayLength(jsiServiceUuids);
        for (int i = 0; i < jsiServiceUuidsLen; i++) {
            JSIValue jsiServiceUuidsStr = JSI::GetPropertyByIndex(jsiServiceUuids, i);
            char *serviceUuidsStrTmp = JSI::ValueToString(jsiServiceUuidsStr);
            JSI::ReleaseValue(jsiServiceUuidsStr);
            if (serviceUuidsStrTmp == nullptr) {
                break;
            }
            std::string serviceUuidsStr(serviceUuidsStrTmp);
            advData.serviceUuids.push_back(serviceUuidsStr);
            JSI::ReleaseString(serviceUuidsStrTmp);
        }
        JSI::ReleaseValue(jsiServiceUuids);
    }
}

static void ExtractManufactureData(const JSIValue &jsiManufactureData, AdvertiseData &advData)
{
    if (!JSI::ValueIsUndefined(jsiManufactureData)) {
        uint32_t jsiManufactureDataLen = JSI::GetArrayLength(jsiManufactureData);
        for (int i = 0; i < jsiManufactureDataLen; i++) {
            ManufactureData manufactureData;
            memset_s(&manufactureData, sizeof(manufactureData), 0, sizeof(manufactureData));
            JSIValue jsiManufactureDataItem = JSI::GetPropertyByIndex(jsiManufactureData, i);
            manufactureData.manufactureId = (uint16_t)JSI::GetNumberProperty(jsiManufactureDataItem, "manufactureId");
            JSIValue jsiManufactureValue = JSI::GetNamedProperty(jsiManufactureDataItem, "manufactureValue");
            size_t manufactureValueLen = 0;
            if (!JSI::ValueIsUndefined(jsiManufactureValue)) {
                uint8_t *manufactureValuePtr = JSI::GetArrayBufferInfo(jsiManufactureValue, manufactureValueLen);
                if (manufactureValuePtr) {
                    manufactureData.manufactureValue.reserve(manufactureValueLen);
                    manufactureData.manufactureValue.assign(manufactureValuePtr, manufactureValuePtr + manufactureValueLen);
                }
                JSI::ReleaseValue(jsiManufactureValue);
            }
            advData.manufactureData.push_back(manufactureData);
            JSI::ReleaseValue(jsiManufactureDataItem);
        }
        JSI::ReleaseValue(jsiManufactureData);
    }
}

static void ExtractServiceData(const JSIValue &jsiServiceData, AdvertiseData &advData)
{
    if (!JSI::ValueIsUndefined(jsiServiceData)) {
        uint32_t jsiServiceDataLen = JSI::GetArrayLength(jsiServiceData);
        for (int i = 0; i < jsiServiceDataLen; i++) {
            JSIValue jsiServiceDataItem = JSI::GetPropertyByIndex(jsiServiceData, i);
            ServiceData serviceData;
            memset_s(&serviceData, sizeof(serviceData), 0, sizeof(serviceData));
            char* serviceUuid = JSI::GetStringProperty(jsiServiceDataItem, "serviceUuid");
            serviceData.serviceUuid = serviceUuid;
            JSI::ReleaseString(serviceUuid);
            JSIValue serviceValue = JSI::GetNamedProperty(jsiServiceDataItem, "serviceValue");
            size_t serviceValueLen = 0;
            if (!JSI::ValueIsUndefined(serviceValue)) {
                uint8_t *serviceValuePtr = JSI::GetArrayBufferInfo(serviceValue, serviceValueLen);
                if (serviceValuePtr) {
                    serviceData.serviceValue.reserve(serviceValueLen);
                    serviceData.serviceValue.assign(serviceValuePtr, serviceValuePtr + serviceValueLen);
                }
                JSI::ReleaseValue(serviceValue);
            }
            advData.serviceData.push_back(serviceData);
            JSI::ReleaseValue(jsiServiceDataItem);
        }
        JSI::ReleaseValue(jsiServiceData);
    }
}

static void ExtractIncludeDeviceName(const JSIValue &jsiAdvData, AdvertiseData &advData)
{
    JSIValue jsiIncludeDeviceName = JSI::GetNamedProperty(jsiAdvData, "includeDeviceName");
    if (!JSI::ValueIsUndefined(jsiIncludeDeviceName)) {
        advData.includeDeviceName = JSI::ValueToBoolean(jsiIncludeDeviceName);
        JSI::ReleaseValue(jsiIncludeDeviceName);
    }
}

static void ExtractAdvertiseResponse(const JSIValue &jsiAdvResp, AdvertiseData &advResponse, bool *hasResponse)
{
    JSIValue jsiServiceUuids = JSI::GetNamedProperty(jsiAdvResp, "serviceUuids");
    ExtractServiceUuids(jsiServiceUuids, advResponse);
    JSIValue jsiManufactureData = JSI::GetNamedProperty(jsiAdvResp, "manufactureData");
    ExtractManufactureData(jsiManufactureData, advResponse);
    JSIValue jsiServiceData = JSI::GetNamedProperty(jsiAdvResp, "serviceData");
    ExtractServiceData(jsiServiceData, advResponse);
    *hasResponse = true;
    ExtractIncludeDeviceName(jsiAdvResp, advResponse);
}

void BleModule::GetAdvertiseWithParame(const JSIValue *args, uint8_t argsSize, AdvertiseSetting &setting,
    AdvertiseData &advData, AdvertiseData &advResponse, bool *hasResponse)
{
    JSIValue jsiSetting = JSI::AcquireValue(args[0]);
    ExtractAdvertiseSetting(jsiSetting, setting);
    JSI::ReleaseValue(jsiSetting);

    JSIValue jsiAdvData = JSI::AcquireValue(args[1]);
    JSIValue jsiServiceUuids = JSI::GetNamedProperty(jsiAdvData, "serviceUuids");
    ExtractServiceUuids(jsiServiceUuids, advData);
    JSIValue jsiManufactureData = JSI::GetNamedProperty(jsiAdvData, "manufactureData");
    ExtractManufactureData(jsiManufactureData, advData);
    JSIValue jsiServiceData = JSI::GetNamedProperty(jsiAdvData, "serviceData");
    ExtractServiceData(jsiServiceData, advData);

    ExtractIncludeDeviceName(jsiAdvData, advData);
    JSI::ReleaseValue(jsiAdvData);

    if (argsSize == ARGC_THREE) {
        JSIValue jsiAdvResp = JSI::AcquireValue(args[ARGC_TWO]);
        ExtractAdvertiseResponse(jsiAdvResp, advResponse, hasResponse);
        JSI::ReleaseValue(jsiAdvResp);
    }
}

void BleModule::GetAdvertiseData(AdvertisingParams &advertisingParams, JSIValue jsiAdvertisingParams, const char *type,
    bool *hasResponse)
{
    // 获取广播数据对象
    JSIValue jsiAdvertisingData = JSI::GetNamedProperty(jsiAdvertisingParams, type);
    // 检查响应数据是否存在
    if (!CheckResponseData(type, jsiAdvertisingData, hasResponse)) {
        JSI::ReleaseValue(jsiAdvertisingData);
        return;
    }
    
    // 处理服务UUID
    ProcessServiceUuids(advertisingParams, jsiAdvertisingData, type);
    
    // 处理制造商数据
    ProcessManufactureData(advertisingParams, jsiAdvertisingData, type);
    
    // 处理服务数据
    ProcessServiceData(advertisingParams, jsiAdvertisingData, type);
    
    // 处理设备名称包含标志
    ProcessIncludeDeviceName(advertisingParams, jsiAdvertisingData, type);
    JSI::ReleaseValue(jsiAdvertisingData);
}

bool BleModule::CheckResponseData(const char *type, JSIValue jsiAdvertisingData, bool *hasResponse)
{
    if (strcmp(type, ADVERTISING_RESPONSE) == 0) {
        if (!JSI::ValueIsUndefined(jsiAdvertisingData)) {
            *hasResponse = true;
        } else {
            return false; // 响应数据未定义，直接返回
        }
    }
    return true;
}

void BleModule::ProcessServiceUuids(AdvertisingParams &advertisingParams, JSIValue jsiAdvertisingData,
    const char *type)
{
    JSIValue jsiServiceUuids = JSI::GetNamedProperty(jsiAdvertisingData, SERVICE_UUIDS);
    if (JSI::ValueIsUndefined(jsiServiceUuids)) {
        return;
    }
    
    uint32_t jsiServiceUuidsLen = JSI::GetArrayLength(jsiServiceUuids);
    for (int i = 0; i < jsiServiceUuidsLen; i++) {
        JSIValue jsiServiceUuidsItem = JSI::GetPropertyByIndex(jsiServiceUuids, i);
        char *serviceUuidsTmp = JSI::ValueToString(jsiServiceUuidsItem);
        JSI::ReleaseValue(jsiServiceUuidsItem);
        if (serviceUuidsTmp == nullptr) {
            break;
        }
        std::string serviceUuids(serviceUuidsTmp);
        JSI::ReleaseString(serviceUuidsTmp);
        // 根据类型添加到对应的广播数据中
        if (strcmp(type, ADVERTISING_DATA) == 0) {
            advertisingParams.advertisingData.serviceUuids.push_back(serviceUuids);
        } else if (strcmp(type, ADVERTISING_RESPONSE) == 0) {
            advertisingParams.advertisingResponse.serviceUuids.push_back(serviceUuids);
        }
    }
    JSI::ReleaseValue(jsiServiceUuids);
}

void BleModule::ProcessManufactureData(AdvertisingParams &advertisingParams, JSIValue jsiAdvertisingData, const char *type)
{
    JSIValue jsiManufactureData = JSI::GetNamedProperty(jsiAdvertisingData, MANUFACTURE_DATA);
    if (JSI::ValueIsUndefined(jsiManufactureData)) {
        return;
    }
    uint32_t jsiManufactureDataLen = JSI::GetArrayLength(jsiManufactureData);
    for (int i = 0; i < jsiManufactureDataLen; i++) {
        ManufactureData manufactureData;
        memset_s(&manufactureData, sizeof(manufactureData), 0, sizeof(manufactureData));

        JSIValue jsiManufactureDataItem = JSI::GetPropertyByIndex(jsiManufactureData, i);
        // 获取制造商ID
        JSIValue jsiManufactureId = JSI::GetNamedProperty(jsiManufactureDataItem, MANUFACTURE_ID);
        manufactureData.manufactureId = (uint16_t)JSI::ValueToNumber(jsiManufactureId);
        JSI::ReleaseValue(jsiManufactureId);
        // 获取制造商值
        ProcessManufactureValue(jsiManufactureDataItem, manufactureData);
        JSI::ReleaseValue(jsiManufactureDataItem);
        // 根据类型添加到对应的广播数据中
        if (strcmp(type, ADVERTISING_DATA) == 0) {
            advertisingParams.advertisingData.manufactureData.push_back(manufactureData);
        } else if (strcmp(type, ADVERTISING_RESPONSE) == 0) {
            advertisingParams.advertisingResponse.manufactureData.push_back(manufactureData);
        }
    }
    JSI::ReleaseValue(jsiManufactureData);
}

void BleModule::ProcessManufactureValue(JSIValue jsiManufactureDataItem, ManufactureData &manufactureData)
{
    JSIValue jsiManufactureValue = JSI::GetNamedProperty(jsiManufactureDataItem, MANUFACTURE_VALUE);
    if (JSI::ValueIsUndefined(jsiManufactureValue)) {
        return;
    }
    size_t jsiManufactureValueLen = 0;
    uint8_t *manufactureValuePtr = JSI::GetArrayBufferInfo(jsiManufactureValue, jsiManufactureValueLen);
    if (manufactureValuePtr) {
        manufactureData.manufactureValue.assign(manufactureValuePtr, manufactureValuePtr + jsiManufactureValueLen);
    }
    JSI::ReleaseValue(jsiManufactureValue);
}

void BleModule::ProcessServiceData(AdvertisingParams &advertisingParams, JSIValue jsiAdvertisingData, const char *type)
{
    JSIValue jsiServiceData = JSI::GetNamedProperty(jsiAdvertisingData, SERVICE_DATA);
    if (JSI::ValueIsUndefined(jsiServiceData)) {
        return;
    }

    uint32_t jsiServiceDataLen = JSI::GetArrayLength(jsiServiceData);
    for (int i = 0; i < jsiServiceDataLen; i++) {
        JSIValue jsiServiceDataItem = JSI::GetPropertyByIndex(jsiServiceData, i);
        ServiceData serviceData;
        memset_s(&serviceData, sizeof(serviceData), 0, sizeof(serviceData));

        // 获取服务UUID
        char* serviceUuid = JSI::GetStringProperty(jsiServiceDataItem, SERVICE_UUID);
        serviceData.serviceUuid = serviceUuid;
        JSI::ReleaseString(serviceUuid);

        // 获取服务值
        ProcessServiceValue(jsiServiceDataItem, serviceData);
        JSI::ReleaseValue(jsiServiceDataItem);
        // 根据类型添加到对应的广播数据中
        if (strcmp(type, ADVERTISING_DATA) == 0) {
            advertisingParams.advertisingData.serviceData.push_back(serviceData);
        } else if (strcmp(type, ADVERTISING_RESPONSE) == 0) {
            advertisingParams.advertisingResponse.serviceData.push_back(serviceData);
        }
    }
    JSI::ReleaseValue(jsiServiceData);
}

void BleModule::ProcessServiceValue(JSIValue jsiServiceDataItem, ServiceData &serviceData)
{
    JSIValue jsiServiceValue = JSI::GetNamedProperty(jsiServiceDataItem, SERVICE_VALUE);
    if (JSI::ValueIsUndefined(jsiServiceValue)) {
        return;
    }

    uint32_t jsiServiceValueLen = JSI::GetArrayLength(jsiServiceValue);
    for (int j = 0; j < jsiServiceValueLen; j++) {
        JSIValue jsiServiceValueItem = JSI::GetPropertyByIndex(jsiServiceValue, j);
        serviceData.serviceValue.push_back(JSI::ValueToNumber(jsiServiceValueItem));
        JSI::ReleaseValue(jsiServiceValueItem);
    }
    JSI::ReleaseValue(jsiServiceValue);
}

void BleModule::ProcessIncludeDeviceName(AdvertisingParams &advertisingParams, JSIValue jsiAdvertisingData, const char *type)
{
    JSIValue jsiIncludeDeviceName = JSI::GetNamedProperty(jsiAdvertisingData, INCLUDE_DEVICE_NAME);
    if (JSI::ValueIsUndefined(jsiIncludeDeviceName)) {
        return;
    }

    bool includeDeviceName = JSI::ValueToBoolean(jsiIncludeDeviceName);

    // 根据类型设置对应的包含设备名称标志
    if (strcmp(type, ADVERTISING_DATA) == 0) {
        advertisingParams.advertisingData.includeDeviceName = includeDeviceName;
    } else if (strcmp(type, ADVERTISING_RESPONSE) == 0) {
        advertisingParams.advertisingResponse.includeDeviceName = includeDeviceName;
    }
    JSI::ReleaseValue(jsiIncludeDeviceName);
}

void BleModule::GetAdvertiseWithCallback(const JSIValue *args, uint8_t argsSize, AdvertisingParams &advertisingParams,
    bool *hasResponse)
{
    memset_s(&advertisingParams, sizeof(advertisingParams), 0, sizeof(advertisingParams));
    JSIValue jsiAdvertisingParams = JSI::AcquireValue(args[0]);
    JSIValue jsiAdvertisingSettings = JSI::GetNamedProperty(jsiAdvertisingParams, "advertisingSettings");
    if (JSI::ValueIsUndefined(jsiAdvertisingSettings)) {
        JSI::ReleaseValue(jsiAdvertisingParams);
        return;
    }
    JSIValue jsiInterval = JSI::GetNamedProperty(jsiAdvertisingSettings, "interval");
    if (!JSI::ValueIsUndefined(jsiInterval)) {
        advertisingParams.advertisingSettings.interval = JSI::ValueToNumber(jsiInterval);
        JSI::ReleaseValue(jsiInterval);
    }
    JSIValue jsiTxPower = JSI::GetNamedProperty(jsiAdvertisingSettings, "txPower");
    if (!JSI::ValueIsUndefined(jsiTxPower)) {
        advertisingParams.advertisingSettings.txPower = JSI::ValueToNumber(jsiTxPower);
        JSI::ReleaseValue(jsiTxPower);
    }
    JSIValue jsiConnectable= JSI::GetNamedProperty(jsiAdvertisingSettings, "connectable");
    if (!JSI::ValueIsUndefined(jsiConnectable)) {
        advertisingParams.advertisingSettings.connectable = JSI::ValueToBoolean(jsiConnectable);
        JSI::ReleaseValue(jsiConnectable);
    }
    GetAdvertiseData(advertisingParams, jsiAdvertisingParams, "advertisingData", hasResponse);
    GetAdvertiseData(advertisingParams, jsiAdvertisingParams, "advertisingResponse", hasResponse);
    JSIValue jsDuration = JSI::GetNamedProperty(jsiAdvertisingParams, "duration");
    if (!JSI::ValueIsUndefined(jsDuration)) {
        advertisingParams.duration = JSI::ValueToNumber(jsDuration);
        JSI::ReleaseValue(jsDuration);
    }
    JSI::ReleaseValueList(jsiAdvertisingParams, jsiAdvertisingSettings);
}

static bool IsAdvertiseSetting(const JSIValue *args, uint8_t argsSize)
{
    bool exist = false;
    JSIValue jsiCallback = JSI::AcquireValue(args[1]);
    if (!JSI::ValueIsFunction(jsiCallback)) {
        exist = true;
    }
    JSI::ReleaseValue(jsiCallback);
    return exist;
}

static errcode_t ProcessDeviceName(AdvertiseData &advData, std::vector<uint8_t> &data)
{
    if (!advData.includeDeviceName) {
        return BLE_SUCCESS;
    }
    uint8_t bleName[MAX_BLE_NAME_LEN] = {0};
    uint8_t bleNameLen = MAX_BLE_NAME_LEN;
    int32_t ret = gap_ble_get_local_name(bleName, &bleNameLen);
    if (ret != BLE_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "gap ble get local name failed!, ret:%X", gap_ble_get_local_name);
        return ret;
    }

    // 添加设备名称到广播数据
    data.push_back(bleNameLen + 1); // 长度字段：名称长度 + 类型字段(1字节)
    data.push_back(BLE_AD_TYPE_COMPLETE_LOCAL_NAME); // 设备名称类型
    for (int i = 0; i < bleNameLen; i++) {
        data.push_back(bleName[i]);
    }
    
    return BLE_SUCCESS;
}

static errcode_t ProcessServiceUuid(AdvertiseData &advData, std::vector<uint8_t> &data)
{
    for (auto serviceUuid : advData.serviceUuids) {
        int32_t len = serviceUuid.length();
        int32_t cnt = 0;
        uint8_t *uuid = (uint8_t *)malloc(len / 2);
        if (uuid == nullptr) {
            HILOG_ERROR(HILOG_MODULE_ACE, "malloc memory failed!");
            return BLE_OPRATION_FAILED;
        }
        
        DecodeUuidFromStr(serviceUuid.c_str(), uuid, &cnt);
        // 添加服务UUID到广播数据
        data.push_back(cnt + 1); // 长度字段：UUID长度 + 类型字段(1字节)
        
        // 根据UUID长度设置对应的广播类型
        switch (cnt) {
            case 2: // 16位UUID
                data.push_back(BLE_AD_TYPE_16BIT_SERVICE_UUID);
                break;
            case 4: // 32位UUID
                data.push_back(BLE_AD_TYPE_32BIT_SERVICE_UUID);
                break;
            case 16: // 128位UUID
                data.push_back(BLE_AD_TYPE_128BIT_SERVICE_UUID);
                break;
            default:
                HILOG_ERROR(HILOG_MODULE_ACE, "serviceUuid error!");
                free(uuid);
                return BLE_INVALID_PARAMETER;
        }

        // 添加UUID数据
        for (int j = 0; j < cnt; j++) {
            data.push_back(uuid[j]);
        }
        
        free(uuid);
        uuid = nullptr;
    }
    
    return BLE_SUCCESS;
}

static errcode_t ProcessGetManufactureData(AdvertiseData &advData, std::vector<uint8_t> &data)
{
    for (auto manufactureData : advData.manufactureData) {
        int32_t len = manufactureData.manufactureValue.size();
        // 添加制造商数据到广播数据
        data.push_back(len + 3); // 长度字段：制造商ID(2字节) + 类型字段(1字节) + 数据长度
        data.push_back(BLE_AD_TYPE_MANUFACTURER_SPECIFIC_DATA); // 制造商数据类型
        data.push_back((manufactureData.manufactureId >> 8) & 0xFF); // 制造商ID高字节
        data.push_back(manufactureData.manufactureId & 0xFF); // 制造商ID低字节

        // 添加制造商数据值
        for (auto value : manufactureData.manufactureValue) {
            data.push_back(value);
        }
    }

    return BLE_SUCCESS;
}

static errcode_t ProcessServiceData(AdvertiseData &advData, std::vector<uint8_t> &data)
{
    for (auto serviceData : advData.serviceData) {
        int32_t len = serviceData.serviceUuid.length();
        int32_t cnt = 0;
        uint8_t *uuid = (uint8_t *)malloc(len / 2);
        if (uuid == nullptr) {
            HILOG_ERROR(HILOG_MODULE_ACE, "malloc memory failed!");
            return BLE_OPRATION_FAILED;
        }
        
        DecodeUuidFromStr(serviceData.serviceUuid.c_str(), uuid, &cnt);
        uint32_t serviceDataLen = cnt + serviceData.serviceValue.size();
        
        // 添加服务数据到广播数据
        data.push_back(serviceDataLen + 1); // 长度字段：UUID长度 + 数据长度 + 类型字段(1字节)
        
        // 根据UUID长度设置对应的服务数据类型
        switch (cnt) {
            case 2: // 16位UUID
                data.push_back(BLE_AD_TYPE_SERVICE_DATA_16BIT);
                break;
            case 4: // 32位UUID
                data.push_back(BLE_AD_TYPE_SERVICE_DATA_32BIT);
                break;
            case 16: // 128位UUID
                data.push_back(BLE_AD_TYPE_SERVICE_DATA_128BIT);
                break;
            default:
                HILOG_ERROR(HILOG_MODULE_ACE, "serviceUuid error!");
                free(uuid);
                return BLE_INVALID_PARAMETER;
        }
        
        // 添加UUID数据
        for (int i = 0; i < cnt; i++) {
            data.push_back(uuid[i]);
        }
        
        free(uuid);
        
        // 添加服务数据值
        for (int j = 0; j < serviceData.serviceValue.size(); j++) {
            data.push_back(serviceData.serviceValue[j]);
        }
    }
    
    return BLE_SUCCESS;
}

static errcode_t SetAdvertisementData(std::vector<uint8_t> &data, gap_ble_config_adv_data_t &cfg_adv_data, bool isResponse)
{
    size_t dataSize = data.size();
    errcode_t ret = 0;
    if (isResponse) {
        cfg_adv_data.scan_rsp_length = dataSize;
        if (cfg_adv_data.scan_rsp_length > MAX_ADV_DATA_LEN) {
            HILOG_ERROR(HILOG_MODULE_ACE, "The length of adv response more than 31!");
            return BLE_INVALID_PARAMETER;
        }

        cfg_adv_data.scan_rsp_data = (uint8_t *)malloc(cfg_adv_data.scan_rsp_length);
        if (cfg_adv_data.scan_rsp_data == nullptr) {
            HILOG_ERROR(HILOG_MODULE_ACE, "malloc memory failed!");
            return BLE_OPRATION_FAILED;
        }

        ret = memcpy_s(cfg_adv_data.scan_rsp_data, cfg_adv_data.scan_rsp_length, data.data(), dataSize);
    } else {
        cfg_adv_data.adv_length = dataSize;
        if (cfg_adv_data.adv_length > MAX_ADV_DATA_LEN) {
            HILOG_ERROR(HILOG_MODULE_ACE, "The length of adv data more than 31!");
            return BLE_INVALID_PARAMETER;
        }

        cfg_adv_data.adv_data = (uint8_t *)malloc(cfg_adv_data.adv_length);
        if (cfg_adv_data.adv_data == nullptr) {
            HILOG_ERROR(HILOG_MODULE_ACE, "malloc memory failed!");
            return BLE_OPRATION_FAILED;
        }

        ret = memcpy_s(cfg_adv_data.adv_data, cfg_adv_data.adv_length, data.data(), dataSize);
    }
    return ret;
}

static errcode_t GetAdvertiseCfgData(AdvertiseData &advData, gap_ble_config_adv_data_t &cfg_adv_data, bool isResponse)
{
    std::vector<uint8_t> data;
    // 处理设备名称
    errcode_t ret = ProcessDeviceName(advData, data);
    if (ret != BLE_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "process device name failed!");
        return ret;
    }

    // 处理服务UUID
    ret = ProcessServiceUuid(advData, data);
    if (ret != BLE_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "process service uuid failed!");
        return ret;
    }

    // 处理制造商数据
    ret = ProcessGetManufactureData(advData, data);
    if (ret != BLE_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "process manufacture data failed!");
        return ret;
    }

    // 处理服务数据
    ret = ProcessServiceData(advData, data);
    if (ret != BLE_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "process service data failed!");
        return ret;
    }

    // 设置广播数据
    return SetAdvertisementData(data, cfg_adv_data, isResponse);
}

static errcode_t CheckAdvArguments(const JSIValue *args, uint8_t argsSize)
{
    if (argsSize < ARGC_TWO || argsSize > ARGC_THREE || args == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "2 or 3 arguments is required.");
        return BLE_INVALID_PARAMETER;
    }
    return BLE_SUCCESS;
}

JSIValue BleModule::StartAdvertising(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    errcode_t ret = CheckAdvArguments(args, argsSize);
    if (ret != BLE_SUCCESS) {
        gap_ble_config_adv_data_t emptyCfg = {0};
        return HandleExit(thisVal, args, argsSize, ret, emptyCfg);
    }

    gap_ble_adv_params_t adv_param;
    memset_s(&adv_param, sizeof(gap_ble_adv_params_t), 0, sizeof(gap_ble_adv_params_t));
    gap_ble_config_adv_data_t cfg_adv_data;
    memset_s(&cfg_adv_data, sizeof(cfg_adv_data), 0, sizeof(cfg_adv_data));
    bool hasResponse = false;

    ret = ProcessAdvertisingParameters(args, argsSize, adv_param, cfg_adv_data, hasResponse);
    if (ret != BLE_SUCCESS) {
        return HandleExit(thisVal, args, argsSize, ret, cfg_adv_data);
    }

    ret = ConfigureAndStartAdvertising(adv_param, cfg_adv_data);
    if (ret != BLE_SUCCESS) {
        return HandleExit(thisVal, args, argsSize, ret, cfg_adv_data);
    }

    return HandleExit(thisVal, args, argsSize, ret, cfg_adv_data);
}

errcode_t BleModule::ProcessAdvertisingParameters(const JSIValue *args, uint8_t argsSize,
    gap_ble_adv_params_t &adv_param, gap_ble_config_adv_data_t &cfg_adv_data, bool &hasResponse)
{
    if ((argsSize == ARGC_THREE || argsSize == ARGC_TWO) && IsAdvertiseSetting(args, argsSize)) {
        return HandleAdvertiseSetting(args, argsSize, cfg_adv_data, hasResponse);
    } else if (argsSize == ARGC_TWO && !IsAdvertiseSetting(args, argsSize)) {
        return HandleAdvertisingParams(args, argsSize, adv_param, cfg_adv_data, hasResponse);
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE, "Invalid parameter!");
        return BLE_INVALID_PARAMETER;
    }
}

errcode_t BleModule::HandleAdvertiseSetting(const JSIValue *args, uint8_t argsSize,
    gap_ble_config_adv_data_t &cfg_adv_data, bool &hasResponse)
{
    AdvertiseSetting setting = {0};
    AdvertiseData advData;
    memset_s(&advData, sizeof(AdvertiseData), 0, sizeof(AdvertiseData));
    AdvertiseData advResponse;
    memset_s(&advResponse, sizeof(AdvertiseData), 0, sizeof(AdvertiseData));

    GetAdvertiseWithParame(args, argsSize, setting, advData, advResponse, &hasResponse);
    errcode_t ret = GetAdvertiseCfgData(advData, cfg_adv_data, false);
    if (ret != BLE_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "HandleAdvertiseSetting ble get adv data failed! ret:%X",
            ret);
        return ret;
    }

    if (hasResponse) {
        ret = GetAdvertiseCfgData(advResponse, cfg_adv_data, true);
        if (ret != BLE_SUCCESS) {
            HILOG_ERROR(HILOG_MODULE_ACE, "ble get adv response data failed! ret:%X", ret);
            return ret;
        }
    }
    return BLE_SUCCESS;
}

errcode_t BleModule::HandleAdvertisingParams(const JSIValue *args, uint8_t argsSize,
    gap_ble_adv_params_t &adv_param, gap_ble_config_adv_data_t &cfg_adv_data, bool &hasResponse)
{
    AdvertisingParams advertisingParams;
    memset_s(&advertisingParams, sizeof(AdvertisingParams), 0, sizeof(AdvertisingParams));
    GetAdvertiseWithCallback(args, argsSize, advertisingParams, &hasResponse);
    adv_param.duration = advertisingParams.duration;

    errcode_t ret = GetAdvertiseCfgData(advertisingParams.advertisingData, cfg_adv_data, false);
    if (ret != BLE_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "ble get adv data failed! ret:%X", ret);
        return ret;
    }

    if (hasResponse) {
        ret = GetAdvertiseCfgData(advertisingParams.advertisingResponse, cfg_adv_data, true);
        if (ret != BLE_SUCCESS) {
            HILOG_ERROR(HILOG_MODULE_ACE, "ble get adv response data failed! ret:%X", ret);
            return ret;
        }
    }
    return BLE_SUCCESS;
}

errcode_t BleModule::ConfigureAndStartAdvertising(gap_ble_adv_params_t &adv_param,
    gap_ble_config_adv_data_t &cfg_adv_data)
{
    errcode_t ret = gap_ble_set_adv_data(advId, &cfg_adv_data);
    if (ret != BLE_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "ble set adv data failed!");
        return ret;
    }

    adv_param.min_interval = MIN_BLE_INTERVEL_LEN;
    adv_param.max_interval = MAX_BLE_INTERVEL_LEN;
    adv_param.adv_type = GAP_BLE_ADV_CONN_SCAN_UNDIR;
    adv_param.channel_map = BLE_ONE_CHANEL;

    ret = gap_ble_set_adv_param(advId, &adv_param);
    if (ret != BLE_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "ble set adv param failed! ret:%X", ret);
        return ret;
    }

    ret = gap_ble_start_adv(advId);
    if (ret != BLE_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "start adv failed! ret:%X", ret);
        return ret;
    }
    return BLE_SUCCESS;
}

JSIValue BleModule::HandleExit(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize, errcode_t ret,
    const gap_ble_config_adv_data_t &cfg_adv_data)
{
    // 处理回调
    if (argsSize == ARGC_TWO && !IsAdvertiseSetting(args, argsSize)) {
        JSIValue value = JSI::CreateNumber(advId);
        JSIValue err = JSI::CreateNumber(ret);
        JSIValue callback = JSI::AcquireValue(args[1]);
        JSIValue argv[ARGC_TWO] = {err, value};
        JSI::CallFunction(callback, thisVal, argv, ARGC_TWO);
        JSI::ReleaseValueList(value, err, callback);
    }

    gap_ble_config_adv_data_t cfg_copy = cfg_adv_data;
    if (cfg_copy.adv_data) {
        free(cfg_copy.adv_data);
    }
    if (cfg_copy.scan_rsp_data) {
        free(cfg_copy.scan_rsp_data);
    }
    
    return JSI::CreateNumber(ret);
}

JSIValue BleModule::StopAdvertising(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    if (argsSize == 0) {
        errcode_t ret = gap_ble_stop_adv(advId);
        if (ret != BLE_SUCCESS) {
            HILOG_ERROR(HILOG_MODULE_ACE, "stop adv failed! ret:%X", ret);
        }
        JSIValue err = JSI::CreateNumber(ret);
        return err;
    } else {
        JSIValue jsiAdvertisingId = JSI::AcquireValue(args[0]);
        uint8_t advertisingId = (uint8_t)JSI::ValueToNumber(jsiAdvertisingId);
        errcode_t ret = gap_ble_stop_adv(advertisingId);
        if (ret != BLE_SUCCESS) {
            HILOG_ERROR(HILOG_MODULE_ACE, "stop adv failed! ret:%X", ret);
        }
        JSIValue err = JSI::CreateNumber(ret);
        JSIValue argv[ARGC_ONE] = {err};
        JSIValue callback = JSI::AcquireValue(args[1]);
        JSI::CallFunction(callback, thisVal, argv, 1);
        JSI::ReleaseValueList(err, callback, jsiAdvertisingId);
    }
    return JSI::CreateUndefined();
}
}  // namespace ACELite
}  // namespace OHOS

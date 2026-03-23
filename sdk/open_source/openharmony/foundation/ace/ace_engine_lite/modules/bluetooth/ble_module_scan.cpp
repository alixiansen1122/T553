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
static void GetFilters(JSIValue filters)
{
    uint32_t filtersLen = JSI::GetArrayLength(filters);
    for (uint32_t i = 0; i < filtersLen; i++) {
        BleScanFilter bleScanFilter;
        memset_s(&bleScanFilter, sizeof(bleScanFilter), 0, sizeof(bleScanFilter));

        JSIValue filter = JSI::GetPropertyByIndex(filters, i);

        // 设备ID
        JSIValue jsiDeviceId = JSI::GetNamedProperty(filter, "deviceId");
        if (!JSI::ValueIsUndefined(jsiDeviceId)) {
            char *deviceId = JSI::ValueToString(jsiDeviceId);
            if (deviceId != nullptr) {
                bleScanFilter.deviceId = deviceId;
                JSI::ReleaseString(deviceId);
            }
            JSI::ReleaseValue(jsiDeviceId);
        }

        // 设备名
        JSIValue jsiName = JSI::GetNamedProperty(filter, "name");
        if (!JSI::ValueIsUndefined(jsiName)) {
            char *name = JSI::ValueToString(jsiName);
            if (name != nullptr) {
                bleScanFilter.name = name;
                JSI::ReleaseString(name);
            }
            JSI::ReleaseValue(jsiName);
        }

        // 服务UUID
        JSIValue jsiServiceUuid = JSI::GetNamedProperty(filter, "serviceUuid");
        if (!JSI::ValueIsUndefined(jsiServiceUuid)) {
            char *serviceUuid = JSI::ValueToString(jsiServiceUuid);
            if (serviceUuid != nullptr) {
                bleScanFilter.serviceUuid = serviceUuid;
                JSI::ReleaseString(serviceUuid);
            }
            JSI::ReleaseValue(jsiServiceUuid);
        }

        // 服务UUID掩码
        JSIValue jsiServiceUuidMask = JSI::GetNamedProperty(filter, "serviceUuidMask");
        if (!JSI::ValueIsUndefined(jsiServiceUuidMask)) {
            char *serviceUuidMask = JSI::ValueToString(jsiServiceUuidMask);
            if (serviceUuidMask != nullptr) {
                bleScanFilter.serviceUuidMask = serviceUuidMask;
                JSI::ReleaseString(serviceUuidMask);
            }
            JSI::ReleaseValue(jsiServiceUuidMask);
        }

        // 服务请求UUID
        JSIValue jsiServiceSolicitationUuid = JSI::GetNamedProperty(filter, "serviceSolicitationUuid");
        if (!JSI::ValueIsUndefined(jsiServiceSolicitationUuid)) {
            char *serviceSolicitationUuid = JSI::ValueToString(jsiServiceSolicitationUuid);
            if (serviceSolicitationUuid != nullptr) {
                bleScanFilter.serviceSolicitationUuid = serviceSolicitationUuid;
                JSI::ReleaseString(serviceSolicitationUuid);
            }
            JSI::ReleaseValue(jsiServiceSolicitationUuid);
        }

        // 服务请求UUID掩码
        JSIValue jsiServiceSolicitationUuidMask = JSI::GetNamedProperty(filter, "serviceSolicitationUuidMask");
        if (!JSI::ValueIsUndefined(jsiServiceSolicitationUuidMask)) {
            char *serviceSolicitationUuidMask = JSI::ValueToString(jsiServiceSolicitationUuidMask);
            if (serviceSolicitationUuidMask != nullptr) {
                bleScanFilter.serviceSolicitationUuidMask = serviceSolicitationUuidMask;
                JSI::ReleaseString(serviceSolicitationUuidMask);
            }
            JSI::ReleaseValue(jsiServiceSolicitationUuidMask);
        }

        // 服务数据
        JSIValue serviceData = JSI::GetNamedProperty(filter, "serviceData");
        size_t serviceDataLen = 0;
        if (!JSI::ValueIsUndefined(serviceData)) {
            uint8_t *serviceDataPtr = JSI::GetArrayBufferInfo(serviceData, serviceDataLen);
            if (serviceDataPtr) {
                bleScanFilter.serviceData.reserve(serviceDataLen);
                bleScanFilter.serviceData.assign(serviceDataPtr, serviceDataPtr + serviceDataLen);
            }
            JSI::ReleaseValue(serviceData);
        }

        // 服务数据掩码
        JSIValue serviceDataMask = JSI::GetNamedProperty(filter, "serviceDataMask");
        size_t serviceDataMaskLen = 0;
        if (!JSI::ValueIsUndefined(serviceDataMask)) {
            uint8_t *serviceDataMaskPtr = JSI::GetArrayBufferInfo(serviceDataMask, serviceDataMaskLen);
            if (serviceDataMaskPtr) {
                bleScanFilter.serviceDataMask.reserve(serviceDataMaskLen);
                bleScanFilter.serviceDataMask.assign(serviceDataMaskPtr, serviceDataMaskPtr + serviceDataMaskLen);
            }
            JSI::ReleaseValue(serviceDataMask);
        }

        // 制造商ID
        JSIValue jsiManufactureId = JSI::GetNamedProperty(filter, "manufactureId");
        if (!JSI::ValueIsUndefined(jsiManufactureId)) {
            bleScanFilter.manufactureId = static_cast<uint16_t>(JSI::ValueToNumber(jsiManufactureId));
            JSI::ReleaseValue(jsiManufactureId);
        }

        // 制造商数据
        JSIValue manufactureData = JSI::GetNamedProperty(filter, "manufactureData");
        size_t manufactureDataLen = 0;
        if (!JSI::ValueIsUndefined(manufactureData)) {
            uint8_t *manufactureDataPtr = JSI::GetArrayBufferInfo(manufactureData, manufactureDataLen);
            if (manufactureDataPtr) {
                bleScanFilter.manufactureData.reserve(manufactureDataLen);
                bleScanFilter.manufactureData.assign(manufactureDataPtr, manufactureDataPtr + manufactureDataLen);
            }
            JSI::ReleaseValue(manufactureData);
        }

        // 制造商数据掩码
        JSIValue manufactureDataMask = JSI::GetNamedProperty(filter, "manufactureDataMask");
        size_t manufactureDataMaskLen = 0;
        if (!JSI::ValueIsUndefined(manufactureDataMask)) {
            uint8_t *manufactureDataMaskPtr = JSI::GetArrayBufferInfo(manufactureDataMask, manufactureDataMaskLen);
            if (manufactureDataMaskPtr) {
                bleScanFilter.manufactureDataMask.reserve(manufactureDataMaskLen);
                bleScanFilter.manufactureDataMask.assign(manufactureDataMaskPtr, manufactureDataMaskPtr +
                    manufactureDataMaskLen);
            }
            JSI::ReleaseValue(manufactureDataMask);
        }

        BleModule::bleScanFilters.push_back(bleScanFilter);

        // 释放所有JSIValue
        JSI::ReleaseValueList(jsiDeviceId, jsiName, jsiServiceUuid, jsiServiceUuidMask,
            jsiServiceSolicitationUuid, jsiServiceSolicitationUuidMask, serviceData,
            serviceDataMask, jsiManufactureId, manufactureData, manufactureDataMask);
        JSI::ReleaseValue(filter);
    }
}

static void CheckScanEnv(const JSIValue thisVal)
{
    // 清空历史数据和过滤条件
    {
        std::lock_guard<std::mutex> lock(BleModule::scanResultMutex);
        BleModule::scanResult.clear();
    }
    BleModule::bleScanFilters.clear();
    // 确保之前的扫描已完全停止
    if (BleModule::workerThreadRunning) {
        HILOG_WARN(HILOG_MODULE_ACE, "Previous scan not fully stopped, stopping it first");
        BleModule::StopBLEScan(thisVal, nullptr, 0);
        osDelay(50); // 短暂延迟确保完全停止
    }
}

static errcode_t CheckScanArguments(const JSIValue *args, uint8_t argsSize) {
    if (argsSize > ARGC_TWO || argsSize < ARGC_ONE || args == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "1 or 2 arguments are required.");
        return BLE_INVALID_PARAMETER;
    }
    return BLE_SUCCESS;
}

static errcode_t ParseFilters(const JSIValue &filters) {
    if (!JSI::ValueIsArray(filters)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Array buffer is required.");
        return BLE_INVALID_PARAMETER;
    }
    GetFilters(filters);
    return BLE_SUCCESS;
}

static void ConfigureScanParameters(const JSIValue &options, gap_ble_scan_params_t &scan_param) {
    JSIValue jsiInterval = JSI::GetNamedProperty(options, "interval");
    if (!JSI::ValueIsUndefined(jsiInterval)) {
        BleModule::scanOptions.interval = JSI::ValueToNumber(jsiInterval);
        JSI::ReleaseValue(jsiInterval);
    }
    JSIValue jsiDutyMode = JSI::GetNamedProperty(options, "dutyMode");
    if (!JSI::ValueIsUndefined(jsiDutyMode)) {
        BleModule::scanOptions.dutyMode = (ScanDuty)JSI::ValueToNumber(jsiDutyMode);
        JSI::ReleaseValue(jsiDutyMode);
    }
    JSIValue jsiMatchMode = JSI::GetNamedProperty(options, "matchMode");
    if (!JSI::ValueIsUndefined(jsiMatchMode)) {
        BleModule::scanOptions.matchMode = (MatchMode)JSI::ValueToNumber(jsiMatchMode);
        JSI::ReleaseValue(jsiMatchMode);
    }
    JSIValue jsiPhyType = JSI::GetNamedProperty(options, "phyType");
    if (!JSI::ValueIsUndefined(jsiPhyType)) {
        BleModule::scanOptions.phyType = (PhyType)JSI::ValueToNumber(jsiPhyType);
        JSI::ReleaseValue(jsiPhyType);
    }
    JSIValue jsiReportMode = JSI::GetNamedProperty(options, "reportMode");
    if (!JSI::ValueIsUndefined(jsiReportMode)) {
        BleModule::scanOptions.reportMode = (ScanReportMode)JSI::ValueToNumber(jsiReportMode);
        JSI::ReleaseValue(jsiReportMode);
    }

    scan_param.scan_interval = BleModule::scanOptions.interval;
    scan_param.scan_window = BleModule::scanOptions.interval;

    if (BleModule::scanOptions.matchMode == 1) {
        scan_param.scan_type = 0x01;  // 主动扫描
    } else if (BleModule::scanOptions.matchMode == 2) {
        scan_param.scan_type = 0x00;  // 被动扫描
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE, "Invalid matchMode parameter: %d", BleModule::scanOptions.matchMode);
        return;
    }

    if (BleModule::scanOptions.phyType == 1) {
        scan_param.scan_phy = BleModule::scanOptions.phyType;
    } else {
        scan_param.scan_phy = 0x02;  // 默认使用LE 1M PHY
    }
    scan_param.scan_filter_policy = 0x00;
}

static void ConfigureDefaultScanParameters(gap_ble_scan_params_t &scan_param) {
    scan_param.scan_interval = 48;  // 48 * 0.625ms = 30ms
    scan_param.scan_window = 48;    // 48 * 0.625ms = 30ms
    scan_param.scan_type = 1;       // 主动扫描
    scan_param.scan_phy = 1;        // LE 1M PHY
    scan_param.scan_filter_policy = 0;
}

static errcode_t StartWorkerThread() {
    if (!BleModule::workerThreadRunning) {
        BleModule::workerThreadRunning = true;
        
        // 创建线程属性
        osThreadAttr_t attr;
        attr.name = "BLEScanWorker";
        attr.attr_bits = 0U;
        attr.cb_mem = NULL;
        attr.cb_size = 0U;
        attr.stack_mem = NULL;
        attr.stack_size = 4096;  // 4096:4KB栈空间
        attr.priority = osPriorityNormal;
        
        // 创建线程
        BleModule::workerThreadId = osThreadNew(WorkerThreadFunc, NULL, &attr);
        if (BleModule::workerThreadId == NULL) {
            HILOG_ERROR(HILOG_MODULE_ACE, "Failed to create worker thread");
            BleModule::workerThreadRunning = false;
            return BLE_OPRATION_FAILED;
        }
    }
    return BLE_SUCCESS;
}

static errcode_t SetAndStartScan(gap_ble_scan_params_t &scan_param) {
    errcode_t ret = gap_ble_set_scan_parameters(&scan_param);
    if (ret != BLE_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "ble set scan parameters failed: %X", ret);
        
        // 停止工作线程
        BleModule::workerThreadRunning = false;
        BleModule::queueCond.notify_all();

        // 等待线程退出
        if (BleModule::workerThreadId != NULL) {
            osThreadJoin(BleModule::workerThreadId);
            BleModule::workerThreadId = NULL;
        }
        
        return ret;
    }

    ret = gap_ble_start_scan();
    if (ret != BLE_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "ble start scan failed: %X", ret);
        
        // 停止工作线程
        BleModule::workerThreadRunning = false;
        BleModule::queueCond.notify_all();
        
        // 等待线程退出
        if (BleModule::workerThreadId != NULL) {
            osThreadJoin(BleModule::workerThreadId);
            BleModule::workerThreadId = NULL;
        }
    }
    return ret;
}

JSIValue BleModule::StartBLEScan(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize) {
    errcode_t ret = CheckScanArguments(args, argsSize);
    if (ret != BLE_SUCCESS) {
        return JSI::CreateNumber(ret);
    }

    CheckScanEnv(thisVal);

    // 解析过滤条件
    JSIValue filters = JSI::AcquireValue(args[0]);
    ret = ParseFilters(filters);
    JSI::ReleaseValue(filters);
    if (ret != BLE_SUCCESS) {
        return JSI::CreateNumber(ret);
    }

    // 配置扫描参数
    gap_ble_scan_params_t scan_param = {0};
    if (argsSize == ARGC_TWO) {
        JSIValue options = JSI::AcquireValue(args[1]);
        ConfigureScanParameters(options, scan_param);
        JSI::ReleaseValue(options);
    } else {
        ConfigureDefaultScanParameters(scan_param);
    }

    // 启动工作线程
    ret = StartWorkerThread();
    if (ret != BLE_SUCCESS) {
        return JSI::CreateNumber(ret);
    }

    // 设置并启动扫描
    ret = SetAndStartScan(scan_param);
    return JSI::CreateNumber(ret);
}

JSIValue BleModule::StopBLEScan(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    // 先停止扫描，避免回调持续触发
    errcode_t ret = gap_ble_stop_scan();
    if (ret != BLE_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_ACE, "ble stop scan failed: %X", ret);
    }

    // 停止工作线程
    if (workerThreadRunning) {
        workerThreadRunning = false;
        queueCond.notify_all();
        
        // 等待线程退出
        if (workerThreadId != NULL) {
            osThreadJoin(workerThreadId);
            workerThreadId = NULL;
        }
    }

    // 清空队列
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        while (!scanDataQueue.empty()) {
            gap_scan_result_data_t scanData = scanDataQueue.front();
            if (scanData.adv_data) {
                free(scanData.adv_data);
                scanData.adv_data = nullptr;
            }
            scanDataQueue.pop();
        }
    }

    // 清空过滤条件
    bleScanFilters.clear();

    return JSI::CreateNumber(ret);
}
}  // namespace ACELite
}  // namespace OHOS

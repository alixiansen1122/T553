/*
 * Copyright (c) CompanyNameMagicTag. 2025. All rights reserved.
 * Description: http control interfaces
 * Author: Software Group
 * Create: 2025-08-06
 */

#ifndef BLE_CLIENT_MODULE_H
#define BLE_CLIENT_MODULE_H

#include "ble_module_common.h"
#include "los_task.h" 
#include "bts_le_gap.h"
#include "bts_gatt_server.h"
#include "cmsis_os2.h"
#include "ohos_init.h"
#include <queue>
#include <mutex>
#include <condition_variable>

namespace OHOS {
namespace ACELite {
class BleModule final : public MemoryHeap {
public:
    BleModule() = default;
    ~BleModule() = default;
    static JSIValue JSGetMtu(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue Connect(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue DisConnect(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue GetServices(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue ReadCharacteristicValue(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue WriteCharacteristicValue(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue ReadDescriptorValue(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue WriteDescriptorValue(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue GetDeviceName(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue GetRssiValue(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue GetDeviceMtu(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue ClientClose(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue CreateGattClientDevice(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue StartBLEScan(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue StopBLEScan(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue StartAdvertising(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue StopAdvertising(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue SetCharacteristicChangeIndication(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue SetCharacteristicChangeNotification(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue On(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue Off(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue ClientOff(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue ClientOn(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue CreateGattServer(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue AddService(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue NotifyCharacteristicChanged(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue ServerOff(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue ServerOn(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue ServerClose(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue RemoveService(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static void CheckSubscription(const std::string &type, const JSIValue err, const JSIValue value,
        const JSIValue deviceId);
    static void Destroy();
    static void GetAdvertiseData(AdvertisingParams &advertisingParams, JSIValue jsiAdvertisingParams, const char *type,
        bool *hasResponse);
    static void GetAdvertiseWithCallback(const JSIValue *args, uint8_t argsSize, AdvertisingParams &advertisingParams,
        bool *hasResponse);
    static void GetAdvertiseWithParame(const JSIValue *args, uint8_t argsSize, AdvertiseSetting &setting,
        AdvertiseData &advData, AdvertiseData &advResponse, bool *hasResponse);
    static void DiscoverCharacteristicsForService(uint8_t client_id, uint16_t conn_id);
    static void StartDescriptorDiscovery(uint8_t client_id, uint16_t conn_id);
    static DiscoveryContext* GetValidDiscoveryContext(uint16_t connId);
    static void ProcessNextDescriptorDiscovery(uint8_t client_id, uint16_t conn_id);
    // 辅助函数：根据UUID查找特征值
    static bool FindCharacteristic(const uint16_t charHandle, const std::string& serviceUuid,
        const std::string& charUuid, BLECharacteristic& outChar, uint16_t connId);
    static void CleanupConnectionResources(uint16_t conn_id);
    // 辅助函数：根据UUID查找描述符
    static bool FindDescriptor(const uint16_t descHandle, const std::string& serviceUuid, const std::string& charUuid,
        const std::string& descUuid, BLEDescriptor& outDesc, uint16_t connId);
    static void OnTerminate();
    static errcode_t ServerDestroy();
    static void ReleaseEventCallback(const std::string& eventName);
    static std::map<std::string, EventCallback *> eventCallbacks;
    static std::mutex scanResultMutex;
    static std::vector<ScanResult> scanResult;
    static uint8_t clientId;
    static uint8_t serverId;
    static std::mutex addrMapMutex;
    static std::map<std::string, uint8_t> addrMap;
    static std::mutex connectMapMutex;
    static std::map<std::string, DeviceInfo> deviceMap;
    static std::map<uint8_t, uint8_t> advertisingMap;
    static uint8_t advId;
    static bool hasOn;
    // 上下文管理相关静态成员
    static std::map<uint16_t, DiscoveryContext> discoveryContexts;
    static std::mutex contextMutex;
    // 操作上下文管理
    static std::vector<GattOperationContext> operationContexts;
    static std::mutex operationMutex;
    static std::vector<BleScanFilter> bleScanFilters;
    static std::map<uint16_t, std::vector<GattService>> discoveredServicesMap;

    static std::queue<gap_scan_result_data_t> scanDataQueue;
    static std::mutex queueMutex;
    static std::condition_variable queueCond;
    static bool workerThreadRunning;
    static osThreadId_t workerThreadId;

    static ScanOptions scanOptions;

    // 全局服务映射表
    static std::map<std::string, uint16_t> g_serviceHandleMap;
    // 服务构建上下文
    static std::mutex serviceBuildContextsMutex;
    static std::map<uint16_t, ServiceBuildContext> g_serviceBuildContexts;
    // 全局特征值句柄映射表
    static std::map<std::string, uint16_t> g_characteristicHandleMap;
    // 删除服务上下文
    static std::map<uint16_t, RemoveServiceContext> g_removeServiceContexts;
    // 通知操作上下文映射
    static std::map<uint16_t, NotifyCharacteristicContext> g_notifyContexts;
    // server notify cbk中handle全局映射表
    static std::map<uint16_t, CharacteristicInfo> g_handleToCharacteristicMap;
    static std::map<uint16_t, DescriptorInfo> g_handleToDescriptorMap;
    // server资源销毁
    static std::mutex serviceCleanMutex;

private:
    static errcode_t ProcessAdvertisingParameters(const JSIValue *args, uint8_t argsSize,
    gap_ble_adv_params_t &adv_param, gap_ble_config_adv_data_t &cfg_adv_data, bool &hasResponse);
    static errcode_t HandleAdvertiseSetting(const JSIValue *args, uint8_t argsSize,
    gap_ble_config_adv_data_t &cfg_adv_data, bool &hasResponse);
    static errcode_t HandleAdvertisingParams(const JSIValue *args, uint8_t argsSize,
    gap_ble_adv_params_t &adv_param, gap_ble_config_adv_data_t &cfg_adv_data, bool &hasResponse);
    static errcode_t ConfigureAndStartAdvertising(gap_ble_adv_params_t &adv_param,
    gap_ble_config_adv_data_t &cfg_adv_data);
    static JSIValue HandleExit(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize, errcode_t ret,
    const gap_ble_config_adv_data_t &cfg_adv_data);
    static bool CheckResponseData(const char *type, JSIValue jsiAdvertisingData, bool *hasResponse);
    static void ProcessServiceUuids(AdvertisingParams &advertisingParams, JSIValue jsiAdvertisingData,
        const char *type);
    static void ProcessManufactureData(AdvertisingParams &advertisingParams, JSIValue jsiAdvertisingData,
        const char *type);
    static void ProcessManufactureValue(JSIValue jsiManufactureDataItem, ManufactureData &manufactureData);
    static void ProcessServiceData(AdvertisingParams &advertisingParams, JSIValue jsiAdvertisingData,
        const char *type);
    static void ProcessServiceValue(JSIValue jsiServiceDataItem, ServiceData &serviceData);
    static void ProcessIncludeDeviceName(AdvertisingParams &advertisingParams, JSIValue jsiAdvertisingData,
        const char *type);
    static std::string GetFirstServerDeviceId(void);

    static errcode_t ReleaseConnect();
    static void ReleaseClose();
    static void DestroyStopScanAndAdv();
    static void ReleaseServerClose();
    static errcode_t ReleaseServerConnect();
    static JSIValue gattClient;
    static JSIValue gattServer;
};
void InitBleModule(JSIValue exports);

}  // namespace ACELite
}  // namespace OHOS
#endif  // BLE_CLIENT_MODULE_H
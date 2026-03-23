/*
 * Copyright (c) CompanyNameMagicTag. 2025. All rights reserved.
 * Description: http control interfaces
 * Author: Software Group
 * Create: 2025-08-06
 */

#ifndef BLE_CLIENT_MODULE_COMMON_H
#define BLE_CLIENT_MODULE_COMMON_H

#include <stdint.h>
#include "jsi.h"
#include "errcode.h"
#include "bts_gatt_client.h"
#include "non_copyable.h"
#include "js_async_work.h"
#include "bts_le_gap.h"
#include "bts_gatt_server.h"
#include <map>
#include <vector>
#include <string> 

namespace OHOS {
namespace ACELite {

#define MAX_CHARACTERISTICVALUE_LEN 128
#define MAX_BLE_NAME_LEN 10
#define MIN_BLE_INTERVEL_LEN 48
#define MAX_BLE_INTERVEL_LEN 96
#define MAX_ADV_DATA_LEN 31
#define NOMAL_MTU_VALUE 512
#define NOMAL_ADV_ID 3
#define MAX_DEVICE_NUM 16

#define BLE_ADDR_LEN 6
#define BLE_INVALID_CHARACTERISTIC_HANDLE 65530
#define BLE_INVALID_DESCRIPTOR_HANDLE 65531

// 蓝牙广播数据类型定义
#define BLE_AD_TYPE_FLAGS 0x01
#define BLE_AD_TYPE_16BIT_SERVICE_UUID 0x03
#define BLE_AD_TYPE_32BIT_SERVICE_UUID 0x05
#define BLE_AD_TYPE_128BIT_SERVICE_UUID 0x07
#define BLE_AD_TYPE_COMPLETE_LOCAL_NAME 0x09
#define BLE_AD_TYPE_MANUFACTURER_SPECIFIC_DATA 0xFF
#define BLE_AD_TYPE_SERVICE_DATA_16BIT 0x16
#define BLE_AD_TYPE_SERVICE_DATA_32BIT 0x20
#define BLE_AD_TYPE_SERVICE_DATA_128BIT 0x21
// 广播数据类型常量定义
constexpr const char* ADVERTISING_DATA = "advertisingData";
constexpr const char* ADVERTISING_RESPONSE = "advertisingResponse";
constexpr const char* SERVICE_UUIDS = "serviceUuids";
constexpr const char* MANUFACTURE_DATA = "manufactureData";
constexpr const char* SERVICE_DATA = "serviceData";
constexpr const char* MANUFACTURE_ID = "manufactureId";
constexpr const char* MANUFACTURE_VALUE = "manufactureValue";
constexpr const char* SERVICE_UUID = "serviceUuid";
constexpr const char* SERVICE_VALUE = "serviceValue";
constexpr const char* INCLUDE_DEVICE_NAME = "includeDeviceName";

constexpr uint8_t ARGC_FOUR = 4;

struct EventCallback {
    JSIValue callback;
    JSIValue context;
};

enum ErrNum {
    BLE_SUCCESS = 0,
    BLE_PERMISSION_DENIED = 201,
    BLE_INVALID_PARAMETER = 401,
    BLE_UNSUPPORT = 801,
    BLE_SERVICE_STOP = 2900001,
    BLE_SERVICE_NOT_FOUND = 2900002,
    BLE_READ_FORBID = 2901000,
    BLE_OPRATION_FAILED = 2900099,
};

enum class MatchType {
    NONE,
    CHARACTERISTIC,
    DESCRIPTOR
};

enum ChannelMap {
    BLE_ONE_CHANEL = 0x1,
    BLE_THREE_CHANEL = 0x7,
};

enum ScanDuty {
    SCAN_MODE_LOW_POWER = 0,
    SCAN_MODE_BALANCED = 1,
    SCAN_MODE_LOW_LATENCY = 2,
    SCAN_MODE_NONE,
};

enum MatchMode {
    MATCH_MODE_AGGRESSIVE = 1,
    MATCH_MODE_STICKY = 2,
    MATCH_MODE_NONE,
};

enum PhyType {
    PHY_LE_1M = 1,
    PHY_LE_ALL_SUPPORTED = 255,
    PHY_LE_NONE,
};

enum ScanReportMode {
    SCAN_REPORT_MODE_NORMAL = 1,
    SCAN_REPORT_MODE_NONE,
};

enum DeviceType {
    BLE_CLIENT = 0,
    BLE_SERVER,
};

struct BLEDescriptor {
    std::string serviceUuid;
    std::string characteristicUuid;
    std::string descriptorUuid;
    std::vector<uint8_t> descriptorValue;
    gattc_discovery_descriptor_result_t descriptor;
};

struct GattProperties {
    bool write;
    bool writeNoResponse;
    bool read;
    bool notify;
    bool indicate;
};

struct BLECharacteristic {
    std::string serviceUuid;
    std::string characteristicUuid;
    std::vector<uint8_t> characteristicValue;
    std::vector<BLEDescriptor> descriptors;
    GattProperties properties;
    gattc_discovery_character_result_t character;
    uint16_t service_start_hdl;
    uint16_t service_end_hdl;
};

struct CallFuncAsyncParams : public MemoryHeap {
    ACE_DISALLOW_COPY_AND_MOVE(CallFuncAsyncParams);
    CallFuncAsyncParams() : err(nullptr), result(nullptr), callback(nullptr), context(nullptr),
        type(), connId(0), characteristicArgs(nullptr){}

    JSIValue err;
    JSIValue result;
    JSIValue callback;
    JSIValue context;
    std::string type;
    uint16_t connId;
    BLECharacteristic *characteristicArgs;
};

struct BleScanFilter {
    std::string deviceId;
    std::string name;
    std::string serviceUuid;
    std::string serviceUuidMask;
    std::string serviceSolicitationUuid;
    std::string serviceSolicitationUuidMask;
    std::vector<uint8_t> serviceData;
    std::vector<uint8_t> serviceDataMask;
    uint16_t manufactureId = 0;
    std::vector<uint8_t> manufactureData;
    std::vector<uint8_t> manufactureDataMask;
};

struct ScanOptions {
    double interval;
    ScanDuty dutyMode;
    MatchMode matchMode;
    PhyType phyType;
    ScanReportMode reportMode;
};

struct ScanResult {
    std::string deviceId;
    int8_t rssi;
    std::vector<uint8_t> data;
    std::string deviceName;
    bool connectable;
};

// AD结构定义
struct AdStruct {
    uint8_t length;       // 类型+数据的总长度
    uint8_t type;         // 类型标志
    std::vector<uint8_t> data; // 数据内容
};

struct BleParsedResult {
    std::vector<std::string> serviceUuids;                     // 服务UUID
    std::vector<std::string> serviceSolicitationUuids;         // 服务请求UUID
    std::vector<std::pair<std::string, std::vector<uint8_t>>> serviceDatas; // 服务数据
    uint16_t manufactureId = 0;                                // 厂商ID
    std::vector<uint8_t> manufactureData;                      // 厂商数据
};

struct ServiceData {
    std::string serviceUuid;
    std::vector<uint8_t> serviceValue;
};

struct ManufactureData {
    uint16_t manufactureId;
    std::vector<uint8_t> manufactureValue;
};

struct AdvertiseSetting {
    double interval;
    double txPower;
    bool connectable;
};

struct AdvertiseData {
    std::vector<std::string> serviceUuids;
    std::vector<ManufactureData> manufactureData;
    std::vector<ServiceData> serviceData;
    bool includeDeviceName;
};


struct AdvertisingParams {
    AdvertiseSetting advertisingSettings;
	AdvertiseData advertisingData;
    AdvertiseData advertisingResponse;
    uint32_t duration;
};

enum ProfileConnectionState {
    STATE_DISCONNECTED = 0,
    STATE_CONNECTING = 1,
    STATE_CONNECTED = 2,
    STATE_DISCONNECTING = 3,
};

enum GattWriteType {
    WRITE = 1,
    WRITE_NO_RESPONSE = 2,
};

enum AdvertisingState {
    STARTED = 1, // advertising started.
    ENABLED = 2, // advertising temporarily enabled.
    DISABLED = 3, // advertising temporarily disabled.
    STOPPED = 4, // advertising stopped.
};

struct AdvertisingStateChangeInfo {
    uint32_t advertisingId;
    AdvertisingState state;
};

struct BLEConnectionChangeState {
    std::string deviceId;
	ProfileConnectionState state;
};

struct GattService {
    std::string serviceUuid;
    bool isPrimary;
    std::vector<BLECharacteristic> characteristics;
    std::vector<GattService> includeServices;
    gattc_discovery_service_result_t service;
    GattService() : isPrimary(true) {}
};

// 发现过程上下文，管理单个连接的服务发现状态
struct DiscoveryContext {
    uint16_t connId;
    JSIValue callback;
    JSIValue thisVal;
    uint32_t currentServiceIndex;
    uint32_t currentCharIndex;
    uint16_t expectedDescCount;   // 预期描述符总数
    uint16_t receivedDescCount;   // 已接收描述符计数

    DiscoveryContext() : connId(0), callback(JSI::CreateUndefined()), thisVal(JSI::CreateUndefined()),
        expectedDescCount(0), receivedDescCount(0) {}
};

// 读写请求上下文，用于匹配请求和回调结果
struct GattOperationContext {
    enum Type {
        READ_CHARACTERISTIC,
        WRITE_CHARACTERISTIC,
        READ_DESCRIPTOR,
        WRITE_DESCRIPTOR
    };

    Type type;
    uint16_t connId; // 关联的连接ID
    std::string serviceUuid;
    std::string charUuid;
    std::string descUuid;
    uint16_t charHandle;
    uint16_t descHandle;
    JSIValue callback;
    JSIValue thisVal;
    GattWriteType writeType; // 仅用于写操作
};

struct DiscoveryData {
    uint16_t connId;
    JSIValue callback;
    JSIValue thisVal;

    ~DiscoveryData() {
        JSI::ReleaseValueList(callback, thisVal);
    }
};

struct DispatchAsync {
    JSIValue err;
    JSIValue result;
    JSIValue callback;
    JSIValue thisVal;
    std::string type;
    uint16_t connId;
    BLECharacteristic characteristicArgs;
};

// 服务构建上下文
struct ServiceBuildContext {
    GattService* originalService;  // 原始服务定义
    GattService* resultService;    // 包含句柄的结果服务
    JSIValue callback;
    JSIValue thisVal;
    uint16_t serviceHandle;
};

// 删除服务上下文
struct RemoveServiceContext {
    JSIValue callback;
    JSIValue thisVal;
    std::string serviceUuid;
    uint16_t serviceHandle;
};

// 通知特征值结构
struct NotifyCharacteristicContext {
    std::string deviceId;
    std::string serviceUuid;
    std::string characteristicUuid;
    std::vector<uint8_t> characteristicValue;
    bool confirm; // true: indicate, false: notify
    JSIValue callback;
    JSIValue thisVal;
};

struct ReadWriteRequestContext {
    std::string deviceId;
    uint16_t transId;
    uint16_t offset;
    std::vector<uint8_t> value;
    std::string serviceUuid;
    std::string characteristicUuid;
    std::string descriptorUuid;
    bool needRsp;
    bool isPrepared;
};

// 句柄到特征值信息的映射
struct CharacteristicInfo {
    std::string serviceUuid;
    std::string characteristicUuid;
    uint16_t valueHandle; // 特征值句柄
};

struct DescriptorInfo {
    std::string serviceUuid;
    std::string characteristicUuid;
    std::string descriptorUuid;
};

struct DeviceInfo {
    uint32_t mtu;
    uint16_t connId;
    ProfileConnectionState connState;
    DeviceType deviceType;
};

uint8_t Char2Num(int8_t c);
bool CompareHexStrings(const std::string& str1, const std::string& str2);
errcode_t DecodeUuidFromStr(const char *uuidStr, uint8_t *uuid, int32_t *cnt);
std::string FormatMacAddress(const uint8_t addr[BLE_ADDR_LEN]);
errcode_t ParseUuidList(const uint8_t *data, uint8_t len, uint8_t uuid_size, std::vector<std::string> &uuid_list);
bool CheckUuidMatch(const std::string &uuid, const std::string &filter, const std::string &mask);
std::string GetUUidStr(const bt_uuid_t &uuid);
void GetAttValueData(uint8_t *dataVec, bool enable, bool indication);
errcode_t SetJsiBLECharacteristic(JSIValue jsiBleCharacteristic, uint16_t connId,
    BLECharacteristic *bleCharacteristicArgs);
void DiscoveryWorkerThread(void* arg);
void BleDispatchAsyncWork(const DispatchAsync &asyncParameter);
void GapBleEnableStackCbk(errcode_t status);
void GapBleDisableStackCbk(errcode_t status);
void GapBleSetAdvDataCbk(uint8_t adv_id, errcode_t status);
void GapBleSetAdvParamCbk(uint8_t advId, errcode_t status);
void GapBleSetScanParamCbk(errcode_t status);
void GapBleStartAdvCbk(uint8_t advId, adv_status_t status);
void GapBleStopAdvCbk(uint8_t advId, adv_status_t status);
void GattcIndicationCbk(uint8_t client_id, uint16_t connId, gattc_handle_value_t *data, errcode_t status);
void GattcNotificationdCbk(uint8_t client_id, uint16_t connId, gattc_handle_value_t *data, errcode_t status);
void GattsAddServiceCbk(uint8_t server_id, bt_uuid_t *uuid, uint16_t handle, errcode_t status);
void GattsAddCharacteristicCbk(uint8_t server_id, bt_uuid_t *uuid, uint16_t service_handle,
    gatts_add_character_result_t *result, errcode_t status);
void GattsAddDescriptorCbk(
    uint8_t server_id, bt_uuid_t *uuid, uint16_t service_handle, uint16_t handle, errcode_t status);
void GattsStartServiceCbk(uint8_t server_id, uint16_t handle, errcode_t status);
void GattsStopServiceCbk(uint8_t server_id, uint16_t handle, errcode_t status);
void GattsReadRequestCbk(uint8_t serverId, uint16_t connId, gatts_req_read_cb_t *readCbPara, errcode_t status);
void GattsWriteRequestCbk(uint8_t server_id, uint16_t conn_id, gatts_req_write_cb_t *write_cb_para, errcode_t status);
void GattsMtuChangedCbk(uint8_t server_id, uint16_t conn_id, uint16_t mtu_size, errcode_t status);
void GattcMtuChangedCbk(uint8_t client_id, uint16_t conn_id, uint16_t mtu_size, errcode_t status);
void GattcWriteCfmCbk(uint8_t client_id, uint16_t conn_id, uint16_t handle, gatt_status_t status);
void GattcReadByUuidRespone(uint8_t client_id, uint16_t conn_id,
    gattc_read_req_by_uuid_param_t *param, errcode_t status);
void GattcReadCfmCbk(uint8_t client_id, uint16_t conn_id, gattc_handle_value_t *read_result, gatt_status_t status);
void GattcDiscoveryDescResponse(uint8_t client_id, uint16_t conn_id, uint16_t character_handle,
    errcode_t status);
void GattcDiscoveryDescCbk(uint8_t client_id, uint16_t conn_id,
    gattc_discovery_descriptor_result_t* descriptor, errcode_t status);
void GattcDiscoveryCharResponse(uint8_t client_id, uint16_t conn_id, gattc_discovery_character_param_t *param,
    errcode_t status);
void GattcDiscoveryCharCbk(uint8_t client_id, uint16_t conn_id,
    gattc_discovery_character_result_t *character, errcode_t status);
void GattcDiscoverySvcResponse(uint8_t client_id, uint16_t conn_id, bt_uuid_t *uuid, errcode_t status);
void GattcDiscoverySvcCbk(uint8_t client_id, uint16_t conn_id, gattc_discovery_service_result_t *service,
    errcode_t status);
void GapBleAuthCompeteCbk(uint16_t conn_id, const bd_addr_t *addr, errcode_t status,
    const ble_auth_info_evt_t* evt);
void GapBleReadRemoteRssiCbk(uint16_t conn_id, int8_t rssi, errcode_t status);
void GapBleConnectParamUpdateCbk(uint16_t conn_id, errcode_t status,
    const gap_ble_conn_param_update_t *param);
void GapBleTerminateAdvCbk(uint8_t adv_id, adv_status_t status);
void GapBlePariedCompleteCbk(uint16_t conn_id, const bd_addr_t *addr, errcode_t status);
void GapBleConnStateChangeCbk(uint16_t conn_id, bd_addr_t *addr, gap_ble_conn_state_t conn_state,
    gap_ble_pair_state_t pair_state, gap_ble_disc_reason_t disc_reason);
void GapBleScanResultCbk(gap_scan_result_data_t *scan_result_data);
void GattsStartServiceCbk(uint8_t server_id, uint16_t handle, errcode_t status);
void GattsStopServiceCbk(uint8_t server_id, uint16_t handle, errcode_t status);
void WorkerThreadFunc(void *argument);
void GetCharacteristic(const JSIValue *args, BLECharacteristic &bleCharacteristicArgs, bool isRead);
void GetProperty(const JSIValue jsiCharacteristic, BLECharacteristic &bleCharacteristicArgs);
bool ValidateWriteParameters(uint8_t argsSize, const JSIValue* args);
// 解析参数函数
errcode_t ParseWriteArguments(const JSIValue* args, BLECharacteristic& bleCharacteristicArgs,
    GattWriteType& writeType);
std::string ParseDeviceId(const JSIValue args);
void InitDiscoveryWorker();
void GetProperties(uint8_t properties, GattProperties &props);
void CreateGattServices(JSIValue *rltArray, const std::vector<GattService> &services);
JSIValue ConstructJSIResult(const gattc_handle_value_t *read_result, const BLECharacteristic *charFound,
    const BLEDescriptor *descFound, const std::string &serviceUuid, const std::string &charUuid,
    const std::string &descUuid);
std::string FormatDeviceName(gap_scan_result_data_t *scan_result_data);
BleParsedResult ParseMixedAdvertisingData(const uint8_t *data, uint8_t len);
bool CheckBasicDeviceMatch(const BleScanFilter& filter, const ScanResult& scanResultTmp);
bool CheckUuidMatch(const BleScanFilter& filter, const BleParsedResult& parsedResult);
bool CheckDataMatch(const BleScanFilter& filter, const BleParsedResult& parsedResult);
bool ReadMatchFilter(const BleScanFilter& filter, const BleParsedResult& parsedResult, 
    const ScanResult& scanResultTmp);
void UpdateScanResult(const ScanResult &scanResultTmp, std::vector<ScanResult>::iterator existingIt,
    std::vector<ScanResult> &scanResultList);

// gatts
errcode_t PerformAddServiceOperation(const GattService& gattService, GattService& resultService, uint16_t& serviceHandle);
JSIValue CreateJsiGattService(const GattService& service);
void ExecuteAddServiceCallback(const JSIValue thisVal, const JSIValue* args, errcode_t ret);
void ExecuteRemoveServiceCallback(const JSIValue thisVal, const JSIValue callback, errcode_t ret);
void RemoveAllCharacteristicHandlesByService(const std::string& serviceUuid);

}  // namespace ACELite
}  // namespace OHOS
#endif  // BLE_CLIENT_MODULE_COMMON_H
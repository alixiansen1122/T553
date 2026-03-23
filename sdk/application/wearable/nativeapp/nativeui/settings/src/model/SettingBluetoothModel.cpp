/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingSettingBluetoothModel
 * Created: 2025-06-05
 */

#include <cstdint>
#include "bluetooth/BluetoothWrapper.h"
#include "player/PlayersModel.h"
#include "phoneservice/PhoneAudio.h"
#include "graphic_service.h"
#include "msg_center.h"
#include "bts_def.h"
#include "bts_gatt_stru.h"
#include "bts_a2dp_source.h"
#include "bts_gatt_server.h"
#include "bts_le_gap.h"
#include "bts_spp.h"
#include "osal_task.h"
#ifdef SUPPORT_ALIPAY_SEC
#include "alipay_feature.h"
#endif
#include "bts_a2dp_sink.h"
#include "bts_feature.h"
#include "bt_audio_hal_interface.h"
#include "audio_manager_c_wrapper.h"
#include "bts_avrcp_target.h"
#include "msg_center_msg.h"
#include "client/msg_center_client.h"
#include "msg_center_protocol.h"
#include "dfx_channel.h"
#include "tcxo.h"
#include "uiservice/ui_service.h"
#include "network/network_listener.h"
#if defined(SUPPORT_LWIP) && defined(SUPPORT_BREDR)
#include "lwip_public.h"
#endif
#include "main/PullUpDownView.h"
#include "settings/common/SettingCommon.h"
#include "settings/page/SettingBluetoothPage.h"
#include "settings/page/SettingConnectionOpen.h"
#include "findphone/FindPhoneMainPage.h"
#include "bluetooth/BluetoothChangeBle.h"
#include "settings/model/SettingBluetoothModel.h"

namespace OHOS {
constexpr uint16_t SETTING_BLUETOOTH_CONN_HANDLE = 64;
constexpr uint16_t SETTING_BLUETOOTH_INTERVAL_TOMAX_MIN = 12;
constexpr uint16_t SETTING_BLUETOOTH_INTERVAL_TOMAX_MAX = 12;
constexpr uint16_t SETTING_BLUETOOTH_INTERVAL_TODEFAULT_MIN = 24;
constexpr uint16_t SETTING_BLUETOOTH_INTERVAL_TODEFAULT_MAX = 48;
constexpr uint16_t SETTING_BLUETOOTH_LATENCY = 0;
constexpr uint16_t SETTING_BLUETOOTH_TIMEOUT_MUTIPLIER = 500;
#define BT_SPP_TASK_STACK_SIZE 2048
#define BT_SPP_TASK_PRI        20
#define TEN                    10

static constexpr uint16_t BLUETOOTH_MAX_NUM = 20;
static constexpr int16_t SETTING_ADDR2 = 2;
static constexpr int16_t SETTING_ADDR3 = 3;
static constexpr int16_t SETTING_ADDR4 = 4;
static constexpr int16_t SETTING_ADDR5 = 5;
static constexpr int16_t BLUETOOTH_ADDR_SET = 8;
static constexpr int16_t WRITECB_PARA_LEGNTH_TWO = 2;
static constexpr int16_t WRITECB_PARA_LEGNTH_NINE = 9;
static constexpr int16_t WRITECB_PARA_LEGNTH_TEN = 10;
static constexpr int16_t WAIT_TIME = 50;
#define MEDIA_A2DP_SNK_INTERFACE 1
#define A2DP_STREAM_ACCEPT_TIMER_PERIOD 500
osal_timer g_setting_a2dp_stream_accept_timer = {nullptr, nullptr, 0, 0};
AudioFormatConfig g_setting_audio_format_config = {AUDIO_DEFAULT, 0, 0};
static uint16_t g_sample_svr_handle = 0;
static uint16_t g_sample_characteristic_handle = 0;
static uint16_t g_diag_characteristic_handle = 0;
static uint16_t g_diag_desc_handle = 0;
constexpr uint16_t g_sample_adv_id = 2;
static bd_addr_t g_bd_addr = { 0 };

SettingBluetoothModel::SettingBluetoothModel()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SettingBluetoothModel::SettingBluetoothModel");
}

SettingBluetoothModel *SettingBluetoothModel::GetInstance(void)
{
    static SettingBluetoothModel instance;
    return &instance;
}

char* SettingBluetoothModel::GetWatchName(void)
{
    return watchName_;
}

// 扫描设备 btEarPhoneList存在数据 设置为1 否则设置为0
void SettingBluetoothModel::SetEarPhoneListHasData(bool status)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SettingBluetoothModel EarPhoneListHasData_ =  %d", status);
    EarPhoneListHasData_ = status;
}

uint32_t SettingBluetoothModel::GetEarPhoneListHasData()
{
    return EarPhoneListHasData_;
}

void SettingBluetoothModel::SetDeviceName(std::string deviceName)
{
    deviceName_ = deviceName;
}

std::string SettingBluetoothModel::GetDeviceName()
{
    return deviceName_;
}

void SettingBluetoothModel::SetBlueToothConnectStatus(bool status)
{
    btConnectStatus_ = status;
}

bool SettingBluetoothModel::GetBlueToothConnectStatus(void)
{
    return btConnectStatus_;
}

void SettingBluetoothModel::SetConnectFragmentStatus(bool status)
{
    connectFragmentStatus_ = status;
}

bool SettingBluetoothModel::GetConnectFragmentStatus(void)
{
    return connectFragmentStatus_;
}

void SettingBluetoothModel::MoveMatchingDeviceToFront()
{
    std::list<BtCaseInfo> matchedItems;
    std::list<BtCaseInfo> otherItems;
    if(btScansDevicesList_.size() == 0) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "MoveMatchingDeviceToFront btScansDevicesList_ size = 0");
        return;
    }
    bd_addr_t bdAddr = *(GetLastConnedAddr());
    for (auto it = btScansDevicesList_.begin(); it != btScansDevicesList_.end(); ++it) {
        if ((bdAddr.addr[0] == it->addr[0]) && (bdAddr.addr[1] == it->addr[1])
                && (bdAddr.addr[SETTING_ADDR2] == it->addr[SETTING_ADDR2])
                && (bdAddr.addr[SETTING_ADDR3] == it->addr[SETTING_ADDR3])
                && (bdAddr.addr[SETTING_ADDR4] == it->addr[SETTING_ADDR4])
                && (bdAddr.addr[SETTING_ADDR5] == it->addr[SETTING_ADDR5])) {
            matchedItems.push_back(*it);
        } else {
            otherItems.push_back(*it);
        }
    }

    btScansDevicesList_.clear();
    btScansDevicesList_.insert(btScansDevicesList_.begin(), matchedItems.begin(), matchedItems.end());
    btScansDevicesList_.insert(btScansDevicesList_.end(), otherItems.begin(), otherItems.end());
}

void SettingBluetoothModel::UpdateConnectStatusForDevice()
{
    for (auto& item : btScansDevicesList_) {
        if (std::strcmp(reinterpret_cast<const char*>(item.deviceName), deviceName_.c_str()) == 0) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "UpdateConnectStatusForDevice deviceName:%s", item.deviceName);
            item.connect_status = "连接中...";
        }
    }
}

void SettingBluetoothModel::UpdateDisconnectStatusForDevice(const bd_addr_t &bdAddr)
{
    for (auto it = btScansDevicesList_.begin(); it != btScansDevicesList_.end(); ++it) {
        if ((bdAddr.addr[0] == it->addr[0]) && (bdAddr.addr[1] == it->addr[1])
                && (bdAddr.addr[SETTING_ADDR2] == it->addr[SETTING_ADDR2])
                && (bdAddr.addr[SETTING_ADDR3] == it->addr[SETTING_ADDR3])
                && (bdAddr.addr[SETTING_ADDR4] == it->addr[SETTING_ADDR4])
                && (bdAddr.addr[SETTING_ADDR5] == it->addr[SETTING_ADDR5])) {
            it->connect_status = "连接已经断开";
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "UpdateDisconnectStatusForDevice addr: %02x%02x%02x%02x%02x%02x",
            it->addr[SETTING_ADDR5], it->addr[SETTING_ADDR4], it->addr[SETTING_ADDR3],
            it->addr[SETTING_ADDR2], it->addr[1], it->addr[0]);
        }
    }
}

void SettingBluetoothModel::UpdateConnectedStatusForDevice(const bd_addr_t &bdAddr)
{
    for (auto it = btScansDevicesList_.begin(); it != btScansDevicesList_.end(); ++it) {
        if ((bdAddr.addr[0] == it->addr[0]) && (bdAddr.addr[1] == it->addr[1])
                && (bdAddr.addr[SETTING_ADDR2] == it->addr[SETTING_ADDR2])
                && (bdAddr.addr[SETTING_ADDR3] == it->addr[SETTING_ADDR3])
                && (bdAddr.addr[SETTING_ADDR4] == it->addr[SETTING_ADDR4])
                && (bdAddr.addr[SETTING_ADDR5] == it->addr[SETTING_ADDR5])) {
            it->connect_status = "已连接";
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "UpdateConnectedStatusForDevice addr: %02x%02x%02x%02x%02x%02x",
            it->addr[SETTING_ADDR5], it->addr[SETTING_ADDR4], it->addr[SETTING_ADDR3],
            it->addr[SETTING_ADDR2], it->addr[1], it->addr[0]);
        }
    }
}

bool SettingBluetoothModel::GetConnectResultPage()
{
    return redirct_;
}

void SettingBluetoothModel::HandleUnpairRequest(void)
{
    bool ret;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SettingSettingBluetoothModel::HandleUnpairRequest lastConnedAddr_: %02x%02x%02x%02x%02x%02x",
        selectedDeviceInfo_.addr[SETTING_ADDR5], selectedDeviceInfo_.addr[SETTING_ADDR4], selectedDeviceInfo_.addr[SETTING_ADDR3],
        selectedDeviceInfo_.addr[SETTING_ADDR2], selectedDeviceInfo_.addr[1], selectedDeviceInfo_.addr[0]);
    ret = gap_remove_pair((const bd_addr_t*)&selectedDeviceInfo_.addr);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SettingSettingBluetoothModel::HandleUnpairRequest gap_remove_pair %d", ret);
}

// 是否需要显示连接成功/失败的页面 在连接新耳机或者断开耳机连接时改变为true
void SettingBluetoothModel::SetConnectResultPage(bool redirct)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SettingBuletoothModel::SetRedirect :%d", redirct);
    redirct_ = redirct;
}

void SettingBluetoothModel::InitBluetoothInfo(void)
{
    errcode_t ret;
    unsigned char nameLen = BD_NAME_LEN;
    memset_s(watchName_, sizeof(watchName_), 0, sizeof(watchName_));
    ret = bluetooth_get_local_name((unsigned char *)watchName_, &nameLen);
    // 为了实验室测试需要，客户需要自己修改
    if ((nameLen == strlen("WatchDemo") + 1) && (strcmp("WatchDemo", watchName_) == 0)) {
        uint32_t currentUs = uapi_tcxo_get_us();
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "currentUs 0x%x", currentUs);
        // 设置蓝牙名称
        (void)sprintf_s(watchName_, sizeof(watchName_), "Watch%x", (currentUs & 0xFFFF));
        ret = bluetooth_set_local_name((unsigned char*)watchName_, strlen((char*)watchName_) + 1);
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "bluetooth_set_local_name %s ret = %u", watchName_, ret);

        //  设置蓝牙地址
        bd_addr_t addr = {{0x1C, 0x1D, 0x2B, 0x0, 0x0, 0x0} ,0};
        addr.addr[3] = currentUs & 0xFF;
        addr.addr[4] = (currentUs & 0XFF00) >> 8;
        addr.addr[5] = currentUs & 0xFF;
        ret = bluetooth_set_local_addr(addr.addr, BD_ADDR_LEN);
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "bluetooth_set_local_addr [0x%x:0x%x] ret = %u", addr.addr[0], addr.addr[1], ret);
    } else {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "watchName_: %s", watchName_);
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "nameLen: %u", nameLen);
    }
}

void SettingBluetoothModel::Init()
{
#ifndef SUPPORT_AUTO_OTA
    errcode_t ret;

    // 设置自动回连，ios一键双连依赖此配置
    bts_feature_t bts_feature = { 0 };
    bts_feature.bt_auto_reconnect_closed = 0;
    ret = bts_set_features(&bts_feature);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "bt bts_set_features ret = %u", ret);

    ret = enable_bt_stack();
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "bt enable_bt_stack ret = %u", ret);

    // 设置安全连接
    gap_ble_sec_params_t sec_param = { 1, GAP_BLE_IO_CAPABILITY_NOINPUTNOOUTPUT, 1, GAP_BLE_GAP_SECURITY_MODE1_LEVEL3 };
    ret = gap_ble_set_sec_param(&sec_param);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "bt gap_ble_set_sec_param ret = %u", ret);

    InitBluetoothInfo();
    SettingBtInitCallBacks();
    wrapper_ble_register();
#ifdef SUPPORT_ALIPAY_SEC
    svr_alipay_bluetooth_init();
#endif
    SettingBtInitCallBacksEx();
    SettingBtInitGatts();
    SettingBtInitSpp();

    /* 实验室测试需要，开机开广播 */
    BleSettingAdvStart();

    ClearScansDevicesList();
    SetUpPairedDevicesList();
#endif
}

void SettingBluetoothModel::ClearScansDevicesList()
{
    btScansDevicesList_.clear();
}

bool SettingBluetoothModel::CheckExistScanList(BtCaseInfo caseInfo)
{
    std::list<BtCaseInfo>::iterator node = btScansDevicesList_.begin();
    if (btScansDevicesList_.empty()) {
        return false;
    }
    while (node != btScansDevicesList_.end()) {
        if ((caseInfo.addr[0] == node->addr[0]) && (caseInfo.addr[1] == node->addr[1])
            && (caseInfo.addr[2] == node->addr[2]) && (caseInfo.addr[3] == node->addr[3])
            && (caseInfo.addr[4] == node->addr[4]) && (caseInfo.addr[5] == node->addr[5])) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "true");
            return true;
        }
        node++;
    }
    return false;
}

std::list<BtCaseInfo>& SettingBluetoothModel::GetScansDevicesList()
{
    return btScansDevicesList_;
}

void SettingBluetoothModel::ScansDevicesListAddCase(BtCaseInfo caseInfo)
{
    SetEarPhoneListHasData(true);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "ScansDevicesListAddCase::caseInfo addr: %02x%02x%02x%02x%02x%02x",
        caseInfo.addr[5], caseInfo.addr[4], caseInfo.addr[3], /* mac下标 0 1 2 3 4 5 */
        caseInfo.addr[2], caseInfo.addr[1], caseInfo.addr[0]);
    if (btScansDevicesList_.size() >= BLUETOOTH_MAX_NUM) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "btScansDevicesList_.size is BLUETOOTH_MAX_NUM");
        return;
    } else {
        btScansDevicesList_.push_back(caseInfo);
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "SettingBluetoothModel::ScansDevicesListAddCase after size: %d", btScansDevicesList_.size());
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "SettingBluetoothModel::ScansDevicesListAddCase %s", caseInfo.deviceName);
}

void SettingBluetoothModel::SetScansCaseRssi(const bd_addr_t *bdAddr, int rssi)
{
    for (auto it = btScansDevicesList_.begin(); it != btScansDevicesList_.end(); it++) {
        if ((it->addr[0] == bdAddr->addr[0]) && (it->addr[1] == bdAddr->addr[1])
            && (it->addr[2] == bdAddr->addr[2]) && (it->addr[3] == bdAddr->addr[3])
            && (it->addr[4] == bdAddr->addr[4]) && (it->addr[5] == bdAddr->addr[5])) {
                it->rssi = rssi;
                WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "btScansDevicesList_ rssi:%d deviceName:%s", it->rssi, it->deviceName);
        }
    }
}


bool CompBuletoothInfo(const BtCaseInfo& a, const BtCaseInfo& b)
{
    if (a.rssi > b.rssi) {
        return true;
    }
    return false;
}

void SettingBluetoothModel::SortScansCaseRssi()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SettingBluetoothModel::SortScansCaseRssi");
    btScansDevicesList_.sort(CompBuletoothInfo);
}

void SettingBluetoothModel::ResetScanDevicesCase(const BtCaseInfo& caseInfo)
{
    for (auto it = btScansDevicesList_.begin(); it != btScansDevicesList_.end(); it++) {
        if ((it->addr[0] == caseInfo.addr[0]) && (it->addr[1] == caseInfo.addr[1])
            && (it->addr[2] == caseInfo.addr[2]) && (it->addr[3] == caseInfo.addr[3])
            && (it->addr[4] == caseInfo.addr[4]) && (it->addr[5] == caseInfo.addr[5])) {
                (void)memcpy_s(it->deviceName, BD_NAME_LEN,
                               caseInfo.deviceName, BD_NAME_LEN);
                WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "ResetConnedDevicesCase deviceName:%s", it->deviceName);
        }
    }
}

/* 已配对设备队列 */
void SettingBluetoothModel::SetPairedDevicesListClear()
{
    btPairedDevicesList_->Clear();
}

void SettingBluetoothModel::SetUpPairedDevicesList()
{
    if (btPairedDevicesList_ == nullptr) {
        btPairedDevicesList_ = new List<BtCaseInfo>();
        if (btPairedDevicesList_ == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingBluetoothModel btPairedDevicesList_ new fail");
            return;
        }
        SetPairedDevicesListClear();
        unsigned int connedDevicesNum = 0;
        bool ret = gap_get_paired_devices_num(&connedDevicesNum);
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "Setup GetPariedDevicesNum ret = %d, num = %d", ret, connedDevicesNum);

        gap_paired_device_info_t *devInfo = (gap_paired_device_info_t*)malloc(sizeof(gap_paired_device_info_t) *connedDevicesNum);
        if (devInfo ==  nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "DevInfo malloc failed %u", sizeof(gap_paired_device_info_t) *connedDevicesNum);
            return;
        }
        ret =  gap_get_paired_devices_list(devInfo, (int *)&connedDevicesNum);
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SetUpPairedDevicesList gap_get_paired_devices_list ret = %d", ret);

        BtCaseInfo caseInfo{};
        for (int i = 0; i < static_cast<int>(connedDevicesNum); ++i) {
            if ((gap_get_device_class((const bd_addr_t *)devInfo[i].addr.addr) & 0x200) != 0) {
                continue;
            }
            (void)memcpy_s(caseInfo.addr, BD_ADDR_LEN, devInfo[i].addr.addr, BD_ADDR_LEN);
            (void)memcpy_s(caseInfo.deviceName, BD_NAME_LEN, devInfo[i].device_name, BD_NAME_LEN);
            caseInfo.connect_status = "连接已经断开";
            AddPairedDevicesCase(caseInfo);
            btScansDevicesList_.push_back(caseInfo);
        }
        free(devInfo);
    }
}

void SettingBluetoothModel::AddPairedDevicesCase(BtCaseInfo caseInfo)
{
    if (btPairedDevicesList_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "btPairedDevicesList_ nullptr");
        return;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AddPairedDevicesCase::caseInfo addr: %02x%02x%02x%02x%02x%02x",
        caseInfo.addr[5], caseInfo.addr[4], caseInfo.addr[3], /* mac下标 0 1 2 3 4 5 */
        caseInfo.addr[2], caseInfo.addr[1], caseInfo.addr[0]);
    if (btPairedDevicesList_->Size() >= BLUETOOTH_MAX_NUM) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "btPairedDevicesList_->Size is BLUETOOTH_MAX_NUM");
        return;
    } else {
        btPairedDevicesList_->PushBack(caseInfo);
    }
}

List<BtCaseInfo>& SettingBluetoothModel::GetPairedDevicesCase()
{
    return *btPairedDevicesList_;
}

bool SettingBluetoothModel::GetAclConnected(void)
{
    return aclConnected_;
}

void SettingBluetoothModel::SetAclConnected(bool tag)
{
    aclConnected_ = tag;
}

void SettingBluetoothModel::UpdateAclConnectStatus(bool status)
{
    aclConnected_ = status;
    if (PullUpDownView::GetInstance() != nullptr) {
        PullUpDownView::GetInstance()->UpdateBlueToothStatus(status);
    }
}

void SettingBluetoothModel::SetSelectedDeviceInfo(BtCaseInfo *caseInfo)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SetSelectedDeviceInfo: %02x%02x%02x%02x%02x%02x",
                  caseInfo->addr[5], caseInfo->addr[4], caseInfo->addr[3], /* macÏÂ±ê 0 1 2 3 4 5 */
                  caseInfo->addr[2], caseInfo->addr[1], caseInfo->addr[0]); /* macÏÂ±ê 0 1 2 3 4 5 */
    (void)memcpy_s(selectedDeviceInfo_.addr, BD_ADDR_LEN,
                   caseInfo->addr, BD_ADDR_LEN);
    (void)memcpy_s(selectedDeviceInfo_.deviceName, BD_NAME_LEN,
                   caseInfo->deviceName, BD_NAME_LEN);
}

BtCaseInfo *SettingBluetoothModel::GetSelectedDeviceInfo(void)
{
    return &selectedDeviceInfo_;
}

void SettingBluetoothModel::SetLastConnedAddr(const bd_addr_t *addr)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SetLastConnedAddr: %02x%02x%02x%02x%02x%02x",
                  addr->addr[5], addr->addr[4], addr->addr[3], /* macÏÂ±ê 0 1 2 3 4 5 */
                  addr->addr[2], addr->addr[1], addr->addr[0]); /* macÏÂ±ê 0 1 2 3 4 5 */
    (void)memcpy_s(lastConnedAddr_.addr, BD_ADDR_LEN, addr->addr, BD_ADDR_LEN);
}

bd_addr_t *SettingBluetoothModel::GetLastConnedAddr(void)
{
    return &lastConnedAddr_;
}

bool SettingBluetoothModel::CheckExistPairedList(BtCaseInfo caseInfo)
{
    ListNode<BtCaseInfo> *node = btPairedDevicesList_ ->Begin();
    if (node == nullptr) {
        return false;
    } else if ((caseInfo.addr[0] == 0) && (caseInfo.addr[1] == 0)
                && (caseInfo.addr[2] == 0) && (caseInfo.addr[3] == 0)
                && (caseInfo.addr[4] == 0) && (caseInfo.addr[5] == 0)) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "CheckExistPairedList false addr: 000000000000");
        return true;
    }
    while (node != btPairedDevicesList_ ->End()) {
        if ((caseInfo.addr[0] == node->data_.addr[0]) && (caseInfo.addr[1] == node->data_.addr[1])
            && (caseInfo.addr[2] == node->data_.addr[2]) && (caseInfo.addr[3] == node->data_.addr[3])
            && (caseInfo.addr[4] == node->data_.addr[4]) && (caseInfo.addr[5] == node->data_.addr[5])) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "CheckExistPairedList true");
            return true;
        }
        node = node->next_;
    }
    return false;
}

void SettingBluetoothModel::AddNodeFromPairListToScanList(const bd_addr_t &bdAddr)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AddNodeFromPairListToScanList");
    // 检查扫描设备列表
    for (auto it = btScansDevicesList_.begin(); it != btScansDevicesList_.end(); ++it) {
        if (memcmp(it->addr, bdAddr.addr, sizeof(bdAddr.addr)) == 0) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AddNodeFromPairListToScanList btScansDevicesList_ true");
            return;
        }
    }

    // 检查已配对设备列表
    ListNode<BtCaseInfo> *node = btPairedDevicesList_->Begin();
    bool found = false;
    while (node != btPairedDevicesList_->End()) {
        if (memcmp(node->data_.addr, bdAddr.addr, sizeof(bdAddr.addr)) == 0) {
            found = true;
            break;
        }
        node = node->next_;
    }

    if (found) {
        btScansDevicesList_.push_back(node->data_);
    } else {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "btScansDevicesList_ error");
    }
}

bool SettingBluetoothModel::GetAdvStartState(void)
{
    return advStartState_;
}
void SettingBluetoothModel::SetAdvStartState(bool state)
{
    advStartState_ = state;
}

int SettingBluetoothModel::ConnectRemoteDevice(const bd_addr_t *addr)
{
    /* 恢厂后耳机连接要开启BT广播否则会导致AVRCP无法音量控制 */
    bool ret = gap_br_set_bt_scan_mode(GAP_SCAN_MODE_CONNECTABLE, 60);
    if (!ret) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "ConnectRemoteDevice gap_br_set_bt_scan_mode ret = 0x%x", ret);
    }
    errcode_t retVal = gap_connect_remote_device(addr);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "ConnectRemoteDevice gap_connect_remote_device ret = 0x%x", retVal);
    (void)memcpy_s(&remoteConnectAddr_, sizeof(bd_addr_t), addr, sizeof(bd_addr_t));
    return retVal;
}

bool SettingBluetoothModel::ConnectRemoteCheck(const bd_addr_t *addr)
{
    bool ret;
    if ((remoteConnectAddr_.addr[0] == addr->addr[0]) && (remoteConnectAddr_.addr[1] == addr->addr[1]) &&
        (remoteConnectAddr_.addr[2] == addr->addr[2]) && (remoteConnectAddr_.addr[3] == addr->addr[3]) &&
        (remoteConnectAddr_.addr[4] == addr->addr[4]) && (remoteConnectAddr_.addr[5] == addr->addr[5])) {
        ret = true;
        (void)memset_s(&remoteConnectAddr_, sizeof(bd_addr_t), 0, sizeof(bd_addr_t));
    } else {
        ret = false;
    }
    return ret;
}

#ifdef __cplusplus
extern "C" {
#endif

// 回调： gap_read_remote_rssi_value 接口的数据上报
void SettingBtGapReadRemoteRssiEventCallback(const bd_addr_t *bdAddr, int rssi, int status)
{
    UNUSED(status);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SettingBtGapReadRemoteRssiEventCallback addr: %02x%02x%02x%02x%02x%02x",
        bdAddr->addr[5], bdAddr->addr[4], bdAddr->addr[3], /* mac下标 0 1 2 3 4 5 */
        bdAddr->addr[2], bdAddr->addr[1], bdAddr->addr[0]); /* mac下标 0 1 2 3 4 5 */
    SettingBluetoothModel::GetInstance()->SetScansCaseRssi(bdAddr, rssi);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "bt SettingBtGapReadRemoteRssiEventCallback SetSortCaseRssi: %d status: %d", rssi, status);
}

// 回调：发现状态变化上报/扫描结束
void SettingBtGapDiscoveryStateChangedCallback(int status)
{
    UNUSED(status);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "bt SettingBtGapDiscoveryStateChangedCallback %d", status);
}

static void LoadScanResult()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "LoadScanResult");
    SettingBluetoothPage* bluetoothPage = SettingBluetoothPage::GetInstance();
    if (bluetoothPage == nullptr) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "bluetoothPage nullptr");
        return;
    }
    bluetoothPage->SetScanStatus(false);

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "LoadScanResult::LoadFindDeviceList");
    bluetoothPage->LoadFindDeviceList();
}

void SettingBluetoothModel::SettingBtGapDiscoveryResultCallbakHandle(const bd_addr_t &bdAddr)
{
    BtCaseInfo caseInfo{};
    (void)memcpy_s(caseInfo.addr, BD_ADDR_LEN, bdAddr.addr, BD_ADDR_LEN);

    unsigned char nameLen = BD_NAME_LEN;
    bool getNameRet = gap_get_device_name(&bdAddr, caseInfo.deviceName, &nameLen);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "gap_discovery_result_callback getNameRet(bool): %d nameLen: %d name:%s",
                  getNameRet, nameLen, caseInfo.deviceName);

    if (nameLen == 0) { // 过滤未扫描到名字的设备
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "nameLen == 0 return");
        return;
    }
    // 去重操作
    bool tag = SettingBluetoothModel::GetInstance()->CheckExistScanList(caseInfo);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "CheckExistScanList ret = %d", tag);
    if (!tag) {
        SettingBluetoothModel::GetInstance()->ScansDevicesListAddCase(caseInfo); // 增加扫描设备信息
        LoadScanResult();
        bool ret = gap_read_remote_rssi_value(&bdAddr); // 获取rssi
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "gap_read_remote_rssi_value ret = %d", ret);
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "bt SettingBtGapDiscoveryResultCallback end");
}

void SettingBtGapPairStatusChangedCallback(const bd_addr_t *bdAddr, int status)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SettingBtGapPairStatusChangedCallback addr: ****%02x%02x%02x%02x status: %d\n",
        bdAddr->addr[3], bdAddr->addr[2], bdAddr->addr[1], bdAddr->addr[0], status); /* addr下标 0 1 2 3 */
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "GAP_PAIR_PAIRED %d", GAP_PAIR_PAIRED);
    if (status == GAP_PAIR_PAIRED) {
        if (SettingBluetoothModel::GetInstance()->GetConnectFragmentStatus()) {
            SettingBluetoothModel::GetInstance()->SetConnectFragmentStatus(false);
            NativeAbility::GetInstance().SwitchPageInSlice(SettingPageId::MAIN_PAGE,
                TransitionType::TRANSITION_INVALID, false);
        }
    }
}

void SettingBtGapPairRequestedCallback(const bd_addr_t *bdAddr)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SettingBtGapPairRequestedCallback addr: ****%02x%02x%02x%02x\n",
        bdAddr->addr[3], bdAddr->addr[2], bdAddr->addr[1], bdAddr->addr[0]); /* addr下标 0 1 2 3 */
}

void SettingBtGapPairConfirmedCallback(const bd_addr_t *bdAddr, int req_type, int number)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SettingBtGapPairConfirmedCallback addr: ****%02x%02x%02x%02x req_type: %d number: %d\n",
        bdAddr->addr[3], bdAddr->addr[2], bdAddr->addr[1], bdAddr->addr[0], /* addr下标 0 1 2 3 */
        req_type, number);
}

// 回调：扫描结果处理/发现结果上报
void SettingBtGapDiscoveryResultCallback(const bd_addr_t *bdAddr)
{
    if ((gap_get_device_class(bdAddr) & DEVICE_MAJOR_CLASS_PHONE) != 0) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SettingBtGapDiscoveryResultCallback: Mobile Phone Device");
        return;
    }

    if (bdAddr == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "addr nullptr");
        return;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "gap_discovery_result_callback addr: %02x%02x%02x%02x%02x%02x",
        bdAddr->addr[5], bdAddr->addr[4], bdAddr->addr[3], /* mac下标 0 1 2 3 4 5 */
        bdAddr->addr[2], bdAddr->addr[1], bdAddr->addr[0]); /* mac下标 0 1 2 3 4 5 */
    GraphicService::GetInstance()->PostGraphicEvent(std::bind(&SettingBluetoothModel::SettingBtGapDiscoveryResultCallbakHandle,
                                                              SettingBluetoothModel::GetInstance(), *bdAddr));
}

static void ACLConnectionSuss(const bd_addr_t &bdAddr)
{
    // 成功提示
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "ACLConnectionSuss conn success");
    SettingBluetoothModel::GetInstance()->SetLastConnedAddr(&bdAddr);
    SettingBluetoothModel::GetInstance()->SetBlueToothConnectStatus(true);
    SettingBluetoothModel::GetInstance()->UpdateConnectedStatusForDevice(bdAddr);
    if (SettingBluetoothPage::GetInstance() != nullptr) {
        SettingBluetoothPage::GetInstance()->LoadFindDeviceList();
    }
    if ((SettingBluetoothModel::GetInstance()->GetConnectResultPage() == true) &&
        (SettingBluetoothPage::GetInstance() != nullptr)) {
        SettingBluetoothPage::GetInstance()->ShowResultGroup(CONNECT_BLUETOOTH_SUCCESS_FRAGMENT);
    } else {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "GetConnectResultPage is false or GetInstance is nullptr");
    }
    SettingBluetoothModel::GetInstance()->SetConnectResultPage(false);
}

static void ACLConnectionFail(const bd_addr_t &bdAddr)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "ACLConnectionFail conn fail");
    SettingBluetoothModel::GetInstance()->SetBlueToothConnectStatus(false);
    SettingBluetoothModel::GetInstance()->UpdateDisconnectStatusForDevice(bdAddr);
    if (SettingBluetoothPage::GetInstance() != nullptr) {
        SettingBluetoothPage::GetInstance()->LoadFindDeviceList();
    }
    if ((SettingBluetoothModel::GetInstance()->GetConnectResultPage()) == true &&
        (SettingBluetoothPage::GetInstance() != nullptr)) {
        SettingBluetoothPage::GetInstance()->ShowResultGroup(CONNECT_BLUETOOTH_FAIL_FRAGMENT);
    }
    // 失败提示页
    SettingBluetoothModel::GetInstance()->SetConnectResultPage(false);
}

void SettingBluetoothModel::SettingBtGapAclStateChangedHandle(gap_acl_state_t state, unsigned int reason, const bd_addr_t &bdAddr)
{
    if ((reason == 0x13) || (reason == 0x14) || (reason == 0x15) || (reason == 0x3D) || (reason == 0x08)) { // 连接断开
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "The peer end is disconnected.");
    } else if (reason == 0x16) { // 主动断开连接
        BtCaseInfo *onClickBtCaseInfo = SettingBluetoothModel::GetInstance()->GetSelectedDeviceInfo();
    } else if ((reason == 0xb) || (reason > 0x45)) { // 0xb:重复点击连接按钮 0x45:acl状态最大参数
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "reason is false.");
    }

    if ((state == GAP_ACL_STATE_CONNECTED) || (state == GAP_ACL_STATE_LE_CONNECTED)) {
        // 将数据存入设备列表
        BtCaseInfo *info = GetSelectedDeviceInfo();
        bool tag = CheckExistPairedList(*info);
        if ((!tag) && (memcmp(info->addr, bdAddr.addr, BD_ADDR_LEN) == 0)) {
            AddPairedDevicesCase(*info);
        }
        // 从配对的列表里面取出数据 ，将该数据填充进入到ScanList
        SettingBluetoothModel::GetInstance()->SetLastConnedAddr(&bdAddr);
        SettingBluetoothModel::GetInstance()->AddNodeFromPairListToScanList(bdAddr);
        if (SettingBluetoothModel::GetInstance()->ConnectRemoteCheck(&bdAddr)) {
            int32_t retA2dp = a2dp_src_connect(&bdAddr);
            if (retA2dp) {
                ACLConnectionFail(bdAddr);
            } else {
                SetLastConnedAddr(&bdAddr);
            }
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "a2dp_src_connect result 0x%x", retA2dp);
        }
    } else {
        ACLConnectionFail(bdAddr);
    }
}

void ChangeBleConnectToMax()
{
    if (msg_center_get_trans_channel() == DIAG_FRAME_FID_BT) {
        gap_conn_param_update_t conn_param = { 0 };
        conn_param.conn_handle = SETTING_BLUETOOTH_CONN_HANDLE;
        conn_param.interval_min = SETTING_BLUETOOTH_INTERVAL_TOMAX_MIN;
        conn_param.interval_max = SETTING_BLUETOOTH_INTERVAL_TOMAX_MAX;
        conn_param.slave_latency = SETTING_BLUETOOTH_LATENCY;
        conn_param.timeout_multiplier = SETTING_BLUETOOTH_TIMEOUT_MUTIPLIER;
        errcode_t ret = gap_ble_connect_param_update(&conn_param);
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "SettingBleConnectCallback change to max interval ret:%d : latency-min:%u latency-max:%u\n",
                        ret, conn_param.interval_min, conn_param.interval_max);
    }
}

void ChangeBleConnectToDefault()
{
    if (msg_center_get_trans_channel() == DIAG_FRAME_FID_BT) {
        gap_conn_param_update_t conn_param = { 0 };
        conn_param.conn_handle = SETTING_BLUETOOTH_CONN_HANDLE;
        conn_param.interval_min = SETTING_BLUETOOTH_INTERVAL_TODEFAULT_MIN;
        conn_param.interval_max = SETTING_BLUETOOTH_INTERVAL_TODEFAULT_MAX;
        conn_param.slave_latency = SETTING_BLUETOOTH_LATENCY;
        conn_param.timeout_multiplier = SETTING_BLUETOOTH_TIMEOUT_MUTIPLIER;
        errcode_t ret = gap_ble_connect_param_update(&conn_param);
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "SettingBleConnectCallback change to default interval ret:%d : latency-min:%u latency-max:%u\n",
                        ret, conn_param.interval_min, conn_param.interval_max);
    }
}

// 回调：ACL连接状态上报
void SettingBtGapAclStateChangedCallback(const bd_addr_t *bdAddr, gap_acl_state_t state, unsigned int reason)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "bt SettingGapAclStateChangedCallback state %d, reason %x succ or err", state, reason);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "addr: %02x%02x%02x%02x%02x%02x",
        bdAddr->addr[5], bdAddr->addr[4], bdAddr->addr[3], /* mac下标 0 1 2 3 4 5 */
        bdAddr->addr[2], bdAddr->addr[1], bdAddr->addr[0]); /* mac下标 0 1 2 3 4 5 */
    static bd_addr_t gapAclConnectAddr{0}; /* 连接后保存手机地址，手表清除手机配对后查询不到class比较手机地址 */
    if ((gap_get_device_class(bdAddr) & 0x200) != 0 || memcmp(gapAclConnectAddr.addr, bdAddr->addr, BD_ADDR_LEN) == 0) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SettingGapAclStateChangedCallback: Mobile Phone Device");
        if (state == GAP_ACL_STATE_DISCONNECTED) {
            /* 通知msg_center，ble状态仍用bt状态，此处不区分 */
            MsgCenterSendMsg(BT_DISCONNECTED_TO_MSGCENTER, 0, nullptr, 0);
            SendMsgToUIService(BT_DISCONNECTED_TO_UI, 0, nullptr, 0);
            // send to ace network
            SettingBluetoothModel::GetInstance()->SetAclConnected(false);
            net_connect_status_change(NETWORK_TYPE_BLUETOOTH, CONNECTION_STATUS_DISCONNECTED);
            // 断开蓝牙连接时，若存在查找手机页面，刷新页面为断开页面
            if (FindPhoneMainPage::GetInstance() != nullptr) {
                FindPhoneMainPage::GetInstance()->ChangeToDisConnect();
            }
#if defined(SUPPORT_LWIP) && defined(SUPPORT_BREDR)
            lwip_close_bt_pan();
#endif
        } else if (state == GAP_ACL_STATE_CONNECTED) {
            (void)memcpy_s(gapAclConnectAddr.addr, BD_ADDR_LEN, bdAddr->addr, BD_ADDR_LEN);
            MsgCenterSendMsg(BT_CONNECTED_TO_MSGCENTER, 0, nullptr, 0);
            SendMsgToUIService(BT_CONNECTED_TO_UI, 0, nullptr, 0);
            // send to ace network
            SettingBluetoothModel::GetInstance()->SetAclConnected(true);
#if defined(SUPPORT_LWIP) && defined(SUPPORT_BREDR)
            lwip_open_bt_pan();
#endif
            net_connect_status_change(NETWORK_TYPE_BLUETOOTH, CONNECTION_STATUS_CONNECTED);
        }
        return;
    }

    GraphicService::GetInstance()->PostGraphicEvent(
        std::bind(&SettingBluetoothModel::SettingBtGapAclStateChangedHandle, SettingBluetoothModel::GetInstance(), state, reason, *bdAddr));
}

// 回调：远端设备名称变化
void SettingBtRemoteNameChangedCallback(const bd_addr_t *bdAddr, const unsigned char *deviceName, unsigned char length)
{
    if ((gap_get_device_class(bdAddr) & 0x200) != 0) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SettingBtRemoteNameChangedCallback: Mobile Phone Device");
        return;
    }
    BtCaseInfo caseInfo{};
    if (bdAddr == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingBtRemoteNameChangedCallback addr nullptr");
        return;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SettingBtRemoteNameChangedCallback addr: %02x%02x%02x%02x%02x%02x",
                  bdAddr->addr[5], bdAddr->addr[4], bdAddr->addr[3], /* mac下标 0 1 2 3 4 5 */
                  bdAddr->addr[2], bdAddr->addr[1], bdAddr->addr[0]); /* mac下标 0 1 2 3 4 5 */

    (void)memcpy_s(caseInfo.addr, BD_ADDR_LEN, bdAddr->addr, BD_ADDR_LEN);
    int ret = memcpy_s(caseInfo.deviceName, BD_NAME_LEN, deviceName, length);
    if (ret != EOK) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingBtRemoteNameChangedCallback memcpy fail!");
        return;
    }
    SettingBluetoothModel::GetInstance()->ResetScanDevicesCase(caseInfo);
}

void SettingBtIsAcceptConnOnSafeModeCallback(const bd_addr_t *bdAddr, bool *res)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SettingBtIsAcceptConnOnSafeModeCallback addr: %02x%02x%02x%02x%02x%02x set *res true",
        bdAddr->addr[5], bdAddr->addr[4], bdAddr->addr[3], /* addr下标 3 4 5 */
        bdAddr->addr[2], bdAddr->addr[1], bdAddr->addr[0]); /* addr下标 0 1 2 */
    *res = true;
}

static void A2dpSettingSrcConnectstateChanged(bd_addr_t *bdAddr, int connState)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "a2dp_src_connectStateChangedCallback addr: %02x%02x%02x%02x%02x%02x connState: %x",
        bdAddr->addr[5], bdAddr->addr[4], bdAddr->addr[3], /* 5 4 3 idx */
        bdAddr->addr[2], bdAddr->addr[1], bdAddr->addr[0], connState); /* 2 1 0 idx */
    if ((gap_get_device_class(bdAddr) & 0x200) != 0) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "a2dp_src_connectstateChangedCb: Mobile Phone Device");
        return;
    }
    if (connState == PROFILE_STATE_CONNECTED) {
        uint32_t ret = avrcp_ct_connect(bdAddr);
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "avrcp_ct_connect ret = 0x%x", ret);
        PhoneAudioManagerSetDeviceConnState(OUT_BLUETOOTH_A2DP, AUDIO_DEVICE_CONNECTED);
        GraphicService::GetInstance()->PostGraphicEvent(std::bind(ACLConnectionSuss, *bdAddr));
    }

    if (connState == PROFILE_STATE_DISCONNECTED) {
        PhoneAudioManagerSetDeviceConnState(OUT_BLUETOOTH_A2DP, AUDIO_DEVICE_DISCONNECTED);
        GraphicService::GetInstance()->PostGraphicEvent(std::bind(ACLConnectionFail, *bdAddr));
        /* 实验室测试耳机回连场景，用户根据需要调整策略 */
        bool ret = gap_br_set_bt_scan_mode(GAP_SCAN_MODE_CONNECTABLE_GENERAL_DISCOVERABLE, 60); // start scan
        if (!ret) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "a2dp disc: gap_br_set_bt_scan_mode ret = 0x%x", ret);
        }
    }
}

static void A2dpSettingSrcPlayingStateChanged(bd_addr_t *bdAddr, int playState)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "A2dpSettingSrcPlayingStateChangedCallback addr: %02x%02x%02x%02x%02x%02x playState: %x",
        bdAddr->addr[5], bdAddr->addr[4], bdAddr->addr[3], /* 5 4 3 idx */
        bdAddr->addr[2], bdAddr->addr[1], bdAddr->addr[0], playState); /* 2 1 0 idx */
}

static void A2dpSettingSrcConfigureChanged(bd_addr_t *bdAddr, a2dp_codec_info_t *info)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "A2dpSettingSrcConfigurationChangedCallback addr: %02x%02x%02x%02x%02x%02x ",
        bdAddr->addr[5], bdAddr->addr[4], bdAddr->addr[3], /* 5 4 3 idx */
        bdAddr->addr[2], bdAddr->addr[1], bdAddr->addr[0]); /* 2 1 0 idx */
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "codec_priority: %04x codec_type: %04x sample_rate: %02x bits_per_sample: %02x channel_mode: %02x ",
        info->codec_priority, info->codec_type, info->sample_rate, info->bits_per_sample, info->channel_mode);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "codec_specific1: %04x codec_specific2: %04x codec_specific3: %04x codec_specific4: %04x",
        info->codec_specific1, info->codec_specific2, info->codec_specific3, info->codec_specific4);
}

static void SettingBleConnStateChangeCallback(unsigned short connId, bd_addr_t *addr, gap_ble_conn_state_t connState,
                                       gap_ble_pair_state_t pairState, gap_ble_disc_reason_t discReason)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "SettingBleConnStateChangeCallback connId 0x%x, connState 0x%x, pairState 0x%x",
                  connId, connState, pairState);
    if (GAP_BLE_STATE_DISCONNECTED == connState) { // restart ble adv when disconnected
        /* 通知msg_center，ble状态仍用bt状态，此处不区分 */
        MsgCenterSendMsg(BT_DISCONNECTED_TO_MSGCENTER, 0, nullptr, 0);
        msg_center_set_ble_conn_state(GAP_BLE_STATE_DISCONNECTED);
        // send to ace network
        net_connect_status_change(NETWORK_TYPE_BLUETOOTH_LE, CONNECTION_STATUS_DISCONNECTED);
        // 断开蓝牙连接时，若存在查找手机页面，刷新页面为断开页面
        if (FindPhoneMainPage::GetInstance() != nullptr) {
            FindPhoneMainPage::GetInstance()->ChangeToDisConnect();
        }
    } else {
        MsgCenterSendMsg(BT_CONNECTED_TO_MSGCENTER, 0, nullptr, 0);
        msg_center_set_ble_conn_state(GAP_BLE_STATE_CONNECTED);
        (void)memcpy_s(&g_bd_addr, sizeof(bd_addr_t), addr, sizeof(bd_addr_t));
        // send to ace network
        net_connect_status_change(NETWORK_TYPE_BLUETOOTH_LE, CONNECTION_STATUS_CONNECTED);
    }
    // send to ace network
}

static void SettingBleAdvStartStateCallback(uint8_t advId, adv_status_t status)
{
    if (g_sample_adv_id == advId) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "SettingBleAdvStartStateCallback advId: %d, status:%d\n", advId, status);
        SettingBluetoothModel::GetInstance()->SetAdvStartState(true);
    }
}

static void SettingBleAdvStopCallback(uint8_t advId, adv_status_t status)
{
    if (g_sample_adv_id == advId) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "SettingBleAdvStopCallback advId: %d, status:%d\n", advId, status);
        SettingBluetoothModel::GetInstance()->SetAdvStartState(false);
    }
}

static void SettingBleAdvTerminateCallback(uint8_t advId, adv_status_t status)
{
    if (g_sample_adv_id == advId) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "SettingBleAdvTerminateCallback advId: %d, status:%d\n", advId, status);
        SettingBluetoothModel::GetInstance()->SetAdvStartState(false);
        // 此处开广播是为了实验室测试需要，客户需要自己修改
        BleSettingAdvStart();
    }
}

static void SettingBtGattsServiceAddCallback(uint8_t serverId, bt_uuid_t *uuid, uint16_t srvcHandle, errcode_t status)
{
    if ((uuid->uuid_len == sizeof(uint16_t)) && (*((uint16_t*)uuid->uuid) == 0xF01F)) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "SettingBtGattsServiceAddCallback 0x%x 0x%x 0x%x",
            status, serverId, srvcHandle);
        uint16_t sample_id = 0xF11F;
        uint16_t diag_id = 0xF21F;
        gatts_add_chara_info_t character;
        uint16_t value = 0;

        memset_s(&character, sizeof(gatts_add_chara_info_t), 0, sizeof(gatts_add_chara_info_t));
        character.chara_uuid.uuid_len = sizeof(sample_id);
        memcpy_s(character.chara_uuid.uuid, sizeof(character.chara_uuid.uuid), (char *)&sample_id, sizeof(sample_id));
        character.permissions = GATT_ATTRIBUTE_PERMISSION_WRITE;
        character.properties = GATT_CHARACTER_PROPERTY_BIT_WRITE_NO_RSP;
        character.value_len = (uint16_t)sizeof(value);
        character.value = (uint8_t *)(&value);
        int ret = gatts_add_characteristic(serverId, srvcHandle, &character);
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "gatts_add_characteristic sample ret 0x%x", ret);

        memset_s(&character, sizeof(gatts_add_chara_info_t), 0, sizeof(gatts_add_chara_info_t));
        character.chara_uuid.uuid_len = sizeof(diag_id);
        memcpy_s(character.chara_uuid.uuid, sizeof(character.chara_uuid.uuid), (char *)&diag_id, sizeof(diag_id));
        character.permissions = GATT_ATTRIBUTE_PERMISSION_WRITE | GATT_ATTRIBUTE_PERMISSION_READ;
        character.properties = GATT_CHARACTER_PROPERTY_BIT_WRITE_NO_RSP | GATT_CHARACTER_PROPERTY_BIT_READ | GATT_CHARACTER_PROPERTY_BIT_NOTIFY;
        character.value_len = (uint16_t)sizeof(value);
        character.value = (uint8_t *)(&value);

        ret = gatts_add_characteristic(serverId, srvcHandle, &character);
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "gatts_add_characteristic diag ret 0x%x", ret);

        g_sample_svr_handle = srvcHandle;
    }
}

static void SettingBtGattsCharacteristicAddCallback(uint8_t serverId, bt_uuid_t *uuid, uint16_t srvcHandle, gatts_add_character_result_t *result, errcode_t status)
{
    if ((uuid->uuid_len == sizeof(uint16_t)) && (*((uint16_t*)uuid->uuid) == 0xF11F)) {
        g_sample_characteristic_handle = result->value_handle;
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "SettingBtGattsCharacteristicAddCallback 0x%x 0x%x 0x%x 0x%x",
                      status, serverId, srvcHandle, g_sample_characteristic_handle);
    }
    if ((uuid->uuid_len == sizeof(uint16_t)) && (*((uint16_t*)uuid->uuid) == 0xF21F)) {
        g_diag_characteristic_handle = result->value_handle;
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "SettingBtGattsCharacteristicAddCallback 0x%x 0x%x 0x%x 0x%x",
                      status, serverId, srvcHandle, g_diag_characteristic_handle);

        uint8_t ccc_val[] = { 0x01, 0x00 };
        uint16_t diag_desc = 0x0229;
        gatts_add_desc_info_t descriptor;
        memset_s(&descriptor, sizeof(gatts_add_desc_info_t), 0, sizeof(gatts_add_desc_info_t));
        descriptor.desc_uuid.uuid_len = sizeof(diag_desc);
        memcpy_s(descriptor.desc_uuid.uuid, sizeof(descriptor.desc_uuid.uuid), (char *)&diag_desc, sizeof(diag_desc));
        descriptor.permissions = GATT_ATTRIBUTE_PERMISSION_READ | GATT_ATTRIBUTE_PERMISSION_WRITE;
        descriptor.value_len = (uint16_t)sizeof(ccc_val);
        descriptor.value = ccc_val;
        int ret = gatts_add_descriptor(serverId, srvcHandle, &descriptor);
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "gatts_add_descriptor diag_desc ret 0x%x", ret);
    }

    if (g_sample_characteristic_handle != 0 && g_diag_characteristic_handle !=0 && g_diag_desc_handle != 0) {
        int ret = gatts_start_service(serverId, srvcHandle);
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "gatts_start_service ret 0x%x", ret);
    }
}

static void SettingBtGattsDescriptorAddCallback(uint8_t serverId, bt_uuid_t *uuid, uint16_t srvcHandle, uint16_t descriptorHandle, errcode_t status)
{
    if (g_sample_svr_handle == srvcHandle) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "SettingBtGattsDescriptorAddCallback 0x%x 0x%x 0x%x 0x%x",
            status, serverId, srvcHandle, descriptorHandle);
        g_diag_desc_handle = descriptorHandle;
    }

    if (g_sample_characteristic_handle != 0 && g_diag_characteristic_handle !=0 && g_diag_desc_handle != 0) {
        int ret = gatts_start_service(serverId, srvcHandle);
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "gatts_start_service ret 0x%x", ret);
    }
}

static void SettingBtGattsServiceStartCallback(uint8_t serverId, uint16_t srvcHandle, errcode_t status)
{
    if (g_sample_svr_handle == srvcHandle) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "SettingBtGattsServiceStartCallback 0x%x 0x%x 0x%x", status, serverId, srvcHandle);
    }
}

static void SettingBtGattsServiceStopCallback(uint8_t serverId, uint16_t srvcHandle, errcode_t status)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "SettingBtGattsServiceStopCallback 0x%x 0x%x 0x%x", status, serverId, srvcHandle);
}

static void SettingBtGattsServiceDeleteCallback(uint8_t serverId, errcode_t status)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "SettingBtGattsServiceDeleteCallback 0x%x 0x%x", status, serverId);
}

static void SettingBtGattsRequestReadCallback(uint8_t serverId, uint16_t connId, gatts_req_read_cb_t *readCbPara, errcode_t status)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SettingBtGattsRequestReadCallback handle = %u", readCbPara->handle);
}

static void SettingBtGattsRequestWriteCallback(uint8_t serverId, uint16_t connId, gatts_req_write_cb_t *writeCbPara,  errcode_t status)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SettingBtGattsRequestWriteCallback handle = %u", writeCbPara->handle);
    if (g_sample_characteristic_handle == writeCbPara->handle) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SettingBtGattsRequestWriteCallback handle = %u", writeCbPara->handle);
        if (writeCbPara->length == 9) {
            if (memcmp("open ctkd", writeCbPara->value, writeCbPara->length) == 0) {
                WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "enable ctkd");
                // 一键双连使能（依赖安全连接参数配置）
                errcode_t ret = gap_ble_set_ctkd_enable(true);
                WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "gap_ble_set_ctkd_enable ret = %u", ret);

                // 发起ble配对
                ret = gap_ble_pair_remote_device(&g_bd_addr);
                WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "gap_ble_pair_remote_device ret = %u ", ret);
            }
        } else if (writeCbPara->length == 10) {
            if (memcmp("close ctkd", writeCbPara->value, writeCbPara->length) == 0) {
                // 一键双连去使能
                errcode_t ret = gap_ble_set_ctkd_enable(false);
                WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "gap_ble_set_ctkd_disable ret = %u", ret);
            }
        }
    } else if (g_diag_characteristic_handle == writeCbPara->handle) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "diag_gatt_write_req_cbk handle = %u", writeCbPara->handle);
        diag_gatt_write_req_cbk(serverId, connId, writeCbPara, status);
    } else if (g_diag_desc_handle == writeCbPara->handle) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "diag_gatt_set_handle handle = %u", writeCbPara->handle);
        if ((writeCbPara->length == 2) && (*(char*)(writeCbPara->value) == 1)) {
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "open gatt channel");
            diag_gatt_set_handle(serverId, connId, g_diag_characteristic_handle);
            msg_center_set_trans_channel(DIAG_FRAME_FID_BT);
        } else if ((writeCbPara->length == 2) && (*(char*)(writeCbPara->value) == 0)) {
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "close gatt channel");
            diag_gatt_set_handle(0, 0, 0);
            msg_center_set_trans_channel(DIAG_FRAME_FID_PHONE);
        }
    }
}

static void SettingBtGattsMtuChangedCallback(uint8_t serverId, uint16_t connId, uint16_t mtuSize, errcode_t status)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "SettingBtGattsMtuChangedCallback connId = %u mtuSize = %u", connId, mtuSize);
}

static void SettingBtA2dpSnkAcceptTimerStop(void)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "stop timer\n");
    if (g_setting_a2dp_stream_accept_timer.timer != nullptr) {
        (void)osal_timer_stop(&g_setting_a2dp_stream_accept_timer);
        (void)osal_timer_destroy(&g_setting_a2dp_stream_accept_timer);
        g_setting_a2dp_stream_accept_timer.timer = nullptr;
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "stop timer success!\n");
}

static void SettingBtA2dpSnStartAcceptHandler(unsigned long data)
{
    UNUSED(data);
    bd_addr_t active_bd_addr = a2dp_snk_get_active_device();
    a2dp_snk_start_accept(&active_bd_addr);
    SettingBtA2dpSnkAcceptTimerStop();
}

static errcode_t SettingBtA2dpSnkStartAcceptTimer(void)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "start timer\n");
    if (g_setting_a2dp_stream_accept_timer.timer != nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "timer is running");
        return ERRCODE_SUCC;
    }

    g_setting_a2dp_stream_accept_timer.handler = SettingBtA2dpSnStartAcceptHandler;
    g_setting_a2dp_stream_accept_timer.interval = A2DP_STREAM_ACCEPT_TIMER_PERIOD;

    int32_t timer_ret = osal_timer_init(&g_setting_a2dp_stream_accept_timer);
    if (timer_ret != EOK) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "create timer error! ret: %x\n", timer_ret);
        SettingBtA2dpSnkAcceptTimerStop();
        return ERRCODE_FAIL;
    }
    timer_ret = osal_timer_start(&g_setting_a2dp_stream_accept_timer);
    if (timer_ret != EOK) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "start timer error! ret: %x\n", timer_ret);
        SettingBtA2dpSnkAcceptTimerStop();
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

static int32_t SettingBtA2dpSnkAudioCreate()
{
#if MEDIA_A2DP_SNK_INTERFACE
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "audioFormat %d channelCount %d sampleRate %d \n",
        g_setting_audio_format_config.audioFormat, g_setting_audio_format_config.channelCount, g_setting_audio_format_config.sampleRate);

    int32_t ret = AudioManagerA2dpSinkSetParam(g_setting_audio_format_config);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "AudioManagerA2dpSinkSetParam ret %x\n", ret);
    ret = AudioManagerA2dpSinkCreate();
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "AudioManagerA2dpSinkCreate ret %x\n", ret);
#endif
    /* 起定时器接受音乐开始请求 */
    (void)SettingBtA2dpSnkStartAcceptTimer();
    return 0;
}

static int32_t SettingBtA2dpSnkAudioDestroy()
{
#if MEDIA_A2DP_SNK_INTERFACE
    int32_t ret = AudioManagerA2dpSinkDestroy();
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "AudioManagerA2dpSinkDestroy ret %x\n", ret);
#endif
    return 0;
}

static void SettingBtA2dpSnkConnectStateChangedCb(bd_addr_t *bd_addr, int conn_state)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "addr: ****%02x%02x%02x%02x conn_state: %x\n",
        bd_addr->addr[3], bd_addr->addr[2], bd_addr->addr[1], bd_addr->addr[0], conn_state); /* 3 2 1 0 idx */
}

static void SettingBtA2dpSnkStartStreamReqCb(bd_addr_t *bd_addr)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "addr: ****%02x%02x%02x%02x \n",
        bd_addr->addr[3], bd_addr->addr[2], bd_addr->addr[1], bd_addr->addr[0]); /* 3 2 1 0 idx */
    SettingBtA2dpSnkAudioCreate();
}

static void SettingBtA2dpSnkPlayingStateChangedCb(bd_addr_t *bd_addr, int playing_state)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "addr: ****%02x%02x%02x%02x playing_state: %x\n",
        bd_addr->addr[3], bd_addr->addr[2], bd_addr->addr[1], bd_addr->addr[0], playing_state); /* 3 2 1 0 idx */
    if (playing_state == A2DP_NOT_PLAYING) {
        SettingBtA2dpSnkAudioDestroy();
    }
}

static void SettingBtA2dpSnkConfigChangedCb(bd_addr_t *bd_addr, a2dp_snk_codec_info_t *info)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "addr: ****%02x%02x%02x%02x ",
        bd_addr->addr[3], bd_addr->addr[2], bd_addr->addr[1], bd_addr->addr[0]); /* 3 2 1 0 idx */
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP,
        "codec_priority: %04x codec_type: %04x sample_rate: %02x bits_per_sample: %02x channel_mode: %02x",
        info->codec_priority, info->codec_type, info->sample_rate, info->bits_per_sample, info->channel_mode);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP,
        "codec_specific1: %04x codec_specific2: %04x codec_specific3: %04x codec_specific4: %04x\n",
        (unsigned int)info->codec_specific1,
        (unsigned int)info->codec_specific2,
        (unsigned int)info->codec_specific3,
        (unsigned int)info->codec_specific4);
#if MEDIA_A2DP_SNK_INTERFACE
    if (info->codec_type == A2DP_SNK_CODEC_TYPE_AAC) {
        g_setting_audio_format_config.audioFormat = AAC_LC;
    } else {
        g_setting_audio_format_config.audioFormat = SBC;
    }
    /* channel count 1 or 2 */
    g_setting_audio_format_config.channelCount = (info->channel_mode == A2DP_SNK_CODEC_CHANNEL_MODE_NONE ? 1 : 2);

    switch (info->sample_rate) {
        case A2DP_SNK_CODEC_SAMPLE_RATE_48000:
            g_setting_audio_format_config.sampleRate = BT_AUDIO_A2DP_SMAPLE_RATE48000;
            break;
        case A2DP_SNK_CODEC_SAMPLE_RATE_88200:
            g_setting_audio_format_config.sampleRate = BT_AUDIO_A2DP_SMAPLE_RATE88200;
            break;
        case A2DP_SNK_CODEC_SAMPLE_RATE_96000:
            g_setting_audio_format_config.sampleRate = BT_AUDIO_A2DP_SMAPLE_RATE96000;
            break;
        default:
            g_setting_audio_format_config.sampleRate = BT_AUDIO_A2DP_SMAPLE_RATE44100;
            break;
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "audioFormat %d channelCount %d sampleRate %d \n",
        g_setting_audio_format_config.audioFormat, g_setting_audio_format_config.channelCount, g_setting_audio_format_config.sampleRate);
#endif
}

// 注册回调函数
static gap_call_backs_t g_settingBtGapCallBacks{};
static a2dp_src_callbacks_t g_settingA2dprcCallbacks{};
static gap_ble_callbacks_t g_settingGapBleCallbacks{};
static gatts_callbacks_t g_settingBtGattsCallbacks{};
static a2dp_snk_callbacks_t g_settingA2dpSnkCallbacks = {};


static void SettingBleConnectCallback(uint16_t conn_id, errcode_t status, const gap_ble_conn_param_update_t *param)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "SettingBleConnectCallback conn_id:%u, interval-1:%u latency:%u timeout:%u\n",conn_id, param->interval, param->latency, param->timeout);
}

void SettingBtInitCallBacks()
{
    g_settingBtGapCallBacks.discovery_result_callback = SettingBtGapDiscoveryResultCallback;
    g_settingBtGapCallBacks.discovery_state_changed_callback = SettingBtGapDiscoveryStateChangedCallback;
    g_settingBtGapCallBacks.acl_state_changed_callbak = SettingBtGapAclStateChangedCallback;
    g_settingBtGapCallBacks.read_remote_rssi_event_callback = SettingBtGapReadRemoteRssiEventCallback;
    g_settingBtGapCallBacks.remote_name_changed_callback = SettingBtRemoteNameChangedCallback;
    g_settingBtGapCallBacks.is_accept_conn_on_safe_mode_callback = SettingBtIsAcceptConnOnSafeModeCallback;
    g_settingBtGapCallBacks.pair_status_changed_callback = SettingBtGapPairStatusChangedCallback;
    g_settingBtGapCallBacks.pair_requested_callback = SettingBtGapPairRequestedCallback;
    g_settingBtGapCallBacks.pair_confiremed_callback = SettingBtGapPairConfirmedCallback;
    gap_register_callbacks(&g_settingBtGapCallBacks);

    g_settingA2dprcCallbacks.connectstate_changed_cb = A2dpSettingSrcConnectstateChanged;
    g_settingA2dprcCallbacks.configuration_changed_cb = A2dpSettingSrcConfigureChanged;
    g_settingA2dprcCallbacks.playing_state_changed_cb = A2dpSettingSrcPlayingStateChanged;
    a2dp_src_register_callbacks(&g_settingA2dprcCallbacks);

    /* a2dp sink */
    g_settingA2dpSnkCallbacks.connectstate_changed_cb = SettingBtA2dpSnkConnectStateChangedCb;
    g_settingA2dpSnkCallbacks.start_stream_request_cb = SettingBtA2dpSnkStartStreamReqCb;
    g_settingA2dpSnkCallbacks.playing_state_changed_cb = SettingBtA2dpSnkPlayingStateChangedCb;
    g_settingA2dpSnkCallbacks.configuration_changed_cb = SettingBtA2dpSnkConfigChangedCb;
    a2dp_snk_register_callbacks(&g_settingA2dpSnkCallbacks);
}

void SettingBtInitCallBacksEx()
{
    g_settingGapBleCallbacks.conn_state_change_cb = SettingBleConnStateChangeCallback;
    g_settingGapBleCallbacks.start_adv_cb = SettingBleAdvStartStateCallback;
    g_settingGapBleCallbacks.stop_adv_cb = SettingBleAdvStopCallback;
    g_settingGapBleCallbacks.terminate_adv_cb = SettingBleAdvTerminateCallback;
    g_settingGapBleCallbacks.conn_param_update_cb = SettingBleConnectCallback;
    wrapper_gap_ble_register_callbacks(&g_settingGapBleCallbacks);

    g_settingBtGattsCallbacks.add_service_cb = SettingBtGattsServiceAddCallback;
    g_settingBtGattsCallbacks.add_characteristic_cb = SettingBtGattsCharacteristicAddCallback;
    g_settingBtGattsCallbacks.add_descriptor_cb = SettingBtGattsDescriptorAddCallback;
    g_settingBtGattsCallbacks.start_service_cb = SettingBtGattsServiceStartCallback;
    g_settingBtGattsCallbacks.stop_service_cb = SettingBtGattsServiceStopCallback;
    g_settingBtGattsCallbacks.delete_service_cb = SettingBtGattsServiceDeleteCallback;
    g_settingBtGattsCallbacks.read_request_cb = SettingBtGattsRequestReadCallback;
    g_settingBtGattsCallbacks.write_request_cb = SettingBtGattsRequestWriteCallback;
    g_settingBtGattsCallbacks.mtu_changed_cb = SettingBtGattsMtuChangedCallback;
    wrapper_gatts_register_callbacks(&g_settingBtGattsCallbacks);
}

void SettingBtInitGatts(void)
{
    uint16_t id = 0xF01F;
    uint16_t ser_id = 0xFF1F;
    uint8_t serverId = 0;
    bt_uuid_t uuid;
    int ret;

    uuid.uuid_len = sizeof(ser_id);
    memcpy_s(uuid.uuid, sizeof(uuid.uuid), (char *)&ser_id, sizeof(ser_id));
    ret = wrapper_gatts_register_server(&uuid, &serverId); /* 注册gatt server */
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "gatts_register_server ret: 0x%x serverId = 0x%x", ret, serverId);

    uuid.uuid_len = sizeof(id);
    memcpy_s(uuid.uuid, sizeof(uuid.uuid), (char *)&id, sizeof(id));
    ret = wrapper_gatts_add_service(serverId, &uuid, true);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "gatts_add_service ret: 0x%x", ret);

    wrapper_gatts_service_start();
}

static uint8_t Char2Num(int8_t c)
{
    if (c >= '0' && c <= '9') {
        return (uint8_t)(c - '0');
    } else if (c >= 'A' && c <= 'F') {
        return (uint8_t)(c - 'A' + TEN);
    } else if (c >= 'a' && c <= 'f') {
        return (uint8_t)(c - 'a' + TEN);
    } else {
        return 0xFF;
    }
}

static uint32_t GenerateUuidFromString(const char *str, uint8_t len, bt_uuid_t *uuid_out)
{
    int slen = (int)strnlen(str, len);
    bool new_uuid = false;
    if (uuid_out == nullptr) {
        return ERRCODE_INVALID_PARAM;
    }

    uuid_out->uuid_len = (uint8_t)slen / 2; /* 2: 2个字符组成一个字节，因此只取偶数个字符 */
    for (int i = 0; i + 1 < slen; i += 2) { /* 2: 同上 */
        uint8_t num1 = Char2Num(str[i]);
        uint8_t num2 = Char2Num(str[i + 1]);
        if (num1 == UINT8_MAX || num2 == UINT8_MAX) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "invalid input %c%c\n", str[i], str[i + 1]);
            return ERRCODE_INVALID_PARAM;
        }

        uuid_out->uuid[i / 2] = (uint8_t)((num1 << BIT4) | num2); /* 2: 同上 */
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "%02x ", (uint8_t)(uuid_out->uuid[i / 2])); /* 2: 同上 */
    }

    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "\n");
    return ERRCODE_SUCC;
}

void SettingBtSppServerCreate(const char *spp_uuid, const char *name, uint8_t name_len)
{
    spp_create_socket_para_t param;
    if (GenerateUuidFromString((const char *)spp_uuid, (uint8_t)strlen(spp_uuid), &param.uuid) != ERRCODE_SUCC) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "get uuid fail\n");
        return;
    }

    param.socket_type = SPP_SOCKET_RFCOMM;
    param.is_encrypt = true;

    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "uuid: %s uuid_len: %u\n", spp_uuid, param.uuid.uuid_len);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "socket_type: %x is_encrypt: %d name: %s nameLen: %u\n", param.socket_type,
        param.is_encrypt, name, name_len);

    int ret = spp_server_create(&param, name, name_len);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "spp_server_create ret: %x\n", ret);
}

static int BtSppServerAcceptTaskBody(void *data)
{
    int server_id = (int)(intptr_t)data;
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "server_id: %x\n", server_id);

    int ret = spp_server_accept(server_id);

    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "spp_server_accept ret: %x\n", ret);

    return OSAL_SUCCESS;
}

/* 等待client连接 */
void SettingBtSppServerAccept(uint8_t server_id)
{
    osal_task *task = osal_kthread_create(BtSppServerAcceptTaskBody, (void *)(uintptr_t)server_id,
        "spp_server_accept_task", BT_SPP_TASK_STACK_SIZE);
    if (task == nullptr) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "create task fail\n");
        return;
    }

    int ret = osal_kthread_set_priority(task, BT_SPP_TASK_PRI);
    if (ret != OSAL_SUCCESS) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "set pri error: %08x\n", ret);
    }

    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "create task success.\n");
}

void SettingBtInitSpp(void)
{
    char uuid[] = "1101";
    char name[] = "COM1";
    uint8_t name_len = strlen(name) + 1;
    uint8_t server_id = 0;

    /* 创建spp */
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "create spp server uuid=%s name=%s name_len=%d\n", uuid, name, name_len);
    SettingBtSppServerCreate(uuid, name, name_len);
    SettingBtSppServerAccept(server_id);
}

void BleSettingAdvStart(void)
{
    int advId = g_sample_adv_id;
    if (SettingBluetoothModel::GetInstance()->GetAdvStartState()) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "BleSettingAdvStart already started");
        gap_ble_stop_adv(advId);
        osDelay(50);
    }
    gap_ble_adv_params_t adv_para;
    memset_s(&adv_para, sizeof(gap_ble_adv_params_t), 0, sizeof(gap_ble_adv_params_t));
    adv_para.min_interval = 0x30;
    adv_para.max_interval = 0x60;
    adv_para.adv_type = GAP_BLE_ADV_CONN_SCAN_UNDIR;
    adv_para.channel_map = 0x7;
    adv_para.duration = 0;
    gap_ble_set_adv_param(advId, &adv_para);
    gap_ble_set_local_appearance(GAP_BLE_APPEARANCE_TYPE_GENERIC_WATCH);
    errcode_t ret = gap_ble_start_adv(advId);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "BleSettingAdvStart bt gap_ble_start_adv  ret = 0x%x", ret);
}

#ifdef __cplusplus
}
#endif
}

/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingBuletoothModel
 * Create: 2025-06-01
 */
#ifndef SETTING_BLUETOOTH_MODEL_H
#define SETTING_BLUETOOTH_MODEL_H

#include <string>
#include <cstring>
#include <list>
#include <cstdio>
#include "gfx_utils/list.h"
#include "bts_def.h"
#include "bts_br_gap.h"
#include "settings/common/SettingUnionItemGroup.h"
#include "settings/common/SettingRefSingleton.h"

namespace OHOS {
typedef struct {
    unsigned char deviceName[BD_NAME_LEN]; /* Name of the device record, must be in UTF-8 */
    unsigned char addr[BD_ADDR_LEN];
    std::string connect_status;
    int16_t rssi; // 蓝牙信号强弱
} BtCaseInfo;

typedef enum {
    BT_MAIN_PAGE = 0,
    RECONNECT_FRAGMENT,
    RECONNECT_SECOND_CONFIRM_FRAGMENT,
    UNPAIR_CONFIRM_FRAGMENT,
    CONNECT_NEW_PHONE_FRAGMENT,
    CONNECT_OPEN_FRAGMENT,
    CONNECT_BLUETOOTH_SUCCESS_FRAGMENT,
    CONNECT_BLUETOOTH_FAIL_FRAGMENT,
} BtGroupId;

class SettingBluetoothModel {
public:
    ~SettingBluetoothModel(){};
    static SettingBluetoothModel *GetInstance(void);
    void Init();
    std::list<BtCaseInfo>& GetScansDevicesList(); // 获取扫描到的设备列表
    void ScansDevicesListAddCase(BtCaseInfo caseInfo); // 设备列表新增数据
    void SetScansCaseRssi(const bd_addr_t *bdAddr, int rssi); // 设置设备的信号值
    void SortScansCaseRssi(); // 根据信号值对设备列表进行排序
    void ClearScansDevicesList();
    bool CheckExistScanList(BtCaseInfo caseInfo); // 去重检查
    void ResetScanDevicesCase(const BtCaseInfo& caseInfo); // 刷新名称变化的设备信息
    // 已配对蓝牙设备
    void SetPairedDevicesListClear();
    void AddPairedDevicesCase(BtCaseInfo caseInfo);
    bool CheckExistPairedList(BtCaseInfo caseInfo);
    List<BtCaseInfo>& GetPairedDevicesCase();
    // 被选中的设备信息
    void SetSelectedDeviceInfo(BtCaseInfo *caseInfo);
    BtCaseInfo *GetSelectedDeviceInfo(void);
    // 最后连接的设备地址
    void SetLastConnedAddr(const bd_addr_t *addr);
    bd_addr_t *GetLastConnedAddr(void);
    void SetAclConnected(bool tag);
    bool GetAclConnected(void);
    void UpdateAclConnectStatus(bool status);
    void SettingBtGapDiscoveryResultCallbakHandle(const bd_addr_t &bdAddr);
    void SettingBtGapAclStateChangedHandle(gap_acl_state_t state, unsigned int reason, const bd_addr_t &bdAddr);
    bool GetAdvStartState(void);
    void SetAdvStartState(bool state);
    int ConnectRemoteDevice(const bd_addr_t *addr);
    bool ConnectRemoteCheck(const bd_addr_t *addr);
    char* GetWatchName(void);
    void ChangeBleConnectToMax();
    void ChangeBleConnectToDefault();
    // 耳机蓝牙连接状态
    void SetBlueToothConnectStatus(bool status);
    bool GetBlueToothConnectStatus();
    void SetDeviceName(std::string deviceName_);
    std::string GetDeviceName();
    void SetEarPhoneListHasData(bool status);
    uint32_t GetEarPhoneListHasData();
    void MoveMatchingDeviceToFront();
    //  更新UI上设备列表的信息
    void UpdateConnectStatusForDevice();
    void UpdateDisconnectStatusForDevice(const bd_addr_t &bdAddr);
    void UpdateConnectedStatusForDevice(const bd_addr_t &bdAddr);
    // 设置是否展示成功或失败的页面
    void SetConnectResultPage(bool redirct);
    bool GetConnectResultPage();
    // 取消配对
    void HandleUnpairRequest(void);
    void AddNodeFromPairListToScanList(const bd_addr_t &bdAddr);
    void SetConnectFragmentStatus(bool status);
    bool GetConnectFragmentStatus();
private:
    SettingBluetoothModel();
    void SetUpPairedDevicesList();
    void InitBluetoothInfo(void);
    bool aclConnected_{false};
    BtCaseInfo selectedDeviceInfo_{};
    bd_addr_t lastConnedAddr_{0};
    bd_addr_t remoteConnectAddr_{0};
    std::list<BtCaseInfo> btScansDevicesList_{};
    List<BtCaseInfo> *btPairedDevicesList_{nullptr};
    bool advStartState_{false};
    char watchName_[BD_NAME_LEN] = { 0 };
    bool btConnectStatus_{false};
    std::string deviceName_;
    uint32_t EarPhoneListHasData_ = 0;
    bool redirct_ = false;
    bool connectFragmentStatus_ = false;
};

#ifdef __cplusplus
extern "C" {
#endif
void SettingBtGapReadRemoteRssiEventCallback(const bd_addr_t *bdAddr, int rssi, int status);
void SettingBtGapDiscoveryStateChangedCallback(int status);
void SettingBtGapAclStateChangedCallback(const bd_addr_t *bdAddr, gap_acl_state_t state, unsigned int reason);
void SettingBtRemoteNameChangedCallback(const bd_addr_t *bdAddr, const unsigned char *deviceName, unsigned char length);
void SettingBtIsAcceptConnOnSafeModeCallback(const bd_addr_t *bd_addr, bool *res);
void SettingBtInitCallBacks(void);
void SettingBtInitCallBacksEx(void);
void SettingBtInitGatts(void);
void SettingBtInitSpp(void);
void BleSettingAdvStart(void);
#ifdef __cplusplus
}
#endif
}

#endif

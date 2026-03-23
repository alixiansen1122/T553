/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingBluetoothHeadsetEvent.
 */
#ifndef SETTING_BLUETOOTH_HEADSET_EVENT_H
#define SETTING_BLUETOOTH_HEADSET_EVENT_H

#include "bts_def.h"
#include "bts_br_gap.h"
#include "td_type.h"
#include "bt_audio.h"
#ifdef __cplusplus
extern "C" {
#endif

enum {
    SETTING_AVRCP_SRV_PRESS_VALUE,
    SETTING_AVRCP_SRV_RELEASED_VALUE,
    SETTING_AVRCP_SRV_INIT_VALUE
};

typedef struct {
    td_pvoid stream_hdl;
    uint8_t loc_volume;
    uint8_t play_stat;
    uint8_t dir_forward;
    uint8_t avrcp_button_flag;
    bd_addr_t bd_addr;
    uint8_t reg_cap_auto;
    uint8_t resrv;
    profile_connect_state_t avrcp_srv_conn_stat;
} SettingBtAvrcpTgInf;

void SettingUsingPressButtonStatusCallback(td_u32 keyOperation, td_u32 keyValue);
void SettingMediaStatusCallback(bt_avrcp_tg_evt_status_param *eventStatusParam);
void SettingPressButtonStatusCallback(td_u32 keyOperation, td_u32 keyValue);
void SettingConnCtStateChangedCallback(const bd_addr_t *bdAddr, profile_connect_state_t state);
void SettingConnectTgStateChangedCallback(const bd_addr_t *bdAddr, profile_connect_state_t state);
void SettingInitBtHeadsetEvent(void);

#ifdef __cplusplus
}
#endif

#endif // SETTING_BLUETOOTH_HEADSET_EVENT_H
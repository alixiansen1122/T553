/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingBluetoothHeadsetEvent
 * Create: 2022-3-22
 */

#include "settings/model/SettingBluetoothHeadsetEvent.h"
#include "bts_avrcp_target.h"
#include "bts_avrcp_controller.h"
#include "player/PlayersModel.h"
#include "graphic_service.h"

using namespace OHOS;

#ifdef __cplusplus
extern "C" {
#endif

#define AVRCP_CT_AUTO_REG_TYPE    1
uint8_t g_avrcp_tg_local_media_vol = AVRCP_ABSOLUTE_VOLUME_INVALID;
#define BT_A2DP_MEDIA_VOL_LEVEL 100

bt_avrcp_tg_bts_cbk g_avrcp_tg_audio_cb;
avrcp_ct_callbacks_t g_avrcp_ct_cb;
avrcp_tg_callbacks_t g_avrcp_tg_cb;
SettingBtAvrcpTgInf g_avrcp_srv_inf;

void SettingBtAvrcpVolumeChangedSyncMedia(uint8_t a2dp_volume)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "[SettingBtAvrcpVolumeChangedSyncMedia]::a2dp_volume = 0x%x\r\n", a2dp_volume);
    int32_t audio_volume = a2dp_volume * BT_A2DP_MEDIA_VOL_LEVEL / AVRCP_ABSOLUTE_VOLUME_PERCENTAGE_100;
    AudioManager::GetInstance().SetVolume(AUDIO_STREAM_A2DP_MUSIC, audio_volume); /* 设置增益音量 */
    g_avrcp_tg_local_media_vol = a2dp_volume;
    avrcp_tg_notify_volume_changed(a2dp_volume);
}

/* tg通知媒体状态 */
static void SettingBtAvrcpTgNotifyMeidaStatus(bt_avrcp_tg_evt_status_param *notifyEventStatusParam)
{
    if (notifyEventStatusParam == nullptr || notifyEventStatusParam->event_id >= AVRCP_NOTIFY_EVENT_RESERVED) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[SettingBtAvrcpTgNotifyMeidaStatus] param is error");
        return;
    }
    switch (notifyEventStatusParam->event_id) {
        case AVRCP_NOTIFY_EVENT_PLAYBACK_STATUS_CHANGED:
            avrcp_tg_notify_playback_status_changed(*(td_u8 *)notifyEventStatusParam->event_status);
            break;
        case AVRCP_NOTIFY_EVENT_TRACK_CHANGED:
            avrcp_tg_notify_track_changed(*(unsigned long long *)notifyEventStatusParam->event_status);
            break;
        case AVRCP_NOTIFY_EVENT_TRACK_REACHED_END:
            avrcp_tg_notify_track_reached_end();
            break;
        case AVRCP_NOTIFY_EVENT_TRACK_REACHED_START:
            avrcp_tg_notify_track_reached_start();
            break;
        case AVRCP_NOTIFY_EVENT_PLAYBACK_POS_CHANGED:
            avrcp_tg_notify_playback_pos_changed(*(td_u32 *)notifyEventStatusParam->event_status);
            break;
        case AVRCP_NOTIFY_EVENT_BATT_STATUS_CHANGED:
        case AVRCP_NOTIFY_EVENT_SYSTEM_STATUS_CHANGED:
        case AVRCP_NOTIFY_EVENT_PLAYER_APPLICATION_SETTING_CHANGED:
            break;
        case AVRCP_NOTIFY_EVENT_NOW_PLAYING_CONTENT_CHANGED:
            avrcp_tg_notify_now_playing_content_changed();
            break;
        case AVRCP_NOTIFY_EVENT_AVAILABLE_PLAYERS_CHANGED:
            avrcp_tg_notify_available_players_changed();
            break;
        case AVRCP_NOTIFY_EVENT_ADDRESSED_PLAYER_CHANGED:
            avrcp_tg_notify_addressed_players_changed(*(td_u16 *)notifyEventStatusParam->event_status,
                *(td_u16 *)(notifyEventStatusParam->event_status + sizeof(td_u16)));
            break;
        case AVRCP_NOTIFY_EVENT_UIDS_CHANGED:
            avrcp_tg_notify_uid_changed(*(td_u16 *)notifyEventStatusParam->event_status);
            break;
        case AVRCP_NOTIFY_EVENT_VOLUME_CHANGED:
            SettingBtAvrcpVolumeChangedSyncMedia(notifyEventStatusParam->event_status[0]);
            break;
        case AVRCP_NOTIFY_EVENT_RESERVED:
            break;
        default:
            break;
    }
}

void SettingPressButtonStatusCallback(td_u32 keyOperation, td_u32 keyValue)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[SettingPressButtonStatusCallback]::keyValue = 0x%x  keyOperation = 0x%x",
        keyValue, keyOperation);
    GraphicService::GetInstance()->PostGraphicEvent(std::bind(SettingUsingPressButtonStatusCallback, keyOperation, keyValue));
}

void SettingBtAvrcpTgNotifyVolumeChangedProc(uint8_t a2dp_volume)
{
    int32_t audio_volume = a2dp_volume * BT_A2DP_MEDIA_VOL_LEVEL / AVRCP_ABSOLUTE_VOLUME_PERCENTAGE_100;
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "NotifyVolume = %d", audio_volume);
    AudioManager::GetInstance().SetVolume(AUDIO_STREAM_MUSIC, audio_volume); // 耳机连接手表场景调节手表音量
}

void SettingUsingPressButtonStatusCallback(td_u32 keyOperation, td_u32 keyValue)
{
    bt_avrcp_tg_evt_status_param event;
    switch (keyOperation) {
        case AVRCP_KEY_VOLUME_UP: // 耳机控制手表加减音量不走key_operation事件
        case AVRCP_KEY_VOLUME_DOWN: // 减小音量
            break;
        case AVRCP_KEY_MUTE: // 静音
            event.event_status[0] = AVRCP_ABSOLUTE_VOLUME_PERCENTAGE_0;
            event.event_id = AVRCP_NOTIFY_EVENT_VOLUME_CHANGED;
            break;
        case AVRCP_KEY_PLAY: // 播放
            (void)PlayersModel::GetInstance()->PlayerPlay();
            event.event_status[0] = AVRCP_PLAY_STATUS_PLAYING;
            event.event_id = AVRCP_NOTIFY_EVENT_PLAYBACK_STATUS_CHANGED;
            break;
        case AVRCP_KEY_STOP: // 停止
            (void)PlayersModel::GetInstance()->PlayerStop();
            event.event_status[0] = AVRCP_PLAY_STATUS_STOPPED;
            event.event_id = AVRCP_NOTIFY_EVENT_PLAYBACK_STATUS_CHANGED;
            break;
        case AVRCP_KEY_PAUSE: // 暂停
            (void)PlayersModel::GetInstance()->PlayerPause();
            event.event_status[0] = AVRCP_PLAY_STATUS_PAUSED;
            event.event_id = AVRCP_NOTIFY_EVENT_PLAYBACK_STATUS_CHANGED;
            break;
        case AVRCP_KEY_FORWARD: // 下一曲
            PlayersModel::GetInstance()->LoopingPlayout();
            event.event_status[0] = AVRCP_PLAY_STATUS_FWD_SEEK;
            event.event_id = AVRCP_NOTIFY_EVENT_PLAYBACK_STATUS_CHANGED;
            break;
        case AVRCP_KEY_BACKWARD: // 上一曲
            PlayersModel::GetInstance()->PreviousPlayout();
            event.event_status[0] = AVRCP_PLAY_STATUS_REV_SEEK;
            event.event_id = AVRCP_NOTIFY_EVENT_PLAYBACK_STATUS_CHANGED;
            break;
        case AVRCP_NOTIFY_EVENT_VOLUME_CHANGED:
            /* 手机连接手表场景，控制手表喇叭声音 */
            event.event_status[0] = keyValue;
            event.event_id = AVRCP_NOTIFY_EVENT_VOLUME_CHANGED;
            SettingBtAvrcpTgNotifyVolumeChangedProc((uint8_t)keyValue);
        default:
            break;
    }

    SettingBtAvrcpTgNotifyMeidaStatus(&event);
}

uint8_t SettingBtAvrcpTgGetLocMediaVolume(void)
{
    if (g_avrcp_tg_local_media_vol == AVRCP_ABSOLUTE_VOLUME_INVALID) {
        int32_t audio_volume = AudioManager::GetInstance().GetVolume(AUDIO_STREAM_A2DP_MUSIC);
        uint8_t a2dp_volume = (uint8_t)(audio_volume * AVRCP_ABSOLUTE_VOLUME_PERCENTAGE_100 / BT_A2DP_MEDIA_VOL_LEVEL);
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "audio_volume 0x%x, a2dp_volume 0x%x\n", audio_volume, a2dp_volume);
        g_avrcp_tg_local_media_vol = a2dp_volume;
    }

    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "get local media volume, a2dp_volume = 0x%x", g_avrcp_tg_local_media_vol);
    return g_avrcp_tg_local_media_vol;
}

void SettingMediaStatusCallback(bt_avrcp_tg_evt_status_param *eventStatusParam)
{
    if (eventStatusParam == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "eventStatusParam is nullptr");
        return;
    }

    int32_t playerStatus = PlayersModel::GetInstance()->GetPlayerstatus();
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "playerStatus = %d", playerStatus);

    switch (eventStatusParam->event_id) {
        case AVRCP_NOTIFY_EVENT_PLAYBACK_STATUS_CHANGED:
            eventStatusParam->event_status[0] = playerStatus;
            break;
        case AVRCP_NOTIFY_EVENT_TRACK_CHANGED:
        case AVRCP_NOTIFY_EVENT_TRACK_REACHED_END:
            break;
        case AVRCP_NOTIFY_EVENT_TRACK_REACHED_START:
            break;
        case AVRCP_NOTIFY_EVENT_PLAYBACK_POS_CHANGED:
            eventStatusParam->event_status[0] = 0xFF;
            break;
        case AVRCP_NOTIFY_EVENT_BATT_STATUS_CHANGED:
        case AVRCP_NOTIFY_EVENT_SYSTEM_STATUS_CHANGED:
        case AVRCP_NOTIFY_EVENT_PLAYER_APPLICATION_SETTING_CHANGED:
            break;
        case AVRCP_NOTIFY_EVENT_NOW_PLAYING_CONTENT_CHANGED:
            break;
        case AVRCP_NOTIFY_EVENT_AVAILABLE_PLAYERS_CHANGED:
            break;
        case AVRCP_NOTIFY_EVENT_ADDRESSED_PLAYER_CHANGED:
            break;
        case AVRCP_NOTIFY_EVENT_UIDS_CHANGED:
            break;
        case AVRCP_NOTIFY_EVENT_VOLUME_CHANGED:
            eventStatusParam->event_status[0] = SettingBtAvrcpTgGetLocMediaVolume();
            break;
        case AVRCP_NOTIFY_EVENT_RESERVED:
            break;
        default :
            break;
    }
}

void SettingConnCtStateChangedCallback(const bd_addr_t *bdAddr, profile_connect_state_t state)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "avrcp ct update conn stat:%u", state);
    if (bdAddr == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "bdaddr nullptr");
        return;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SettingConnCtStateChangedCallback addr: %02x%02x%02x%02x%02x%02x ",
        bdAddr->addr[5], bdAddr->addr[4], bdAddr->addr[3], /* 5 4 3 idx */
        bdAddr->addr[2], bdAddr->addr[1], bdAddr->addr[0]); /* 2 1 0 idx */
    (void)memcpy_s(g_avrcp_srv_inf.bd_addr.addr, BD_ADDR_LEN, bdAddr->addr, BD_ADDR_LEN);
    g_avrcp_srv_inf.avrcp_srv_conn_stat = state;
}

static bd_addr_t *SettingBtAvrcpSrvGetActiveAddr(void)
{
    return &g_avrcp_srv_inf.bd_addr;
}

void SettingConnectTgStateChangedCallback(const bd_addr_t *bdAddr, profile_connect_state_t state)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "avrcp tg update conn stat:%u", state);
    if (bdAddr == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "bdaddr nullptr");
        return;
    }

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SettingConnCtStateChangedCallback addr: %02x%02x%02x%02x%02x%02x ",
        bdAddr->addr[5], bdAddr->addr[4], bdAddr->addr[3], /* 5 4 3 idx */
        bdAddr->addr[2], bdAddr->addr[1], bdAddr->addr[0]); /* 2 1 0 idx */
    (void)memcpy_s(g_avrcp_srv_inf.bd_addr.addr, BD_ADDR_LEN, bdAddr->addr, BD_ADDR_LEN);
    g_avrcp_srv_inf.avrcp_srv_conn_stat = state;

    /* 手表耳机链接场景时，耳机控制音量是一个事件而非key_operation，故需要在手表侧向耳机注册音量变更事件 */
    if (state == PROFILE_STATE_CONNECTED) {
        (void)avrcp_ct_get_supported_events(SettingBtAvrcpSrvGetActiveAddr());
    }
}

static void SettingBtAvrcpCtPressButtonRspCallback(const bd_addr_t *bd_addr, int32_t key_operation)
{
    if (bd_addr == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "null pointer\n");
        return;
    }
    if (g_avrcp_srv_inf.avrcp_button_flag == SETTING_AVRCP_SRV_RELEASED_VALUE) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "avrcp ct press button button_flag change:%u->%u\n",
            g_avrcp_srv_inf.avrcp_button_flag, SETTING_AVRCP_SRV_PRESS_VALUE);
        g_avrcp_srv_inf.avrcp_button_flag = SETTING_AVRCP_SRV_PRESS_VALUE;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "avrcp ct press button cbk key:%d button_flag:%u\n",
        key_operation, g_avrcp_srv_inf.avrcp_button_flag);
}

static void SettingBtAvrcpPressReleaseButtonLog(uint32_t key_operation)
{
    switch (key_operation) {
        case AVRCP_KEY_VOLUME_UP:
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AVRCP_KEY_VOLUME_UP\n");
            break;
        case AVRCP_KEY_VOLUME_DOWN:
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AVRCP_KEY_VOLUME_DOWN\n");
            break;
        case AVRCP_KEY_MUTE:
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AVRCP_KEY_VOLUME_MUTE\n");
            break;
        case AVRCP_KEY_PLAY:
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AVRCP_KEY_PLAY\n");
            break;
        case AVRCP_KEY_STOP:
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AVRCP_KEY_STOP\n");
            break;
        case AVRCP_KEY_PAUSE:
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AVRCP_KEY_PAUSE\n");
            break;
        case AVRCP_KEY_FORWARD:
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AVRCP_KEY_FORWARD\n");
            break;
        case AVRCP_KEY_BACKWARD:
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AVRCP_KEY_BACKWARD\n");
            break;
        case AVRCP_NOTIFY_EVENT_VOLUME_CHANGED:
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AVRCP_EVENT_VOLUME_CHANGED\n");
            break;
        default:
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "invalid\n");
            break;
    }
}

static void SettingBtAvrcpCtReleaseButtonRspCallback(const bd_addr_t *bd_addr, int32_t key_operation)
{
    UNUSED(bd_addr);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "avrcp ct release button cbk key:%d\n", key_operation);
    if (g_avrcp_srv_inf.avrcp_button_flag == SETTING_AVRCP_SRV_PRESS_VALUE) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "avrcp ct released button button_flag change:%u->%u\n",
            g_avrcp_srv_inf.avrcp_button_flag, SETTING_AVRCP_SRV_RELEASED_VALUE);
        g_avrcp_srv_inf.avrcp_button_flag = SETTING_AVRCP_SRV_RELEASED_VALUE;
    }
    SettingBtAvrcpPressReleaseButtonLog((uint32_t)key_operation);
}

static void SettingBtAvrcpCtSupportCompanyCallback(const bd_addr_t *bd_addr, avrcp_ct_support_company_para_t *company)
{
    if (bd_addr == nullptr || company == nullptr || company->company_id == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP,  "null pointer\n");
        return;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "get company list:");
    for (uint8_t i = 0; i < company->company_num; i++) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, " 0x%x ", *company->company_id);
    }
}

static void SettingBtAvrcpCtSupportEventCallback(const bd_addr_t *bd_addr, avrcp_ct_support_event_para_t *event)
{
    if (bd_addr == nullptr || event == nullptr || event->event_id == nullptr) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "null pointer\n");
        return;
    }

    for (uint8_t i = 0; i < event->event_num; i++) {
        if (event->event_id[i] == AVRCP_NOTIFY_EVENT_VOLUME_CHANGED) {
            int ret = avrcp_ct_register_notification(bd_addr, event->event_id[i], 0);
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "register event_id: 0x%x, ret = 0x%x", event->event_id[i], ret);
        }
    }

    for (uint8_t i = 0; i < event->event_num; i++) {
        if (event->event_id[i] != AVRCP_NOTIFY_EVENT_VOLUME_CHANGED) {
            int ret = avrcp_ct_register_notification(bd_addr, event->event_id[i], 0);
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "register event_id: 0x%x, ret = 0x%x", event->event_id[i], ret);
        }
    }
}

static void SettingBtAvrcpCtGetElementAttriCallback(const bd_addr_t *bd_addr, avrcp_ct_element_attr_para_cb_t *attr_cb_para)
{
    if (bd_addr == nullptr || attr_cb_para == nullptr || attr_cb_para->attr_value == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "null pointer\n");
        return;
    }

    for (uint8_t i = 0; i < attr_cb_para->attr_num; i++) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "attrId-charaId-valLen: 0x%x-0x%x-%u \n",
            attr_cb_para->attr_value[i].attr_id,
            attr_cb_para->attr_value[i].character_set_id,
            attr_cb_para->attr_value[i].value_len);
        switch (attr_cb_para->attr_value[i].attr_id) {
            case AVRCP_CT_ELEMENT_ATTR_TITLE:
                WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "Attr Title:");
                break;
            case AVRCP_CT_ELEMENT_ATTR_ARTIST_NAME:
                WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "Attr Name:");
                break;
            case AVRCP_CT_ELEMENT_ATTR_ALBUM_NAME:
                WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "Attr Album:");
                break;
            case AVRCP_CT_ELEMENT_ATTR_TRACK_NUMBER:
                WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "Attr Track Number:");
                break;
            case AVRCP_CT_ELEMENT_ATTR_TOTAL_NUMBER_OF_TRACKS:
                WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "Attr Total Number Of Track:");
                break;
            case AVRCP_CT_ELEMENT_ATTR_TRACK_GENRE:
                WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "Attr Track GENRE:");
                break;
            case AVRCP_CT_ELEMENT_ATTR_TRACK_PLAYING_TIME:
                WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "Attr Track Playing Time_ms:");
                break;
            default:
                WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "Other:");
                break;
        }

        uint8_t *value = (uint8_t *)(void *)malloc(attr_cb_para->attr_value[i].value_len + 1);
        if (attr_cb_para->attr_value[i].value_len > 0 &&  attr_cb_para->attr_value[i].value != nullptr) {
            if (memcpy_s(value, attr_cb_para->attr_value[i].value_len,
                attr_cb_para->attr_value[i].value, attr_cb_para->attr_value[i].value_len) != EOK) {
                free(value);
                WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "memcpy_s fail.");
                return;
            }
        }
        value[attr_cb_para->attr_value[i].value_len] = '\0';
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, " %s \n", value);
        free(value);
    }
}

static void SettingBtAvrcpCtGetPlayStatusCallback(const bd_addr_t *bd_addr, avrcp_ct_play_status_cb_t *status)
{
    if (bd_addr == nullptr || status == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "null pointer\n");
        return;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "test get play status songlen:%u, songpos:%u, playstatus:%u:",
        status->song_length, status->song_position, status->play_status);
}

static void SettingBtAvrcpCtHdlNotifyVolumeRspCbk(avrcp_ct_notification_value_cb_t *notification)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "Set local volume: 0x%x\n", notification->volume); /* 注册事件后通知的音量 */
    SettingBtAvrcpTgNotifyVolumeChangedProc((uint8_t)notification->volume);
}

static void SettingBtAvrcpCtNotificationCallback(
    const bd_addr_t *bd_addr, avrcp_notify_event_t event_id, avrcp_ct_notification_value_cb_t *notification)
{
    if (bd_addr == nullptr || notification == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "null pointer\n");
        return;
    }

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "ct notification cbk event_id:0x%x\n", event_id);
    bool notify_flag = true;
    switch (event_id) {
        case AVRCP_NOTIFY_EVENT_PLAYBACK_STATUS_CHANGED:
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "tg notify ct playback status changed :%u\n",
                notification->play_status);
            break;
        case AVRCP_NOTIFY_EVENT_VOLUME_CHANGED:
            SettingBtAvrcpCtHdlNotifyVolumeRspCbk(notification);
            break;
        case AVRCP_NOTIFY_EVENT_NOW_PLAYING_CONTENT_CHANGED:
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "play conent changed event_id:0x%x\n", event_id);
            break;
        case AVRCP_NOTIFY_EVENT_TRACK_CHANGED:
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "track changed event_id:0x%x\n", event_id);
            break;
        case AVRCP_NOTIFY_EVENT_TRACK_REACHED_END:
        case AVRCP_NOTIFY_EVENT_TRACK_REACHED_START:
        case AVRCP_NOTIFY_EVENT_PLAYBACK_POS_CHANGED:
        case AVRCP_NOTIFY_EVENT_BATT_STATUS_CHANGED:
        case AVRCP_NOTIFY_EVENT_SYSTEM_STATUS_CHANGED:
        case AVRCP_NOTIFY_EVENT_PLAYER_APPLICATION_SETTING_CHANGED:
        case AVRCP_NOTIFY_EVENT_AVAILABLE_PLAYERS_CHANGED:
        case AVRCP_NOTIFY_EVENT_ADDRESSED_PLAYER_CHANGED:
        case AVRCP_NOTIFY_EVENT_UIDS_CHANGED:
        case AVRCP_NOTIFY_EVENT_RESERVED:
        default:
            notify_flag = false;
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "invalid event id event_id:0x%x\n", event_id);
            break;
    }
    if (notify_flag) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "notify reg event_id: 0x%x\n", event_id);
        avrcp_ct_register_notification(bd_addr, event_id, 0); /* TG通知变化后CT重新注册事件 */
    }
}

void SettingInitBtHeadsetEvent()
{
    g_avrcp_tg_audio_cb.notify_pass_through_status_cbk = SettingPressButtonStatusCallback;
    g_avrcp_tg_audio_cb.get_media_status_cbk = SettingMediaStatusCallback;

    g_avrcp_tg_cb.conn_state_changed_cb = SettingConnectTgStateChangedCallback;

    g_avrcp_ct_cb.conn_state_changed_cb = SettingConnCtStateChangedCallback;
    g_avrcp_ct_cb.press_button_cb = SettingBtAvrcpCtPressButtonRspCallback;
    g_avrcp_ct_cb.release_button_cb = SettingBtAvrcpCtReleaseButtonRspCallback;
    g_avrcp_ct_cb.supported_companies_cb = SettingBtAvrcpCtSupportCompanyCallback;
    g_avrcp_ct_cb.supported_event_cb = SettingBtAvrcpCtSupportEventCallback;
    g_avrcp_ct_cb.element_attr_cb = SettingBtAvrcpCtGetElementAttriCallback;
    g_avrcp_ct_cb.play_status_cb = SettingBtAvrcpCtGetPlayStatusCallback;
    g_avrcp_ct_cb.notification_cb = SettingBtAvrcpCtNotificationCallback;

    avrcp_tg_register_callbacks(&g_avrcp_tg_cb);
    avrcp_ct_register_callbacks(&g_avrcp_ct_cb);
    bt_avrcp_tg_register_audio_cbk(&g_avrcp_tg_audio_cb);
}
#ifdef __cplusplus
}
#endif
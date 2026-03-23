/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: PhoneService.cpp
 * Create: 2021-09-22
 */

#include "phoneservice/PhoneService.h"
#include "phoneservice/PhoneAudio.h"
#include "securec.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "audio_base_type.h"
#include "audio_manager.h"
#include <ctime>
#include "wearable_log.h"
#include "unistd.h"
#include "dirent.h"
#include "sys/stat.h"
#include "thread_adapter.h"
#include "graphic_service.h"
#include "broadcast_service.h"
#include "broadcast_feature.h"
#include "service_id_define.h"
#include "graphic_mutex.h"
#include "samgr_lite.h"
#include "uiservice/ui_service.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CALL_LOG_PATH "/user/data/calllog.bin"
#define CALL_TMP_LOG_PATH "/user/data/calltmplog.bin"
#define CALL_LOG_DIR "/user/data"

#ifndef UNUSED
#define UNUSED(x) ((void)(x))
#endif

#define INVALID_PHONE_SERVICE_VALUE (-1)

typedef enum {
    AUDIO_UNKNOW = 0,
    AUDIO_ACT_INTERRUPT_WATCH_RING, // 手表铃声焦点
    AUDIO_ACT_INTERRUPT_BT_SCO, // 蓝牙通话焦点
    AUDIO_CONN_IN_BT_SCO,
    AUDIO_CONN_OUT_BT_SCO,
    AUDIO_STREAM_INIT,
    AUDIO_STREAM_START_SCO,
    AUDIO_STREAM_START_DN,
    AUDIO_STREAM_START_UP,
} AudioState;

static PhoneCallInfo g_callList[MULTI_CALL_MAX_NUME];
static uint32 g_currCallNum = 0;
static uint32 g_finishOperate = OPERATE_NON_WATCH_FINISH;
static uint32 g_scoFlag = HFP_SCO_STATE_DISCONNECTED; // sco连接状态，同时也表示通话在手表还是在手机
static uint32 g_btConnState = PROFILE_STATE_DISCONNECTED; // 蓝牙连接状态
static bd_addr_t g_bdAddr; // 设备的蓝牙地址
static uint32 g_currCallState = HFP_HF_CALL_STATE_FINISHED;
static bool g_completeCallFlag = false; // 完整通话流程：从incoming、dialing或alerting开始才算完整
static uint32 g_audioState = AUDIO_UNKNOW;
static OHOS::GraphicMutex g_audioMutex;
static uint32 g_call_codec = mSBC;

static int g_hdlUpMic = INVALID_PHONE_SERVICE_VALUE;
static int g_hdlUpSco = INVALID_PHONE_SERVICE_VALUE;
static int g_hdlDnSpk = INVALID_PHONE_SERVICE_VALUE;
static int g_hdlDnSco = INVALID_PHONE_SERVICE_VALUE;

PhoneServiceCb g_callChanged;
void HfpIncomingEventPublish(uint16 topic, const PhoneHfpCallInfo &callsInfo);

static void PhoneServiceScreenCtr(bool turnOnFlag)
{
    if (turnOnFlag) {
        ScreenTurnOn(-1);
    } else {
        ScreenTurnOff();
    }
}

static void InitMutex(void)
{
}

uint32 GetCallCodec(void)
{
    return g_call_codec;
}

int RegisterCallChangedCb(PhoneServiceCb *phoneServiceFun)
{
    if (phoneServiceFun == nullptr) {
        return OHOS_FAILURE;
    }

    (void)memcpy_s(&g_callChanged, sizeof(g_callChanged), phoneServiceFun, sizeof(g_callChanged));

    return OHOS_SUCCESS;
}

static void CallChangedCb(const PhoneCallInfo *phoneCall)
{
    g_callChanged.callChanged(phoneCall);
}

static void PhoneServiceErrorCallback(PhoneServiceErrorType type, int volume)
{
    g_callChanged.phoneServiceErrorCb(type, volume);
}

static void ScoStatusCb(hfp_sco_connect_state_t state)
{
    g_callChanged.scoStatusCb(state);
}

int GetCurrCallState(void)
{
    return g_currCallState;
}

// 初始化媒体
static void AudioInit(void)
{
    int32_t ret = PhoneAudioManagerInit();
    if (ret != OHOS_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[AudioInit] failed!");
    }
}

// 释放音频焦点
static void AudioDeactInterrupt(AudioStreamType type)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[AudioDeactInterrupt] start type:0x%x, audioState:0x%x",
                  type, g_audioState);

    if ((type == AUDIO_STREAM_VOICE_CALL_BT_SCO && g_audioState >= AUDIO_ACT_INTERRUPT_BT_SCO) ||
        (type == AUDIO_STREAM_RING && g_audioState == AUDIO_ACT_INTERRUPT_WATCH_RING)) {
        (void)PhoneAudioManagerDeactInterrupt();
    }
}

// 申请音频焦点
static void AudioActInterrupt(AudioStreamType type)
{
    int ret = OHOS_SUCCESS;

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[AudioActInterrupt] start type:0x%x, audioState:0x%x",
                  type, g_audioState);

    if (type != AUDIO_STREAM_VOICE_CALL_BT_SCO && type != AUDIO_STREAM_RING) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[AudioActInterrupt] failed! type:0x%x, audioState:0x%x",
                      type, g_audioState);
        return;
    }

	g_audioMutex.Lock();
    // 申请蓝牙通话焦点
    if (type == AUDIO_STREAM_VOICE_CALL_BT_SCO && g_audioState < AUDIO_ACT_INTERRUPT_BT_SCO) {
        if (g_audioState == AUDIO_ACT_INTERRUPT_WATCH_RING) {
            AudioDeactInterrupt(AUDIO_STREAM_RING);
        }

        ret = PhoneAudioManagerActInterrupt(type);
        if (ret == OHOS_SUCCESS) {
            g_audioState = AUDIO_ACT_INTERRUPT_BT_SCO;
        }
    } else if (type == AUDIO_STREAM_RING && g_audioState < AUDIO_ACT_INTERRUPT_WATCH_RING) {
        ret = PhoneAudioManagerActInterrupt(type);
        if (ret == OHOS_SUCCESS) {
            g_audioState = AUDIO_ACT_INTERRUPT_WATCH_RING;
        }
    }

    if (ret != OHOS_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[AudioActInterrupt] failed! type:0x%x, audioState:0x%x",
                      type, g_audioState);
    }

    g_audioMutex.Unlock();
}

// 媒体连接SCO设备
static void AudioConnectScoDevInit(void)
{
    int ret;

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[AudioConnectScoDevInit] start scoFlag:0x%x, audioState:0x%x",
                  g_scoFlag, g_audioState);
    if (g_scoFlag != HFP_SCO_STATE_CONNECTED || g_audioState < AUDIO_ACT_INTERRUPT_BT_SCO) {
        return;
    }

    g_audioMutex.Lock();
    if (g_audioState < AUDIO_STREAM_INIT) {
        if (g_audioState < AUDIO_CONN_IN_BT_SCO) {
            // 配置媒体连接SCO设配状态（下行），只有在SCO连接后才能设置
            ret = PhoneAudioManagerSetDeviceConnState(IN_BLUETOOTH_SCO, AUDIO_DEVICE_CONNECTED);
            if (ret != OHOS_SUCCESS) {
                WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[AudioConnectScoDevInit] sco in ret:%d", ret);
                g_audioMutex.Unlock();
                return;
            } else {
                g_audioState = AUDIO_CONN_IN_BT_SCO;
            }
        }

        if (g_audioState < AUDIO_CONN_OUT_BT_SCO) {
            // 配置媒体连接SCO设配状态（上行），只有在SCO连接后才能设置
            ret = PhoneAudioManagerSetDeviceConnState(OUT_BLUETOOTH_SCO, AUDIO_DEVICE_CONNECTED);
            if (ret != OHOS_SUCCESS) {
                WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[AudioConnectScoDevInit] sco out ret:%d", ret);
                g_audioMutex.Unlock();
                return;
            } else {
                g_audioState = AUDIO_CONN_OUT_BT_SCO;
            }
        }

        if (g_hdlUpSco == INVALID_PHONE_SERVICE_VALUE) {
            g_hdlUpSco = PhoneAudioStreamOutInit(AUDIO_UP_LINK);
        }

        if (g_hdlDnSco == INVALID_PHONE_SERVICE_VALUE) {
            g_hdlDnSco = PhoneAudioStreamInInit(AUDIO_DOWN_LINK);
        }

        if (g_hdlUpSco != INVALID_PHONE_SERVICE_VALUE && g_hdlDnSco != INVALID_PHONE_SERVICE_VALUE) {
            g_audioState = AUDIO_STREAM_INIT;
        }
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[AudioConnectScoDevInit] end audioState:%d, hdlUpSco:%d, hdlDnSco:%d",
                  g_audioState, g_hdlUpSco, g_hdlDnSco);
    g_audioMutex.Unlock();

    return;
}

// 建立上行通路
static void AudioUpStart(void)
{
    int ret = OHOS_SUCCESS;
    uint32 micShmId = 0;

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[AudioUpStart] start scoFlag:%d, audioState:%d", g_scoFlag, g_audioState);

    if (g_scoFlag != HFP_SCO_STATE_CONNECTED || g_audioState < AUDIO_STREAM_INIT) {
        return;
    }

    g_audioMutex.Lock();
    if (g_audioState < AUDIO_STREAM_START_UP) {
        if (g_hdlUpMic == INVALID_PHONE_SERVICE_VALUE) {
            g_hdlUpMic = PhoneAudioStreamInInit(AUDIO_UP_LINK);
        }

        if (g_audioState < AUDIO_STREAM_START_SCO) {
            ret = PhoneAudioStreamInStart(g_hdlDnSco);
            if (ret != OHOS_SUCCESS) {
                WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[AudioUpStart] dn_sco ret:%d", ret);
            }

            ret |= PhoneAudioStreamOutStart(g_hdlUpSco);
            if (ret != OHOS_SUCCESS) {
                WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[AudioUpStart] up_sco ret:%d", ret);
            }
            if (ret == OHOS_SUCCESS) {
                g_audioState = AUDIO_STREAM_START_SCO;
            }
        }

        ret |= PhoneAudioStreamInStart(g_hdlUpMic);
        if (ret != OHOS_SUCCESS) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[AudioUpStart] up_mic ret:%d", ret);
        }

        ret |= PhoneAudioStreamInGetChannelId(g_hdlUpMic, &micShmId);
        if (ret != OHOS_SUCCESS) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[AudioUpStart] up_mic shm ret:%d", ret);
        }

        ret |= PhoneAudioStreamOutAttachFrontend(g_hdlUpSco, micShmId);
        if (ret != OHOS_SUCCESS) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[AudioUpStart] up_sco attach ret:%d", ret);
        }

        if (ret == OHOS_SUCCESS) {
            g_audioState = AUDIO_STREAM_START_UP;
        }
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[AudioUpStart] end audioState:%d, hdlUpMic:%d, hdlUpSco:%d, micShmId:0x%x",
        g_audioState, g_hdlUpMic, g_hdlUpSco, micShmId);
    g_audioMutex.Unlock();
    return;
}

// 建立下行通路
static void AudioDnStart(void)
{
    int ret = OHOS_SUCCESS;
    uint32 spkShmId = 0;

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[AudioDnStart] start scoFlag:%d, audioState:%d", g_scoFlag, g_audioState);

    if (g_scoFlag != HFP_SCO_STATE_CONNECTED || g_audioState < AUDIO_STREAM_INIT) {
        return;
    }

    g_audioMutex.Lock();
    if (g_audioState < AUDIO_STREAM_START_DN) {
        if (g_hdlDnSpk == INVALID_PHONE_SERVICE_VALUE) {
            g_hdlDnSpk = PhoneAudioStreamOutInit(AUDIO_DOWN_LINK);
        }

        if (g_audioState < AUDIO_STREAM_START_SCO) {
            ret = PhoneAudioStreamOutStart(g_hdlUpSco);
            if (ret != OHOS_SUCCESS) {
                WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[AudioDnStart] up_sco ret:%d", ret);
            }

            ret |= PhoneAudioStreamInStart(g_hdlDnSco);
            if (ret != OHOS_SUCCESS) {
                WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[AudioDnStart] dn_sco ret:%d", ret);
            }

            if (ret == OHOS_SUCCESS) {
                g_audioState = AUDIO_STREAM_START_SCO;
            }
        }

        ret |= PhoneAudioStreamOutStart(g_hdlDnSpk);
        if (ret != OHOS_SUCCESS) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[AudioDnStart] dn_spk ret:%d", ret);
        }

        ret |= PhoneAudioStreamOutGetChannelId(g_hdlDnSpk, &spkShmId);
        if (ret != OHOS_SUCCESS) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[AudioDnStart] dn_spk shm ret:%d", ret);
        }

        ret |= PhoneAudioStreamInAttachBackend(g_hdlDnSco, spkShmId);
        if (ret != OHOS_SUCCESS) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[AudioDnStart] dn_sco attach ret:%d", ret);
        }

        if (ret == OHOS_SUCCESS) {
            g_audioState = AUDIO_STREAM_START_DN;
        }
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[AudioDnStart] end audioState:%d, hdlDnSpk:%d, hdlDnSco:%d, spkShmId:0x%x",
        g_audioState, g_hdlDnSpk, g_hdlDnSco, spkShmId);
    g_audioMutex.Unlock();

    return;
}

// 同步音量
static void SyncVolume(void)
{
    unsigned char volume = GetCallCurrVolume();
    SetSpeakerVolume(volume);
}

// 蓝牙通话下行
static void AudioDn(void)
{
    // 申请蓝牙通话音频焦点
    AudioActInterrupt(AUDIO_STREAM_VOICE_CALL_BT_SCO);

    // 媒体连接SCO设备
    AudioConnectScoDevInit();

    // 建立音频下行通路
    AudioDnStart();

    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "[AudioDn] audioState:%d", g_audioState);
}

// 蓝牙通话上行
static void AudioUp(void)
{
    // 申请蓝牙通话音频焦点
    AudioActInterrupt(AUDIO_STREAM_VOICE_CALL_BT_SCO);

    // 媒体连接SCO设备
    AudioConnectScoDevInit();

    // 建立音频上行通路
    AudioUpStart();
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "[AudioUp] audioState:%d", g_audioState);
}

static void AudioSetup(void)
{
    if (g_audioState < AUDIO_STREAM_START_DN) {
        AudioDn();
    }

    if (g_audioState != AUDIO_STREAM_START_UP) {
        AudioUp();
    }
}

static void AudioUpRelease(void)
{
    int ret;

    if (g_hdlUpSco != INVALID_PHONE_SERVICE_VALUE) {
        ret = PhoneAudioStreamOutStop(g_hdlUpSco);
        if (ret != OHOS_SUCCESS) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[AudioUpRelease] [OutStop] hdlUpSco ret:%d", ret);
        }
        ret = PhoneAudioStreamOutDeinit(g_hdlUpSco);
        if (ret != OHOS_SUCCESS) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[AudioUpRelease] [OutDeinit] hdlUpSco ret:%d", ret);
        }
    }

    if (g_hdlUpMic != INVALID_PHONE_SERVICE_VALUE) {
        ret = PhoneAudioStreamInStop(g_hdlUpMic);
        if (ret != OHOS_SUCCESS) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[AudioDnRelease] [InStop] hdlUpMic ret:%d", ret);
        }
        ret = PhoneAudioStreamInDeinit(g_hdlUpMic);
        if (ret != OHOS_SUCCESS) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[AudioDnRelease] [InDeinit] hdlUpMic ret:%d", ret);
        }
    }

    if (g_audioState >= AUDIO_CONN_OUT_BT_SCO) {
        ret = PhoneAudioManagerSetDeviceConnState(OUT_BLUETOOTH_SCO, AUDIO_DEVICE_DISCONNECTED);
        if (ret != OHOS_SUCCESS) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[AudioDnRelease] sco out ret:%d", ret);
        }
    }

    g_hdlUpSco = INVALID_PHONE_SERVICE_VALUE;
    g_hdlUpMic = INVALID_PHONE_SERVICE_VALUE;
}

static void AudioDnRelease(void)
{
    int ret;

    if (g_hdlDnSco != INVALID_PHONE_SERVICE_VALUE) {
        ret = PhoneAudioStreamInStop(g_hdlDnSco);
        if (ret != OHOS_SUCCESS) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[AudioUpRelease] [InStop] hdlDnSco ret:%d", ret);
        }
        ret = PhoneAudioStreamInDeinit(g_hdlDnSco);
        if (ret != OHOS_SUCCESS) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[AudioUpRelease] [InDeinit] hdlDnSco ret:%d", ret);
        }
    }

    if (g_hdlDnSpk != INVALID_PHONE_SERVICE_VALUE) {
        ret = PhoneAudioStreamOutStop(g_hdlDnSpk);
        if (ret != OHOS_SUCCESS) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[AudioDnRelease] [OutStop] hdlDnSpk ret:%d", ret);
        }
        ret = PhoneAudioStreamOutDeinit(g_hdlDnSpk);
        if (ret != OHOS_SUCCESS) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[AudioDnRelease] [OutDeinit] hdlDnSpk ret:%d", ret);
        }
    }

    if (g_audioState >= AUDIO_CONN_IN_BT_SCO) {
        ret = PhoneAudioManagerSetDeviceConnState(IN_BLUETOOTH_SCO, AUDIO_DEVICE_DISCONNECTED);
        if (ret != OHOS_SUCCESS) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[AudioDnRelease] sco in ret:%d", ret);
        }
    }

    g_hdlDnSco = INVALID_PHONE_SERVICE_VALUE;
    g_hdlDnSpk = INVALID_PHONE_SERVICE_VALUE;
}


/* 销毁上下行通路 释放音频焦点 */
void AudioRelease(void)
{
    if (g_audioState == AUDIO_UNKNOW) {
        return;
    }

    g_audioMutex.Lock();
    AudioUpRelease();
    AudioDnRelease();

    /* 释放音频焦点 */
    AudioDeactInterrupt(AUDIO_STREAM_VOICE_CALL_BT_SCO);
    AudioDeactInterrupt(AUDIO_STREAM_RING); // 适配某些机型不触发SCO
    g_audioState = AUDIO_UNKNOW;

    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "[AudioRelease] audioState:%d,", g_audioState);
    g_audioMutex.Unlock();
}

static void CallListInit(void)
{
    g_currCallNum = 0;
    (void)memset_s(g_callList, sizeof(g_callList), 0, sizeof(g_callList));
    for (int i = 0; i < MULTI_CALL_MAX_NUME; i++) {
        g_callList[i].lastStatus = HFP_HF_CALL_STATE_FINISHED;
    }
}

static void CopyBdAddr(const bd_addr_t &bdAddr)
{
    unsigned char addr[BD_ADDR_LEN] = {0};
    if (strncmp((const char*)g_bdAddr.addr, (const char*)addr, BD_ADDR_LEN) == 0) {
        (void)memcpy_s(g_bdAddr.addr, BD_ADDR_LEN, bdAddr.addr, BD_ADDR_LEN);
    }
}

static void CatBdAddr(const bd_addr_t &bdAddr)
{
#if WEARABLEOG_DEBUG_ENABLE
    for (unsigned char i = 0; i < BD_ADDR_LEN; i++) {
        printf("%X", bdAddr.addr[i]);
    }
    printf("");
#else
    UNUSED(bdAddr);
#endif
}

static void BtConnectedPro(const bd_addr_t &bdAddr)
{
    g_btConnState = PROFILE_STATE_CONNECTED;
    CopyBdAddr(bdAddr);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[BtConnectedPro] g_bdAddr: ");
    CatBdAddr(g_bdAddr);
}

static void BtDisconnectedPro(void)
{
    g_btConnState = PROFILE_STATE_DISCONNECTED;
    (void)memset_s(g_bdAddr.addr, BD_ADDR_LEN, 0, BD_ADDR_LEN);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[BtDisconnectedPro] g_bdAddr: ");
    CatBdAddr(g_bdAddr);

    // 通知APP蓝牙断开连接
    PhoneServiceErrorCallback(ERROR_BT, PROFILE_STATE_DISCONNECTED);
    CallListInit();

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "BtDisconnectedPro callState:%d", g_currCallState);
    if (g_currCallState == HFP_HF_CALL_STATE_INCOMING) {
        /* 处理呼入阶段，取消手表配对 */
        PhoneHfpCallInfo call = {0};
        HfpIncomingEventPublish(TOPIC_EVENT_HFP_FINISHED, call);
    }

    g_currCallState = HFP_HF_CALL_STATE_FINISHED;
    g_scoFlag = HFP_SCO_STATE_DISCONNECTED;
    g_completeCallFlag = false;
}

// 蓝牙状态连接
static void ConnStateChanged(const bd_addr_t &bdAddr, profile_connect_state_t state)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "[ConnStateChanged] state = 0x%X bdAddr: ", state);
                  CatBdAddr(bdAddr);

    switch (state) {
        case PROFILE_STATE_CONNECTING: // 蓝牙正在连接
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[ConnStateChanged] BT connecting");
            break;
        case PROFILE_STATE_CONNECTED: // 蓝牙连接成功
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[ConnStateChanged] BT connected");
            BtConnectedPro(bdAddr);
            break;
        case PROFILE_STATE_DISCONNECTING: // 蓝牙连接断连
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[ConnStateChanged] BT disconnecting");
            break;
        case PROFILE_STATE_DISCONNECTED: // 蓝牙断开连接
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[ConnStateChanged] BT disconnected");
            BtDisconnectedPro();
            break;
        default:
            break;
    }
}

static void ConnStateChangedCb(const bd_addr_t *bdAddr, profile_connect_state_t state)
{
    GraphicService::GetInstance()->PostGraphicEvent(std::bind(ConnStateChanged, *bdAddr, state));
}

// 获取蓝牙状态
uint32 GetBtConnectStatus(void)
{
    g_btConnState = (uint32)hfp_hf_get_device_connect_state(&g_bdAddr);
    return g_btConnState;
}

// SCO连接处理
static void ScoConnectPro(void)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "[ScoConnectPro] CallNum:%d, ScoFlag:%d", g_currCallNum, g_scoFlag);
    // 通知sco建立连接(或通话转移：手机 -> 手表)
    ScoStatusCb(HFP_SCO_STATE_CONNECTED);
    if (g_currCallState == HFP_HF_CALL_STATE_ACTIVE && g_scoFlag == HFP_SCO_STATE_CONNECTED) {
        // The call is transferred from the phone to the watch. Need to show the call notification view.
        PhoneHfpCallInfo call = {0};
        HfpIncomingEventPublish(TOPIC_EVENT_HFP_PHONE_TO_WATCH, call);
    }

    AudioSetup();
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "[ScoConnectPro] audioState:%d", g_audioState);
}

// SCO断连处理
static void ScoDisconnectPro(void)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "[ScoDisconnectPro] start CallState:%d", g_currCallState);

    ScoStatusCb(HFP_SCO_STATE_DISCONNECTED);
    if ((g_currCallState == HFP_HF_CALL_STATE_ACTIVE && g_scoFlag == HFP_SCO_STATE_DISCONNECTED) ||
        (g_scoFlag == HFP_SCO_STATE_CONNECT_FAILURE)) {
        // The call is transferred from the watch to the phone. Need to close the call notification view.
        PhoneHfpCallInfo call = {0};
        HfpIncomingEventPublish(TOPIC_EVENT_HFP_WATCH_TO_PHONE, call);
    }
    AudioRelease(); // 为了保持时序，此处采取同步调用
    usleep(5000); // 增加延时确保SCO断开AUDIO通路释放成功
}

// Sco连接状态变化回调函数，音频连接状态
static void ScoStateChanged(const bd_addr_t &bdAddr, hfp_sco_connect_state_t state)
{
    g_scoFlag = state;
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "[ScoStateChanged] hfpState:%d, ScoFlag:%d, CallNum:%d", state,
        g_scoFlag, g_currCallNum);

    switch (state) {
        case HFP_SCO_STATE_CONNECTING:
            PhoneServiceScreenCtr(1);
            if (g_scoFlag != HFP_SCO_STATE_CONNECTED) {
                g_scoFlag = HFP_SCO_STATE_CONNECTED;
                CopyBdAddr(bdAddr);
                GraphicService::GetInstance()->PostGraphicEvent([]() {
                    ScoConnectPro();
                    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[ScoStateChanged] SCO connecting!");
                });
            }
            break;
        case HFP_SCO_STATE_CONNECTED:
            // 同步音量
            SyncVolume();
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[ScoStateChanged] SCO connected!");
            break;

        case HFP_SCO_STATE_DISCONNECTING:
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[ScoStateChanged] SCO disconnecting!");
            break;

        case HFP_SCO_STATE_DISCONNECTED:
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[ScoStateChanged] SCO disconnected!");
            ScoDisconnectPro();
            PhoneServiceScreenCtr(0);
            break;

        case HFP_SCO_STATE_CONNECT_FAILURE:
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[ScoStateChanged] SCO connect failure!");
            ScoDisconnectPro();
            PhoneServiceScreenCtr(0);
            break;

        default:
            break;
    }
}

static void ScoStateChangedCb(const bd_addr_t *bdAddr, hfp_sco_connect_state_t state)
{
    GraphicService::GetInstance()->PostGraphicEvent(std::bind(ScoStateChanged, *bdAddr, state));
}

static void CallLogCreatDir(void)
{
    DIR *dir = opendir(CALL_LOG_DIR);
    if (dir == nullptr) {
        mkdir(CALL_LOG_DIR, S_IREAD | S_IWRITE);
    } else {
        closedir(dir);
    }
    return;
}

// 获取通话记录文件长度
static int GetCallLogFileSize(void)
{
    int len = 0;
    FILE *fp = fopen(CALL_LOG_PATH, "rb");
    if (fp == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[GetCallLogFileSize] fopen %s failed", CALL_LOG_PATH);
        return OHOS_FAILURE;
    }

    fseek(fp, 0, SEEK_END);
    len = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    fclose(fp);

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[GetCallLogFileSize] len:%d", len);
    return len;
}

// 添加通话进通话记录文件
static void AddToCallLog(const CallLogInfo &info)
{
    FILE *fp = nullptr;
    size_t ret;

    fp = fopen(CALL_LOG_PATH, "ab");
    if (fp == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[AddToCallLog] fopen %s failed!", CALL_LOG_PATH);
        return;
    }

    ret = fwrite(&info, sizeof(CallLogInfo), 1, fp);
    if (ret != 1) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[AddToCallLog] fwrite failed! ret:%d", ret);
    }

    fclose(fp);
    return;
}

static int CopyPhoneLogToTmpLog(void)
{
    FILE *fp = nullptr;
    FILE *ftp = nullptr;
    CallLogInfo *info = nullptr;

    int retLen;
    int fileSize = GetCallLogFileSize();
    if (fileSize == OHOS_FAILURE || fileSize == 0) {
        return fileSize;
    }

    retLen = fileSize - sizeof(CallLogInfo);
    if (retLen == 0) {
        return ClearCallLogFile(); // 只有一条记录，清空通话记录
    }

    fp = fopen(CALL_LOG_PATH, "rb");
    if (fp == nullptr) {
        return OHOS_FAILURE;
    }
    ftp = fopen(CALL_TMP_LOG_PATH, "wb+");
    if (ftp == nullptr) {
        fclose(fp);
        return OHOS_FAILURE;
    }

    info = (CallLogInfo *)malloc(fileSize);
    if (info == nullptr) {
        fclose(fp);
        fclose(ftp);
        return OHOS_FAILURE;
    }

    (void)memset_s(info, fileSize, 0, fileSize);
    if (fread(info, fileSize, 1, fp) != 1) {
        retLen = OHOS_FAILURE;
        goto END;
    }

    if (fwrite((info + 1), retLen, 1, ftp) != 1) {
        retLen = OHOS_FAILURE;
        goto END;
    }

END:
    fclose(fp);
    fclose(ftp);
    free(info);
    info = nullptr;
    return retLen;
}

// 删除第一条通话记录
int RemoveFirstCallLog(void)
{
    FILE *fp = nullptr;
    FILE *ftp = nullptr;
    CallLogInfo *info = nullptr;

    int ret = OHOS_SUCCESS;
    int fileSize = CopyPhoneLogToTmpLog();
    if (fileSize == OHOS_FAILURE || fileSize == 0) {
        return fileSize;
    }

    fp = fopen(CALL_LOG_PATH, "wb+");
    if (fp == nullptr) {
        return OHOS_FAILURE;
    }
    ftp = fopen(CALL_TMP_LOG_PATH, "rb");
    if (ftp == nullptr) {
        fclose(fp);
        return OHOS_FAILURE;
    }

    info = (CallLogInfo *)malloc(fileSize);
    if (info == nullptr) {
        fclose(fp);
        fclose(ftp);
        return OHOS_FAILURE;
    }
    (void)memset_s(info, fileSize, 0, fileSize);
    if (fread(info, fileSize, 1, ftp) != 1) {
        ret = OHOS_FAILURE;
        goto END;
    }

    if (fwrite(info, fileSize, 1, fp) != 1) {
        ret = OHOS_FAILURE;
        goto END;
    }

END:
    fclose(fp);
    fclose(ftp);
    free(info);
    info = nullptr;
    return ret;
}

// 添加通话记录
static void AddToCallLogPro(const PhoneCallInfo *callInfo)
{
    int fileSize = GetCallLogFileSize();
    int num = fileSize / sizeof(CallLogInfo);
    if (num >= CALL_LOG_MAX_NUM) {
        int ret = RemoveFirstCallLog();
        if (ret == OHOS_FAILURE) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[AddToCallLogPro] RemoveFirstCallLog error");
            return;
        }
    }

    CallLogInfo info = { 0 };
    int ret = memcpy_s(info.name, NAME_SIZE, callInfo->name, callInfo->nameLen);
    if (ret != EOK) {
        info.nameLen = 0;
    } else {
        info.nameLen = callInfo->nameLen;
    }

    ret = memcpy_s(info.tel, MAX_PHONE_NUM, callInfo->unitCalls.number, callInfo->unitCalls.number_len);
    if (ret != EOK) {
        info.telLen = 0;
    } else {
        info.telLen = callInfo->unitCalls.number_len;
    }

    info.creationTime = callInfo->creationTime;
    if (callInfo->lastStatus == HFP_HF_CALL_STATE_ACTIVE || callInfo->lastStatus == HFP_HF_CALL_STATE_HELD ||
        callInfo->lastStatus == HFP_HF_CALL_STATE_RESPONSE_HELD) {
        info.callDurTime = time(nullptr) - callInfo->activeTime;
    } else {
        info.callDurTime = 0;
    }

    if (callInfo->lastStatus == HFP_HF_CALL_STATE_INCOMING || callInfo->lastStatus == HFP_HF_CALL_STATE_WAITING) {
        info.status = CALL_MISSED;
    } else if (callInfo->unitCalls.outgoing == 1) {
        info.status = CALL_OUTGOING;
    } else {
        info.status = CALL_INCOMING;
    }

    AddToCallLog(info);
}

// 获取通话记录
int GetCallLog(CallLogInfo **info, int *num)
{
    FILE *fp = fopen(CALL_LOG_PATH, "rb");
    if (fp == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[GetCallLog] fopen %s failed!", CALL_LOG_PATH);
        return OHOS_FAILURE;
    }

    int fileSize = GetCallLogFileSize();
    if (fileSize == 0) {
        *info = nullptr;
        *num = 0;
        fclose(fp);
        return OHOS_SUCCESS;
    }

    int number = fileSize / sizeof(CallLogInfo);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[GetCallLog] fileSize:%d, number:%d", fileSize, number);
    CallLogInfo *callInfo = new CallLogInfo[number];
    if (callInfo == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[GetCallLog] new callInfo fail!");
        *info = nullptr;
        *num = 0;
        fclose(fp);
        return OHOS_FAILURE;
    }
    (void)memset_s(callInfo, fileSize, 0, fileSize);

    int ret = fread(callInfo, sizeof(CallLogInfo), number, fp);
    if (ret != number) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[GetCallLog] fread fp fail ret:%d", ret);
        delete[] callInfo;
        *info = nullptr;
        *num = 0;
        fclose(fp);
        return OHOS_FAILURE;
    }

    *info = callInfo;
    *num = number;
    fclose(fp);
    return OHOS_SUCCESS;
}

// 清空通话记录
int ClearCallLogFile(void)
{
    FILE *fp = fopen(CALL_LOG_PATH, "wb");
    if (fp == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[ClearCallLogFile] fopen %s failed!", CALL_LOG_PATH);
        return OHOS_FAILURE;
    }

    fclose(fp);
    return OHOS_SUCCESS;
}

// 查找号码对应的电话信息在list中的位置
static PhoneCallInfo *FindCallInfoInCallList(const unsigned char *number, unsigned char numberLen)
{
    int i;
    const char *str1 = nullptr;
    const char *str2 = nullptr;

    if (g_currCallNum == 0) {
        return nullptr;
    }

    for (i = 0; i < MULTI_CALL_MAX_NUME; i++) {
        if (numberLen != g_callList[i].telLen) {
            continue;
        } else {
            str1 = (const char*)number;
            str2 = (const char*)g_callList[i].tel;

            if (strncmp(str1, str2, numberLen) == 0) {
                return &g_callList[i];
            }
        }
    }

    return nullptr;
}

static void CatPhoneNumber(const PhoneHfpCallInfo &call)
{
#if WEARABLEOG_DEBUG_ENABLE
    for (unsigned char i = 0; i < call.number_len; i++) {
        printf("%c", call.number[i]);
    }
    printf("");
#else
    UNUSED(call);
#endif
}

static void AddCurrCallToCallList(const PhoneHfpCallInfo &call)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[AddCurrCallToCallList] state:%d", call.state);
    if (g_currCallNum >= MULTI_CALL_MAX_NUME ||
        (call.state != HFP_HF_CALL_STATE_ACTIVE && call.state != HFP_HF_CALL_STATE_HELD &&
        call.state != HFP_HF_CALL_STATE_ALERTING)) {
        return;
    }

    g_currCallNum++;
    int32_t tmp = g_currCallNum - 1;
    PhoneCallInfo *phoneCall = &g_callList[tmp];

    if (call.state == HFP_HF_CALL_STATE_ACTIVE) { // 正在通话
        phoneCall->operate = OPERATE_PHONE_ACTIVE;
        phoneCall->activeTime = time(nullptr);
    } else if (call.state == HFP_HF_CALL_STATE_HELD) { // 挂起
        phoneCall->operate = OPERATE_PHONE_HOLD;
        phoneCall->activeTime = time(nullptr);
    } else if (call.state == HFP_HF_CALL_STATE_ALERTING) { // 呼出，对方响铃
        phoneCall->operate = OPERATE_ALERTING;
    }

    // 使用通讯录接口获取当前电话对应的备注人名(通讯录待开发)

    phoneCall->lastStatus = call.state;
    phoneCall->creationTime = time(nullptr);

    int32_t ret = memcpy_s(phoneCall->tel, MAX_PHONE_NUM, call.number, call.number_len);
    if (ret != EOK) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[AddCurrCallToCallList] copy tel ret:%d", ret);
        CatPhoneNumber(call);
    }
    phoneCall->telLen = call.number_len;
    (void)memcpy_s(&phoneCall->unitCalls, sizeof(PhoneHfpCallInfo), &call, sizeof(PhoneHfpCallInfo));
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[AddCurrCallToCallList] end");

    // 通知手表电话呼入
    CallChangedCb(phoneCall);
}

// 通话状态变化--呼出，正在拨号处理
static void CallChangeDialPro(const PhoneHfpCallInfo &call)
{
    g_currCallNum++;
    int32_t tmp = g_currCallNum - 1;
    PhoneCallInfo *phoneCall = &g_callList[tmp];

    // 使用通讯录接口获取当前电话对应的备注人名(通讯录待开发)

    phoneCall->operate = OPERATE_DIALING;
    phoneCall->lastStatus = call.state;
    phoneCall->creationTime = time(nullptr);

    int32_t ret = memcpy_s(phoneCall->tel, MAX_PHONE_NUM, call.number, call.number_len);
    if (ret != EOK) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[CallChangeDialPro] copy tel ret:%d", ret);
        CatPhoneNumber(call);
    }
    phoneCall->telLen = call.number_len;

    (void)memcpy_s(&phoneCall->unitCalls, sizeof(PhoneHfpCallInfo), &call, sizeof(PhoneHfpCallInfo));

    // 通知手表正在拨号
    CallChangedCb(phoneCall);
}

// 通话状态变化--呼出，对方响铃
static void CallChangeAlertPro(const PhoneHfpCallInfo &call)
{
    PhoneCallInfo *phoneCall = FindCallInfoInCallList((const unsigned char*)call.number, call.number_len);
    if (phoneCall == nullptr) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[CallChangeAlertPro] Alert no this call");
        AddCurrCallToCallList(call);
        return;
    }
    phoneCall->operate = OPERATE_ALERTING;
    phoneCall->lastStatus = call.state;

    (void)memcpy_s(&phoneCall->unitCalls, sizeof(PhoneHfpCallInfo), &call, sizeof(PhoneHfpCallInfo));

    // 通知手表正对方响铃
    CallChangedCb(phoneCall);
}

void HfpIncomingEventPublish(uint16 topic, const PhoneHfpCallInfo &callsInfo)
{
    int32 ret;
    uint32 size = sizeof(PhoneHfpCallInfo);
    BroadcastApi *broadcastApi = nullptr;
    IUnknown *api = nullptr;

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "HfpIncomingEventPublish topic=%u!!", topic);

    void *publishData = malloc(size);
    if (publishData == nullptr) {
        return;
    }
    ret = memcpy_s(publishData, size, (const void *)&callsInfo, size);
    if (ret != EOK) {
        free(publishData);
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "HfpIncomingEventPublish memcpy_s fail! ret = %d", ret);
        return;
    }

    Request request = {
        .msgId = topic,
        .len = size,
        .msgValue = 0,
        .data = publishData,
    };

    /* public request */
    api = SAMGR_GetInstance()->GetFeatureApi(BROADCAST_SERVICE_NAME, BROADCAST_FEATURE_NAME);
    if (api == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "GetFeatureApi failed");
        free(publishData);
        return;
    }
    ret = api->QueryInterface(api, DEFAULT_VERSION, (void **)&broadcastApi);
    if (ret != 0 || broadcastApi == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "QueryInterface failed");
        free(publishData);
        return;
    }
    broadcastApi->PublishTopic((IUnknown *)broadcastApi, &request);
    broadcastApi->Release((IUnknown *)broadcastApi);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "HfpIncomingEventPublish end!!");

    return;
}

// 通话状态变化--呼入处理
static void CallChangeIncomePro(const PhoneHfpCallInfo &call)
{
    if (g_currCallNum >= MULTI_CALL_MAX_NUME ||
        (call.state != HFP_HF_CALL_STATE_INCOMING && call.state != HFP_HF_CALL_STATE_WAITING)) {
        return;
    }

    PhoneCallInfo *phoneCall = FindCallInfoInCallList((const unsigned char*)call.number, call.number_len);
    if (phoneCall != nullptr) {
        // 通话状态由呼入等待转换成呼入情况
    } else {
        g_currCallNum++;
        int32_t tmp = g_currCallNum - 1;
        phoneCall = &g_callList[tmp];
    }

    // 使用通讯录接口获取当前电话对应的备注人名(通讯录待开发)

    if (call.state == HFP_HF_CALL_STATE_INCOMING) {
        phoneCall->operate = OPERATE_INCOMING;
    } else {
        phoneCall->operate = OPERATE_WAITING;
    }
    phoneCall->lastStatus = call.state;
    phoneCall->creationTime = time(nullptr);

    int32_t ret = memcpy_s(phoneCall->tel, MAX_PHONE_NUM, call.number, call.number_len);
    if (ret != EOK) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[CallChangeIncomePro] copy tel ret:%d", ret);
        CatPhoneNumber(call);
    }
    phoneCall->telLen = call.number_len;

    (void)memcpy_s(&phoneCall->unitCalls, sizeof(PhoneHfpCallInfo), &call, sizeof(PhoneHfpCallInfo));

    // 发布电话呼入事件
    HfpIncomingEventPublish(TOPIC_EVENT_HFP_INCOMMING, phoneCall->unitCalls);
    // 通知手表电话呼入
    CallChangedCb(phoneCall);
}

// 通话状态变化--接听流程处理
static void CallChangeActivePro(const PhoneHfpCallInfo &call)
{
    PhoneCallInfo *phoneCall = FindCallInfoInCallList((const unsigned char*)call.number, call.number_len);
    if (phoneCall == nullptr) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[CallChangeActivePro] Active no this call");
        AddCurrCallToCallList(call);
        TransCallToMobilePhone();
        return;
    }

    if (phoneCall->lastStatus == HFP_HF_CALL_STATE_HELD && IsCompleteCallFlow()) { // 挂起状态切换到接听状态
        phoneCall->operate = OPERATE_WATCH_ACTIVE;
    } else if (phoneCall->lastStatus == HFP_HF_CALL_STATE_HELD && (!IsCompleteCallFlow())) {
        phoneCall->operate = OPERATE_PHONE_ACTIVE;
    } else if (phoneCall->operate == OPERATE_WATCH_ACTIVE) { // 手表接听
        // 不做操作
    } else {
        if (phoneCall->unitCalls.outgoing == 1 &&
            (phoneCall->lastStatus == HFP_HF_CALL_STATE_DIALING || phoneCall->lastStatus == HFP_HF_CALL_STATE_ALERTING)) {
            phoneCall->operate = OPERATE_PEER_ACTIVE; // 对方接听
        } else {
            // 手机接听
            phoneCall->operate = OPERATE_PHONE_ACTIVE;
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[CallChangeActivePro] phone active!");
        }
    }

    if (phoneCall->activeTime == 0 &&
        (phoneCall->lastStatus == HFP_HF_CALL_STATE_DIALING || phoneCall->lastStatus == HFP_HF_CALL_STATE_ALERTING ||
        phoneCall->lastStatus == HFP_HF_CALL_STATE_INCOMING || phoneCall->lastStatus == HFP_HF_CALL_STATE_WAITING)) {
        phoneCall->activeTime = time(nullptr);
    }

    phoneCall->lastStatus = call.state;
    (void)memcpy_s(&phoneCall->unitCalls, sizeof(PhoneHfpCallInfo), &call, sizeof(PhoneHfpCallInfo));
    if (phoneCall->operate == OPERATE_PHONE_ACTIVE) {
        // 发布电话接听事件，手表关闭来电通知
        HfpIncomingEventPublish(TOPIC_EVENT_HFP_PHONE_ACTIVE, phoneCall->unitCalls);
    } else if (phoneCall->operate == OPERATE_WATCH_ACTIVE && g_scoFlag == HFP_SCO_STATE_CONNECTED) {
        // 发布手表接听事件，通知切换接听界面
        HfpIncomingEventPublish(TOPIC_EVENT_HFP_WATCH_ACTIVE, phoneCall->unitCalls);
    }
    // 通知手表触发接听
    CallChangedCb(phoneCall);
}

// 通话状态变化--拒接/结束流程处理
static void CallChangeFinishPro(const PhoneHfpCallInfo &call)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[CallChangeFinishPro] start scoFlag:%d, CallNum:%d",
        g_scoFlag, g_currCallNum);
    if (g_currCallNum == 0) {
        return;
    }

    PhoneCallInfo *phoneCall = FindCallInfoInCallList((const unsigned char*)call.number, call.number_len);
    if (phoneCall == nullptr) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[CallChangeFinishPro] Finish no this call");
        return;
    }
    if (g_finishOperate == OPERATE_WATCH_FINISH) {
        phoneCall->operate = OPERATE_WATCH_FINISH;
    } else if (g_finishOperate == OPERATE_WATCH_REJECT) {
        phoneCall->operate = OPERATE_WATCH_REJECT;
    } else {
        phoneCall->operate = OPERATE_NON_WATCH_FINISH;
    }
    g_finishOperate = OPERATE_NON_WATCH_FINISH;
    g_currCallNum--;

    // 添加通话记录
    AddToCallLogPro(phoneCall);

    // 发布电话结束事件
    HfpIncomingEventPublish(TOPIC_EVENT_HFP_FINISHED, phoneCall->unitCalls);

    // 通知手表通话结束
    CallChangedCb(phoneCall);

    (void)memset_s(phoneCall, sizeof(PhoneCallInfo), 0, sizeof(PhoneCallInfo));
    phoneCall->lastStatus = HFP_HF_CALL_STATE_FINISHED;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[CallChangeFinishPro] end  scoFlag:%d, CallNum:%d",
                  g_scoFlag, g_currCallNum);
}

// 通话状态变化--挂起通话（当前只有手机挂起触发（并且手表不需感知），手表不支持挂起触发，对方挂起不感知）
static void CallChangeHeldPro(const PhoneHfpCallInfo &call)
{
    PhoneCallInfo *phoneCall = FindCallInfoInCallList((const unsigned char*)call.number, call.number_len);
    if (phoneCall == nullptr) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[CallChangeHeldPro] Held no this call");
        AddCurrCallToCallList(call);
        return;
    }

    // 手机挂起通话
    phoneCall->operate = OPERATE_PHONE_HOLD;
    if (phoneCall->lastStatus == HFP_HF_CALL_STATE_INCOMING || phoneCall->lastStatus == HFP_HF_CALL_STATE_WAITING) {
        phoneCall->activeTime = time(nullptr);
    }
    phoneCall->lastStatus = call.state;
    (void)memcpy_s(&phoneCall->unitCalls, sizeof(PhoneHfpCallInfo), &call, sizeof(PhoneHfpCallInfo));

    // 通知手表触发挂起
    CallChangedCb(phoneCall);
}

static void PlayWatchRing(void)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[PlayWatchRing] start");
}

static void CallAudioHandle(const PhoneHfpCallInfo &call)
{
    int state = call.state;

    if (state == HFP_HF_CALL_STATE_INCOMING || state == HFP_HF_CALL_STATE_DIALING ||
        state == HFP_HF_CALL_STATE_ALERTING) { // 电话呼入，支持播放手机铃声
        if (call.in_band_ring) {
            // 播放手机端铃声
            AudioDn();
        } else {
            if (g_scoFlag != HFP_SCO_STATE_CONNECTED) {
                // 播放手表铃声
                PlayWatchRing();
            }
        }
    } else if (state == HFP_HF_CALL_STATE_FINISHED) {
        return;
    } else if (g_scoFlag == HFP_SCO_STATE_CONNECTED) {
        AudioSetup();
    }
}

static void CatCallStateChangedInfo(const bd_addr_t &bdAddr, const PhoneHfpCallInfo &call)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "***************************CallStateChanged***************************");
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "scoFlag:%d, CallNum:%d, bdAddr: ", g_scoFlag, g_currCallNum);
    CatBdAddr(bdAddr);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "id:%d, state:%d", call.id, call.state);
    CatPhoneNumber(call);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "multi_party:%d, outgoing:%d, in_band_ring:%d, creation_time:%ld",
        call.multi_party, call.outgoing, call.in_band_ring, call.creation_time);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "***********************************************************************");
}

// 设置完整通话标志
static void SetCompleteCallFlag(const PhoneHfpCallInfo &call)
{
    if (call.state == HFP_HF_CALL_STATE_INCOMING) {
        g_completeCallFlag = true;
    } else if ((call.state == HFP_HF_CALL_STATE_DIALING || call.state == HFP_HF_CALL_STATE_ALERTING) &&
        (g_currCallNum == 1 || g_currCallNum == 0)) {
        g_completeCallFlag = true;
    } else if (call.state == HFP_HF_CALL_STATE_ACTIVE) {
        g_completeCallFlag = true;
    }
}

bool IsCompleteCallFlow(void)
{
    return g_completeCallFlag;
}

// 通话状态变化上报回调
static void CallStateChanged(const bd_addr_t &bdAddr, const PhoneHfpCallInfo &call)
{
    if (g_btConnState != PROFILE_STATE_CONNECTED) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "call is null or BT disconnected connStatus: 0x%X", g_btConnState);
        return;
    }

    CopyBdAddr(bdAddr);
    SetCompleteCallFlag(call);
    CatCallStateChangedInfo(bdAddr, call);
    CallAudioHandle(call);

    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "CallStateChanged state:%d", call.state);
    switch (call.state) {
        case HFP_HF_CALL_STATE_ACTIVE: // 正在通话
            CallChangeActivePro(call);
            break;
        case HFP_HF_CALL_STATE_HELD: // 挂起
            CallChangeHeldPro(call);
            break;
        case HFP_HF_CALL_STATE_DIALING: // 呼出，正在拨号
            PhoneServiceScreenCtr(1);
            GraphicService::GetInstance()->PostGraphicEvent([call]() {  CallChangeDialPro(call); });
            break;
        case HFP_HF_CALL_STATE_ALERTING:  // 呼出，对方响铃
            PhoneServiceScreenCtr(1);
            GraphicService::GetInstance()->PostGraphicEvent([call]() {  CallChangeAlertPro(call); });
            break;
        case HFP_HF_CALL_STATE_INCOMING:  // 呼入
            PhoneServiceScreenCtr(1);
            GraphicService::GetInstance()->PostGraphicEvent([call]() {  CallChangeIncomePro(call); });
            break;
        case HFP_HF_CALL_STATE_WAITING:  // 呼入等待
            PhoneServiceScreenCtr(1);
            GraphicService::GetInstance()->PostGraphicEvent([call]() {  CallChangeIncomePro(call); });
            break;
        case HFP_HF_CALL_STATE_FINISHED: // 拒接/结束电话
            CallChangeFinishPro(call);
            PhoneServiceScreenCtr(0);
            break;
        default:
            break;
    }

    // 规避某些型号手机在电话呼入时重复断开和连接SCO
    if (call.state == HFP_HF_CALL_STATE_FINISHED) {
        if (g_currCallNum == 0) {
            g_currCallState = HFP_HF_CALL_STATE_FINISHED;
        }
    } else {
        g_currCallState = call.state;
    }
}

static void CallStateChangedCb(const bd_addr_t *bdAddr, const hfp_hf_calls_info_t *call)
{
    PhoneHfpCallInfo callInfo = { 0 };
    (void)memcpy_s(&callInfo.bd_addr, sizeof(callInfo.bd_addr), &(call->bd_addr), sizeof(call->bd_addr));
    callInfo.id = call->id;
    callInfo.state = call->state;
    (void)memcpy_s(callInfo.number, MAX_PHONE_NUM, call->number, call->number_len);
    callInfo.number_len = call->number_len;
    callInfo.uuid = call->uuid;
    callInfo.multi_party = call->multi_party;
    callInfo.outgoing = call->outgoing;
    callInfo.in_band_ring = call->in_band_ring;
    callInfo.creation_time = call->creation_time;
    GraphicService::GetInstance()->PostGraphicEvent(std::bind(CallStateChanged, *bdAddr, callInfo));
}

static void BtHfpVolumeChanged(int volumeType, unsigned char hfpVolume)
{
    // mic
    if (volumeType == HFP_VOLUME_MIC) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "VolumeChanged Type:%d, hfpVolume:%d",
            volumeType, hfpVolume);
    }

    // speak
    if (volumeType == HFP_VOLUME_SPEAKER) {
        int32_t minVolume = PhoneAudioGetMinVolume();
        int32_t maxVolume = PhoneAudioGetMaxVolume();
        int32_t volume = hfpVolume * (maxVolume - minVolume) / CALL_VOLUME_MAX; // 转换
        volume = (volume == 0) ? ((maxVolume - minVolume) / CALL_VOLUME_MAX) : volume;
        bool ret = PhoneAudioSetVolume(volume);
        if (!ret) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "VolumeChanged Type:%d, ret:%d",
                          volumeType, ret);
        }
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "VolumeChanged Type:%d, min:%d, max:%d volume:%d ret:%d",
                      volumeType, minVolume, maxVolume, volume, ret);
    }
}

// 音量调节回调
static void VolumeChanged(const bd_addr_t *bdAddr, hfp_volume_type_t type, unsigned char volume)
{
    UNUSED(bdAddr);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[VolumeChanged] type:%d volume:%d", type, volume);
    GraphicService::GetInstance()->PostGraphicEvent(std::bind(BtHfpVolumeChanged, type, volume));
}

// 拨号失败回调
static void DialErrorCode(const bd_addr_t *bdAddr, int code)
{
    UNUSED(bdAddr);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[DialErrorCode] code:%d", code);
    PhoneServiceErrorCallback(ERROR_DIAL, code);
}

static void HfpBatteryChanged(const bd_addr_t *bdAddr, int batteryLevel)
{
    UNUSED(bdAddr);
    UNUSED(batteryLevel);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[HfpBatteryChanged] batteryLevel:%d", batteryLevel);
}

static void HfpSignalStrengthChanged(const bd_addr_t *bdAddr, int signalStrength)
{
    UNUSED(bdAddr);
    UNUSED(signalStrength);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[HfpSignalStrengthChanged] signalStrength:%d", signalStrength);
}

static void HfpRegistStatusChanged(const bd_addr_t *bdAddr, int status)
{
    UNUSED(bdAddr);
    UNUSED(status);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[HfpRegistStatusChanged] status:%d", status);
}

static void HfpRoamingStatusChanged(const bd_addr_t *bdAddr, int status)
{
    UNUSED(bdAddr);
    UNUSED(status);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[HfpRoamingStatusChanged] status:%d", status);
}

static void HfpOperatorSelection(const bd_addr_t *bdAddr, unsigned char *name, unsigned int len)
{
    UNUSED(bdAddr);
    UNUSED(name);
    UNUSED(len);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[HfpOperatorSelection] name: %s, len:%d", name, len);
}

static void HfpSubscriberNumber(const bd_addr_t *bdAddr, unsigned char *number, unsigned int len)
{
    UNUSED(bdAddr);
    UNUSED(number);
    UNUSED(len);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[HfpSubscriberNumber] number: %s, len:%d", number, len);
}

static void VoiceRecogStatusChanged(const bd_addr_t *bdAddr, int status)
{
    UNUSED(bdAddr);
    UNUSED(status);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[VoiceRecogStatusChanged] status:%d", status);
}

static void InbandRingtoneChanged(const bd_addr_t *bdAddr, int status)
{
    UNUSED(bdAddr);
    UNUSED(status);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[InbandRingtoneChanged] status:%d", status);
}

static void CodecChanged(const bd_addr_t *bdAddr, int codec)
{
    UNUSED(bdAddr);
    if (codec == HFP_HF_CODEC_ID_CVSD) {
        g_call_codec = PCM;
    } else if (codec == HFP_HF_CODEC_ID_MSBC) {
        g_call_codec = mSBC;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[CodecChanged] codec:%d, call_codec:%d", codec, g_call_codec);
}


static void RegisterHfpHfCallbacks(void)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[RegisterHfpHfCallbacks] start");
    int ret;
    hfp_hf_callbacks_t callbacks = {
        ConnStateChangedCb,
        ScoStateChangedCb,
        HfpBatteryChanged,
        HfpSignalStrengthChanged,
        HfpRegistStatusChanged,
        HfpRoamingStatusChanged,
        HfpOperatorSelection,
        HfpSubscriberNumber,
        VoiceRecogStatusChanged,
        InbandRingtoneChanged,
        VolumeChanged,
        CallStateChangedCb,
        DialErrorCode,
        CodecChanged,
    };

    ret = hfp_hf_register_callbacks(&callbacks);
    if (ret != 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[RegisterHfpHfCallbacks] ret:%d", ret);
    }
}

void PhoneServiceInit(void)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[PhoneServiceInit] start");
    g_scoFlag = HFP_SCO_STATE_DISCONNECTED;
    AudioInit();
    InitMutex();
    CallLogCreatDir();
    if (GetCallLogFileSize() == OHOS_FAILURE) {  // 如果不存在文件，创建新文件
        (void)ClearCallLogFile();
    }
    (void)memset_s(g_bdAddr.addr, BD_ADDR_LEN, 0, BD_ADDR_LEN);
    CallListInit();
    RegisterHfpHfCallbacks();
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[PhoneServiceInit] end");
}

// 手表主动拨打电话
int DialCall(unsigned char *number, unsigned char len)
{
    int ret;
    unsigned char *tel = nullptr;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[DialCall] start");
    if (g_btConnState != PROFILE_STATE_CONNECTED) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[DialCall] BT connStatus: 0x%X", g_btConnState);
        return OHOS_FAILURE;
    }

    // 有通话时，不允许拨出电话
    if (g_currCallNum > 0 || len > MAX_PHONE_NUM || number == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[DialCall] g_currCallNum:%d", g_currCallNum);
        return OHOS_FAILURE;
    }

    tel = (unsigned char *)malloc(len + 1);
    if (tel == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[DialCall] malloc tel error");
        return OHOS_FAILURE;
    }
    (void)memcpy_s(tel, len, number, len);
    *(tel + len) = '\0';
    ret = hfp_hf_start_dial(&g_bdAddr, tel, len);
    if (ret != ERRCODE_BT_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[DialCall] hfp_hf_start_dial ret %d", ret);
        free(tel);
        return OHOS_FAILURE;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[DialCall] success!");
    free(tel);
    return OHOS_SUCCESS;
}

// 手表主动接听
int AcceptIncomingCall(void)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[AcceptIncomingCall] start");
    int32_t ret;
    PhoneCallInfo *phoneCall = nullptr;
    PhoneCallInfo *tmp = g_callList;

    if (g_btConnState != PROFILE_STATE_CONNECTED) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[AcceptIncomingCall] BT connStatus: 0x%X", g_btConnState);
        return OHOS_FAILURE;
    }

    for (int32_t i = 0; i < MULTI_CALL_MAX_NUME; i++) {
        if (tmp->lastStatus == HFP_HF_CALL_STATE_INCOMING || tmp->lastStatus == HFP_HF_CALL_STATE_WAITING) {
            phoneCall = tmp;
            break;
        }
        tmp++;
    }

    if (phoneCall == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[AcceptIncomingCall] no incoming");
        return OHOS_FAILURE;
    }

    phoneCall->operate = OPERATE_WATCH_ACTIVE;

    if (g_currCallNum > 1) {
        ret = hfp_hf_accept_incoming_call(&g_bdAddr, HFP_HF_ACCEPT_CALL_ACTION_HOLD); // 正在通话，有人呼入，接听呼入电话，挂起当前通话
    } else {
        ret = hfp_hf_accept_incoming_call(&g_bdAddr, HFP_HF_ACCEPT_CALL_ACTION_NONE); // 无人通话，有人呼入，接听呼入电话
    }
    if (ret != ERRCODE_BT_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[AcceptIncomingCall] ret:%d", ret);
        phoneCall->operate = OPERATE_NO_OPE;
        return OHOS_FAILURE;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[AcceptIncomingCall] success!");
    return OHOS_SUCCESS;
}

// 手表拒接呼入来电，成功：0，失败：错误码
int RejectIncomingCall(void)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[RejectIncomingCall] start");
    PhoneCallInfo *phoneCall = nullptr;
    PhoneCallInfo *tmp = g_callList;

    if (g_btConnState != PROFILE_STATE_CONNECTED) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[RejectIncomingCall] BT connStatus: 0x%X", g_btConnState);
        return OHOS_FAILURE;
    }

    for (int32_t i = 0; i < MULTI_CALL_MAX_NUME; i++) {
        if (tmp->lastStatus == HFP_HF_CALL_STATE_INCOMING || tmp->lastStatus == HFP_HF_CALL_STATE_WAITING) {
            phoneCall = tmp;
        }
        tmp++;
    }

    if (phoneCall == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[RejectIncomingCall] no incoming");
        return OHOS_SUCCESS;
    }

    int32_t ret = hfp_hf_reject_incoming_call(&g_bdAddr);
    if (ret != ERRCODE_BT_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[RejectIncomingCall] ret:%d", ret);
        return OHOS_FAILURE;
    }
    g_finishOperate = OPERATE_WATCH_REJECT;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[RejectIncomingCall] success!");
    return OHOS_SUCCESS;
}

// 手表结束通话
int FinishCall(void)
{
    if (g_btConnState != PROFILE_STATE_CONNECTED) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[FinishCall] BT connStatus: 0x%X", g_btConnState);
        return OHOS_FAILURE;
    }
    int ret = hfp_hf_finish_call(&g_bdAddr);
    if (ret != ERRCODE_BT_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[FinishCall] ret:%d", ret);
        return OHOS_FAILURE;
    }
    g_finishOperate = OPERATE_WATCH_FINISH;
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "FinishCall");
    return OHOS_SUCCESS;
}

// 获取当前正在通话路数
int GetCurrCallNum(void)
{
    return g_currCallNum;
}

/**
 * @brief 获取当前通话路数和对应的通话信息
 *
 * @param callList 用来接收通话信息，需调用者分配空间，大小为MULTI_CALL_MAX_NUME * sizeof(PhoneCallInfo)
 * @param listNum callList中PhoneCallInfo个数，大小为MULTI_CALL_MAX_NUME
 * @param retNum 接收当前正在进行的通话路数
 * @return 返回值 成功返回0，失败返回错误码
 */
int GetCallInfoInCallList(PhoneCallInfo callList[], int listNum, int *retNum)
{
    PhoneCallInfo *tmp = g_callList;
    PhoneCallInfo *tmp2 = callList;

    if (callList == nullptr || listNum < MULTI_CALL_MAX_NUME) {
        *retNum = 0;
        return OHOS_FAILURE;
    }

    if (g_currCallNum == 0) {
        *retNum = 0;
        return OHOS_SUCCESS;
    }

    int32_t len = sizeof(PhoneCallInfo);
    for (int32_t i = 0; i < MULTI_CALL_MAX_NUME; i++) {
        if (tmp->lastStatus == HFP_HF_CALL_STATE_FINISHED) {
            tmp++;
        } else {
            (void)memcpy_s(tmp2, len, tmp, len);
            tmp++;
            tmp2++;
        }
    }

    *retNum = g_currCallNum;
    return OHOS_SUCCESS;
}

// 获取当前通话音量大小 (0 - 15)
// hfp音量范围 0-15, media 音量范围 0-100
unsigned char GetCallCurrVolume(void)
{
    int32_t volume = PhoneAudioGetVolume();
    int32_t minVolume = PhoneAudioGetMinVolume();
    int32_t maxVolume = PhoneAudioGetMaxVolume();
    unsigned char hfpVolume = (unsigned char)((volume * CALL_VOLUME_MAX) / (maxVolume - minVolume)); // 转换

    if (volume != 0 && hfpVolume == 0) {
        hfpVolume = 1;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[GetCallCurrVolume] volume:%d, minVolume:%d, maxVolume:%d, hfpVolume:%d",
        volume, minVolume, maxVolume, hfpVolume);
    return hfpVolume;
}

void SetSpeakerVolume(unsigned char hfpVolume)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[SetSpeakerVolume] start, hfpVolume:%d", hfpVolume);
    int32_t minVolume = PhoneAudioGetMinVolume();
    int32_t maxVolume = PhoneAudioGetMaxVolume();
    int32_t volume = hfpVolume * (maxVolume - minVolume) / CALL_VOLUME_MAX; // 转换
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[SetSpeakerVolume] minVolume:%d, maxVolume:%d, volume:%d",
        minVolume, maxVolume, volume);

    int32_t ret = PhoneAudioSetVolume(volume);
    if (!ret) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[SetSpeakerVolume] PhoneAudioSetVolume ret:%d", ret);
        return;
    }

    ret = hfp_hf_set_volume(&g_bdAddr, HFP_VOLUME_SPEAKER, hfpVolume);
    if (ret != ERRCODE_BT_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[SetSpeakerVolume] set_volume ret:%d, hfpVolume:%d",
            ret, hfpVolume);
        return;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[SetSpeakerVolume] success!");

    return;
}

// 设置通话speaker静音
bool SetSpeakerMute(bool isMute)
{
    return PhoneAudioSetSpeakerMute(isMute);
}

// 设置通话mic静音/取消静音
bool SetMicMute(bool isMute)
{
    return PhoneAudioSetMicMute(isMute);
}

// 获取SCO的连接状态
int GetScoConnectState(void)
{
    return g_scoFlag;
}

// 通话转移到手表
int TransCallToWatch(void)
{
    int ret;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[TransCallToWatch] start");

    if (g_btConnState != PROFILE_STATE_CONNECTED) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[TransCallToWatch] BT connStatus: 0x%X", g_btConnState);
        return OHOS_FAILURE;
    }

    ret = GetScoConnectState();
    if (ret == HFP_SCO_STATE_CONNECTED || ret == HFP_SCO_STATE_CONNECTING) {
        return OHOS_SUCCESS;
    } else {
        ret = hfp_hf_connect_sco(&g_bdAddr);
        if (ret != 0) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[TransCallToWatch] ret:%d", ret);
        }
        return ret;
    }
}

// 通话转移到手机
int TransCallToMobilePhone(void)
{
    int ret;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[TransCallToMobilePhone] start");

    if (g_btConnState != PROFILE_STATE_CONNECTED) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[TransCallToMobilePhone] BT connStatus: 0x%X", g_btConnState);
        return OHOS_FAILURE;
    }

    ret = GetScoConnectState();
    if (ret == HFP_SCO_STATE_DISCONNECTED || ret == HFP_SCO_STATE_DISCONNECTING) {
        return OHOS_SUCCESS;
    } else {
        ret = hfp_hf_disconnect_sco(&g_bdAddr);
        if (ret != 0) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[TransCallToMobilePhone] ret:%d", ret);
        }
        return ret;
    }
}

#ifdef __cplusplus
}
#endif

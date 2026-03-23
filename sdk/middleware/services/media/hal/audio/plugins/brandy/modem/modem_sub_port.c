/*
* Copyright (c) @CompanyNameMagicTag. 2023-2023. All rights reserved.
* Description: modem sub port
* Author: Media Software Group
* Create: 2023-07-05
*/

#include "modem_sub_port.h"

#include <unistd.h>
#include "securec.h"

#include "soc_uapi_adp.h"
#include "soc_uapi_ai.h"
#include "soc_uapi_sea.h"
#include "soc_uapi_adec.h"
#include "soc_uapi_aenc.h"
#include "soc_uapi_sound.h"

#include "plugin_common.h"
#include "media_hal_common.h"
#include "media_hal_thread_adapt.h"

#if defined(BRANDY_PRODUCT_EVB)
#include "product_evb_standard.h"
#elif defined(BRANDY_PRODUCT_EVB4)
#include "product_evb4_standard.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define MODULE_NAME "AudioModemSubPort"

#define MODEM_UPLINK_SND_ID UAPI_SND_1
#define MODEM_DOWNLINK_SND_ID UAPI_SND_0
#define MODEM_UPLINK_SOUND_PORT_NUM 1
#define MODEM_DOWNLINK_SOUND_PORT_NUM 1

#define MODEM_HEADSET_DOWNLINK_SOUND_PORT_NUM 1

#define MODEM_PORT_TRACK_NUM 2
#define SEA_ENABLE   "seaenable"

static enum AudioFormat g_supportFormats[] = {
    AUDIO_FORMAT_PCM_16_BIT
};

typedef struct {
    enum AudioPortPin portPin;
    enum SubPortType subPortType;
} PortPair;

static const PortPair g_portPairs[] = {
    { PIN_IN_MODEM, SUB_INPUT_PORT_MODEM },
    { PIN_OUT_MODEM, SUB_OUTPUT_PORT_MODEM },
    { PIN_IN_MODEM_HEADSET, SUB_INPUT_PORT_MODEM_HEADSET },
    { PIN_OUT_MODEM_HEADSET, SUB_OUTPUT_PORT_MODEM_HEADSET },
};

typedef enum {
    MODEM_UPLINK,
    MODEM_HEADSET_UPLINK,
    MODEM_DOWNLINK,
    MODEM_HEADSET_DOWNLINK
} WorkScene;

struct SubPortContext;

typedef struct {
    /* track context */
    AudioPortTrack common;
    int32_t trackId;
    WorkScene workScene;
    struct PortPluginAttr inputAttr;
    enum SubPortType subPortType;
    /* sdk uplink context */
    td_handle uplinkAiHandle;
    td_handle uplinkAdpHandle;
    td_handle uplinkSeaHandle;
    td_handle uplinkAencHandle;
    td_handle uplinkAdecHandle;
    td_handle uplinkTrackHandle;
    td_handle uplinkSoundHandle;
    /* sdk downlink context */
    td_handle downlinkAiHandle;
    td_handle downlinkAdpHandle;
    td_handle downlinkAencHandle;
    td_handle downlinkAdecHandle;
    td_handle downlinkTrackHandle;
    td_handle downlinkSoundHandle;
} SubPortTrackContext;

typedef struct SubPortContext {
    SubPortTrackContext *portTrack[MODEM_PORT_TRACK_NUM];
} SubPortContext;

typedef int32_t (*CreateTrackFunc)(SubPortTrackContext *portTrackCtx);
typedef int32_t (*DestroyTrackFunc)(const SubPortTrackContext *portTrackCtx);

typedef int32_t (*EnableTrackFunc)(SubPortTrackContext *portTrackCtx);
typedef int32_t (*DisableTrackFunc)(const SubPortTrackContext *portTrackCtx);

typedef int32_t (*TrackSetMuteFunc)(const SubPortTrackContext *portTrackCtx, bool mute);
typedef int32_t (*TrackGetMuteFunc)(const SubPortTrackContext *portTrackCtx, bool *mute);

typedef int32_t (*TrackSetVolumeFunc)(const SubPortTrackContext *portTrackCtx, float volume);
typedef int32_t (*TrackGetVolumeFunc)(const SubPortTrackContext *portTrackCtx, float *volume);

typedef int32_t (*PauseTrackFunc)(SubPortTrackContext *portTrackCtx);
typedef int32_t (*ResumeTrackFunc)(SubPortTrackContext *portTrackCtx);

typedef int32_t (*SendFrameFunc)(const SubPortTrackContext *portTrackCtx, const uint8_t *buffer, uint64_t requestBytes,
    uint64_t *replyBytes);

typedef int32_t (*InvokeFunc)(SubPortTrackContext *portTrackCtx, enum InvokeID invokeID,
    struct InvokeAttr *invokeAttr);

static int32_t CreateTrackInModemUplinkScene(SubPortTrackContext *portTrackCtx);
static int32_t DestroyTrackInModemUplinkScene(const SubPortTrackContext *portTrackCtx);
static int32_t EnableTrackInModemUplinkScene(SubPortTrackContext *portTrackCtx);
static int32_t DisableTrackInModemUplinkScene(const SubPortTrackContext *portTrackCtx);
static int32_t TrackSetMuteInModemUplinkScene(const SubPortTrackContext *portTrackCtx, bool mute);
static int32_t TrackGetMuteInModemUplinkScene(const SubPortTrackContext *portTrackCtx, bool *mute);
static int32_t TrackSetVolumeInModemUplinkScene(const SubPortTrackContext *portTrackCtx, float volume);
static int32_t TrackGetVolumeInModemUplinkScene(const SubPortTrackContext *portTrackCtx, float *volume);
static int32_t PauseTrackInModemUplinkScene(SubPortTrackContext *portTrackCtx);
static int32_t ResumeTrackInModemUplinkScene(SubPortTrackContext *portTrackCtx);
static int32_t SendFrameInModemUplinkScene(const SubPortTrackContext *portTrackCtx, const uint8_t *buffer,
    uint64_t requestBytes, uint64_t *replyBytes);
static int32_t InvokeInModemUplinkScene(SubPortTrackContext *portTrackCtx, enum InvokeID invokeID,
    struct InvokeAttr *invokeAttr);

static int32_t CreateTrackInModemDownlinkScene(SubPortTrackContext *portTrackCtx);
static int32_t DestroyTrackInModemDownlinkScene(const SubPortTrackContext *portTrackCtx);
static int32_t EnableTrackInModemDownlinkScene(SubPortTrackContext *portTrackCtx);
static int32_t DisableTrackInModemDownlinkScene(const SubPortTrackContext *portTrackCtx);
static int32_t TrackSetMuteInModemDownlinkScene(const SubPortTrackContext *portTrackCtx, bool mute);
static int32_t TrackGetMuteInModemDownlinkScene(const SubPortTrackContext *portTrackCtx, bool* mute);
static int32_t TrackSetVolumeInModemDownlinkScene(const SubPortTrackContext *portTrackCtx, float volume);
static int32_t TrackGetVolumeInModemDownlinkScene(const SubPortTrackContext *portTrackCtx, float* volume);
static int32_t PauseTrackInModemDownlinkScene(SubPortTrackContext *portTrackCtx);
static int32_t ResumeTrackInModemDownlinkScene(SubPortTrackContext *portTrackCtx);
static int32_t SendFrameInModemDownlinkScene(const SubPortTrackContext *portTrackCtx, const uint8_t *buffer,
    uint64_t requestBytes, uint64_t *replyBytes);
static int32_t InvokeInModemDownlinkScene(SubPortTrackContext *portTrackCtx, enum InvokeID invokeID,
    struct InvokeAttr* invokeAttr);

static int32_t CreateTrackInModemHeadsetUplinkScene(SubPortTrackContext *portTrackCtx);
static int32_t DestroyTrackInModemHeadsetUplinkScene(const SubPortTrackContext *portTrackCtx);
static int32_t EnableTrackInModemHeadsetUplinkScene(SubPortTrackContext *portTrackCtx);
static int32_t DisableTrackInModemHeadsetUplinkScene(const SubPortTrackContext *portTrackCtx);
static int32_t TrackSetMuteInModemHeadsetUplinkScene(const SubPortTrackContext *portTrackCtx, bool mute);
static int32_t TrackGetMuteInModemHeadsetUplinkScene(const SubPortTrackContext *portTrackCtx, bool *mute);
static int32_t TrackSetVolumeInModemHeadsetUplinkScene(const SubPortTrackContext *portTrackCtx, float volume);
static int32_t TrackGetVolumeInModemHeadsetUplinkScene(const SubPortTrackContext *portTrackCtx, float *volume);
static int32_t PauseTrackInModemHeadsetUplinkScene(SubPortTrackContext *portTrackCtx);
static int32_t ResumeTrackInModemHeadsetUplinkScene(SubPortTrackContext *portTrackCtx);
static int32_t SendFrameInModemHeadsetUplinkScene(const SubPortTrackContext *portTrackCtx, const uint8_t *buffer,
    uint64_t requestBytes, uint64_t *replyBytes);
static int32_t InvokeInModemHeadsetUplinkScene(SubPortTrackContext *portTrackCtx, enum InvokeID invokeID,
    struct InvokeAttr *invokeAttr);

static int32_t CreateTrackInModemHeadsetDownlinkScene(SubPortTrackContext *portTrackCtx);
static int32_t DestroyTrackInModemHeadsetDownlinkScene(const SubPortTrackContext *portTrackCtx);
static int32_t EnableTrackInModemHeadsetDownlinkScene(SubPortTrackContext *portTrackCtx);
static int32_t DisableTrackInModemHeadsetDownlinkScene(const SubPortTrackContext *portTrackCtx);
static int32_t TrackSetMuteInModemHeadsetDownlinkScene(const SubPortTrackContext *portTrackCtx, bool mute);
static int32_t TrackGetMuteInModemHeadsetDownlinkScene(const SubPortTrackContext *portTrackCtx, bool* mute);
static int32_t TrackSetVolumeInModemHeadsetDownlinkScene(const SubPortTrackContext *portTrackCtx, float volume);
static int32_t TrackGetVolumeInModemHeadsetDownlinkScene(const SubPortTrackContext *portTrackCtx, float* volume);
static int32_t PauseTrackInModemHeadsetDownlinkScene(SubPortTrackContext *portTrackCtx);
static int32_t ResumeTrackInModemHeadsetDownlinkScene(SubPortTrackContext *portTrackCtx);
static int32_t SendFrameInModemHeadsetDownlinkScene(const SubPortTrackContext *portTrackCtx, const uint8_t *buffer,
    uint64_t requestBytes, uint64_t *replyBytes);
static int32_t InvokeInModemHeadsetDownlinkScene(SubPortTrackContext *portTrackCtx, enum InvokeID invokeID,
    struct InvokeAttr* invokeAttr);

typedef struct {
    WorkScene workScene;
    CreateTrackFunc createTrack;
    DestroyTrackFunc destroyTrack;
    EnableTrackFunc enableTrack;
    DisableTrackFunc disableTrack;
    TrackSetMuteFunc trackSetMute;
    TrackGetMuteFunc trackGetMute;
    TrackSetVolumeFunc trackSetVolume;
    TrackGetVolumeFunc trackGetVolume;
    PauseTrackFunc pauseTrack;
    ResumeTrackFunc resumeTrack;
    SendFrameFunc sendFrame;
    InvokeFunc invoke;
} FuncsMap;

static FuncsMap g_funcsList[] = {
    {
        MODEM_UPLINK,
        CreateTrackInModemUplinkScene, DestroyTrackInModemUplinkScene,
        EnableTrackInModemUplinkScene, DisableTrackInModemUplinkScene,
        TrackSetMuteInModemUplinkScene, TrackGetMuteInModemUplinkScene,
        TrackSetVolumeInModemUplinkScene, TrackGetVolumeInModemUplinkScene,
        PauseTrackInModemUplinkScene, ResumeTrackInModemUplinkScene,
        SendFrameInModemUplinkScene, InvokeInModemUplinkScene
    },
    {
        MODEM_HEADSET_UPLINK,
        CreateTrackInModemHeadsetUplinkScene, DestroyTrackInModemHeadsetUplinkScene,
        EnableTrackInModemHeadsetUplinkScene, DisableTrackInModemHeadsetUplinkScene,
        TrackSetMuteInModemHeadsetUplinkScene, TrackGetMuteInModemHeadsetUplinkScene,
        TrackSetVolumeInModemHeadsetUplinkScene, TrackGetVolumeInModemHeadsetUplinkScene,
        PauseTrackInModemHeadsetUplinkScene, ResumeTrackInModemHeadsetUplinkScene,
        SendFrameInModemHeadsetUplinkScene, InvokeInModemHeadsetUplinkScene
    },
    {
        MODEM_DOWNLINK,
        CreateTrackInModemDownlinkScene, DestroyTrackInModemDownlinkScene,
        EnableTrackInModemDownlinkScene, DisableTrackInModemDownlinkScene,
        TrackSetMuteInModemDownlinkScene, TrackGetMuteInModemDownlinkScene,
        TrackSetVolumeInModemDownlinkScene, TrackGetVolumeInModemDownlinkScene,
        PauseTrackInModemDownlinkScene, ResumeTrackInModemDownlinkScene,
        SendFrameInModemDownlinkScene, InvokeInModemDownlinkScene
    },
    {
        MODEM_HEADSET_DOWNLINK,
        CreateTrackInModemHeadsetDownlinkScene, DestroyTrackInModemHeadsetDownlinkScene,
        EnableTrackInModemHeadsetDownlinkScene, DisableTrackInModemHeadsetDownlinkScene,
        TrackSetMuteInModemHeadsetDownlinkScene, TrackGetMuteInModemHeadsetDownlinkScene,
        TrackSetVolumeInModemHeadsetDownlinkScene, TrackGetVolumeInModemHeadsetDownlinkScene,
        PauseTrackInModemHeadsetDownlinkScene, ResumeTrackInModemHeadsetDownlinkScene,
        SendFrameInModemHeadsetDownlinkScene, InvokeInModemHeadsetDownlinkScene
    },
};

typedef struct {
    bool used;
    int32_t trackId;
} TrackStatus;

static TrackStatus g_trackStatusList[MODEM_PORT_TRACK_NUM];
static MediaHalMutexHandle g_trackResourceLock = NULL;
static bool g_trackMngInit = false;

/* sdk sound open/close reference count */
static int32_t g_subPortSndRefCnt = 0;
static MediaHalMutexHandle g_subPortLock = NULL;

static bool GetSubPortType(enum AudioPortPin portPin, enum SubPortType *subPortType)
{
    bool isFound = false;
    for (uint32_t i = 0; i < ARRAY_SIZE(g_portPairs); i++) {
        if (portPin == g_portPairs[i].portPin) {
            *subPortType = g_portPairs[i].subPortType;
            isFound = true;
            break;
        }
    }

    if (!isFound) {
        MEDIA_HAL_LOGE(MODULE_NAME, "modem port pin 0x%x is invalid", portPin);
    }
    return isFound;
}

static bool GetWorkScene(enum AudioPortPin portPin, WorkScene *workScene)
{
    switch (portPin) {
        case PIN_IN_MODEM:
            *workScene = MODEM_DOWNLINK;
            break;
        case PIN_OUT_MODEM:
            *workScene = MODEM_UPLINK;
            break;
        case PIN_IN_MODEM_HEADSET:
            *workScene = MODEM_HEADSET_DOWNLINK;
            break;
        case PIN_OUT_MODEM_HEADSET:
            *workScene = MODEM_HEADSET_UPLINK;
            break;
        default:
            MEDIA_HAL_LOGW(MODULE_NAME, "invalid pin 0x%x", portPin);
            return false;
    }

    return true;
}

int32_t AudioModemSubPortTrackRegCallback(AudioHandle trackHandle, TrackCallback callback, AudioHandle caller)
{
    MEDIA_HAL_UNUSED(trackHandle);
    MEDIA_HAL_UNUSED(callback);
    MEDIA_HAL_UNUSED(caller);
    return MEDIA_HAL_OK;
}

static int32_t SubPortTrackInit(const SubPortContext *portCtx, const struct PortPluginAttr *inputAttr,
    enum AudioPortPin portPin, SubPortTrackContext *portTrackCtx)
{
    MEDIA_HAL_UNUSED(portCtx);
    portTrackCtx->inputAttr = *inputAttr;

    portTrackCtx->inputAttr.bitWidth = AUDIO_BIT_WIDTH_16_BIT;

    if (!GetSubPortType(portPin, &portTrackCtx->subPortType)) {
        MEDIA_HAL_LOGE(MODULE_NAME, "modem GetSubPortType failed");
        return MEDIA_HAL_ERR;
    }

    if (!GetWorkScene(portPin, &portTrackCtx->workScene)) {
        MEDIA_HAL_LOGE(MODULE_NAME, "modem GetWorkScene failed");
        return MEDIA_HAL_ERR;
    }

    portTrackCtx->common.GetFrameCount = NULL;
    portTrackCtx->common.GetLatency = AudioModemSubPortTrackGetLatency;
    portTrackCtx->common.EnableTrack = AudioModemSubPortEnableTrack;
    portTrackCtx->common.PauseTrack = AudioModemSubPortPauseTrack;
    portTrackCtx->common.ResumeTrack = AudioModemSubPortResumeTrack;
    portTrackCtx->common.FlushTrack = NULL;
    portTrackCtx->common.DisableTrack = AudioModemSubPortDisableTrack;
    portTrackCtx->common.AcquireFrame = AudioModemSubPortAcquireFrame;
    portTrackCtx->common.SendFrame = AudioModemSubPortSendFrame;
    portTrackCtx->common.GetPosition = AudioModemSubPortGetPosition;
    portTrackCtx->common.SetMute = AudioModemSubPortTrackSetMute;
    portTrackCtx->common.GetMute = AudioModemSubPortTrackGetMute;
    portTrackCtx->common.SetVolume = AudioModemSubPortTrackSetVolume;
    portTrackCtx->common.GetVolume = AudioModemSubPortTrackGetVolume;
    portTrackCtx->common.Invoke = AudioModemSubPortInvoke;
    portTrackCtx->common.RegCallback = AudioModemSubPortTrackRegCallback;

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t SubPortTrackManagerInit(void)
{
    MediaHalInitStaticMutexLock(&g_trackResourceLock);
    MediaHalMutexLock(g_trackResourceLock);
    if (!g_trackMngInit) {
        for (int32_t i = 0; i < MODEM_PORT_TRACK_NUM; i++) {
            g_trackStatusList[i].trackId = i;
            g_trackStatusList[i].used = false;
        }
        g_trackMngInit = true;
    }
    MediaHalMutexUnLock(g_trackResourceLock);

    MEDIA_HAL_LOGD(MODULE_NAME, "modem SubPortTrackManagerInit success");
    return MEDIA_HAL_OK;
}

static int32_t SubPortTrackManagerDeInit(void)
{
    MediaHalInitStaticMutexLock(&g_trackResourceLock);
    MediaHalMutexLock(g_trackResourceLock);
    if (g_trackMngInit) {
        for (int32_t i = 0; i < MODEM_PORT_TRACK_NUM; i++) {
            if (g_trackStatusList[i].used) {
                MEDIA_HAL_LOGE(MODULE_NAME, "modem port track %d not free", g_trackStatusList[i].trackId);
            }
        }
        g_trackMngInit = false;
    }
    MediaHalMutexUnLock(g_trackResourceLock);

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t SubPortGetFreeTrack(void)
{
    MediaHalInitStaticMutexLock(&g_trackResourceLock);
    MediaHalMutexLock(g_trackResourceLock);
    if (!g_trackMngInit) {
        MediaHalMutexUnLock(g_trackResourceLock);
        MEDIA_HAL_LOGE(MODULE_NAME, "modem port track manager not init");
        return INVALID_ID;
    }

    for (int32_t i = 0; i < MODEM_PORT_TRACK_NUM; i++) {
        if (!g_trackStatusList[i].used) {
            g_trackStatusList[i].used = true;
            int32_t trackId = g_trackStatusList[i].trackId;
            MediaHalMutexUnLock(g_trackResourceLock);
            return trackId;
        }
    }
    MediaHalMutexUnLock(g_trackResourceLock);
    MEDIA_HAL_LOGE(MODULE_NAME, "modem port no free track");
    return INVALID_ID;
}

static int32_t SubPortFreeTrack(int32_t trackId)
{
    MediaHalInitStaticMutexLock(&g_trackResourceLock);
    MediaHalMutexLock(g_trackResourceLock);
    for (int32_t i = 0; i < MODEM_PORT_TRACK_NUM; i++) {
        if (g_trackStatusList[i].trackId == trackId) {
            if (!g_trackStatusList[i].used) {
                MediaHalMutexUnLock(g_trackResourceLock);
                MEDIA_HAL_LOGE(MODULE_NAME, "modem port track id: %d not used can't free", trackId);
                return MEDIA_HAL_ERR;
            }
            g_trackStatusList[i].used = false;
            MediaHalMutexUnLock(g_trackResourceLock);
            return MEDIA_HAL_OK;
        }
    }
    MediaHalMutexUnLock(g_trackResourceLock);
    MEDIA_HAL_LOGE(MODULE_NAME, "track id: %d is invalid", trackId);
    return MEDIA_HAL_ERR;
}

static int32_t AIDevOpen(SubPortTrackContext *portTrackCtx)
{
    if (portTrackCtx->workScene == MODEM_HEADSET_UPLINK) {
        MEDIA_HAL_LOGD(MODULE_NAME, "headset uplink no need ai device");
        return MEDIA_HAL_OK;
    }

    int32_t ret;
    uapi_ai_attr aiAttr = {};
    uapi_ai_port aiPort;
    bool uplink = (portTrackCtx->workScene == MODEM_UPLINK) ? true : false;
    if (uplink) {
        aiPort = BUILTIN_AI_PORT;
    } else {
        aiPort = EXTERNAL_AI_PORT;
    }
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_ai_get_default_attr(aiPort, &aiAttr));
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "get default ai attr failed");
    if (uplink) {
        GetBuiltinAIAttr(&aiAttr);
        aiAttr.ref_attr.enable = TD_TRUE;
        aiAttr.ref_attr.port = BUILTIN_SND_OUT_PORT;
        MEDIA_HAL_DOFUNC_TIME(ret = uapi_ai_open(&portTrackCtx->uplinkAiHandle, aiPort, &aiAttr));
    } else {
        GetExternAIAttr(&aiAttr);
        MEDIA_HAL_DOFUNC_TIME(ret = uapi_ai_open(&portTrackCtx->downlinkAiHandle, aiPort, &aiAttr));
    }
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "ai open failed");
        return ret;
    }

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t AIDevClose(const SubPortTrackContext *portTrackCtx)
{
    if (portTrackCtx->workScene == MODEM_HEADSET_UPLINK) {
        MEDIA_HAL_LOGD(MODULE_NAME, "headset uplink no need ai device");
        return MEDIA_HAL_OK;
    }

    int32_t ret;
    bool uplink = (portTrackCtx->workScene == MODEM_UPLINK) ? true : false;
    if (uplink) {
        MEDIA_HAL_DOFUNC_TIME(ret = uapi_ai_close(portTrackCtx->uplinkAiHandle));
    } else {
        MEDIA_HAL_DOFUNC_TIME(ret = uapi_ai_close(portTrackCtx->downlinkAiHandle));
    }
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "ai close failed");
        return ret;
    }

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static void GetHeadsetDownlinkAOAttr(uapi_snd_attr *sndAttr, SubPortTrackContext *portTrackCtx)
{
    sndAttr->port_num = MODEM_HEADSET_DOWNLINK_SOUND_PORT_NUM;
    sndAttr->port_attr[0].out_port = UAPI_SND_OUT_PORT_CAST0;
    sndAttr->bit_depth = UAPI_AUDIO_BIT_DEPTH_16;
    sndAttr->sample_rate = portTrackCtx->inputAttr.sampleRate;
    sndAttr->channels = portTrackCtx->inputAttr.channelCount;
}

static int32_t AODevOpen(SubPortTrackContext *portTrackCtx)
{
    int32_t ret;
    uapi_snd_attr sndAttr = {};
    uapi_snd sndId;
    bool uplink = (portTrackCtx->workScene == MODEM_UPLINK || portTrackCtx->workScene == MODEM_HEADSET_UPLINK) ?
        true : false;
    if (uplink) {
        sndId = MODEM_UPLINK_SND_ID;
    } else {
        sndId = MODEM_DOWNLINK_SND_ID;
    }
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_get_default_attr(sndId, &sndAttr));
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "get default sound attr failed");
        return ret;
    }
    if (uplink) {
        GetExternAOAttr(&sndAttr);
        MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_open(&portTrackCtx->uplinkSoundHandle, sndId, &sndAttr));
    } else {
        if (portTrackCtx->workScene == MODEM_HEADSET_DOWNLINK) {
            GetHeadsetDownlinkAOAttr(&sndAttr, portTrackCtx);
        } else {
            GetBuiltinAOAttr(&sndAttr);
        }
        MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_open(&portTrackCtx->downlinkSoundHandle, sndId, &sndAttr));
    }
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "sound open failed");

    // only speaker need set voip audio effect
    uapi_snd_aef_profile profile = UAPI_SND_AEF_PROFILE_NONE;
    if (uplink) {
        MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_set_aef_profile(portTrackCtx->uplinkSoundHandle, profile));
    } else {
        if (portTrackCtx->workScene == MODEM_HEADSET_DOWNLINK) {
            MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_set_aef_profile(portTrackCtx->downlinkSoundHandle, profile));
            ret = uapi_snd_set_track_mode(portTrackCtx->downlinkSoundHandle,
                sndAttr.port_attr[0].out_port, SND_TRACK_MODE_BT_CAT1);
        } else {
            profile = UAPI_SND_AEF_PROFILE_VOIP;
            MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_set_aef_profile(portTrackCtx->downlinkSoundHandle, profile));
            ret = uapi_snd_set_track_mode(portTrackCtx->downlinkSoundHandle,
                sndAttr.port_attr[0].out_port, SND_TRACK_MODE_LOCAL_CAT1);
        }
    }
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "set profile failed");

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t AODevClose(const SubPortTrackContext *portTrackCtx)
{
    int32_t ret;
    bool uplink = (portTrackCtx->workScene == MODEM_UPLINK || portTrackCtx->workScene == MODEM_HEADSET_UPLINK) ?
        true : false;
    if (uplink) {
        MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_close(portTrackCtx->uplinkSoundHandle));
    } else {
        MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_close(portTrackCtx->downlinkSoundHandle));
    }
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "sound close failed");
        return ret;
    }

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t AIAODevOpen(SubPortTrackContext *portTrackCtx)
{
    int32_t ret = AIDevOpen(portTrackCtx);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "ai device open failed");
        return ret;
    }

    ret = AODevOpen(portTrackCtx);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "ao device open failed");
        (void)AIDevClose(portTrackCtx);
        return ret;
    }

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t AIAODevClose(const SubPortTrackContext *portTrackCtx)
{
    int32_t ret = AIDevClose(portTrackCtx);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "ai close failed");

    ret = AODevClose(portTrackCtx);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "ao close failed");

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t CreateTrackInModemUplinkScene(SubPortTrackContext *portTrackCtx)
{
    int32_t ret = AudioSDKSeaOpen(&portTrackCtx->uplinkSeaHandle, portTrackCtx->inputAttr);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "sea open failed");

    uapi_snd_track_attr trackAttr = {};
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_get_track_default_attr(&trackAttr));
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "get default track attr failed");
        goto SEA_CLOSE;
    }

    MEDIA_HAL_DOFUNC_TIME(
        ret = uapi_snd_create_track(&portTrackCtx->uplinkTrackHandle, portTrackCtx->uplinkSoundHandle, &trackAttr));
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "create track failed");
        goto SEA_CLOSE;
    }

    ret = AudioSDKAttach(AUDIO_MOD_AI, portTrackCtx->uplinkAiHandle, portTrackCtx->uplinkSeaHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "ai attach sea failed");
        goto DESTROY_TRACK;
    }

    ret = AudioSDKAttach(AUDIO_MOD_SEA, portTrackCtx->uplinkSeaHandle, portTrackCtx->uplinkTrackHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "sea attach track failed");
        goto AI_DETACH_SEA;
    }

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
AI_DETACH_SEA:
    (void)AudioSDKDetach(AUDIO_MOD_AI, portTrackCtx->uplinkAiHandle, portTrackCtx->uplinkSeaHandle);
DESTROY_TRACK:
    (void)uapi_snd_destroy_track(portTrackCtx->uplinkTrackHandle);
SEA_CLOSE:
    (void)AudioSDKSeaClose(portTrackCtx->uplinkSeaHandle);
    return ret;
}

static int32_t DestroyTrackInModemUplinkScene(const SubPortTrackContext *portTrackCtx)
{
    int32_t ret = AudioSDKDetach(AUDIO_MOD_SEA, portTrackCtx->uplinkSeaHandle, portTrackCtx->uplinkTrackHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "sea detach track failed");

    ret = AudioSDKDetach(AUDIO_MOD_AI, portTrackCtx->uplinkAiHandle, portTrackCtx->uplinkSeaHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "ai detach sea failed");

    MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_destroy_track(portTrackCtx->uplinkTrackHandle));
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "destroy track failed");

    ret = AudioSDKSeaClose(portTrackCtx->uplinkSeaHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "sea close failed");

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t EnableTrackInModemUplinkScene(SubPortTrackContext *portTrackCtx)
{
    int32_t ret;
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_ai_start(portTrackCtx->uplinkAiHandle));
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "ai start failed");

    ret = uapi_sea_start(portTrackCtx->uplinkSeaHandle);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "sea start failed");

    ret = uapi_snd_track_start(portTrackCtx->uplinkTrackHandle, TD_NULL);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "track start failed");

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t DisableTrackInModemUplinkScene(const SubPortTrackContext *portTrackCtx)
{
    int32_t ret;
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_ai_stop(portTrackCtx->uplinkAiHandle));
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "ai stop failed");

    ret = uapi_sea_stop(portTrackCtx->uplinkSeaHandle);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "sea stop failed");

    ret = uapi_snd_track_stop(portTrackCtx->uplinkTrackHandle);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "track stop failed");

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t TrackSetMuteInModemUplinkScene(const SubPortTrackContext *portTrackCtx, bool mute)
{
    int32_t ret;
    td_bool isMute = mute ? TD_TRUE : TD_FALSE;
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_ai_set_mute(portTrackCtx->uplinkAiHandle, isMute));
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "set ai mute failed");

    MEDIA_HAL_LOGI(MODULE_NAME, "set mute %d success", mute);
    return MEDIA_HAL_OK;
}

static int32_t TrackGetMuteInModemUplinkScene(const SubPortTrackContext *portTrackCtx, bool *mute)
{
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(mute);
    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

static int32_t TrackSetVolumeInModemUplinkScene(const SubPortTrackContext *portTrackCtx, float volume)
{
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(volume);
    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

static int32_t TrackGetVolumeInModemUplinkScene(const SubPortTrackContext *portTrackCtx, float *volume)
{
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(volume);
    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

static int32_t PauseTrackInModemUplinkScene(SubPortTrackContext *portTrackCtx)
{
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_LOGD(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

static int32_t ResumeTrackInModemUplinkScene(SubPortTrackContext *portTrackCtx)
{
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

static int32_t SendFrameInModemUplinkScene(const SubPortTrackContext *portTrackCtx, const uint8_t *buffer,
    uint64_t requestBytes, uint64_t *replyBytes)
{
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(buffer);
    MEDIA_HAL_UNUSED(requestBytes);
    MEDIA_HAL_UNUSED(replyBytes);
    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

static int32_t ModemSetSeaEnable(AudioHandle trackHandle, char *value, uint32_t valueLen)
{
    SubPortTrackContext *portTrackInCtx = (SubPortTrackContext *)trackHandle;
    CHK_NULL_RETURN(value, MEDIA_HAL_INVALID_PARAM, "input param value is null");
    return SetSeaEnable(portTrackInCtx->uplinkSeaHandle, value, valueLen);
}

static struct AudioPortInvoke g_invokeInModemUplinkList[] = {
    {SEA_ENABLE, ModemSetSeaEnable, true},
};

static int32_t InvokeInModemUplinkScene(SubPortTrackContext *portTrackCtx, enum InvokeID invokeID,
    struct InvokeAttr *invokeAttr)
{
    int32_t ret = 0;
    switch (invokeID) {
        case INVOKE_ID_SET_EXTRA_PARAMS:
        {
            size_t size = sizeof(g_invokeInModemUplinkList) / sizeof(struct AudioPortInvoke);
            ret = ExtraParamsInvoke(portTrackCtx, invokeAttr, g_invokeInModemUplinkList, (uint32_t)size);
            CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "extra param invoke failed!");
            break;
        }
        default:
            MEDIA_HAL_LOGW(MODULE_NAME, "invalid invoke id: %d", invokeID);
            return MEDIA_HAL_INVALID_PARAM;
    }
    return MEDIA_HAL_OK;
}

static int32_t CreateTrackInModemHeadsetUplinkScene(SubPortTrackContext *portTrackCtx)
{
    int32_t ret = AudioSDKAdpOpen(&portTrackCtx->uplinkAdpHandle);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uplink adp open failed");

    ret = AudioSDKAdecOpen(&portTrackCtx->uplinkAdecHandle, portTrackCtx->inputAttr);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "adec open failed");
        goto ADP_CLOSE;
    }

    uapi_snd_track_attr trackAttr = {};
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_get_track_default_attr(&trackAttr));
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "get default track attr failed");
        goto ADEC_CLOSE;
    }

    MEDIA_HAL_DOFUNC_TIME(
        ret = uapi_snd_create_track(&portTrackCtx->uplinkTrackHandle, portTrackCtx->uplinkSoundHandle, &trackAttr));
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "create track failed");
        goto ADEC_CLOSE;
    }

    ret = AudioSDKAttach(AUDIO_MOD_ADP, portTrackCtx->uplinkAdpHandle, portTrackCtx->uplinkAdecHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "adp atach adec failed");
        goto DESTROY_TRACK;
    }

    ret = AudioSDKAttach(AUDIO_MOD_ADEC, portTrackCtx->uplinkAdecHandle, portTrackCtx->uplinkTrackHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "adec attach track failed");
        goto ADP_DETACH_ADEC;
    }

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;

ADP_DETACH_ADEC:
    (void)AudioSDKDetach(AUDIO_MOD_ADP, portTrackCtx->uplinkAdpHandle, portTrackCtx->uplinkAdecHandle);
DESTROY_TRACK:
    (void)uapi_snd_destroy_track(portTrackCtx->uplinkTrackHandle);
ADEC_CLOSE:
    (void)AudioSDKAdecClose(portTrackCtx->uplinkAdecHandle);
ADP_CLOSE:
    (void)AudioSDKAdpClose(portTrackCtx->uplinkAdpHandle);
    return ret;
}

static int32_t DestroyTrackInModemHeadsetUplinkScene(const SubPortTrackContext *portTrackCtx)
{
    int32_t ret = AudioSDKDetach(AUDIO_MOD_ADEC, portTrackCtx->uplinkAdecHandle, portTrackCtx->uplinkTrackHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "adec detach track failed");

    ret = AudioSDKDetach(AUDIO_MOD_ADP, portTrackCtx->uplinkAdpHandle, portTrackCtx->uplinkAdecHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "adp detach adec failed");

    MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_destroy_track(portTrackCtx->uplinkTrackHandle));
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "destroy track failed");

    ret = AudioSDKAdecClose(portTrackCtx->uplinkAdecHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "adec close failed");

    ret = AudioSDKAdpClose(portTrackCtx->uplinkAdpHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "adp close failed");

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t EnableTrackInModemHeadsetUplinkScene(SubPortTrackContext *portTrackCtx)
{
    int32_t ret;
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_track_start(portTrackCtx->uplinkTrackHandle, TD_NULL));
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "track start failed");

    MEDIA_HAL_DOFUNC_TIME(ret = uapi_adec_start(portTrackCtx->uplinkAdecHandle));
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "adec start failed");
        goto TRACK_STOP;
    }

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;

TRACK_STOP:
    (void)uapi_snd_track_stop(portTrackCtx->uplinkTrackHandle);
    return ret;
}

static int32_t DisableTrackInModemHeadsetUplinkScene(const SubPortTrackContext *portTrackCtx)
{
    int32_t ret;
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_track_stop(portTrackCtx->uplinkTrackHandle));
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "track stop failed");

    MEDIA_HAL_DOFUNC_TIME(ret = uapi_adec_stop(portTrackCtx->uplinkAdecHandle));
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "adec stop failed");

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t TrackSetMuteInModemHeadsetUplinkScene(const SubPortTrackContext *portTrackCtx, bool mute)
{
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(mute);
    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

static int32_t TrackGetMuteInModemHeadsetUplinkScene(const SubPortTrackContext *portTrackCtx, bool *mute)
{
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(mute);
    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

static int32_t TrackSetVolumeInModemHeadsetUplinkScene(const SubPortTrackContext *portTrackCtx, float volume)
{
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(volume);
    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

static int32_t TrackGetVolumeInModemHeadsetUplinkScene(const SubPortTrackContext *portTrackCtx, float *volume)
{
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(volume);
    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

static int32_t PauseTrackInModemHeadsetUplinkScene(SubPortTrackContext *portTrackCtx)
{
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_LOGD(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

static int32_t ResumeTrackInModemHeadsetUplinkScene(SubPortTrackContext *portTrackCtx)
{
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

static int32_t SendFrameInModemHeadsetUplinkScene(const SubPortTrackContext *portTrackCtx, const uint8_t *buffer,
    uint64_t requestBytes, uint64_t *replyBytes)
{
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(buffer);
    MEDIA_HAL_UNUSED(requestBytes);
    MEDIA_HAL_UNUSED(replyBytes);
    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

static int32_t InvokeInModemHeadsetUplinkScene(SubPortTrackContext *portTrackCtx, enum InvokeID invokeID,
    struct InvokeAttr *invokeAttr)
{
    MEDIA_HAL_UNUSED(portTrackCtx);

    switch (invokeID) {
        case INVOKE_ID_GET_CHANNEL_ID:
            *((uint32_t *)invokeAttr->request) = portTrackCtx->uplinkAdpHandle;
            invokeAttr->replyBytes = (uint64_t)sizeof(uint32_t);
            MEDIA_HAL_LOGI(MODULE_NAME, "Uplink data channel id: 0x%x success", *((uint32_t *)invokeAttr->request));
            break;
        default:
            MEDIA_HAL_LOGW(MODULE_NAME, "Uplink invalid invoke id: %d", invokeID);
            return MEDIA_HAL_INVALID_PARAM;
    }

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t ConvertAencAttr(struct PortPluginAttr inputAttr, uapi_aenc_attr *aencAttr)
{
    uapi_acodec_id aencCodecId;
    if (!AudioConvertAudioFormatToCodecId(inputAttr.audioFormat, &aencCodecId)) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioConvertAudioFormatToCodecId failed");
        return MEDIA_HAL_INVALID_PARAM;
    }

    aencAttr->codec_id = aencCodecId;
    aencAttr->param.interleaved = true;
    aencAttr->param.channels = inputAttr.channelCount;
    aencAttr->param.bit_depth = inputAttr.bitWidth;
    aencAttr->param.sample_rate = inputAttr.sampleRate;
    aencAttr->param.samples_per_frame = ((uint32_t)inputAttr.sampleRate) / SAMPLERATE_TO_PTNUMPERFRM;
    aencAttr->param.private_data = NULL;
    aencAttr->param.private_data_size = 0;

    MEDIA_HAL_LOGI(MODULE_NAME, "audioFormat: 0x%x, sampleRate: %d, channelCount: %u, bitWidth: %u",
        inputAttr.audioFormat, inputAttr.sampleRate, inputAttr.channelCount, inputAttr.bitWidth);

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t CreateTrackInModemHeadsetDownlinkScene(SubPortTrackContext *portTrackCtx)
{
    uapi_aenc_attr aencAttr = {};
    int32_t ret = ConvertAencAttr(portTrackCtx->inputAttr, &aencAttr);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "convert aenc attr failed");

    uapi_snd_track_attr trackAttr = {};
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_get_track_default_attr(&trackAttr));
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "get track default attr failed");

    ret = AudioSDKAdpOpen(&portTrackCtx->downlinkAdpHandle);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "downlink adp open failed");

    if (AudioSDKAencOpen(&portTrackCtx->downlinkAencHandle, aencAttr) != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "aenc open failed");
        goto ADP_CLOSE;
    }

    MEDIA_HAL_DOFUNC_TIME(
        ret = uapi_snd_create_track(&portTrackCtx->downlinkTrackHandle, portTrackCtx->downlinkSoundHandle, &trackAttr));
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "create track failed");
        goto AENC_CLOSE;
    }

    ret = AudioSDKAttach(AUDIO_MOD_AI, portTrackCtx->downlinkAiHandle, portTrackCtx->downlinkTrackHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "ai attach track failed");
        goto DESTROY_TRACK;
    }

    ret = AudioSDKAttach(AUDIO_MOD_SOUND, portTrackCtx->downlinkSoundHandle, portTrackCtx->downlinkAencHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "sound attach aenc failed");
        goto AI_DETACH_TRACK;
    }

    ret = AudioSDKAttach(AUDIO_MOD_AENC, portTrackCtx->downlinkAencHandle, portTrackCtx->downlinkAdpHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "aenc attach adp failed");
        goto SOUND_DETACH_AENC;
    }

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;

SOUND_DETACH_AENC:
    (void)AudioSDKDetach(AUDIO_MOD_SOUND, portTrackCtx->downlinkSoundHandle, portTrackCtx->downlinkAencHandle);
AI_DETACH_TRACK:
    (void)AudioSDKDetach(AUDIO_MOD_AI, portTrackCtx->downlinkAiHandle, portTrackCtx->downlinkTrackHandle);
DESTROY_TRACK:
    (void)uapi_snd_destroy_track(portTrackCtx->uplinkTrackHandle);
AENC_CLOSE:
    (void)AudioSDKAencClose(portTrackCtx->downlinkAencHandle);
ADP_CLOSE:
    (void)AudioSDKAdpClose(portTrackCtx->downlinkAdpHandle);
    return ret;
}

static int32_t DestroyTrackInModemHeadsetDownlinkScene(const SubPortTrackContext *portTrackCtx)
{
    int32_t ret = AudioSDKDetach(AUDIO_MOD_AENC, portTrackCtx->downlinkAencHandle, portTrackCtx->downlinkAdpHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "aenc detach adp failed");

    ret = AudioSDKDetach(AUDIO_MOD_SOUND, portTrackCtx->downlinkSoundHandle, portTrackCtx->downlinkAencHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "sound detach aenc failed");

    ret = AudioSDKDetach(AUDIO_MOD_AI, portTrackCtx->downlinkAiHandle, portTrackCtx->downlinkTrackHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "ai detach adec track");

    MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_destroy_track(portTrackCtx->downlinkTrackHandle));
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "destroy track failed");

    ret = AudioSDKAencClose(portTrackCtx->downlinkAencHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "aenc close failed");

    ret = AudioSDKAdpClose(portTrackCtx->downlinkAdpHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "adp close failed");

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t EnableTrackInModemHeadsetDownlinkScene(SubPortTrackContext *portTrackCtx)
{
    int32_t ret;
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_ai_start(portTrackCtx->downlinkAiHandle));
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "ai start failed");

    MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_track_start(portTrackCtx->downlinkTrackHandle, TD_NULL));
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGI(MODULE_NAME, "track start failed");
        goto AI_STOP;
    }

    MEDIA_HAL_DOFUNC_TIME(ret = uapi_aenc_start(portTrackCtx->downlinkAencHandle));
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGI(MODULE_NAME, "aenc start failed");
        goto TRACK_STOP;
    }

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;

AI_STOP:
    (void)uapi_ai_stop(portTrackCtx->downlinkAiHandle);
TRACK_STOP:
    (void)uapi_snd_track_stop(portTrackCtx->downlinkTrackHandle);
    return ret;
}

static int32_t DisableTrackInModemHeadsetDownlinkScene(const SubPortTrackContext *portTrackCtx)
{
    int32_t ret;
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_ai_stop(portTrackCtx->downlinkAiHandle));
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "ai stop failed");

    MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_track_stop(portTrackCtx->downlinkTrackHandle));
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "track stop failed");

    MEDIA_HAL_DOFUNC_TIME(ret = uapi_aenc_stop(portTrackCtx->downlinkAencHandle));
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "aenc stop failed");

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t TrackSetMuteInModemHeadsetDownlinkScene(const SubPortTrackContext *portTrackCtx, bool mute)
{
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(mute);
    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

static int32_t TrackGetMuteInModemHeadsetDownlinkScene(const SubPortTrackContext *portTrackCtx, bool *mute)
{
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(mute);
    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

static int32_t TrackSetVolumeInModemHeadsetDownlinkScene(const SubPortTrackContext *portTrackCtx, float volume)
{
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(volume);
    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

static int32_t TrackGetVolumeInModemHeadsetDownlinkScene(const SubPortTrackContext *portTrackCtx, float *volume)
{
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(volume);
    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

static int32_t PauseTrackInModemHeadsetDownlinkScene(SubPortTrackContext *portTrackCtx)
{
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_LOGD(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

static int32_t ResumeTrackInModemHeadsetDownlinkScene(SubPortTrackContext *portTrackCtx)
{
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

static int32_t SendFrameInModemHeadsetDownlinkScene(const SubPortTrackContext *portTrackCtx, const uint8_t *buffer,
    uint64_t requestBytes, uint64_t *replyBytes)
{
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(buffer);
    MEDIA_HAL_UNUSED(requestBytes);
    MEDIA_HAL_UNUSED(replyBytes);
    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

static int32_t InvokeInModemHeadsetDownlinkScene(SubPortTrackContext *portTrackCtx, enum InvokeID invokeID,
    struct InvokeAttr *invokeAttr)
{
    switch (invokeID) {
        case INVOKE_ID_GET_CHANNEL_ID:
            *((uint32_t *)invokeAttr->request) = portTrackCtx->downlinkAdpHandle;
            invokeAttr->replyBytes = (uint64_t)sizeof(uint32_t);
            MEDIA_HAL_LOGI(MODULE_NAME, "Downlink data channel id: 0x%x success", *((uint32_t *)invokeAttr->request));
            break;
        default:
            MEDIA_HAL_LOGW(MODULE_NAME, "Downlink invalid invoke id: %d", invokeID);
            return MEDIA_HAL_INVALID_PARAM;
    }

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t CreateTrackInModemDownlinkScene(SubPortTrackContext *portTrackCtx)
{
    int32_t ret;
    uapi_snd_track_attr trackAttr = {};
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_get_track_default_attr(&trackAttr));
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "get default track attr failed");
    MEDIA_HAL_DOFUNC_TIME(
        ret = uapi_snd_create_track(&portTrackCtx->downlinkTrackHandle, portTrackCtx->downlinkSoundHandle, &trackAttr));
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "create track failed");

    ret = AudioSDKAttach(AUDIO_MOD_AI, portTrackCtx->downlinkAiHandle, portTrackCtx->downlinkTrackHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "ai attach track failed");
        (void)uapi_snd_destroy_track(portTrackCtx->downlinkTrackHandle);
        return ret;
    }

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t DestroyTrackInModemDownlinkScene(const SubPortTrackContext *portTrackCtx)
{
    int32_t ret = AudioSDKDetach(AUDIO_MOD_AI, portTrackCtx->downlinkAiHandle, portTrackCtx->downlinkTrackHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "ai detach track failed");

    MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_destroy_track(portTrackCtx->downlinkTrackHandle));
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "destroy track failed");

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t EnableTrackInModemDownlinkScene(SubPortTrackContext *portTrackCtx)
{
    int32_t ret;
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_ai_start(portTrackCtx->downlinkAiHandle));
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "ai start failed");

    ret = uapi_snd_track_start(portTrackCtx->downlinkTrackHandle, TD_NULL);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "track start failed");

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t DisableTrackInModemDownlinkScene(const SubPortTrackContext *portTrackCtx)
{
    int32_t ret;
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_ai_stop(portTrackCtx->downlinkAiHandle));
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "ai stop failed");

    ret = uapi_snd_track_stop(portTrackCtx->downlinkTrackHandle);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "track stop failed");

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t TrackSetMuteInModemDownlinkScene(const SubPortTrackContext *portTrackCtx, bool mute)
{
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(mute);
    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

static int32_t TrackGetMuteInModemDownlinkScene(const SubPortTrackContext *portTrackCtx, bool *mute)
{
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(mute);
    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

static int32_t TrackSetVolumeInModemDownlinkScene(const SubPortTrackContext *portTrackCtx, float volume)
{
    int32_t ret;
    int32_t tmpVolume;
    uapi_snd_gain sndGain = {};
    VolumePercentToDb(volume, &tmpVolume);
    sndGain.integer = tmpVolume;
    sndGain.decimal = 0;
    uapi_audio_toggle_config volumeToggleConfig = { 0 };
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_set_track_volume(portTrackCtx->downlinkTrackHandle, &sndGain,
        &volumeToggleConfig));
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_snd_set_track_volume failed");
    return MEDIA_HAL_OK;
}

static int32_t TrackGetVolumeInModemDownlinkScene(const SubPortTrackContext *portTrackCtx, float *volume)
{
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(volume);
    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

static int32_t PauseTrackInModemDownlinkScene(SubPortTrackContext *portTrackCtx)
{
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_LOGD(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

static int32_t ResumeTrackInModemDownlinkScene(SubPortTrackContext *portTrackCtx)
{
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

static int32_t SendFrameInModemDownlinkScene(const SubPortTrackContext *portTrackCtx, const uint8_t *buffer,
    uint64_t requestBytes, uint64_t *replyBytes)
{
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(buffer);
    MEDIA_HAL_UNUSED(requestBytes);
    MEDIA_HAL_UNUSED(replyBytes);
    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

static int32_t InvokeInModemDownlinkScene(SubPortTrackContext *portTrackCtx, enum InvokeID invokeID,
    struct InvokeAttr *invokeAttr)
{
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(invokeID);
    MEDIA_HAL_UNUSED(invokeAttr);
    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}


int32_t AudioModemSubPortTrackGetLatency(AudioHandle trackHandle, uint32_t *latencyMs)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;

    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(latencyMs);

    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

int32_t AudioModemSubPortEnableTrack(AudioHandle trackHandle)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;

    int32_t ret = g_funcsList[portTrackCtx->workScene].enableTrack(portTrackCtx);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "work scene %d enable track failed", portTrackCtx->workScene);
        return MEDIA_HAL_ERR;
    }

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

int32_t AudioModemSubPortDisableTrack(AudioHandle trackHandle)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;

    int32_t ret = g_funcsList[portTrackCtx->workScene].disableTrack(portTrackCtx);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "work scene %d disable track failed", portTrackCtx->workScene);
    }

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

int32_t AudioModemSubPortPauseTrack(AudioHandle trackHandle)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;

    MEDIA_HAL_LOGD(MODULE_NAME, "modem pause not support");
    return MEDIA_HAL_OK;
}

int32_t AudioModemSubPortResumeTrack(AudioHandle trackHandle)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;

    MEDIA_HAL_LOGD(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

int32_t AudioModemSubPortAcquireFrame(AudioHandle trackHandle, uint8_t *buffer, uint64_t requestBytes,
    uint64_t *replyBytes)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;

    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(buffer);
    MEDIA_HAL_UNUSED(requestBytes);
    MEDIA_HAL_UNUSED(replyBytes);

    MEDIA_HAL_LOGD(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

int32_t AudioModemSubPortGetPosition(AudioHandle trackHandle, uint64_t *frames, struct AudioTimeStamp *time)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;

    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(frames);
    MEDIA_HAL_UNUSED(time);

    MEDIA_HAL_LOGD(MODULE_NAME, "modem getposition not support");
    return MEDIA_HAL_OK;
}

int32_t AudioModemSubPortSendFrame(AudioHandle trackHandle, const uint8_t *buffer, uint64_t requestBytes,
    uint64_t *replyBytes)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;

    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(buffer);
    MEDIA_HAL_UNUSED(requestBytes);
    MEDIA_HAL_UNUSED(replyBytes);

    MEDIA_HAL_LOGD(MODULE_NAME, "modem sendFream not support");
    return MEDIA_HAL_OK;
}

int32_t AudioModemSubPortTrackSetMute(AudioHandle trackHandle, bool mute)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;

    int32_t ret = g_funcsList[portTrackCtx->workScene].trackSetMute(portTrackCtx, mute);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "work scene %d track set mute %d failed", portTrackCtx->workScene, mute);
        return MEDIA_HAL_ERR;
    }
    return MEDIA_HAL_OK;
}

int32_t AudioModemSubPortTrackGetMute(AudioHandle trackHandle, bool *mute)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;

    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(mute);

    MEDIA_HAL_LOGI(MODULE_NAME, "modem get mute not support");
    return MEDIA_HAL_OK;
}

int32_t AudioModemSubPortTrackSetVolume(AudioHandle trackHandle, float volume)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
    int32_t ret = g_funcsList[portTrackCtx->workScene].trackSetVolume(portTrackCtx, volume);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "work scene %d set volume failed", portTrackCtx->workScene);
        return ret;
    }
    return MEDIA_HAL_OK;
}

int32_t AudioModemSubPortTrackGetVolume(AudioHandle trackHandle, float *volume)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;

    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(volume);

    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

int32_t AudioModemSubPortInvoke(AudioHandle trackHandle, enum InvokeID invokeID, struct InvokeAttr *invokeAttr)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;

    int32_t ret = g_funcsList[portTrackCtx->workScene].invoke(trackHandle, invokeID, invokeAttr);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "work scene %d invoke track failed", portTrackCtx->workScene);
        return MEDIA_HAL_ERR;
    }

    MEDIA_HAL_LOGD(MODULE_NAME, "success");

    return MEDIA_HAL_OK;
}

int32_t AudioModemSubPortCreate(AudioHandle *portHandle)
{
    SubPortContext *portCtx = (SubPortContext *)malloc(sizeof(SubPortContext));
    if (portCtx == NULL) {
        MEDIA_HAL_LOGI(MODULE_NAME, "malloc failed");
        return MEDIA_HAL_NO_MEM;
    }

    int32_t ret = MEDIA_HAL_OK;
    if (memset_s(portCtx, sizeof(SubPortContext), 0, sizeof(SubPortContext)) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "memset_s failed");
        ret = MEDIA_HAL_ERR;
        goto FREE;
    }

    ret = SubPortTrackManagerInit();
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "SubPortTrackManagerInit failed");
        goto FREE;
    }

    *portHandle = portCtx;
    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return ret;

FREE:
    free(portCtx);
    return ret;
}

int32_t AudioModemSubPortDestroy(AudioHandle portHandle)
{
    SubPortContext *portCtx = (SubPortContext *)portHandle;

    int32_t ret = SubPortTrackManagerDeInit();
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "SubPortTrackManagerDeInit failed");

    free(portCtx);

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

int32_t AudioModemSubPortGetCapability(const struct AudioPort *port, struct AudioPortCapability *capability)
{
    capability->formats = g_supportFormats;
    MEDIA_HAL_UNUSED(port);
    MEDIA_HAL_UNUSED(capability);
    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

int32_t AudioModemSubPortCreateTrack(AudioHandle portHandle, const struct PortPluginAttr *inputAttr,
    enum AudioPortPin portPin, AudioHandle *trackHandle)
{
    SubPortContext *portCtx = (SubPortContext *)portHandle;

    int32_t trackId = SubPortGetFreeTrack();
    CHK_COND_RETURN(trackId == INVALID_ID, MEDIA_HAL_INVALID_PARAM, "not found free track");

    int32_t ret = MEDIA_HAL_OK;
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)malloc(sizeof(SubPortTrackContext));
    if (portTrackCtx == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "malloc failed");
        ret = MEDIA_HAL_NO_MEM;
        goto FREE_TRACK;
    }

    if (memset_s(portTrackCtx, sizeof(SubPortTrackContext), 0, sizeof(SubPortTrackContext)) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "memset_s failed");
        ret = MEDIA_HAL_ERR;
        goto FREE_TRACK_CTX;
    }

    ret = SubPortTrackInit(portCtx, inputAttr, portPin, portTrackCtx);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "SubPortTrackInit failed");
        goto FREE_TRACK_CTX;
    }

    ret = AIAODevOpen(portTrackCtx);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "aiao open failed");
        goto FREE_TRACK_CTX;
    }

    ret = g_funcsList[portTrackCtx->workScene].createTrack(portTrackCtx);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "modem work scene %d create track failed", portTrackCtx->workScene);
        goto AIAO_DEV_CLOSE;
    }

    portTrackCtx->trackId = trackId;
    portCtx->portTrack[trackId] = portTrackCtx;
    *trackHandle = &portTrackCtx->common;
    MEDIA_HAL_LOGD(MODULE_NAME, "modem success trackId (%d)", portTrackCtx->trackId);
    return MEDIA_HAL_OK;

AIAO_DEV_CLOSE:
    (void)AIAODevClose(portTrackCtx);
FREE_TRACK_CTX:
    free(portTrackCtx);
FREE_TRACK:
    (void)SubPortFreeTrack(trackId);
    return ret;
}

static bool ModemPortIsValidTrack(const SubPortContext *portCtx, const SubPortTrackContext *portTrackCtx)
{
    for (int32_t i = 0; i < MODEM_PORT_TRACK_NUM; i++) {
        if (portCtx->portTrack[i] == portTrackCtx) {
            return true;
        }
    }
    return false;
}

int32_t AudioModemSubPortDestroyTrack(AudioHandle portHandle, AudioHandle trackHandle)
{
    SubPortContext *portCtx = (SubPortContext *)portHandle;
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;

    bool isValid = ModemPortIsValidTrack(portCtx, portTrackCtx);
    CHK_FAILED_RETURN(isValid, true, MEDIA_HAL_INVALID_PARAM, "track is invalid");

    int32_t ret = SubPortFreeTrack(portTrackCtx->trackId);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "SubPortFreeTrack failed");

    ret = g_funcsList[portTrackCtx->workScene].destroyTrack(portTrackCtx);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "work scene %d destroyTrack failed", portTrackCtx->workScene);
    }

    ret = AIAODevClose(portTrackCtx);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "aiao close failed");

    MEDIA_HAL_LOGD(MODULE_NAME, "success trackId (%d)", portTrackCtx->trackId);

    portCtx->portTrack[portTrackCtx->trackId] = NULL;
    free(portTrackCtx);
    return MEDIA_HAL_OK;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

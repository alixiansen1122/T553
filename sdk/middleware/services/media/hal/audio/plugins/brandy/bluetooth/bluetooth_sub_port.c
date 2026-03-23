/*
* Copyright (c) @CompanyNameMagicTag. 2021-2021. All rights reserved.
* Description: bluetooth sub port
* Author: Media Software Group
* Create: 2021-09-28
*/

#include "bluetooth_sub_port.h"

#include <unistd.h>
#include "securec.h"

#ifdef AUDIO_SDK_SUPPORT
#include "soc_uapi_adp.h"
#include "soc_uapi_adec.h"
#include "soc_uapi_aenc.h"
#include "soc_uapi_sound.h"
#endif
#ifdef BT_HAL_SUPPORT
#include "bt_audio_hal_interface.h"
#endif
#if defined(BRANDY_PRODUCT_EVB)
#include "product_evb_standard.h"
#elif defined(BRANDY_PRODUCT_EVB4)
#include "product_evb4_standard.h"
#endif
#include "plugin_common.h"
#include "media_hal_common.h"
#include "latm_simple_muxer.h"
#include "media_hal_thread_adapt.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define MODULE_NAME "AudioBluetoothSubPort"

// 蓝牙音乐中途响起闹钟，闹钟响起中途接听电话，最多会有4路track共存
#define BT_PORT_MAX_TRACK_NUM 4

#ifdef AUDIO_SDK_SUPPORT
// debug: SND_0 for speaker playback, SND_1 for mixing
#define CAST_SND_ID UAPI_SND_1
#endif

#define CAST_SND_PORT_NUM 1

#define BT_DETACH_FAIL_RETRY_TIMES 3

#define MAX_KEY_STR_LEN 10

#define MAX_BUF_LEN 1024

#define ADTS_HEADER_BYTE_NUM_NO_CRC 7
#define ADTS_HEADER_BYTE_NUM_WITH_CRC 9
#define AVRCP_VOLUME_RANGE 128

#define WAIT_BT_STOP_SUCCESS_ONCE_TIME_US 10000
#define WAIT_BT_STOP_SUCCESS_MAX_RETRY_COUNT 100

static enum AudioFormat g_supportFormats[] = {
#ifdef SUPPORT_LOW_DELAY
    AUDIO_FORMAT_SBC,
    AUDIO_FORMAT_AAC_LC
#endif
};

typedef struct {
    enum AudioPortPin portPin;
    enum SubPortType subPortType;
} PortPair;

static const PortPair g_portPairs[] = {
    { PIN_IN_BT_SCO, SUB_INPUT_PORT_BT_SCO },
    { PIN_OUT_BT_SCO, SUB_OUTPUT_PORT_BT_SCO },
    { PIN_OUT_BT_A2DP, SUB_OUTPUT_PORT_A2DP },
    { PIN_IN_BT_A2DP, SUB_INPUT_PORT_A2DP }
};

typedef enum {
    BLUETOOTH_SCO = 0,
    BLUETOOTH_IN_A2DP_NORMAL,
    BLUETOOTH_OUT_A2DP_NORMAL,
    BLUETOOTH_OUT_A2DP_LOWDELAY,
} WorkScene;

struct SubPortContext;

typedef struct {
    /* track impl */
    AudioPortTrack common;
#ifdef AUDIO_SDK_SUPPORT
    /* adp context */
    td_handle inputAdpHandle;
    td_handle outputAdpHandle;
    /* adec context */
    td_handle adecHandle;
    /* aenc context */
    td_handle aencHandle;
    /* sound context */
    td_handle castSndHandle;
    /* sound track context */
    td_handle trackHandle;
    bool isReleased;
    float volume;
#endif
    /* port track context */
    int32_t trackId;
    WorkScene workScene;
    TrackCallback callback;
    AudioHandle caller;
#ifdef BT_HAL_SUPPORT
    /* bluetooth context */
    bool btStarted;
    td_pvoid btStreamHandle;
    uint32_t a2dpSinkMemId;
#endif
    /* sub port context */
    enum SubPortType subPortType;
    /* sub port context */
    struct SubPortContext *subPortCtx;
    /* attr context */
    bool isRaw;
    int32_t trackCount;
    struct PortPluginAttr inputAttr;
    /* audio basic param for latm when low delay scene */
    AudioBasicParam audBasicParamForLATM;
    /* Record whether public resources need to be processed due to miniaturization and Bluetooth limitations. */
    bool isHandlePublicResource;
} SubPortTrackContext;

typedef struct SubPortContext {
    /****************************************************************************************/
    /* 1. castSndHandle/aencHandle/outputAdpHandle for BLUETOOTH_A2DP_NORMAL WorkScene      */
    /* 2. sdk usage sample as follows:                                                      */
    /* track1 --                                                                            */
    /*          |--> sound(mixing) + aenc + adp(output)                                     */
    /* track2 --                                                                            */
    /****************************************************************************************/
#ifdef AUDIO_SDK_SUPPORT
    /* sound context */
    td_handle castSndHandle;
    /* aenc context */
    td_handle aencHandle;
    /* adp context */
    td_handle outputAdpHandle;
#endif
#ifdef BT_HAL_SUPPORT
    /* bluetooth context */
    bool btScoStarted;
    bool btA2dpStarted;
    td_pvoid btScoStreamHandle;
    td_pvoid btA2dpStreamHandle;
    td_pvoid btA2dpSinkStreamHandle;
#endif
#ifdef AUDIO_SDK_SUPPORT
    uapi_audio_sample_rate sampleRate;
    uapi_audio_channel channelCount;
#endif
#ifdef BT_HAL_SUPPORT
    bt_audio_a2dp_config_chg_data btA2dpConfigChgdata;
    bt_audio_event_type btDevState;
#endif
    /* record track info */
    MediaHalMutexHandle a2dpTrackMutex;
    int32_t a2dpTrackCount;
    int32_t scoTrackCount;
    SubPortTrackContext *portTrack[BT_PORT_MAX_TRACK_NUM];
} SubPortContext;

typedef enum {
    AUDIO_SAMPLE_FREQ_8000    = 8000,        /**< 8 kHz */
    AUDIO_SAMPLE_FREQ_12000   = 12000,       /**< 12 kHz */
    AUDIO_SAMPLE_FREQ_11025   = 11025,       /**< 11.025 kHz */
    AUDIO_SAMPLE_FREQ_16000   = 16000,       /**< 16 kHz */
    AUDIO_SAMPLE_FREQ_22050   = 22050,       /**< 22.050 kHz */
    AUDIO_SAMPLE_FREQ_24000   = 24000,       /**< 24 kHz */
    AUDIO_SAMPLE_FREQ_32000   = 32000,       /**< 32 kHz */
    AUDIO_SAMPLE_FREQ_44100   = 44100,       /**< 44.1 kHz */
    AUDIO_SAMPLE_FREQ_48000   = 48000,       /**< 48 kHz */
    AUDIO_SAMPLE_FREQ_64000   = 64000,       /**< 64 kHz */
    AUDIO_SAMPLE_FREQ_96000   = 96000,       /**< 96 kHz */
    AUDIO_SAMPLE_FREQ_INVALID = 0xFFFFFFFFu, /**< Invalid sampling rate */
} AudioSampleRatesTemp;

typedef int32_t (*CreateTrackFunc)(SubPortTrackContext *portTrackCtx);
typedef int32_t (*DestroyTrackFunc)(const SubPortTrackContext *portTrackCtx);

typedef int32_t (*EnableTrackFunc)(SubPortTrackContext *portTrackCtx);
typedef int32_t (*DisableTrackFunc)(const SubPortTrackContext *portTrackCtx);

typedef int32_t (*TrackSetMuteFunc)(const SubPortTrackContext *portTrackCtx, bool mute);
typedef int32_t (*TrackGetMuteFunc)(const SubPortTrackContext *portTrackCtx, bool *mute);

typedef int32_t (*TrackSetVolumeFunc)(SubPortTrackContext *portTrackCtx, float volume);
typedef int32_t (*TrackGetVolumeFunc)(const SubPortTrackContext *portTrackCtx, float *volume);

typedef int32_t (*PauseTrackFunc)(SubPortTrackContext *portTrackCtx);
typedef int32_t (*ResumeTrackFunc)(SubPortTrackContext *portTrackCtx);

typedef int32_t (*SendFrameFunc)(const SubPortTrackContext *portTrackCtx, const uint8_t *buffer, uint64_t requestBytes,
    uint64_t *replyBytes);

typedef int32_t (*GetPositionFunc)(SubPortTrackContext *portTrackCtx, uint64_t *frames, struct AudioTimeStamp *time);

typedef int32_t (*InvokeFunc)(SubPortTrackContext *portTrackCtx, enum InvokeID invokeID,
    const struct InvokeAttr *invokeAttr);

typedef int32_t (*RegCallbackFunc)(SubPortTrackContext *portTrackCtx, TrackCallback callback, const AudioHandle caller);

static int32_t CreateTrackInBluetoothSCOScene(SubPortTrackContext *portTrackCtx);
static int32_t DestroyTrackInBluetoothSCOScene(const SubPortTrackContext *portTrackCtx);
static int32_t EnableTrackInBluetoothSCOScene(SubPortTrackContext *portTrackCtx);
static int32_t DisableTrackInBluetoothSCOScene(const SubPortTrackContext *portTrackCtx);
static int32_t TrackSetMuteInBluetoothSCOScene(const SubPortTrackContext *portTrackCtx, bool mute);
static int32_t TrackGetMuteInBluetoothSCOScene(const SubPortTrackContext *portTrackCtx, bool *mute);
static int32_t TrackSetVolumeInBluetoothSCOScene(SubPortTrackContext *portTrackCtx, float volume);
static int32_t TrackGetVolumeInBluetoothSCOScene(const SubPortTrackContext *portTrackCtx, float *volume);
static int32_t PauseTrackInBluetoothSCOScene(SubPortTrackContext *portTrackCtx);
static int32_t ResumeTrackInBluetoothSCOScene(SubPortTrackContext *portTrackCtx);
static int32_t SendFrameInBluetoothSCOScene(const SubPortTrackContext *portTrackCtx, const uint8_t *buffer,
    uint64_t requestBytes, uint64_t *replyBytes);
static int32_t GetPositionInBluetoothSCOScene(SubPortTrackContext *portTrackCtx, uint64_t *frames,
    struct AudioTimeStamp *time);
static int32_t InvokeInBluetoothSCOScene(SubPortTrackContext *portTrackCtx, enum InvokeID invokeID,
    const struct InvokeAttr *invokeAttr);
static int32_t RegCallbackInBluetoothSCOScene(SubPortTrackContext *portTrackCtx, TrackCallback callback,
    const AudioHandle caller);

static int32_t CreateTrackInBluetoothA2DPSinkNormalScene(SubPortTrackContext *portTrackCtx);
static int32_t DestroyTrackInBluetoothA2DPSinkNormalScene(const SubPortTrackContext *portTrackCtx);
static int32_t EnableTrackInBluetoothA2DPSinkNormalScene(SubPortTrackContext *portTrackCtx);
static int32_t DisableTrackInBluetoothA2DPSinkNormalScene(const SubPortTrackContext *portTrackCtx);
static int32_t TrackSetMuteInBluetoothA2DPSinkNormalScene(const SubPortTrackContext *portTrackCtx, bool mute);
static int32_t TrackGetMuteInBluetoothA2DPSinkNormalScene(const SubPortTrackContext *portTrackCtx, bool* mute);
static int32_t TrackSetVolumeInBluetoothA2DPSinkNormalScene(SubPortTrackContext *portTrackCtx, float volume);
static int32_t TrackGetVolumeInBluetoothA2DPSinkNormalScene(const SubPortTrackContext *portTrackCtx, float* volume);
static int32_t PauseTrackInBluetoothA2DPSinkNormalScene(SubPortTrackContext *portTrackCtx);
static int32_t ResumeTrackInBluetoothA2DPSinkNormalScene(SubPortTrackContext *portTrackCtx);
static int32_t SendFrameInBluetoothA2DPSinkNormalScene(const SubPortTrackContext *portTrackCtx, const uint8_t *buffer,
    uint64_t requestBytes, uint64_t *replyBytes);
static int32_t GetPositionInBluetoothA2DPSinkNormalScene(SubPortTrackContext *portTrackCtx, uint64_t *frames,
    struct AudioTimeStamp *time);
static int32_t InvokeInBluetoothA2DPSinkNormalScene(SubPortTrackContext *portTrackCtx, enum InvokeID invokeID,
    const struct InvokeAttr* invokeAttr);
static int32_t RegCallbackInBluetoothA2DPSinkNormalScene(SubPortTrackContext *portTrackCtx,
    TrackCallback callback, const AudioHandle caller);

static int32_t CreateTrackInBluetoothA2DPSourceNormalScene(SubPortTrackContext *portTrackCtx);
static int32_t DestroyTrackInBluetoothA2DPSourceNormalScene(const SubPortTrackContext *portTrackCtx);
static int32_t EnableTrackInBluetoothA2DPSourceNormalScene(SubPortTrackContext *portTrackCtx);
static int32_t DisableTrackInBluetoothA2DPSourceNormalScene(const SubPortTrackContext *portTrackCtx);
static int32_t TrackSetMuteInBluetoothA2DPSourceNormalScene(const SubPortTrackContext *portTrackCtx, bool mute);
static int32_t TrackGetMuteInBluetoothA2DPSourceNormalScene(const SubPortTrackContext *portTrackCtx, bool *mute);
static int32_t TrackSetVolumeInBluetoothA2DPSourceNormalScene(SubPortTrackContext *portTrackCtx, float volume);
static int32_t TrackGetVolumeInBluetoothA2DPSourceNormalScene(const SubPortTrackContext *portTrackCtx, float *volume);
static int32_t PauseTrackInBluetoothA2DPSourceNormalScene(SubPortTrackContext *portTrackCtx);
static int32_t ResumeTrackInBluetoothA2DPSourceNormalScene(SubPortTrackContext *portTrackCtx);
static int32_t SendFrameInBluetoothA2DPSourceNormalScene(const SubPortTrackContext *portTrackCtx, const uint8_t *buffer,
    uint64_t requestBytes, uint64_t *replyBytes);
static int32_t GetPositionInBluetoothA2DPSourceNormalScene(SubPortTrackContext *portTrackCtx, uint64_t *frames,
    struct AudioTimeStamp *time);
static int32_t InvokeInBluetoothA2DPSourceNormalScene(SubPortTrackContext *portTrackCtx, enum InvokeID invokeID,
    const struct InvokeAttr *invokeAttr);
static int32_t RegCallbackInBluetoothA2DPSourceNormalScene(SubPortTrackContext *portTrackCtx,
    TrackCallback callback, const AudioHandle caller);

static int32_t CreateTrackInBluetoothA2DPLowDelayScene(SubPortTrackContext *portTrackCtx);
static int32_t DestroyTrackInBluetoothA2DPLowDelayScene(const SubPortTrackContext *portTrackCtx);
static int32_t EnableTrackInBluetoothA2DPLowDelayScene(SubPortTrackContext *portTrackCtx);
static int32_t DisableTrackInBluetoothA2DPLowDelayScene(const SubPortTrackContext *portTrackCtx);
static int32_t TrackSetMuteInBluetoothA2DPLowDelayScene(const SubPortTrackContext *portTrackCtx, bool mute);
static int32_t TrackGetMuteInBluetoothA2DPLowDelayScene(const SubPortTrackContext *portTrackCtx, bool *mute);
static int32_t TrackSetVolumeInBluetoothA2DPLowDelayScene(SubPortTrackContext *portTrackCtx, float volume);
static int32_t TrackGetVolumeInBluetoothA2DPLowDelayScene(const SubPortTrackContext *portTrackCtx, float *volume);
static int32_t PauseTrackInBluetoothA2DPLowDelayScene(SubPortTrackContext *portTrackCtx);
static int32_t ResumeTrackInBluetoothA2DPLowDelayScene(SubPortTrackContext *portTrackCtx);
static int32_t SendFrameInBluetoothA2DPLowDelayScene(const SubPortTrackContext *portTrackCtx, const uint8_t *buffer,
    uint64_t requestBytes, uint64_t *replyBytes);
static int32_t GetPositionInBluetoothA2DPLowDelayScene(SubPortTrackContext *portTrackCtx, uint64_t *frames,
    struct AudioTimeStamp *time);
static int32_t InvokeInBluetoothA2DPLowDelayScene(SubPortTrackContext *portTrackCtx, enum InvokeID invokeID,
    const struct InvokeAttr *invokeAttr);
static int32_t RegCallbackInBluetoothA2DPLowDelayScene(SubPortTrackContext *portTrackCtx, TrackCallback callback,
    const AudioHandle caller);

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
    GetPositionFunc getPosition;
    InvokeFunc invoke;
    RegCallbackFunc regCallback;
} FuncsMap;

static FuncsMap g_funcsList[] = {
    {
        BLUETOOTH_SCO,
        CreateTrackInBluetoothSCOScene, DestroyTrackInBluetoothSCOScene,
        EnableTrackInBluetoothSCOScene, DisableTrackInBluetoothSCOScene,
        TrackSetMuteInBluetoothSCOScene, TrackGetMuteInBluetoothSCOScene,
        TrackSetVolumeInBluetoothSCOScene, TrackGetVolumeInBluetoothSCOScene,
        PauseTrackInBluetoothSCOScene, ResumeTrackInBluetoothSCOScene,
        SendFrameInBluetoothSCOScene, GetPositionInBluetoothSCOScene,
        InvokeInBluetoothSCOScene, RegCallbackInBluetoothSCOScene
    },
    {
        BLUETOOTH_IN_A2DP_NORMAL,
        CreateTrackInBluetoothA2DPSinkNormalScene, DestroyTrackInBluetoothA2DPSinkNormalScene,
        EnableTrackInBluetoothA2DPSinkNormalScene, DisableTrackInBluetoothA2DPSinkNormalScene,
        TrackSetMuteInBluetoothA2DPSinkNormalScene, TrackGetMuteInBluetoothA2DPSinkNormalScene,
        TrackSetVolumeInBluetoothA2DPSinkNormalScene, TrackGetVolumeInBluetoothA2DPSinkNormalScene,
        PauseTrackInBluetoothA2DPSinkNormalScene, ResumeTrackInBluetoothA2DPSinkNormalScene,
        SendFrameInBluetoothA2DPSinkNormalScene, GetPositionInBluetoothA2DPSinkNormalScene,
        InvokeInBluetoothA2DPSinkNormalScene, RegCallbackInBluetoothA2DPSinkNormalScene
    },
    {
        BLUETOOTH_OUT_A2DP_NORMAL,
        CreateTrackInBluetoothA2DPSourceNormalScene, DestroyTrackInBluetoothA2DPSourceNormalScene,
        EnableTrackInBluetoothA2DPSourceNormalScene, DisableTrackInBluetoothA2DPSourceNormalScene,
        TrackSetMuteInBluetoothA2DPSourceNormalScene, TrackGetMuteInBluetoothA2DPSourceNormalScene,
        TrackSetVolumeInBluetoothA2DPSourceNormalScene, TrackGetVolumeInBluetoothA2DPSourceNormalScene,
        PauseTrackInBluetoothA2DPSourceNormalScene, ResumeTrackInBluetoothA2DPSourceNormalScene,
        SendFrameInBluetoothA2DPSourceNormalScene, GetPositionInBluetoothA2DPSourceNormalScene,
        InvokeInBluetoothA2DPSourceNormalScene, RegCallbackInBluetoothA2DPSourceNormalScene
    },
    {
        BLUETOOTH_OUT_A2DP_LOWDELAY,
        CreateTrackInBluetoothA2DPLowDelayScene, DestroyTrackInBluetoothA2DPLowDelayScene,
        EnableTrackInBluetoothA2DPLowDelayScene, DisableTrackInBluetoothA2DPLowDelayScene,
        TrackSetMuteInBluetoothA2DPLowDelayScene, TrackGetMuteInBluetoothA2DPLowDelayScene,
        TrackSetVolumeInBluetoothA2DPLowDelayScene, TrackGetVolumeInBluetoothA2DPLowDelayScene,
        PauseTrackInBluetoothA2DPLowDelayScene, ResumeTrackInBluetoothA2DPLowDelayScene,
        SendFrameInBluetoothA2DPLowDelayScene, GetPositionInBluetoothA2DPLowDelayScene,
        InvokeInBluetoothA2DPLowDelayScene, RegCallbackInBluetoothA2DPLowDelayScene
    },
};

typedef struct {
    bool used;
    bool paused;
    int32_t trackId;
} TrackStatus;

static TrackStatus g_trackStatusList[BT_PORT_MAX_TRACK_NUM];
static int32_t g_trackActiveCount = 0;
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
        MEDIA_HAL_LOGE(MODULE_NAME, "port pin 0x%x is invalid", portPin);
    }
    return isFound;
}

static bool GetWorkScene(enum AudioPortPin portPin, enum AudioFormat audioFormat, WorkScene *workScene)
{
    bool isLowDelay = false;
    for (uint32_t i = 0; i < ARRAY_SIZE(g_supportFormats); i++) {
        if (audioFormat == g_supportFormats[i]) {
            isLowDelay = true;
        }
    }
    MEDIA_HAL_LOGI(MODULE_NAME, "audioFormat[0x%x] isLowDelay[%d]", audioFormat, isLowDelay);
    switch (portPin) {
        case PIN_IN_BT_SCO:
        case PIN_OUT_BT_SCO:
            *workScene = BLUETOOTH_SCO;
            break;
        case PIN_IN_BT_A2DP:
            *workScene = BLUETOOTH_IN_A2DP_NORMAL;
            break;
        case PIN_OUT_BT_A2DP:
            *workScene = isLowDelay ? BLUETOOTH_OUT_A2DP_LOWDELAY : BLUETOOTH_OUT_A2DP_NORMAL;
            break;
        default:
            MEDIA_HAL_LOGW(MODULE_NAME, "invalid pin 0x%x", portPin);
            return false;
    }

    return true;
}

static bool GetBTStreamHandle(const SubPortContext *portCtx, SubPortTrackContext *portTrackCtx)
{
#ifdef BT_HAL_SUPPORT
    /* check bluetooth stream handle is vailed */
    if (portTrackCtx->workScene == BLUETOOTH_SCO) {
        if (portCtx->btScoStreamHandle != NULL) {
            portTrackCtx->btStreamHandle = portCtx->btScoStreamHandle;
        } else {
            MEDIA_HAL_LOGE(MODULE_NAME, "work scene bluetooth sco no valid stream handle!");
            return false;
        }
    } else if (portTrackCtx->workScene == BLUETOOTH_IN_A2DP_NORMAL) {
        if (portCtx->btA2dpSinkStreamHandle == NULL) {
            MEDIA_HAL_LOGE(MODULE_NAME, "work scene bluetooth a2dp sink no valid stream handle!");
            return false;
        }
        portTrackCtx->btStreamHandle = portCtx->btA2dpSinkStreamHandle;
    } else if (portTrackCtx->workScene == BLUETOOTH_OUT_A2DP_NORMAL) {
        if (portCtx->btA2dpStreamHandle == NULL) {
            MEDIA_HAL_LOGE(MODULE_NAME, "work scene bluetooth a2dp source no valid stream handle!");
            return false;
        }
        portTrackCtx->btStreamHandle = portCtx->btA2dpStreamHandle;
    }
#else
    MEDIA_HAL_UNUSED(portCtx);
    MEDIA_HAL_UNUSED(portTrackCtx);
#endif

    return true;
}

int32_t AudioBluetoothSubPortTrackRegCallback(AudioHandle trackHandle, TrackCallback callback, AudioHandle caller)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;

    int32_t ret = g_funcsList[portTrackCtx->workScene].regCallback(portTrackCtx, callback, caller);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "work scene %d register callback failed", portTrackCtx->workScene);
        return MEDIA_HAL_ERR;
    }

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t SubPortTrackInit(SubPortContext *portCtx, const struct PortPluginAttr *inputAttr,
    enum AudioPortPin portPin, SubPortTrackContext *portTrackCtx)
{
    portTrackCtx->inputAttr = *inputAttr;

    /* need fix: the player supports only 16-bit width pcm, need stream manager pass bit depth parameter */
    portTrackCtx->inputAttr.bitWidth = AUDIO_BIT_WIDTH_16_BIT;

    if (!GetSubPortType(portPin, &portTrackCtx->subPortType)) {
        MEDIA_HAL_LOGE(MODULE_NAME, "GetSubPortType failed");
        return MEDIA_HAL_ERR;
    }

    if (!GetWorkScene(portPin, inputAttr->audioFormat, &portTrackCtx->workScene)) {
        MEDIA_HAL_LOGE(MODULE_NAME, "GetWorkScene failed");
        return MEDIA_HAL_ERR;
    }

    if (!GetBTStreamHandle(portCtx, portTrackCtx)) {
        MEDIA_HAL_LOGE(MODULE_NAME, "GetBTStreamHandle failed");
        return MEDIA_HAL_ERR;
    }

    portTrackCtx->common.GetFrameCount = NULL;
    portTrackCtx->common.GetLatency = AudioBluetoothSubPortTrackGetLatency;
    portTrackCtx->common.EnableTrack = AudioBluetoothSubPortEnableTrack;
    portTrackCtx->common.PauseTrack = AudioBluetoothSubPortPauseTrack;
    portTrackCtx->common.ResumeTrack = AudioBluetoothSubPortResumeTrack;
    portTrackCtx->common.FlushTrack = AudioBluetoothSubPortFlushTrack;
    portTrackCtx->common.DisableTrack = AudioBluetoothSubPortDisableTrack;
    portTrackCtx->common.AcquireFrame = AudioBluetoothSubPortAcquireFrame;
    portTrackCtx->common.SendFrame = AudioBluetoothSubPortSendFrame;
    portTrackCtx->common.GetPosition = AudioBluetoothSubPortGetPosition;
    portTrackCtx->common.SetMute = AudioBluetoothSubPortTrackSetMute;
    portTrackCtx->common.GetMute = AudioBluetoothSubPortTrackGetMute;
    portTrackCtx->common.SetVolume = AudioBluetoothSubPortTrackSetVolume;
    portTrackCtx->common.GetVolume = AudioBluetoothSubPortTrackGetVolume;
    portTrackCtx->common.Invoke = AudioBluetoothSubPortInvoke;
    portTrackCtx->common.RegCallback = AudioBluetoothSubPortTrackRegCallback;
    portTrackCtx->isRaw = IsRawFormat(inputAttr->audioFormat);
    portTrackCtx->subPortCtx = portCtx;
    portTrackCtx->isHandlePublicResource = true;
    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

#ifdef AUDIO_SDK_SUPPORT
static int32_t CastSndOpen(td_handle *sndHandle, uapi_audio_sample_rate sampleRate, uapi_audio_channel channelCount)
{
    MediaHalInitStaticMutexLock(&g_subPortLock);
    MediaHalMutexLock(g_subPortLock);
    if (g_subPortSndRefCnt == 0) {
        int32_t ret;
        uapi_snd_attr sndAttr = {};
        MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_get_default_attr(CAST_SND_ID, &sndAttr));
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "uapi_snd_get_default_attr failed");
            MediaHalMutexUnLock(g_subPortLock);
            return ret;
        }
        sndAttr.port_num = CAST_SND_PORT_NUM;
        sndAttr.port_attr[0].out_port = UAPI_SND_OUT_PORT_CAST0;
        /* current platform only support 16 bitwidth */
        sndAttr.bit_depth = UAPI_AUDIO_BIT_DEPTH_16;
        sndAttr.sample_rate = sampleRate;
        sndAttr.channels = channelCount;

        MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_open(sndHandle, CAST_SND_ID, &sndAttr));
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "uapi_snd_open failed");
            MediaHalMutexUnLock(g_subPortLock);
            return ret;
        }
        ret = uapi_snd_set_track_mode(*sndHandle, sndAttr.port_attr[0].out_port, SND_TRACK_MODE_BT_MUSIC);
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "set track mode failed, ret = %d", ret);
            MediaHalMutexUnLock(g_subPortLock);
            return ret;
        }
    }
    g_subPortSndRefCnt++;
    MediaHalMutexUnLock(g_subPortLock);

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t CastSndClose(td_handle sndHandle)
{
    MediaHalInitStaticMutexLock(&g_subPortLock);
    MediaHalMutexLock(g_subPortLock);
    g_subPortSndRefCnt--;
    if (g_subPortSndRefCnt == 0) {
        int32_t ret;
        MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_close(sndHandle));
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "uapi_snd_close failed");
            MediaHalMutexUnLock(g_subPortLock);
            return ret;
        }
    }
    MediaHalMutexUnLock(g_subPortLock);

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}
#endif

static int32_t SubPortTrackManagerInit(void)
{
    MediaHalInitStaticMutexLock(&g_trackResourceLock);
    MediaHalMutexLock(g_trackResourceLock);
    if (g_trackMngInit == false) {
        for (int32_t i = 0; i < BT_PORT_MAX_TRACK_NUM; i++) {
            g_trackStatusList[i].trackId = i;
            g_trackStatusList[i].used = false;
            g_trackStatusList[i].paused = false;
        }
        g_trackMngInit = true;
    }
    MediaHalMutexUnLock(g_trackResourceLock);

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t SubPortTrackManagerDeInit(void)
{
    MediaHalInitStaticMutexLock(&g_trackResourceLock);
    MediaHalMutexLock(g_trackResourceLock);
    if (g_trackMngInit == true) {
        for (int32_t i = 0; i < BT_PORT_MAX_TRACK_NUM; i++) {
            if (g_trackStatusList[i].used == true) {
                MEDIA_HAL_LOGE(MODULE_NAME, "buetooth port track %d not free", g_trackStatusList[i].trackId);
            }
        }
        g_trackMngInit = false;
    }
    MediaHalMutexUnLock(g_trackResourceLock);

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t SubPortGetFreeTrack(int32_t *trackCount)
{
    MediaHalInitStaticMutexLock(&g_trackResourceLock);
    MediaHalMutexLock(g_trackResourceLock);
    if (g_trackMngInit == false) {
        MediaHalMutexUnLock(g_trackResourceLock);
        MEDIA_HAL_LOGE(MODULE_NAME, "buetooth port track manager not init");
        return INVALID_ID;
    }

    for (int32_t i = 0; i < BT_PORT_MAX_TRACK_NUM; i++) {
        if (g_trackStatusList[i].used == false) {
            g_trackStatusList[i].used = true;
            *trackCount = ++g_trackActiveCount;
            int32_t trackId = g_trackStatusList[i].trackId;
            MediaHalMutexUnLock(g_trackResourceLock);
            return trackId;
        }
    }
    MediaHalMutexUnLock(g_trackResourceLock);
    MEDIA_HAL_LOGE(MODULE_NAME, "buetooth port no free track");
    return INVALID_ID;
}

static int32_t SubPortFreeTrack(int32_t trackId, int32_t *trackCount)
{
    MediaHalInitStaticMutexLock(&g_trackResourceLock);
    MediaHalMutexLock(g_trackResourceLock);
    for (int32_t i = 0; i < BT_PORT_MAX_TRACK_NUM; i++) {
        if (g_trackStatusList[i].trackId == trackId) {
            if (g_trackStatusList[i].used == false) {
                MediaHalMutexUnLock(g_trackResourceLock);
                MEDIA_HAL_LOGE(MODULE_NAME, "buetooth port track id: %d not used can't free", trackId);
                return MEDIA_HAL_ERR;
            }
            g_trackStatusList[i].used = false;
            *trackCount = --g_trackActiveCount;
            MediaHalMutexUnLock(g_trackResourceLock);
            return MEDIA_HAL_OK;
        }
    }
    MediaHalMutexUnLock(g_trackResourceLock);
    MEDIA_HAL_LOGE(MODULE_NAME, "track id: %d is invalid", trackId);
    return MEDIA_HAL_ERR;
}

static int32_t CreateTrackInBluetoothSCOScene(SubPortTrackContext *portTrackCtx)
{
    MEDIA_HAL_UNUSED(portTrackCtx);

    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

static int32_t DestroyTrackInBluetoothSCOScene(const SubPortTrackContext *portTrackCtx)
{
#ifdef BT_HAL_SUPPORT
#ifdef SUPPORT_BREDR
    bt_audio_port_params portParam = {};
    portParam.port_type = (portTrackCtx->subPortType == SUB_INPUT_PORT_BT_SCO) ? SCO_IN : SCO_OUT;
    td_u32 ret = bt_detach_audio_port(portTrackCtx->btStreamHandle, &portParam);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, MEDIA_HAL_ERR, "bt_detach_audio_port failed");
#endif
#else
    MEDIA_HAL_UNUSED(portTrackCtx);
#endif

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t EnableTrackInBluetoothSCOScene(SubPortTrackContext *portTrackCtx)
{
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

static int32_t DisableTrackInBluetoothSCOScene(const SubPortTrackContext *portTrackCtx)
{
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

static int32_t TrackSetMuteInBluetoothSCOScene(const SubPortTrackContext *portTrackCtx, bool mute)
{
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(mute);

    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

static int32_t TrackGetMuteInBluetoothSCOScene(const SubPortTrackContext *portTrackCtx, bool *mute)
{
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(mute);

    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

static int32_t TrackSetVolumeInBluetoothSCOScene(SubPortTrackContext *portTrackCtx, float volume)
{
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(volume);

    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

static int32_t TrackGetVolumeInBluetoothSCOScene(const SubPortTrackContext *portTrackCtx, float *volume)
{
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(volume);

    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

static int32_t PauseTrackInBluetoothSCOScene(SubPortTrackContext *portTrackCtx)
{
#ifdef BT_HAL_SUPPORT
#ifdef SUPPORT_BREDR
    td_u32 ret = bt_pause_audio_stream(portTrackCtx->btStreamHandle);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, MEDIA_HAL_ERR, "bt_pause_audio_stream failed");
#endif
#else
    MEDIA_HAL_UNUSED(portTrackCtx);
#endif

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t ResumeTrackInBluetoothSCOScene(SubPortTrackContext *portTrackCtx)
{
    MEDIA_HAL_UNUSED(portTrackCtx);

    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

static int32_t SendFrameInBluetoothSCOScene(const SubPortTrackContext *portTrackCtx, const uint8_t *buffer,
    uint64_t requestBytes, uint64_t *replyBytes)
{
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(buffer);
    MEDIA_HAL_UNUSED(requestBytes);
    MEDIA_HAL_UNUSED(replyBytes);

    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

static int32_t GetPositionInBluetoothSCOScene(SubPortTrackContext *portTrackCtx, uint64_t *frames,
    struct AudioTimeStamp *time)
{
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(frames);
    MEDIA_HAL_UNUSED(time);

    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

static int32_t InvokeInBluetoothSCOScene(SubPortTrackContext *portTrackCtx, enum InvokeID invokeID,
    const struct InvokeAttr *invokeAttr)
{
    int32_t ret;
    char key[MAX_KEY_STR_LEN] = { 0 };
    uint32_t scoShmId = 0;

    switch (invokeID) {
        case INVOKE_ID_SET_EXTRA_PARAMS:
            ret = sscanf_s((const char *)invokeAttr->request, "%s = %u", key, MAX_KEY_STR_LEN, &scoShmId);
            if (ret == MEDIA_HAL_ERR) {
                MEDIA_HAL_LOGI(MODULE_NAME, "sscanf_s failed");
                return ret;
            }
            MEDIA_HAL_LOGI(MODULE_NAME, "extra param key = %s, value = %d", key, scoShmId);
            break;
        default:
            MEDIA_HAL_LOGW(MODULE_NAME, "invalid invoke id: %d", invokeID);
            return MEDIA_HAL_INVALID_PARAM;
    }

#if defined(BT_HAL_SUPPORT) && defined(SUPPORT_BREDR)
    bt_audio_port_params portParam = {};
    portParam.port_type = (portTrackCtx->subPortType == SUB_INPUT_PORT_BT_SCO) ? SCO_IN : SCO_OUT;
    portParam.share_mem_id = scoShmId;
    td_u32 btRet = bt_attach_audio_port(portTrackCtx->btStreamHandle, &portParam);
    CHK_FAILED_RETURN(btRet, MEDIA_HAL_OK, MEDIA_HAL_ERR, "bt_attach_audio_port failed");
#endif

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t RegCallbackInBluetoothSCOScene(SubPortTrackContext *portTrackCtx, TrackCallback callback,
    const AudioHandle caller)
{
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(callback);
    MEDIA_HAL_UNUSED(caller);

    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

static int32_t SndCreateTrack(SubPortTrackContext *portTrackCtx)
{
#ifdef AUDIO_SDK_SUPPORT
    int32_t ret;
    uapi_snd_track_attr defTrackAttr = {};
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_get_track_default_attr(&defTrackAttr));
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_snd_get_track_default_attr failed");

    MEDIA_HAL_DOFUNC_TIME(
        ret = uapi_snd_create_track(&(portTrackCtx->trackHandle), portTrackCtx->castSndHandle, &defTrackAttr));
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_snd_create_track failed");
#else
    MEDIA_HAL_UNUSED(portTrackCtx);
#endif

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t SndDestroyTrack(const SubPortTrackContext *portTrackCtx)
{
#ifdef AUDIO_SDK_SUPPORT
    int32_t ret;
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_destroy_track(portTrackCtx->trackHandle));
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_snd_destroy_track failed");
#else
    MEDIA_HAL_UNUSED(portTrackCtx);
#endif

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

#ifdef AUDIO_SDK_SUPPORT
static int32_t AudioSDKAttachAdpAndAdec(SubPortTrackContext *portTrackCtx)
{
    int32_t ret = AudioSDKAttach(AUDIO_MOD_ADP, portTrackCtx->inputAdpHandle, portTrackCtx->adecHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "adp attach adec failed");
        return ret;
    }
    ret = AudioSDKAttach(AUDIO_MOD_ADEC, portTrackCtx->adecHandle, portTrackCtx->trackHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "adec attach track failed");
        (void)AudioSDKDetach(AUDIO_MOD_ADP, portTrackCtx->inputAdpHandle, portTrackCtx->adecHandle);
    }
    return ret;
}
#endif

static int32_t SndAttach(SubPortTrackContext *portTrackCtx)
{
#ifdef AUDIO_SDK_SUPPORT
    int32_t ret;
    if (portTrackCtx->isRaw) {
        ret = AudioSDKAdpOpen(&portTrackCtx->inputAdpHandle);
        CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "input adp open failed");
        ret = AudioSDKAttach(AUDIO_MOD_ADP, portTrackCtx->inputAdpHandle, portTrackCtx->trackHandle);
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "adp attach track failed");
            goto ADP_CLOSE;
        }
    } else {
        ret = AudioSDKAdpOpen(&portTrackCtx->inputAdpHandle);
        CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "input adp open failed");
        ret = IsNeedSetExtraFlag(portTrackCtx->inputAttr.audioFormat, &portTrackCtx->inputAttr.isNeedSetExtraParam);
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "set extra flag failed");
            goto ADP_CLOSE;
        }
        if (portTrackCtx->inputAttr.isNeedSetExtraParam) {
            MEDIA_HAL_LOGI(MODULE_NAME, "need SetExtraParam adec and attach delay");
            return MEDIA_HAL_OK;
        }
        ret = AudioSDKAdecOpen(&(portTrackCtx->adecHandle), portTrackCtx->inputAttr);
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "adec open failed");
            goto ADP_CLOSE;
        }
        ret = AudioSDKAttachAdpAndAdec(portTrackCtx);
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "adp attach adec failed");
            goto ADEC_CLOSE;
        }
    }
#else
    MEDIA_HAL_UNUSED(portTrackCtx);
#endif

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;

#ifdef AUDIO_SDK_SUPPORT
ADEC_CLOSE:
    (void)AudioSDKAdecClose(portTrackCtx->adecHandle);
ADP_CLOSE:
    (void)AudioSDKAdpClose(portTrackCtx->inputAdpHandle);
    return ret;
#endif
}

static int32_t SndDetach(const SubPortTrackContext *portTrackCtx)
{
#ifdef AUDIO_SDK_SUPPORT
    int32_t ret;
    if (portTrackCtx->isRaw) {
        ret = AudioSDKDetach(AUDIO_MOD_ADP, portTrackCtx->inputAdpHandle, portTrackCtx->trackHandle);
        CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "adp detach track failed");
        ret = AudioSDKAdpClose(portTrackCtx->inputAdpHandle);
        CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "adp close failed");
    } else {
        ret = AudioSDKDetach(AUDIO_MOD_ADEC, portTrackCtx->adecHandle, portTrackCtx->trackHandle);
        CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "adec detach track failed");
        ret = AudioSDKDetach(AUDIO_MOD_ADP, portTrackCtx->inputAdpHandle, portTrackCtx->adecHandle);
        CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "adp detach adec failed");
        ret = AudioSDKAdecClose(portTrackCtx->adecHandle);
        CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "adec close failed");
        ret = AudioSDKAdpClose(portTrackCtx->inputAdpHandle);
        CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "adp close failed");
    }

    ret = SndDestroyTrack(portTrackCtx);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "sound destroy track failed");
#else
    MEDIA_HAL_UNUSED(portTrackCtx);
#endif

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t CreateTrackInBluetoothA2DPSourceNormalScene(SubPortTrackContext *portTrackCtx)
{
#ifdef AUDIO_SDK_SUPPORT
    int32_t ret = SndCreateTrack(portTrackCtx);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "SndCreateTrack failed");

    ret = SndAttach(portTrackCtx);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "SndAttach failed");
        goto DESTROY_TRACK;
    }
#else
    MEDIA_HAL_UNUSED(portTrackCtx);
#endif

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;

#ifdef AUDIO_SDK_SUPPORT
DESTROY_TRACK:
    (void)SndDestroyTrack(portTrackCtx);
    return ret;
#endif
}

static int32_t CreateTrackInBluetoothA2DPSinkNormalScene(SubPortTrackContext *portTrackCtx)
{
    MEDIA_HAL_UNUSED(portTrackCtx);
    return MEDIA_HAL_OK;
}

static int32_t DestroyTrackInBluetoothA2DPSourceNormalScene(const SubPortTrackContext *portTrackCtx)
{
#ifdef MINI_MEMORY_SUPPORT
    // 目前支持 SDK 小型化方案，暂停时刻会销毁通路，同时正常退出和切换输出设备也会销毁通路，避免重复销毁，需要检查下，典型场景如下：
    // 1.蓝牙音乐暂停状态退出播放
    // 2.蓝牙音乐暂停状态断开蓝牙耳机
    // 3.蓝牙音乐播放中途接听电话，通话中途断开耳机，通话挂断后恢复播放
    if (portTrackCtx->isReleased) {
        MEDIA_HAL_LOGI(MODULE_NAME, "have released");
        return MEDIA_HAL_OK;
    }
#endif

#ifdef AUDIO_SDK_SUPPORT
    int32_t ret = SndDetach(portTrackCtx);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "SndDetach failed");
        return MEDIA_HAL_ERR;
    }
#else
    MEDIA_HAL_UNUSED(portTrackCtx);
#endif

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t DestroyTrackInBluetoothA2DPSinkNormalScene(const SubPortTrackContext *portTrackCtx)
{
#if defined(BT_HAL_SUPPORT) && defined(SUPPORT_BREDR)
    bt_audio_port_params portParam = {};
    portParam.port_type = A2DP;
    portParam.share_mem_id = portTrackCtx->a2dpSinkMemId;
    td_u32 ret = bt_detach_audio_port(portTrackCtx->btStreamHandle, &portParam);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, MEDIA_HAL_ERR, "bt_detach_audio_port failed");
#endif
    MEDIA_HAL_LOGI(MODULE_NAME, "Destroy Track success");
    return MEDIA_HAL_OK;
}


static int32_t StartByAudioModule(const SubPortTrackContext *portTrackCtx, AudioSDKModuleType module)
{
#ifdef AUDIO_SDK_SUPPORT
    int32_t ret;
    switch (module) {
        case AUDIO_MOD_AENC:
            MEDIA_HAL_DOFUNC_TIME(ret = uapi_aenc_start(portTrackCtx->aencHandle));
            CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_aenc_start failed");
            break;
        case AUDIO_MOD_ADEC:
            MEDIA_HAL_DOFUNC_TIME(ret = uapi_adec_start(portTrackCtx->adecHandle));
            CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_adec_start failed");
            break;
        default:
            MEDIA_HAL_LOGE(MODULE_NAME, "invalid audio module: %d", module);
            return MEDIA_HAL_INVALID_PARAM;
    }
#else
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(module);
#endif

    MEDIA_HAL_LOGD(MODULE_NAME, "start success trackId (%d)", portTrackCtx->trackId);
    return MEDIA_HAL_OK;
}

static int32_t StopByAudioModule(const SubPortTrackContext *portTrackCtx, AudioSDKModuleType module)
{
#ifdef AUDIO_SDK_SUPPORT
    int32_t ret;
    switch (module) {
        case AUDIO_MOD_AENC: {
            MEDIA_HAL_DOFUNC_TIME(ret = uapi_aenc_stop(portTrackCtx->aencHandle));
            CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_aenc_stop failed");
            break;
        }
        case AUDIO_MOD_ADEC:
            MEDIA_HAL_DOFUNC_TIME(ret = uapi_adec_stop(portTrackCtx->adecHandle));
            CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_adec_stop failed");
            break;
        default:
            MEDIA_HAL_LOGE(MODULE_NAME, "invalid audio module: %d", module);
            return MEDIA_HAL_INVALID_PARAM;
    }
#else
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(module);
#endif

    MEDIA_HAL_LOGD(MODULE_NAME, "stop success trackId (%d)", portTrackCtx->trackId);
    return MEDIA_HAL_OK;
}

static int32_t SetExtraDataForAdecAndAttach(SubPortTrackContext *portTrackCtx)
{
#ifdef AUDIO_SDK_SUPPORT
    int32_t ret = AudioSDKAdecOpen(&(portTrackCtx->adecHandle), portTrackCtx->inputAttr);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "adec open failed");
        return ret;
    }
    ret = AudioSDKAttach(AUDIO_MOD_ADP, portTrackCtx->inputAdpHandle, portTrackCtx->adecHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "adp attach adec failed");
        goto ADEC_CLOSE;
    }
    ret = AudioSDKAttach(AUDIO_MOD_ADEC, portTrackCtx->adecHandle, portTrackCtx->trackHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "adec attach track failed");
        goto ADP_DETACH_ADEC;
    }
#else
    MEDIA_HAL_UNUSED(portTrackCtx);
#endif
    return MEDIA_HAL_OK;

#ifdef AUDIO_SDK_SUPPORT
ADP_DETACH_ADEC:
    (void)AudioSDKDetach(AUDIO_MOD_ADP, portTrackCtx->inputAdpHandle, portTrackCtx->adecHandle);
ADEC_CLOSE:
    (void)AudioSDKAdecClose(portTrackCtx->adecHandle);
    return ret;
#endif
}

#ifdef AUDIO_SDK_SUPPORT
static int32_t AdpEventCB(td_handle adpHandle, uapi_adp_event_type event, td_void *param, td_void *context)
{
    MEDIA_HAL_UNUSED(adpHandle);
    MEDIA_HAL_UNUSED(param);
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)context;
    if (portTrackCtx->callback == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "callback is null");
        return MEDIA_HAL_ERR;
    }
    int32_t ret;
    switch (event) {
        case UAPI_ADP_EVENT_EOS:
            ret = portTrackCtx->callback(AUDIO_RENDER_EOS, NULL, 0, portTrackCtx->caller);
            CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "adp event eos callback failed");
            break;
        case UAPI_ADP_EVENT_EMPTY:
            ret = portTrackCtx->callback(AUDIO_BUFFER_EMPTY, NULL, 0, portTrackCtx->caller);
            CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "adp event buffer empty callback failed");
            break;
        default:
            MEDIA_HAL_LOGW(MODULE_NAME, "invalid event: %d", event);
            break;
    }

    return MEDIA_HAL_OK;
}
#endif

static int32_t InitAencAttr(const SubPortContext *portCtx, uapi_aenc_attr *aencAttr)
{
#ifdef BT_HAL_SUPPORT
    const bt_a2dp_codec_param *codecParams = &portCtx->btA2dpConfigChgdata.codec;
    if (codecParams->codec_type != BT_AUDIO_CODEC_SBC) {
        MEDIA_HAL_LOGE(MODULE_NAME, "unsupport codec type: 0x%x", codecParams->codec_type);
        return MEDIA_HAL_ERR;
    }

    aencAttr->codec_id = UAPI_ACODEC_ID_SBC;
    aencAttr->max_trans_unit_size = codecParams->payload;
    aencAttr->param.interleaved = true;
    aencAttr->param.channels = portCtx->channelCount;
    aencAttr->param.bit_depth = AUDIO_BIT_WIDTH_16_BIT;
    aencAttr->param.sample_rate = portCtx->sampleRate;
    aencAttr->param.samples_per_frame = ((uint32_t)portCtx->sampleRate) / SAMPLERATE_TO_PTNUMPERFRM;
    aencAttr->param.private_data = (td_void *)(portCtx->btA2dpConfigChgdata.codec.codec_caps);
    aencAttr->param.private_data_size = codecParams->cap_len;

    MEDIA_HAL_LOGI(MODULE_NAME, "audioFormat: 0x%x, sampleRate: %d, channelCount: %u, bitWidth: %u, payload: %u",
        AUDIO_FORMAT_SBC, portCtx->sampleRate, portCtx->channelCount, AUDIO_BIT_WIDTH_16_BIT, codecParams->payload);
#endif
    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t EnableTrackInBluetoothA2DPSourceNormalScene(SubPortTrackContext *portTrackCtx)
{
#ifdef AUDIO_SDK_SUPPORT
    int32_t ret;
    if (portTrackCtx->inputAttr.isNeedSetExtraParam) {
        if (SetExtraDataForAdecAndAttach(portTrackCtx) != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "SetExtraDataAdecAndAttach failed");
            return MEDIA_HAL_ERR;
        }
    }

    uapi_adp_event_proc adpCallback = AdpEventCB;
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_adp_register_event_proc(portTrackCtx->inputAdpHandle, adpCallback,
        (td_void *)portTrackCtx));
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_adp_register_event_proc failed");

    ret = uapi_snd_track_start(portTrackCtx->trackHandle, TD_NULL);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_snd_track_start failed");

    if (!portTrackCtx->isRaw) {
        ret = StartByAudioModule(portTrackCtx, AUDIO_MOD_ADEC);
        CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "StartAudioModule ADEC failed");
    }
#endif

    MEDIA_HAL_LOGD(MODULE_NAME, "success trackId (%d)", portTrackCtx->trackId);
    return MEDIA_HAL_OK;
}

static int32_t EnableTrackInBluetoothA2DPSinkNormalScene(SubPortTrackContext *portTrackCtx)
{
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_LOGI(MODULE_NAME, "EnableTrackInBluetoothA2DPSinkNormalScene not support");
    return MEDIA_HAL_OK;
}

static int32_t DisableTrackInBluetoothA2DPSourceNormalScene(const SubPortTrackContext *portTrackCtx)
{
#ifdef AUDIO_SDK_SUPPORT
#ifdef MINI_MEMORY_SUPPORT
    // 目前支持 SDK 小型化方案，暂停时刻会销毁通路，同时正常退出和切换输出设备也会销毁通路，避免重复销毁，需要检查下，典型场景如下：
    // 1.蓝牙音乐暂停状态退出播放
    // 2.蓝牙音乐暂停状态断开蓝牙耳机
    // 3.蓝牙音乐播放中途接听电话，通话中途断开耳机，通话挂断后恢复播放
    if (portTrackCtx->isReleased) {
        MEDIA_HAL_LOGI(MODULE_NAME, "have released");
        return MEDIA_HAL_OK;
    }
#endif
    int32_t ret = uapi_snd_track_stop(portTrackCtx->trackHandle);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_snd_track_stop failed");

    if (!portTrackCtx->isRaw) {
        ret = StopByAudioModule(portTrackCtx, AUDIO_MOD_ADEC);
        CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "StopByAudioModule ADEC failed");
    }
#endif

    MEDIA_HAL_LOGD(MODULE_NAME, "success trackId (%d)", portTrackCtx->trackId);
    return MEDIA_HAL_OK;
}

static int32_t TrackSetMuteInBluetoothA2DPSourceNormalScene(const SubPortTrackContext *portTrackCtx, bool mute)
{
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(mute);

    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

static int32_t TrackGetMuteInBluetoothA2DPSourceNormalScene(const SubPortTrackContext *portTrackCtx, bool *mute)
{
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(mute);

    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

static int32_t TrackSetVolumeInBluetoothA2DPSourceNormalScene(SubPortTrackContext *portTrackCtx, float volume)
{
#ifdef AUDIO_SDK_SUPPORT
#ifdef MINI_MEMORY_SUPPORT
    // 目前支持 SDK 小型化方案，暂停的时候会销毁通路，设置音量操作会失败，此时需要记录音量信息，在恢复播放的时候作用上去
    if (portTrackCtx->isReleased) {
        portTrackCtx->volume = volume;
        return MEDIA_HAL_OK;
    }
#endif
    int32_t ret;
    int32_t tmpVolume;
    uapi_snd_gain sndGain = {};
    VolumePercentToDb(volume, &tmpVolume);
    sndGain.integer = tmpVolume;
    sndGain.decimal = 0;
    uapi_audio_toggle_config volumeToggleConfig = { 0 };
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_set_track_volume(portTrackCtx->trackHandle, &sndGain,
        &volumeToggleConfig));
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_snd_set_track_volume failed");

    portTrackCtx->volume = volume;
#else
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(volume);
#endif

    MEDIA_HAL_LOGI(MODULE_NAME, "set volume %f success", volume);
    return MEDIA_HAL_OK;
}

static int32_t TrackGetVolumeInBluetoothA2DPSourceNormalScene(const SubPortTrackContext *portTrackCtx, float *volume)
{
    CHK_NULL_RETURN(volume, MEDIA_HAL_ERR, "volume is NULL");
    *volume = portTrackCtx->volume;
    return MEDIA_HAL_OK;
}

#ifdef MINI_MEMORY_SUPPORT
static int32_t SetPause(int32_t trackId, bool pause)
{
    MediaHalMutexLock(g_trackResourceLock);
    for (int32_t i = 0; i < BT_PORT_MAX_TRACK_NUM; i++) {
        if (g_trackStatusList[i].trackId != trackId) {
            continue;
        }
        if (!g_trackStatusList[i].used) {
            MediaHalMutexUnLock(g_trackResourceLock);
            MEDIA_HAL_LOGE(MODULE_NAME, "buetooth port track id: %d not used can't pause", trackId);
            return MEDIA_HAL_INVALID_PARAM;
        }
        g_trackStatusList[i].paused = pause;
        MediaHalMutexUnLock(g_trackResourceLock);
        return MEDIA_HAL_OK;
    }
    MediaHalMutexUnLock(g_trackResourceLock);
    MEDIA_HAL_LOGE(MODULE_NAME, "track id: %d is invalid", trackId);
    return MEDIA_HAL_ERR;
}

static bool IsNeedHandlePublicResource(void)
{
    MediaHalMutexLock(g_trackResourceLock);
    for (int32_t i = 0; i < BT_PORT_MAX_TRACK_NUM; i++) {
        if (g_trackStatusList[i].used) {
            if (!g_trackStatusList[i].paused) {
                MediaHalMutexUnLock(g_trackResourceLock);
                return false;
            }
        }
    }
    MediaHalMutexUnLock(g_trackResourceLock);
    return true;
}

static int32_t AudioSDKStopInternal(const SubPortTrackContext *portTrackCtx)
{
    int32_t ret = uapi_snd_track_stop(portTrackCtx->trackHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "sound track stop failed");

    if (!portTrackCtx->isRaw) {
        MEDIA_HAL_DOFUNC_TIME(ret = uapi_adec_stop(portTrackCtx->adecHandle));
        CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "adec stop failed");
    }
    if (portTrackCtx->isHandlePublicResource) {
        MEDIA_HAL_DOFUNC_TIME(ret = uapi_aenc_stop(portTrackCtx->aencHandle));
        CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "aenc stop failed");
    }

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

#if defined(BT_HAL_SUPPORT) && defined(SUPPORT_BREDR)
static int32_t BtDetachHandle(const SubPortTrackContext *portTrackCtx, bt_audio_port_params portParam)
{
    int32_t count = 0;
    td_u32 btRet = MEDIA_HAL_OK;
    while (count < BT_DETACH_FAIL_RETRY_TIMES) {
        btRet = bt_detach_audio_port(portTrackCtx->btStreamHandle, &portParam);
        if (btRet == MEDIA_HAL_OK) {
            MEDIA_HAL_LOGI(MODULE_NAME, "bt detach audio port success!");
            return MEDIA_HAL_OK;
        }
        count++;
        MEDIA_HAL_LOGE(MODULE_NAME, "bt detach audio port retry ret = %x, count: %d", count, btRet);
    }
    return MEDIA_HAL_ERR;
}
#endif

static int32_t BluetoothDetachInternal(const SubPortTrackContext *portTrackCtx)
{
    int32_t ret = MEDIA_HAL_OK;
#if defined(BT_HAL_SUPPORT) && defined(SUPPORT_BREDR)
    if (portTrackCtx->isHandlePublicResource) {
        bt_audio_port_params portParam = {};
        portParam.port_type = A2DP;
        portParam.share_mem_id = portTrackCtx->outputAdpHandle;
        ret = bt_stop_audio_stream(portTrackCtx->btStreamHandle);
        CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "bt stop stream failed");
        ret = BtDetachHandle(portTrackCtx, portParam);
        CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "bt detach handle failed");
    }
#endif
    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return ret;
}

static int32_t AudioSDKDetachInternal(const SubPortTrackContext *portTrackCtx)
{
    int32_t ret;
    if (portTrackCtx->isRaw) {
        ret = AudioSDKDetach(AUDIO_MOD_ADP, portTrackCtx->inputAdpHandle, portTrackCtx->trackHandle);
        CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "adp detach track failed");
    } else {
        ret = AudioSDKDetach(AUDIO_MOD_ADEC, portTrackCtx->adecHandle, portTrackCtx->trackHandle);
        CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "adec detach track failed");
        ret = AudioSDKDetach(AUDIO_MOD_ADP, portTrackCtx->inputAdpHandle, portTrackCtx->adecHandle);
        CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "adp detach adec failed");
    }
    if (portTrackCtx->isHandlePublicResource) {
        ret = AudioSDKDetach(AUDIO_MOD_AENC, portTrackCtx->aencHandle, portTrackCtx->outputAdpHandle);
        CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "aenc detach adp failed");
        ret = AudioSDKDetach(AUDIO_MOD_SOUND, portTrackCtx->castSndHandle, portTrackCtx->aencHandle);
        CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "sound detach aenc failed");
    }

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t AudioSDKCloseInternal(const SubPortTrackContext *portTrackCtx)
{
    int32_t ret;
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_destroy_track(portTrackCtx->trackHandle));
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "sound destroy track failed");
    if (portTrackCtx->isHandlePublicResource) {
        ret = AudioSDKAdpClose(portTrackCtx->outputAdpHandle);
        CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "adp close failed");

        ret = AudioSDKAencClose(portTrackCtx->aencHandle);
        CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "aenc close failed");
    }
    ret = CastSndClose(portTrackCtx->subPortCtx->castSndHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "sound close failed");

    ret = AudioSDKAdecClose(portTrackCtx->adecHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "adec close failed");

    ret = AudioSDKAdpClose(portTrackCtx->inputAdpHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "adp close failed");

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}
#endif

static int32_t PauseTrackInBluetoothA2DPSourceNormalScene(SubPortTrackContext *portTrackCtx)
{
#ifdef MINI_MEMORY_SUPPORT
    MediaHalMutexLock(portTrackCtx->subPortCtx->a2dpTrackMutex);
    int32_t ret = SetPause(portTrackCtx->trackId, true);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, " pause track failed");
        MediaHalMutexUnLock(portTrackCtx->subPortCtx->a2dpTrackMutex);
        return MEDIA_HAL_ERR;
    }
    portTrackCtx->isHandlePublicResource = IsNeedHandlePublicResource();
    // pick up bluetooth voice call when bluetooth music playback
    // 1. sdk stop/detach/close and bt detach for save shared memory
    // 2. sdk aenc close because of only one encoder is supported
    ret = AudioSDKStopInternal(portTrackCtx);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "audio stop failed");
    ret = BluetoothDetachInternal(portTrackCtx);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "bluetooth detach failed");
    ret = AudioSDKDetachInternal(portTrackCtx);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "audio detach failed");
    ret = AudioSDKCloseInternal(portTrackCtx);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "audio close failed");
    portTrackCtx->isReleased = true;
    portTrackCtx->subPortCtx->a2dpTrackCount--;
    MediaHalMutexUnLock(portTrackCtx->subPortCtx->a2dpTrackMutex);
#endif

#ifdef BT_HAL_SUPPORT
    MEDIA_HAL_LOGD(MODULE_NAME, "success, cur id[%d] bt devState[%d]", portTrackCtx->trackId,
        portTrackCtx->subPortCtx->btDevState);
#endif
    return MEDIA_HAL_OK;
}

#ifdef MINI_MEMORY_SUPPORT
static int32_t AudioSDKOpenInternal(SubPortTrackContext *portTrackCtx)
{
    int32_t ret = CastSndOpen(&portTrackCtx->subPortCtx->castSndHandle, portTrackCtx->subPortCtx->sampleRate,
        portTrackCtx->subPortCtx->channelCount);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "sound open failed");

    portTrackCtx->castSndHandle = portTrackCtx->subPortCtx->castSndHandle;
    ret = SndCreateTrack(portTrackCtx);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "sound create track failed");
        goto SOUND_CLOSE;
    }

    ret = AudioSDKAdpOpen(&portTrackCtx->inputAdpHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "input adp open failed");
        goto DESTROY_TRACK;
    }

    ret = AudioSDKAdecOpen(&(portTrackCtx->adecHandle), portTrackCtx->inputAttr);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "adec open failed");
        goto ADP_CLOSE;
    }
    if (portTrackCtx->isHandlePublicResource) {
        uapi_aenc_attr aencAttr = {};
        ret = InitAencAttr(portTrackCtx->subPortCtx, &aencAttr);
        CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "init aenc attr failed");
        ret = AudioSDKAencOpen(&portTrackCtx->aencHandle, aencAttr);
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "audio attach failed");
            goto ADEC_CLOSE;
        }
        portTrackCtx->subPortCtx->aencHandle = portTrackCtx->aencHandle;

        ret = AudioSDKAdpOpen(&portTrackCtx->outputAdpHandle);
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "output adp open failed");
            goto AENC_CLOSE;
        }
        portTrackCtx->subPortCtx->outputAdpHandle = portTrackCtx->outputAdpHandle;
    }
    return MEDIA_HAL_OK;

AENC_CLOSE:
    (void)AudioSDKAencClose(portTrackCtx->aencHandle);
ADEC_CLOSE:
    (void)AudioSDKAdecClose(portTrackCtx->adecHandle);
ADP_CLOSE:
    (void)AudioSDKAdpClose(portTrackCtx->inputAdpHandle);
DESTROY_TRACK:
    (void)uapi_snd_destroy_track(portTrackCtx->trackHandle);
SOUND_CLOSE:
    (void)CastSndClose(portTrackCtx->subPortCtx->castSndHandle);
    return ret;
}

static int32_t AudioSDKAttachInternal(const SubPortTrackContext *portTrackCtx)
{
    int32_t ret = 0;
    if (portTrackCtx->isHandlePublicResource) {
        ret = AudioSDKAttach(AUDIO_MOD_SOUND, portTrackCtx->castSndHandle, portTrackCtx->aencHandle);
        CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "sound attach aenc failed");

        ret = AudioSDKAttach(AUDIO_MOD_AENC, portTrackCtx->aencHandle, portTrackCtx->outputAdpHandle);
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "aenc attach adp failed");
            goto SND_DETACH_AENC;
        }
    }

    if (portTrackCtx->isRaw) {
        ret = AudioSDKAttach(AUDIO_MOD_ADP, portTrackCtx->inputAdpHandle, portTrackCtx->trackHandle);
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "adp attach track failed");
            goto AENC_DETACH_ADP;
        }
    } else {
        ret = AudioSDKAttach(AUDIO_MOD_ADP, portTrackCtx->inputAdpHandle, portTrackCtx->adecHandle);
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "adp attach adec failed");
            goto AENC_DETACH_ADP;
        }
        ret = AudioSDKAttach(AUDIO_MOD_ADEC, portTrackCtx->adecHandle, portTrackCtx->trackHandle);
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "adec attach track failed");
            goto ADP_DETACH_ADEC;
        }
    }

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;

ADP_DETACH_ADEC:
    (void)AudioSDKDetach(AUDIO_MOD_ADP, portTrackCtx->inputAdpHandle, portTrackCtx->adecHandle);
AENC_DETACH_ADP:
    (void)AudioSDKDetach(AUDIO_MOD_AENC, portTrackCtx->aencHandle, portTrackCtx->outputAdpHandle);
SND_DETACH_AENC:
    (void)AudioSDKDetach(AUDIO_MOD_SOUND, portTrackCtx->castSndHandle, portTrackCtx->aencHandle);
    return ret;
}

static int32_t BluetoothAttachInternal(const SubPortTrackContext *portTrackCtx)
{
#if defined(BT_HAL_SUPPORT) && defined(SUPPORT_BREDR)
    if (portTrackCtx->isHandlePublicResource) {
        bt_audio_port_params portParam = {};
        portParam.port_type = A2DP;
        portParam.share_mem_id = portTrackCtx->outputAdpHandle;
        td_u32 btRet = bt_attach_audio_port(portTrackCtx->btStreamHandle, &portParam);
        CHK_FAILED_RETURN(btRet, MEDIA_HAL_OK, MEDIA_HAL_ERR, "bt attach audio port failed");
    }
#endif
    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t AudioSDKStartInternal(const SubPortTrackContext *portTrackCtx)
{
    int32_t ret;
    uapi_adp_event_proc adpCallback = AdpEventCB;
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_adp_register_event_proc(portTrackCtx->inputAdpHandle, adpCallback,
        (td_void *)portTrackCtx));
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "register adp event failed");

    ret = uapi_snd_track_start(portTrackCtx->trackHandle, TD_NULL);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "sound track start failed");

    if (!portTrackCtx->isRaw) {
        MEDIA_HAL_DOFUNC_TIME(ret = uapi_adec_start(portTrackCtx->adecHandle));
        CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "adec start failed");
    }
    if (portTrackCtx->isHandlePublicResource) {
        MEDIA_HAL_DOFUNC_TIME(ret = uapi_aenc_start(portTrackCtx->aencHandle));
        CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "aenc start failed");
    }

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}
#endif

static int32_t ResumeTrackInBluetoothA2DPSourceNormalScene(SubPortTrackContext *portTrackCtx)
{
    // 蓝牙音乐+蓝牙通话交互场景，手表蓝牙去使能，此时手机和蓝牙设备同时断开，在音乐恢复的时候可能设备已经断开
    bool a2dpStreamClosed = (portTrackCtx->subPortCtx->btDevState == BT_AUDIO_A2DP_STREAM_CLOSED);
    CHK_COND_RETURN(a2dpStreamClosed, MEDIA_HAL_OK, "a2dp stream has been closed");

    uint32_t btRet = MEDIA_HAL_OK;
#ifdef MINI_MEMORY_SUPPORT
    MediaHalMutexLock(portTrackCtx->subPortCtx->a2dpTrackMutex);
    portTrackCtx->isHandlePublicResource = IsNeedHandlePublicResource();
    int32_t ret = SetPause(portTrackCtx->trackId, false);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, " resume track failed");
        MediaHalMutexUnLock(portTrackCtx->subPortCtx->a2dpTrackMutex);
        return MEDIA_HAL_ERR;
    }
    // 该判断放在设置状态后面的原因：两路混音场景，第一路恢复播放，需要重新搭建通路，
    // 第二路恢复播放，不需要搭建通路，如果判断放在前面，就会直接返回，不会设置状态,
    // 因此，当前代码逻辑简单理解，先设置状态，再判断是否搭建通路

    ret = AudioSDKOpenInternal(portTrackCtx);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "audio open failed");
        MediaHalMutexUnLock(portTrackCtx->subPortCtx->a2dpTrackMutex);
        return ret;
    }

    ret = AudioSDKAttachInternal(portTrackCtx);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "audio attach failed");
        goto AUDIO_CLOSE;
    }
    ret = BluetoothAttachInternal(portTrackCtx);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "bluetooth attach failed");
        goto AUDIO_DETACH;
    }
    ret = AudioSDKStartInternal(portTrackCtx);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "audio start failed");
        goto BT_DETACH;
    }
    portTrackCtx->isReleased = false;
    portTrackCtx->subPortCtx->a2dpTrackCount++;
    MediaHalMutexUnLock(portTrackCtx->subPortCtx->a2dpTrackMutex);
#if defined(BT_HAL_SUPPORT) && defined(SUPPORT_BREDR)
    if (portTrackCtx->isHandlePublicResource) {
        btRet = bt_start_audio_stream(portTrackCtx->btStreamHandle);
        if (btRet != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "bt start audio stream failed");
            goto AUDIO_STOP;
        }
    }
#endif
#else
#if defined(BT_HAL_SUPPORT) && defined(SUPPORT_BREDR)
    btRet = bt_start_audio_stream(portTrackCtx->btStreamHandle);
    CHK_FAILED_RETURN(btRet, MEDIA_HAL_OK, MEDIA_HAL_ERR, "bt start audio stream failed");
#endif
#endif

#ifdef BT_HAL_SUPPORT
    MEDIA_HAL_LOGI(MODULE_NAME, "success trackId (%d), bt device state (%d)",
        portTrackCtx->trackId, portTrackCtx->subPortCtx->btDevState);
#endif

    return MEDIA_HAL_OK;

#ifdef MINI_MEMORY_SUPPORT
#if defined(BT_HAL_SUPPORT) && defined(SUPPORT_BREDR)
AUDIO_STOP:
    (void)AudioSDKStopInternal(portTrackCtx);
#endif
BT_DETACH:
    (void)BluetoothDetachInternal(portTrackCtx);
AUDIO_DETACH:
    (void)AudioSDKDetachInternal(portTrackCtx);
AUDIO_CLOSE:
    (void)AudioSDKCloseInternal(portTrackCtx);
    MediaHalMutexUnLock(portTrackCtx->subPortCtx->a2dpTrackMutex);
    return ret;
#endif
}

static int32_t DisableTrackInBluetoothA2DPSinkNormalScene(const SubPortTrackContext *portTrackCtx)
{
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_LOGI(MODULE_NAME, "DisableTrackInBluetoothA2DPSinkNormalScene not support");
    return MEDIA_HAL_OK;
}

static int32_t TrackSetMuteInBluetoothA2DPSinkNormalScene(const SubPortTrackContext *portTrackCtx, bool mute)
{
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(mute);

    MEDIA_HAL_LOGI(MODULE_NAME, "TrackSetMuteInBluetoothA2DPSinkNormalScene not support");
    return MEDIA_HAL_OK;
}

static int32_t TrackGetMuteInBluetoothA2DPSinkNormalScene(const SubPortTrackContext *portTrackCtx, bool* mute)
{
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(mute);

    MEDIA_HAL_LOGI(MODULE_NAME, "TrackGetMuteInBluetoothA2DPSinkNormalScene not support");
    return MEDIA_HAL_OK;
}

static int32_t TrackSetVolumeInBluetoothA2DPSinkNormalScene(SubPortTrackContext *portTrackCtx, float volume)
{
    MEDIA_HAL_UNUSED(portTrackCtx);

    MEDIA_HAL_LOGI(MODULE_NAME, "set volume %f", volume);
    MEDIA_HAL_LOGI(MODULE_NAME, "TrackSetVolumeInBluetoothA2DPSinkNormalScene not support");
    return MEDIA_HAL_OK;
}

static int32_t TrackGetVolumeInBluetoothA2DPSinkNormalScene(const SubPortTrackContext *portTrackCtx, float* volume)
{
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(volume);

    MEDIA_HAL_LOGI(MODULE_NAME, "TrackGetVolumeInBluetoothA2DPSinkNormalScene not support");
    return MEDIA_HAL_OK;
}

static int32_t PauseTrackInBluetoothA2DPSinkNormalScene(SubPortTrackContext *portTrackCtx)
{
#ifdef BT_HAL_SUPPORT
    MEDIA_HAL_LOGI(MODULE_NAME, "success, cur id[%d] bt devState[%d], bt pause is unnecessary",
        portTrackCtx->trackId, portTrackCtx->subPortCtx->btDevState);
#else
    MEDIA_HAL_UNUSED(portTrackCtx);
#endif
    return MEDIA_HAL_OK;
}

static int32_t ResumeTrackInBluetoothA2DPSinkNormalScene(SubPortTrackContext *portTrackCtx)
{
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_LOGI(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t SendFrameInBluetoothA2DPSourceNormalScene(const SubPortTrackContext *portTrackCtx, const uint8_t *buffer,
    uint64_t requestBytes, uint64_t *replyBytes)
{
#ifdef AUDIO_SDK_SUPPORT
    int32_t ret;
    if (portTrackCtx->isRaw) {
        uapi_audio_frame audioFrame = {
            .interleaved = true,
            .bit_depth = portTrackCtx->inputAttr.bitWidth,
            .sample_rate = portTrackCtx->inputAttr.sampleRate,
            .channels = portTrackCtx->inputAttr.channelCount,
            .pcm_samples = ((uint32_t)portTrackCtx->inputAttr.sampleRate) / SAMPLERATE_TO_PTNUMPERFRM,
            .pcm_buffer = NULL,
            .bits_buffer = (int32_t *)buffer,
            .bits_bytes = (uint32_t)requestBytes,
            .frame_index = 0,
            .eos = false,
        };
        ret = uapi_adp_send_frame(portTrackCtx->inputAdpHandle, &audioFrame);
        if (ret != MEDIA_HAL_OK) {
            *replyBytes = 0;
            return MEDIA_HAL_ERR_AGAIN;
        }
        *replyBytes = requestBytes;
    } else {
        uapi_stream_buf streamBuf = {};
        streamBuf.data = (uint8_t *)buffer;
        streamBuf.size = (uint32_t)requestBytes;
        streamBuf.pts = 0;
        streamBuf.eos = (buffer == NULL && requestBytes == 0) ? TD_TRUE : TD_FALSE;
        ret = uapi_adp_send_stream(portTrackCtx->inputAdpHandle, &streamBuf);
        if (ret != MEDIA_HAL_OK) {
            *replyBytes = 0;
            return MEDIA_HAL_ERR_AGAIN;
        }
        *replyBytes = requestBytes;
    }
#else
    MEDIA_HAL_UNUSED(portTrackCtx);
#endif

    return MEDIA_HAL_OK;
}

static int32_t SendFrameInBluetoothA2DPSinkNormalScene(const SubPortTrackContext *portTrackCtx, const uint8_t* buffer,
    uint64_t requestBytes, uint64_t* replyBytes)
{
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(buffer);
    MEDIA_HAL_UNUSED(requestBytes);
    MEDIA_HAL_UNUSED(replyBytes);
    MEDIA_HAL_LOGI(MODULE_NAME, "SendFrameInBluetoothA2DPSinkNormalScene not support");
    return MEDIA_HAL_OK;
}

static int32_t GetPositionInBluetoothA2DPSinkNormalScene(SubPortTrackContext *portTrackCtx, uint64_t *frames,
    struct AudioTimeStamp *time)
{
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(frames);
    MEDIA_HAL_UNUSED(time);

    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

static int32_t InvokeInBluetoothA2DPSinkNormalScene(SubPortTrackContext *portTrackCtx, enum InvokeID invokeID,
    const struct InvokeAttr* invokeAttr)
{
    char key[MAX_KEY_STR_LEN] = { 0 };
    uint32_t scoShmId = 0;
    int32_t ret;
    switch (invokeID) {
        case INVOKE_ID_SET_EXTRA_PARAMS:
            ret = sscanf_s((const char*)invokeAttr->request, "%s = %u", key, MAX_KEY_STR_LEN, &scoShmId);
            if (ret == MEDIA_HAL_ERR) {
                MEDIA_HAL_LOGI(MODULE_NAME, "sscanf_s failed");
                return ret;
            }
            MEDIA_HAL_LOGI(MODULE_NAME, "extra param key = %s, value = 0x%x", key, scoShmId);
            break;
        default:
            MEDIA_HAL_LOGW(MODULE_NAME, "invalid invoke id: %d", invokeID);
            return MEDIA_HAL_INVALID_PARAM;
    }

#if defined(BT_HAL_SUPPORT) && defined(SUPPORT_BREDR)
    portTrackCtx->a2dpSinkMemId = scoShmId;
    bt_audio_port_params portParam = {};
    portParam.port_type = A2DP;
    portParam.share_mem_id = scoShmId;
    td_u32 btRet = bt_attach_audio_port(portTrackCtx->btStreamHandle, &portParam);
    CHK_FAILED_RETURN(btRet, MEDIA_HAL_OK, MEDIA_HAL_ERR, "bt_attach_audio_port failed");
#endif
    MEDIA_HAL_LOGI(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t RegCallbackInBluetoothA2DPSinkNormalScene(SubPortTrackContext *portTrackCtx,
    TrackCallback callback, const AudioHandle caller)
{
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(callback);
    MEDIA_HAL_UNUSED(caller);

    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

static int32_t GetPositionInBluetoothA2DPSourceNormalScene(SubPortTrackContext *portTrackCtx, uint64_t *frames,
    struct AudioTimeStamp *time)
{
    MEDIA_HAL_UNUSED(frames);

#ifdef MINI_MEMORY_SUPPORT
    if (portTrackCtx->isReleased) {
        MEDIA_HAL_UNUSED(portTrackCtx);
        MEDIA_HAL_UNUSED(time);
        MEDIA_HAL_LOGI(MODULE_NAME, "have released");
        return MEDIA_HAL_OK;
    }
#endif
    return UpdateCurrentPosition(portTrackCtx->trackHandle, time);
}

static int32_t InvokeInBluetoothA2DPSourceNormalScene(SubPortTrackContext *portTrackCtx, enum InvokeID invokeID,
    const struct InvokeAttr *invokeAttr)
{
    SubPortTrackContext *portTrackOutCtx = (SubPortTrackContext *)portTrackCtx;
    char keyFlag[MAX_KEY_STR_SIZE] = { 0 };
    char value[MAX_KEY_VALUE_SIZE] = { 0 };
    int32_t ret = 0;
    switch (invokeID) {
        case INVOKE_ID_SET_EXTRA_PARAMS:
            ret = ParseKeyValueForExtra(invokeAttr, keyFlag, MAX_KEY_STR_SIZE, value, MAX_KEY_VALUE_SIZE);
            CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "ParseKeyValueForExtra failed");
            if (strcmp(keyFlag, "extradata") == 0) {
                ret = SetExtraData(&portTrackOutCtx->inputAttr, value, (uint32_t)(strlen(value) + 1));
                CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "SetExtraData failed");
            }
            break;
        default:
            MEDIA_HAL_LOGW(MODULE_NAME, "invalid invoke id: %d", invokeID);
            return MEDIA_HAL_INVALID_PARAM;
    }

    MEDIA_HAL_LOGD(MODULE_NAME, "invoke %d success", invokeID);
    return MEDIA_HAL_OK;
}

static int32_t RegCallbackInBluetoothA2DPSourceNormalScene(SubPortTrackContext *portTrackCtx,
    TrackCallback callback, const AudioHandle caller)
{
    portTrackCtx->callback = callback;
    portTrackCtx->caller = caller;
    return MEDIA_HAL_OK;
}

static SampleFreqIndex ConvertSampleFreqTemp(AudioSampleRatesTemp sampleRate)
{
    switch (sampleRate) {
        case AUDIO_SAMPLE_FREQ_8000: {
            return SAMPLE_FREQ_INDEX_8000;
        }
        case AUDIO_SAMPLE_FREQ_12000: {
            return SAMPLE_FREQ_INDEX_12000;
        }
        case AUDIO_SAMPLE_FREQ_11025: {
            return SAMPLE_FREQ_INDEX_11025;
        }
        case AUDIO_SAMPLE_FREQ_16000: {
            return SAMPLE_FREQ_INDEX_16000;
        }
        case AUDIO_SAMPLE_FREQ_22050: {
            return SAMPLE_FREQ_INDEX_22050;
        }
        case AUDIO_SAMPLE_FREQ_24000: {
            return SAMPLE_FREQ_INDEX_24000;
        }
        case AUDIO_SAMPLE_FREQ_32000: {
            return SAMPLE_FREQ_INDEX_32100;
        }
        case AUDIO_SAMPLE_FREQ_44100: {
            return SAMPLE_FREQ_INDEX_44100;
        }
        case AUDIO_SAMPLE_FREQ_48000: {
            return SAMPLE_FREQ_INDEX_48000;
        }
        case AUDIO_SAMPLE_FREQ_64000: {
            return SAMPLE_FREQ_INDEX_64000;
        }
        case AUDIO_SAMPLE_FREQ_96000: {
            return SAMPLE_FREQ_INDEX_96000;
        }
        default: {
            MEDIA_HAL_LOGI(MODULE_NAME, "should not be here[%d]", sampleRate);
            return SAMPLE_FREQ_INDEX_INVALID;
        }
    }
}

static ChannelConfiguration ConvertChannelCount(uint32_t channelCount)
{
    if (channelCount != SINGLE_CHANNEL && channelCount != PAIR_CHANNEL) {
        return INVALID_CHANNEL_COUNT;
    }
    return (ChannelConfiguration)channelCount;
}

static int32_t CreateTrackInBluetoothA2DPLowDelayScene(SubPortTrackContext *portTrackCtx)
{
    MEDIA_HAL_LOGD(MODULE_NAME, "audioFormat[%d] sampleRate[%d] channelCount[%u]",
        portTrackCtx->inputAttr.audioFormat, portTrackCtx->inputAttr.sampleRate, portTrackCtx->inputAttr.channelCount);
    portTrackCtx->audBasicParamForLATM.audioObjType =
        portTrackCtx->inputAttr.audioFormat == AUDIO_FORMAT_AAC_LC ? AOT_AAC_LC : AOT_NULL;
    AudioSampleRatesTemp sampleRate = (AudioSampleRatesTemp)portTrackCtx->inputAttr.sampleRate;
    portTrackCtx->audBasicParamForLATM.sampleFreqIndex = ConvertSampleFreqTemp(sampleRate);
    portTrackCtx->audBasicParamForLATM.channelConfig = ConvertChannelCount(portTrackCtx->inputAttr.channelCount);
    bool isInvalidParam = portTrackCtx->audBasicParamForLATM.audioObjType == AOT_NULL ||
        portTrackCtx->audBasicParamForLATM.sampleFreqIndex == SAMPLE_FREQ_INDEX_INVALID ||
        portTrackCtx->audBasicParamForLATM.channelConfig == INVALID_CHANNEL_COUNT;
    if (isInvalidParam) {
        return MEDIA_HAL_ERR;
    }

    return MEDIA_HAL_OK;
}

static int32_t DestroyTrackInBluetoothA2DPLowDelayScene(const SubPortTrackContext *portTrackCtx)
{
    MEDIA_HAL_UNUSED(portTrackCtx);

    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

static int32_t EnableTrackInBluetoothA2DPLowDelayScene(SubPortTrackContext *portTrackCtx)
{
#ifdef BT_HAL_LOW_DELAY_SUPPORT
    int32_t ret = bt_start_audio_stream(portTrackCtx->btStreamHandle);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "bt_start_audio_stream failed");
    sleep(1); // workaround, bt start is async, wait 1 sec
#else
    MEDIA_HAL_UNUSED(portTrackCtx);
#endif

    MEDIA_HAL_LOGD(MODULE_NAME, "and wait 1s success");
    return MEDIA_HAL_OK;
}

static int32_t DisableTrackInBluetoothA2DPLowDelayScene(const SubPortTrackContext *portTrackCtx)
{
#ifdef BT_HAL_LOW_DELAY_SUPPORT
    int32_t ret = bt_stop_audio_stream(portTrackCtx->btStreamHandle);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "bt_stop_audio_stream failed");
#else
    MEDIA_HAL_UNUSED(portTrackCtx);
#endif

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t TrackSetMuteInBluetoothA2DPLowDelayScene(const SubPortTrackContext *portTrackCtx, bool mute)
{
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(mute);

    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

static int32_t TrackGetMuteInBluetoothA2DPLowDelayScene(const SubPortTrackContext *portTrackCtx, bool *mute)
{
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(mute);

    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

static int32_t TrackSetVolumeInBluetoothA2DPLowDelayScene(SubPortTrackContext *portTrackCtx, float volume)
{
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(volume);

    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

static int32_t TrackGetVolumeInBluetoothA2DPLowDelayScene(const SubPortTrackContext *portTrackCtx, float *volume)
{
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(volume);

    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

static int32_t PauseTrackInBluetoothA2DPLowDelayScene(SubPortTrackContext *portTrackCtx)
{
#ifdef BT_HAL_LOW_DELAY_SUPPORT
#if defined(BT_HAL_SUPPORT) && defined(SUPPORT_BREDR)
    int32_t ret = bt_pause_audio_stream(portTrackCtx->btStreamHandle);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "bt_pause_audio_stream failed");
#endif
#else
    MEDIA_HAL_UNUSED(portTrackCtx);
#endif

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t ResumeTrackInBluetoothA2DPLowDelayScene(SubPortTrackContext *portTrackCtx)
{
    MEDIA_HAL_UNUSED(portTrackCtx);

    MEDIA_HAL_LOGD(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

static int32_t SendFrameInBluetoothA2DPLowDelayScene(const SubPortTrackContext *portTrackCtx, const uint8_t *buffer,
    uint64_t requestBytes, uint64_t *replyBytes)
{
#ifdef BT_HAL_LOW_DELAY_SUPPORT
    uint32_t adtsHeaderByteNum =
        ((buffer[0] & 0x01) == 1) ? ADTS_HEADER_BYTE_NUM_NO_CRC : ADTS_HEADER_BYTE_NUM_WITH_CRC;
    // 分配空间 --> 只分配一次 pend
    uint8_t *sendBuf = (uint8_t *)malloc(MAX_BUF_LEN);
    CHK_NULL_RETURN(sendBuf, MEDIA_HAL_ERR, "sendBuf malloc err");

    // latm头形成
    uint32_t esDatasize = (uint32_t)requestBytes - adtsHeaderByteNum;
    uint32_t bitNumOfHeader = 0;
    ConstructLatmHeader12Byte(&portTrackCtx->audBasicParamForLATM, esDatasize, &bitNumOfHeader, sendBuf, MAX_BUF_LEN);

    // latm头和ES数据拼接
    uint32_t offset_tmp = bitNumOfHeader / BIT_NUM_ONE_BYTE;
    const uint8_t *pureEsData = buffer + adtsHeaderByteNum;
    if (memcpy_s(&sendBuf[offset_tmp], MAX_BUF_LEN - offset_tmp, pureEsData, esDatasize) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "memcpy fail!");
        free(sendBuf);
        return MEDIA_HAL_ERR;
    }
    int32_t ret = bt_write_audio_data(portTrackCtx->btStreamHandle, (td_void *)sendBuf, offset_tmp + esDatasize);
    free(sendBuf);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGW(MODULE_NAME, "bt_write_audio_data failed");
        return ret;
    }
    *replyBytes = requestBytes; // 如果不装填，上面会认为 render 这边已经满了，就会持续送同样的帧
#else
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(buffer);
    MEDIA_HAL_UNUSED(requestBytes);
    MEDIA_HAL_UNUSED(replyBytes);
#endif
    return MEDIA_HAL_OK;
}

static int32_t GetPositionInBluetoothA2DPLowDelayScene(SubPortTrackContext *portTrackCtx, uint64_t *frames,
    struct AudioTimeStamp *time)
{
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(frames);
    MEDIA_HAL_UNUSED(time);

    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

static int32_t InvokeInBluetoothA2DPLowDelayScene(SubPortTrackContext *portTrackCtx, enum InvokeID invokeID,
    const struct InvokeAttr *invokeAttr)
{
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(invokeID);
    MEDIA_HAL_UNUSED(invokeAttr);

    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

static int32_t RegCallbackInBluetoothA2DPLowDelayScene(SubPortTrackContext *portTrackCtx, TrackCallback callback,
    const AudioHandle caller)
{
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(callback);
    MEDIA_HAL_UNUSED(caller);

    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

int32_t AudioBluetoothSubPortTrackGetLatency(AudioHandle trackHandle, uint32_t *latencyMs)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;

    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(latencyMs);

    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

int32_t AudioBluetoothSubPortEnableTrack(AudioHandle trackHandle)
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

int32_t AudioBluetoothSubPortDisableTrack(AudioHandle trackHandle)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;

    int32_t ret = g_funcsList[portTrackCtx->workScene].disableTrack(portTrackCtx);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "work scene %d disable track failed", portTrackCtx->workScene);
    }

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

int32_t AudioBluetoothSubPortPauseTrack(AudioHandle trackHandle)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;

    int32_t ret = g_funcsList[portTrackCtx->workScene].pauseTrack(portTrackCtx);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "work scene %d pause track failed", portTrackCtx->workScene);
    }

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

int32_t AudioBluetoothSubPortResumeTrack(AudioHandle trackHandle)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;

    int32_t ret = g_funcsList[portTrackCtx->workScene].resumeTrack(portTrackCtx);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "work scene %d resume track failed", portTrackCtx->workScene);
    }

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

int32_t AudioBluetoothSubPortFlushTrack(AudioHandle trackHandle)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_LOGD(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

int32_t AudioBluetoothSubPortAcquireFrame(AudioHandle trackHandle, uint8_t *buffer, uint64_t requestBytes,
    uint64_t *replyBytes)
{
    /* for bluetooth headset recording scenario */
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;

    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(buffer);
    MEDIA_HAL_UNUSED(requestBytes);
    MEDIA_HAL_UNUSED(replyBytes);

    MEDIA_HAL_LOGD(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

int32_t AudioBluetoothSubPortGetPosition(AudioHandle trackHandle, uint64_t *frames, struct AudioTimeStamp *time)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;

    CHK_FAILED_RETURN_NO_LOG(portTrackCtx->workScene, BLUETOOTH_OUT_A2DP_NORMAL, MEDIA_HAL_OK);

    int32_t ret = g_funcsList[portTrackCtx->workScene].getPosition(portTrackCtx, frames, time);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "work scene %d get position failed", portTrackCtx->workScene);
        return ret;
    }

    return MEDIA_HAL_OK;
}

int32_t AudioBluetoothSubPortSendFrame(AudioHandle trackHandle, const uint8_t *buffer, uint64_t requestBytes,
    uint64_t *replyBytes)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;

    return g_funcsList[portTrackCtx->workScene].sendFrame(portTrackCtx, buffer, requestBytes, replyBytes);
}

int32_t AudioBluetoothSubPortTrackSetMute(AudioHandle trackHandle, bool mute)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;

    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(mute);

    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

int32_t AudioBluetoothSubPortTrackGetMute(AudioHandle trackHandle, bool *mute)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;

    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(mute);

    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

int32_t AudioBluetoothSubPortTrackSetVolume(AudioHandle trackHandle, float volume)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;

    int32_t ret = g_funcsList[portTrackCtx->workScene].trackSetVolume(portTrackCtx, volume);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "work scene %d set volume failed", portTrackCtx->workScene);
        return ret;
    }

    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

int32_t AudioBluetoothSubPortTrackGetVolume(AudioHandle trackHandle, float *volume)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;

    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(volume);

    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

int32_t AudioBluetoothSubPortInvoke(AudioHandle trackHandle, enum InvokeID invokeID, struct InvokeAttr *invokeAttr)
{
    /* for set shm id in voice call scenario */
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;

    int32_t ret = g_funcsList[portTrackCtx->workScene].invoke(portTrackCtx, invokeID, invokeAttr);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "work scene %d invoke failed", portTrackCtx->workScene);
        return ret;
    }

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

#ifdef BT_HAL_SUPPORT
static bool ConvertSampleFrequencyToSampleRate(td_u32 sampleFrequency, uapi_audio_sample_rate *sampleRate)
{
    MEDIA_HAL_LOGI(MODULE_NAME, "sampleFrequency: 0x%x", sampleFrequency);

    switch (sampleFrequency) {
        case BT_AUDIO_A2DP_SBC_SF_16000:
            *sampleRate = UAPI_AUDIO_SAMPLE_RATE_16K;
            break;
        case BT_AUDIO_A2DP_SBC_SF_32000:
            *sampleRate = UAPI_AUDIO_SAMPLE_RATE_32K;
            break;
        case BT_AUDIO_A2DP_SBC_SF_44100:
            *sampleRate = UAPI_AUDIO_SAMPLE_RATE_44K;
            break;
        case BT_AUDIO_A2DP_SBC_SF_48000:
            *sampleRate = UAPI_AUDIO_SAMPLE_RATE_48K;
            break;
        default:
            MEDIA_HAL_LOGE(MODULE_NAME, "invalid sample frequency: 0x%x", sampleFrequency);
            return false;
    }

    return true;
}
#endif

#ifdef BT_HAL_SUPPORT
static bool ConvertChnlModeToChannelCount(td_u8 chnlMode, uapi_audio_channel *channelCount)
{
    MEDIA_HAL_LOGI(MODULE_NAME, "chnlMode: 0x%x", chnlMode);

    switch (chnlMode) {
        case BT_AUDIO_A2DP_SBC_CHMODE_MONO:
            *channelCount = UAPI_AUDIO_CHANNEL_1;
            break;
        case BT_AUDIO_A2DP_SBC_CHMODE_DUAL_CHANNEL:
            *channelCount = UAPI_AUDIO_CHANNEL_2;
            break;
        case BT_AUDIO_A2DP_SBC_CHMODE_STEREO:
            *channelCount = UAPI_AUDIO_CHANNEL_2;
            break;
        case BT_AUDIO_A2DP_SBC_CHMODE_JOINT_STEREO:
            *channelCount = UAPI_AUDIO_CHANNEL_2;
            break;
        default:
            MEDIA_HAL_LOGE(MODULE_NAME, "invalid channel mode: 0x%x", chnlMode);
            return false;
    }
    return true;
}
#endif

static bool HaveActiveTrack(const SubPortContext *portCtx)
{
    for (int32_t i = 0; i < BT_PORT_MAX_TRACK_NUM; i++) {
        if (portCtx->portTrack[i] != NULL) {
            return true;
        }
    }
    return false;
}

#ifdef BT_HAL_SUPPORT
static int32_t Reconfigure(SubPortContext *portCtx)
{
    bt_a2dp_codec_param param1 = {};
    td_u32 btRet =
        bt_get_audio_parameter(portCtx->btA2dpStreamHandle, BT_AUDIO_PARAM_A2DP_CODEC, &param1, sizeof(param1));
    CHK_FAILED_RETURN(btRet, MEDIA_HAL_OK, MEDIA_HAL_ERR, "bt_get_audio_parameter failed");

    if (param1.codec_type != BT_AUDIO_CODEC_SBC) {
        MEDIA_HAL_LOGE(MODULE_NAME, "unsupport codec type: 0x%x", param1.codec_type);
        return MEDIA_HAL_ERR;
    }

    bt_a2dp_sbc_codec_caps *caps = (bt_a2dp_sbc_codec_caps *)param1.codec_caps;
    MEDIA_HAL_LOGI(MODULE_NAME, "sbc sampleFreq: %02x chnlMode: %02x blockLen: %02x subBand: %02x \
        allocMtd: %02x minBitpool: %02x, maxBitpool: %02x",
        caps->sample_frequency, caps->chnl_mode, caps->block_length, caps->subband, caps->alloc_method,
        caps->min_bitpool, caps->max_bitpool);
    bool success = ConvertSampleFrequencyToSampleRate(caps->sample_frequency, &portCtx->sampleRate);
    CHK_COND_RETURN(!success, MEDIA_HAL_INVALID_PARAM, "invalid sample frequency");
    success = ConvertChnlModeToChannelCount(caps->chnl_mode, &portCtx->channelCount);
    CHK_COND_RETURN(!success, MEDIA_HAL_INVALID_PARAM, "invalid channel mode");

    bt_a2dp_codec_param param2 = portCtx->btA2dpConfigChgdata.codec;
    if (param2.codec_type != BT_AUDIO_CODEC_SBC) {
        MEDIA_HAL_LOGE(MODULE_NAME, "unsupport codec type: 0x%x", param2.codec_type);
        return MEDIA_HAL_ERR;
    }

    // 蓝牙音乐暂停时，蓝牙设备断开、重连，此时BTH会上报属性改变，我们只需记录下来，不作用到AENC模块，
    // 恢复播放的时候，再将新属性作用上去
    MediaHalMutexLock(portCtx->a2dpTrackMutex);
    if (portCtx->a2dpTrackCount == 0) {
        MEDIA_HAL_LOGI(MODULE_NAME, "no a2dp track");
        MediaHalMutexUnLock(portCtx->a2dpTrackMutex);
        return MEDIA_HAL_OK;
    }
    MediaHalMutexUnLock(portCtx->a2dpTrackMutex);

    uapi_aenc_attr aencAttr = {};
    aencAttr.codec_id = UAPI_ACODEC_ID_SBC;
    aencAttr.max_trans_unit_size = param2.payload;
    aencAttr.param.interleaved = true;
    aencAttr.param.channels = portCtx->channelCount;
    aencAttr.param.bit_depth = AUDIO_BIT_WIDTH_16_BIT;
    aencAttr.param.sample_rate = portCtx->sampleRate;
    aencAttr.param.samples_per_frame = ((uint32_t)portCtx->sampleRate) / SAMPLERATE_TO_PTNUMPERFRM;
    aencAttr.param.private_data = (td_void *)(portCtx->btA2dpConfigChgdata.codec.codec_caps);
    aencAttr.param.private_data_size = param2.cap_len;

    int32_t ret;
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_aenc_set_attr(portCtx->aencHandle, &aencAttr));
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_aenc_set_attr failed");

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}
#endif

#ifdef BT_HAL_SUPPORT
static td_void GetBtA2dpConfigData(const td_void *data, SubPortContext *portCtx)
{
    bt_audio_a2dp_config_chg_data configData = *(bt_audio_a2dp_config_chg_data *)data;
    portCtx->btA2dpConfigChgdata.stream_hdl = configData.stream_hdl;
    portCtx->btA2dpConfigChgdata.codec.codec_type = configData.codec.codec_type;
    portCtx->btA2dpConfigChgdata.codec.cap_len = configData.codec.cap_len;
    portCtx->btA2dpConfigChgdata.codec.payload = configData.codec.payload;

    if (memcpy_s(portCtx->btA2dpConfigChgdata.codec.codec_caps, BT_A2DP_CODECCAPS_MAX_LEN, configData.codec.codec_caps,
        configData.codec.cap_len) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "memcpy fail!");
    }

    MEDIA_HAL_LOGI(MODULE_NAME, "codec type: %d, payload: %d, cap_len: %d", configData.codec.codec_type,
        configData.codec.payload, configData.codec.cap_len);
    for (int32_t i = 0; i < configData.codec.cap_len; i++) {
        MEDIA_HAL_LOGI(MODULE_NAME, "codec_caps[%u]:%02x ", i, configData.codec.codec_caps[i]);
    }
    portCtx->btDevState = BT_AUDIO_A2DP_STREAM_CONFIG_CHANGE;

    if (HaveActiveTrack(portCtx)) {
        (void)Reconfigure(portCtx);
    }
}

static void UpdateBluetoothA2dpStreamHandle(SubPortContext *portCtx)
{
    CHK_NULL_RETURN_NONE(portCtx->btA2dpStreamHandle, "a2dp stream handle is null");

    if (!HaveActiveTrack(portCtx)) {
        MEDIA_HAL_LOGD(MODULE_NAME, "bluetooth a2dp stream handle no update required because no active track");
        return;
    }

    for (uint32_t i = 0; i < BT_PORT_MAX_TRACK_NUM; i++) {
        if (portCtx->portTrack[i] == NULL || portCtx->portTrack[i]->workScene != BLUETOOTH_OUT_A2DP_NORMAL) {
            continue;
        }
        portCtx->portTrack[i]->btStreamHandle = portCtx->btA2dpStreamHandle;
    }
}

static td_void BtEventCB(bt_audio_event_type type, const td_void *data, td_s32 size, td_void *portContext)
{
    MEDIA_HAL_LOGI(MODULE_NAME, "event type = %d, size = %d", type, size);
    SubPortContext *portCtx = (SubPortContext *)portContext;
    switch (type) {
        case BT_AUDIO_A2DP_SNK_STREAM_CREATE:
            portCtx->btA2dpSinkStreamHandle = *((td_pvoid *)data);
            break;
        case BT_AUDIO_A2DP_SNK_STREAM_CLOSED:
            portCtx->btA2dpSinkStreamHandle = NULL;
            break;
        case BT_AUDIO_A2DP_STREAM_CREATE:
            portCtx->btA2dpStreamHandle = *((td_pvoid *)data);
            // 实时更新蓝牙A2DP流句柄，比如蓝牙音乐暂停时刻，蓝牙设备断开再连接，
            // 流句柄会变化，需要同步更新到内部维护的track中
            UpdateBluetoothA2dpStreamHandle(portCtx);
            portCtx->btDevState = BT_AUDIO_A2DP_STREAM_CREATE;
            break;
        case BT_AUDIO_A2DP_STREAM_OPENED: {
            // 首次播放通过OPENED事件上报准确mtu值，需要传给DSP编码打包，此时CONFIG_CHANGE事件上报的是无效值，
            // 播放中途CONFIG_CHANGE会上报准确mtu值
            bt_audio_a2dp_stream_open_data openData = *(bt_audio_a2dp_stream_open_data *)data;
            portCtx->btA2dpConfigChgdata.codec.payload = openData.stream_mtu;
        }
            portCtx->btDevState = BT_AUDIO_A2DP_STREAM_OPENED;
            break;
        case BT_AUDIO_A2DP_STREAM_STRAT:
            portCtx->btDevState = BT_AUDIO_A2DP_STREAM_STRAT;
            break;
        case BT_AUDIO_A2DP_STREAM_SUSPENDED:
            portCtx->btDevState = BT_AUDIO_A2DP_STREAM_SUSPENDED;
            break;
        case BT_AUDIO_A2DP_STREAM_CLOSED:
            portCtx->btA2dpStreamHandle = NULL;
            portCtx->btDevState = BT_AUDIO_A2DP_STREAM_CLOSED;
            break;
        case BT_AUDIO_A2DP_STREAM_CONFIG_CHANGE: {
            GetBtA2dpConfigData(data, portCtx);
            break;
        }
        case BT_AUDIO_SCO_CONN_ESTABLISHED:
            portCtx->btScoStreamHandle = *((td_pvoid *)data);
            portCtx->btDevState = BT_AUDIO_SCO_CONN_ESTABLISHED;
            break;
        case BT_AUDIO_SCO_CONN_RELEASED:
            portCtx->btScoStreamHandle = NULL;
            portCtx->btDevState = BT_AUDIO_SCO_CONN_RELEASED;
            break;
        default:
            MEDIA_HAL_LOGI(MODULE_NAME, "invalid callback event type: %d", type);
            break;
    }
}
#endif

int32_t AudioBluetoothSubPortCreate(AudioHandle *portHandle)
{
    SubPortContext *portCtx = (SubPortContext *)malloc(sizeof(SubPortContext));
    if (portCtx == NULL) {
        MEDIA_HAL_LOGI(MODULE_NAME, "malloc failed");
        return MEDIA_HAL_NO_MEM;
    }

    int32_t ret = MEDIA_HAL_OK;
    uint32_t btRet = MEDIA_HAL_OK;
    MediaHalMutexAttr attr = { false };
    if (memset_s(portCtx, sizeof(SubPortContext), 0, sizeof(SubPortContext)) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "memset_s failed");
        ret = MEDIA_HAL_ERR;
        goto FREE;
    }

    portCtx->a2dpTrackMutex = MediaHalMutexCreate(&attr);
    if (portCtx->a2dpTrackMutex == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "create mutex failed");
        goto FREE;
    }

    ret = SubPortTrackManagerInit();
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "SubPortTrackManagerInit failed");
        goto DESTROY_MUTEX;
    }
#if defined(BT_HAL_SUPPORT) && defined(SUPPORT_BREDR)
    btRet = (uint32_t)bt_register_audio_listener(BtEventCB, portCtx);
    if (btRet != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "bt_register_audio_listener failed, ret:0x%x", btRet);
        ret = MEDIA_HAL_ERR;
        goto DESTROY_MUTEX;
    }
#endif
    *portHandle = portCtx;
    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;

DESTROY_MUTEX:
    MediaHalMutexDestroy(&portCtx->a2dpTrackMutex);
FREE:
    free(portCtx);
    return ret;
}

int32_t AudioBluetoothSubPortDestroy(AudioHandle portHandle)
{
    SubPortContext *portCtx = (SubPortContext *)portHandle;

#if defined(BT_HAL_SUPPORT) && defined(SUPPORT_BREDR)
    td_u32 btRet = bt_deregister_audio_listener(BtEventCB);
    CHK_FAILED_NO_RETURN(btRet, MEDIA_HAL_OK, "bt_deregister_audio_listener failed");
#endif

    int32_t ret = SubPortTrackManagerDeInit();
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "SubPortTrackManagerDeInit failed");

    MediaHalMutexDestroy(&portCtx->a2dpTrackMutex);

    free(portCtx);

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

int32_t AudioBluetoothSubPortGetCapability(const struct AudioPort *port, struct AudioPortCapability *capability)
{
    capability->formats = g_supportFormats;
    MEDIA_HAL_UNUSED(port);
    MEDIA_HAL_UNUSED(capability);
    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}


static int32_t BluetoothAttach(SubPortContext *portCtx, SubPortTrackContext *portTrackCtx)
{
#if defined(BT_HAL_SUPPORT) && defined(SUPPORT_BREDR)
    bt_audio_port_params portParam = {};
    portParam.port_type = A2DP;
    portParam.share_mem_id = portCtx->outputAdpHandle;
    td_u32 btRet = bt_attach_audio_port(portTrackCtx->btStreamHandle, &portParam);
    CHK_FAILED_RETURN(btRet, MEDIA_HAL_OK, MEDIA_HAL_ERR, "bt attach audio port failed");
#endif
    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t BluetoothDetach(const SubPortContext *portCtx, const SubPortTrackContext *portTrackCtx)
{
    int32_t ret = MEDIA_HAL_OK;
#if defined(BT_HAL_SUPPORT) && defined(SUPPORT_BREDR)
    bt_audio_port_params portParam = {};
    portParam.port_type = A2DP;
    portParam.share_mem_id = portCtx->outputAdpHandle;
    ret = BtDetachHandle(portTrackCtx, portParam);
#endif
    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return ret;
}

static int32_t SingletonResourceStartInternal(const SubPortContext *portCtx, SubPortTrackContext *portTrackCtx)
{
#ifdef AUDIO_SDK_SUPPORT
    portTrackCtx->aencHandle = portCtx->aencHandle;
    int32_t ret = StartByAudioModule(portTrackCtx, AUDIO_MOD_AENC);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "aenc start failed");
#endif
#if defined(BT_HAL_SUPPORT) && defined(SUPPORT_BREDR)
    td_u32 btRet = bt_start_audio_stream(portTrackCtx->btStreamHandle);
    if (btRet != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "bt start audio stream failed");
        (void)StopByAudioModule(portTrackCtx, AUDIO_MOD_AENC);
        return MEDIA_HAL_ERR;
    }
    MEDIA_HAL_LOGI(MODULE_NAME, "trackId (%d), bt device state (%d)", portTrackCtx->trackId, portCtx->btDevState);
#endif

    MEDIA_HAL_LOGD(MODULE_NAME, "success trackId (%d)", portTrackCtx->trackId);
    return MEDIA_HAL_OK;
}

static int32_t EstablishPipelineInternal(SubPortContext *portCtx, SubPortTrackContext *portTrackCtx,
    int32_t trackCount)
{
#ifdef AUDIO_SDK_SUPPORT
    if (trackCount > 1) {
        MEDIA_HAL_LOGI(MODULE_NAME, "in the multi-instance scenario, only one aenc is required");
        return MEDIA_HAL_OK;
    }

    uapi_aenc_attr aencAttr = {};
    int32_t ret = InitAencAttr(portCtx, &aencAttr);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "InitAencAttr failed");

    ret = AudioSDKAencOpen(&portCtx->aencHandle, aencAttr);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "aenc open failed");

    ret = AudioSDKAdpOpen(&portCtx->outputAdpHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "adp open failed");
        goto AENC_CLOSE;
    }
    ret = AudioSDKAttach(AUDIO_MOD_SOUND, portCtx->castSndHandle, portCtx->aencHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "sound attach aenc failed");
        goto ADP_CLOSE;
    }
    ret = AudioSDKAttach(AUDIO_MOD_AENC, portCtx->aencHandle, portCtx->outputAdpHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "aenc attach adp failed");
        goto SND_DETACH_AENC;
    }
#endif
    ret = BluetoothAttach(portCtx, portTrackCtx);
    if (ret != MEDIA_HAL_OK) {
        goto SND_DETACH_AENC;
    }

    ret = SingletonResourceStartInternal(portCtx, portTrackCtx);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "singleton resource start internal failed");
        goto BLUETOOTH_DETACH;
    }
    return MEDIA_HAL_OK;
BLUETOOTH_DETACH:
    (void)BluetoothDetach(portCtx, portTrackCtx);
#ifdef AUDIO_SDK_SUPPORT
SND_DETACH_AENC:
    (void)AudioSDKDetach(AUDIO_MOD_AENC, portCtx->aencHandle, portCtx->outputAdpHandle);
    (void)AudioSDKDetach(AUDIO_MOD_SOUND, portCtx->castSndHandle, portCtx->aencHandle);
ADP_CLOSE:
    (void)AudioSDKAdpClose(portCtx->outputAdpHandle);
AENC_CLOSE:
    (void)AudioSDKAencClose(portCtx->aencHandle);
#endif
    return ret;
}

static void IncrementTrackCount(SubPortTrackContext *portTrackCtx)
{
    if (portTrackCtx->workScene == BLUETOOTH_IN_A2DP_NORMAL) {
        return;
    }
    if (portTrackCtx->workScene == BLUETOOTH_SCO) {
        portTrackCtx->subPortCtx->scoTrackCount++;
    } else {
        portTrackCtx->subPortCtx->a2dpTrackCount++;
    }
}

static void DecreaseTrackCount(SubPortTrackContext *portTrackCtx)
{
    if (portTrackCtx->workScene == BLUETOOTH_IN_A2DP_NORMAL) {
        return;
    }
    if (portTrackCtx->workScene == BLUETOOTH_SCO) {
        portTrackCtx->subPortCtx->scoTrackCount--;
    } else {
#ifdef MINI_MEMORY_SUPPORT
        if (!portTrackCtx->isReleased) {
            portTrackCtx->subPortCtx->a2dpTrackCount--;
        }
#else
        portTrackCtx->subPortCtx->a2dpTrackCount--;
#endif
    }
}

static int32_t EstablishTranscodeBackendPipeline(SubPortContext *portCtx, SubPortTrackContext *portTrackCtx,
    int32_t trackId)
{
    CHK_FAILED_RETURN_NO_LOG(portTrackCtx->workScene, BLUETOOTH_OUT_A2DP_NORMAL, MEDIA_HAL_OK);
    IncrementTrackCount(portTrackCtx);
#if defined(BT_HAL_SUPPORT) && defined(SUPPORT_BREDR)
    bt_a2dp_codec_param codecParam = {};
    td_u32 btRet = bt_get_audio_parameter(portTrackCtx->btStreamHandle, BT_AUDIO_PARAM_A2DP_CODEC, &codecParam,
        sizeof(codecParam));
    CHK_FAILED_RETURN(btRet, MEDIA_HAL_OK, MEDIA_HAL_ERR, "bt_get_audio_parameter failed");

    if (codecParam.codec_type != BT_AUDIO_CODEC_SBC) {
        MEDIA_HAL_LOGE(MODULE_NAME, "unsupport codec type: 0x%x", codecParam.codec_type);
        return MEDIA_HAL_ERR;
    }

    bt_a2dp_sbc_codec_caps *caps = (bt_a2dp_sbc_codec_caps *)codecParam.codec_caps;
    MEDIA_HAL_LOGI(MODULE_NAME, "sbc sampleFreq: %02x chnlMode: %02x blockLen: %02x subBand: %02x \
        allocMtd: %02x minBitpool: %02x maxBitpool: %02x",
        caps->sample_frequency, caps->chnl_mode, caps->block_length, caps->subband, caps->alloc_method,
        caps->min_bitpool, caps->max_bitpool);
    bool success = ConvertSampleFrequencyToSampleRate(caps->sample_frequency, &portCtx->sampleRate);
    CHK_COND_RETURN(!success, MEDIA_HAL_INVALID_PARAM, "invalid sample frequency");
    success = ConvertChnlModeToChannelCount(caps->chnl_mode, &portCtx->channelCount);
    CHK_COND_RETURN(!success, MEDIA_HAL_INVALID_PARAM, "invalid channel mode");
#endif
#ifdef AUDIO_SDK_SUPPORT
    int32_t ret = CastSndOpen(&portCtx->castSndHandle, portCtx->sampleRate, portCtx->channelCount);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "CastSndOpen failed");

    ret = EstablishPipelineInternal(portCtx, portTrackCtx, portCtx->a2dpTrackCount);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "EstablishPipelineInternal failed");
        goto SND_CLOSE;
    }

    portTrackCtx->trackCount = portCtx->a2dpTrackCount;
    portTrackCtx->castSndHandle = portCtx->castSndHandle;
    portTrackCtx->aencHandle = portCtx->aencHandle;
    portTrackCtx->outputAdpHandle = portCtx->outputAdpHandle;
    MEDIA_HAL_LOGD(MODULE_NAME, "success trackId (%d)", trackId);
#endif
    return MEDIA_HAL_OK;

#ifdef AUDIO_SDK_SUPPORT
SND_CLOSE:
    (void)CastSndClose(portCtx->castSndHandle);
    return ret;
#endif
}

static int32_t SingletonResourceStopInternal(const SubPortTrackContext *portTrackCtx)
{
#ifdef AUDIO_SDK_SUPPORT
    int32_t ret = StopByAudioModule(portTrackCtx, AUDIO_MOD_AENC);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "aenc stop failed");
#endif
#if defined(BT_HAL_SUPPORT) && defined(SUPPORT_BREDR)
    td_u32 btRet = bt_stop_audio_stream(portTrackCtx->btStreamHandle);
    CHK_FAILED_NO_RETURN(btRet, MEDIA_HAL_OK, "bt stop audio stream failed");
#endif

    MEDIA_HAL_LOGD(MODULE_NAME, "success trackId (%d)", portTrackCtx->trackId);
    return MEDIA_HAL_OK;
}

static int32_t AbrogationTranscodeBackendPipeline(const SubPortContext *portCtx,
    SubPortTrackContext *portTrackCtx)
{
#ifdef AUDIO_SDK_SUPPORT
    MEDIA_HAL_UNUSED(portCtx);
    CHK_FAILED_RETURN_NO_LOG(portTrackCtx->workScene, BLUETOOTH_OUT_A2DP_NORMAL, MEDIA_HAL_OK);
    DecreaseTrackCount(portTrackCtx);
    if (portTrackCtx->subPortCtx->a2dpTrackCount > 0) {
        MEDIA_HAL_LOGI(MODULE_NAME, "in the multi-instance scenario, need keep one aenc");
        (void)CastSndClose(portTrackCtx->subPortCtx->castSndHandle);
        return MEDIA_HAL_OK;
    }

#ifdef MINI_MEMORY_SUPPORT
        // 目前支持 SDK 小型化方案，暂停时刻会销毁通路，同时正常退出和切换输出设备也会销毁通路，避免重复销毁，需要检查下，典型场景如下：
        // 1.蓝牙音乐暂停状态退出播放
        // 2.蓝牙音乐暂停状态断开蓝牙耳机
        // 3.蓝牙音乐播放中途接听电话，通话中途断开耳机，通话挂断后恢复播放
        if (portTrackCtx->isReleased) {
            MEDIA_HAL_LOGI(MODULE_NAME, "have released");
            return MEDIA_HAL_OK;
        }
#endif

    int32_t ret = SingletonResourceStopInternal(portTrackCtx);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "singleton resource stop internal failed");

    ret = BluetoothDetach(portCtx, portTrackCtx);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "bt detach failed");
    ret = AudioSDKDetach(AUDIO_MOD_AENC, portTrackCtx->aencHandle, portTrackCtx->outputAdpHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "aenc detach adp failed");
    ret = AudioSDKDetach(AUDIO_MOD_SOUND, portTrackCtx->castSndHandle, portTrackCtx->aencHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "sound detach aenc failed");
    ret = AudioSDKAdpClose(portTrackCtx->outputAdpHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "adp close failed");
    ret = AudioSDKAencClose(portTrackCtx->aencHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "aenc close failed");
    ret = CastSndClose(portTrackCtx->subPortCtx->castSndHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "CastSndClose failed");
#else
    MEDIA_HAL_UNUSED(portCtx);
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(trackCount);
#endif

    MEDIA_HAL_LOGD(MODULE_NAME, "success trackId (%d)", portTrackCtx->trackId);
    return MEDIA_HAL_OK;
}

int32_t AudioBluetoothSubPortCreateTrack(AudioHandle portHandle, const struct PortPluginAttr *inputAttr,
    enum AudioPortPin portPin, AudioHandle *trackHandle)
{
    SubPortContext *portCtx = (SubPortContext *)portHandle;
    int32_t trackCount = -1;
    MediaHalMutexLock(portCtx->a2dpTrackMutex);
    int32_t trackId = SubPortGetFreeTrack(&trackCount);
    if (trackId == INVALID_ID) {
        MediaHalMutexUnLock(portCtx->a2dpTrackMutex);
        return MEDIA_HAL_INVALID_PARAM;
    }
    int32_t ret = MEDIA_HAL_ERR;
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)malloc(sizeof(SubPortTrackContext));
    if (portTrackCtx == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "malloc failed");
        ret = MEDIA_HAL_NO_MEM;
        goto FREE_TRACK;
    }

    if (memset_s(portTrackCtx, sizeof(SubPortTrackContext), 0, sizeof(SubPortTrackContext)) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "memset_s failed");
        goto FREE_TRACK_CTX;
    }

    if (SubPortTrackInit(portCtx, inputAttr, portPin, portTrackCtx) != MEDIA_HAL_OK) {
        goto FREE_TRACK_CTX;
    }
    /* sound + aenc + adp for BLUETOOTH_A2DP_NORMAL workScene */
    ret = EstablishTranscodeBackendPipeline(portCtx, portTrackCtx, trackId);
    if (ret != MEDIA_HAL_OK) {
        DecreaseTrackCount(portTrackCtx);
        goto FREE_TRACK_CTX;
    }
    
    ret = g_funcsList[portTrackCtx->workScene].createTrack(portTrackCtx);
    if (ret != MEDIA_HAL_OK) {
        goto ABROGATION_PIPELINE;
    }

    portTrackCtx->trackId = trackId;
    portCtx->portTrack[trackId] = portTrackCtx;
    *trackHandle = &portTrackCtx->common;
    /* A2dp sink 和 sco 不占用蓝牙资源和aenc，所以这里默认就是暂停的状态 */
    if (portTrackCtx->workScene == BLUETOOTH_IN_A2DP_NORMAL ||
        portTrackCtx->workScene == BLUETOOTH_SCO) {
        SetPause(trackId, true);
    }
    MediaHalMutexUnLock(portCtx->a2dpTrackMutex);
    return MEDIA_HAL_OK;

ABROGATION_PIPELINE:
    (void)AbrogationTranscodeBackendPipeline(portCtx, portTrackCtx);
FREE_TRACK_CTX:
    free(portTrackCtx);
FREE_TRACK:
    (void)SubPortFreeTrack(trackId, &trackCount);
    MediaHalMutexUnLock(portCtx->a2dpTrackMutex);
    return ret;
}

static bool BluetoothPortIsValidTrack(const SubPortContext *portCtx, const SubPortTrackContext *portTrackCtx)
{
    for (int32_t i = 0; i < BT_PORT_MAX_TRACK_NUM; i++) {
        if (portCtx->portTrack[i] == portTrackCtx) {
            return true;
        }
    }
    return false;
}

int32_t AudioBluetoothSubPortDestroyTrack(AudioHandle portHandle, AudioHandle trackHandle)
{
    SubPortContext *portCtx = (SubPortContext *)portHandle;
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;

    MediaHalMutexLock(portCtx->a2dpTrackMutex);
    bool isValid = BluetoothPortIsValidTrack(portCtx, portTrackCtx);
    if (!isValid) {
        MEDIA_HAL_LOGE(MODULE_NAME, "track is invalid");
        MediaHalMutexUnLock(portCtx->a2dpTrackMutex);
        return MEDIA_HAL_INVALID_PARAM;
    }
    int32_t trackCount = -1;
    int32_t ret = SubPortFreeTrack(portTrackCtx->trackId, &trackCount);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "SubPortFreeTrack failed");

    portTrackCtx->trackCount = trackCount;
    ret = g_funcsList[portTrackCtx->workScene].destroyTrack(portTrackCtx);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "work scene %d destroyTrack failed", portTrackCtx->workScene);
    }
    /* sound + aenc + adp for BLUETOOTH_A2DP_NORMAL workScene */
    ret = AbrogationTranscodeBackendPipeline(portCtx, portTrackCtx);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "AbrogationTranscodeBackendPipeline failed");

    MEDIA_HAL_LOGD(MODULE_NAME, "success trackId (%d)", portTrackCtx->trackId);

    if (portTrackCtx->inputAttr.extraData != NULL) {
        free(portTrackCtx->inputAttr.extraData);
        portTrackCtx->inputAttr.extraData = NULL;
        portTrackCtx->inputAttr.extraDataSize = 0;
    }

    portCtx->portTrack[portTrackCtx->trackId] = NULL;
    MediaHalMutexUnLock(portCtx->a2dpTrackMutex);
    free(portTrackCtx);
    return MEDIA_HAL_OK;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

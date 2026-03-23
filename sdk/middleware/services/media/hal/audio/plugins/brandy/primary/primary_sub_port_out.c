/*
* Copyright (c) @CompanyNameMagicTag. 2021-2021. All rights reserved.
* Description: primary sub port out
* Author: Media Software Group
* Create: 2021-09-28
*/

#include "primary_sub_port_out.h"

#include <math.h>
#include "securec.h"

#ifdef AUDIO_SDK_SUPPORT
#include "soc_uapi_adp.h"
#include "soc_uapi_adec.h"
#include "soc_uapi_sound.h"
#include "soc_uapi_aef.h"
#endif
#if defined(BRANDY_PRODUCT_EVB)
#include "product_evb_standard.h"
#elif defined(BRANDY_PRODUCT_EVB4)
#include "product_evb4_standard.h"
#endif
#include "plugin_common.h"
#include "media_hal_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define MODULE_NAME "AudioPrimarySubPortOut"
#define EXTRA_DATA "extradata"
#define VOLUME_TOGGLE_DATA "vtoggle"
#define NOTIFY_TOGGLE_DATA "ntoggle"
#define SPEAKER_CALI_COEF_CONFIG_DATA "spkcali"
#define AEF_ENABLE "aefenable"
#define SET_DB_VOLUME "setdbvol"
#define GET_AEF_PARAM "getaefparam"
#define SET_AEF_PARAM "setaefparam"
#define SPEAKER_CALI_COEF_CONFIG_LEN 96
#define AEF_PARAM_LEN 256

#ifdef AUDIO_SDK_SUPPORT
#define AO_DEV_MAX_NUM UAPI_SND_MAX
#define AO_DEV_MAX_TRACK_NUM UAPI_SND_TRACK_MAX
#endif

static enum AudioFormat g_supportFormats[] = {
    AUDIO_FORMAT_PCM_16_BIT,
    AUDIO_FORMAT_AAC_LC,
    AUDIO_FORMAT_MP3,
    AUDIO_FORMAT_SBC,
    AUDIO_FORMAT_FLAC,
};

#ifdef AUDIO_SDK_SUPPORT
typedef struct {
    enum AudioCategory audioCategory;
    uapi_snd_aef_profile audioProfile;
} AudioCategoryPair;

static AudioCategoryPair g_audioCategoryPairs[] = {
    { AUDIO_IN_MEDIA, UAPI_SND_AEF_PROFILE_MUSIC },
    { AUDIO_IN_COMMUNICATION, UAPI_SND_AEF_PROFILE_VOIP },
    { AUDIO_IN_OTHERS, UAPI_SND_AEF_PROFILE_MUSIC}
};
#endif

struct SubPortOutContext;

typedef struct {
    /* track impl */
    AudioPortTrack common;
#ifdef AUDIO_SDK_SUPPORT
    /* adp context */
    td_handle inputAdpHandle;
    /* adec context */
    td_handle adecHandle;
    /* sound track context */
    int32_t sndTrackId;
    td_handle trackHandle;
    bool isReleased;
    uapi_audio_toggle_config volumeToggleConfig;
    uapi_audio_toggle_config notifyToggleConfig;
#endif
    /* attr context */
    bool isRaw;
    bool mute;
    float volume;
    struct PortPluginAttr inputAttr;
    /* sub port context */
    struct SubPortOutContext *subPortCtx;
    TrackCallback callback;
    AudioHandle caller;
} SubPortTrackOutContext;

typedef struct SubPortOutContext {
#ifdef AUDIO_SDK_SUPPORT
    /* sound context */
    int32_t sndDevId;
    td_handle sndHandle;
    /* record track context */
    SubPortTrackOutContext *portTrack[AO_DEV_MAX_TRACK_NUM];
#endif
    MediaHalMutexHandle createTrackMutex;
} SubPortOutContext;

typedef struct {
    bool used;
    int32_t devId;
} SubPortOutMng;

/* protect sound device resource */
static bool g_subPortOutMngInited = false;
#ifdef AUDIO_SDK_SUPPORT
static SubPortOutMng g_subPortOuts[AO_DEV_MAX_NUM];
#endif
static MediaHalMutexHandle g_subPortOutMngLock = NULL;

/* sdk sound open/close reference count */
static int32_t g_subPortTrackOutSndRefCount = 0;
static MediaHalMutexHandle g_subPortTrackOutLock = NULL;

static void SubPortOutManagerInit(void)
{
#ifdef AUDIO_SDK_SUPPORT
    MediaHalMutexLock(g_subPortOutMngLock);
    if (!g_subPortOutMngInited) {
        for (int32_t i = 0; i < AO_DEV_MAX_NUM; i++) {
            g_subPortOuts[i].used = false;
            g_subPortOuts[i].devId = i;
        }
        g_subPortOutMngInited = true;
    }
    MediaHalMutexUnLock(g_subPortOutMngLock);
#endif
}

static int32_t SubPortOutManagerGetFreeDev(void)
{
#ifdef AUDIO_SDK_SUPPORT
    MediaHalMutexLock(g_subPortOutMngLock);
    if (!g_subPortOutMngInited) {
        MediaHalMutexUnLock(g_subPortOutMngLock);
        return MEDIA_HAL_ERR;
    }
    for (int32_t i = 0; i < AO_DEV_MAX_NUM; i++) {
        if (!g_subPortOuts[i].used) {
            g_subPortOuts[i].used = true;
            int32_t devId = g_subPortOuts[i].devId;
            MediaHalMutexUnLock(g_subPortOutMngLock);
            return devId;
        }
    }
    MediaHalMutexUnLock(g_subPortOutMngLock);
#endif
    return INVALID_ID;
}

static int32_t SubPortOutManagerFreeDev(int32_t devId)
{
#ifdef AUDIO_SDK_SUPPORT
    MediaHalMutexLock(g_subPortOutMngLock);
    if (!g_subPortOutMngInited) {
        MediaHalMutexUnLock(g_subPortOutMngLock);
        return MEDIA_HAL_ERR;
    }
    for (int32_t i = 0; i < AO_DEV_MAX_NUM; i++) {
        if (g_subPortOuts[i].devId == devId) {
            g_subPortOuts[i].used = false;
            MediaHalMutexUnLock(g_subPortOutMngLock);
            return MEDIA_HAL_OK;
        }
    }
    MediaHalMutexUnLock(g_subPortOutMngLock);
    MEDIA_HAL_LOGE(MODULE_NAME, "invalid sound device id: %d", devId);
#else
    MEDIA_HAL_UNUSED(devId);
#endif
    return MEDIA_HAL_ERR;
}

typedef struct {
    bool used;
    int32_t trackId;
} SubPortTrackOutMng;

#ifdef AUDIO_SDK_SUPPORT
/* protect sound device track resource */
static bool g_subPortTrackOutMngInited = false;
static SubPortTrackOutMng g_subPortTrackOuts[AO_DEV_MAX_TRACK_NUM];
static MediaHalMutexHandle g_subPortTrackOutMngLock = NULL;
#endif

static void SubPortTrackOutManagerInit(void)
{
#ifdef AUDIO_SDK_SUPPORT
    MediaHalMutexLock(g_subPortTrackOutMngLock);
    if (!(g_subPortTrackOutMngInited)) {
        for (int32_t i = 0; i < AO_DEV_MAX_TRACK_NUM; i++) {
            g_subPortTrackOuts[i].used = false;
            g_subPortTrackOuts[i].trackId = i;
        }
        g_subPortTrackOutMngInited = true;
    }
    MediaHalMutexUnLock(g_subPortTrackOutMngLock);
#endif
}

static void SubPortTrackOutManagerDeInit(void)
{
#ifdef AUDIO_SDK_SUPPORT
    MediaHalMutexLock(g_subPortTrackOutMngLock);
    if (g_subPortTrackOutMngInited) {
        for (int32_t i = 0; i < AO_DEV_MAX_TRACK_NUM; i++) {
            if (g_subPortTrackOuts[i].used) {
                MEDIA_HAL_LOGE(MODULE_NAME, "sub port track out %d not free", g_subPortTrackOuts[i].trackId);
            }
        }
        g_subPortTrackOutMngInited = false;
    }
    MediaHalMutexUnLock(g_subPortTrackOutMngLock);
#endif
}

static int32_t SubPortTrackOutManagerGetFreeTrack(void)
{
#ifdef AUDIO_SDK_SUPPORT
    MediaHalMutexLock(g_subPortTrackOutMngLock);
    if (!(g_subPortTrackOutMngInited)) {
        MediaHalMutexUnLock(g_subPortTrackOutMngLock);
        MEDIA_HAL_LOGE(MODULE_NAME, "sub port track out not init");
        return INVALID_ID;
    }
    for (int32_t i = 0; i < AO_DEV_MAX_TRACK_NUM; i++) {
        if (!(g_subPortTrackOuts[i].used)) {
            g_subPortTrackOuts[i].used = true;
            int32_t trackId = g_subPortTrackOuts[i].trackId;
            MediaHalMutexUnLock(g_subPortTrackOutMngLock);
            return trackId;
        }
    }
    MediaHalMutexUnLock(g_subPortTrackOutMngLock);
    MEDIA_HAL_LOGE(MODULE_NAME, "sub port out no free track");
#endif
    return INVALID_ID;
}

static int32_t SubPortTrackOutManagerFreeTrack(int32_t trackId)
{
#ifdef AUDIO_SDK_SUPPORT
    MediaHalMutexLock(g_subPortTrackOutMngLock);
    for (int32_t i = 0; i < AO_DEV_MAX_TRACK_NUM; i++) {
        if (g_subPortTrackOuts[i].trackId == trackId) {
            if (!(g_subPortTrackOuts[i].used)) {
                MediaHalMutexUnLock(g_subPortTrackOutMngLock);
                MEDIA_HAL_LOGE(MODULE_NAME, "sub port out track %d not used can't free", trackId);
                return MEDIA_HAL_ERR;
            }
            g_subPortTrackOuts[i].used = false;
            MediaHalMutexUnLock(g_subPortTrackOutMngLock);
            return MEDIA_HAL_OK;
        }
    }
    MediaHalMutexUnLock(g_subPortTrackOutMngLock);
    MEDIA_HAL_LOGE(MODULE_NAME, "track id: %d is invalid", trackId);
#else
    MEDIA_HAL_UNUSED(trackId);
#endif
    return MEDIA_HAL_ERR;
}

static int32_t SubPortTrackOutInit(SubPortTrackOutContext **portTrackOutCtx, const struct PortPluginAttr *inputAttr)
{
    SubPortTrackOutContext *subPortTrackOutCtx = (SubPortTrackOutContext *)malloc(sizeof(SubPortTrackOutContext));
    if (subPortTrackOutCtx == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "malloc failed");
        return MEDIA_HAL_NO_MEM;
    }

    if (memset_s(subPortTrackOutCtx, sizeof(SubPortTrackOutContext), 0, sizeof(SubPortTrackOutContext)) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "memset_s failed");
        free(subPortTrackOutCtx);
        return MEDIA_HAL_ERR;
    }
#ifdef AUDIO_SDK_SUPPORT
    subPortTrackOutCtx->inputAttr = *inputAttr;

    /* The default bit width is 16 bits. The 24-bit width is processed separately. */
    subPortTrackOutCtx->inputAttr.bitWidth = AUDIO_BIT_WIDTH_16_BIT;

    subPortTrackOutCtx->common.audioPin = PIN_OUT_SPEAKER;
    subPortTrackOutCtx->common.GetFrameCount = NULL;
    subPortTrackOutCtx->common.GetLatency = AudioPrimarySubPortOutGetLatency;
    subPortTrackOutCtx->common.EnableTrack = AudioPrimarySubPortOutEnableTrack;
    subPortTrackOutCtx->common.DisableTrack = AudioPrimarySubPortOutDisableTrack;
    subPortTrackOutCtx->common.PauseTrack = AudioPrimarySubPortOutPauseTrack;
    subPortTrackOutCtx->common.ResumeTrack = AudioPrimarySubPortOutResumeTrack;
    subPortTrackOutCtx->common.FlushTrack = AudioPrimarySubPortOutFlushTrack;
    subPortTrackOutCtx->common.AcquireFrame = NULL;
    subPortTrackOutCtx->common.SendFrame = AudioPrimarySubPortOutSendFrame;
    subPortTrackOutCtx->common.GetPosition = AudioPrimarySubPortOutGetPosition;
    subPortTrackOutCtx->common.SetMute = AudioPrimarySubPortOutTrackSetMute;
    subPortTrackOutCtx->common.GetMute = AudioPrimarySubPortOutTrackGetMute;
    subPortTrackOutCtx->common.SetVolume = AudioPrimarySubPortOutTrackSetVolume;
    subPortTrackOutCtx->common.GetVolume = AudioPrimarySubPortOutTrackGetVolume;
    subPortTrackOutCtx->common.Invoke = AudioPrimarySubPortOutInvoke;
    subPortTrackOutCtx->common.RegCallback = AudioPrimarySubPortOutRegCallback;
    subPortTrackOutCtx->volumeToggleConfig.toggle_valid = false;
    subPortTrackOutCtx->volumeToggleConfig.toggle_count = 0;
    subPortTrackOutCtx->notifyToggleConfig.toggle_valid = false;
    subPortTrackOutCtx->notifyToggleConfig.toggle_count = 0;

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
#else
    MEDIA_HAL_UNUSED(portTrackOutCtx);
    MEDIA_HAL_UNUSED(inputAttr);
#endif
    *portTrackOutCtx = subPortTrackOutCtx;
    return MEDIA_HAL_OK;
}

#ifdef AUDIO_SDK_SUPPORT
static uapi_snd_aef_profile ConvertAudioCategoryToAudioEffectProfile(const enum AudioCategory audioCategory)
{
    for (uint32_t i = 0; i < ARRAY_SIZE(g_audioCategoryPairs); i++) {
        if (audioCategory == g_audioCategoryPairs[i].audioCategory) {
            return g_audioCategoryPairs[i].audioProfile;
        }
    }

    return UAPI_SND_AEF_PROFILE_MAX;
}
#endif

static int32_t SetAefProfile(SubPortOutContext *portOutCtx, const enum AudioCategory audioCategory)
{
    uapi_snd_aef_profile profile = ConvertAudioCategoryToAudioEffectProfile(audioCategory);
    if (profile == UAPI_SND_AEF_PROFILE_MAX) {
        MEDIA_HAL_LOGE(MODULE_NAME, "invalid audio category %d", audioCategory);
        return MEDIA_HAL_INVALID_PARAM;
    }
    int32_t ret = uapi_snd_set_aef_profile(portOutCtx->sndHandle, profile);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "uapi_snd_set_aef_profile(%d) failed: 0x%x", profile, ret);
    }
    ret = uapi_snd_set_port_aef_enable(portOutCtx->sndHandle, BUILTIN_SND_OUT_PORT,
        SND_PORT_AEF_TYPE, SND_PORT_AEF_ENABLE);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "uapi_snd_set_port_aef_enable(%d) failed: 0x%x", SND_PORT_AEF_TYPE, ret);
    }
    SetAefExtraParam(portOutCtx->sndHandle);
    return MEDIA_HAL_OK;
}

static int32_t AODevOpen(SubPortOutContext *portOutCtx, const enum AudioCategory audioCategory)
{
#ifdef AUDIO_SDK_SUPPORT
    MediaHalInitStaticMutexLock(&g_subPortTrackOutLock);
    MediaHalMutexLock(g_subPortTrackOutLock);
    if (g_subPortTrackOutSndRefCount == 0) {
        int32_t ret = MEDIA_HAL_OK;
        uapi_snd_attr sndAttr = {};
        MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_get_default_attr(portOutCtx->sndDevId, &sndAttr));
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "uapi_snd_get_default_attr failed");
            MediaHalMutexUnLock(g_subPortTrackOutLock);
            return ret;
        }
#ifndef MELODY_AUDIO_SUPPORT
        GetBuiltinAOAttr(&sndAttr);
#endif
        MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_open(&(portOutCtx->sndHandle), portOutCtx->sndDevId, &sndAttr));
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "uapi_snd_open failed");
            MediaHalMutexUnLock(g_subPortTrackOutLock);
            return ret;
        }
        if (audioCategory == AUDIO_IN_COMMUNICATION) {
            ret = uapi_snd_set_track_mode(portOutCtx->sndHandle, sndAttr.port_attr[0].out_port, SND_TRACK_MODE_BT_SCO);
        } else if (audioCategory == AUDIO_IN_MEDIA) {
            ret = uapi_snd_set_track_mode(portOutCtx->sndHandle,
                sndAttr.port_attr[0].out_port, SND_TRACK_MODE_LOCAL_MUSIC);
        }
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "set track mode failed, ret = %d", ret);
            MediaHalMutexUnLock(g_subPortTrackOutLock);
            return MEDIA_HAL_INVALID_PARAM;
        }
        ret = SetAefProfile(portOutCtx, audioCategory);
        if (ret != MEDIA_HAL_OK) {
            MediaHalMutexUnLock(g_subPortTrackOutLock);
            return MEDIA_HAL_INVALID_PARAM;
        }
    }
    g_subPortTrackOutSndRefCount++;
    MediaHalMutexUnLock(g_subPortTrackOutLock);
#else
    MEDIA_HAL_UNUSED(portOutCtx);
    MEDIA_HAL_UNUSED(audioCategory);
#endif
    return MEDIA_HAL_OK;
}

#ifdef AUDIO_SDK_SUPPORT
static int32_t AODevClose(td_handle sndHandle)
{
    MediaHalInitStaticMutexLock(&g_subPortTrackOutLock);
    MediaHalMutexLock(g_subPortTrackOutLock);
    g_subPortTrackOutSndRefCount--;
    if (g_subPortTrackOutSndRefCount == 0) {
        int32_t ret;
        MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_close(sndHandle));
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "uapi_snd_close failed");
            MediaHalMutexUnLock(g_subPortTrackOutLock);
            return ret;
        }
    }
    MediaHalMutexUnLock(g_subPortTrackOutLock);

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}
#endif

static int32_t SndCreateTrackFromFormat(SubPortOutContext *portOutCtx, SubPortTrackOutContext *portTrackOutCtx)
{
#ifdef AUDIO_SDK_SUPPORT
    int32_t ret;
    uapi_snd_track_attr sndTrackDefAttr = {};
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_get_track_default_attr(&sndTrackDefAttr));
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_snd_get_track_default_attr failed");
    if (portTrackOutCtx->inputAttr.type == AUDIO_IN_MEDIA) {
        sndTrackDefAttr.track_type = UAPI_SND_TRACK_TYPE_BT_MUSIC;
    } else if (portTrackOutCtx->inputAttr.type == AUDIO_IN_COMMUNICATION) {
        sndTrackDefAttr.track_type = UAPI_SND_TRACK_TYPE_BT_VOIP;
    } else {
        sndTrackDefAttr.track_type = UAPI_SND_TRACK_TYPE_SLAVE;
    }
    MEDIA_HAL_DOFUNC_TIME(
        ret = uapi_snd_create_track(&(portTrackOutCtx->trackHandle), portOutCtx->sndHandle, &sndTrackDefAttr));
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_snd_create_track failed");

    portTrackOutCtx->isRaw = IsRawFormat(portTrackOutCtx->inputAttr.audioFormat);
#else
    MEDIA_HAL_UNUSED(portOutCtx);
    MEDIA_HAL_UNUSED(portTrackOutCtx);
#endif
    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

#ifdef AUDIO_SDK_SUPPORT

static int32_t EstablishPipelineWithRaw(SubPortTrackOutContext *portTrackOutCtx)
{
    int32_t ret = AudioSDKAttach(AUDIO_MOD_ADP, portTrackOutCtx->inputAdpHandle, portTrackOutCtx->trackHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "adp attach track failed");
        (void)AudioSDKAdpClose(portTrackOutCtx->inputAdpHandle);
        return ret;
    }
    return MEDIA_HAL_OK;
}

static int32_t EstablishPipelineWithOutRaw(SubPortTrackOutContext *portTrackOutCtx)
{
    int32_t ret = IsNeedSetExtraFlag(portTrackOutCtx->inputAttr.audioFormat,
        &portTrackOutCtx->inputAttr.isNeedSetExtraParam);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "set extra flag failed");
        goto ADP_CLOSE;
    }
    if (portTrackOutCtx->inputAttr.isNeedSetExtraParam) {
        MEDIA_HAL_LOGI(MODULE_NAME, "need SetExtraParam adec and attach delay");
        return MEDIA_HAL_OK;
    }
    ret = AudioSDKAdecOpen(&portTrackOutCtx->adecHandle, portTrackOutCtx->inputAttr);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "adec open failed");
        goto ADP_CLOSE;
    }
    ret = AudioSDKAttach(AUDIO_MOD_ADP, portTrackOutCtx->inputAdpHandle, portTrackOutCtx->adecHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "adp attach adec failed");
        goto ADEC_CLOSE;
    }
    ret = AudioSDKAttach(AUDIO_MOD_ADEC, portTrackOutCtx->adecHandle, portTrackOutCtx->trackHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "adec attach track failed");
        goto ADP_DETACH_ADEC;
    }
    return MEDIA_HAL_OK;
ADP_DETACH_ADEC:
    (void)AudioSDKDetach(AUDIO_MOD_ADP, portTrackOutCtx->inputAdpHandle, portTrackOutCtx->adecHandle);
ADEC_CLOSE:
    (void)AudioSDKAdecClose(portTrackOutCtx->adecHandle);
ADP_CLOSE:
    (void)AudioSDKAdpClose(portTrackOutCtx->inputAdpHandle);
    return ret;
}
#endif

static int32_t EstablishPipeline(SubPortOutContext *portOutCtx, SubPortTrackOutContext *portTrackOutCtx)
{
#ifdef AUDIO_SDK_SUPPORT
    int32_t ret = SndCreateTrackFromFormat(portOutCtx, portTrackOutCtx);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "input adp open failed");
    ret = AudioSDKAdpOpen(&portTrackOutCtx->inputAdpHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "input adp open failed");
        goto DESTROY_TRACK;
    }
    if (portTrackOutCtx->isRaw) {
        ret = EstablishPipelineWithRaw(portTrackOutCtx);
        if (ret != MEDIA_HAL_OK) {
            goto DESTROY_TRACK;
        }
    } else {
        ret = EstablishPipelineWithOutRaw(portTrackOutCtx);
        if (ret != MEDIA_HAL_OK) {
            goto DESTROY_TRACK;
        }
    }
#else
    MEDIA_HAL_UNUSED(portTrackOutCtx);
#endif

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;

#ifdef AUDIO_SDK_SUPPORT
DESTROY_TRACK:
    (void)uapi_snd_destroy_track(portTrackOutCtx->trackHandle);
    return ret;
#endif
}

static int32_t SndDetach(const SubPortTrackOutContext *portTrackOutCtx)
{
#ifdef AUDIO_SDK_SUPPORT
    int32_t ret;

    if (portTrackOutCtx->isRaw) {
        ret = AudioSDKDetach(AUDIO_MOD_ADP, portTrackOutCtx->inputAdpHandle, portTrackOutCtx->trackHandle);
        CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "adp detach track failed");
    } else {
        ret = AudioSDKDetach(AUDIO_MOD_ADP, portTrackOutCtx->inputAdpHandle, portTrackOutCtx->adecHandle);
        CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "adp detach adec failed");
        ret = AudioSDKDetach(AUDIO_MOD_ADEC, portTrackOutCtx->adecHandle, portTrackOutCtx->trackHandle);
        CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "adec detach track failed");
        ret = AudioSDKAdecClose(portTrackOutCtx->adecHandle);
        CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "adec close failed");
    }

    ret = AudioSDKAdpClose(portTrackOutCtx->inputAdpHandle);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "adp close failed");
#else
    MEDIA_HAL_UNUSED(portTrackOutCtx);
#endif

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

int32_t AudioPrimarySubPortOutGetLatency(AudioHandle trackHandle, uint32_t *latencyMs)
{
    SubPortTrackOutContext *portTrackOutCtx = (SubPortTrackOutContext *)trackHandle;

    MEDIA_HAL_UNUSED(portTrackOutCtx);
    MEDIA_HAL_UNUSED(latencyMs);

    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}
#ifdef AUDIO_SDK_SUPPORT
static int32_t AdpEventCB(td_handle adpHandle, uapi_adp_event_type event, td_void *param, td_void *context)
{
    MEDIA_HAL_UNUSED(adpHandle);
    MEDIA_HAL_UNUSED(param);
    SubPortTrackOutContext *portTrackOutCtx = (SubPortTrackOutContext *)context;
    if (portTrackOutCtx->callback == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "callback is null");
        return MEDIA_HAL_ERR;
    }
    int32_t ret;
    switch (event) {
        case UAPI_ADP_EVENT_EOS:
            ret = portTrackOutCtx->callback(AUDIO_RENDER_EOS, NULL, 0, portTrackOutCtx->caller);
            CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "adp event eos callback failed");
            break;
        case UAPI_ADP_EVENT_EMPTY:
            ret = portTrackOutCtx->callback(AUDIO_BUFFER_EMPTY, NULL, 0, portTrackOutCtx->caller);
            CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "adp event buffer empty callback failed");
            break;
        default:
            MEDIA_HAL_LOGW(MODULE_NAME, "invalid event: %d", event);
            break;
    }

    return MEDIA_HAL_OK;
}
#endif

int32_t AudioPrimarySubPortOutEnableTrack(AudioHandle trackHandle)
{
#ifdef AUDIO_SDK_SUPPORT
    SubPortTrackOutContext *portTrackOutCtx = (SubPortTrackOutContext *)trackHandle;

    int32_t ret;
    if (portTrackOutCtx->inputAttr.isNeedSetExtraParam) {
        ret = AudioSDKAdecOpen(&portTrackOutCtx->adecHandle, portTrackOutCtx->inputAttr);
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "adec open failed");
            return ret;
        }
        ret = AudioSDKAttach(AUDIO_MOD_ADP, portTrackOutCtx->inputAdpHandle, portTrackOutCtx->adecHandle);
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "adp attach adec failed");
            return ret;
        }
        ret = AudioSDKAttach(AUDIO_MOD_ADEC, portTrackOutCtx->adecHandle, portTrackOutCtx->trackHandle);
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "adec attach track failed");
            return ret;
        }
    }
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_track_start(portTrackOutCtx->trackHandle,
        &portTrackOutCtx->notifyToggleConfig));
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_snd_track_start failed");
    uapi_adp_event_proc adpCallback = AdpEventCB;
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_adp_register_event_proc(portTrackOutCtx->inputAdpHandle, adpCallback,
        (td_void *)portTrackOutCtx));
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_adp_register_event_proc failed");
    if (!portTrackOutCtx->isRaw) {
        MEDIA_HAL_DOFUNC_TIME(ret = uapi_adec_start(portTrackOutCtx->adecHandle));
        CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_adec_start failed");
    }
#else
    MEDIA_HAL_UNUSED(trackHandle);
#endif

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

int32_t AudioPrimarySubPortOutDisableTrack(AudioHandle trackHandle)
{
#ifdef AUDIO_SDK_SUPPORT
    SubPortTrackOutContext *portTrackOutCtx = (SubPortTrackOutContext *)trackHandle;

#ifdef MINI_MEMORY_SUPPORT
    // 目前支持 SDK 小型化方案，暂停的时候会销毁通路，正常退出和切换输出设备也会销毁通路，需要检查下，典型场景如下：
    // 1.本地音乐暂停状态退出播放
    // 2.本地音乐暂停状态连接蓝牙耳机
    // 3.本地音乐播放中途接听电话，通话中途连接耳机，通话挂断后恢复播放
    if (portTrackOutCtx->isReleased) {
        MEDIA_HAL_LOGI(MODULE_NAME, "have released");
        return MEDIA_HAL_OK;
    }
#endif

    int32_t ret;
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_track_stop(portTrackOutCtx->trackHandle));
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_snd_track_stop failed");

    if (!portTrackOutCtx->isRaw) {
        MEDIA_HAL_DOFUNC_TIME(ret = uapi_adec_stop(portTrackOutCtx->adecHandle));
        CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_adec_stop failed");
    }
#else
    MEDIA_HAL_UNUSED(trackHandle);
#endif

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t AudioSDKStopInternal(const SubPortTrackOutContext *portTrackOutCtx)
{
    int32_t ret;
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_track_stop(portTrackOutCtx->trackHandle));
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "sound track stop failed");

    if (!portTrackOutCtx->isRaw) {
        MEDIA_HAL_DOFUNC_TIME(ret = uapi_adec_stop(portTrackOutCtx->adecHandle));
        CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "adec stop failed");
    }

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t AudioSDKDetachInternal(const SubPortTrackOutContext *portTrackOutCtx)
{
    int32_t ret;
    if (portTrackOutCtx->isRaw) {
        ret = AudioSDKDetach(AUDIO_MOD_ADP, portTrackOutCtx->inputAdpHandle, portTrackOutCtx->trackHandle);
        CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "adp detach track failed");
    } else {
        ret = AudioSDKDetach(AUDIO_MOD_ADP, portTrackOutCtx->inputAdpHandle, portTrackOutCtx->adecHandle);
        CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "adp detach adec failed");
        ret = AudioSDKDetach(AUDIO_MOD_ADEC, portTrackOutCtx->adecHandle, portTrackOutCtx->trackHandle);
        CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "adec detach track failed");
    }

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t AudioSDKCloseInternal(SubPortTrackOutContext *portTrackOutCtx)
{
    int32_t ret;
    if (!portTrackOutCtx->isRaw) {
        ret = AudioSDKAdecClose(portTrackOutCtx->adecHandle);
        CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "adec close failed");
    }

    ret = AudioSDKAdpClose(portTrackOutCtx->inputAdpHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "adp close failed");

    MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_destroy_track(portTrackOutCtx->trackHandle));
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "sound destroy track failed");

    ret = AODevClose(portTrackOutCtx->subPortCtx->sndHandle);

    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "sound close failed");

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

int32_t AudioPrimarySubPortOutPauseTrack(AudioHandle trackHandle)
{
#ifdef AUDIO_SDK_SUPPORT
    SubPortTrackOutContext *portTrackOutCtx = (SubPortTrackOutContext *)trackHandle;

    int32_t ret;
#ifdef MINI_MEMORY_SUPPORT
    MediaHalMutexLock(portTrackOutCtx->subPortCtx->createTrackMutex);
    ret = AudioSDKStopInternal(portTrackOutCtx);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "audio stop failed");
    ret = AudioSDKDetachInternal(portTrackOutCtx);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "audio detach failed");
    ret = AudioSDKCloseInternal(portTrackOutCtx);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "audio close failed");
    portTrackOutCtx->isReleased = true;
    MediaHalMutexUnLock(portTrackOutCtx->subPortCtx->createTrackMutex);
#else
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_track_pause(portTrackOutCtx->trackHandle));
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "sound track pause failed");
#endif

#else
    MEDIA_HAL_UNUSED(trackHandle);
#endif

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t AudioSDKOpenInternal(SubPortTrackOutContext *portTrackOutCtx)
{
    int32_t ret = AODevOpen(portTrackOutCtx->subPortCtx, portTrackOutCtx->inputAttr.type);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "sound open failed");

    ret = SndCreateTrackFromFormat(portTrackOutCtx->subPortCtx, portTrackOutCtx);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "input adp open failed");
        goto AO_CLOSE;
    }
    ret = AudioSDKAdpOpen(&portTrackOutCtx->inputAdpHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "input adp open failed");
        goto DESTROY_TRACK;
    }
    if (!portTrackOutCtx->isRaw) {
        ret = AudioSDKAdecOpen(&portTrackOutCtx->adecHandle, portTrackOutCtx->inputAttr);
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "adec open failed");
            goto ADP_CLOSE;
        }
    }

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;

ADP_CLOSE:
    (void)AudioSDKAdpClose(portTrackOutCtx->inputAdpHandle);
DESTROY_TRACK:
    (void)uapi_snd_destroy_track(portTrackOutCtx->trackHandle);
AO_CLOSE:
    (void)AODevClose(portTrackOutCtx->subPortCtx->sndHandle);
    return ret;
}

static int32_t AudioSDKAttachInternal(const SubPortTrackOutContext *portTrackOutCtx)
{
    int32_t ret;

    if (portTrackOutCtx->isRaw) {
        ret = AudioSDKAttach(AUDIO_MOD_ADP, portTrackOutCtx->inputAdpHandle, portTrackOutCtx->trackHandle);
        CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "adp attach track failed");
    } else {
        ret = AudioSDKAttach(AUDIO_MOD_ADP, portTrackOutCtx->inputAdpHandle, portTrackOutCtx->adecHandle);
        CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "adp attach adec failed");
        ret = AudioSDKAttach(AUDIO_MOD_ADEC, portTrackOutCtx->adecHandle, portTrackOutCtx->trackHandle);
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "adec attach track failed");
            goto ADP_DETACH_ADEC;
        }
    }

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;

ADP_DETACH_ADEC:
    (void)AudioSDKDetach(AUDIO_MOD_ADP, portTrackOutCtx->inputAdpHandle, portTrackOutCtx->adecHandle);
    return ret;
}

static int32_t AudioSDKStartInternal(const SubPortTrackOutContext *portTrackOutCtx)
{
    int32_t ret;
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_track_start(portTrackOutCtx->trackHandle, TD_NULL));
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "sound track start failed");

    if (!portTrackOutCtx->isRaw) {
        MEDIA_HAL_DOFUNC_TIME(ret = uapi_adec_start(portTrackOutCtx->adecHandle));
        CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "adec start failed");
    }

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

int32_t AudioPrimarySubPortOutResumeTrack(AudioHandle trackHandle)
{
#ifdef AUDIO_SDK_SUPPORT
    SubPortTrackOutContext *portTrackOutCtx = (SubPortTrackOutContext *)trackHandle;

    int32_t ret;
#ifdef MINI_MEMORY_SUPPORT
    MediaHalMutexLock(portTrackOutCtx->subPortCtx->createTrackMutex);
    ret = AudioSDKOpenInternal(portTrackOutCtx);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "audio open failed");
        MediaHalMutexUnLock(portTrackOutCtx->subPortCtx->createTrackMutex);
        return ret;
    }
    ret = AudioSDKAttachInternal(portTrackOutCtx);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "audio attach failed");
        goto AUDIO_CLOSE;
    }
    ret = AudioSDKStartInternal(portTrackOutCtx);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "audio start failed");
        goto AUDIO_DETACH;
    }
    portTrackOutCtx->isReleased = false;
    uapi_adp_event_proc adpCallback = AdpEventCB;
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_adp_register_event_proc(portTrackOutCtx->inputAdpHandle, adpCallback,
        (td_void *)portTrackOutCtx));
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "register adp event failed");
        goto AUDIO_STOP;
    }
    MediaHalMutexUnLock(portTrackOutCtx->subPortCtx->createTrackMutex);
#else
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_track_resume(portTrackOutCtx->trackHandle));
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "sound track resume failed");
#endif

#else
    MEDIA_HAL_UNUSED(trackHandle);
#endif
    return MEDIA_HAL_OK;

#ifdef MINI_MEMORY_SUPPORT
AUDIO_STOP:
    (void)AudioSDKStopInternal(portTrackOutCtx);
AUDIO_DETACH:
    (void)AudioSDKDetachInternal(portTrackOutCtx);
AUDIO_CLOSE:
    (void)AudioSDKCloseInternal(portTrackOutCtx);
    MediaHalMutexUnLock(portTrackOutCtx->subPortCtx->createTrackMutex);
    return ret;
#endif
}

int32_t AudioPrimarySubPortOutFlushTrack(const AudioHandle trackHandle)
{
    MEDIA_HAL_UNUSED(trackHandle);
    MEDIA_HAL_LOGW(MODULE_NAME, "AudioPrimarySubPortOutFlushTrack not support");
    return MEDIA_HAL_OK;
}

int32_t AudioPrimarySubPortOutSendFrame(AudioHandle trackHandle, const uint8_t *buffer, uint64_t requestBytes,
    uint64_t *replyBytes)
{
#ifdef AUDIO_SDK_SUPPORT
    SubPortTrackOutContext *portTrackOutCtx = (SubPortTrackOutContext *)trackHandle;

    int32_t ret;
    if (portTrackOutCtx->isRaw) {
        uapi_audio_frame audioFrame = {
            .interleaved = true,
            .bit_depth = portTrackOutCtx->inputAttr.bitWidth,
            .sample_rate = portTrackOutCtx->inputAttr.sampleRate,
            .channels = portTrackOutCtx->inputAttr.channelCount,
            .pcm_samples = ((uint32_t)portTrackOutCtx->inputAttr.sampleRate) / SAMPLERATE_TO_PTNUMPERFRM,
            .pcm_buffer = NULL,
            .bits_buffer = (int32_t *)buffer,
            .bits_bytes = (uint32_t)requestBytes,
            .frame_index = 0,
            .eos = false,
        };
        ret = uapi_adp_send_frame(portTrackOutCtx->inputAdpHandle, &audioFrame);
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
        ret = uapi_adp_send_stream(portTrackOutCtx->inputAdpHandle, &streamBuf);
        if (ret != MEDIA_HAL_OK) {
            *replyBytes = 0;
            return MEDIA_HAL_ERR_AGAIN;
        }
        *replyBytes = requestBytes;
    }
#else
    MEDIA_HAL_UNUSED(trackHandle);
    MEDIA_HAL_UNUSED(buffer);
    MEDIA_HAL_UNUSED(requestBytes);
    MEDIA_HAL_UNUSED(replyBytes);
#endif

    return MEDIA_HAL_OK;
}

int32_t AudioPrimarySubPortOutRegCallback(AudioHandle trackHandle, TrackCallback callback, AudioHandle caller)
{
    SubPortTrackOutContext *portTrackOutCtx = (SubPortTrackOutContext *)trackHandle;
    portTrackOutCtx->callback = callback;
    portTrackOutCtx->caller = caller;
    return MEDIA_HAL_OK;
}

int32_t AudioPrimarySubPortOutGetPosition(AudioHandle trackHandle, uint64_t *frames, struct AudioTimeStamp *time)
{
    MEDIA_HAL_UNUSED(frames);

    SubPortTrackOutContext *portTrackOutCtx = (SubPortTrackOutContext *)trackHandle;

#ifdef MINI_MEMORY_SUPPORT
    if (portTrackOutCtx->isReleased) {
        MEDIA_HAL_UNUSED(time);
        MEDIA_HAL_LOGI(MODULE_NAME, "have released");
        return MEDIA_HAL_OK;
    }
#endif
    return UpdateCurrentPosition(portTrackOutCtx->trackHandle, time);
}

int32_t AudioPrimarySubPortOutTrackSetMute(AudioHandle trackHandle, bool mute)
{
#ifdef AUDIO_SDK_SUPPORT
    SubPortTrackOutContext *portTrackOutCtx = (SubPortTrackOutContext *)trackHandle;

    int32_t ret;
    td_bool isMute = (mute == true) ? TD_TRUE : TD_FALSE;

#ifdef MINI_MEMORY_SUPPORT
    // 目前支持 SDK 小型化方案，暂停的时候会销毁通路，设置静音操作会失败，此时需要记录静音信息，在恢复播放的时候作用上去
    if (portTrackOutCtx->isReleased) {
        portTrackOutCtx->mute = mute;
        return MEDIA_HAL_OK;
    }
#endif
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_set_track_mute(portTrackOutCtx->trackHandle, isMute));
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_snd_set_track_mute failed");
    portTrackOutCtx->mute = mute;
#else
    MEDIA_HAL_UNUSED(trackHandle);
    MEDIA_HAL_UNUSED(mute);
#endif

    MEDIA_HAL_LOGI(MODULE_NAME, "set mute %d success", mute);
    return MEDIA_HAL_OK;
}

int32_t AudioPrimarySubPortOutTrackGetMute(AudioHandle trackHandle, bool *mute)
{
#ifdef AUDIO_SDK_SUPPORT
    SubPortTrackOutContext *portTrackOutCtx = (SubPortTrackOutContext *)trackHandle;

    int32_t ret;
    td_bool isMute = TD_FALSE;

#ifdef MINI_MEMORY_SUPPORT
    // 目前支持 SDK 小型化方案，暂停的时候会销毁通路，获取静音操作会失败，此时返回之前记录的静音信息即可
    if (portTrackOutCtx->isReleased) {
        *mute = portTrackOutCtx->mute;
        return MEDIA_HAL_OK;
    }
#endif
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_get_track_mute(portTrackOutCtx->trackHandle, &isMute));
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_snd_get_track_mute failed");
    *mute = (isMute == TD_TRUE) ? true : false;
#else
    MEDIA_HAL_UNUSED(trackHandle);
    MEDIA_HAL_UNUSED(mute);
#endif

    MEDIA_HAL_LOGI(MODULE_NAME, "get mute %d success", *mute);
    return MEDIA_HAL_OK;
}

int32_t AudioPrimarySubPortOutTrackSetVolume(AudioHandle trackHandle, float volume)
{
#ifdef AUDIO_SDK_SUPPORT
    SubPortTrackOutContext *portTrackOutCtx = (SubPortTrackOutContext *)trackHandle;
#ifdef MINI_MEMORY_SUPPORT
    // 目前支持 SDK 小型化方案，暂停的时候会销毁通路，设置音量操作会失败，此时需要记录音量信息，在恢复播放的时候作用上去
    if (portTrackOutCtx->isReleased) {
        portTrackOutCtx->volume = volume;
        return MEDIA_HAL_OK;
    }
#endif
    int32_t ret;
    int32_t tmpVolume;
    uapi_snd_gain sndGain = {};
    VolumePercentToDb(volume, &tmpVolume);
    sndGain.integer = tmpVolume;
    sndGain.decimal = 0;

    MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_set_track_volume(portTrackOutCtx->trackHandle, &sndGain,
        &portTrackOutCtx->volumeToggleConfig));
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_snd_set_track_volume failed");

    portTrackOutCtx->volume = volume;
    MEDIA_HAL_LOGI(MODULE_NAME, "set volume %d success", tmpVolume);
#else
    MEDIA_HAL_UNUSED(trackHandle);
    MEDIA_HAL_UNUSED(volume);
#endif

    return MEDIA_HAL_OK;
}

int32_t AudioPrimarySubPortOutTrackGetVolume(AudioHandle trackHandle, float *volume)
{
    SubPortTrackOutContext *portTrackOutCtx = (SubPortTrackOutContext *)trackHandle;

    *volume = portTrackOutCtx->volume;
    MEDIA_HAL_LOGI(MODULE_NAME, "get volume %f success", *volume);
    return MEDIA_HAL_OK;
}

#ifdef AUDIO_SDK_SUPPORT
static int32_t SetExtraParam(AudioHandle trackHandle, char *value, uint32_t valueLen)
{
    MEDIA_HAL_UNUSED(valueLen);
    SubPortTrackOutContext *portTrackOutCtx = (SubPortTrackOutContext *)trackHandle;
    int32_t ret = SetExtraData(&portTrackOutCtx->inputAttr, value, (uint32_t)(strlen(value) + 1));
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "set Extra data failed");
    return MEDIA_HAL_OK;
}

static int32_t SetAefEnable(AudioHandle trackHandle, char *value, uint32_t valueLen)
{
    MEDIA_HAL_UNUSED(valueLen);
    SubPortTrackOutContext *portTrackOutCtx = (SubPortTrackOutContext *)trackHandle;
    CHK_NULL_RETURN(value, MEDIA_HAL_INVALID_PARAM, "input param value is null");
    int32_t status = 0;
    int32_t effectType = UAPI_AEF_TYPE_NULL;
    int32_t ret = sscanf_s(value, "%d#%d", &status, &effectType);
    if (ret == -1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "set aef status sscanf_s failed!");
        return MEDIA_HAL_ERR;
    }
    bool enable = status == 0 ? false : true;
    ret = uapi_snd_set_port_aef_enable(portTrackOutCtx->subPortCtx->sndHandle, BUILTIN_SND_OUT_PORT,
        (uapi_aef_type)effectType, enable);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "set aef status failed! ret = %d", ret);
        return ret;
    }
    return MEDIA_HAL_OK;
}

static int32_t SetAefParam(AudioHandle trackHandle, char *value, uint32_t valueLen)
{
    MEDIA_HAL_UNUSED(valueLen);
    SubPortTrackOutContext *portTrackOutCtx = (SubPortTrackOutContext *)trackHandle;
    CHK_NULL_RETURN(value, MEDIA_HAL_INVALID_PARAM, "input param value is null");
    uint32_t paramLen = 0;
    uint8_t configFlag[MAX_KEY_STR_SIZE] = { 0 };
    uint32_t cmd = 0;
    int32_t ret = sscanf_s(value, "%s = %u#%u:", configFlag, MAX_KEY_STR_SIZE, &cmd, &paramLen);
    if (ret == -1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "set aef param sscanf_s failed!");
        return MEDIA_HAL_ERR;
    }
    uint8_t param[AEF_PARAM_LEN] = { 0 };
    ret = GetParamByValue(param, AEF_PARAM_LEN, value, valueLen, paramLen);
    if (ret != MEDIA_HAL_OK) {
        return ret;
    }
    ret = uapi_snd_set_aef_param(portTrackOutCtx->subPortCtx->sndHandle, BUILTIN_SND_OUT_PORT, cmd,
        (const td_void *)param, paramLen);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "set aef param failed! ret = %d", ret);
        return ret;
    }
    return MEDIA_HAL_OK;
}

static int32_t GetAefParam(AudioHandle trackHandle, char *value, uint32_t valueLen)
{
    MEDIA_HAL_UNUSED(valueLen);
    SubPortTrackOutContext *portTrackOutCtx = (SubPortTrackOutContext *)trackHandle;
    CHK_NULL_RETURN(value, MEDIA_HAL_INVALID_PARAM, "input param value is null");
    uint8_t configFlag[MAX_KEY_STR_SIZE] = { 0 };
    uint32_t cmd = 0;
    uint32_t paramLen = 0;
    int32_t ret = sscanf_s((const char *)value, "%s = %u#%u", configFlag, MAX_KEY_STR_SIZE, &cmd, &paramLen);
    if (ret == -1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "get aef param sscanf_s failed!");
        return MEDIA_HAL_ERR;
    }
    ret = uapi_snd_get_aef_param(portTrackOutCtx->subPortCtx->sndHandle, BUILTIN_SND_OUT_PORT, cmd,
        (td_void *)value, paramLen);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "get aef param failed! ret = %d", ret);
        return ret;
    }
    return MEDIA_HAL_OK;
}

static struct AudioPortInvoke g_primaryOutInvokeFuncList[] = {
    {EXTRA_DATA, SetExtraParam, true},
    {AEF_ENABLE, SetAefEnable, true},
    {SET_AEF_PARAM, SetAefParam, false},
    {GET_AEF_PARAM, GetAefParam, false},
};
#endif

int32_t AudioPrimarySubPortOutInvoke(AudioHandle trackHandle, enum InvokeID invokeID, struct InvokeAttr *invokeAttr)
{
#ifdef AUDIO_SDK_SUPPORT
    int32_t ret = 0;
    switch (invokeID) {
        case INVOKE_ID_GET_CHANNEL_ID:
        {
            SubPortTrackOutContext *portTrackOutCtx = (SubPortTrackOutContext *)trackHandle;
            *((uint32_t *)invokeAttr->request) = portTrackOutCtx->inputAdpHandle;
            invokeAttr->replyBytes = (uint64_t)sizeof(uint32_t);
            MEDIA_HAL_LOGI(MODULE_NAME, "data channel id: 0x%x", *((uint32_t *)invokeAttr->request));
            break;
        }
        case INVOKE_ID_SET_EXTRA_PARAMS:
        case INVOKE_ID_GET_EXTRA_PARAMS:
        {
            size_t size = sizeof(g_primaryOutInvokeFuncList) / sizeof(struct AudioPortInvoke);
            ret = ExtraParamsInvoke(trackHandle, invokeAttr, g_primaryOutInvokeFuncList, (uint32_t)size);
            CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "extra param invoke failed!");
            break;
        }
        default:
            MEDIA_HAL_LOGW(MODULE_NAME, "invalid invoke id: %d", invokeID);
            return MEDIA_HAL_INVALID_PARAM;
    }
#else
    MEDIA_HAL_UNUSED(trackHandle);
    MEDIA_HAL_UNUSED(invokeID);
    MEDIA_HAL_UNUSED(invokeAttr);
#endif

    MEDIA_HAL_LOGD(MODULE_NAME, "invoke %d success", invokeID);
    return MEDIA_HAL_OK;
}

int32_t AudioPrimarySubPortOutCreate(AudioHandle *portHandle)
{
#ifdef AUDIO_SDK_SUPPORT
    if (!g_subPortOutMngInited) {
        SubPortOutManagerInit();
    }

    if (!g_subPortTrackOutMngInited) {
        SubPortTrackOutManagerInit();
    }

    int32_t sndDevId = SubPortOutManagerGetFreeDev();
    CHK_COND_RETURN(sndDevId == INVALID_ID, MEDIA_HAL_ERR, "no free AO device");

    int32_t ret;
    SubPortOutContext *portOutCtx = (SubPortOutContext *)malloc(sizeof(SubPortOutContext));
    if (portOutCtx == NULL) {
        MEDIA_HAL_LOGI(MODULE_NAME, "malloc failed");
        ret = MEDIA_HAL_NO_MEM;
        goto FREE_SND_DEV;
    }

    if (memset_s(portOutCtx, sizeof(SubPortOutContext), 0, sizeof(SubPortOutContext)) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "memset_s failed");
        ret = MEDIA_HAL_ERR;
        goto FREE_PORT;
    }
    portOutCtx->createTrackMutex = MediaHalMutexCreate(NULL);
    if (portOutCtx->createTrackMutex == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "create mutex failed");
        goto FREE_PORT;
    }
    portOutCtx->sndDevId = sndDevId;
    *portHandle = portOutCtx;
#else
    MEDIA_HAL_UNUSED(portHandle);
#endif
    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;

#ifdef AUDIO_SDK_SUPPORT
FREE_PORT:
    free(portOutCtx);
FREE_SND_DEV:
    (void)SubPortOutManagerFreeDev(sndDevId);
    return ret;
#endif
}

int32_t AudioPrimarySubPortOutDestroy(AudioHandle portHandle)
{
#ifdef AUDIO_SDK_SUPPORT
    SubPortOutContext *portOutCtx = (SubPortOutContext *)portHandle;

    (void)SubPortOutManagerFreeDev(portOutCtx->sndDevId);

    (void)SubPortTrackOutManagerDeInit();

    MediaHalMutexDestroy(&portOutCtx->createTrackMutex);

    free(portOutCtx);
#else
    MEDIA_HAL_UNUSED(portHandle);
#endif

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

int32_t AudioPrimarySubPortOutGetCapability(const struct AudioPort *port, struct AudioPortCapability *capability)
{
    capability->formats = g_supportFormats;
    MEDIA_HAL_UNUSED(port);
    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

int32_t AudioPrimarySubPortOutCreateTrack(AudioHandle portHandle, const struct PortPluginAttr *inputAttr,
    AudioHandle *trackHandle)
{
    SubPortOutContext *portOutCtx = (SubPortOutContext *)portHandle;
    MediaHalMutexLock(portOutCtx->createTrackMutex);
    int32_t sndTrackId = SubPortTrackOutManagerGetFreeTrack();
    if (sndTrackId == INVALID_ID) {
        MEDIA_HAL_LOGE(MODULE_NAME, "no free sound track");
        MediaHalMutexUnLock(portOutCtx->createTrackMutex);
        return MEDIA_HAL_INVALID_PARAM;
    }
    SubPortTrackOutContext *portTrackOutCtx = NULL;
    int32_t ret = SubPortTrackOutInit(&portTrackOutCtx, inputAttr);
    if (ret != MEDIA_HAL_OK || portTrackOutCtx == NULL) {
        goto FREE_TRACK;
    }
#ifdef AUDIO_SDK_SUPPORT
    ret = AODevOpen(portOutCtx, portTrackOutCtx->inputAttr.type);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AODevOpen failed");
        goto FREE_TRACK_CTX;
    }

    /* pcm: adp + track, es: adp + adec + track */
    ret = EstablishPipeline(portOutCtx, portTrackOutCtx);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "EstablishPipeline failed");
        portTrackOutCtx->subPortCtx = portOutCtx;
        goto AO_DEV_CLOSE;
    }

    portTrackOutCtx->sndTrackId = sndTrackId;
    portTrackOutCtx->subPortCtx = portOutCtx;
    portOutCtx->portTrack[sndTrackId] = portTrackOutCtx;
    *trackHandle = &portTrackOutCtx->common;
#endif
    MediaHalMutexUnLock(portOutCtx->createTrackMutex);
    return MEDIA_HAL_OK;
AO_DEV_CLOSE:
#ifdef AUDIO_SDK_SUPPORT
    (void)AODevClose(portOutCtx->sndHandle);
#endif
FREE_TRACK_CTX:
    free(portTrackOutCtx);
FREE_TRACK:
#ifdef AUDIO_SDK_SUPPORT
    (void)SubPortTrackOutManagerFreeTrack(sndTrackId);
#endif
    MediaHalMutexUnLock(portOutCtx->createTrackMutex);
    return ret;
}

static bool PrimaryPortIsValidTrack(const SubPortOutContext *portOutCtx, const SubPortTrackOutContext *portTrackOutCtx)
{
#ifdef AUDIO_SDK_SUPPORT
    for (int32_t i = 0; i < AO_DEV_MAX_TRACK_NUM; i++) {
        if (portOutCtx->portTrack[i] == portTrackOutCtx) {
            return true;
        }
    }
#else
    MEDIA_HAL_UNUSED(portOutCtx);
    MEDIA_HAL_UNUSED(portTrackOutCtx);
#endif
    return false;
}

int32_t AudioPrimarySubPortOutDestroyTrack(AudioHandle portHandle, AudioHandle trackHandle)
{
    SubPortOutContext *portOutCtx = (SubPortOutContext *)portHandle;
    SubPortTrackOutContext *portTrackOutCtx = (SubPortTrackOutContext *)trackHandle;
    MediaHalMutexLock(portOutCtx->createTrackMutex);
    if (!PrimaryPortIsValidTrack(portOutCtx, portTrackOutCtx)) {
        MEDIA_HAL_LOGE(MODULE_NAME, "track is invalid");
        MediaHalMutexUnLock(portOutCtx->createTrackMutex);
        return MEDIA_HAL_INVALID_PARAM;
    }

#ifdef AUDIO_SDK_SUPPORT
    int32_t ret = MEDIA_HAL_ERR;
    // 如果支持 SDK 小型化方案，暂停的时候会销毁通路，正常退出和切换输出设备也会销毁通路，需要检查下，典型场景如下：
    // 1.本地音乐暂停状态退出播放
    // 2.本地音乐暂停状态连接蓝牙耳机
    // 3.本地音乐播放中途接听电话，通话中途连接耳机，通话挂断后恢复播放
    if (!portTrackOutCtx->isReleased) {
        ret = SndDetach(portTrackOutCtx);
        CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "SndDetach failed");

        MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_destroy_track(portTrackOutCtx->trackHandle));
        CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "uapi_snd_destroy_track failed");

        ret = AODevClose(portOutCtx->sndHandle);
        CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "AODevClose failed");
    }
    ret = SubPortTrackOutManagerFreeTrack(portTrackOutCtx->sndTrackId);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "SubPortTrackOutManagerFreeTrack failed");
#endif
    MediaHalMutexUnLock(portOutCtx->createTrackMutex);
    if (portTrackOutCtx->inputAttr.extraData != NULL) {
        free(portTrackOutCtx->inputAttr.extraData);
        portTrackOutCtx->inputAttr.extraData = NULL;
        portTrackOutCtx->inputAttr.extraDataSize = 0;
    }

    free(portTrackOutCtx);
    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

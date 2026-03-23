/*
* Copyright (c) @CompanyNameMagicTag. 2021-2021. All rights reserved.
* Description: primary sub port in
* Author: Media Software Group
* Create: 2021-09-28
*/

#include "primary_sub_port_in.h"

#include "securec.h"

#ifdef AUDIO_SDK_SUPPORT

#include "soc_uapi_ai.h"
#include "soc_uapi_adp.h"
#include "soc_uapi_aenc.h"
#include "soc_uapi_sea.h"
#endif

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

#define MODULE_NAME "AudioPrimarySubPortIn"

/* need fix: used for external buffer allocation, internal means samples per frame */
#define AUDIO_HAL_FRAME_COUNT 1024

#define VOLUME_UNIT 100
#define MIC_VOLUME   "micvolume"
#define MIC_TYPE     "mictype"
#define SEA_ENABLE   "seaenable"
#define GET_SEA_PARAM "getseaparam"
#define SET_SEA_PARAM "setseaparam"
#define SET_BIT_RATE  "setbitrate"
#define SEA_PARAM_LEN 256
#define OGG_HEAD_LEN 8
#define MP3_ENC_BIT_RATE_PRECISION 1000

static enum AudioFormat g_supportFormats[] = {
    AUDIO_FORMAT_PCM_16_BIT,
    AUDIO_FORMAT_mSBC,
};

typedef struct {
    /* track impl */
    AudioPortTrack common;
    /* ai context */
    uint64_t frameIndex;
#ifdef AUDIO_SDK_SUPPORT
    td_handle aiHandle;
    struct AudioTimeStamp aiTimeStamp;
    /* adp context */
    td_handle outputAdpHandle;
    /* sea context */
    td_handle seaHandle;
    /* aenc context */
    td_handle aencHandle;
    uapi_acodec_id aencCodecId;
#endif
    /* attr context */
    bool isRaw;
    struct PortPluginAttr inputAttr;
    bool isSupportMultiFrame;
    int32_t offset;
} SubPortTrackInContext;

typedef struct {
#ifdef AUDIO_SDK_SUPPORT
    td_handle aiHandle;
#endif
    td_handle reserved;
} SubPortInContext;

static uint32_t g_subPortTrackInOpened = 0;
static MediaHalMutexHandle g_subPortTrackInLock = NULL;

typedef struct {
    enum AudioCategory audioType;
    enum AudioFormat audioFormat;
    uint32_t sampleRate;
    uint32_t channelCount;
} SubPortTrackInCapabilities;

static SubPortTrackInCapabilities g_caps[] = {
    { AUDIO_IN_MEDIA, AUDIO_FORMAT_SILK, 16000, 1 },
    { AUDIO_IN_MEDIA, AUDIO_FORMAT_MP3, 16000, 1 },
    { AUDIO_IN_MEDIA, AUDIO_FORMAT_SBC, 16000, 1 },
    { AUDIO_IN_MEDIA, AUDIO_FORMAT_PCM_16_BIT, 16000, 1 },
    { AUDIO_IN_MEDIA, AUDIO_FORMAT_OPUS, 16000, 1 },
    { AUDIO_IN_COMMUNICATION, AUDIO_FORMAT_mSBC, 16000, 1 },
    { AUDIO_IN_COMMUNICATION, AUDIO_FORMAT_AMR_WB, 16000, 1 },
    { AUDIO_IN_COMMUNICATION, AUDIO_FORMAT_PCM_16_BIT, 8000, 1},
    { AUDIO_IN_COMMUNICATION, AUDIO_FORMAT_PCM_16_BIT, 16000, 1},
    { AUDIO_IN_COMMUNICATION, AUDIO_FORMAT_OPUS, 16000, 1 },
    { AUDIO_IN_COMMUNICATION, AUDIO_FORMAT_MP3, 16000, 1 },
};

static bool IsSupported(const struct PortPluginAttr *inputAttr)
{
    for (uint32_t i = 0; i < ARRAY_SIZE(g_caps); i++) {
        if (inputAttr->type == g_caps[i].audioType &&
            inputAttr->audioFormat == g_caps[i].audioFormat &&
            inputAttr->sampleRate == g_caps[i].sampleRate &&
            inputAttr->channelCount == g_caps[i].channelCount) {
            return true;
        }
    }
    return false;
}

static int32_t SubPortTrackInInit(SubPortTrackInContext *portTrackInCtx, const struct PortPluginAttr *inputAttr)
{
    /* check whether the input attribute is supported */
    if (!IsSupported(inputAttr)) {
        MEDIA_HAL_LOGE(MODULE_NAME, "invalid input attribute");
        return MEDIA_HAL_INVALID_PARAM;
    }

    portTrackInCtx->inputAttr = *inputAttr;

    /* The default bit width is 16 bits. The 24-bit width is processed separately. */
    portTrackInCtx->inputAttr.bitWidth = AUDIO_BIT_WIDTH_16_BIT;

    portTrackInCtx->common.audioPin = PIN_IN_MIC;
    portTrackInCtx->common.GetFrameCount = AudioPrimarySubPortInGetFrameCount;
    portTrackInCtx->common.GetLatency = NULL;
    portTrackInCtx->common.EnableTrack = AudioPrimarySubPortInEnableTrack;
    portTrackInCtx->common.PauseTrack = NULL;
    portTrackInCtx->common.ResumeTrack = NULL;
    portTrackInCtx->common.FlushTrack = NULL;
    portTrackInCtx->common.DisableTrack = AudioPrimarySubPortInDisableTrack;
    portTrackInCtx->common.AcquireFrame = AudioPrimarySubPortInAcquireFrame;
    portTrackInCtx->common.SendFrame = NULL;
    portTrackInCtx->common.GetPosition = AudioPrimarySubPortInGetPosition;
    portTrackInCtx->common.SetMute = AudioPrimarySubPortInSetMute;
    portTrackInCtx->common.GetMute = AudioPrimarySubPortInGetMute;
    portTrackInCtx->common.SetVolume = AudioPrimarySubPortInSetVolume;
    portTrackInCtx->common.GetVolume = AudioPrimarySubPortInGetVolume;
    portTrackInCtx->common.Invoke = AudioPrimarySubPortInInvoke;
    portTrackInCtx->isSupportMultiFrame = false;
    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

#ifdef AUDIO_SDK_SUPPORT
static int32_t AIDevOpen(td_handle *aiHandle, enum AudioCategory audioCategory)
{
    MediaHalInitStaticMutexLock(&g_subPortTrackInLock);
    MediaHalMutexLock(g_subPortTrackInLock);
    if (g_subPortTrackInOpened == 0) {
        int32_t ret;
        uapi_ai_attr aiAttr = {};
        uapi_ai_port aiPort;
#ifdef MELODY_AUDIO_SUPPORT
        // workaround: currently, melody ai port is UAPI_AI_PORT_LPADC0, and then it'll be unified with brandy.
        aiPort = UAPI_AI_PORT_LPADC0;
#else
        aiPort = BUILTIN_AI_PORT;
#endif
        MEDIA_HAL_DOFUNC_TIME(ret = uapi_ai_get_default_attr(aiPort, &aiAttr));
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "uapi_ai_get_default_attr failed");
            MediaHalMutexUnLock(g_subPortTrackInLock);
            return ret;
        }
#ifndef MELODY_AUDIO_SUPPORT
        GetBuiltinAIAttr(&aiAttr);
#endif
        if (audioCategory == AUDIO_IN_COMMUNICATION) {
#ifdef MELODY_AUDIO_SUPPORT
            aiAttr.ref_attr.enable = TD_TRUE;
            aiAttr.ref_attr.port = UAPI_SND_OUT_PORT_DAC0;
#else
            aiAttr.ref_attr.enable = TD_TRUE;
            aiAttr.ref_attr.port = BUILTIN_SND_OUT_PORT;
#endif
        } else {
            aiAttr.ref_attr.enable = TD_FALSE;
        }
        MEDIA_HAL_DOFUNC_TIME(ret = uapi_ai_open(aiHandle, aiPort, &aiAttr));
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "uapi_ai_open failed");
            MediaHalMutexUnLock(g_subPortTrackInLock);
            return ret;
        }
        uapi_ai_gain gain = {};
        gain.integer = DEFAULTE_AI_VOL_DB_INTERGER;
        gain.decimal = 0;
        MEDIA_HAL_LOGI(MODULE_NAME, "set default AI volume.integer = %d,  decimal = %d", gain.integer, gain.decimal);
        MEDIA_HAL_DOFUNC_TIME(ret = uapi_ai_set_volume(*aiHandle, &gain));
        CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "uapi_ai_set_volume failed");
    }
    g_subPortTrackInOpened++;
    MediaHalMutexUnLock(g_subPortTrackInLock);

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}
#endif

#ifdef AUDIO_SDK_SUPPORT
static int32_t AIDevClose(td_handle aiHandle)
{
    MediaHalInitStaticMutexLock(&g_subPortTrackInLock);
    MediaHalMutexLock(g_subPortTrackInLock);
    g_subPortTrackInOpened--;
    if (g_subPortTrackInOpened == 0) {
        int32_t ret;
        MEDIA_HAL_DOFUNC_TIME(ret = uapi_ai_close(aiHandle));
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "uapi_ai_close failed");
            MediaHalMutexUnLock(g_subPortTrackInLock);
            return ret;
        }
    }
    MediaHalMutexUnLock(g_subPortTrackInLock);

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}
#endif

static int32_t AudioSDKOpenInternal(SubPortTrackInContext *portTrackInCtx)
{
#ifdef AUDIO_SDK_SUPPORT
    int32_t ret = AudioSDKAdpOpen(&portTrackInCtx->outputAdpHandle);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "output adp open failed");
    struct PortPluginAttr inputAttrTemp = portTrackInCtx->inputAttr;
    uapi_aenc_attr aencAttr = {};
    uapi_acodec_id aencCodecId;
#ifdef AUDIO_SEA_SUPPORT
    ret = AudioSDKSeaOpen(&portTrackInCtx->seaHandle, inputAttrTemp);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "sea open failed");
        goto ADP_CLOSE;
    }
#endif
    if (!AudioConvertAudioFormatToCodecId(inputAttrTemp.audioFormat, &aencCodecId)) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioConvertAudioFormatToCodecId failed");
        ret = MEDIA_HAL_INVALID_PARAM;
        goto SEA_CLOSE;
    }
    aencAttr.codec_id = aencCodecId;
    aencAttr.param.interleaved = true;
    if (inputAttrTemp.type == AUDIO_IN_MEDIA) {
        aencAttr.frame_mode_output = TD_TRUE;
        portTrackInCtx->isSupportMultiFrame = !aencAttr.frame_mode_output;
    }
    aencAttr.param.channels = inputAttrTemp.channelCount;
    aencAttr.param.bit_depth = inputAttrTemp.bitWidth;
    aencAttr.param.sample_rate = inputAttrTemp.sampleRate;
    aencAttr.param.samples_per_frame = ((uint32_t)inputAttrTemp.sampleRate) / SAMPLERATE_TO_PTNUMPERFRM;
    aencAttr.param.private_data = NULL;
    aencAttr.param.private_data_size = 0;
    uint32_t bitRate = MP3_ENC_BIT_RATE / MP3_ENC_BIT_RATE_PRECISION;
    if (aencCodecId == UAPI_ACODEC_ID_MP3) {
        aencAttr.param.private_data = &bitRate;
        aencAttr.param.private_data_size = sizeof(bitRate);
    }
    ret = AudioSDKAencOpen(&portTrackInCtx->aencHandle, aencAttr);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "aenc open failed");
        goto SEA_CLOSE;
    }
#else
    MEDIA_HAL_UNUSED(portTrackInCtx);
#endif
    return MEDIA_HAL_OK;
#ifdef AUDIO_SDK_SUPPORT
SEA_CLOSE:
#ifdef AUDIO_SEA_SUPPORT
    (void)AudioSDKSeaClose(portTrackInCtx->seaHandle);
ADP_CLOSE:
#endif
    (void)AudioSDKAdpClose(portTrackInCtx->outputAdpHandle);
    return ret;
#endif
}

static int32_t AudioSDKCloseInternal(SubPortTrackInContext *portTrackInCtx)
{
#ifdef AUDIO_SDK_SUPPORT
    int32_t ret = AudioSDKAdpClose(portTrackInCtx->outputAdpHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "adp close failed");

#ifdef AUDIO_SEA_SUPPORT
    ret = AudioSDKSeaClose(portTrackInCtx->seaHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "sea close failed");
#endif

    ret = AudioSDKAencClose(portTrackInCtx->aencHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "aenc close failed");
#else
    MEDIA_HAL_UNUSED(portTrackInCtx);
#endif

    return MEDIA_HAL_OK;
}

#ifdef AUDIO_SDK_SUPPORT
static int32_t AudioSDKAttachInternal(td_handle aiHandle, SubPortTrackInContext *portTrackInCtx)
{
#ifdef AUDIO_SEA_SUPPORT
    int32_t ret = AudioSDKAttach(AUDIO_MOD_AI, aiHandle, portTrackInCtx->seaHandle);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "ai attach sea failed");

    ret = AudioSDKAttach(AUDIO_MOD_SEA, portTrackInCtx->seaHandle, portTrackInCtx->aencHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "sea attach aenc failed");
        goto AI_DETACH_SEA;
    }
#else
    int32_t ret = AudioSDKAttach(AUDIO_MOD_AI, aiHandle, portTrackInCtx->aencHandle);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "ai attach aenc failed");
#endif

    ret = AudioSDKAttach(AUDIO_MOD_AENC, portTrackInCtx->aencHandle, portTrackInCtx->outputAdpHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "aenc attach adp failed");
#ifdef AUDIO_SEA_SUPPORT
        goto SEA_DETACH_AENC;
#else
        goto AI_DETACH_AENC;
#endif
    }

    return MEDIA_HAL_OK;

#ifdef AUDIO_SEA_SUPPORT
SEA_DETACH_AENC:
    (void)AudioSDKDetach(AUDIO_MOD_SEA, portTrackInCtx->seaHandle, portTrackInCtx->aencHandle);
AI_DETACH_SEA:
    (void)AudioSDKDetach(AUDIO_MOD_AI, portTrackInCtx->aiHandle, portTrackInCtx->seaHandle);
#else
AI_DETACH_AENC:
    (void)AudioSDKDetach(AUDIO_MOD_AI, portTrackInCtx->aiHandle, portTrackInCtx->aencHandle);
#endif
    return ret;
}
#endif

#ifdef AUDIO_SDK_SUPPORT
static int32_t EstablishPipeline(td_handle aiHandle, SubPortTrackInContext *portTrackInCtx)
{
    int32_t ret;
    portTrackInCtx->isRaw = IsRawFormat(portTrackInCtx->inputAttr.audioFormat);
    if (portTrackInCtx->isRaw) {
        ret = AudioSDKAdpOpen(&portTrackInCtx->outputAdpHandle);
        CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "output adp open failed");
        ret = AudioSDKAttach(AUDIO_MOD_AI, aiHandle, portTrackInCtx->outputAdpHandle);
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "ai attach adp failed");
            goto ADP_CLOSE;
        }
    } else {
        ret = AudioSDKOpenInternal(portTrackInCtx);
        CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "AudioSDKOpenInternal failed");

        ret = AudioSDKAttachInternal(aiHandle, portTrackInCtx);
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "AudioSDKAttachInternal failed");
            goto AUDIO_SDK_CLOSE;
        }
    }

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;

AUDIO_SDK_CLOSE:
    (void)AudioSDKCloseInternal(portTrackInCtx);
ADP_CLOSE:
    (void)AudioSDKAdpClose(portTrackInCtx->outputAdpHandle);
    return ret;
}
#endif

static int32_t AIDetach(const SubPortTrackInContext *portTrackInCtx)
{
#ifdef AUDIO_SDK_SUPPORT
    int32_t ret;

    if (portTrackInCtx->isRaw) {
        ret = AudioSDKDetach(AUDIO_MOD_AI, portTrackInCtx->aiHandle, portTrackInCtx->outputAdpHandle);
        CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "ai detach adp failed");
    } else {
#ifdef AUDIO_SEA_SUPPORT
        ret = AudioSDKDetach(AUDIO_MOD_AI, portTrackInCtx->aiHandle, portTrackInCtx->seaHandle);
        CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "ai detach sea failed");
        ret = AudioSDKDetach(AUDIO_MOD_SEA, portTrackInCtx->seaHandle, portTrackInCtx->aencHandle);
        CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "sea detach aenc failed");
#else
        ret = AudioSDKDetach(AUDIO_MOD_AI, portTrackInCtx->aiHandle, portTrackInCtx->aencHandle);
        CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "ai detach aenc failed");
#endif
        ret = AudioSDKDetach(AUDIO_MOD_AENC, portTrackInCtx->aencHandle, portTrackInCtx->outputAdpHandle);
        CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "aenc detach adp failed");
#ifdef AUDIO_SEA_SUPPORT
        ret = AudioSDKSeaClose(portTrackInCtx->seaHandle);
        CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "sea close failed");
#endif
        ret = AudioSDKAencClose(portTrackInCtx->aencHandle);
        CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "aenc close failed");
    }
    ret = AudioSDKAdpClose(portTrackInCtx->outputAdpHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "adp close failed");
#else
    MEDIA_HAL_UNUSED(portTrackInCtx);
#endif

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

int32_t AudioPrimarySubPortInGetFrameCount(const AudioHandle trackHandle, uint64_t *count)
{
    MEDIA_HAL_UNUSED(trackHandle);

    *count = AUDIO_HAL_FRAME_COUNT;

    MEDIA_HAL_LOGI(MODULE_NAME, "frame count: %llu", *count);
    return MEDIA_HAL_OK;
}

int32_t AudioPrimarySubPortInEnableTrack(AudioHandle trackHandle)
{
#ifdef AUDIO_SDK_SUPPORT
    SubPortTrackInContext *portTrackInCtx = (SubPortTrackInContext *)trackHandle;

    int32_t ret;
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_ai_start(portTrackInCtx->aiHandle));
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_ai_start failed");

    if (!portTrackInCtx->isRaw) {
#ifdef AUDIO_SEA_SUPPORT
        MEDIA_HAL_DOFUNC_TIME(ret = uapi_sea_start(portTrackInCtx->seaHandle));
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGI(MODULE_NAME, "uapi_sea_start failed");
            goto AI_STOP;
        }
#endif
        MEDIA_HAL_DOFUNC_TIME(ret = uapi_aenc_start(portTrackInCtx->aencHandle));
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGI(MODULE_NAME, "uapi_aenc_start failed");
            goto SEA_STOP;
        }
    }
#else
    MEDIA_HAL_UNUSED(trackHandle);
#endif

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;

#ifdef AUDIO_SDK_SUPPORT
SEA_STOP:
#ifdef AUDIO_SEA_SUPPORT
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_sea_stop(portTrackInCtx->seaHandle));
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "uapi_sea_stop failed");
AI_STOP:
#endif
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_ai_stop(portTrackInCtx->aiHandle));
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "uapi_ai_stop failed");
    return ret;
#endif
}

int32_t AudioPrimarySubPortInDisableTrack(AudioHandle trackHandle)
{
#ifdef AUDIO_SDK_SUPPORT
    SubPortTrackInContext *portTrackInCtx = (SubPortTrackInContext *)trackHandle;

    int32_t ret;
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_ai_stop(portTrackInCtx->aiHandle));
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_ai_stop failed");

    if (!portTrackInCtx->isRaw) {
#ifdef AUDIO_SEA_SUPPORT
        MEDIA_HAL_DOFUNC_TIME(ret = uapi_sea_stop(portTrackInCtx->seaHandle));
        CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_sea_stop failed");
#endif
        MEDIA_HAL_DOFUNC_TIME(ret = uapi_aenc_stop(portTrackInCtx->aencHandle));
        CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_aenc_stop failed");
    }
#else
    MEDIA_HAL_UNUSED(trackHandle);
#endif

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t CheckParam(SubPortTrackInContext *portTrackInCtx, uint8_t *buffer,
    uint64_t requestBytes, uapi_stream_buf *audioStream)
{
    if (requestBytes < audioStream->size) {
        if (portTrackInCtx->isSupportMultiFrame) {
            audioStream->size = requestBytes;
        } else {
            if (memcpy_s(buffer, (size_t)requestBytes, audioStream->data + portTrackInCtx->offset,
                (size_t)requestBytes) != EOK) {
                MEDIA_HAL_LOGE(MODULE_NAME, "memcpy_s failed");
                return MEDIA_HAL_ERR;
            }
            return MEDIA_HAL_NOT_SUPPORT_GET_MULT_FRAME;
        }
    }
    if (requestBytes > SIZE_MAX) {
        MEDIA_HAL_LOGE(MODULE_NAME, "input buffer size overflow");
        return MEDIA_HAL_INVALID_PARAM;
    }
    if (portTrackInCtx->inputAttr.audioFormat == AUDIO_FORMAT_OPUS) {
        portTrackInCtx->offset = OGG_HEAD_LEN;
    } else {
        portTrackInCtx->offset = 0;
    }
    if (memcpy_s(buffer, (size_t)requestBytes, audioStream->data + portTrackInCtx->offset,
        audioStream->size - portTrackInCtx->offset) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "memcpy_s failed");
        return MEDIA_HAL_ERR;
    }
    int64_t sec = 0;
    int64_t nsec = 0;
    int32_t ret = MediaHalConvertUSecToSecAndNSec(audioStream->pts, &sec, &nsec);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "convert time from us to sec and nsec failed");
        return MEDIA_HAL_ERR;
    }

    portTrackInCtx->aiTimeStamp.tvSec = sec;
    portTrackInCtx->aiTimeStamp.tvNSec = nsec;
    portTrackInCtx->frameIndex++;
    return MEDIA_HAL_OK;
}

int32_t AudioPrimarySubPortInAcquireFrame(AudioHandle trackHandle, uint8_t *buffer, uint64_t requestBytes,
    uint64_t *replyBytes)
{
#ifdef AUDIO_SDK_SUPPORT
    SubPortTrackInContext *portTrackInCtx = (SubPortTrackInContext *)trackHandle;

    uapi_stream_buf audioStream = {};
    int32_t ret = uapi_adp_acquire_stream(portTrackInCtx->outputAdpHandle, &audioStream);
    if (ret != MEDIA_HAL_OK) {
        *replyBytes = 0;
        return MEDIA_HAL_ERR_AGAIN;
    }
    ret = CheckParam(portTrackInCtx, buffer, requestBytes, &audioStream);
    if (ret != MEDIA_HAL_OK) {
        if (ret == MEDIA_HAL_NOT_SUPPORT_GET_MULT_FRAME) {
            MEDIA_HAL_LOGI(MODULE_NAME, "acquire buffer lenth bigger than requestBytes");
            audioStream.size = requestBytes;
            *replyBytes = requestBytes;
            (void)uapi_adp_release_stream(portTrackInCtx->outputAdpHandle, &audioStream);
            return MEDIA_HAL_OK;
        }
        MEDIA_HAL_LOGE(MODULE_NAME, "param is invalid");
        return ret;
    }
    *replyBytes = audioStream.size - portTrackInCtx->offset;
    ret = uapi_adp_release_stream(portTrackInCtx->outputAdpHandle, &audioStream);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "release stream failed: 0x%x", ret);
    }
#else
    MEDIA_HAL_UNUSED(buffer);
    MEDIA_HAL_UNUSED(requestBytes);
    MEDIA_HAL_UNUSED(replyBytes);
#endif

    return MEDIA_HAL_OK;
}

int32_t AudioPrimarySubPortInGetPosition(AudioHandle trackHandle,
    uint64_t *frames, struct AudioTimeStamp *time)
{
#ifdef AUDIO_SDK_SUPPORT
    SubPortTrackInContext *portTrackInCtx = (SubPortTrackInContext *)trackHandle;

    time->tvSec = portTrackInCtx->aiTimeStamp.tvSec;
    time->tvNSec = portTrackInCtx->aiTimeStamp.tvNSec;
    *frames = portTrackInCtx->frameIndex;
#else
    MEDIA_HAL_UNUSED(trackHandle);
    MEDIA_HAL_UNUSED(frames);
    MEDIA_HAL_UNUSED(time);
#endif

    return MEDIA_HAL_OK;
}

int32_t AudioPrimarySubPortInSetMute(AudioHandle trackHandle, bool mute)
{
#ifdef AUDIO_SDK_SUPPORT
    SubPortTrackInContext *portTrackInCtx = (SubPortTrackInContext *)trackHandle;

    int32_t ret;
    td_bool isMute = (mute == true) ? TD_TRUE : TD_FALSE;
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_ai_set_mute(portTrackInCtx->aiHandle, isMute));
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_ai_set_mute failed");
#else
    MEDIA_HAL_UNUSED(trackHandle);
#endif

    MEDIA_HAL_LOGI(MODULE_NAME, "set mute %d success", mute);
    return MEDIA_HAL_OK;
}

int32_t AudioPrimarySubPortInGetMute(AudioHandle trackHandle, bool *mute)
{
#ifdef AUDIO_SDK_SUPPORT
    SubPortTrackInContext *portTrackInCtx = (SubPortTrackInContext *)trackHandle;

    int32_t ret;
    td_bool isMute = TD_FALSE;
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_ai_get_mute(portTrackInCtx->aiHandle, &isMute));
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_ai_get_mute failed");
    *mute = (isMute == TD_TRUE) ? true : false;
#else
    MEDIA_HAL_UNUSED(trackHandle);
#endif

    MEDIA_HAL_LOGI(MODULE_NAME, "get mute %d success", *mute);
    return MEDIA_HAL_OK;
}

int32_t AudioPrimarySubPortInSetVolume(AudioHandle trackHandle, float volume)
{
#ifdef AUDIO_SDK_SUPPORT
    SubPortTrackInContext *portTrackInCtx = (SubPortTrackInContext *)trackHandle;

    int32_t ret;
    uapi_ai_gain gain = {};
    gain.integer = (td_s32)(volume * VOLUME_UNIT);
    gain.decimal = 0;
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_ai_set_volume(portTrackInCtx->aiHandle, &gain));
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_ai_set_volume failed");
#else
    MEDIA_HAL_UNUSED(trackHandle);
#endif

    MEDIA_HAL_LOGI(MODULE_NAME, "set volume %f success", volume);
    return MEDIA_HAL_OK;
}

int32_t AudioPrimarySubPortInGetVolume(AudioHandle trackHandle, float *volume)
{
#ifdef AUDIO_SDK_SUPPORT
    SubPortTrackInContext *portTrackInCtx = (SubPortTrackInContext *)trackHandle;

    int32_t ret;
    uapi_ai_gain gain = {};
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_ai_get_volume(portTrackInCtx->aiHandle, &gain));
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_ai_get_volume failed");

    *volume = (float)gain.integer;
#else
    MEDIA_HAL_UNUSED(trackHandle);
#endif

    MEDIA_HAL_LOGI(MODULE_NAME, "get volume %f success", *volume);
    return MEDIA_HAL_OK;
}
#ifdef AUDIO_SDK_SUPPORT
static int32_t PrimarySetSeaEnable(AudioHandle trackHandle, char *value, uint32_t valueLen)
{
    SubPortTrackInContext *portTrackInCtx = (SubPortTrackInContext *)trackHandle;
    CHK_NULL_RETURN(value, MEDIA_HAL_INVALID_PARAM, "input param value is null");
#ifdef MELODY_AUDIO_SUPPORT
    if (!portTrackInCtx->isNeedOpenSea) {
        MEDIA_HAL_LOGE(MODULE_NAME, "the sea is not open. can not set sea options!");
        return MEDIA_HAL_ERR;
    }
#endif
    return SetSeaEnable(portTrackInCtx->seaHandle, value, valueLen);
}

static int32_t SetSeaParam(AudioHandle trackHandle, char *value, uint32_t valueLen)
{
    MEDIA_HAL_UNUSED(valueLen);
    SubPortTrackInContext *portTrackInCtx = (SubPortTrackInContext *)trackHandle;
    CHK_NULL_RETURN(value, MEDIA_HAL_INVALID_PARAM, "input param value is null");
    uint32_t paramLen = 0;
    uint8_t configFlag[MAX_KEY_STR_SIZE] = { 0 };
    int32_t ret = sscanf_s(value, "%s = %u:", configFlag, MAX_KEY_STR_SIZE, &paramLen);
    if (ret == -1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "set aef param sscanf_s failed!");
        return MEDIA_HAL_ERR;
    }
    uint8_t param[SEA_PARAM_LEN] = { 0 };
    ret = GetParamByValue(param, SEA_PARAM_LEN, value, valueLen, paramLen);
    if (ret != MEDIA_HAL_OK) {
        return ret;
    }
    ret = uapi_sea_set_param(portTrackInCtx->seaHandle, UAPI_SEA_AFE_SEE, (const td_void *)param, paramLen);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "set aef param failed! ret = %d", ret);
        return ret;
    }
    return MEDIA_HAL_OK;
}

static int32_t SetBitRate(AudioHandle trackHandle, char *value, uint32_t valueLen)
{
    MEDIA_HAL_UNUSED(valueLen);
    SubPortTrackInContext *portTrackInCtx = (SubPortTrackInContext *)trackHandle;
    CHK_NULL_RETURN(value, MEDIA_HAL_INVALID_PARAM, "input param value is null");
    uint32_t bitRate = 0;
    int32_t ret = sscanf_s(value, "%u", &bitRate);
    if (ret == -1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "set bit rate sscanf_s failed!");
        return MEDIA_HAL_ERR;
    }
    ret = uapi_aenc_set_param(portTrackInCtx->aencHandle, UAPI_AENC_PARAM_BIT_RATE, &bitRate, sizeof(uint32_t));
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "uapi set aenc send cmd bit Rate faild, ret: 0x%x", ret);
        return MEDIA_HAL_ERR;
    }
    return MEDIA_HAL_OK;
}

static int32_t GetSeaParam(AudioHandle trackHandle, char *value, uint32_t valueLen)
{
    MEDIA_HAL_UNUSED(valueLen);
    SubPortTrackInContext *portTrackInCtx = (SubPortTrackInContext *)trackHandle;
    CHK_NULL_RETURN(value, MEDIA_HAL_INVALID_PARAM, "input param value is null");
    uint8_t configFlag[MAX_KEY_STR_SIZE] = { 0 };
    uint32_t paramLen = 0;
    int32_t ret = sscanf_s((const char *)value, "%s = %u", configFlag, MAX_KEY_STR_SIZE, &paramLen);
    if (ret == -1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "set aef param sscanf_s failed!");
        return MEDIA_HAL_ERR;
    }
    ret = uapi_sea_get_param(portTrackInCtx->seaHandle, UAPI_SEA_AFE_SEE, (td_void *)value, paramLen);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "get aef param failed! ret = %d", ret);
        return ret;
    }
    return MEDIA_HAL_OK;
}

static struct AudioPortInvoke g_primaryInInvokeFuncList[] = {
    {SEA_ENABLE, PrimarySetSeaEnable, true},
    {SET_SEA_PARAM, SetSeaParam, false},
    {GET_SEA_PARAM, GetSeaParam, false},
    {SET_BIT_RATE, SetBitRate, true},
};
#endif

int32_t AudioPrimarySubPortInInvoke(AudioHandle trackHandle, enum InvokeID invokeID, struct InvokeAttr *invokeAttr)
{
#ifdef AUDIO_SDK_SUPPORT
    int32_t ret = 0;
    switch (invokeID) {
        case INVOKE_ID_GET_CHANNEL_ID:
        {
            SubPortTrackInContext *portTrackInCtx = (SubPortTrackInContext *)trackHandle;
            *((uint32_t *)invokeAttr->request) = portTrackInCtx->outputAdpHandle;
            invokeAttr->replyBytes = (uint64_t)sizeof(uint32_t);
            MEDIA_HAL_LOGI(MODULE_NAME, "data channel id: 0x%x success", *((uint32_t *)invokeAttr->request));
            break;
        }
        case INVOKE_ID_SET_EXTRA_PARAMS:
        case INVOKE_ID_GET_EXTRA_PARAMS:
        {
            size_t size = sizeof(g_primaryInInvokeFuncList) / sizeof(struct AudioPortInvoke);
            ret = ExtraParamsInvoke(trackHandle, invokeAttr, g_primaryInInvokeFuncList, (uint32_t)size);
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

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

int32_t AudioPrimarySubPortInCreate(AudioHandle *portHandle)
{
    SubPortInContext *portInCtx = (SubPortInContext *)malloc(sizeof(SubPortInContext));
    if (portInCtx == NULL) {
        MEDIA_HAL_LOGI(MODULE_NAME, "malloc failed");
        return MEDIA_HAL_NO_MEM;
    }

    int32_t ret;
    if (memset_s(portInCtx, sizeof(SubPortInContext), 0, sizeof(SubPortInContext)) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "memset_s failed");
        ret = MEDIA_HAL_ERR;
        goto FREE;
    }

    *portHandle = portInCtx;
    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;

FREE:
    free(portInCtx);
    return ret;
}

int32_t AudioPrimarySubPortInDestroy(AudioHandle portHandle)
{
    SubPortInContext *portInCtx = (SubPortInContext *)portHandle;

    free(portInCtx);

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

int32_t AudioPrimarySubPortInGetCapability(const struct AudioPort *port, struct AudioPortCapability *capability)
{
    MEDIA_HAL_UNUSED(port);
    capability->formats =  g_supportFormats;

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

int32_t AudioPrimarySubPortInCreateTrack(AudioHandle portHandle, const struct PortPluginAttr *inputAttr,
    AudioHandle *trackHandle)
{
    SubPortInContext *portInCtx = (SubPortInContext *)portHandle;

    SubPortTrackInContext *portTrackInCtx = (SubPortTrackInContext *)malloc(sizeof(SubPortTrackInContext));
    if (portTrackInCtx == NULL) {
        MEDIA_HAL_LOGI(MODULE_NAME, "malloc failed");
        return MEDIA_HAL_NO_MEM;
    }

    int32_t ret;
    if (memset_s(portTrackInCtx, sizeof(SubPortTrackInContext), 0, sizeof(SubPortTrackInContext)) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "memset_s failed");
        ret = MEDIA_HAL_ERR;
        goto FREE_TRACK_IN;
    }

    ret = SubPortTrackInInit(portTrackInCtx, inputAttr);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "SubPortTrackInInit failed");
        goto FREE_TRACK_IN;
    }

#ifdef AUDIO_SDK_SUPPORT
    ret = AIDevOpen(&portInCtx->aiHandle, inputAttr->type);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AIDevOpen failed");
        goto FREE_TRACK_IN;
    }

    /* pcm: ai + adp, es: ai + sea + aenc + adp */
    ret = EstablishPipeline(portInCtx->aiHandle, portTrackInCtx);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGI(MODULE_NAME, "EstablishPipeline failed: 0x%x", ret);
        goto AI_CLOSE;
    }

    portTrackInCtx->aiHandle = portInCtx->aiHandle;
    *trackHandle = &portTrackInCtx->common;
#endif

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;

AI_CLOSE:
#ifdef AUDIO_SDK_SUPPORT
    (void)AIDevClose(portInCtx->aiHandle);
#endif
FREE_TRACK_IN:
    free(portTrackInCtx);
    return ret;
}

int32_t AudioPrimarySubPortInDestroyTrack(AudioHandle portHandle, AudioHandle trackHandle)
{
#ifdef AUDIO_SDK_SUPPORT
    SubPortTrackInContext *portTrackInCtx = (SubPortTrackInContext *)trackHandle;

    int32_t ret = AIDetach(portTrackInCtx);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "AIDetach failed");

    ret = AIDevClose(portTrackInCtx->aiHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "AIDevClose failed");

    free(portTrackInCtx);
#else
    MEDIA_HAL_UNUSED(trackHandle);
#endif
    MEDIA_HAL_UNUSED(portHandle);
    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

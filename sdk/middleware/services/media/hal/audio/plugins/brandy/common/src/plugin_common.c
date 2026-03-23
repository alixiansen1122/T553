/*
* Copyright (c) @CompanyNameMagicTag. 2021-2021. All rights reserved.
* Description: audio plugin common
* Author: Media Software Group
* Create: 2021-05-18
*/
#include "plugin_common.h"
#include <unistd.h>
#include <string.h>
#include <math.h>
#include "media_hal_common.h"
#include "audio_types.h"

#include "primary_port.h"
#include "bluetooth_port.h"
#include "modem_port.h"
#if defined(BRANDY_PRODUCT_EVB)
#include "product_evb_standard.h"
#elif defined(BRANDY_PRODUCT_EVB4)
#include "product_evb4_standard.h"
#endif
#ifdef SUPPORT_NV
#include "nv.h"
#include "key_id.h"
#ifdef MELODY_AUDIO_SUPPORT
#include "hearing_aid_key_struct.h"
#else
#include "common.h"
#endif
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define MODULE_NAME "AudioPortPluginCommon"
#define WAIT_CREATE_AENC_LOOP_CNT 100
#define WAIT_CREATE_AENC_SUCCESS_US 10000

static bool g_audioSdkIsInited = false;
static bool g_isCanCreateAenc = true;
static MediaHalMutexHandle g_audioSdkInitLock = NULL;
#ifdef SUPPORT_NV
static audio_vqe_param_t g_vqeParam = { 0 };
static audio_aef_param_t g_aefParam = { 0 };
static uint8_t g_productTestMode = 0;
#endif
#define VOLUME_UNIT 100
#define VOLUME_DB_MIN (-70)
#define VOLUME_DB_MAX 0
#define PERCENT_TO_GAIN_LOG_FACTOR 20

void VolumePercentToDb(float volume, int32_t *db)
{
    double tmpDb = PERCENT_TO_GAIN_LOG_FACTOR * log(volume * VOLUME_UNIT / VOLUME_UNIT);
    if (tmpDb < VOLUME_DB_MIN) {
        tmpDb = VOLUME_DB_MIN;
    } else if (tmpDb > VOLUME_DB_MAX) {
        tmpDb = VOLUME_DB_MAX;
    }
    *db = (int32_t)tmpDb;
}

bool IsRawFormat(enum AudioFormat audioFormat)
{
    // The 24-bit PCM is processed as the ES stream and decoded by the ADEC.
    // The 16-bit PCM is processed as the ES stream for BT SCO msbc and cvsd pipeline same.
    if (audioFormat == AUDIO_FORMAT_PCM_8_BIT || audioFormat == AUDIO_FORMAT_PCM_32_BIT) {
        return true;
    }
    return false;
}

#ifdef AUDIO_SDK_SUPPORT
static AudioFormatMapping g_audioFormatMap[] = {
    {AUDIO_FORMAT_PCM_8_BIT, UAPI_ACODEC_ID_PCM},
    {AUDIO_FORMAT_PCM_16_BIT, UAPI_ACODEC_ID_PCM},
    {AUDIO_FORMAT_PCM_24_BIT, UAPI_ACODEC_ID_PCM},
    {AUDIO_FORMAT_PCM_32_BIT, UAPI_ACODEC_ID_PCM},
    {AUDIO_FORMAT_AAC_MAIN, UAPI_ACODEC_ID_AAC},
    {AUDIO_FORMAT_AAC_LC, UAPI_ACODEC_ID_AAC},
    {AUDIO_FORMAT_AAC_LD, UAPI_ACODEC_ID_AAC},
    {AUDIO_FORMAT_AAC_ELD, UAPI_ACODEC_ID_AAC},
    {AUDIO_FORMAT_AAC_HE_V1, UAPI_ACODEC_ID_AAC},
    {AUDIO_FORMAT_AAC_HE_V2, UAPI_ACODEC_ID_AAC},
    {AUDIO_FORMAT_SBC, UAPI_ACODEC_ID_SBC},
    {AUDIO_FORMAT_MP3, UAPI_ACODEC_ID_MP3},
    {AUDIO_FORMAT_mSBC, UAPI_ACODEC_ID_MSBC},
    {AUDIO_FORMAT_FLAC, UAPI_ACODEC_ID_FLAC},
    {AUDIO_FORMAT_OPUS, UAPI_ACODEC_ID_OPUS},
    {AUDIO_FORMAT_VORBIS, UAPI_ACODEC_ID_VORBIS},
    {AUDIO_FORMAT_APE, UAPI_ACODEC_ID_APE},
    {AUDIO_FORMAT_SILK, UAPI_ACODEC_ID_SILK},
    {AUDIO_FORMAT_L2HC, UAPI_ACODEC_ID_L2HC},
    {AUDIO_FORMAT_AMR_WB, UAPI_ACODEC_ID_AMRWB},
};

bool AudioConvertAudioFormatToCodecId(enum AudioFormat audioFormat, uapi_acodec_id *codecId)
{
    if (codecId == NULL) {
        return false;
    }
    for (uint32_t i = 0; i < sizeof(g_audioFormatMap) / sizeof(AudioFormatMapping); i++) {
        if (g_audioFormatMap[i].audioFormat == audioFormat) {
            *codecId = g_audioFormatMap[i].codecId;
            return true;
        }
    }
    MEDIA_HAL_LOGW(MODULE_NAME, "Invalid audioFormat: 0x%x", audioFormat);
    return false;
}

static void AudioNvRead(void)
{
    uint16_t len = 0;
    errcode_t nvRet = uapi_nv_read(NV_ID_FTM_CONFIG, sizeof(uint8_t), &len, &g_productTestMode);
    if (nvRet != ERRCODE_SUCC) {
        g_productTestMode = 0;
    }
    nvRet = uapi_nv_read(NV_ID_AUDIO_VQE_PARAM, (uint16_t)sizeof(audio_vqe_param_t), &len, (uint8_t *)&g_vqeParam);
    if (nvRet != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGW(MODULE_NAME, "nv read sea param faild!");
    }
    if (SND_PORT_AEF_TYPE == UAPI_AEF_TYPE_SWS1) {
        nvRet = uapi_nv_read(NV_ID_AUDIO_AEF_SWS_PARAM, (uint16_t)sizeof(audio_aef_param_t), &len,
            (uint8_t *)&g_aefParam);
        if (nvRet != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGW(MODULE_NAME, "nv read aef param faild!");
        }
    }
}

static int32_t AudioSDKInputInit(void)
{
    int32_t ret;
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_ai_init());
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "uapi_ai_init failed");
        return ret;
    }

#ifdef AUDIO_SEA_SUPPORT
#ifdef SUPPORT_NV
    AudioNvRead();
#endif
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_sea_init());
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "uapi_sea_init failed");
        goto AI_DEINIT;
    }
#endif
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_aenc_init());
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "uapi_aenc_init failed");
        goto SEA_DEINIT;
    }

    return MEDIA_HAL_OK;

SEA_DEINIT:
#ifdef AUDIO_SEA_SUPPORT
    (void)uapi_sea_deinit;
AI_DEINIT:
#endif
    (void)uapi_ai_deinit();
    return ret;
}

static int32_t AudioSDKInputDeInit(void)
{
    int32_t ret;
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_aenc_deinit());
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "uapi_aenc_deinit failed");

#ifdef AUDIO_SEA_SUPPORT
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_sea_deinit());
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "uapi_sea_deinit failed");
#endif

    MEDIA_HAL_DOFUNC_TIME(ret = uapi_ai_deinit());
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "uapi_ai_deinit failed");

    return MEDIA_HAL_OK;
}

static int32_t AudioSDKOutputInit(void)
{
    int32_t ret;
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_adec_init());
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "uapi_adec_init failed");
        return ret;
    }

    MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_init());
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "uapi_snd_init failed");
        goto ADEC_DEINIT;
    }

    return MEDIA_HAL_OK;

ADEC_DEINIT:
    (void)uapi_adec_deinit();
    return ret;
}

static int32_t AudioSDKOutputDeInit(void)
{
    int32_t ret;
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_deinit());
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "uapi_snd_deinit failed");

    MEDIA_HAL_DOFUNC_TIME(ret = uapi_adec_deinit());
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "uapi_adec_deinit failed");

    return MEDIA_HAL_OK;
}

int32_t AudioSDKInit()
{
    int32_t ret;

    MediaHalInitStaticMutexLock(&g_audioSdkInitLock);
    MediaHalMutexLock(g_audioSdkInitLock);
    if (g_audioSdkIsInited) {
        MEDIA_HAL_LOGI(MODULE_NAME, "audio sdk have inited");
        MediaHalMutexUnLock(g_audioSdkInitLock);
        return MEDIA_HAL_OK;
    }

    MEDIA_HAL_DOFUNC_TIME(ret = uapi_adp_init());
    if (ret != MEDIA_HAL_OK) {
        MediaHalMutexUnLock(g_audioSdkInitLock);
        MEDIA_HAL_LOGE(MODULE_NAME, "uapi_adp_init failed");
        return ret;
    }

    ret = AudioSDKInputInit();
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioSDKInputInit failed");
        goto ADP_DEINIT;
    }

    ret = AudioSDKOutputInit();
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioSDKOutputInit failed");
        goto INPUT_DEINIT;
    }

    g_audioSdkIsInited = true;
    MediaHalMutexUnLock(g_audioSdkInitLock);
    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;

INPUT_DEINIT:
    (void)AudioSDKInputDeInit();
ADP_DEINIT:
    (void)uapi_adp_deinit();
    MediaHalMutexUnLock(g_audioSdkInitLock);
    return ret;
}

int32_t AudioSDKDeInit(void)
{
    int32_t ret;
    MediaHalInitStaticMutexLock(&g_audioSdkInitLock);
    MediaHalMutexLock(g_audioSdkInitLock);

    if (!g_audioSdkIsInited) {
        MEDIA_HAL_LOGI(MODULE_NAME, "audio sdk not inited");
        MediaHalMutexUnLock(g_audioSdkInitLock);
        return MEDIA_HAL_OK;
    }

    ret = AudioSDKInputDeInit();
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "AudioSDKInputDeInit failed");

    ret = AudioSDKOutputDeInit();
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "AudioSDKOutputDeInit failed");

    MEDIA_HAL_DOFUNC_TIME(ret = uapi_adp_deinit());
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "uapi_adp_deinit failed");

    g_audioSdkIsInited = false;
    MediaHalMutexUnLock(g_audioSdkInitLock);
    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

int32_t AudioSDKAdpOpen(td_handle *adpHandle)
{
    int32_t ret;
    uapi_adp_attr adpAttr = {};
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_adp_get_def_attr(&adpAttr));
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_adp_get_def_attr failed");
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_adp_create(adpHandle, &adpAttr));
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_adp_create failed");

    MEDIA_HAL_LOGI(MODULE_NAME, "adpHandle: 0x%x", *adpHandle);

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

int32_t AudioSDKAdpClose(td_handle adpHandle)
{
    int32_t ret;
    MEDIA_HAL_LOGI(MODULE_NAME, "adpHandle: 0x%x", adpHandle);
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_adp_destroy(adpHandle));
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_adp_destroy failed");

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static bool IsSupportedCodecFormat(struct PortPluginAttr inputAttr)
{
    if (inputAttr.audioFormat == AUDIO_FORMAT_PCM_24_BIT) {
        return false;
    }

    return true;
}

int32_t AudioSDKAdecOpen(td_handle *adecHandle, struct PortPluginAttr inputAttr)
{
    int32_t ret;
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_adec_create(adecHandle));
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_adec_create failed");

    uapi_adec_attr adecAttr = {};
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_adec_get_attr(*adecHandle, &adecAttr));
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "uapi_adec_get_attr failed");

    uapi_acodec_id adecCodecId;
    if (!AudioConvertAudioFormatToCodecId(inputAttr.audioFormat, &adecCodecId)) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioConvertAudioFormatToCodecId failed");
        ret = MEDIA_HAL_INVALID_PARAM;
        goto CLOSE;
    }

    if (!IsSupportedCodecFormat(inputAttr)) {
        MEDIA_HAL_LOGE(MODULE_NAME, "unsupported codec format");
        ret = MEDIA_HAL_INVALID_PARAM;
        goto CLOSE;
    }

    adecAttr.codec_id = adecCodecId;
    adecAttr.param.sample_rate = inputAttr.sampleRate;
    adecAttr.param.channels = inputAttr.channelCount;
    adecAttr.param.bit_depth = inputAttr.bitWidth;
    if (inputAttr.isNeedSetExtraParam) {
        if (inputAttr.extraData != NULL && inputAttr.extraDataSize != 0) {
            adecAttr.param.private_data = inputAttr.extraData;
            adecAttr.param.private_data_size = inputAttr.extraDataSize;
        }
    }
    MEDIA_HAL_LOGI(MODULE_NAME, "codec_id: 0x%x, sample_rate: %d, channels: %d, bit_depth: %d",
        adecAttr.codec_id, adecAttr.param.sample_rate, adecAttr.param.channels, adecAttr.param.bit_depth);
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_adec_set_attr(*adecHandle, &adecAttr));
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "uapi_adec_set_attr failed: 0x%x, codec id: 0x%x", ret, adecCodecId);
        goto CLOSE;
    }

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;

CLOSE:
    MEDIA_HAL_DOFUNC_TIME((void)uapi_adec_destroy(*adecHandle));
    return ret;
}

int32_t AudioSDKAdecClose(td_handle adecHandle)
{
    int32_t ret;
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_adec_destroy(adecHandle));
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_adec_destroy failed");

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t SetSeaExtraParam(td_handle seaHandle)
{
    uint32_t size = g_vqeParam.size * (uint32_t)sizeof(g_vqeParam.data[0]);
    int32_t ret = uapi_sea_set_param(seaHandle, UAPI_SEA_AFE_SEE, (const td_void *)g_vqeParam.data, (td_u32)size);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "uapi set sea param faild, ret: 0x%x", ret);
        return MEDIA_HAL_ERR;
    }
    return MEDIA_HAL_OK;
}

void SetAefExtraParam(td_handle soundHandle)
{
    if (SND_PORT_AEF_TYPE == UAPI_AEF_TYPE_SWS1) {
        uint32_t size = g_aefParam.size * (uint32_t)sizeof(g_aefParam.data[0]);
        int32_t ret = uapi_snd_set_aef_param(soundHandle, BUILTIN_SND_OUT_PORT, SND_PORT_AEF_TYPE,
            (const td_void *)g_aefParam.data, (td_u32)size);
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "uapi set aef param faild, ret: 0x%x", ret);
        }
    }
}

static int32_t SetSeaAfeAttr(td_handle seaHandle)
{
    if (g_productTestMode == 0) {
        MEDIA_HAL_LOGI(MODULE_NAME, "sea does not need to be disabled in normal mode.");
        return MEDIA_HAL_OK;
    }
    uapi_sea_afe_attr afeAttr = {
        .type = UAPI_SEA_AFE_SEE,
        .mode = UAPI_SEA_NORM_MODE,
        .u.see.options = 0,
    };
    int32_t ret = uapi_sea_set_afe_attr(seaHandle, UAPI_SEA_AFE_SEE, &afeAttr);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "uapi set sea afe attr faild, ret: 0x%x", ret);
        return MEDIA_HAL_ERR;
    }
    return MEDIA_HAL_OK;
}

int32_t AudioSDKSeaOpen(td_handle *seaHandle, struct PortPluginAttr inputAttr)
{
    int32_t ret;
    uapi_sea_attr seaAttr = {};
    uapi_sea_eng_sel seaEng = {};

    seaEng.afe.type = UAPI_SEA_AFE_SEE;
    seaEng.afe.lib_id = UAPI_SEA_LIB_SEE;

    MEDIA_HAL_DOFUNC_TIME(ret = uapi_sea_load_engine(UAPI_SEA_LIB_SEE, "imedia_2mic"));
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "load sea engine failed");

    MEDIA_HAL_DOFUNC_TIME(ret =  uapi_sea_get_default_attr(&seaEng, &seaAttr));
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "get sea default attr failed");
        goto SEA_UNLOAD_ENGINE;
    }

    if (inputAttr.type == AUDIO_IN_COMMUNICATION) {
        seaAttr.ref_pcm.channels = inputAttr.channelCount;
    } else {
        seaAttr.ref_pcm.channels = 0;
    }

    MEDIA_HAL_DOFUNC_TIME(ret =  uapi_sea_create(seaHandle, &seaEng, &seaAttr));
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "create sea failed");
        goto SEA_UNLOAD_ENGINE;
    }
    ret = SetSeaAfeAttr(*seaHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGW(MODULE_NAME, "set sea aef attr failed");
    }
    ret = SetSeaExtraParam(*seaHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGW(MODULE_NAME, "set sea param failed");
    }
    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;

SEA_UNLOAD_ENGINE:
    (void)uapi_sea_unload_engine(UAPI_SEA_LIB_SEE, "imedia_2mic");
    return ret;
}

int32_t AudioSDKSeaClose(td_handle seaHandle)
{
    int32_t ret;
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_sea_destroy(seaHandle));
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "destroy sea failed");

    MEDIA_HAL_DOFUNC_TIME(ret = uapi_sea_unload_engine(UAPI_SEA_LIB_SEE, "imedia_2mic"));
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "unload sea engine failed");
    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t WaitCreateAnec(void)
{
    int32_t cnt = 0;
    while (cnt < WAIT_CREATE_AENC_LOOP_CNT) {
        MediaHalMutexLock(g_audioSdkInitLock);
        if (g_isCanCreateAenc) {
            g_isCanCreateAenc = false;
            MediaHalMutexUnLock(g_audioSdkInitLock);
            return MEDIA_HAL_OK;
        }
        MediaHalMutexUnLock(g_audioSdkInitLock);
        usleep(WAIT_CREATE_AENC_SUCCESS_US);
        cnt++;
    }
    return MEDIA_HAL_ERR;
}

int32_t AudioSDKAencOpen(td_handle *aencHandle, uapi_aenc_attr aencAttr)
{
    int32_t ret;
    ret = WaitCreateAnec();
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "wait create anec time out!");
        return ret;
    }
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_aenc_create(aencHandle));
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "uapi_aenc_create failed");
        g_isCanCreateAenc = true;
        return ret;
    }

    MEDIA_HAL_LOGI(MODULE_NAME, "codec_id: 0x%x", aencAttr.codec_id);
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_aenc_set_attr(*aencHandle, &aencAttr));
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "uapi_aenc_set_attr failed: 0x%x, codec id: 0x%x", ret, aencAttr.codec_id);
        goto CLOSE;
    }
#ifndef MELODY_AUDIO_SUPPORT
    if (aencAttr.codec_id == UAPI_ACODEC_ID_OPUS) {
        uint32_t bitRate = OPUS_ENC_BIT_RATE;
        ret = uapi_aenc_set_param(*aencHandle, UAPI_AENC_PARAM_BIT_RATE, &bitRate, sizeof(uint32_t));
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "uapi set aenc send cmd bit Rate faild, ret: 0x%x", ret);
            goto CLOSE;
        }
    }
#endif
    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;

CLOSE:
    MEDIA_HAL_DOFUNC_TIME((void)uapi_aenc_destroy(*aencHandle));
    g_isCanCreateAenc = true;
    return ret;
}

int32_t AudioSDKAencClose(td_handle aencHandle)
{
    int32_t ret;
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_aenc_destroy(aencHandle));
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_aenc_destroy failed");
    MediaHalMutexLock(g_audioSdkInitLock);
    g_isCanCreateAenc = true;
    MediaHalMutexUnLock(g_audioSdkInitLock);
    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

int32_t AudioSDKAttach(AudioSDKModuleType modType, td_handle srcHandle, td_handle dstHandle)
{
    int32_t ret;
    switch (modType) {
        case AUDIO_MOD_AI:
            MEDIA_HAL_DOFUNC_TIME(ret = uapi_ai_attach_output(srcHandle, dstHandle));
            CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_ai_attach_output");
            break;
        case AUDIO_MOD_ADP:
            MEDIA_HAL_DOFUNC_TIME(ret = uapi_adp_attach_output(srcHandle, dstHandle));
            CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_adp_attach_output failed");
            break;
        case AUDIO_MOD_ADEC:
            MEDIA_HAL_DOFUNC_TIME(ret = uapi_adec_attach_output(srcHandle, dstHandle));
            CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_adec_attach_output failed");
            break;
        case AUDIO_MOD_AENC:
            MEDIA_HAL_DOFUNC_TIME(ret = uapi_aenc_attach_output(srcHandle, dstHandle));
            CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_aenc_attach_output failed");
            break;
        case AUDIO_MOD_SEA:
            MEDIA_HAL_DOFUNC_TIME(ret = uapi_sea_attach_output(srcHandle, UAPI_SEA_OUTPUT_ASR_SRC, dstHandle));
            CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_sea_attach_output failed");
            break;
        case AUDIO_MOD_SOUND:
            MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_attach_output(srcHandle, UAPI_SND_OUT_PORT_CAST0, dstHandle));
            CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_snd_attach_output failed");
            break;
        default:
            MEDIA_HAL_LOGW(MODULE_NAME, "invalid module type %d", modType);
            return MEDIA_HAL_INVALID_PARAM;
    }

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

int32_t AudioSDKDetach(AudioSDKModuleType modType, td_handle srcHandle, td_handle dstHandle)
{
    int32_t ret;
    switch (modType) {
        case AUDIO_MOD_AI:
            MEDIA_HAL_DOFUNC_TIME(ret = uapi_ai_detach_output(srcHandle, dstHandle));
            CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_ai_detach_output failed");
            break;
        case AUDIO_MOD_ADP:
            MEDIA_HAL_DOFUNC_TIME(ret = uapi_adp_detach_output(srcHandle, dstHandle));
            CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_adp_detach_output failed");
            break;
        case AUDIO_MOD_ADEC:
            MEDIA_HAL_DOFUNC_TIME(ret = uapi_adec_detach_output(srcHandle, dstHandle));
            CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_adec_detach_output failed");
            break;
        case AUDIO_MOD_AENC:
            MEDIA_HAL_DOFUNC_TIME(ret = uapi_aenc_detach_output(srcHandle, dstHandle));
            CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_aenc_detach_output failed");
            break;
        case AUDIO_MOD_SEA:
            MEDIA_HAL_DOFUNC_TIME(ret = uapi_sea_detach_output(srcHandle, UAPI_SEA_OUTPUT_ASR_SRC, dstHandle));
            CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_sea_detach_output failed");
            break;
        case AUDIO_MOD_SOUND:
            MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_detach_output(srcHandle, UAPI_SND_OUT_PORT_CAST0, dstHandle));
            CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_snd_detach_output failed");
            break;
        default:
            MEDIA_HAL_LOGW(MODULE_NAME, "invalid module type %d", modType);
            return MEDIA_HAL_INVALID_PARAM;
    }

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

#endif

bool AudioPluginIsValidPin(enum AudioPortPin pin, const char *pluginName)
{
    if (strcmp("primary", pluginName) == 0) {
        if (pin == PIN_IN_MIC || pin == PIN_OUT_SPEAKER) {
            return true;
        }
    } else if (strcmp("bluetooth", pluginName) == 0) {
        if (pin == PIN_IN_BT_SCO || pin == PIN_OUT_BT_SCO ||
            pin == PIN_OUT_BT_A2DP || pin == PIN_IN_BT_A2DP) {
            return true;
        }
    } else if (strcmp("modem", pluginName) == 0) {
        if (pin == PIN_IN_MODEM || pin == PIN_OUT_MODEM ||
            pin == PIN_IN_MODEM_HEADSET || pin == PIN_OUT_MODEM_HEADSET) {
            return true;
        }
    }

    return false;
}

int32_t ParseKeyValueForExtra(const struct InvokeAttr *invokeAttr,
    char *key, int32_t keyLen, char *value, int32_t valueLen)
{
    MEDIA_HAL_UNUSED(keyLen);
    MEDIA_HAL_UNUSED(valueLen);
    int32_t ret = sscanf_s((const char *)invokeAttr->request, "%s = %s", key, MAX_KEY_STR_SIZE,
        value, MAX_KEY_VALUE_SIZE);
    if (ret == MEDIA_HAL_ERR) {
        MEDIA_HAL_LOGE(MODULE_NAME, "sscanf_s failed, ret = %d", ret);
        return ret;
    }
    if (strlen(value) == 0) {
        MEDIA_HAL_LOGI(MODULE_NAME, "value invalid");
        return MEDIA_HAL_INVALID_PARAM;
    }
    return MEDIA_HAL_OK;
}

int32_t IsNeedSetExtraFlag(enum AudioFormat audioFormat, bool *IsSetExtraParam)
{
#ifdef AUDIO_SDK_SUPPORT
    uapi_acodec_id adecCodecId;
    if (!AudioConvertAudioFormatToCodecId(audioFormat, &adecCodecId)) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioConvertAudioFormatToCodecId failed");
        return MEDIA_HAL_INVALID_PARAM;
    }
    *IsSetExtraParam = (adecCodecId == UAPI_ACODEC_ID_FLAC) ? true : false;
#endif
    return MEDIA_HAL_OK;
}

int32_t SetExtraData(struct PortPluginAttr *inputAttr, const char *value, uint32_t valueLen)
{
#ifdef AUDIO_SDK_SUPPORT
    int32_t ret = 0;
    if (valueLen <= 1) {
        MEDIA_HAL_LOGI(MODULE_NAME, "valueLen invalid ExtraData not need set");
        return MEDIA_HAL_OK;
    }
    if (inputAttr->extraData != NULL) {
        free(inputAttr->extraData);
        inputAttr->extraData = NULL;
        inputAttr->extraDataSize = 0;
    }
    inputAttr->extraData = malloc(valueLen);
    if (inputAttr->extraData == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, " malloc failed");
        return MEDIA_HAL_NO_MEM;
    }
    (void)memset_s(inputAttr->extraData, valueLen, 0, valueLen);
    int32_t tempPoint = 0;
    char temp[3] = {0}; /* 3 byte */
    char *endPtr = NULL;
    unsigned long strNum;
    uint32_t outdataNum = 0;
    for (uint32_t i = 0; i < strlen(value) / 2; i++) { /* 2 char --> 1 byte(hexadecimal) */
        temp[0] = *(value + tempPoint);
        temp[1] = *(value + tempPoint + 1);
        strNum = strtoul(temp, &endPtr, 16); /* 16: hexadecimal */
        ret = memcpy_s(inputAttr->extraData + outdataNum,
            valueLen, &strNum, sizeof(unsigned char));
        if (ret != EOK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "memcpy_s failed");
            return MEDIA_HAL_ERR;
        }
        outdataNum += (uint32_t)sizeof(unsigned char);
        tempPoint += 2; /* 2 char --> 1 byte(hexadecimal) */
    }
    inputAttr->extraDataSize = outdataNum;
#endif
    return MEDIA_HAL_OK;
}

#ifdef AUDIO_SDK_SUPPORT
int32_t SaveToggleConfig(uapi_audio_toggle_config *toggleConfig, const char *value, uint32_t valueLen)
{
    MEDIA_HAL_UNUSED(valueLen);
    CHK_NULL_RETURN(toggleConfig, MEDIA_HAL_INVALID_PARAM, "input param toggleConfig is null");
    CHK_NULL_RETURN(value, MEDIA_HAL_INVALID_PARAM, "input param value is null");
    uint32_t toggleCount = 0;
    int32_t toggleValid = 0;
    int32_t ret = sscanf_s(value, "%d:%u", &toggleValid, &toggleCount);
    if (ret == -1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "save toggle config sscanf_s failed!");
        return MEDIA_HAL_ERR;
    }
    toggleConfig->toggle_valid = false;
    if (toggleValid != 0) {
        toggleConfig->toggle_valid = true;
    }
    toggleConfig->toggle_count = toggleCount;
    return MEDIA_HAL_OK;
}

int32_t UpdateCurrentPosition(td_handle trackHandle, struct AudioTimeStamp *time)
{
    int32_t ret;
    int64_t pts = 0;
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_get_track_play_pts(trackHandle, &pts));
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "get sound track pts failed");
    int64_t sec = 0;
    int64_t nsec = 0;
    ret = MediaHalConvertUSecToSecAndNSec(pts, &sec, &nsec);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "convert time from us to sec and nsec failed");
        return MEDIA_HAL_ERR;
    }

    time->tvSec = sec;
    time->tvNSec = nsec;

    return MEDIA_HAL_OK;
}

int32_t GetParamByValue(uint8_t *param, uint32_t paramLen, const char *value,
    uint32_t valueLen, uint32_t configLen)
{
    uint32_t prefixlen = 0;
    for (uint32_t i = 0; i < valueLen; i++) {
        if (value[i] == ':') {
            prefixlen = i + 1;
            break;
        }
    }
    if (prefixlen == 0) {
        MEDIA_HAL_LOGE(MODULE_NAME, "prefix is not found.");
        return MEDIA_HAL_ERR;
    }
    if (memcpy_s(param, paramLen, value + prefixlen, configLen) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "set spk cali coef memcpy_s failed!");
        return MEDIA_HAL_ERR;
    }
    return MEDIA_HAL_OK;
}

int32_t SetSeaEnable(td_handle seaHandle, const char *value, uint32_t valueLen)
{
    MEDIA_HAL_UNUSED(valueLen);
    int32_t seaStatus = 0;
    int32_t ret = sscanf_s(value, "%d", &seaStatus);
    if (ret == -1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "set sea enable sscanf_s failed!");
        return MEDIA_HAL_ERR;
    }
    uapi_sea_afe_attr afeAttr = {
        .type = UAPI_SEA_AFE_SEE,
        .mode = UAPI_SEA_NORM_MODE,
        .u.see.options = UAPI_SEA_AFE_OPT_AGC | UAPI_SEA_AFE_OPT_AEC |
            UAPI_SEA_AFE_OPT_WNR | UAPI_SEA_AFE_OPT_ANR,
    };
    if (seaStatus == 0) {
        afeAttr.u.see.options = 0;
    }
    ret = uapi_sea_set_afe_attr(seaHandle, UAPI_SEA_AFE_SEE, &afeAttr);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "uapi set sea afe attr faild, ret: 0x%x", ret);
        return MEDIA_HAL_ERR;
    }
    return MEDIA_HAL_OK;
}
#endif

int32_t ExtraParamsInvoke(AudioHandle portHandle, struct InvokeAttr *invokeAttr,
    struct AudioPortInvoke invokeList[], uint32_t listSize)
{
    CHK_NULL_RETURN(portHandle, MEDIA_HAL_INVALID_PARAM, "Input param handle is null");
    CHK_NULL_RETURN(invokeAttr, MEDIA_HAL_INVALID_PARAM, "Input param invokeAttr is null");
    char keyFlag[MAX_KEY_STR_SIZE] = { 0 };
    char value[MAX_KEY_VALUE_SIZE] = { 0 };
    int32_t ret = ParseKeyValueForExtra(invokeAttr, keyFlag, MAX_KEY_STR_SIZE, value, MAX_KEY_VALUE_SIZE);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "ParseKeyValueForExtra failed");
    AudioPortInvokeFunc invokeFunc = NULL;
    for (size_t i = 0; i < listSize; i++) {
        if (strcmp(invokeList[i].invokeName, keyFlag) == 0) {
            invokeFunc = invokeList[i].invokefunc;
            if (invokeList[i].isNeedParseKeyValue) {
                return invokeFunc(portHandle, value, (uint32_t)(strlen(value) + 1));
            }
            return invokeFunc(portHandle, invokeAttr->request, (uint32_t)(strlen(invokeAttr->request) + 1));
        }
    }
    if (invokeFunc == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "not found invoke function!");
        return MEDIA_HAL_INVALID_PARAM;
    }
    return MEDIA_HAL_OK;
}


int32_t GetBuiltinAIAttr(uapi_ai_attr *aiAttr)
{
    aiAttr->port_attr.adc.rx_type = BUILTIN_AI_PORT_ATTR_ADC_RX_TYPE;
    aiAttr->pcm_attr.channels = BUILTIN_AI_PCM_ATTR_AUDIO_CH;
    aiAttr->pcm_attr.bit_depth = BUILTIN_AI_PCM_ATTR_BIT_DEPTH;
    aiAttr->pcm_attr.sample_rate = BUILTIN_AI_PCM_ATTR_SAMPLE_RATE;
    aiAttr->pcm_attr.sample_per_frame = aiAttr->pcm_attr.sample_rate / BUILTIN_AI_PCM_ATTR_FRAME_PER_SEC;
    return MEDIA_HAL_OK;
}

int32_t GetBuiltinAOAttr(uapi_snd_attr *sndAttr)
{
    sndAttr->port_num = BUILTIN_SND_OUT_PORT_NUM;
    sndAttr->port_attr[0].out_port = BUILTIN_SND_OUT_PORT;
    sndAttr->channels = BUILTIN_SND_OUT_AUDIO_CH;
    sndAttr->bit_depth = BUILTIN_SND_OUT_BIT_DEPTH;
    sndAttr->sample_rate = BUILTIN_SND_OUT_SAMPLE_RATE;
    sndAttr->port_attr[0].u.i2s_attr.attr.bclk = BUILTIN_SND_OUT_I2S_ATTR_I2S_BCLK;
    sndAttr->port_attr[0].u.i2s_attr.attr.bit_depth = BUILTIN_SND_OUT_I2S_ATTR_BIT_DEPTH;
    sndAttr->port_attr[0].u.i2s_attr.attr.channels = BUILTIN_SND_OUT_I2S_ATTR_AUDIO_CH;
    sndAttr->port_attr[0].u.i2s_attr.attr.i2s_mode = BUILTIN_SND_OUT_I2S_ATTR_I2S_STD_MODE;
    sndAttr->port_attr[0].u.i2s_attr.attr.master = BUILTIN_SND_OUT_I2S_ATTR_MASTER;
    sndAttr->port_attr[0].u.i2s_attr.attr.mclk = BUILTIN_SND_OUT_I2S_ATTR_I2S_MCLK;
    sndAttr->port_attr[0].u.i2s_attr.attr.pcm_delay_cycle = BUILTIN_SND_OUT_I2S_ATTR_I2S_PCM_DELAY;
    sndAttr->port_attr[0].u.i2s_attr.attr.pcm_sample_rise_edge = BUILTIN_SND_OUT_I2S_ATTR_SAMPLE_RISE_EDGE;
    return MEDIA_HAL_OK;
}

int32_t GetExternAIAttr(uapi_ai_attr *aiAttr)
{
    aiAttr->port_attr.i2s.i2s_attr.master = EXTERNAL_AI_I2S_ATTR_MASTER;
    aiAttr->port_attr.i2s.i2s_attr.i2s_mode = EXTERNAL_AI_I2S_ATTR_I2S_MODE;
    aiAttr->port_attr.i2s.i2s_attr.mclk = EXTERNAL_AI_I2S_ATTR_MCLK;
    aiAttr->port_attr.i2s.i2s_attr.bclk = EXTERNAL_AI_I2S_ATTR_BCLK;
    aiAttr->port_attr.i2s.i2s_attr.bit_depth = EXTERNAL_AI_I2S_ATTR_BIT_DEPTH;
    aiAttr->port_attr.i2s.i2s_attr.channels = EXTERNAL_AI_I2S_ATTR_AUDIO_CH;
    aiAttr->port_attr.i2s.i2s_attr.pcm_sample_rise_edge = EXTERNAL_AI_I2S_ATTR_SAMPLE_RISE_EDGE;
    aiAttr->port_attr.i2s.i2s_attr.pcm_delay_cycle = EXTERNAL_AI_I2S_ATTR_PCM_DELAY_CYCLE;
    aiAttr->pcm_attr.channels = EXTERNAL_AI_PCM_ATTR_CHANNELS;
    aiAttr->pcm_attr.bit_depth = EXTERNAL_AI_PCM_ATTR_BIT_DEPTH;
    aiAttr->pcm_attr.sample_rate = EXTERNAL_AI_PCM_ATTR_SAMPLE_RATE;
    aiAttr->pcm_attr.sample_per_frame = aiAttr->pcm_attr.sample_rate / EXTERNAL_AI_PCM_ATTR_FRAME_PER_SEC;
    aiAttr->ref_attr.enable = TD_FALSE;
    return MEDIA_HAL_OK;
}

int32_t GetExternAOAttr(uapi_snd_attr *sndAttr)
{
    sndAttr->port_num = EXTERNAL_SND_OUT_PORT_NUM;
    sndAttr->port_attr[0].out_port = EXTERNAL_SND_OUT_PORT;
    sndAttr->port_attr[0].u.i2s_attr.attr.bit_depth = EXTERNAL_SND_OUT_I2S_ATTR_BIT_DEPTH;
    sndAttr->port_attr[0].u.i2s_attr.attr.channels = EXTERNAL_SND_OUT_I2S_ATTR_CHANNEL;
    sndAttr->port_attr[0].u.i2s_attr.attr.i2s_mode = EXTERNAL_SND_OUT_I2S_ATTR_I2S_MODE;
    sndAttr->port_attr[0].u.i2s_attr.attr.mclk = EXTERNAL_SND_OUT_I2S_ATTR_MCLK;
    sndAttr->port_attr[0].u.i2s_attr.attr.master = EXTERNAL_SND_OUT_I2S_ATTR_MASTER;
    sndAttr->port_attr[0].u.i2s_attr.attr.pcm_sample_rise_edge = EXTERNAL_SND_OUT_I2S_ATTR_SAMPLE_RISE_EDGE;
    sndAttr->port_attr[0].u.i2s_attr.attr.bclk = EXTERNAL_SND_OUT_I2S_ATTR_BCLK;
    sndAttr->channels = EXTERNAL_SND_OUT_AUDIO_CHANNEL;
    sndAttr->bit_depth = EXTERNAL_SND_OUT_AUDIO_BIT_DEPTH;
    sndAttr->sample_rate = EXTERNAL_SND_OUT_AUDIO_SAMPLE_RATE;
    return MEDIA_HAL_OK;
}

const AudioPortPlugin *GetPortPluginFuncs(AudioPluginType pluginType)
{
    if (pluginType == AUDIO_PLUGIN_PRIMARY) {
        return GetPrimaryPortPluginFuncs(pluginType);
    } else if (pluginType == AUDIO_PLUGIN_BLUETOOTH) {
        return GetBluetoothPortPluginFuncs(pluginType);
    } else if (pluginType == AUDIO_PLUGIN_MODEM) {
#ifdef SUPPORT_MODEM
        return GetModemPortPluginFuncs(pluginType);
#else
        return NULL;
#endif
    }

    MEDIA_HAL_LOGE(MODULE_NAME, "invalid plugin type %d", pluginType);
    return NULL;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

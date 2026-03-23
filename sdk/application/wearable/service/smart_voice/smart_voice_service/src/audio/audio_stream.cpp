/*
 * Copyright (c) CompanyNameMagicTag 2021. All rights reserved.
 * Description: audio stream api.
 */

#include "audio_stream.h"
#include "audio_manager.h"
#include "audio_base_type.h"
#include "audio_stream_in.h"
#include "audio_errors.h"
#include "wearable_log.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef UNUSED
#define UNUSED(x) ((void)(x))
#endif

using Audio::AudioInterrupt;
using Audio::AudioManager;
using Audio::AudioStreamIn;
using Audio::InterruptListener;

static constexpr td_s32 AUDIO_CHANNEL_1 = 1;
static constexpr td_s32 AUDIO_SAMPLE_RATE_16K = 16000;
static constexpr td_s32 AUDIO_STREAM_MAX_INSTANCE_NUM = 2;
static constexpr td_s32 INVALID_AUDIO_VALUE = (-1);

static AudioManager &g_amInstance = AudioManager::GetInstance();
static std::shared_ptr<AudioStreamIn> g_streamIn;
static AudioSession g_sessionId;
static AudioInterrupt g_interrupt;

class AudioStreamInterruptListener : public InterruptListener {
public:
    AudioStreamInterruptListener(ActInterruptCb callback)
    {
        cb = callback;
    };

    ~AudioStreamInterruptListener() {};

    ActInterruptCb cb = NULL;

    void OnInterrupt(td_s32 type, td_s32 hint) override
    {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE, "[AudioStreamInterruptListener] OnInterrupt: type = %d, hint = %d", type, hint);
        if (cb) {
            cb(type, hint);
        }
    }
};

td_s32 AudioStreamInit(td_void)
{
    td_bool ret = g_amInstance.Initialize();
    if (!ret) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "[%s] line:%d, audio manager init failed", __FUNCTION__, __LINE__);
        return AUDIO_ERROR;
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE, "[%s] line:%d, success", __FUNCTION__, __LINE__);
    return AUDIO_SUCCESS;
}

td_s32 AudioManagerActInterrupt(ActInterruptCb cb)
{
    g_sessionId = g_amInstance.MakeSessionId();
    if (g_sessionId == AUDIO_SESSION_ID_NONE) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "[%s] line:%d, make session id failed", __FUNCTION__, __LINE__);
        return AUDIO_ERROR;
    }

    WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE, "[%s] line:%d, session id = %d", __FUNCTION__, __LINE__, g_sessionId);

    std::shared_ptr<AudioStreamInterruptListener> AudioIntListener = std::make_shared<AudioStreamInterruptListener>(cb);
    g_interrupt = { AUDIO_STREAM_VOICE_RECOGNITION, g_sessionId, AudioIntListener };
    if (g_amInstance.ActivateAudioInterrupt(g_interrupt) == INTERRUPT_FAILED) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "[%s] line:%d, ActivateAudioInterrupt failed", __FUNCTION__, __LINE__);
        return AUDIO_ERROR;
    }

    WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE, "[%s] line:%d, success", __FUNCTION__, __LINE__);
    return AUDIO_SUCCESS;
}

td_s32 AudioStreamCreate(td_void)
{
    g_streamIn = std::make_shared<AudioStreamIn>();
    if (g_streamIn == nullptr || g_streamIn.get() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "[%s] line:%d make_shared AudioStreamIn failed", __FUNCTION__, __LINE__);
        return AUDIO_ERROR;
    }

    CapturerInputConfig captureConfig = {
        .audioFormat = PCM,
        .streamType = AUDIO_STREAM_VOICE_RECOGNITION,
        .sampleRate = AUDIO_SAMPLE_RATE_16K,
        .sessionID = g_sessionId,
        .channelCount = AUDIO_CHANNEL_1,
        .linkDir = AUDIO_LINK_NONE,
    };
    td_s32 ret = g_streamIn->Init(captureConfig);
    if (ret != AUDIO_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "[%s] line:%d streamIn init failed, ret: 0x%x", __FUNCTION__, __LINE__, ret);
        return AUDIO_ERROR;
    }

    WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE, "[%s] line:%d success", __FUNCTION__, __LINE__);
    return AUDIO_SUCCESS;
}


td_s32 AudioStreamStart(td_void)
{
    td_s32 ret = g_streamIn->Start();
    if (ret != AUDIO_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "[%s] line:%d streamIn Start failed ret: 0x%x", __FUNCTION__, __LINE__, ret);
        return AUDIO_ERROR;
    }

    WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE, "[%s] line:%d success", __FUNCTION__, __LINE__);
    return AUDIO_SUCCESS;
}

td_s32 AudioStreamGetChannelId(td_u32 *channelId)
{
    td_u32 chanId;
    td_s32 ret = g_streamIn->GetCurrentChannelId(chanId);
    if (ret != AUDIO_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "[%s] line:%d streamIn get Id failed ret: 0x%x", __FUNCTION__, __LINE__, ret);
        return AUDIO_ERROR;
    }

    WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE, "[%s] line:%d stream in channelId: 0x%x", __FUNCTION__, __LINE__, chanId);
    *channelId = chanId;
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE, "[%s] line:%d success", __FUNCTION__, __LINE__);
    return AUDIO_SUCCESS;
}

td_s32 AudioStreamStop(td_void)
{
    td_s32 ret = g_streamIn->Stop();
    if (ret != AUDIO_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "[%s] line:%d streamIn Stop failed ret: 0x%x", __FUNCTION__, __LINE__, ret);
    }

    WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE, "[%s] line:%d success", __FUNCTION__, __LINE__);
    return AUDIO_SUCCESS;
}

td_s32 AudioStreamDestroy(td_void)
{
    if (g_streamIn != nullptr) {
        g_streamIn->Release();
        g_streamIn = nullptr;
    }

    WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE, "[%s] line:%d success", __FUNCTION__, __LINE__);
    return AUDIO_SUCCESS;
}

td_s32 AudioStreamDeactInterrupt(td_void)
{
    td_s32 ret = g_amInstance.DeactivateAudioInterrupt(g_interrupt);
    if (ret != AUDIO_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "[%s] line:%d DeactivateAudioInterrupt failed ret = %d", __FUNCTION__, __LINE__,
            ret);
        return AUDIO_ERROR;
    }

    WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE, "[%s] line:%d success", __FUNCTION__, __LINE__);
    return AUDIO_SUCCESS;
}

#ifdef __cplusplus
}
#endif

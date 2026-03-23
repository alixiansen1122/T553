/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: PhoneAudio.cpp
 * Create: 2021-09-22
 */

#include "phoneservice/PhoneAudio.h"
#include "phoneservice/PhoneService.h"
#include <iostream>
#include <memory>
#include "audio_manager.h"
#include "audio_base_type.h"
#include "audio_stream_in.h"
#include "audio_stream_out.h"
#include "audio_errors.h"
#include "wearable_log.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef UNUSED
#define UNUSED(x) ((void)(x))
#endif

using Audio::AudioManager;
using Audio::AudioStreamIn;
using Audio::AudioStreamOut;
using Audio::InterruptListener;
using Audio::AudioInterrupt;

static constexpr int32 AUDIO_CHANNEL_1 = 1;
static constexpr int32 AUDIO_SAMPLE_RATE_16K = 16000;
static constexpr int32 AUDIO_SAMPLE_RATE_8K = 8000;
static constexpr int32 AUDIO_STREAM_MAX_INSTANCE_NUM = 2;
static constexpr int32 INVALID_AUDIO_VALUE = (-1);

class PhoneAudioInterruptListener : public InterruptListener {
public:
    PhoneAudioInterruptListener(){};
    ~PhoneAudioInterruptListener(){};

    void OnInterrupt(int32 type, int32 hint) override
    {
        UNUSED(type);
        UNUSED(hint);
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[PhoneAudioInterruptListener] OnInterrupt: type = %d, hint = %d", type, hint);
    }
};

static AudioManager& g_amInstance = AudioManager::GetInstance();
static std::shared_ptr<AudioStreamIn> g_streamIn[AUDIO_STREAM_MAX_INSTANCE_NUM];
static std::shared_ptr<AudioStreamOut> g_streamOut[AUDIO_STREAM_MAX_INSTANCE_NUM];
std::shared_ptr<PhoneAudioInterruptListener> g_audioIntListener = nullptr;
static AudioSession g_sessionId;
static AudioInterrupt g_interrupt;

int32 PhoneAudioGetMinVolume(void)
{
    return g_amInstance.GetMinVolume(AUDIO_STREAM_VOICE_CALL_BT_SCO);
}

int32 PhoneAudioGetMaxVolume(void)
{
    return g_amInstance.GetMaxVolume(AUDIO_STREAM_VOICE_CALL_BT_SCO);
}

int32 PhoneAudioGetVolume(void)
{
    return g_amInstance.GetVolume(AUDIO_STREAM_VOICE_CALL_BT_SCO);
}

bool PhoneAudioSetVolume(int32 volume)
{
    return g_amInstance.SetVolume(AUDIO_STREAM_VOICE_CALL_BT_SCO, volume);
}

// 设置通话speaker静音/取消静音
bool PhoneAudioSetSpeakerMute(bool isMute)
{
    bool ret;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[PhoneAudioSetSpeakerMute] start");
    ret = g_amInstance.IsMute(AUDIO_STREAM_VOICE_CALL_BT_SCO);
    if (ret == isMute) {
        return true;
    }

    if (isMute) {
        ret = g_amInstance.Mute(AUDIO_STREAM_VOICE_CALL_BT_SCO);
    } else {
        ret = g_amInstance.UnMute(AUDIO_STREAM_VOICE_CALL_BT_SCO);
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[PhoneAudioSetSpeakerMute] end! ret = %d", ret);
    return ret;
}

// 设置通话mic静音/取消静音
bool PhoneAudioSetMicMute(bool isMute)
{
    bool ret;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[PhoneAudioSetMicMute] start");
    ret = g_amInstance.IsMicrophoneMute();
    if (ret == isMute) {
        return true;
    }
    ret = g_amInstance.SetMicrophoneMute(isMute);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[PhoneAudioSetMicMute] end! ret = %d", ret);
    return ret;
}

static bool CheckParam(int32 instanceNum)
{
    if (instanceNum == 0 || instanceNum == 1) {
        return true;
    }
    return false;
}

int32 PhoneAudioManagerInit(void)
{
    /* audio manager init */
    bool ret = g_amInstance.Initialize(); // load audio adapter
    if (!ret) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[%s] line:%d, audio manager init failed", __FUNCTION__, __LINE__);
        return AUDIO_ERROR;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[%s] line:%d, success", __FUNCTION__, __LINE__);
    return AUDIO_SUCCESS;
}

int32 PhoneAudioManagerActInterrupt(AudioStreamType streamType)
{
    /* request audio focus */
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[%s] streamType:0x%x", __FUNCTION__, streamType);

    g_sessionId = g_amInstance.MakeSessionId();
    if (g_sessionId == AUDIO_SESSION_ID_NONE) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[%s] line:%d, make session id failed", __FUNCTION__, __LINE__);
        return AUDIO_ERROR;
    }

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[%s] line:%d, session id = %d", __FUNCTION__, __LINE__, g_sessionId);

    g_audioIntListener = std::make_shared<PhoneAudioInterruptListener>();
    if (g_audioIntListener.get() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[%s] line:%d, g_audioIntListener is null", __FUNCTION__, __LINE__);
        return AUDIO_ERROR;
    }
    g_interrupt = { streamType, g_sessionId, g_audioIntListener };
    if (g_amInstance.ActivateAudioInterrupt(g_interrupt) == INTERRUPT_FAILED) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[%s] line:%d, ActivateAudioInterrupt failed", __FUNCTION__, __LINE__);
        return AUDIO_ERROR;
    }

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[%s] line:%d, success", __FUNCTION__, __LINE__);
    return AUDIO_SUCCESS;
}

int32 PhoneAudioManagerSetDeviceConnState(AudioDeviceType deviceType, AudioDeviceConnectState connectState)
{
    int32 ret = g_amInstance.SetDeviceConnectionState(deviceType, connectState);
    if (ret != AUDIO_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[%s] line:%d failed, deviceType = 0x%x connectState = %d ret = %d",
            __FUNCTION__, __LINE__, deviceType, connectState, ret);
        return AUDIO_ERROR;
    }

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[%s] line:%d success, deviceType = 0x%x connectState = %d",
                  __FUNCTION__, __LINE__, deviceType, connectState);
    return AUDIO_SUCCESS;
}

int32 PhoneAudioStreamInInit(AudioLinkDirection linkDir)
{
    int32 instanceNum = INVALID_AUDIO_VALUE;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[%s] linkDir:0x%x", __FUNCTION__, linkDir);

    /* create ASI instance UP_LINK: index 0,  DOWN_LINK: index 1 */
    if (linkDir == AUDIO_UP_LINK) {
        g_streamIn[0] = std::make_shared<AudioStreamIn>();
        instanceNum = 0;
    } else if (linkDir == AUDIO_DOWN_LINK) {
        g_streamIn[1] = std::make_shared<AudioStreamIn>();
        instanceNum = 1;
    } else {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[%s] line:%d failed, invalid linkDir = %d", __FUNCTION__, __LINE__, linkDir);
        return AUDIO_ERROR;
    }

    if (g_streamIn[instanceNum] == nullptr || g_streamIn[instanceNum].get() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[%s] line:%d make_shared AudioStreamIn failed", __FUNCTION__, __LINE__);
        return AUDIO_ERROR;
    }

    /* init ASI */
    CapturerInputConfig captureConfig = {
        .streamType = AUDIO_STREAM_VOICE_CALL_BT_SCO,
        .sessionID = g_sessionId,
        .channelCount = AUDIO_CHANNEL_1,
        .linkDir = linkDir,
    };

    captureConfig.audioFormat = (AudioCodecFormat)GetCallCodec();
    if (captureConfig.audioFormat == PCM) {
        captureConfig.sampleRate = AUDIO_SAMPLE_RATE_8K;
    } else if (captureConfig.audioFormat == mSBC) {
        captureConfig.sampleRate = AUDIO_SAMPLE_RATE_16K;
    }

    int32 ret = g_streamIn[instanceNum]->Init(captureConfig);
    if (ret != AUDIO_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[%s] line:%d streamIn init failed, ret: 0x%x", __FUNCTION__, __LINE__, ret);
        return AUDIO_ERROR;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[%s] line:%d success, instanceNum = %d", __FUNCTION__, __LINE__, instanceNum);
    return instanceNum;
}

int32 PhoneAudioStreamOutInit(AudioLinkDirection linkDir)
{
    int32 instanceNum = INVALID_AUDIO_VALUE;

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[%s] linkDir:0x%x", __FUNCTION__, linkDir);

    /* create ASO instance UP_LINK: index 0,  DOWN_LINK: index 1 */
    if (linkDir == AUDIO_UP_LINK) {
        g_streamOut[0] = std::make_shared<AudioStreamOut>();
        instanceNum = 0;
    } else if (linkDir == AUDIO_DOWN_LINK) {
        g_streamOut[1] = std::make_shared<AudioStreamOut>();
        instanceNum = 1;
    } else {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[%s] line:%d failed, invalid linkDir = %d", __FUNCTION__, __LINE__, linkDir);
        return AUDIO_ERROR;
    }

    if (g_streamOut[instanceNum] == nullptr || g_streamOut[instanceNum].get() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[%s] line:%d make_shared AudioStreamOut failed", __FUNCTION__, __LINE__);
        return AUDIO_ERROR;
    }

    /* init ASO */
    AudioRendererConfig renderConfig = {
        .streamType = AUDIO_STREAM_VOICE_CALL_BT_SCO,
        .sessionID = g_sessionId,
        .channelCount = AUDIO_CHANNEL_1,
        .linkDir = linkDir,
    };

    renderConfig.audioFormat = (AudioCodecFormat)GetCallCodec();
    if (renderConfig.audioFormat == PCM) {
        renderConfig.sampleRate = AUDIO_SAMPLE_RATE_8K;
    } else if (renderConfig.audioFormat == mSBC) {
        renderConfig.sampleRate = AUDIO_SAMPLE_RATE_16K;
    }

    int32 ret = g_streamOut[instanceNum]->Init(renderConfig);
    if (ret != AUDIO_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[%s] line:%d streamOut init failed, ret: 0x%x", __FUNCTION__, __LINE__, ret);
        return AUDIO_ERROR;
    }
    /* dump stream */
    g_streamOut[instanceNum]->DumpInfo();

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[%s] line:%d success, instanceNum = %d", __FUNCTION__, __LINE__, instanceNum);
    return instanceNum;
}

int32 PhoneAudioStreamInStart(int32 instanceNum)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[%s] instanceNum:0x%x", __FUNCTION__, instanceNum);
    bool isValid = CheckParam(instanceNum);
    if (!isValid) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[%s] line:%d instanceNum %d is invalid", __FUNCTION__, __LINE__, instanceNum);
        return AUDIO_ERROR;
    }

    /* start ASI */
    int32 ret = g_streamIn[instanceNum]->Start();
    if (ret != AUDIO_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[%s] line:%d streamIn Start failed ret: 0x%x", __FUNCTION__, __LINE__, ret);
        return AUDIO_ERROR;
    }

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[%s] line:%d success", __FUNCTION__, __LINE__);
    return AUDIO_SUCCESS;
}

int32 PhoneAudioStreamOutStart(int32 instanceNum)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[%s] instanceNum:0x%x", __FUNCTION__, instanceNum);
    bool isValid = CheckParam(instanceNum);
    if (!isValid) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[%s] line:%d instanceNum %d is invalid", __FUNCTION__, __LINE__, instanceNum);
        return AUDIO_ERROR;
    }

    /* start ASO */
    int32 ret = g_streamOut[instanceNum]->Play();
    if (ret != AUDIO_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[%s] line:%d streamOut Play failed ret: 0x%x", __FUNCTION__, __LINE__, ret);
        return AUDIO_ERROR;
    }

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[%s] line:%d success", __FUNCTION__, __LINE__);
    return AUDIO_SUCCESS;
}

int32 PhoneAudioStreamInGetChannelId(int32 instanceNum, uint32 *shmId)
{
    bool isValid = CheckParam(instanceNum);
    if (!isValid || shmId == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[%s] line:%d instanceNum %d is invalid", __FUNCTION__, __LINE__, instanceNum);
        return AUDIO_ERROR;
    }

    /* obtain ASI channel id(adp handle) */
    uint32 channelId;
    int32 ret = g_streamIn[instanceNum]->GetCurrentChannelId(channelId);
    if (ret != AUDIO_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[%s] line:%d streamIn get Id failed ret: 0x%x", __FUNCTION__, __LINE__, ret);
        return AUDIO_ERROR;
    }

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[%s] line:%d stream in channelId: 0x%x", __FUNCTION__, __LINE__, channelId);

    *shmId = channelId;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[%s] line:%d success", __FUNCTION__, __LINE__);
    return AUDIO_SUCCESS;
}

int32 PhoneAudioStreamOutGetChannelId(int32 instanceNum, uint32 *shmId)
{
    bool isValid = CheckParam(instanceNum);
    if (!isValid || shmId == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[%s] line:%d instanceNum %d is invalid", __FUNCTION__, __LINE__, instanceNum);
        return AUDIO_ERROR;
    }

    /* obtain ASO channel id(adp handle) */
    uint32 channelId;
    int32 ret = g_streamOut[instanceNum]->GetCurrentChannelId(channelId);
    if (ret != AUDIO_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[%s] line:%d streamOut get Id failed ret: 0x%x", __FUNCTION__, __LINE__, ret);
        return AUDIO_ERROR;
    }

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[%s] line:%d stream out channelId: 0x%x", __FUNCTION__, __LINE__, channelId);

    *shmId = channelId;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[%s] line:%d success", __FUNCTION__, __LINE__);
    return AUDIO_SUCCESS;
}

int32 PhoneAudioStreamInAttachBackend(int32 instanceNum, uint32 backendId)
{
    bool isValid = CheckParam(instanceNum);
    if (!isValid) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[%s] line:%d instanceNum %d is invalid", __FUNCTION__, __LINE__, instanceNum);
        return AUDIO_ERROR;
    }

    /* ASI set shm id */
    int32 ret = g_streamIn[instanceNum]->AttachBackend(backendId);
    if (ret != AUDIO_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[%s] line:%d streamIn AttachBackend failed backendId: 0x%x, ret: 0x%x",
            __FUNCTION__, __LINE__, backendId, ret);
        return AUDIO_ERROR;
    }

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[%s] line:%d, stream in instanceNum: %d, backendId: 0x%x",
        __FUNCTION__, __LINE__, instanceNum, backendId);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[%s] line:%d success", __FUNCTION__, __LINE__);
    return AUDIO_SUCCESS;
}

int32 PhoneAudioStreamOutAttachFrontend(int32 instanceNum, uint32 frontendId)
{
    bool isValid = CheckParam(instanceNum);
    if (!isValid) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[%s] line:%d instanceNum %d is invalid", __FUNCTION__, __LINE__, instanceNum);
        return AUDIO_ERROR;
    }

    /* ASO set shm id */
    int32 ret = g_streamOut[instanceNum]->AttachFrontend(frontendId);
    if (ret != AUDIO_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[%s] line:%d streamOut AttachFrontend failed frontendId: 0x%x, ret: 0x%x",
            __FUNCTION__, __LINE__, frontendId, ret);
        return AUDIO_ERROR;
    }

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[%s] line:%d, stream out instanceNum: %d, frontendId: 0x%x",
        __FUNCTION__, __LINE__, instanceNum, frontendId);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[%s] line:%d success", __FUNCTION__, __LINE__);
    return AUDIO_SUCCESS;
}

int32 PhoneAudioStreamInStop(int32 instanceNum)
{
    bool isValid = CheckParam(instanceNum);
    if (!isValid) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[%s] line:%d instanceNum %d is invalid", __FUNCTION__, __LINE__, instanceNum);
        return AUDIO_ERROR;
    }

    /* stop ASI */
    int32 ret = g_streamIn[instanceNum]->Stop();
    if (ret != AUDIO_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[%s] line:%d streamIn Stop failed ret: 0x%x", __FUNCTION__, __LINE__, ret);
    }

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[%s] line:%d success", __FUNCTION__, __LINE__);
    return AUDIO_SUCCESS;
}

int32 PhoneAudioStreamOutStop(int32 instanceNum)
{
    bool isValid = CheckParam(instanceNum);
    if (!isValid) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[%s] line:%d instanceNum %d is invalid", __FUNCTION__, __LINE__, instanceNum);
        return AUDIO_ERROR;
    }

    /* stop ASO */
    int32 ret = g_streamOut[instanceNum]->Stop();
    if (ret != AUDIO_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[%s] line:%d streamOut Stop failed ret: 0x%x", __FUNCTION__, __LINE__, ret);
    }

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[%s] line:%d success", __FUNCTION__, __LINE__);
    return AUDIO_SUCCESS;
}

int32 PhoneAudioStreamInDeinit(int32 instanceNum)
{
    bool isValid = CheckParam(instanceNum);
    if (!isValid) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[%s] line:%d instanceNum %d is invalid", __FUNCTION__, __LINE__, instanceNum);
        return AUDIO_ERROR;
    }

    /* deinit ASI */
    if (g_streamIn[instanceNum] != nullptr) {
        g_streamIn[instanceNum]->Release();
        g_streamIn[instanceNum] = nullptr;
    }

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[%s] line:%d success", __FUNCTION__, __LINE__);
    return AUDIO_SUCCESS;
}

int32 PhoneAudioStreamOutDeinit(int32 instanceNum)
{
    bool isValid = CheckParam(instanceNum);
    if (!isValid) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[%s] line:%d instanceNum %d is invalid", __FUNCTION__, __LINE__, instanceNum);
        return AUDIO_ERROR;
    }

    /* deinit ASO */
    if (g_streamOut[instanceNum] != nullptr) {
        g_streamOut[instanceNum]->Release();
        g_streamOut[instanceNum] = nullptr;
    }

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[%s] line:%d success", __FUNCTION__, __LINE__);
    return AUDIO_SUCCESS;
}

int32 PhoneAudioManagerDeactInterrupt(void)
{
    /* release audio focus */
    int32 ret = g_amInstance.DeactivateAudioInterrupt(g_interrupt);
    if (ret != AUDIO_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[%s] line:%d DeactivateAudioInterrupt failed ret = %d",
            __FUNCTION__, __LINE__, ret);
        return AUDIO_ERROR;
    }

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "[%s] line:%d success", __FUNCTION__, __LINE__);
    return AUDIO_SUCCESS;
}

#ifdef __cplusplus
}
#endif

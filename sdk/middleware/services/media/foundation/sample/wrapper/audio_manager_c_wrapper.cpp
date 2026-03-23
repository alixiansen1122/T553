/*
 * Copyright (c) CompanyNameMagicTag. 2023-2023. All rights reserved.
 * Description: audio manager c wrapper
 * Author: Media Software Group
 * Create: 2023-08-11
 */

#include "audio_manager_c_wrapper.h"

#include <iostream>
#include <memory>
#include <cstdlib>
#include <cstdio>
#include <string>
#include "securec.h"
#include "audio_utils.h"
#include "audio_errors.h"
#include "audio_manager.h"
#include "audio_stream_in.h"
#include "audio_stream_out.h"
#include "audio_base_type.h"
#include "media_thread_adapt.h"
#include "bts_br_gap.h"
#include "bts_a2dp_source.h"
#include "bts_a2dp_sink.h"

using Audio::AudioManager;
using Audio::AudioStreamIn;
using Audio::AudioStreamOut;
using Audio::InterruptListener;
using Audio::AudioInterrupt;

#ifdef __cplusplus
extern "C" {
#endif

#define LOG_MODULE_NAME "AudioManagerCWrapper"
#define BUFFER_SIZE            120
#define VQE_PREFIX_LEN         18
#define PEQ_PREFIX_LEN         50
#define NV_AUDIO_PEQ_PARAM_SIZE 41
#define NV_AUDIO_VQE_PARAM_SIZE 53
#define PEQ_PARAM_SIZE         (NV_AUDIO_PEQ_PARAM_SIZE * sizeof(uint32_t))
#define VQE_PARAM_SIZE         (NV_AUDIO_VQE_PARAM_SIZE * sizeof(uint32_t))
#define SEA_BUFFER_SIZE        (VQE_PARAM_SIZE + VQE_PREFIX_LEN)
#define AEF_BUFFER_SIZE        (PEQ_PARAM_SIZE + PEQ_PREFIX_LEN)
#define AEF_BUFFER_LEN         256
#define SEC_TO_US            1000000
#define US_TO_NS             1000

static AudioManager& g_amInstance = AudioManager::GetInstance();
static std::shared_ptr<AudioStreamIn> g_streamIn = nullptr;
static std::shared_ptr<AudioStreamOut> g_streamOut = nullptr;
static AudioSession g_sessionId;
static AudioInterrupt g_interrupt;
static MediaMutexHandle g_a2dpSinkMutex = nullptr;

typedef struct {
    AudioSession sessionId;
    AudioInterrupt interrupt;
    std::shared_ptr<AudioStreamOut> a2dpAsohandle;
    std::shared_ptr<AudioStreamIn> a2dpAsihandle;
    bool isA2dpSinkPause;
    bool isA2dpSinkStopped;
    bool isA2dpSinkResumed;
    MediaMutexHandle mutex;
    MediaThreadCondHandle cond;
    MediaThreadIdHandle taskHandle;
} AudioManagerA2dpContext;

static AudioManagerA2dpContext *g_a2dpCtx = nullptr;
static CapturerInputConfig  g_a2dpInputConfig = {};
static AudioRendererConfig  g_a2dpRenderConfig = {};

class AudioManagerInterruptListener : public InterruptListener {
public:
    AudioManagerInterruptListener(){};
    ~AudioManagerInterruptListener()override {};

    void OnInterrupt(int32_t type, int32_t hint) override
    {
        ALOGI("type:%d hint:%d", type, hint);
    }
};

class AudioManagerA2dpInterruptListener : public InterruptListener {
public:
    explicit AudioManagerA2dpInterruptListener(AudioManagerA2dpContext *context)
    {
        a2dpContext = context;
    };
    ~AudioManagerA2dpInterruptListener() override {} ;

    void OnInterrupt(int32_t type, int32_t hint) override
    {
        ALOGI("type:%d hint:%d", type, hint);
        if (g_a2dpCtx == nullptr || a2dpContext == nullptr) {
            ALOGE("context is NULL");
            return;
        }
        if (type == INTERRUPT_TYPE_BEGIN && hint == INTERRUPT_HINT_PAUSE) {
            ALOGI("a2dp sink INTERRUPT_HINT_PAUSE");
            MediaMutexLock(a2dpContext->mutex);
            a2dpContext->isA2dpSinkPause = true;
            MediaThreadCondSignal(a2dpContext->cond);
            MediaMutexUnLock(a2dpContext->mutex);
        } else if (type == INTERRUPT_TYPE_END && hint == INTERRUPT_HINT_RESUME) {
            ALOGI("a2dp sink INTERRUPT_HINT_RESUME");
            MediaMutexLock(a2dpContext->mutex);
            a2dpContext->isA2dpSinkResumed = true;
            MediaThreadCondSignal(a2dpContext->cond);
            MediaMutexUnLock(a2dpContext->mutex);
        } else if (type == INTERRUPT_TYPE_BEGIN && hint == INTERRUPT_HINT_STOP) {
            ALOGI("a2dp sink INTERRUPT_HINT_STOP");
            MediaMutexLock(a2dpContext->mutex);
            a2dpContext->isA2dpSinkStopped = true;
            MediaThreadCondSignal(a2dpContext->cond);
            MediaMutexUnLock(a2dpContext->mutex);
        }
    }
private:
    AudioManagerA2dpContext *a2dpContext = nullptr;
};

bool AudioManagerInit(void)
{
    bool success = g_amInstance.Initialize();
    if (!success) {
        ALOGE("audio manager init failed");
        return false;
    }

    ALOGD("success");
    return true;
}

AudioSession AudioManagerMakeSessionId(void)
{
    return g_amInstance.MakeSessionId();
}

int32_t AudioManagerSetDeviceConnectionState(AudioDeviceType deviceType, AudioDeviceConnectState connectState)
{
    int32_t ret = g_amInstance.SetDeviceConnectionState(deviceType, connectState);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("set device 0x%x connect %d failed 0x%x", deviceType, connectState, ret);
        return AUDIO_ERROR;
    }

    ALOGD("success");
    return AUDIO_SUCCESS;
}

int32_t AudioManagerActivateInterrupt(AudioStreamType streamType, AudioSession sessionId)
{
    std::shared_ptr<AudioManagerInterruptListener> listener = std::make_shared<AudioManagerInterruptListener>();
    if (listener == nullptr || listener.get() == nullptr) {
        ALOGE("listener is nullptr");
        return AUDIO_ERROR;
    }
    g_interrupt = { streamType, sessionId, listener };
    if (g_amInstance.ActivateAudioInterrupt(g_interrupt) == INTERRUPT_FAILED) {
        ALOGE("activate audio interrupt faild");
        return AUDIO_ERROR;
    }

    ALOGD("success");
    return AUDIO_SUCCESS;
}

int32_t AudioManagerDeactivateInterrupt(void)
{
    if (g_amInstance.DeactivateAudioInterrupt(g_interrupt) != AUDIO_SUCCESS) {
        ALOGE("deactivate audio interrupt faild");
        return AUDIO_ERROR;
    }

    ALOGD("success");
    return AUDIO_SUCCESS;
}

int32_t AudioStreamInInit(CapturerInputConfig captureConfig)
{
    g_streamIn = std::make_shared<AudioStreamIn>();
    if (g_streamIn == nullptr || g_streamIn.get() == nullptr) {
        ALOGE("audio stream in is nullptr");
        return AUDIO_ERROR;
    }

    int32_t ret = g_streamIn->Init(captureConfig);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("audio stream in init failed 0x%x", ret);
        return AUDIO_ERROR;
    }

    ALOGD("success");
    return AUDIO_SUCCESS;
}

int32_t AudioStreamInStart(void)
{
    int32_t ret = g_streamIn->Start();
    if (ret != AUDIO_SUCCESS) {
        ALOGE("audio stream in start failed 0x%x", ret);
        return AUDIO_ERROR;
    }

    ALOGD("success");
    return AUDIO_SUCCESS;
}

int32_t AudioStreamInObtainBuffer(uint8_t *data, uint32_t *size)
{
    CapturerBuffer buffer;
    buffer.data = data;
    buffer.size = *size;
    int32_t ret = g_streamIn->ObtainBuffer(buffer, false);
    if (ret < 0) {
        return ret;
    }
    *size = static_cast<uint32_t>(ret);
    return AUDIO_SUCCESS;
}

int32_t AudioStreamInGetPosition(int64_t *pos)
{
    if (pos == NULL) {
        ALOGE("pos is NULL!");
        return AUDIO_ERROR;
    }
    struct timespec timestamp = {};
    int32_t ret = g_streamIn->GetTimestamp(timestamp, ::Audio::Timebase::MONOTONIC);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("streamIn_::GetTimestamp failed:0x%x", ret);
        return AUDIO_ERROR;
    }
    *pos = timestamp.tv_sec *SEC_TO_US + timestamp.tv_nsec / US_TO_NS;
    return AUDIO_SUCCESS;
}

int32_t AudioStreamInSetBitRate(uint32_t bitrate)
{
    std::string str("setbitrate = " + std::to_string(bitrate));
    if (g_streamIn == nullptr) {
        ALOGE("audio stream in handle is NUll!");
        return AUDIO_ERROR;
    }
    int32_t ret = g_streamIn->SetParam(str.c_str(), str.size());
    if (ret != AUDIO_SUCCESS) {
        ALOGE("set bitrate failed");
        return AUDIO_ERROR;
    }
    return AUDIO_SUCCESS;
}

int32_t AudioStreamInStop(void)
{
    int32_t ret = g_streamIn->Stop();
    if (ret != AUDIO_SUCCESS) {
        ALOGE("audio stream in stop failed 0x%x", ret);
        return AUDIO_ERROR;
    }

    ALOGD("success");
    return AUDIO_SUCCESS;
}

int32_t AudioStreamInDeinit(void)
{
    if (g_streamIn != nullptr) {
        g_streamIn->Release();
        g_streamIn.reset();
        g_streamIn = nullptr;
    }

    ALOGD("success");
    return AUDIO_SUCCESS;
}

int32_t AudioStreamOutInit(AudioRendererConfig renderConfig)
{
    g_streamOut = std::make_shared<AudioStreamOut>();
    if (g_streamOut == nullptr || g_streamOut.get() == nullptr) {
        ALOGE("audio stream out is nullptr");
        return AUDIO_ERROR;
    }

    int32_t ret = g_streamOut->Init(renderConfig);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("audio stream out init failed 0x%x", ret);
        return AUDIO_ERROR;
    }

    ALOGD("success");
    return AUDIO_SUCCESS;
}

int32_t AudioStreamOutSetMute(bool mute)
{
    int32_t ret = g_streamOut->SetMute(mute);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("audio stream out set mute failed 0x%x", ret);
        return AUDIO_ERROR;
    }

    ALOGD("success");
    return AUDIO_SUCCESS;
}

int32_t AudioManagerSetSeaEnable(bool enable)
{
    std::string str("seaenable = " + std::to_string(enable));
    int32_t ret = g_streamOut->SetParam(str.c_str(), str.size());
    if (ret != AUDIO_SUCCESS) {
        ALOGE("set sea enable failed");
        return AUDIO_ERROR;
    }

    return AUDIO_SUCCESS;
}

int32_t AudioManagerSetVolume(AudioStreamType streamType, int32_t volume)
{
    if (!g_amInstance.SetVolume(streamType, volume)) {
        ALOGE("set cat1 downlink volume failed");
        return AUDIO_ERROR;
    }
    return AUDIO_SUCCESS;
}

int32_t AudioManagerGetVolume(AudioStreamType streamType)
{
    return g_amInstance.GetVolume(streamType);
}

int32_t AudioStreamOutPlay(void)
{
    int32_t ret = g_streamOut->Play();
    if (ret != AUDIO_SUCCESS) {
        ALOGE("audio stream out play failed 0x%x", ret);
        return AUDIO_ERROR;
    }

    ALOGD("success");
    return AUDIO_SUCCESS;
}

int32_t AudioStreamOutStreamWrite(const uint8_t *data, uint32_t size)
{
    uint64_t writtenSize = 0;
    AudioRendererConfig renderConfig = {};
    int32_t ret = g_streamOut->StreamWrite(data, size, writtenSize, renderConfig);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("audio stream out play failed 0x%x", ret);
        return AUDIO_ERROR;
    }

    ALOGD("success");
    return AUDIO_SUCCESS;
}

int32_t AudioStreamOutStop(void)
{
    int32_t ret = g_streamOut->Stop();
    if (ret != AUDIO_SUCCESS) {
        ALOGE("audio stream out stop failed 0x%x", ret);
        return AUDIO_ERROR;
    }

    ALOGD("success");
    return AUDIO_SUCCESS;
}

int32_t AudioStreamOutDeinit(void)
{
    if (g_streamOut != nullptr) {
        g_streamOut->Release();
        g_streamOut.reset();
        g_streamOut = nullptr;
    }

    ALOGD("success");
    return AUDIO_SUCCESS;
}

static bool CheckA2dpSinkConnectState(void)
{
    bd_addr_t activeBdAddr = a2dp_snk_get_active_device();
    int state = a2dp_snk_get_playing_state(&activeBdAddr);
    if (state == A2DP_IS_PLAYING) {
        return true;
    }
    return false;
}

static bool CheckA2dpSourceConnectState(void)
{
    bd_addr_t activeBdAddr = a2dp_src_get_active_device();
    int32_t connState = a2dp_src_get_device_connect_state(&activeBdAddr);
    if (connState == PROFILE_STATE_CONNECTED) {
        return true;
    }
    return false;
}

static int32_t AudioManagerA2dpSinkContextInit(void)
{
    if (g_a2dpCtx != nullptr) {
        ALOGI("a2dp sink context has been inited!");
        return AUDIO_SUCCESS;
    }
    g_a2dpCtx = new AudioManagerA2dpContext;
    if (g_a2dpCtx == nullptr) {
        ALOGE("malloc failed");
        return AUDIO_ERROR;
    }
    g_a2dpCtx->sessionId = AUDIO_SESSION_ID_NONE;
    g_a2dpCtx->a2dpAsohandle = nullptr;
    g_a2dpCtx->a2dpAsihandle = nullptr;
    g_a2dpCtx->isA2dpSinkPause = false;
    g_a2dpCtx->isA2dpSinkStopped = false;
    g_a2dpCtx->isA2dpSinkResumed = false;
    g_a2dpCtx->mutex = MediaMutexCreate(nullptr);
    if (g_a2dpCtx->mutex == nullptr) {
        ALOGE("create mutex failed");
        goto EXIT2;
    }
    g_a2dpCtx->cond = MediaThreadCondCreate();
    if (g_a2dpCtx->cond == nullptr) {
        ALOGE("create cond failed");
        goto EXIT1;
    }
    return AUDIO_SUCCESS;
EXIT1:
    (void)MediaMutexDestroy(&g_a2dpCtx->mutex);
EXIT2:
    free(g_a2dpCtx);
    g_a2dpCtx = nullptr;
    return AUDIO_ERROR;
}

int32_t AudioManagerA2dpSinkSetParam(AudioFormatConfig config)
{
    g_a2dpInputConfig.audioFormat = config.audioFormat;
    g_a2dpInputConfig.sampleRate = config.sampleRate;
    g_a2dpInputConfig.channelCount = config.channelCount;
    g_a2dpInputConfig.streamType = AUDIO_STREAM_A2DP_MUSIC;
    g_a2dpInputConfig.linkDir = AUDIO_LINK_NONE;

    g_a2dpRenderConfig.audioFormat = config.audioFormat;
    g_a2dpRenderConfig.sampleRate = config.sampleRate;
    g_a2dpRenderConfig.channelCount = config.channelCount;
    g_a2dpRenderConfig.streamType = AUDIO_STREAM_A2DP_MUSIC;
    g_a2dpRenderConfig.linkDir = AUDIO_LINK_NONE;
    return AUDIO_SUCCESS;
}

int32_t AudioManagerA2dpSinkSetAefParam(int32_t effectType, const char *param, uint32_t length)
{
    if (param == nullptr) {
        ALOGE("aef param null");
        return AUDIO_ERROR;
    }
    if (g_a2dpCtx == nullptr || g_a2dpCtx->a2dpAsohandle == nullptr) {
        ALOGE("aso handle is null!");
        return AUDIO_ERROR;
    }
    char *aefBuf = (char *)malloc(AEF_BUFFER_SIZE);
    if (aefBuf == nullptr) {
        ALOGE("malloc fail");
        return AUDIO_ERROR;
    }
    if (memset_s(aefBuf, AEF_BUFFER_SIZE, 0, AEF_BUFFER_SIZE) != EOK) {
        ALOGE("memset_s fail");
        free(aefBuf);
        return AUDIO_ERROR;
    }
    int32_t prefixLen = sprintf_s(aefBuf, AEF_BUFFER_SIZE, "%s = %d#%u:", "setaefparam", effectType, length);
    if (prefixLen < 0) {
        ALOGE("sprintf_s failed");
        free(aefBuf);
        return AUDIO_ERROR;
    }
    if (memcpy_s(aefBuf + prefixLen, AEF_BUFFER_SIZE - prefixLen, param, length) != EOK) {
        ALOGE("memcpy_s failed");
        free(aefBuf);
        return AUDIO_ERROR;
    }
    int32_t ret = g_a2dpCtx->a2dpAsohandle->SetParam(aefBuf, AEF_BUFFER_SIZE);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("set param failed");
        free(aefBuf);
        return AUDIO_ERROR;
    }
    free(aefBuf);
    return AUDIO_SUCCESS;
}

int32_t AudioManagerA2dpSinkSetAefEnable(int32_t effectType, bool enable)
{
    char buf[BUFFER_SIZE] = { 0 };
    int32_t ret = sprintf_s(buf, BUFFER_SIZE, "%s = %d#%d", "aefenable", enable, effectType);
    if (ret < 0) {
        ALOGE("sprintf_s failed");
        return AUDIO_ERROR;
    }
    if (g_a2dpCtx == nullptr || g_a2dpCtx->a2dpAsohandle == nullptr) {
        ALOGE("aso handle is null!");
        return AUDIO_ERROR;
    }
    ret = g_a2dpCtx->a2dpAsohandle->SetParam(buf, BUFFER_SIZE);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("set aso param failed");
        return AUDIO_ERROR;
    }
    return AUDIO_SUCCESS;
}

int32_t AudioManagerSetSeaParam(const uint8_t *seaParam, uint16_t len)
{
    if (seaParam == nullptr) {
        ALOGE("sea_param null");
        return AUDIO_INVALID_PARAMS;
    }
    char *seaBuf = (char *)malloc(SEA_BUFFER_SIZE);
    if (seaBuf == nullptr) {
        ALOGE("malloc fail");
        return AUDIO_NO_MEM;
    }
    if (memset_s(seaBuf, SEA_BUFFER_SIZE, 0, SEA_BUFFER_SIZE) != EOK) {
        ALOGE("memset_s fail");
        free(seaBuf);
        return AUDIO_NO_MEM;
    }
    int32_t prefixLen = sprintf_s(seaBuf, SEA_BUFFER_SIZE, "%s = %u:", "setseaparam", len);
    if (prefixLen < 0) {
        ALOGE("sprintf_s failed");
        free(seaBuf);
        return AUDIO_ERROR;
    }
    if (memcpy_s(seaBuf + prefixLen, SEA_BUFFER_SIZE - prefixLen, seaParam, len) != EOK) {
        ALOGE("memcpy_s failed");
        free(seaBuf);
        return AUDIO_ERROR;
    }
    int32_t ret = g_amInstance.SetParam(AUDIO_UP_LINK, seaBuf, AEF_BUFFER_LEN);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("set param failed");
        free(seaBuf);
        return AUDIO_ERROR;
    }
    free(seaBuf);
    ALOGI("set param success!");
    return AUDIO_SUCCESS;
}

int32_t AudioManagerGetSeaParam(uint8_t *seaParam, uint16_t len)
{
    if (seaParam == nullptr) {
        ALOGE("sea_param null");
        return AUDIO_INVALID_PARAMS;
    }
    char *seaBuf = (char *)malloc(SEA_BUFFER_SIZE);
    if (seaBuf == nullptr) {
        ALOGE("malloc fail");
        return AUDIO_NO_MEM;
    }
    if (memset_s(seaBuf, SEA_BUFFER_SIZE, 0, SEA_BUFFER_SIZE) != EOK) {
        ALOGE("memset_s fail");
        free(seaBuf);
        return AUDIO_NO_MEM;
    }
    int32_t prefixLen = sprintf_s(seaBuf, SEA_BUFFER_SIZE, "%s = %u", "getseaparam", len);
    if (prefixLen < 0) {
        ALOGE("sprintf_s failed");
        free(seaBuf);
        return AUDIO_ERROR;
    }
    int32_t ret = g_amInstance.GetParam(AUDIO_UP_LINK, seaBuf, SEA_BUFFER_SIZE);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("get param failed");
        free(seaBuf);
        return AUDIO_ERROR;
    }
    if (memcpy_s(seaParam, SEA_BUFFER_SIZE, seaBuf, len) != EOK) {
        ALOGE("memcpy_s failed");
        free(seaBuf);
        return AUDIO_ERROR;
    }
    free(seaBuf);
    ALOGI("get param success!");
    return AUDIO_SUCCESS;
}

static void WaitA2dpStartWhenDelay(AudioManagerA2dpContext *a2dpContext)
{
    while (true) {
        if (a2dpContext->isA2dpSinkResumed ||
            a2dpContext->isA2dpSinkStopped) {
            break;
        }
        ALOGI("a2dp sink delay start");
        MediaThreadCondWait(a2dpContext->cond, a2dpContext->mutex);
        ALOGI("a2dp sink delay end");
    }
}

int32_t AudioManagerSetAefParam(int32_t effectType, const uint8_t *aefParam, uint16_t len)
{
    if (aefParam == nullptr) {
        ALOGE("aef_param null");
        return AUDIO_INVALID_PARAMS;
    }
    char *aefBuf = (char *)malloc(AEF_BUFFER_LEN);
    if (aefBuf == nullptr) {
        ALOGE("malloc fail");
        return AUDIO_NO_MEM;
    }
    if (memset_s(aefBuf, AEF_BUFFER_LEN, 0, AEF_BUFFER_LEN) != EOK) {
        ALOGE("memset_s fail");
        free(aefBuf);
        return AUDIO_NO_MEM;
    }
    int32_t prefixLen = sprintf_s(aefBuf, AEF_BUFFER_LEN, "%s = %d#%u:", "setaefparam", effectType, len);
    if (prefixLen < 0) {
        ALOGE("sprintf_s failed");
        free(aefBuf);
        return AUDIO_ERROR;
    }
    if (memcpy_s(aefBuf + prefixLen, AEF_BUFFER_LEN - prefixLen, aefParam, len) != EOK) {
        ALOGE("memcpy_s failed");
        free(aefBuf);
        return AUDIO_ERROR;
    }
    int32_t ret = g_amInstance.SetParam(AUDIO_DOWN_LINK, aefBuf, AEF_BUFFER_LEN);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("set param failed");
        free(aefBuf);
        return AUDIO_ERROR;
    }
    free(aefBuf);
    ALOGI("set param success!");
    return AUDIO_SUCCESS;
}

int32_t AudioManagerGetAefParam(int32_t effectType, uint8_t *aefParam, uint16_t len)
{
    if (aefParam == nullptr) {
        ALOGE("aef_param null");
        return AUDIO_INVALID_PARAMS;
    }
    char *aefBuf = (char *)malloc(AEF_BUFFER_LEN);
    if (aefBuf == nullptr) {
        ALOGE("malloc fail");
        return AUDIO_NO_MEM;
    }
    if (memset_s(aefBuf, AEF_BUFFER_LEN, 0, AEF_BUFFER_LEN) != EOK) {
        ALOGE("memset_s fail");
        free(aefBuf);
        return AUDIO_NO_MEM;
    }
    int32_t prefixLen = sprintf_s(aefBuf, AEF_BUFFER_LEN, "%s = %d#%u:", "getaefparam", effectType, len);
    if (prefixLen < 0) {
        ALOGE("sprintf_s failed");
        free(aefBuf);
        return AUDIO_ERROR;
    }
    int32_t ret = g_amInstance.GetParam(AUDIO_DOWN_LINK, aefBuf, AEF_BUFFER_LEN);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("get param failed");
        free(aefBuf);
        return AUDIO_ERROR;
    }
    if (memcpy_s(aefParam, AEF_BUFFER_LEN, aefBuf, len) != EOK) {
        ALOGE("memcpy_s failed");
        free(aefBuf);
        return AUDIO_ERROR;
    }
    free(aefBuf);
    ALOGI("get param success!");
    return AUDIO_SUCCESS;
}

static int32_t DisConnectBluetoothA2dpDevice(void)
{
    if (CheckA2dpSourceConnectState()) {
        int32_t ret = g_amInstance.SetDeviceConnectionState(OUT_BLUETOOTH_A2DP,
            AUDIO_DEVICE_DISCONNECTED);
        if (ret != AUDIO_SUCCESS) {
            ALOGE("set OUT_BLUETOOTH_A2DP disConnected fail.\n");
            return ret;
        }
    }
    return AUDIO_SUCCESS;
}

static int32_t ConnectBluetoothA2dpDevice(void)
{
    if (CheckA2dpSourceConnectState()) {
        int32_t ret = g_amInstance.SetDeviceConnectionState(OUT_BLUETOOTH_A2DP,
            AUDIO_DEVICE_CONNECTED);
        if (ret != AUDIO_SUCCESS) {
            ALOGE("set OUT_BLUETOOTH_A2DP connected fail.\n");
            return ret;
        }
    }
    return AUDIO_SUCCESS;
}

static int32_t AudioManagerA2dpSessionInitInner(AudioManagerA2dpContext *a2dpContext)
{
    AudioSession sessionID = g_amInstance.MakeSessionId();
    if (sessionID == AUDIO_SESSION_ID_NONE) {
        ALOGE("get sessionid fail.\n");
        return AUDIO_ERROR;
    }
    g_a2dpInputConfig.sessionID = sessionID;
    g_a2dpRenderConfig.sessionID = sessionID;
    std::shared_ptr<AudioManagerA2dpInterruptListener> listener =
        std::make_shared<AudioManagerA2dpInterruptListener>(a2dpContext);
    if (listener == nullptr || listener.get() == nullptr) {
        ALOGE("listener is nullptr");
        return AUDIO_ERROR;
    }
    a2dpContext->interrupt = { AUDIO_STREAM_A2DP_MUSIC, sessionID, listener };
    return AUDIO_SUCCESS;
}

static int32_t AudioManagerA2dpSessionInit(AudioManagerA2dpContext *a2dpContext)
{
    bool success = g_amInstance.Initialize();
    if (!success) {
        ALOGE("audio manager init failed");
        return AUDIO_ERROR;
    }
    int32_t ret = 0;
    while (true) {
        ret = AudioManagerA2dpSessionInitInner(a2dpContext);
        if (ret != AUDIO_SUCCESS) {
            return AUDIO_ERROR;
        }
        /* After the delayed stream is woken up, the focus needs to be applied for again
       because the focus may be outdated and the policy needs to be obtained again. */
        int32_t strategy = g_amInstance.ActivateAudioInterrupt(a2dpContext->interrupt);
        if (strategy == INTERRUPT_FAILED) {
            ALOGE("activate audio interrupt faild");
            return AUDIO_ERROR;
        }
        if (strategy == INTERRUPT_DELAYED) {
            WaitA2dpStartWhenDelay(a2dpContext);
            if (a2dpContext->isA2dpSinkStopped) {
                goto EXIT;
            }
            continue;
        }
        break;
    }
    a2dpContext->isA2dpSinkResumed = false;
    a2dpContext->isA2dpSinkPause = false;
    ret = DisConnectBluetoothA2dpDevice();
    if (ret != AUDIO_SUCCESS) {
        goto EXIT;
    }
    ret = g_amInstance.SetDeviceConnectionState(IN_BLUETOOTH_A2DP,
        AUDIO_DEVICE_CONNECTED);
    if (ret != AUDIO_SUCCESS) {
        ConnectBluetoothA2dpDevice();
        goto EXIT;
    }
    return AUDIO_SUCCESS;
EXIT:
    g_amInstance.DeactivateAudioInterrupt(a2dpContext->interrupt);
    return AUDIO_ERROR;
}

static void AudioManagerA2dpStreamInDeinit(AudioManagerA2dpContext *a2dpContext)
{
    if (a2dpContext->a2dpAsihandle != nullptr) {
        a2dpContext->a2dpAsihandle->Release();
        a2dpContext->a2dpAsihandle.reset();
        a2dpContext->a2dpAsihandle = nullptr;
    }
}

static void AudioManagerA2dpStreamOutDeinit(AudioManagerA2dpContext *a2dpContext)
{
    if (a2dpContext->a2dpAsohandle != nullptr) {
        a2dpContext->a2dpAsohandle->Release();
        a2dpContext->a2dpAsohandle.reset();
        a2dpContext->a2dpAsohandle = nullptr;
    }
}

static int32_t AudioManagerA2dpInit(AudioManagerA2dpContext *a2dpContext, bool isNeedActivate)
{
    int32_t ret = AUDIO_SUCCESS;
    if (isNeedActivate) {
        ret = AudioManagerA2dpSessionInit(a2dpContext);
        if (ret != AUDIO_SUCCESS) {
            ALOGE("session init fail \n");
            return ret;
        }
    }
    a2dpContext->a2dpAsihandle = std::make_shared<AudioStreamIn>();
    if (a2dpContext->a2dpAsihandle == nullptr || a2dpContext->a2dpAsihandle.get() == nullptr) {
        ALOGE("audio stream out is nullptr");
        goto exit;
    }

    ret = a2dpContext->a2dpAsihandle->Init(g_a2dpInputConfig);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("AudioStreamInInit fail \n");
        a2dpContext->a2dpAsihandle->Release();
        goto exit;
    }
    a2dpContext->a2dpAsohandle = std::make_shared<AudioStreamOut>();
    if (a2dpContext->a2dpAsohandle == nullptr || a2dpContext->a2dpAsohandle.get() == nullptr) {
        ALOGE("audio stream out is nullptr");
        goto exit;
    }
    ret = a2dpContext->a2dpAsohandle->Init(g_a2dpRenderConfig);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("AudioStreamOutInit fail \n");
        AudioManagerA2dpStreamInDeinit(a2dpContext);
        AudioManagerA2dpStreamOutDeinit(a2dpContext);
        goto exit;
    }

    return AUDIO_SUCCESS;
exit:
    (void)ConnectBluetoothA2dpDevice();
    g_amInstance.SetDeviceConnectionState(IN_BLUETOOTH_A2DP, AUDIO_DEVICE_DISCONNECTED);
    g_amInstance.DeactivateAudioInterrupt(a2dpContext->interrupt);
    return ret;
}

static void AudioManagerA2dpReleaseHandle(AudioManagerA2dpContext *a2dpContext)
{
    AudioManagerA2dpStreamInDeinit(a2dpContext);
    AudioManagerA2dpStreamOutDeinit(a2dpContext);
    g_amInstance.SetDeviceConnectionState(IN_BLUETOOTH_A2DP, AUDIO_DEVICE_DISCONNECTED);
    g_amInstance.DeactivateAudioInterrupt(a2dpContext->interrupt);
}

static int32_t AudioManagerA2dpSinkStart(AudioManagerA2dpContext *a2dpContext)
{
    if (a2dpContext->a2dpAsihandle == nullptr || a2dpContext->a2dpAsohandle == nullptr) {
        ALOGE("asi or aso handle has been destroyed!");
        return AUDIO_SUCCESS;
    }
    int32_t ret = a2dpContext->a2dpAsihandle->Start();
    if (ret != AUDIO_SUCCESS) {
        ALOGE("AudioStreamInStart fail.");
        AudioManagerA2dpReleaseHandle(a2dpContext);
        return ret;
    }

    ret = a2dpContext->a2dpAsohandle->Play();
    if (ret != AUDIO_SUCCESS) {
        ALOGE("AudioStreamOutStart fail.");
        a2dpContext->a2dpAsihandle->Stop();
        AudioManagerA2dpReleaseHandle(a2dpContext);
        return ret;
    }
    uint32_t shmId;
    ret = a2dpContext->a2dpAsohandle->GetCurrentChannelId(shmId);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("AudioStreamOutGetCurrentChannelId fail.");
        goto exit;
    }

    ret = a2dpContext->a2dpAsihandle->AttachBackend(shmId);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("AudioStreamInAttachBackend fail.");
        goto exit;
    }

    return AUDIO_SUCCESS;
exit:
    a2dpContext->a2dpAsihandle->Stop();
    a2dpContext->a2dpAsohandle->Stop();
    AudioManagerA2dpReleaseHandle(a2dpContext);
    return ret;
}

static int32_t AudioManagerA2dpSinkStop(AudioManagerA2dpContext *a2dpContext)
{
    int32_t ret = a2dpContext->a2dpAsihandle->Stop();
    if (ret != AUDIO_SUCCESS) {
        ALOGE("AudioStreamInStop fail.");
    }
    ret = a2dpContext->a2dpAsohandle->Stop();
    if (ret != AUDIO_SUCCESS) {
        ALOGE("AudioStreamOutStop fail.");
    }
    return AUDIO_SUCCESS;
}

static int32_t AudioManagerA2dpDeinit(AudioManagerA2dpContext *a2dpContext, bool isNeedDeactivate)
{
    AudioManagerA2dpStreamInDeinit(a2dpContext);
    AudioManagerA2dpStreamOutDeinit(a2dpContext);
    if (isNeedDeactivate) {
        int32_t ret = g_amInstance.SetDeviceConnectionState(IN_BLUETOOTH_A2DP, AUDIO_DEVICE_DISCONNECTED);
        if (ret != AUDIO_SUCCESS) {
            ALOGE("AudioManagerSetDeviceConnectionState fail.\n");
        }
        ConnectBluetoothA2dpDevice();
        ret = g_amInstance.DeactivateAudioInterrupt(a2dpContext->interrupt);
        if (ret != AUDIO_SUCCESS) {
            return ret;
        }
    }
    return AUDIO_SUCCESS;
}

static int32_t AudioManagerA2dpSinkResume(AudioManagerA2dpContext *a2dpContext)
{
    if (!CheckA2dpSinkConnectState()) {
        ALOGI("a2dp sink has been disconnected.not need to resume\n");
        return AUDIO_SUCCESS;
    }
    if (a2dpContext->a2dpAsihandle != nullptr || a2dpContext->a2dpAsohandle != nullptr) {
        ALOGI("a2dp is existed, not need to resume.\n");
        return AUDIO_SUCCESS;
    }
    int32_t ret = AudioManagerA2dpInit(a2dpContext, false);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("a2dp sink init fail.\n");
        return AUDIO_ERROR;
    }
    ret = AudioManagerA2dpSinkStart(a2dpContext);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("a2dp sink start fail.\n");
        return AUDIO_ERROR;
    }
    return AUDIO_SUCCESS;
}

static int32_t AudioManagerA2dpSinkPause(AudioManagerA2dpContext *a2dpContext)
{
    if (a2dpContext->a2dpAsihandle == nullptr || a2dpContext->a2dpAsohandle == nullptr) {
        ALOGI("asi or aso handle has been destroy!");
        return AUDIO_SUCCESS;
    }
    AudioManagerA2dpSinkStop(a2dpContext);
    AudioManagerA2dpDeinit(a2dpContext, false);
    return AUDIO_SUCCESS;
}

static int32_t AudioManagerA2dpSinkAllStop(AudioManagerA2dpContext *a2dpContext)
{
    if (a2dpContext->a2dpAsihandle == nullptr || a2dpContext->a2dpAsohandle == nullptr) {
        // The stream has been destroyed. You need to delete the focus.
        AudioManagerA2dpDeinit(a2dpContext, true);
        return AUDIO_SUCCESS;
    }
    AudioManagerA2dpSinkStop(a2dpContext);
    AudioManagerA2dpDeinit(a2dpContext, true);
    return AUDIO_SUCCESS;
}

static int32_t ProcessA2dpInterrupt(AudioManagerA2dpContext *a2dpContext)
{
    if (a2dpContext->isA2dpSinkResumed) {
        int32_t ret = AudioManagerA2dpSinkResume(a2dpContext);
        if (ret != AUDIO_SUCCESS) {
            ALOGE("a2dp sink resume failed.");
            a2dpContext->isA2dpSinkResumed = false;
            return AUDIO_ERROR;
        }
        a2dpContext->isA2dpSinkResumed = false;
    }
    if (a2dpContext->isA2dpSinkPause) {
        AudioManagerA2dpSinkPause(a2dpContext);
        a2dpContext->isA2dpSinkPause = false;
    }
    return AUDIO_SUCCESS;
}

static void *A2dpSinkThread(void *priv)
{
    AudioManagerA2dpContext *a2dpContext = (AudioManagerA2dpContext *)priv;
    MediaMutexLock(a2dpContext->mutex);

    g_a2dpCtx->sessionId = AUDIO_SESSION_ID_NONE;
    g_a2dpCtx->a2dpAsohandle = nullptr;
    g_a2dpCtx->a2dpAsihandle = nullptr;
    g_a2dpCtx->isA2dpSinkPause = false;
    g_a2dpCtx->isA2dpSinkStopped = false;
    g_a2dpCtx->isA2dpSinkResumed = false;
    int32_t ret = AudioManagerA2dpInit(a2dpContext, true);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("a2dp sink init fail.\n");
        goto EXIT;
    }
    ret = AudioManagerA2dpSinkStart(a2dpContext);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("a2dp sink start fail.\n");
        goto EXIT;
    }
    while (true) {
        if (a2dpContext->isA2dpSinkStopped) {
            AudioManagerA2dpSinkAllStop(a2dpContext);
            break;
        }
        MediaThreadCondWait(a2dpContext->cond, a2dpContext->mutex);
        ret = ProcessA2dpInterrupt(a2dpContext);
        if (ret != AUDIO_SUCCESS) {
            goto EXIT;
        }
    }
EXIT:
    MediaMutexUnLock(a2dpContext->mutex);
    return nullptr;
}

int32_t AudioManagerA2dpSinkCreate(void)
{
    MediaInitStaticMutexLock(&g_a2dpSinkMutex);
    MediaMutexLock(g_a2dpSinkMutex);
    if (g_a2dpCtx != nullptr) {
        ALOGE("a2dp sink is exist, need wait some time to create!");
        MediaMutexUnLock(g_a2dpSinkMutex);
        return AUDIO_SUCCESS;
    }
    int32_t ret = AudioManagerA2dpSinkContextInit();
    if (ret != AUDIO_SUCCESS) {
        MediaMutexUnLock(g_a2dpSinkMutex);
        return AUDIO_ERROR;
    }
    MediaThreadattr attr = { "A2dpSinkThread", 0x2000, THREAD_SCHED_INVALID, 0, false };
    g_a2dpCtx->taskHandle = MediaThreadCreate(A2dpSinkThread, g_a2dpCtx, &attr);
    if (g_a2dpCtx->taskHandle == nullptr) {
        ALOGE("create voice call volte  thread failed");
        (void)MediaThreadCondDestroy(&g_a2dpCtx->cond);
        (void)MediaMutexDestroy(&g_a2dpCtx->mutex);
        free(g_a2dpCtx);
        g_a2dpCtx = nullptr;
        MediaMutexUnLock(g_a2dpSinkMutex);
        return AUDIO_ERROR;
    }
    MediaMutexUnLock(g_a2dpSinkMutex);
    return AUDIO_SUCCESS;
}

int32_t AudioManagerA2dpSinkDestroy(void)
{
    MediaInitStaticMutexLock(&g_a2dpSinkMutex);
    MediaMutexLock(g_a2dpSinkMutex);
    if (g_a2dpCtx == nullptr) {
        ALOGE("a2dp context is null, execute start command first");
        MediaMutexUnLock(g_a2dpSinkMutex);
        return AUDIO_SUCCESS;
    }
    ALOGI("A2dp sink stop signal one");
    MediaMutexLock(g_a2dpCtx->mutex);
    g_a2dpCtx->isA2dpSinkStopped = true;
    MediaThreadCondSignal(g_a2dpCtx->cond);

    MediaMutexUnLock(g_a2dpCtx->mutex);
    MediaThreadJoin(&g_a2dpCtx->taskHandle);
    (void)MediaThreadCondDestroy(&g_a2dpCtx->cond);
    (void)MediaMutexDestroy(&g_a2dpCtx->mutex);
    free(g_a2dpCtx);
    g_a2dpCtx = nullptr;
    MediaMutexUnLock(g_a2dpSinkMutex);
    return AUDIO_SUCCESS;
}

#ifdef __cplusplus
}
#endif

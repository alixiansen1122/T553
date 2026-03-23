/*
 * Copyright (c) CompanyNameMagicTag. 2023-2023. All rights reserved.
 * Description: voice call volte sample wrapper
 * Author: Media Software Group
 * Create: 2023-07-06
 */

#include "voice_call_volte_sample_wrapper.h"

#include <climits>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string>
#include <iostream>
#include <unistd.h>
#include "securec.h"

#include "audio_manager.h"
#include "audio_stream_in.h"
#include "audio_stream_out.h"
#include "audio_base_type.h"

#include "media_log.h"
#include "media_thread_adapt.h"
#include "audio_manager_c_wrapper.h"

#ifdef __cplusplus
extern "C" {
#endif

using namespace::Audio;

#define MEDIA_CMD_LEN_MAX 128
#define MEDIA_UNUSED(x) ((void)(x))

#define ARRAY_SIZE(array) (sizeof((array)) / sizeof((array)[0]))
const int32_t HEX_BASE = 16;
const int32_t NUMBER_BASE = 10;
const int32_t CHANNEL_COUNT = 2;
const int32_t SAMPLERATE_16000 = 16000;

typedef struct {
    AudioSession sessionId;
    AudioInterrupt interrupt;
    bool isVoiceCallVolteStopped;
    MediaMutexHandle mutex;
    MediaThreadCondHandle cond;
    uint8_t *data;
    uint32_t size;
    MediaThreadIdHandle taskHandle;
    AudioStreamType streamType;
} VoiceCallVolteContext;

static MediaMutexHandle g_voiceCallVolteContextMutex = nullptr;
static VoiceCallVolteContext *g_voiceCallVolteContext = nullptr;

static AudioManager& g_amInstance = AudioManager::GetInstance();
#ifdef LOCAL_VOICE_CALL
static std::shared_ptr<AudioStreamIn> g_streamIn = nullptr;
static std::shared_ptr<AudioStreamOut> g_streamOut = nullptr;
#else
static std::shared_ptr<AudioStreamIn> g_streamInUplink = nullptr;
static std::shared_ptr<AudioStreamIn> g_streamInDwnink = nullptr;
static std::shared_ptr<AudioStreamOut> g_streamOutUplink = nullptr;
static std::shared_ptr<AudioStreamOut> g_streamOutDownink = nullptr;
#endif

static int32_t StartVoiceCall(int32_t argc, const char **argv);
static int32_t StopVoiceCall(int32_t argc, const char **argv);
static int32_t VoiceCallSetVolume(int32_t argc, const char **argv);
static int32_t VoiceCallGetVolume(int32_t argc, const char **argv);
static int32_t VoiceCAllSeaEnable(int32_t argc, const char **argv);
static int32_t VoiceCallMicMute(int32_t argc, const char **argv);
static int32_t VoiceCallSetBitRate(int32_t argc, const char **argv);

typedef struct {
    char cmd[MEDIA_CMD_LEN_MAX];
    int32_t (*func)(int32_t argc, const char **argv);
} FuncsMap;

static FuncsMap g_voiceCallVolteFuncs[] = {
    { "start", StartVoiceCall },
    { "stop", StopVoiceCall },
    { "setvolume", VoiceCallSetVolume },
    { "getvolume", VoiceCallGetVolume },
    { "seaenable", VoiceCAllSeaEnable },
    { "mute", VoiceCallMicMute },
    { "bitrate", VoiceCallSetBitRate },
};

class VoiceCallVolteInterruptListener : public InterruptListener {
public:
    explicit VoiceCallVolteInterruptListener(VoiceCallVolteContext *in) : context(in)
    {
    }
    ~VoiceCallVolteInterruptListener() override {};

    void OnInterrupt(int32_t type, int32_t hint) override
    {
        MEDIA_INFO_LOG("OnInterrupt, type:%d, hint:%d", type, hint);

        if (type == INTERRUPT_TYPE_BEGIN && hint == INTERRUPT_HINT_PAUSE) {
            MEDIA_INFO_LOG("voice call volte pause signal one");
        }
        if (type == INTERRUPT_TYPE_END && hint == INTERRUPT_HINT_RESUME) {
            MEDIA_INFO_LOG("voice call volte resume signal one");
        }
        if (type == INTERRUPT_TYPE_BEGIN && hint == INTERRUPT_HINT_STOP) {
            MEDIA_INFO_LOG("voice call volte stop begin signal one");
        }
        if (type == INTERRUPT_TYPE_END && hint == INTERRUPT_HINT_STOP) {
            MEDIA_INFO_LOG("voice call volte stop end signal one");
            context->isVoiceCallVolteStopped = true;
        }
    }

private:
    VoiceCallVolteContext *context;
};

static void DeinitVoiceCallVolteResources(VoiceCallVolteContext *context)
{
#ifdef LOCAL_VOICE_CALL
    (void)g_streamIn.reset();
    (void)g_streamOut.reset();
#else
    (void)g_streamInUplink.reset();
    (void)g_streamOutUplink.reset();
    (void)g_streamInDwnink.reset();
    (void)g_streamOutDownink.reset();
#endif
}

static bool InitVoiceCallVolteResources(VoiceCallVolteContext *context)
{
    context->mutex = MediaMutexCreate(nullptr);
    if (context->mutex == nullptr) {
        MEDIA_ERR_LOG("create mutex failed");
        return false;
    }
    context->cond = MediaThreadCondCreate();
    if (context->cond == nullptr) {
        MEDIA_ERR_LOG("create Voice Call Volte thread cond failed");
        MediaMutexDestroy(&context->mutex);
        return false;
    }
    return true;
}

static int32_t LocalVoiceCallVolteStart(VoiceCallVolteContext *context)
{
    int32_t ret = AudioManagerSetDeviceConnectionState(IN_MODEM, AUDIO_DEVICE_CONNECTED);
    if (ret != 0) {
        MEDIA_ERR_LOG("set modem in device connect failed");
    }

    ret = AudioManagerSetDeviceConnectionState(OUT_MODEM, AUDIO_DEVICE_CONNECTED);
    if (ret != 0) {
        MEDIA_ERR_LOG("set modem out device connect failed");
    }

    // create downlink AudioStreamIn
    CapturerInputConfig captureConfig = {};
    captureConfig.audioFormat = PCM;
    captureConfig.channelCount = CHANNEL_COUNT;
    captureConfig.linkDir = AUDIO_DOWN_LINK;
    captureConfig.sampleRate = SAMPLERATE_16000;
    captureConfig.sessionID = context->sessionId;
    captureConfig.streamType = context->streamType;

    ret = AudioStreamInInit(captureConfig);
    if (ret != 0) {
        MEDIA_ERR_LOG("audio stream in init failed");
    }

    // create uplink AudioStreamOut
    AudioRendererConfig renderConfig = {};
    renderConfig.audioFormat = PCM;
    renderConfig.channelCount = 1;
    renderConfig.linkDir = AUDIO_UP_LINK;
    renderConfig.sampleRate = SAMPLERATE_16000;
    renderConfig.sessionID = context->sessionId;
    renderConfig.streamType = context->streamType;

    ret = AudioStreamOutInit(renderConfig);
    if (ret != 0) {
        MEDIA_ERR_LOG("audio stream out init failed");
    }

    ret = AudioStreamInStart();
    if (ret != 0) {
        MEDIA_ERR_LOG("audio stream in start failed");
    }

    ret = AudioStreamOutPlay();
    if (ret != 0) {
        MEDIA_ERR_LOG("audio stream out play failed");
    }
    return ret;
}

static int32_t LocalVoiceCallVolteStop(void)
{
    int32_t ret = AudioStreamInStop();
    if (ret != 0) {
        MEDIA_ERR_LOG("audio stream in stop failed");
    }

    ret = AudioStreamOutStop();
    if (ret != 0) {
        MEDIA_ERR_LOG("audio stream out stop failed");
    }

    ret = AudioStreamInDeinit();
    if (ret != 0) {
        MEDIA_ERR_LOG("audio stream in release failed");
    }

    ret = AudioStreamOutDeinit();
    if (ret != 0) {
        MEDIA_ERR_LOG("audio stream out release failed");
    }
    if (g_voiceCallVolteContext->streamType == AUDIO_STREAM_VOICE_CALL_VOLTE) {
        ret = AudioManagerSetDeviceConnectionState(IN_MODEM, AUDIO_DEVICE_DISCONNECTED);
        if (ret != 0) {
            MEDIA_ERR_LOG("set modem in device disconnect failed");
        }

        ret = AudioManagerSetDeviceConnectionState(OUT_MODEM, AUDIO_DEVICE_DISCONNECTED);
        if (ret != 0) {
            MEDIA_ERR_LOG("set modem out device disconnect failed");
        }
    }
    return ret;
}
#ifndef LOCAL_VOICE_CALL
static int32_t BluetoothVoiceCallVolteInit(VoiceCallVolteContext *context)
{
    g_streamInUplink = std::make_shared<AudioStreamIn>();
    CapturerInputConfig captureConfig = {};
    captureConfig.audioFormat = mSBC;
    captureConfig.channelCount = 1;
    captureConfig.linkDir = AUDIO_UP_LINK;
    captureConfig.sampleRate = SAMPLERATE_16000;
    captureConfig.sessionID = context->sessionId;
    captureConfig.streamType = context->streamType;
    int32_t ret = g_streamInUplink->Init(captureConfig);
    if (ret != 0) {
        MEDIA_ERR_LOG("audio stream in uplink init failed");
    }
    g_streamOutUplink = std::make_shared<AudioStreamOut>();
    AudioRendererConfig renderConfig = {};
    renderConfig.audioFormat = mSBC;
    renderConfig.channelCount = 1;
    renderConfig.linkDir = AUDIO_UP_LINK;
    renderConfig.sampleRate = SAMPLERATE_16000;
    renderConfig.sessionID = context->sessionId;
    renderConfig.streamType = context->streamType;
    ret = g_streamOutUplink->Init(renderConfig);
    if (ret != 0) {
        MEDIA_ERR_LOG("audio stream out uplink init failed");
    }
    g_streamInDwnink = std::make_shared<AudioStreamIn>();
    captureConfig.linkDir = AUDIO_DOWN_LINK;
    ret = g_streamInDwnink->Init(captureConfig);
    if (ret != 0) {
        MEDIA_ERR_LOG("audio stream in downink init failed");
    }
    g_streamOutDownink = std::make_shared<AudioStreamOut>();
    renderConfig.linkDir = AUDIO_DOWN_LINK;
    ret = g_streamOutDownink->Init(renderConfig);
    if (ret != 0) {
        MEDIA_ERR_LOG("audio stream out downink init failed");
    }
    return ret;
}

static int32_t BluetoothVoiceCallVolteStart(void)
{
    int32_t ret = g_streamInUplink->Start();
    if (ret != 0) {
        MEDIA_ERR_LOG("audio stream in uplink start failed");
    }

    ret = g_streamOutUplink->Play();
    if (ret != 0) {
        MEDIA_ERR_LOG("audio stream out uplink play failed");
    }

    ret = g_streamInDwnink->Start();
    if (ret != 0) {
        MEDIA_ERR_LOG("audio stream in downink start failed");
    }

    ret = g_streamOutDownink->Play();
    if (ret != 0) {
        MEDIA_ERR_LOG("audio stream out downink play failed");
    }

    uint32_t asoUplinkChannelId = 0;
    g_streamOutUplink->GetCurrentChannelId(asoUplinkChannelId);

    MEDIA_INFO_LOG("asoUplinkChannelId: 0x%x", asoUplinkChannelId);

    g_streamInUplink->AttachBackend(asoUplinkChannelId);

    uint32_t asiDownlinkChannelId = 0;
    g_streamInDwnink->GetCurrentChannelId(asiDownlinkChannelId);

    MEDIA_INFO_LOG("asiDownlinkChannelId: 0x%x", asiDownlinkChannelId);

    g_streamOutDownink->AttachFrontend(asiDownlinkChannelId);
    return ret;
}

static int32_t BluetoothVoiceCallVolteStop(void)
{
    int32_t ret = g_streamInUplink->Stop();
    if (ret != 0) {
        MEDIA_ERR_LOG("audio stream in uplink stop failed");
    }

    if (g_streamOutUplink->Stop() != 0) {
        MEDIA_ERR_LOG("audio stream out uplink stop failed");
    }

    if (g_streamInDwnink->Stop() != 0) {
        MEDIA_ERR_LOG("audio stream in downink stop failed");
    }

    ret = g_streamOutDownink->Stop();
    if (ret != 0) {
        MEDIA_ERR_LOG("audio stream out downink stop failed");
    }

    ret = g_streamInUplink->Release();
    if (ret != 0) {
        MEDIA_ERR_LOG("audio stream in uplink release failed");
    }

    ret = g_streamOutUplink->Release();
    if (ret != 0) {
        MEDIA_ERR_LOG("audio stream out uplink release failed");
    }

    ret = g_streamInDwnink->Release();
    if (ret != 0) {
        MEDIA_ERR_LOG("audio stream in downink release failed");
    }

    ret = g_streamOutDownink->Release();
    if (ret != 0) {
        MEDIA_ERR_LOG("audio stream out downink release failed");
    }

    ret = g_amInstance.SetDeviceConnectionState(IN_BLUETOOTH_SCO, AUDIO_DEVICE_DISCONNECTED);
    if (ret != 0) {
        MEDIA_ERR_LOG("set sco in device disconnect failed");
    }

    ret = g_amInstance.SetDeviceConnectionState(OUT_BLUETOOTH_SCO, AUDIO_DEVICE_DISCONNECTED);
    if (ret != 0) {
        MEDIA_ERR_LOG("set sco out device disconnect failed");
    }

    ret = g_amInstance.SetDeviceConnectionState(IN_MODEM_HEADSET, AUDIO_DEVICE_DISCONNECTED);
    if (ret != 0) {
        MEDIA_ERR_LOG("set modem headset in device disconnect failed");
    }

    ret = g_amInstance.SetDeviceConnectionState(OUT_MODEM_HEADSET, AUDIO_DEVICE_DISCONNECTED);
    if (ret != 0) {
        MEDIA_ERR_LOG("set modem headset out device disconnect failed");
    }
    return ret;
}
#endif
static int32_t RunLocalVoiceCallVolte(VoiceCallVolteContext *context)
{
    bool success = InitVoiceCallVolteResources(context);
    if (!success) {
        MEDIA_ERR_LOG("init voice call volte resources failed");
        return -1;
    }
    // create downlink AudioStreamIn
    CapturerInputConfig captureConfig = {};
    captureConfig.audioFormat = mSBC;
    captureConfig.channelCount = 1;
    captureConfig.linkDir = AUDIO_UP_LINK;
    captureConfig.sampleRate = SAMPLERATE_16000;
    captureConfig.sessionID = context->sessionId;
    captureConfig.streamType = context->streamType;
    int32_t ret = AudioStreamInInit(captureConfig);
    if (ret != 0) {
        MEDIA_ERR_LOG("audio stream in init failed");
        return ret;
    }
    // create uplink AudioStreamOut
    AudioRendererConfig renderConfig = {};
    renderConfig.audioFormat = mSBC;
    renderConfig.channelCount = 1;
    renderConfig.linkDir = AUDIO_DOWN_LINK;
    renderConfig.sampleRate = SAMPLERATE_16000;
    renderConfig.sessionID = context->sessionId;
    renderConfig.streamType = context->streamType;
    ret = AudioStreamOutInit(renderConfig);
    if (ret != 0) {
        MEDIA_ERR_LOG("audio stream out init failed");
        return ret;
    }
    ret = AudioStreamInStart();
    if (ret != 0) {
        MEDIA_ERR_LOG("audio stream in start failed");
        return ret;
    }
    ret = AudioStreamOutPlay();
    if (ret != 0) {
        MEDIA_ERR_LOG("audio stream out play failed");
        return ret;
    }
    while (true) {
        MediaMutexLock(context->mutex);
        if (context->isVoiceCallVolteStopped) {
            MediaMutexUnLock(context->mutex);
            break;
        }
        MediaMutexUnLock(context->mutex);
        context->size = 0x800;
        ret = AudioStreamInObtainBuffer(context->data, &context->size);
        if (ret != 0) {
            usleep(10000);
            continue;
        }
        while (true) {
            ret = AudioStreamOutStreamWrite(context->data, context->size);
            if (ret != 0) {
                MEDIA_ERR_LOG("audio stream out send data failed");
                usleep(10000);
                continue;
            }
            break;
        }
    }
    ret = LocalVoiceCallVolteStop();
    DeinitVoiceCallVolteResources(context);
    return ret;
}

static int32_t RunVoiceCallVolte(VoiceCallVolteContext *context)
{
    bool success = InitVoiceCallVolteResources(context);
    if (!success) {
        MEDIA_ERR_LOG("init voice call volte resources failed");
        return -1;
    }
    int32_t ret = 0;
#ifdef LOCAL_VOICE_CALL
    ret = LocalVoiceCallVolteStart(context);
    if (ret != 0) {
        return ret;
    }
#else
    ret = g_amInstance.SetDeviceConnectionState(IN_BLUETOOTH_SCO, AUDIO_DEVICE_CONNECTED);
    if (ret != 0) {
        MEDIA_ERR_LOG("set sco in device connect failed");
    }
    ret = g_amInstance.SetDeviceConnectionState(OUT_BLUETOOTH_SCO, AUDIO_DEVICE_CONNECTED);
    if (ret != 0) {
        MEDIA_ERR_LOG("set sco out device connect failed");
    }
    ret = g_amInstance.SetDeviceConnectionState(IN_MODEM_HEADSET, AUDIO_DEVICE_CONNECTED);
    if (ret != 0) {
        MEDIA_ERR_LOG("set modem headset in device connect failed");
    }
    ret = g_amInstance.SetDeviceConnectionState(OUT_MODEM_HEADSET, AUDIO_DEVICE_CONNECTED);
    if (ret != 0) {
        MEDIA_ERR_LOG("set modem headset out device connect failed");
    }
    ret = BluetoothVoiceCallVolteInit(context);
    if (ret != 0) {
        return ret;
    }
    BluetoothVoiceCallVolteStart();
#endif
    MediaMutexLock(context->mutex);
    while (!context->isVoiceCallVolteStopped) {
        MediaThreadCondWait(context->cond, context->mutex);
    }
    MediaMutexUnLock(context->mutex);

#ifdef LOCAL_VOICE_CALL
    LocalVoiceCallVolteStop();
#else
    BluetoothVoiceCallVolteStop();
#endif
    DeinitVoiceCallVolteResources(context);
    return 0;
}

static bool RequsetAudioFocus(VoiceCallVolteContext *context)
{
    bool success;
#ifdef LOCAL_VOICE_CALL
    success = AudioManagerInit();
#else
    success = g_amInstance.Initialize();
#endif
    if (!success) {
        MEDIA_ERR_LOG("audiomanager init failed");
        return false;
    }

    AudioSession sessionId;
#ifdef LOCAL_VOICE_CALL
    sessionId = AudioManagerMakeSessionId();
#else
    sessionId = g_amInstance.MakeSessionId();
#endif
    if (sessionId == AUDIO_SESSION_ID_NONE) {
        MEDIA_ERR_LOG("audio session id invalid");
        return false;
    }

#ifdef LOCAL_VOICE_CALL
    if (AudioManagerActivateInterrupt(context->streamType, sessionId) == INTERRUPT_FAILED) {
        MEDIA_ERR_LOG("activate audio interrupt failed");
        return false;
    }
#else
    std::shared_ptr<VoiceCallVolteInterruptListener> interruptListener =
        std::make_shared<VoiceCallVolteInterruptListener>(context);
    if (interruptListener == nullptr || interruptListener.get() == nullptr) {
        MEDIA_ERR_LOG("voice call volte interrupt listener is nullptr");
        return false;
    }

    AudioInterrupt interrupt = { context->streamType, sessionId, interruptListener };
    if (g_amInstance.ActivateAudioInterrupt(interrupt) == INTERRUPT_FAILED) {
        MEDIA_ERR_LOG("activate audio interrupt failed");
        return false;
    }
#endif

    context->sessionId = sessionId;
#ifndef LOCAL_VOICE_CALL
    context->interrupt = interrupt;
#endif

    return true;
}

#ifdef LOCAL_VOICE_CALL
static bool ReleaseAudioFocus(void)
{
    if (AudioManagerDeactivateInterrupt() != 0) {
        MEDIA_ERR_LOG("deactivate audio interrupt failed");
        return false;
    }
    return true;
}
#else
static bool ReleaseAudioFocus(VoiceCallVolteContext *context)
{
    if (g_amInstance.DeactivateAudioInterrupt(context->interrupt) != 0) {
        MEDIA_ERR_LOG("deactivate audio interrupt failed");
        return false;
    }
    return true;
}
#endif

static void *LocalVoiceCallVolteThread(void *priv)
{
    VoiceCallVolteContext *context = (VoiceCallVolteContext *)priv;
    bool success = RequsetAudioFocus(context);
    if (!success) {
        MEDIA_ERR_LOG("requset audio focus failed");
        return nullptr;
    }
    int32_t ret = RunLocalVoiceCallVolte(context);
    if (ret != 0) {
        MEDIA_ERR_LOG("run voice call volte failed");
        (void)ReleaseAudioFocus();
        return nullptr;
    }
    success = ReleaseAudioFocus();
    if (!success) {
        MEDIA_ERR_LOG("release audio focus failed");
        return nullptr;
    }
    return nullptr;
}

static void *VoiceCallVolteThread(void *priv)
{
    VoiceCallVolteContext *context = (VoiceCallVolteContext *)priv;

    bool success = RequsetAudioFocus(context);
    if (!success) {
        MEDIA_ERR_LOG("requset audio focus failed");
        return nullptr;
    }

    int32_t ret = RunVoiceCallVolte(context);
    if (ret != 0) {
        MEDIA_ERR_LOG("run voice call volte failed");
#ifdef LOCAL_VOICE_CALL
        (void)ReleaseAudioFocus();
#else
        (void)ReleaseAudioFocus(context);
#endif
        return nullptr;
    }

#ifdef LOCAL_VOICE_CALL
    success = ReleaseAudioFocus();
#else
    success = ReleaseAudioFocus(context);
#endif
    if (!success) {
        MEDIA_ERR_LOG("release audio focus failed");
        return nullptr;
    }

    return nullptr;
}

static int32_t StartVoiceCall(int32_t argc, const char **argv)
{
    MediaInitStaticMutexLock(&g_voiceCallVolteContextMutex);
    MediaMutexLock(g_voiceCallVolteContextMutex);
    if (g_voiceCallVolteContext != nullptr) {
        MEDIA_ERR_LOG("invalid operation");
        goto EXIT1;
    }
    g_voiceCallVolteContext = (VoiceCallVolteContext *)malloc(sizeof(VoiceCallVolteContext));
    if (g_voiceCallVolteContext == nullptr) {
        MEDIA_ERR_LOG("malloc failed");
        goto EXIT1;
    }
    if (memset_s(g_voiceCallVolteContext, sizeof(VoiceCallVolteContext), 0, sizeof(VoiceCallVolteContext)) != EOK) {
        MEDIA_ERR_LOG("memset_s failed");
        goto EXIT2;
    }
    if (argc <= 0x1) {
        MEDIA_ERR_LOG("The number of input parameters is insufficient. num = %d", argc);
        goto EXIT2;
    }
    g_voiceCallVolteContext->streamType = (AudioStreamType)strtol(argv[0x1], nullptr, HEX_BASE);
    MediaMutexUnLock(g_voiceCallVolteContextMutex);
    if (g_voiceCallVolteContext->streamType == AUDIO_STREAM_VOICE_CALL_VOLTE) {
        MediaThreadattr attr = { "VoiceCallVolteThread", 0x2000, THREAD_SCHED_INVALID, 0, false };
        MediaThreadIdHandle taskHandle = MediaThreadCreate(VoiceCallVolteThread, g_voiceCallVolteContext, &attr);
        if (taskHandle == nullptr) {
            MEDIA_ERR_LOG("create voice call volte  thread failed");
            goto EXIT2;
        }
        g_voiceCallVolteContext->taskHandle = taskHandle;
    } else if (g_voiceCallVolteContext->streamType == AUDIO_STREAM_VOICE_CALL_VOLTE_SPI) {
        g_voiceCallVolteContext->data = (uint8_t *)malloc(0x400);
        g_voiceCallVolteContext->size = 0x400;
        MediaThreadattr attr = { "LocalVoiceCallVolteThread", 0x2000, THREAD_SCHED_INVALID, 0, false };
        MediaThreadIdHandle taskHandle = MediaThreadCreate(LocalVoiceCallVolteThread, g_voiceCallVolteContext, &attr);
        if (taskHandle == nullptr) {
            MEDIA_ERR_LOG("create voice call volte  thread failed");
            goto EXIT2;
        }
        g_voiceCallVolteContext->taskHandle = taskHandle;
    }
    return 0;
EXIT2:
    free(g_voiceCallVolteContext);
    g_voiceCallVolteContext = nullptr;
EXIT1:
    MediaMutexUnLock(g_voiceCallVolteContextMutex);
    return -1;
}

static int32_t StopVoiceCall(int32_t argc, const char **argv)
{
    if (argc <= 0x1) {
        MEDIA_ERR_LOG("The number of input parameters is insufficient. num = %d", argc);
        return MEDIA_ERR;
    }
    MediaMutexLock(g_voiceCallVolteContextMutex);
    if (g_voiceCallVolteContext == nullptr) {
        MEDIA_ERR_LOG("voice call volte context is null, execute start command first");
        MediaMutexUnLock(g_voiceCallVolteContextMutex);
        return MEDIA_ERR;
    }
    if (g_voiceCallVolteContext->isVoiceCallVolteStopped) {
        MEDIA_ERR_LOG("voice call volte already stopped");
        MediaMutexUnLock(g_voiceCallVolteContextMutex);
        return MEDIA_OK;
    }
    AudioStreamType streamType = (AudioStreamType)strtol(argv[0x1], nullptr, HEX_BASE);
    if (g_voiceCallVolteContext->streamType != streamType) {
        MEDIA_ERR_LOG("Unable to find an audio stream for the current stream type!");
        MediaMutexUnLock(g_voiceCallVolteContextMutex);
        return MEDIA_ERR;
    }
    MediaMutexLock(g_voiceCallVolteContext->mutex);
    g_voiceCallVolteContext->isVoiceCallVolteStopped = true;
    MediaThreadCondSignal(g_voiceCallVolteContext->cond);
    MediaMutexUnLock(g_voiceCallVolteContext->mutex);
    MediaThreadJoin(&g_voiceCallVolteContext->taskHandle);
    (void)MediaThreadCondDestroy(&g_voiceCallVolteContext->cond);
    (void)MediaMutexDestroy(&g_voiceCallVolteContext->mutex);

    if (g_voiceCallVolteContext != nullptr) {
        free(g_voiceCallVolteContext);
        g_voiceCallVolteContext = nullptr;
    }
    MediaMutexUnLock(g_voiceCallVolteContextMutex);
    return MEDIA_OK;
}

static int32_t VoiceCAllSeaEnable(int32_t argc, const char **argv)
{
    if (g_voiceCallVolteContext->streamType != AUDIO_STREAM_VOICE_CALL_VOLTE) {
        MEDIA_ERR_LOG("not support set sae enable!");
        return -1;
    }
    if (argc <= 0x1) {
        MEDIA_ERR_LOG("The number of input parameters is insufficient. num = %d", argc);
        return MEDIA_ERR;
    }
    int32_t enable = strtol(argv[0x1], nullptr, NUMBER_BASE);
    int32_t ret = 0;
    if (enable == 0) {
        ret = AudioManagerSetSeaEnable(false);
    } else {
        ret = AudioManagerSetSeaEnable(true);
    }
    if (ret != 0) {
        MEDIA_ERR_LOG("set sae enable failed");
        return -1;
    }
    return 0;
}

static int32_t VoiceCallMicMute(int32_t argc, const char **argv)
{
    if (g_voiceCallVolteContext->streamType != AUDIO_STREAM_VOICE_CALL_VOLTE) {
        MEDIA_ERR_LOG("not support set mic mute!");
        return -1;
    }
    if (argc <= 0x1) {
        MEDIA_ERR_LOG("The number of input parameters is insufficient. num = %d", argc);
        return MEDIA_ERR;
    }
    AudioLinkDirection direction = (AudioLinkDirection)strtoul(argv[0x2], NULL, 0xA);
    int32_t mute = strtol(argv[0x1], nullptr, NUMBER_BASE);
    int32_t ret = 0;
    if (mute == 0) {
        ret = AudioStreamOutSetMute(false);
    } else {
        ret = AudioStreamOutSetMute(true);
    }
    if (ret != 0) {
        MEDIA_ERR_LOG("set mic mute failed");
        return -1;
    }
    return 0;
}

static int32_t VoiceCallSetBitRate(int32_t argc, const char **argv)
{
    if (g_voiceCallVolteContext->streamType != AUDIO_STREAM_VOICE_CALL_VOLTE) {
        MEDIA_ERR_LOG("not support Set Bit Rate!");
        return -1;
    }
    if (argc <= 0x1) {
        MEDIA_ERR_LOG("The number of input parameters is insufficient. num = %d", argc);
        return MEDIA_ERR;
    }
    uint32_t bitRate = (uint32_t)strtol(argv[0x1], nullptr, NUMBER_BASE);
    int32_t ret = AudioStreamInSetBitRate(bitRate);
    if (ret != 0) {
        MEDIA_ERR_LOG("SetBitRate:%d failed", bitRate);
        return -1;
    }
    MEDIA_ERR_LOG("SetBitRate:%d success", bitRate);
    return 0;
}

static int32_t VoiceCallSetVolume(int32_t argc, const char **argv)
{
    if (argc <= 0x2) {
        MEDIA_ERR_LOG("The number of input parameters is insufficient. num = %d", argc);
        return MEDIA_ERR;
    }
    AudioStreamType streamType = (AudioStreamType)strtoul(argv[0x1], NULL, HEX_BASE);
    int32_t linearVolume = (int32_t)strtol(argv[0x2], nullptr, NUMBER_BASE);
    int32_t ret = AudioManagerSetVolume(streamType, linearVolume);
    if (ret != 0) {
        MEDIA_ERR_LOG("SetVolume:%d failed", linearVolume);
        return -1;
    }
    MEDIA_ERR_LOG("SetVolume:%d success", linearVolume);
    return 0;
}

static int32_t VoiceCallGetVolume(int32_t argc, const char **argv)
{
    if (argc <= 0x1) {
        MEDIA_ERR_LOG("The number of input parameters is insufficient. num = %d", argc);
        return MEDIA_ERR;
    }
    AudioStreamType streamType = (AudioStreamType)strtoul(argv[0x1], NULL, HEX_BASE);
    int32_t ret = AudioManagerGetVolume(streamType);
    MEDIA_ERR_LOG("GetVolume:%d", ret);
    return 0;
}

int32_t VoiceCallVolteSample(int32_t argc, const char *argv[])
{
    MEDIA_INFO_LOG("enter");

    MEDIA_INFO_LOG("input param:%s", argv[0]);

    int32_t ret = -1;
    for (uint32_t i = 0; i < ARRAY_SIZE(g_voiceCallVolteFuncs); i++) {
        if (strcmp(g_voiceCallVolteFuncs[i].cmd, argv[0]) == 0) {
            ret = g_voiceCallVolteFuncs[i].func(argc, argv);
        } else {
            continue;
        }
        MEDIA_INFO_LOG("%s execute %s", g_voiceCallVolteFuncs[i].cmd, (ret != 0) ? "failed" : "success");
    }

    // will free g_voiceCallVolteContext when voice call volte exit
    MEDIA_INFO_LOG("exit");
    return 0;
}

#ifdef __cplusplus
};
#endif

/*
 * Copyright (c) CompanyNameMagicTag. 2022-2022. All rights reserved.
 * Description: audio scene interaction header file
 * Author: Media Software Group
 * Create: 2022-08-09
 */

#include "audio_scene_interaction.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "securec.h"

#include "source.h"
#include "player.h"
#include "format.h"
#include "audio_manager.h"
#include "media_log.h"
#include "media_thread_adapt.h"

#ifdef __cplusplus
extern "C" {
#endif

using OHOS::Media::Player;
using OHOS::Media::PlayerSeekMode;
using OHOS::Media::Source;
using OHOS::Media::Format;
using OHOS::Media::StreamSource;
using OHOS::Media::StreamCallback;
using namespace OHOS::Media;
using namespace::Audio;
using Audio::AudioManager;

class AlarmClockInterruptListener;
class RingInterruptListener;

static const int32_t SUCCESS = 0;
static const int32_t FAILURE = (-1);
static const int32_t WAITE_ALARM_CLOCK_COMING_S = 10;
static const int32_t WAITE_RING_COMING_S = 10;

static AudioManager& g_amInstance = AudioManager::GetInstance();

typedef struct {
    MediaMutexHandle mutex;
    MediaThreadCondHandle cond;
    AudioInterrupt interrupt;
    AudioSession sessionId;
    bool playbackComplete;
    bool interruptHintPause;
    bool interruptHintResume;
    bool interruptHintStop;
    std::shared_ptr<Player> player;
    std::shared_ptr<PlayerCallback> callback;
} AudioSceneInteractionCommonContext;

typedef struct {
    AudioSceneInteractionCommonContext common;
    std::shared_ptr<AlarmClockInterruptListener> alarmClockInterruptListener;
} AlarmClockContext;

typedef struct {
    AudioSceneInteractionCommonContext common;
    std::shared_ptr<RingInterruptListener> ringInterruptListener;
} RingContext;

class AlarmClockInterruptListener : public InterruptListener {
public:
    AlarmClockInterruptListener(AlarmClockContext *in)
    {
        context = in;
    }

    ~AlarmClockInterruptListener() override {}

    void OnInterrupt(int32_t type, int32_t hint) override
    {
        MEDIA_INFO_LOG("AlarmClock OnInterrupt, type:%d, hint:%d", type, hint);

        if (type == INTERRUPT_TYPE_BEGIN && hint == INTERRUPT_HINT_PAUSE) {
            MEDIA_INFO_LOG("AlarmClock OnInterrupt player pause signal");
            MediaMutexLock(context->common.mutex);
            context->common.interruptHintPause = true;
            MediaThreadCondSignal(context->common.cond);
            MediaMutexUnLock(context->common.mutex);
        } else if (type == INTERRUPT_TYPE_END && hint == INTERRUPT_HINT_RESUME) {
            MEDIA_INFO_LOG("AlarmClock OnInterrupt player resume signal");
            MediaMutexLock(context->common.mutex);
            context->common.interruptHintResume = true;
            MediaThreadCondSignal(context->common.cond);
            MediaMutexUnLock(context->common.mutex);
        } else if (type == INTERRUPT_TYPE_BEGIN && hint == INTERRUPT_HINT_STOP) {
            MEDIA_INFO_LOG("AlarmClock OnInterrupt player stop signal");
            MediaMutexLock(context->common.mutex);
            context->common.interruptHintStop = true;
            MediaThreadCondSignal(context->common.cond);
            MediaMutexUnLock(context->common.mutex);
        } else {
            MEDIA_ERR_LOG("unknow type:%d, hint:%d", type, hint);
        }
    }

private:
    AlarmClockContext *context;
};

class AlarmClockPlayerCallbackImpl : public PlayerCallback {
public:
    AlarmClockPlayerCallbackImpl(AlarmClockContext *in)
    {
        context = in;
    }

    ~AlarmClockPlayerCallbackImpl() override {}

    void OnPlaybackComplete() override
    {
        MEDIA_INFO_LOG("AlarmClock playback complete callback signal");
        MediaMutexLock(context->common.mutex);
        context->common.playbackComplete = true;
        MediaThreadCondSignal(context->common.cond);
        MediaMutexUnLock(context->common.mutex);
    }

    void OnError(int32_t errorType, int32_t errorCode) override
    {
        MEDIA_INFO_LOG("errorType:%d errorCode:%d", errorType, errorCode);
    }

    void OnInfo(int32_t type, int32_t extra) override
    {
        MEDIA_INFO_LOG("type:%d extra:%d", type, extra);
    }

    void OnRewindToComplete() override
    {
        MEDIA_INFO_LOG("rewind complete");
    }

private:
    AlarmClockContext *context;
};

class RingInterruptListener : public InterruptListener {
public:
    RingInterruptListener(RingContext *in)
    {
        context = in;
    }

    ~RingInterruptListener() override {}

    void OnInterrupt(int32_t type, int32_t hint) override
    {
        MEDIA_INFO_LOG("Ring OnInterrupt, type:%d, hint:%d", type, hint);

        if (type == INTERRUPT_TYPE_BEGIN && hint == INTERRUPT_HINT_PAUSE) {
            MEDIA_INFO_LOG("Ring OnInterrupt player pause signal");
            MediaMutexLock(context->common.mutex);
            context->common.interruptHintPause = true;
            MediaThreadCondSignal(context->common.cond);
            MediaMutexUnLock(context->common.mutex);
        } else if (type == INTERRUPT_TYPE_END && hint == INTERRUPT_HINT_RESUME) {
            MEDIA_INFO_LOG("Ring OnInterrupt player resume signal");
            MediaMutexLock(context->common.mutex);
            context->common.interruptHintResume = true;
            MediaThreadCondSignal(context->common.cond);
            MediaMutexUnLock(context->common.mutex);
        } else if (type == INTERRUPT_TYPE_BEGIN && hint == INTERRUPT_HINT_STOP) {
            MEDIA_INFO_LOG("Ring OnInterrupt player stop signal");
            MediaMutexLock(context->common.mutex);
            context->common.interruptHintStop = true;
            MediaThreadCondSignal(context->common.cond);
            MediaMutexUnLock(context->common.mutex);
        } else {
            MEDIA_ERR_LOG("unknow type:%d, hint:%d", type, hint);
        }
    }

private:
    RingContext *context;
};

class RingPlayerCallbackImpl : public PlayerCallback {
public:
    RingPlayerCallbackImpl(RingContext *in)
    {
        context = in;
    }

    ~RingPlayerCallbackImpl() override {}

    void OnPlaybackComplete() override
    {
        MEDIA_INFO_LOG("Ring playback complete callback signal");
        MediaMutexLock(context->common.mutex);
        context->common.playbackComplete = true;
        MediaThreadCondSignal(context->common.cond);
        MediaMutexUnLock(context->common.mutex);
    }

    void OnError(int32_t errorType, int32_t errorCode) override
    {
        MEDIA_INFO_LOG("errorType:%d, errorCode:%d", errorType, errorCode);
    }

    void OnInfo(int32_t type, int32_t extra) override
    {
        MEDIA_INFO_LOG("type: %d extra: %d", type, extra);
    }

    void OnRewindToComplete() override
    {
        MEDIA_INFO_LOG("rewind complete");
    }

private:
    RingContext *context;
};

static int32_t ActivateAudioInterruptWrapper(void *context, AudioStreamType streamType)
{
    if (!g_amInstance.Initialize()) {
        MEDIA_ERR_LOG("audiomanager init failed");
        return FAILURE;
    }

    if (streamType == AUDIO_STREAM_ALARM_CLOCK) {
        AlarmClockContext *alarmClockContext = (AlarmClockContext *)context;
        alarmClockContext->common.sessionId = g_amInstance.MakeSessionId();
        if (alarmClockContext->common.sessionId == AUDIO_SESSION_ID_NONE) {
            MEDIA_ERR_LOG("session id invalid");
            return FAILURE;
        }

        alarmClockContext->alarmClockInterruptListener =
            std::make_shared<AlarmClockInterruptListener>(alarmClockContext);
        alarmClockContext->common.interrupt = { streamType, alarmClockContext->common.sessionId,
            alarmClockContext->alarmClockInterruptListener };
        if (g_amInstance.ActivateAudioInterrupt(alarmClockContext->common.interrupt) == INTERRUPT_FAILED) {
            MEDIA_ERR_LOG("ActivateAudioInterrupt failed");
            return FAILURE;
        }
    } else {
        RingContext *ringContext = (RingContext *)context;
        ringContext->common.sessionId = g_amInstance.MakeSessionId();
        if (ringContext->common.sessionId == AUDIO_SESSION_ID_NONE) {
            MEDIA_ERR_LOG("session id invalid");
            return FAILURE;
        }

        ringContext->ringInterruptListener =
            std::make_shared<RingInterruptListener>(ringContext);
        ringContext->common.interrupt = { streamType, ringContext->common.sessionId,
            ringContext->ringInterruptListener };

        if (g_amInstance.ActivateAudioInterrupt(ringContext->common.interrupt) == INTERRUPT_FAILED) {
            MEDIA_ERR_LOG("ActivateAudioInterrupt failed");
            return FAILURE;
        }
    }
    return SUCCESS;
}

static int32_t MediaMutexCondCreateWrapper(void *context, AudioStreamType streamType)
{
    if (streamType == AUDIO_STREAM_ALARM_CLOCK) {
        AlarmClockContext *alarmClockContext = (AlarmClockContext *)context;
        alarmClockContext->common.mutex = MediaMutexCreate(nullptr);
        if (alarmClockContext->common.mutex == nullptr) {
            MEDIA_ERR_LOG("thread mutex nullptr");
            return FAILURE;
        }

        alarmClockContext->common.cond = MediaThreadCondCreate();
        if (alarmClockContext->common.cond == nullptr) {
            MEDIA_ERR_LOG("thread cond nullptr");
            (void)MediaMutexDestroy(&alarmClockContext->common.mutex);
            return FAILURE;
        }
    } else {
        RingContext *ringContext = (RingContext *)context;
        ringContext->common.mutex = MediaMutexCreate(nullptr);
        if (ringContext->common.mutex == nullptr) {
            MEDIA_ERR_LOG("thread mutex nullptr");
            return FAILURE;
        }

        ringContext->common.cond = MediaThreadCondCreate();
        if (ringContext->common.cond == nullptr) {
            MEDIA_ERR_LOG("thread cond nullptr");
            (void)MediaMutexDestroy(&ringContext->common.mutex);
            return FAILURE;
        }
    }

    return SUCCESS;
}

static int32_t PlayerWrapper(void *context, AudioStreamType streamType)
{
    if (streamType == AUDIO_STREAM_ALARM_CLOCK) {
        AlarmClockContext *alarmClockContext = (AlarmClockContext *)context;
        alarmClockContext->common.player = std::make_shared<Player>();
        if (alarmClockContext->common.player == nullptr || alarmClockContext->common.player.get() == nullptr) {
            MEDIA_ERR_LOG("player nullptr");
            return FAILURE;
        }

        std::string uri("user/test/cai.mp3");
        std::map<std::string, std::string> header;
        Source source(uri, header);
        (void)alarmClockContext->common.player->SetSource(source);
        (void)alarmClockContext->common.player->Prepare();

        alarmClockContext->common.callback = std::make_shared<AlarmClockPlayerCallbackImpl>(alarmClockContext);
        if (alarmClockContext->common.callback == nullptr || alarmClockContext->common.callback.get() == nullptr) {
            MEDIA_ERR_LOG("player callback nullptr");
        }

        (void)alarmClockContext->common.player->SetPlayerCallback(alarmClockContext->common.callback);
        (void)alarmClockContext->common.player->SetAudioSessionId(alarmClockContext->common.sessionId);
        (void)alarmClockContext->common.player->SetAudioStreamType(AUDIO_STREAM_ALARM_CLOCK);
    } else {
        RingContext *ringContext = (RingContext *)context;
        ringContext->common.player = std::make_shared<Player>();
        if (ringContext->common.player == nullptr || ringContext->common.player.get() == nullptr) {
            MEDIA_ERR_LOG("player nullptr");
            return FAILURE;
        }

        std::string uri("user/test/guyongzhe_8k_2ch_8k_0x20000.mp3");
        std::map<std::string, std::string> header;
        Source source(uri, header);

        (void)ringContext->common.player->SetSource(source);
        (void)ringContext->common.player->Prepare();

        ringContext->common.callback = std::make_shared<RingPlayerCallbackImpl>(ringContext);
        if (ringContext->common.callback == nullptr || ringContext->common.callback.get() == nullptr) {
            MEDIA_ERR_LOG("player callback nullptr");
        }

        (void)ringContext->common.player->SetPlayerCallback(ringContext->common.callback);
        (void)ringContext->common.player->SetAudioSessionId(ringContext->common.sessionId);
        (void)ringContext->common.player->SetAudioStreamType(AUDIO_STREAM_RING);
    }

    return SUCCESS;
}

static void ProcessInterrupt(AudioSceneInteractionCommonContext *common)
{
    if (common->interruptHintPause) {
        (void)common->player->Pause();
        common->interruptHintPause = false;
    }
    if (common->interruptHintResume) {
        (void)common->player->Play();
        common->interruptHintResume = false;
    }
    if (common->interruptHintStop) {
        (void)common->player->Stop();
        (void)common->player->Reset();
        (void)common->player->Release();
    }
}

void *AudioSceneInteraction001Thread(void *priv)
{
    (void)(priv);
    int32_t ret = FAILURE;

    AlarmClockContext *context = (AlarmClockContext *)malloc(sizeof(AlarmClockContext));
    if (context == nullptr) {
        MEDIA_ERR_LOG("malloc failed");
        return nullptr;
    }

    if (memset_s(context, sizeof(AlarmClockContext), 0, sizeof(AlarmClockContext)) != EOK) {
        MEDIA_ERR_LOG("memset_s failed");
    }

    ret = ActivateAudioInterruptWrapper((void *)context, AUDIO_STREAM_ALARM_CLOCK);
    if (ret != SUCCESS) {
        free(context);
        return nullptr;
    }

    free(context);

    MEDIA_ERR_LOG("hould not reach here, activate AUDIO_STREAM_ALARM_CLOCK interrupt expected failed");

    return nullptr;
}

static void DeactivateAudioInterruptWrapper(void *context, AudioStreamType streamType)
{
    if (streamType == AUDIO_STREAM_ALARM_CLOCK) {
        AlarmClockContext *alarmClockContext = (AlarmClockContext *)context;
        if (g_amInstance.DeactivateAudioInterrupt(alarmClockContext->common.interrupt) != 0) {
            MEDIA_ERR_LOG("deactivate audio interrupt failed");
        }
    } else {
        RingContext *ringContext = (RingContext *)context;
        if (g_amInstance.DeactivateAudioInterrupt(ringContext->common.interrupt) != 0) {
            MEDIA_ERR_LOG("deactivate audio interrupt failed");
        }
    }
    return;
}

static void MediaMutexCondDestroyWrapper(void *context, AudioStreamType streamType)
{
    if (streamType == AUDIO_STREAM_ALARM_CLOCK) {
        AlarmClockContext *alarmClockContext = (AlarmClockContext *)context;
        if (alarmClockContext->common.mutex != nullptr) {
            MediaMutexDestroy(&alarmClockContext->common.mutex);
            alarmClockContext->common.mutex = nullptr;
        }
        if (alarmClockContext->common.mutex != nullptr) {
            MediaThreadCondDestroy(&alarmClockContext->common.mutex);
            alarmClockContext->common.mutex = nullptr;
        }
    } else {
        RingContext *ringContext = (RingContext *)context;
        if (ringContext->common.mutex != nullptr) {
            MediaMutexDestroy(&ringContext->common.mutex);
            ringContext->common.mutex = nullptr;
        }
        if (ringContext->common.cond != nullptr) {
            MediaThreadCondDestroy(&ringContext->common.cond);
            ringContext->common.cond = nullptr;
        }
    }
    return;
}

static int32_t AudioRingContextInit(RingContext **ringContext)
{
    RingContext *context = (RingContext *)malloc(sizeof(RingContext));
    if (context == nullptr) {
        MEDIA_ERR_LOG("malloc failed");
        return FAILURE;
    }
    if (memset_s(context, sizeof(RingContext), 0, sizeof(RingContext)) != EOK) {
        MEDIA_ERR_LOG("memset_s failed");
    }

    int32_t ret = ActivateAudioInterruptWrapper((void *)context, AUDIO_STREAM_RING);
    if (ret != SUCCESS) {
        free(context);
        MEDIA_ERR_LOG("ActivateAudioInterruptWrapper failed");
        return FAILURE;
    }

    ret = MediaMutexCondCreateWrapper((void *)context, AUDIO_STREAM_RING);
    if (ret != SUCCESS) {
        (void)DeactivateAudioInterruptWrapper((void *)context, AUDIO_STREAM_RING);
        free(context);
        MEDIA_ERR_LOG("MediaMutexCondCreateWrapper failed");
        return FAILURE;
    }

    ret = PlayerWrapper((void *)context, AUDIO_STREAM_RING);
    if (ret != SUCCESS) {
        (void)MediaMutexCondDestroyWrapper((void *)context, AUDIO_STREAM_RING);
        (void)DeactivateAudioInterruptWrapper((void *)context, AUDIO_STREAM_RING);
        free(context);
        MEDIA_ERR_LOG("PlayerWrapper failed");
        return FAILURE;
    }
    *ringContext = context;
    return SUCCESS;
}

static int32_t AudioAlarmClockContextInit(AlarmClockContext **alarmClockContext)
{
    AlarmClockContext *context = (AlarmClockContext *)malloc(sizeof(AlarmClockContext));
    if (context == nullptr) {
        MEDIA_ERR_LOG("malloc failed");
        return FAILURE;
    }

    if (memset_s(context, sizeof(AlarmClockContext), 0, sizeof(AlarmClockContext)) != EOK) {
        MEDIA_ERR_LOG("memset_s failed");
    }

    int32_t ret = ActivateAudioInterruptWrapper((void *)context, AUDIO_STREAM_ALARM_CLOCK);
    if (ret != SUCCESS) {
        free(context);
        MEDIA_ERR_LOG("ActivateAudioInterruptWrapper failed");
        return FAILURE;
    }

    ret = MediaMutexCondCreateWrapper((void *)context, AUDIO_STREAM_ALARM_CLOCK);
    if (ret != SUCCESS) {
        (void)DeactivateAudioInterruptWrapper((void *)context, AUDIO_STREAM_ALARM_CLOCK);
        free(context);
        MEDIA_ERR_LOG("MediaMutexCondCreateWrapper failed");
        return FAILURE;
    }

    ret = PlayerWrapper((void *)context, AUDIO_STREAM_ALARM_CLOCK);
    if (ret != SUCCESS) {
        (void)MediaMutexCondDestroyWrapper((void *)context, AUDIO_STREAM_ALARM_CLOCK);
        (void)DeactivateAudioInterruptWrapper((void *)context, AUDIO_STREAM_ALARM_CLOCK);
        free(context);
        MEDIA_ERR_LOG("PlayerWrapper failed");
        return FAILURE;
    }
    *alarmClockContext = context;
    return SUCCESS;
}

static void AudioRingExit(RingContext *ringContext)
{
    if (g_amInstance.DeactivateAudioInterrupt(ringContext->common.interrupt) != 0) {
        MEDIA_ERR_LOG("deactivate audio interrupt failed");
    }
    MediaMutexDestroy(&ringContext->common.mutex);
    MediaThreadCondDestroy(&ringContext->common.cond);
    free(ringContext);
}

static void AudioAlarmClockExit(AlarmClockContext *alarmClockContext)
{
    if (g_amInstance.DeactivateAudioInterrupt(alarmClockContext->common.interrupt) != 0) {
        MEDIA_ERR_LOG("deactivate audio interrupt failed");
    }
    MediaMutexDestroy(&alarmClockContext->common.mutex);
    MediaThreadCondDestroy(&alarmClockContext->common.cond);
    free(alarmClockContext);
}

static void HandlePlayBackCompleted(RingContext *ringContext, AlarmClockContext *alarmClockContext)
{
    MediaMutexLock(alarmClockContext->common.mutex);
    while (!alarmClockContext->common.playbackComplete && !alarmClockContext->common.interruptHintStop) {
        MediaThreadCondWait(alarmClockContext->common.cond, alarmClockContext->common.mutex);
        ProcessInterrupt(&(alarmClockContext->common));
    }
    MediaMutexUnLock(alarmClockContext->common.mutex);

    MediaMutexLock(ringContext->common.mutex);
    while (!ringContext->common.playbackComplete && !ringContext->common.interruptHintStop) {
        MediaThreadCondWait(ringContext->common.cond, ringContext->common.mutex);
        ProcessInterrupt(&(ringContext->common));
    }
    MediaMutexUnLock(ringContext->common.mutex);
    if (!ringContext->common.interruptHintStop) {
        (void)ringContext->common.player->Stop();
        (void)ringContext->common.player->Reset();
        (void)ringContext->common.player->Release();
    }

    if (!alarmClockContext->common.interruptHintStop) {
        (void)alarmClockContext->common.player->Stop();
        (void)alarmClockContext->common.player->Reset();
        (void)alarmClockContext->common.player->Release();
    }
}

void *AudioSceneInteraction002Thread(void *priv)
{
    (void)(priv);
    int32_t ret = FAILURE;
    RingContext *ringContext = nullptr;
    AlarmClockContext *alarmClockContext = nullptr;
    ret = AudioRingContextInit(&ringContext);
    if (ret != SUCCESS) {
        return nullptr;
    }
    // 1. ring play
    ret = ringContext->common.player->Play();
    if (ret != 0) {
        MEDIA_ERR_LOG("ring player play failed");
        AudioRingExit(ringContext);
        MEDIA_INFO_LOG("AudioSceneInteraction002Thread exit");
        return nullptr;
    }
    // 2. sleep 10s then alarm clock coming
    sleep(WAITE_ALARM_CLOCK_COMING_S);
    ret = AudioAlarmClockContextInit(&alarmClockContext);
    if (ret != SUCCESS) {
        AudioRingExit(ringContext);
        return nullptr;
    }
    // 3. alarm clock play
    ret = alarmClockContext->common.player->Play();
    if (ret != 0) {
        MEDIA_ERR_LOG("alarm clock player play failed");
        AudioAlarmClockExit(alarmClockContext);
        AudioRingExit(ringContext);
        MEDIA_INFO_LOG("AudioSceneInteraction002Thread exit");
        return nullptr;
    }
    // 4. wait alarm clock/ring playback completed, assume alarm clock first playback completed
    HandlePlayBackCompleted(ringContext, alarmClockContext);
    AudioAlarmClockExit(alarmClockContext);
    AudioRingExit(ringContext);
    MEDIA_INFO_LOG("AudioSceneInteraction002Thread exit");
    return nullptr;
}

void *AudioSceneInteraction003Thread(void *priv)
{
    (void)(priv);
    int32_t ret = FAILURE;
    AlarmClockContext *context = nullptr;
    ret = AudioAlarmClockContextInit(&context);
    if (ret != SUCCESS) {
        return nullptr;
    }
    // 1. alarm clock play
    ret = context->common.player->Play();
    if (ret != 0) {
        MEDIA_ERR_LOG("alarm clock player play failed");
        AudioAlarmClockExit(context);
        MEDIA_INFO_LOG("AudioSceneInteraction003Thread exit");
        return nullptr;
    }
    MediaMutexLock(context->common.mutex);
    while (!context->common.playbackComplete && !context->common.interruptHintStop) {
        MediaThreadCondWait(context->common.cond, context->common.mutex);
        ProcessInterrupt(&(context->common));
    }
    MediaMutexUnLock(context->common.mutex);

    if (!context->common.interruptHintStop) {
        (void)context->common.player->Stop();
        (void)context->common.player->Reset();
        (void)context->common.player->Release();
    }
    AudioAlarmClockExit(context);
    MEDIA_INFO_LOG("AudioSceneInteraction003Thread exit");
    return nullptr;
}

void *AudioSceneInteraction004Thread(void *priv)
{
    (void)(priv);
    int32_t ret = FAILURE;
    RingContext *ringContext = nullptr;
    AlarmClockContext *alarmClockContext = nullptr;

    ret = AudioAlarmClockContextInit(&alarmClockContext);
    if (ret != SUCCESS) {
        return nullptr;
    }
    // 1. alarm clock play
    ret = alarmClockContext->common.player->Play();
    if (ret != 0) {
        MEDIA_ERR_LOG("alarm clock player play failed");
        AudioAlarmClockExit(alarmClockContext);
        MEDIA_INFO_LOG("AudioSceneInteraction004Thread exit");
        return nullptr;
    }

    // 2. sleep 10s then ring clock coming
    sleep(WAITE_RING_COMING_S);
    ret = AudioRingContextInit(&ringContext);
    if (ret != SUCCESS) {
        AudioAlarmClockExit(alarmClockContext);
        return nullptr;
    }
    // 3. ring play
    ret = ringContext->common.player->Play();
    if (ret != 0) {
        MEDIA_ERR_LOG("ring player play failed");
        AudioAlarmClockExit(alarmClockContext);
        AudioRingExit(ringContext);
        MEDIA_INFO_LOG("AudioSceneInteraction004Thread exit");
        return nullptr;
    }

    // 4. if the alarm clock receives an stop interrupt, respond here.
    if (alarmClockContext->common.interruptHintStop) {
        (void)alarmClockContext->common.player->Stop();
        (void)alarmClockContext->common.player->Reset();
        (void)alarmClockContext->common.player->Release();
    }

    // 5. wait ring/alarm clock playback completed, ring EXCLUSIVE alarm clock
    HandlePlayBackCompleted(ringContext, alarmClockContext);
    AudioAlarmClockExit(alarmClockContext);
    AudioRingExit(ringContext);
    MEDIA_INFO_LOG("AudioSceneInteraction004Thread exit");
    return nullptr;
}

void *AudioSceneInteraction005Thread(void *priv)
{
    (void)(priv);
    int32_t ret = FAILURE;

    AlarmClockContext *alarmClockContext = nullptr;
    ret = AudioAlarmClockContextInit(&alarmClockContext);
    if (ret != SUCCESS) {
        return nullptr;
    }
    // 1. alarm clock play
    ret = alarmClockContext->common.player->Play();
    if (ret != 0) {
        MEDIA_ERR_LOG("alarm clock player play failed");
        AudioAlarmClockExit(alarmClockContext);
        MEDIA_INFO_LOG("AudioSceneInteraction005Thread exit");
        return nullptr;
    }
    MediaMutexLock(alarmClockContext->common.mutex);
    while (!alarmClockContext->common.playbackComplete && !alarmClockContext->common.interruptHintStop) {
        MediaThreadCondWait(alarmClockContext->common.cond, alarmClockContext->common.mutex);
        ProcessInterrupt(&(alarmClockContext->common));
    }
    MediaMutexUnLock(alarmClockContext->common.mutex);

    if (!alarmClockContext->common.interruptHintStop) {
        (void)alarmClockContext->common.player->Stop();
        (void)alarmClockContext->common.player->Reset();
        (void)alarmClockContext->common.player->Release();
    }
    AudioAlarmClockExit(alarmClockContext);
    MEDIA_INFO_LOG("AudioSceneInteraction005Thread exit");
    return nullptr;
}

void *AudioSceneInteraction006Thread(void *priv)
{
    (void)(priv);
    int32_t ret = FAILURE;

    RingContext *ringContext = nullptr;
    ret = AudioRingContextInit(&ringContext);
    if (ret != SUCCESS) {
        return nullptr;
    }
    ret = ringContext->common.player->Play();
    if (ret != 0) {
        MEDIA_ERR_LOG("ring player play failed");
        AudioRingExit(ringContext);
        MEDIA_INFO_LOG("AudioSceneInteraction006Thread exit");
        return nullptr;
    }

    MediaMutexLock(ringContext->common.mutex);
    while (!ringContext->common.playbackComplete && !ringContext->common.interruptHintStop) {
        MediaThreadCondWait(ringContext->common.cond, ringContext->common.mutex);
        ProcessInterrupt(&(ringContext->common));
    }
    MediaMutexUnLock(ringContext->common.mutex);

    if (!ringContext->common.interruptHintStop) {
        (void)ringContext->common.player->Stop();
        (void)ringContext->common.player->Reset();
        (void)ringContext->common.player->Release();
    }
    AudioRingExit(ringContext);
    MEDIA_INFO_LOG("AudioSceneInteraction006Thread exit");
    return nullptr;
}

/* focus: BT SCO Voice Call, incoming: Alarm Clock */
int32_t AudioSceneInteraction001(int32_t argc, const char *argv[])
{
    MEDIA_UNUSED(argc);
    MEDIA_UNUSED(argv);
    MediaThreadattr attr = { "AudioSceneInteraction001Thread", 0x2000, THREAD_SCHED_INVALID, 0, false };
    MediaThreadIdHandle taskHandle = MediaThreadCreate(AudioSceneInteraction001Thread, nullptr, &attr);
    if (taskHandle == nullptr) {
        MEDIA_ERR_LOG("create thread failed");
        return -1;
    }

    MediaThreadJoin(&taskHandle);
    MEDIA_INFO_LOG("AudioSceneInteraction001 exit");
    return 0;
}

/* focus: Ring, incoming: Alarm Clock */
int32_t AudioSceneInteraction002(int32_t argc, const char *argv[])
{
    MEDIA_UNUSED(argc);
    MEDIA_UNUSED(argv);
    MediaThreadattr attr = { "AudioSceneInteraction002Thread", 0x2000, THREAD_SCHED_INVALID, 0, true };
    MediaThreadIdHandle taskHandle = MediaThreadCreate(AudioSceneInteraction002Thread, nullptr, &attr);
    if (taskHandle == nullptr) {
        MEDIA_ERR_LOG("create thread failed");
        return -1;
    }

    MediaThreadJoin(&taskHandle);
    MEDIA_INFO_LOG("AudioSceneInteraction002 exit");
    return 0;
}

/* focus: Alarm Clock, incoming: BT SCO Voice Call */
int32_t AudioSceneInteraction003(int32_t argc, const char *argv[])
{
    MEDIA_UNUSED(argc);
    MEDIA_UNUSED(argv);
    MediaThreadattr attr = { "AudioSceneInteraction003Thread", 0x2000, THREAD_SCHED_INVALID, 0, false };
    MediaThreadIdHandle taskHandle = MediaThreadCreate(AudioSceneInteraction003Thread, nullptr, &attr);
    if (taskHandle == nullptr) {
        MEDIA_ERR_LOG("create thread failed");
        return -1;
    }

    MediaThreadJoin(&taskHandle);
    MEDIA_INFO_LOG("AudioSceneInteraction003 exit");
    return 0;
}

/* focus: Alarm Clock, incoming: Ring */
int32_t AudioSceneInteraction004(int32_t argc, const char *argv[])
{
    MEDIA_UNUSED(argc);
    MEDIA_UNUSED(argv);
    MediaThreadattr attr = { "AudioSceneInteraction004Thread", 0x2000, THREAD_SCHED_INVALID, 0, false };
    MediaThreadIdHandle taskHandle = MediaThreadCreate(AudioSceneInteraction004Thread, nullptr, &attr);
    if (taskHandle == nullptr) {
        MEDIA_ERR_LOG("create thread failed");
        return -1;
    }

    MediaThreadJoin(&taskHandle);
    MEDIA_INFO_LOG("AudioSceneInteraction004 exit");
    return 0;
}

/* focus: Alarm Clock, incoming: Music */
int32_t AudioSceneInteraction005(int32_t argc, const char *argv[])
{
    MEDIA_UNUSED(argc);
    MEDIA_UNUSED(argv);
    MediaThreadattr attr = { "AudioSceneInteraction005Thread", 0x2000, THREAD_SCHED_INVALID, 0, false };
    MediaThreadIdHandle taskHandle = MediaThreadCreate(AudioSceneInteraction005Thread, nullptr, &attr);
    if (taskHandle == nullptr) {
        MEDIA_ERR_LOG("create thread failed");
        return -1;
    }

    MEDIA_INFO_LOG("AudioSceneInteraction005 exit");
    return 0;
}

/* focus: Music, incoming: Ring */
int32_t AudioSceneInteraction006(int32_t argc, const char *argv[])
{
    MEDIA_UNUSED(argc);
    MEDIA_UNUSED(argv);
    MediaThreadattr attr = { "AudioSceneInteraction006Thread", 0x2000, THREAD_SCHED_INVALID, 0, true };
    MediaThreadIdHandle taskHandle = MediaThreadCreate(AudioSceneInteraction006Thread, nullptr, &attr);
    if (taskHandle == nullptr) {
        MEDIA_ERR_LOG("create thread failed");
        return -1;
    }

    MEDIA_INFO_LOG("AudioSceneInteraction006 exit");
    return 0;
}

/* focus: Music, incoming: Alarm Clock */
int32_t AudioSceneInteraction007(int32_t argc, const char *argv[])
{
    MEDIA_UNUSED(argc);
    MEDIA_UNUSED(argv);
    // AudioSceneInteraction005Thread: alarm clock play thread
    MediaThreadattr attr = { "AudioSceneInteraction007Thread", 0x2000, THREAD_SCHED_INVALID, 0, true };
    MediaThreadIdHandle taskHandle = MediaThreadCreate(AudioSceneInteraction005Thread, nullptr, &attr);
    if (taskHandle == nullptr) {
        MEDIA_ERR_LOG("create thread failed");
        return -1;
    }

    MEDIA_INFO_LOG("AudioSceneInteraction007 exit");
    return 0;
}

#ifdef __cplusplus
};
#endif

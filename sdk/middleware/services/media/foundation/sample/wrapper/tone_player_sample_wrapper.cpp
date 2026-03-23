/*
 * Copyright (c) CompanyNameMagicTag. 2023-2023. All rights reserved.
 * Description: tone player sample wrapper
 * Author: Media Software Group
 * Create: 2023-03-18
 */

#include "tone_player_sample_wrapper.h"
#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include "securec.h"
#ifdef MEMORY_MINI
#include "media_hal_thread_adapt.h"
#include "media_hal_common.h"
#else
#include "audio_manager.h"
#include "audio_base_type.h"
#include "media_log.h"
#include "media_thread_adapt.h"
#include "audio_manager_wrapper.h"
#endif
#include "tone_player.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef MEMORY_MINI
#define MODULE_NAME "TonePlayerSample"
#define TONE_PLAYER_SAMPLE_LOGV(fmt, ...) MEDIA_HAL_LOGV(MODULE_NAME, fmt, ##__VA_ARGS__)
#define TONE_PLAYER_SAMPLE_LOGD(fmt, ...) MEDIA_HAL_LOGD(MODULE_NAME, fmt, ##__VA_ARGS__)
#define TONE_PLAYER_SAMPLE_LOGI(fmt, ...) MEDIA_HAL_LOGI(MODULE_NAME, fmt, ##__VA_ARGS__)
#define TONE_PLAYER_SAMPLE_LOGW(fmt, ...) MEDIA_HAL_LOGW(MODULE_NAME, fmt, ##__VA_ARGS__)
#define TONE_PLAYER_SAMPLE_LOGE(fmt, ...) MEDIA_HAL_LOGE(MODULE_NAME, fmt, ##__VA_ARGS__)
#define TONE_PLAYER_SAMPLE_MUTEX_CREATE(attr) MediaHalMutexCreate(attr)
#define TONE_PLAYER_SAMPLE_MUTEX_LOCK(lock) MediaHalMutexLock(lock)
#define TONE_PLAYER_SAMPLE_MUTEX_UN_LOCK(lock) MediaHalMutexUnLock(lock)
#define TONE_PLAYER_SAMPLE_MUTEX_DESTROY(mutex) MediaHalMutexDestroy(mutex)
#define TONE_PLAYER_SAMPLE_THREAD_COND_CREATE()   MediaHalThreadCondCreate()
#define TONE_PLAYER_SAMPLE_THREAD_COND_SIGNAL(condHandle)   MediaHalThreadCondSignal(condHandle)
#define TONE_PLAYER_SAMPLE_THREAD_COND_WAIT(condHandle, mutexHandle) \
    MediaHalThreadCondWait(condHandle, mutexHandle)
#define TONE_PLAYER_SAMPLE_THREAD_COND_DESTROY(condHandle) MediaHalThreadCondDestroy(condHandle)
#define TONE_PLAYER_SAMPLE_THREAD_JOIN(thdId) MediaHalThreadJoin(thdId)
#else
#define TONE_PLAYER_SAMPLE_LOGD(fmt, ...) MEDIA_DEBUG_LOG(fmt, ##__VA_ARGS__)
#define TONE_PLAYER_SAMPLE_LOGE(fmt, ...) MEDIA_ERR_LOG(fmt, ##__VA_ARGS__)
#define TONE_PLAYER_SAMPLE_LOGW(fmt, ...) MEDIA_WARNING_LOG(fmt, ##__VA_ARGS__)
#define TONE_PLAYER_SAMPLE_LOGI(fmt, ...) MEDIA_INFO_LOG(fmt, ##__VA_ARGS__)
#define TONE_PLAYER_SAMPLE_LOGF(fmt, ...) MEDIA_FATAL_LOG(fmt, ##__VA_ARGS__)
#define TONE_PLAYER_SAMPLE_MUTEX_CREATE(attr) MediaMutexCreate(attr)
#define TONE_PLAYER_SAMPLE_MUTEX_LOCK(lock) MediaMutexLock(lock)
#define TONE_PLAYER_SAMPLE_MUTEX_UN_LOCK(lock) MediaMutexUnLock(lock)
#define TONE_PLAYER_SAMPLE_MUTEX_DESTROY(mutex) MediaMutexDestroy(mutex)
#define TONE_PLAYER_SAMPLE_THREAD_COND_CREATE()   MediaThreadCondCreate()
#define TONE_PLAYER_SAMPLE_THREAD_COND_SIGNAL(condHandle)   MediaThreadCondSignal(condHandle)
#define TONE_PLAYER_SAMPLE_THREAD_COND_WAIT(condHandle, mutexHandle)\
    MediaThreadCondWait(condHandle, mutexHandle)
#define TONE_PLAYER_SAMPLE_THREAD_COND_DESTROY(condHandle) MediaThreadCondDestroy(condHandle)
#define TONE_PLAYER_SAMPLE_THREAD_JOIN(thdId) MediaThreadJoin(thdId)
#endif

#define TONE_PLAY_UNUSED(x) ((void)(x))
#define TONE_PLAY_ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define TONE_PLAY_MEDIA_PATH_MAX 260
#define TONE_PLAY_CONTROL_CMD_MAX_LEN 20
#define TONE_PLAY_SAMPLE_RETURN_VAL_IF_NULL(condition)                                           \
    do {                                                                               \
        if ((condition) == nullptr) {                                                  \
            TONE_PLAYER_SAMPLE_LOGE("" #condition " is NULL error");    \
            return -1;                                                         \
        }                                                                              \
    } while (0)

#ifndef MEMORY_MINI
using namespace::Audio;
using Audio::AudioManager;
#endif // !MEMORY_MINI

typedef struct TagTestSample {
#ifdef MEMORY_MINI
    MediaHalMutexHandle mutex;
    MediaHalThreadCondHandle cond;
#else
    MediaMutexHandle mutex;
    MediaThreadCondHandle cond;
#endif
    int32_t cirBufRunning;
    char filePath[TONE_PLAY_MEDIA_PATH_MAX];
    bool playError;
    bool playCompleted;
    bool isStop;
    TonePlayerStates playState;
    uint64_t position;
#ifndef MEMORY_MINI
    bool interruptHintPause;
    int32_t sessionId;
    bool interruptHintResume;
    bool interruptHintStop;
#endif
} TonePlayTestSample;

typedef struct {
    char filePath[TONE_PLAY_MEDIA_PATH_MAX];
    char control[TONE_PLAY_CONTROL_CMD_MAX_LEN];
    TonePlayTestSample testSample;
#ifdef MEMORY_MINI
    MediaHalThreadIdHandle playTask;
#else
    ToggleConfig notifyToggleConfig;
    MediaThreadIdHandle playTask;
#endif
} TonePlaySampleParam;

typedef enum {
    TONE_PLAY_MEDIA_AUDIO_PLAY_START,             // 00
    TONE_PLAY_MEDIA_AUDIO_PLAY_PAUSE,             // 01
    TONE_PLAY_MEDIA_AUDIO_PLAY_RESUME,            // 02
    TONE_PLAY_MEDIA_AUDIO_PLAY_STOP,              // 03
    TONE_PLAY_MEDIA_AUDIO_PLAY_SET_VOLUME,        // 05
    TONE_PLAY_MEDIA_AUDIO_PLAY_CURRENT_STATE,     // 06
    TONE_PLAY_MEDIA_AUDIO_PLAY_CURRENT_POSITION,  // 07
    TONE_PLAY_MEDIA_AUDIO_PLAY_SINGLE_LOOP_ON,    // 08
    TONE_PLAY_MEDIA_AUDIO_PLAY_SINGLE_LOOP_OFF,   // 09
    TONE_PLAY_MEDIA_AUDIO_MAX_PLAY_ID,
} TonePlaySampleCmd;

typedef struct {
    TonePlaySampleCmd cmdId;
    char cmd[TONE_PLAY_CONTROL_CMD_MAX_LEN];
} TonePlaySampleType;

TonePlaySampleType g_tonePlayerFunlist[] = {
    { TONE_PLAY_MEDIA_AUDIO_PLAY_PAUSE, "pause" },
    { TONE_PLAY_MEDIA_AUDIO_PLAY_RESUME, "resume" },
    { TONE_PLAY_MEDIA_AUDIO_PLAY_SET_VOLUME, "setvolume" },
    { TONE_PLAY_MEDIA_AUDIO_PLAY_STOP, "stop" },
    { TONE_PLAY_MEDIA_AUDIO_PLAY_CURRENT_STATE, "currentstate" },
    { TONE_PLAY_MEDIA_AUDIO_PLAY_CURRENT_POSITION, "currentposition" },
    { TONE_PLAY_MEDIA_AUDIO_PLAY_SINGLE_LOOP_ON, "loopon" },
    { TONE_PLAY_MEDIA_AUDIO_PLAY_SINGLE_LOOP_OFF, "loopoff" },
};

static TonePlaySampleParam g_tonePlaySampleParam = {};
static TonePlayerHandle g_tonePlayer = nullptr;
static TonePlayTestSample g_tonePlaySample = {};
static bool g_tonePlayTaskAlive = false;

static uint32_t ConvertFuncId()
{
    uint32_t ret = TONE_PLAY_MEDIA_AUDIO_MAX_PLAY_ID;
    for (uint32_t i = 0; i < TONE_PLAY_ARRAY_SIZE(g_tonePlayerFunlist); i++) {
        bool matchCmd = (strncasecmp(g_tonePlaySampleParam.control, g_tonePlayerFunlist[i].cmd,
            strlen(g_tonePlayerFunlist[i].cmd)) == 0);
        if (matchCmd) {
            TONE_PLAYER_SAMPLE_LOGI("play command %s matched", g_tonePlaySampleParam.control);
            return g_tonePlayerFunlist[i].cmdId;
        }
    }
    return ret;
}

static int32_t TonePlayerTaskInitInfo(TonePlaySampleParam *sampleParam, AudioSession& sessionId)
{
    TONE_PLAYER_SAMPLE_LOGI("PlayerTaskEntry enter");

    TonePlayTestSample sample = {};
    if (memset_s(&sampleParam->testSample, sizeof(TonePlayTestSample), 0, sizeof(TonePlayTestSample)) != EOK) {
        TONE_PLAYER_SAMPLE_LOGE("memset_s failed");
    }

    sampleParam->testSample.playCompleted = false;
    sampleParam->testSample.isStop = false;
#ifndef MEMORY_MINI
    sampleParam->testSample.interruptHintPause = false;
    sampleParam->testSample.interruptHintResume = false;
    sampleParam->testSample.interruptHintStop = false;
#endif
    if (memcpy_s(sampleParam->testSample.filePath, TONE_PLAY_MEDIA_PATH_MAX,
        sampleParam->filePath, strlen(sampleParam->filePath) + 1) != EOK) {
        TONE_PLAYER_SAMPLE_LOGE("memcpy_s failed");
    }
#ifdef MEMORY_MINI
    MEDIA_HAL_UNUSED(sessionId);
#else
    sessionId = AudioManagerMakeSessionId();
    if (sessionId == AUDIO_SESSION_ID_NONE) {
        TONE_PLAYER_SAMPLE_LOGE("AUDIO_SESSION_ID_NONE");
        return -1;
    }
    int32_t ret = AudioManagerActivateInterrupt(AUDIO_STREAM_NOTIFICATION_SYSTEM, sessionId);
    if (ret != 0) {
        TONE_PLAYER_SAMPLE_LOGE("ActivateAudioInterrupt failed");
        return -1;
    }
#endif
    sampleParam->testSample.mutex = TONE_PLAYER_SAMPLE_MUTEX_CREATE(nullptr);
    if (sampleParam->testSample.mutex == nullptr) {
        TONE_PLAYER_SAMPLE_LOGE("create mutex failed");
#ifndef MEMORY_MINI
        (void)AudioManagerDeactivateInterrupt(AUDIO_STREAM_NOTIFICATION_SYSTEM);
#endif
        return -1;
    }
    sampleParam->testSample.cond = TONE_PLAYER_SAMPLE_THREAD_COND_CREATE();
    if (sampleParam->testSample.cond == nullptr) {
        TONE_PLAYER_SAMPLE_LOGE("create cond failed");
        (void)TONE_PLAYER_SAMPLE_MUTEX_DESTROY(&sampleParam->testSample.mutex);
#ifndef MEMORY_MINI
        (void)AudioManagerDeactivateInterrupt(AUDIO_STREAM_NOTIFICATION_SYSTEM);
#endif
        return -1;
    }
#ifndef MEMORY_MINI
    sampleParam->testSample.sessionId = sessionId;
#endif
    TONE_PLAYER_SAMPLE_LOGI("end");
    return 0;
}

static void TonePlayerRunExit(TonePlaySampleParam *sampleParam)
{
    TONE_PLAYER_SAMPLE_MUTEX_LOCK(sampleParam->testSample.mutex);
    sampleParam->testSample.cirBufRunning = 0;
    TONE_PLAYER_SAMPLE_MUTEX_UN_LOCK(sampleParam->testSample.mutex);
#ifndef MEMORY_MINI
    if (sampleParam->testSample.interruptHintStop) {
        return;
    }
#endif
    TonePlayerStop(g_tonePlayer);
    TONE_PLAYER_SAMPLE_LOGI("TonePlayerStop exit");
}

static void TonePlayerStreamExit(TonePlaySampleParam *sampleParam)
{
#ifndef MEMORY_MINI
    AudioManagerDeactivateInterrupt(AUDIO_STREAM_NOTIFICATION_SYSTEM);
#endif
    TONE_PLAYER_SAMPLE_MUTEX_DESTROY(&sampleParam->testSample.mutex);
    TONE_PLAYER_SAMPLE_THREAD_COND_DESTROY(&sampleParam->testSample.cond);
    g_tonePlayTaskAlive = false;
    TONE_PLAYER_SAMPLE_LOGI("PlayerTaskEntry exit");
}

#ifndef MEMORY_MINI
static void ToneProcessInterrupt(TonePlayTestSample *sample)
{
    if (sample->interruptHintPause) {
        TonePlayerPause(g_tonePlayer);
        sample->interruptHintPause = false;
    }
    if (sample->interruptHintResume) {
        TonePlayerPlay(g_tonePlayer);
        sample->interruptHintResume = false;
    }
    if (sample->interruptHintStop) {
        TonePlayerStop(g_tonePlayer);
        DestroyTonePlayer(g_tonePlayer);
    }
}
#endif

static void TonePlayerWaitPlayCompletedOrStop(TonePlaySampleParam *sampleParam)
{
    TONE_PLAYER_SAMPLE_MUTEX_LOCK(sampleParam->testSample.mutex);
    while (!sampleParam->testSample.playCompleted &&
        !sampleParam->testSample.isStop &&
#ifndef MEMORY_MINI
        !sampleParam->testSample.interruptHintStop &&
#endif
        !sampleParam->testSample.playError) {
        TONE_PLAYER_SAMPLE_THREAD_COND_WAIT(sampleParam->testSample.cond, sampleParam->testSample.mutex);
#ifndef MEMORY_MINI
        ToneProcessInterrupt(&sampleParam->testSample);
#endif
    }
    TONE_PLAYER_SAMPLE_MUTEX_UN_LOCK(sampleParam->testSample.mutex);
}

static void TonePlayerEeventHandle(TonePlayerEvent enEvent, const void *data, const void *cookie)
{
    if (cookie == nullptr) {
        TONE_PLAYER_SAMPLE_LOGE("playback cookie is nullptr");
        return;
    }
    TONE_PLAY_UNUSED(data);
    TonePlayTestSample *testSample = (TonePlayTestSample *)cookie;
    switch (enEvent) {
        case TONE_PLAY_EVENT_EOF:
            TONE_PLAYER_SAMPLE_LOGI("playback complete signal one");
            TONE_PLAYER_SAMPLE_MUTEX_LOCK(testSample->mutex);
            testSample->playCompleted = true;
            TONE_PLAYER_SAMPLE_THREAD_COND_SIGNAL(testSample->cond);
            TONE_PLAYER_SAMPLE_MUTEX_UN_LOCK(testSample->mutex);
            break;
        case TONE_PLAY_EVENT_ERROR:
            TONE_PLAYER_SAMPLE_LOGI("playback error signal one");
            TONE_PLAYER_SAMPLE_MUTEX_LOCK(testSample->mutex);
            testSample->playError = true;
            TONE_PLAYER_SAMPLE_THREAD_COND_SIGNAL(testSample->cond);
            TONE_PLAYER_SAMPLE_MUTEX_UN_LOCK(testSample->mutex);
            break;
        case TONE_PLAY_EVENT_BUTT:
            break;
        default:
            break;
    }
}

static void *TonePlayerTaskEntry(void *priv)
{
    TonePlaySampleParam *sampleParam = (TonePlaySampleParam *)priv;
    AudioSession sessionId = 0;
    int32_t ret = TonePlayerTaskInitInfo(sampleParam, sessionId);
    if (ret != 0) {
        return nullptr;
    }
    ret = CreateTonePlayer(&g_tonePlayer);
    if (ret != 0) {
        TONE_PLAYER_SAMPLE_LOGE("g_adapter make failed");
        goto EXIT1;
    }
#ifndef MEMORY_MINI
    ret = TonePlayerSetToggleConfig(g_tonePlayer, g_tonePlaySampleParam.notifyToggleConfig);
    if (ret != 0) {
        goto EXIT2;
    }
#endif
    ret = TonePlayerSetDataSource(g_tonePlayer, sampleParam->testSample.filePath,
        strlen(sampleParam->testSample.filePath) + 1);
    if (ret != 0) {
        TONE_PLAYER_SAMPLE_LOGE("tone player set data source failed");
        goto EXIT2;
    }
    ret = TonePlayerPrepare(g_tonePlayer);
    if (ret != 0) {
        TONE_PLAYER_SAMPLE_LOGE("tone player prepare failed");
        goto EXIT3;
    }
    ret = TonePlayerSetSessionId(g_tonePlayer, sessionId);
    if (ret != 0) {
        TONE_PLAYER_SAMPLE_LOGE("tone player set session id failed");
        goto EXIT3;
    }
    ret = TonePlayerRegCallback(g_tonePlayer, TonePlayerEeventHandle, &sampleParam->testSample);
    if (ret != 0) {
        TONE_PLAYER_SAMPLE_LOGE("tone player reg callback failed");
        goto EXIT3;
    }
    ret = TonePlayerPlay(g_tonePlayer);
    if (ret != 0) {
        TONE_PLAYER_SAMPLE_LOGE("tone player play failed");
        goto EXIT3;
    }
    g_tonePlayTaskAlive = true;
    TonePlayerWaitPlayCompletedOrStop(sampleParam);

EXIT3:
    TonePlayerRunExit(sampleParam);
EXIT2:
    (void)DestroyTonePlayer(g_tonePlayer);
EXIT1:
    TonePlayerStreamExit(sampleParam);
    TONE_PLAYER_SAMPLE_LOGI("all exit");
    return nullptr;
}

static int32_t TonePlaySampleEnableSingleLooping()
{
    return 0;
}

static int32_t TonePlaySamplePause()
{
    int32_t ret = TonePlayerPause(g_tonePlayer);
    if (ret != 0) {
        TONE_PLAYER_SAMPLE_LOGE("Pause failed: %d", ret);
        return -1;
    }

    TONE_PLAYER_SAMPLE_LOGI("Pause success");
    return 0;
}

static int32_t TonePlaySampleResume()
{
    int32_t ret = TonePlayerPlay(g_tonePlayer);
    if (ret != 0) {
        TONE_PLAYER_SAMPLE_LOGE("failed: %d", ret);
        return -1;
    }
    TONE_PLAYER_SAMPLE_LOGI("Resume success");
    return 0;
}

static int32_t TonePlaySampleStop()
{
    TONE_PLAYER_SAMPLE_MUTEX_LOCK(g_tonePlaySampleParam.testSample.mutex);
    g_tonePlaySampleParam.testSample.isStop = true;
    TONE_PLAYER_SAMPLE_THREAD_COND_SIGNAL(g_tonePlaySampleParam.testSample.cond);
    TONE_PLAYER_SAMPLE_MUTEX_UN_LOCK(g_tonePlaySampleParam.testSample.mutex);
    if (g_tonePlayTaskAlive) {
        TONE_PLAYER_SAMPLE_THREAD_JOIN(&g_tonePlaySampleParam.playTask);
        g_tonePlayTaskAlive = false;
    }
    TONE_PLAYER_SAMPLE_LOGI("Stop success");
    return 0;
}

static int32_t TonePlaySampleSetVolume(int32_t argc, const char **argv)
{
    if (argc < 0x2) {
        TONE_PLAYER_SAMPLE_LOGI("Stop success");
        return -1;
    }
    int32_t linearVolume = atoi(argv[0x1]);
#ifdef MEMORY_MINI
    int32_t ret = TonePlayerSetVolume(g_tonePlayer, linearVolume);
#else
    ToggleConfig volumeToggleConfig = { {false}, {0} };
    if (argc >= 0x3) { //  The input parameter contains toggle count
        volumeToggleConfig.toggleValid = true;
        volumeToggleConfig.toggleCount = atoll(argv[2]) & 0xFFFFFFFF;
    }
    int32_t ret = AudioManagerSetVolumeAndToggleConfig(linearVolume, volumeToggleConfig);
#endif
    if (ret != 0) {
        TONE_PLAYER_SAMPLE_LOGE("SetVolume(%d) failed", linearVolume);
        return -1;
    }
    TONE_PLAYER_SAMPLE_LOGI("SetVolume(%d) success", linearVolume);
    return 0;
}

static int32_t TonePlaySampleState()
{
    TonePlayerStates state;
    int32_t ret = TonePlayerGetCurPlayState(g_tonePlayer, &state);
    if (ret != 0) {
        TONE_PLAYER_SAMPLE_LOGE("GetPlayerState failed: %d", ret);
        return -1;
    }

    // log error level only for auto test
    TONE_PLAYER_SAMPLE_LOGE("GetPlayerState success, current state: '%s'\n",
        (state == TONE_PLAY_STATUS_ERR) ? "Error" :
        (state == TONE_PLAY_STATUS_IDLE) ? "Idle" :
        (state == TONE_PLAY_STATUS_INIT) ? "Initialized" :
        (state == TONE_PLAY_STATUS_PREPARING) ? "Preparing" :
        (state == TONE_PLAY_STATUS_PREPARED) ? "Prepared" :
        (state == TONE_PLAY_STATUS_PLAY) ? "Playback started" :
        (state == TONE_PLAY_STATUS_PAUSE) ? "Playback paused" :
        (state == TONE_PLAY_STATUS_STOPPED) ? "Playback stopped" :
        (state == TONE_PLAY_STATUS_COMPLETE) ? "Playback completed" : "unknown");
    return 0;
}

static int32_t TonePlaySamplePosition()
{
    uint64_t position;
    int32_t ret = TonePlayerGetCurPosition(g_tonePlayer, &position);
    if (ret != 0) {
        TONE_PLAYER_SAMPLE_LOGE("GetPlayerState failed: %d", ret);
        return -1;
    }
    // log error level only for auto test
    TONE_PLAYER_SAMPLE_LOGE("get current postion success, current postion: %llu ms", position);
    return 0;
}

static int32_t HandleTonePlayerSampleCmd(int32_t argc, const char **argv)
{
    uint32_t funId = ConvertFuncId();
    int32_t ret = -1;
    switch (funId) {
        case TONE_PLAY_MEDIA_AUDIO_PLAY_PAUSE:
            ret = TonePlaySamplePause();
            break;
        case TONE_PLAY_MEDIA_AUDIO_PLAY_RESUME:
            ret = TonePlaySampleResume();
            break;
        case TONE_PLAY_MEDIA_AUDIO_PLAY_STOP:
            ret = TonePlaySampleStop();
            break;
        case TONE_PLAY_MEDIA_AUDIO_PLAY_SET_VOLUME:
            ret = TonePlaySampleSetVolume(argc, argv);
            break;
        case TONE_PLAY_MEDIA_AUDIO_PLAY_CURRENT_STATE:
            ret = TonePlaySampleState();
            break;
        case TONE_PLAY_MEDIA_AUDIO_PLAY_CURRENT_POSITION:
            ret = TonePlaySamplePosition();
            break;
        case TONE_PLAY_MEDIA_AUDIO_PLAY_SINGLE_LOOP_ON:
            ret = TonePlaySampleEnableSingleLooping();
            break;
        default:
            TONE_PLAYER_SAMPLE_LOGE("input cmd %s invaild", g_tonePlaySampleParam.control);
            ret = 0;
            break;
    }
    return ret;
}

static int32_t TonePlayerSampleInit(int32_t argc, const char **argv)
{
    for (int32_t i = 0; i < argc; i++) {
        TONE_PLAYER_SAMPLE_LOGI("argv[%d]: %s", i, argv[i]);
    }
    TONE_PLAY_SAMPLE_RETURN_VAL_IF_NULL(argv);
    if (argc < 1) {
        return -1;
    }
    return 0;
}

int32_t TonePlayerSample(int32_t argc, const char *argv[])
{
    int32_t ret = TonePlayerSampleInit(argc, argv);
    if (ret != 0) {
        return ret;
    }
    if (memcpy_s(g_tonePlaySampleParam.control, sizeof(g_tonePlaySampleParam.control),
        argv[0], strlen(argv[0]) + 1) != EOK) {
        TONE_PLAYER_SAMPLE_LOGE("memcpy_s failed");
        return -1;
    }
    TONE_PLAYER_SAMPLE_LOGI("[PlayerSample] argv[0]: %s", argv[0]);
    if (!g_tonePlayTaskAlive && strncmp(g_tonePlaySampleParam.control, "xx", strlen("xx")) == 0) { /* 2: xx */
        if (argc < 0x4) { /* 4: invalid argc */
            return -1;
        }
#ifndef MEMORY_MINI
        ret = AudioManagerInitialize();
        if (ret != 0) {
            TONE_PLAYER_SAMPLE_LOGE("audiomanager init failed");
            return -1;
        }
        g_tonePlaySampleParam.notifyToggleConfig.toggleValid = false;
        g_tonePlaySampleParam.notifyToggleConfig.toggleCount = 0;
        if (argc >= 0x5) {
            g_tonePlaySampleParam.notifyToggleConfig.toggleValid = true;
            g_tonePlaySampleParam.notifyToggleConfig.toggleCount = atoll(argv[4]) & 0xFFFFFFFF;
        }
#endif
        g_tonePlaySampleParam.testSample = g_tonePlaySample;
        if (strlen(argv[0x1]) < TONE_PLAY_MEDIA_PATH_MAX) {
            if (memcpy_s(g_tonePlaySampleParam.filePath, TONE_PLAY_MEDIA_PATH_MAX,
                argv[0x1], strlen(argv[0x1]) + 1) != EOK) {
                TONE_PLAYER_SAMPLE_LOGE("memcpy_s failed");
                return -1;
            }
        } else {
            TONE_PLAYER_SAMPLE_LOGI("input path too long");
            return -1;
        }
#ifdef MEMORY_MINI
        MediaHalThreadattr attr = { "toneplayerTask", 0x1000, THREAD_SCHED_INVALID, 0, true };
        MediaHalThreadIdHandle taskHandle1 = MediaHalThreadCreate(TonePlayerTaskEntry, &g_tonePlaySampleParam, &attr);
#else
        MediaThreadattr attr = { "toneplayerTask", 0x1000, THREAD_SCHED_INVALID, 0, true };
        MediaThreadIdHandle taskHandle1 = MediaThreadCreate(TonePlayerTaskEntry, &g_tonePlaySampleParam, &attr);
#endif
        if (taskHandle1 == nullptr) {
            TONE_PLAYER_SAMPLE_LOGE("thread create failed");
            return -1;
        }
    } else if (g_tonePlayTaskAlive && strncmp(g_tonePlaySampleParam.control, "xx", strlen("xx")) == 0) { /* 2: xx */
        TONE_PLAYER_SAMPLE_LOGE("playerTask exists\nEnter the stop command first");
    }
    ret = HandleTonePlayerSampleCmd(argc, argv);
    TONE_PLAYER_SAMPLE_LOGI("PlayerSample end!");
    return ret;
}
#ifdef __cplusplus
};
#endif

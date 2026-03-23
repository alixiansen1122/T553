/*
 * Copyright (c) CompanyNameMagicTag. 2021-2021. All rights reserved.
 * Description: media hal common
 * Author: Media Software Group
 * Create: 2021-04-02
 */

#include "player_sample_wrapper.h"
#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include <strings.h>
#include <climits>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <atomic>
#ifdef SUPPORT_SYS_PRCTL
#include <sys/prctl.h>
#endif
#include "source.h"
#include "player.h"
#include "format.h"
#include "securec.h"
#include "audio_manager.h"
#include "audio_base_type.h"
#include "media_log.h"
#include "media_thread_adapt.h"
const uint32_t BUFFER_SIZE = 120;
const uint32_t VQE_PREFIX_LEN = 18;
const uint32_t PEQ_PREFIX_LEN = 50;
const uint32_t NV_AUDIO_PEQ_PARAM_SIZE = 41;
const uint32_t NV_AUDIO_VQE_PARAM_SIZE = 53;
const uint32_t PEQ_PARAM_SIZE = (NV_AUDIO_PEQ_PARAM_SIZE * sizeof(uint32_t));
const uint32_t VQE_PARAM_SIZE = (NV_AUDIO_VQE_PARAM_SIZE * sizeof(uint32_t));
const uint32_t SEA_BUFFER_SIZE = (VQE_PARAM_SIZE + VQE_PREFIX_LEN);
const uint32_t AEF_BUFFER_SIZE = (PEQ_PARAM_SIZE + PEQ_PREFIX_LEN);

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
const int32_t STREAM_SOURCE_FORMAT_MAX_LEN = 10;
const int32_t STREAM_PROCESS_SLEEP_TIME_US = 5000;
const uint32_t STREAM_SOURCE_READ_LEN = 1024;
const int32_t LOOP_WAIT_PLAY_EXIT_SLEEP_TIME_US = 100000;
const uint32_t STREAM_FLAG_EOS = 4;
const uint32_t STREAM_FLAG_PARTIAL_FRAME = 8;
const uint32_t MAX_ARGC = 5;
#define MEDIA_PATH_MAX 260
#define MEDIA_PATH_KEY_LEN 20
#define MEDIA_MAX_PLAYER_TASK 6
#define DEFAULE_HTTP_1 "http://openaudio.cos.tx.xmcdn.com/group64/M0B/02/30/wKgMaV1whaLAcRGQAE_0uZWMCp8270.mp3"
#define DEFAULE_HTTP_2 "http://cdn.gohudong.com/Public/Hudong/Games/GameShake2019/assets/audio/game_running.mp3"
#define DEFAULE_HTTP_3 "https://music.163.com/song/media/outer/url?id=2166584564.mp3"
#define DEFAULT_HTTP_4 "http://openaudio.cos.tx.xmcdn.com/group64/M0B/02/30/wKgMaV1whaLAcRGQAE_0uZWMCp8270.mp3"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#define IS_OK(ret) \
do { \
    if ((ret) != 0) { \
        MEDIA_ERR_LOG("ret:%d", ret); \
    } \
} while (0)

#define SAMPLE_RETURN_VAL_IF_NULL(condition)                                           \
    do {                                                                               \
        if ((condition) == nullptr) {                                                  \
            MEDIA_ERR_LOG("is nullptr");    \
            return -1;                                                         \
        }                                                                              \
    } while (0)

class StreamSourceSample;
typedef struct {
    char formatType[STREAM_SOURCE_FORMAT_MAX_LEN];
    uint32_t  sampleRate;
    uint32_t  sampleFormat;
    uint32_t  channel;
    uint32_t  channelLayout;
} StreamSourceFormat;

typedef struct TagTestSample {
    std::shared_ptr<Player> adapter;
    MediaThreadIdHandle streamSoruceProcess;
    MediaMutexHandle mutex;
    MediaThreadCondHandle cond;
    int32_t cirBufRunning;
    SourceType sourceType;
    char    filePath[MEDIA_PATH_MAX];
    StreamSourceFormat format;
    std::shared_ptr<StreamSourceSample> streamSample;
    bool userPaused;
    bool playError;
    bool playCompleted;
    bool isStop;
    bool interruptHintPause;
    bool interruptHintResume;
    bool interruptHintStop;
} TestSample;

typedef struct {
    SourceType sourceType;
    char    filePath[MEDIA_PATH_MAX];
    StreamSourceFormat format;
    MediaMutexHandle sampleMutex;
    char    control[20];
    TestSample  testSample;
    AudioInterrupt interrupt;
    MediaThreadIdHandle playTask;
    bool isSingleLoop;
    std::atomic<bool> taskAlive;
    std::shared_ptr<Player> player;
    AudioStreamType streamType;
} SampleParam;

typedef struct TagIdleBuffer {
    size_t idx;
    size_t offset;
    size_t size;
} IdleBuffer;

typedef struct {
    MediaThreadIdHandle playTask;
    AudioStreamType streamType;
} PlayerTaskContext;

static PlayerTaskContext g_playerTask[MEDIA_MAX_PLAYER_TASK] = {
    {nullptr, AUDIO_STREAM_INVALID},
    {nullptr, AUDIO_STREAM_INVALID},
    {nullptr, AUDIO_STREAM_INVALID},
    {nullptr, AUDIO_STREAM_INVALID},
    {nullptr, AUDIO_STREAM_INVALID},
    {nullptr, AUDIO_STREAM_INVALID},
};

static MediaMutexHandle g_playerTaskMutex = nullptr;

static const char *g_songList[] = {
    "/user/test/cai.mp3",
    "/user/test/guyongzhe_8k_2ch_8k_0x20000.mp3"
};

static uint32_t g_totalSongEntries = sizeof(g_songList) / sizeof(g_songList[0]);
static uint32_t g_currentSongIndex = 0;

static SampleParam *g_sampleParam = nullptr;
static SampleParam *g_tonePlayersampleParam = nullptr;
static AudioManager& g_amInstance = AudioManager::GetInstance();
static TestSample  g_sample;
static TestSample  g_tonePlayersample;

static void WaitPlayerTaskEnd(SampleParam *sampleParam);
static void StorePlayerTaskInfo(SampleParam *sampleParam);

typedef enum {
    MEDIA_AUDIO_PLAY_START,             // 00
    MEDIA_AUDIO_PLAY_PAUSE,             // 01
    MEDIA_AUDIO_PLAY_RESUME,            // 02
    MEDIA_AUDIO_PLAY_STOP,              // 03
    MEDIA_AUDIO_PLAY_ALBUM_INFO,        // 04
    MEDIA_AUDIO_PLAY_SET_VOLUME,        // 05
    MEDIA_AUDIO_PLAY_GET_MUSIC_VOLUME,  // 06
    MEDIA_AUDIO_PLAY_SET_BT_SCO_VOLUME, // 07
    MEDIA_AUDIO_PLAY_GET_BT_SCO_VOLUME, // 08
    MEDIA_AUDIO_PLAY_SINGLE_LOOP_ON,    // 09
    MEDIA_AUDIO_PLAY_SINGLE_LOOP_OFF,   // 09
    MEDIA_AUDIO_PLAY_SWITCH_SONG_PREV,  // 0A
    MEDIA_AUDIO_PLAY_SWITCH_SONG_NEXT,  // 0A
    MEDIA_AUDIO_PLAY_SEEK,
    MEDIA_AUDIO_PLAY_CURRENT_STATE,
    MEDIA_AUDIO_PLAY_DUMP_INFO,
    MEDIA_AUDIO_PLAY_SET_AEF_PARAM,
    MEDIA_AUDIO_PLAY_AEF_ENABLE,
    MEDIA_AUDIO_MAX_PLAY_ID,
} PlaySampleCmd;

typedef struct {
    char key[MEDIA_PATH_KEY_LEN];
    char value[MEDIA_PATH_MAX];
} PlaySampleHttpPath;

typedef struct {
    PlaySampleCmd cmdId;
    char cmd[64];
} PlaySampleType;

static PlaySampleHttpPath g_httpPathlist[] = {
    { "http1://", DEFAULE_HTTP_1 },
    { "http2://", DEFAULE_HTTP_2 },
    { "http3://", DEFAULE_HTTP_3 },
    { "http4://", DEFAULT_HTTP_4 }
};

PlaySampleType g_funlist[] = {
    { MEDIA_AUDIO_PLAY_PAUSE, "pause" },
    { MEDIA_AUDIO_PLAY_RESUME, "resume" },
    { MEDIA_AUDIO_PLAY_ALBUM_INFO, "albuminfo" },
    { MEDIA_AUDIO_PLAY_SET_VOLUME, "setvolume" },
    { MEDIA_AUDIO_PLAY_GET_MUSIC_VOLUME, "getmusicvolume" },
    { MEDIA_AUDIO_PLAY_SET_BT_SCO_VOLUME, "scovolume" },
    { MEDIA_AUDIO_PLAY_GET_BT_SCO_VOLUME, "getscovolume" },
    { MEDIA_AUDIO_PLAY_SINGLE_LOOP_ON, "loopon" },
    { MEDIA_AUDIO_PLAY_SINGLE_LOOP_OFF, "loopoff" },
    { MEDIA_AUDIO_PLAY_SWITCH_SONG_PREV, "prevsong" },
    { MEDIA_AUDIO_PLAY_SWITCH_SONG_NEXT, "nextsong" },
    { MEDIA_AUDIO_PLAY_SEEK, "seek" },
    { MEDIA_AUDIO_PLAY_STOP, "stop" },
    { MEDIA_AUDIO_PLAY_CURRENT_STATE, "currentstate" },
    { MEDIA_AUDIO_PLAY_DUMP_INFO, "dumpinfo" },
    { MEDIA_AUDIO_PLAY_SET_AEF_PARAM, "setaefparam" },
    { MEDIA_AUDIO_PLAY_AEF_ENABLE, "aefenable" },
};

class StreamSourceSample : public StreamSource {
public:
    StreamSourceSample(void);
    virtual ~StreamSourceSample(void);
    void OnBufferAvailable(size_t index, size_t offset, size_t size) override;
    void SetStreamCallback(const std::shared_ptr<StreamCallback> &callback) override;
    uint8_t *GetBufferAddress(size_t idx);
    void QueueBuffer(size_t index, size_t offset, size_t size, int64_t timestampUs, uint32_t flags);
    int32_t GetAvailableBuffer(IdleBuffer* buffer);

private:
    std::weak_ptr<StreamCallback> m_callBack;
    std::vector<IdleBuffer> aviableBuffer;
    MediaMutexHandle m_mutex;
};

StreamSourceSample::StreamSourceSample(void)
    : m_mutex(MediaMutexCreate(nullptr))
{
    aviableBuffer.clear();
}

StreamSourceSample::~StreamSourceSample(void)
{
    aviableBuffer.clear();
    MediaMutexDestroy(&m_mutex);
}

void StreamSourceSample::SetStreamCallback(const std::shared_ptr<StreamCallback> &callback)
{
    m_callBack = callback;
}

uint8_t *StreamSourceSample::GetBufferAddress(size_t idx)
{
    std::shared_ptr<StreamCallback> callback = m_callBack.lock();
    if (callback == nullptr) {
        return nullptr;
    }
    return callback->GetBuffer(idx);
}

void StreamSourceSample::QueueBuffer(size_t index, size_t offset, size_t size, int64_t timestampUs, uint32_t flags)
{
    std::shared_ptr<StreamCallback> callback = m_callBack.lock();
    if (callback == nullptr) {
        return;
    }
    callback->QueueBuffer(index, offset, size, timestampUs, flags);
}

void StreamSourceSample::OnBufferAvailable(size_t index, size_t offset, size_t size)
{
    IdleBuffer buffer;
    MediaMutexLock(m_mutex);
    buffer.idx = index;
    buffer.offset = offset;
    buffer.size = size;
    aviableBuffer.push_back(buffer);
    MediaMutexUnLock(m_mutex);
}

int32_t StreamSourceSample::GetAvailableBuffer(IdleBuffer* buffer)
{
    SAMPLE_RETURN_VAL_IF_NULL(buffer);
    MediaMutexLock(m_mutex);
    if (aviableBuffer.empty()) {
        MediaMutexUnLock(m_mutex);
        return -1;
    }
    *buffer = aviableBuffer[0];
    aviableBuffer.erase(aviableBuffer.begin());
    MediaMutexUnLock(m_mutex);
    return 0;
}

static FILE* OpenStreamFile(TestSample *sample)
{
    if (strlen(sample->filePath) == 0) {
        return nullptr;
    }

    char resolvedPath[PATH_MAX] = { 0 };
    if (realpath(sample->filePath, resolvedPath) == nullptr) {
        MEDIA_ERR_LOG("realpath file error");
        return nullptr;
    }
    if (memcpy_s(resolvedPath, sizeof(resolvedPath), sample->filePath, sizeof(sample->filePath)) != EOK) {
        MEDIA_ERR_LOG("memcpy_s failed");
    }
    FILE* pFile = fopen(resolvedPath, "rb");
    if (pFile == nullptr) {
        MEDIA_ERR_LOG("fopen failed");
    }

    return pFile;
}

static void* StreamSourceProcess(void* arg)
{
    IdleBuffer buffer;
    int32_t ret;
    uint8_t *data = nullptr;
    size_t readLen;
    size_t len;

    TestSample *sample = (TestSample *)arg;
    FILE *pFile = OpenStreamFile(sample);
    if (pFile == nullptr) {
        goto EXIT;
    }

#ifdef SUPPORT_SYS_PRCTL
    prctl(PR_SET_NAME, "StreamProc", 0, 0, 0);
#endif
    MEDIA_INFO_LOG("file:%s", sample->filePath);
    while (true) {
        MediaMutexLock(sample->mutex);
        if (!sample->cirBufRunning) {
            MediaMutexUnLock(sample->mutex);
            break;
        }
        MediaMutexUnLock(sample->mutex);
        ret = sample->streamSample->GetAvailableBuffer(&buffer);
        if (ret != 0) {
            usleep(STREAM_PROCESS_SLEEP_TIME_US);
            continue;
        }
        data = sample->streamSample->GetBufferAddress(buffer.idx);
        if (data == nullptr) {
            MEDIA_ERR_LOG("get buffer null");
            break;
        }
        len = (buffer.size < STREAM_SOURCE_READ_LEN) ? buffer.size : STREAM_SOURCE_READ_LEN;
        readLen = fread(data + buffer.offset, 1, len, pFile);
        if (readLen <= len && readLen > 0) {
            sample->streamSample->QueueBuffer(buffer.idx, buffer.offset, readLen, 0, STREAM_FLAG_PARTIAL_FRAME);
        } else {
            sample->streamSample->QueueBuffer(buffer.idx, buffer.offset, readLen, 0, STREAM_FLAG_EOS);
            MEDIA_INFO_LOG("read file eof readLen:%d", readLen);
            break;
        }
    }
    fclose(pFile);
    MEDIA_INFO_LOG("exit");
EXIT:
    MediaMutexLock(sample->mutex);
    sample->cirBufRunning = 0;
    MediaMutexUnLock(sample->mutex);
    return nullptr;
}

/*
INTERRUPT_TYPE_BEGIN INTERRUPT_HINT_PAUSE
INTERRUPT_TYPE_BEGIN INTERRUPT_HINT_PAUSE
INTERRUPT_TYPE_END INTERRUPT_HINT_RESUME
INTERRUPT_TYPE_BEGIN INTERRUPT_HINT_STOP
INTERRUPT_TYPE_END INTERRUPT_HINT_STOP
*/
class PlayerInterruptListener : public InterruptListener {
public:
    PlayerInterruptListener(TestSample &in)
    {
        sample = &in;
    }
    ~PlayerInterruptListener() override {};
    void SetOwner(std::shared_ptr<Player> player)
    {
        player_ = player;
    };

    void RestFlag()
    {
        isPaused_ = false;
        isDelayed_ = false;
        isResumed_ = false;
        isBeginStopped_ = false;
        isEndStopped_ = false;
    };
    bool IsPaused()
    {
        std::cout << " ##### isPaused:" << isPaused_ << std::endl;
        return isPaused_;
    };
    bool IsDelayed()
    {
        return isDelayed_;
    };
    bool IsResumed()
    {
        return isResumed_;
    };
    bool IsBeginStopped()
    {
        return isBeginStopped_;
    };
    bool IsEndStopped()
    {
        return isEndStopped_;
    };

    void OnInterrupt(int32_t type, int32_t hint) override
    {
        MEDIA_INFO_LOG("OnInterrupt, type:%d, hint:%d", type, hint);

        if (type == INTERRUPT_TYPE_BEGIN && hint == INTERRUPT_HINT_PAUSE) {
            isPaused_ = true;
            isDelayed_ = true;

            if (sample->userPaused) {
                MEDIA_INFO_LOG("OnInterrupt for pausing playback when the user is paused, no response required");
                return;
            }

            MEDIA_INFO_LOG("PlayerInterruptListener OnInterrupt player pause signal one");
            MediaMutexLock(sample->mutex);
            sample->interruptHintPause = true;
            MediaThreadCondSignal(sample->cond);
            MediaMutexUnLock(sample->mutex);
        }
        if (type == INTERRUPT_TYPE_END && hint == INTERRUPT_HINT_RESUME) {
            isResumed_ = true;

            if (sample->userPaused) {
                MEDIA_INFO_LOG("OnInterrupt for resuming playback when the user is paused, no response required");
                return;
            }

            MEDIA_INFO_LOG("PlayerInterruptListener OnInterrupt player resume signal one");
            MediaMutexLock(sample->mutex);
            sample->interruptHintResume = true;
            MediaThreadCondSignal(sample->cond);
            MediaMutexUnLock(sample->mutex);
        }
        if (type == INTERRUPT_TYPE_BEGIN && hint == INTERRUPT_HINT_STOP) {
            isBeginStopped_ = true;

            MEDIA_INFO_LOG("PlayerInterruptListener OnInterrupt player stop signal one");
            MediaMutexLock(sample->mutex);
            sample->interruptHintStop = true;
            MediaThreadCondSignal(sample->cond);
            MediaMutexUnLock(sample->mutex);
        }
        if (type == INTERRUPT_TYPE_END && hint == INTERRUPT_HINT_STOP) {
            isEndStopped_ = true;
        }
    }

private:
    TestSample *sample;
    std::shared_ptr<Player> player_ = nullptr;
    bool isPaused_ = false;
    bool isDelayed_ = false;
    bool isResumed_ = false;
    bool isBeginStopped_ = false;
    bool isEndStopped_ = false;
};

class PlayerCallbackImpl : public PlayerCallback {
public:
    PlayerCallbackImpl(TestSample &in)
    {
        sample = &in;
    }

    ~PlayerCallbackImpl() override {}

    void OnPlaybackComplete() override
    {
        MEDIA_INFO_LOG("playback complete signal one");
        MediaMutexLock(sample->mutex);
        sample->playCompleted = true;
        MediaThreadCondSignal(sample->cond);
        MediaMutexUnLock(sample->mutex);
    }

    void OnError(int32_t errorType, int32_t errorCode) override
    {
        MEDIA_INFO_LOG("error type:%d, error code:%d", errorType, errorCode);
        if (errorType != PlayerErrorType::PLAYER_ERROR_UNKNOWN) {
            MEDIA_INFO_LOG("unsupport error type: %d", errorType);
            return;
        }
        if (errorCode == PlayerErrorCode::PLAYER_ERROR_CODE_AUD_PLAY_FAIL) {
            MEDIA_INFO_LOG("playback error signal one");
            MediaMutexLock(sample->mutex);
            sample->playError = true;
            MediaThreadCondSignal(sample->cond);
            MediaMutexUnLock(sample->mutex);
        }
    }

    void OnInfo(int32_t type, int32_t extra) override
    {
        MEDIA_INFO_LOG("PlayerCallback OnInfo, current state:%s",
            (type == PlayerInfoType::PLAYER_INFO_PLAY_START) ? "play start" :
            (type == PlayerInfoType::PLAYER_INFO_PLAYING) ? "playing" :
            (type == PlayerInfoType::PLAYER_INFO_PLAY_END) ? "playEnd" :
            (type == PlayerInfoType::PLAYER_INFO_SEEK_START) ? "seek start" :
            (type == PlayerInfoType::PLAYER_INFO_SEEK_END) ? "seek end" :
            (type == PlayerInfoType::PLAYER_INFO_PAUSED) ? "paused" :
            (type == PlayerInfoType::PLAYER_INFO_STOPPED) ? "stoped" : "unknown");
    }

    void OnRewindToComplete() override
    {
        MEDIA_INFO_LOG("OnRewindToComplete test");
    }

private:
    TestSample *sample;
};

static int32_t PlayerSetStreamSource(std::shared_ptr<Player> player, TestSample &sample,
    const StreamSourceFormat &format)
{
    if (sample.sourceType != SourceType::SOURCE_TYPE_STREAM) {
        MEDIA_ERR_LOG("sourcetype not SOURCE_TYPE_STREAM:%d", SourceType::SOURCE_TYPE_STREAM);
        return -1;
    }
    if (memcpy_s(&(sample.format), sizeof(sample.format),
        &format, sizeof(StreamSourceFormat)) != EOK) {
        MEDIA_ERR_LOG("memcpy_s sample.format failed");
    }
    if (strlen(sample.format.formatType) == 0) {
        MEDIA_ERR_LOG("intput invalid format type");
        return -1;
    }
    sample.streamSample = std::make_shared<StreamSourceSample>();
    if (sample.streamSample == nullptr) {
        return -1;
    }
    Format formats;
    if (strncasecmp(sample.format.formatType, "aac", strlen("aac")) == 0) {
        formats.PutStringValue(AUDIO_CODEC_MIME, MIME_AUDIO_AAC);
        MEDIA_INFO_LOG("set aac");
    } else if (strncasecmp(sample.format.formatType, "mp3", strlen("mp3")) == 0) {
        formats.PutStringValue(AUDIO_CODEC_MIME, MIME_AUDIO_MP3);
        MEDIA_INFO_LOG("set mp3");
    } else if (strncasecmp(sample.format.formatType, "pcm", strlen("pcm")) == 0) {
        formats.PutStringValue(AUDIO_CODEC_MIME, MIME_AUDIO_PCM);
        MEDIA_INFO_LOG("set pcm");
    } else {
        MEDIA_ERR_LOG("format:%s not support", sample.format.formatType);
        return -1;
    }
    if (sample.format.sampleRate > 0) {
        formats.PutIntValue(SAMPLERATE_MIME, sample.format.sampleRate);
    }
    if (sample.format.channel > 0) {
        formats.PutIntValue(CHANNEL_MIME, sample.format.channel);
    }
    Source source(sample.streamSample, formats);
    int32_t ret = player->SetSource(source);
    if (ret != 0) {
        MEDIA_ERR_LOG("set source failed!");
        return ret;
    }
    sample.cirBufRunning = 1;
    MediaThreadattr attr = { "StreamProc", 0x2000, THREAD_SCHED_INVALID, 0, false };
    sample.streamSoruceProcess = MediaThreadCreate(StreamSourceProcess, &sample, &attr);
    if (sample.streamSoruceProcess == nullptr) {
        MEDIA_ERR_LOG("thread create failed");
        ret = -1;
    }
    return ret;
}

static void ProcessInterrupt(SampleParam *sampleParam)
{
    if (sampleParam->testSample.interruptHintPause) {
        (void)sampleParam->player->Pause();
        sampleParam->testSample.interruptHintPause = false;
    }
    if (sampleParam->testSample.interruptHintResume) {
        (void)sampleParam->player->Play(); // error
        sampleParam->testSample.interruptHintResume = false;
    }
    if (sampleParam->testSample.interruptHintStop) {
        (void)sampleParam->player->Stop();
        (void)sampleParam->player->Reset();
        (void)sampleParam->player->Release();
    }
}

static void WaitPlayStartWhenDelay(SampleParam *sampleParam)
{
    MediaMutexLock(sampleParam->testSample.mutex);
    while (true) {
        if (sampleParam->testSample.interruptHintResume ||
            sampleParam->testSample.interruptHintStop) {
            MediaMutexUnLock(sampleParam->testSample.mutex);
            break;
        }
        MediaThreadCondWait(sampleParam->testSample.cond, sampleParam->testSample.mutex);
    }
}

static int32_t PlayerTaskInitInfo(SampleParam *sampleParam, AudioSession &sessionId)
{
    TestSample  sample;
    MEDIA_INFO_LOG("PlayerTaskEntry enter");
#ifdef SUPPORT_SYS_PRCTL
    prctl(PR_SET_NAME, "playerTask", 0, 0, 0);
#endif
    if (memset_s(&sampleParam->testSample, sizeof(TestSample), 0, sizeof(TestSample)) != EOK) {
        MEDIA_ERR_LOG("memset_s failed");
    }
    sampleParam->testSample.mutex = MediaMutexCreate(nullptr);
    if (sampleParam->testSample.mutex == nullptr) {
        MEDIA_ERR_LOG("create mutex failed");
        return -1;
    }
    sampleParam->testSample.cond = MediaThreadCondCreate();
    if (sampleParam->testSample.cond == nullptr) {
        MEDIA_ERR_LOG("create cond failed");
        return -1;
    }
    sampleParam->testSample.playCompleted = false;
    sampleParam->testSample.isStop = false;
    sampleParam->testSample.interruptHintPause = false;
    sampleParam->testSample.interruptHintResume = false;
    sampleParam->testSample.interruptHintStop = false;
    sampleParam->testSample.sourceType = sampleParam->sourceType;
    if (memcpy_s(sampleParam->testSample.filePath, MEDIA_PATH_MAX,
        sampleParam->filePath, strlen(sampleParam->filePath) + 1) != EOK) {
        MEDIA_ERR_LOG("memcpy_s failed");
    }
    bool init = g_amInstance.Initialize();
    if (!init) {
        MEDIA_ERR_LOG("audiomanager init failed");
        return -1;
    }
    while (true) {
        sessionId = g_amInstance.MakeSessionId();
        if (sessionId == AUDIO_SESSION_ID_NONE) {
            MEDIA_ERR_LOG("AUDIO_SESSION_ID_NONE");
            return -1;
        }
        std::shared_ptr<PlayerInterruptListener> playerInterruptListener =
            std::make_shared<PlayerInterruptListener>(sampleParam->testSample);
        sampleParam->interrupt = { sampleParam->streamType, sessionId, playerInterruptListener };
        /* After the delayed stream is woken up, the focus needs to be applied for again
        because the focus may be outdated and the policy needs to be obtained again. */
        int32_t strategy = g_amInstance.ActivateAudioInterrupt(sampleParam->interrupt);
        if (strategy == INTERRUPT_FAILED) {
            MEDIA_ERR_LOG("ActivateAudioInterrupt failed");
            return -1;
        }
        if (strategy == INTERRUPT_DELAYED) {
            WaitPlayStartWhenDelay(sampleParam);
            if (sampleParam->testSample.interruptHintStop) {
                g_amInstance.DeactivateAudioInterrupt(sampleParam->interrupt);
                return -1;
            }
            continue;
        }
        break;
    }
    sampleParam->testSample.interruptHintPause = false;
    sampleParam->testSample.interruptHintResume = false;
    return 0;
}

static void PlayerRunExit(SampleParam *sampleParam)
{
     /* stop and exit */
    MediaMutexLock(sampleParam->testSample.mutex);
    sampleParam->testSample.cirBufRunning = 0;
    MediaMutexUnLock(sampleParam->testSample.mutex);
    if (sampleParam->testSample.streamSoruceProcess != nullptr) {
        MediaThreadJoin(&sampleParam->testSample.streamSoruceProcess);
    }
    if (!sampleParam->testSample.interruptHintStop) {
        sampleParam->player->Stop();
        sampleParam->player->Reset();
        sampleParam->player->Release();
    }
}

static void PlayerStreamExit(SampleParam *sampleParam)
{
    if (g_amInstance.DeactivateAudioInterrupt(sampleParam->interrupt) != 0) {
        MEDIA_ERR_LOG("deactivate audio interrupt fail");
    }
    MediaMutexLock(sampleParam->testSample.mutex);
    sampleParam->taskAlive.store(false);
    if (!sampleParam->testSample.isStop) {
        if (sampleParam->playTask == nullptr) {
            MediaThreadCondWait(sampleParam->testSample.cond, sampleParam->testSample.mutex);
        }
        MediaThreadSetDetach(sampleParam->playTask, true);
        sampleParam->playTask = nullptr;
    }
    MediaMutexUnLock(sampleParam->testSample.mutex);
    MediaMutexDestroy(&sampleParam->testSample.mutex);
    MediaThreadCondDestroy(&sampleParam->testSample.cond);
}

void WaitPlayCompletedOrStop(SampleParam *sampleParam)
{
    while (true) {
        if (sampleParam->testSample.playCompleted ||
            sampleParam->testSample.isStop ||
            sampleParam->testSample.interruptHintStop ||
            sampleParam->testSample.playError) {
            MediaMutexUnLock(sampleParam->testSample.mutex);
            break;
        }
        MediaThreadCondWait(sampleParam->testSample.cond, sampleParam->testSample.mutex);
        ProcessInterrupt(sampleParam);
    }
}

static void *PlayerTaskEntry(void *priv)
{
    SampleParam *sampleParam = (SampleParam*)priv;
    int64_t duration = 0;
    AudioSession sessionId = 0;
    std::shared_ptr<Player> player = sampleParam->player;
    int32_t ret = PlayerTaskInitInfo(sampleParam, sessionId);
    if (ret != 0) {
        PlayerStreamExit(sampleParam);
        return nullptr;
    }
    MediaMutexLock(sampleParam->testSample.mutex);
    if (sampleParam->testSample.sourceType == SourceType::SOURCE_TYPE_STREAM) {
        if (PlayerSetStreamSource(player, sampleParam->testSample, sampleParam->format) < 0) {
            MediaMutexUnLock(sampleParam->testSample.mutex);
            PlayerStreamExit(sampleParam);
            return nullptr;
        }
    } else {
        std::string uri(sampleParam->testSample.filePath);
        std::map<std::string, std::string> header;
        Source source(uri, header);
        ret = player->SetSource(source);
    }
    ret = player->Prepare();
    std::shared_ptr<PlayerCallback> cb = std::make_shared<PlayerCallbackImpl>(sampleParam->testSample);
    player->SetPlayerCallback(cb);
    ret = player->SetAudioSessionId(sessionId);
    ret = player->GetDuration(duration);
    ret = player->SetAudioStreamType(sampleParam->streamType);
    /* start play */
    ret = player->Play();
    if (ret != 0) {
        MediaMutexUnLock(sampleParam->testSample.mutex);
        goto EXIT;
    }
    if (sampleParam->isSingleLoop) {
        // enable loop
        ret = player->EnableSingleLooping(true);
        if (ret != 0) {
            MEDIA_ERR_LOG("EnableSingleLooping failed:%d", ret);
        }
    }
    WaitPlayCompletedOrStop(sampleParam);
EXIT:
    PlayerRunExit(sampleParam);
    PlayerStreamExit(sampleParam);
    MEDIA_INFO_LOG("PlayerTaskEntry exit");
    return nullptr;
}

static int32_t CheckInputParam(SampleParam *sampleParam, int32_t argc, const char** argv)
{
    if ((uint32_t)argc <= MAX_ARGC) { /* 5: total 6 param */
        return -1;
    }

    if (strncpy_s(sampleParam->format.formatType, sizeof(sampleParam->format.formatType),
        argv[0x3], strlen(argv[0x3])) != 0) { /* 3: format */
        MEDIA_ERR_LOG("strncpy_s argv[0x3] failed");
    }
    sampleParam->format.sampleRate = atoi(argv[0x4]); /* 4: samplerate */
    MEDIA_INFO_LOG("CheckInputParam sampleRate:%d", sampleParam->format.sampleRate);
    sampleParam->format.channel = atoi(argv[0x5]); /* 5: channel */
    MEDIA_INFO_LOG("CheckInputParam channel:%d", sampleParam->format.channel);
    return 0;
}

static uint32_t ConvertFuncId(SampleParam *sampleParam)
{
    uint32_t ret = MEDIA_AUDIO_MAX_PLAY_ID;
    for (uint32_t i = 0; i < ARRAY_SIZE(g_funlist); i++) {
        bool matchCmd = (strncasecmp(sampleParam->control, g_funlist[i].cmd, strlen(g_funlist[i].cmd)) == 0);
        if (matchCmd) {
            MEDIA_INFO_LOG("play command %s matched", sampleParam->control);
            return g_funlist[i].cmdId;
        }
    }
    return ret;
}

static int32_t PlaySamplePause(SampleParam *sampleParam)
{
    if (sampleParam->player == nullptr || sampleParam->player.get() == nullptr) {
        MEDIA_ERR_LOG("player is nullptr");
        return -1;
    }

    int32_t ret = sampleParam->player->Pause();
    if (ret != 0) {
        MEDIA_ERR_LOG("Pause failed:%d", ret);
        return -1;
    }
    g_sampleParam->testSample.userPaused = true;

    MEDIA_INFO_LOG("success");
    return 0;
}

static int32_t PlaySampleResume(SampleParam *sampleParam)
{
    if (sampleParam->player == nullptr || sampleParam->player.get() == nullptr) {
        MEDIA_ERR_LOG(" player is nullptr");
        return -1;
    }

    int32_t ret = sampleParam->player->Play();
    if (ret != 0) {
        MEDIA_ERR_LOG("failed:%d", ret);
        return -1;
    }
    sampleParam->testSample.userPaused = false;

    MEDIA_INFO_LOG("success");
    return 0;
}

static int32_t PlaySampleStop(SampleParam *sampleParam)
{
    MediaMutexLock(sampleParam->testSample.mutex);
    sampleParam->testSample.isStop = true;
    MediaThreadCondSignal(sampleParam->testSample.cond);
    MediaMutexUnLock(sampleParam->testSample.mutex);
    WaitPlayerTaskEnd(sampleParam);
    MEDIA_ERR_LOG("success");
    return 0;
}

static int32_t PlaySampleAlbuminfo(SampleParam *sampleParam)
{
    if (sampleParam->player == nullptr || sampleParam->player.get() == nullptr) {
        MEDIA_ERR_LOG("player is nullptr");
        return -1;
    }

    Format albumInfo;
    albumInfo.PutStringValue(FORMAT_KEY, FORMAT_GET_ALBUM_INFO);
    int32_t ret = sampleParam->player->GetParameter(albumInfo);
    if (ret != 0) {
        MEDIA_ERR_LOG("GetAlbuminfo failed:%d", ret);
        return -1;
    }

    bool success = false;
    std::string value;
    success = albumInfo.GetStringValue(FORMAT_KEY_ALBUM, value);
    MEDIA_INFO_LOG("album info:%s", success ? value.c_str() : "unknown");
    success = albumInfo.GetStringValue(FORMAT_KEY_TITLE, value);
    MEDIA_INFO_LOG("title info:%s", success ? value.c_str() : "unknown");
    success = albumInfo.GetStringValue(FORMAT_KEY_ARTIST, value);
    MEDIA_INFO_LOG("artist info:%s", success ? value.c_str() : "unknown");
    success = albumInfo.GetStringValue(FORMAT_KEY_COMPOSER, value);
    MEDIA_INFO_LOG("composer info:%s", success ? value.c_str() : "unknown");
    success = albumInfo.GetStringValue(FORMAT_KEY_AUTHOR, value);
    MEDIA_INFO_LOG("author info:%s", success ? value.c_str() : "unknown");

    return 0;
}

static int32_t PlaySampleSetVolume(const char** argv)
{
    int32_t linearVolume = std::stoi(argv[0x1]);
    bool success = g_amInstance.SetVolume(AUDIO_STREAM_MUSIC, linearVolume);
    if (!success) {
        MEDIA_ERR_LOG("SetVolume:%d failed", linearVolume);
        return -1;
    }

    MEDIA_INFO_LOG("SetVolume:%d success", linearVolume);
    return 0;
}

static int32_t PlaySampleSetBTSCOVolume(const char** argv)
{
    int32_t scoVolume = std::stoi(argv[0x1]);
    MEDIA_INFO_LOG("set sco volume:%d", scoVolume);
    bool success = g_amInstance.SetVolume(AUDIO_STREAM_VOICE_CALL_BT_SCO, scoVolume);
    if (!success) {
        MEDIA_ERR_LOG("set sco volume failed");
    }
    scoVolume = g_amInstance.GetVolume(AUDIO_STREAM_VOICE_CALL_BT_SCO);
    MEDIA_INFO_LOG("get sco volume:%d", scoVolume);
    return 0;
}

static int32_t PlaySampleGetMusicVolume()
{
    int32_t getVolume = g_amInstance.GetVolume(AUDIO_STREAM_MUSIC);
    MEDIA_INFO_LOG("getmusicvolume:%d", getVolume);
    return 0;
}

static int32_t PlaySampleGetBTSCOVolume()
{
    int32_t getVolume = g_amInstance.GetVolume(AUDIO_STREAM_VOICE_CALL_BT_SCO);
    MEDIA_INFO_LOG("getscovolume:%d", getVolume);
    return 0;
}

static int32_t PlaySampleSingleLoopOn(SampleParam *sampleParam)
{
    if (sampleParam->player == nullptr || sampleParam->player.get() == nullptr) {
        MEDIA_ERR_LOG("player is nullptr");
        return -1;
    }
    if (sampleParam->testSample.playCompleted) {
        MEDIA_ERR_LOG("playCompleted, Enter the stop command, then play");
        return -1;
    }

    MEDIA_INFO_LOG("SingleLooping on");
    bool loop = true;
    int32_t ret = sampleParam->player->EnableSingleLooping(loop);
    if (ret != 0) {
        MEDIA_ERR_LOG("EnableSingleLooping failed ret:%d", ret);
    }
    sampleParam->isSingleLoop = sampleParam->player->IsSingleLooping();
    if (!sampleParam->isSingleLoop) {
        MEDIA_ERR_LOG("IsSingleLooping failed IsSingleLooping:%d", sampleParam->isSingleLoop);
    }
    MEDIA_INFO_LOG("IsSingleLooping:%d", sampleParam->isSingleLoop);
    return 0;
}

static int32_t PlaySampleSingleLoopOff(SampleParam *sampleParam)
{
    if (sampleParam->player == nullptr || sampleParam->player.get() == nullptr) {
        MEDIA_ERR_LOG("player is nullptr");
        return NULL;
    }
    if (sampleParam->testSample.playCompleted) {
        MEDIA_ERR_LOG("playCompleted, Enter the stop command, then play");
        return NULL;
    }
    MEDIA_INFO_LOG("SingleLooping off");
    bool loop = false;
    int32_t ret = sampleParam->player->EnableSingleLooping(loop);
    if (ret != 0) {
        MEDIA_ERR_LOG("EnableSingleLooping failed ret:%d", ret);
    }

    sampleParam->isSingleLoop = sampleParam->player->IsSingleLooping();
    if (sampleParam->isSingleLoop) {
        MEDIA_ERR_LOG("IsSingleLooping failed IsSingleLooping:%d", sampleParam->isSingleLoop);
    }
    MEDIA_INFO_LOG("IsSingleLooping:%d", sampleParam->isSingleLoop);
    return 0;
}

static int32_t PlaySampleSwitchSong(SampleParam *sampleParam)
{
    if (sampleParam->player == nullptr || sampleParam->player.get() == nullptr) {
        MEDIA_ERR_LOG("player is nullptr");
        return -1;
    }

    MediaMutexLock(sampleParam->testSample.mutex);
    sampleParam->testSample.isStop = true;
    MediaThreadCondSignal(sampleParam->testSample.cond);
    MediaMutexUnLock(sampleParam->testSample.mutex);
    WaitPlayerTaskEnd(sampleParam);
    sampleParam->taskAlive.store(true);
    if (strncmp(sampleParam->control, "prevsong", strlen("prevsong")) == 0) {
        if (g_currentSongIndex == 0) {
            g_currentSongIndex = g_totalSongEntries - 1;
        } else {
            g_currentSongIndex--;
        }
        MEDIA_INFO_LOG("switch prev song:%d/%d", g_currentSongIndex, g_totalSongEntries);
    } else {
        if (g_currentSongIndex == g_totalSongEntries - 1) {
            g_currentSongIndex = 0;
        } else {
            g_currentSongIndex++;
        }
        MEDIA_INFO_LOG("switch next song:%d/%d", g_currentSongIndex, g_totalSongEntries);
    }

    sampleParam->sourceType = SourceType::SOURCE_TYPE_URI;
    if (strlen(g_songList[g_currentSongIndex]) < PATH_MAX) {
        if (memcpy_s(sampleParam->filePath, MEDIA_PATH_MAX,
            g_songList[g_currentSongIndex], strlen(g_songList[g_currentSongIndex]) + 1) != EOK) {
            MEDIA_ERR_LOG("memcpy_s failed");
        }
    }

    sampleParam->player = std::make_shared<Player>();
    if (sampleParam->player == nullptr || sampleParam->player.get() == nullptr) {
        MEDIA_ERR_LOG("g_adapter make failed");
        return -1;
    }

    MediaThreadattr attr = { "playerTask", 0x1800, THREAD_SCHED_INVALID, 80, false };
    MediaThreadIdHandle taskHandle = MediaThreadCreate(PlayerTaskEntry, sampleParam, &attr);
    if (taskHandle == nullptr) {
        MEDIA_ERR_LOG("thread create failed");
        MediaThreadCondSignal(sampleParam->testSample.cond);
        return -1;
    }
    sampleParam->playTask = taskHandle;
    StorePlayerTaskInfo(sampleParam);
    MediaThreadCondSignal(sampleParam->testSample.cond);
    MEDIA_INFO_LOG("next/previous end");
    return 0;
}

static int32_t PlaySampleSeek(SampleParam *sampleParam, const char** argv)
{
    if (sampleParam->player == nullptr || sampleParam->player.get() == nullptr) {
        MEDIA_ERR_LOG("player is nullptr");
        return -1;
    }
    if (sampleParam->testSample.playCompleted) {
        MEDIA_ERR_LOG("playCompleted, Enter the stop command, then play");
        return -1;
    }
    MEDIA_INFO_LOG("seek");
    int64_t timeMs = std::stol(argv[0x1]);
    MEDIA_INFO_LOG("input seek time:%lld", timeMs);
    int32_t ret = sampleParam->player->Rewind(timeMs, PlayerSeekMode::PLAYER_SEEK_CLOSEST_SYNC);
    IS_OK(ret);
    MEDIA_INFO_LOG("seek end");
    return 0;
}

static int32_t PlaySampleSetAefParam(SampleParam *sampleParam, const char **argv, int32_t argc)
{
    if (sampleParam->player == nullptr || sampleParam->player.get() == nullptr) {
        MEDIA_ERR_LOG("player is nullptr");
        return -1;
    }
    if (sampleParam->testSample.playCompleted) {
        MEDIA_ERR_LOG("playCompleted, Enter the stop command, then play");
        return -1;
    }
    if (argc < 3) {
        MEDIA_ERR_LOG("The number of input parameters is insufficient.");
        return -1;
    }
    char *aefBuf = (char *)malloc(AEF_BUFFER_SIZE);
    if (aefBuf == nullptr) {
        MEDIA_ERR_LOG("malloc fail");
        return -1;
    }
    if (memset_s(aefBuf, AEF_BUFFER_SIZE, 0, AEF_BUFFER_SIZE) != EOK) {
        MEDIA_ERR_LOG("memset_s fail");
        free(aefBuf);
        return -1;
    }
    uint32_t length = static_cast<uint32_t>(std::stoi(argv[0x3]));
    uint32_t paramData = static_cast<uint32_t>(std::stoi(argv[0x2]));
    int32_t effectType = static_cast<int32_t>(std::stoi(argv[0x1]));
    int32_t prefixLen = sprintf_s(aefBuf, AEF_BUFFER_SIZE, "%d#%u:", effectType, length);
    if (prefixLen < 0) {
        MEDIA_ERR_LOG("sprintf_s failed");
        free(aefBuf);
        return -1;
    }
    char *paramNewData = reinterpret_cast<char*>(&paramData);
    if (memcpy_s(aefBuf + prefixLen, AEF_BUFFER_SIZE - prefixLen, paramNewData, length) != EOK) {
        MEDIA_ERR_LOG("memcpy_s failed");
        free(aefBuf);
        return -1;
    }
    Format param;
    CharValue value = {};
    value.data = aefBuf;
    value.len = AEF_BUFFER_SIZE;
    param.PutStringValue(FORMAT_KEY, "setaefparam");
    param.PutCharValue("setaefparam", value);
    int32_t ret = sampleParam->player->SetParameter(param);
    if (ret != 0) {
        free(aefBuf);
        return -1;
    }
    free(aefBuf);
    return 0;
}

static int32_t PlaySampleSetAefEnable(SampleParam *sampleParam, const char **argv, int32_t argc)
{
    if (sampleParam->player == nullptr || sampleParam->player.get() == nullptr) {
        MEDIA_ERR_LOG("player is nullptr");
        return -1;
    }
    if (sampleParam->testSample.playCompleted) {
        MEDIA_ERR_LOG("playCompleted, Enter the stop command, then play");
        return -1;
    }
    if (argc < 2) { /* argc must bigger than 2 */
        MEDIA_ERR_LOG("The number of input parameters is insufficient.");
        return -1;
    }
    int32_t effectType = static_cast<int32_t>(std::stoi(argv[0x1]));
    int32_t enable = static_cast<int32_t>(std::stoi(argv[0x2]));
    char buf[BUFFER_SIZE] = { 0 };
    int32_t ret = sprintf_s(buf, BUFFER_SIZE, "%d#%d", enable, effectType);
    if (ret < 0) {
        MEDIA_ERR_LOG("sprintf_s failed");
        return -1;
    }
    Format param;
    CharValue value = {};
    value.data = buf;
    value.len = BUFFER_SIZE;
    param.PutStringValue(FORMAT_KEY, "aefenable");
    param.PutCharValue("aefenable", value);
    ret = sampleParam->player->SetParameter(param);
    if (ret != 0) {
        return -1;
    }
    return 0;
}

static int32_t PlaySampleState(SampleParam *sampleParam)
{
    if (sampleParam->player == nullptr || sampleParam->player.get() == nullptr) {
        MEDIA_ERR_LOG("player is nullptr");
        return -1;
    }

    int32_t state;
    int32_t ret = sampleParam->player->GetPlayerState(state);
    if (ret != 0) {
        MEDIA_ERR_LOG("GetPlayerState failed:%d", ret);
        return -1;
    }

    MEDIA_INFO_LOG("GetPlayerState success, current state:%s",
        (state == PLAYER_STATE_ERROR) ? "Error" :
        (state == PLAYER_IDLE) ? "Idle" :
        (state == PLAYER_INITIALIZED) ? "Initialized" :
        (state == PLAYER_PREPARING) ? "Preparing" :
        (state == PLAYER_PREPARED) ? "Prepared" :
        (state == PLAYER_STARTED) ? "Playback started" :
        (state == PLAYER_PAUSED) ? "Playback paused" :
        (state == PLAYER_STOPPED) ? "Playback stopped" :
        (state == PLAYER_PLAYBACK_COMPLETE) ? "Playback completed" : "unknown");
    return 0;
}

static int32_t PlaySampleGetPlayerDumpInfo(SampleParam *sampleParam)
{
    if (sampleParam->player == nullptr || sampleParam->player.get() == nullptr) {
        MEDIA_ERR_LOG("player is nullptr");
        return -1;
    }
    PlayerDebugInfo *playerInfo = new PlayerDebugInfo();
    int32_t ret = sampleParam->player->DumpInfo(playerInfo);
    if (ret != 0) {
        delete playerInfo;
        MEDIA_ERR_LOG("player get DumpInfo failed:%d", ret);
        return -1;
    }
    delete playerInfo;
    return 0;
}

static int32_t PlayerSampleInit(SampleParam **sampleParam, int32_t argc, const char** argv, SourceType &sourceType)
{
    for (int32_t i = 0; i < argc; i++) {
        MEDIA_INFO_LOG(" argv[%d]:%s", i, argv[i]);
    }
    if (argc < 1) {
        return -1;
    }
    if (sampleParam == nullptr) {
        MEDIA_ERR_LOG("sampleParam is nullptr");
        return -1;
    }
    if (*sampleParam == nullptr) {
        *sampleParam = (SampleParam *)malloc(sizeof(SampleParam));
        if (*sampleParam == nullptr) {
            MEDIA_ERR_LOG("malloc failed");
            return -1;
        }
        (*sampleParam)->taskAlive.store(false);
        if (memset_s(*sampleParam, sizeof(SampleParam), 0, sizeof(SampleParam)) != EOK) {
            MEDIA_ERR_LOG("memset_s failed");
        }
        (*sampleParam)->isSingleLoop = false;
    }

    if (sourceType == SourceType::SOURCE_TYPE_STREAM) {
        if (CheckInputParam(*sampleParam, argc, argv) < 0) {
            free(*sampleParam);
            *sampleParam = nullptr;
            return -1;
        }
    }
    return 0;
}

static int32_t HandlePlayerSampleCmd(SampleParam *sampleParam, const char** argv, int32_t argc)
{
    if (sampleParam == nullptr) {
        MEDIA_ERR_LOG("sampleParam is nullptr!");
        return -1;
    }
    uint32_t funId = ConvertFuncId(sampleParam);
    switch (funId) {
        case MEDIA_AUDIO_PLAY_PAUSE:
            return PlaySamplePause(sampleParam);
        case MEDIA_AUDIO_PLAY_RESUME:
            return PlaySampleResume(sampleParam);
        case MEDIA_AUDIO_PLAY_STOP:
            return PlaySampleStop(sampleParam);
        case MEDIA_AUDIO_PLAY_ALBUM_INFO:
            return PlaySampleAlbuminfo(sampleParam);
        case MEDIA_AUDIO_PLAY_SET_VOLUME:
            return PlaySampleSetVolume(argv);
        case MEDIA_AUDIO_PLAY_GET_MUSIC_VOLUME:
            return PlaySampleGetMusicVolume();
        case MEDIA_AUDIO_PLAY_SET_BT_SCO_VOLUME:
            return PlaySampleSetBTSCOVolume(argv);
        case MEDIA_AUDIO_PLAY_GET_BT_SCO_VOLUME:
            return PlaySampleGetBTSCOVolume();
        case MEDIA_AUDIO_PLAY_SINGLE_LOOP_ON:
            return PlaySampleSingleLoopOn(sampleParam);
        case MEDIA_AUDIO_PLAY_SINGLE_LOOP_OFF:
            return PlaySampleSingleLoopOff(sampleParam);
        case MEDIA_AUDIO_PLAY_SWITCH_SONG_PREV:
        case MEDIA_AUDIO_PLAY_SWITCH_SONG_NEXT:
            return PlaySampleSwitchSong(sampleParam);
        case MEDIA_AUDIO_PLAY_SEEK:
            return PlaySampleSeek(sampleParam, argv);
        case MEDIA_AUDIO_PLAY_CURRENT_STATE:
            return PlaySampleState(sampleParam);
        case MEDIA_AUDIO_PLAY_DUMP_INFO:
            return PlaySampleGetPlayerDumpInfo(sampleParam);
        case MEDIA_AUDIO_PLAY_SET_AEF_PARAM:
            return PlaySampleSetAefParam(sampleParam, argv, argc);
        case MEDIA_AUDIO_PLAY_AEF_ENABLE:
            return PlaySampleSetAefEnable(sampleParam, argv, argc);
        default:
            MEDIA_ERR_LOG("input cmd:%s invaild", sampleParam->control);
            return 0;
    }
}

static void WaitPlayerTaskEnd(SampleParam *sampleParam)
{
    MediaInitStaticMutexLock(&g_playerTaskMutex);
    MediaMutexLock(g_playerTaskMutex);
    for (size_t i = 0; i < MEDIA_MAX_PLAYER_TASK; i++) {
        if (g_playerTask[i].playTask == nullptr ||
            g_playerTask[i].streamType == AUDIO_STREAM_INVALID) {
            continue;
        }
        if (sampleParam->playTask == g_playerTask[i].playTask &&
            sampleParam->streamType == g_playerTask[i].streamType &&
            sampleParam->testSample.isStop) {
            MediaMutexUnLock(g_playerTaskMutex);
            MediaThreadJoin(&sampleParam->playTask);
            MediaMutexLock(g_playerTaskMutex);
            g_playerTask[i].streamType = AUDIO_STREAM_INVALID;
            g_playerTask[i].playTask = nullptr;
            break;
        }
    }
    MediaMutexUnLock(g_playerTaskMutex);
}

static bool FindAndUpdateplayerTask(SampleParam *sampleParam)
{
    for (size_t i = 0; i < MEDIA_MAX_PLAYER_TASK; i++) {
        if (g_playerTask[i].playTask == sampleParam->playTask ||
            g_playerTask[i].streamType == sampleParam->streamType) {
            g_playerTask[i].streamType = sampleParam->streamType;
            g_playerTask[i].playTask = sampleParam->playTask;
            return true;
        }
    }
    return false;
}

static void StorePlayerTaskInfo(SampleParam *sampleParam)
{
    MediaInitStaticMutexLock(&g_playerTaskMutex);
    MediaMutexLock(g_playerTaskMutex);
    if (FindAndUpdateplayerTask(sampleParam)) {
        MediaMutexUnLock(g_playerTaskMutex);
        return;
    }
    for (size_t i = 0; i < MEDIA_MAX_PLAYER_TASK; i++) {
        if (g_playerTask[i].playTask == nullptr &&
            g_playerTask[i].streamType == AUDIO_STREAM_INVALID) {
            g_playerTask[i].streamType = sampleParam->streamType;
            g_playerTask[i].playTask = sampleParam->playTask;
            break;
        }
    }
    MediaMutexUnLock(g_playerTaskMutex);
}

static void GetRealPath(SampleParam *sampleParam, int32_t argc, const char **argv)
{
    bool isFound = false;
    for (uint32_t i = 0; i < ARRAY_SIZE(g_httpPathlist); i++) {
        bool matchCmd = (strncasecmp(argv[1], g_httpPathlist[i].key, strlen(g_httpPathlist[i].key)) == 0);
        if (matchCmd) {
            if (memcpy_s(sampleParam->filePath, MEDIA_PATH_MAX, g_httpPathlist[i].value,
                strlen(g_httpPathlist[i].value) + 1) != EOK) {
                MEDIA_ERR_LOG("memcpy_s failed");
            }
            isFound = true;
            break;
        }
    }
    if (!isFound) {
        if (memcpy_s(sampleParam->filePath, MEDIA_PATH_MAX, argv[0x1], strlen(argv[0x1]) + 1) != EOK) {
            MEDIA_ERR_LOG("memcpy_s failed");
        }
    }
}

int32_t PlayerSample(int32_t argc, const char** argv)
{
    SourceType sourceType = SourceType::SOURCE_TYPE_URI;
    int32_t ret = PlayerSampleInit(&g_sampleParam, argc, argv, sourceType);
    if (ret != 0) {
        return ret;
    }
    if (memcpy_s(g_sampleParam->control, sizeof(g_sampleParam->control),
        argv[0], strlen(argv[0]) + 1) != EOK) {
        MEDIA_ERR_LOG("memcpy_s failed, g_sampleParam->control:%s", g_sampleParam->control);
    }
    if (strncmp(g_sampleParam->control, "xx", strlen("xx")) == 0) {
        if (g_sampleParam->taskAlive.load()) {
            MEDIA_ERR_LOG("playerTask exists, Enter the stop command first");
            return -1;
        }
        g_sampleParam->taskAlive.store(true);
        if (argc < 4) { /* 4: invalid argc */
            goto FREE;
        }
        g_sampleParam->streamType = AUDIO_STREAM_MUSIC;
        g_sampleParam->sourceType = sourceType;
        g_sampleParam->testSample = g_sample;
        g_sampleParam->player = std::make_shared<Player>();
        if (g_sampleParam->player == nullptr || g_sampleParam->player.get() == nullptr) {
            MEDIA_ERR_LOG("player make failed");
            goto FREE;
        }

        if (strlen(argv[0x1]) < PATH_MAX) {
            GetRealPath(g_sampleParam, argc, argv);
        } else {
            MEDIA_ERR_LOG("input path too long");
            goto FREE;
        }
        WaitPlayerTaskEnd(g_sampleParam);
        MediaThreadattr attr = { "playerTask", 0x1800, THREAD_SCHED_INVALID, 80, false };
        MediaThreadIdHandle taskHandle = MediaThreadCreate(PlayerTaskEntry, g_sampleParam, &attr);
        if (taskHandle == nullptr) {
            MEDIA_ERR_LOG("thread create failed");
            MediaThreadCondSignal(g_sampleParam->testSample.cond);
            goto FREE;
        }
        g_sampleParam->playTask = taskHandle;
        StorePlayerTaskInfo(g_sampleParam);
        MediaThreadCondSignal(g_sampleParam->testSample.cond);
    } else {
        ret = HandlePlayerSampleCmd(g_sampleParam, argv, argc);
    }
    MEDIA_INFO_LOG("PlayerSample end");
    return ret;

FREE:
    free(g_sampleParam);
    g_sampleParam = nullptr;
    return -1;
}

int32_t TonePlayerSampleTest(int32_t argc, const char *argv[])
{
    SourceType sourceType = SourceType::SOURCE_TYPE_URI;
    int32_t ret = PlayerSampleInit(&g_tonePlayersampleParam, argc, argv, sourceType);
    if (ret != 0) {
        return ret;
    }
    if (memcpy_s(g_tonePlayersampleParam->control, sizeof(g_tonePlayersampleParam->control),
        argv[0], strlen(argv[0]) + 1) != EOK) {
        MEDIA_ERR_LOG("memcpy_s failed, g_tonePlayersampleParam->control:%s", g_tonePlayersampleParam->control);
    }
    if (strncmp(g_tonePlayersampleParam->control, "xx", strlen("xx")) == 0) {
        if (g_tonePlayersampleParam->taskAlive.load()) {
            MEDIA_ERR_LOG("playerTask exists, Enter the stop command first");
            return -1;
        }
        g_tonePlayersampleParam->taskAlive.store(true);
        if (argc < 4) { /* 4: invalid argc */
            goto FREE;
        }
        g_tonePlayersampleParam->streamType = AUDIO_STREAM_NOTIFICATION_SYSTEM;
        g_tonePlayersampleParam->sourceType = sourceType;
        g_tonePlayersampleParam->testSample = g_tonePlayersample;
        g_tonePlayersampleParam->player = std::make_shared<Player>();
        if (g_tonePlayersampleParam->player == nullptr || g_tonePlayersampleParam->player.get() == nullptr) {
            MEDIA_ERR_LOG("player make failed");
            goto FREE;
        }

        if (strlen(argv[0x1]) < PATH_MAX) {
            GetRealPath(g_tonePlayersampleParam, argc, argv);
        } else {
            MEDIA_ERR_LOG("input path too long");
            goto FREE;
        }
        WaitPlayerTaskEnd(g_tonePlayersampleParam);
        MediaThreadattr attr = { "tonePlayerTask", 0x1800, THREAD_SCHED_INVALID, 80, false };
        MediaThreadIdHandle taskHandle = MediaThreadCreate(PlayerTaskEntry, g_tonePlayersampleParam, &attr);
        if (taskHandle == nullptr) {
            MEDIA_ERR_LOG("thread create failed");
            MediaThreadCondSignal(g_sampleParam->testSample.cond);
            goto FREE;
        }
        g_tonePlayersampleParam->playTask = taskHandle;
        StorePlayerTaskInfo(g_tonePlayersampleParam);
        MediaThreadCondSignal(g_tonePlayersampleParam->testSample.cond);
    } else {
        ret = HandlePlayerSampleCmd(g_tonePlayersampleParam, argv, argc);
    }
    MEDIA_INFO_LOG("tonePlayerSample end");
    return ret;

FREE:
    free(g_tonePlayersampleParam);
    g_tonePlayersampleParam = nullptr;
    return -1;
}

#ifdef __cplusplus
};
#endif

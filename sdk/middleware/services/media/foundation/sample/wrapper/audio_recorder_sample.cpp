/*
* Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
* Description: audio recorder sample
* Author: Media Software Group
* Create: 2024-07-30
*/

#include "audio_recorder_sample.h"
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
#include "securec.h"
#include "audio_manager.h"
#include "media_log.h"
#include "media_thread_adapt.h"
#ifdef SUPPORT_AUDIO_RECORDER
#include "audio_recorder.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
#ifdef SUPPORT_AUDIO_RECORDER
using namespace::Audio;
using Audio::AudioManager;
const uint64_t RECORDER_CONTROL_CMD_LEN = 30;
const uint64_t MEDIA_PATH_MAX = 260;
const uint64_t PER_FILE_STORAGE_MAX_DURATION = 60;
const uint64_t DEFAULTE_MAX_STORAGE_THEARHOLD = 90;
const uint64_t PER_FILE_STORAGE_MAX_SIZE = 2ULL * 1024 * 1024 * 1024;
static AudioManager &g_amInstance = AudioManager::GetInstance();

#define TEST_SAMPLE_RETURN_VAL_IF_NULL(condition)                                           \
    do {                                                                               \
        if ((condition) == nullptr) {                                                  \
            MEDIA_ERR_LOG("" #condition " is nullptr error");    \
            return -1;                                                         \
        }                                                                              \
    } while (0)

typedef struct {
    char outDir[MEDIA_PATH_MAX];
    AudioSourceType inputSource;
    AudioCodecFormat audioFormat;
    int32_t sampleRate;
    int32_t channelCount;
    AudioStreamType streamType;
    AudioBitWidth bitWidth;
    AudioSession sessionID;
    RecorderCallback callback;
    void *cookie;
    uint32_t splitFileDuration;
    uint64_t splitFileSize;
    bool enableOverwriteFiles;
    uint32_t storageWarningThresholdPercent;
} AudioRecordInfo;

typedef struct TagTestSample {
    char    filePath[MEDIA_PATH_MAX];
    char control[RECORDER_CONTROL_CMD_LEN];
    bool needStop;
    AudioRecorderHandle recorderHandle;
    AudioInterrupt interrupt;
    MediaThreadIdHandle audioRecordProcess;
    AudioRecordInfo audioRecordInfo;
    bool isRecording;
    MediaMutexHandle mutex;
    MediaThreadCondHandle cond;
    uint32_t splitFileDuration;
    uint64_t splitFileSize;
    bool enableOverwriteFiles;
    uint32_t storageWarningThresholdPercent;
} TestSample;

typedef int32_t (*InvokeFunc)(int32_t argc, const char *argv[]);

typedef struct {
    const char *cmdName;
    InvokeFunc invoke;
} RecordTestInvoke;

static bool g_interruptHintStop = false;
static TestSample g_sample = {
    .splitFileDuration = PER_FILE_STORAGE_MAX_DURATION,
    .splitFileSize = PER_FILE_STORAGE_MAX_SIZE,
    .enableOverwriteFiles = false,
    .storageWarningThresholdPercent = DEFAULTE_MAX_STORAGE_THEARHOLD,
};

struct NameAudioFormat {
    std::string name;
    AudioCodecFormat audioFormat;
};

static NameAudioFormat g_audioCodecFormat[] = {
    {"mp3", MP3},
    {"opus", OPUS},
    {"pcm", PCM},
    {"silk", SILK},
};

class AudioRecorderInterruptListener : public InterruptListener {
    public:
    AudioRecorderInterruptListener() {};
    ~AudioRecorderInterruptListener() override {};

    void OnInterrupt(int32_t type, int32_t hint) override
    {
        MEDIA_INFO_LOG("AudioRecorderInterruptListener OnInterrupt, type:%d, hint:%d", type, hint);

        if (type == INTERRUPT_TYPE_BEGIN && hint == INTERRUPT_HINT_PAUSE) {
            MEDIA_INFO_LOG("OnInterrupt pause not supported");
        }
        if (type == INTERRUPT_TYPE_END && hint == INTERRUPT_HINT_RESUME) {
            MEDIA_INFO_LOG("OnInterrupt resume not supported");
        }
        if (type == INTERRUPT_TYPE_BEGIN && hint == INTERRUPT_HINT_STOP) {
            g_interruptHintStop = true;
        }
        if (type == INTERRUPT_TYPE_END && hint == INTERRUPT_HINT_STOP) {
            MEDIA_INFO_LOG("OnInterrupt stop not supported");
        }
    }
};

static void AudioCaptureGetFormat(const char **argv, AudioCodecFormat &audioFormat)
{
    std::string inputFormat = std::string(argv[0x1]);
    for (size_t i = 0; i < (sizeof(g_audioCodecFormat) / sizeof(g_audioCodecFormat[0])); i++) {
        if (inputFormat == g_audioCodecFormat[i].name) {
            audioFormat = g_audioCodecFormat[i].audioFormat;
            MEDIA_INFO_LOG("[AudioCaptureSample] set audioFormat %s", g_audioCodecFormat[i].name.c_str());
        }
    }
}

static int32_t AudioRecordGetInfo(const char **argv, AudioRecordInfo *audioRecordInfo)
{
    AudioCaptureGetFormat(argv, audioRecordInfo->audioFormat);
    audioRecordInfo->sampleRate = atoi(argv[0x2]); /* 2: samplerate */
    MEDIA_INFO_LOG("set capture parameter sampleRate:%d", audioRecordInfo->sampleRate);
    audioRecordInfo->channelCount = atoi(argv[0x3]); /* 3: channel */
    MEDIA_INFO_LOG("set capture parameter channel:%d", audioRecordInfo->channelCount);
    audioRecordInfo->bitWidth = (AudioBitWidth)atoi(argv[0x4]); /* 4: bitWidth */
    MEDIA_INFO_LOG("set capture parameter bitWidth:%d", audioRecordInfo->bitWidth);
    MEDIA_INFO_LOG("input argv[5]:%s.", argv[0x5]); /* 5: filePath set argv[3] */
    if (strlen(argv[0x5]) >= MEDIA_PATH_MAX) {
        MEDIA_ERR_LOG("input path too long");
        return MEDIA_ERR;
    }
    if (memcpy_s(audioRecordInfo->outDir, MEDIA_PATH_MAX, argv[0x5], strlen(argv[0x5]) + 1) != EOK) {
        MEDIA_ERR_LOG("memcpy_s failed");
        return MEDIA_ERR;
    }
    return MEDIA_OK;
}

static void AudioRecordEeventHandle(uint32_t enEvent, const void *data, const void *cookie)
{
    MEDIA_UNUSED(data);
    TestSample *sample = (TestSample *)cookie;
    if (enEvent == AUDIO_RECORD_STORAGE_REACHE_MAX_VALUE ||
        enEvent == AUDIO_RECORD_EVENT_ERROR) {
        MediaMutexLock(sample->mutex);
        sample->isRecording = false;
        MediaMutexUnLock(sample->mutex);
        MediaThreadCondSignal(sample->cond);
    } else {
        MEDIA_ERR_LOG("not support envent type!");
    }
}

static int32_t ActivateAudioInterrupt(TestSample *sample, AudioSession *sessionId)
{
    bool init = g_amInstance.Initialize();
    if (!init) {
        MEDIA_ERR_LOG("audiomanager init failed");
        return MEDIA_ERR;
    }
    *sessionId = g_amInstance.MakeSessionId();
    if (*sessionId == AUDIO_SESSION_ID_NONE) {
        MEDIA_ERR_LOG("AUDIO_SESSION_ID_NONE");
        return MEDIA_ERR;
    }
    sample->mutex = MediaMutexCreate(nullptr);
    if (sample->mutex == nullptr) {
        MEDIA_ERR_LOG("create mutex failed");
        return MEDIA_ERR;
    }
    sample->cond = MediaThreadCondCreate();
    if (sample->cond == nullptr) {
        MEDIA_ERR_LOG("create cond failed");
        (void)MediaMutexDestroy(&sample->mutex);
        return MEDIA_ERR;
    }
    std::shared_ptr<AudioRecorderInterruptListener> recorderInterruptListener =
        std::make_shared<AudioRecorderInterruptListener>();
    sample->interrupt = { AUDIO_STREAM_VOICE_RECORD, *sessionId, recorderInterruptListener };
    int32_t ret = g_amInstance.ActivateAudioInterrupt(sample->interrupt);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("ActivateAudioInterrupt failed");
        (void)MediaMutexDestroy(&sample->mutex);
        (void)MediaThreadCondDestroy(&sample->cond);
        return MEDIA_ERR;
    }
    return MEDIA_OK;
}

static int32_t CheckInputParam(int32_t argc)
{
    if (argc < 0x5) {
        MEDIA_ERR_LOG("argc:%d is invalid, must be 5 parameters", argc);
        return -1;
    }
    return 0;
}

static void ConvertRecordInfo(CapturerInputConfig *current, const AudioRecordInfo *src)
{
    current->audioFormat = src->audioFormat;
    current->sampleRate = src->sampleRate;
    current->channelCount = src->channelCount;
    current->streamType = src->streamType;
    current->bitWidth = src->bitWidth;
    current->sessionID = src->sessionID;
    current->linkDir = AUDIO_UP_LINK;
}

static errcode_t AudioRecorderAllPrepare(AudioRecorderHandle handle, const AudioRecordInfo *recordInfo)
{
    errcode_t ret = AudioRecorderPrepare(handle);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("audio recorder prepare failed!");
        return ret;
    }
    ret = AudioRecorderEnableOverwriteFiles(handle, recordInfo->enableOverwriteFiles);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("audio recorder enable over write files failed!");
        return ret;
    }
    ret = AudioRecorderSetStorageWarningThreshold(handle, recordInfo->storageWarningThresholdPercent);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("audio recorder set storage warning threshold failed!");
        return ret;
    }
    ret = AudioRecorderSetMaxDuration(handle, recordInfo->splitFileDuration);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("audio recorder set max duration failed!");
        return ret;
    }
    ret = AudioRecorderSetMaxFileSize(handle, recordInfo->splitFileSize);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("audio recorder set max file size failed!");
        return ret;
    }
    return ret;
}

static errcode_t AudioRecorderServiceStart(AudioRecorderHandle *handle, const AudioRecordInfo *recordInfo)
{
    if (recordInfo == nullptr) {
        MEDIA_ERR_LOG("recordInfo is nullptr!");
        return MEDIA_ERR;
    }
    CapturerInputConfig capturerInfo = {};
    ConvertRecordInfo(&capturerInfo, recordInfo);
    errcode_t ret = AudioRecorderCreate(handle);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("AudioRecorderCreate failed!");
        return MEDIA_ERR;
    }
    ret = AudioRecorderSetInfo(*handle, &capturerInfo);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("audio recorder set info failed!");
        goto EXIT;
    }
    ret = AudioRecorderSetOutputPath(*handle, recordInfo->outDir, strlen(recordInfo->outDir) + 1);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("audio recorder set output path failed!");
        goto EXIT;
    }
    ret = AudioRecorderAllPrepare(*handle, recordInfo);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("audio recorder all prepare failed!");
        goto EXIT;
    }
    ret = AudioRecorderSetCallback(*handle, recordInfo->callback, recordInfo->cookie);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("audio recorder set max call back failed!");
        goto EXIT;
    }
    ret = AudioRecorderStart(*handle);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("audio recorder set max start failed!");
        goto EXIT;
    }
    return MEDIA_OK;
EXIT:
    AudioRecorderDestroy(*handle);
    return MEDIA_ERR;
}

static errcode_t AudioRecorderServiceStop(const AudioRecorderHandle handle)
{
    errcode_t ret = AudioRecorderStop(handle);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("audio recorder stop failed!");
    }
    ret = AudioRecorderDestroy(handle);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("audio recorder destroy failed!");
    }
    return ret;
}

static void *AudioRecordProcess(void *arg)
{
    TestSample *sample = (TestSample *)arg;
    AudioSession sessionId = AUDIO_SESSION_ID_NONE;
    int32_t ret = ActivateAudioInterrupt(sample, &sessionId);
    if (ret != MEDIA_OK) {
        return nullptr;
    }
    sample->audioRecordInfo.sessionID = sessionId;
    sample->audioRecordInfo.inputSource = AUDIO_MIC;
    sample->audioRecordInfo.streamType = AUDIO_STREAM_VOICE_RECORD;
    sample->audioRecordInfo.callback = AudioRecordEeventHandle;
    sample->audioRecordInfo.cookie = sample;
    sample->audioRecordInfo.splitFileDuration = sample->splitFileDuration;
    sample->audioRecordInfo.splitFileSize = sample->splitFileSize;
    sample->audioRecordInfo.enableOverwriteFiles = sample->enableOverwriteFiles;
    sample->audioRecordInfo.storageWarningThresholdPercent = sample->storageWarningThresholdPercent;
    ret = AudioRecorderServiceStart(&sample->recorderHandle, &sample->audioRecordInfo);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("AudioRecordStart failed!");
        goto EXIT;
    }
    while (true) {
        MediaMutexLock(sample->mutex);
        if (!sample->isRecording) {
            MediaMutexUnLock(sample->mutex);
            break;
        }
        MediaThreadCondWait(sample->cond, sample->mutex);
        MediaMutexUnLock(sample->mutex);
    }
    AudioRecorderServiceStop(sample->recorderHandle);
EXIT:
    MediaMutexDestroy(&sample->mutex);
    MediaThreadCondDestroy(&sample->cond);
    g_sample.isRecording = false;
    (void)g_amInstance.DeactivateAudioInterrupt(sample->interrupt);
    MEDIA_INFO_LOG("AudioRecord exit success!");
    return nullptr;
}

int32_t SampleAudioRecorderStart(int32_t argc, const char **argv)
{
    int32_t ret = CheckInputParam(argc);
    if (ret != MEDIA_OK) {
        return ret;
    }
    if (g_sample.isRecording) {
        MEDIA_INFO_LOG("audio capture is running, please input control cmd");
        return MEDIA_ERR;
    }
    MediaThreadattr attr = { "AudioRecordTask", 0x1000, THREAD_SCHED_INVALID, 0, false };
    g_sample.isRecording = true;
    if (memset_s(g_sample.filePath, MEDIA_PATH_MAX, 0, MEDIA_PATH_MAX) != EOK) {
        MEDIA_ERR_LOG("memset_s failed");
        goto EXIT;
    }
    ret = AudioRecordGetInfo(argv, &g_sample.audioRecordInfo);
    if (ret != MEDIA_OK) {
        goto EXIT;
    }
    g_sample.audioRecordProcess = MediaThreadCreate(AudioRecordProcess, &g_sample, &attr);
    if (g_sample.audioRecordProcess == nullptr) {
        MEDIA_ERR_LOG("thread create failed");
        goto EXIT;
    }
    return MEDIA_OK;
EXIT:
    g_sample.isRecording = false;
    return MEDIA_ERR;
}

int32_t SampleAudioRecorderStop(int32_t argc, const char **argv)
{
    MEDIA_UNUSED(argc);
    MEDIA_UNUSED(argv);
    MediaMutexLock(g_sample.mutex);
    if (!g_sample.isRecording) {
        MEDIA_INFO_LOG("audio record has been stoped, not need to stop!");
        MediaMutexUnLock(g_sample.mutex);
        return MEDIA_OK;
    }
    g_sample.isRecording = false;
    MediaThreadCondSignal(g_sample.cond);
    MediaMutexUnLock(g_sample.mutex);
    MediaThreadJoin(&g_sample.audioRecordProcess);
    g_sample.audioRecordProcess = nullptr;
    MEDIA_INFO_LOG("stop command excute success");
    MediaMutexUnLock(g_sample.mutex);
    return MEDIA_OK;
}

int32_t SampleSetDeleteFileEnable(int32_t argc, const char **argv)
{
    if (argc < 2) {
        MEDIA_ERR_LOG("argc:%d is invalid, must be 2 parameters", argc);
        return MEDIA_INVALID_PARAM;
    }
    bool isNeedDeleteFile = true;
    uint32_t value = strtoul(argv[1], NULL, 0xA);
    if (value == 0) {
        isNeedDeleteFile = false;
    }
    g_sample.enableOverwriteFiles = isNeedDeleteFile;
    return MEDIA_OK;
}

int32_t SampleGetFileRemainTime(int32_t argc, const char **argv)
{
    uint32_t time = 0;
    int32_t ret = AudioRecorderGetRemainingTime(g_sample.recorderHandle,
        AUDIO_RECORD_CURRENT_FILE_REMAIN_TIME, &time);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("Get file Remaining Time failed!");
        return ret;
    }
    MEDIA_INFO_LOG("Get file Remaining Time %u!", time);
    return MEDIA_OK;
}

int32_t SampleGetStorageRemainTime(int32_t argc, const char **argv)
{
    uint32_t time = 0;
    int32_t ret = AudioRecorderGetRemainingTime(g_sample.recorderHandle,
        AUDIO_RECORD_CURRENT_STORAGE_REMAIN_TIME, &time);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("get storage remaining time failed!");
        return ret;
    }
    MEDIA_INFO_LOG("get storage remaining Time %u!", time);
    return MEDIA_OK;
}

int32_t SampleSetMaxStorageThreshold(int32_t argc, const char **argv)
{
    if (argc < 2) {
        MEDIA_ERR_LOG("argc:%d is invalid, must be 2 parameters", argc);
        return MEDIA_INVALID_PARAM;
    }
    g_sample.storageWarningThresholdPercent = strtoull(argv[1], NULL, 0xA);
    return MEDIA_OK;
}

int32_t SampleSplitFileDuration(int32_t argc, const char **argv)
{
    if (argc < 2) {
        MEDIA_ERR_LOG("argc:%d is invalid, must be 2 parameters", argc);
        return MEDIA_INVALID_PARAM;
    }
    g_sample.splitFileDuration = strtoul(argv[1], NULL, 0xA);
    return MEDIA_OK;
}

int32_t SampleSplitFileSize(int32_t argc, const char **argv)
{
    if (argc < 2) {
        MEDIA_ERR_LOG("argc:%d is invalid, must be 2 parameters", argc);
        return MEDIA_INVALID_PARAM;
    }
    g_sample.splitFileSize = strtoull(argv[1], NULL, 0xA);
    return MEDIA_OK;
}

int32_t SampleGetFileInfo(int32_t argc, const char **argv)
{
    MEDIA_UNUSED(argc);
    MEDIA_UNUSED(argv);
    RecordFileInfo info = {};
    int32_t ret = AudioRecorderGetFileInfo(g_sample.recorderHandle, &info);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("get file info failed!");
        return ret;
    }
    MEDIA_INFO_LOG("current file name[ %s ]!", info.fileName);
    MEDIA_INFO_LOG("current file size[ %llu ]!", info.fileSize);
    MEDIA_INFO_LOG("current file date[ %04u-%02u-%02u,%02u:%02u:%02u ]!", info.fileDateYear,
        info.fileDateMonth, info.fileDateDay, info.fileDateHour, info.fileDateMin, info.fileDateSec);
    MEDIA_INFO_LOG("current file duration[ %u ]!", info.fileDuration);
    return MEDIA_OK;
}

static RecordTestInvoke g_recordInvokeFuncList[] = {
    {"start", SampleAudioRecorderStart},
    {"stop", SampleAudioRecorderStop},
    {"deletefile", SampleSetDeleteFileEnable},
    {"getfileremaintime", SampleGetFileRemainTime},
    {"getstorageremaintime", SampleGetStorageRemainTime},
    {"maxstoragethreshold", SampleSetMaxStorageThreshold},
    {"splitfileduration", SampleSplitFileDuration},
    {"splitfilesize", SampleSplitFileSize},
    {"getinfo", SampleGetFileInfo},
};
#endif

int32_t AudioRecorderSample(int32_t argc, const char *argv[])
{
#ifdef SUPPORT_AUDIO_RECORDER
    TEST_SAMPLE_RETURN_VAL_IF_NULL(argv);
    MEDIA_INFO_LOG("enter");
    if (argc < 1) {
        MEDIA_ERR_LOG("at least one parameter is required!");
        return MEDIA_INVALID_PARAM;
    }
    for (int32_t i = 0; i < argc; i++) {
        MEDIA_INFO_LOG("argv[%d]: %s\n", i, argv[i]);
    }
    size_t size = sizeof(g_recordInvokeFuncList) / sizeof(RecordTestInvoke);
    InvokeFunc invokeFunc = nullptr;
    for (size_t i = 0; i < size; i++) {
        if (strcmp(argv[0], g_recordInvokeFuncList[i].cmdName) == 0) {
            invokeFunc = g_recordInvokeFuncList[i].invoke;
            break;
        }
    }
    if (invokeFunc == nullptr) {
        MEDIA_ERR_LOG("not support cmd!");
        return MEDIA_INVALID_PARAM;
    }
    return invokeFunc(argc, argv);
#else
    MEDIA_UNUSED(argv);
    MEDIA_UNUSED(argc);
    return 0;
#endif
}

#ifdef __cplusplus
};
#endif
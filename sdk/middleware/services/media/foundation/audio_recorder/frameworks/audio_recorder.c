/*
 * Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
 * Description: audio recorder define
 * Author: Media Software Group
 * Create: 2025-03-15
 */

#include "audio_recorder.h"
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include "securec.h"
#include "audio_recorder_muxer.h"
#ifdef SUPPORT_AUDIO_STREAM_IN_C
#include "audio_stream_in.h"
#else
#include "audio_manager_c_wrapper.h"
#endif
#include "media_log.h"
#include "media_thread_adapt.h"
#if defined(SUPPORT_FEATURE_CONFIG)
#include "feature_config.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
#define AUDIO_RECORDER_MODE_NAME "AudioRecorder"
#define AUDIO_RECORDER_ERR_LOG(fmt, ...) MEDIA_ERR_LOG(fmt, ##__VA_ARGS__)
#define AUDIO_RECORDER_INFO_LOG(fmt, ...) MEDIA_INFO_LOG(fmt, ##__VA_ARGS__)
#define AUDIO_RECORDER_MUTEX_CREATE(attr) MediaMutexCreate(attr)
#define AUDIO_RECORDER_MUTEX_LOCK(lock) MediaMutexLock(lock)
#define AUDIO_RECORDER_MUTEX_UNLOCK(lock) MediaMutexUnLock(lock)
#define AUDIO_RECORDER_INIT_STATIC_MUTEX_LOCK(mutex) MediaInitStaticMutexLock(mutex)
#define AUDIO_RECORDER_MUTEX_DESTROY(mutex) MediaMutexDestroy(mutex)
#define AUDIO_RECORDER_THREAD_CREATE(func, argument, attr) MediaThreadCreate(func, argument, attr)
#define AUDIO_RECORDER_THREAD_COND_CREATE()   MediaThreadCondCreate()
#define AUDIO_RECORDER_THREAD_COND_SIGNAL(condHandle)   MediaThreadCondSignal(condHandle)
#define AUDIO_RECORDER_THREAD_COND_TIME_WAIT(condHandle, mutexHandle, delayUs) \
    MediaThreadCondTimeWait(condHandle, mutexHandle, delayUs)
#define AUDIO_RECORDER_THREAD_COND_DESTROY(condHandle) MediaThreadCondDestroy(condHandle)
#define AUDIO_RECORDER_THREAD_JOIN(thdId) MediaThreadJoin(thdId)
#define AUDIO_MAX_MEMOR_THRESHOLD_PER 100
#define OPUS_ENCODE_HEADER_SIZE 8
#define AUDIO_RECORDER_FILE_INFO_SIZE 350
#define WAV_MAX_DATA_BUFFER_SIZE (16 * 1024)
#define OGG_MAX_DATA_BUFFER_SIZE (28)
#define MP3_MAX_DATA_BUFFER_SIZE (2 * 1024)
#define SILK_MAX_DATA_BUFFER_SIZE (256)

#define AUDIO_RECORD_CHECK_NULL_RETURN(ptr, ret, outStr) \
do { \
    if ((ptr) == NULL) { \
        AUDIO_RECORDER_ERR_LOG("%s ", ((outStr) != NULL) ? (outStr) : " "); \
        return (ret); \
    } \
} while (0)

#define AUDIO_RECORD_CHECK_FAILED_RETURN(ptr, ret, outStr) \
do { \
    if (!(ptr)) { \
        AUDIO_RECORDER_ERR_LOG("%s ", ((outStr) != NULL) ? (outStr) : " "); \
        return (ret); \
    } \
} while (0)

#define AUDIO_RECORD_CHECK_FAILED_RETURN_WITH_UNLOCK(value, target, ret, printfString, mutex) \
do { \
    if ((value) != (target)) { \
        AUDIO_RECORDER_ERR_LOG("%s, ret:%d", (printfString) ? (printfString) : " ", ret); \
        AUDIO_RECORDER_MUTEX_UNLOCK((mutex)); \
        return ret; \
    } \
} while (0)

#define AUDIO_RECORD_CHECK_COND_RETURN(value, ret, outStr) \
do { \
    if ((value)) { \
        AUDIO_RECORDER_ERR_LOG("%s ret: %d", ((outStr) != NULL) ? (outStr) : " ", (value)); \
        return (ret); \
    } \
} while (0)
#ifdef SUPPORT_AUDIO_STREAM_IN_C
#define MAX_PRODUCER_THREAD_STACK_SIZE 0x800
#else
#define MAX_PRODUCER_THREAD_STACK_SIZE 0x1800
#endif
#define MAX_CONSUMER_THREAD_STACK_SIZE 0x1000
#define AUDIO_RECORDER_MAX_CONTINOUS_REND_FRAME_FULL_COUNT 200
#define AUDIO_RECORDER_MAX_INSTANCE_NUM 1
#define AUDIO_RECORDER_RENDER_EMPTY_SLEEP_TIME_US  5000u
#define AUDIO_RECORDER_MAX_MEMORY_SLEEP_TIME_US  10000000u
#ifdef SUPPORT_FEATURE_CONFIG
#define CAPTURE_ONE_FRAME_TIME (1000 / CONFIG_AUDIO_AI_FRAME_PER_SEC)
#define AUDIO_RECORDER_ONE_FRAME  ((1000 / CONFIG_AUDIO_AI_FRAME_PER_SEC) * 1000)
#else
#define AUDIO_RECORDER_ONE_FRAME  10000u
#define CAPTURE_ONE_FRAME_TIME 10
#endif
#ifdef SUPPORT_ERROR_CODE
#define AUDIO_RECORDER_RETRY_READ (ERRCODE_MEDIA_AUDIO_RETRY_READ)
#else
#define AUDIO_RECORDER_RETRY_READ (-7)
#endif
#define AUDIO_RECORDER_NOT_SUPPORT_GET_FRAME (-8)
#define AUDIO_RECORDER_OK 0
#define AUDIO_RECORDER_ERR (-1)
#define AUDIO_RECORDER_NO_MEM (-2)
#define AUDIO_RECORDER_FAIL (-3)
#define AUDIO_RECORDER_INVALID_PARAM (-4)
#define AUDIO_RECORDER_CAPTURE_FATEL (-5)
#define AUDIO_RECORDER_INVAILD_ID 0xFFFF
#define MAX_PREEMPTION_TIMES 2
#define AUDIO_CONSUMER_BUFFER_FULL_SLEEP_TIME_US  1000000u
#define AUDIO_CONSUMER_BUFFER_FREE_SLEEP_TIME_US  1000000u
#define AUDIO_CONSUMER_BUFFER_WRITE_FAIL_SLEEP_US  10000u

#define AUDIO_RECORDER_MAGIC_ID 0xAAAB
#define HANDLE_SHIFT_LEN 16
#define GET_AUDIO_RECORDER_MAGIC_ID(handle) (((uint32_t)(handle)) >> HANDLE_SHIFT_LEN)
#define GET_AUDIO_RECORDER_HANDLE(id) ((AUDIO_RECORDER_MAGIC_ID << HANDLE_SHIFT_LEN) | (id))
#define GET_AUDIO_RECORDER_ID(handle) (((uint32_t)(handle)) & 0xFFFF)
#define BUFFER_SIZE 20
#define SET_DB_VOLUME "setdbvol"
#define AUDIO_RECORD_MEDIA_PATH_MAX 256
#define CAPTURE_ONE_SECOND_SAMPLE_RATE 1000
#define RECORDE_PARAM_BUF_LEN 25
#define RECORDE_MAX_BUF_SIZE 2

typedef struct {
    void *data;
    uint32_t dataSize;
    int64_t pts;
} RecorderBuffer;

typedef struct {
    RecorderBuffer buffer[RECORDE_MAX_BUF_SIZE];
    uint32_t front;
    uint32_t rear;
    uint32_t wraparound;
    void *lock;
} ThreadSafeQueue;

typedef struct {
    AudioCodecFormat audioFormat;
    uint32_t maxBufferSize;
}AudioRecorderFormatBufferSize;

static AudioRecorderFormatBufferSize g_formatBufferSizeMapping[] = {
    {MP3, MP3_MAX_DATA_BUFFER_SIZE},
    {SILK, SILK_MAX_DATA_BUFFER_SIZE},
    {PCM, WAV_MAX_DATA_BUFFER_SIZE},
    {OPUS, OGG_MAX_DATA_BUFFER_SIZE},
};

typedef struct {
    const void *cookie;
    RecorderCallback eventFunc;
#ifdef SUPPORT_AUDIO_STREAM_IN_C
    AudioStreamInHandle audioStreamInHandle;
#endif
    FormatFrame audioFrame;
    AudioRecorderStates state;
    void *instanceLock;
    void *producerLock;
    void *producerCond;
    void *consumerCond;
    void *producerProcess;
    void *consumerProcess;
    bool producerThreadExit;
    bool consumerThreadExit;
    uint32_t producerSleepTime;
    uint32_t consumerSleepTime;
    bool isUsed;
    AudioRecorderMuxer muxerHandle;
    CapturerInputConfig config;
    bool enableOverwriteFiles;
    uint32_t storageWarningThresholdPercent;
    MuxerCallback callBack;
    ThreadSafeQueue freeDataQueue;
    ThreadSafeQueue fullDataQueue;
    bool isNeedReadNewBuffer;
    uint32_t maxBufferSize;
} AudioRecorderContext;

typedef enum {
    AUDIO_RECORD_SET_CALLBACK, /* Set the callback function. */
    AUDIO_RECORD_SET_DELETE_OLD_FILES_ENABLE, /* Setting whether to delete old files */
    AUDIO_RECORD_SET_MAX_STORAGE_THREAHOLD, /* Setting the Maximum Storage Space Threshold */
    AUDIO_RECORD_SET_MARK,
    AUDIO_RECORD_CURRENT_STORAGE_REMAIND_TIME, /* Obtains the remaining time of the current storage space. */
    AUDIO_RECORD_CURRENT_FILE_REMAIND_TIME, /* Obtains the remaining recording time of the current file. */
    AUDIO_RECORD_CURRENT_FILE_INFO,
} AudioRecordInvokeId;

static AudioRecorderContext g_audioRecorder[AUDIO_RECORDER_MAX_INSTANCE_NUM] = {
    {.instanceLock = NULL, .muxerHandle = NULL},
};

static int16_t g_audioRecorderId[AUDIO_RECORDER_MAX_INSTANCE_NUM] = {-1};
static void *g_audioRecorderMutex = NULL;

static int32_t InitQueue(ThreadSafeQueue *queue)
{
    if (queue == NULL) {
        AUDIO_RECORDER_ERR_LOG("Queue is NULL!");
        return AUDIO_RECORDER_INVALID_PARAM;
    }
    queue->front = 0;
    queue->rear = 0;
    queue->wraparound = 0;
    if (queue->lock == NULL) {
        queue->lock = AUDIO_RECORDER_MUTEX_CREATE(NULL);
        if (queue->lock == NULL) {
            AUDIO_RECORDER_ERR_LOG("Queue mutex create failed!");
            return AUDIO_RECORDER_ERR;
        }
    }
    return AUDIO_RECORDER_OK;
}

static bool QueueFull(const ThreadSafeQueue *queue)
{
    bool full = false;
    if (queue->rear > queue->front) {
        full = (queue->rear - queue->front) == RECORDE_MAX_BUF_SIZE;
    } else {
        full = (queue->front - queue->rear) == RECORDE_MAX_BUF_SIZE;
    }
    return full;
}

static bool QueueEmpty(const ThreadSafeQueue *queue)
{
    return queue->rear == queue->front && queue->wraparound == 0;
}

static void Enqueue(ThreadSafeQueue *queue, const RecorderBuffer *buffer)
{
    if (queue == NULL || buffer == NULL) {
        AUDIO_RECORDER_ERR_LOG("queue or buffer is NULL!");
        return;
    }
    AUDIO_RECORDER_MUTEX_LOCK(queue->lock);
    if (QueueFull(queue)) {
        AUDIO_RECORDER_ERR_LOG("Queue is full, cannot enqueue!");
        AUDIO_RECORDER_MUTEX_UNLOCK(queue->lock);
        return;
    }
    if (queue->rear >= RECORDE_MAX_BUF_SIZE) {
        queue->rear = (queue->rear) % RECORDE_MAX_BUF_SIZE;
        queue->wraparound++;
    }
    queue->buffer[queue->rear].data = buffer->data;
    queue->buffer[queue->rear].dataSize = buffer->dataSize;
    queue->buffer[queue->rear].pts = buffer->pts;
    queue->rear++;
    AUDIO_RECORDER_MUTEX_UNLOCK(queue->lock);
}

static RecorderBuffer *Dequeue(ThreadSafeQueue *queue)
{
    RecorderBuffer *value = NULL;
    if (queue == NULL) {
        AUDIO_RECORDER_ERR_LOG("queue is NULL!");
        return value;
    }
    AUDIO_RECORDER_MUTEX_LOCK(queue->lock);
    if (QueueEmpty(queue)) {
        AUDIO_RECORDER_MUTEX_UNLOCK(queue->lock);
        return value;
    }
    if (queue->front >= RECORDE_MAX_BUF_SIZE) {
        queue->front = (queue->front) % RECORDE_MAX_BUF_SIZE;
        queue->wraparound--;
    }
    value = &queue->buffer[queue->front];
    queue->front++;
    AUDIO_RECORDER_MUTEX_UNLOCK(queue->lock);
    return value;
}

void DeinitQueue(ThreadSafeQueue *queue)
{
    AUDIO_RECORDER_MUTEX_DESTROY(&queue->lock);
}

static AudioRecorderContext *GetAudioRecorderContext(const AudioRecorderHandle handle)
{
    AudioRecorderContext *recorderContext = NULL;
    uint16_t audioRecorderId;
    AUDIO_RECORDER_MUTEX_LOCK(g_audioRecorderMutex);
    if (GET_AUDIO_RECORDER_MAGIC_ID(handle) != AUDIO_RECORDER_MAGIC_ID) {
        AUDIO_RECORDER_ERR_LOG("invalid audio recorder handle!");
        goto EXIT;
    }
    audioRecorderId = GET_AUDIO_RECORDER_ID(handle);
    if (audioRecorderId >= AUDIO_RECORDER_MAX_INSTANCE_NUM) {
        AUDIO_RECORDER_ERR_LOG("invalid audio recorder id: %u!", audioRecorderId);
        goto EXIT;
    }
    if (!g_audioRecorder[audioRecorderId].isUsed) {
        AUDIO_RECORDER_ERR_LOG("audio recorder handle: 0x%x error, isUsed: %d",
            handle, g_audioRecorder[audioRecorderId].isUsed);
        goto EXIT;
    }
    recorderContext = &g_audioRecorder[audioRecorderId];
EXIT:
    AUDIO_RECORDER_MUTEX_UNLOCK(g_audioRecorderMutex);
    return recorderContext;
}

static uint16_t GetNewAudioRecorderId(void)
{
    uint16_t audioRecorderId;
    for (audioRecorderId = 0; audioRecorderId < AUDIO_RECORDER_MAX_INSTANCE_NUM; audioRecorderId++) {
        if (g_audioRecorderId[audioRecorderId] == -1) {
            g_audioRecorderId[audioRecorderId] = audioRecorderId;
            break;
        }
    }
    return audioRecorderId;
}

static uint16_t GetAudioRecorderAvailableId(void)
{
    uint16_t audioRecorderId = GetNewAudioRecorderId();
    if (audioRecorderId >= AUDIO_RECORDER_MAX_INSTANCE_NUM) {
        for (audioRecorderId = 0; audioRecorderId < AUDIO_RECORDER_MAX_INSTANCE_NUM; audioRecorderId++) {
            if (!g_audioRecorder[audioRecorderId].isUsed) {
                g_audioRecorderId[audioRecorderId] = -1;
            }
        }
        audioRecorderId = GetNewAudioRecorderId();
    }
    return audioRecorderId;
}

static int32_t AudioStreamSinkStart(AudioRecorderContext *recorderContext)
{
    int32_t ret = AUDIO_RECORDER_OK;
#ifdef SUPPORT_AUDIO_STREAM_IN_C
    ret = AudioStreamInCreate(&recorderContext->audioStreamInHandle);
    if (ret != AUDIO_RECORDER_OK) {
        AUDIO_RECORDER_ERR_LOG("create failed");
        return AUDIO_RECORDER_ERR;
    }
    ret = AudioStreamInInit(recorderContext->audioStreamInHandle, &recorderContext->config,
        NULL, NULL);
    if (ret != AUDIO_RECORDER_OK) {
        AUDIO_RECORDER_ERR_LOG("init failed");
        AudioStreamInDestroy(recorderContext->audioStreamInHandle);
        return AUDIO_RECORDER_ERR;
    }
    ret = AudioStreamInStart(recorderContext->audioStreamInHandle);
    if (ret != AUDIO_RECORDER_OK) {
        AUDIO_RECORDER_ERR_LOG("start failed");
        goto EXIT;
    }
    return ret;
EXIT:
    AudioStreamInRelease(recorderContext->audioStreamInHandle);
    AudioStreamInDestroy(recorderContext->audioStreamInHandle);
#else
    ret = AudioStreamInInit(recorderContext->config);
    if (ret != AUDIO_RECORDER_OK) {
        AUDIO_RECORDER_ERR_LOG("create failed");
        return ret;
    }
    ret = AudioStreamInStart();
    if (ret != AUDIO_RECORDER_OK) {
        AUDIO_RECORDER_ERR_LOG("start failed");
        AudioStreamInDeinit();
    }
#endif
    return ret;
}

static bool AudioRecorderGetMaxBufferSize(AudioRecorderContext *recorderContext, AudioCodecFormat audioFormat)
{
    if (recorderContext == NULL) {
        AUDIO_RECORDER_ERR_LOG("recorderContext is NULL!");
        return false;
    }
    for (uint32_t i = 0; i < sizeof(g_formatBufferSizeMapping) / sizeof(AudioRecorderFormatBufferSize); i++) {
        if (g_formatBufferSizeMapping[i].audioFormat == audioFormat) {
            recorderContext->maxBufferSize = g_formatBufferSizeMapping[i].maxBufferSize;
            return true;
        }
    }
    AUDIO_RECORDER_ERR_LOG("not found support format[%d]", audioFormat);
    return false;
}

static uint32_t AudioRecorderGetPacketLength(AudioRecorderContext *recorderContext)
{
    uint32_t len = 0;
    uint32_t bitWidth = recorderContext->config.bitWidth / 0x8;
    len = recorderContext->config.channelCount *
        (recorderContext->config.sampleRate / CAPTURE_ONE_SECOND_SAMPLE_RATE * CAPTURE_ONE_FRAME_TIME) * bitWidth;
    return len;
}

static int32_t AudioRecorderInitBufferData(AudioRecorderContext *recorderContext)
{
    int32_t ret = InitQueue(&recorderContext->freeDataQueue);
    if (ret != AUDIO_RECORDER_OK) {
        return AUDIO_RECORDER_ERR;
    }
    ret = InitQueue(&recorderContext->fullDataQueue);
    if (ret != AUDIO_RECORDER_OK) {
        goto DEINIT_FREE_QUEUE;
    }
    for (size_t i = 0; i < RECORDE_MAX_BUF_SIZE; i++) {
        RecorderBuffer buffer = {};
        buffer.data = malloc(recorderContext->maxBufferSize);
        if (buffer.data == NULL) {
            AUDIO_RECORDER_ERR_LOG("audio recorder malloc Data failed! i = %d", i);
            goto DEINIT_FULL_QUEUE;
        }
        buffer.dataSize = 0;
        buffer.pts = 0;
        Enqueue(&recorderContext->freeDataQueue, &buffer);
    }
    return AUDIO_RECORDER_OK;
DEINIT_FULL_QUEUE:
    DeinitQueue(&recorderContext->fullDataQueue);
DEINIT_FREE_QUEUE:
    DeinitQueue(&recorderContext->freeDataQueue);
    return AUDIO_RECORDER_ERR;
}

static void AudioRecorderDestroyBufferData(AudioRecorderContext *recorderContext)
{
    for (size_t i = 0; i < RECORDE_MAX_BUF_SIZE; i++) {
        if (recorderContext->freeDataQueue.buffer[i].data != NULL) {
            free(recorderContext->freeDataQueue.buffer[i].data);
            recorderContext->freeDataQueue.buffer[i].data = NULL;
        }
    }
    DeinitQueue(&recorderContext->freeDataQueue);
    DeinitQueue(&recorderContext->fullDataQueue);
}

static RecorderBuffer *AudioRecorderGetFreeBuffer(AudioRecorderContext *recorderContext)
{
    return Dequeue(&recorderContext->freeDataQueue);
}

static void AudioRecorderPutFreeBuffer(AudioRecorderContext *recorderContext, const RecorderBuffer *buffer)
{
    Enqueue(&recorderContext->freeDataQueue, buffer);
}

static void AudioRecorderPutFullBuffer(AudioRecorderContext *recorderContext, const RecorderBuffer *buffer)
{
    Enqueue(&recorderContext->fullDataQueue, buffer);
}

static RecorderBuffer *AudioRecorderGetFullBuffer(AudioRecorderContext *recorderContext)
{
    return Dequeue(&recorderContext->fullDataQueue);
}

static int32_t AudioRecorderPushPacketToFile(AudioRecorderContext *recorderContext, RecorderBuffer *buffer)
{
    int32_t ret = 0;
    recorderContext->audioFrame.data = buffer->data;
    recorderContext->audioFrame.len = buffer->dataSize;
    recorderContext->audioFrame.timestampUs = buffer->pts;
    ret = AudioRecorderMuxerWriteData(recorderContext->muxerHandle, &recorderContext->audioFrame);
    if (ret != AUDIO_RECORDER_OK) {
        AUDIO_RECORDER_ERR_LOG("audio recorder Write Data failed!");
    }
    buffer->dataSize = 0;
    AudioRecorderPutFreeBuffer(recorderContext, buffer);
    AUDIO_RECORDER_THREAD_COND_SIGNAL(recorderContext->producerCond);
    return ret;
}

static void AudioRecorderSendData(AudioRecorderContext *recorderContext,
    RecorderBuffer *buffer, uint32_t size)
{
    if (buffer == NULL) {
        return;
    }
    if (buffer->dataSize > 0) {
#ifdef SUPPORT_AUDIO_STREAM_IN_C
        float frameNum = buffer->dataSize / (size * 1.0f);
        buffer->pts += frameNum * AUDIO_RECORDER_ONE_FRAME;
#endif
        AudioRecorderPutFullBuffer(recorderContext, buffer);
        AUDIO_RECORDER_THREAD_COND_SIGNAL(recorderContext->consumerCond);
        recorderContext->isNeedReadNewBuffer = true;
    }
}

static int32_t AudioRecorderGetPacketfromProducer(AudioRecorderContext *recorderContext,
    RecorderBuffer *buffer, uint32_t size)
{
    int32_t ret = 0;
    CapturerBuffer capturebuffer = { 0 };
    struct timespec timestamp = {};
    AUDIO_RECORD_CHECK_NULL_RETURN(buffer, AUDIO_RECORDER_ERR, "buffer is NULL!");
    capturebuffer.size = recorderContext->maxBufferSize - buffer->dataSize;
    capturebuffer.data = (uint8_t *)buffer->data + buffer->dataSize;
    int32_t readSize = 0;
#ifdef SUPPORT_AUDIO_STREAM_IN_C
    ret = AudioStreamInObtainBuffer(recorderContext->audioStreamInHandle, &capturebuffer);
    readSize = ret;
#else
    ret = AudioStreamInObtainBuffer(capturebuffer.data, &capturebuffer.size);
    readSize = capturebuffer.size;
    int64_t pos = -1;
    (void)AudioStreamInGetPosition(&pos);
    if (pos != -1) {
        buffer->pts = pos;
    }
#endif
    if (ret < AUDIO_RECORDER_OK) {
        if (ret == AUDIO_RECORDER_NOT_SUPPORT_GET_FRAME) {
            AudioRecorderSendData(recorderContext, buffer, size);
            return AUDIO_RECORDER_OK;
        }
        recorderContext->producerSleepTime = AUDIO_RECORDER_RENDER_EMPTY_SLEEP_TIME_US;
        return ret;
    }
    buffer->dataSize += readSize;
#ifdef SUPPORT_AUDIO_STREAM_IN_C
    if (buffer->dataSize == recorderContext->maxBufferSize) {
        AudioRecorderSendData(recorderContext, buffer, size);
    }
#endif
    return AUDIO_RECORDER_OK;
}

static void *AudioRecorderProducerProcess(void *priv)
{
    AUDIO_RECORD_CHECK_NULL_RETURN(priv, NULL, "priv is NULL");
    AudioRecorderContext *record = (AudioRecorderContext *)priv;
    int32_t ret = AUDIO_RECORDER_OK;
#ifdef SUPPORT_AUDIO_STREAM_IN_C
    uint32_t size = AudioRecorderGetPacketLength(record);
    if (size <= 0) {
        AUDIO_RECORDER_ERR_LOG("audio recorder get packet length failed!");
        return NULL;
    }
#else
    uint32_t size = 0;
#endif
    RecorderBuffer *buffer = NULL;
    record->isNeedReadNewBuffer = true;
    while (true) {
        AUDIO_RECORDER_MUTEX_LOCK(record->producerLock);
        if (record->producerThreadExit) {
            AudioRecorderSendData(record, buffer, size);
            AUDIO_RECORDER_INFO_LOG("audio producer sch process exit");
            goto EXIT;
        }
        record->producerSleepTime = 0;
        if (record->isNeedReadNewBuffer) {
            buffer = AudioRecorderGetFreeBuffer(record);
            if (buffer == NULL) {
                record->producerSleepTime = AUDIO_CONSUMER_BUFFER_FULL_SLEEP_TIME_US;
                goto CONTINUE;
            }
            record->isNeedReadNewBuffer = false;
        }
        ret = AudioRecorderGetPacketfromProducer(record, buffer, size);
        if (ret != AUDIO_RECORDER_OK) {
            if ((uint32_t)ret == AUDIO_RECORDER_RETRY_READ) {
                goto CONTINUE;
            }
            goto EXIT;
        }
    CONTINUE:
        if (record->producerSleepTime > 0) {
            AUDIO_RECORDER_THREAD_COND_TIME_WAIT(record->producerCond, record->producerLock, record->producerSleepTime);
        }
        AUDIO_RECORDER_MUTEX_UNLOCK(record->producerLock);
    }
EXIT:
    record->producerThreadExit = true;
    AUDIO_RECORDER_MUTEX_UNLOCK(record->producerLock);
    return NULL;
}

static void* AudioRecorderConsumerProcess(void *priv)
{
    AUDIO_RECORD_CHECK_NULL_RETURN(priv, NULL, "priv is NULL");
    AudioRecorderContext *record = (AudioRecorderContext *)priv;
    RecorderBuffer *buffer = NULL;
    while (true) {
        AUDIO_RECORDER_MUTEX_LOCK(record->instanceLock);
        if (record->consumerThreadExit && buffer == NULL) {
            AUDIO_RECORDER_INFO_LOG("audio consumer sch process exit");
            goto EXIT;
        }
        record->consumerSleepTime = 0;
        buffer = AudioRecorderGetFullBuffer(record);
        if (buffer == NULL) {
            record->consumerSleepTime = AUDIO_CONSUMER_BUFFER_FREE_SLEEP_TIME_US;
            goto CONTINUE;
        }
        int32_t ret = AudioRecorderPushPacketToFile(record, buffer);
        if (ret != AUDIO_RECORDER_OK) {
            if ((uint32_t)ret == AUDIO_RECORDER_RETRY_READ) {
                AUDIO_RECORDER_ERR_LOG("AudioRecorderPushPacketToFile AUDIO_RECORDER_RETRY_READ");
                goto CONTINUE;
            }
            AUDIO_RECORDER_ERR_LOG("audio recorder send data failed! ret = %d", ret);
            goto EXIT;
        }
CONTINUE:
        if (record->consumerSleepTime > 0) {
            AUDIO_RECORDER_THREAD_COND_TIME_WAIT(record->consumerCond, record->instanceLock, record->consumerSleepTime);
        }
        buffer = NULL;
        AUDIO_RECORDER_MUTEX_UNLOCK(record->instanceLock);
    }
EXIT:
    record->consumerThreadExit = true;
    AUDIO_RECORDER_MUTEX_UNLOCK(record->instanceLock);
    return NULL;
}

static int32_t InitAudioRecorderMutexAndCond(uint16_t audioRecorderId)
{
    if (g_audioRecorder[audioRecorderId].instanceLock == NULL) {
        g_audioRecorder[audioRecorderId].instanceLock = AUDIO_RECORDER_MUTEX_CREATE(NULL);
        if (g_audioRecorder[audioRecorderId].instanceLock == NULL) {
            AUDIO_RECORDER_ERR_LOG("mutex instanceLock create failed!");
            return AUDIO_RECORDER_ERR;
        }
    }
    g_audioRecorder[audioRecorderId].producerCond = AUDIO_RECORDER_THREAD_COND_CREATE();
    if (g_audioRecorder[audioRecorderId].producerCond == NULL) {
        AUDIO_RECORDER_ERR_LOG("producercond create failed!");
        goto INSTANCE_LOCK;
    }
    g_audioRecorder[audioRecorderId].consumerCond = AUDIO_RECORDER_THREAD_COND_CREATE();
    if (g_audioRecorder[audioRecorderId].consumerCond == NULL) {
        AUDIO_RECORDER_ERR_LOG("consumerCond create failed!");
        goto PRODUCER_COND;
    }
    if (g_audioRecorder[audioRecorderId].producerLock == NULL) {
        g_audioRecorder[audioRecorderId].producerLock = AUDIO_RECORDER_MUTEX_CREATE(NULL);
        if (g_audioRecorder[audioRecorderId].producerLock == NULL) {
            AUDIO_RECORDER_ERR_LOG("mutex producerLock create failed!");
            goto CONSUMER_COND;
        }
    }
    return AUDIO_RECORDER_OK;
CONSUMER_COND:
    AUDIO_RECORDER_THREAD_COND_DESTROY(&g_audioRecorder[audioRecorderId].consumerCond);
PRODUCER_COND:
    AUDIO_RECORDER_THREAD_COND_DESTROY(&g_audioRecorder[audioRecorderId].producerCond);
INSTANCE_LOCK:
    AUDIO_RECORDER_MUTEX_DESTROY(&g_audioRecorder[audioRecorderId].instanceLock);
    return AUDIO_RECORDER_ERR;
}

static int32_t InitAudioRecorderContext(uint16_t audioRecorderId)
{
    g_audioRecorder[audioRecorderId].state = AUDIO_RECORD_STATUS_IDLE;
    g_audioRecorder[audioRecorderId].config.audioFormat = PCM;
    g_audioRecorder[audioRecorderId].config.streamType = AUDIO_STREAM_VOICE_RECORD;
    g_audioRecorder[audioRecorderId].config.channelCount = 1;
    g_audioRecorder[audioRecorderId].config.linkDir = AUDIO_UP_LINK;
    g_audioRecorder[audioRecorderId].config.sampleRate = 0;
#ifdef SUPPORT_SEA_CONFIG
    g_audioRecorder[audioRecorderId].config.micType = CAPTURE_MIC_TYPE_0;
    g_audioRecorder[audioRecorderId].config.isProductTestMode = false;
#endif
    g_audioRecorder[audioRecorderId].config.sessionID = 0;
    g_audioRecorder[audioRecorderId].audioFrame.data = NULL;
    g_audioRecorder[audioRecorderId].audioFrame.len = 0;
#ifdef SUPPORT_AUDIO_STREAM_IN_C
    g_audioRecorder[audioRecorderId].audioStreamInHandle = NULL;
#endif
    g_audioRecorder[audioRecorderId].producerThreadExit = false;
    g_audioRecorder[audioRecorderId].consumerThreadExit = false;
    g_audioRecorder[audioRecorderId].consumerProcess = NULL;
    g_audioRecorder[audioRecorderId].producerProcess = NULL;
    g_audioRecorder[audioRecorderId].producerSleepTime = 0;
    g_audioRecorder[audioRecorderId].consumerSleepTime = 0;
    g_audioRecorder[audioRecorderId].eventFunc = NULL;
    int32_t ret = InitAudioRecorderMutexAndCond(audioRecorderId);
    if (ret != AUDIO_RECORDER_OK) {
        AUDIO_RECORDER_ERR_LOG("InitAudioRecorderMutexAndCond failed!");
    }
    return ret;
}

static int32_t MuxerOnErrorCallback(void *handle, EventErrorType type, int32_t ext)
{
    AUDIO_RECORD_CHECK_NULL_RETURN(handle, AUDIO_RECORDER_INVALID_PARAM, "handle is NULL");
    AudioRecorderContext *recorderContext = (AudioRecorderContext *)(handle);
    switch (type) {
        case ERROR_TYPE_CREATE_FILE_FAIL:
        case ERROR_TYPE_WRITE_FILE_FAIL:
        case ERROR_TYPE_CLOSE_FILE_FAIL:
        case ERROR_TYPE_INTERNAL_OPERATION_FAIL:
            AUDIO_RECORDER_INFO_LOG("AUDIO_RECORD_EVENT_ERROR type = %d!", type);
            recorderContext->eventFunc(AUDIO_RECORD_EVENT_ERROR, &ext, recorderContext->cookie);
            break;
        default:
            break;
    }
    return AUDIO_RECORDER_OK;
}

static int32_t MuxerOnInfoCallback(void *handle, EventInfoType type, int32_t ext)
{
    AUDIO_RECORD_CHECK_NULL_RETURN(handle, AUDIO_RECORDER_INVALID_PARAM, "handle is NULL");
    AudioRecorderContext *recorderContext = (AudioRecorderContext *)(handle);
    switch (type) {
        case MAX_DURATION_APPROACHING:
            AUDIO_RECORDER_INFO_LOG("MAX_DURATION_APPROACHING!");
            break;
        case MAX_FILESIZE_APPROACHING:
            AUDIO_RECORDER_INFO_LOG("MAX_FILESIZE_APPROACHING!");
            break;
        case MAX_FILESIZE_REACHED:
            AUDIO_RECORDER_INFO_LOG("MAX_FILESIZE_REACHED!");
            break;
        case MAX_DURATION_REACHED:
            AUDIO_RECORDER_INFO_LOG("MAX_DURATION_REACHED!");
            break;
        case MAX_MEMORYSIZE_REACHED:
            recorderContext->eventFunc(AUDIO_RECORD_STORAGE_REACHE_MAX_VALUE, &ext, recorderContext->cookie);
            recorderContext->consumerSleepTime = AUDIO_RECORDER_MAX_MEMORY_SLEEP_TIME_US;
            break;
        default:
            break;
    }
    return AUDIO_RECORDER_OK;
}

errcode_t AudioRecorderCreate(AudioRecorderHandle *handle)
{
    AUDIO_RECORD_CHECK_NULL_RETURN(handle, AUDIO_RECORDER_INVALID_PARAM, "handle is NULL");
    AUDIO_RECORDER_INIT_STATIC_MUTEX_LOCK(&g_audioRecorderMutex);
    AUDIO_RECORDER_MUTEX_LOCK(g_audioRecorderMutex);
    int32_t ret = AUDIO_RECORDER_OK;
    uint16_t audioRecorderId = GetAudioRecorderAvailableId();
    if (audioRecorderId >= AUDIO_RECORDER_MAX_INSTANCE_NUM) {
        AUDIO_RECORDER_ERR_LOG("not found free audio recorder handle id!");
        goto EXIT;
    }
    ret = AudioRecorderMuxerCreate(&g_audioRecorder[audioRecorderId].muxerHandle);
    if (ret != AUDIO_RECORDER_OK || g_audioRecorder[audioRecorderId].muxerHandle == NULL) {
        AUDIO_RECORDER_ERR_LOG("create muxer failed! ret = %d", ret);
        goto EXIT;
    }
    ret = InitAudioRecorderContext(audioRecorderId);
    if (ret != AUDIO_RECORDER_OK) {
        AUDIO_RECORDER_ERR_LOG("init audio recorder context failed!");
        goto EXIT;
    }
    g_audioRecorder[audioRecorderId].isUsed = true;
    *handle = (AudioRecorderHandle)GET_AUDIO_RECORDER_HANDLE(audioRecorderId);
    AUDIO_RECORDER_MUTEX_UNLOCK(g_audioRecorderMutex);
    return AUDIO_RECORDER_OK;
EXIT:
    AUDIO_RECORDER_MUTEX_UNLOCK(g_audioRecorderMutex);
    return AUDIO_RECORDER_ERR;
}

errcode_t AudioRecorderSetInfo(const AudioRecorderHandle handle, const CapturerInputConfig *config)
{
    AUDIO_RECORD_CHECK_NULL_RETURN(handle, AUDIO_RECORDER_INVALID_PARAM, "audio recorder is NULL!");
    AudioRecorderContext *recorderContext = GetAudioRecorderContext(handle);
    AUDIO_RECORD_CHECK_NULL_RETURN(recorderContext, AUDIO_RECORDER_INVALID_PARAM, "recorderContext is NULL!");
    AUDIO_RECORD_CHECK_NULL_RETURN(config, AUDIO_RECORDER_INVALID_PARAM, "config is NULL!");
    AUDIO_RECORDER_MUTEX_LOCK(recorderContext->instanceLock);
    if (recorderContext->state != AUDIO_RECORD_STATUS_IDLE) {
        AUDIO_RECORDER_ERR_LOG("the call is not allowed in the current state.state = %d", recorderContext->state);
        AUDIO_RECORDER_MUTEX_UNLOCK(recorderContext->instanceLock);
        return AUDIO_RECORDER_ERR;
    }
    if (!AudioRecorderGetMaxBufferSize(recorderContext, config->audioFormat)) {
        AUDIO_RECORDER_ERR_LOG("get max buffer size failed!");
        AUDIO_RECORDER_MUTEX_UNLOCK(recorderContext->instanceLock);
        return AUDIO_RECORDER_ERR;
    }
    int32_t ret = AudioRecorderInitBufferData(recorderContext);
    if (ret != AUDIO_RECORDER_OK) {
        AUDIO_RECORDER_ERR_LOG("AudioRecorderInitBufferData failed!");
        AudioRecorderDestroyBufferData(recorderContext);
        AUDIO_RECORDER_MUTEX_UNLOCK(recorderContext->instanceLock);
        return ret;
    }
    recorderContext->config.audioFormat = config->audioFormat;
    recorderContext->config.streamType = config->streamType;
    recorderContext->config.channelCount = config->channelCount;
    recorderContext->config.bitWidth = config->bitWidth;
    recorderContext->config.linkDir = config->linkDir;
    recorderContext->config.sampleRate = config->sampleRate;
#ifdef SUPPORT_SEA_CONFIG
    recorderContext->config.isProductTestMode = config->isProductTestMode;
    recorderContext->config.micType = config->micType;
#endif
    recorderContext->config.sessionID = config->sessionID;
    AUDIO_RECORDER_MUTEX_UNLOCK(recorderContext->instanceLock);
    return AUDIO_RECORDER_OK;
}

errcode_t AudioRecorderSetOutputPath(const AudioRecorderHandle handle, const char *outPath, uint32_t outPathLen)
{
    AUDIO_RECORD_CHECK_NULL_RETURN(handle, AUDIO_RECORDER_INVALID_PARAM, "audio recorder is NULL!");
    AudioRecorderContext *recorderContext = GetAudioRecorderContext(handle);
    AUDIO_RECORD_CHECK_NULL_RETURN(recorderContext, AUDIO_RECORDER_INVALID_PARAM, "recorderContext is NULL!");
    AUDIO_RECORD_CHECK_NULL_RETURN(outPath, AUDIO_RECORDER_INVALID_PARAM, "filepath is NULL!");
    if (outPathLen > AUDIO_RECORD_MEDIA_PATH_MAX) {
        AUDIO_RECORDER_ERR_LOG("filePathLen is invalid!");
        return AUDIO_RECORDER_INVALID_PARAM;
    }
    AUDIO_RECORDER_MUTEX_LOCK(recorderContext->instanceLock);
    if (recorderContext->state != AUDIO_RECORD_STATUS_IDLE) {
        AUDIO_RECORDER_ERR_LOG("the call is not allowed in the current state.state = %d", recorderContext->state);
        AUDIO_RECORDER_MUTEX_UNLOCK(recorderContext->instanceLock);
        return AUDIO_RECORDER_ERR;
    }
#ifdef SUPPORT_AUDIO_STREAM_IN_C
#ifndef ONLY_UT_TEST
    char startWith[] = "/user/";
    if (strncmp(outPath, startWith, strlen(startWith)) != 0) {
        AUDIO_RECORDER_ERR_LOG("path is not start with '/user/' !");
        AUDIO_RECORDER_MUTEX_UNLOCK(recorderContext->instanceLock);
        return AUDIO_RECORDER_ERR;
    }
#endif
#endif
    errcode_t ret = AudioRecorderMuxerSetOutputPath(recorderContext->muxerHandle,
        recorderContext->config.audioFormat, outPath, outPathLen);
    if (ret != AUDIO_RECORDER_OK) {
        AUDIO_RECORDER_ERR_LOG("muxer set source failed, ret:%d", ret);
        AUDIO_RECORDER_MUTEX_UNLOCK(recorderContext->instanceLock);
        return AUDIO_RECORDER_ERR;
    }
    ret = AudioRecorderMuxerAddTrack(recorderContext->muxerHandle, &recorderContext->config);
    if (ret != AUDIO_RECORDER_OK) {
        AUDIO_RECORDER_ERR_LOG("muxer add track source failed, ret:%d", ret);
        AUDIO_RECORDER_MUTEX_UNLOCK(recorderContext->instanceLock);
        return AUDIO_RECORDER_ERR;
    }
    recorderContext->state = AUDIO_RECORD_STATUS_INIT;
    AUDIO_RECORDER_MUTEX_UNLOCK(recorderContext->instanceLock);
    return AUDIO_RECORDER_OK;
}

errcode_t AudioRecorderPrepare(const AudioRecorderHandle handle)
{
    AUDIO_RECORD_CHECK_NULL_RETURN(handle, AUDIO_RECORDER_INVALID_PARAM, "audio recorder is NULL!");
    AudioRecorderContext *recorderContext = GetAudioRecorderContext(handle);
    AUDIO_RECORD_CHECK_NULL_RETURN(recorderContext, AUDIO_RECORDER_INVALID_PARAM, "recorderContext is NULL!");
    AUDIO_RECORDER_MUTEX_LOCK(recorderContext->instanceLock);
    if (recorderContext->state != AUDIO_RECORD_STATUS_INIT) {
        AUDIO_RECORDER_ERR_LOG("the call is not allowed in the current state.state = %d", recorderContext->state);
        AUDIO_RECORDER_MUTEX_UNLOCK(recorderContext->instanceLock);
        return AUDIO_RECORDER_ERR;
    }
    errcode_t ret = AudioRecorderMuxerPrepare(recorderContext->muxerHandle);
    if (ret != AUDIO_RECORDER_OK) {
        AUDIO_RECORDER_ERR_LOG("muxer perpare failed! ret = %d", ret);
        AUDIO_RECORDER_MUTEX_UNLOCK(recorderContext->instanceLock);
        return AUDIO_RECORDER_ERR;
    }
    recorderContext->callBack.handle = recorderContext;
    recorderContext->callBack.OnErrorCallback = MuxerOnErrorCallback;
    recorderContext->callBack.OnInfoCallback = MuxerOnInfoCallback;
    ret = AudioRecorderMuxerSetCallBack(recorderContext->muxerHandle, &recorderContext->callBack);
    if (ret != AUDIO_RECORDER_OK) {
        AUDIO_RECORDER_ERR_LOG("audio recorder muxer set callback failed! ret = %d", ret);
        AUDIO_RECORDER_MUTEX_UNLOCK(recorderContext->instanceLock);
        return AUDIO_RECORDER_ERR;
    }
    recorderContext->state = AUDIO_RECORD_STATUS_PREPARED;
    AUDIO_RECORDER_MUTEX_UNLOCK(recorderContext->instanceLock);
    return AUDIO_RECORDER_OK;
}

errcode_t AudioRecorderEnableOverwriteFiles(const AudioRecorderHandle handle, bool enable)
{
    AUDIO_RECORD_CHECK_NULL_RETURN(handle, AUDIO_RECORDER_INVALID_PARAM, "audio recorder is NULL!");
    AudioRecorderContext *recorderContext = GetAudioRecorderContext(handle);
    AUDIO_RECORD_CHECK_NULL_RETURN(recorderContext, AUDIO_RECORDER_INVALID_PARAM, "recorderContext is NULL!");
    AUDIO_RECORDER_MUTEX_LOCK(recorderContext->instanceLock);
    if (recorderContext->state > AUDIO_RECORD_STATUS_PREPARED) {
        AUDIO_RECORDER_INFO_LOG("can not to set enable Over write Files!,curState = %d", recorderContext->state);
        AUDIO_RECORDER_MUTEX_UNLOCK(recorderContext->instanceLock);
        return AUDIO_RECORDER_ERR;
    }
    ParameterItem item = {};
    item.key = AUDIO_RECORD_SET_DELETE_OLD_FILES_ENABLE;
    if (enable) {
        item.value.u32Value = 1;
    } else {
        item.value.u32Value = 0;
    }
    errcode_t ret = AudioRecorderMuxerSetParameter(recorderContext->muxerHandle, &item, 1);
    if (ret != AUDIO_RECORDER_OK) {
        AUDIO_RECORDER_INFO_LOG("audio recorder muxer set parameter failed, ret = %d", ret);
        AUDIO_RECORDER_MUTEX_UNLOCK(recorderContext->instanceLock);
        return AUDIO_RECORDER_ERR;
    }
    AUDIO_RECORDER_MUTEX_UNLOCK(recorderContext->instanceLock);
    return AUDIO_RECORDER_OK;
}

errcode_t AudioRecorderSetStorageWarningThreshold(const AudioRecorderHandle handle, uint32_t percent)
{
    AUDIO_RECORD_CHECK_NULL_RETURN(handle, AUDIO_RECORDER_INVALID_PARAM, "audio recorder is NULL!");
    AudioRecorderContext *recorderContext = GetAudioRecorderContext(handle);
    AUDIO_RECORD_CHECK_NULL_RETURN(recorderContext, AUDIO_RECORDER_INVALID_PARAM, "recorderContext is NULL!");
    AUDIO_RECORDER_MUTEX_LOCK(recorderContext->instanceLock);
    ParameterItem item = {};
    errcode_t ret = 0;
    if (recorderContext->state > AUDIO_RECORD_STATUS_PREPARED) {
        AUDIO_RECORDER_INFO_LOG("can not to set memory threshold percent!,curState = %d", recorderContext->state);
        goto ERROR;
    }
    if (percent < 1 || percent > AUDIO_MAX_MEMOR_THRESHOLD_PER) {
        AUDIO_RECORDER_INFO_LOG("The current threshold[%u] setting ratio is not supported. = %d", percent);
        goto ERROR;
    }
    item.key = AUDIO_RECORD_SET_MAX_STORAGE_THREAHOLD;
    item.value.u32Value = percent;
    ret = AudioRecorderMuxerSetParameter(recorderContext->muxerHandle, &item, 1);
    if (ret != AUDIO_RECORDER_OK) {
        AUDIO_RECORDER_INFO_LOG("audio recorder muxer set parameter failed, ret = %d", ret);
        goto ERROR;
    }
    AUDIO_RECORDER_MUTEX_UNLOCK(recorderContext->instanceLock);
    return AUDIO_RECORDER_OK;
ERROR:
    AUDIO_RECORDER_MUTEX_UNLOCK(recorderContext->instanceLock);
    return AUDIO_RECORDER_ERR;
}

errcode_t AudioRecorderWriteMarks(const AudioRecorderHandle handle, const uint8_t *buffer, const int32_t bufLen)
{
    MEDIA_UNUSED(buffer);
    MEDIA_UNUSED(bufLen);
    AUDIO_RECORD_CHECK_NULL_RETURN(handle, AUDIO_RECORDER_INVALID_PARAM, "audio recorder is NULL!");
    AudioRecorderContext *recorderContext = GetAudioRecorderContext(handle);
    AUDIO_RECORD_CHECK_NULL_RETURN(recorderContext, AUDIO_RECORDER_INVALID_PARAM, "recorderContext is NULL!");
    AUDIO_RECORDER_MUTEX_LOCK(recorderContext->instanceLock);
    ParameterItem item = {};
    errcode_t ret = 0;
    if (recorderContext->state != AUDIO_RECORD_STATUS_START) {
        AUDIO_RECORDER_INFO_LOG("can not to set memory threshold percent!,curState = %d", recorderContext->state);
        goto ERROR;
    }
    item.key = AUDIO_RECORD_SET_MARK;
    item.value.u32Value = 0;
    ret = AudioRecorderMuxerSetParameter(recorderContext->muxerHandle, &item, 1);
    if (ret != AUDIO_RECORDER_OK) {
        AUDIO_RECORDER_INFO_LOG("audio recorder muxer set parameter failed, ret = %d", ret);
        goto ERROR;
    }
    AUDIO_RECORDER_MUTEX_UNLOCK(recorderContext->instanceLock);
    return AUDIO_RECORDER_OK;
ERROR:
    AUDIO_RECORDER_MUTEX_UNLOCK(recorderContext->instanceLock);
    return AUDIO_RECORDER_ERR;
}

errcode_t AudioRecorderSetMaxDuration(const AudioRecorderHandle handle, uint32_t duration)
{
    AUDIO_RECORD_CHECK_NULL_RETURN(handle, AUDIO_RECORDER_INVALID_PARAM, "audio recorder is NULL!");
    AudioRecorderContext *recorderContext = GetAudioRecorderContext(handle);
    AUDIO_RECORD_CHECK_NULL_RETURN(recorderContext, AUDIO_RECORDER_INVALID_PARAM, "recorderContext is NULL!");
    AUDIO_RECORDER_MUTEX_LOCK(recorderContext->instanceLock);
    if (recorderContext->state > AUDIO_RECORD_STATUS_PREPARED) {
        AUDIO_RECORDER_ERR_LOG("the call is not allowed in the current state.state = %d", recorderContext->state);
        AUDIO_RECORDER_MUTEX_UNLOCK(recorderContext->instanceLock);
        return AUDIO_RECORDER_ERR;
    }
    errcode_t ret = AudioRecorderMuxerSetMaxDuration(recorderContext->muxerHandle, duration);
    AUDIO_RECORDER_MUTEX_UNLOCK(recorderContext->instanceLock);
    return ret;
}

errcode_t AudioRecorderSetMaxFileSize(const AudioRecorderHandle handle, uint64_t size)
{
    AUDIO_RECORD_CHECK_NULL_RETURN(handle, AUDIO_RECORDER_INVALID_PARAM, "audio recorder is NULL!");
    AudioRecorderContext *recorderContext = GetAudioRecorderContext(handle);
    AUDIO_RECORD_CHECK_NULL_RETURN(recorderContext, AUDIO_RECORDER_INVALID_PARAM, "recorderContext is NULL!");
    AUDIO_RECORDER_MUTEX_LOCK(recorderContext->instanceLock);
    if (recorderContext->state > AUDIO_RECORD_STATUS_PREPARED) {
        AUDIO_RECORDER_ERR_LOG("the call is not allowed in the current state.state = %d", recorderContext->state);
        AUDIO_RECORDER_MUTEX_UNLOCK(recorderContext->instanceLock);
        return AUDIO_RECORDER_ERR;
    }
    errcode_t ret = AudioRecorderMuxerSetMaxFileSize(recorderContext->muxerHandle, size);
    AUDIO_RECORDER_MUTEX_UNLOCK(recorderContext->instanceLock);
    return ret;
}

errcode_t AudioRecorderSetCallback(const AudioRecorderHandle handle, RecorderCallback callback, const void *cookie)
{
    AUDIO_RECORD_CHECK_NULL_RETURN(handle, AUDIO_RECORDER_INVALID_PARAM, "audio recorder is NULL!");
    AudioRecorderContext *recorderContext = GetAudioRecorderContext(handle);
    AUDIO_RECORD_CHECK_NULL_RETURN(recorderContext, AUDIO_RECORDER_INVALID_PARAM, "recorderContext is NULL!");
    AUDIO_RECORDER_MUTEX_LOCK(recorderContext->instanceLock);
    recorderContext->cookie = cookie;
    recorderContext->eventFunc = callback;
    AUDIO_RECORDER_MUTEX_UNLOCK(recorderContext->instanceLock);
    return AUDIO_RECORDER_OK;
}

errcode_t AudioRecorderGetRemainingTime(const AudioRecorderHandle handle, uint32_t type, uint32_t *time)
{
    AUDIO_RECORD_CHECK_NULL_RETURN(handle, AUDIO_RECORDER_INVALID_PARAM, "audio recorder is NULL!");
    AUDIO_RECORD_CHECK_NULL_RETURN(time, AUDIO_RECORDER_INVALID_PARAM, "time is NULL!");
    AudioRecorderContext *recorderContext = GetAudioRecorderContext(handle);
    AUDIO_RECORD_CHECK_NULL_RETURN(recorderContext, AUDIO_RECORDER_INVALID_PARAM, "recorderContext is NULL!");
    AUDIO_RECORDER_MUTEX_LOCK(recorderContext->instanceLock);
    if (recorderContext->state != AUDIO_RECORD_STATUS_START) {
        AUDIO_RECORDER_ERR_LOG("the call is not allowed in the current state.state = %d", recorderContext->state);
        AUDIO_RECORDER_MUTEX_UNLOCK(recorderContext->instanceLock);
        return AUDIO_RECORDER_ERR;
    }
    ParameterItem item = {};
    if (type == AUDIO_RECORD_CURRENT_FILE_REMAIN_TIME) {
        item.key = AUDIO_RECORD_CURRENT_FILE_REMAIND_TIME;
    } else if (type == AUDIO_RECORD_CURRENT_STORAGE_REMAIN_TIME) {
        item.key = AUDIO_RECORD_CURRENT_STORAGE_REMAIND_TIME;
    }
    item.value.u32Value = AudioRecorderGetPacketLength(recorderContext);
    errcode_t ret = AudioRecorderMuxerGetParameter(recorderContext->muxerHandle, &item, 1);
    if (ret != AUDIO_RECORDER_OK) {
        AUDIO_RECORDER_INFO_LOG("audio recorder muxer set parameter failed, ret = %d", ret);
        AUDIO_RECORDER_MUTEX_UNLOCK(recorderContext->instanceLock);
        return AUDIO_RECORDER_ERR;
    }
    *time = item.value.u32Value;
    AUDIO_RECORDER_MUTEX_UNLOCK(recorderContext->instanceLock);
    return AUDIO_RECORDER_OK;
}

errcode_t AudioRecorderGetFileInfo(const AudioRecorderHandle handle, RecordFileInfo *info)
{
    AUDIO_RECORD_CHECK_NULL_RETURN(handle, AUDIO_RECORDER_INVALID_PARAM, "audio recorder is NULL!");
    AUDIO_RECORD_CHECK_NULL_RETURN(info, AUDIO_RECORDER_INVALID_PARAM, "info is NULL!");
    AudioRecorderContext *recorderContext = GetAudioRecorderContext(handle);
    AUDIO_RECORD_CHECK_NULL_RETURN(recorderContext, AUDIO_RECORDER_INVALID_PARAM, "recorderContext is NULL!");
    AUDIO_RECORDER_MUTEX_LOCK(recorderContext->instanceLock);
    if (recorderContext->state < AUDIO_RECORD_STATUS_PREPARED ||
        recorderContext->state > AUDIO_RECORD_STATUS_START) {
        AUDIO_RECORDER_ERR_LOG("the call is not allowed in the current state.state = %d", recorderContext->state);
        AUDIO_RECORDER_MUTEX_UNLOCK(recorderContext->instanceLock);
        return AUDIO_RECORDER_ERR;
    }
    ParameterItem item = {};
    item.key = AUDIO_RECORD_CURRENT_FILE_INFO;
    char value[AUDIO_RECORDER_FILE_INFO_SIZE] = {};
    item.value.pValue = value;
    int32_t ret = AudioRecorderMuxerGetParameter(recorderContext->muxerHandle, &item, 1);
    if (ret != AUDIO_RECORDER_OK) {
        AUDIO_RECORDER_INFO_LOG("audio recorder muxer set parameter failed, ret = %d", ret);
        AUDIO_RECORDER_MUTEX_UNLOCK(recorderContext->instanceLock);
        return AUDIO_RECORDER_ERR;
    }
    ret = sscanf_s((const char *)item.value.pValue, "%llu,%u,%u,%u,%u,%u,%u,%u=%s",
        &info->fileSize, &info->fileDateYear, &info->fileDateMonth, &info->fileDateDay, &info->fileDateHour,
        &info->fileDateMin, &info->fileDateSec, &info->fileDuration, info->fileName, MAX_PATH_LEN);
    if (ret == -1) {
        MEDIA_ERR_LOG("sscanf_s failed, ret = %d", ret);
        AUDIO_RECORDER_MUTEX_UNLOCK(recorderContext->instanceLock);
        return AUDIO_RECORDER_ERR;
    }
    AUDIO_RECORDER_MUTEX_UNLOCK(recorderContext->instanceLock);
    return AUDIO_RECORDER_OK;
}

int32_t AudioRecorderCreateThread(AudioRecorderContext *recorderContext)
{
    MediaThreadattr producerAttr = { "AudioRecordProducerSch", MAX_PRODUCER_THREAD_STACK_SIZE,
        THREAD_SCHED_INVALID, 80, false };
    recorderContext->producerProcess = AUDIO_RECORDER_THREAD_CREATE(AudioRecorderProducerProcess,
        recorderContext, &producerAttr);
    if (recorderContext->producerProcess == NULL) {
        AUDIO_RECORDER_ERR_LOG("record prosucer sch process thread create failed");
        return AUDIO_RECORDER_ERR;
    }
    MediaThreadattr consumerAttr = { "AudioRecordConsumerSch", MAX_CONSUMER_THREAD_STACK_SIZE,
        THREAD_SCHED_INVALID, 80, false };
    recorderContext->consumerProcess = AUDIO_RECORDER_THREAD_CREATE(AudioRecorderConsumerProcess,
        recorderContext, &consumerAttr);
    if (recorderContext->consumerProcess == NULL) {
        AUDIO_RECORDER_ERR_LOG("record consumer sch process thread create failed");
        AUDIO_RECORDER_MUTEX_LOCK(recorderContext->producerLock);
        recorderContext->producerThreadExit = true;
        AUDIO_RECORDER_MUTEX_UNLOCK(recorderContext->producerLock);
        AUDIO_RECORDER_THREAD_COND_SIGNAL(recorderContext->producerCond);
        AUDIO_RECORDER_THREAD_JOIN(&recorderContext->producerProcess);
        return AUDIO_RECORDER_ERR;
    }
    return AUDIO_RECORDER_OK;
}

errcode_t AudioRecorderStart(const AudioRecorderHandle handle)
{
    AUDIO_RECORD_CHECK_NULL_RETURN(handle, AUDIO_RECORDER_INVALID_PARAM, "recorder is NULL!");
    AudioRecorderContext *recorderContext = GetAudioRecorderContext(handle);
    AUDIO_RECORD_CHECK_NULL_RETURN(recorderContext, AUDIO_RECORDER_INVALID_PARAM, "recorderContext is NULL!");
    AUDIO_RECORDER_MUTEX_LOCK(recorderContext->instanceLock);
    if (recorderContext->state == AUDIO_RECORD_STATUS_START) {
        AUDIO_RECORDER_INFO_LOG("no need to repeat record operation");
        AUDIO_RECORDER_MUTEX_UNLOCK(recorderContext->instanceLock);
        return AUDIO_RECORDER_ERR;
    }
    if (recorderContext->state != AUDIO_RECORD_STATUS_PREPARED) {
        AUDIO_RECORDER_ERR_LOG("The status is incorrect. The recored cannot be performed.state = %d",
            recorderContext->state);
        AUDIO_RECORDER_MUTEX_UNLOCK(recorderContext->instanceLock);
        return AUDIO_RECORDER_ERR;
    }
    errcode_t ret = AudioRecorderMuxerStart(recorderContext->muxerHandle);
    AUDIO_RECORD_CHECK_FAILED_RETURN_WITH_UNLOCK(ret, 0, ret, "muxer start failed", recorderContext->instanceLock);
    ret = AudioStreamSinkStart(recorderContext);
    if (ret != AUDIO_RECORDER_OK) {
        AUDIO_RECORDER_ERR_LOG("SinkStart failed");
        AUDIO_RECORDER_MUTEX_UNLOCK(recorderContext->instanceLock);
        return AUDIO_RECORDER_ERR;
    }
    if (AudioRecorderCreateThread(recorderContext) != AUDIO_RECORDER_OK) {
        AUDIO_RECORDER_ERR_LOG("AudioRecorderCreateThread failed");
        AudioRecorderDestroyBufferData(recorderContext);
        if (recorderContext->producerCond != NULL) {
            AUDIO_RECORDER_THREAD_COND_DESTROY(&recorderContext->producerCond);
        }
        if (recorderContext->consumerCond != NULL) {
            AUDIO_RECORDER_THREAD_COND_DESTROY(&recorderContext->consumerCond);
        }
        AUDIO_RECORDER_MUTEX_UNLOCK(recorderContext->instanceLock);
        return AUDIO_RECORDER_ERR;
    }
    recorderContext->state = AUDIO_RECORD_STATUS_START;
    AUDIO_RECORDER_MUTEX_UNLOCK(recorderContext->instanceLock);
    return AUDIO_RECORDER_OK;
}

errcode_t AudioRecorderStop(const AudioRecorderHandle handle)
{
    AUDIO_RECORD_CHECK_NULL_RETURN(handle, AUDIO_RECORDER_INVALID_PARAM, "lite recorder is NULL!");
    AudioRecorderContext *recorderContext = GetAudioRecorderContext(handle);
    AUDIO_RECORD_CHECK_NULL_RETURN(recorderContext, AUDIO_RECORDER_INVALID_PARAM, "recorderContext is NULL!");
    AUDIO_RECORDER_MUTEX_LOCK(recorderContext->instanceLock);
    if ((recorderContext->state != AUDIO_RECORD_STATUS_START) &&
        (recorderContext->state !=AUDIO_RECORD_STATUS_ERR)) {
        AUDIO_RECORDER_INFO_LOG("current state: %d, no need to do stop", recorderContext->state);
        AUDIO_RECORDER_MUTEX_UNLOCK(recorderContext->instanceLock);
        return AUDIO_RECORDER_ERR;
    }
    AUDIO_RECORDER_MUTEX_UNLOCK(recorderContext->instanceLock);
    if (recorderContext->producerProcess != NULL) {
        AUDIO_RECORDER_MUTEX_LOCK(recorderContext->producerLock);
        recorderContext->producerThreadExit = true;
        AUDIO_RECORDER_MUTEX_UNLOCK(recorderContext->producerLock);
        AUDIO_RECORDER_THREAD_COND_SIGNAL(recorderContext->producerCond);
        AUDIO_RECORDER_THREAD_JOIN(&recorderContext->producerProcess);
        recorderContext->producerProcess = NULL;
    }
    if (recorderContext->consumerProcess != NULL) {
        AUDIO_RECORDER_MUTEX_LOCK(recorderContext->instanceLock);
        recorderContext->consumerThreadExit = true;
        AUDIO_RECORDER_MUTEX_UNLOCK(recorderContext->instanceLock);
        AUDIO_RECORDER_THREAD_COND_SIGNAL(recorderContext->consumerCond);
        AUDIO_RECORDER_THREAD_JOIN(&recorderContext->consumerProcess);
        recorderContext->consumerProcess = NULL;
    }
    AUDIO_RECORDER_MUTEX_LOCK(recorderContext->instanceLock);
#ifdef SUPPORT_AUDIO_STREAM_IN_C
    errcode_t ret = AudioStreamInStop(recorderContext->audioStreamInHandle);
#else
    int32_t ret = AudioStreamInStop();
#endif
    if (ret != AUDIO_RECORDER_OK) {
        AUDIO_RECORDER_ERR_LOG("stream out stop failed = %d", ret);
        AUDIO_RECORDER_MUTEX_UNLOCK(recorderContext->instanceLock);
        return ret;
    }
    ret = AudioRecorderMuxerStop(recorderContext->muxerHandle);
    if (ret != AUDIO_RECORDER_OK) {
        AUDIO_RECORDER_ERR_LOG("muxer stop failed = %d", ret);
        AUDIO_RECORDER_MUTEX_UNLOCK(recorderContext->instanceLock);
        return ret;
    }
    recorderContext->state = AUDIO_RECORD_STATUS_STOPPED;
    AUDIO_RECORDER_MUTEX_UNLOCK(recorderContext->instanceLock);
    return AUDIO_RECORDER_OK;
}

errcode_t AudioRecorderDestroy(const AudioRecorderHandle handle)
{
    AUDIO_RECORD_CHECK_NULL_RETURN(handle, AUDIO_RECORDER_INVALID_PARAM, "audio recorder is NULL!");
    AudioRecorderContext *recorderContext = GetAudioRecorderContext(handle);
    AUDIO_RECORD_CHECK_NULL_RETURN(recorderContext, AUDIO_RECORDER_INVALID_PARAM, "recorderContext is NULL!");
    AUDIO_RECORDER_MUTEX_LOCK(recorderContext->instanceLock);
    if (recorderContext->state != AUDIO_RECORD_STATUS_IDLE &&
        recorderContext->state != AUDIO_RECORD_STATUS_STOPPED) {
        AUDIO_RECORDER_MUTEX_UNLOCK(recorderContext->instanceLock);
        AudioRecorderStop(handle);
        AUDIO_RECORDER_MUTEX_LOCK(recorderContext->instanceLock);
    }
    AudioRecorderMuxerDestroy(recorderContext->muxerHandle);
#ifdef SUPPORT_AUDIO_STREAM_IN_C
    AudioStreamInRelease(recorderContext->audioStreamInHandle);
    AudioStreamInDestroy(recorderContext->audioStreamInHandle);
#else
    AudioStreamInDeinit();
#endif
    AudioRecorderDestroyBufferData(recorderContext);
    if (recorderContext->producerCond != NULL) {
        AUDIO_RECORDER_THREAD_COND_DESTROY(&recorderContext->producerCond);
    }
    if (recorderContext->consumerCond != NULL) {
        AUDIO_RECORDER_THREAD_COND_DESTROY(&recorderContext->consumerCond);
    }
    recorderContext->isUsed = false;
    AUDIO_RECORDER_MUTEX_UNLOCK(recorderContext->instanceLock);
    AUDIO_RECORDER_MUTEX_DESTROY(&recorderContext->producerLock);
    return AUDIO_RECORDER_OK;
}
#ifdef __cplusplus
}
#endif

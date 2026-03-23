/*
 * Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
 * Description: audio recorder muxer define
 * Author: Media Software Group
 * Create: 2025-03-10
 */

#include "audio_recorder_muxer.h"
#include "media_log.h"
#include "format_interface.h"
#include "format_type.h"
#include "securec.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LOG_MODULE_NAME "AudioRecorderMuxer"
#define READ_FRAME_TIMEOUT 20
#define MUXER_INSTANCE_NUM 2
#define HANDLE_SHIFT_LEN 16
#define MUXER_INVAILD_ID 0xFFFF
#define MUXER_MAGIC_ID 0xBBBA
#define GET_MUXER_MAGIC_ID(handle) (((uint32_t)(handle)) >> HANDLE_SHIFT_LEN)
#define GET_MUXER_HANDLE(id) ((MUXER_MAGIC_ID << HANDLE_SHIFT_LEN) | (id))
#define GET_MUXER_ID(handle) (((uint32_t)(handle)) & 0xFFFF)

#define CHECK_NULL_RETURN(ptr, ret, outStr) \
do { \
    if ((ptr) == NULL) { \
        MEDIA_ERR_LOG("%s ", ((outStr) != NULL) ? (outStr) : " "); \
        return (ret); \
    } \
} while (0)

#define CHECK_FAILED_RETURN(value, target, ret, outStr) \
do { \
    if ((value) != (target)) { \
        MEDIA_ERR_LOG("%s ", ((outStr) != NULL) ? (outStr) : " "); \
        return (ret); \
    } \
} while (0)

typedef struct {
    bool inited;
    bool prepared;
    bool started;
    void *formatMuxerHandle;
    FormatCallback formatListener;
    int64_t maxFileSize;
    int64_t maxDuration;
    bool isUsed;
    MuxerCallback callBack;
    void *priv;
    FormatOutputConfig outputConfig;
} AudioRecorderMuxerContext;


static AudioRecorderMuxerContext g_muxerContext[MUXER_INSTANCE_NUM] = {
    {.formatMuxerHandle = NULL},
    {.formatMuxerHandle = NULL},
};

static int32_t g_muxerContextId[MUXER_INSTANCE_NUM] = { -1, -1};

static AudioRecorderMuxerContext *GetMuxerContext(const AudioRecorderMuxer handle)
{
    AudioRecorderMuxerContext *muxerContext = NULL;
    uint16_t muxerId;
    if (GET_MUXER_MAGIC_ID(handle) != MUXER_MAGIC_ID) {
        MEDIA_ERR_LOG("invalid muxer handle!");
        goto EXIT;
    }
    muxerId = GET_MUXER_ID(handle);
    if (muxerId >= MUXER_INSTANCE_NUM) {
        MEDIA_ERR_LOG("invalid muxer id: %u!", muxerId);
        goto EXIT;
    }
    if (!g_muxerContext[muxerId].isUsed) {
        MEDIA_ERR_LOG("muxer handle: 0x%x error, isUsed: %d", handle, g_muxerContext[muxerId].isUsed);
        goto EXIT;
    }
    muxerContext = &g_muxerContext[muxerId];
EXIT:
    return muxerContext;
}

static uint16_t GetNewMuxerId(void)
{
    uint16_t muxerId;
    for (muxerId = 0; muxerId < MUXER_INSTANCE_NUM; muxerId++) {
        if (g_muxerContextId[muxerId] == -1) {
            g_muxerContextId[muxerId] = muxerId;
            break;
        }
    }
    return muxerId;
}

static uint16_t GetMuxerAvailableId(void)
{
    uint16_t muxerId = GetNewMuxerId();
    if (muxerId >= MUXER_INSTANCE_NUM) {
        for (muxerId = 0; muxerId < MUXER_INSTANCE_NUM; muxerId++) {
            if (!g_muxerContext[muxerId].isUsed) {
                g_muxerContextId[muxerId] = -1;
            }
        }
        muxerId = GetNewMuxerId();
    }
    return muxerId;
}

static int32_t ErrorEventProc(void *handle, int32_t errorType, int32_t errorCode)
{
    AudioRecorderMuxerContext *muxerContext = (AudioRecorderMuxerContext *)handle;
    CHECK_NULL_RETURN(muxerContext, MEDIA_INVALID_PARAM, "handle is NULL!");
    if (muxerContext->callBack.OnErrorCallback == NULL || muxerContext->callBack.handle == NULL) {
        MEDIA_ERR_LOG("ErrorCallBack or handle is NULL");
        return MEDIA_INVALID_PARAM;
    }
    return muxerContext->callBack.OnErrorCallback(muxerContext->callBack.handle, errorType, errorCode);
}

static int32_t InfoEventProc(void *handle, int32_t type, int32_t extra)
{
    AudioRecorderMuxerContext *muxerContext = (AudioRecorderMuxerContext *)handle;
    CHECK_NULL_RETURN(muxerContext, MEDIA_INVALID_PARAM, "handle is NULL!");
    if (muxerContext->callBack.OnInfoCallback == NULL || muxerContext->callBack.handle == NULL) {
        MEDIA_ERR_LOG("InfoCallBack or handle is NULL");
        return MEDIA_INVALID_PARAM;
    }
    return muxerContext->callBack.OnInfoCallback(muxerContext->callBack.handle, type, extra);
}

int32_t AudioRecorderMuxerCreate(AudioRecorderMuxer *muxerHandle)
{
    CHECK_NULL_RETURN(muxerHandle, MEDIA_INVALID_PARAM, "handle is NULL!");
    uint16_t muxerId = GetMuxerAvailableId();
    if (muxerId >= MUXER_INSTANCE_NUM) {
        *muxerHandle = NULL;
        MEDIA_ERR_LOG("not found free asi handle id!");
        return MEDIA_ERR;
    }
    g_muxerContext[muxerId].inited = false;
    g_muxerContext[muxerId].prepared = false;
    g_muxerContext[muxerId].started = false;
    g_muxerContext[muxerId].formatMuxerHandle = NULL;
    g_muxerContext[muxerId].outputConfig.type = OUTPUT_TYPE_URI;
    g_muxerContext[muxerId].outputConfig.fd = -1;
    g_muxerContext[muxerId].outputConfig.format = OUTPUT_FORMAT_INVALID;
    g_muxerContext[muxerId].maxFileSize = -1;
    g_muxerContext[muxerId].maxDuration = -1;
    g_muxerContext[muxerId].formatListener.OnError = ErrorEventProc;
    g_muxerContext[muxerId].formatListener.OnInfo = InfoEventProc;
    g_muxerContext[muxerId].formatListener.privateDataHandle = &g_muxerContext[muxerId];
    g_muxerContext[muxerId].isUsed = true;
    g_muxerContext[muxerId].priv = NULL;
    *muxerHandle = (AudioRecorderMuxer)GET_MUXER_HANDLE(muxerId);
    
    return MEDIA_OK;
}

static void ConvertFormat(AudioCodecFormat formatType, FormatOutputConfig *outputConfig)
{
    if (outputConfig == NULL) {
        MEDIA_ERR_LOG("outputConfig is NULL!");
        return;
    }
    switch (formatType) {
        case MP3:
            outputConfig->format = OUTPUT_FORMAT_MP3;
            break;
        case PCM:
            outputConfig->format = OUTPUT_FORMAT_WAV;
            break;
        case SILK:
            outputConfig->format = OUTPUT_FORMAT_SILK;
            break;
        case OPUS:
            outputConfig->format = OUTPUT_FORMAT_OGG;
            break;
        default:
            MEDIA_ERR_LOG("not support format type = %d,", formatType);
            break;
    }
}

int32_t AudioRecorderMuxerSetOutputPath(AudioRecorderMuxer muxerHandle,
    AudioCodecFormat formatType, const char *outPath, const uint32_t outPathLen)
{
    CHECK_NULL_RETURN(muxerHandle, MEDIA_INVALID_PARAM, "muxer handle is null");
    AudioRecorderMuxerContext *muxerContext = GetMuxerContext(muxerHandle);
    CHECK_NULL_RETURN(muxerContext, MEDIA_INVALID_PARAM, "muxer context is invalid!");
    if (memcpy_s(muxerContext->outputConfig.url, URL_LEN, outPath, outPathLen) != EOK) {
        MEDIA_ERR_LOG("memcpy_s failed %s", outPath);
        return MEDIA_INVALID_PARAM;
    }
    ConvertFormat(formatType, &muxerContext->outputConfig);
    int32_t ret = FormatMuxerCreate(&muxerContext->formatMuxerHandle, &muxerContext->outputConfig);
    if (ret != MEDIA_OK || muxerContext->formatMuxerHandle == NULL) {
        muxerContext->isUsed = false;
        MEDIA_ERR_LOG("FormatMuxerCreate failed");
        return MEDIA_ERR;
    }
    muxerContext->inited = true;
    return MEDIA_OK;
}

int32_t AudioRecorderMuxerAddTrack(AudioRecorderMuxer muxerHandle, const CapturerInputConfig *info)
{
    CHECK_NULL_RETURN(muxerHandle, MEDIA_INVALID_PARAM, "muxer handle is null");
    AudioRecorderMuxerContext *muxerContext = GetMuxerContext(muxerHandle);
    CHECK_NULL_RETURN(muxerContext, MEDIA_INVALID_PARAM, "muxer context is invalid!");
    TrackSource trackSource = {};
    trackSource.trackSourceType = TRACK_SOURCE_TYPE_AUDIO;
    trackSource.trackSourceInfo.audioInfo.codecType = CODEC_PCM;
    trackSource.trackSourceInfo.audioInfo.sampleRate = info->sampleRate;
    trackSource.trackSourceInfo.audioInfo.channelCount = info->channelCount;
#ifdef SUPPORT_NEW_FORMAT
    trackSource.trackSourceInfo.audioInfo.sampleBitWidth = (FormatAudioSampleFmt)info->bitWidth;
#else
    trackSource.trackSourceInfo.audioInfo.sampleBitWidth = (AudioSampleFmt)info->bitWidth;
#endif
    trackSource.trackSourceInfo.audioInfo.samplesPerFrame = 0;
    trackSource.trackSourceInfo.audioInfo.avgBytesPerSec = 0;
    return FormatMuxerAddTrack(muxerContext->formatMuxerHandle, &trackSource);
}

int32_t AudioRecorderMuxerSetMaxDuration(AudioRecorderMuxer muxerHandle, int64_t duration)
{
    CHECK_NULL_RETURN(muxerHandle, MEDIA_INVALID_PARAM, "muxer handle is null");
    AudioRecorderMuxerContext *muxerContext = GetMuxerContext(muxerHandle);
    CHECK_NULL_RETURN(muxerContext, MEDIA_INVALID_PARAM, "muxer context is invalid!");
    if (muxerContext->started) {
        MEDIA_ERR_LOG("Recorder is started ,SetMaxDuration must be setted before Prepare");
        return MEDIA_ERR;
    }
    if (duration <= 0) {
        MEDIA_ERR_LOG("invalid  MaxDuration size:%lld", duration);
        return MEDIA_INVALID_PARAM;
    }
    muxerContext->maxDuration = duration;
    if (!muxerContext->prepared) {
        MEDIA_ERR_LOG("current state is not prepared, can not set!");
        return MEDIA_ERR;
    }
    return FormatMuxerSetMaxFileDuration(muxerContext->formatMuxerHandle, duration);
}

int32_t AudioRecorderMuxerSetMaxFileSize(AudioRecorderMuxer muxerHandle, int64_t fileSize)
{
    CHECK_NULL_RETURN(muxerHandle, MEDIA_INVALID_PARAM, "muxer handle is null");
    AudioRecorderMuxerContext *muxerContext = GetMuxerContext(muxerHandle);
    CHECK_NULL_RETURN(muxerContext, MEDIA_INVALID_PARAM, "muxer context is invalid!");
    if (muxerContext->started) {
        MEDIA_ERR_LOG("Recorder is started ,SetMaxFileSize must be setted before Prepare");
        return MEDIA_ERR;
    }
    if (fileSize <= 0) {
        MEDIA_ERR_LOG("invalid  MaxFileSize size:%lld", fileSize);
        return MEDIA_INVALID_PARAM;
    }
    muxerContext->maxFileSize = fileSize;
    if (!muxerContext->prepared) {
        MEDIA_ERR_LOG("current state is not prepared, can not set!");
        return MEDIA_ERR;
    }
    return FormatMuxerSetMaxFileSize(muxerContext->formatMuxerHandle, fileSize);
}

int32_t AudioRecorderMuxerSetCallBack(AudioRecorderMuxer muxerHandle, const MuxerCallback *callBack)
{
    CHECK_NULL_RETURN(muxerHandle, MEDIA_INVALID_PARAM, "muxer handle is null");
    CHECK_NULL_RETURN(callBack, MEDIA_INVALID_PARAM, "callBack is null");
    AudioRecorderMuxerContext *muxerContext = GetMuxerContext(muxerHandle);
    CHECK_NULL_RETURN(muxerContext, MEDIA_INVALID_PARAM, "muxer context is invalid!");
    muxerContext->callBack = *callBack;
    return MEDIA_OK;
}

int32_t AudioRecorderMuxerPrepare(AudioRecorderMuxer muxerHandle)
{
    CHECK_NULL_RETURN(muxerHandle, MEDIA_INVALID_PARAM, "muxer handle is null");
    AudioRecorderMuxerContext *muxerContext = GetMuxerContext(muxerHandle);
    CHECK_FAILED_RETURN(muxerContext->inited, true, MEDIA_INVALID_PARAM, "not inited");
    if (muxerContext->prepared) {
        return MEDIA_OK;
    }
    if (muxerContext->maxDuration != -1) {
        int32_t ret = FormatMuxerSetMaxFileDuration(muxerContext->formatMuxerHandle, muxerContext->maxDuration);
        if (ret != MEDIA_OK) {
            MEDIA_ERR_LOG("FormatMuxersetMaxFileDuration failed 0x%x", ret);
        }
    }
    if (muxerContext->maxFileSize != -1) {
        int32_t ret = FormatMuxerSetMaxFileSize(muxerContext->formatMuxerHandle, muxerContext->maxFileSize);
        if (ret != MEDIA_OK) {
            MEDIA_ERR_LOG("FormatMuxersetMaxFileSize failed 0x%x", ret);
        }
    }
    if (FormatMuxerSetCallBack(muxerContext->formatMuxerHandle, &muxerContext->formatListener) != 0) {
        MEDIA_ERR_LOG("FormatMuxerSetCallBack failed");
    }
    muxerContext->prepared = true;
    return MEDIA_OK;
}

int32_t AudioRecorderMuxerStart(AudioRecorderMuxer muxerHandle)
{
    CHECK_NULL_RETURN(muxerHandle, MEDIA_INVALID_PARAM, "muxer handle is null");
    AudioRecorderMuxerContext *muxerContext = GetMuxerContext(muxerHandle);
    CHECK_NULL_RETURN(muxerContext, MEDIA_INVALID_PARAM, "muxer context is invalid!");
    CHECK_FAILED_RETURN(muxerContext->prepared, true, -1, "not prepared");
    int32_t ret = FormatMuxerStart(muxerContext->formatMuxerHandle);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("FormatMuxerStart failed 0x%x", ret);
        return ret;
    }
    muxerContext->started = true;
    return MEDIA_OK;
}

int32_t AudioRecorderMuxerWriteData(AudioRecorderMuxer muxerHandle, FormatFrame *frame)
{
    CHECK_NULL_RETURN(muxerHandle, MEDIA_INVALID_PARAM, "muxer handle is null");
    AudioRecorderMuxerContext *muxerContext = GetMuxerContext(muxerHandle);
    CHECK_NULL_RETURN(muxerContext, MEDIA_INVALID_PARAM, "muxer context is invalid!");
    CHECK_FAILED_RETURN(muxerContext->started, true, -1, "not started");
    return FormatMuxerWriteFrame(muxerContext->formatMuxerHandle, frame);
}

int32_t AudioRecorderMuxerSetParameter(AudioRecorderMuxer muxerHandle, const ParameterItem *item, int32_t itemNum)
{
    CHECK_NULL_RETURN(muxerHandle, MEDIA_INVALID_PARAM, "muxer handle is null");
    AudioRecorderMuxerContext *muxerContext = GetMuxerContext(muxerHandle);
    CHECK_NULL_RETURN(muxerContext, MEDIA_INVALID_PARAM, "muxer context is invalid!");
    CHECK_FAILED_RETURN(muxerContext->prepared, true, -1, "not prepared");
    return FormatMuxerSetParameter(muxerContext->formatMuxerHandle, 0, item, itemNum);
}

int32_t AudioRecorderMuxerGetParameter(AudioRecorderMuxer muxerHandle, ParameterItem *item, int32_t itemNum)
{
    CHECK_NULL_RETURN(muxerHandle, MEDIA_INVALID_PARAM, "muxer handle is null");
    AudioRecorderMuxerContext *muxerContext = GetMuxerContext(muxerHandle);
    CHECK_NULL_RETURN(muxerContext, MEDIA_INVALID_PARAM, "muxer context is invalid!");
    CHECK_FAILED_RETURN(muxerContext->started, true, -1, "not started");
    return FormatMuxerGetParameter(muxerContext->formatMuxerHandle, 0, item, itemNum);
}

int32_t AudioRecorderMuxerStop(AudioRecorderMuxer muxerHandle)
{
    CHECK_NULL_RETURN(muxerHandle, MEDIA_INVALID_PARAM, "muxer handle is null");
    AudioRecorderMuxerContext *muxerContext = GetMuxerContext(muxerHandle);
    CHECK_NULL_RETURN(muxerContext, MEDIA_INVALID_PARAM, "muxer context is invalid!");
    if (muxerContext->formatMuxerHandle == NULL) {
        return MEDIA_OK;
    }
    return FormatMuxerStop(muxerContext->formatMuxerHandle, true);
}

int32_t AudioRecorderMuxerDestroy(AudioRecorderMuxer muxerHandle)
{
    CHECK_NULL_RETURN(muxerHandle, MEDIA_INVALID_PARAM, "muxer handle is null");
    AudioRecorderMuxerContext *muxerContext = GetMuxerContext(muxerHandle);
    CHECK_NULL_RETURN(muxerContext, MEDIA_INVALID_PARAM, "muxer context is invalid!");
    if (muxerContext->formatMuxerHandle != NULL) {
        FormatMuxerDestroy(muxerContext->formatMuxerHandle);
        muxerContext->formatMuxerHandle = NULL;
    }
    muxerContext->isUsed = false;
    return MEDIA_OK;
}
#ifdef __cplusplus
}
#endif
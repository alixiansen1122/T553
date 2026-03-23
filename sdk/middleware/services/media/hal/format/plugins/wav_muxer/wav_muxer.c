/*
* Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
* Description: wav_muxer.c
* Author: Media Software Group
* Create: 2025-03-10
*/

#include "wav_muxer.h"
#include <malloc.h>
#include <math.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/prctl.h>
#include <unistd.h>
#include "muxer_interface.h"
#include "dtcf_manager.h"
#include "media_hal_common.h"
#include "wav.h"
#include "securec.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define MODULE_NAME "WavMuxer"

#define BYTE_UNIT 1024
#define WAV_MUXER_PRIORITY 100
#ifdef SUPPORT_FEATURE_CONFIG
#define INTERVAL_RECORD_FRAME (1000 / CONFIG_AUDIO_AI_FRAME_PER_SEC)
#else
#define INTERVAL_RECORD_FRAME 10
#endif
#define MSEC_PER_SEC (1000)
#define REC_SPLIT_TIME_THRESHOLD (0.9)
#define REC_SPLIT_SIZE_THRESHOLD (0.9)
#ifdef TIME_CONSUMING_PRINTING
#define MAX_TIMES 100
#define MAX_TIME 100000
#endif
#define REC_US_TO_S (1000000.0)

#define SIZE_PER_SAMPLE 2

/* <max file size, determined by the file system */
#ifndef ONLY_UT_TEST
/* (4*1024*1024*1024-1*1024*1024*1024) */
#define REC_MAX_FILE_SIZE (3221225472LL)
#else
/* 20*1024*1024 */
#define REC_MAX_FILE_SIZE (20971520LL)
#endif
#define FILE_INFO_BUFFER_SIZE 350


typedef struct {
    MuxerOutput muxerOutput;
    MuxerTrackSource trackSource;
    char fileName[REC_FILE_NAME_LEN];
    WavFile *fd;
    bool muxerStarted;
    int64_t maxFileSize;
    int64_t maxDuration;
    PluginCallback callBack;
    DtcfCallBack dtcfCallBack;
    uint64_t streamSize;
    bool splitSizeBegin;
    bool splitTimeBegin;
    uint32_t realRecTimeLenMs;
    uint64_t baseFramePts;
    MediaHalMutexHandle mutex;
    uint64_t maxMemorySize;
    uint64_t startTime;
    bool isFirstStart;
    bool isSupportSplit;
} WavMuxerContext;

typedef struct {
    int32_t dataLength;
    int64_t frameTimeStamp;
} WavFrameData;

#ifdef TIME_CONSUMING_PRINTING
static uint32_t g_times = 0;
static uint32_t g_startTime = 0;
static uint32_t g_endTime = 0;
static uint32_t g_maxTime = 0;
static uint32_t g_minTime = MAX_TIME;
#endif

static void GetCurrentTime(uint64_t *time)
{
    if (time == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "time is NULL!");
        return;
    }
    *time = uapi_tcxo_get_us();
}

static void ReportInfo(PluginCallback callBack, PluginInfoType infoType, int32_t extra)
{
    CHK_NULL_RETURN_NONE(callBack.OnInfo, "OnInfo invalid pointer");
    callBack.OnInfo(callBack.handle, infoType, extra);
}

static void ReportError(PluginCallback callBack, int32_t errorType, int32_t errorCode)
{
    CHK_NULL_RETURN_NONE(callBack.OnError, "OnError invalid pointer");
    callBack.OnError(callBack.handle, errorType, errorCode);
}

static int32_t DtcfEventCallBack(void *callBack, DtcfCallBackType errorType, int32_t errorCode)
{
    CHK_NULL_RETURN(callBack, MEDIA_HAL_INVALID_PARAM, "callBack invalid pointer");
    WavMuxerContext *context = (WavMuxerContext *)callBack;
    CHK_NULL_RETURN(context->callBack.OnError, MEDIA_HAL_INVALID_PARAM, "OnError invalid pointer");
    PluginErrorType pluginErrorType;
    switch (errorType) {
        case DELETE_THE_OLDEST_FAILED:
            context->callBack.OnError(context->callBack.handle, ERROR_TYPE_INTERNAL_OPERATION_FAIL, errorCode);
            break;
        case MEMORY_REACH_THE_MAXIMUM_VALUE:
            context->callBack.OnInfo(context->callBack.handle, MAX_MEMORYSIZE_REACHED, errorCode);
            break;
        default:
            MEDIA_HAL_LOGE(MODULE_NAME, "not support type = %d", errorType);
            break;
    }
    return MEDIA_HAL_OK;
}

static int32_t WavMuxerFind(const MuxerOutput *output, const ProtocolFun *protocol)
{
    CHK_NULL_RETURN(output, MEDIA_HAL_INVALID_PARAM, "output is NULL");
    if (output->type == OUT_TYPE_INVALID) {
        MEDIA_HAL_LOGE(MODULE_NAME, "out type invlid");
        return MEDIA_HAL_INVALID_PARAM;
    }
    if (protocol != NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "not support extra protocol");
        return MEDIA_HAL_INVALID_PARAM;
    }
    if (output->format != FORMAT_TYPE_WAV) {
        return MEDIA_HAL_ERR;
    }
    MEDIA_HAL_LOGI(MODULE_NAME, "WavMuxer Found");
    return MEDIA_HAL_OK;
}

static int32_t OpenOutTypeUriProcess(const MuxerOutput *output, DtcfCallBack callBack)
{
    int32_t optInit = DtcfMgrFileOptInit(output->url, strlen(output->url) + 1, callBack);
    if (optInit != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "DtcfMgrFileOptInit fail 0x%x", optInit);
        (void)DtcfMgrFileOptDeInit();
        return MEDIA_HAL_ERR;
    }
    return MEDIA_HAL_OK;
}

static int32_t InitWavMuxer(WavMuxerContext *ctx)
{
    if (ctx->muxerOutput.type == OUT_TYPE_URI) {
        if (DtcfMgrIsFile(ctx->muxerOutput.url, REC_FILE_NAME_LEN)) {
            ctx->isSupportSplit = false;
            MEDIA_HAL_LOGI(MODULE_NAME, "DtcfMgrIsFile is enter, ctx->muxerOutput.url = %s", ctx->muxerOutput.url);
            int32_t ret = DtcfMgrMakeDir(ctx->muxerOutput.url, REC_FILE_NAME_LEN);
            if (ret != MEDIA_HAL_OK) {
                MEDIA_HAL_LOGE(MODULE_NAME, "DtcfMgrMakeDir error");
                return MEDIA_HAL_ERR;
            }
            ctx ->fd = WavOpen(ctx->muxerOutput.url, WAV_OPEN_WRITE);
            if (ctx->fd == NULL) {
                MEDIA_HAL_LOGE(MODULE_NAME, "open file error, errno = %d", errno);
                return MEDIA_HAL_ERR;
            }
            if (memcpy_s(ctx->fileName, REC_FILE_NAME_LEN, ctx->muxerOutput.url, REC_FILE_NAME_LEN) != EOK) {
                MEDIA_HAL_LOGE(MODULE_NAME, "memcpy_s failed");
                WavClose(ctx->fd);
                ctx->fd = NULL;
                return MEDIA_HAL_ERR;
            }
            return MEDIA_HAL_OK;
        }
        ctx->isSupportSplit = true;
        int32_t ret = OpenOutTypeUriProcess(&ctx->muxerOutput, ctx->dtcfCallBack);
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "OpenOutTypeUriProcess error, ret = %d", ret);
            return MEDIA_HAL_ERR;
        }
        char fileNames[2][REC_FILE_NAME_LEN];
        ret = DtcfMgrRequestFileNames(DTCF_FILE_TYPE_WAV, DTCF_DIR_RECORD_WAV_FILE, fileNames);
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "DtcfMgrRequestFileNames error");
            return MEDIA_HAL_ERR;
        }
        ctx ->fd = WavOpen(fileNames[0], WAV_OPEN_WRITE);
        if (ctx->fd == NULL) {
            MEDIA_HAL_LOGE(MODULE_NAME, "wav open failed!");
            return MEDIA_HAL_ERR;
        }
        if (memcpy_s(ctx->fileName, REC_FILE_NAME_LEN, fileNames[0], REC_FILE_NAME_LEN) != EOK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "memcpy_s failed");
            return MEDIA_HAL_ERR;
        }
        WavWriteHeader(ctx->fd);
    }
    return MEDIA_HAL_OK;
}

static int32_t WavMuxerOpen(const MuxerOutput *output, const ProtocolFun *protocol, RecMuxerHandle *handle)
{
    MEDIA_HAL_UNUSED(protocol);
    CHK_NULL_RETURN(output, MEDIA_HAL_INVALID_PARAM, "output is null");
    CHK_NULL_RETURN(handle, MEDIA_HAL_INVALID_PARAM, "handle is null");
    
    WavMuxerContext *ctx = (WavMuxerContext*)malloc(sizeof(WavMuxerContext));
    CHK_NULL_RETURN(ctx, MEDIA_HAL_NO_MEM, "WavMuxerContext is null");
    int32_t ret = MEDIA_HAL_OK;
    if (memset_s(ctx, sizeof(WavMuxerContext), 0x0, sizeof(WavMuxerContext)) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "memset_s WavMuxerContext error");
        goto CREATE_FAILED;
    }
    ctx->mutex = MediaHalMutexCreate(NULL);
    if (ctx->mutex == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "MediaHalMutexCreate failed!");
        goto CREATE_FAILED;
    }
    ctx->muxerOutput = *output;
    ctx->dtcfCallBack.handle = ctx;
    ctx->dtcfCallBack.CallBack = DtcfEventCallBack;
    ret = InitWavMuxer(ctx);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "CreateWavMuxer failed:0x%x", ret);
        goto MUXER_DESTORY;
    }
    *handle = ctx;
    if (ctx->isSupportSplit) {
        ret = DtcfMgrGetStorageMaxSize(&ctx->maxMemorySize);
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "DtcfGetStorageMaxSize failed!");
        }
    }
    MEDIA_HAL_LOGI(MODULE_NAME, "WavMuxerOpen :%d", ret);
    return MEDIA_HAL_OK;
MUXER_DESTORY:
    MediaHalMutexDestroy(&ctx->mutex);
CREATE_FAILED:
    free(ctx);
    return MEDIA_HAL_ERR;
}

static int32_t DeinitWavMuxer(WavMuxerContext *ctx)
{
    CHK_NULL_RETURN(ctx, MEDIA_HAL_INVALID_PARAM, "handle");
    WavWriteHeader(ctx->fd);
    WavClose(ctx->fd);
    ctx->fd = NULL;
    ctx->muxerStarted = false;
    ctx->isFirstStart = false;
    ctx->isSupportSplit = false;
    ctx->startTime = 0;
    MEDIA_HAL_LOGI(MODULE_NAME, "DeinitWavMuxer success");
    return MEDIA_HAL_OK;
}

static int32_t WavMuxerDestroy(RecMuxerHandle handle)
{
    WavMuxerContext *ctx = (WavMuxerContext *)handle;
    CHK_NULL_RETURN(ctx, MEDIA_HAL_INVALID_PARAM, "handle");
    int32_t ret = DeinitWavMuxer(ctx);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "call DeinitWavMuxer fail:0x%x", ret);
    }
    if (ctx->isSupportSplit) {
        (void)DtcfMgrFileOptDeInit();
    }
    MediaHalMutexDestroy(&ctx->mutex);
    free(ctx);
    MEDIA_HAL_LOGI(MODULE_NAME, "WavMuxerDestroy success");
    return MEDIA_HAL_OK;
}

static int32_t AddAudioTrack(WavMuxerContext *ctx)
{
    CHK_NULL_RETURN(ctx, MEDIA_HAL_INVALID_PARAM, "ctx is null");
    WavSetSampleRate(ctx->fd, ctx->trackSource.info.audioInfo.sampleRate);
#ifdef MELODY_DEMO_BMIC_TX
    WavSetNumChannels(ctx->fd, 1);
#else
    WavSetNumChannels(ctx->fd, ctx->trackSource.info.audioInfo.channelCount);
#endif
    int32_t sampleBitWidth = ctx->trackSource.info.audioInfo.sampleBitWidth;
    if (sampleBitWidth == AUDIO_SAMPLE_FORMAT_FLOAT) {
        WavSetFormat(ctx->fd, WAV_FORMAT_IEEE_FLOAT);
        sampleBitWidth = AUDIO_SAMPLE_FORMAT_S32;
    }
    WavSetValidBitsPerSample(ctx->fd, sampleBitWidth);
    return MEDIA_HAL_OK;
}

static int32_t WavMuxerAddTrack(RecMuxerHandle handle, const MuxerTrackSource *trackSource)
{
    CHK_NULL_RETURN(trackSource, MEDIA_HAL_INVALID_PARAM, "trackSource is null");
    WavMuxerContext *ctx = (WavMuxerContext *)handle;
    CHK_NULL_RETURN(ctx, MEDIA_HAL_INVALID_PARAM, "WavMuxerContext is null");
    if (trackSource->type != STREAM_TYPE_AUDIO) {
        MEDIA_HAL_LOGE(MODULE_NAME, "only support STREAM_TYPE_AUDIO!");
        return MEDIA_HAL_ERR;
    }
    ctx->trackSource = *trackSource;
    int32_t ret = AddAudioTrack(ctx);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "MuxerAddTrack failed [0x%x]", ret);
    }
    return ret;
}

static int32_t WavMuxerStart(RecMuxerHandle handle)
{
    WavMuxerContext *ctx = (WavMuxerContext *)handle;
    CHK_NULL_RETURN(ctx, MEDIA_HAL_INVALID_PARAM, "ctx is null");
    CHK_COND_RETURN(ctx->muxerStarted, MEDIA_HAL_OK, "ctx is Started");
    ctx->muxerStarted = true;
    ctx->splitSizeBegin = !ctx->isSupportSplit;
    ctx->splitTimeBegin = !ctx->isSupportSplit;
    MEDIA_HAL_LOGI(MODULE_NAME, "RawMuxerStart success ");
    return MEDIA_HAL_OK;
}

static bool SplitIsAchiveSize(WavMuxerContext *ctx)
{
    bool result = false;
    if (ctx->splitSizeBegin) {
        if (ctx->maxFileSize > 0 && ctx->streamSize >= ctx->maxFileSize * REC_SPLIT_SIZE_THRESHOLD) {
            ReportInfo(ctx->callBack, MAX_FILESIZE_APPROACHING,
                ctx->muxerOutput.type == OUT_TYPE_FD ? ctx->muxerOutput.fd : -1);
            MEDIA_HAL_LOGI(MODULE_NAME, "size %lld Approaching maxFileSize %lld", ctx->streamSize, ctx->maxFileSize);
            ctx->splitSizeBegin = false;
        }
    }

    if ((ctx->maxFileSize > 0 && ctx->streamSize >= (uint64_t)ctx->maxFileSize) ||
        ctx->streamSize >= REC_MAX_FILE_SIZE) {
        ReportInfo(ctx->callBack, MAX_FILESIZE_REACHED,
            ctx->muxerOutput.type == OUT_TYPE_FD ? ctx->muxerOutput.fd : -1);
        result = true;
        MEDIA_HAL_LOGI(MODULE_NAME, "size %lld has Approached maxFileSize %lld", ctx->streamSize, ctx->maxFileSize);
        ctx->streamSize = 0;
        ctx->splitSizeBegin = true;
        ctx->baseFramePts = 0;
    }
    return result;
}

static bool SplitIsAchiveTime(WavMuxerContext *ctx)
{
    bool result = false;
    uint32_t realRecTimeLen = ctx->realRecTimeLenMs / MSEC_PER_SEC;
    if (ctx->splitTimeBegin) {
        if (ctx->maxDuration > 0 && realRecTimeLen >= ctx->maxDuration * REC_SPLIT_TIME_THRESHOLD) {
            ReportInfo(ctx->callBack, MAX_DURATION_APPROACHING,
                ctx->muxerOutput.type == OUT_TYPE_FD ? ctx->muxerOutput.fd : -1);
            MEDIA_HAL_LOGI(MODULE_NAME, "realRecTimeLen %u Approaching maxDuration %lld",
                realRecTimeLen, ctx->maxDuration);
            ctx->splitTimeBegin = false;
        }
    }
    if (ctx->maxDuration > 0 && realRecTimeLen >= ctx->maxDuration) {
        // split and reset manual split info
        ReportInfo(ctx->callBack, MAX_DURATION_REACHED,
            ctx->muxerOutput.type == OUT_TYPE_FD ? ctx->muxerOutput.fd : -1);
        result = true;
        MEDIA_HAL_LOGI(MODULE_NAME, "time %u has Approached maxDuration %lld", ctx->realRecTimeLenMs, ctx->maxDuration);
        ctx->splitTimeBegin = true;
        ctx->baseFramePts = 0;
        ctx->streamSize = 0;
    }
    return result;
}

static int32_t ReConfigMuxer(WavMuxerContext *ctx)
{
    int32_t ret = DeinitWavMuxer(ctx);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "call DeinitWavMuxer fail:0x%x", ret);
        return MEDIA_HAL_ERR;
    }
    ret = InitWavMuxer(ctx);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "CreateWavMuxer failed:0x%x", ret);
        return MEDIA_HAL_ERR;
    }
    ret = AddAudioTrack(ctx);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AddAudioTrack failed:0x%x", ret);
        DeinitWavMuxer(ctx);
        return MEDIA_HAL_ERR;
    }
    ctx->muxerStarted = true;
    MEDIA_HAL_LOGE(MODULE_NAME, "ReConfigMuxer success");
    return MEDIA_HAL_OK;
}

#ifdef TIME_CONSUMING_PRINTING
static uint32_t WavMuxerWriteStartTime(void)
{
    if (g_times == 0) {
        g_startTime = uapi_tcxo_get_ms();
    }
    return uapi_tcxo_get_ms();
}

static void WavMuxerWriteEndTime(uint32_t startTime)
{
    g_times++;
    uint32_t endTime = uapi_tcxo_get_ms();
    uint32_t writeTime = endTime - startTime;
    g_maxTime = (writeTime > g_maxTime) ? writeTime : g_maxTime;
    g_minTime = (writeTime < g_minTime) ? writeTime : g_minTime;
    if (g_times == MAX_TIMES) {
        g_endTime = uapi_tcxo_get_ms();
        uint32_t totalTime = g_endTime - g_startTime;
        MEDIA_HAL_LOGI(MODULE_NAME, " WavWrite totalTime = %ums", totalTime);
        MEDIA_HAL_LOGI(MODULE_NAME, " WavWrite maxTime = %ums", g_maxTime);
        MEDIA_HAL_LOGI(MODULE_NAME, " WavWrite minTime = %ums", g_minTime);
        g_times = 0;
        g_maxTime = 0;
        g_minTime = MAX_TIME;
    }
}
#endif

static int32_t WavMuxerWriteFrameInner(WavMuxerContext *ctx, const FormatPacket *frameData,
    const WavFrameData *wavFrameData)
{
#ifdef TIME_CONSUMING_PRINTING
    uint32_t startTime = WavMuxerWriteStartTime();
#endif
    if (!ctx->isFirstStart) {
        GetCurrentTime(&ctx->startTime);
        ctx->isFirstStart = true;
    }
    if (WavWrite(ctx->fd, frameData->data, frameData->len) != frameData->len) {
        MEDIA_HAL_LOGE(MODULE_NAME, "fwrite failed errno:%d", errno);
        return MEDIA_HAL_ERR_AGAIN;
    }
#ifdef TIME_CONSUMING_PRINTING
        WavMuxerWriteEndTime(startTime);
#endif
    if (ctx->isSupportSplit) {
        if (ctx->streamSize == 0) {
            ctx->streamSize += wavFrameData->dataLength;
        }
        if (ctx->baseFramePts == 0) {
            ctx->baseFramePts = wavFrameData->frameTimeStamp;
        }
    }
    return MEDIA_HAL_OK;
}

static int32_t HandleWavFrame(WavMuxerContext *ctx, WavFrameData *wavFrameData, const FormatPacket *frameData)
{
    CHK_NULL_RETURN(frameData, MEDIA_HAL_INVALID_PARAM, "wavFrameData is NULL!");
    wavFrameData->dataLength = frameData->len;
    if (ctx->isSupportSplit) {
        wavFrameData->frameTimeStamp = frameData->timestampUs;
        uint32_t frameNum = frameData->timestampUs / (MSEC_PER_SEC* INTERVAL_RECORD_FRAME);
        uint32_t frameSize = frameData->len;
        if (frameNum != 0) {
            frameSize = frameData->len / frameNum;
        }
        MEDIA_HAL_LOGI(MODULE_NAME, "frameData->timestampUs = %lld, frameNum = %d, frameSize = %d!",
            frameData->timestampUs, frameNum, frameSize);
        if (ctx->maxDuration < 1 || ctx->maxFileSize < frameSize) {
            MEDIA_HAL_LOGE(MODULE_NAME, "file duration or size is not right, write file failed!");
            return MEDIA_HAL_ERR;
        }
    }
    return MEDIA_HAL_OK;
}

static int32_t WavMuxerWriteFrame(RecMuxerHandle handle, const FormatPacket *frameData)
{
    CHK_NULL_RETURN(frameData, MEDIA_HAL_INVALID_PARAM, "frameData is NULL!");
    WavMuxerContext *ctx = (WavMuxerContext *)handle;
    CHK_NULL_RETURN(ctx, MEDIA_HAL_INVALID_PARAM, "WavMuxerContext is null");
    CHK_COND_RETURN(!ctx->muxerStarted, MEDIA_HAL_ERR, "WavMuxerContext is not Started");
    if (ctx->isSupportSplit) {
        if (DtcfMgrHandleRecordMaxSize(frameData->len, ctx->maxMemorySize) != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "handle reac max size failed!");
            return MEDIA_HAL_ERR;
        }
    }
    WavFrameData wavFrameData = {};
    if (HandleWavFrame(ctx, &wavFrameData, frameData) != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "handle wav frame failed!");
        return MEDIA_HAL_ERR;
    }
    MediaHalMutexLock(ctx->mutex);
    int32_t ret = MEDIA_HAL_OK;
    if (ctx->isSupportSplit) {
        ctx->streamSize += wavFrameData.dataLength;
        ctx->realRecTimeLenMs = (uint32_t)((wavFrameData.frameTimeStamp - ctx->baseFramePts) / MSEC_PER_SEC);
        if (SplitIsAchiveSize(ctx) || SplitIsAchiveTime(ctx)) {
            ret = ReConfigMuxer(ctx);
            if (ret != MEDIA_HAL_OK) {
                ReportError(ctx->callBack, ERROR_TYPE_INTERNAL_OPERATION_FAIL, ERROR_TYPE_INTERNAL_OPERATION_FAIL);
                MediaHalMutexUnLock(ctx->mutex);
                return MEDIA_HAL_ERR;
            }
        }
    }
    ret = WavMuxerWriteFrameInner(ctx, frameData, &wavFrameData);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "WavMuxerWriteFrameInner failed!");
        MediaHalMutexUnLock(ctx->mutex);
        return ret;
    }
    MediaHalMutexUnLock(ctx->mutex);
    return MEDIA_HAL_OK;
}

static int32_t WavMuxerStop(RecMuxerHandle handle, bool block)
{
    CHK_NULL_RETURN(handle, MEDIA_HAL_INVALID_PARAM, "handle");
    WavMuxerContext *ctx = (WavMuxerContext *)handle;
    CHK_NULL_RETURN(ctx, MEDIA_HAL_INVALID_PARAM, "WavMuxerContext");
    CHK_COND_RETURN(!ctx->muxerStarted, MEDIA_HAL_ERR, "WavMuxerContext is not Started");
    MEDIA_HAL_LOGI(MODULE_NAME, " block %d", block);
    ctx->muxerStarted = false;
    return MEDIA_HAL_OK;
}

static int32_t WavMuxerSetMaxFileSize(RecMuxerHandle handle, int64_t bytes)
{
    CHK_NULL_RETURN(handle, MEDIA_HAL_INVALID_PARAM, "handle");
    WavMuxerContext *ctx = (WavMuxerContext *)handle;
    MEDIA_HAL_LOGI(MODULE_NAME, "bytes:%lld ", bytes);
    ctx->maxFileSize = bytes;
    return MEDIA_HAL_OK;
}

static int32_t WavMuxerSetMaxFileDuration(RecMuxerHandle handle, int64_t durationUs)
{
    CHK_NULL_RETURN(handle, MEDIA_HAL_INVALID_PARAM, "handle");
    WavMuxerContext *ctx = (WavMuxerContext *)handle;
    MEDIA_HAL_LOGI(MODULE_NAME, "durationUs:%lld ", durationUs);
    ctx->maxDuration = durationUs;
    return MEDIA_HAL_OK;
}

static int32_t WavMuxerSetNextOutputFile(RecMuxerHandle handle, int32_t fd)
{
    WavMuxerContext *ctx = (WavMuxerContext *)handle;
    CHK_NULL_RETURN(ctx, MEDIA_HAL_INVALID_PARAM, "handle is null");
    MEDIA_HAL_LOGI(MODULE_NAME, "fd:%d", fd);
    if (ctx->muxerOutput.type == OUT_TYPE_URI) {
        return MEDIA_HAL_ERR;
    }
    ctx->muxerOutput.fd = fd;
    return MEDIA_HAL_OK;
}

static int32_t MuxerSetCallBack(WavMuxerContext *ctx, const PluginCallback *callBack)
{
    CHK_NULL_RETURN(callBack, MEDIA_HAL_INVALID_PARAM, "callBack");
    ctx->callBack = *callBack;
    return MEDIA_HAL_OK;
}

static void GetCurrentStorageTimeRemaindTime(WavMuxerContext *ctx, uint32_t packetLen, uint32_t *time)
{
    if (!ctx->isSupportSplit) {
        MEDIA_HAL_LOGE(MODULE_NAME, "not support get recorder time!");
        return;
    }
    MediaHalMutexLock(ctx->mutex);
    *time = ((DtcfMgrGetCurrentMemorySize(ctx->maxMemorySize) - DtcfMgrGetCurrentRecordedSize()) / packetLen) *
        INTERVAL_RECORD_FRAME;
    MEDIA_HAL_LOGI(MODULE_NAME, "remaind storage packetLen = %u, time:%u\n", packetLen, *time);
    MediaHalMutexUnLock(ctx->mutex);
}

static void GetCurrentFileRemaindTime(WavMuxerContext *ctx, uint32_t packetLen, uint32_t *time)
{
    if (!ctx->isSupportSplit) {
        MEDIA_HAL_LOGE(MODULE_NAME, "not support get file time!");
        return;
    }
    MediaHalMutexLock(ctx->mutex);
    uint32_t currentSec = ((ctx->maxFileSize / packetLen) * INTERVAL_RECORD_FRAME) / MSEC_PER_SEC;
    if (currentSec >= ctx->maxDuration) {
        *time = (ctx->maxDuration * MSEC_PER_SEC) - ctx->realRecTimeLenMs;
    } else {
        *time = ((ctx->maxFileSize - ctx->streamSize) / packetLen) * INTERVAL_RECORD_FRAME;
    }
    MEDIA_HAL_LOGI(MODULE_NAME, "remaind file packetLen = %u, time:%u", packetLen, *time);
    MediaHalMutexUnLock(ctx->mutex);
}

static int32_t GetCurrentFileInfo(WavMuxerContext *ctx, char **value)
{
    CHK_NULL_RETURN(ctx, MEDIA_HAL_INVALID_PARAM, "handle is null");
    CHK_NULL_RETURN(value, MEDIA_HAL_INVALID_PARAM, "value is null");
    CHK_NULL_RETURN(*value, MEDIA_HAL_INVALID_PARAM, "real value is null");
    if (!ctx->isSupportSplit) {
        MEDIA_HAL_LOGE(MODULE_NAME, "not support get file info!");
        return MEDIA_HAL_ERR;
    }
    MediaHalMutexLock(ctx->mutex);
    FileDate fileDate = {};
    int32_t ret = DtcfMgrGetCurrentFileDate(&fileDate);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "Get Current File Date failed:%d", ret);
        MediaHalMutexUnLock(ctx->mutex);
        return ret;
    }
    char *lastSlash = strrchr(ctx->fileName, '/');
    if (lastSlash == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "Get Current File name failed!");
        MediaHalMutexUnLock(ctx->mutex);
        return MEDIA_HAL_ERR;
    }
    ret = sprintf_s(*value, FILE_INFO_BUFFER_SIZE, "%llu,%u,%u,%u,%u,%u,%u,%u=%s", ctx->streamSize,
        fileDate.fileDateYear, fileDate.fileDateMonth, fileDate.fileDateDay, fileDate.fileDateHour,
        fileDate.fileDateMin, fileDate.fileDateSec, ctx->realRecTimeLenMs, lastSlash + 1);
    if (ret == -1 || ret >= FILE_INFO_BUFFER_SIZE) {
        MEDIA_HAL_LOGE(MODULE_NAME, "sprintf_s failed!");
        MediaHalMutexUnLock(ctx->mutex);
        return MEDIA_HAL_ERR;
    }
    MediaHalMutexUnLock(ctx->mutex);
    return MEDIA_HAL_OK;
}

static int32_t WavMuxerInvokeRecordMarks(WavMuxerContext *ctx)
{
    CHK_NULL_RETURN(ctx->fd, MEDIA_HAL_INVALID_PARAM, "ctx->fd is NULL!");
    uint64_t markTime = 0;
    GetCurrentTime(&markTime);
    float marks = (markTime - ctx->startTime) / REC_US_TO_S;
    uint32_t sampleRate = WavGetSampleRate(ctx->fd);
    uint32_t channel = WavGetNumChannels(ctx->fd);
    uint32_t sampleNum = marks * sampleRate * channel;
    return WavRecordMarks(ctx->fd, sampleNum);
}

static int32_t WavMuxerInvoke(RecMuxerHandle handle, uint32_t invokeId, void *arg)
{
    int32_t ret = MEDIA_HAL_OK;
    WavMuxerContext *ctx = (WavMuxerContext *)handle;
    CHK_NULL_RETURN(ctx, MEDIA_HAL_INVALID_PARAM, "muxerContext is NULL!");
    CHK_NULL_RETURN(arg, MEDIA_HAL_INVALID_PARAM, "arg is NULL!");
    MEDIA_HAL_LOGI(MODULE_NAME, "invokeId:%d ", invokeId);
    switch (invokeId) {
        case INVOKE_SET_CALLBACK:
            ret = MuxerSetCallBack(ctx, (const PluginCallback *)arg);
            break;
        case INVOKE_SET_DELETE_OLD_FILES_ENABLE: {
            uint32_t value = *((uint32_t *)arg);
            bool enable = false;
            if (value != 0) {
                enable = true;
            }
            if (!ctx->isSupportSplit) {
                enable = false;
            }
            DtcfMgrSetDeleteFileEnable(enable);
            break;
        }
        case INVOKE_SET_MAX_STORAGE_THRESHOLD: {
            uint32_t value = *((uint32_t *)arg);
            DtcfMgrSetMaxStorageThreshold(value);
            break;
        }
        case INVOKE_RECORDE_WRITE_MARKS: {
            ret = WavMuxerInvokeRecordMarks(ctx);
            break;
        }
        case INVOKE_CURRENT_STORAGE_REMAIND_TIME: {
            uint32_t packetLen = *((uint32_t *)arg);
            GetCurrentStorageTimeRemaindTime(ctx, packetLen, (uint32_t *)arg);
            break;
        }
        case INVOKE_CURRENT_FILE_REMAIND_TIME: {
            uint32_t packetLen = *((uint32_t *)arg);
            GetCurrentFileRemaindTime(ctx, packetLen, (uint32_t *)arg);
            break;
        }
        case INVOKE_CURRENT_FILE_INFO: {
            ret = GetCurrentFileInfo(ctx, (char **)arg);
            break;
        }
        default:
            MEDIA_HAL_LOGI(MODULE_NAME, "not support invokeId:%d ", invokeId);
            break;
    }
    return ret;
}

MuxerPluginEntry g_wavMuxerEntry = {
    .desc = {
        .libName = "libplugin_muxer_wav.so",
        .name = "wav_muxer",
        .version = "version 1.0.0",
        .description = "wav_muxer",
        .supportCapability = "wav",
        .priority = WAV_MUXER_PRIORITY,
    },
    .fmt_find = WavMuxerFind,
    .fmt_open = WavMuxerOpen,
    .fmt_add_track = WavMuxerAddTrack,
    .fmt_start = WavMuxerStart,
    .fmt_write = WavMuxerWriteFrame,
    .fmt_stop = WavMuxerStop,
    .fmt_set_max_size = WavMuxerSetMaxFileSize,
    .fmt_set_max_duration = WavMuxerSetMaxFileDuration,
    .fmt_set_next_outputfile = WavMuxerSetNextOutputFile,
    .fmt_invoke = WavMuxerInvoke,
    .fmt_close = WavMuxerDestroy,
};

#ifdef ENABLE_DL_DEMUXER_PLUGIN
const MuxerPluginEntry *GetMuxer(void)
{
    return &g_wavMuxerEntry;
}
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */
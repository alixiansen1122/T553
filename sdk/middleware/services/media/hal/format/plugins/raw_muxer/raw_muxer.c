/*
* Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
* Description: wav_muxer.c
* Author: Media Software Group
* Create: 2025-03-10
*/

#include "raw_muxer.h"
#include <malloc.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/prctl.h>
#include <unistd.h>
#include <limits.h>
#include <fcntl.h>
#include "muxer_interface.h"
#include "dtcf_manager.h"
#include "media_hal_common.h"
#include "securec.h"

#define MODULE_NAME "RawMuxer"

#define BYTE_UNIT 1024
#define RAW_MUXER_PRIORITY 100
#ifdef SUPPORT_FEATURE_CONFIG
#define INTERVAL_RECORD_FRAME (1000 / CONFIG_AUDIO_AI_FRAME_PER_SEC)
#else
#define INTERVAL_RECORD_FRAME 10
#endif
#define MSEC_PER_SEC (1000)
#define REC_SPLIT_TIME_THRESHOLD (0.9)
#define REC_SPLIT_SIZE_THRESHOLD (0.9)
#define REC_US_TO_S (1000000.0)

#define SIZE_PER_SAMPLE 2
#define FILE_MODEL 0644

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
    int32_t fp;
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
    bool isSupportSplit;
} RawMuxerContext;

typedef struct {
    int32_t dataLength;
    int64_t frameTimeStamp;
} RawFrameData;

static DTCF_FILE_TYPE_E g_fileType = DTCF_FILE_TYPE_BUTT;
static DTCF_DIR_E g_dirType = DTCF_DIR_BUTT;

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
    RawMuxerContext *context = (RawMuxerContext *)callBack;
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

static int32_t RawMuxerFind(const MuxerOutput *output, const ProtocolFun *protocol)
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
    if (output->format == FORMAT_TYPE_MP3) {
        g_fileType = DTCF_FILE_TYPE_MP3;
        g_dirType = DTCF_DIR_RECORD_MP3_FILE;
    } else if (output->format == FORMAT_TYPE_SILK) {
        g_fileType = DTCF_FILE_TYPE_SILK;
        g_dirType = DTCF_DIR_RECORD_SILK_FILE;
    } else {
        MEDIA_HAL_LOGI(MODULE_NAME, "rawMuxer not Found format = %d", output->format);
        return MEDIA_HAL_ERR;
    }
    MEDIA_HAL_LOGI(MODULE_NAME, "rawMuxer Found");
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

static int32_t InitRawMuxer(RawMuxerContext *ctx)
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
            ctx->fp = open(ctx->muxerOutput.url, O_RDWR | O_CREAT | O_TRUNC, FILE_MODEL);
            if (ctx->fp < 0) {
                MEDIA_HAL_LOGE(MODULE_NAME, "open file error, errno = %d", errno);
                return MEDIA_HAL_ERR;
            }
            if (memcpy_s(ctx->fileName, REC_FILE_NAME_LEN, ctx->muxerOutput.url, REC_FILE_NAME_LEN) != EOK) {
                MEDIA_HAL_LOGE(MODULE_NAME, "memcpy_s failed");
                close(ctx->fp);
                ctx->fp = -1;
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
        char fileNames[0x2][REC_FILE_NAME_LEN];
        ret = DtcfMgrRequestFileNames(g_fileType, g_dirType, fileNames);
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "DtcfMgrRequestFileNames error");
            return MEDIA_HAL_ERR;
        }
        ctx->fp = open(fileNames[0], O_RDWR | O_CREAT | O_TRUNC, 0);
        if (ctx->fp < 0) {
            MEDIA_HAL_LOGE(MODULE_NAME, "open file error, errno = %d", errno);
            return MEDIA_HAL_ERR;
        }
        if (memcpy_s(ctx->fileName, REC_FILE_NAME_LEN, fileNames[0], REC_FILE_NAME_LEN) != EOK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "memcpy_s failed");
            return MEDIA_HAL_ERR;
        }
    }
    return MEDIA_HAL_OK;
}

static int32_t RawMuxerOpen(const MuxerOutput *output, const ProtocolFun *protocol, RecMuxerHandle *handle)
{
    MEDIA_HAL_UNUSED(protocol);
    CHK_NULL_RETURN(output, MEDIA_HAL_INVALID_PARAM, "output is null");
    CHK_NULL_RETURN(handle, MEDIA_HAL_INVALID_PARAM, "handle is null");
    
    RawMuxerContext *ctx = (RawMuxerContext*)malloc(sizeof(RawMuxerContext));
    CHK_NULL_RETURN(ctx, MEDIA_HAL_NO_MEM, "RawMuxerContext is null");
    int32_t ret = MEDIA_HAL_OK;
    if (memset_s(ctx, sizeof(RawMuxerContext), 0x0, sizeof(RawMuxerContext)) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "memset_s RawMuxerContext error");
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
    ret = InitRawMuxer(ctx);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "CreateRawMuxer failed:0x%x", ret);
        goto MUXER_DESTORY;
    }
    *handle = ctx;
    if (ctx->isSupportSplit) {
        ret = DtcfMgrGetStorageMaxSize(&ctx->maxMemorySize);
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "DtcfGetStorageMaxSize failed!");
        }
    }
    MEDIA_HAL_LOGI(MODULE_NAME, "RawMuxerOpen :%d", ret);
    return MEDIA_HAL_OK;
MUXER_DESTORY:
    MediaHalMutexDestroy(&ctx->mutex);
CREATE_FAILED:
    free(ctx);
    return MEDIA_HAL_ERR;
}

static int32_t DeinitRawMuxer(RawMuxerContext *ctx)
{
    CHK_NULL_RETURN(ctx, MEDIA_HAL_INVALID_PARAM, "handle");
    close(ctx->fp);
    ctx->fp = -1;
    ctx->muxerStarted = false;
    ctx->isSupportSplit = false;
    MEDIA_HAL_LOGI(MODULE_NAME, "DeinitRawMuxer success");
    return MEDIA_HAL_OK;
}

static int32_t RawMuxerDestroy(RecMuxerHandle handle)
{
    RawMuxerContext *ctx = (RawMuxerContext *)handle;
    CHK_NULL_RETURN(ctx, MEDIA_HAL_INVALID_PARAM, "handle");
    int32_t ret = DeinitRawMuxer(ctx);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "call DeinitRawMuxer fail:0x%x", ret);
    }
    if (ctx->isSupportSplit) {
        (void)DtcfMgrFileOptDeInit();
    }
    MediaHalMutexDestroy(&ctx->mutex);
    free(ctx);
    MEDIA_HAL_LOGI(MODULE_NAME, "RawMuxerDestroy success");
    return MEDIA_HAL_OK;
}

static int32_t RawMuxerAddTrack(RecMuxerHandle handle, const MuxerTrackSource *trackSource)
{
    CHK_NULL_RETURN(trackSource, MEDIA_HAL_INVALID_PARAM, "trackSource is null");
    RawMuxerContext *ctx = (RawMuxerContext *)handle;
    CHK_NULL_RETURN(ctx, MEDIA_HAL_INVALID_PARAM, "RawMuxerContext is null");
    if (trackSource->type != STREAM_TYPE_AUDIO) {
        MEDIA_HAL_LOGE(MODULE_NAME, "only support STREAM_TYPE_AUDIO!");
        return MEDIA_HAL_ERR;
    }
    ctx->trackSource = *trackSource;
    return MEDIA_HAL_OK;
}

static int32_t RawMuxerStart(RecMuxerHandle handle)
{
    RawMuxerContext *ctx = (RawMuxerContext *)handle;
    CHK_NULL_RETURN(ctx, MEDIA_HAL_INVALID_PARAM, "ctx is null");
    CHK_COND_RETURN(ctx->muxerStarted, MEDIA_HAL_OK, "ctx is Started");
    ctx->muxerStarted = true;
    ctx->splitSizeBegin = !ctx->isSupportSplit;
    ctx->splitTimeBegin = !ctx->isSupportSplit;
    MEDIA_HAL_LOGI(MODULE_NAME, "RawMuxerStart success ");
    return MEDIA_HAL_OK;
}

static bool SplitIsAchiveSize(RawMuxerContext *ctx)
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

static bool SplitIsAchiveTime(RawMuxerContext *ctx)
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

static int32_t ReConfigMuxer(RawMuxerContext *ctx)
{
    int32_t ret = DeinitRawMuxer(ctx);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "call DeinitRawMuxer fail:0x%x", ret);
        return MEDIA_HAL_ERR;
    }
    ret = InitRawMuxer(ctx);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "InitRawMuxer failed:0x%x", ret);
        return MEDIA_HAL_ERR;
    }
    ctx->muxerStarted = true;
    MEDIA_HAL_LOGE(MODULE_NAME, "ReConfigMuxer success");
    return MEDIA_HAL_OK;
}

static int32_t RawMuxerWriteFrameInner(RawMuxerContext *ctx, const FormatPacket *frameData,
    const RawFrameData *wavFrameData)
{
    if (write(ctx->fp, frameData->data, frameData->len) != frameData->len) {
        MEDIA_HAL_LOGE(MODULE_NAME, "fwrite failed errno:%d", errno);
        return MEDIA_HAL_ERR_AGAIN;
    }
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

static int32_t HandleRawFrame(RawMuxerContext *ctx, RawFrameData *wavFrameData, const FormatPacket *frameData)
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
        if (ctx->maxDuration < 1 || ctx->maxFileSize < frameSize) {
            MEDIA_HAL_LOGE(MODULE_NAME, "file duration or size is not right, write file failed!");
            return MEDIA_HAL_ERR;
        }
    }
    return MEDIA_HAL_OK;
}

static int32_t RawMuxerWriteFrame(RecMuxerHandle handle, const FormatPacket *frameData)
{
    CHK_NULL_RETURN(frameData, MEDIA_HAL_INVALID_PARAM, "frameData is NULL!");
    RawMuxerContext *ctx = (RawMuxerContext *)handle;
    CHK_NULL_RETURN(ctx, MEDIA_HAL_INVALID_PARAM, "RawMuxerContext is null");
    CHK_COND_RETURN(!ctx->muxerStarted, MEDIA_HAL_ERR, "RawMuxerContext is not Started");
    if (ctx->isSupportSplit) {
        if (DtcfMgrHandleRecordMaxSize(frameData->len, ctx->maxMemorySize) != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "handle reac max size failed!");
            return MEDIA_HAL_ERR;
        }
    }
    RawFrameData wavFrameData = {};
    if (HandleRawFrame(ctx, &wavFrameData, frameData) != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "handle wav frame failed!");
        return MEDIA_HAL_ERR;
    }
    MediaHalMutexLock(ctx->mutex);
    if (ctx->isSupportSplit) {
        ctx->streamSize += wavFrameData.dataLength;
        ctx->realRecTimeLenMs = (uint32_t)((wavFrameData.frameTimeStamp - ctx->baseFramePts) / MSEC_PER_SEC);
        if (SplitIsAchiveSize(ctx) || SplitIsAchiveTime(ctx)) {
            int32_t ret = ReConfigMuxer(ctx);
            if (ret != MEDIA_HAL_OK) {
                ReportError(ctx->callBack, ERROR_TYPE_INTERNAL_OPERATION_FAIL, ERROR_TYPE_INTERNAL_OPERATION_FAIL);
                MediaHalMutexUnLock(ctx->mutex);
                return MEDIA_HAL_ERR;
            }
        }
    }
    int32_t ret = RawMuxerWriteFrameInner(ctx, frameData, &wavFrameData);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "RawMuxerWriteFrameInner failed!");
        MediaHalMutexUnLock(ctx->mutex);
        return ret;
    }
    MediaHalMutexUnLock(ctx->mutex);
    return MEDIA_HAL_OK;
}

static int32_t RawMuxerStop(RecMuxerHandle handle, bool block)
{
    CHK_NULL_RETURN(handle, MEDIA_HAL_INVALID_PARAM, "handle");
    RawMuxerContext *ctx = (RawMuxerContext *)handle;
    CHK_NULL_RETURN(ctx, MEDIA_HAL_INVALID_PARAM, "RawMuxerContext");
    CHK_COND_RETURN(!ctx->muxerStarted, MEDIA_HAL_ERR, "RawMuxerContext is not Started");
    MEDIA_HAL_LOGI(MODULE_NAME, " block %d", block);
    ctx->muxerStarted = false;
    return MEDIA_HAL_OK;
}

static int32_t RawMuxerSetMaxFileSize(RecMuxerHandle handle, int64_t bytes)
{
    CHK_NULL_RETURN(handle, MEDIA_HAL_INVALID_PARAM, "handle");
    RawMuxerContext *ctx = (RawMuxerContext *)handle;
    MEDIA_HAL_LOGI(MODULE_NAME, "bytes:%lld ", bytes);
    ctx->maxFileSize = bytes;
    return MEDIA_HAL_OK;
}

static int32_t RawMuxerSetMaxFileDuration(RecMuxerHandle handle, int64_t durationUs)
{
    CHK_NULL_RETURN(handle, MEDIA_HAL_INVALID_PARAM, "handle");
    RawMuxerContext *ctx = (RawMuxerContext *)handle;
    MEDIA_HAL_LOGI(MODULE_NAME, "durationUs:%lld ", durationUs);
    ctx->maxDuration = durationUs;
    return MEDIA_HAL_OK;
}

static int32_t RawMuxerSetNextOutputFile(RecMuxerHandle handle, int32_t fd)
{
    RawMuxerContext *ctx = (RawMuxerContext *)handle;
    CHK_NULL_RETURN(ctx, MEDIA_HAL_INVALID_PARAM, "handle is null");
    if (ctx->muxerOutput.type == OUT_TYPE_URI) {
        return MEDIA_HAL_ERR;
    }
    ctx->muxerOutput.fd = fd;
    return MEDIA_HAL_OK;
}

static int32_t MuxerSetCallBack(RawMuxerContext *ctx, const PluginCallback *callBack)
{
    CHK_NULL_RETURN(callBack, MEDIA_HAL_INVALID_PARAM, "callBack");
    ctx->callBack = *callBack;
    return MEDIA_HAL_OK;
}

static void GetCurrentStorageTimeRemaindTime(RawMuxerContext *ctx, uint32_t packetLen, uint32_t *time)
{
    if (!ctx->isSupportSplit) {
        MEDIA_HAL_LOGI(MODULE_NAME, "not support get recorder time!");
        return;
    }
    MediaHalMutexLock(ctx->mutex);
    *time = ((DtcfMgrGetCurrentMemorySize(ctx->maxMemorySize) - DtcfMgrGetCurrentRecordedSize()) / packetLen) *
        INTERVAL_RECORD_FRAME;
    MEDIA_HAL_LOGI(MODULE_NAME, "remaind storage packetLen = %u, time:%u\n", packetLen, *time);
    MediaHalMutexUnLock(ctx->mutex);
}

static void GetCurrentFileRemaindTime(RawMuxerContext *ctx, uint32_t packetLen, uint32_t *time)
{
    if (!ctx->isSupportSplit) {
        MEDIA_HAL_LOGI(MODULE_NAME, "not support get file time!");
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

static int32_t GetCurrentRawFileInfo(RawMuxerContext *ctx, char **value)
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

static int32_t RawMuxerInvoke(RecMuxerHandle handle, uint32_t invokeId, void *arg)
{
    int32_t ret = MEDIA_HAL_OK;
    RawMuxerContext *ctx = (RawMuxerContext *)handle;
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
            ret = GetCurrentRawFileInfo(ctx, (char **)arg);
            break;
        }
        default:
            MEDIA_HAL_LOGI(MODULE_NAME, "not support invokeId:%d ", invokeId);
            break;
    }
    return ret;
}

MuxerPluginEntry g_rawMuxerEntry = {
    .desc = {
        .libName = "libplugin_muxer_raw.so",
        .name = "raw_muxer",
        .version = "version 1.0.0",
        .description = "raw_muxer",
        .supportCapability = "raw",
        .priority = RAW_MUXER_PRIORITY,
    },
    .fmt_find = RawMuxerFind,
    .fmt_open = RawMuxerOpen,
    .fmt_add_track = RawMuxerAddTrack,
    .fmt_start = RawMuxerStart,
    .fmt_write = RawMuxerWriteFrame,
    .fmt_stop = RawMuxerStop,
    .fmt_set_max_size = RawMuxerSetMaxFileSize,
    .fmt_set_max_duration = RawMuxerSetMaxFileDuration,
    .fmt_set_next_outputfile = RawMuxerSetNextOutputFile,
    .fmt_invoke = RawMuxerInvoke,
    .fmt_close = RawMuxerDestroy,
};

#ifdef ENABLE_DL_DEMUXER_PLUGIN
const MuxerPluginEntry *GetMuxer(void)
{
    return &g_rawMuxerEntry;
}
#endif
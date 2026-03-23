/*
 * Copyright (c) @CompanyNameMagicTag. 2025-2025. All rights reserved.
 * Description: buffer cache for online play
 * Author: Media Software Group
 * Create: 2025-6-20
 */

#include "demuxer_buffer.h"

#include <stdint.h>
#include <inttypes.h>
#include "securec.h"
#include "demuxer_interface.h"
#include "format_type.h"
#include "format_common.h"
#include "hal_list.h"
#include "media_hal_thread_adapt.h"
#include "media_hal_common.h"
#include "demuxer_common.h"

#define MODULE_NAME "DemuxerBuffer"
#define DEFAULT_MAX_BUFFER_SIZE (2 * 1024 * 1024) // 2MB
#define DEFAULT_MAX_BUFFER_DURATION (10 * 1000) // 10s
#define BUFFER_HIGH_LEVEL (0.7) // 70%

typedef enum {
    STOPPED,            // thread stopped
    NORMAL_READING,     // thread is working to read frame
    BUFFER_FULL_SLEEP,  // thread is sleeping because buffer is full
    GET_EOF_SLEEP,      // thread is sleeping because received eof flag
} BufferThreadStatus;

typedef enum {
    BUFFER_FULL_NONE,
    BUFFER_FULL_DURATION,
    BUFFER_FULL_SIZE,
} BufferFullType;

typedef struct {
    int32_t streamIndex;
    int64_t timeStampUs;
    FormatSeekMode mode;
    bool shoudDoSeek;
    int32_t doSeekResult;
    MediaHalThreadCondHandle seekCond;
} SeekContext;

typedef struct {
    bool running;
    int32_t fmtReadResult;
    BufferThreadStatus status;
    FormatBufferSetting bufferSetting;
    BufferFullType bufferFullType;
    MediaHalMutexHandle mutex;
    MediaHalThreadCondHandle cond;
    MediaHalThreadIdHandle threadId;
    const DemuxerPluginEntry *demuxer;
    FormatHandle demuxerHdl;
    List bufferQueue;
    int64_t totalTime;
    int64_t totalSize;
    SeekContext seekCtx;
} BufferManager;

static void InitSeekContext(SeekContext *seekCtx)
{
    seekCtx->streamIndex = -1;
    seekCtx->timeStampUs = -1;
    seekCtx->mode = SEEK_MODE_BUT;
    seekCtx->shoudDoSeek = false;
    seekCtx->doSeekResult = -1;
    seekCtx->seekCond = MediaHalThreadCondCreate();
    return;
}

FormatHandle CreateDemuxerBuffer(void)
{
    BufferManager *ctx = (BufferManager*)malloc(sizeof(BufferManager));
    CHK_NULL_RETURN(ctx, NULL, "malloc BufferManager failed");
    ctx->running = false;
    ctx->fmtReadResult = 0;
    ctx->status = STOPPED;
    ctx->bufferSetting.maxDurationMs = DEFAULT_MAX_BUFFER_DURATION;
    ctx->bufferSetting.maxSize = DEFAULT_MAX_BUFFER_SIZE;
    ctx->bufferFullType = BUFFER_FULL_NONE;
    MediaHalMutexAttr attr = { false };
    ctx->mutex = MediaHalMutexCreate(&attr);
    ctx->cond = MediaHalThreadCondCreate();
    ctx->threadId = NULL;
    ctx->demuxer = NULL;
    ctx->demuxerHdl = NULL;
    HalListInit(&(ctx->bufferQueue), NULL);
    ctx->totalTime = 0;
    ctx->totalSize = 0;
    InitSeekContext(&(ctx->seekCtx));
    return (FormatHandle)ctx;
}

int32_t SetBufferConfig(FormatHandle handle, const FormatBufferSetting *setting)
{
    CHK_NULL_RETURN(handle, -1, "input param handle null");
    CHK_NULL_RETURN(setting, -1, "input param setting null");
    BufferManager *ctx = (BufferManager*)handle;
    ctx->bufferSetting = *setting;
    return 0;
}

int32_t GetBufferConfig(FormatHandle handle, FormatBufferSetting *setting)
{
    CHK_NULL_RETURN(handle, -1, "input param handle null");
    CHK_NULL_RETURN(setting, -1, "input param setting null");
    BufferManager *ctx = (BufferManager*)handle;
    *setting = ctx->bufferSetting;
    return 0;
}

static FormatFrame *PacketToFrame(const FormatPacket *packet)
{
    if (packet == NULL || packet->packetType > PACKET_TYPE_BUT) {
        return NULL;
    }

    FormatFrame *frame = (FormatFrame*)malloc(sizeof(FormatFrame));
    CHK_NULL_RETURN(frame, NULL, "malloc FormatFrame failed");
    FormatPacketToFrame(packet, frame);

    frame->data = (uint8_t*)malloc(packet->len);
    if (frame->data == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "malloc frame data failed");
        goto FREE_FRAME;
    }
    if (memcpy_s(frame->data, packet->len, packet->data, packet->len) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "memcpy_s packet data failed");
        goto FREE_DATA;
    }

    if (frame->itemCnt > 0) {
        size_t itemLen = frame->itemCnt * sizeof(ParameterItem);
        frame->item = (ParameterItem*)malloc(itemLen);
        if (frame->item == NULL) {
            MEDIA_HAL_LOGE(MODULE_NAME, "malloc frame item failed");
            goto FREE_DATA;
        }
        if (memcpy_s(frame->item, itemLen, packet->item, itemLen) != EOK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "memcpy_s frame item failed");
            goto FREE_ITEM;
        }
    }

    return frame;

FREE_ITEM:
    free(frame->item);
    frame->item = NULL;
FREE_DATA:
    free(frame->data);
    frame->data = NULL;
FREE_FRAME:
    free(frame);
    frame = NULL;

    return NULL;
}

static void ClearBuffer(BufferManager *ctx)
{
    FormatFrame *frame = NULL;
    while (!HalListEmpty(&(ctx->bufferQueue))) {
        frame = (FormatFrame*)HalListFront(&(ctx->bufferQueue));
        HalListPopFront(&(ctx->bufferQueue));
        if (frame != NULL) {
            FreeBufferFrame(frame);
            free(frame);
            frame = NULL;
        }
    }
    ctx->totalSize = 0;
    ctx->totalTime = 0;
    ctx->bufferFullType = BUFFER_FULL_NONE;
    return;
}

static int32_t DoSeek(BufferManager *ctx)
{
    ClearBuffer(ctx);
    int32_t ret = ctx->demuxer->fmt_seek_pts(ctx->demuxerHdl, ctx->seekCtx.streamIndex,
        ctx->seekCtx.timeStampUs, ctx->seekCtx.mode);
    ctx->seekCtx.shoudDoSeek = false;
    ctx->seekCtx.doSeekResult = ret;
    MediaHalThreadCondSignal(ctx->seekCtx.seekCond);
    return ret;
}

static bool IsBufferFull(BufferManager *ctx)
{
    int64_t allowMaxSize = ctx->bufferSetting.maxSize;
    if (allowMaxSize > 0 && ctx->totalSize >= allowMaxSize) {
        ctx->bufferFullType = BUFFER_FULL_SIZE;
        return true;
    }

    int64_t allowMaxDuration = ctx->bufferSetting.maxDurationMs;
    if (allowMaxDuration > 0 && ctx->totalTime >= allowMaxDuration) {
        ctx->bufferFullType = BUFFER_FULL_DURATION;
        return true;
    }

    return false;
}

static bool IsBufferNotEnough(BufferManager *ctx)
{
    int64_t allowMaxSize = ctx->bufferSetting.maxSize;
    if (allowMaxSize > 0 && ctx->bufferFullType == BUFFER_FULL_SIZE &&
        ctx->totalSize <= (int64_t)(allowMaxSize * BUFFER_HIGH_LEVEL)) {
        return true;
    }

    int64_t allowMaxTime = ctx->bufferSetting.maxDurationMs;
    if (allowMaxTime > 0 && ctx->bufferFullType == BUFFER_FULL_DURATION &&
        ctx->totalTime <= (int64_t)(allowMaxTime * BUFFER_HIGH_LEVEL)) {
        return true;
    }

    return false;
}

static void DealThreadStatus(BufferManager *ctx)
{
    if (ctx->fmtReadResult == RET_FILE_EOF) {
        ctx->status = GET_EOF_SLEEP;
        MEDIA_HAL_LOGE(MODULE_NAME, "buffer thread get eof flag");
    } else if (IsBufferFull(ctx)) {
        ctx->status = BUFFER_FULL_SLEEP;
    } else {
        ctx->status = NORMAL_READING;
    }
    return;
}

static int32_t ReadDemuxerPacket(BufferManager *ctx, FormatPacket *packet)
{
    int32_t ret = ctx->demuxer->fmt_read(ctx->demuxerHdl, packet);
    if ((ret == MEDIA_HAL_OK) && (packet->data != NULL) && (packet->len == 0)) {
        MEDIA_HAL_LOGE(MODULE_NAME, "fmt_read frame zero len");
        ret = MEDIA_HAL_ERR;
    }
    return ret;
}

static int32_t FreeDemuxerPacket(BufferManager *ctx, FormatPacket *packet)
{
    int32_t ret = ctx->demuxer->fmt_free(ctx->demuxerHdl, packet);
    packet->data = NULL;
    packet->len = 0;
    packet->item = NULL;
    packet->itemCnt = 0;
    return ret;
}

static void PushFrame(BufferManager *ctx, const FormatPacket *packet)
{
    FormatFrame *frame = PacketToFrame(packet);
    if (frame == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "PacketToFrame error");
        return;
    }
    HalListPushBack(&(ctx->bufferQueue), (uintptr_t)frame);
    ctx->totalSize += frame->len;
    ctx->totalTime += frame->durationUs;
    return;
}

static FormatFrame *PopFrame(BufferManager *ctx)
{
    FormatFrame *frontFrame = (FormatFrame*)HalListFront(&(ctx->bufferQueue));
    HalListPopFront(&(ctx->bufferQueue));
    if (frontFrame != NULL) {
        ctx->totalSize -= frontFrame->len;
        ctx->totalTime -= frontFrame->durationUs;
    }
    return frontFrame;
}

static void *BufferThread(void *args)
{
    BufferManager *ctx = (BufferManager*)args;
    FormatPacket packet;
    int32_t ret;
    ctx->status = NORMAL_READING;
    while (true) {
        MediaHalMutexLock(ctx->mutex);
        if (!ctx->running) {
            MediaHalMutexUnLock(ctx->mutex);
            break;
        }
        if (ctx->seekCtx.shoudDoSeek) {
            DoSeek(ctx);
            ctx->fmtReadResult = 0;
        }
        DealThreadStatus(ctx);
        if (ctx->status != NORMAL_READING) {
            MediaHalThreadCondWait(ctx->cond, ctx->mutex);
            MediaHalMutexUnLock(ctx->mutex);
            continue;
        }
        MediaHalMutexUnLock(ctx->mutex);

        ret = ReadDemuxerPacket(ctx, &packet);

        MediaHalMutexLock(ctx->mutex);
        ctx->fmtReadResult = ret;
        if (ctx->fmtReadResult != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "fmt_read error");
            MediaHalMutexUnLock(ctx->mutex);
            continue;
        }
        PushFrame(ctx, &packet);
        MediaHalMutexUnLock(ctx->mutex);

        FreeDemuxerPacket(ctx, &packet);
    }

    ClearBuffer(ctx);
    ctx->status = STOPPED;
    return NULL;
}

int32_t StartBufferThread(FormatHandle handle, FormatHandle demuxerHdl,
    const DemuxerPluginEntry *demuxer, const char *url)
{
    CHK_NULL_RETURN(handle, -1, "input param handle null");
    CHK_NULL_RETURN(demuxerHdl, -1, "input param demuxerHdl null");
    CHK_NULL_RETURN(demuxer, -1, "input param demuxer null");

    BufferManager *ctx = (BufferManager*)handle;
    ctx->demuxerHdl = demuxerHdl;
    ctx->demuxer = demuxer;
    MediaHalThreadattr attr = { "BufferThread", 0x1000, THREAD_SCHED_INVALID, 0, false };
    if (HasM3U8(url)) {
        attr.stackSize = 0xDF00;
    }
    ctx->running = true;
    ctx->threadId = MediaHalThreadCreate(BufferThread, handle, &attr);
    if (ctx->threadId == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "crate BufferManager thread failed");
        ctx->running = false;
        return -1;
    }
    return 0;
}

int32_t ReadBufferFrame(FormatHandle handle, FormatFrame *frame)
{
    CHK_NULL_RETURN(handle, -1, "input param handle null");
    CHK_NULL_RETURN(frame, -1, "input param frame null");

    int32_t ret = 0;
    BufferManager *ctx = (BufferManager*)handle;
    MediaHalMutexLock(ctx->mutex);
    if (HalListEmpty(&(ctx->bufferQueue))) {
        if (ctx->status == GET_EOF_SLEEP) {
            MEDIA_HAL_LOGE(MODULE_NAME, "return eof when ReadBufferFrame");
            MediaHalMutexUnLock(ctx->mutex);
            return RET_FILE_EOF;
        }
        if (ctx->fmtReadResult == MEDIA_HAL_ERR) {
            MEDIA_HAL_LOGE(MODULE_NAME, "return error when ReadBufferFrame");
            MediaHalMutexUnLock(ctx->mutex);
            return MEDIA_HAL_ERR;
        }
        MEDIA_HAL_LOGE(MODULE_NAME, "return nodata when ReadBufferFrame");
        MediaHalMutexUnLock(ctx->mutex);
        return RET_NODATA;
    }

    FormatFrame *frontFrame = PopFrame(ctx);
    if (frontFrame == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "get frame from queue falied");
        MediaHalMutexUnLock(ctx->mutex);
        return -1;
    }

    if (ctx->status == BUFFER_FULL_SLEEP && IsBufferNotEnough(ctx)) {
        MediaHalThreadCondSignal(ctx->cond);
    }
    if (memcpy_s(frame, sizeof(FormatFrame), frontFrame, sizeof(FormatFrame)) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "memcpy_s frame failed");
        ret = -1;
    }
    free(frontFrame);
    frontFrame = NULL;
    MediaHalMutexUnLock(ctx->mutex);
    return ret;
}

int32_t FreeBufferFrame(FormatFrame *frame)
{
    CHK_NULL_RETURN(frame, -1, "input param frame null");
    if (frame->data != NULL) {
        free(frame->data);
        frame->data = NULL;
        frame->len = 0;
    }
    if (frame->item != NULL) {
        free(frame->item);
        frame->item = NULL;
        frame->itemCnt = 0;
    }
    return 0;
}

int32_t SeekBuffer(FormatHandle handle, int32_t streamIndex, int64_t timeStampUs, FormatSeekMode mode)
{
    CHK_NULL_RETURN(handle, -1, "input param handle null");
    BufferManager *ctx = (BufferManager*)handle;
    MediaHalMutexLock(ctx->mutex);
    ctx->seekCtx.streamIndex = streamIndex;
    ctx->seekCtx.timeStampUs = timeStampUs;
    ctx->seekCtx.mode = mode;
    ctx->seekCtx.shoudDoSeek = true;
    ctx->seekCtx.doSeekResult = -1;
    MediaHalThreadCondSignal(ctx->cond);
    while (ctx->seekCtx.shoudDoSeek && ctx->running) {
        MediaHalThreadCondWait(ctx->seekCtx.seekCond, ctx->mutex);
    }
    MediaHalMutexUnLock(ctx->mutex);
    return ctx->seekCtx.doSeekResult;
}

int32_t StopBufferThread(FormatHandle handle)
{
    CHK_NULL_RETURN(handle, -1, "input param handle null");
    BufferManager *ctx = (BufferManager*)handle;
    MediaHalMutexLock(ctx->mutex);
    ctx->running = false;
    MediaHalThreadCondSignal(ctx->cond);
    MediaHalThreadCondSignal(ctx->seekCtx.seekCond);
    MediaHalMutexUnLock(ctx->mutex);
    MediaHalThreadJoin(&(ctx->threadId));
    return 0;
}

int32_t DestoryDemuxerBuffer(FormatHandle *handle)
{
    CHK_NULL_RETURN(handle, -1, "input param handle null");
    CHK_NULL_RETURN(*handle, -1, "input param *handle null");
    BufferManager *ctx = (BufferManager*)(*handle);
    if (ctx->running) {
        StopBufferThread(*handle);
    }
    MediaHalMutexDestroy(&(ctx->mutex));
    MediaHalThreadCondDestroy(&(ctx->cond));
    MediaHalThreadCondDestroy(&(ctx->seekCtx.seekCond));
    HalListDeinit(&(ctx->bufferQueue));
    free(ctx);
    *handle = NULL;
    return 0;
}

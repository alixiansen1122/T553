/*
 * Copyright (c) CompanyNameMagicTag. 2022-2022. All rights reserved.
 * Description: jpeg vdec plugin
 * Author: Media Software Group
 * Create: 2022-09-20
 */

#include "native_vdec_jpeg.h"
#include "media_hal_common.h"
#include "media_hal_thread_adapt.h"
#include "limits.h"

#include "vdec_common.h"

#include "soc_jpeg_api.h"
#if defined(ENABLE_UIKIT) || defined(ENABLE_LVGL)
#include "graphic_service_wrapper.h"
#endif

#define MODULE_NAME "jpeg_vdec"

#define MAX_BUFFER_NUM 5

extern void ArchDCacheFlush(void);

typedef struct {
    OutputInfo outBuf;
    bool inited;
} BufferDesp;

typedef struct {
    BufferDesp bufs[MAX_BUFFER_NUM];
    int32_t readIndex;
    int32_t writeIndex;
    MediaHalMutexHandle queueMutex;
} ArrayQueue;

typedef enum {
    INITED,
    STARTED,
    STOPPED
} JpegCtxState;

typedef struct {
    bool sync;
    bool receiveEos;
    MediaHalThreadIdHandle asyncJPEGThread;
    JpegCtxState state;
    ArrayQueue outFreeQueue;
    ArrayQueue outFilledQueue;
} NativeJPEGContext;

static MediaHalMutexHandle g_mutex = NULL;

#define JPEG_DEC_WIDTH_MAX 600
#define JPEG_DEC_HEIGHT_MAX 600

#define JPEG_DEC_WIDTH_MIN 8
#define JPEG_DEC_HEIGHT_MIN 8

static const CodecCapbility g_vdecCapEntries[] = {
    {
        MEDIA_MIMETYPE_IMAGE_JPEG, VIDEO_DECODER, {ALGIN_LEVEL_2, ALGIN_LEVEL_2},
        {JPEG_DEC_WIDTH_MIN, JPEG_DEC_HEIGHT_MIN}, {JPEG_DEC_WIDTH_MAX, JPEG_DEC_HEIGHT_MAX},
        VID_BITRATE_LEVEL1, VID_BITRATE_LEVEL2, {{INVALID_PROFILE}, 1}, {{INVALID_LEVEL}, 1},
        {{YVU_SEMIPLANAR_420}, 1}, 1, 1, ALLOCATE_INPUT_BUFFER_USER | ALLOCATE_OUTPUT_BUFFER_CODEC, 0
    },
    {
        MEDIA_MIMETYPE_IMAGE_MJPEG, VIDEO_DECODER, {ALGIN_LEVEL_2, ALGIN_LEVEL_2},
        {JPEG_DEC_WIDTH_MIN, JPEG_DEC_HEIGHT_MIN}, {JPEG_DEC_WIDTH_MAX, JPEG_DEC_HEIGHT_MAX},
        VID_BITRATE_LEVEL1, VID_BITRATE_LEVEL2, {{INVALID_PROFILE}, 1}, {{INVALID_LEVEL}, 1},
        {{YVU_SEMIPLANAR_420}, 1}, 1, 1, ALLOCATE_INPUT_BUFFER_USER | ALLOCATE_OUTPUT_BUFFER_CODEC, 0
    },
};

static void InitOutQueue(ArrayQueue *que)
{
    for (int32_t i = 0; i < MAX_BUFFER_NUM; i++) {
        if (memset_s(&(que->bufs[i].outBuf), sizeof(OutputInfo), 0, sizeof(OutputInfo)) != EOK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "memset_s failed");
        }
        CodecBufferInfo *inf = (CodecBufferInfo *)malloc(sizeof(CodecBufferInfo));
        if (inf == NULL) {
            MEDIA_HAL_LOGE(MODULE_NAME, "malloc failed");
            return;
        }
        if (memset_s(inf, sizeof(CodecBufferInfo), 0, sizeof(CodecBufferInfo)) != EOK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "memset_s failed");
        }
        que->bufs[i].outBuf.buffers = inf;
        que->bufs[i].inited = true;
    }

    que->readIndex = 0;
    que->writeIndex = 0;
    return;
}

static void DeinitOutQueue(ArrayQueue *que)
{
    for (int32_t i = 0; i < MAX_BUFFER_NUM; i++) {
        if (que->bufs[i].outBuf.buffers != NULL) {
            free(que->bufs[i].outBuf.buffers);
            que->bufs[i].outBuf.buffers = NULL;
        }
        if (memset_s(&(que->bufs[i].outBuf), sizeof(OutputInfo), 0, sizeof(OutputInfo)) != EOK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "memset_s failed");
        }
        que->bufs[i].inited = false;
    }

    que->readIndex = 0;
    que->writeIndex = 0;
    return;
}

static bool IsFull(const ArrayQueue *que)
{
    if ((que->writeIndex + 1) % MAX_BUFFER_NUM == que->readIndex) {
        return true;
    }
    return false;
}

static bool PushOutInfoInQueue(ArrayQueue *que, const OutputInfo *outBuf)
{
    MediaHalMutexLock(que->queueMutex);
    if (IsFull(que)) {
        MediaHalMutexUnLock(que->queueMutex);
        return false;
    }

    if (que->bufs[que->writeIndex].outBuf.buffers == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "buffer is NULL");
        MediaHalMutexUnLock(que->queueMutex);
        return false;
    }

    // 注意这里有陷阱，不能直接结构体赋值，否则会将队列元素中malloc的指针覆盖
    // 此处采用逐项赋值方式，稳妥
    que->bufs[que->writeIndex].outBuf.bufferCnt = outBuf->bufferCnt;
    que->bufs[que->writeIndex].outBuf.buffers[0] = outBuf->buffers[0]; // 元素中的内存是 init 时分配的
    que->bufs[que->writeIndex].outBuf.flag = outBuf->flag;
    que->bufs[que->writeIndex].outBuf.sequence = outBuf->sequence;
    que->bufs[que->writeIndex].outBuf.timeStamp = outBuf->timeStamp;
    que->bufs[que->writeIndex].outBuf.type = outBuf->type;
    que->bufs[que->writeIndex].outBuf.vendorPrivate = outBuf->vendorPrivate;
    que->writeIndex++;
    que->writeIndex %= MAX_BUFFER_NUM;
    MediaHalMutexUnLock(que->queueMutex);
    return true;
}

static bool IsEmpty(const ArrayQueue *que)
{
    bool ret = que->readIndex == que->writeIndex;
    return ret;
}

static bool PopOutInfoFromQueue(ArrayQueue *que, OutputInfo *outBuf)
{
    MediaHalMutexLock(que->queueMutex);
    if (IsEmpty(que)) {
        MediaHalMutexUnLock(que->queueMutex);
        return false;
    }

    // 注意这里有陷阱，不能直接结构体赋值，否则会将队列元素中malloc的指针覆盖
    // 此处采用逐项赋值方式，稳妥
    outBuf->bufferCnt = que->bufs[que->readIndex].outBuf.bufferCnt;
    outBuf->buffers[0] = que->bufs[que->readIndex].outBuf.buffers[0];
    outBuf->flag = que->bufs[que->readIndex].outBuf.flag;
    outBuf->sequence = que->bufs[que->readIndex].outBuf.sequence;
    outBuf->timeStamp = que->bufs[que->readIndex].outBuf.timeStamp;
    outBuf->type = que->bufs[que->readIndex].outBuf.type;
    outBuf->vendorPrivate = que->bufs[que->readIndex].outBuf.vendorPrivate;
    que->readIndex++;
    que->readIndex %= MAX_BUFFER_NUM;
    MediaHalMutexUnLock(que->queueMutex);
    return true;
}

static int32_t NativePrepareDecode(jpeg_decompress_ptr dinfo, const InputInfo *frame)
{
    td_bool isMem = true;
    struct jpeg_source_struct src = { 0 };

    src.size = frame->buffers[0].length;
    src.vir_buf = (td_char *)frame->buffers[0].addr;
    // input es buffer address from malloc with cache, the decoder cannot directly access the address,
    // so we need to flush the cache
    ArchDCacheFlush();
    src.phy_buf = (td_u32)(uintptr_t)src.vir_buf;

    dinfo->output_yuv420sp = true;
    dinfo->scale = JPEG_SCALE_1;

    int32_t ret = uapi_jpeg_decomress_create(dinfo, &src, isMem);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "jpeg create fail:0x%x", ret);
    }

    return ret;
}

static int32_t NativeJPEGCreate(void **vdecHdl, const Param *attr, int len)
{
    CHK_NULL_RETURN(vdecHdl, MEDIA_HAL_ERR, "vdecHdl null");
    MEDIA_HAL_UNUSED(attr);
    MEDIA_HAL_UNUSED(len);

    NativeJPEGContext *jpegCtx = (NativeJPEGContext *)malloc(sizeof(NativeJPEGContext));
    if (jpegCtx == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "malloc failed");
        return MEDIA_HAL_ERR;
    }

    if (memset_s(jpegCtx, sizeof(NativeJPEGContext), 0, sizeof(NativeJPEGContext)) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "memset_s failed");
    }

    InitOutQueue(&jpegCtx->outFreeQueue);
    MediaHalMutexAttr mutexAttr = { false };
    jpegCtx->outFreeQueue.queueMutex = MediaHalMutexCreate(&mutexAttr);
    InitOutQueue(&jpegCtx->outFilledQueue);
    jpegCtx->outFilledQueue.queueMutex = MediaHalMutexCreate(&mutexAttr);
    jpegCtx->state = INITED;
    jpegCtx->sync = true; // todo 未来可能要调整成异步的
    *vdecHdl = jpegCtx;

    return MEDIA_HAL_OK;
}

static int32_t NativeJPEGDestroy(void *nativeVdecHdl)
{
    CHK_NULL_RETURN(nativeVdecHdl, MEDIA_HAL_ERR, "vdec handle is NULL");

    NativeJPEGContext *jpegCtx = (NativeJPEGContext *)nativeVdecHdl;

    DeinitOutQueue(&jpegCtx->outFreeQueue);
    DeinitOutQueue(&jpegCtx->outFilledQueue);

    MediaHalMutexDestroy(&jpegCtx->outFreeQueue.queueMutex);
    MediaHalMutexDestroy(&jpegCtx->outFilledQueue.queueMutex);

    free(jpegCtx);
    return MEDIA_HAL_OK;
}

#ifdef JPEG_DEC_ASYNC
static void *AsyncJpegCodecProcess(void *arg)
{
    NativeJPEGContext *jpegCtx = (NativeJPEGContext *)arg;
    CHK_NULL_RETURN(jpegCtx, MEDIA_HAL_ERR, "jpegCtx null");
    MEDIA_HAL_LOGD(MODULE_NAME, "async process state:%d", devCtx->devState);
    prctl(PR_SET_NAME, "virtualCameraProcess", 0, 0, 0);
    while (jpegCtx->state == STARTED) {
        // 1. todo 从输入队列中读取 input (注意数据有效性)

        // 2. 从空闲buffer中获取到可用的 outbuffer
        OutputInfo curOutBuffer = { 0 };
        int32_t tryCnt = TRY_MAX_COUNT;
        while (!!PopOutInfoFromQueue(&jpegCtx->outFreeQueue, &curOutBuffer)) {
            if (tryCnt-- == 0) {
                MEDIA_HAL_LOGE(MODULE_NAME, "pop free buffer fail in many times");
                return NULL;
            }
            usleep(ASYNC_PROCESS_SLEEP_TIME_5MS);
        }

        if (NativeDecodeJpeg(vdecStream, &curOutBuffer) != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "decode jpeg fail");
            break;
        }
    }

    MEDIA_HAL_LOGE(MODULE_NAME, "out thread out");
    return NULL;
}
#endif

static int32_t NativeJPEGStart(void *nativeVdecHdl)
{
    CHK_NULL_RETURN(nativeVdecHdl, MEDIA_HAL_ERR, "vdecHdl null");
    NativeJPEGContext *jpegCtx = (NativeJPEGContext *)nativeVdecHdl;
    int32_t ret = MEDIA_HAL_OK;
    jpegCtx->state = STARTED;

#ifdef JPEG_DEC_ASYNC
    MediaHalThreadattr attr = { "codecJpegProcess", 0x4000, THREAD_SCHED_INVALID, 0, false };
    jpegCtx->asyncJPEGThread = MediaHalThreadCreate(AsyncJpegCodecProcess, jpegCtx, &attr);
    if (jpegCtx.asyncJPEGThread == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "async jpeg thread create failed");
        ret = MEDIA_HAL_ERR;
    }
#endif

    return ret;
}

static int32_t NativeDoDecode(jpeg_decompress_ptr dinfo, OutputInfo *curOutBuffer)
{
    struct jpeg_dst_struct dst = { 0 };
    dst.stride = dinfo->output.stride[0];
    dst.uv_stride = dinfo->output.stride[1];
    if (UINT_MAX - dinfo->output.size[0] < dinfo->output.size[1]) {
        dst.size = UINT_MAX;
    } else {
        dst.size = dinfo->output.size[0] + dinfo->output.size[1];
    }
    dst.uv_offset = dinfo->output.size[0];
    dst.vir_buf = (td_char *)curOutBuffer->buffers[0].addr;
    dst.phy_buf = (td_u32)(uintptr_t)dst.vir_buf;

    uint64_t startTime = GetTimeTicks64();
    int32_t ret = uapi_jpeg_decompress_start(dinfo, &dst);
    MEDIA_HAL_LOGD(MODULE_NAME, "decode ret:%d cost: %lld", ret, GetTimeTicks64() - startTime);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "jpeg decode fail:0x%x", ret);
        return ret;
    }

    return MEDIA_HAL_OK;
}

static int32_t NativeDecodeJpeg(NativeJPEGContext *jpegCtx, const InputInfo *frame, OutputInfo *curOutBuffer)
{
    struct jpeg_decompress_struct dinfo = { 0 };
    int32_t ret = NativePrepareDecode(&dinfo, frame);
    if (ret != MEDIA_HAL_OK) {
        goto FAIL2;
    }

    MEDIA_HAL_LOGD(MODULE_NAME, "in w:%d h:%d c:%d", dinfo.image_width, dinfo.image_height, dinfo.image_color_space);
    MEDIA_HAL_LOGD(MODULE_NAME, "ou y w:%d h:%d s:%d s:%d", dinfo.output.width[0], dinfo.output.height[0],
        dinfo.output.stride[0], dinfo.output.size[0]);
    MEDIA_HAL_LOGD(MODULE_NAME, "ou uv w:%d h:%d s:%d s:%d", dinfo.output.width[1], dinfo.output.height[1],
        dinfo.output.stride[1], dinfo.output.size[1]);

    ret = NativeDoDecode(&dinfo, curOutBuffer);
    if (ret != MEDIA_HAL_OK) {
        goto FAIL1;
    }

    // 更新 outbuffer 时间戳，播放器音视频同步策略会用到
    curOutBuffer->timeStamp = frame->pts;
    // 解码完把解码后的 outbuffer 放置到已装填buffer
    if (!PushOutInfoInQueue(&jpegCtx->outFilledQueue, curOutBuffer)) {
        MEDIA_HAL_LOGE(MODULE_NAME, "push filled buffer fail");
        ret = MEDIA_HAL_ERR;
        goto FAIL1;
    }

FAIL1:
    uapi_jpeg_decompress_destroy(&dinfo);
FAIL2:
    return ret;
}

static int32_t NativeJPEGSendStream(void *nativeVdecHdl, const InputInfo *vdecStream, uint32_t timeout)
{
    NativeJPEGContext *jpegCtx = (NativeJPEGContext *)nativeVdecHdl;
    CHK_NULL_RETURN(jpegCtx, MEDIA_HAL_ERR, "vdecCtx null");
    CHK_NULL_RETURN(vdecStream, MEDIA_HAL_ERR, "vdecStream null");
    MEDIA_HAL_UNUSED(timeout);

    if (vdecStream->flag == STREAM_FLAG_EOS) {
        jpegCtx->receiveEos = true;
        MEDIA_HAL_LOGI(MODULE_NAME, "received input eos");
        return MEDIA_HAL_OK;
    }

    // 对接到 jpeg 上
    // 从空闲 buffer 中获取到可用的 outbuffer
    // 当前不起单独解码线程，通路try通后考虑起单线程优化（需要额外考虑数据有效性）
    OutputInfo curOutBuffer = {};
    curOutBuffer.buffers = (CodecBufferInfo *)malloc(sizeof(CodecBufferInfo));
    if (curOutBuffer.buffers == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "malloc fail");
        return MEDIA_HAL_ERR;
    }
    if (!PopOutInfoFromQueue(&jpegCtx->outFreeQueue, &curOutBuffer)) {
        MEDIA_HAL_LOGE(MODULE_NAME, "pop free out buffer fail, queue out buf first at begin");
        free(curOutBuffer.buffers);
        return MEDIA_HAL_ERR;
    }

    if (NativeDecodeJpeg(jpegCtx, vdecStream, &curOutBuffer) != MEDIA_HAL_OK) {
        free(curOutBuffer.buffers);
        MEDIA_HAL_LOGE(MODULE_NAME, "decode jpeg fail");
        return MEDIA_HAL_ERR;
    }

    free(curOutBuffer.buffers);
    return MEDIA_HAL_OK;
}

static int32_t NativeJPEGQueueOutBuf(void *nativeVdecHdl, OutputInfo *outInfo, int releaseFenceFd)
{
    MEDIA_HAL_UNUSED(releaseFenceFd);
    NativeJPEGContext *jpegCtx = (NativeJPEGContext *)nativeVdecHdl;
    CHK_NULL_RETURN(jpegCtx, MEDIA_HAL_INVALID_PARAM, "vdecCtx null");
    CHK_NULL_RETURN(outInfo, MEDIA_HAL_INVALID_PARAM, "outInfo null");
    // 这里要求 buffers 所指向的内存由调用方分配
    CHK_NULL_RETURN(outInfo->buffers, MEDIA_HAL_ERR, "out buf is NULL");

    if (outInfo->bufferCnt != 1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "not support multi buffer in OutputInfo");
        return MEDIA_HAL_ERR;
    }

    if (!PushOutInfoInQueue(&jpegCtx->outFreeQueue, outInfo)) {
        return MEDIA_HAL_ERR;
    }

    return MEDIA_HAL_OK;
}

static int32_t NativeJPEGDeqOutBuf(void *nativeVdecHdl, OutputInfo *outInfo, uint32_t timeout, int *acquireFd)
{
    MEDIA_HAL_UNUSED(acquireFd);
    MEDIA_HAL_UNUSED(timeout);
    int32_t retCode = MEDIA_HAL_ERR;
    NativeJPEGContext *jpegCtx = (NativeJPEGContext *)nativeVdecHdl;
    CHK_NULL_RETURN(jpegCtx, MEDIA_HAL_ERR, "vdecCtx null");
    CHK_NULL_RETURN(outInfo, MEDIA_HAL_ERR, "outInfo null");
    // 这里要求 buffers 所指向的内存由调用方分配
    CHK_NULL_RETURN(outInfo->buffers, false, "out buf is NULL");

    if (jpegCtx->receiveEos) {
        MEDIA_HAL_LOGI(MODULE_NAME, "received input eos");
        return VDEC_FRAME_BUF_EOS;
    }

    // 从解码帧队列中 pop buffer
    if (!PopOutInfoFromQueue(&jpegCtx->outFilledQueue, outInfo)) {
        MEDIA_HAL_LOGE(MODULE_NAME, "pop filled buffer fail");
        return MEDIA_HAL_ERR;
    }
    return MEDIA_HAL_OK;
}

static int32_t NativeJPEGStop(void *nativeVdecHdl)
{
    NativeJPEGContext *jpegCtx = (NativeJPEGContext *)nativeVdecHdl;
    CHK_NULL_RETURN(jpegCtx, MEDIA_HAL_ERR, "vdecCtx null");

    MediaHalInitStaticMutexLock(&g_mutex);
    MediaHalMutexLock(g_mutex);
    jpegCtx->state = STOPPED;
#ifdef JPEG_DEC_ASYNC
    MediaHalThreadJoin(&jpegCtx->asyncJPEGThread);
#endif
    MediaHalMutexUnLock(g_mutex);
    return MEDIA_HAL_OK;
}

static int32_t NativeJPEGGetStatus(void *nativeVdecHdl, AvVdecStatus *vdecStatus)
{
    MEDIA_HAL_UNUSED(nativeVdecHdl);
    MEDIA_HAL_UNUSED(vdecStatus);
    return MEDIA_HAL_OK;
}

static int32_t NativeJPEGReset(void *nativeVdecHdl)
{
    CHK_NULL_RETURN(nativeVdecHdl, MEDIA_HAL_ERR, "vdecHdl null");
    NativeJPEGContext *jpegCtx = (NativeJPEGContext *)nativeVdecHdl;

    DeinitOutQueue(&jpegCtx->outFreeQueue);
    DeinitOutQueue(&jpegCtx->outFilledQueue);

    InitOutQueue(&jpegCtx->outFreeQueue);
    InitOutQueue(&jpegCtx->outFilledQueue);

    jpegCtx->receiveEos = false;
    return MEDIA_HAL_OK;
}

static int32_t NativeJPEGInvoke(void *nativeVdecHdl, uint32_t cmd, void *arg1, void *arg2)
{
    MEDIA_HAL_UNUSED(nativeVdecHdl);
    MEDIA_HAL_UNUSED(cmd);
    MEDIA_HAL_UNUSED(arg1);
    MEDIA_HAL_UNUSED(arg2);
    MEDIA_HAL_LOGD(MODULE_NAME, "invalid cmd %d", cmd);
    return MEDIA_HAL_OK;
}

void *VDecoderGetImpl(void)
{
    AvVideoDecoder *decoder = (AvVideoDecoder *)malloc(sizeof(AvVideoDecoder));
    if (decoder == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "malloc failed");
        return NULL;
    }
    if (memset_s(decoder, sizeof(AvVideoDecoder), 0x00, sizeof(AvVideoDecoder)) != EOK) {
        free(decoder);
        MEDIA_HAL_LOGE(MODULE_NAME, "memset_s failed");
        return NULL;
    }

    decoder->pluginCommon.pfnCreate = NativeJPEGCreate;
    decoder->pluginCommon.isHardwarePlugin = true;
    decoder->pluginCommon.pfnDestroy = NativeJPEGDestroy;
    decoder->pluginCommon.pfnStart = NativeJPEGStart;
    decoder->pluginCommon.pfnStop = NativeJPEGStop;
    decoder->pfnQueueInput = NativeJPEGSendStream;
    decoder->pfnDequeueOutput = NativeJPEGDeqOutBuf;
    decoder->pfnQueueOutput = NativeJPEGQueueOutBuf;
    decoder->pfnGetStatus = NativeJPEGGetStatus;
    decoder->pluginCommon.pfnReset = NativeJPEGReset;
    decoder->pluginCommon.pfnInvoke = NativeJPEGInvoke;

    int validCapNum = sizeof(g_vdecCapEntries) / sizeof(g_vdecCapEntries[0]);
    decoder->pluginCommon.validCapbilityNum = validCapNum;
    for (int i = 0; i < validCapNum; i++) {
        decoder->pluginCommon.capbilites[i] = g_vdecCapEntries[i];
    }

    return decoder;
}

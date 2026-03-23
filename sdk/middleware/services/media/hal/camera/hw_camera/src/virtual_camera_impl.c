/*
 * Copyright (c) @CompanyNameMagicTag. 2021-2021. All rights reserved.
 * Description: virtual camera impl
 * Author: media_develop team
 * Create: 20211012
 */

#include "hw_camera.h"
#include "demuxer_interface.h"
#include "demuxer_common.h"
#include "protocol_interface.h"
#include "soc_jpeg_api.h"
#include "media_hal_common.h"
#include "media_hal_thread_adapt.h"

#include <sys/prctl.h>
#include <unistd.h>
#include <pthread.h>
#include "securec.h"
#ifdef SUPPORT_DMA
#include "asm/dma.h"
#endif

#define MODULE_NAME "virtual_camera"
#define MAX_BUFFER_NUM 5
#define ONLY_ONE_CAMERA_DEVICE 1
#define TRY_MAX_COUNT 1000 // ui draw slow when first pic, so improve sleep time
#define DEMUX_READ_RETRY_TIME_US 10000
#define ASYNC_PROCESS_SLEEP_TIME_US 10000

extern void ArchDCacheFlush(void);

typedef enum {
    OPENED,
    CLOSED
} DeviceState;

typedef struct {
    HalBuffer halBuf;
    bool valid;
} BufferDesp;

typedef struct {
    BufferDesp bufs[MAX_BUFFER_NUM];
    int32_t readIndex;
    int32_t writeIndex;
    MediaHalMutexHandle queueMutex;
} ArrayQueue;

typedef struct {
    DemuxerPluginEntry demuxPlugin;
    ProtocolFun proto;
    void *demuxHandle;
    DeviceState devState;
    ArrayQueue freeQueue;
    ArrayQueue filledQueue;
    uint32_t width;
    uint32_t height;
    StreamAttr streamInfo;
    char *url;
    BufferAvailable bufferAvailbleCallback;
    HwCameraVoidPtr userData;
    MediaHalThreadIdHandle callbackThread;
} HwCameraContext;

static HwCameraContext g_hwCameraContext = { 0 };
static MediaHalMutexHandle g_mutex = NULL;

static void InitQueue(ArrayQueue *que)
{
    for (int32_t i = 0; i < MAX_BUFFER_NUM; i++) {
        que->bufs[i].valid = false;
        if (memset_s(&(que->bufs[i].halBuf), sizeof(HalBuffer), 0, sizeof(HalBuffer)) != EOK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "memset_s failed");
        }
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

static bool IsEmpty(const ArrayQueue *que)
{
    bool ret = que->readIndex == que->writeIndex;
    return ret;
}

static bool PushElement(ArrayQueue *que, const HalBuffer *halBuf)
{
    MediaHalMutexLock(que->queueMutex);
    if (IsFull(que)) {
        MediaHalMutexUnLock(que->queueMutex);
        return false;
    }
    BufferDesp bufDesp = {*halBuf, true};
    que->bufs[que->writeIndex] = bufDesp;
    que->writeIndex++;
    que->writeIndex %= MAX_BUFFER_NUM;
    MediaHalMutexUnLock(que->queueMutex);
    return true;
}

static bool PopElement(ArrayQueue *que, HalBuffer *halBuf)
{
    MediaHalMutexLock(que->queueMutex);
    if (IsEmpty(que)) {
        MediaHalMutexUnLock(que->queueMutex);
        return false;
    }
    *halBuf = que->bufs[que->readIndex].halBuf;
    que->readIndex++;
    que->readIndex %= MAX_BUFFER_NUM;
    MediaHalMutexUnLock(que->queueMutex);
    return true;
}

int32_t HwCameraInit(void)
{
    return MEDIA_HAL_OK;
}

int32_t HwCameraDeinit(void)
{
    return MEDIA_HAL_OK;
}

int32_t HwCameraGetDeviceNum(uint8_t *num)
{
    if (num == NULL) {
        return MEDIA_HAL_INVALID_PARAM;
    }
    *num = ONLY_ONE_CAMERA_DEVICE;

    return MEDIA_HAL_OK;
}

int32_t HwCameraGetDeviceList(uint32_t *cameraList, uint8_t listNum)
{
    CHK_NULL_RETURN(cameraList, MEDIA_HAL_INVALID_PARAM, "cameraList is null");
    if (listNum != ONLY_ONE_CAMERA_DEVICE) {
        return MEDIA_HAL_INVALID_PARAM;
    }
    *cameraList = ONLY_ONE_CAMERA_DEVICE - 1;

    return MEDIA_HAL_OK;
}

int32_t HwCameraDeviceOpen(uint32_t cameraId)
{
    MEDIA_HAL_UNUSED(cameraId);
    MediaHalInitStaticMutexLock(&g_mutex);
    MediaHalMutexLock(g_mutex);
    const DemuxerPluginEntry *demux = GetDemuxer();
    const ProtocolPluginEntry *proto = GetProtocol();
    if (demux == NULL || proto == NULL) {
        MediaHalMutexUnLock(g_mutex);
        return MEDIA_HAL_ERR;
    }

    if (memcpy_s(&(g_hwCameraContext.demuxPlugin), sizeof(DemuxerPluginEntry),
        demux, sizeof(DemuxerPluginEntry)) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "memcpy_s failed");
        MediaHalMutexUnLock(g_mutex);
        return MEDIA_HAL_ERR;
    }

    g_hwCameraContext.proto.protocol_find = proto->protocol_find;
    g_hwCameraContext.proto.protocol_open = proto->protocol_open;
    g_hwCameraContext.proto.protocol_read = proto->protocol_read;
    g_hwCameraContext.proto.protocol_write = proto->protocol_write;
    g_hwCameraContext.proto.protocol_seek = proto->protocol_seek;
    g_hwCameraContext.proto.protocol_seek_stream = proto->protocol_seek_stream;
    g_hwCameraContext.proto.protocol_invoke = proto->protocol_invoke;
    g_hwCameraContext.proto.protocol_close = proto->protocol_close;

    InitQueue(&g_hwCameraContext.freeQueue);
    MediaHalMutexAttr attr = { false };
    g_hwCameraContext.freeQueue.queueMutex = MediaHalMutexCreate(&attr);
    InitQueue(&g_hwCameraContext.filledQueue);
    g_hwCameraContext.filledQueue.queueMutex = MediaHalMutexCreate(&attr);
    g_hwCameraContext.devState = OPENED;
    MediaHalMutexUnLock(g_mutex);

    return MEDIA_HAL_OK;
}

int32_t HwCameraDeviceClose(uint32_t cameraId)
{
    MEDIA_HAL_UNUSED(cameraId);
    MediaHalInitStaticMutexLock(&g_mutex);
    MediaHalMutexLock(g_mutex);
    InitQueue(&g_hwCameraContext.freeQueue);
    InitQueue(&g_hwCameraContext.filledQueue);
    g_hwCameraContext.devState = CLOSED;
    if (g_hwCameraContext.demuxPlugin.fmt_close != NULL) {
        g_hwCameraContext.demuxPlugin.fmt_close(g_hwCameraContext.demuxHandle);
    }

    if (g_hwCameraContext.url != NULL) {
        free(g_hwCameraContext.url);
        g_hwCameraContext.url = NULL;
    }

    MediaHalMutexDestroy(&(g_hwCameraContext.freeQueue.queueMutex));
    MediaHalMutexDestroy(&(g_hwCameraContext.filledQueue.queueMutex));
    if (memset_s(&g_hwCameraContext, sizeof(HwCameraContext), 0, sizeof(HwCameraContext)) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "memset_s failed");
    }

    MediaHalMutexUnLock(g_mutex);

    return MEDIA_HAL_OK;
}

int32_t HwCameraDeviceSetInfo(uint32_t cameraId, const DeviceInfo *info)
{
    MEDIA_HAL_UNUSED(cameraId);
    CHK_NULL_RETURN(info, MEDIA_HAL_INVALID_PARAM, "DeviceInfo is null");
    if (info->u.privateData == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "privateData is null");
        return MEDIA_HAL_INVALID_PARAM;
    }
    MediaHalInitStaticMutexLock(&g_mutex);
    MediaHalMutexLock(g_mutex);
    if (g_hwCameraContext.devState != OPENED) {
        MediaHalMutexUnLock(g_mutex);
        return MEDIA_HAL_ERR;
    }
    if (g_hwCameraContext.url != NULL) {
        free(g_hwCameraContext.url);
        g_hwCameraContext.url = NULL;
    }
    g_hwCameraContext.url = strdup(info->u.privateData);
    if (g_hwCameraContext.url == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "strdup failed");
        MediaHalMutexUnLock(g_mutex);
        return MEDIA_HAL_ERR;
    }
    MediaHalMutexUnLock(g_mutex);
    return MEDIA_HAL_OK;
}

int32_t HwCameraStreamsCreate(uint32_t cameraId,
    const StreamAttr *streamList, uint32_t *streamIdList, uint8_t num)
{
    MEDIA_HAL_UNUSED(cameraId);
    CHK_NULL_RETURN(streamList, MEDIA_HAL_INVALID_PARAM, "streamList is null");
    CHK_NULL_RETURN(streamIdList, MEDIA_HAL_INVALID_PARAM, "streamIdList is null");
    CHK_NULL_RETURN(g_hwCameraContext.url, MEDIA_HAL_INVALID_PARAM, "url is null");
    if (num != ONLY_ONE_CAMERA_DEVICE) {
        return MEDIA_HAL_INVALID_PARAM;
    }
    MediaHalInitStaticMutexLock(&g_mutex);
    MediaHalMutexLock(g_mutex);
    g_hwCameraContext.streamInfo = *streamList;
    *streamIdList = ONLY_ONE_CAMERA_DEVICE - 1;
    DemuxerSource source = { 0 };
    source.type = DEMUXER_SOURCE_TYPE_URI;
    if (strcpy_s(source.url, URL_LEN, g_hwCameraContext.url) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "strcpy_s failed");
        MediaHalMutexUnLock(g_mutex);
        return MEDIA_HAL_ERR;
    }
    ProtocolFun *protocol = &(g_hwCameraContext.proto);
    int32_t ret = g_hwCameraContext.demuxPlugin.fmt_find(&source, protocol);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "demux format find fail:%d", ret);
        MediaHalMutexUnLock(g_mutex);
        return MEDIA_HAL_ERR;
    }

    int32_t score = 0;
    ret = g_hwCameraContext.demuxPlugin.fmt_open(&source, protocol, &g_hwCameraContext.demuxHandle, &score);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "demux format open fail:%d", ret);
        MediaHalMutexUnLock(g_mutex);
        return MEDIA_HAL_ERR;
    }
    MEDIA_HAL_LOGI(MODULE_NAME, "mjpeg demux open successful, score is %d\n", score);
    MediaHalMutexUnLock(g_mutex);
    return MEDIA_HAL_OK;
}

int32_t HwCameraStreamsDestroy(uint32_t cameraId, const uint32_t *streamIdList, uint8_t num)
{
    (void)cameraId;
    (void)streamIdList;
    (void)num;
    return MEDIA_HAL_OK;
}

static int32_t HwCameraPrepareDecode(jpeg_decompress_ptr dinfo, const FormatPacket *frame)
{
    td_bool isMem = true;
    struct jpeg_source_struct src = { 0 };
    src.size = (uint32_t)frame->len;
    src.vir_buf = (td_char *)frame->data;
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

static int32_t HwCameraDoDecode(jpeg_decompress_ptr dinfo, HalBuffer *curOutBuffer)
{
    struct jpeg_dst_struct dst = { 0 };
    dst.stride    = dinfo->output.stride[0];
    dst.uv_stride = dinfo->output.stride[1];
    dst.size      = dinfo->output.size[0] + dinfo->output.size[1];
    dst.uv_offset = dinfo->output.size[0];
    dst.vir_buf = curOutBuffer->virAddr;
    dst.phy_buf = (td_u32)(uintptr_t)dst.vir_buf;
    if (memset_s(dst.vir_buf, (size_t)curOutBuffer->size, 0, (size_t)curOutBuffer->size) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "memset_s failed");
    }

    uint64_t startTime = GetTimeTicks64();
    int32_t ret = uapi_jpeg_decompress_start(dinfo, &dst);
    MEDIA_HAL_LOGD(MODULE_NAME, "decode ret:%d const %lld", ret, GetTimeTicks64() - startTime);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "jpeg decode fail:0x%x", ret);
        return ret;
    }
    curOutBuffer->format = FORMAT_YVU420;
    curOutBuffer->size = (int32_t)dst.size;
    curOutBuffer->width = (int32_t)dinfo->image_width;
    curOutBuffer->height = (int32_t)dinfo->image_height;
    curOutBuffer->stride0 = (int32_t)dst.stride;
    curOutBuffer->stride1 = (int32_t)dst.uv_stride;

    return MEDIA_HAL_OK;
}


static int32_t HwCameraDecodeJpeg(HwCameraContext *devCtx, FormatPacket *frame, HalBuffer *curOutBuffer)
{
    struct jpeg_decompress_struct dinfo = { 0 };
    int32_t ret = HwCameraPrepareDecode(&dinfo, frame);
    if (ret != MEDIA_HAL_OK) {
        goto FAIL2;
    }

    MEDIA_HAL_LOGD(MODULE_NAME, "in w:%d h:%d c:%d",
        dinfo.image_width, dinfo.image_height, dinfo.image_color_space);
    MEDIA_HAL_LOGD(MODULE_NAME, "ou y w:%d h:%d s:%d s:%d", dinfo.output.width[0], dinfo.output.height[0],
        dinfo.output.stride[0], dinfo.output.size[0]);
    MEDIA_HAL_LOGD(MODULE_NAME, "ou uv w:%d h:%d s:%d s:%d", dinfo.output.width[1], dinfo.output.height[1],
        dinfo.output.stride[1], dinfo.output.size[1]);

    // 1. start decode
    ret = HwCameraDoDecode(&dinfo, curOutBuffer);
    if (ret != MEDIA_HAL_OK) {
        goto FAIL1;
    }

    // 2. push output buffer to filled queue after decode
    if (!PushElement(&devCtx->filledQueue, curOutBuffer)) {
        MEDIA_HAL_LOGE(MODULE_NAME, "push filled buffer fail");
        ret = MEDIA_HAL_ERR;
        goto FAIL1;
    }

    // 3. reclaim resource
FAIL1:
    uapi_jpeg_decompress_destroy(&dinfo);
FAIL2:
    if (devCtx->demuxPlugin.fmt_free(devCtx->demuxHandle, frame) != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "demux free fail");
        return MEDIA_HAL_ERR;
    }
    return ret;
}

static void* AsyncProcess(void *arg)
{
    HwCameraContext *devCtx = (HwCameraContext *)arg;
    if (devCtx == NULL || devCtx->demuxHandle == NULL || devCtx->demuxPlugin.fmt_read == NULL) {
        return NULL;
    }

    MEDIA_HAL_LOGD(MODULE_NAME, "async process state:%d", devCtx->devState);
    prctl(PR_SET_NAME, "virtualCameraProcess", 0, 0, 0);
    while (devCtx->devState == OPENED) {
        // 1. demux read input frame
        FormatPacket frame = { 0 };
        int32_t ret = devCtx->demuxPlugin.fmt_read(devCtx->demuxHandle, &frame);
        if (ret == RET_FILE_EOF) {
            break;
        }
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "demux read fail:%d", ret);
            usleep(DEMUX_READ_RETRY_TIME_US);
            continue;
        }

        // 2. pop buffer from free queue
        HalBuffer curOutBuffer = { 0 };
        int32_t tryCnt = TRY_MAX_COUNT;
        while (!PopElement(&devCtx->freeQueue, &curOutBuffer)) {
            if (tryCnt-- == 0) {
                MEDIA_HAL_LOGE(MODULE_NAME, "pop free buffer fail in many times");
                return NULL;
            }
            usleep(ASYNC_PROCESS_SLEEP_TIME_US);
        }

        // 3. decode jpeg frame
        if (HwCameraDecodeJpeg(devCtx, &frame, &curOutBuffer) != MEDIA_HAL_OK) {
            break;
        }

        // 4. pop buffer from filled queue and callback to the camera's stream manager
        HalBuffer decodedBuffer;
        if (!PopElement(&devCtx->filledQueue, &decodedBuffer)) {
            MEDIA_HAL_LOGE(MODULE_NAME, "pop filled buffer fail:%d", ret);
            return NULL;
        }
        if (devCtx->bufferAvailbleCallback != NULL && devCtx->userData != NULL) {
            devCtx->bufferAvailbleCallback(ONLY_ONE_CAMERA_DEVICE - 1, &decodedBuffer,
                ONLY_ONE_CAMERA_DEVICE, devCtx->userData);
        }
    }

    MEDIA_HAL_LOGE(MODULE_NAME, "out thread out");
    return NULL;
}

int32_t HwCameraSetBufferCallback(uint32_t cameraId, const BufferAvailable callback, const HwCameraVoidPtr userData)
{
    CHK_NULL_RETURN(callback, MEDIA_HAL_INVALID_PARAM, "callback is NULL");
    CHK_NULL_RETURN(userData, MEDIA_HAL_INVALID_PARAM, "userData is NULL");
    if (cameraId != ONLY_ONE_CAMERA_DEVICE - 1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "cameraId is wrong");
        return MEDIA_HAL_INVALID_PARAM;
    }
    MediaHalInitStaticMutexLock(&g_mutex);
    MediaHalMutexLock(g_mutex);
    if (g_hwCameraContext.devState != OPENED) {
        MEDIA_HAL_LOGE(MODULE_NAME, "wrong state");
        MediaHalMutexUnLock(g_mutex);
        return MEDIA_HAL_INVALID_PARAM;
    }
    g_hwCameraContext.bufferAvailbleCallback = callback;
    g_hwCameraContext.userData = (HwCameraVoidPtr)userData;
    MediaHalMutexUnLock(g_mutex);
    return MEDIA_HAL_OK;
}

int32_t HwCameraStreamOn(uint32_t cameraId, uint32_t streamId)
{
    MediaHalInitStaticMutexLock(&g_mutex);
    MediaHalMutexLock(g_mutex);
    if (cameraId != ONLY_ONE_CAMERA_DEVICE - 1 || streamId != ONLY_ONE_CAMERA_DEVICE - 1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "cameraId or streamId is wrong");
        MediaHalMutexUnLock(g_mutex);
        return MEDIA_HAL_INVALID_PARAM;
    }
    if (g_hwCameraContext.devState != OPENED ||
        g_hwCameraContext.bufferAvailbleCallback == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "wrong state or callback is NULL");
        MediaHalMutexUnLock(g_mutex);
        return MEDIA_HAL_INVALID_PARAM;
    }

    int32_t ret = MEDIA_HAL_OK;
    MediaHalThreadattr attr = { "virtualCameraProcess", 0x4000, THREAD_SCHED_INVALID, 0, false };
    g_hwCameraContext.callbackThread = MediaHalThreadCreate(AsyncProcess, &g_hwCameraContext, &attr);
    if (g_hwCameraContext.callbackThread == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "thread create failed");
        ret = MEDIA_HAL_ERR;
    }

    MediaHalMutexUnLock(g_mutex);
    return ret;
}

int32_t HwCameraStreamOff(uint32_t cameraId, uint32_t streamId)
{
    if (cameraId != ONLY_ONE_CAMERA_DEVICE - 1 ||
        streamId != ONLY_ONE_CAMERA_DEVICE - 1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "cameraId or streamId is wrong");
        return MEDIA_HAL_INVALID_PARAM;
    }
    MediaHalInitStaticMutexLock(&g_mutex);
    MediaHalMutexLock(g_mutex);
    g_hwCameraContext.devState = CLOSED;
    MediaHalThreadJoin(&g_hwCameraContext.callbackThread);
    MediaHalMutexUnLock(g_mutex);
    return MEDIA_HAL_OK;
}

int32_t HwCameraQueueBuf(uint32_t cameraId, uint32_t streamId, HalBuffer *buffer, uint8_t num)
{
    if (cameraId != ONLY_ONE_CAMERA_DEVICE - 1 ||
        streamId != ONLY_ONE_CAMERA_DEVICE - 1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "cameraId[%d] or streamId[%d] is wrong", cameraId, streamId);
        return MEDIA_HAL_INVALID_PARAM;
    }
    for (int32_t i = 0; i < num; i++) {
        if (!PushElement(&g_hwCameraContext.freeQueue, &(buffer[i]))) {
            return MEDIA_HAL_ERR;
        }
    }

    return MEDIA_HAL_OK;
}

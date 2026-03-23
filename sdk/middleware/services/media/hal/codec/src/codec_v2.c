/*
 * Copyright (c) @CompanyNameMagicTag. 2020-2021. All rights reserved.
 * Description: codec module
 * Author: Media Software Group
 * Create: 2020-6-20
 */

#include <errno.h>
#include <limits.h>
#include <unistd.h>
#ifdef SUPPORT_SYS_PRCTL
#include <sys/prctl.h>
#endif
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include "buffer_channel.h"
#include "codec_interface.h"
#include "codec_config.h"
#include "codec_plugin_manager.h"
#include "media_hal_common.h"
#include "media_hal_thread_adapt.h"
#include "securec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */
#define PARAM_UPPER_LIMIT 500
#define SLEEP_US 30000 // 30ms
#define TIME_OUT_US 3000000 // 3s
#define US_TO_NSEC 1000
#define PATH_MAX_LEN 1025

#define MODULE_NAME "CODEC_INTERFACE"
#define MAX_HANDLE_INSTANCE_NUM 10

typedef struct {
    void *codecHandle;
    CodecType type;
    int instanceNum;
    AvCodecMime mime;
    bool isStarted;
    uint32_t frameIndex;
    bool isBindedMode;
    CodecCallback callback;
    uintptr_t callbackInstance;
    MediaHalThreadIdHandle callbackThread;
    bool asyncInputMode;
    bool asyncOutputMode;
    bool isOutputThreadRunning;
    MediaHalMutexHandle ctxMutex;
    MediaHalThreadCondHandle ctxCond;
} CodecCtx;

typedef struct {
    CODEC_HANDLETYPE handle;
    bool isValid;
} HandleRecord;

typedef struct {
    char *dirPath;
    char *getImplSymbol;
    CodecType type;
} PluginConfig;

static const PluginConfig g_plugCfgs[] = {
    {"/usr/lib/", "VDecoderGetImpl", VIDEO_DECODER},
    {"/usr/lib/", "ADecoderGetImpl", AUDIO_DECODER},
    {"/usr/lib/", "AEncoderGetImpl", AUDIO_ENCODER},
    {"/usr/lib/", "VEncoderGetImpl", VIDEO_ENCODER},
};

#define NO_WAIT  0

static const CodecCapbility capEntries[] = {
    {
        MEDIA_MIMETYPE_VIDEO_AVC, VIDEO_DECODER, {ALGIN_LEVEL_2, ALGIN_LEVEL_2}, {128, 128}, {1920, 1080},
        VID_BITRATE_LEVEL1, VID_BITRATE_LEVEL2, {{AAC_MAIN_PROFILE}, 1}, {{AVC_LEVEL_1}, 1},
        {{YVU_SEMIPLANAR_420}, 1}, 1, 1, ALLOCATE_INPUT_BUFFER_USER | ALLOCATE_OUTPUT_BUFFER_CODEC, 0
    },

    {
        MEDIA_MIMETYPE_VIDEO_HEVC, VIDEO_DECODER, {ALGIN_LEVEL_2, ALGIN_LEVEL_2}, {128, 128}, {1920, 1080},
        VID_BITRATE_LEVEL1, VID_BITRATE_LEVEL2, {{HEVC_MAIN_PROFILE}, 1}, {{HEVC_LEVEL_MAIN_2}, 1},
        {{YVU_SEMIPLANAR_420}, 1}, 1, 1, ALLOCATE_INPUT_BUFFER_USER | ALLOCATE_OUTPUT_BUFFER_CODEC, 0
    },

    {
        MEDIA_MIMETYPE_IMAGE_JPEG, VIDEO_DECODER, {ALGIN_LEVEL_2, ALGIN_LEVEL_2}, {128, 128}, {1920, 1080},
        VID_BITRATE_LEVEL1, VID_BITRATE_LEVEL2, {{INVALID_PROFILE}, 1}, {{INVALID_LEVEL}, 1},
        {{YVU_SEMIPLANAR_420}, 1}, 1, 1, ALLOCATE_INPUT_BUFFER_USER | ALLOCATE_OUTPUT_BUFFER_CODEC, 0
    },

    {
        MEDIA_MIMETYPE_AUDIO_AAC, AUDIO_DECODER, {ALGIN_LEVEL_INVALID, ALGIN_LEVEL_INVALID}, {0, 0}, {0, 0},
        VID_BITRATE_LEVEL1, AUD_BITRATE_LEVEL1, {{AAC_MAIN_PROFILE}, 1}, {{INVALID_LEVEL}, 1},
        {{YVU_SEMIPLANAR_420}, 1}, 1, 1, ALLOCATE_INPUT_BUFFER_USER | ALLOCATE_OUTPUT_BUFFER_CODEC, 0
    },

    {
        MEDIA_MIMETYPE_AUDIO_G711A, AUDIO_DECODER, {ALGIN_LEVEL_INVALID, ALGIN_LEVEL_INVALID}, {0, 0}, {0, 0},
        VID_BITRATE_LEVEL1, AUD_BITRATE_LEVEL1, {{INVALID_PROFILE}, 1}, {{INVALID_LEVEL}, 1},
        {{PIX_FORMAT_INVALID}, 1}, 1, 1, ALLOCATE_INPUT_BUFFER_USER | ALLOCATE_OUTPUT_BUFFER_CODEC, 0
    },

    {
        MEDIA_MIMETYPE_AUDIO_G711U, AUDIO_DECODER, {ALGIN_LEVEL_INVALID, ALGIN_LEVEL_INVALID}, {0, 0}, {0, 0},
        VID_BITRATE_LEVEL1, AUD_BITRATE_LEVEL1, {{INVALID_PROFILE}, 1}, {{INVALID_LEVEL}, 1},
        {{PIX_FORMAT_INVALID}, 1}, 1, 1, ALLOCATE_INPUT_BUFFER_USER | ALLOCATE_OUTPUT_BUFFER_CODEC, 0
    },

    {
        MEDIA_MIMETYPE_AUDIO_G726, AUDIO_DECODER, {ALGIN_LEVEL_INVALID, ALGIN_LEVEL_INVALID}, {0, 0}, {0, 0},
        VID_BITRATE_LEVEL1, AUD_BITRATE_LEVEL1, {{INVALID_PROFILE}, 1}, {{INVALID_LEVEL}, 1},
        {{PIX_FORMAT_INVALID}, 1}, 1, 1, ALLOCATE_INPUT_BUFFER_USER | ALLOCATE_OUTPUT_BUFFER_CODEC, 0
    }
};

static bool g_inited = false;
static MediaHalMutexHandle g_mutex = NULL;
static HandleRecord g_handlesRecord[MAX_HANDLE_INSTANCE_NUM];

static struct timeval g_timeInitStart = { };
static struct timeval g_timeInitEnd = { };
static struct timeval g_timeDeinitStart = { };
static struct timeval g_timeDeinitEnd = { };

static void InitHandleRecord(void)
{
    MediaHalInitStaticMutexLock(&g_mutex);
    MediaHalMutexLock(g_mutex);
    for (int i = 0; i < MAX_HANDLE_INSTANCE_NUM; i++) {
        g_handlesRecord[i].handle = NULL;
        g_handlesRecord[i].isValid = false;
    }
    MediaHalMutexUnLock(g_mutex);
}

static bool RecordThisHandle(CODEC_HANDLETYPE handle)
{
    bool hasSpace = false;
    MediaHalInitStaticMutexLock(&g_mutex);
    MediaHalMutexLock(g_mutex);
    for (int i = 0; i < MAX_HANDLE_INSTANCE_NUM; i++) {
        if (!g_handlesRecord[i].isValid) {
            hasSpace = true;
            g_handlesRecord[i].handle = handle;
            g_handlesRecord[i].isValid = true;
            break;
        }
    }
    MediaHalMutexUnLock(g_mutex);
    if (!hasSpace) {
        MEDIA_HAL_LOGW(MODULE_NAME, "no space to record handle");
        return false;
    }
    return true;
}

static bool CheckValidHandle(const CODEC_HANDLETYPE handle)
{
    if (handle == NULL) {
        return false;
    }
    MediaHalInitStaticMutexLock(&g_mutex);
    MediaHalMutexLock(g_mutex);
    for (int i = 0; i < MAX_HANDLE_INSTANCE_NUM; i++) {
        if (g_handlesRecord[i].isValid && g_handlesRecord[i].handle == handle) {
            MediaHalMutexUnLock(g_mutex);
            return true;
        }
    }
    MediaHalMutexUnLock(g_mutex);
    return false;
}

static void DeleteHandle(const CODEC_HANDLETYPE handle)
{
    MediaHalInitStaticMutexLock(&g_mutex);
    MediaHalMutexLock(g_mutex);
    for (int i = 0; i < MAX_HANDLE_INSTANCE_NUM; i++) {
        if (g_handlesRecord[i].isValid && g_handlesRecord[i].handle == handle) {
            g_handlesRecord[i].isValid = false;
            g_handlesRecord[i].handle = NULL;
        }
    }
    MediaHalMutexUnLock(g_mutex);
}

int32_t CodecInit(void)
{
    MEDIA_HAL_LOGI(MODULE_NAME, "in");
    MediaHalInitStaticMutexLock(&g_mutex);
    MediaHalMutexLock(g_mutex);
    if (g_inited) {
        MediaHalMutexUnLock(g_mutex);
        return MEDIA_HAL_OK;
    }
    int ret = MEDIA_HAL_ERR;
    gettimeofday(&g_timeInitStart, NULL);
    size_t cfgSize = sizeof(g_plugCfgs) / sizeof(g_plugCfgs[0]);
    for (size_t i = 0; i < cfgSize; i++) {
        ret = SearchLoadValidPlugin(g_plugCfgs[i].dirPath, g_plugCfgs[i].getImplSymbol, g_plugCfgs[i].type);
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "search and load plugin type[%d] failed", g_plugCfgs[i].type);
            MediaHalMutexUnLock(g_mutex);
            return MEDIA_HAL_ERR;
        }
    }
    gettimeofday(&g_timeInitEnd, NULL);
    CalDiffTimeBetween(g_timeInitStart, g_timeInitEnd, MODULE_NAME, "Codec init", true);
    g_inited = true;
    MediaHalMutexUnLock(g_mutex);
    InitHandleRecord();
    MEDIA_HAL_LOGI(MODULE_NAME, "out");
    return MEDIA_HAL_OK;
}

int32_t CodecDeinit(void)
{
    MEDIA_HAL_LOGI(MODULE_NAME, "in");
    MediaHalInitStaticMutexLock(&g_mutex);
    MediaHalMutexLock(g_mutex);
    gettimeofday(&g_timeDeinitStart, NULL);
    if (g_inited) {
        size_t cfgSize = sizeof(g_plugCfgs) / sizeof(g_plugCfgs[0]);
        for (size_t i = 0; i < cfgSize; i++) {
            int ret = UnLoadPlugin(g_plugCfgs[i].type);
            if (ret != MEDIA_HAL_OK) {
                MEDIA_HAL_LOGE(MODULE_NAME, "unload plugin type[%d] failed", g_plugCfgs[i].type);
                MediaHalMutexUnLock(g_mutex);
                return MEDIA_HAL_ERR;
            }
        }
        g_inited = false;
    }
    gettimeofday(&g_timeDeinitEnd, NULL);
    CalDiffTimeBetween(g_timeDeinitStart, g_timeDeinitEnd, MODULE_NAME, "Codec deinit", true);
    MediaHalMutexUnLock(g_mutex);
    MEDIA_HAL_LOGI(MODULE_NAME, "out");
    return MEDIA_HAL_OK;
}

int32_t CodecEnumerateCapbility(uint32_t index, CodecCapbility *cap)
{
    MEDIA_HAL_UNUSED(index);
    MEDIA_HAL_UNUSED(cap);

    MEDIA_HAL_LOGE(MODULE_NAME, "not support now");
    return MEDIA_HAL_ERR;
}

int32_t CodecGetCapbility(AvCodecMime mime, CodecType type, uint32_t flags, CodecCapbility *cap)
{
    MEDIA_HAL_UNUSED(flags);
    if (cap == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "cap ptr is null");
        return MEDIA_HAL_ERR;
    }

    size_t size = sizeof(capEntries) / sizeof(capEntries[0]);
    for (size_t i = 0; i < size; i++) {
        bool match = capEntries[i].mime == mime && capEntries[i].type == type;
        if (match) {
            *cap = capEntries[i];
            return MEDIA_HAL_OK;
        }
    }
    return MEDIA_HAL_ERR;
}

static int32_t FindDomainKind(const Param *attr, int len, CodecType *type)
{
    bool isFound = false;
    for (int i = 0; i < len; i++) {
        if (attr[i].key == KEY_CODEC_TYPE) {
            if (attr[i].size != sizeof(CodecType)) {
                MEDIA_HAL_LOGE(MODULE_NAME, "param kParamIndexDomainKind size wrong");
                return MEDIA_HAL_ERR;
            }
            *type = *((CodecType *)(attr[i].val));
            isFound = true;
            break;
        }
    }
    if (!isFound) {
        return MEDIA_HAL_ERR;
    }
    return MEDIA_HAL_OK;
}

static int32_t WhichType(const Param *attr, int len, CodecType *type)
{
    int32_t ret = FindDomainKind(attr, len, type);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "param not find domian or kind");
        return MEDIA_HAL_ERR;
    }
    return MEDIA_HAL_OK;
}

static int32_t CreateHandler(const Param *attr, int len, CodecType type, CodecCtx *ctx)
{
    void *handle = NULL;
    MEDIA_HAL_LOGD(MODULE_NAME, "codec type[%s]", GetCodecTypeName(type));
    int32_t ret = CodecChannelCreate(&handle, type, attr, len);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "type[%d] channel create failed ret:0X%X", type, ret);
        return MEDIA_HAL_ERR;
    }
    ctx->codecHandle = handle;
    ctx->type = type;
    ctx->instanceNum = 0;
    ctx->isBindedMode = false;
    ctx->isStarted = false;
    if (memset_s(&(ctx->callback), sizeof(CodecCallback), 0x00, sizeof(CodecCallback)) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "memset_s failed");
    }
    ctx->asyncInputMode = false;
    ctx->asyncOutputMode = false;
    MediaHalMutexAttr mutexAttr = { false };
    ctx->ctxMutex = MediaHalMutexCreate(&mutexAttr);
    if (ctx->ctxMutex == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "create codec context mutex failed");
        (void)CodecChannelDestroy((CodecBufferChannel *)handle, type);
        return MEDIA_HAL_ERR;
    }
    ctx->ctxCond = MediaHalThreadCondCreate();
    if (ctx->ctxCond == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "create codec context cond failed");
        MediaHalMutexDestroy(&ctx->ctxMutex);
        (void)CodecChannelDestroy((CodecBufferChannel *)handle, type);
        return MEDIA_HAL_ERR;
    }
    return MEDIA_HAL_OK;
}

int32_t CodecCreate(const char* name, const Param *attr, int len, CODEC_HANDLETYPE *handle)
{
    MEDIA_HAL_LOGD(MODULE_NAME, "in");
    if (attr == NULL || handle == NULL || name == NULL || len > PARAM_UPPER_LIMIT) {
        MEDIA_HAL_LOGE(MODULE_NAME, "param is invalid");
        return MEDIA_HAL_ERR;
    }

    CodecType type = INVALID_TYPE;
    int32_t ret = WhichType(attr, len, &type);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "codec type invalid");
        return MEDIA_HAL_ERR;
    }

    CodecCtx *codecCtx = (CodecCtx *)malloc(sizeof(CodecCtx));
    if (codecCtx == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "malloc context error");
        return MEDIA_HAL_ERR;
    }
    if (!RecordThisHandle(codecCtx)) {
        free(codecCtx);
        return MEDIA_HAL_ERR;
    }
    if (memset_s(codecCtx, sizeof(CodecCtx), 0x00, sizeof(CodecCtx)) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "memset_s failed");
    }
    MediaHalInitStaticMutexLock(&g_mutex);
    MediaHalMutexLock(g_mutex);
    ret = CreateHandler(attr, len, type, codecCtx);
    MediaHalMutexUnLock(g_mutex);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "CreateHandler fail");
        DeleteHandle(codecCtx);
        free(codecCtx);
        return MEDIA_HAL_ERR;
    }

    *handle = codecCtx;
    MEDIA_HAL_LOGD(MODULE_NAME, "out");
    return MEDIA_HAL_OK;
}

static void DestoryHandler(CodecCtx *ctx)
{
    CodecBufferChannel *channelCtx = (CodecBufferChannel *)ctx->codecHandle;
    int32_t ret = CodecChannelDestroy(channelCtx, ctx->type);
    ctx->codecHandle = NULL;
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "channel destroy failed");
    }
}

int32_t CodecDestroy(CODEC_HANDLETYPE handle)
{
    if (!CheckValidHandle(handle)) {
        MEDIA_HAL_LOGE(MODULE_NAME, "handle is not valid");
        return MEDIA_HAL_ERR;
    }
    MediaHalInitStaticMutexLock(&g_mutex);
    MediaHalMutexLock(g_mutex);
    CodecCtx *codecCtx = (CodecCtx *)handle;
    DestoryHandler(codecCtx);
    MediaHalMutexUnLock(g_mutex);
    DeleteHandle(handle);
    MediaHalMutexDestroy(&codecCtx->ctxMutex);
    MediaHalThreadCondDestroy(&codecCtx->ctxCond);
    free(codecCtx);
    return MEDIA_HAL_OK;
}

int32_t CodecSetPortMode(CODEC_HANDLETYPE handle, DirectionType type, BufferMode mode)
{
    MEDIA_HAL_UNUSED(type);
    MEDIA_HAL_UNUSED(mode);
    if (!CheckValidHandle(handle)) {
        MEDIA_HAL_LOGE(MODULE_NAME, "handle is not valid");
        return MEDIA_HAL_ERR;
    }

    return MEDIA_HAL_OK;
}

int32_t CodecSetParameter(CODEC_HANDLETYPE handle, const Param *params, int paramCnt)
{
    if (!CheckValidHandle(handle) || params == NULL || paramCnt > PARAM_UPPER_LIMIT) {
        MEDIA_HAL_LOGE(MODULE_NAME, "handle or params is not valid");
        return MEDIA_HAL_ERR;
    }

    CodecCtx *ctx = (CodecCtx *)handle;
    MediaHalMutexLock(ctx->ctxMutex);
    CodecBufferChannel *channelCtx = (CodecBufferChannel *)ctx->codecHandle;
    if (channelCtx == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "CodecBufferChannel NULL");
        MediaHalMutexUnLock(ctx->ctxMutex);
        return MEDIA_HAL_INVALID_PARAM;
    }

    if (channelCtx->channelCreated != true) {
        MEDIA_HAL_LOGE(MODULE_NAME, "Channel not created yet");
        MediaHalMutexUnLock(ctx->ctxMutex);
        return MEDIA_HAL_ERR;
    }

    int32_t ret = MEDIA_HAL_OK;
    for (int i = 0; i < paramCnt; i++) {
        switch (params[i].key) {
            case KEY_DEVICE_ID: {
                ret = SetDevice(channelCtx, ctx->type, INPUT_TYPE, &params[i]);
                break;
            }
            case KEY_IMAGE_Q_FACTOR: {
                ret = CodecVencSetParameter(channelCtx, ctx->type, &params[i]);
                break;
            }
            default: {
                MEDIA_HAL_LOGE(MODULE_NAME, "CodecSetParameter invalid param key");
                break;
            }
        }
    }
    MediaHalMutexUnLock(ctx->ctxMutex);
    return ret;
}

int32_t CodecGetParameter(CODEC_HANDLETYPE handle, Param *params, int paramCnt)
{
    MEDIA_HAL_UNUSED(paramCnt);
    if (!CheckValidHandle(handle)) {
        MEDIA_HAL_LOGE(MODULE_NAME, "handle is not valid");
        return MEDIA_HAL_ERR;
    }
    if (params == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "param is NULL");
        return MEDIA_HAL_ERR;
    }

    return MEDIA_HAL_OK;
}

static void* AsyncOutputProcess(void *arg)
{
    if (arg == NULL) {
        return NULL;
    }
    CodecCtx *codecCtx = (CodecCtx *)arg;
    MEDIA_HAL_LOGD(MODULE_NAME, "async process isOutputThreadRunning:%d", codecCtx->isOutputThreadRunning);
#ifdef SUPPORT_SYS_PRCTL
    prctl(PR_SET_NAME, "AsyncOutputProcess", 0, 0, 0);
#endif
    while (codecCtx->isOutputThreadRunning) {
        OutputInfo outInfo = {};
        int32_t ret = CodecChannelDequeueOutput(codecCtx->codecHandle, codecCtx->type, &outInfo, 0);
        if (ret != MEDIA_HAL_OK) {
            if (ret == CHANNEL_ERR_CHANNEL_CACHED) {
                MEDIA_HAL_LOGE(MODULE_NAME, "previous frame not released, wait a while");
                usleep(SLEEP_US);
            } else {
                MEDIA_HAL_LOGE(MODULE_NAME, "async process deq out fail");
            }
            continue;
        }

        if (codecCtx->callback.OutputBufferAvailable != NULL) {
            codecCtx->callback.OutputBufferAvailable((uintptr_t)codecCtx, codecCtx->callbackInstance, &outInfo);
        }
    }
    MediaHalThreadCondSignal(codecCtx->ctxCond);
    MEDIA_HAL_LOGD(MODULE_NAME, "out thread out");
    return NULL;
}

static int32_t StartCodec(CodecCtx *ctx)
{
    CodecBufferChannel *channelCtx = (CodecBufferChannel *)ctx->codecHandle;
    int32_t ret = CodecChannelStart(channelCtx, ctx->type);
    if (ret != MEDIA_HAL_OK) {
        return MEDIA_HAL_ERR;
    }

    if (ctx->asyncOutputMode) {
        ctx->isOutputThreadRunning = true;
        MediaHalThreadattr attr = { "CodecAsyncProcess", 0x1000, THREAD_SCHED_INVALID, 0, false };
        ctx->callbackThread = MediaHalThreadCreate(AsyncOutputProcess, ctx, &attr);
        if (ctx->callbackThread == NULL) {
            MEDIA_HAL_LOGE(MODULE_NAME, "create thread failed");
            ctx->isOutputThreadRunning = false;
            return MEDIA_HAL_ERR;
        }
    }
    ctx->isStarted = true;
    ctx->frameIndex = 0;
    return MEDIA_HAL_OK;
}

int32_t CodecStart(CODEC_HANDLETYPE handle)
{
    if (!CheckValidHandle(handle)) {
        MEDIA_HAL_LOGE(MODULE_NAME, "handle is not valid");
        return MEDIA_HAL_ERR;
    }
    CodecCtx *codecCtx = (CodecCtx *)handle;
    MediaHalMutexLock(codecCtx->ctxMutex);
    if (codecCtx->isStarted) {
        MediaHalMutexUnLock(codecCtx->ctxMutex);
        return MEDIA_HAL_OK;
    }
    int32_t ret = StartCodec(codecCtx);
    MediaHalMutexUnLock(codecCtx->ctxMutex);
    if (ret != MEDIA_HAL_OK) {
        return MEDIA_HAL_ERR;
    }
    MEDIA_HAL_LOGD(MODULE_NAME, "CodecStart success");
    return MEDIA_HAL_OK;
}

static int32_t StopCodec(CodecCtx *ctx)
{
    CodecBufferChannel *channelCtx = (CodecBufferChannel *)ctx->codecHandle;
    int32_t ret = CodecChannelStop(channelCtx, ctx->type);
    if (ret != MEDIA_HAL_OK) {
        return MEDIA_HAL_ERR;
    }
    ctx->isStarted = false;
    return MEDIA_HAL_OK;
}

int32_t CodecStop(CODEC_HANDLETYPE handle)
{
    if (!CheckValidHandle(handle)) {
        MEDIA_HAL_LOGE(MODULE_NAME, "handle is not valid");
        return MEDIA_HAL_ERR;
    }

    CodecCtx *codecCtx = (CodecCtx *)handle;
    MediaHalMutexLock(codecCtx->ctxMutex);
    int32_t ret;
    if (codecCtx->asyncOutputMode && codecCtx->isOutputThreadRunning) {
        codecCtx->isOutputThreadRunning = false;
        MediaHalThreadCondTimeWait(codecCtx->ctxCond, codecCtx->ctxMutex, TIME_OUT_US);
        MediaHalThreadJoin(&codecCtx->callbackThread);
    }
    ret = StopCodec(codecCtx);
    MediaHalMutexUnLock(codecCtx->ctxMutex);
    if (ret != MEDIA_HAL_OK) {
        return MEDIA_HAL_ERR;
    }
    return MEDIA_HAL_OK;
}

int32_t CodecFlush(CODEC_HANDLETYPE handle, DirectionType directType)
{
    MEDIA_HAL_UNUSED(directType);
    if (!CheckValidHandle(handle)) {
        MEDIA_HAL_LOGE(MODULE_NAME, "handle is not valid");
        return MEDIA_HAL_ERR;
    }

    CodecCtx *codecCtx = (CodecCtx *)handle;
    MediaHalMutexLock(codecCtx->ctxMutex);
    CodecBufferChannel *channelCtx = (CodecBufferChannel *)codecCtx->codecHandle;
    int32_t ret = CodecChannelFlush(channelCtx, codecCtx->type);
    MediaHalMutexUnLock(codecCtx->ctxMutex);
    if (ret != MEDIA_HAL_OK) {
        return MEDIA_HAL_ERR;
    }
    return MEDIA_HAL_OK;
}

int32_t CodecQueueInput(CODEC_HANDLETYPE handle, const InputInfo *inputData, uint32_t timeoutMs)
{
    if (!CheckValidHandle(handle)) {
        MEDIA_HAL_LOGE(MODULE_NAME, "handle is not valid");
        return MEDIA_HAL_ERR;
    }

    CodecCtx *codecCtx = (CodecCtx *)handle;
    MediaHalMutexLock(codecCtx->ctxMutex);
    if (!codecCtx->isStarted) {
        MEDIA_HAL_LOGE(MODULE_NAME, "not start");
        MediaHalMutexUnLock(codecCtx->ctxMutex);
        return MEDIA_HAL_ERR;
    }

    CodecBufferChannel *channelCtx = (CodecBufferChannel *)codecCtx->codecHandle;
    int32_t ret = CodecChannelQueueInput(channelCtx, codecCtx->type, inputData, timeoutMs);
    MediaHalMutexUnLock(codecCtx->ctxMutex);
    return ret;
}

int32_t CodecDequeInput(CODEC_HANDLETYPE handle, uint32_t timeoutMs, InputInfo *inputData)
{
    MEDIA_HAL_UNUSED(timeoutMs);
    if (!CheckValidHandle(handle)) {
        MEDIA_HAL_LOGE(MODULE_NAME, "handle is not valid");
        return MEDIA_HAL_ERR;
    }
    if (inputData == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "param is NULL");
        return MEDIA_HAL_ERR;
    }
    return MEDIA_HAL_OK;
}

int32_t CodecQueueOutput(CODEC_HANDLETYPE handle, OutputInfo *outInfo, uint32_t timeoutMs, int releaseFenceFd)
{
    MEDIA_HAL_UNUSED(releaseFenceFd);
    if (!CheckValidHandle(handle)) {
        MEDIA_HAL_LOGE(MODULE_NAME, "handle is not valid");
        return MEDIA_HAL_ERR;
    }

    CodecCtx *codecCtx = (CodecCtx *)handle;
    MediaHalMutexLock(codecCtx->ctxMutex);
    CodecBufferChannel *channelCtx = (CodecBufferChannel *)codecCtx->codecHandle;
    int32_t ret = CodecChannelQueueOutput(channelCtx, codecCtx->type, outInfo, timeoutMs);
    MediaHalMutexUnLock(codecCtx->ctxMutex);
    return ret;
}

int32_t CodecDequeueOutput(CODEC_HANDLETYPE handle, uint32_t timeoutMs, int *acquireFd, OutputInfo *outInfo)
{
    MEDIA_HAL_UNUSED(acquireFd);
    if (!CheckValidHandle(handle)) {
        MEDIA_HAL_LOGE(MODULE_NAME, "handle is not valid");
        return MEDIA_HAL_ERR;
    }
    CodecCtx *codecCtx = (CodecCtx *)handle;
    MediaHalMutexLock(codecCtx->ctxMutex);
    CodecBufferChannel *channelCtx = (CodecBufferChannel *)codecCtx->codecHandle;
    int32_t ret = CodecChannelDequeueOutput(channelCtx, codecCtx->type, outInfo, timeoutMs);
    MediaHalMutexUnLock(codecCtx->ctxMutex);
    return ret;
}

int32_t CodecSetCallback(CODEC_HANDLETYPE handle, const CodecCallback *cb, uintptr_t instance)
{
    if (!CheckValidHandle(handle)) {
        MEDIA_HAL_LOGE(MODULE_NAME, "handle is not valid");
        return MEDIA_HAL_ERR;
    }
    if (cb == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "callback is NULL");
        return MEDIA_HAL_ERR;
    }
    CodecCtx *codecCtx = (CodecCtx *)handle;
    MediaHalMutexLock(codecCtx->ctxMutex);
    codecCtx->callback = *cb;
    codecCtx->callbackInstance = instance;
    codecCtx->asyncInputMode = (codecCtx->callback.InputBufferAvailable != NULL) ? true : false;
    codecCtx->asyncOutputMode = (codecCtx->callback.OutputBufferAvailable != NULL) ? true : false;
    MediaHalMutexUnLock(codecCtx->ctxMutex);
    return MEDIA_HAL_OK;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

/*
 * Copyright (c) @CompanyNameMagicTag. 2020-2020. All rights reserved.
 * Description: codec module
 * Author: Media Software Group
 * Create: 2020-08-11
 */

#include "buffer_channel.h"
#include <limits.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <errno.h>
#include "codec_plugin_manager.h"
#include "media_hal_common.h"
#include "media_hal_thread_adapt.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define MODULE_NAME "CODEC_BUFFER_CHANNEL"

int32_t CodecChannelCreate(void **channelCtx, CodecType type, const Param *attr, int len)
{
    CHK_NULL_RETURN(channelCtx, MEDIA_HAL_INVALID_PARAM, "create buffer channel context is invalid");

    AvCodecMime mime = GetCodecMime(attr, len);
    if (mime == MEDIA_MIMETYPE_INVALID) {
        return MEDIA_HAL_ERR;
    }

    void *pluginEntry = FindSpecificPlugin(mime, true, type);
    if (pluginEntry == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "could not find correspond plugin for type: %d\n", mime);
        return MEDIA_HAL_ERR;
    }

    CodecBufferChannel *chanCtx = (CodecBufferChannel *)malloc(sizeof(CodecBufferChannel));
    if (chanCtx == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "malloc CodecBufferChannel failed\n");
        return MEDIA_HAL_ERR;
    }
    if (memset_s(chanCtx, sizeof(CodecBufferChannel), 0x00, sizeof(CodecBufferChannel)) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "memset_s failed");
    }
    chanCtx->mime = mime;

    MediaHalMutexAttr mutexAttr = { false };
    chanCtx->channelLock = MediaHalMutexCreate(&mutexAttr);
    if (chanCtx->channelLock == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "create codec channel mutex failed");
        free(chanCtx);
        return MEDIA_HAL_ERR;
    }

    chanCtx->pluginCommon = (CodecPluginCommon *)pluginEntry;
    if (chanCtx->pluginCommon->pfnCreate == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "create fun ptr is NULL\n");
        MediaHalMutexDestroy(&chanCtx->channelLock);
        free(chanCtx);
        return MEDIA_HAL_INVALID_PARAM;
    }
    int32_t ret = chanCtx->pluginCommon->pfnCreate(&chanCtx->pluginHandle, attr, len);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "channel create failed ret:0X%X", ret);
        MediaHalMutexDestroy(&chanCtx->channelLock);
        free(chanCtx);
        return ret;
    }

    chanCtx->channelCreated = true;
    chanCtx->channelStarted = false;
    chanCtx->cached = false;
    chanCtx->isBindedMode = false;
    *channelCtx = (void *)chanCtx;
    return MEDIA_HAL_OK;
}

int32_t CodecChannelDestroy(CodecBufferChannel *channelCtx, CodecType type)
{
    MEDIA_HAL_UNUSED(type);
    CHK_NULL_RETURN(channelCtx, MEDIA_HAL_INVALID_PARAM, "CodecBufferChannel context is null");
    CHK_NULL_RETURN(channelCtx->pluginHandle, MEDIA_HAL_INVALID_PARAM, "pluginHandle is null");
    CHK_NULL_RETURN(channelCtx->pluginCommon, MEDIA_HAL_INVALID_PARAM, "pluginCommon is null");
    CHK_NULL_RETURN(channelCtx->pluginCommon->pfnDestroy, MEDIA_HAL_INVALID_PARAM,
        "destroy fun ptr is NULL");
    int32_t ret = channelCtx->pluginCommon->pfnDestroy(channelCtx->pluginHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "plugin destroy failed Ret:0X%X", ret);
    }
    MediaHalMutexDestroy(&channelCtx->channelLock);
    free(channelCtx);
    return ret;
}

int32_t CodecChannelStart(CodecBufferChannel *channelCtx, CodecType type)
{
    MEDIA_HAL_LOGD(MODULE_NAME, "start in type[%d]", type);
    CHK_NULL_RETURN(channelCtx, MEDIA_HAL_INVALID_PARAM, "CodecBufferChannel context is null");
    CHK_NULL_RETURN(channelCtx->pluginHandle, MEDIA_HAL_INVALID_PARAM, "pluginHandle is null");
    CHK_NULL_RETURN(channelCtx->pluginCommon, MEDIA_HAL_INVALID_PARAM, "pluginCommon is null");
    CHK_NULL_RETURN(channelCtx->pluginCommon->pfnStart, MEDIA_HAL_INVALID_PARAM, "start fun ptr is NULL");

    MediaHalMutexLock(channelCtx->channelLock);
    int32_t ret = channelCtx->pluginCommon->pfnStart(channelCtx->pluginHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "plugin start failed ret:0X%X type[%d]", ret, type);
        MediaHalMutexUnLock(channelCtx->channelLock);
        return ret;
    }
    channelCtx->channelStarted = true;
    MediaHalMutexUnLock(channelCtx->channelLock);
    MEDIA_HAL_LOGD(MODULE_NAME, "start out type[%d] common", type);
    return MEDIA_HAL_OK;
}

int32_t CodecChannelStop(CodecBufferChannel *channelCtx, CodecType type)
{
    MEDIA_HAL_LOGD(MODULE_NAME, "stop in type[%d]", type);
    CHK_NULL_RETURN(channelCtx, MEDIA_HAL_INVALID_PARAM, "CodecBufferChannel context is null");
    CHK_NULL_RETURN(channelCtx->pluginHandle, MEDIA_HAL_INVALID_PARAM, "pluginHandle is null");
    CHK_NULL_RETURN(channelCtx->pluginCommon, MEDIA_HAL_INVALID_PARAM, "pluginCommon is NULL");

    if (channelCtx->channelCreated == false) {
        MEDIA_HAL_LOGE(MODULE_NAME, "channel not created, type[%d]", type);
        return MEDIA_HAL_ERR;
    }
    if (channelCtx->channelStarted == false) {
        MEDIA_HAL_LOGE(MODULE_NAME, "channel has stopped, just return type[%d]", type);
        return MEDIA_HAL_OK;
    }

    CHK_NULL_RETURN(channelCtx->pluginCommon->pfnStop, MEDIA_HAL_INVALID_PARAM, "stop fun ptr is NULL");
    MediaHalMutexLock(channelCtx->channelLock);
    int32_t ret = channelCtx->pluginCommon->pfnStop(channelCtx->pluginHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "stop failed ret:0X%X type[%d]", ret, type);
        MediaHalMutexUnLock(channelCtx->channelLock);
        return ret;
    }
    channelCtx->channelStarted = false;
    MediaHalMutexUnLock(channelCtx->channelLock);
    MEDIA_HAL_LOGD(MODULE_NAME, "stop out type[%d]", type);
    return MEDIA_HAL_OK;
}

int32_t CodecChannelFlush(CodecBufferChannel *channelCtx, CodecType type)
{
    CHK_NULL_RETURN(channelCtx, MEDIA_HAL_INVALID_PARAM, "CodecBufferChannel context is null");
    CHK_NULL_RETURN(channelCtx->pluginHandle, MEDIA_HAL_INVALID_PARAM, "pluginHandle is null");
    CHK_NULL_RETURN(channelCtx->pluginCommon, MEDIA_HAL_INVALID_PARAM, "pluginCommon is NULL");
    CHK_NULL_RETURN(channelCtx->pluginCommon->pfnReset, MEDIA_HAL_INVALID_PARAM, "reset fun ptr is NULL");

    MediaHalMutexLock(channelCtx->channelLock);
    int32_t ret = channelCtx->pluginCommon->pfnReset(channelCtx->pluginHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "flush failed ret[0X%X] type[%d]", ret, type);
        MediaHalMutexUnLock(channelCtx->channelLock);
        return ret;
    }
    MediaHalMutexUnLock(channelCtx->channelLock);
    return MEDIA_HAL_OK;
}

int32_t CodecChannelQueueInput(CodecBufferChannel *channelCtx, CodecType type,
    const InputInfo *inputData, uint32_t timeoutMs)
{
    CHK_NULL_RETURN(channelCtx, MEDIA_HAL_INVALID_PARAM, "CodecBufferChannel context is null");
    CHK_NULL_RETURN(channelCtx->pluginHandle, MEDIA_HAL_INVALID_PARAM, "pluginHandle is null");
    CHK_NULL_RETURN(channelCtx->pluginCommon, MEDIA_HAL_INVALID_PARAM, "vdecPluginEntry is null");

    MediaHalMutexLock(channelCtx->channelLock);
    int32_t ret;
    switch (type) {
        case VIDEO_DECODER: {
            AvVideoDecoder *vdecPluginEntry = (AvVideoDecoder *)channelCtx->pluginCommon;
            ret = vdecPluginEntry->pfnQueueInput(channelCtx->pluginHandle, inputData, timeoutMs);
            if (ret == VDEC_ERR_STREAM_BUF_FULL) {
                ret = CODEC_ERR_STREAM_BUF_FULL;
                goto FAILED;
            } else if (ret != MEDIA_HAL_OK) {
                ret = CODEC_ERR_INVALID_OP;
                goto FAILED;
            }
            break;
        }
        case AUDIO_DECODER: {
            AvAudioDecoder *adecPluginEntry = (AvAudioDecoder *)channelCtx->pluginCommon;
            ret = adecPluginEntry->pfnQueueInput(channelCtx->pluginHandle, inputData,
                                                 channelCtx->frameIndex++, false);
            if (ret == ADEC_ERR_STREAM_BUF_FULL) {
                ret = CODEC_ERR_STREAM_BUF_FULL;
                goto FAILED;
            } else if (ret != MEDIA_HAL_OK) {
                ret = CODEC_ERR_INVALID_OP;
                goto FAILED;
            }
            break;
        }
        case VIDEO_ENCODER:
        case AUDIO_ENCODER:
            break;
        default:
            MEDIA_HAL_LOGE(MODULE_NAME, "invalid type");
            break;
    }
    MediaHalMutexUnLock(channelCtx->channelLock);
    return MEDIA_HAL_OK;

FAILED:
    if (ret == CODEC_ERR_STREAM_BUF_FULL) {
        MEDIA_HAL_LOGD(MODULE_NAME, "send input but full wait a while, type[%d]", type);
    } else {
        MEDIA_HAL_LOGE(MODULE_NAME, "send input failed ret[0X%X] type[%d]", ret, type);
    }
    MediaHalMutexUnLock(channelCtx->channelLock);
    return ret;
}

// 1. out buffer allocated by surface no need cache at first
// 2. out buffer allocated by codec need cache at first
int32_t CodecChannelQueueOutput(CodecBufferChannel *channelCtx, CodecType type,
    OutputInfo *outInfo, uint32_t timeoutMs)
{
    MEDIA_HAL_UNUSED(timeoutMs);
    CHK_NULL_RETURN(channelCtx, MEDIA_HAL_INVALID_PARAM, "CodecBufferChannel context is null");
    CHK_NULL_RETURN(channelCtx->pluginHandle, MEDIA_HAL_INVALID_PARAM, "pluginHandle is null");
    CHK_NULL_RETURN(channelCtx->pluginCommon, MEDIA_HAL_INVALID_PARAM, "pluginCommon is null");
    CHK_NULL_RETURN(outInfo, MEDIA_HAL_INVALID_PARAM, "OutputInfo is null");

    if (channelCtx->cached) {
        return MEDIA_HAL_OK;
    }

    MediaHalMutexLock(channelCtx->channelLock);
    int32_t ret;
    switch (type) {
        case VIDEO_DECODER: {
            AvVideoDecoder *vdecPluginEntry = (AvVideoDecoder *)channelCtx->pluginCommon;
            ret = vdecPluginEntry->pfnQueueOutput(channelCtx->pluginHandle, outInfo, -1);
            break;
        }
        case AUDIO_DECODER: {
            AvAudioDecoder *adecPluginEntry = (AvAudioDecoder *)channelCtx->pluginCommon;
            ret = adecPluginEntry->pfnQueueOutput(channelCtx->pluginHandle, outInfo);
            break;
        }
        case VIDEO_ENCODER: {
            AvVideoEncoder *vencPluginEntry = (AvVideoEncoder *)channelCtx->pluginCommon;
            ret = vencPluginEntry->pfnQueueOutput(channelCtx->pluginHandle, outInfo);
            break;
        }
        case AUDIO_ENCODER: {
            AvAudioEncoder *aencPluginEntry = (AvAudioEncoder *)channelCtx->pluginCommon;
            ret = aencPluginEntry->pfnQueueOutput(channelCtx->pluginHandle, outInfo);
            break;
        }
        default: {
            MEDIA_HAL_LOGE(MODULE_NAME, "invalid type");
            ret = MEDIA_HAL_ERR;
            break;
        }
    }
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "plugin queueout failed[0X%X] type[%d]", ret, type);
    }
    channelCtx->cached = true;
    MediaHalMutexUnLock(channelCtx->channelLock);
    return ret;
}

static int32_t ChannelDequeueOutInternal(CodecBufferChannel *channelCtx, CodecType type,
    OutputInfo *outInfo, uint32_t timeoutMs)
{
    int32_t ret;
    switch (type) {
        case VIDEO_DECODER: {
            AvVideoDecoder *vdecPluginEntry = (AvVideoDecoder *)channelCtx->pluginCommon;
            ret = vdecPluginEntry->pfnDequeueOutput(channelCtx->pluginHandle, outInfo, timeoutMs, NULL);
            if (ret == VDEC_ERR_FRAME_BUF_EMPTY) {
                ret = CODEC_ERR_FRAME_BUF_EMPTY;
            } else if (ret == VDEC_FRAME_BUF_EOS) {
                ret =  CODEC_RECEIVE_EOS;
            }
            break;
        }
        case AUDIO_DECODER: {
            AvAudioDecoder *adecPluginEntry = (AvAudioDecoder *)channelCtx->pluginCommon;
            ret = adecPluginEntry->pfnDequeueOutput(channelCtx->pluginHandle, outInfo, false);
            if (ret == ADEC_ERR_FRAME_BUF_EMPTY) {
                ret = CODEC_ERR_FRAME_BUF_EMPTY;
            } else if (ret == ADEC_RECEIVE_EOS) {
                ret =  CODEC_RECEIVE_EOS;
            }
            break;
        }
        case VIDEO_ENCODER: {
            AvVideoEncoder *vencPluginEntry = (AvVideoEncoder *)channelCtx->pluginCommon;
            ret = vencPluginEntry->pfnDequeueOutput(channelCtx->pluginHandle, outInfo, false);
            break;
        }
        case AUDIO_ENCODER: {
            AvAudioEncoder *aencPluginEntry = (AvAudioEncoder *)channelCtx->pluginCommon;
            ret = aencPluginEntry->pfnDequeueOutput(channelCtx->pluginHandle, outInfo, false);
            break;
        }
        default: {
            MEDIA_HAL_LOGE(MODULE_NAME, "invalid type");
            ret = MEDIA_HAL_ERR;
            break;
        }
    }
    return ret;
}

int32_t CodecChannelDequeueOutput(CodecBufferChannel *channelCtx, CodecType type,
    OutputInfo *outInfo, uint32_t timeoutMs)
{
    CHK_NULL_RETURN(channelCtx, MEDIA_HAL_INVALID_PARAM, "CodecBufferChannel context is null");
    CHK_NULL_RETURN(channelCtx->pluginHandle, MEDIA_HAL_INVALID_PARAM, "pluginHandle is null");
    CHK_NULL_RETURN(outInfo, MEDIA_HAL_INVALID_PARAM, "OutputInfo is null");

    if (!channelCtx->cached) {
        return MEDIA_HAL_OK;
    }

    if (channelCtx->channelStarted == false) {
        MEDIA_HAL_LOGE(MODULE_NAME, "channel not started type[%d]", type);
        return CHANNEL_ERR_CHANNEL_NOT_STARTED;
    }

    MediaHalMutexLock(channelCtx->channelLock);
    int32_t ret = ChannelDequeueOutInternal(channelCtx, type, outInfo, timeoutMs);
    if (ret != MEDIA_HAL_OK) {
        channelCtx->cached = false;
        MEDIA_HAL_LOGD(MODULE_NAME, "plugin dequeue out failed[OX%X] type[%d]", ret, type);
        MediaHalMutexUnLock(channelCtx->channelLock);
        return ret;
    }
    channelCtx->cached = false;
    MediaHalMutexUnLock(channelCtx->channelLock);
    return MEDIA_HAL_OK;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

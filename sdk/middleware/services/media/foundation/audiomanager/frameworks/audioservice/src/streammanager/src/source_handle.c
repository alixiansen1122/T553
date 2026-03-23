/*
* Copyright (c) CompanyNameMagicTag. 2021-2021. All rights reserved.
* Description: source handle interfaces
* Author: Media Software Group
* Create: 2021-04-21
*/

#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "audio_capture.h"
#include "audio_errors.h"
#include "audio_utils.h"
#include "device_manager.h"
#include "policy_manager.h"
#include "securec.h"
#include "source_handle.h"

#define LOG_MODULE_NAME "StreamMgr"
#define ERR_INVALID_READ (-1)
#define PART_STR_NUM 15
#define TOTAL_STR_NUM 25

typedef struct {
    int32_t streamId;
    AudioSession sessionId;
    int32_t streamType;
    int32_t format;
    float volume;
    uint32_t sampleRate;
    uint32_t channelCnt;
    uint32_t defalutAudioInputDevice;
    uint32_t userAudioInputDevice;
    CapturerCallback callback;
    const void *caller;
    struct AudioCapture *capture;
    StreamState state;
    bool micMute;
} SourceHandleContext;

uintptr_t CreateSourceHandle(int32_t streamId, const AudioStreamInfo *inf,
    uint32_t defaultRouteDevice, VolumeInf volInf)
{
    // 创建的时候，需要全局锁吗，初步看起来不用，涉及到全局共享变量的才需要
    // malloc context，然后赋值
    SourceHandleContext *sourceHandleCtx = (SourceHandleContext*)malloc(sizeof(SourceHandleContext));
    CHK_NULL_RETURN(sourceHandleCtx, (uintptr_t)NULL, "malloc SourceHandleContext fail");
    memset_s(sourceHandleCtx, sizeof(SourceHandleContext), 0x00, sizeof(SourceHandleContext));
    sourceHandleCtx->streamId = streamId;
    sourceHandleCtx->sessionId = inf->sessionID;
    sourceHandleCtx->streamType = inf->streamType;
    sourceHandleCtx->format = inf->format;
    sourceHandleCtx->defalutAudioInputDevice = defaultRouteDevice; // fw device
    sourceHandleCtx->sampleRate = inf->sampleRate;
    sourceHandleCtx->channelCnt = (uint32_t)inf->channelCount;
    sourceHandleCtx->state = IDLE;
    sourceHandleCtx->micMute = volInf.streamMute;
    sourceHandleCtx->volume = volInf.typeVolume;
    return (uintptr_t)sourceHandleCtx;
}

int32_t DestroySourceHandle(uintptr_t handle)
{
    SourceHandleContext *context = (SourceHandleContext *)handle;
    CHK_NULL_RETURN(context, AUDIO_ERROR, "source handle is NULL");
    if (context->state == IDLE) {
        free(context);
        return AUDIO_SUCCESS;
    }
    uint32_t validFwDevice = context->userAudioInputDevice == 0 ?
        context->defalutAudioInputDevice :
        context->userAudioInputDevice;
    int32_t ret = DeviceMgrDestroyStreamIn(validFwDevice, context->capture);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("source[%d] destroy failed 0x%x", context->streamId, ret);
        free(context);
        return AUDIO_ERROR;
    }
    free(context);
    return AUDIO_SUCCESS;
}

int32_t SetSourceCallback(uintptr_t handle, CapturerCallback cbk, const void *caller)
{
    SourceHandleContext *context = (SourceHandleContext *)handle;
    CHK_NULL_RETURN(context, AUDIO_ERROR, "track handle is NULL");
    context->callback = cbk;
    context->caller = caller;
    return AUDIO_SUCCESS;
}

int32_t GetSourceDeviceId(uintptr_t handle)
{
    SourceHandleContext *context = (SourceHandleContext *)handle;
    CHK_NULL_RETURN(context, AUDIO_ERROR, "source handle is NULL");
    int32_t deviceId = (int32_t)((context->userAudioInputDevice != 0) ? context->userAudioInputDevice :
                                                            context->defalutAudioInputDevice);
    return deviceId;
}

int32_t GetSourceChannelId(uintptr_t handle, uint32_t *channelId)
{
    SourceHandleContext *context = (SourceHandleContext *)handle;
    CHK_NULL_RETURN(context, AUDIO_ERROR, "source handle is NULL");
    if (context->state != RUNNING) {
        return AUDIO_BAD_STATE;
    }
    int32_t ret = context->capture->attr.GetCurrentChannelId((AudioHandle)context->capture, channelId);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("Get channel id failed: 0x%x", ret);
        return AUDIO_ERROR;
    }
    ALOGI("Channel id: 0x%x", *channelId);
    return AUDIO_SUCCESS;
}

int32_t GetSourceSessionId(uintptr_t handle, AudioSession *sessionId)
{
    CHK_NULL_RETURN(sessionId, AUDIO_ERROR, "sessionId is NULL");
    SourceHandleContext *context = (SourceHandleContext *)handle;
    CHK_NULL_RETURN(context, AUDIO_ERROR, "source handle is NULL");
    *sessionId = context->sessionId;
    return AUDIO_SUCCESS;
}

AudioStreamType GetSourceStreamType(uintptr_t handle)
{
    SourceHandleContext *context = (SourceHandleContext *)handle;
    CHK_NULL_RETURN(context, AUDIO_STREAM_INVALID, "source handle is NULL");
    return (AudioStreamType)context->streamType;
}

int32_t GetSourceStreamId(uintptr_t handle)
{
    SourceHandleContext *context = (SourceHandleContext *)handle;
    CHK_NULL_RETURN(context, AUDIO_ERROR, "source handle is NULL");
    return context->streamId;
}

int32_t SourceAttachBackend(uintptr_t handle, uint32_t backEndId)
{
    SourceHandleContext *context = (SourceHandleContext *)handle;
    CHK_NULL_RETURN(context, AUDIO_ERROR, "source handle is NULL");
    if (context->state != RUNNING) {
        return AUDIO_BAD_STATE;
    }

    char keyValuePair[TOTAL_STR_NUM] = { 0 };
    int convertRet = sprintf_s(keyValuePair, sizeof(keyValuePair), "%s = %u", "attachId", backEndId);
    if (convertRet == AUDIO_ERROR) {
        ALOGE("splice key=value str fail[%d]", convertRet);
        return AUDIO_ERROR;
    }
    int32_t ret = context->capture->attr.SetExtraParams((AudioHandle)context->capture, keyValuePair);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("set backEnd Id failed: 0x%x", ret);
        return AUDIO_ERROR;
    }
    ALOGI("backEndId: 0x%x", backEndId);
    return AUDIO_SUCCESS;
}

int32_t GetSourceTimestamp(uintptr_t handle, struct timespec *timestamp)
{
    SourceHandleContext *context = (SourceHandleContext *)handle;
    CHK_NULL_RETURN(context, AUDIO_ERROR, "source handle is NULL");
    if (context->state != RUNNING) {
        ALOGE("source not run");
        return AUDIO_ERROR;
    }

    uint64_t frameIndex;
    struct AudioTimeStamp time = {};
    int32_t ret = context->capture->GetCapturePosition(context->capture, &frameIndex, &time);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("GetCapturePosition failed 0x%x", ret);
        return AUDIO_ERROR;
    }

    timestamp->tv_sec = (time_t)time.tvSec;
    timestamp->tv_nsec = (long)time.tvNSec;
    return AUDIO_SUCCESS;
}

int32_t SetSourceInputDevice(uintptr_t handle, int32_t deviceId, const AudioStreamInfo *info)
{
    // 1. 查看状态，如果还未running，就直接设置
    // 2. 如果已经running，调用 changeToute
    AUDIO_UNUSED(info);
    SourceHandleContext *context = (SourceHandleContext *)handle;
    CHK_NULL_RETURN(context, AUDIO_ERROR, "track handle is NULL");
    if (!PolicyMgrCanBeRoutedToThisDevice(deviceId, false)) {
        goto ERR;
    }

    if (context->state == IDLE) {
        context->userAudioInputDevice = (uint32_t)deviceId;
    } else if (context->state == RUNNING || context->state == PAUSE) {
        int32_t halDevice = 0;
        if (ConvertOutDeviceToHal((uint32_t)deviceId, &halDevice) != ONE_DEVICE) {
            goto ERR;
        }
        struct AudioSceneDescriptor sceneDescriptor;
        sceneDescriptor.desc.pins = (uint32_t)halDevice;
        int32_t ret = context->capture->scene.SelectScene((AudioHandle)context->capture, &sceneDescriptor);
        if (ret != AUDIO_SUCCESS) {
            goto ERR;
        }
    } else {
        ALOGE("bad state[%d]", context->state);
        goto ERR;
    }
    return AUDIO_SUCCESS;

ERR:
    return AUDIO_ERROR;
}

static bool CheckCaptureValid(const struct AudioCapture *capturer)
{
    CHK_NULL_RETURN(capturer, false, "capturer is NULL");

    CHK_NULL_RETURN(capturer->control.Start, false, "capturer control Start is NULL");
    CHK_NULL_RETURN(capturer->control.Stop, false, "capturer control Stop is NULL");

    CHK_NULL_RETURN(capturer->attr.GetCurrentChannelId, false, "capturer attr GetCurrentChannelId is NULL");

    CHK_NULL_RETURN(capturer->scene.SelectScene, false, "capturer scene SelectScene is NULL");
    CHK_NULL_RETURN(capturer->volume.GetMute, false, "capturer volume GetMute is NULL");
    CHK_NULL_RETURN(capturer->volume.SetMute, false, "capturer volume SetMute is NULL");
    return true;
}

int32_t StartSource(uintptr_t handle)
{
    SourceHandleContext *context = (SourceHandleContext *)handle;
    CHK_NULL_RETURN(context, AUDIO_ERROR, "source handle is NULL");
    if (context->state == RUNNING) {
        return AUDIO_SUCCESS;
    }
    if (context->state == STOP) {
        return AUDIO_ERROR;
    }
    if (context->state == IDLE) {
        uint32_t validFwDevice = context->userAudioInputDevice == 0 ? context->defalutAudioInputDevice :
                                                                      context->userAudioInputDevice;
        AudioStreamConfig attr = { context->streamType,
                                   false,
                                   context->format,
                                   context->sampleRate,
                                   context->channelCnt,
                                   0 };
        Handle capture;
        int32_t ret = DeviceMgrCreateStreamIn(validFwDevice, attr, &capture);
        if (ret != AUDIO_SUCCESS) {
            ALOGE("source[%d] start failed 0x%x", context->streamId, ret);
            return AUDIO_ERROR;
        }
        context->capture = (struct AudioCapture *)capture;
        if (!CheckCaptureValid(context->capture)) {
            ALOGE("capturer is invalid");
            return AUDIO_ERROR;
        }
    }
    context->state = RUNNING;
    int32_t ret = context->capture->volume.SetMute((AudioHandle)context->capture, context->micMute);
    if (ret != AUDIO_SUCCESS) {
        return AUDIO_ERROR;
    }
    if (context->streamType == AUDIO_STREAM_VOICE_CALL_VOLTE) {
        ret = context->capture->volume.SetVolume((AudioHandle)context->capture, context->volume);
        if (ret != AUDIO_SUCCESS) {
            return AUDIO_ERROR;
        }
    }
    ret = context->capture->control.Start((AudioHandle)context->capture);
    if (ret != AUDIO_SUCCESS) {
        return AUDIO_ERROR;
    }

    return AUDIO_SUCCESS;
}

int32_t ObtainSourceBuffer(uintptr_t handle, const CapturerBuffer *buffer)
{
    SourceHandleContext *context = (SourceHandleContext *)handle;
    CHK_NULL_RETURN(context, AUDIO_ERROR, "source handle is NULL");
    if (context->state != RUNNING) {
        ALOGE("source not run");
        return AUDIO_ERROR;
    }

    uint64_t readlen = ERR_INVALID_READ;
    int32_t ret = context->capture->CaptureFrame(context->capture, buffer->data, buffer->size, &readlen);
    if (ret != AUDIO_SUCCESS) {
        if (ret != AUDIO_RETRY_READ &&
            ret != AUDIO_NOT_SUPPORT_GET_FRAME) {
            ALOGE("CaptureFrame failed 0x%x", ret);
        }
        return ret;
    }
    return (int32_t)readlen;
}

int32_t StopSource(uintptr_t handle)
{
    SourceHandleContext *context = (SourceHandleContext *)handle;
    CHK_NULL_RETURN(context, AUDIO_ERROR, "source handle is NULL");
    if (context->state == STOP) {
        return AUDIO_SUCCESS;
    }
    if (context->state == IDLE) {
        return AUDIO_ERROR;
    }
    context->state = STOP;
    int32_t ret = context->capture->control.Stop((AudioHandle)context->capture);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("source[%d] stop failed 0x%x", context->streamId, ret);
        return ret;
    }
    return AUDIO_SUCCESS;
}

int32_t DeactivateCorrespondingSource(uintptr_t handle)
{
    SourceHandleContext *context = (SourceHandleContext *)handle;
    CHK_NULL_RETURN(context, AUDIO_ERROR, "TrackHandleContext *context is NULL");
    if (context->state == STOP) {
        return AUDIO_SUCCESS;
    }
    if (context->state == IDLE) {
        context->state = STOP;
    }
    if (context->state == RUNNING) {
        context->state = STOP;
        int32_t ret = context->capture->control.Stop((AudioHandle)context->capture);
        if (ret != AUDIO_SUCCESS) {
            ALOGE("source[%d] stop failed 0x%x in activate progress", context->streamId, ret);
            return AUDIO_ERROR;
        }
    }
    return AUDIO_SUCCESS;
}

int32_t SetSourceMute(uintptr_t handle, bool mute)
{
    SourceHandleContext *context = (SourceHandleContext *)handle;
    CHK_NULL_RETURN(context, AUDIO_ERROR, "source handle is NULL");
    if (context->state != IDLE && context->state != RUNNING) {
        return AUDIO_ERROR;
    }
    if (context->state == IDLE) {
        context->micMute = mute;
        return AUDIO_SUCCESS;
    }
    int32_t ret = context->capture->volume.SetMute((AudioHandle)context->capture, mute);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("context->capture->volume.SetMute failed");
        return AUDIO_ERROR;
    }
    context->micMute = mute;
    return AUDIO_SUCCESS;
}

int32_t SetSourceVolume(uintptr_t handle, float volume)
{
    SourceHandleContext *context = (SourceHandleContext *)handle;
    CHK_NULL_RETURN(context, AUDIO_ERROR, "source handle is NULL");
    if (context->state == IDLE || context->state == STOP) {
        ALOGE("set volume failed, current state:%d", context->state);
        return AUDIO_BAD_STATE;
    }
    int32_t ret = context->capture->volume.SetVolume((AudioHandle)context->capture, volume);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("set volume failed");
        return AUDIO_ERROR;
    }
    return AUDIO_SUCCESS;
}

StreamDebugInfo DumpSourceInfo(uintptr_t handle)
{
    SourceHandleContext *context = (SourceHandleContext *)handle;
    if (context == NULL) {
        StreamDebugInfo temp = {};
        return temp;
    }
    ALOGD("=================stream in info streamId:%d sessionId:%u begin======================",
        context->streamId, context->sessionId);

    uint32_t curDevice = (context->userAudioInputDevice != 0) ? context->userAudioInputDevice :
                                                              context->defalutAudioInputDevice;
    ALOGD("streamState[%s] stream type[%s] stream in device[0X%X]",
        StringState(context->state), StringType(context->streamType), curDevice);

    ALOGD("=================stream in info streamId:%d sessionId:%u end======================",
        context->streamId, context->sessionId);

    AudioStreamInfo streamInfo = {
        context->streamType, context->sessionId, context->format,
        context->sampleRate, 0, context->channelCnt,
        AUDIO_CHANNEL_MODE_NONE, AUDIO_STREAM_FLAG_NONE,
        AUDIO_LINK_NONE, 0
    };

    StreamDebugInfo inf = { context->state, context->streamId, streamInfo, 0, 0, curDevice };
    return inf;
}

int32_t SetSourceParam(uintptr_t handle, const char *keyValueList, size_t len)
{
    AUDIO_UNUSED(len);
    SourceHandleContext *context = (SourceHandleContext *)handle;
    CHK_NULL_RETURN(context, AUDIO_ERROR, "source handle is NULL");
    CHK_NULL_RETURN(keyValueList, AUDIO_ERROR, "keyValueList is NULL");
    if (context->state != IDLE && context->state != RUNNING) {
        return AUDIO_ERROR;
    }
    if (context->capture == NULL) {
        ALOGE("not create capture");
        return AUDIO_ERROR;
    }
    int32_t ret = context->capture->attr.SetExtraParams((AudioHandle)context->capture, keyValueList);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("call set extra params failed ");
        return AUDIO_ERROR;
    }
    return AUDIO_SUCCESS;
}

int32_t GetSourceParam(uintptr_t handle, char *keyValueList, size_t len)
{
    SourceHandleContext *context = (SourceHandleContext *)handle;
    CHK_NULL_RETURN(context, AUDIO_ERROR, "source handle is NULL");
    CHK_NULL_RETURN(keyValueList, AUDIO_ERROR, "keyValueList is NULL");
    if (context->capture == NULL) {
        ALOGE("not create capture");
        return AUDIO_ERROR;
    }
    int32_t ret = context->capture->attr.GetExtraParams((AudioHandle)context->capture, keyValueList, (int32_t)len);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("call get extra params failed ");
        return AUDIO_ERROR;
    }
    return AUDIO_SUCCESS;
}
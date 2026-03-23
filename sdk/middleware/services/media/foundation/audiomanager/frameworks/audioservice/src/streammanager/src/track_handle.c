/*
* Copyright (c) CompanyNameMagicTag. 2021-2021. All rights reserved.
* Description: track interfaces
* Author: Media Software Group
* Create: 2021-04-21
*/

#include <stdlib.h>
#include <math.h>
#include "audio_render.h"
#include "audio_errors.h"
#include "device_manager.h"
#include "policy_manager.h"
#include "securec.h"
#include "media_thread_adapt.h"
#include "track_handle.h"
#ifdef SUPPORT_SET_DSP_CLOCK_FREQ
#include "hifi_clock_freq_manager.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define LOG_MODULE_NAME "TrackHandle"
#define OUT_DEVICE_NUM 3
#define ONE_DEVICE 1
#define NO_DEVICE 0
#define S2NS 1000000000
#define VOLUME_THRESHOLD (0.618)

typedef struct {
    int32_t streamId;
    uint32_t defalutAudioOutDevice;
    uint32_t userAudioOutDevice;
    float mixWeight;
    float typeVolume;
    float streamVolume;
    float streamVolumeInBt;
    bool mute;
    uint32_t routeChangedDevice;
    AudioStreamInfo streamInfo;
    struct AudioRender *render;
    StreamState state;
    RendererCallback callback;
    const void *caller;
    char *keyValueList;
    bool eos;
    MediaMutexHandle deviceChangeMutex;
} TrackHandleContext;

typedef struct {
    int32_t streamId;
    TrackHandleContext *ctx;
    bool used;
} StreamPair;

#define CTX_MAX_NUM 10
#define TOTAL_STR_NUM 25

uintptr_t CreateTrackHandle(int32_t streamId, const AudioStreamInfo *inf, uint32_t defalutOutDevice, VolumeInf volInf)
{
    TrackHandleContext *trackHandleCtx = (TrackHandleContext*)malloc(sizeof(TrackHandleContext));
    CHK_NULL_RETURN(trackHandleCtx, (uintptr_t)NULL, "malloc TrackHandleContext fail");
    memset_s(trackHandleCtx, sizeof(TrackHandleContext), 0x00, sizeof(TrackHandleContext));
    trackHandleCtx->streamId = streamId;
    ALOGI("type is:%s", StringType(inf->streamType));
    trackHandleCtx->streamVolume = 1.0;
    trackHandleCtx->streamVolumeInBt = -1;
    trackHandleCtx->defalutAudioOutDevice = defalutOutDevice; // fw device
    trackHandleCtx->mute = volInf.streamMute;
    trackHandleCtx->typeVolume = volInf.typeVolume;
    trackHandleCtx->state = IDLE;
    trackHandleCtx->streamInfo = *inf;
    trackHandleCtx->eos = false;
    trackHandleCtx->deviceChangeMutex = MediaMutexCreate(NULL);
    if (trackHandleCtx->deviceChangeMutex == NULL) {
        ALOGE("create device change mutex failed");
        free(trackHandleCtx);
        return (uintptr_t)NULL;
    }
    trackHandleCtx->render = NULL;
    return (uintptr_t)trackHandleCtx;
}

int32_t DestroyTrackHandle(uintptr_t handle)
{
    TrackHandleContext *context = (TrackHandleContext *)handle;
    CHK_NULL_RETURN(context, AUDIO_ERROR, "track handle is NULL");
    MediaMutexLock(context->deviceChangeMutex);
    ALOGI("type is:%s", StringType(context->streamInfo.streamType));
    uint32_t validFwDevice = context->userAudioOutDevice == 0 ?
        context->defalutAudioOutDevice :
        context->userAudioOutDevice;
    int32_t ret = AUDIO_SUCCESS;
    if (context->state == IDLE) {
        goto EXIT;
    }
    ret = DeviceMgrDestroyStreamOut(validFwDevice, context->render);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("track[%d] destroy failed 0x%x", context->streamId, ret);
    }
EXIT:
    // 输出设备切换场景，底层解码器会销毁再创建，FALC解码需要重新设置extraData信息，流销毁再释放相关内存
    if (context->keyValueList != NULL) {
        free(context->keyValueList);
        context->keyValueList = NULL;
    }
    MediaMutexUnLock(context->deviceChangeMutex);
    MediaMutexDestroy(&context->deviceChangeMutex);
    free(context);
    return ret;
}

// 现在只是简单保存下
int32_t SetCallback(uintptr_t handle, RendererCallback cbk, const void *caller)
{
    TrackHandleContext *context = (TrackHandleContext *)handle;
    CHK_NULL_RETURN(context, AUDIO_ERROR, "track handle is NULL");
    context->callback = cbk;
    context->caller = caller;
    return AUDIO_SUCCESS;
}

static bool CheckRenderValid(const struct AudioRender *render)
{
    CHK_NULL_RETURN(render, false, "render is NULL");

    CHK_NULL_RETURN(render->SetRenderSpeed, false, "render SetRenderSpeed is NULL");
    CHK_NULL_RETURN(render->control.Start, false, "render control Start is NULL");
    CHK_NULL_RETURN(render->control.Resume, false, "render control Resume is NULL");
    CHK_NULL_RETURN(render->control.Pause, false, "render control Pause is NULL");
    CHK_NULL_RETURN(render->control.Stop, false, "render control Stop is NULL");
    CHK_NULL_RETURN(render->control.Flush, false, "render control Flush is NULL");

    CHK_NULL_RETURN(render->RenderFrame, false, "render RenderFrame is NULL");
    CHK_NULL_RETURN(render->GetRenderPosition, false, "render GetRenderPosition is NULL");

    CHK_NULL_RETURN(render->attr.GetCurrentChannelId, false, "render attr GetCurrentChannelId is NULL");
    CHK_NULL_RETURN(render->attr.SetExtraParams, false, "render attr SetExtraParams is NULL");
    CHK_NULL_RETURN(render->attr.GetExtraParams, false, "render attr GetExtraParams is NULL");

    CHK_NULL_RETURN(render->scene.SelectScene, false, "render scene SelectScene is NULL");

    CHK_NULL_RETURN(render->volume.SetVolume, false, "render volume SetVolume is NULL");
    CHK_NULL_RETURN(render->volume.SetMute, false, "render volume SetMute is NULL");
    CHK_NULL_RETURN(render->RegCallback, false, "render RegCallback is NULL");
    return true;
}

static int32_t OnRenderCallback(enum AudioCallbackType type, void *reserved, void *cookie)
{
    AUDIO_UNUSED(reserved);
    TrackHandleContext *context = (TrackHandleContext *)cookie;
    if (context->callback == NULL) {
        if (type == AUDIO_BUFFER_EMPTY) {
            return AUDIO_SUCCESS;
        }
        ALOGE("call back is null!");
        return AUDIO_ERROR;
    }
    return context->callback(context->caller, (AudioCallbackEventType)type, NULL);
}

static bool IsSpeakerSwitchToBt(const TrackHandleContext *context, int32_t targetDevice)
{
    uint32_t curDevice = (context->userAudioOutDevice != 0) ? context->userAudioOutDevice :
        context->defalutAudioOutDevice;
    // only A2DP can swicth
    return (curDevice == OUT_BUILTIN_SPEAKER) && (targetDevice == OUT_BLUETOOTH_A2DP);
}

static bool IsBtSwitchToSpeaker(const TrackHandleContext *context, int32_t targetDevice)
{
    uint32_t curDevice = (context->userAudioOutDevice != 0) ? context->userAudioOutDevice :
        context->defalutAudioOutDevice;
    // only A2DP can swicth
    return (curDevice == OUT_BLUETOOTH_A2DP) && (targetDevice == OUT_BUILTIN_SPEAKER);
}

static int32_t ProcessCurStream(TrackHandleContext *context)
{
    // 1. stop cur stream
    int32_t ret = context->render->control.Stop((AudioHandle)context->render);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("change device, track[%d] stop failed 0x%x", context->streamId, ret);
        return ret;
    }
    // 2. destroy cur stream
    uint32_t validFwDevice = context->userAudioOutDevice == 0 ?
        context->defalutAudioOutDevice :
        context->userAudioOutDevice;
    ret = DeviceMgrDestroyStreamOut(validFwDevice, context->render);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("change device, track[%d] destroy failed 0x%x", context->streamId, ret);
        return AUDIO_ERROR;
    }
    context->render = NULL;
#ifdef SUPPORT_SET_DSP_CLOCK_FREQ
    SetHifiClockFreqByStreamTypeAndDeviceType(context->streamInfo.streamType, (AudioDeviceType)validFwDevice, false);
#endif
    return AUDIO_SUCCESS;
}

static int32_t SetExtraData(TrackHandleContext *context)
{
    if (context->keyValueList != NULL) {
        int32_t ret = context->render->attr.SetExtraParams((AudioHandle)context->render, context->keyValueList);
        if (ret != AUDIO_SUCCESS) {
            ALOGE("track_handle.c SetExTraData:call render->attr.SetExtraParams failed ");
            if (context->keyValueList != NULL) {
                free(context->keyValueList);
                context->keyValueList = NULL;
            }
            return AUDIO_ERROR;
        }
    }
    return AUDIO_SUCCESS;
}

static int32_t ProcessDestStream(TrackHandleContext *context, int32_t deviceId)
{
    // 1. create dest stream
    AudioStreamConfig attr = { context->streamInfo.streamType,
                               false,
                               context->streamInfo.format,
                               context->streamInfo.sampleRate,
                               context->streamInfo.channelCount,
                               context->streamInfo.sampleFmt };
    Handle render;
    int32_t ret = DeviceMgrCreateStreamOut((uint32_t)deviceId, attr, &render);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("change device, track[%d] start failed 0x%x", context->streamId, ret);
        return AUDIO_ERROR;
    }
    context->render = (struct AudioRender *)render;
    if (!CheckRenderValid(context->render)) {
        ALOGE("change device, render is invalid");
        goto DESTROY;
    }
    if (SetExtraData(context) != AUDIO_SUCCESS) {
        ALOGI("render call SetExTraData failed");
        goto DESTROY;
    }
    ret = context->render->RegCallback((AudioHandle)context->render, OnRenderCallback, (void *)context);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("render call RegCallback failed");
        goto DESTROY;
    }
    ret = context->render->control.Start((AudioHandle)context->render);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("change device, render start fail");
        goto DESTROY;
    }
    return AUDIO_SUCCESS;

DESTROY:
    (void)DeviceMgrDestroyStreamOut((uint32_t)deviceId, context->render);
    context->render = NULL;
    return AUDIO_ERROR;
}

inline static bool IsVolumeValid(float volumeAbout)
{
    return volumeAbout >= 0.0 && volumeAbout <= 1.0;
}

static int32_t ChangeRouteInner(TrackHandleContext *context, int32_t deviceId)
{
    CHK_NULL_RETURN(context->render, AUDIO_ERROR, "hal render is NULL");
    // 1. process cur stream
    int32_t ret = ProcessCurStream(context);
    CHK_FAILED_RETURN(ret, AUDIO_SUCCESS, AUDIO_ERROR, "process cur stream fail");
    // 2. process dest stream
    ret = ProcessDestStream(context, deviceId);
    CHK_FAILED_RETURN(ret, AUDIO_SUCCESS, AUDIO_ERROR, "process dest stream fail");

    // 3. set volume
    bool valid = IsVolumeValid(context->typeVolume) && IsVolumeValid(context->streamVolume) &&
        IsVolumeValid(context->mixWeight);
    CHK_FALSE_RETURN(valid, AUDIO_ERROR, "arg is invalid!");
    float finalVolume = context->typeVolume * context->streamVolume * context->mixWeight;
    if (IsSpeakerSwitchToBt(context, deviceId)) {
        finalVolume = (finalVolume > VOLUME_THRESHOLD) ? VOLUME_THRESHOLD : finalVolume;
        if (fabs(context->streamVolumeInBt + 1.0) >= COMPARE_THRESHOLD) {
            ALOGE("set volume in bt volume[%f] <= -1.0", context->streamVolumeInBt);
            return AUDIO_ERROR;
        }
        // context->typeVolume * context->mixWeight不能等于0，后面有除0风险
        if (context->typeVolume * context->mixWeight < COMPARE_THRESHOLD) {
            context->streamVolumeInBt = finalVolume;
        } else {
            context->streamVolumeInBt = finalVolume / (context->typeVolume * context->mixWeight);
        }
        ALOGI("speaker switch to bluetooth stream volume: %f, bluetooth volume: %f, final volume: %f",
            context->streamVolume, context->streamVolumeInBt, finalVolume);
    }
    if (IsBtSwitchToSpeaker(context, deviceId)) {
        context->streamVolumeInBt = -1;
        ALOGI("bluetooth switch to speaker stream volume: %f, bluetooth volume: %f, final volume: %f",
            context->streamVolume, context->streamVolumeInBt, finalVolume);
    }
    CHK_NULL_RETURN(context->render->volume.SetVolume, AUDIO_ERROR, "hal render SetVolume is NULL");
    ret = context->render->volume.SetVolume((AudioHandle)context->render, finalVolume);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("track[%d] SetVolume failed 0x%x", context->streamId, ret);
        return ret;
    }
    // 4. check if need send eos
    if (context->eos) {
        uint64_t replyBytes = 0;
        ret = context->render->RenderFrame(context->render, NULL, 0, &replyBytes);
        if (ret != AUDIO_SUCCESS) {
            ALOGE("track[%d] render eos failed 0x%x", context->streamId, ret);
            return ret;
        }
    }
    context->userAudioOutDevice = (uint32_t)deviceId;
    return AUDIO_SUCCESS;
}

int32_t ChangeRoute(uintptr_t handle, int32_t deviceId)
{
    TrackHandleContext *context = (TrackHandleContext *)handle;
    CHK_NULL_RETURN(context, AUDIO_ERROR, "track handle is NULL");
    if (context->state == IDLE || context->state == STOP) {
        return AUDIO_ERROR;
    }
    if (ChangeRouteInner(context, deviceId) != AUDIO_SUCCESS) {
        return AUDIO_ERROR;
    }
    return AUDIO_SUCCESS;
}

static bool IsAlreadyInWork(const TrackHandleContext *context, uint32_t deviceId)
{
    uint32_t curDevice = (context->userAudioOutDevice != 0) ? context->userAudioOutDevice :
        context->defalutAudioOutDevice;
    return ((curDevice & deviceId) != 0);
}

int32_t SetTrackOutputDevice(uintptr_t handle, int32_t deviceId, const AudioStreamInfo *info)
{
    // 1. 查看状态，如果还未running，就直接设置
    // 2. 如果已经running，调用 changeRoute
    AUDIO_UNUSED(info);
    TrackHandleContext *context = (TrackHandleContext *)handle;
    CHK_NULL_RETURN(context, AUDIO_ERROR, "track handle is NULL");
    MediaMutexLock(context->deviceChangeMutex);
    if (IsAlreadyInWork(context, (uint32_t)deviceId)) {
        // 本地播放中途接听蓝牙通话，通话中途蓝牙设备反复回连/断开，最终处在断开状态，
        // 在挂断电话后，恢复播放的时候无需切换设备
        context->routeChangedDevice = UNKNOWN;
        MediaMutexUnLock(context->deviceChangeMutex);
        return AUDIO_SUCCESS;
    }
    if (!PolicyMgrCanBeRoutedToThisDevice(deviceId, true)) {
        goto ERR;
    }
    if (context->state == IDLE) {
        context->userAudioOutDevice = (uint32_t)deviceId;
    } else if (context->state == RUNNING) {
        (void)context->callback(context->caller, AUDIO_EVENT_OUTPUT_DEVICE_CHANGED, NULL);
        if (ChangeRouteInner(context, deviceId) != AUDIO_SUCCESS) {
            goto ERR;
        }
    } else if (context->state == PAUSE) {
        context->routeChangedDevice = (uint32_t)deviceId;
    } else {
        ALOGE("bad state[%d]", context->state);
        goto ERR;
    }
    MediaMutexUnLock(context->deviceChangeMutex);
    return AUDIO_SUCCESS;

ERR:
    if (context->callback == NULL) {
        ALOGE("callback func is NULL");
        MediaMutexUnLock(context->deviceChangeMutex);
        return AUDIO_ERROR;
    }
    (void)context->callback(context->caller, AUDIO_EVENT_ERROR_OCCUR, NULL);
    MediaMutexUnLock(context->deviceChangeMutex);
    return AUDIO_ERROR;
}

StreamState GetState(uintptr_t handle)
{
    TrackHandleContext *context = (TrackHandleContext *)handle;
    CHK_NULL_RETURN(context, IDLE, "track handle is NULL");
    StreamState curState = context->state;
    return curState;
}

int32_t GetHandleId(uintptr_t handle, int32_t *streamId, AudioSession *sessionId)
{
    TrackHandleContext *context = (TrackHandleContext *)handle;
    CHK_NULL_RETURN(context, AUDIO_ERROR, "track handle is NULL");
    *streamId = context->streamId;
    *sessionId = context->streamInfo.sessionID;
    return AUDIO_SUCCESS;
}

static int32_t UpdateMixWeigthWhenResume(TrackHandleContext *context)
{
    AudioStreamDescriptor newStreamDesc = {context->streamId, context->streamInfo.streamType,
                                           context->typeVolume, context->streamInfo.linkDir};
    AudioStreamStrategy *strategy = PolicyMgrGetStrategyForStream(context->streamInfo.sessionID,
                                                                  &newStreamDesc, STREAM_OUT_RESUME);
    CHK_NULL_RETURN(strategy, false, "strategy is NULL");
    // 检查
    bool find = false;
    for (uint32_t i = 0; i < strategy->activeStreamNum; i++) {
        if (context->streamId != strategy->activeStreams[i]->streamDesc.streamIndex) {
            continue;
        }
        find = true;
        // 权重变化，当即作用到hal
        if (fabs(context->mixWeight - strategy->activeStreams[i]->mixWeight) >= COMPARE_THRESHOLD) {
            context->mixWeight = strategy->activeStreams[i]->mixWeight;
            float finalVol = context->typeVolume * context->streamVolume * context->mixWeight;
            int32_t ret = context->render->volume.SetVolume((AudioHandle)context->render, finalVol);
            CHK_FAILED_RETURN(ret, AUDIO_SUCCESS, ret, "set volume fail");
        }
        break;
    }
    if (!find) {
        return AUDIO_NOT_FOUND_ACTIVE_STREAM;
    }
    return AUDIO_SUCCESS;
}

static int32_t SetVolumeAndSetMute(const TrackHandleContext *context)
{
    bool valid = IsVolumeValid(context->typeVolume) && IsVolumeValid(context->streamVolume) &&
        IsVolumeValid(context->mixWeight);
    if (!valid) {
        return AUDIO_ERROR;
    }
    // 音量和mute给入hal
    float finalVolume = context->typeVolume * context->streamVolume * context->mixWeight;
    int32_t ret = context->render->volume.SetVolume((AudioHandle)context->render, finalVolume);
    ret = context->render->volume.SetMute((AudioHandle)context->render, context->mute);
    if (ret != AUDIO_SUCCESS) {
        return AUDIO_ERROR;
    }
    return AUDIO_SUCCESS;
}

static int32_t CreateAudioRenderHandle(TrackHandleContext *context)
{
    uint32_t validFwDevice = context->userAudioOutDevice == 0 ? context->defalutAudioOutDevice :
        context->userAudioOutDevice;
    AudioStreamConfig attr = { context->streamInfo.streamType, false,
        context->streamInfo.format,
        context->streamInfo.sampleRate,
        context->streamInfo.channelCount,
        context->streamInfo.sampleFmt };
    Handle render;
    int32_t ret = DeviceMgrCreateStreamOut(validFwDevice, attr, &render);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("track[%d] start failed 0x%x", context->streamId, ret);
        return AUDIO_ERROR;
    }
    context->render = (struct AudioRender *)render;

    return AUDIO_SUCCESS;
}

static void DestroyAudioRenderHandle(TrackHandleContext *context)
{
    uint32_t validFwDevice = context->userAudioOutDevice == 0 ? context->defalutAudioOutDevice :
        context->userAudioOutDevice;
    int32_t ret = DeviceMgrDestroyStreamOut(validFwDevice, context->render);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("track[%d] destroy failed 0x%x", context->streamId, ret);
    }
    context->render = NULL;
}

static int32_t ResumeTrack(TrackHandleContext *context)
{
    bool needChangeRoute = (context->routeChangedDevice != UNKNOWN);
    int32_t ret = UpdateMixWeigthWhenResume(context);
    CHK_FAILED_RETURN_NO_LOG(ret, AUDIO_SUCCESS, ret);
    if (!needChangeRoute) {
        if (context->render->control.Resume((AudioHandle)context->render) != AUDIO_SUCCESS) {
            return AUDIO_ERROR;
        }
        if (SetVolumeAndSetMute(context) != AUDIO_SUCCESS) {
            return AUDIO_ERROR;
        }
    } else {
        // 1. change output device
        if (ChangeRouteInner(context, (int32_t)context->routeChangedDevice) != AUDIO_SUCCESS) {
            ALOGE("change route fail when resume");
            goto ERR;
        }
        // 2. restore flag and device
        context->routeChangedDevice = UNKNOWN;
    }
    context->state = RUNNING;
    return AUDIO_SUCCESS;

ERR:
    CHK_NULL_RETURN(context->callback, AUDIO_ERROR, "callback func is NULL");
    (void)context->callback(context->caller, AUDIO_EVENT_ERROR_OCCUR, NULL);
    return AUDIO_ERROR;
}

int32_t StartTrack(uintptr_t handle)
{
    TrackHandleContext *context = (TrackHandleContext *)handle;
    CHK_NULL_RETURN(context, AUDIO_ERROR, "track handle is NULL");
    MediaMutexLock(context->deviceChangeMutex);
    if (context->state == RUNNING) {
        MediaMutexUnLock(context->deviceChangeMutex);
        return AUDIO_SUCCESS;
    }
    if (context->state == STOP) {
        MediaMutexUnLock(context->deviceChangeMutex);
        return AUDIO_ERROR;
    }
    // 暂停后恢复分 主动暂停后恢复 和 被动暂停后恢复，
    // 被动暂停后恢复，需要重新查看权重变化;主动暂停后权重不会变化;
    // 由于目前还不能区分是哪种恢复，因此统一处理
    // 被动暂停后恢复典型场景分析:
    // 例如A1-A2混音，此时B抢占A1A2，待B结束，要恢复A1A2其中一个时(定为Ax)，Ax需要重新计算权重
    if (context->state == PAUSE) {
        // 蓝牙音乐中途接听蓝牙电话，通话中途手表去使能，此时手机和蓝牙设备同时和手表断开，
        // 蓝牙音乐恢复和输出设备切换从不同线程触发，同时访问底层蓝牙插件资源，需要加锁保证时序
        int32_t ret = ResumeTrack(context);
        MediaMutexUnLock(context->deviceChangeMutex);
        return ret;
    }
    // IDLE 状态要把 hal 创建起来
    int32_t ret = CreateAudioRenderHandle(context);
    if (ret != AUDIO_SUCCESS) {
        MediaMutexUnLock(context->deviceChangeMutex);
        return AUDIO_ERROR;
    }
    if (!CheckRenderValid(context->render)) {
        ALOGE("render is invalid");
        goto DESTROY_RENDER_HANDLE;
    }
    if (SetExtraData(context) != AUDIO_SUCCESS) {
        ALOGI("render call SetExTraData failed");
        goto DESTROY_RENDER_HANDLE;
    }
    ret = context->render->RegCallback((AudioHandle)context->render, OnRenderCallback, (void *)context);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("render call RegCallback failed");
        goto DESTROY_RENDER_HANDLE;
    }
    ret = context->render->control.Start((AudioHandle)context->render); // 只是在开始的时候调用start
    if (ret != AUDIO_SUCCESS) {
        goto DESTROY_RENDER_HANDLE;
    }
    context->state = RUNNING;
    if (SetVolumeAndSetMute(context) != AUDIO_SUCCESS) {
        goto DESTROY_RENDER_HANDLE;
    }
    MediaMutexUnLock(context->deviceChangeMutex);
    return AUDIO_SUCCESS;

DESTROY_RENDER_HANDLE:
    DestroyAudioRenderHandle(context);
    MediaMutexUnLock(context->deviceChangeMutex);
    return AUDIO_ERROR;
}

int32_t PauseTrack(uintptr_t handle)
{
    TrackHandleContext *context = (TrackHandleContext *)handle;
    CHK_NULL_RETURN(context, AUDIO_ERROR, "track handle is NULL");
    MediaMutexLock(context->deviceChangeMutex);
    if (context->state == PAUSE) {
        MediaMutexUnLock(context->deviceChangeMutex);
        return AUDIO_SUCCESS;
    }
    if (context->state == IDLE || context->state == STOP) {
        MediaMutexUnLock(context->deviceChangeMutex);
        return AUDIO_BAD_STATE;
    }
    if (context->render == NULL) {
        MediaMutexUnLock(context->deviceChangeMutex);
        return AUDIO_ERROR;
    }
    int32_t ret = context->render->control.Pause((AudioHandle)context->render);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("track[%d] pause failed 0x%x", context->streamId, ret);
        MediaMutexUnLock(context->deviceChangeMutex);
        return ret;
    }
    context->state = PAUSE;
    MediaMutexUnLock(context->deviceChangeMutex);
    return AUDIO_SUCCESS;
}

int32_t StopTrack(uintptr_t handle)
{
    TrackHandleContext *context = (TrackHandleContext *)handle;
    CHK_NULL_RETURN(context, AUDIO_ERROR, "track handle is NULL");
    MediaMutexLock(context->deviceChangeMutex);
    if (context->state == STOP) {
        MediaMutexUnLock(context->deviceChangeMutex);
        return AUDIO_SUCCESS;
    }
    if (context->state == IDLE) {
        MediaMutexUnLock(context->deviceChangeMutex);
        return AUDIO_BAD_STATE;
    }
    context->state = STOP;
    if (context->render == NULL) {
        MediaMutexUnLock(context->deviceChangeMutex);
        return AUDIO_ERROR;
    }
    int32_t ret = context->render->control.Stop((AudioHandle)context->render);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("track[%d] stop failed 0x%x", context->streamId, ret);
        MediaMutexUnLock(context->deviceChangeMutex);
        return ret;
    }
    MediaMutexUnLock(context->deviceChangeMutex);
    return AUDIO_SUCCESS;
}

int32_t SetTrackVolume(uintptr_t handle, float typeVolume, float mixWeight, float streamVolume)
{
    TrackHandleContext *context = (TrackHandleContext *)handle;
    CHK_NULL_RETURN(context, AUDIO_ERROR, "track handle is NULL");
    MediaMutexLock(context->deviceChangeMutex);
    if (context->state == IDLE || context->state == STOP) {
        MediaMutexUnLock(context->deviceChangeMutex);
        return AUDIO_BAD_STATE;
    }
    if (context->keyValueList != NULL) {
        if (SetExtraData(context) != AUDIO_SUCCESS) {
            ALOGI("set volume extra data failed!");
        }
    }
    float totalVolume = 0;
    // 做值的校验
    bool valid = IsVolumeValid(typeVolume) && IsVolumeValid(mixWeight);
    if (!valid) {
        ALOGE("track[%d] set track, but arg is bad", context->streamId);
        goto ERROR;
    }
    context->typeVolume = typeVolume;
    if (fabs(streamVolume - NOT_SET_STREAM_VOLUME) >= COMPARE_THRESHOLD) {
        if (!IsVolumeValid(streamVolume)) {
            ALOGE("track[%d] set track, but arg is bad", context->streamId);
            goto ERROR;
        }
        context->streamVolume = streamVolume;
    }

    // 计算最终音量，给入hal
    totalVolume = context->typeVolume * context->streamVolume * mixWeight;
    if (context->render != NULL && context->render->volume.SetVolume != NULL) { // not set to hal when idle
        int32_t ret = context->render->volume.SetVolume((AudioHandle)context->render, totalVolume);
        if (ret != AUDIO_SUCCESS) {
            ALOGE("track[%d] SetVolume failed 0x%x", context->streamId, ret);
            goto ERROR;
        }
    }
    MediaMutexUnLock(context->deviceChangeMutex);
    return AUDIO_SUCCESS;

ERROR:
    MediaMutexUnLock(context->deviceChangeMutex);
    return AUDIO_ERROR;
}

float GetTrackVolume(uintptr_t handle)
{
    TrackHandleContext *context = (TrackHandleContext *)handle;
    CHK_NULL_RETURN(context, AUDIO_ERROR, "track handle is NULL");
    float streamVol = context->streamVolume;
    return streamVol;
}

int32_t SetStreamPlayBackSpeed(uintptr_t handle, float speed)
{
    TrackHandleContext *context = (TrackHandleContext *)handle;
    CHK_NULL_RETURN(context, AUDIO_ERROR, "track handle is NULL");
    // 状态
    MediaMutexLock(context->deviceChangeMutex);
    if (context->state == STOP) {
        ALOGE("track[%d] is stop", context->streamId);
        MediaMutexUnLock(context->deviceChangeMutex);
        return AUDIO_ERROR;
    }
    // 调南向
    int32_t ret = context->render->SetRenderSpeed(context->render, speed);
    if (ret != AUDIO_SUCCESS) {
        MediaMutexUnLock(context->deviceChangeMutex);
        return AUDIO_ERROR;
    }
    MediaMutexUnLock(context->deviceChangeMutex);
    return AUDIO_SUCCESS;
}

int32_t StreamWriteTrack(uintptr_t handle, const uint8_t *buffer, uint64_t bufferSize, uint64_t *replyBytes)
{
    TrackHandleContext *context = (TrackHandleContext *)handle;
    CHK_NULL_RETURN(context, AUDIO_ERROR, "track handle is NULL");
    // 状态
    MediaMutexLock(context->deviceChangeMutex);
    if (context->state != RUNNING) {
        // 场景B打断场景A，策略结果是场景A业务暂停，由于中断结果上报和场景B业务启动是异步的，
        // 因此当场景B业务启动时，场景A业务可能还未响应暂停，此时流管理模块会检查老流状态，
        // 如果是运行态，会暂停该流，但是场景A播放数据轮转线程还在运行，会持续送数据下来，
        // 这种情况，日志打印不能是错误级别，最多是告警级别
        if (context->state == PAUSE) {
            ALOGW("track[%d] is pause, write failed", context->streamId);
        } else {
            ALOGE("track[%d] is neither running or pause, write failed", context->streamId);
        }
        *replyBytes = 0;
        MediaMutexUnLock(context->deviceChangeMutex);
        return AUDIO_ERROR;
    }
    // 调南向
    if (context->render == NULL) {
        MediaMutexUnLock(context->deviceChangeMutex);
        return AUDIO_ERROR;
    }
    context->eos = (buffer == NULL && bufferSize == 0) ? true : false;
    int32_t ret = context->render->RenderFrame(context->render, buffer, bufferSize, replyBytes);
    MediaMutexUnLock(context->deviceChangeMutex);
    return ret;
}

AudioStreamType GetStreamType(uintptr_t handle)
{
    TrackHandleContext *context = (TrackHandleContext *)handle;
    CHK_NULL_RETURN(context, AUDIO_STREAM_INVALID, "track handle is NULL");
    return context->streamInfo.streamType;
}

int32_t SetStreamMixWeight(uintptr_t handle, float setMixWeight)
{
    TrackHandleContext *context = (TrackHandleContext *)handle;
    CHK_NULL_RETURN(context, AUDIO_ERROR, "track handle is NULL");
    context->mixWeight = setMixWeight;
    return AUDIO_SUCCESS;
}

bool GetStreamMixWeight(uintptr_t handle, float *mixWeight)
{
    TrackHandleContext *context = (TrackHandleContext *)handle;
    CHK_NULL_RETURN(context, false, "track handle is NULL");
    *mixWeight = context->mixWeight;
    return true;
}

int32_t SetTrackMute(uintptr_t handle, bool mute)
{
    TrackHandleContext *context = (TrackHandleContext *)handle;
    CHK_NULL_RETURN(context, AUDIO_ERROR, "track handle is NULL");
    // 状态
    MediaMutexLock(context->deviceChangeMutex);
    if (context->state == STOP) {
        ALOGE("track[%d] state is wrong, mute fail", context->streamId);
        MediaMutexUnLock(context->deviceChangeMutex);
        return AUDIO_BAD_STATE;
    } else if (context->state == IDLE) {
        context->mute = mute;
        MediaMutexUnLock(context->deviceChangeMutex);
        return AUDIO_SUCCESS;
    }
    // 调南向
    int32_t ret = context->render->volume.SetMute((AudioHandle)context->render, mute);
    if (ret != AUDIO_SUCCESS) {
        MediaMutexUnLock(context->deviceChangeMutex);
        return AUDIO_ERROR;
    }
    context->mute = mute;
    MediaMutexUnLock(context->deviceChangeMutex);
    return AUDIO_SUCCESS;
}

int32_t GetStreamDeviceId(uintptr_t handle)
{
    TrackHandleContext *context = (TrackHandleContext *)handle;
    CHK_NULL_RETURN(context, AUDIO_ERROR, "track handle is NULL");
    int32_t deviceId = (context->userAudioOutDevice != 0) ? (int32_t)context->userAudioOutDevice :
                                                            (int32_t)context->defalutAudioOutDevice;
    return deviceId;
}

int32_t GetStreamChannelId(uintptr_t handle, uint32_t *channelId)
{
    TrackHandleContext *context = (TrackHandleContext *)handle;
    CHK_NULL_RETURN(context, AUDIO_ERROR, "track handle is NULL");
    MediaMutexLock(context->deviceChangeMutex);
    if (context->state != RUNNING) {
        MediaMutexUnLock(context->deviceChangeMutex);
        return AUDIO_BAD_STATE;
    }
    int32_t ret = context->render->attr.GetCurrentChannelId(context->render, channelId);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("Get channel id failed: 0x%x", ret);
        MediaMutexUnLock(context->deviceChangeMutex);
        return AUDIO_ERROR;
    }
    ALOGI("Channel id: 0x%x", *channelId);
    MediaMutexUnLock(context->deviceChangeMutex);
    return AUDIO_SUCCESS;
}

int32_t TrackAttachFrontend(uintptr_t handle, uint32_t frontendId)
{
    TrackHandleContext *context = (TrackHandleContext *)handle;
    CHK_NULL_RETURN(context, AUDIO_ERROR, "track handle is NULL");
    MediaMutexLock(context->deviceChangeMutex);
    if (context->state != RUNNING) {
        MediaMutexUnLock(context->deviceChangeMutex);
        return AUDIO_BAD_STATE;
    }

    char keyValuePair[TOTAL_STR_NUM] = { 0 };
    int convertRet = sprintf_s(keyValuePair, sizeof(keyValuePair), "%s = %u", "attachId", frontendId);
    if (convertRet == AUDIO_ERROR) {
        ALOGE("splice key=value str fail[%d]", convertRet);
        MediaMutexUnLock(context->deviceChangeMutex);
        return AUDIO_ERROR;
    }
    int32_t ret = context->render->attr.SetExtraParams((AudioHandle)context->render, keyValuePair);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("set front end id failed: 0x%x", ret);
        MediaMutexUnLock(context->deviceChangeMutex);
        return AUDIO_ERROR;
    }
    ALOGI("frontend Id: 0x%x", frontendId);
    MediaMutexUnLock(context->deviceChangeMutex);
    return AUDIO_SUCCESS;
}

static int32_t GetAudioTimeStamp(struct AudioTimeStamp *timestamp, const TrackHandleContext *context)
{
    if (context->state != RUNNING &&
        context->state != PAUSE) {
        return AUDIO_BAD_STATE;
    }
    uint64_t frames;
    if (context->render == NULL) {
        return AUDIO_ERROR;
    }
    int32_t ret = context->render->GetRenderPosition(context->render, &frames, timestamp);
    if (ret != AUDIO_SUCCESS) {
        return AUDIO_ERROR;
    }
    return AUDIO_SUCCESS;
}

int32_t GetStreamRenderPosition(uintptr_t handle, int64_t *position)
{
    TrackHandleContext *context = (TrackHandleContext *)handle;
    CHK_NULL_RETURN(context, AUDIO_ERROR, "track handle is NULL");
    MediaMutexLock(context->deviceChangeMutex);
    struct AudioTimeStamp timestamp = {};
    int32_t ret = GetAudioTimeStamp(&timestamp, context);
    if (ret != AUDIO_SUCCESS) {
        MediaMutexUnLock(context->deviceChangeMutex);
        return ret;
    }
    if (timestamp.tvSec >= (INT64_MAX / S2NS) ||
        timestamp.tvSec * S2NS >= (INT64_MAX - timestamp.tvNSec)) {
        *position = INT64_MAX;
        MediaMutexUnLock(context->deviceChangeMutex);
        return AUDIO_ERROR;
    }

    *position = timestamp.tvSec * S2NS + timestamp.tvNSec;
    MediaMutexUnLock(context->deviceChangeMutex);
    return AUDIO_SUCCESS;
}

int32_t GetStreamTimestamp(const uintptr_t handle, struct timespec *position)
{
    const TrackHandleContext *context = (const TrackHandleContext *)handle;
    CHK_NULL_RETURN(context, AUDIO_ERROR, "track handle is NULL");
    MediaMutexLock(context->deviceChangeMutex);
    struct AudioTimeStamp timestamp = {};
    int32_t ret = GetAudioTimeStamp(&timestamp, context);
    if (ret != AUDIO_SUCCESS) {
        MediaMutexUnLock(context->deviceChangeMutex);
        return ret;
    }
    position->tv_sec = (time_t)timestamp.tvSec;
    position->tv_nsec = (long)timestamp.tvNSec;
    MediaMutexUnLock(context->deviceChangeMutex);
    return AUDIO_SUCCESS;
}

int32_t TrackFlush(uintptr_t handle)
{
    TrackHandleContext *context = (TrackHandleContext *)handle;
    CHK_NULL_RETURN(context, AUDIO_ERROR, "track handle is NULL");
    // 状态
    MediaMutexLock(context->deviceChangeMutex);
    if (context->state != PAUSE) {
        ALOGE("track[%d] state is wrong, mute fail", context->streamId);
        MediaMutexUnLock(context->deviceChangeMutex);
        return AUDIO_ERROR;
    }
    // 调南向
    int32_t ret = context->render->control.Flush((AudioHandle)context->render);
    if (ret != AUDIO_SUCCESS) {
        MediaMutexUnLock(context->deviceChangeMutex);
        return AUDIO_ERROR;
    }
    MediaMutexUnLock(context->deviceChangeMutex);
    return AUDIO_SUCCESS;
}

StreamDebugInfo DumpTrackInfo(uintptr_t handle)
{
    TrackHandleContext *context = (TrackHandleContext *)handle;
    if (context == NULL) {
        StreamDebugInfo temp = {};
        return temp;
    }
    ALOGD("=================stream out info streamId:%d sessionId:%u begin======================",
        context->streamId, context->streamInfo.sessionID);

    uint32_t curDevice = (context->userAudioOutDevice != 0) ? context->userAudioOutDevice :
                                                              context->defalutAudioOutDevice;
    ALOGD("streamState[%s] stream type[%s] stream vol[%f] stream mixWeight[%f] stream out dev[0X%X]",
        StringState(context->state),
        StringType(context->streamInfo.streamType),
        context->streamVolume,
        context->mixWeight, curDevice);

    ALOGD("=================stream out info streamId:%d sessionId:%u end======================",
        context->streamId, context->streamInfo.sessionID);

    StreamDebugInfo inf = {
        context->state, context->streamId, context->streamInfo,
        context->streamVolume, context->mixWeight, curDevice
    };
    return inf;
}

int32_t SetTrackParam(uintptr_t handle, const char *keyValueList, size_t len)
{
    TrackHandleContext *context = (TrackHandleContext *)handle;
    CHK_NULL_RETURN(context, AUDIO_ERROR, "TrackHandleContext *context is NULL");
    CHK_NULL_RETURN(keyValueList, AUDIO_ERROR, "TrackHandleContext *keyValueList is NULL");
    if (len == 0) {
        ALOGE("len invalid ");
        return AUDIO_ERROR;
    }
    MediaMutexLock(context->deviceChangeMutex);
    if (context->keyValueList != NULL) {
        free(context->keyValueList);
        context->keyValueList = NULL;
    }
    context->keyValueList = malloc(len + 1);
    if (context->keyValueList == NULL) {
        ALOGE("malloc failed");
        MediaMutexUnLock(context->deviceChangeMutex);
        return AUDIO_ERROR;
    }
    int32_t ret = memcpy_s(context->keyValueList, len + 1, keyValueList, len);
    if (ret < 0) {
        ALOGE("memcpy_s failed");
        free(context->keyValueList);
        context->keyValueList = NULL;
        MediaMutexUnLock(context->deviceChangeMutex);
        return AUDIO_ERROR;
    }
    if (context->render != NULL) {
        ret = SetExtraData(context);
        if (ret != AUDIO_SUCCESS) {
            ALOGE("Set extra data failed! ret = %d", ret);
            MediaMutexUnLock(context->deviceChangeMutex);
            return ret;
        }
    }
    MediaMutexUnLock(context->deviceChangeMutex);
    return AUDIO_SUCCESS;
}

int32_t GetTrackParam(uintptr_t handle, char *keyValueList, size_t len)
{
    TrackHandleContext *context = (TrackHandleContext *)handle;
    CHK_NULL_RETURN(context, AUDIO_ERROR, "TrackHandleContext *context is NULL");
    CHK_NULL_RETURN(keyValueList, AUDIO_ERROR, "TrackHandleContext *keyValueList is NULL");
    if (len == 0) {
        ALOGE("len invalid ");
        return AUDIO_ERROR;
    }
    MediaMutexLock(context->deviceChangeMutex);
    if (context->render != NULL) {
        int32_t ret = context->render->attr.GetExtraParams((AudioHandle)context->render, keyValueList, (int32_t)len);
        if (ret != AUDIO_SUCCESS) {
            ALOGE("get extra data failed! ret = %d", ret);
            MediaMutexUnLock(context->deviceChangeMutex);
            return ret;
        }
    }
    MediaMutexUnLock(context->deviceChangeMutex);
    return AUDIO_SUCCESS;
}

int32_t DeactivateCorrespondingTrack(uintptr_t handle)
{
    TrackHandleContext *context = (TrackHandleContext *)handle;
    CHK_NULL_RETURN(context, AUDIO_ERROR, "TrackHandleContext *context is NULL");
    MediaMutexLock(context->deviceChangeMutex);
    if (context->state == STOP) {
        MediaMutexUnLock(context->deviceChangeMutex);
        return AUDIO_SUCCESS;
    }
    if (context->state == IDLE || context->state == PAUSE) {
        context->state = STOP;
    }
    if (context->state == RUNNING) {
        context->state = STOP;
        if (context->render == NULL) {
            MediaMutexUnLock(context->deviceChangeMutex);
            return AUDIO_ERROR;
        }
        int32_t ret = context->render->control.Stop((AudioHandle)context->render);
        if (ret != AUDIO_SUCCESS) {
            ALOGE("track[%d] stop failed 0x%x in deactive progress", context->streamId, ret);
            MediaMutexUnLock(context->deviceChangeMutex);
            return ret;
        }
    }
    MediaMutexUnLock(context->deviceChangeMutex);
    return AUDIO_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


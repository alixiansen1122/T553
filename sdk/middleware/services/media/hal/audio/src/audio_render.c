/*
* Copyright (c) @CompanyNameMagicTag. 2020-2020. All rights reserved.
* Description:audio create render
* Author: Media Software Group
* Create: 2020-08-11
*/

#include <stdlib.h>
#include <math.h>
#include "audio_internal.h"
#include "audio_port_plugin.h"

#define CONFIG_OUT_LATENCY_MS  100 // unit: ms

// 1 buffer: 8000(8kHz sample rate) * 2(bytes, PCM_16_BIT) * 1(channel)
// 1 frame: 1024(sample) * 2(bytes, PCM_16_BIT) * 1(channel)
#define CONFIG_FRAME_SIZE      (1024 * 2 * 1)
#define CONFIG_FRAME_COUNT     ((8000 * 2 * 1 + (CONFIG_FRAME_SIZE - 1)) / CONFIG_FRAME_SIZE)
#define STERO_TO_MONO_FACTOR 2

static int32_t OnTrackCallback(enum AudioCallbackType type, const void *param, uint32_t len, AudioHandle caller)
{
    MEDIA_HAL_UNUSED(len);
    MEDIA_HAL_UNUSED(param);
    CHK_NULL_RETURN(caller, MEDIA_HAL_INVALID_PARAM, "input param caller null");
    struct AudioHwRender *hwRender = (struct AudioHwRender *)caller;
    if (hwRender->renderCallback == NULL) {
        if (type == AUDIO_BUFFER_EMPTY) {
            return MEDIA_HAL_OK;
        }
        MEDIA_HAL_LOGE(MODULE_NAME, "renderCallback is NULL");
        return MEDIA_HAL_ERR;
    }
    return hwRender->renderCallback(type, NULL, hwRender->caller);
}

int32_t AudioRenderStart(AudioHandle handle)
{
    CHK_NULL_RETURN(handle, MEDIA_HAL_INVALID_PARAM, "input param handle null");

#ifdef ENABALE_VQE
    struct InvokeAttr invokeAttr = { 0 };
    enum VqeScene vqeScene = VQE_SCENE_AO_MUSIC; // todo The playback scenario needs to be differentiated.
#endif
    struct AudioHwRender *hwRender = (struct AudioHwRender *)handle;
    MediaHalMutexLock(hwRender->mutex);
    if (hwRender->started) {
        MediaHalMutexUnLock(hwRender->mutex);
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioRenderStart, have started");
        return MEDIA_HAL_OK;
    }
    TrackCallback trackCallback = OnTrackCallback;
    int32_t ret = hwRender->trackHandle->RegCallback(hwRender->trackHandle, trackCallback, (AudioHandle)hwRender);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioPortTrack RegCallback failed: 0x%x", ret);
        MediaHalMutexUnLock(hwRender->mutex);
        return ret;
    }
    ret = hwRender->trackHandle->EnableTrack(hwRender->trackHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioPortTrack EnableTrack failed: 0x%x", ret);
        MediaHalMutexUnLock(hwRender->mutex);
        return ret;
    }
    ret = hwRender->trackHandle->SetVolume(hwRender->trackHandle, AUDIO_DEFAULT_VOLUME);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioPortTrack SetVolume failed: 0x%x", ret);
    }
    hwRender->started = true;
    hwRender->paused = false;
#ifdef ENABALE_VQE
    invokeAttr.request = &vqeScene;
    ret = hwRender->trackHandle->Invoke(hwRender->trackHandle, INVOKE_ID_CONFIG_VQE, &invokeAttr);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioPortTrack Invoke INVOKE_ID_CONFIG_VQE failed: 0x%x", ret);
    }
#endif
    MediaHalMutexUnLock(hwRender->mutex);
    return MEDIA_HAL_OK;
}

int32_t AudioRenderStop(AudioHandle handle)
{
    CHK_NULL_RETURN(handle, MEDIA_HAL_INVALID_PARAM, "input param handle null");

    struct AudioHwRender *hwRender = (struct AudioHwRender *)handle;
    MediaHalMutexLock(hwRender->mutex);
    if (!hwRender->started) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioRenderStop, not started");
        MediaHalMutexUnLock(hwRender->mutex);
        return MEDIA_HAL_ERR;
    }
    int32_t ret = hwRender->trackHandle->DisableTrack(hwRender->trackHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioPortTrack DisableTrack failed 0x%x", ret);
    }
    hwRender->started = false;
    MediaHalMutexUnLock(hwRender->mutex);
    return ret;
}

int32_t AudioRenderPause(AudioHandle handle)
{
    CHK_NULL_RETURN(handle, MEDIA_HAL_INVALID_PARAM, "input param handle null");

    struct AudioHwRender *hwRender = (struct AudioHwRender *)handle;
    MediaHalMutexLock(hwRender->mutex);
    if (!hwRender->started) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioRenderPause, not started");
        MediaHalMutexUnLock(hwRender->mutex);
        return MEDIA_HAL_ERR;
    }
    if (hwRender->paused) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioRenderPause, current paused");
        MediaHalMutexUnLock(hwRender->mutex);
        return MEDIA_HAL_OK;
    }
    int32_t ret = hwRender->trackHandle->PauseTrack(hwRender->trackHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioPortTrack PuaseTrack failed 0x%x", ret);
        MediaHalMutexUnLock(hwRender->mutex);
        return ret;
    }
    hwRender->paused = true;
    MediaHalMutexUnLock(hwRender->mutex);
    return MEDIA_HAL_OK;
}

int32_t AudioRenderResume(AudioHandle handle)
{
    CHK_NULL_RETURN(handle, MEDIA_HAL_INVALID_PARAM, "input param handle null");

    struct AudioHwRender *hwRender = (struct AudioHwRender *)handle;
    MediaHalMutexLock(hwRender->mutex);
    if (!hwRender->started) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioRenderResume, not started");
        MediaHalMutexUnLock(hwRender->mutex);
        return MEDIA_HAL_ERR;
    }
    if (!hwRender->paused) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioRenderPause, current in play");
        MediaHalMutexUnLock(hwRender->mutex);
        return MEDIA_HAL_OK;
    }
    int32_t ret = hwRender->trackHandle->ResumeTrack(hwRender->trackHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "audio track resume failed: 0x%x", ret);
        MediaHalMutexUnLock(hwRender->mutex);
        return ret;
    }
    hwRender->paused = false;
    MediaHalMutexUnLock(hwRender->mutex);
    return MEDIA_HAL_OK;
}

int32_t AudioRenderFlush(AudioHandle handle)
{
    CHK_NULL_RETURN(handle, MEDIA_HAL_INVALID_PARAM, "input param handle null");

    struct AudioHwRender *hwRender = (struct AudioHwRender *)handle;
    MediaHalMutexLock(hwRender->mutex);
    if (!hwRender->started) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioRenderFlush, not started");
        MediaHalMutexUnLock(hwRender->mutex);
        return MEDIA_HAL_ERR;
    }
    int32_t ret = hwRender->trackHandle->FlushTrack(hwRender->trackHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioPortTrack FlushTrack failed 0x%x", ret);
    }
    MediaHalMutexUnLock(hwRender->mutex);
    return MEDIA_HAL_OK;
}

int32_t AudioRenderGetFrameSize(AudioHandle handle, uint64_t *size)
{
    CHK_NULL_RETURN(handle, MEDIA_HAL_INVALID_PARAM, "input param handle null");
    CHK_NULL_RETURN(size, MEDIA_HAL_INVALID_PARAM, "input param size null");

    struct AudioHwRender *hwRender = (struct AudioHwRender *)handle;
    MediaHalMutexLock(hwRender->mutex);
    *size = CONFIG_FRAME_SIZE;
    MediaHalMutexUnLock(hwRender->mutex);
    return MEDIA_HAL_OK;
}

int32_t AudioRenderGetFrameCount(AudioHandle handle, uint64_t *count)
{
    CHK_NULL_RETURN(handle, MEDIA_HAL_INVALID_PARAM, "input param handle null");
    CHK_NULL_RETURN(count, MEDIA_HAL_INVALID_PARAM, "input param count null");

    struct AudioHwRender *hwRender = (struct AudioHwRender *)handle;
    MediaHalMutexLock(hwRender->mutex);
    *count = CONFIG_FRAME_COUNT;
    MediaHalMutexUnLock(hwRender->mutex);
    return MEDIA_HAL_OK;
}

int32_t AudioRenderSetSampleAttributes(AudioHandle handle, const struct AudioSampleAttributes *attrs)
{
    CHK_NULL_RETURN(handle, MEDIA_HAL_INVALID_PARAM, "input param handle null");
    CHK_NULL_RETURN(attrs, MEDIA_HAL_INVALID_PARAM, "input param attrs null");

    struct AudioHwRender *hwRender = (struct AudioHwRender *)handle;
    MediaHalMutexLock(hwRender->mutex);
    hwRender->attrs.format = attrs->format;
    hwRender->attrs.sampleRate = attrs->sampleRate;
    hwRender->attrs.channelCount = attrs->channelCount;
    MediaHalMutexUnLock(hwRender->mutex);
    return MEDIA_HAL_OK;
}

int32_t AudioRenderGetSampleAttributes(AudioHandle handle, struct AudioSampleAttributes *attrs)
{
    CHK_NULL_RETURN(handle, MEDIA_HAL_INVALID_PARAM, "input param handle null");
    CHK_NULL_RETURN(attrs, MEDIA_HAL_INVALID_PARAM, "input param attrs null");

    struct AudioHwRender *hwRender = (struct AudioHwRender *)handle;
    MediaHalMutexLock(hwRender->mutex);
    attrs->format = hwRender->attrs.format;
    attrs->sampleRate = hwRender->attrs.sampleRate;
    attrs->channelCount = hwRender->attrs.channelCount;
    MediaHalMutexUnLock(hwRender->mutex);
    return MEDIA_HAL_OK;
}

int32_t AudioRenderGetCurrentChannelId(AudioHandle handle, uint32_t *channelId)
{
    CHK_NULL_RETURN(handle, MEDIA_HAL_INVALID_PARAM, "input param handle null");
    CHK_NULL_RETURN(channelId, MEDIA_HAL_INVALID_PARAM, "input param channelId null");

    struct AudioHwRender *hwRender = (struct AudioHwRender *)handle;
    MediaHalMutexLock(hwRender->mutex);
    struct InvokeAttr invokeAttr = {};
    invokeAttr.request = channelId;
    invokeAttr.requestBytes = (uint32_t)sizeof(uint32_t);
    int32_t ret = hwRender->trackHandle->Invoke(hwRender->trackHandle, INVOKE_ID_GET_CHANNEL_ID, &invokeAttr);
    if (ret != MEDIA_HAL_OK || invokeAttr.replyBytes != sizeof(uint32_t)) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioPortTrack Invoke INVOKE_ID_GET_CHANNEL_ID failed: 0x%x", ret);
        MediaHalMutexUnLock(hwRender->mutex);
        return ret;
    }
    MediaHalMutexUnLock(hwRender->mutex);
    MEDIA_HAL_LOGI(MODULE_NAME, "channelId: 0x%x", *channelId);
    return MEDIA_HAL_OK;
}

int32_t AudioRenderSetExtraParams(AudioHandle handle, const char *keyValueList)
{
    CHK_NULL_RETURN(handle, MEDIA_HAL_INVALID_PARAM, "input param handle null");
    CHK_NULL_RETURN(keyValueList, MEDIA_HAL_INVALID_PARAM, "input param keyValueList null");

    struct AudioHwRender *hwRender = (struct AudioHwRender *)handle;
    MediaHalMutexLock(hwRender->mutex);
    struct InvokeAttr invokeAttr = {};
    invokeAttr.request = (void *)keyValueList;
    int32_t ret = hwRender->trackHandle->Invoke(hwRender->trackHandle, INVOKE_ID_SET_EXTRA_PARAMS, &invokeAttr);
    if (ret != MEDIA_HAL_OK) {
        MediaHalMutexUnLock(hwRender->mutex);
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioPortTrack Invoke INVOKE_ID_SET_EXTRA_PARAMS failed: 0x%x", ret);
        return ret;
    }
    MediaHalMutexUnLock(hwRender->mutex);

    MEDIA_HAL_LOGI(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

int32_t AudioRenderGetExtraParams(AudioHandle handle, char *keyValueList, int32_t listLenth)
{
    CHK_NULL_RETURN(handle, MEDIA_HAL_INVALID_PARAM, "input param handle null");
    CHK_NULL_RETURN(keyValueList, MEDIA_HAL_INVALID_PARAM, "input param keyValueList null");

    struct AudioHwRender *hwRender = (struct AudioHwRender *)handle;
    MediaHalMutexLock(hwRender->mutex);
    struct InvokeAttr invokeAttr = {};
    invokeAttr.request = (void *)keyValueList;
    invokeAttr.requestBytes = (uint64_t)listLenth;
    int32_t ret = hwRender->trackHandle->Invoke(hwRender->trackHandle, INVOKE_ID_GET_EXTRA_PARAMS, &invokeAttr);
    if (ret != MEDIA_HAL_OK) {
        MediaHalMutexUnLock(hwRender->mutex);
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioPortTrack Invoke INVOKE_ID_GET_EXTRA_PARAMS failed: 0x%x", ret);
        return ret;
    }
    MediaHalMutexUnLock(hwRender->mutex);

    MEDIA_HAL_LOGI(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}


int32_t AudioRenderCheckSceneCapability(AudioHandle handle, const struct AudioSceneDescriptor *scene,
    bool *supported)
{
    CHK_NULL_RETURN(handle, MEDIA_HAL_INVALID_PARAM, "input param handle null");
    CHK_NULL_RETURN(scene, MEDIA_HAL_INVALID_PARAM, "input param scene null");
    CHK_NULL_RETURN(supported, MEDIA_HAL_INVALID_PARAM, "input param supported null");

    struct AudioHwRender *hwRender = (struct AudioHwRender *)handle;
    MediaHalMutexLock(hwRender->mutex);
    *supported = false;
    if (scene->desc.portId == 0 && scene->desc.pins == PIN_OUT_SPEAKER) {
        *supported = true;
    }
    MediaHalMutexUnLock(hwRender->mutex);
    return MEDIA_HAL_OK;
}

int32_t AudioRenderSelectScene(AudioHandle handle, const struct AudioSceneDescriptor *scene)
{
    CHK_NULL_RETURN(handle, MEDIA_HAL_INVALID_PARAM, "input param handle null");
    CHK_NULL_RETURN(scene, MEDIA_HAL_INVALID_PARAM, "input param scene null");

    struct AudioHwRender *hwRender = (struct AudioHwRender *)handle;
    MediaHalMutexLock(hwRender->mutex);
    if (scene->desc.portId != 0 || scene->desc.pins != PIN_OUT_SPEAKER) {
        MediaHalMutexUnLock(hwRender->mutex);
        return MEDIA_HAL_INVALID_PARAM;
    }
    MediaHalMutexUnLock(hwRender->mutex);
    MEDIA_HAL_LOGV(MODULE_NAME, "select scene %d/%d success", scene->desc.portId, scene->desc.pins);
    return MEDIA_HAL_OK;
}

int32_t AudioRenderSetMute(AudioHandle handle, bool mute)
{
    CHK_NULL_RETURN(handle, MEDIA_HAL_INVALID_PARAM, "input param handle null");

    struct AudioHwRender *hwRender = (struct AudioHwRender *)handle;
    MediaHalMutexLock(hwRender->mutex);
    int32_t ret = hwRender->trackHandle->SetMute(hwRender->trackHandle, mute);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioPortTrack SetMute failed: 0x%x", ret);
    }

    MediaHalMutexUnLock(hwRender->mutex);
    return ret;
}

int32_t AudioRenderGetMute(AudioHandle handle, bool *mute)
{
    CHK_NULL_RETURN(handle, MEDIA_HAL_INVALID_PARAM, "input param handle null");
    CHK_NULL_RETURN(mute, MEDIA_HAL_INVALID_PARAM, "input param mute null");

    struct AudioHwRender *hwRender = (struct AudioHwRender *)handle;
    MediaHalMutexLock(hwRender->mutex);
    int32_t ret = hwRender->trackHandle->GetMute(hwRender->trackHandle, mute);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioPortTrack GetMute failed: 0x%x", ret);
    }

    MediaHalMutexUnLock(hwRender->mutex);
    return MEDIA_HAL_OK;
}

int32_t AudioRenderSetVolume(AudioHandle handle, float volume)
{
    CHK_NULL_RETURN(handle, MEDIA_HAL_INVALID_PARAM, "input param handle null");

    struct AudioHwRender *hwRender = (struct AudioHwRender *)handle;
    MediaHalMutexLock(hwRender->mutex);
    if (!(hwRender->started)) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioRenderSetVolume, not started");
        MediaHalMutexUnLock(hwRender->mutex);
        return MEDIA_HAL_ERR;
    }
    if (volume < MEDIA_HAL_MIN_VOLUME || volume > MEDIA_HAL_MAX_VOLUME) {
        MEDIA_HAL_LOGE(MODULE_NAME, "volume: %f is invalid", volume);
        MediaHalMutexUnLock(hwRender->mutex);
        return MEDIA_HAL_INVALID_PARAM;
    }
    int32_t ret = hwRender->trackHandle->SetVolume(hwRender->trackHandle, volume);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioPortTrack SetVolume failed: 0x%x", ret);
    }

    MediaHalMutexUnLock(hwRender->mutex);
    return ret;
}

int32_t AudioRenderGetVolume(AudioHandle handle, float *volume)
{
    CHK_NULL_RETURN(handle, MEDIA_HAL_INVALID_PARAM, "input param handle null");
    CHK_NULL_RETURN(volume, MEDIA_HAL_INVALID_PARAM, "input param volume null");

    struct AudioHwRender *hwRender = (struct AudioHwRender *)handle;
    MediaHalMutexLock(hwRender->mutex);
    int32_t ret = hwRender->trackHandle->GetVolume(hwRender->trackHandle, volume);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioPortTrack GetVolume failed: 0x%x", ret);
    }

    MediaHalMutexUnLock(hwRender->mutex);
    return ret;
}

int32_t AudioRenderGetGainThreshold(AudioHandle handle, float *min, float *max)
{
    CHK_NULL_RETURN(handle, MEDIA_HAL_INVALID_PARAM, "input param handle null");
    struct AudioHwRender *hwRender = (struct AudioHwRender *)handle;
    MediaHalMutexLock(hwRender->mutex);
    UNUSED(min);
    UNUSED(max);
    MEDIA_HAL_LOGE(MODULE_NAME, "AudioRenderGetGainThreshold not support now");
    MediaHalMutexUnLock(hwRender->mutex);
    return MEDIA_HAL_OK;
}

int32_t AudioRenderGetGain(AudioHandle handle, float *gain)
{
    CHK_NULL_RETURN(handle, MEDIA_HAL_INVALID_PARAM, "input param handle null");
    struct AudioHwRender *hwRender = (struct AudioHwRender *)handle;
    MediaHalMutexLock(hwRender->mutex);
    UNUSED(gain);
    MEDIA_HAL_LOGE(MODULE_NAME, "AudioRenderGetGain not support now");
    MediaHalMutexUnLock(hwRender->mutex);
    return MEDIA_HAL_OK;
}

int32_t AudioRenderSetGain(AudioHandle handle, float gain)
{
    CHK_NULL_RETURN(handle, MEDIA_HAL_INVALID_PARAM, "input param handle null");
    struct AudioHwRender *hwRender = (struct AudioHwRender *)handle;
    MediaHalMutexLock(hwRender->mutex);
    UNUSED(gain);
    MEDIA_HAL_LOGE(MODULE_NAME, "AudioRenderSetGain not support now");
    MediaHalMutexUnLock(hwRender->mutex);
    return MEDIA_HAL_OK;
}

int32_t AudioRenderGetLatency(struct AudioRender *render, uint32_t *ms)
{
    CHK_NULL_RETURN(render, MEDIA_HAL_INVALID_PARAM, "input param render null");
    CHK_NULL_RETURN(ms, MEDIA_HAL_INVALID_PARAM, "input param ms null");

    struct AudioHwRender *hwRender = (struct AudioHwRender *)render;
    MediaHalMutexLock(hwRender->mutex);
    *ms = CONFIG_OUT_LATENCY_MS;
    MediaHalMutexUnLock(hwRender->mutex);
    return MEDIA_HAL_OK;
}

int32_t AudioRenderRenderFrame(struct AudioRender *render, const void *frame,
    uint64_t requestBytes, uint64_t *replyBytes)
{
    CHK_NULL_RETURN(render, MEDIA_HAL_INVALID_PARAM, "input param render null");
    CHK_NULL_RETURN(replyBytes, MEDIA_HAL_INVALID_PARAM, "input param replyBytes null");

    if ((frame != NULL && requestBytes == 0) || (frame == NULL && requestBytes != 0)) {
        MEDIA_HAL_LOGE(MODULE_NAME, "frame addr and size mismatched");
        return MEDIA_HAL_INVALID_PARAM;
    }
    int32_t ret;
    struct AudioHwRender *hwRender = (struct AudioHwRender *)render;
    MediaHalMutexLock(hwRender->mutex);
    if (!hwRender->started) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioRenderRenderFrame, not started");
        MediaHalMutexUnLock(hwRender->mutex);
        return MEDIA_HAL_ERR;
    }
    if (hwRender->steroToMono) {
        uint64_t monoDataLen = requestBytes / STERO_TO_MONO_FACTOR;
        uint64_t monoReplyBytes = 0;
        ret = hwRender->trackHandle->SendFrame(hwRender->trackHandle, (const uint8_t *)frame, monoDataLen,
            &monoReplyBytes);
        *replyBytes = (monoDataLen == monoReplyBytes) ? requestBytes : 0;
    } else {
        ret = hwRender->trackHandle->SendFrame(hwRender->trackHandle, (const uint8_t *)frame, requestBytes, replyBytes);
    }
    MediaHalMutexUnLock(hwRender->mutex);
    return ret;
}

int32_t AudioRenderGetRenderPosition(struct AudioRender *render, uint64_t *frames, struct AudioTimeStamp *time)
{
    CHK_NULL_RETURN(render, MEDIA_HAL_INVALID_PARAM, "input param render null");
    CHK_NULL_RETURN(frames, MEDIA_HAL_INVALID_PARAM, "input param frames null");
    CHK_NULL_RETURN(time, MEDIA_HAL_INVALID_PARAM, "input param time null");

    struct AudioHwRender *hwRender = (struct AudioHwRender *)render;
    MediaHalMutexLock(hwRender->mutex);
    if (!hwRender->started) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioRenderGetRenderPosition, not started");
        MediaHalMutexUnLock(hwRender->mutex);
        return MEDIA_HAL_ERR;
    }
    int32_t ret = hwRender->trackHandle->GetPosition(hwRender->trackHandle, frames, time);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioPortTrack GetPosition failed: 0x%x", ret);
    }
    MediaHalMutexUnLock(hwRender->mutex);
    return ret;
}

int32_t AudioRenderSetRenderSpeed(struct AudioRender *render, float speed)
{
    CHK_NULL_RETURN(render, MEDIA_HAL_INVALID_PARAM, "input param render null");

    struct AudioHwRender *hwRender = (struct AudioHwRender *)render;
    MediaHalMutexLock(hwRender->mutex);
    hwRender->speed = speed;
    MediaHalMutexUnLock(hwRender->mutex);
    return MEDIA_HAL_OK;
}

int32_t AudioRenderGetRenderSpeed(struct AudioRender *render, float *speed)
{
    CHK_NULL_RETURN(render, MEDIA_HAL_INVALID_PARAM, "input param render null");
    CHK_NULL_RETURN(speed, MEDIA_HAL_INVALID_PARAM, "input param speed null");

    struct AudioHwRender *hwRender = (struct AudioHwRender *)render;
    MediaHalMutexLock(hwRender->mutex);
    *speed = hwRender->speed;
    MediaHalMutexUnLock(hwRender->mutex);
    return MEDIA_HAL_OK;
}

int32_t AudioRenderSetChannelMode(struct AudioRender *render, enum AudioChannelMode mode)
{
    CHK_NULL_RETURN(render, MEDIA_HAL_INVALID_PARAM, "input param render null");

    struct AudioHwRender *hwRender = (struct AudioHwRender *)render;
    MediaHalMutexLock(hwRender->mutex);
    hwRender->mode = mode;
    MediaHalMutexUnLock(hwRender->mutex);
    return MEDIA_HAL_OK;
}

int32_t AudioRenderGetChannelMode(struct AudioRender *render, enum AudioChannelMode *mode)
{
    CHK_NULL_RETURN(render, MEDIA_HAL_INVALID_PARAM, "input param render null");
    CHK_NULL_RETURN(mode, MEDIA_HAL_INVALID_PARAM, "input param mode null");

    struct AudioHwRender *hwRender = (struct AudioHwRender *)render;
    MediaHalMutexLock(hwRender->mutex);
    *mode = hwRender->mode;
    MediaHalMutexUnLock(hwRender->mutex);
    return MEDIA_HAL_OK;
}

int32_t AudioRenderRegCallback(struct AudioRender *render, RenderCallback callback, void *cookie)
{
    CHK_NULL_RETURN(render, MEDIA_HAL_INVALID_PARAM, "input param render null");
    CHK_NULL_RETURN(callback, MEDIA_HAL_INVALID_PARAM, "input param callback null");
    CHK_NULL_RETURN(cookie, MEDIA_HAL_INVALID_PARAM, "input param cookie null");

    struct AudioHwRender *hwRender = (struct AudioHwRender *)render;
    MediaHalMutexLock(hwRender->mutex);
    hwRender->renderCallback = callback;
    hwRender->caller = cookie;
    MediaHalMutexUnLock(hwRender->mutex);
    return MEDIA_HAL_OK;
}

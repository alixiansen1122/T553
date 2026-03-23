/*
* Copyright (c) @CompanyNameMagicTag. 2020-2020. All rights reserved.
* Description:Audio Capture
* Author: Media Software Group
* Create: 2020-08-11
*/
#include <stdlib.h>
#include "audio_internal.h"
#include "audio_port_plugin.h"
#include "audio_capture.h"

#define CONFIG_IN_GAIN_MIN     (-50.0) // unit: db
#define CONFIG_IN_GAIN_MAX     6.0 // unit: db

// 1 buffer: 8000(8kHz sample rate) * 2(bytes, PCM_16_BIT) * 1(channel)
// 1 frame: 1024(sample) * 2(bytes, PCM_16_BIT) * 1(channel)
#define CONFIG_FRAME_SIZE      (1024 * 2 * 1)

int32_t AudioCaptureStart(AudioHandle handle)
{
    CHK_NULL_RETURN(handle, MEDIA_HAL_INVALID_PARAM, "input param handle null");

    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)handle;
    MediaHalMutexLock(hwCapture->mutex);
    if (hwCapture->started) {
        MediaHalMutexUnLock(hwCapture->mutex);
        MEDIA_HAL_LOGE(MODULE_NAME, "hwCapture have started");
        return MEDIA_HAL_OK;
    }
    int32_t ret = hwCapture->trackHandle->EnableTrack(hwCapture->trackHandle);
    if (ret != MEDIA_HAL_OK) {
        MediaHalMutexUnLock(hwCapture->mutex);
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioPortTrack EnableTrack failed: 0x%x", ret);
        return ret;
    }
    hwCapture->started = true;
#ifdef SUPPORT_VQE // Dependent product
    enum VqeScene vqeScene;
    if (hwCapture->attrs.type == AUDIO_IN_MEDIA) {
        vqeScene = VQE_SCENE_AI_RECORD;
    } else if (hwCapture->attrs.type == AUDIO_IN_COMMUNICATION) {
        vqeScene = VQE_SCENE_AI_TALK;
    }
    struct InvokeAttr invokeAttr = {};
    invokeAttr.request = &vqeScene;
    ret = hwCapture->trackHandle->Invoke(hwCapture->trackHandle, INVOKE_ID_CONFIG_VQE, &invokeAttr);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioPortTrack Invoke INVOKE_ID_CONFIG_INPUT_VQE failed: 0x%x", ret);
    }
#endif
    MediaHalMutexUnLock(hwCapture->mutex);
    return MEDIA_HAL_OK;
}

int32_t AudioCaptureStop(AudioHandle handle)
{
    CHK_NULL_RETURN(handle, MEDIA_HAL_INVALID_PARAM, "input param handle null");

    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)handle;
    MediaHalMutexLock(hwCapture->mutex);
    if (!hwCapture->started) {
        MediaHalMutexUnLock(hwCapture->mutex);
        MEDIA_HAL_LOGE(MODULE_NAME, "hwCapture not started");
        return MEDIA_HAL_ERR;
    }
    int32_t ret = hwCapture->trackHandle->DisableTrack(hwCapture->trackHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioPortTrack DisableTrack failed: 0x%x", ret);
    }
    hwCapture->started = false;
    MediaHalMutexUnLock(hwCapture->mutex);
    return MEDIA_HAL_OK;
}

int32_t AudioCapturePause(AudioHandle handle)
{
    UNUSED(handle);
    MEDIA_HAL_LOGI(MODULE_NAME, "AudioCapturePause not support");
    return MEDIA_HAL_OK;
}

int32_t AudioCaptureResume(AudioHandle handle)
{
    CHK_NULL_RETURN(handle, MEDIA_HAL_INVALID_PARAM, "input param handle null");

    UNUSED(handle);
    MEDIA_HAL_LOGI(MODULE_NAME, "AudioCaptureResume not support");
    return MEDIA_HAL_OK;
}

int32_t AudioCaptureFlush(AudioHandle handle)
{
    CHK_NULL_RETURN(handle, MEDIA_HAL_INVALID_PARAM, "input param handle null");

    UNUSED(handle);
    MEDIA_HAL_LOGI(MODULE_NAME, "AudioCaptureFlush not support");
    return MEDIA_HAL_OK;
}

int32_t AudioCaptureGetFrameSize(AudioHandle handle, uint64_t *size)
{
    CHK_NULL_RETURN(handle, MEDIA_HAL_INVALID_PARAM, "input param handle null");
    CHK_NULL_RETURN(size, MEDIA_HAL_INVALID_PARAM, "input param size null");

    UNUSED(handle);

    *size = CONFIG_FRAME_SIZE;
    return MEDIA_HAL_OK;
}

int32_t AudioCaptureGetFrameCount(AudioHandle handle, uint64_t *count)
{
    CHK_NULL_RETURN(handle, MEDIA_HAL_INVALID_PARAM, "input param handle null");
    CHK_NULL_RETURN(count, MEDIA_HAL_INVALID_PARAM, "input param count null");

    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)handle;
    MediaHalMutexLock(hwCapture->mutex);
    int32_t ret = hwCapture->trackHandle->GetFrameCount(hwCapture->trackHandle, count);
    if (ret != MEDIA_HAL_OK) {
        MediaHalMutexUnLock(hwCapture->mutex);
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioPortTrack GetFrameCount failed: 0x%x", ret);
        return ret;
    }
    MEDIA_HAL_LOGI(MODULE_NAME, "GetFrameCount: %llu", *count);
    MediaHalMutexUnLock(hwCapture->mutex);
    return MEDIA_HAL_OK;
}

int32_t AudioCaptureSetSampleAttributes(AudioHandle handle, const struct AudioSampleAttributes *attrs)
{
    CHK_NULL_RETURN(handle, MEDIA_HAL_INVALID_PARAM, "input param handle null");
    CHK_NULL_RETURN(attrs, MEDIA_HAL_INVALID_PARAM, "input param attrs null");

    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)handle;
    MediaHalMutexLock(hwCapture->mutex);
    hwCapture->attrs.format = attrs->format;
    hwCapture->attrs.sampleRate = attrs->sampleRate;
    hwCapture->attrs.channelCount = attrs->channelCount;
    MediaHalMutexUnLock(hwCapture->mutex);
    return MEDIA_HAL_OK;
}

int32_t AudioCaptureGetSampleAttributes(AudioHandle handle, struct AudioSampleAttributes *attrs)
{
    CHK_NULL_RETURN(handle, MEDIA_HAL_INVALID_PARAM, "input param handle null");
    CHK_NULL_RETURN(attrs, MEDIA_HAL_INVALID_PARAM, "input param attrs null");

    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)handle;
    MediaHalMutexLock(hwCapture->mutex);
    attrs->format = hwCapture->attrs.format;
    attrs->sampleRate = hwCapture->attrs.sampleRate;
    attrs->channelCount = hwCapture->attrs.channelCount;
    MediaHalMutexUnLock(hwCapture->mutex);
    return MEDIA_HAL_OK;
}

int32_t AudioCaptureGetCurrentChannelId(AudioHandle handle, uint32_t *channelId)
{
    CHK_NULL_RETURN(handle, MEDIA_HAL_INVALID_PARAM, "input param handle null");
    CHK_NULL_RETURN(channelId, MEDIA_HAL_INVALID_PARAM, "input param channelId null");

    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)handle;
    MediaHalMutexLock(hwCapture->mutex);
    struct InvokeAttr invokeAttr = {};
    invokeAttr.request = channelId;
    invokeAttr.requestBytes = (uint64_t)sizeof(uint32_t);
    int32_t ret = hwCapture->trackHandle->Invoke(hwCapture->trackHandle, INVOKE_ID_GET_CHANNEL_ID, &invokeAttr);
    if (ret != MEDIA_HAL_OK || invokeAttr.replyBytes != (uint64_t)sizeof(uint32_t)) {
        MediaHalMutexUnLock(hwCapture->mutex);
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioPortTrack Invoke INVOKE_ID_GET_CHANNEL_ID failed: 0x%x", ret);
        return ret;
    }
    MediaHalMutexUnLock(hwCapture->mutex);
    MEDIA_HAL_LOGI(MODULE_NAME, "channelId: 0x%x", *channelId);
    return MEDIA_HAL_OK;
}

int32_t AudioCaptureSetExtraParams(AudioHandle handle, const char *keyValueList)
{
    CHK_NULL_RETURN(handle, MEDIA_HAL_INVALID_PARAM, "input param handle null");
    CHK_NULL_RETURN(keyValueList, MEDIA_HAL_INVALID_PARAM, "input param keyValueList null");

    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)handle;
    MediaHalMutexLock(hwCapture->mutex);
    struct InvokeAttr invokeAttr = {};
    invokeAttr.request = (void *)keyValueList;
    int32_t ret = hwCapture->trackHandle->Invoke(hwCapture->trackHandle, INVOKE_ID_SET_EXTRA_PARAMS, &invokeAttr);
    if (ret != MEDIA_HAL_OK) {
        MediaHalMutexUnLock(hwCapture->mutex);
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioPortTrack Invoke INVOKE_ID_SET_EXTRA_PARAMS failed: 0x%x", ret);
        return ret;
    }
    MediaHalMutexUnLock(hwCapture->mutex);

    MEDIA_HAL_LOGI(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

int32_t AudioCaptureGetExtraParams(AudioHandle handle, char *keyValueList, int32_t listLenth)
{
    CHK_NULL_RETURN(handle, MEDIA_HAL_INVALID_PARAM, "input param handle null");
    CHK_NULL_RETURN(keyValueList, MEDIA_HAL_INVALID_PARAM, "input param keyValueList null");

    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)handle;
    MediaHalMutexLock(hwCapture->mutex);
    struct InvokeAttr invokeAttr = {};
    invokeAttr.request = (void *)keyValueList;
    invokeAttr.replyBytes = (uint64_t)listLenth;
    int32_t ret = hwCapture->trackHandle->Invoke(hwCapture->trackHandle, INVOKE_ID_GET_EXTRA_PARAMS, &invokeAttr);
    if (ret != MEDIA_HAL_OK) {
        MediaHalMutexUnLock(hwCapture->mutex);
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioPortTrack Invoke INVOKE_ID_GET_EXTRA_PARAMS failed: 0x%x", ret);
        return ret;
    }
    MediaHalMutexUnLock(hwCapture->mutex);

    MEDIA_HAL_LOGI(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

int32_t AudioCaptureCheckSceneCapability(AudioHandle handle, const struct AudioSceneDescriptor *scene,
                                         bool *supported)
{
    CHK_NULL_RETURN(handle, MEDIA_HAL_INVALID_PARAM, "input param handle null");
    CHK_NULL_RETURN(scene, MEDIA_HAL_INVALID_PARAM, "input param scene null");
    CHK_NULL_RETURN(supported, MEDIA_HAL_INVALID_PARAM, "input param supported null");

    UNUSED(handle);

    *supported = false;
    if (scene->desc.portId == 0 && scene->desc.pins == PIN_IN_MIC) {
        *supported = true;
    }
    return MEDIA_HAL_OK;
}

int32_t AudioCaptureSelectScene(AudioHandle handle, const struct AudioSceneDescriptor *scene)
{
    CHK_NULL_RETURN(handle, MEDIA_HAL_INVALID_PARAM, "input param handle null");
    CHK_NULL_RETURN(scene, MEDIA_HAL_INVALID_PARAM, "input param scene null");

    UNUSED(handle);

    if (scene->desc.portId != 0 || scene->desc.pins != PIN_IN_MIC) {
        return MEDIA_HAL_INVALID_PARAM;
    }
    MEDIA_HAL_LOGV(MODULE_NAME, "select scene %d/%d success", scene->desc.portId, scene->desc.pins);
    return MEDIA_HAL_OK;
}

int32_t AudioCaptureSetMute(AudioHandle handle, bool mute)
{
    CHK_NULL_RETURN(handle, MEDIA_HAL_INVALID_PARAM, "input param handle null");

    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)handle;
    MediaHalMutexLock(hwCapture->mutex);
    int32_t ret = hwCapture->trackHandle->SetMute(hwCapture->trackHandle, mute);
    if (ret != MEDIA_HAL_OK) {
        MediaHalMutexUnLock(hwCapture->mutex);
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioPortTrack SetMute failed: 0x%x", ret);
        return ret;
    }
    MediaHalMutexUnLock(hwCapture->mutex);
    return MEDIA_HAL_OK;
}

int32_t AudioCaptureGetMute(AudioHandle handle, bool *mute)
{
    CHK_NULL_RETURN(handle, MEDIA_HAL_INVALID_PARAM, "input param handle null");
    CHK_NULL_RETURN(mute, MEDIA_HAL_INVALID_PARAM, "input param mute null");

    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)handle;
    MediaHalMutexLock(hwCapture->mutex);
    int32_t ret = hwCapture->trackHandle->GetMute(hwCapture->trackHandle, mute);
    if (ret != MEDIA_HAL_OK) {
        MediaHalMutexUnLock(hwCapture->mutex);
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioPortTrack GetMute failed: 0x%x", ret);
        return ret;
    }
    MediaHalMutexUnLock(hwCapture->mutex);
    return MEDIA_HAL_OK;
}

int32_t AudioCaptureSetVolume(AudioHandle handle, float volume)
{
    CHK_NULL_RETURN(handle, MEDIA_HAL_INVALID_PARAM, "input param handle null");

    MEDIA_HAL_LOGI(MODULE_NAME, "volume:%f linear:%d", volume, (int32_t)volume);
    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)handle;
    MediaHalMutexLock(hwCapture->mutex);
    int32_t ret = hwCapture->trackHandle->SetVolume(hwCapture->trackHandle, volume);
    if (ret != MEDIA_HAL_OK) {
        MediaHalMutexUnLock(hwCapture->mutex);
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioPortTrack SetVolume failed: 0x%x", ret);
        return ret;
    }
    MediaHalMutexUnLock(hwCapture->mutex);
    return MEDIA_HAL_OK;
}

int32_t AudioCaptureGetVolume(AudioHandle handle, float *volume)
{
    CHK_NULL_RETURN(handle, MEDIA_HAL_INVALID_PARAM, "input param handle null");
    CHK_NULL_RETURN(volume, MEDIA_HAL_INVALID_PARAM, "input param volume null");

    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)handle;
    MediaHalMutexLock(hwCapture->mutex);
    int32_t ret = hwCapture->trackHandle->GetVolume(hwCapture->trackHandle, volume);
    if (ret != MEDIA_HAL_OK) {
        MediaHalMutexUnLock(hwCapture->mutex);
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioPortTrack GetVolume failed: 0x%x", ret);
        return ret;
    }
    MediaHalMutexUnLock(hwCapture->mutex);
    return MEDIA_HAL_OK;
}

int32_t AudioCaptureGetGainThreshold(AudioHandle handle, float *min, float *max)
{
    CHK_NULL_RETURN(handle, MEDIA_HAL_INVALID_PARAM, "input param handle null");
    CHK_NULL_RETURN(min, MEDIA_HAL_INVALID_PARAM, "input param min null");
    CHK_NULL_RETURN(max, MEDIA_HAL_INVALID_PARAM, "input param max null");

    UNUSED(handle);

    *min = CONFIG_IN_GAIN_MIN;
    *max = CONFIG_IN_GAIN_MAX;
    return MEDIA_HAL_OK;
}

int32_t AudioCaptureGetGain(AudioHandle handle, float *gain)
{
    CHK_NULL_RETURN(handle, MEDIA_HAL_INVALID_PARAM, "input param handle null");
    CHK_NULL_RETURN(gain, MEDIA_HAL_INVALID_PARAM, "input param gain null");

    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)handle;
    *gain = hwCapture->gain;
    return MEDIA_HAL_OK;
}

int32_t AudioCaptureSetGain(AudioHandle handle, float gain)
{
    CHK_NULL_RETURN(handle, MEDIA_HAL_INVALID_PARAM, "input param handle null");

    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)handle;
    MediaHalMutexLock(hwCapture->mutex);
    hwCapture->gain = gain;
    MediaHalMutexUnLock(hwCapture->mutex);
    return MEDIA_HAL_OK;
}

int32_t AudioCaptureCaptureFrame(struct AudioCapture *capture, void *frame,
    uint64_t requestBytes, uint64_t *replyBytes)
{
    CHK_NULL_RETURN(capture, MEDIA_HAL_INVALID_PARAM, "input param capture null");
    CHK_NULL_RETURN(frame, MEDIA_HAL_INVALID_PARAM, "input param frame null");
    CHK_NULL_RETURN(replyBytes, MEDIA_HAL_INVALID_PARAM, "input param replyBytes null");

    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)capture;
    MediaHalMutexLock(hwCapture->mutex);
    if (!hwCapture->started) {
        MediaHalMutexUnLock(hwCapture->mutex);
        MEDIA_HAL_LOGE(MODULE_NAME, "hwCapture not started");
        return MEDIA_HAL_ERR;
    }
    int32_t ret = hwCapture->trackHandle->AcquireFrame(hwCapture->trackHandle, frame, requestBytes, replyBytes);
    if (ret != MEDIA_HAL_OK) {
        if (ret != MEDIA_HAL_ERR_AGAIN &&
            ret != MEDIA_HAL_NOT_SUPPORT_GET_MULT_FRAME) {
            MEDIA_HAL_LOGE(MODULE_NAME, "AudioPortTrack AcquireFrame failed: 0x%x", ret);
        }
        MediaHalMutexUnLock(hwCapture->mutex);
        return ret;
    }
    hwCapture->frames++;
    MediaHalMutexUnLock(hwCapture->mutex);
    return MEDIA_HAL_OK;
}

int32_t AudioCaptureGetCapturePosition(struct AudioCapture *capture, uint64_t *frames, struct AudioTimeStamp *time)
{
    CHK_NULL_RETURN(capture, MEDIA_HAL_INVALID_PARAM, "input param capture null");
    CHK_NULL_RETURN(frames, MEDIA_HAL_INVALID_PARAM, "input param frames null");
    CHK_NULL_RETURN(time, MEDIA_HAL_INVALID_PARAM, "input param time null");

    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)capture;
    MediaHalMutexLock(hwCapture->mutex);
    if (!hwCapture->started) {
        MediaHalMutexUnLock(hwCapture->mutex);
        MEDIA_HAL_LOGE(MODULE_NAME, "hwCapture not started");
        return MEDIA_HAL_ERR;
    }
    int32_t ret = hwCapture->trackHandle->GetPosition(hwCapture->trackHandle, frames, time);
    if (ret != MEDIA_HAL_OK) {
        MediaHalMutexUnLock(hwCapture->mutex);
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioPortTrack GetPosition failed: 0x%x", ret);
        return ret;
    }
    MediaHalMutexUnLock(hwCapture->mutex);
    return MEDIA_HAL_OK;
}

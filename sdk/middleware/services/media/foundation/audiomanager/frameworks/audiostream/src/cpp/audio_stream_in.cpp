/*
* Copyright (c) CompanyNameMagicTag. 2021-2021. All rights reserved.
* Description: audio stream interfaces
* Author: Media Software Group
* Create: 2021-04-21
*/

#include "audio_stream_in.h"
#include "stream_manager.h"
#include "source_handle.h"
#include "audio_utils.h"
#include "audio_errors.h"
#include "media_thread_adapt.h"

#define LOG_MODULE_NAME "AudioStreamIn"

namespace Audio {
static void ConvertCaptureInfo(const CapturerInputConfig &src, AudioStreamInfo &dst)
{
    dst.streamType = src.streamType;
    dst.format = src.audioFormat;
    dst.sampleRate = src.sampleRate;
    dst.channelCount = src.channelCount;
    dst.sessionID = src.sessionID;
    dst.channelMask = AUDIO_CHANNEL_MODE_NONE;
    dst.audioStreamFlag = AUDIO_STREAM_FLAG_NONE;
    dst.linkDir = src.linkDir;
}

AudioStreamIn::AudioStreamIn()
    : streamHandle_((uintptr_t)nullptr),
      mutex(MediaMutexCreate(nullptr))
{
}

AudioStreamIn::~AudioStreamIn()
{
    MediaMutexDestroy(&mutex);
}

int32_t AudioStreamIn::Init(const CapturerInputConfig &config, CapturerCallback cbk, const void *caller)
{
    AudioStreamInfo info;
    ConvertCaptureInfo(config, info);
    MediaMutexLock(mutex);
    uintptr_t streamHandle = StreamMgrCreateStreamIn(&info);
    if (streamHandle == 0) {
        MediaMutexUnLock(mutex);
        ALOGE("create stream fail");
        return AUDIO_ERROR;
    }

    streamHandle_ = streamHandle;
    SetSourceCallback(streamHandle_, cbk, caller);
    MediaMutexUnLock(mutex);
    return AUDIO_SUCCESS;
}

int32_t AudioStreamIn::GetCurrentDeviceId() const
{
    int32_t deviceId;
    MediaMutexLock(mutex);
    deviceId = GetSourceDeviceId(streamHandle_);
    MediaMutexUnLock(mutex);
    return deviceId;
}

int32_t AudioStreamIn::GetCurrentChannelId(uint32_t &channelId) const
{
    MediaMutexLock(mutex);
    if (GetSourceChannelId(streamHandle_, &channelId) != AUDIO_SUCCESS) {
        ALOGE("get channel id fail");
        MediaMutexUnLock(mutex);
        return AUDIO_ERROR;
    }
    MediaMutexUnLock(mutex);
    return AUDIO_SUCCESS;
}

int32_t AudioStreamIn::AttachBackend(uint32_t &backendId) const
{
    MediaMutexLock(mutex);
    if (SourceAttachBackend(streamHandle_, backendId) != AUDIO_SUCCESS) {
        ALOGE("attach backend fail");
        MediaMutexUnLock(mutex);
        return AUDIO_ERROR;
    }
    MediaMutexUnLock(mutex);
    return AUDIO_SUCCESS;
}

int32_t AudioStreamIn::GetTimestamp(struct timespec &timestamp, Timebase base) const
{
    AUDIO_UNUSED(base);
    MediaMutexLock(mutex);
    if (GetSourceTimestamp(streamHandle_, &timestamp) != AUDIO_SUCCESS) {
        ALOGE("get timestamp fail");
        MediaMutexUnLock(mutex);
        return AUDIO_ERROR;
    }
    MediaMutexUnLock(mutex);
    return AUDIO_SUCCESS;
}

int32_t AudioStreamIn::Start() const
{
    MediaMutexLock(mutex);
    if (StartSource(streamHandle_) != 0) {
        ALOGE("start stream fail");
        MediaMutexUnLock(mutex);
        return AUDIO_ERROR;
    }
    MediaMutexUnLock(mutex);
    return AUDIO_SUCCESS;
}

int32_t AudioStreamIn::Stop() const
{
    MediaMutexLock(mutex);
    if (StopSource(streamHandle_) != 0) {
        ALOGE("stop stream fail");
        MediaMutexUnLock(mutex);
        return AUDIO_ERROR;
    }
    MediaMutexUnLock(mutex);
    return AUDIO_SUCCESS;
}

int32_t AudioStreamIn::SetInputDevice(int32_t deviceId, AudioRendererConfig &renderConfig) const
{
    AUDIO_UNUSED(renderConfig);
    MediaMutexLock(mutex);
    if (SetSourceInputDevice(streamHandle_, deviceId, nullptr) != 0) {
        ALOGE("set out device to track fail");
        MediaMutexUnLock(mutex);
        return AUDIO_ERROR;
    }
    MediaMutexUnLock(mutex);
    return AUDIO_SUCCESS;
}

int32_t AudioStreamIn::ObtainBuffer(CapturerBuffer &buffer, bool isBlocking) const
{
    int32_t ret;
    AUDIO_UNUSED(isBlocking);
    MediaMutexLock(mutex);
    ret = ObtainSourceBuffer(streamHandle_, &buffer);
    MediaMutexUnLock(mutex);
    return ret;
}

int32_t AudioStreamIn::Release() const
{
    ALOGD("in");
    MediaMutexLock(mutex);
    if (StreamMgrDestroyStreamIn(streamHandle_) != 0) {
        ALOGE("destroy stream fail");
        MediaMutexUnLock(mutex);
        return AUDIO_ERROR;
    }
    ALOGD("out");
    MediaMutexUnLock(mutex);
    return AUDIO_SUCCESS;
}

int32_t AudioStreamIn::SetParam(const char *keyValueList, int32_t len) const
{
    MediaMutexLock(mutex);
    int32_t ret = SetSourceParam(streamHandle_, keyValueList, len);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("AudioStreamIn::SetParam call SetSourceParam failed, ret:%d.", ret);
        MediaMutexUnLock(mutex);
        return AUDIO_ERROR;
    }
    MediaMutexUnLock(mutex);
    return AUDIO_SUCCESS;
}

int32_t AudioStreamIn::GetParam(char *keyValueList, int32_t len)
{
    MediaMutexLock(mutex);
    int32_t ret = GetSourceParam(streamHandle_, keyValueList, len);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("AudioStreamIn::GetParam call GetSourceParam failed, ret:%d.", ret);
        MediaMutexUnLock(mutex);
        return AUDIO_ERROR;
    }
    MediaMutexUnLock(mutex);
    return AUDIO_SUCCESS;
}
}

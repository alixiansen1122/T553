/*
* Copyright (c) CompanyNameMagicTag. 2021-2021. All rights reserved.
* Description: audio stream interfaces
* Author: Media Software Group
* Create: 2021-04-21
*/

#include "audio_stream_out.h"
#include "stream_manager.h"
#include "track_handle.h"
#include "audio_utils.h"
#include "audio_errors.h"
#include "media_thread_adapt.h"

#define LOG_MODULE_NAME "AudioStreamOut"

namespace Audio {
static void ConvertStreamInfo(AudioRendererConfig &src, AudioStreamInfo &dst)
{
    dst.streamType = src.streamType;
    dst.format = src.audioFormat;
    dst.sampleRate = src.sampleRate;
    dst.channelCount = (int32_t)(src.channelCount);
    dst.sessionID = (uint32_t)(src.sessionID);
    dst.channelMask = AUDIO_CHANNEL_MODE_NONE;
    dst.audioStreamFlag = AUDIO_STREAM_FLAG_NONE;
    dst.linkDir = src.linkDir;
    dst.sampleFmt = src.sampleFmt;
}

AudioStreamOut::AudioStreamOut()
    : streamHandle_((uintptr_t)nullptr),
      mutex(MediaMutexCreate(nullptr))
{
}

AudioStreamOut::~AudioStreamOut()
{
    MediaMutexDestroy(&mutex);
}

int32_t AudioStreamOut::Init(AudioRendererConfig &config, RendererCallback cbk, const void *caller)
{
    ALOGD("in");
    AudioStreamInfo info = {};
    ConvertStreamInfo(config, info);
    MediaMutexLock(mutex);
    uintptr_t streamHandle = StreamMgrCreateStreamOut(&info);
    if (streamHandle == 0) {
        MediaMutexUnLock(mutex);
        ALOGE("create streamHandle fail");
        return AUDIO_ERROR;
    }

    streamHandle_ = streamHandle;
    SetCallback(streamHandle_, cbk, caller);
    MediaMutexUnLock(mutex);
    ALOGD("out");

    return AUDIO_SUCCESS;
}

int32_t AudioStreamOut::SetOutputDevice(int32_t deviceId, AudioRendererConfig &renderConfig) const
{
    AUDIO_UNUSED(renderConfig);
    ALOGD("in");
    MediaMutexLock(mutex);
    if (SetTrackOutputDevice(streamHandle_, deviceId, nullptr) != 0) {
        ALOGE("set out device to track fail");
        MediaMutexUnLock(mutex);
        return AUDIO_ERROR;
    }
    ALOGD("out");
    MediaMutexUnLock(mutex);
    return AUDIO_SUCCESS;
}

int32_t AudioStreamOut::Play() const
{
    ALOGD("in");
    MediaMutexLock(mutex);
    int32_t ret = StartTrack(streamHandle_);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("start stream fail");
        MediaMutexUnLock(mutex);
        return ret;
    }
    ALOGD("out");
    MediaMutexUnLock(mutex);
    return AUDIO_SUCCESS;
}

int32_t AudioStreamOut::Pause() const
{
    ALOGD("in");
    MediaMutexLock(mutex);
    if (PauseTrack(streamHandle_) != 0) {
        ALOGE("start stream fail");
        MediaMutexUnLock(mutex);
        return AUDIO_ERROR;
    }
    ALOGD("out");
    MediaMutexUnLock(mutex);
    return AUDIO_SUCCESS;
}

int32_t AudioStreamOut::Stop() const
{
    ALOGD("in");
    MediaMutexLock(mutex);
    if (StopTrack(streamHandle_) != 0) {
        ALOGE("start stream fail");
        MediaMutexUnLock(mutex);
        return AUDIO_ERROR;
    }
    ALOGD("out");
    MediaMutexUnLock(mutex);
    return AUDIO_SUCCESS;
}

int32_t AudioStreamOut::Release() const
{
    ALOGD("in");
    MediaMutexLock(mutex);
    if (StreamMgrDestroyStreamOut(streamHandle_) != 0) {
        ALOGE("destroy stream fail");
        MediaMutexUnLock(mutex);
        return AUDIO_ERROR;
    }
    ALOGD("out");
    MediaMutexUnLock(mutex);
    return AUDIO_SUCCESS;
}

int32_t AudioStreamOut::StreamWrite(const uint8_t *buffer, uint64_t userSize, uint64_t &writtenSize,
    AudioRendererConfig &renderConfig) const
{
    AUDIO_UNUSED(renderConfig);
    MediaMutexLock(mutex);
    int32_t ret = StreamWriteTrack(streamHandle_, buffer, userSize, &writtenSize);
    if (writtenSize != userSize) {
        ALOGD("write es data may be wrong, user[%llu],written[%llu]",
            userSize, writtenSize);
    }
    if (ret != 0) {
        writtenSize = 0;
        MediaMutexUnLock(mutex);
        return AUDIO_ERROR;
    }
    MediaMutexUnLock(mutex);
    return AUDIO_SUCCESS;
}

int32_t AudioStreamOut::SetMute(bool mute) const
{
    MediaMutexLock(mutex);
    int32_t ret = SetTrackMute(streamHandle_, mute);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("set track mute fail");
        MediaMutexUnLock(mutex);
        return AUDIO_ERROR;
    }
    MediaMutexUnLock(mutex);
    return AUDIO_SUCCESS;
}

int32_t AudioStreamOut::SetVolume(float volume) const
{
    MediaMutexLock(mutex);
    AudioStreamType type = GetStreamType(streamHandle_);
    float typeVolume;
    if (!StreamMgrGetTypeVolume(type, &typeVolume)) {
        ALOGE("get type volume fail");
        MediaMutexUnLock(mutex);
        return AUDIO_ERROR;
    }

    float mixWeight;
    if (!GetStreamMixWeight(streamHandle_, &mixWeight)) {
        ALOGE("get mix weight fail");
        MediaMutexUnLock(mutex);
        return AUDIO_ERROR;
    }

    int32_t ret = SetTrackVolume(streamHandle_, typeVolume, mixWeight, volume);
    if (ret != 0) {
        ALOGE("destroy stream fail");
        MediaMutexUnLock(mutex);
        return AUDIO_ERROR;
    }
    MediaMutexUnLock(mutex);
    return AUDIO_SUCCESS;
}

int32_t AudioStreamOut::SetPlayBackSpeed(float speed) const
{
    MediaMutexLock(mutex);
    if (SetStreamPlayBackSpeed(streamHandle_, speed) != 0) {
        ALOGE("set stream speed fail");
        MediaMutexUnLock(mutex);
        return AUDIO_ERROR;
    }
    MediaMutexUnLock(mutex);
    return AUDIO_SUCCESS;
}

int32_t AudioStreamOut::GetCurrentDeviceId() const
{
    int32_t deviceId;
    MediaMutexLock(mutex);
    deviceId = GetStreamDeviceId(streamHandle_);
    MediaMutexUnLock(mutex);
    return deviceId;
}

int32_t AudioStreamOut::GetCurrentChannelId(uint32_t &channelId) const
{
    MediaMutexLock(mutex);
    if (GetStreamChannelId(streamHandle_, &channelId) != AUDIO_SUCCESS) {
        ALOGE("get channel id fail");
        MediaMutexUnLock(mutex);
        return AUDIO_ERROR;
    }
    MediaMutexUnLock(mutex);
    return AUDIO_SUCCESS;
}

int32_t AudioStreamOut::AttachFrontend(uint32_t &frontendId) const
{
    MediaMutexLock(mutex);
    if (TrackAttachFrontend(streamHandle_, frontendId) != AUDIO_SUCCESS) {
        ALOGE("attach frontend fail");
        MediaMutexUnLock(mutex);
        return AUDIO_ERROR;
    }
    MediaMutexUnLock(mutex);
    return AUDIO_SUCCESS;
}

int32_t AudioStreamOut::GetPosition(int64_t &position) const
{
    MediaMutexLock(mutex);
    int32_t ret = GetStreamRenderPosition(streamHandle_, &position);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("get position fail");
        MediaMutexUnLock(mutex);
        return ret;
    }
    MediaMutexUnLock(mutex);
    return AUDIO_SUCCESS;
}

int32_t AudioStreamOut::GetTimestamp(struct timespec &timeStamp) const
{
    MediaMutexLock(mutex);
    if (GetStreamTimestamp(streamHandle_, &timeStamp) != 0) {
        ALOGE("get timestamp fail");
        MediaMutexUnLock(mutex);
        return AUDIO_ERROR;
    }
    MediaMutexUnLock(mutex);
    return AUDIO_SUCCESS;
}

int32_t AudioStreamOut::Flush() const
{
    MediaMutexLock(mutex);
    if (TrackFlush(streamHandle_) != AUDIO_SUCCESS) {
        ALOGE("flush fail");
        MediaMutexUnLock(mutex);
        return AUDIO_ERROR;
    }
    MediaMutexUnLock(mutex);
    return AUDIO_SUCCESS;
}

StreamDebugInfo AudioStreamOut::DumpInfo() const
{
    StreamDebugInfo info;
    MediaMutexLock(mutex);
    info = DumpTrackInfo(streamHandle_);
    MediaMutexUnLock(mutex);
    return info;
}

int32_t AudioStreamOut::SetParam(const char *keyValueList, int32_t len) const
{
    MediaMutexLock(mutex);
    int32_t ret = SetTrackParam(streamHandle_, keyValueList, len);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("AudioStreamOut::SetParam call SetTrackParam failed, ret:%d.", ret);
        MediaMutexUnLock(mutex);
        return AUDIO_ERROR;
    }
    MediaMutexUnLock(mutex);
    return AUDIO_SUCCESS;
}

int32_t AudioStreamOut::GetParam(char *keyValueList, int32_t len)
{
    MediaMutexLock(mutex);
    int32_t ret = GetTrackParam(streamHandle_, keyValueList, len);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("AudioStreamOut::GetParam call GetTrackParam failed, ret:%d.", ret);
        MediaMutexUnLock(mutex);
        return AUDIO_ERROR;
    }
    MediaMutexUnLock(mutex);
    return AUDIO_SUCCESS;
}
}

/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
#include "audio_source.h"
#include "media_errors.h"
#include "media_log.h"

using namespace OHOS::Media;

namespace OHOS {
namespace Audio {
AudioSource::AudioSource()
    : initialized_(false),
      started_(false),
      streamIn_(nullptr)
{
    MEDIA_DEBUG_LOG("ctor");
}

AudioSource::~AudioSource()
{
    if (initialized_) {
        Release();
    }
    MEDIA_DEBUG_LOG("dtor");
}

int32_t AudioSource::InitCheck()
{
    if (!initialized_) {
        MEDIA_ERR_LOG("not initialized");
        return ERR_ILLEGAL_STATE;
    }
    return SUCCESS;
}

bool AudioSource::GetMinFrameCount(int32_t sampleRate, int32_t channelCount,
    AudioCodecFormat audioFormat, size_t &frameCount)
{
    if (sampleRate <= 0 || channelCount <= 0 || audioFormat < AUDIO_DEFAULT || audioFormat >= FORMAT_INVALID) {
        MEDIA_ERR_LOG("invalid params sampleRate:%d channelCount:%d audioFormat:%d",
            sampleRate, channelCount, audioFormat);
        return false;
    }
    frameCount = 0;
    return true;
}

uint64_t AudioSource::GetFrameCount()
{
    int32_t ret = InitCheck();
    if (ret != SUCCESS) {
        return ret;
    }
    uint64_t frameCount = 1024; // not supported currently, default: 1024
    return frameCount;
}

int32_t AudioSource::EnumDeviceBySourceType(AudioSourceType inputSource, std::vector<AudioDeviceDesc> &devices)
{
    uint32_t i;
    bool isMatch = false;

    std::vector<AudioDeviceInfo> availableInputDevices = amIntance_.GetDevices(INPUT_DEVICES_FLAG);
    for (i = 0; i < availableInputDevices.size(); i++) {
        if (inputSource == AUDIO_MIC || inputSource == AUDIO_SOURCE_DEFAULT) {
            if (availableInputDevices[i].device == IN_BUILTIN_MIC) {
                isMatch = true;
                break;
            }
        }
    }

    if (!isMatch) {
        MEDIA_ERR_LOG("invalid inputSource:%d", inputSource);
        return ERR_INVALID_PARAM;
    }

    AudioDeviceDesc deviceDesc;
    deviceDesc.deviceId = availableInputDevices[i].device;
    deviceDesc.inputSourceType = AUDIO_MIC;
    devices.push_back(deviceDesc);

    return SUCCESS;
}

int32_t AudioSource::Initialize(const AudioSourceConfig &config)
{
    streamIn_ = std::make_shared<AudioStreamIn>();
    if (streamIn_.get() == nullptr) {
        MEDIA_ERR_LOG("AudioStreamIn new failed");
        return ERR_UNKNOWN;
    }

    CapturerInputConfig captureConfig = {};
    captureConfig.audioFormat = config.audioFormat;
    captureConfig.streamType = (config.streamUsage == TYPE_DEFAULT) ? AUDIO_STREAM_VOICE_RECORD : config.streamUsage;
    captureConfig.sampleRate = config.sampleRate;
    captureConfig.sessionID = config.sessionID;
    captureConfig.channelCount = config.channelCount;
    captureConfig.bitWidth = config.bitWidth;

    int32_t ret = streamIn_->Init(captureConfig);
    if (ret != SUCCESS) {
        MEDIA_ERR_LOG("AudioStreamIn init failed");
        return ret;
    }

    initialized_ = true;
    return SUCCESS;
}

int32_t AudioSource::SetInputDevice(uint32_t deviceId)
{
    MEDIA_UNUSED(deviceId);
    MEDIA_WARNING_LOG("WARNING: SetInputDevice not supported");
    return SUCCESS;
}

int32_t AudioSource::GetCurrentDeviceId(uint32_t &deviceId)
{
    if (!started_) {
        MEDIA_ERR_LOG("AudioSource not Start");
        return ERR_ILLEGAL_STATE;
    }

    int32_t ret = streamIn_->GetCurrentChannelId(deviceId);
    if (ret != SUCCESS) {
        MEDIA_ERR_LOG("GetCurrentChannelId failed:0x%x", ret);
        return ret;
    }

    MEDIA_INFO_LOG("ChannelId:0x%x", deviceId);
    return SUCCESS;
}

int32_t AudioSource::Start()
{
    int32_t ret = InitCheck();
    if (ret != SUCCESS) {
        return ret;
    }
    ret = streamIn_->Start();
    if (ret != SUCCESS) {
        MEDIA_ERR_LOG("streamIn_ Start failed:0x%x", ret);
        return ret;
    }

    started_ = true;
    return SUCCESS;
}

int32_t AudioSource::ReadFrame(AudioFrame &frame, bool isBlockingRead)
{
    MEDIA_UNUSED(isBlockingRead);
    if (!started_) {
        MEDIA_ERR_LOG("AudioSource not Start");
        return ERR_INVALID_READ;
    }

    CapturerBuffer capBuf = {};
    capBuf.data = frame.buffer;
    capBuf.size = frame.bufferLen;
    capBuf.frameCount = frame.frames;
    int32_t readlen = streamIn_->ObtainBuffer(capBuf, true);
    if (readlen == ERR_INVALID_READ) {
        MEDIA_ERR_LOG("streamIn_::ObtainBuffer failed:0x%x", readlen);
        return ERR_INVALID_READ;
    } else if (readlen == ERR_RETRY_READ) {
        return ERR_RETRY_READ;
    }

    int32_t ret = streamIn_->GetTimestamp(frame.time, ::Audio::Timebase::MONOTONIC);
    if (ret != SUCCESS) {
        MEDIA_ERR_LOG("streamIn_::GetTimestamp failed:0x%x", ret);
        return ERR_INVALID_READ;
    }

    return readlen;
}

int32_t AudioSource::Stop()
{
    MEDIA_INFO_LOG("AudioSource::Stop");
    if (!started_) {
        MEDIA_ERR_LOG("AudioSource not Start");
        return ERR_ILLEGAL_STATE;
    }

    int32_t ret = streamIn_->Stop();
    if (ret != SUCCESS) {
        MEDIA_ERR_LOG("Stop failed:0x%x", ret);
        return ret;
    }

    started_ = false;
    return SUCCESS;
}

int32_t AudioSource::Release()
{
    int32_t ret = InitCheck();
    if (ret != SUCCESS) {
        return ret;
    }
    if (streamIn_.get() != nullptr) {
        streamIn_->Release();
        streamIn_.reset();
    }

    initialized_ = false;
    MEDIA_INFO_LOG("AudioSource Released");
    return SUCCESS;
}

}  // namespace Audio
}

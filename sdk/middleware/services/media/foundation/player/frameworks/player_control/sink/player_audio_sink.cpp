/*
 * Copyright (c) 2020-2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "player_audio_sink.h"
#include <unistd.h>
#include <cmath>
#include <sys/time.h>
#include "audio_manager.h"
#include "media_log.h"
#include "player_check.h"
#include "audio_errors.h"
#include "media_mem.h"

using namespace Audio;
namespace OHOS {
namespace Media {
const int32_t HALF_FACTOR = 2;
const int32_t MAX_QUEUE_BUF_NUM = 32;
#ifndef UNSUPPORT_AUDIO_DECODER
const int32_t AUDIO_SAMPLE_WIDTH_BYTE = 2;
#endif
const int32_t MAX_EXTRA_PARAM_SIZE = 1024;
// current maximum scenario bluetooth music playback retry count(converted to time: 200 * 10ms = 2s)
const int32_t MAX_CONTINOUS_REND_FRAME_FULL_COUNT = 200;

AudioSink::AudioSink()
    : started_(false),
      paused_(false),
      eosSended_(false),
      rendFrameCnt_(0),
      lastRendFrameCnt_(0),
      continousRendFrameFullCnt_(0),
      syncHdl_(nullptr),
      rendStartTime_(-1),
      lastRendPts_(AV_INVALID_PTS),
#ifdef MINI_MEMORY_SUPPORT
      lastRenderPosition_(0),
      hasSendEosFrame_(false),
#endif
      lastRendSysTimeMs_(-1),
      renderDelay_(0),
      leftVolume_(0.0f),
      rightVolume_(0.0f),
      eosPts_(AV_INVALID_PTS),
      receivedEos_(false),
      mutex_(MediaMutexCreate(nullptr)),
      outStream_(nullptr)
      
{
    ResetRendStartTime();
#ifdef UNSUPPORT_AUDIO_DECODER
    esFrameCacheQue_.clear();
    esFrameReleaseQue_.clear();
#else
    frameCacheQue_.clear();
    frameReleaseQue_.clear();
#endif
    callBack_.onEventCallback = nullptr;
    callBack_.priv = nullptr;
    attr_.sinkType = SINK_TYPE_BUT;
    attr_.trackId = 0;
    attr_.audAttr.format = 0;
    attr_.audAttr.sampleFmt = 0;
    attr_.audAttr.sampleRate = 0;
    attr_.audAttr.channel = 0;
    attr_.audAttr.volume = 0.0;
}

AudioSink::~AudioSink()
{
    DeInit();
}

void AudioSink::DeInit()
{
    started_ = false;
#ifdef UNSUPPORT_AUDIO_DECODER
    if (esFrameCacheQue_.size() != 0 || esFrameReleaseQue_.size() != 0) {
        MEDIA_ERR_LOG("frames should be released before DeInit!!!");
    }
#else
    if (frameCacheQue_.size() != 0 || frameReleaseQue_.size() != 0) {
        MEDIA_ERR_LOG("frames should be released before DeInit!!!");
    }
#endif
    if (outStream_ != nullptr) {
        outStream_->Release();
        outStream_ = nullptr;
    }
#ifdef UNSUPPORT_AUDIO_DECODER
    esFrameCacheQue_.clear();
    esFrameReleaseQue_.clear();
#else
    frameCacheQue_.clear();
    frameReleaseQue_.clear();
#endif
    MediaMutexDestroy(&mutex_);
}

int32_t AudioSink::OnRenderCallback(const void *caller, AudioCallbackEventType event, const void *info)
{
    (void)info;
    CHECK_NULL_RETURN(caller, SINK_INVALID_PARAM, "render callback param caller is nullptr");
    const AudioSink *sink = static_cast<const AudioSink *>(caller);

    switch (event) {
        case AUDIO_EVENT_RENDER_EOS:
            return sink->callBack_.onEventCallback(sink->callBack_.priv, EVNET_AUDIO_PLAY_EOS, 0, 0);
        case AUDIO_EVENT_ERROR_OCCUR:
            return sink->callBack_.onEventCallback(sink->callBack_.priv, EVNET_AUDIO_RUNNING_ERR, 0, 0);
        case AUDIO_EVENT_BUFFER_EMPTY:
            return sink->callBack_.onEventCallback(sink->callBack_.priv, EVNET_AUDIO_BUFFER_EMPTY, 0, 0);
        case AUDIO_EVENT_OUTPUT_DEVICE_CHANGED:
            return sink->callBack_.onEventCallback(sink->callBack_.priv, EVNET_OUTPUT_DEVICE_CHANGED, 0, 0);
        default:
            MEDIA_ERR_LOG("Invalid event type: %d", event);
            return SINK_INVALID_PARAM;
    }
}

int32_t AudioSink::Init(const SinkAttr &atrr)
{
    attr_ = atrr;
    outStream_ = std::make_shared<AudioStreamOut>();
    if (outStream_ == nullptr || outStream_.get() == nullptr) {
        MEDIA_ERR_LOG("AudioStreamOut new failed");
        return SINK_OPEN_STREAM_FAILED;
    }
    AudioRendererConfig renderConfig;
    renderConfig.streamType = static_cast<AudioStreamType>(attr_.audAttr.streamType);
    renderConfig.audioFormat = static_cast<AudioCodecFormat>(attr_.audAttr.format);
    renderConfig.sampleRate = attr_.audAttr.sampleRate;
    renderConfig.sessionID = attr_.audAttr.sessionID;
    renderConfig.channelCount = attr_.audAttr.channel;
    renderConfig.sampleFmt = attr_.audAttr.sampleFmt;
    renderConfig.linkDir = AUDIO_DOWN_LINK;
    renderConfig.bitWidth = (AudioBitWidth)attr_.audAttr.bitWidth;
    RendererCallback renderCallback = OnRenderCallback;
    MEDIA_INFO_LOG("AudioSink init audioformat:%d", renderConfig.audioFormat);
    if (outStream_->Init(renderConfig, renderCallback, this) != AUDIO_SUCCESS) {
        MEDIA_ERR_LOG("AudioStreamOut init failed");
        return SINK_OPEN_STREAM_FAILED;
    }
    /* dump stream */
    outStream_->DumpInfo();
    MEDIA_INFO_LOG("init success");
    return SINK_SUCCESS;
}

void AudioSink::ResetRendStartTime()
{
    lastRendFrameCnt_ = 0;
    lastRendPts_ = AV_INVALID_PTS;
    rendStartTime_ = AV_INVALID_PTS;
}

void AudioSink::GetStatus(AudioSinkStatus &status) const
{
    status.audFrameCount = rendFrameCnt_;
}

#ifndef UNSUPPORT_AUDIO_DECODER
void AudioSink::ReleaseQueHeadFrame(void)
{
    MediaMutexLock(mutex_);
    if (frameCacheQue_.size() != 0) {
        OutputInfo frame = frameCacheQue_[0];
        frameCacheQue_.erase(frameCacheQue_.begin());
        frameReleaseQue_.push_back(frame);
    }
    MediaMutexUnLock(mutex_);
}
#endif

#ifdef UNSUPPORT_AUDIO_DECODER
void AudioSink::ReleaseQueHeadEsFrame(void)
{
    MediaMutexLock(mutex_);
    if (esFrameCacheQue_.size() != 0) {
        EsFrameInfo frame = esFrameCacheQue_[0];
        esFrameCacheQue_.erase(esFrameCacheQue_.begin());
        esFrameReleaseQue_.push_back(frame);
    }
    MediaMutexUnLock(mutex_);
}

int32_t AudioSink::GetRenderEsFrame(EsFrameInfo &renderFrame, const EsFrameInfo &frame)
{
    int32_t ret = SINK_QUE_EMPTY;
    MediaMutexLock(mutex_);
    if (frame.type == ESFRAME_TYPE_AUDIO && frame.addr != nullptr && frame.size != 0) {
        esFrameCacheQue_.push_back(frame);
    }
    if (esFrameCacheQue_.size() != 0) {
        renderFrame = esFrameCacheQue_[0];
        ret = SINK_SUCCESS;
    }
    MediaMutexUnLock(mutex_);
    return ret;
}

void AudioSink::RelaseQueAllEsFrame(void)
{
    MediaMutexLock(mutex_);
    size_t queSize = esFrameReleaseQue_.size();
    if (queSize > MAX_QUEUE_BUF_NUM) {
        MediaMutexUnLock(mutex_);
        return;
    }
    for (size_t i = 0; i < queSize; i++) {
        esFrameReleaseQue_.push_back(esFrameReleaseQue_[i]);
    }
    esFrameReleaseQue_.clear();
    MediaMutexUnLock(mutex_);
}

int32_t AudioSink::DequeReleaseEsFrame(EsFrameInfo &frame)
{
    MediaMutexLock(mutex_);
    if (esFrameReleaseQue_.size() == 0) {
        MediaMutexUnLock(mutex_);
        return SINK_QUE_EMPTY;
    }
    frame = esFrameReleaseQue_[0];
    esFrameReleaseQue_.erase(esFrameReleaseQue_.begin());
    MediaMutexUnLock(mutex_);
    return SINK_SUCCESS;
}

#else
void AudioSink::UpdateAudioPts(int64_t lastPts, int64_t &timestamp, OutputInfo &renderFrame)
{
    if (renderFrame.timeStamp == -1) {
        float sampleCnt = (renderFrame.buffers[0].length / attr_.audAttr.channel) / AUDIO_SAMPLE_WIDTH_BYTE;
        float duration = (sampleCnt / attr_.audAttr.sampleRate) * MS_SCALE;
        renderFrame.timeStamp = lastPts + duration;
    }
    timestamp = renderFrame.timeStamp;
    renderDelay_ = 0;
}

void AudioSink::QueueRenderFrame(const OutputInfo &frame, bool cacheQueue)
{
    if (frame.type != AUDIO_DECODER || frame.bufferCnt == 0) {
        return;
    }
    MediaMutexLock(mutex_);
    if (cacheQueue) {
        frameCacheQue_.push_back(frame);
    } else {
        frameReleaseQue_.push_back(frame);
    }
    MediaMutexUnLock(mutex_);
}

int32_t AudioSink::GetRenderFrame(OutputInfo &renderFrame, const OutputInfo &frame)
{
    int32_t ret = SINK_QUE_EMPTY;
    MediaMutexLock(mutex_);
    if (frame.type == AUDIO_DECODER && frame.bufferCnt != 0) {
        frameCacheQue_.push_back(frame);
    }
    if (frameCacheQue_.size() != 0) {
        renderFrame = frameCacheQue_[0];
        ret = SINK_SUCCESS;
    }
    MediaMutexUnLock(mutex_);
    return ret;
}

void AudioSink::RelaseQueHeadFrame(void)
{
    MediaMutexLock(mutex_);
    if (frameCacheQue_.size() != 0) {
        OutputInfo frame = frameCacheQue_[0];
        frameCacheQue_.erase(frameCacheQue_.begin());
        frameReleaseQue_.push_back(frame);
    }
    MediaMutexUnLock(mutex_);
}

void AudioSink::RelaseQueAllFrame(void)
{
    size_t i;
    size_t queSize;
    MediaMutexLock(mutex_);
    queSize = frameCacheQue_.size();
    if (queSize > MAX_QUEUE_BUF_NUM) {
        MediaMutexUnLock(mutex_);
        return;
    }
    for (i = 0; i < queSize; i++) {
        frameReleaseQue_.push_back(frameCacheQue_[i]);
    }
    frameCacheQue_.clear();
    MediaMutexUnLock(mutex_);
}

int32_t AudioSink::DequeReleaseFrame(OutputInfo &frame)
{
    MediaMutexLock(mutex_);
    if (frameReleaseQue_.size() == 0) {
        MediaMutexUnLock(mutex_);
        return SINK_QUE_EMPTY;
    }
    frame = frameReleaseQue_[0];
    frameReleaseQue_.erase(frameReleaseQue_.begin());
    MediaMutexUnLock(mutex_);
    return SINK_SUCCESS;
}

int32_t AudioSink::WriteToAudioDevice(OutputInfo &renderFrame)
{
    int32_t ret;
    uint32_t writeLen = 0;
    AudioRendererConfig renderConfig;
    if (renderFrame.buffers == nullptr) {
        return SINK_RENDER_ERROR;
    }
    ret = outStream_->StreamWrite(renderFrame.buffers[0].addr + renderFrame.buffers[0].offset,
        static_cast<uint32_t>(renderFrame.buffers[0].length), writeLen, renderConfig);
    if (static_cast<uint32_t>(renderFrame.buffers[0].length) != writeLen) {
        return SINK_RENDER_FULL;
    } else if (ret != AUDIO_SUCCESS) {
        MEDIA_ERR_LOG("RenderFrame failed ret: %x", ret);
        return SINK_RENDER_ERROR;
    }
    return SINK_SUCCESS;
}

int32_t AudioSink::RenderFrame(OutputInfo &frame)
{
    int64_t crtPlayPts = 0;
    OutputInfo renderFrame;

    if (paused_) {
        QueueRenderFrame(frame, true);
        return SINK_SUCCESS;
    }
    if (!started_ || outStream_ == nullptr) {
        QueueRenderFrame(frame, false);
        MEDIA_ERR_LOG("paused or audio dev not inited");
        return SINK_RENDER_ERROR;
    }

    if (GetRenderFrame(renderFrame, frame) != SINK_SUCCESS) {
        if (receivedEos_) {
            RenderRptEvent(EVNET_AUDIO_PLAY_EOS);
            return SINK_RENDER_EOS;
        }
        return SINK_QUE_EMPTY;
    }

    UpdateAudioPts(lastRendPts_, crtPlayPts, renderFrame);
    int ret = WriteToAudioDevice(renderFrame);
    if (ret == SINK_SUCCESS || ret == SINK_RENDER_ERROR) {
        lastRendPts_ = renderFrame.timeStamp;
        rendFrameCnt_++;
    }
    if (ret != SINK_RENDER_FULL) {
        RelaseQueHeadFrame();
    }
    return ret;
}
#endif

void AudioSink::RenderRptEvent(EventCbType event)
{
    if (callBack_.onEventCallback != nullptr) {
        if (event == EVNET_AUDIO_PLAY_EOS && eosSended_) {
            return;
        }
        callBack_.onEventCallback(callBack_.priv, event, 0, 0);
        if (event == EVNET_AUDIO_PLAY_EOS) {
            eosSended_ = true;
        }
    }
}

int32_t AudioSink::WriteEsFrameToAudioDevice(const EsFrameInfo &frame)
{
    int32_t ret;
    if (continousRendFrameFullCnt_ == MAX_CONTINOUS_REND_FRAME_FULL_COUNT) {
        ret = callBack_.onEventCallback(callBack_.priv, EVNET_AUDIO_RUNNING_ERR, 0, 0);
        if (ret != SINK_SUCCESS) {
            MEDIA_ERR_LOG("callback failed");
        }
        MEDIA_ERR_LOG("continous render frame full count exceed upper limit");
        ReleaseQueHeadEsFrame();
        return SINK_RENDER_FATEL;
    }

    uint64_t writeLen = 0;
    AudioRendererConfig renderConfig;
    ret = outStream_->StreamWrite(frame.addr, frame.size, writeLen, renderConfig);
    if (frame.size != writeLen) {
        ReleaseQueHeadEsFrame();
        continousRendFrameFullCnt_++;
        return SINK_RENDER_FULL;
    } else if (ret != AUDIO_SUCCESS) {
        ReleaseQueHeadEsFrame();
        MEDIA_ERR_LOG("RenderFrame failed ret: %x", ret);
        continousRendFrameFullCnt_ = 0;
        return SINK_RENDER_ERROR;
    }
    ReleaseQueHeadEsFrame();
    continousRendFrameFullCnt_ = 0;
#ifdef MINI_MEMORY_SUPPORT
    if (frame.addr == nullptr && frame.size == 0) {
        hasSendEosFrame_ = true;
    }
#endif
    return (frame.addr == nullptr) ? SINK_RENDER_EOS : SINK_SUCCESS;
}

int32_t AudioSink::RenderEsFrame(const EsFrameInfo &frame)
{
    EsFrameInfo renderFrame = {};
    SyncRet syncRet = SYNC_RET_PLAY;

    if (!started_ || outStream_ == nullptr) {
        MEDIA_ERR_LOG("paused or audio dev not inited");
        return SINK_RENDER_ERROR;
    }

    if (GetRenderEsFrame(renderFrame, frame) != SINK_SUCCESS) {
        if (receivedEos_ && frame.addr == nullptr) {
            // send eos flag to sdk
            return WriteEsFrameToAudioDevice(frame);
        }
        return SINK_QUE_EMPTY;
    }

    int64_t posNs = AV_INVALID_PTS;
    int32_t ret = outStream_->GetPosition(posNs);
    if (ret != AUDIO_SUCCESS) {
        if ((uint32_t)ret == AUDIO_BAD_STATE) {
            ReleaseQueHeadEsFrame();
            return SINK_RENDER_DELAY;
        }
        MEDIA_ERR_LOG("get position failed");
        ReleaseQueHeadEsFrame();
        return SINK_RENDER_DELAY;
    }

    ret = (syncHdl_ != nullptr) ? syncHdl_->ProcAudFrame(renderFrame.pts, syncRet) : SINK_SUCCESS;
    if (ret != HI_SUCCESS) {
        MEDIA_ERR_LOG("process audio frame pts: %lld failed", posNs / NANOS_PER_MILLISECOND);
        ReleaseQueHeadEsFrame();
        return SINK_RENDER_ERROR;
    }

    if (syncRet == SYNC_RET_PLAY) {
        ret = WriteEsFrameToAudioDevice(renderFrame);
    } else if (syncRet == SYNC_RET_DROP) {
        ReleaseQueHeadEsFrame();
        ret = SINK_SUCCESS;
    } else if (syncRet == SYNC_RET_REPEAT) {
        ret = SINK_RENDER_DELAY;
    } else {
        MEDIA_ERR_LOG("sync failed: %d", syncRet);
        ReleaseQueHeadEsFrame();
        ret = SINK_RENDER_ERROR;
    }

    if (ret == SINK_SUCCESS || ret == SINK_RENDER_ERROR) {
#ifdef MINI_MEMORY_SUPPORT
        lastRendPts_ = posNs / NANOS_PER_MILLISECOND;
#else
        lastRendPts_ = frame.pts;
#endif
        rendFrameCnt_++;
    }
    return ret;
}

void AudioSink::RenderEos(void)
{
    receivedEos_ = true;
    eosPts_ = lastRendPts_;
}

int32_t AudioSink::Start(void)
{
    CHECK_TRUE_CONDITION_RETURN(started_, SINK_SUCCESS, "have started");
    CHECK_NULL_RETURN(outStream_, SINK_INVALID_OP, "audio stream not inited");
    int32_t ret = outStream_->Play();
    if (ret == AUDIO_SUCCESS) {
        started_ = true;
        return SINK_SUCCESS;
    } else {
        return SINK_INVALID_OP;
    }
}

int32_t AudioSink::SetVolume(const float left, const float right)
{
    float volume;
    CHECK_FALSE_CONDITION_RETURN(started_, -1, "not started");
    if (outStream_ == nullptr) {
        MEDIA_ERR_LOG("AudioSink::SetVolume failed audiostream_ null");
        return -1;
    }

    leftVolume_ = left;
    rightVolume_ = right;
    if ((leftVolume_ == 0) && (rightVolume_ != 0)) {
        volume = rightVolume_;
    } else if ((leftVolume_ != 0) && (rightVolume_ == 0)) {
        volume = leftVolume_;
    } else {
        volume = (leftVolume_ + rightVolume_) / HALF_FACTOR;
    }
    outStream_->SetVolume(volume);
    return SINK_SUCCESS;
}

int32_t AudioSink::GetVolume(float &left, float &right) const
{
    left = leftVolume_;
    right = rightVolume_;
    return SINK_SUCCESS;
}

int32_t AudioSink::Stop(void)
{
    MEDIA_INFO_LOG("audio sink stop");
    if (started_ && outStream_ != nullptr) {
        CHECK_FAILED_RETURN(outStream_->Stop(), 0, -1, "stop() fail");
    }
#ifndef UNSUPPORT_AUDIO_DECODER
    RelaseQueAllFrame();
#else
    RelaseQueAllEsFrame();
#endif
    rendFrameCnt_ = 0;
    started_ = false;
    paused_ = false;
    eosSended_ = false;
    return SINK_SUCCESS;
}

int32_t AudioSink::Pause(void)
{
    if (started_ && outStream_ != nullptr) {
        CHECK_FAILED_RETURN(outStream_->Pause(), 0, -1, "pause() fail");
    }
    paused_ = true;
    RecordLastPosition();
    return SINK_SUCCESS;
}

int32_t AudioSink::Resume(void)
{
    CHECK_FALSE_CONDITION_RETURN(paused_, -1, "not paused");
    CHECK_NULL_RETURN(outStream_, -1, "audiostream_ is null");
    int32_t ret = outStream_->Play();
    CHECK_FAILED_RETURN(ret, 0, ret, "resume fail");
    paused_ = false;
#ifdef MINI_MEMORY_SUPPORT
    // If the EOS frame transmission is complete during pause and resume, the EOS frame needs to be transmitted again.
    // Otherwise, the DSP cannot call back the EOS function.
    if (hasSendEosFrame_) {
        uint64_t writeLen = 0;
        AudioRendererConfig renderConfig;
        int32_t retValue = outStream_->StreamWrite(nullptr, 0, writeLen, renderConfig);
        if (retValue != AUDIO_SUCCESS) {
            MEDIA_ERR_LOG("send eos frame failed!");
        }
    }
#endif
    return SINK_SUCCESS;
}

int32_t AudioSink::Reset(void)
{
#ifndef UNSUPPORT_AUDIO_DECODER
    RelaseQueAllFrame();
#else
    RelaseQueAllEsFrame();
#endif
    if (started_ && outStream_ != nullptr) {
        CHECK_FAILED_RETURN(outStream_->Flush(), 0, -1, "flush() fail");
    }
    ResetRendStartTime();
    renderDelay_ = 0;
#ifdef MINI_MEMORY_SUPPORT
    hasSendEosFrame_ = false;
#endif
    return SINK_SUCCESS;
}

int32_t AudioSink::RegisterCallBack(const PlayEventCallback &callback)
{
    callBack_ = callback;
    return SINK_SUCCESS;
}

void AudioSink::SetSync(PlayerSync *sync)
{
    syncHdl_ = sync;
}

int32_t AudioSink::Flush(void)
{
    if (started_ && outStream_ != nullptr) {
        CHECK_FAILED_RETURN(outStream_->Flush(), 0, -1, "flush() fail");
    }
    renderDelay_ = 0;
    return 0;
}

void AudioSink::GetRenderPosition(int64_t &position) const
{
    CHECK_FALSE_CONDITION_RETURN_NONE(started_, "not started");
    CHECK_NULL_RETURN_NONE(outStream_, "audiostream_ is null");

    int64_t posNs = AV_INVALID_PTS;
    int32_t ret = outStream_->GetPosition(posNs);
    if (ret != SINK_SUCCESS) {
        MEDIA_ERR_LOG("get position failed");
        position = AV_INVALID_PTS;
        return;
    }

    position = posNs / NANOS_PER_MILLISECOND;
#ifdef MINI_MEMORY_SUPPORT
    position += lastRenderPosition_;
#endif
}

int32_t AudioSink::SetSpeed(const float speed) const
{
    CHECK_NULL_RETURN(outStream_, -1, "audiostream_ is null");
    outStream_->SetPlayBackSpeed(speed);
    return SINK_SUCCESS;
}

bool AudioSink::IsOffloadSupported(const AudioOffloadParam &offloadInfo) const
{
    bool result;
    AudioOffloadInfo info;
    info.streamType = static_cast<AudioStreamType>(offloadInfo.streamType);
    info.format = static_cast<AudioCodecFormat>(offloadInfo.format);
    info.samplerate = offloadInfo.sampleRate;
    info.channel = offloadInfo.channel;
    info.bitwidth = offloadInfo.bitWidth;
    AudioManager &audioMgr = AudioManager::GetInstance();
    result = audioMgr.IsSupportOffload(info);
    return result;
}

int32_t AudioSink::SetOutputDevice(const int32_t deviceId) const
{
    CHECK_NULL_RETURN(outStream_, -1, "audiostream_ is null");
    AudioRendererConfig renderConfig;
    renderConfig.streamType = static_cast<AudioStreamType>(attr_.audAttr.streamType);
    renderConfig.audioFormat = static_cast<AudioCodecFormat>(attr_.audAttr.format);
    renderConfig.sampleRate = attr_.audAttr.sampleRate;
    renderConfig.sessionID = attr_.audAttr.sessionID;
    renderConfig.channelCount = attr_.audAttr.channel;
    return outStream_->SetOutputDevice(deviceId, renderConfig);
}

void AudioSink::RecordLastPosition()
{
#ifdef MINI_MEMORY_SUPPORT
    lastRenderPosition_ += lastRendPts_;
#endif
}

int32_t AudioSink::SetParam(const std::string &key, const DataType type, const void *value, uint32_t len) const
{
    (void)type;
    uint32_t bufLen = len + 30; /* 30 to save "key" */
    if (bufLen < 0 || bufLen > MAX_EXTRA_PARAM_SIZE) {
        MEDIA_ERR_LOG("The bufLen is invalid = %u", bufLen);
        return SINK_INVALID_OP;
    }
    const char *keyStr = key.c_str();
    char *buf = (char *)MediaMemMalloc(bufLen);
    if (buf == nullptr) {
        MEDIA_ERR_LOG("AudioSink::SetParam malloc failed");
        return SINK_INVALID_OP;
    }
    int32_t prefixLen = sprintf_s(buf, bufLen, "%s = ", keyStr);
    if (prefixLen < 0) {
        MEDIA_ERR_LOG("AudioSink::SetParam sprintf_s failed");
        MediaMemFree(buf);
        return SINK_INVALID_OP;
    }
    if (memcpy_s(buf + prefixLen, bufLen - prefixLen, value, len) != EOK) {
        MEDIA_ERR_LOG("memcpy_s failed");
        MediaMemFree(buf);
        return -1;
    }
    int32_t ret = outStream_->SetParam(buf, bufLen);
    if (buf != nullptr) {
        MediaMemFree(buf);
        buf = nullptr;
    }
    return ret;
}

int32_t AudioSink::GetParam(const std::string &key, const DataType type, void *value, uint32_t len)
{
    (void)type;
    uint32_t bufLen = MAX_EXTRA_PARAM_SIZE;
    if (len  > MAX_EXTRA_PARAM_SIZE - 30) { /* 30 to save "key" */
        MEDIA_ERR_LOG("The input length is too large.len = %u", len);
        return SINK_INVALID_OP;
    }
    const char *keyStr = key.c_str();
    char *buf = (char *)MediaMemMalloc(bufLen);
    if (buf == nullptr) {
        MEDIA_ERR_LOG("AudioSink::getParam malloc failed");
        return SINK_INVALID_OP;
    }
    int32_t prefixLen = sprintf_s(buf, bufLen, "%s = ", keyStr, value);
    if (prefixLen < 0) {
        MEDIA_ERR_LOG("AudioSink::getParam sprintf_s failed");
        goto EXIT;
    }
    if (memcpy_s(buf + prefixLen, bufLen - prefixLen, value, len) != EOK) {
        MEDIA_ERR_LOG("memcpy_s failed");
        goto EXIT;
    }
    if (outStream_->GetParam(buf, bufLen) != SINK_SUCCESS) {
        goto EXIT;
    }
    if (memcpy_s(value, len, buf, len) != EOK) {
        MEDIA_ERR_LOG("memcpy_s failed");
    }
    MediaMemFree(buf);
    return SINK_SUCCESS;
EXIT:
    if (buf != nullptr) {
        MediaMemFree(buf);
        buf = nullptr;
    }
    return -1;
}
}
}

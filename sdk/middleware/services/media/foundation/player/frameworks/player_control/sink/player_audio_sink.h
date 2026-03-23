/*
 * Copyright (c) 2020-2023 Huawei Device Co., Ltd.
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

#ifndef PLAYER_AUDIO_SINK_H
#define PLAYER_AUDIO_SINK_H

#include <memory>
#include <vector>
#include "player_sink_type.h"
#include "player_define.h"
#ifndef UNSUPPORT_AUDIO_DECODER
#include "codec_type.h"
#endif
#include "player_sync.h"
#include "audio_stream_out.h"
#include "media_thread_adapt.h"

namespace OHOS {
namespace Media {
using namespace std;
using Audio::AudioStreamOut;
class AudioSink {
public:
    AudioSink();
    ~AudioSink();
    int32_t Init(const SinkAttr &atrr);
    void DeInit(void);
    int32_t Start(void);
    int32_t Stop(void);
    int32_t Flush(void);
    int32_t Reset(void);
    int32_t Pause(void);
    int32_t Resume(void);
    int32_t SetSpeed(const float speed) const;
    int32_t GetSpeed(float &speed);
#ifdef UNSUPPORT_AUDIO_DECODER
    int32_t DequeReleaseEsFrame(EsFrameInfo &frame);
#else
    int32_t RenderFrame(OutputInfo &frame);
    int32_t DequeReleaseFrame(OutputInfo &frame);
#endif
    int32_t RenderEsFrame(const EsFrameInfo &frame);
    int32_t SetVolume(const float left, const float right);
    int32_t GetVolume(float &left, float &right) const;
    int32_t SetMute(bool mute);
    int32_t GetMute(bool &mute);
    void SetSync(PlayerSync *sync);
    int32_t SetParam(const std::string &key, const DataType type, const void *value, uint32_t len) const;
    int32_t GetParam(const std::string &key, const DataType type, void* value, uint32_t size);
    int32_t RegisterCallBack(const PlayEventCallback &callback);
    void GetStatus(AudioSinkStatus &status) const;
    void RenderEos(void);
    void GetRenderPosition(int64_t &position) const;
    bool IsOffloadSupported(const AudioOffloadParam &offloadInfo) const;
    int32_t SetOutputDevice(const int32_t deviceId) const;
    void RecordLastPosition();
private:
    void ResetRendStartTime();
    void SendAudioEndOfStream();
#ifdef UNSUPPORT_AUDIO_DECODER
    void RenderRptEvent(EventCbType event);
    static int32_t OnRenderCallback(const void *caller, AudioCallbackEventType event, const void *info);
    int32_t GetRenderEsFrame(EsFrameInfo &renderFrame, const EsFrameInfo &frame);
    void ReleaseQueHeadEsFrame(void);
    void RelaseQueAllEsFrame(void);
    int32_t WriteEsFrameToAudioDevice(const EsFrameInfo &frame);
#else
    int32_t WriteToAudioDevice(OutputInfo &renderFrame);
    void UpdateAudioPts(int64_t lastPts, int64_t& timestamp, OutputInfo &renderFrame);
    int GetRenderFrame(OutputInfo &renderFrame, const OutputInfo &frame);
    void QueueRenderFrame(const OutputInfo &frame, bool cacheQueue);
    void ReleaseQueHeadFrame(void);
    void RelaseQueAllFrame(void);
#endif
    bool started_;
    bool paused_;
    bool eosSended_;
    uint64_t rendFrameCnt_;
    uint64_t lastRendFrameCnt_;
    uint32_t continousRendFrameFullCnt_; // continous render frame full count
    SinkAttr attr_;
    PlayerSync *syncHdl_;
    int64_t rendStartTime_;
    int64_t lastRendPts_;
#ifdef MINI_MEMORY_SUPPORT
    int64_t lastRenderPosition_;
    bool hasSendEosFrame_;
#endif
    int64_t lastRendSysTimeMs_;
    uint32_t renderDelay_;
    PlayEventCallback callBack_;
    float leftVolume_;
    float rightVolume_;
    int64_t eosPts_;
    bool receivedEos_;
    MediaMutexHandle mutex_;
#ifdef UNSUPPORT_AUDIO_DECODER
    std::vector<EsFrameInfo> esFrameCacheQue_;
    std::vector<EsFrameInfo> esFrameReleaseQue_;
#else
    std::vector<OutputInfo> frameCacheQue_;
    std::vector<OutputInfo> frameReleaseQue_;
#endif
    std::shared_ptr<AudioStreamOut> outStream_;
};
}  // namespace Media
}  // namespace OHOS

#endif  // PLAYER_SINK_H

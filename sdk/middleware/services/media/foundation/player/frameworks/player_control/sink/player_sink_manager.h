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

#ifndef PLAYER_SINK_MANAGER_H
#define PLAYER_SINK_MANAGER_H

#include <memory>
#include <vector>
#include <string>

#include "player_sink_type.h"
#include "player_define.h"
#include "player_audio_sink.h"
#include "player_video_sink.h"

namespace OHOS {
namespace Media {
using namespace std;

struct AudioSinkInfo {
    int32_t trackId;
    std::shared_ptr<AudioSink> sink;
};

struct VideoSinkInfo {
    int32_t trackId;
    std::shared_ptr<VideoSink> sink;
};

struct SinkInfo {
    SinkType sinkType;
    int32_t trackId;
    union {
        AudioSinkInfo vidSink;
        AudioSinkInfo audSink;
    } param;
};

class SinkManager {
friend VideoSink;
friend AudioSink;

public:
    SinkManager();
    ~SinkManager();
    int32_t AddNewSink(const SinkAttr &attr);
    int32_t Start(void);
    int32_t Stop(void);
    int32_t Flush(void) const;
    int32_t Reset(void);
    int32_t Pause(void);
    int32_t Resume(void);
    int32_t SetSpeed(const float speed);
    int32_t GetSpeed(float &speed) const;
    int32_t RenderFrame(OutputInfo &frame);
    bool HaveCacheFrame(bool isAudio);
    int DequeReleaseFrame(bool audioSink, OutputInfo &frame);
    int32_t DequeReleaseEsFrame(bool audioSink, EsFrameInfo &frame);
    int32_t RenderEsFrame(const EsFrameInfo &frame) const;
    int32_t SetVolume(const float left, const float right);
    int32_t GetVolume(float &left, float &right) const;
    int32_t SetParam(const std::string &key, const DataType type, const void* value, int32_t len) const;
    int32_t GetParam(const std::string &key, const DataType type, void *value, int32_t size);
    int32_t RegisterCallBack(const PlayEventCallback &callback);
    int32_t GetStatus(PlayerStreamInfo &streamInfo) const;
    void RenderEos(const bool isAudio);
    void GetRenderPosition(int64_t &position) const;
    bool IsOffloadSupported(const AudioOffloadParam &offloadInfo) const;
    int32_t SetOutputDevice(const int32_t deviceId) const;
    void RecordLastPosition();
private:
    float speed_;
    float leftVolume_;
    float rightVolume_;
    bool paused_;
    bool started_;
    int32_t audioSinkNum_;
    int32_t videoSinkNum_;
    std::shared_ptr<PlayerSync> sync_;
    AudioSinkInfo audioSinkInfo_[MAX_PIPELINE_SINK_NUM];
    VideoSinkInfo videoSinkInfo_[MAX_PIPELINE_SINK_NUM];
    PlayEventCallback callBack_;
    bool recieveAudioEos_;
    bool recieveVideoEos_;
};
}  // namespace Media
}  // namespace OHOS

#endif  // PLAYER_SINK_MANAGER_H

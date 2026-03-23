/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#ifndef AUDIO_PLAYER_H
#define AUDIO_PLAYER_H

#include <string>
#include <vector>
#include "jsi.h"
#include "player.h"
#include "audio_base_type.h"
#include "media_thread_adapt.h"
#include "audio_manager.h"
#include "audio_player_service.h"

namespace OHOS {
namespace ACELite {
using namespace OHOS;
using namespace::Audio;
class AudioPlayer;
class AudioEventListener {
public:
    AudioEventListener() = default;

    explicit AudioEventListener(JSIValue callback);

    ~AudioEventListener();

    JSIValue GetCallback() const;

    void OnTrigger() const;

private:
    JSIValue callback_;
};

void TriggerEventListener(const AudioEventListener *listener);
void AsyncExecuteCallback(void *arg);

class AudioPlayerCallback : public OHOS::AudioPlayerServiceCallback {
public:
    AudioPlayerCallback() = delete;

    explicit AudioPlayerCallback(AudioPlayer *audioPlayer);

    ~AudioPlayerCallback() override {}
    void OnInterrupt(int32_t type, int32_t hint) override;
    void OnError(int32_t errorType, int32_t errorCode) override;
    void OnPlaybackComplete() override;
    void OnPlay() override;
    void OnPause() override;
    void OnStop() override;
private:
    AudioPlayer *audioPlayer_;
};

class AudioPlayer {
public:
    static AudioPlayer *GetInstance();

    void ForkUpdateTimeThread();

    void StopUpdateTimeThread();

    bool ResetPlayer();

    void ReleaseEventListeners();
#ifdef HMF_DECRYPT_DATA_ENABLE
    void SetDecryptLibraryPath(char *path, uint32_t len);
#endif
    void ReleaseSrc();

    bool Play();

    bool Pause();

    bool Stop();

    char *GetSrc(bool innerFlag) const;

    bool SetSrcList(std::vector<std::string> srcList);

    bool SetBackgroundPlay(bool enable);

    bool IsBackgroundPlay(void);

    double GetCurrentTime() const;

    double GetDuration() const;

    bool GetAutoPlay() const;

    bool IsLooping() const;

    bool IsPlayListLoop() const;

    bool IsMuted() const;

    const char *GetStatus() const;

    double GetVolume();

    bool IsPlaying() const;

    bool SetSrc(char *src, bool innerFlag);

    bool SetCurrentTime(double currentTime) const;

    bool SetAutoPlay(bool autoPlay);

    bool SetLoop(bool loop);

    bool SetVolume(double volume);

    bool SetStreamType(AudioStreamType streamType);

    bool SetMuted(bool muted);

    const AudioEventListener *GetOnPlayListener() const;

    const AudioEventListener *GetOnPauseListener() const;

    const AudioEventListener *GetOnStopListener() const;

    const AudioEventListener *GetOnLoadedDataListener() const;

    const AudioEventListener *GetOnEndedListener() const;

    const AudioEventListener *GetOnErrorListener() const;

    const AudioEventListener *GetOnTimeUpdateListener() const;

    void SetOnPlayListener(AudioEventListener *onPlayListener);

    void SetOnPauseListener(AudioEventListener *onPauseListener);

    void SetOnStopListener(AudioEventListener *onStopListener);

    void SetOnLoadedDataListener(AudioEventListener *onLoadedDataListener);

    void SetOnEndedListener(AudioEventListener *onEndedListener);

    void SetOnErrorListener(AudioEventListener *onErrorListener);

    void SetOnTimeUpdateListener(AudioEventListener *onTimeUpdateListener);

    int32_t GetSrcInnerPrefix(const char *src, char *dest);

    bool IsSysSrc();

    void GetAlbumInfo(AudioPlayerAlbumInfo &albumInfo, const char *src);

private:
    AudioPlayer();
    ~AudioPlayer();
    static MediaMutexHandle lock_;
    static MediaThreadCondHandle condition_;
    static void *UpdateTimeHandler(void *arg);
    std::shared_ptr<AudioPlayerCallback> callback_ = nullptr;
    AudioEventListener *onPlayListener_;
    AudioEventListener *onPauseListener_;
    AudioEventListener *onStopListener_;
    AudioEventListener *onLoadedDataListener_;
    AudioEventListener *onEndedListener_;
    AudioEventListener *onErrorListener_;
    AudioEventListener *onTimeUpdateListener_;
    char *src_;
    char *srcInner_;
    std::string status_;
    double volume_;
    bool autoPlay_;
    bool isRunning_;
    bool isSysSrc_;
    bool isPlayList_;
    AudioPlayerLoopMode playLoopMode_ = AudioPlayerLoopMode::AUDIO_PLAYER_INVAILD_LOOP;
    MediaThreadIdHandle updateTimeProcess_;
    AudioStreamType streamType_ = AUDIO_STREAM_MUSIC;
};
} // namespace ACELite
} // namespace OHOS
#endif // AUDIO_PLAYER_H

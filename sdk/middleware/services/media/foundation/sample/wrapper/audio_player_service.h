/*
 * Copyright (c) CompanyNameMagicTag. 2023-2023. All rights reserved.
 * Description: audio player service
 * Author: Media Software Group
 * Create: 2024-12-10
 */

#ifndef AUDIO_PLAY_SERVICE_H
#define AUDIO_PLAY_SERVICE_H
#include <string>
#include <vector>
#include <memory>
#include "audio_manager.h"
#include "player.h"
#include "audio_base_type.h"
#include "media_thread_adapt.h"

namespace OHOS {
using namespace::Audio;
using namespace OHOS::Media;
using OHOS::Media::Player;

typedef struct AudioPlayerAlbumInfo {
    std::string src;
    std::string title;
    std::string artist;
} AudioPlayerAlbumInfo;

enum class AudioPlayerStates : int32_t {
    AUDIO_PLAYER_IDLE,
    AUDIO_PLAYER_ENTERED,
    AUDIO_PLAYER_PAUSED,
    AUDIO_PLAYER_RESUMEING,
    AUDIO_PLAYER_PLAYED,
    AUDIO_PLAYER_STOPED,
    AUDIO_PLAYER_EXITED,
};

enum class AudioPlayerLoopMode : int32_t {
    AUDIO_PLAYER_PLAYLIST_LOOP,
    AUDIO_PLAYER_SINGLE_LOOP,
    AUDIO_PLAYER_INVAILD_LOOP,
};

class AudioPlayerServiceCallback {
public:
    AudioPlayerServiceCallback() = default;
    virtual ~AudioPlayerServiceCallback() {}
    virtual void OnInterrupt(int32_t type, int32_t hint) = 0;
    virtual void OnError(int32_t errorType, int32_t errorCode) = 0;
    virtual void OnPlaybackComplete() = 0;
    virtual void OnPlay() = 0;
    virtual void OnPause() = 0;
    virtual void OnStop() = 0;
};

class AudioPlayerService : public PlayerCallback, public InterruptListener,
    public std::enable_shared_from_this<AudioPlayerService> {
public:
    ~AudioPlayerService() override;
    static std::shared_ptr<AudioPlayerService> GetInstance();
    int32_t SetPlayListSource(std::vector<std::string> playlist, int32_t index = 0);
    int32_t SetPlaySource(std::string src);
    int32_t SetAudioStreamType(AudioStreamType type);
    int32_t SetBackgroundMode(bool isBackground);
#ifdef HMF_DECRYPT_DATA_ENABLE
    void SetDecryptLibraryPath(char *path, uint32_t len);
#endif
    bool IsBackgroundMode(void);
    void SetAudioPlayerCallback(const std::shared_ptr<AudioPlayerServiceCallback> &cb);
    int32_t Start(void);
    int32_t Stop(void);
    int32_t Pause(void);
    int32_t Resume(void);
    int32_t Reset(void);
    int32_t Seek(int64_t mSeconds);
    int32_t PlayNext(void);
    int32_t PlayPrev(void);
    int32_t GetDumpInfo(PlayerDebugInfo *playerInfo);
    int32_t GetCurrentTime(int64_t &time);
    int32_t GetPlayStatus(void);
    int32_t SetPlayLoopMode(AudioPlayerLoopMode loopMode);
    bool IsSingleLooping(void);
    bool IsPlayListLooping(void);
    bool IsPlaying(void);
    std::string GetCurrentPlaySource(void);
    int32_t GetDuration(int64_t &durationMs);
    void GetAlbumInfo(AudioPlayerAlbumInfo &albumInfo, const char *src);
    bool HasM3U8(const std::string& str);
protected:
    void OnPlaybackComplete() override;
    void OnError(int32_t errorType, int32_t errorCode) override;
    void OnInfo(int32_t type, int32_t extra) override;
    void OnRewindToComplete() override;
    void OnPlay();
    void OnPause();
    void OnStop();
    void OnInterrupt(int32_t type, int32_t hint) override;
private:
    AudioPlayerService(const AudioPlayerService &) = delete;
    AudioPlayerService &operator=(const AudioPlayerService &) = delete;
    AudioPlayerService();
    bool RequsetAudioFocus(void);
    bool ReleaseAudioFocus(void);
    bool InitPlayerResources(void);
    void DeinitPlayerResources(void);
    void ProcessInterrupt(void);
    int32_t AudioPlay(void);
    int32_t AudioPlayInner(void);
    void AudioStopAndExit(void);
    int32_t ReleasePlayer(void);
    int32_t SetAudioSessionIdAndStreamType(void);
    int32_t RunAudioPlay(void);
    void AudioPlayErrorCallBack(int32_t errorType, int32_t errorCode);
    void *AudioPlayThreadProcess(void);
    int32_t StartPrepare(void);
    void ResetAudioPlayState(void);
    static void *AudioPlayThread(void *arg);
#ifdef HMF_DECRYPT_DATA_ENABLE
    char *loadDecryptPath_ = nullptr;
    uint32_t loadDecryptPathLen_ = 0;
#endif
    AudioPlayerStates currentState_ = AudioPlayerStates::AUDIO_PLAYER_IDLE;
    AudioSession sessionId_ = AUDIO_SESSION_ID_NONE;
    bool isPlaybackCompleted_ = false;
    bool isPlaybackStopped_ = false;
    bool interruptHintPause_ = false;
    bool interruptHintResume_ = false;
    bool interruptHintStop_ = false;
    bool isBackgroundMode_ = false;
    bool playError_ = false;
    bool isCutSong_ = false;
    AudioPlayerLoopMode playerLoopMode_ = AudioPlayerLoopMode::AUDIO_PLAYER_INVAILD_LOOP;
    shared_ptr<Player> player_ = nullptr;
    MediaMutexHandle mutex_ = nullptr;
    MediaThreadCondHandle cond_ = nullptr;
    MediaThreadCondHandle exitCond_ = nullptr;
    MediaThreadIdHandle threadHandle_ = nullptr;
    int32_t playIndex_ = 0;
    std::vector<std::string> playList_;
    AudioStreamType streamType_ = AUDIO_STREAM_MUSIC;
    static std::shared_ptr<AudioPlayerService> instance_;
    shared_ptr<AudioPlayerServiceCallback> callBack_ = nullptr;
    shared_ptr<Player> albumInfoPlayer_ = nullptr;
};
}
#endif
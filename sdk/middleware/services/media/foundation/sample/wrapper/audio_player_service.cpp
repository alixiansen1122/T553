/*
 * Copyright (c) CompanyNameMagicTag. 2023-2023. All rights reserved.
 * Description: audio player service
 * Author: Media Software Group
 * Create: 2024-12-10
 */

#include "audio_player_service.h"
#include "audio_manager.h"
#include "media_log.h"
#include "securec.h"
#ifdef HMF_DECRYPT_DATA_ENABLE
#include "audio_decrypt.h"
#endif // HMF_DECRYPT_DATA_ENABLE


namespace OHOS {
static AudioManager &g_amInstance = AudioManager::GetInstance();

std::shared_ptr<AudioPlayerService> AudioPlayerService::instance_ =
std::shared_ptr<AudioPlayerService>(new AudioPlayerService());

void AudioPlayerService::OnInterrupt(int32_t type, int32_t hint)
{
    MEDIA_INFO_LOG("OnInterrupt, type:%d, hint:%d", type, hint);
    if (callBack_ != nullptr && !isBackgroundMode_) {
        callBack_->OnInterrupt(type, hint);
        return;
    }
    if (type == INTERRUPT_TYPE_BEGIN && hint == INTERRUPT_HINT_PAUSE) {
        MEDIA_INFO_LOG("player pause signal one");
        MediaMutexLock(mutex_);
        interruptHintPause_ = true;
        MediaThreadCondSignal(cond_);
        MediaMutexUnLock(mutex_);
    }
    if (type == INTERRUPT_TYPE_END && hint == INTERRUPT_HINT_RESUME) {
        MEDIA_INFO_LOG("player resume signal one");
        MediaMutexLock(mutex_);
        interruptHintResume_ = true;
        MediaThreadCondSignal(cond_);
        MediaMutexUnLock(mutex_);
    }
    if (type == INTERRUPT_TYPE_BEGIN && hint == INTERRUPT_HINT_STOP) {
        MEDIA_INFO_LOG("player stop begin signal one");
        MediaMutexLock(mutex_);
        interruptHintStop_ = true;
        MediaThreadCondSignal(cond_);
        MediaMutexUnLock(mutex_);
    }
    if (type == INTERRUPT_TYPE_END && hint == INTERRUPT_HINT_STOP) {
        MEDIA_INFO_LOG("player stop end signal one");
    }
}

void AudioPlayerService::OnPlaybackComplete()
{
    if (callBack_ != nullptr && !isBackgroundMode_) {
        callBack_->OnPlaybackComplete();
        MEDIA_INFO_LOG("on playback complete!");
    }
    MediaMutexLock(mutex_);
    if (playerLoopMode_ != AudioPlayerLoopMode::AUDIO_PLAYER_PLAYLIST_LOOP) {
        MediaMutexUnLock(mutex_);
        return;
    }
    MEDIA_INFO_LOG("on playback complete!");
    isPlaybackCompleted_ = true;
    MediaThreadCondSignal(cond_);
    MediaMutexUnLock(mutex_);
}

void AudioPlayerService::OnError(int32_t errorType, int32_t errorCode)
{
    if (errorType != PlayerErrorType::PLAYER_ERROR_UNKNOWN) {
        MEDIA_ERR_LOG("unsupport error type:%d", errorType);
        return;
    }
    if (errorCode == PlayerErrorCode::PLAYER_ERROR_CODE_AUD_PLAY_FAIL) {
        MEDIA_INFO_LOG("playback error signal one");
        if (callBack_ != nullptr && !isBackgroundMode_) {
            callBack_->OnError(errorType, errorCode);
        } else {
            MediaMutexLock(mutex_);
            playError_ = true;
            MediaThreadCondSignal(cond_);
            MediaMutexUnLock(mutex_);
        }
    }
}

void AudioPlayerService::OnInfo(int32_t type, int32_t extra)
{
    MEDIA_INFO_LOG("on playback info, type:%dm extra:%d", type, extra);
}

void AudioPlayerService::OnRewindToComplete()
{
    MEDIA_INFO_LOG("on playback rewind to complete");
}

void AudioPlayerService::DeinitPlayerResources(void)
{
    (void)player_.reset();
    (void)MediaThreadCondDestroy(&cond_);
}

bool AudioPlayerService::RequsetAudioFocus(void)
{
    bool success = g_amInstance.Initialize();
    if (!success) {
        MEDIA_ERR_LOG("audiomanager init failed");
        return false;
    }
    AudioSession sessionId = g_amInstance.MakeSessionId();
    if (sessionId == AUDIO_SESSION_ID_NONE) {
        MEDIA_ERR_LOG("audio session id invalid");
        return false;
    }
    sessionId_ = sessionId;
    AudioInterrupt interrupt = { streamType_, sessionId_, shared_from_this() };
    if (g_amInstance.ActivateAudioInterrupt(interrupt) == INTERRUPT_FAILED) {
        MEDIA_ERR_LOG("activate audio interrupt failed");
        return false;
    }
    return true;
}

bool AudioPlayerService::ReleaseAudioFocus(void)
{
    AudioInterrupt interrupt = { streamType_, sessionId_, shared_from_this() };
    if (g_amInstance.DeactivateAudioInterrupt(interrupt) != 0) {
        MEDIA_ERR_LOG("deactivate audio interrupt failed");
        return false;
    }
    return true;
}

bool AudioPlayerService::InitPlayerResources(void)
{
    cond_ = MediaThreadCondCreate();
    if (cond_ == nullptr) {
        MEDIA_ERR_LOG("create thread cond failed");
        return false;
    }
    player_ = std::make_shared<Player>();
    if (player_ == nullptr || player_.get() == nullptr) {
        MEDIA_ERR_LOG("player is nullptr");
        MediaThreadCondDestroy(&cond_);
        return false;
    }
    if (playIndex_ < 0 ||
        playList_.size() == 0 ||
        playIndex_ >= playList_.size()) {
        MEDIA_ERR_LOG("The index[%d] of the playlist[%d] is incorrect!", playIndex_, playList_.size());
        return false;
    }
    return true;
}

void AudioPlayerService::ProcessInterrupt(void)
{
    if (interruptHintPause_) {
        player_->Pause();
        currentState_ = AudioPlayerStates::AUDIO_PLAYER_PAUSED;
        interruptHintPause_ = false;
    }
    if (interruptHintResume_) {
        player_->Play(); // error
        currentState_ = AudioPlayerStates::AUDIO_PLAYER_PLAYED;
        interruptHintResume_ = false;
    }
}

int32_t AudioPlayerService::AudioPlayInner(void)
{
    int32_t ret = player_->Play();
    if (ret != 0) {
        MEDIA_ERR_LOG("player start failed!");
        return ret;
    }
    player_->EnableSingleLooping(playerLoopMode_ == AudioPlayerLoopMode::AUDIO_PLAYER_SINGLE_LOOP);
    currentState_ = AudioPlayerStates::AUDIO_PLAYER_PLAYED;
    isCutSong_ = false;
    OnPlay();
    return ret;
}

int32_t AudioPlayerService::AudioPlay(void)
{
    player_->SetPlayerCallback(shared_from_this());
    int32_t ret = 0;
    if (currentState_ != AudioPlayerStates::AUDIO_PLAYER_PAUSED) {
        ret = AudioPlayInner();
        if (ret != 0) {
            goto EXIT;
        }
    }
    while (!isPlaybackCompleted_ &&
        !isPlaybackStopped_ &&
        !interruptHintStop_ &&
        !playError_ &&
        !isCutSong_) {
        MEDIA_INFO_LOG("wait play start !");
        MediaThreadCondWait(cond_, mutex_);
        MEDIA_INFO_LOG("wait play end !");
        ProcessInterrupt();
        if (currentState_ == AudioPlayerStates::AUDIO_PLAYER_RESUMEING) {
            ret = AudioPlayInner();
            if (ret != 0) {
                goto EXIT;
            }
        }
    }
    return MEDIA_OK;
EXIT:
    MEDIA_ERR_LOG("play failed");
    (void)player_->Reset();
    (void)player_->Release();
    return MEDIA_ERR;
}

void AudioPlayerService::AudioStopAndExit(void)
{
    int32_t ret = player_->Stop();
    if (ret != 0) {
        MEDIA_ERR_LOG("stop failed");
    }
    ret = player_->Reset();
    if (ret != 0) {
        MEDIA_ERR_LOG("reset failed");
    }
    currentState_ = AudioPlayerStates::AUDIO_PLAYER_STOPED;
    isPlaybackCompleted_ = false;
    OnStop();
}

int32_t AudioPlayerService::ReleasePlayer(void)
{
    (void)player_->Reset();
    (void)player_->Release();
    (void)DeinitPlayerResources();
    return MEDIA_ERR;
}

int32_t AudioPlayerService::SetAudioSessionIdAndStreamType(void)
{
    int32_t ret = MEDIA_OK;
    ret = player_->SetAudioSessionId(sessionId_);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("set audio sessionId failed");
        return ret;
    }
    ret = player_->SetAudioStreamType(streamType_);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("set audio stream type failed");
        return ret;
    }
    return ret;
}

int32_t AudioPlayerService::RunAudioPlay(void)
{
    bool success = InitPlayerResources();
    if (!success) {
        MEDIA_ERR_LOG("init player resources failed");
        return MEDIA_ERR;
    }
    int32_t ret = 0;
    while(playIndex_ < playList_.size() &&
        !isPlaybackStopped_ &&
        !interruptHintStop_) {
        std::map<std::string, std::string> header;
        MediaMutexLock(mutex_);
        Source source = Source(playList_[playIndex_], header);
        MEDIA_INFO_LOG("current play source = %s, index = %d",
            source.GetSourceUri().c_str(), playIndex_);
        ret = player_->SetSource(source);
        if (ret != MEDIA_OK) {
            MEDIA_ERR_LOG("set source failed");
            MediaMutexUnLock(mutex_);
            (void)DeinitPlayerResources();
            return MEDIA_ERR;
        }
        ret = player_->Prepare();
        if (ret != MEDIA_OK) {
            MEDIA_ERR_LOG("prepare failed");
            MediaMutexUnLock(mutex_);
            return ReleasePlayer();
        }
        ret = SetAudioSessionIdAndStreamType();
        if (ret != MEDIA_OK) {
            MediaMutexUnLock(mutex_);
            return ReleasePlayer();
        }
        ret = AudioPlay();
        if (ret != MEDIA_OK) {
            MediaMutexUnLock(mutex_);
            return ReleasePlayer();
        }
        AudioStopAndExit();
        if (!isCutSong_) {
            playIndex_++;
            if (playIndex_ >= playList_.size()) {
                playIndex_ = 0;
            }
        }
        if (playerLoopMode_ != AudioPlayerLoopMode::AUDIO_PLAYER_PLAYLIST_LOOP) {
            MediaMutexUnLock(mutex_);
            MEDIA_ERR_LOG("isPlayListLoop exit");
            break;
        }
        MediaMutexUnLock(mutex_);
    }
    DeinitPlayerResources();
    return MEDIA_OK;
}

void AudioPlayerService::AudioPlayErrorCallBack(int32_t errorType, int32_t errorCode)
{
    MediaMutexLock(mutex_);
    if (callBack_ != nullptr) {
        MediaMutexUnLock(mutex_);
        callBack_->OnError(errorType, errorCode);
        return;
    }
    MediaMutexUnLock(mutex_);
}

void *AudioPlayerService::AudioPlayThreadProcess(void)
{
    int32_t ret = 0;
    bool success = false;
    bool isError = false;
    if (currentState_ == AudioPlayerStates::AUDIO_PLAYER_PLAYED) {
        MEDIA_ERR_LOG("the video is playing.");
        isError = true;
        goto EXIT;
    }
    success = RequsetAudioFocus();
    if (!success) {
        MEDIA_ERR_LOG("requset audio focus failed!");
        isError = true;
        goto EXIT;
    }
    ret = RunAudioPlay();
    if (ret != 0) {
        MEDIA_ERR_LOG("run audio play failed!");
        isError = true;
        goto EXIT;
    }
    success = ReleaseAudioFocus();
    if (!success) {
        MEDIA_ERR_LOG("release audio focus failed!");
    }
EXIT:
    MediaMutexLock(mutex_);
    currentState_ = AudioPlayerStates::AUDIO_PLAYER_EXITED;
    MediaMutexUnLock(mutex_);
    MEDIA_INFO_LOG("exit all!");
    MediaThreadCondSignal(exitCond_);
    if (isError) {
        AudioPlayErrorCallBack(PlayerCallback::PlayerErrorType::PLAYER_ERROR_UNKNOWN,
            PlayerCallback::PlayerErrorCode::PLAYER_ERROR_CODE_AUD_PLAY_FAIL);
    }
    return nullptr;
}

void *AudioPlayerService::AudioPlayThread(void *arg)
{
    AudioPlayerService *audioPlayer = (AudioPlayerService *)arg;
    return audioPlayer->AudioPlayThreadProcess();
}

AudioPlayerService::AudioPlayerService() noexcept
    : PlayerCallback(), InterruptListener()
{
}

AudioPlayerService::~AudioPlayerService()
{
    Reset();
#ifdef HMF_DECRYPT_DATA_ENABLE
    if (loadDecryptPath_ != nullptr) {
        delete[] loadDecryptPath_;
        loadDecryptPath_ = nullptr;
        loadDecryptPathLen_ = 0;
    }
#endif
    if (albumInfoPlayer_ != nullptr) {
        albumInfoPlayer_.reset();
        albumInfoPlayer_ = nullptr;
    }
}

std::shared_ptr<AudioPlayerService> AudioPlayerService::GetInstance()
{
    return instance_;
}

void AudioPlayerService::ResetAudioPlayState(void)
{
    isPlaybackCompleted_ = false;
    isPlaybackStopped_ = false;
    interruptHintStop_ = false;
    interruptHintResume_ = false;
    interruptHintStop_ = false;
    isBackgroundMode_ = false;
    isCutSong_ = false;
    playError_ = false;
}

int32_t AudioPlayerService::SetPlayListSource(std::vector<std::string> playlist, int32_t index)
{
    if (currentState_ != AudioPlayerStates::AUDIO_PLAYER_IDLE) {
        MEDIA_ERR_LOG("The current status[%d] is not IDLE. Cannot set the playlist.!", currentState_);
        return MEDIA_ERR;
    }
    if (index >= playlist.size()) {
        MEDIA_ERR_LOG("The playback index is incorrect. The length[%d] of the playlist exceeds the maximum.!", index);
        return MEDIA_ERR;
    }
    playList_ = playlist;
    playIndex_ = index;
    return MEDIA_OK;
}

int32_t AudioPlayerService::SetPlaySource(std::string src)
{
    if (currentState_ != AudioPlayerStates::AUDIO_PLAYER_IDLE) {
        MEDIA_ERR_LOG("The current status[%d] is incorrect. Cannot set the play source.!", currentState_);
        return MEDIA_ERR;
    }
    size_t i = 0;
    for (; i < playList_.size(); i++) {
        if (playList_[i] == src) {
            playIndex_ = i;
            break;
        }
    }
    if (i == playList_.size()) {
        playList_.push_back(src);
        playIndex_ = playList_.size() - 1;
    }
    return MEDIA_OK;
}

int32_t AudioPlayerService::SetBackgroundMode(bool isBackground)
{
    MediaMutexLock(mutex_);
    isBackgroundMode_ = isBackground;
    MediaMutexUnLock(mutex_);
    return MEDIA_OK;
}

#ifdef HMF_DECRYPT_DATA_ENABLE
void AudioPlayerService::SetDecryptLibraryPath(char *path, uint32_t len)
{
    if (loadDecryptPath_ == nullptr) {
        loadDecryptPath_ = new char[len];
        loadDecryptPathLen_ = len;
    }
    if (loadDecryptPathLen_ != len) {
        delete[] loadDecryptPath_;
        loadDecryptPath_ = new char[len];
        loadDecryptPathLen_ = len;
    }
    if (loadDecryptPath_ == nullptr) {
        MEDIA_ERR_LOG("loadDecryptPath_ new failed!");
        loadDecryptPathLen_ = 0;
        return;
    }
    if (memcpy_s(loadDecryptPath_, loadDecryptPathLen_, path, len) != EOK) {
        MEDIA_ERR_LOG("memcpy_s  decrypt path failed!");
        delete[] loadDecryptPath_;
        loadDecryptPath_ = nullptr;
    }
}
#endif

bool AudioPlayerService::IsBackgroundMode(void)
{
    return isBackgroundMode_;
}

int32_t AudioPlayerService::SetAudioStreamType(AudioStreamType type)
{
    if (currentState_ != AudioPlayerStates::AUDIO_PLAYER_IDLE) {
        MEDIA_ERR_LOG("The current status[%d] is incorrect. Cannot set the stream type.!", currentState_);
        return MEDIA_ERR;
    }
    streamType_ = type;
    return MEDIA_OK;
}

void AudioPlayerService::SetAudioPlayerCallback(const std::shared_ptr<AudioPlayerServiceCallback> &cb)
{
    callBack_ = cb;
}

int32_t AudioPlayerService::StartPrepare(void)
{
    if (mutex_ == nullptr) {
        mutex_ = MediaMutexCreate(nullptr);
        if (mutex_ == nullptr) {
            MEDIA_ERR_LOG("create mutex failed");
            return MEDIA_ERR;
        }
    }
    if (exitCond_ == nullptr) {
        exitCond_ = MediaThreadCondCreate();
        if (exitCond_ == nullptr) {
            MEDIA_ERR_LOG("create exit thread cond failed");
            (void)MediaMutexDestroy(&mutex_);
            return MEDIA_ERR;
        }
    }
    return MEDIA_OK;
}

bool AudioPlayerService::HasM3U8(const std::string& str)
{
    size_t pos = str.find(".m3u8");
    return pos != std::string::npos;
}

int32_t AudioPlayerService::Start(void)
{
    if (currentState_ != AudioPlayerStates::AUDIO_PLAYER_IDLE) {
        MEDIA_INFO_LOG("audio play is not idle status[%d], can not start!", currentState_);
        return MEDIA_OK;
    }
#ifdef HMF_DECRYPT_DATA_ENABLE
    if (loadDecryptPath_ != nullptr) {
        AUDIO_LoadDecryptLibrary(loadDecryptPath_);
    }
#endif
    int32_t ret = StartPrepare();
    if (ret != 0) {
        return ret;
    }
    ResetAudioPlayState();
    currentState_ = AudioPlayerStates::AUDIO_PLAYER_ENTERED;
    MediaThreadattr attr = { "AudioPlayThread", 0x1800, THREAD_SCHED_INVALID, 0, true };
    if (HasM3U8(playList_.at(playIndex_))) {
        attr.stackSize = 0x9000;
    }
    threadHandle_ = MediaThreadCreate(AudioPlayThread, this, &attr);
    if (threadHandle_ == nullptr) {
        MEDIA_ERR_LOG("create thread failed");
        (void)MediaMutexDestroy(&mutex_);
        (void)MediaThreadCondDestroy(&exitCond_);
        currentState_ = AudioPlayerStates::AUDIO_PLAYER_IDLE;
        return MEDIA_ERR;
    }
    return MEDIA_OK;
}

int32_t AudioPlayerService::Pause(void)
{
    MediaMutexLock(mutex_);
    if (currentState_ == AudioPlayerStates::AUDIO_PLAYER_IDLE ||
        isPlaybackStopped_ ||
        isPlaybackCompleted_ ||
        interruptHintStop_ ||
        currentState_ == AudioPlayerStates::AUDIO_PLAYER_PAUSED ||
        currentState_ > AudioPlayerStates::AUDIO_PLAYER_PLAYED) {
        MEDIA_ERR_LOG("currentstate[%d] is invaild, can not pause play", currentState_);
        MediaMutexUnLock(mutex_);
        return MEDIA_ERR;
    }
    if (player_ == nullptr ||
        currentState_ < AudioPlayerStates::AUDIO_PLAYER_PLAYED) {
        currentState_ = AudioPlayerStates::AUDIO_PLAYER_PAUSED;
        MediaMutexUnLock(mutex_);
        return MEDIA_OK;
    }
    int32_t ret = player_->Pause();
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("pause failed");
        MediaMutexUnLock(mutex_);
        return MEDIA_ERR;
    }
    currentState_ = AudioPlayerStates::AUDIO_PLAYER_PAUSED;
    MediaMutexUnLock(mutex_);
    OnPause();
    return ret;
}

int32_t AudioPlayerService::Resume(void)
{
    MediaMutexLock(mutex_);
    if (currentState_ != AudioPlayerStates::AUDIO_PLAYER_PAUSED ||
        isPlaybackStopped_ ||
        isPlaybackCompleted_ ||
        interruptHintStop_) {
        MEDIA_ERR_LOG("currentstate[%d] is invaild, can not resume play", currentState_);
        MediaMutexUnLock(mutex_);
        return MEDIA_ERR;
    }
    if (player_ == nullptr ||
        (currentState_ < AudioPlayerStates::AUDIO_PLAYER_PLAYED &&
        currentState_ != AudioPlayerStates::AUDIO_PLAYER_PAUSED)) {
        MEDIA_INFO_LOG("resume success status[%d]!", currentState_);
        currentState_ = AudioPlayerStates::AUDIO_PLAYER_RESUMEING;
        MediaMutexUnLock(mutex_);
        return MEDIA_OK;
    }
    currentState_ = AudioPlayerStates::AUDIO_PLAYER_RESUMEING;
    MediaThreadCondSignal(cond_);
    MediaMutexUnLock(mutex_);
    MEDIA_INFO_LOG("resume success!");
    return MEDIA_OK;
}

int32_t AudioPlayerService::Reset(void)
{
    if (currentState_ == AudioPlayerStates::AUDIO_PLAYER_IDLE) {
        MEDIA_INFO_LOG("currently in IDLE state, no reset is required!");
        return MEDIA_OK;
    }
    if (currentState_ != AudioPlayerStates::AUDIO_PLAYER_STOPED) {
        (void)Stop();
    }
    MediaMutexLock(mutex_);
    playList_.clear();
    playIndex_ = 0;
    isPlaybackCompleted_ = false;
    isPlaybackStopped_ = false;
    interruptHintPause_ = false;
    interruptHintResume_ = false;
    interruptHintStop_ = false;
    isBackgroundMode_ = false;
    playError_ = false;
    playerLoopMode_ = AudioPlayerLoopMode::AUDIO_PLAYER_INVAILD_LOOP;
    isCutSong_ = false;
    currentState_ = AudioPlayerStates::AUDIO_PLAYER_IDLE;
    streamType_ = AUDIO_STREAM_MUSIC;
    MediaMutexUnLock(mutex_);
    (void)MediaMutexDestroy(&mutex_);
    (void)MediaThreadCondDestroy(&exitCond_);
#ifdef HMF_DECRYPT_DATA_ENABLE
    AUDIO_UnLoadDecryptLibrary();
#endif
    MEDIA_INFO_LOG("reset success[%d]", currentState_);
    return MEDIA_OK;
}

int32_t AudioPlayerService::GetDumpInfo(PlayerDebugInfo *playerInfo)
{
    MediaMutexLock(mutex_);
    if (player_ == nullptr) {
        MEDIA_ERR_LOG("can not get dump info, player is NULL!");
        MediaMutexUnLock(mutex_);
        return MEDIA_ERR;
    }
    int32_t ret = player_->DumpInfo(playerInfo);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("get dump info failed");
        MediaMutexUnLock(mutex_);
        return ret;
    }
    MediaMutexUnLock(mutex_);
    return ret;
}

int32_t AudioPlayerService::Seek(int64_t mSeconds)
{
    MediaMutexLock(mutex_);
    if (currentState_ != AudioPlayerStates::AUDIO_PLAYER_PLAYED &&
        currentState_ != AudioPlayerStates::AUDIO_PLAYER_PAUSED) {
        MEDIA_ERR_LOG("The operation cannot be performed because the current state is not playing.");
        MediaMutexUnLock(mutex_);
        return MEDIA_ERR;
    }
    if (player_ == nullptr) {
        MEDIA_ERR_LOG("can not seek! player is NULL");
        MediaMutexUnLock(mutex_);
        return MEDIA_ERR;
    }
    int32_t ret = player_->Rewind(mSeconds, PLAYER_SEEK_CLOSEST_SYNC);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("video player seek failed!");
    }
    MediaMutexUnLock(mutex_);
    return ret;
}

int32_t AudioPlayerService::GetCurrentTime(int64_t &time)
{
    MediaMutexLock(mutex_);
    if (player_ == nullptr) {
        MEDIA_ERR_LOG("can not get current play position! player is NULL");
        MediaMutexUnLock(mutex_);
        return MEDIA_ERR;
    }
    int32_t ret = player_->GetCurrentTime(time);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("audio player get current play position failed!");
    }
    MediaMutexUnLock(mutex_);
    return ret;
}

int32_t AudioPlayerService::Stop(void)
{
    MediaMutexLock(mutex_);
    if (currentState_ < AudioPlayerStates::AUDIO_PLAYER_ENTERED ||
        currentState_ > AudioPlayerStates::AUDIO_PLAYER_PLAYED) {
        MEDIA_ERR_LOG("The operation cannot be performed because the current state[%d] is not playing.",
            currentState_);
        MediaMutexUnLock(mutex_);
        return MEDIA_ERR;
    }
    if (isPlaybackStopped_ || isPlaybackCompleted_ || interruptHintStop_) {
        if (playerLoopMode_ == AudioPlayerLoopMode::AUDIO_PLAYER_SINGLE_LOOP) {
            if (!isPlaybackCompleted_) {
                MEDIA_ERR_LOG("audio play already stopped");
                MediaMutexUnLock(mutex_);
                return MEDIA_OK;
            }
        } else {
            MEDIA_ERR_LOG("audio play already stopped");
            MediaMutexUnLock(mutex_);
            return MEDIA_OK;
        }
    }
    isPlaybackStopped_ = true;
    playIndex_ = playList_.size();
    MediaThreadCondSignal(cond_);
    MEDIA_INFO_LOG("MediaThreadCondWait start Stop!");
    MediaThreadCondWait(exitCond_, mutex_);
    MediaMutexUnLock(mutex_);
    MEDIA_INFO_LOG("MediaThreadCondWait end Stop!");
    return MEDIA_OK;
}

int32_t AudioPlayerService::PlayNext(void)
{
    MediaMutexLock(mutex_);
    if (currentState_ < AudioPlayerStates::AUDIO_PLAYER_ENTERED ||
        currentState_ > AudioPlayerStates::AUDIO_PLAYER_PLAYED) {
        MEDIA_ERR_LOG("The operation cannot be performed because the current state[%d] is not playing.",
            currentState_);
        MediaMutexUnLock(mutex_);
        return MEDIA_ERR;
    }
    playIndex_++;
    if (playIndex_ >= playList_.size()) {
        playIndex_ = 0;
    }
    MEDIA_INFO_LOG("PlayNext current index[%d]", playIndex_);
    isCutSong_ = true;
    MediaThreadCondSignal(cond_);
    MediaMutexUnLock(mutex_);
    return MEDIA_OK;
}

int32_t AudioPlayerService::PlayPrev(void)
{
    MediaMutexLock(mutex_);
    if (currentState_ < AudioPlayerStates::AUDIO_PLAYER_ENTERED ||
        currentState_ > AudioPlayerStates::AUDIO_PLAYER_PLAYED) {
        MEDIA_ERR_LOG("The operation cannot be performed because the current state[%d] is not playing.",
            currentState_);
        MediaMutexUnLock(mutex_);
        return MEDIA_ERR;
    }
    playIndex_--;
    if (playIndex_ < 0) {
        playIndex_ = playList_.size() - 1;
    }
    MEDIA_INFO_LOG("PlayNext current index[%d]", playIndex_);
    isCutSong_ = true;
    MediaThreadCondSignal(cond_);
    MediaMutexUnLock(mutex_);
    return MEDIA_OK;
}

int32_t AudioPlayerService::GetPlayStatus(void)
{
    MediaMutexLock(mutex_);
    int32_t state = (int32_t)currentState_;
    MediaMutexUnLock(mutex_);
    return state;
}

int32_t AudioPlayerService::SetPlayLoopMode(AudioPlayerLoopMode loopMode)
{
    if (loopMode < AudioPlayerLoopMode::AUDIO_PLAYER_PLAYLIST_LOOP ||
        loopMode > AudioPlayerLoopMode::AUDIO_PLAYER_INVAILD_LOOP) {
        MEDIA_INFO_LOG("invalid loop mode[%d]!", loopMode);
        return MEDIA_ERR;
    }
    playerLoopMode_ = loopMode;
    if (player_ != nullptr) {
        if (loopMode == AudioPlayerLoopMode::AUDIO_PLAYER_SINGLE_LOOP) {
            return player_->EnableSingleLooping(true);
        } else {
            return player_->EnableSingleLooping(false);
        }
    }
    return MEDIA_OK;
}

bool AudioPlayerService::IsSingleLooping(void)
{
    return playerLoopMode_ == AudioPlayerLoopMode::AUDIO_PLAYER_SINGLE_LOOP;
}

bool AudioPlayerService::IsPlayListLooping(void)
{

    return playerLoopMode_ == AudioPlayerLoopMode::AUDIO_PLAYER_PLAYLIST_LOOP;
}

bool AudioPlayerService::IsPlaying(void)
{
    if (player_ == nullptr) {
        MEDIA_INFO_LOG("player_ is NULL!");
        return false;
    }
    return player_->IsPlaying();
}

std::string AudioPlayerService::GetCurrentPlaySource(void)
{
    std::string source = "";
    MediaMutexLock(mutex_);
    if (playIndex_ < 0 || playIndex_ >= playList_.size()) {
        MEDIA_ERR_LOG("playIndex_[%d] is invalid!", playIndex_);
        MediaMutexUnLock(mutex_);
        return "";
    }
    source = playList_.at(playIndex_);
    MediaMutexUnLock(mutex_);
    return source;
}

int32_t AudioPlayerService::GetDuration(int64_t &durationMs)
{
    if (player_ == nullptr) {
        MEDIA_ERR_LOG("player_ is NULL!");
        return MEDIA_ERR;
    }
    return player_->GetDuration(durationMs);
}

void AudioPlayerService::OnPlay()
{
    if (callBack_ == nullptr) {
        MEDIA_ERR_LOG("callBack_ is NULL!");
        return;
    }
    callBack_->OnPlay();
}

void AudioPlayerService::OnPause()
{
    if (callBack_ == nullptr) {
        MEDIA_ERR_LOG("callBack_ is NULL!");
        return;
    }
    callBack_->OnPause();
}

void AudioPlayerService::OnStop()
{
    if (callBack_ == nullptr) {
        MEDIA_ERR_LOG("callBack_ is NULL!");
        return;
    }
    callBack_->OnStop();
}

void AudioPlayerService::GetAlbumInfo(AudioPlayerAlbumInfo &albumInfo, const char *src)
{
    if (src == nullptr) {
        MEDIA_ERR_LOG("src is nullptr!");
        return;
    }
    if (albumInfoPlayer_ == nullptr) {
        albumInfoPlayer_= std::make_shared<Player>();
    }
    if (strncmp(src, "http", 4) == 0) { // 4 means lenth of string "http"
        MEDIA_ERR_LOG("GetAlbumInfo unsupport http play");
        return;
    }
    albumInfo.src = src;
    Source source = Source(src);
    Format fmtAlbumInfo;
    fmtAlbumInfo.PutStringValue(FORMAT_KEY, FORMAT_GET_ALBUM_INFO);
    (void)albumInfoPlayer_->SetSource(source);
    (void)albumInfoPlayer_->Prepare();
    int32_t ret = albumInfoPlayer_->GetParameter(fmtAlbumInfo);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("albumInfoPlayer_ GetfmtAlbumInfo failed:%d", ret);
        albumInfo.title = "";
        albumInfo.artist = "";
        albumInfoPlayer_->Reset();
        return;
    }
    std::string value;
    fmtAlbumInfo.GetStringValue(FORMAT_KEY_TITLE, value);
    albumInfo.title = value;
    fmtAlbumInfo.GetStringValue(FORMAT_KEY_ARTIST, value);
    albumInfo.artist = value;
    albumInfoPlayer_->Reset();
    return;
}
} // namespace OHOS
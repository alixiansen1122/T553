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

#include "audio_player.h"
#include <cerrno>
#include <sys/prctl.h>
#include <sys/time.h>
#include <unistd.h>
#include "js_async_work.h"
#include "media_log.h"
#include "bundlems_slite_client.h"
#include "abilityms_slite_client.h"
#include "js_ability.h"

#define DELETE_NOT_NULL(pointer) \
    if ((pointer) != nullptr) {  \
        delete (pointer);        \
        (pointer) = nullptr;     \
    }

namespace OHOS {
namespace ACELite {
constexpr char STATUS_PLAY[] = "play";
constexpr char STATUS_PAUSE[] = "pause";
constexpr char STATUS_STOP[] = "stop";
constexpr uint16_t MAX_VOLUME = 100;
constexpr double DEFAULT_VOLUME = 1.0;
constexpr double MILLISECONDS_PER_SECOND = 1000.0;
constexpr uint32_t MICROSECONDS_PER_SECOND = 1000 * 1000;
char absPath[MAX_PATH_LEN] = {0};

static Audio::AudioManager &g_amInstance = Audio::AudioManager::GetInstance();

void TriggerEventListener(const AudioEventListener *listener)
{
    if (listener == nullptr) {
        // listener is nullptr
        return;
    }
    if (!JsAsyncWork::DispatchAsyncWork(AsyncExecuteCallback, const_cast<AudioEventListener *>(listener))) {
        MEDIA_ERR_LOG("dispatch async work failed.");
    }
}

void AsyncExecuteCallback(void *arg)
{
    AudioEventListener *listener = static_cast<AudioEventListener *>(arg);
    if (listener == nullptr) {
        MEDIA_ERR_LOG("async execute callback failed.");
        return;
    }
    listener->OnTrigger();
}

AudioEventListener::AudioEventListener(JSIValue callback) : callback_(JSI::AcquireValue(callback))
{}

AudioEventListener::~AudioEventListener()
{
    if (!JSI::ValueIsUndefined(callback_)) {
        JSI::ReleaseValue(callback_);
    }
}

JSIValue AudioEventListener::GetCallback() const
{
    return callback_;
}

void AudioEventListener::OnTrigger() const
{
    if (JSI::ValueIsFunction(callback_)) {
        JSI::CallFunction(callback_, JSI::CreateUndefined(), nullptr, 0);
    }
}

AudioPlayerCallback::AudioPlayerCallback(AudioPlayer *audioPlayer) : AudioPlayerServiceCallback(), audioPlayer_(audioPlayer)
{}

void AudioPlayerCallback::OnPlaybackComplete()
{
    MEDIA_INFO_LOG("AudioPlayerCallback::OnPlaybackComplete enter");

    TriggerEventListener(audioPlayer_->GetOnEndedListener());

    MEDIA_INFO_LOG("AudioPlayerCallback::OnPlaybackComplete exit");
}

void AudioPlayerCallback::OnPlay()
{
    MEDIA_INFO_LOG("AudioPlayerCallback::OnPlay enter");

    TriggerEventListener(audioPlayer_->GetOnPlayListener());

    MEDIA_INFO_LOG("AudioPlayerCallback::OnPlay exit");
}

void AudioPlayerCallback::OnPause()
{
    MEDIA_INFO_LOG("AudioPlayerCallback::OnPause enter");

    TriggerEventListener(audioPlayer_->GetOnPauseListener());

    MEDIA_INFO_LOG("AudioPlayerCallback::OnPause exit");
}

void AudioPlayerCallback::OnStop()
{
    MEDIA_INFO_LOG("AudioPlayerCallback::OnStop enter");

    TriggerEventListener(audioPlayer_->GetOnStopListener());

    MEDIA_INFO_LOG("AudioPlayerCallback::OnStop exit");
}

void AudioPlayerCallback::OnError(int32_t errorType, int32_t errorCode)
{
    MEDIA_UNUSED(errorType);
    MEDIA_UNUSED(errorCode);
    MEDIA_INFO_LOG("AudioPlayerCallback::OnError enter");
    TriggerEventListener(audioPlayer_->GetOnErrorListener());
    MEDIA_INFO_LOG("AudioPlayerCallback::OnError exit");
}

void InterruptAsyncExecuteCallback(void *arg)
{
    int *num = static_cast<int *>(arg);
    if (*num == INTERRUPT_HINT_PAUSE) {
        AudioPlayer::GetInstance()->Pause();
    } else if (*num == INTERRUPT_HINT_RESUME) {
        AudioPlayer::GetInstance()->Play();
    } else if (*num == INTERRUPT_HINT_STOP) {
        AudioPlayer::GetInstance()->ResetPlayer();
    }
}

void AudioPlayerCallback::OnInterrupt(int32_t type, int32_t hint)
{
    if (type == INTERRUPT_TYPE_BEGIN && hint == INTERRUPT_HINT_PAUSE) {
        if (!JsAsyncWork::DispatchAsyncWork(InterruptAsyncExecuteCallback, &hint)) {
            MEDIA_ERR_LOG("dispatch async work failed. PAUSE");
        }
    } else if (type == INTERRUPT_TYPE_END && hint == INTERRUPT_HINT_RESUME) {
        if (!JsAsyncWork::DispatchAsyncWork(InterruptAsyncExecuteCallback, &hint)) {
            MEDIA_ERR_LOG("dispatch async work failed. PLAY");
        }
    } else if (type == INTERRUPT_TYPE_BEGIN && hint == INTERRUPT_HINT_STOP) {
        if (!JsAsyncWork::DispatchAsyncWork(InterruptAsyncExecuteCallback, &hint)) {
            MEDIA_ERR_LOG("dispatch async work failed. STOP");
        }
    }
}

MediaMutexHandle AudioPlayer::lock_;
MediaThreadCondHandle AudioPlayer::condition_;

AudioPlayer *AudioPlayer::GetInstance()
{
    static AudioPlayer audioPlayer;
    return &audioPlayer;
}

AudioPlayer::AudioPlayer()
    : onPlayListener_(nullptr), onPauseListener_(nullptr), onStopListener_(nullptr),
      onLoadedDataListener_(nullptr), onEndedListener_(nullptr), onErrorListener_(nullptr),
      onTimeUpdateListener_(nullptr), src_(nullptr), srcInner_(nullptr), status_(STATUS_STOP), volume_(DEFAULT_VOLUME),
      autoPlay_(false), isRunning_(false), isSysSrc_(false), isPlayList_(false), updateTimeProcess_(nullptr)
{}

AudioPlayer::~AudioPlayer()
{
    ReleaseSrc();
}

void AudioPlayer::ForkUpdateTimeThread()
{
    MEDIA_DEBUG_LOG("fork update time thread.");
    if (isRunning_) {
        return;
    }
    isRunning_ = true;
    lock_ = MediaMutexCreate(nullptr);
    condition_ = MediaThreadCondCreate();

    MediaThreadattr attr = {"UpdateTimeHandler", 0x1000, THREAD_SCHED_INVALID, 0, false};
    updateTimeProcess_ = MediaThreadCreate(AudioPlayer::UpdateTimeHandler, this, &attr);
    if (updateTimeProcess_ == nullptr) {
        MEDIA_ERR_LOG("fork thread failed.");
        MediaMutexLock(lock_);
        isRunning_ = false;
        MediaMutexUnLock(lock_);
        MediaMutexDestroy(&lock_);
        MediaThreadCondDestroy(&condition_);
        return;
    }
    MEDIA_DEBUG_LOG("fork thread success");
}

void *AudioPlayer::UpdateTimeHandler(void *arg)
{
    if (arg == nullptr) {
        MEDIA_ERR_LOG("audio player is null.");
        return nullptr;
    }
    AudioPlayer *audioPlayer = static_cast<AudioPlayer *>(arg);

    struct timespec timeout;
    int retCode;
    while (audioPlayer->isRunning_) {
        MediaMutexLock(lock_);
        if (!audioPlayer->IsPlaying()) {
            // waiting for playing
            MediaThreadCondTimeWait(condition_, lock_, MICROSECONDS_PER_SECOND);
        } else {
            MediaThreadCondTimeWait(condition_, lock_, MICROSECONDS_PER_SECOND);
            TriggerEventListener(audioPlayer->GetOnTimeUpdateListener());
        }
        MediaMutexUnLock(lock_);
    }
    MediaMutexLock(lock_);
    audioPlayer->isRunning_ = false;
    MediaMutexUnLock(lock_);
    MEDIA_INFO_LOG("update time handler end!");
    return nullptr;
}

int32_t AudioPlayer::GetSrcInnerPrefix(const char *src, char *dest)
{
    int ret = -1;
    const char *bundleName = OHOS::ACELite::JSAbility::GetPackageName();
    if (bundleName != nullptr) {
        BundleInfo bundleInfo;
        ret = BundleMsClient::GetInstance().GetBundleInfo(bundleName, 0, &bundleInfo);
        if (ret == 0) {
            int size = sprintf_s(dest, MAX_PATH_LEN - 1, "%s/assets/entry/%s", bundleInfo.codePath, src);
            ret = (size < 0) ? -1 : 0;
        } else {
            MEDIA_ERR_LOG("failed to get bundleInfo!");
        }
    } else {
        MEDIA_ERR_LOG("failed to get jsapp's bundleName!");
    }
    return ret;
}

void AudioPlayer::StopUpdateTimeThread()
{
    MediaMutexLock(lock_);
    isRunning_ = false;
    MediaMutexUnLock(lock_);
    if (updateTimeProcess_ != nullptr) {
        MediaThreadJoin(&updateTimeProcess_);
    }
    MediaMutexDestroy(&lock_);
    MediaThreadCondDestroy(&condition_);
}

bool AudioPlayer::ResetPlayer()
{
    ReleaseSrc();
    isPlayList_ = false;
    if (AudioPlayerService::GetInstance()->Reset() != 0) {
        MEDIA_ERR_LOG("fail to reset audio.");
        return false;
    }
    return true;
}

void AudioPlayer::ReleaseEventListeners()
{
    DELETE_NOT_NULL(onPlayListener_);
    DELETE_NOT_NULL(onPauseListener_);
    DELETE_NOT_NULL(onStopListener_);
    DELETE_NOT_NULL(onLoadedDataListener_);
    DELETE_NOT_NULL(onEndedListener_);
    DELETE_NOT_NULL(onErrorListener_);
    DELETE_NOT_NULL(onTimeUpdateListener_);
    callback_ = nullptr;
}

void AudioPlayer::ReleaseSrc()
{
    if (src_ != nullptr && isSysSrc_) {
        ace_free(src_);
        src_ = nullptr;
    }
    if (srcInner_ != nullptr) {
        ace_free(srcInner_);
        srcInner_ = nullptr;
    }
    (void)memset_s(absPath, MAX_PATH_LEN, 0, MAX_PATH_LEN);
}

#ifdef HMF_DECRYPT_DATA_ENABLE
void AudioPlayer::SetDecryptLibraryPath(char *path, uint32_t len)
{
    AudioPlayerService::GetInstance()->SetDecryptLibraryPath(path, len);
}
#endif

bool AudioPlayer::Play()
{
    if (AudioPlayerService::GetInstance()->GetPlayStatus() ==
        (int32_t)AudioPlayerStates::AUDIO_PLAYER_PAUSED) {
        if (AudioPlayerService::GetInstance()->Resume() != 0) {
            MEDIA_ERR_LOG("resume failed!");
            return false;
        }
        MediaThreadCondSignal(condition_);
        status_ = STATUS_PLAY;
        return true;
    }
    if (AudioPlayerService::GetInstance()->SetAudioStreamType(streamType_) != 0) {
        MEDIA_ERR_LOG("fail to SetAudioStreamType streamType = %x.", streamType_);
        return false;
    }
    callback_ = std::make_shared<AudioPlayerCallback>(this);
    AudioPlayerService::GetInstance()->SetAudioPlayerCallback(callback_);
    if (AudioPlayerService::GetInstance()->Start() != 0) {
        MEDIA_ERR_LOG("fail to play audio.");
        return false;
    }
    MediaThreadCondSignal(condition_);
    status_ = STATUS_PLAY;
    return true;
}

bool AudioPlayer::Pause()
{
    if (AudioPlayerService::GetInstance()->Pause() != 0) {
        MEDIA_ERR_LOG("fail to pause audio.");
        return false;
    }
    status_ = STATUS_PAUSE;
    return true;
}

bool AudioPlayer::Stop()
{
    if (AudioPlayerService::GetInstance()->Stop() != 0) {
        MEDIA_ERR_LOG("fail to stop audio.");
        return false;
    }
    status_ = STATUS_STOP;
    return true;
}

char *AudioPlayer::GetSrc(bool innerFlag) const
{
    if (!IsPlayListLoop()) {
        if (srcInner_) {
            return absPath;
        }
        return src_;
    } else {
        std::string str = AudioPlayerService::GetInstance()->GetCurrentPlaySource();
        if (!str.empty()) {
            (void)memset_s(absPath, MAX_PATH_LEN, 0, MAX_PATH_LEN);
            if (strcpy_s(absPath, MAX_PATH_LEN, str.c_str()) != EOK) {
                MEDIA_ERR_LOG("strcpy_s failed");
                return nullptr;
            }
            return absPath;
        }
    }
    return nullptr;
}

bool AudioPlayer::SetSrcList(std::vector<std::string> srcList)
{
    if (src_ != nullptr ||
        srcInner_ != nullptr) {
        MEDIA_ERR_LOG("the src interface should not be called before setting the playlist!");
        return false;
    }
    int32_t ret = AudioPlayerService::GetInstance()->SetPlayListSource(srcList);
    if (ret != 0) {
        MEDIA_ERR_LOG("audio player set play list failed!.");
        return false;
    }
    isPlayList_ = true;
    if (playLoopMode_ != AudioPlayerLoopMode::AUDIO_PLAYER_INVAILD_LOOP) {
        return SetLoop(true);
    }
    return true;
}

bool AudioPlayer::SetBackgroundPlay(bool enable)
{
    int32_t ret = AudioPlayerService::GetInstance()->SetBackgroundMode(enable);
    if (ret != 0) {
        MEDIA_ERR_LOG("audio player set backgroundPlay failed!.");
        return false;
    }
    return true;
}

bool AudioPlayer::IsBackgroundPlay(void)
{
    return AudioPlayerService::GetInstance()->IsBackgroundMode();
}

double AudioPlayer::GetCurrentTime() const
{
    if (status_ == STATUS_STOP) {
        return -1;
    }
    int64_t currentTime = 0;
    if (AudioPlayerService::GetInstance()->GetCurrentTime(currentTime) != 0) {
        MEDIA_ERR_LOG("fail to get audio currentTime property.");
    }
    return currentTime / MILLISECONDS_PER_SECOND;
}

double AudioPlayer::GetDuration() const
{
    if (status_ == STATUS_STOP) {
        return -1;
    }

    int64_t duation = 0;
    if (AudioPlayerService::GetInstance()->GetDuration(duation) != 0) {
        MEDIA_ERR_LOG("fail to get audio duaiton property.");
    }
    return duation / MILLISECONDS_PER_SECOND;
}

bool AudioPlayer::GetAutoPlay() const
{
    return autoPlay_;
}

bool AudioPlayer::IsLooping() const
{
    if (isPlayList_) {
        return AudioPlayerService::GetInstance()->IsPlayListLooping();
    }
    return AudioPlayerService::GetInstance()->IsSingleLooping();
}

bool AudioPlayer::IsPlayListLoop() const
{
    return AudioPlayerService::GetInstance()->IsPlayListLooping();
}

bool AudioPlayer::IsMuted() const
{
    return g_amInstance.IsMute(streamType_);
}

const char *AudioPlayer::GetStatus() const
{
    return status_.c_str();
}

double AudioPlayer::GetVolume()
{
    int32_t volume = g_amInstance.GetVolume(streamType_);
    volume_ = static_cast<double>(volume) / MAX_VOLUME;
    return volume_;
}

bool AudioPlayer::IsPlaying() const
{
    return AudioPlayerService::GetInstance()->IsPlaying();
}

bool AudioPlayer::SetSrc(char *src, bool innerFlag)
{
    if (AudioPlayerService::GetInstance()->GetPlayStatus() !=
        (int32_t)AudioPlayerStates::AUDIO_PLAYER_IDLE) {
        (void)ResetPlayer();
    }
    (void)ReleaseSrc();
    std::string uri;
    if (innerFlag) {
        srcInner_ = src;
        (void)memset_s(absPath, MAX_PATH_LEN, 0, MAX_PATH_LEN);
        if (GetSrcInnerPrefix(src, absPath) != 0) {
            MEDIA_ERR_LOG("fail to get absolute path of srcInner.");
            return false;
        }
        uri += absPath;
        isSysSrc_ = false;
    } else {
        uri += src;
        src_ = src;
        isSysSrc_ = true;
    }
    if (AudioPlayerService::GetInstance()->SetPlaySource(uri) != 0) {
        MEDIA_ERR_LOG("fail to Set PlaySource.");
        return false;
    }
    if (!isPlayList_ && playLoopMode_ != AudioPlayerLoopMode::AUDIO_PLAYER_INVAILD_LOOP) {
        return SetLoop(true);
    }
    return true;
}

bool AudioPlayer::SetCurrentTime(double currentTime) const
{
    if (currentTime < 0) {
        MEDIA_ERR_LOG("currentTime must be larger than or equals 0.");
        return false;
    }
    int64_t position = static_cast<int64_t>(currentTime * MILLISECONDS_PER_SECOND);
    if (AudioPlayerService::GetInstance()->Seek(position) != 0) {
        MEDIA_ERR_LOG("fail to seek audio currentTime property.");
        return false;
    }
    return true;
}

bool AudioPlayer::SetAutoPlay(bool autoPlay)
{
    if (autoPlay && src_ != nullptr && status_ == STATUS_STOP) {
        if (!Play()) {
            MEDIA_ERR_LOG("fail to auto play audio.");
        }
    }
    autoPlay_ = autoPlay;
    return true;
}

bool AudioPlayer::SetLoop(bool loop)
{
    if (isPlayList_) {
        playLoopMode_ = loop ? AudioPlayerLoopMode::AUDIO_PLAYER_PLAYLIST_LOOP:
            AudioPlayerLoopMode::AUDIO_PLAYER_INVAILD_LOOP;
    } else {
        playLoopMode_ = loop ? AudioPlayerLoopMode::AUDIO_PLAYER_SINGLE_LOOP:
            AudioPlayerLoopMode::AUDIO_PLAYER_INVAILD_LOOP;
    }
    if (AudioPlayerService::GetInstance()->SetPlayLoopMode(playLoopMode_) != 0) {
        MEDIA_ERR_LOG("set single loop failed!");
        return false;
    }
    return true;
}

bool AudioPlayer::SetVolume(double volume)
{
    if (volume < 0 || volume > 1) {
        MEDIA_ERR_LOG("invalid parameter.");
        return false;
    }
    float parsedVolume = static_cast<float>(volume * MAX_VOLUME);
    bool succ = g_amInstance.SetVolume(streamType_, parsedVolume);
    if (!succ) {
        MEDIA_ERR_LOG("fail to set volume.");
        return false;
    }
    volume_ = volume;
    return true;
}

bool AudioPlayer::SetStreamType(AudioStreamType streamType)
{
    streamType_ = streamType;
    return true;
}

bool AudioPlayer::SetMuted(bool muted)
{
    bool ret = false;
    if (muted) {
        ret = g_amInstance.Mute(streamType_);
    } else {
        ret = g_amInstance.UnMute(streamType_);
    }
    return ret;
}

const AudioEventListener *AudioPlayer::GetOnPlayListener() const
{
    return onPlayListener_;
}

const AudioEventListener *AudioPlayer::GetOnPauseListener() const
{
    return onPauseListener_;
}

const AudioEventListener *AudioPlayer::GetOnStopListener() const
{
    return onStopListener_;
}

const AudioEventListener *AudioPlayer::GetOnLoadedDataListener() const
{
    return onLoadedDataListener_;
}

const AudioEventListener *AudioPlayer::GetOnEndedListener() const
{
    return onEndedListener_;
}

const AudioEventListener *AudioPlayer::GetOnErrorListener() const
{
    return onErrorListener_;
}

const AudioEventListener *AudioPlayer::GetOnTimeUpdateListener() const
{
    return onTimeUpdateListener_;
}

void AudioPlayer::SetOnPlayListener(AudioEventListener *onPlayListener)
{
    if (onPlayListener_ != nullptr) {
        delete onPlayListener_;
    }
    onPlayListener_ = onPlayListener;
}

void AudioPlayer::SetOnPauseListener(AudioEventListener *onPauseListener)
{
    if (onPauseListener_ != nullptr) {
        delete onPauseListener_;
    }
    onPauseListener_ = onPauseListener;
}

void AudioPlayer::SetOnStopListener(AudioEventListener *onStopListener)
{
    if (onStopListener_ != nullptr) {
        delete onStopListener_;
    }
    onStopListener_ = onStopListener;
}

void AudioPlayer::SetOnLoadedDataListener(AudioEventListener *onLoadedDataListener)
{
    if (onLoadedDataListener_ != nullptr) {
        delete onLoadedDataListener_;
    }
    onLoadedDataListener_ = onLoadedDataListener;
}

void AudioPlayer::SetOnEndedListener(AudioEventListener *onEndedListener)
{
    if (onEndedListener_ != nullptr) {
        delete onEndedListener_;
    }
    onEndedListener_ = onEndedListener;
}

void AudioPlayer::SetOnErrorListener(AudioEventListener *onErrorListener)
{
    if (onErrorListener_ != nullptr) {
        delete onErrorListener_;
    }
    onErrorListener_ = onErrorListener;
}

void AudioPlayer::SetOnTimeUpdateListener(AudioEventListener *onTimeUpdateListener)
{
    if (onTimeUpdateListener_ != nullptr) {
        delete onTimeUpdateListener_;
    }
    onTimeUpdateListener_ = onTimeUpdateListener;
}

bool AudioPlayer::IsSysSrc()
{
    return isSysSrc_;
}

void AudioPlayer::GetAlbumInfo(AudioPlayerAlbumInfo &albumInfo, const char *src)
{
    AudioPlayerService::GetInstance()->GetAlbumInfo(albumInfo, src);
}

}  // namespace ACELite
}  // namespace OHOS

/*
 * Copyright (c) CompanyNameMagicTag. 2023-2023. All rights reserved.
 * Description: video player wrapper
 * Author: Media Software Group
 * Create: 2023-12-10
 */

#include "video_play_wrapper.h"
#include "media_log.h"

const static int32_t WAIT_SURFACE_DEINIT_SUCCESS_US = 10000;
const static int32_t REQUEST_BUFFER_REPET_COUNT = 10;
static const int32_t WAIT_VIDEO_EXIT_SUCCESS_US = 10000;
static const uint32_t WAIT_VIDEO_EXIT_MAX_RETRY_COUNT = 100;

namespace OHOS {

static AudioManager &g_amInstance = AudioManager::GetInstance();
MediaVideoPlay *MediaVideoPlay::play = nullptr;

class VideoPlayerInterruptListener : public InterruptListener {
public:
    explicit VideoPlayerInterruptListener(MediaVideoPlay *videoPlay)
        : videoPlay_(videoPlay)
    {
    }
    ~VideoPlayerInterruptListener() override {};

    void OnInterrupt(int32_t type, int32_t hint) override
    {
        MEDIA_INFO_LOG("OnInterrupt, type:%d, hint:%d\n", type, hint);

        if (type == INTERRUPT_TYPE_BEGIN && hint == INTERRUPT_HINT_PAUSE) {
            MEDIA_INFO_LOG("player pause signal one\n");
        }
        if (type == INTERRUPT_TYPE_END && hint == INTERRUPT_HINT_RESUME) {
            MEDIA_INFO_LOG("player resume signal one\n");
        }
        if (type == INTERRUPT_TYPE_BEGIN && hint == INTERRUPT_HINT_STOP) {
            MEDIA_INFO_LOG("player stop begin signal one\n");
            MediaMutexLock(videoPlay_->mutex_);
            videoPlay_->interruptHintStop_ = true;
            MediaThreadCondSignal(videoPlay_->cond_);
            MediaMutexUnLock(videoPlay_->mutex_);
        }
        if (type == INTERRUPT_TYPE_END && hint == INTERRUPT_HINT_STOP) {
            MEDIA_INFO_LOG("player stop end signal one\n");
        }
    }

private:
    MediaVideoPlay *videoPlay_;
};

class VideoPlayCallback : public PlayerCallback {
public:
    explicit VideoPlayCallback(MediaVideoPlay *videoPlay)
        : videoPlay_(videoPlay)
    {
    }

    ~VideoPlayCallback() override
    {
    }

    void OnPlaybackComplete() override
    {
        MEDIA_INFO_LOG("on playback complete\n");
        MediaMutexLock(videoPlay_->mutex_);
        videoPlay_->isPlaybackCompleted_ = true;
        MediaThreadCondSignal(videoPlay_->cond_);
        MediaMutexUnLock(videoPlay_->mutex_);
    }

    void OnError(int32_t errorType, int32_t errorCode) override
    {
        if (errorType != PlayerErrorType::PLAYER_ERROR_UNKNOWN) {
            MEDIA_ERR_LOG("unsupport error type:%d", errorType);
            return;
        }
        if (errorCode == PlayerErrorCode::PLAYER_ERROR_CODE_AUD_PLAY_FAIL) {
            MEDIA_ERR_LOG("playback error signal one");
            MediaMutexLock(videoPlay_->mutex_);
            videoPlay_->playError_ = true;
            MediaThreadCondSignal(videoPlay_->cond_);
            MediaMutexUnLock(videoPlay_->mutex_);
        }
    }

    void OnInfo(int32_t type, int32_t extra) override
    {
        MEDIA_ERR_LOG("on playback info, type:%dm extra:%d", type, extra);
    }

    void OnRewindToComplete() override
    {
        MEDIA_ERR_LOG("on playback rewind to complete\n");
    }

private:
    MediaVideoPlay *videoPlay_;
};

static bool RequsetAudioFocus(MediaVideoPlay *videoPlay)
{
    if (videoPlay->isPureVideo_) {
        return true;
    }
    bool success = g_amInstance.Initialize();
    if (!success) {
        MEDIA_ERR_LOG("audiomanager init failed\n");
        return false;
    }

    AudioSession sessionId = g_amInstance.MakeSessionId();
    if (sessionId == AUDIO_SESSION_ID_NONE) {
        MEDIA_ERR_LOG("audio session id invalid\n");
        return false;
    }

    std::shared_ptr<VideoPlayerInterruptListener> interruptListener =
        std::make_shared<VideoPlayerInterruptListener>(videoPlay);
    if (interruptListener == nullptr || interruptListener.get() == nullptr) {
        MEDIA_ERR_LOG("video player interrupt listener is nullptr\n");
        return false;
    }

    AudioInterrupt interrupt = { AUDIO_STREAM_FITNESS_VIDEO, sessionId, interruptListener };
    if (g_amInstance.ActivateAudioInterrupt(interrupt) == INTERRUPT_FAILED) {
        MEDIA_ERR_LOG("activate audio interrupt failed\n");
        return false;
    }

    videoPlay->sessionId_ = sessionId;
    videoPlay->interrupt_ = interrupt;

    return true;
}

static bool ReleaseAudioFocus(MediaVideoPlay *videoPlay)
{
    if (videoPlay->isPureVideo_) {
        return true;
    }
    if (g_amInstance.DeactivateAudioInterrupt(videoPlay->interrupt_) != 0) {
        MEDIA_ERR_LOG("deactivate audio interrupt failed\n");
        return false;
    }

    return true;
}

static bool InitPlayerResources(MediaVideoPlay *videoPlay)
{
    videoPlay->mutex_ = MediaMutexCreate(nullptr);
    if (videoPlay->mutex_ == nullptr) {
        MEDIA_ERR_LOG("create mutex failed\n");
        return false;
    }
    videoPlay->cond_ = MediaThreadCondCreate();
    if (videoPlay->cond_ == nullptr) {
        MEDIA_ERR_LOG("create thread cond failed\n");
        MediaMutexDestroy(&videoPlay->mutex_);
        return false;
    }
    shared_ptr<Player> player = std::make_shared<Player>();
    if (player == nullptr || player.get() == nullptr) {
        MEDIA_ERR_LOG("player is nullptr\n");
        MediaMutexDestroy(&videoPlay->mutex_);
        MediaThreadCondDestroy(&videoPlay->cond_);
        return false;
    }

    // prepare buffers which are used to playing video
    LiteSurface* liteSurface = dynamic_cast<LiteSurface*>(videoPlay->surface_);
    if (liteSurface != nullptr) {
        liteSurface->PrepareBuffers();
    }
    videoPlay->player_ = player;
    return true;
}

static void DeinitPlayerResources(MediaVideoPlay *videoPlay)
{
    (void)videoPlay->player_.reset();
    videoPlay->isExited_ = true;
    videoPlay->isEntered_ = false;
    (void)MediaThreadCondDestroy(&videoPlay->cond_);
}

static int32_t VideoPlay(MediaVideoPlay *videoPlay)
{
    std::shared_ptr<PlayerCallback> callback = std::make_shared<VideoPlayCallback>(videoPlay);
    if (callback == nullptr || callback.get() == nullptr) {
        MEDIA_ERR_LOG("callback is nullptr\n");
        return MEDIA_ERR;
    }
    videoPlay->player_->SetPlayerCallback(callback);
    int32_t ret = 0;
    MediaMutexLock(videoPlay->mutex_);
    if (!videoPlay->isPause_) {
        ret = videoPlay->player_->Play(); 
        if (ret != 0) {
            goto EXIT;
        }
        videoPlay->player_->EnableSingleLooping(videoPlay->isLoop_);
        if (videoPlay->button_ != nullptr) {
            videoPlay->button_->SetText("Pause");
        }
    }
    videoPlay->isPlayed_ = true;
    while (!videoPlay->isPlaybackCompleted_ &&
        !videoPlay->isPlaybackStopped_ &&
        !videoPlay->interruptHintStop_ &&
        !videoPlay->playError_) {
        MediaThreadCondWait(videoPlay->cond_, videoPlay->mutex_);
        if (videoPlay->isResume_) {
            ret = videoPlay->player_->Play();
            if (ret != 0) {
                goto EXIT;
            }
            videoPlay->isResume_ = false;
            videoPlay->player_->EnableSingleLooping(videoPlay->isLoop_);
        }
    }
    MediaMutexUnLock(videoPlay->mutex_);
    return 0;
EXIT:
    MEDIA_ERR_LOG("play failed\n");
    (void)videoPlay->player_->Reset();
    (void)videoPlay->player_->Release();
    MediaMutexUnLock(videoPlay->mutex_);
    return MEDIA_ERR;
}

static void VideoStopAndExit(MediaVideoPlay *videoPlay)
{
    MediaMutexLock(videoPlay->mutex_);
    int32_t ret = videoPlay->player_->Stop();
    if (ret != 0) {
        MEDIA_ERR_LOG("stop failed");
    }
    ret = videoPlay->player_->Reset();
    if (ret != 0) {
        MEDIA_ERR_LOG("reset failed");
    }
    videoPlay->isPlaybackCompleted_ = false;
    videoPlay->isPlayed_ = false;
    videoPlay->isPause_ = false;
    if (videoPlay->button_ != nullptr) {
        videoPlay->button_->SetText("Play");
    }
    MediaMutexUnLock(videoPlay->mutex_);
}

static int32_t ReleasePlayer(MediaVideoPlay *videoPlay)
{
    (void)videoPlay->player_->Reset();
    (void)videoPlay->player_->Release();
    (void)DeinitPlayerResources(videoPlay);
    return MEDIA_ERR;
}

static void SendBackgroundFrame(MediaVideoPlay *videoPlay)
{
    int32_t ret = videoPlay->player_->Release();
    if (ret != 0) {
        MEDIA_ERR_LOG("release failed");
    }
    if (!videoPlay->needSendBackgroundFrame_) {
        (void)DeinitPlayerResources(videoPlay);
        return;
    }
    SurfaceBuffer *buf = nullptr;
    int32_t requestCount = 0;
    while ((buf = videoPlay->surface_->RequestBuffer()) == nullptr) {
        if (requestCount++ > REQUEST_BUFFER_REPET_COUNT) {
            MEDIA_ERR_LOG("request the last buffer failed");
            break;
        }
        LiteSurface *liteSurface = dynamic_cast<LiteSurface *>(videoPlay->surface_);
        if (liteSurface == nullptr) {
            MEDIA_ERR_LOG("liteSurface is null!");
            break;
        }
        videoPlay->surface_->CancelBuffer(liteSurface->GetBackBuf());
    }
    if (buf != nullptr) {
        buf->format = PIXEL_FMT_BUTT;
        videoPlay->surface_->FlushBuffer(buf);
    }
    (void)DeinitPlayerResources(videoPlay);
}

static Source GetVideoPlaySource(MediaVideoPlay *videoPlay)
{
    Source source;
    if (!videoPlay->uri_.empty()) {
        std::map<std::string, std::string> header;
        source = Source(videoPlay->uri_, header);
    } else if (videoPlay->fd_ > 0) {
        if (videoPlay->isLoop_) {
            int32_t ret = lseek(videoPlay->fd_, 0, SEEK_SET);
            if (ret == -1) {
                MEDIA_ERR_LOG("lseek[fd = %d] file fail, error[%d]\n", videoPlay->fd_, errno);
                return source;
            }
        }
        source = Source(videoPlay->fd_, videoPlay->offset_, 0);
    }
    return source;
}

static int32_t SetAudioSessionIdAndStreamType(MediaVideoPlay *videoPlay)
{
    int32_t ret = MEDIA_OK;
    if (!videoPlay->isPureVideo_) {
        ret = videoPlay->player_->SetAudioSessionId(videoPlay->sessionId_);
        if (ret != MEDIA_OK) {
            MEDIA_ERR_LOG("set audio sessionId failed\n");
            return ret;
        }
        ret = videoPlay->player_->SetAudioStreamType(AUDIO_STREAM_FITNESS_VIDEO);
        if (ret != MEDIA_OK) {
            MEDIA_ERR_LOG("set audio stream type failed\n");
            return ret;
        }
    }
    return ret;
}

static int32_t RunVideoPlay(MediaVideoPlay *videoPlay)
{
    bool success = InitPlayerResources(videoPlay);
    if (!success) {
        MEDIA_ERR_LOG("init player resources failed\n");
        return MEDIA_ERR;
    }
    int32_t ret = 0;
    Source source = GetVideoPlaySource(videoPlay);
    ret = videoPlay->player_->SetSource(source);
    if (ret != 0) {
        MEDIA_ERR_LOG("set source failed\n");
        (void)DeinitPlayerResources(videoPlay);
        return MEDIA_ERR;
    }
    ret = videoPlay->player_->Prepare();
    if (ret != 0) {
        MEDIA_ERR_LOG("prepare failed\n");
        return ReleasePlayer(videoPlay);
    }
    ret = SetAudioSessionIdAndStreamType(videoPlay);
    if (ret != 0) {
        return ReleasePlayer(videoPlay);
    }
    ret = videoPlay->player_->SetVideoSurface(videoPlay->surface_);
    if (ret != 0) {
        MEDIA_ERR_LOG("set video surface failed\n");
        return ReleasePlayer(videoPlay);
    }
    ret = VideoPlay(videoPlay);
    if (ret != 0) {
        return ReleasePlayer(videoPlay);
    }
    VideoStopAndExit(videoPlay);
    SendBackgroundFrame(videoPlay);
    return 0;
}

void *MediaVideoPlay::VideoPlayThread(void *arg)
{
    MediaVideoPlay *videoPlay = (MediaVideoPlay *)arg;
    if (videoPlay->isPlayed_) {
        MEDIA_ERR_LOG("the video is playing.\n");
        return nullptr;
    }
    bool success = RequsetAudioFocus(videoPlay);
    if (!success) {
        MEDIA_ERR_LOG("requset audio focus failed\n");
        return nullptr;
    }
    int32_t ret = RunVideoPlay(videoPlay);
    if (ret != 0) {
        // clear buffers
        LiteSurface* liteSurface = dynamic_cast<LiteSurface*>(videoPlay->surface_);
        liteSurface->AsyncClearBuffers();
        MEDIA_ERR_LOG("run video play failed\n");
    }

    success = ReleaseAudioFocus(videoPlay);
    if (!success) {
        MEDIA_ERR_LOG("release audio focus failed\n");
    }
    MediaMutexLock(videoPlay->mutex_);
    if (!videoPlay->isPlaybackStopped_ && videoPlay->IsSyncExitMode()) {
        if (videoPlay->threadHandle_ == nullptr) {
            MediaThreadCondWait(videoPlay->cond_, videoPlay->mutex_);
        }
        videoPlay->SetSyncExitMode(false);
        MediaThreadSetDetach(videoPlay->threadHandle_, true);
    }
    MediaMutexUnLock(videoPlay->mutex_);
    (void)MediaMutexDestroy(&videoPlay->mutex_);
    MEDIA_INFO_LOG("exit all!\n");
    return nullptr;
}

MediaVideoPlay::MediaVideoPlay(Surface *surface, UILabelButton *button1,
    std::string uri, bool isPureVideo)
    : surface_(surface),
      button_(button1),
      uri_(uri),
      isPureVideo_(isPureVideo)
{
    interrupt_.interruptListener = nullptr;
    interrupt_.sessionID = 0;
    interrupt_.streamType = AUDIO_STREAM_INVALID;
}

MediaVideoPlay::MediaVideoPlay(Surface *surface, int32_t fd, uint64_t offset, bool isPureVideo)
    : surface_(surface),
    fd_(fd),
    offset_(offset),
    isPureVideo_(isPureVideo)
{
    interrupt_.interruptListener = nullptr;
    interrupt_.sessionID = 0;
    interrupt_.streamType = AUDIO_STREAM_INVALID;
}

MediaVideoPlay::~MediaVideoPlay()
{
    DestroyVideoPlaySource();
    play = nullptr;
}

void MediaVideoPlay::ResetVideoPlayState(void)
{
    isExited_ = false;
    isPlaybackCompleted_ = false;
    isPlaybackStopped_ = false;
    interruptHintStop_ = false;
    playError_ = false;
    isSyncExitMode_ = true;
    isPause_ = false;
    isPlayed_ = false;
    isResume_ = false;
}

int32_t MediaVideoPlay::StartVideoPlay(void)
{
    if (isEntered_) {
        MEDIA_INFO_LOG("video play is not stop, can not start!");
        return MEDIA_OK;
    }

    isEntered_ = true;
    ResetVideoPlayState();
    MediaThreadattr attr = { "VideoPlayThread", 0x2000, THREAD_SCHED_INVALID, 0, true };
    if (isSyncExitMode_) {
        attr.detached = false;
    }
    threadHandle_ = MediaThreadCreate(VideoPlayThread, this, &attr);
    if (threadHandle_ == nullptr) {
        MEDIA_ERR_LOG("create thread failed\n");
        isEntered_ = false;
        MediaThreadCondSignal(cond_);
        return MEDIA_ERR;
    }
    play = this;
    MediaThreadCondSignal(cond_);
    return MEDIA_OK;
}

int32_t MediaVideoPlay::PauseVideoPlay(void)
{
    MediaMutexLock(mutex_);
    if (!isEntered_ || isPlaybackStopped_ ||
        isPlaybackCompleted_ || interruptHintStop_ ||
        isPause_) {
        MEDIA_ERR_LOG("current state is invaild, can not pause play");
        MediaMutexUnLock(mutex_);
        return MEDIA_ERR;
    }
    isPause_ = true;
    if (player_ == nullptr || !isPlayed_ ) {
        MediaMutexUnLock(mutex_);
        return MEDIA_OK;
    }
    int32_t ret = player_->Pause();
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("pause failed");
        MediaMutexUnLock(mutex_);
        return MEDIA_ERR;
    }
    MediaMutexUnLock(mutex_);
    return ret;
}

int32_t MediaVideoPlay::ResumeVideoPlay(void)
{
    MediaMutexLock(mutex_);
    if (!isEntered_ || isPlaybackStopped_ ||
        isPlaybackCompleted_ || interruptHintStop_ ||
        !isPause_) {
        MEDIA_ERR_LOG("current state is invaild, can not resume play");
        MediaMutexUnLock(mutex_);
        return MEDIA_ERR;
    }
    isPause_ = false;
    if (player_ == nullptr || !isPlayed_) {
        MediaMutexUnLock(mutex_);
        return MEDIA_OK;
    }
    isResume_ = true;
    MediaThreadCondSignal(cond_);
    MediaMutexUnLock(mutex_);
    return MEDIA_OK;
}

int32_t MediaVideoPlay::GetDumpInfo(PlayerDebugInfo *playerInfo)
{
    MediaMutexLock(mutex_);
    if (player_ == nullptr) {
        MEDIA_ERR_LOG("can not get dump info!");
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

int32_t MediaVideoPlay::SeekVideoPlay(int64_t mSeconds)
{
    MediaMutexLock(mutex_);
    if (player_ == nullptr) {
        MEDIA_ERR_LOG("can not seek!");
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

int32_t MediaVideoPlay::GetCurrentPosition(int64_t *currentPosition)
{
    MediaMutexLock(mutex_);
    if (player_ == nullptr) {
        MEDIA_ERR_LOG("can not get current play position!");
        MediaMutexUnLock(mutex_);
        return MEDIA_ERR;
    }
    int32_t ret = player_->GetCurrentTime(*currentPosition);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("video player get current play position failed!");
    }
    MediaMutexUnLock(mutex_);
    return ret;
}

int32_t MediaVideoPlay::StopVideoPlay(void)
{
    MediaMutexLock(mutex_);
    if (isPlaybackStopped_ || isPlaybackCompleted_ || interruptHintStop_) {
        if (isLoop_) {
            if (!isPlaybackCompleted_) {
                MEDIA_ERR_LOG("video play already stopped");
                MediaMutexUnLock(mutex_);
                return MEDIA_OK;
            }
        } else {
            MEDIA_ERR_LOG("video play already stopped");
            MediaMutexUnLock(mutex_);
            return MEDIA_OK;
        }
    }
    isPlaybackStopped_ = true;
    MediaThreadCondSignal(cond_);
    if (isSyncExitMode_) {
        MediaMutexUnLock(mutex_);
        MediaThreadJoin(&threadHandle_);
        play = nullptr;
        return MEDIA_OK;
    }
    MediaMutexUnLock(mutex_);
    play = nullptr;
    return MEDIA_OK;
}

bool MediaVideoPlay::IsExitCompletely(void)
{
    return isExited_;
}

void MediaVideoPlay::SetVideoPlayLoop(bool isLoop)
{
    MediaMutexLock(mutex_);
    isLoop_ = isLoop;
    MediaMutexUnLock(mutex_);
}

void MediaVideoPlay::SetSyncExitMode(bool isSyncExitMode)
{
    if (!isEntered_) {
        isSyncExitMode_ = isSyncExitMode;
    }
}

bool MediaVideoPlay::IsSyncExitMode()
{
    return isSyncExitMode_;
}

void MediaVideoPlay::DestroyVideoPlaySource(void)
{
    if (isEntered_) {
        needSendBackgroundFrame_ = false;
        StopVideoPlay();
    }
}
};

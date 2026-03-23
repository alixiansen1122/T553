/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
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

#if defined(_WIN32)
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#endif
#include "player_impl.h"
#include <cinttypes>
#include <climits>
#include <string>
#ifdef SUPPORT_SYS_PRCTL
#include <sys/prctl.h>
#endif
#include "unistd.h"
#if !defined(_WIN32)
#include "securec.h"
#endif
#include "format_type.h"
#include "player_check.h"
#include "hi_liteplayer_err.h"
#include "hi_liteplayer.h"
#include "player_define.h"
#include "media_log.h"
#include "parameter_item.h"
#include "codec_interface.h"

using namespace std;
using OHOS::Media::AdapterStreamCallback;

namespace OHOS {
namespace Media {
const int32_t INVALID_MEDIA_POSITION = -1;
const int32_t DEFAULT_REWIND_TIME = 0;
const int32_t IDLE_QUEQUE_SLEEP_TIME_US = 5000;
const float MAX_MEDIA_VOLUME = 300.0f;
const int32_t POS_NOTIFY_INTERVAL_MS = 300;
const uint32_t INVALID_SESSION_ID = 0;

Player::PlayerImpl::PlayerImpl()
    : player_(nullptr), speed_(1.0), playerControlState_(PLAY_STATUS_IDLE),
      isSingleLoop_(false),
      currentPosition_(0),
      rewindPosition_(INVALID_MEDIA_POSITION),
#if defined(ENABLE_UIKIT) || defined(ENABLE_LVGL)
      surface_(nullptr),
#endif
      currentState_(PLAYER_IDLE),
      rewindMode_(PLAYER_SEEK_PREVIOUS_SYNC),
      currentRewindMode_(PLAYER_SEEK_PREVIOUS_SYNC),
      audioStreamType_(AUDIO_STREAM_MUSIC),
      lock_(nullptr),
      rewindLock_(nullptr),
      callback_(nullptr),
      inited_ (false),
      released_(false),
      isStreamSource_(false),
      bufferSource_(nullptr),
      streamCallback_(nullptr),
      extraRewind_(false),
      sessionId_(0),
      outDevice_(0)
{
    if (memset_s(&formatFileInfo_, sizeof(formatFileInfo_), 0, sizeof(FormatFileInfo)) != EOK) {
        MEDIA_ERR_LOG("memset_s failed");
    }
    formatFileInfo_.s32UsedVideoStreamIndex = -1;
    formatFileInfo_.s32UsedAudioStreamIndex = -1;
    formatFileInfo_.s64Duration = -1;
    formatFileInfo_.trackCnt = 0;
    buffer_.idx = -1;
    buffer_.flag = 0;
    buffer_.offset = 0;
    buffer_.size = 0;
    buffer_.timestamp = 0;
    if (memset_s(&mediaAttr_, sizeof(mediaAttr_), 0, sizeof(PlayerControlStreamAttr)) != EOK) {
        MEDIA_ERR_LOG("memset_s failed");
    }
    gettimeofday(&rewindStratTime_, nullptr);
    gettimeofday(&rewindEndTime_, nullptr);
    gettimeofday(&playerStopTime_, nullptr);
    gettimeofday(&playerReleaseTime_, nullptr);
}

int32_t Player::PlayerImpl::Init(void)
{
    int32_t ret;

#ifdef PLAYER_ENABLE_KPI
    ret = SetMediaLogEnabledLevel(MEDIA_LOG_ERR);
    if (ret != 0) {
        MEDIA_ERR_LOG("SetMediaLogEnabledLevel failed");
    }
#endif

    MEDIA_INFO_LOG("process in");
    if (inited_) {
        return 0;
    }

#if !defined(UNSUPPORT_AUDIO_DECODER) || !defined(UNSUPPORT_VIDEO_DECODER)
    ret = CodecInit();
    if (ret != 0) {
        return ret;
    }
#endif

    if (memset_s(&buffer_, sizeof(QueBuffer), 0, sizeof(QueBuffer)) != EOK) {
        return -1;
    }
    buffer_.idx = -1;

    lock_ = MediaMutexCreate(nullptr);
    if (lock_ == nullptr) {
        MEDIA_ERR_LOG("create mutex failed");
        return -1;
    }
    rewindLock_ = MediaMutexCreate(nullptr);
    if (rewindLock_ == nullptr) {
        MEDIA_ERR_LOG("create mutex failed");
        MediaMutexDestroy(&lock_);
        return -1;
    }

    inited_ = true;
    MEDIA_INFO_LOG("process out");
    return 0;
}

int32_t Player::PlayerImpl::DeInit(void)
{
    if (!inited_) {
        return 0;
    }
    if (!released_) {
        Release();
    }
    MediaMutexDestroy(&lock_);
    MediaMutexDestroy(&rewindLock_);
    inited_ = false;
    return 0;
}

Player::PlayerImpl::~PlayerImpl()
{
    DeInit();
    player_ = nullptr;
}

int32_t Player::PlayerImpl::SetSource(const Source &source)
{
    MEDIA_INFO_LOG("process in, source_type:%d", source.GetSourceType());
    MediaMutexLock(lock_);
    CHECK_TRUE_CONDITION_RETURN_WITH_UNLOCK(released_, -1, "have released or not create", lock_);
    if (currentState_ != PLAYER_IDLE) {
        MEDIA_ERR_LOG("current state is:%d, not support SetSource\n", currentState_);
        MediaMutexUnLock(lock_);
        return -1;
    }
    GetPlayer();
    CHK_NULL_RETURN_WITH_UNLOCK(player_, lock_, "player_ is nullptr");
    int32_t ret = -1;
    SourceType sType = source.GetSourceType();
    if (sType == SourceType::SOURCE_TYPE_URI) {
        ret = SetUriSource(source);
    } else if (sType == SourceType::SOURCE_TYPE_FD) {
        ret = SetFdSource(source);
    } else if (sType == SourceType::SOURCE_TYPE_STREAM) {
        ret = SetStreamSource(source);
    } else {
        MEDIA_ERR_LOG("SetSource failed, source type is %d", static_cast<int32_t>(sType));
    }
    if (ret != 0) {
        ResetInner();
    }
    MediaMutexUnLock(lock_);
    return ret;
}

static void ShowFileInfo(const FormatFileInfo *fileInfo)
{
    for (int i = 0; i < HI_DEMUXER_RESOLUTION_CNT; i++) {
        const StreamResolution *resolution = &fileInfo->stSteamResolution[i];
        if (resolution->u32Width == 0 || resolution->u32Height == 0) {
            break;
        }
        MEDIA_INFO_LOG("video[%d],w=%u,h=%u,stream index=%d ", i, resolution->u32Width,
            resolution->u32Height, resolution->s32VideoStreamIndex);
    }
    MEDIA_INFO_LOG("audio duration=%lld,channel count=%u,sample rate=%u",
        fileInfo->s64Duration, fileInfo->u32AudioChannelCnt, fileInfo->u32SampleRate);

    MEDIA_INFO_LOG("used audio stream index=%d, used video stream index=%d",
        fileInfo->s32UsedAudioStreamIndex, fileInfo->s32UsedVideoStreamIndex);
}

void Player::PlayerImpl::UpdateState(PlayerImpl *curPlayer, PlayerStatus state)
{
    if (curPlayer == nullptr) {
        return;
    }
    curPlayer->playerControlState_ = state;
    MEDIA_INFO_LOG("@@player UpdateState, state:%d", state);
}

void Player::PlayerImpl::PlayerControlDefaultEventCb(PlayerImpl *curPlayer, PlayerControlEvent enEvent, const void *pData)
{
    if (pData == nullptr || curPlayer == nullptr) {
        return;
    }
    PlayerControlEvent subEventData = *static_cast<const PlayerControlEvent *>(pData);
    int32_t eventType = enEvent;
    if (eventType == PLAYERCONTROL_FIRST_AUDIO_FRAME ||
        eventType == PLAYERCONTROL_FIRST_VIDEO_FRAME) {
        eventType = PlayerCallback::PlayerInfoType::PLAYER_INFO_PLAYING;
    } else if (eventType == PLAYERCONTROL_EVENT_PAUSED) {
        eventType = PlayerCallback::PlayerInfoType::PLAYER_INFO_PAUSED;

    }
    curPlayer->OnInfoPlayCallBack(eventType, subEventData);
}

void Player::PlayerImpl::PlayerControlEventCb(void *pPlayer, PlayerControlEvent enEvent, const void *pData)
{
    PlayerControlError subErr = PLAYERCONTROL_ERROR_BUTT;
    PlayerImpl *curPlayer = (PlayerImpl *)pPlayer;
    CHK_NULL_RETURN_NONE(curPlayer, "the handle is error");
    switch (enEvent) {
        case PLAYERCONTROL_EVENT_STATE_CHANGED:
            if (pData == nullptr) {
                return;
            }
            curPlayer->UpdateState(curPlayer, *static_cast<const PlayerStatus *>(pData));
            break;
        case PLAYERCONTROL_EVENT_EOF:
            MEDIA_INFO_LOG("end of file");
            curPlayer->NotifyPlaybackComplete(curPlayer);
            break;
        case PLAYERCONTROL_BUFFER_EMPTY:
            curPlayer->PlayerThreadCondSignal();
            break;
        case PLAYERCONTROL_EVENT_SOF:
            MEDIA_INFO_LOG("start of file");
            break;
        case PLAYERCONTROL_EVENT_ERROR:
            if (pData == nullptr) {
                return;
            }
            subErr = *static_cast<const PlayerControlError *>(pData);
            MEDIA_ERR_LOG("error: %d", subErr);
            if (curPlayer->callback_ != nullptr) {
                curPlayer->callback_->OnError(subErr, subErr);
            }
            break;
        case PLAYERCONTROL_EVENT_PROGRESS:
            if (pData == nullptr) {
                return;
            }
            curPlayer->currentPosition_ = *static_cast<const int64_t *>(pData);
            break;
        case PLAYERCONTROL_EVENT_SEEK_END:
            if (pData == nullptr) {
                return;
            }
            MEDIA_INFO_LOG("seek action end, time is %lld",  *static_cast<const int64_t *>(pData));
            curPlayer->NotifySeekComplete(curPlayer, *static_cast<const int64_t *>(pData));
            break;
        default:
            PlayerControlDefaultEventCb(curPlayer, enEvent, pData);
            break;
    }
}

int32_t Player::PlayerImpl::Prepare()
{
    int ret;
    MEDIA_INFO_LOG("process in");
    MediaMutexLock(lock_);
    CHECK_TRUE_CONDITION_RETURN_WITH_UNLOCK(released_, -1, "have released or not create", lock_);
    CHK_NULL_RETURN_WITH_UNLOCK(player_, lock_, "player_ is nullptr");
    if (currentState_ == PLAYER_PREPARED) {
        MEDIA_ERR_LOG("have operate prepare before");
        MediaMutexUnLock(lock_);
        return 0;
    }
    if (currentState_ != PLAYER_INITIALIZED) {
        MEDIA_ERR_LOG("Can not Prepare, currentState_ is %u", currentState_);
        MediaMutexUnLock(lock_);
        return -1;
    }
    PlayerCtrlCallbackParam param;
    param.player = this;
    param.callbackFun = PlayerControlEventCb;
    ret = player_->RegCallback(param);
    if (ret != 0) {
        MEDIA_ERR_LOG("RegCallback exec failed ");
        MediaMutexUnLock(lock_);
        return -1;
    }

    currentState_ = PLAYER_PREPARING;
    ret = player_->Prepare();
    if (ret != 0) {
        MEDIA_ERR_LOG("Prepare exec failed ");
        currentState_ = PLAYER_INITIALIZED;
        MediaMutexUnLock(lock_);
        return -1;
    }
    currentState_ = PLAYER_PREPARED;

    ret = player_->GetFileInfo(formatFileInfo_);
    if (ret != 0) {
        MEDIA_ERR_LOG("GetFileInfo failed");
        MediaMutexUnLock(lock_);
        return ret;
    }
    ShowFileInfo(&formatFileInfo_);
    MediaMutexUnLock(lock_);
    MEDIA_INFO_LOG("process out");
    return 0;
}

int32_t Player::PlayerImpl::SetMediaStream(void)
{
    int32_t ret;

    mediaAttr_.s32VidStreamId = formatFileInfo_.s32UsedVideoStreamIndex;
    mediaAttr_.s32AudStreamId = formatFileInfo_.s32UsedAudioStreamIndex;
    ret = player_->SetMedia(mediaAttr_);
    if (ret != 0) {
        MEDIA_ERR_LOG("SetMedia  exec failed");
        return  ret;
    }

    for (int i = 0; i < HI_DEMUXER_RESOLUTION_CNT; i++) {
        StreamResolution *resolution = &formatFileInfo_.stSteamResolution[i];
        if (resolution->s32VideoStreamIndex == mediaAttr_.s32VidStreamId) {
            MEDIA_INFO_LOG("used video w=%u,h=%u,index=%d",
                resolution->u32Width, resolution->u32Height, mediaAttr_.s32VidStreamId);
            break;
        }
    }
    return 0;
}

int32_t Player::PlayerImpl::Play()
{
    int ret;
    MEDIA_INFO_LOG("process in");
    MediaMutexLock(lock_);
    CHECK_TRUE_CONDITION_RETURN_WITH_UNLOCK(released_, -1, "have released or not create", lock_);
    CHK_NULL_RETURN_WITH_UNLOCK(player_, lock_, "player_ is nullptr");
    if (currentState_ != PLAYER_PREPARED && currentState_ != PLAYER_PAUSED &&
        currentState_ != PLAYER_STARTED) {
        MEDIA_ERR_LOG("Can not Play, currentState is %d", currentState_);
        MediaMutexUnLock(lock_);
        return -1;
    }
    if (currentState_ == PLAYER_PREPARED) {
        ret = SetMediaStream();
        CHECK_FAILED_RETURN_WITH_UNLOCK(ret, 0, ret, "SetMeidaStream failed", lock_);
    }

    ret = player_->Play();
    if (ret != 0) {
        MEDIA_ERR_LOG("Play exec failed %x", ret);
        MediaMutexUnLock(lock_);
        return -1;
    }
    OnInfoPlayCallBack(PlayerCallback::PlayerInfoType::PLAYER_INFO_PLAY_START, 0);
    currentState_ = PLAYER_STARTED;
    MediaMutexUnLock(lock_);
    MEDIA_INFO_LOG("process out");
    return 0;
}

bool Player::PlayerImpl::IsPlaying()
{
    MEDIA_INFO_LOG("process in");
    MediaMutexLock(lock_);
    CHECK_TRUE_CONDITION_RETURN_WITH_UNLOCK(released_, 0, "have released or not create", lock_);
    bool isPlaying = false;
    if (player_ != nullptr) {
        isPlaying = (currentState_ != PLAYER_STARTED) ? false : true;
    }
    MediaMutexUnLock(lock_);
    return isPlaying;
}

int32_t Player::PlayerImpl::Pause()
{
    MEDIA_INFO_LOG("process in");
    MediaMutexLock(lock_);
    CHECK_TRUE_CONDITION_RETURN_WITH_UNLOCK(released_, -1, "have released or not create", lock_);
    CHK_NULL_RETURN_WITH_UNLOCK(player_, lock_, "player_ is nullptr");
    if (currentState_ == PLAYER_PAUSED) {
        MEDIA_ERR_LOG("currentState_ is %d", currentState_);
        MediaMutexUnLock(lock_);
        return 0;
    }
    if (currentState_ != PLAYER_STARTED) {
        MEDIA_ERR_LOG("Can not Pause, currentState_ is %d", currentState_);
        MediaMutexUnLock(lock_);
        return -1;
    }

    player_->Pause();
    currentState_ = PLAYER_PAUSED;
    MediaMutexUnLock(lock_);
    return 0;
}

int32_t Player::PlayerImpl::Stop()
{
    MEDIA_INFO_LOG("process in");
#ifdef PLAYER_KPI_ON
    player_->RecordTime(playerStopTime_);
#endif
    MediaMutexLock(lock_);
    CHECK_TRUE_CONDITION_RETURN_WITH_UNLOCK(released_, -1, "have released", lock_);
    if (currentState_ == PLAYER_STOPPED) {
        MediaMutexUnLock(lock_);
        return 0;
    }
    if ((currentState_ != PLAYER_STARTED) && (currentState_ != PLAYER_PAUSED) &&
        (currentState_ != PLAYER_PLAYBACK_COMPLETE) && (currentState_ != PLAYER_STATE_ERROR)) {
        MEDIA_INFO_LOG("current state: %d, no need to do stop", currentState_);
        MediaMutexUnLock(lock_);
        return -1;
    }

    if (player_ != nullptr) {
        int32_t ret = player_->Stop();
        if (ret != 0) {
            MEDIA_ERR_LOG("Stop failed, ret is %d", ret);
        }
    }
    currentState_ = PLAYER_STOPPED;
    OnInfoPlayCallBack(PlayerCallback::PlayerInfoType::PLAYER_INFO_STOPPED, 0);
    MediaMutexUnLock(lock_);
    MEDIA_INFO_LOG("process out");
    return 0;
}

int32_t Player::PlayerImpl::RewindInner(int64_t mSeconds, PlayerSeekMode mode)
{
    CHK_NULL_RETURN(player_, "player_ is nullptr");
    if (mSeconds < DEFAULT_REWIND_TIME) {
        MEDIA_WARNING_LOG("Attempt to rewind to invalid position %lld", mSeconds);
        mSeconds = DEFAULT_REWIND_TIME;
    }
    int32_t ret;
    int64_t durationMs = -1;
    GetDurationInner(durationMs);
    if ((durationMs > DEFAULT_REWIND_TIME) && (mSeconds > durationMs)) {
        MEDIA_WARNING_LOG("failed, request is %lld, durationMs is %lld", mSeconds, durationMs);
        return -1;
    }
    currentRewindMode_ = mode;
    if (rewindPosition_ >= DEFAULT_REWIND_TIME) {
        rewindPosition_ = mSeconds;
        rewindMode_ = mode;
        MEDIA_ERR_LOG("is deal last rewind time:%lld", mSeconds);
        return 0;
    }

    rewindPosition_ = mSeconds;
    rewindMode_ = mode;
    OnInfoPlayCallBack(PlayerCallback::PlayerInfoType::PLAYER_INFO_SEEK_START, 0);
    ret = player_->Seek(mSeconds);
    if (ret != 0) {
        MEDIA_ERR_LOG("do seek failed, ret is %d", ret);
    }
    return ret;
}

bool Player::PlayerImpl::IsValidRewindMode(PlayerSeekMode mode)
{
    switch (mode) {
        case PLAYER_SEEK_PREVIOUS_SYNC:
        case PLAYER_SEEK_NEXT_SYNC:
        case PLAYER_SEEK_CLOSEST_SYNC:
        case PLAYER_SEEK_CLOSEST:
            break;
        default:
            MEDIA_ERR_LOG("Unknown rewind mode %d", mode);
            return false;
    }
    return true;
}

int32_t Player::PlayerImpl::Rewind(int64_t mSeconds, int32_t mode)
{
    MEDIA_INFO_LOG("process in");
#ifdef PLAYER_KPI_ON
    player_->RecordTime(rewindStratTime_);
#endif
    MediaMutexLock(lock_);
    CHECK_TRUE_CONDITION_RETURN_WITH_UNLOCK(released_, -1, "have released or not create", lock_);
    if (currentState_ != PLAYER_STARTED && currentState_ != PLAYER_PAUSED) {
        MEDIA_ERR_LOG("Can not Rewind, currentState_ is %d", currentState_);
        MediaMutexUnLock(lock_);
        return -1;
    }

    if (!IsValidRewindMode(static_cast<PlayerSeekMode>(mode))) {
        MEDIA_ERR_LOG("Rewind failed, msec is %lld, mode is %d", mSeconds, mode);
        MediaMutexUnLock(lock_);
        return -1;
    }

    if (isStreamSource_) {
        MEDIA_ERR_LOG("Failed, streamsource not support Rewind");
        MediaMutexUnLock(lock_);
        return -1;
    }

    MediaMutexLock(rewindLock_);
    int32_t ret = RewindInner(mSeconds, static_cast<PlayerSeekMode>(mode));
    if (ret != 0) {
        MEDIA_ERR_LOG("ReWind failed, ret is %d", ret);
    } else {
        currentPosition_ = mSeconds;
        extraRewind_ = true;
    }
    MediaMutexUnLock(rewindLock_);
    MediaMutexUnLock(lock_);
    MEDIA_INFO_LOG("process out");
    return ret;
}

int32_t Player::PlayerImpl::SetVolume(float leftVolume, float rightVolume)
{
    MEDIA_INFO_LOG("process in");
    MediaMutexLock(lock_);
    VolumeAttr attr;
    CHECK_TRUE_CONDITION_RETURN_WITH_UNLOCK(released_, -1, "have released or not create", lock_);
    CHK_NULL_RETURN_WITH_UNLOCK(player_, lock_, "player_ is nullptr");
    if ((currentState_ != PLAYER_STARTED) && (currentState_ != PLAYER_PAUSED) &&
        (currentState_ != PLAYER_PREPARED) && (currentState_ != PLAYER_INITIALIZED)) {
        MEDIA_ERR_LOG("SetVolume failed, currentState_ is %d", currentState_);
        MediaMutexUnLock(lock_);
        return -1;
    }
    if (leftVolume < 0 || leftVolume > MAX_MEDIA_VOLUME || rightVolume < 0 || rightVolume > MAX_MEDIA_VOLUME) {
        MEDIA_ERR_LOG("SetVolume failed, the volume should be set to a value ranging from 0 to 300");
        MediaMutexUnLock(lock_);
        return -1;
    }
    attr.leftVolume = leftVolume;
    attr.rightVolume = rightVolume;
    int ret = player_->SetVolume(attr);
    if (ret != 0) {
        MEDIA_ERR_LOG("SetVolume failed %x", ret);
    }
    MediaMutexUnLock(lock_);
    return ret;
}

#if defined(ENABLE_UIKIT) || defined(ENABLE_LVGL)
#if defined(ENABLE_UIKIT)
int32_t Player::PlayerImpl::SetSurface(Surface *surface)
#elif defined(ENABLE_LVGL)
int32_t Player::PlayerImpl::SetSurface(LvSurfaceView *surface)
#endif
{
    MEDIA_INFO_LOG("process in");
    MediaMutexLock(lock_);
    CHECK_TRUE_CONDITION_RETURN_WITH_UNLOCK(released_, false, "have released or not create", lock_);
    if ((currentState_ != PLAYER_PREPARED) && (currentState_ != PLAYER_INITIALIZED)) {
        MEDIA_ERR_LOG("SetSurface failed, currentState_ is %d", currentState_);
        MediaMutexUnLock(lock_);
        return -1;
    }
    surface_ = surface;
    player_->SetSurface(surface);
    MediaMutexUnLock(lock_);
    return 0;
}
#endif

bool Player::PlayerImpl::IsSingleLooping()
{
    MEDIA_INFO_LOG("process in");
    MediaMutexLock(lock_);
    CHECK_TRUE_CONDITION_RETURN_WITH_UNLOCK(released_, false, "have released or not create", lock_);
    bool isLoop = (player_ == nullptr) ? false : isSingleLoop_;
    MediaMutexUnLock(lock_);
    return isLoop;
}

int32_t Player::PlayerImpl::GetPlayerState(int32_t &state)
{
    MEDIA_INFO_LOG("process in");
    MediaMutexLock(lock_);
    CHECK_TRUE_CONDITION_RETURN_WITH_UNLOCK(released_, -1, "have released or not create", lock_);
    state = currentState_;
    MediaMutexUnLock(lock_);
    return 0;
}

int32_t Player::PlayerImpl::GetCurrentTime(int64_t &currentPosition)
{
    MEDIA_DEBUG_LOG("process in");
    MediaMutexLock(lock_);
    CHECK_TRUE_CONDITION_RETURN_WITH_UNLOCK(released_, -1, "have released or not create", lock_);
    currentPosition = (currentPosition_ >= 0) ? currentPosition_ : 0;
    MediaMutexUnLock(lock_);
    return 0;
}

void Player::PlayerImpl::GetDurationInner(int64_t &durationMs)
{
    durationMs = formatFileInfo_.s64Duration;
}

int32_t Player::PlayerImpl::GetDuration(int64_t &durationMs)
{
    MEDIA_INFO_LOG("process in");
    MediaMutexLock(lock_);
    CHECK_TRUE_CONDITION_RETURN_WITH_UNLOCK(released_, -1, "have released or not create", lock_);
    if (currentState_ == PLAYER_IDLE || currentState_ == PLAYER_INITIALIZED) {
        durationMs = 0;
        MediaMutexUnLock(lock_);
        return 0;
    }
    GetDurationInner(durationMs);
    MediaMutexUnLock(lock_);
    return 0;
}

bool Player::PlayerImpl::IsSpeedValid(float speed)
{
    if (speed > 0 && speed <= TPLAY_SPEED_2X_FAST) {
        return true;
    }
    bool isValid = false;
    switch ((int)speed) {
        case TPLAY_SPEED_64X_REWIND:
        case TPLAY_SPEED_32X_REWIND:
        case TPLAY_SPEED_16X_REWIND:
        case TPLAY_SPEED_8X_REWIND:
        case TPLAY_SPEED_4X_REWIND:
        case TPLAY_SPEED_2X_REWIND:
        case TPLAY_SPEED_1X_REWIND:
        case TPLAY_SPEED_BASE:
        case TPLAY_SPEED_2X_FAST:
        case TPLAY_SPEED_4X_FAST:
        case TPLAY_SPEED_8X_FAST:
        case TPLAY_SPEED_16X_FAST:
        case TPLAY_SPEED_32X_FAST:
        case TPLAY_SPEED_64X_FAST:
            isValid = true;
            break;
        default:
            break;
    }
    return isValid;
}

int32_t Player::PlayerImpl::SetPlaybackSpeed(float speed)
{
    MEDIA_INFO_LOG("process in");
    MediaMutexLock(lock_);
    CHECK_TRUE_CONDITION_RETURN_WITH_UNLOCK(released_, -1, "have released or not create", lock_);
    if (!IsSpeedValid(speed)) {
        MEDIA_ERR_LOG(" speed:%f is invalid", speed);
        MediaMutexUnLock(lock_);
        return -1;
    }
    CHK_NULL_RETURN_WITH_UNLOCK(player_, lock_, "player_ is nullptr");
    if (currentState_ != PLAYER_STARTED) {
        MEDIA_ERR_LOG(" currentState_ is %d", currentState_);
        MediaMutexUnLock(lock_);
        return -1;
    }
    if (speed == speed_) {
        MediaMutexUnLock(lock_);
        return 0;
    }
    if ((isStreamSource_) && (speed != 1.0f)) {
        MEDIA_ERR_LOG("Failed, streamsource not support abnormal speed");
        MediaMutexUnLock(lock_);
        return -1;
    }

    if (formatFileInfo_.s32UsedVideoStreamIndex == -1 || formatFileInfo_.s32UsedAudioStreamIndex != -1) {
        MEDIA_ERR_LOG("audio movie not support abnormal speed");
        MediaMutexUnLock(lock_);
        return -1;
    }

    CHECK_FAILED_PRINT(player_->SetPlaybackSpeed(speed), 0, "tplayer failed");
    speed_ = speed;
    MediaMutexUnLock(lock_);
    return 0;
}

int32_t Player::PlayerImpl::GetPlaybackSpeed(float &speed)
{
    MEDIA_INFO_LOG("process in");
    MediaMutexLock(lock_);
    CHECK_TRUE_CONDITION_RETURN_WITH_UNLOCK(released_, -1, "have released or not create", lock_);
    speed = (currentState_ != PLAYER_PAUSED) ? speed_ : 0;
    MediaMutexUnLock(lock_);
    return 0;
}

bool Player::PlayerImpl::IsValidStreamType(int32_t type)
{
    return type == AUDIO_STREAM_ALARM_SYSTEM ||
           type == AUDIO_STREAM_ALARM_CLOCK || type == AUDIO_STREAM_RING ||
           type == AUDIO_STREAM_NOTIFICATION_PROMPT || type == AUDIO_STREAM_MUSIC ||
           type == AUDIO_STREAM_FITNESS_VIDEO || type == AUDIO_STREAM_NOTIFICATION_SYSTEM;
}

int32_t Player::PlayerImpl::SetAudioStreamType(int32_t type)
{
    MEDIA_INFO_LOG("process in");
    if (!IsValidStreamType(type)) {
        MEDIA_ERR_LOG("stream type 0x%x is invalid", type);
        return -1;
    }
    MediaMutexLock(lock_);
    CHECK_TRUE_CONDITION_RETURN_WITH_UNLOCK(released_, -1, "have released or not create", lock_);
    CHK_NULL_RETURN_WITH_UNLOCK(player_, lock_, "player_ is nullptr");
    int32_t ret = player_->Invoke(INVOKE_SET_AUDIOSTREAM_TYPE, &type);
    if (ret != 0) {
        MediaMutexUnLock(lock_);
        return -1;
    }
    audioStreamType_ = type;
    MediaMutexUnLock(lock_);
    return 0;
}

int32_t Player::PlayerImpl::GetAudioStreamType(int32_t &type)
{
    MEDIA_INFO_LOG("process in");
    MediaMutexLock(lock_);
    if (released_) {
        MEDIA_ERR_LOG("have released or not create");
        MediaMutexUnLock(lock_);
        return -1;
    }
    type = static_cast<int32_t>(audioStreamType_);
    MediaMutexUnLock(lock_);
    return 0;
}

void Player::PlayerImpl::ResetInner(void)
{
    isSingleLoop_ = false;
    if (player_ != nullptr) {
        if (currentState_ != PLAYER_IDLE && currentState_ != PLAYER_STOPPED) {
            CHECK_FAILED_PRINT(player_->Stop(), HI_SUCCESS, "stop failed");
        }
        (void)player_->Deinit();
        player_.reset();
        player_ = nullptr;
    }
    if (bufferSource_ != nullptr) {
        bufferSource_.reset();
        bufferSource_ = nullptr;
    }
    if (streamCallback_ != nullptr) {
        streamCallback_.reset();
        streamCallback_ = nullptr;
    }
    if (callback_ != nullptr) {
        callback_.reset();
        callback_ = nullptr;
    }
    currentState_ = PLAYER_IDLE;
    currentRewindMode_ = PLAYER_SEEK_PREVIOUS_SYNC;
    rewindPosition_ = INVALID_MEDIA_POSITION;
    rewindMode_ = PLAYER_SEEK_PREVIOUS_SYNC;
    isSingleLoop_ = false;
    speed_ = 1.0;
    currentPosition_ = 0;
    extraRewind_ = false;
    playerControlState_ = PLAY_STATUS_IDLE;
    isStreamSource_ = false;
    if (memset_s(&formatFileInfo_, sizeof(formatFileInfo_), 0, sizeof(FormatFileInfo)) != EOK) {
        MEDIA_ERR_LOG("memset_s failed");
    }
    formatFileInfo_.s32UsedVideoStreamIndex = -1;
    formatFileInfo_.s32UsedAudioStreamIndex = -1;
    formatFileInfo_.s64Duration = -1;
    if (memset_s(&mediaAttr_, sizeof(mediaAttr_), 0, sizeof(PlayerControlStreamAttr)) != EOK) {
        MEDIA_ERR_LOG("memset_s failed");
    }
    if (memset_s(&buffer_, sizeof(QueBuffer), 0, sizeof(QueBuffer)) != EOK) {
        MEDIA_ERR_LOG("memset_s failed");
    }
    buffer_.idx = -1;
}

int32_t Player::PlayerImpl::Reset(void)
{
    MEDIA_INFO_LOG("process in");
    MediaMutexLock(lock_);
    CHECK_TRUE_CONDITION_RETURN_WITH_UNLOCK(released_, -1, "have released or not create", lock_);
    if (currentState_ == PLAYER_IDLE) {
        MediaMutexUnLock(lock_);
        return 0;
    }
    ResetInner();
    MediaMutexUnLock(lock_);
    return 0;
}

int32_t Player::PlayerImpl::Release()
{
    MEDIA_INFO_LOG("process in");
    MediaMutexLock(lock_);
    CHECK_TRUE_CONDITION_RETURN_WITH_UNLOCK(released_, 0, "have released or not create", lock_);
    ResetInner();
    currentState_ = PLAYER_STATE_ERROR;
    released_ = true;
    MediaMutexUnLock(lock_);
#ifdef PLAYER_KPI_ON
    string playerOut("KPI_PlayerStopTime");
    player_->CalcTimeDiffUsPrint(playerStopTime_, playerReleaseTime_, playerOut, KPI_PRINT);
#endif
    return 0;
}

int Player::PlayerImpl::CreatePlayerParamCheck(PlayerControlParam &createParam)
{
    if (createParam.u32PlayPosNotifyIntervalMs < MIN_NOTIFY_INTERVAL_MS &&
        createParam.u32PlayPosNotifyIntervalMs > 0) {
        MEDIA_ERR_LOG("notify interval small than min value %d",
            MIN_NOTIFY_INTERVAL_MS);
        return HI_ERR_PLAYERCONTROL_ILLEGAL_PARAM;
    }
    if ((createParam.u32AudioEsBufSize < AV_ESBUF_SIZE_MIN && createParam.u32AudioEsBufSize > 0) ||
        createParam.u32AudioEsBufSize > AUDIO_ESBUF_SIZE_LIMIT) {
        MEDIA_ERR_LOG("audio esbuffer illegal %u",
            createParam.u32AudioEsBufSize);
        return HI_ERR_PLAYERCONTROL_ILLEGAL_PARAM;
    }
    return 0;
}

int Player::PlayerImpl::GetPlayer()
{
    PlayerControlParam playerParam;
    if (player_ != nullptr) {
        return 0;
    }
    if (memset_s(&playerParam, sizeof(PlayerControlParam), 0x00, sizeof(playerParam)) != EOK) {
        MEDIA_INFO_LOG("memset_s playerParam failed");
        return -1;
    }

    playerParam.u32PlayPosNotifyIntervalMs = POS_NOTIFY_INTERVAL_MS;
    if (CreatePlayerParamCheck(playerParam) != 0) {
        MEDIA_ERR_LOG("CreatePlayerParamCheck failed");
        return -1;
    }
    player_ = std::make_shared<PlayerControl>();
    if (player_ == nullptr || player_.get() == nullptr) {
        MEDIA_ERR_LOG("playerControl new failed");
        return HI_ERR_PLAYERCONTROL_MEM_MALLOC;
    }
    if (player_->Init(playerParam) != HI_SUCCESS) {
        MEDIA_ERR_LOG("playerControl init failed");
        return HI_ERR_PLAYERCONTROL_MEM_MALLOC;
    }
    currentState_ = PLAYER_INITIALIZED;
    return 0;
}

void Player::PlayerImpl::SetPlayerCallback(const std::shared_ptr<PlayerCallback> &cb)
{
    MEDIA_INFO_LOG("process in");
    MediaMutexLock(lock_);
    if (released_) {
        MEDIA_ERR_LOG("have released or not create");
        MediaMutexUnLock(lock_);
        return;
    }
    callback_ = cb;
    MediaMutexUnLock(lock_);
}

void Player::PlayerImpl::NotifyPlaybackComplete(PlayerImpl *curPlayer)
{
    if (!isSingleLoop_ || speed_ != 1.0f) {
        if (curPlayer->formatFileInfo_.s64Duration == -1) {
            curPlayer->formatFileInfo_.s64Duration = curPlayer->currentPosition_;
        }
        curPlayer->currentState_ = PLAYER_PLAYBACK_COMPLETE;
        MEDIA_INFO_LOG("OnPlayBackComplete, iscallbackNull:%d", (curPlayer->callback_ == nullptr));
        if (curPlayer != nullptr && curPlayer->callback_ != nullptr) {
            curPlayer->OnInfoPlayCallBack(PlayerCallback::PlayerInfoType::PLAYER_INFO_PLAY_END, 0);
            curPlayer->callback_->OnPlaybackComplete();
        }
        return;
    }
    MediaMutexLock(rewindLock_);
    (void)curPlayer->RewindInner(0, PLAYER_SEEK_PREVIOUS_SYNC);
    curPlayer->currentPosition_ = 0;
    curPlayer->extraRewind_ = false;
    MediaMutexUnLock(rewindLock_);
}

void Player::PlayerImpl::NotifySeekComplete(PlayerImpl *curPlayer, int64_t seekToMs)
{
#ifdef PLAYER_KPI_ON
    string playerSeekEnd("KPI_PlayerSeekTime");
    player_->CalcTimeDiffUsPrint(rewindStratTime_, rewindEndTime_, playerSeekEnd, KPI_PRINT);
#endif
    MediaMutexLock(curPlayer->rewindLock_);
    if (curPlayer->rewindPosition_ != -1 && curPlayer->rewindPosition_ != seekToMs) {
        curPlayer->rewindMode_ = PLAYER_SEEK_PREVIOUS_SYNC;
        curPlayer->RewindInner(curPlayer->rewindPosition_, curPlayer->currentRewindMode_);
        curPlayer->rewindPosition_ = -1;
        MediaMutexUnLock(curPlayer->rewindLock_);
        return;
    }

    curPlayer->currentRewindMode_ = curPlayer->rewindMode_ = PLAYER_SEEK_PREVIOUS_SYNC;
    curPlayer->rewindPosition_ = -1;
    if (curPlayer->callback_ != nullptr && extraRewind_) {
        extraRewind_ = false;
        curPlayer->OnInfoPlayCallBack(PlayerCallback::PlayerInfoType::PLAYER_INFO_SEEK_END, 0);
        curPlayer->callback_->OnRewindToComplete();
    }
    MediaMutexUnLock(curPlayer->rewindLock_);
}

int32_t Player::PlayerImpl::EnableSingleLooping(bool loop)
{
    MEDIA_INFO_LOG("process in");
    MediaMutexLock(lock_);
    CHECK_TRUE_CONDITION_RETURN_WITH_UNLOCK(released_, -1, "have released or not create", lock_);
    CHECK_TRUE_CONDITION_RETURN_WITH_UNLOCK(isStreamSource_, -1, "stream source not support loop player", lock_);
    if (currentState_ == PLAYER_STOPPED || currentState_ == PLAYER_PLAYBACK_COMPLETE || currentState_ == PLAYER_IDLE) {
        MEDIA_ERR_LOG(" currentState_ is %d", currentState_);
        MediaMutexUnLock(lock_);
        return -1;
    }
    isSingleLoop_ = loop;
    if (player_ != nullptr) {
        player_->EnableSingleLooping(loop);
    }
    MediaMutexUnLock(lock_);
    return 0;
}

int32_t Player::PlayerImpl::SetUriSource(const Source &source)
{
    const std::string uri = source.GetSourceUri();
    if (uri.empty()) {
        MEDIA_ERR_LOG("SetUriSource failed, uri source do not set uri parameter");
        return -1;
    }
    if (uri.find("http") != 0) {
        char filePath[PATH_MAX];
#if !defined(_WIN32)
    if (realpath(uri.c_str(), filePath) == nullptr) {
#else
    if (_fullpath(filePath, uri.c_str(), PATH_MAX) == nullptr) {
#endif
        MEDIA_ERR_LOG("Realpath input file failed");
        return -1;
    }
    if (access(filePath, R_OK) == -1) {
        MEDIA_ERR_LOG("No permission to read the file");
        return -1;
        }
    }
    int32_t ret = player_->SetDataSource(uri.c_str());
    if (ret != 0) {
        MEDIA_ERR_LOG("SetSource failed, ret is %d, uri is %s", ret, uri.c_str());
        return ret;
    }
    return 0;
}

int32_t Player::PlayerImpl::SetFdSource(const Source &source)
{
    int32_t fd = source.GetSourceFd();
    if (fd == -1) {
        MEDIA_ERR_LOG("SetfdSource failed, fd source do not set fd parameter");
        return -1;
    }
    SourceFdParam  fdSrc = {};
    fdSrc.fd = fd;
    fdSrc.offset = source.GetSourceFdOffset();
    int32_t ret = player_->SetDataSource(fdSrc);
    if (ret != 0) {
        MEDIA_ERR_LOG("SetSource failed, ret is %d, fd is %d, offset is %d",
            ret, fdSrc.fd, fdSrc.offset);
        return ret;
    }
    return 0;
}

AdapterStreamCallback::AdapterStreamCallback(const std::shared_ptr<StreamSource> &stream,
    const std::shared_ptr<BufferSource> &buffer)
    : streamProcess_(nullptr),
      mutex_(MediaMutexCreate(nullptr)),
      isRunning_(false)
{
    streamSource_ = stream;
    bufferSource_ = buffer;
}

AdapterStreamCallback::~AdapterStreamCallback(void)
{
    DeInit();
}

void *AdapterStreamCallback::IdleBufferProcess(void *arg)
{
    int ret;
    QueBuffer buffer;
    BufferInfo info;
    if (memset_s(&info, sizeof(info), 0x00, sizeof(info)) != EOK) {
        return nullptr;
    }
    AdapterStreamCallback *process = (AdapterStreamCallback*)arg;
    if (process == nullptr) {
        return nullptr;
    }

#ifdef SUPPORT_SYS_PRCTL
    prctl(PR_SET_NAME, "IdlbufProc", 0, 0, 0);
#endif
    MEDIA_INFO_LOG("process start");
    while (true) {
        MediaMutexLock(process->mutex_);
        if (!process->isRunning_) {
            MediaMutexUnLock(process->mutex_);
            break;
        }
        MediaMutexUnLock(process->mutex_);
        if (process->bufferSource_ == nullptr) {
            MEDIA_ERR_LOG("bufferSource_ null break");
            break;
        }
        if (process->bufferSource_->GetIdleQueSize() == 0) {
            usleep(IDLE_QUEQUE_SLEEP_TIME_US);
            continue;
        }
        ret = process->bufferSource_->DequeIdleBuffer(&buffer, 0);
        if (ret == 0) {
            process->bufferSource_->GetBufferInfo(buffer.idx, &info);
            std::shared_ptr<StreamSource> stream = process->streamSource_.lock();
            if (stream == nullptr) {
                MEDIA_ERR_LOG("stream not exist break");
                break;
            }
            stream->OnBufferAvailable(buffer.idx, 0, info.bufLen);
        }
    };
    MediaMutexLock(process->mutex_);
    process->isRunning_ = false;
    MediaMutexUnLock(process->mutex_);
    MEDIA_INFO_LOG("work end");
    return nullptr;
}

int32_t AdapterStreamCallback::Init(void)
{
    MediaMutexLock(mutex_);
    isRunning_ = true;
    MediaMutexUnLock(mutex_);

    MediaThreadattr attr = { "IdlbufProc", 0x2000, THREAD_SCHED_INVALID, 0, false };
    streamProcess_ = MediaThreadCreate(IdleBufferProcess, this, &attr);
    if (streamProcess_ == nullptr) {
        MEDIA_ERR_LOG("thread create failed");
        MediaMutexLock(mutex_);
        isRunning_ = false;
        MediaMutexUnLock(mutex_);
        return -1;
    }
    return 0;
}

void AdapterStreamCallback::DeInit(void)
{
    MediaMutexLock(mutex_);
    isRunning_ = false;
    MediaMutexUnLock(mutex_);
    if (streamProcess_ != nullptr) {
        MediaThreadJoin(&streamProcess_);
    }
    MediaMutexDestroy(&mutex_);
}

uint8_t *AdapterStreamCallback::GetBuffer(size_t index)
{
    BufferInfo info;
    if (bufferSource_ == nullptr) {
        MEDIA_ERR_LOG("bufferSource null");
        return nullptr;
    }
    if (bufferSource_->GetBufferInfo(index, &info) != 0) {
        MEDIA_ERR_LOG("GetBufferInfo failed");
        return nullptr;
    }
    return (uint8_t*)info.virAddr;
}

void AdapterStreamCallback::QueueBuffer(size_t index, size_t offset, size_t size, int64_t timestampUs, uint32_t flags)
{
    QueBuffer buffer;
    if (bufferSource_ == nullptr) {
        MEDIA_ERR_LOG("bufferSource null");
        return;
    }

    buffer.idx = index;
    buffer.flag = flags;
    buffer.offset = offset;
    buffer.size = size;
    buffer.timestamp = timestampUs;
    if (bufferSource_->QueFilledBuffer(&buffer) != 0) {
        MEDIA_ERR_LOG("QueFilledBuffer failed");
    }
}

void AdapterStreamCallback::SetParameters(const Format &params)
{
    MEDIA_UNUSED(params);
    MEDIA_ERR_LOG("process, not support");
}

int32_t Player::PlayerImpl::GetReadableSize(const void *handle)
{
    const PlayerImpl *playImpl = static_cast<const PlayerImpl*>(handle);
    CHK_NULL_RETURN(playImpl, "playImpl is nullptr");
    if (playImpl->bufferSource_ == nullptr) {
        MEDIA_ERR_LOG("bufferSource null");
        return -1;
    }
    return playImpl->bufferSource_->GetFilledQueDataSize();
}

void Player::PlayerImpl::ResetPlayerBuffer(PlayerImpl *playImpl, BufferInfo info)
{
    playImpl->buffer_.offset = 0;
    playImpl->buffer_.size = info.size;
    playImpl->bufferSource_->QueIdleBuffer(&playImpl->buffer_);
    playImpl->buffer_.idx = -1;
}

int32_t Player::PlayerImpl::ReadData(void *handle, uint8_t *data, int32_t size, int32_t timeOutMs, DataFlags *flags)
{
    MEDIA_UNUSED(timeOutMs);
    PlayerImpl *playImpl = (PlayerImpl*)handle;
    CHK_NULL_RETURN(playImpl, "playImpl is nullptr");
    CHK_NULL_RETURN(playImpl->bufferSource_, "bufferSource_ is nullptr");
    if (data == nullptr || size < 0  || flags == nullptr) {
        MEDIA_ERR_LOG("data null or buffer size < 0");
        return -1;
    }
    BufferInfo info;
    int readLen;
    if (playImpl->buffer_.idx == -1) {
        if (playImpl->bufferSource_->GetFilledQueSize() <= 0) {
            return 0;
        }
        if (playImpl->bufferSource_->DequeFilledBuffer(&playImpl->buffer_, 0) != 0) {
            playImpl->buffer_.idx = -1;
            return 0;
        }
    }
    if (playImpl->bufferSource_->GetBufferInfo(playImpl->buffer_.idx, &info) != 0) {
        return 0;
    }
    /* read all buffer data */
    if (playImpl->buffer_.size <= size) {
        if (playImpl->buffer_.size == 0 && playImpl->buffer_.flag == BUFFER_FLAG_EOS) {
            ResetPlayerBuffer(playImpl, info);
            *flags = DATA_FLAG_EOS;
            return 0;
        }
        if (memcpy_s(data, size, (unsigned char*)(info.virAddr) + playImpl->buffer_.offset,
            playImpl->buffer_.size) != EOK) {
            return -1;
        }
        *flags = (playImpl->buffer_.flag == BUFFER_FLAG_EOS) ? DATA_FLAG_EOS : DATA_FLAG_PARTIAL_FRAME;
        readLen = playImpl->buffer_.size;
        ResetPlayerBuffer(playImpl, info);
    } else {
        if (memcpy_s(data, size, (unsigned char*)(info.virAddr) + playImpl->buffer_.offset, size) != EOK) {
            return -1;
        }
        playImpl->buffer_.offset += size;
        playImpl->buffer_.size -= size;
        *flags = DATA_FLAG_PARTIAL_FRAME;
        readLen = size;
    }
    return readLen;
}

int32_t Player::PlayerImpl::GetStreamParam(const Format &format, StreamParam &param)
{
    std::string mimeType;
    if (!format.GetStringValue(AUDIO_CODEC_MIME, mimeType) || mimeType.length() == 0) {
        MEDIA_ERR_LOG("get mime type failed");
        return -1;
    }
    if (strcmp(mimeType.c_str(), MIME_AUDIO_AAC) == 0) {
        param.codecType = CODEC_AAC;
    } else if (strcmp(mimeType.c_str(), MIME_AUDIO_MP3) == 0) {
        param.codecType = CODEC_MP3;
    } else if (strcmp(mimeType.c_str(), MIME_AUDIO_PCM) == 0) {
        param.codecType = CODEC_PCM;
    } else {
        MEDIA_ERR_LOG("not support mime type:%s ", mimeType.c_str());
        return -1;
    }
    int32_t samplerate;
    if (!format.GetIntValue(SAMPLERATE_MIME, samplerate)) {
        MEDIA_ERR_LOG("get samplerate type failed");
        return -1;
    }
    param.sampleRate = samplerate;

    int32_t channel;
    if (!format.GetIntValue(CHANNEL_MIME, channel)) {
        MEDIA_ERR_LOG("get channel type failed");
        return -1;
    }
    param.channelCount = channel;
    return 0;
}

int32_t Player::PlayerImpl::SetStreamSource(const Source &source)
{
    Format format;
    StreamSourceParam  sourceParam = {};
    isStreamSource_ = true;
    isSingleLoop_ = false;
    format.CopyFrom(source.GetSourceStreamFormat());
    if (GetStreamParam(format, sourceParam.param) != 0) {
        MEDIA_ERR_LOG("GetStreamParam failed");
        return -1;
    }
    bufferSource_ = std::make_shared<BufferSource>();
    if (bufferSource_ == nullptr) {
        MEDIA_ERR_LOG("new BufferSource failed");
        return -1;
    }
    bufferSource_->Init();
    std::shared_ptr<StreamSource> stream = source.GetSourceStream();
    streamCallback_ = std::make_shared<AdapterStreamCallback>(stream, bufferSource_);
    if (streamCallback_ == nullptr || streamCallback_.get() == nullptr) {
        MEDIA_ERR_LOG("new AdapterStreamCallback failed");
        return -1;
    }
    streamCallback_->Init();
    stream->SetStreamCallback(streamCallback_);

    sourceParam.stream.handle = this;
    sourceParam.stream.ReadData = ReadData;
    sourceParam.stream.GetReadableSize = GetReadableSize;
    int32_t ret = player_->SetDataSource(sourceParam);
    if (ret != 0) {
        MEDIA_ERR_LOG("SetDataSource  exec failed");
        return -1;
    }
    return 0;
}

int32_t Player::PlayerImpl::SetParameter(const Format &params)
{
    MediaMutexLock(lock_);
    CHK_NULL_RETURN_WITH_UNLOCK(player_, lock_, "player_ is nullptr");
    std::string strKey;
    if (!params.GetStringValue(FORMAT_KEY, strKey)) {
        MEDIA_ERR_LOG("set GetParameter key fail");
    }
    CharValue value = {};
    int32_t ret = -1;
    if (!params.GetCharValue(strKey, value) || value.data == nullptr) {
        MEDIA_ERR_LOG("get char value failed!");
        goto EXIT;
    }
    ret = player_->SetParam(strKey, value.data, value.len);
EXIT:
    MediaMutexUnLock(lock_);
    return ret;
}

int32_t Player::PlayerImpl::GetParameter(Format &params)
{
    MediaMutexLock(lock_);
    CHK_NULL_RETURN_WITH_UNLOCK(player_, lock_, "player_ is nullptr");
    std::string value;
    int32_t ret = 0;
    if (!params.GetStringValue(FORMAT_KEY, value)) {
        MEDIA_ERR_LOG("get  GetParameter key fail");
    }
    if (value.compare(FORMAT_GET_ALBUM_INFO) == 0) {
        if (formatFileInfo_.albumInfoCnt == 0 || formatFileInfo_.albumInfo == nullptr) {
            goto EXIT;
        }

        const FormatAlbumArtInfoItem *info = formatFileInfo_.albumInfo;
        for (uint32_t i = 0; i < formatFileInfo_.albumInfoCnt; i++) {
            params.PutStringValue(info[i].key, info[i].value);
        }
    } else {
        CharValue realValue = {};
        if (!params.GetCharValue(value, realValue) || realValue.data == nullptr) {
            MEDIA_ERR_LOG("GetParameter CharValue fail");
            goto EXIT;
        }
        ret = player_->GetParam(value, realValue.data, realValue.len);
        if (ret != 0) {
            goto EXIT;
        }
    }
    MediaMutexUnLock(lock_);
    return 0;
EXIT:
    MediaMutexUnLock(lock_);
    return -1;
}

int32_t Player::PlayerImpl::GetTotalTracks(int32_t &totalTracks)
{
    MEDIA_INFO_LOG("process in");
    MediaMutexLock(lock_);
    CHECK_TRUE_CONDITION_RETURN_WITH_UNLOCK(released_, -1, "have released or not create", lock_);
    if (currentState_ != PLAYER_PREPARED && currentState_ != PLAYER_STARTED && currentState_ != PLAYER_PAUSED &&
        currentState_ != PLAYER_STOPPED && currentState_ != PLAYER_PLAYBACK_COMPLETE) {
        MediaMutexUnLock(lock_);
        return -1;
    }
    totalTracks = formatFileInfo_.trackCnt;
    MediaMutexUnLock(lock_);
    return 0;
}

int32_t Player::PlayerImpl::SetAudioSessionId(uint32_t sessionId)
{
    MEDIA_INFO_LOG("process in");
    if (sessionId == INVALID_SESSION_ID) {
        return -1;
    }
    MediaMutexLock(lock_);
    if (currentState_ != PLAYER_INITIALIZED && currentState_ != PLAYER_PREPARED) {
        MEDIA_ERR_LOG("failed, state %d", currentState_);
        MediaMutexUnLock(lock_);
        return -1;
    }
    CHK_NULL_RETURN_WITH_UNLOCK(player_, lock_, "player_ is nullptr");

    int32_t ret = player_->Invoke(INVOKE_SET_AUDIO_SESSION, &sessionId);
    if (ret != 0) {
        MEDIA_ERR_LOG("INVOKE_SET_AUDIO_SESSION failed");
        MediaMutexUnLock(lock_);
        return -1;
    }
    sessionId_ = sessionId;
    MediaMutexUnLock(lock_);
    return 0;
}

int32_t Player::PlayerImpl::GetAudioSessionId(uint32_t &sessionId)
{
    MEDIA_INFO_LOG("process in");
    MediaMutexLock(lock_);
    if (released_) {
        MEDIA_ERR_LOG("have released or not create");
        MediaMutexUnLock(lock_);
        return -1;
    }
    sessionId = sessionId_;
    MediaMutexUnLock(lock_);
    return 0;
}

int32_t Player::PlayerImpl::SetOutputDevice(int32_t deviceId)
{
    MEDIA_INFO_LOG("process in");
    MediaMutexLock(lock_);
    CHK_NULL_RETURN_WITH_UNLOCK(player_, lock_, "player_ is nullptr");

    int32_t ret = player_->Invoke(INVOKE_SET_OUTPUT_DEVICE, &deviceId);
    if (ret != 0) {
        MEDIA_ERR_LOG("INVOKE_SET_OUTPUT_DEVICE failed");
        MediaMutexUnLock(lock_);
        return -1;
    }
    outDevice_ = deviceId;
    MediaMutexUnLock(lock_);
    return 0;
}

int32_t Player::PlayerImpl::GetRoutedDeviceId(int32_t &deviceId)
{
    MEDIA_INFO_LOG("process in");
    MediaMutexLock(lock_);
    if (released_) {
        MEDIA_ERR_LOG("have released or not create");
        MediaMutexUnLock(lock_);
        return -1;
    }
    deviceId = outDevice_;
    MediaMutexUnLock(lock_);
    return 0;
}

void Player::PlayerImpl::PrintControlDumpInfo(const PlayerControlDebugInfo *playerControlInfo)
{
    printf("sessionId: %u\n", playerControlInfo->sessionId);
    printf("output device: 0x%x\n", playerControlInfo->outDevice);
    printf("currentPosition: %lld\n", playerControlInfo->currentPosition);
    printf("rewindPosition: %lld\n", playerControlInfo->rewindPosition);
    printf("play speed: %f\n", playerControlInfo->speed);
    printf("player state: %d\n", playerControlInfo->playerControlState);
    printf("is single loop: %s\n", playerControlInfo->isSingleLoop ? "yes" : "no");
    printf("is audio started: %s\n", playerControlInfo->isAudioStarted ? "yes" : "no");
    printf("is video started: %s\n", playerControlInfo->isVideoStarted ? "yes" : "no");
    printf("has Read Audio end: %s\n", playerControlInfo->hasRenderAudioEos ? "yes" : "no");
    printf("has Read video end: %s\n", playerControlInfo->hasRenderVideoEos ? "yes" : "no");
    printf("is off load Audio: %s\n", playerControlInfo->offloadAudio ? "yes" : "no");
}

void Player::PlayerImpl::PrintFileDumpInfo(const PlayerFileDebugInfo *playerFileInfo)
{
    printf("filePath: %s\n", playerFileInfo->filePath.c_str());
    printf("fd: %d\n", playerFileInfo->fd);
    printf("offset: %llu\n", playerFileInfo->offset);
    printf("streamType: 0x%x\n", playerFileInfo->streamType);
    printf("start time: %lld\n", playerFileInfo->s64StartTime);
    printf("file size: %lld\n", playerFileInfo->s64FileSize);
    printf("Duration: %lld\n", playerFileInfo->s64Duration);
    printf("video stream index: %d\n", playerFileInfo->s32UsedVideoStreamIndex);
    printf("audio stream inde: %d\n", playerFileInfo->s32UsedAudioStreamIndex);
}

void Player::PlayerImpl::PrintAudioAndVideoDumpInfo(const PlayerAudioDebugInfo *playerAudioInfo,
    const PlayerVideoDebugInfo *playerVideolInfo)
{
    if (formatFileInfo_.s32UsedAudioStreamIndex >= 0) {
        printf("-----------audio stream info-----------\n");
        printf("audio stream index: %d\n", playerAudioInfo->s32UsedAudioStreamIndex);
        printf("audio type: %d\n", playerAudioInfo->enAudioType);
        printf("audio channel Cnt: %u\n", playerAudioInfo->u32AudioChannelCnt);
        printf("sample format: %u\n", playerAudioInfo->sampleFmt);
        printf("SampleRate: %u\n", playerAudioInfo->u32SampleRate);
        printf("bitWidth: %u\n", playerAudioInfo->bitWidth);
    }
    if (formatFileInfo_.s32UsedVideoStreamIndex >= 0) {
        printf("-----------video stream info-----------\n");
        printf("video stream index: %d\n", playerVideolInfo->s32UsedVideoStreamIndex);
        printf("video type: %d\n", playerVideolInfo->enVideoType);
        printf("height: %u\n", playerVideolInfo->u32Height);
        printf("widt: %u\n", playerVideolInfo->u32Width);
        printf("frameBufCnt: %u\n", playerVideolInfo->frameBufCnt);
    }
}

void Player::PlayerImpl::PrintPlayerDumpInfo(const PlayerDebugInfo *playerInfo)
{
    if (formatFileInfo_.s64Duration != -1) {
        printf("-----------Player file info-----------\n");
        PrintFileDumpInfo(&playerInfo->fileDebugInfo);
        printf("\n");
    }
    if (player_ != nullptr) {
        printf("-----------Player stream info-----------\n");
        PrintAudioAndVideoDumpInfo(&playerInfo->audioDebugInfo, &playerInfo->videoDebugInfo);
        printf("\n");
        printf("-----------Player control info-----------\n");
        PrintControlDumpInfo(&playerInfo->playControlInfo);
        printf("\n");
    }
}

void Player::PlayerImpl::PlayerThreadCondSignal(void)
{
    player_->PlayerControlCondSignal();
}

void Player::PlayerImpl::OnInfoPlayCallBack(int32_t type, int32_t extra)
{
    if (callback_ != nullptr) {
        callback_->OnInfo(type, extra);
    }
}

int32_t Player::PlayerImpl::DumpInfo(PlayerDebugInfo *playerInfo)
{
    if (playerInfo == nullptr) {
        MEDIA_ERR_LOG("playerInfo is nullptr!");
        return -1;
    }
    playerInfo->fileDebugInfo.streamType = audioStreamType_;
    if (formatFileInfo_.s64Duration != -1) {
        playerInfo->fileDebugInfo.s64StartTime = formatFileInfo_.s64StartTime;
        playerInfo->fileDebugInfo.s64Duration = formatFileInfo_.s64Duration;
        playerInfo->fileDebugInfo.s64FileSize = formatFileInfo_.s64FileSize;
        playerInfo->fileDebugInfo.s32UsedAudioStreamIndex = formatFileInfo_.s32UsedAudioStreamIndex;
        playerInfo->fileDebugInfo.s32UsedVideoStreamIndex = formatFileInfo_.s32UsedVideoStreamIndex;
        if (formatFileInfo_.s32UsedVideoStreamIndex >= 0) {
            playerInfo->videoDebugInfo.s32UsedVideoStreamIndex = formatFileInfo_.s32UsedVideoStreamIndex;
        }
        if (formatFileInfo_.s32UsedAudioStreamIndex >= 0) {
            playerInfo->audioDebugInfo.s32UsedAudioStreamIndex = formatFileInfo_.s32UsedAudioStreamIndex;
            playerInfo->audioDebugInfo.sampleFmt = formatFileInfo_.sampleFmt;
            playerInfo->audioDebugInfo.u32SampleRate = formatFileInfo_.u32SampleRate;
            playerInfo->audioDebugInfo.u32AudioChannelCnt = formatFileInfo_.u32AudioChannelCnt;
        }
    }
    if (player_ != nullptr) {
        player_->GetPlayerControlDumpInfo(playerInfo->playControlInfo);
        playerInfo->playControlInfo.speed = speed_;
        playerInfo->playControlInfo.rewindPosition = rewindPosition_;
        playerInfo->playControlInfo.playerControlState = playerControlState_;
        playerInfo->playControlInfo.isSingleLoop = isSingleLoop_;
        player_->GetPlayerFileDumpInfo(playerInfo->fileDebugInfo);
        if (formatFileInfo_.s32UsedVideoStreamIndex >= 0) {
            player_->GetPlayerVideoDumpInfo(playerInfo->videoDebugInfo);
        }
        if (formatFileInfo_.s32UsedAudioStreamIndex >= 0) {
            player_->GetPlayerAudioDumpInfo(playerInfo->audioDebugInfo);
        }
    }
    PrintPlayerDumpInfo(playerInfo);
    return 0;
}
}  // namespace Media
}  // namespace OHOS

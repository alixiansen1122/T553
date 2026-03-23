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

#include <unistd.h>
#include <sys/time.h>
#include "player_sink_type.h"
#include "media_log.h"
#include "player_check.h"
#include "player_sync.h"

namespace OHOS {
namespace Media {
const int32_t SYNC_STOP_NEGATIVE_MS = 6000;
const int32_t SYNC_STOP_PLUS_MS = 3000;
const int32_t SYNC_START_NEGATIVE_MS = 100;
const int32_t SYNC_START_PLUS_MS = 50;
const int32_t MAX_VID_LOST_FRAMENUM = 10;
const int32_t FIRST_VID_RENDER_WAIT_TIME_WHEN_NO_AUDIO = 50000;
const int32_t FIRST_AUDIO_RENDER_WAIT_VID_TIME = 10000000; /* audio wait first video frame time */
const int32_t SS2US = 1000000;
#ifdef SUPPORT_AUDIO_VIDEO_SYNC
const int32_t US2MS = 1000;
const int64_t INT64_MAX_VALUE = 0x7fffffffffffffff;
#endif

static int64_t GetCurTimeUs()
{
    struct timeval ts = { 0, 0 };
    gettimeofday(&ts, nullptr);
    return (((int64_t)ts.tv_sec) * SS2US) + ((int64_t)ts.tv_usec);
}

SyncRet PlayerSync::CheckAVDiff(int64_t latenessMs, uint32_t &continueLost)
{
    SyncRet syncRet = SYNC_RET_PLAY;

    if (continueLost >= MAX_VID_LOST_FRAMENUM) {
        continueLost = 0;
        return syncRet;
    }
    // sync lateness out range exception; pts jump err, just skip this frame
    if (latenessMs > (int64_t)syncAttr_.syncStopRegion.vidNegativeTime ||
        latenessMs < (-(int64_t)syncAttr_.syncStopRegion.vidPlusTime)) {
        syncRet = SYNC_RET_DROP;
        continueLost++;
        return syncRet;
    }
    if (latenessMs > (int64_t)syncAttr_.syncStartRegion.vidNegativeTime) {
        syncRet = SYNC_RET_DROP;
        continueLost++;
    } else if (latenessMs < (-(int64_t)syncAttr_.syncStartRegion.vidPlusTime)) {
        syncRet = SYNC_RET_REPEAT;
    } else {
        syncRet = SYNC_RET_PLAY;
        continueLost = 0;
    }
    return syncRet;
}

/* Synchronization processing in tplay mode, considering only video
 * stream which using system time as the reference.
 * lateness = (system time difference) * speed - PTS) / speed.
 */
int32_t PlayerSync::TPlayProcess(int64_t timestampUs, SyncRet &syncRet)
{
    int64_t nowTsUs;
    int64_t latenessUs = 0;
    int64_t realTimeElapseUs = GetCurTimeUs() - frstVidFrameTime_;
    int64_t trickTimeElapseUs = static_cast<int64_t>(realTimeElapseUs * speed_);

    if (speed_ > 0) {
        nowTsUs = firstVidFrameTs_ + trickTimeElapseUs;
        latenessUs = (nowTsUs - timestampUs);
    } else {  // backward
        nowTsUs = firstVidFrameTs_ - trickTimeElapseUs;
        if (nowTsUs < 0) {
            nowTsUs = 0;
        }
        latenessUs = (timestampUs - nowTsUs);
    }
    int64_t latenessMs = (speed_ != 0) ? (latenessUs / (static_cast<int>(speed_ * MS_SCALE))) : 0;
    syncRet = CheckAVDiff(latenessMs, continousVidLost_);
    return HI_SUCCESS;
}

int32_t PlayerSync::Reset(SyncChn syncChn)
{
    diffAvRenderTime_ = 0;

    if (syncChn == SYNC_CHN_VID) {
        MediaMutexLock(vidSyncLock_);
        isFristVidFrame_ = true;
        vidTimeSourceDelta_ = 0;
        lastVideoTsUs_ = AV_INVALID_PTS;
        continousVidLost_ = 0;
        firstVidFrameTs_ = AV_INVALID_PTS;
        frstVidFrameTime_ = 0;
        MediaMutexUnLock(vidSyncLock_);
    } else if (syncChn == SYNC_CHN_AUD) {
        MediaMutexLock(audSyncLock_);
        isFristAudFrame_ = true;
        audTimeSourceDelta_ = 0;
        lastAudioTsUs_ = AV_INVALID_PTS;
        lastAudioRealTsUs_ = AV_INVALID_PTS;
        firstAudFrameTs_ = AV_INVALID_PTS;
        firstAudFrameTime_ = 0;
        MediaMutexUnLock(audSyncLock_);
    } else {
        MEDIA_ERR_LOG("invalid sync chn: %d", syncChn);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

int32_t PlayerSync::GetStatus(PlaySyncStatus &info)
{
    info.firstAudPts = (firstVidFrameTs_ == AV_INVALID_PTS) ? AV_INVALID_PTS : (firstVidFrameTs_ / MS_SCALE);
    info.lastAudPts = (lastAudioTsUs_ == AV_INVALID_PTS) ? AV_INVALID_PTS : (lastAudioTsUs_ / MS_SCALE);
    info.firstVidPts = (firstVidFrameTs_ == AV_INVALID_PTS) ? AV_INVALID_PTS : (firstVidFrameTs_ / MS_SCALE);
    info.lastVidPts = (lastVideoTsUs_ == AV_INVALID_PTS) ? AV_INVALID_PTS : (lastVideoTsUs_ / MS_SCALE);
    info.diffTime = diffAvRenderTime_;
    info.localTime = AV_INVALID_PTS;
    return HI_SUCCESS;
}

int32_t PlayerSync::GetAttr(PlayerSyncAttr &syncAttr)
{
    syncAttr = syncAttr_;
    return HI_SUCCESS;
}

int32_t PlayerSync::SetAttr(PlayerSyncAttr &syncAttr)
{
    syncAttr_ = syncAttr;
    return HI_SUCCESS;
}

PlayerSync::PlayerSync()
    : isVidEnable_(false),
      isAudEnable_(false),
      isFristVidFrame_(false),
      vidTimeSourceDelta_(0),
      lastVideoTsUs_(AV_INVALID_PTS),
      firstVidFrameTs_(0),
      frstVidFrameTime_(0),
      continousVidLost_(0),
      isFristAudFrame_(false),
      audTimeSourceDelta_(0),
      lastAudioTsUs_(AV_INVALID_PTS),
      lastAudioRealTsUs_(0),
      firstAudFrameTs_(AV_INVALID_PTS),
      firstAudFrameTime_(0),
      speed_(1.0),
      diffAvRenderTime_(0),
      isInited_(false)
{
    syncAttr_.isQuickOutput = true;
    syncAttr_.refType = PLAYER_SYNC_REF_AUD;
    syncAttr_.syncStartRegion.vidNegativeTime = SYNC_START_NEGATIVE_MS;
    syncAttr_.syncStartRegion.vidPlusTime = SYNC_START_PLUS_MS;
    syncAttr_.syncStopRegion.vidNegativeTime = SYNC_STOP_NEGATIVE_MS;
    syncAttr_.syncStopRegion.vidPlusTime = SYNC_STOP_PLUS_MS;
}

PlayerSync::~PlayerSync()
{
    (void)Deinit();
}

int32_t PlayerSync::Deinit(void)
{
    int32_t ret = HI_SUCCESS;
    if (isInited_) {
        MediaMutexDestroy(&audSyncLock_);
        MediaMutexDestroy(&vidSyncLock_);
        isInited_ = false;
    }
    return ret;
}

int32_t PlayerSync::Init(void)
{
    if (isInited_) {
        return HI_SUCCESS;
    }

    audSyncLock_ = MediaMutexCreate(nullptr);
    vidSyncLock_ = MediaMutexCreate(nullptr);

    (void)Reset(SYNC_CHN_VID);
    (void)Reset(SYNC_CHN_AUD);
    isInited_ = true;
    return HI_SUCCESS;
}

int32_t PlayerSync::Start(SyncChn syncChn)
{
    if (!isInited_) {
        MEDIA_ERR_LOG("sync start before inited");
        return HI_FAILURE;
    }
    if (syncChn == SYNC_CHN_VID) {
        isVidEnable_ = true;
    } else if (syncChn == SYNC_CHN_AUD) {
        isAudEnable_ = true;
    } else {
        MEDIA_ERR_LOG("invalid sync chn: %d", syncChn);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

int32_t PlayerSync::SetSpeed(float speed)
{
    if (!isInited_) {
        MEDIA_ERR_LOG("sync SetSpeed before inited");
        return HI_FAILURE;
    }
    MediaMutexLock(vidSyncLock_);
    speed_ = speed;
    MediaMutexUnLock(vidSyncLock_);
    return HI_SUCCESS;
}

/* only used to resume from tplay to normal */
int32_t PlayerSync::Resume()
{
    if (!isInited_) {
        MEDIA_ERR_LOG("sync Resume before inited");
        return HI_FAILURE;
    }
    MediaMutexLock(vidSyncLock_);
    speed_ = 1.0;
    MediaMutexUnLock(vidSyncLock_);
    return HI_SUCCESS;
}

int32_t PlayerSync::Stop(SyncChn syncChn)
{
    if (!isInited_) {
        MEDIA_ERR_LOG("sync Stop before inited");
        return HI_FAILURE;
    }
    if (syncChn == SYNC_CHN_VID) {
        isVidEnable_ = false;
        Reset(syncChn);
    } else if (syncChn == SYNC_CHN_AUD) {
        isAudEnable_ = false;
        Reset(syncChn);
    } else {
        MEDIA_ERR_LOG("invalid sync chn: %d", syncChn);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

void PlayerSync::UpdateCurTimeWithAudio(int64_t &nowUs)
{
    if (syncAttr_.refType == PLAYER_SYNC_REF_AUD) {
        // ref with audio pts
        if (lastAudioTsUs_ != AV_INVALID_PTS) {
            /* compensate for audio frame pts gap maybe too long, eg: 64ms */
            nowUs = lastAudioTsUs_ + (GetCurTimeUs() - lastAudioRealTsUs_);
        }
    }
}

SyncRet PlayerSync::ProcessFristVideoFrame(int64_t ptsUs)
{
    if (!isAudEnable_) {
        if (frstVidFrameTime_ == 0) {
            frstVidFrameTime_ = GetCurTimeUs();
        }
        if (GetCurTimeUs() - frstVidFrameTime_ < FIRST_VID_RENDER_WAIT_TIME_WHEN_NO_AUDIO) {
            return SYNC_RET_REPEAT;
        }
    }

    // caculate pts and clock delta of first frame
    int64_t firstTimeUs = GetCurTimeUs();
    vidTimeSourceDelta_ = firstTimeUs - ptsUs;
    lastVideoTsUs_ = ptsUs;
    frstVidFrameTime_ = firstTimeUs;
    firstVidFrameTs_ = ptsUs;
    isFristVidFrame_ = false;
    return SYNC_RET_PLAY;
}

int32_t PlayerSync::CheckVidSyncEnable(SyncRet &result, int32_t &ret)
{
    if (!isVidEnable_) {
        MEDIA_ERR_LOG("sync module have not enabled");
        result = SYNC_RET_DROP;
        ret = HI_FAILURE;
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

int32_t PlayerSync::CheckAudSyncEnable(SyncRet &result, int32_t &ret)
{
    if (!isAudEnable_) {
        MEDIA_ERR_LOG("sync module have not enabled");
        ret = HI_FAILURE;
        return HI_FAILURE;
    }
    if (!isVidEnable_) {
        MEDIA_DEBUG_LOG("pure audio no need sync");
        result = SYNC_RET_PLAY;
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

int32_t PlayerSync::ProcVidFrame(int64_t ptsMs, SyncRet &result)
{
    result = SYNC_RET_PLAY;
#ifdef SUPPORT_AUDIO_VIDEO_SYNC
    // add pause/resume cause sound freezes, temporarily close av sync
    CHECK_FALSE_CONDITION_RETURN(isInited_, HI_FAILURE, "sync ProcVidFrame before inited");
    int64_t nowUs = AV_INVALID_PTS;
    int64_t ptsUs = AV_INVALID_PTS;
    int32_t ret = HI_SUCCESS;

    MediaMutexLock(vidSyncLock_);
    if (CheckVidSyncEnable(result, ret) != HI_SUCCESS) {
        goto UNLOCK;
    }
    if (syncAttr_.refType == PLAYER_SYNC_REF_NONE) {
        result = SYNC_RET_PLAY;
        goto UNLOCK;
    }
    if (ptsMs > INT64_MAX_VALUE / US2MS) {
        goto UNLOCK;
    }
    ptsUs = ptsMs * US2MS;

    // first video frame quickoutput
    if (isFristVidFrame_) {
        result = ProcessFristVideoFrame(ptsUs);
        goto UNLOCK;
    }
    if (speed_ != 1.0) {
        (void)TPlayProcess(ptsUs, result);
        lastVideoTsUs_ = ptsUs;
        goto UNLOCK;
    }
    nowUs = GetCurTimeUs() - vidTimeSourceDelta_;
    UpdateCurTimeWithAudio(nowUs);
    diffAvRenderTime_ = (nowUs - ptsUs) / MS_SCALE;
    if (syncAttr_.refType == PLAYER_SYNC_REF_VID) {
        if (diffAvRenderTime_ < (-(int64_t)(syncAttr_.syncStartRegion.vidPlusTime))) {
            result = SYNC_RET_REPEAT;
        } else { /* do not care the fps and decode speed, just play */
            result = SYNC_RET_PLAY;
        }
    } else {
        result = CheckAVDiff(diffAvRenderTime_, continousVidLost_);
    }
    lastVideoTsUs_ = ptsUs;
    // fall through
UNLOCK:
    MediaMutexUnLock(vidSyncLock_);
    return ret;
#else
    PLAYER_UNUSED(ptsMs);
    return HI_SUCCESS;
#endif
}

void PlayerSync::WaitForFirstVideoFrame(SyncRet &result, bool &timeOut)
{
    if (firstAudFrameTime_ == 0) {
        firstAudFrameTime_ = GetCurTimeUs();
    }
    if (GetCurTimeUs() - firstAudFrameTime_ <= FIRST_AUDIO_RENDER_WAIT_VID_TIME) {
        result = SYNC_RET_DROP;
        timeOut = false;
    } else {
        timeOut = true;
    }
}

int32_t PlayerSync::ProcessFristAudioFrame(int64_t timeUs)
{
    // caculate pts and clock delta of first frame
    int64_t firstTimeUs = GetCurTimeUs();
    audTimeSourceDelta_ = firstTimeUs - timeUs;
    isFristAudFrame_ = false;
    firstAudFrameTs_ = timeUs;
    return HI_SUCCESS;
}

void PlayerSync::ProcessAudioDiff(int64_t timeUs)
{
    // calc current should be timestamp ref in sys clock
    int64_t nowUs = GetCurTimeUs() - audTimeSourceDelta_;
    int64_t latenessMs = (nowUs - timeUs) / MS_SCALE;
    // if have mechanism to fast or shorter audio sound play, handle here, now we just ignore
    if (latenessMs > (int64_t)syncAttr_.syncStopRegion.vidNegativeTime ||
        latenessMs < (-(int64_t)syncAttr_.syncStopRegion.vidPlusTime)) {
    }
}

int32_t PlayerSync::ProcAudFrame(int64_t ptsMs, SyncRet &result)
{
    result = SYNC_RET_PLAY;
#ifdef SUPPORT_AUDIO_VIDEO_SYNC
    // add pause/resume cause sound freezes, temporarily close av sync
    CHECK_FALSE_CONDITION_RETURN(isInited_, HI_FAILURE, "sync ProcAudFrame before inited");
    bool timeOut = false;
    int64_t timeUs = AV_INVALID_PTS;
    int32_t ret = HI_SUCCESS;

    MediaMutexLock(audSyncLock_);
    result = SYNC_RET_DROP;
    if (CheckAudSyncEnable(result, ret)) {
        goto UNLOCK;
    }
    if (ptsMs > INT64_MAX_VALUE / US2MS) {
        goto UNLOCK;
    }
    /* video stream exist, and wait for first video frame. video out first */
    if (isVidEnable_ && isFristVidFrame_) {
        WaitForFirstVideoFrame(result, timeOut);
        if (!timeOut) {
            goto UNLOCK;
        }
    }
    timeUs = ptsMs * US2MS;

    if (isFristAudFrame_ && ProcessFristAudioFrame(timeUs) != HI_SUCCESS) {
        result = SYNC_RET_REPEAT;
        ret = HI_SUCCESS;
        goto UNLOCK;
    }

    if (syncAttr_.refType == PLAYER_SYNC_REF_AUD) {
        ProcessAudioDiff(timeUs);
        lastAudioTsUs_ = timeUs;
        lastAudioRealTsUs_ = GetCurTimeUs();
    } else if (syncAttr_.refType == PLAYER_SYNC_REF_VID) {
        MEDIA_ERR_LOG("current do not support ref video, if have audio");
        result = SYNC_RET_DROP;
        ret = HI_FAILURE;
        goto UNLOCK;
    }
    result = SYNC_RET_PLAY;
    // fall through
UNLOCK:
    MediaMutexUnLock(audSyncLock_);
    return ret;
#else
    PLAYER_UNUSED(ptsMs);
    return HI_SUCCESS;
#endif
}
};
};

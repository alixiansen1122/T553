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

#ifndef PLAYER_IMPL_H
#define PLAYER_IMPL_H

#include <memory>
#include <vector>
#include <sys/time.h>
#include "buffer_source.h"
#include "hi_demuxer.h"
#include "liteplayer.h"
#include "player_define.h"
#include "player.h"
#include "source.h"
#if defined(ENABLE_UIKIT)
#include "surface.h"
#elif defined(ENABLE_LVGL)
#include "lv_surface_view.h"
#endif
#include "media_thread_adapt.h"

namespace OHOS {
namespace Media {
using OHOS::Media::PlayerControl;
using namespace std;
enum PlayAdapterEvt {
    PAYERADP_EVT_PREPARED           = 0,
    PAYERADP_EVT_PLAY_COMPLETE      = 1,
    PAYERADP_EVT_REWIND_COMPLETE    = 2,
    PAYERADP_EVT_VIDEO_SOLUTION     = 3,
    PAYERADP_EVT_VIDEO_PLAYED       = 4,
    PAYERADP_EVT_PLAY_TIME          = 5,
    PAYERADP_EVT_ERROR              = 6,
    PAYERADP_EVT_INFO               = 7,
};

class AdapterStreamCallback : public StreamCallback {
public:
    AdapterStreamCallback(const std::shared_ptr<StreamSource> &stream, const std::shared_ptr<BufferSource> &buffer);
    virtual ~AdapterStreamCallback();
    uint8_t* GetBuffer(size_t index) override;
    void QueueBuffer(size_t index, size_t offset, size_t size, int64_t timestampUs, uint32_t flags) override;
    void SetParameters(const Format &params) override;
    int32_t Init(void);
    void DeInit(void);
private:
    static void* IdleBufferProcess(void* arg);

    std::weak_ptr<StreamSource> streamSource_;
    std::shared_ptr<BufferSource> bufferSource_;
    MediaThreadIdHandle streamProcess_;
    MediaMutexHandle mutex_;
    bool isRunning_;
};

class Player::PlayerImpl {
friend class BufferSource;
friend struct StreamSource;

public:
    PlayerImpl();
    ~PlayerImpl();
    int32_t SetSource(const Source &source);
    int32_t Prepare();
    int32_t Play();
    bool IsPlaying();
    int32_t Pause();
    int32_t Stop();
    int32_t Rewind(int64_t mSeconds, int32_t mode);
    int32_t SetVolume(float leftVolume, float rightVolume);
#if defined(ENABLE_UIKIT)
    int32_t SetSurface(Surface *surface);
#elif defined(ENABLE_LVGL)
    int32_t SetSurface(LvSurfaceView *surface);
#endif
    bool IsSingleLooping();
    int32_t GetPlayerState(int32_t &state);
    int32_t GetCurrentTime(int64_t &currentPosition);
    int32_t GetDuration(int64_t &durationMs);
    int32_t SetPlaybackSpeed(float speed);
    int32_t GetPlaybackSpeed(float &speed);
    int32_t SetAudioStreamType(int32_t type);
    int32_t GetAudioStreamType(int32_t &type);
    int32_t Reset(void);
    int32_t Release();
    int32_t EnableSingleLooping(bool loop);
    void SetPlayerCallback(const std::shared_ptr<PlayerCallback> &cb);
    int32_t Init(void);
    int32_t DeInit(void);
    int32_t SetParameter(const Format &params);
    int32_t GetParameter(Format &params);
    int32_t GetTotalTracks(int32_t &totalTracks);
    int32_t SetAudioSessionId(uint32_t sessionId);
    int32_t GetAudioSessionId(uint32_t &sessionId);
    int32_t SetOutputDevice(int32_t deviceId);
    int32_t GetRoutedDeviceId(int32_t &deviceId);
    int32_t DumpInfo(PlayerDebugInfo *playerInfo);
    void PlayerThreadCondSignal(void);
    void OnInfoPlayCallBack(int32_t type, int32_t extra);
private:
    void PrintPlayerDumpInfo(const PlayerDebugInfo *playerInfo);
    void PrintControlDumpInfo(const PlayerControlDebugInfo *playerControlInfo);
    void PrintFileDumpInfo(const PlayerFileDebugInfo *playerFileInfo);
    void PrintAudioAndVideoDumpInfo(const PlayerAudioDebugInfo *playerAudioInfo,
        const PlayerVideoDebugInfo *playerVideolInfo);
    void NotifySeekComplete(PlayerImpl *curPlayer, int64_t seekToMs);
    void NotifyPlaybackComplete(PlayerImpl *curPlayer);
    int32_t SetUriSource(const Source &source);
    int32_t SetFdSource(const Source &source);
    int SetStreamSource(const Source &source);
    int32_t RewindInner(int64_t mSeconds, PlayerSeekMode mode);
    bool IsValidRewindMode(PlayerSeekMode mode);
    void UpdateState(PlayerImpl *curPlayer, PlayerStatus state);
    static void PlayerControlEventCb(void* pPlayer, PlayerControlEvent enEvent, const void* pData);
    static void PlayerControlDefaultEventCb(PlayerImpl *curPlayer, PlayerControlEvent enEvent, const void *pData);
    int GetPlayer();
    static int32_t GetReadableSize(const void *handle);
    static void ResetPlayerBuffer(PlayerImpl *playImpl, BufferInfo info);
    static int32_t ReadData(void *handle, uint8_t *data, int32_t size, int32_t timeOutMs, DataFlags *flags);
    int CreatePlayerParamCheck(PlayerControlParam &createParam);
    void GetDurationInner(int64_t &durationMs);
    void ResetInner(void);
    int32_t SetMediaStream(void);
    bool IsValidStreamType(int32_t type);
    static int32_t GetStreamParam(const Format &format, StreamParam &param);
    bool IsSpeedValid(float speed);

    std::shared_ptr<PlayerControl> player_;
    float speed_;
    FormatFileInfo formatFileInfo_;
    PlayerControlStreamAttr mediaAttr_;
    PlayerStatus playerControlState_;
    bool isSingleLoop_;
    int64_t currentPosition_;
    int64_t rewindPosition_;
#if defined(ENABLE_UIKIT)
    Surface *surface_;
#elif defined(ENABLE_LVGL)
    LvSurfaceView *surface_;
#endif
    uint32_t currentState_;
    PlayerSeekMode rewindMode_;
    PlayerSeekMode currentRewindMode_;
    int audioStreamType_;
    MediaMutexHandle lock_;
    MediaMutexHandle rewindLock_;
    std::shared_ptr<PlayerCallback> callback_;
    bool inited_;
    bool released_;
    bool isStreamSource_;
    QueBuffer buffer_;
    std::shared_ptr<BufferSource> bufferSource_;
    std::shared_ptr<AdapterStreamCallback> streamCallback_;
    bool extraRewind_;
    uint32_t sessionId_;
    int32_t      outDevice_;
    struct timeval rewindStratTime_;
    struct timeval rewindEndTime_;
    struct timeval playerStopTime_;
    struct timeval playerReleaseTime_;
};
}  // namespace Media
}  // namespace OHOS

#endif  // PLAYER_IMPL_H

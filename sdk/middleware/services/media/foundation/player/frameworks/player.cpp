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

#include "player.h"

#include <cinttypes>
#include <sys/stat.h>

#include "media_log.h"
#include "player_impl.h"

using namespace std;

namespace OHOS {
namespace Media {

#define CHK_NULL_RETURN(ptr) \
do { \
    if ((ptr) == nullptr) { \
        MEDIA_ERR_LOG("ptr null"); \
        return (-1); \
    } \
} while (0)

Player::Player()
    : impl_(new (std::nothrow) PlayerImpl())
{
    MEDIA_INFO_LOG("Player process");
}

Player::~Player()
{
    MEDIA_INFO_LOG("~Player process");
}

int32_t Player::SetSource(const Source &source)
{
    int32_t ret;
    CHK_NULL_RETURN(impl_);
    ret = impl_->Init();
    if (ret == 0) {
        ret = impl_->SetSource(source);
    }
    return ret;
}

int32_t Player::Prepare()
{
    CHK_NULL_RETURN(impl_);
    return impl_->Prepare();
}

int32_t Player::Play()
{
    CHK_NULL_RETURN(impl_);
    return impl_->Play();
}

bool Player::IsPlaying()
{
    CHK_NULL_RETURN(impl_);
    return impl_->IsPlaying();
}

int32_t Player::Pause()
{
    CHK_NULL_RETURN(impl_);
    return impl_->Pause();
}

int32_t Player::Stop()
{
    CHK_NULL_RETURN(impl_);
    return impl_->Stop();
}

int32_t Player::Rewind(int64_t mSeconds, int32_t mode)
{
    CHK_NULL_RETURN(impl_);
    int32_t ret = impl_->Rewind(mSeconds, mode);
    return ret;
}

int32_t Player::SetVolume(float leftVolume, float rightVolume)
{
    CHK_NULL_RETURN(impl_);
    return impl_->SetVolume(leftVolume, rightVolume);
}

#if defined(ENABLE_UIKIT) || defined(ENABLE_LVGL)
#if defined(ENABLE_UIKIT)
int32_t Player::SetVideoSurface(Surface *surface)
#elif defined(ENABLE_LVGL)
int32_t Player::SetVideoSurface(LvSurfaceView *surface)
#endif
{
    CHK_NULL_RETURN(impl_);
    return impl_->SetSurface(surface);
}
#endif

bool Player::IsSingleLooping()
{
    CHK_NULL_RETURN(impl_);
    return impl_->IsSingleLooping();
}

int32_t Player::GetCurrentTime(int64_t &time) const
{
    CHK_NULL_RETURN(impl_);
    return impl_->GetCurrentTime(time);
}

int32_t Player::GetDuration(int64_t &durationMs) const
{
    CHK_NULL_RETURN(impl_);
    return impl_->GetDuration(durationMs);
}

int32_t Player::Reset()
{
    CHK_NULL_RETURN(impl_);
    return impl_->Reset();
}

int32_t Player::Release()
{
    CHK_NULL_RETURN(impl_);
    int32_t ret = impl_->Release();
    MEDIA_INFO_LOG("process out");
    return ret;
}

void Player::SetPlayerCallback(const std::shared_ptr<PlayerCallback> &cb)
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("impl_ null");
        return;
    }
    impl_->SetPlayerCallback(cb);
}

int32_t Player::EnableSingleLooping(bool loop)
{
    CHK_NULL_RETURN(impl_);
    return impl_->EnableSingleLooping(loop);
}

int32_t Player::GetPlayerState(int32_t &state) const
{
    CHK_NULL_RETURN(impl_);
    return impl_->GetPlayerState(state);
}

int32_t Player::SetPlaybackSpeed(float speed)
{
    CHK_NULL_RETURN(impl_);
    return impl_->SetPlaybackSpeed(speed);
}

int32_t Player::GetPlaybackSpeed(float &speed)
{
    CHK_NULL_RETURN(impl_);
    return impl_->GetPlaybackSpeed(speed);
}

int32_t Player::SetAudioStreamType(int32_t type)
{
    CHK_NULL_RETURN(impl_);
    return impl_->SetAudioStreamType(type);
}

int32_t Player::GetAudioStreamType(int32_t &type)
{
    CHK_NULL_RETURN(impl_);
    return impl_->GetAudioStreamType(type);
}

int32_t Player::SetParameter(const Format &params)
{
    CHK_NULL_RETURN(impl_);
    return impl_->SetParameter(params);
}

int32_t Player::GetParameter(Format &params)
{
    CHK_NULL_RETURN(impl_);
    return impl_->GetParameter(params);
}

int32_t Player::GetTotalTracks(int32_t &totalTracks)
{
    CHK_NULL_RETURN(impl_);
    return impl_->GetTotalTracks(totalTracks);
}

int32_t Player::SetAudioSessionId(uint32_t sessionId)
{
    CHK_NULL_RETURN(impl_);
    return impl_->SetAudioSessionId(sessionId);
}
int32_t Player::GetAudioSessionId(uint32_t &sessionId)
{
    CHK_NULL_RETURN(impl_);
    return impl_->GetAudioSessionId(sessionId);
}
int32_t Player::SetOutputDevice(int32_t deviceId)
{
    CHK_NULL_RETURN(impl_);
    return impl_->SetOutputDevice(deviceId);
}
int32_t Player::GetRoutedDeviceId(int32_t &deviceId)
{
    CHK_NULL_RETURN(impl_);
    return impl_->GetRoutedDeviceId(deviceId);
}
int32_t Player::DumpInfo(PlayerDebugInfo *playerInfo)
{
    CHK_NULL_RETURN(impl_);
    return impl_->DumpInfo(playerInfo);
}
}  // namespace Media
}  // namespace OHOS

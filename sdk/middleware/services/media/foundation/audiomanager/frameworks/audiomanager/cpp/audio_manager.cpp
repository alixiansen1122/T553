/*
* Copyright (c) CompanyNameMagicTag. 2021-2021. All rights reserved.
* Description: audio manager
* Author: Media Software Group
* Create: 2021-02-28
*/

#include "audio_manager.h"
#include "audio_manager_impl.h"
#include "securec.h"
#include "audio_errors.h"
#include "audio_utils.h"

#define LOG_MODULE_NAME "AudioManager"

namespace Audio {
AudioManager::AudioManager() noexcept
    : impl_(nullptr)
{
}

AudioManager::~AudioManager()
{
    delete impl_;
    impl_ = nullptr;
}

bool AudioManager::Initialize()
{
    if (impl_ == nullptr) {
        impl_ = new AudioManagerImpl();
    }
    CHK_NULL_RETURN(impl_, false, "impl_ is nullptr");
    return impl_->Initialize();
}

AudioSession AudioManager::MakeSessionId()
{
    CHK_NULL_RETURN(impl_, AUDIO_SESSION_ID_NONE, "impl_ is nullptr");
    return impl_->MakeSessionId();
}

bool AudioManager::SetVolume(AudioStreamType streamType, int32_t volume)
{
    CHK_NULL_RETURN(impl_, false, "impl_ is nullptr");
    return impl_->SetVolume(streamType, volume);
}

int32_t AudioManager::GetVolume(AudioStreamType streamType)
{
    CHK_NULL_RETURN(impl_, AUDIO_ERROR, "impl_ is nullptr");
    return impl_->GetVolume(streamType);
}

int32_t AudioManager::GetMinVolume(AudioStreamType streamType)
{
    CHK_NULL_RETURN(impl_, AUDIO_ERROR, "impl_ is nullptr");
    return impl_->GetMinVolume(streamType);
}

int32_t AudioManager::GetMaxVolume(AudioStreamType streamType)
{
    CHK_NULL_RETURN(impl_, AUDIO_ERROR, "impl_ is nullptr");
    return impl_->GetMaxVolume(streamType);
}

bool AudioManager::Mute(AudioStreamType streamType)
{
    CHK_NULL_RETURN(impl_, false, "impl_ is nullptr");
    return impl_->Mute(streamType);
}

bool AudioManager::UnMute(AudioStreamType streamType)
{
    CHK_NULL_RETURN(impl_, false, "impl_ is nullptr");
    return impl_->UnMute(streamType);
}

bool AudioManager::IsMute(AudioStreamType streamType)
{
    CHK_NULL_RETURN(impl_, false, "impl_ is nullptr");
    return impl_->IsMute(streamType);
}

bool AudioManager::SetMasterMute(bool isMute)
{
    CHK_NULL_RETURN(impl_, false, "impl_ is nullptr");
    return impl_->SetMasterMute(isMute);
}

bool AudioManager::IsMasterMute()
{
    CHK_NULL_RETURN(impl_, false, "impl_ is nullptr");
    return impl_->IsMasterMute();
}

bool AudioManager::SetMicrophoneMute(bool isMute)
{
    CHK_NULL_RETURN(impl_, false, "impl_ is nullptr");
    return impl_->SetMicrophoneMute(isMute);
}

bool AudioManager::IsMicrophoneMute()
{
    CHK_NULL_RETURN(impl_, false, "impl_ is nullptr");
    return impl_->IsMicrophoneMute();
}

bool AudioManager::SetRingerMode(AudioRingMode mode)
{
    CHK_NULL_RETURN(impl_, false, "impl_ is nullptr");
    return impl_->SetRingerMode(mode);
}

int32_t AudioManager::GetRingerMode()
{
    CHK_NULL_RETURN(impl_, AUDIO_ERROR, "impl_ is nullptr");
    return impl_->GetRingerMode();
}


std::vector<AudioDeviceInfo> AudioManager::GetDevices(DeviceFlag flag)
{
    std::vector<AudioDeviceInfo> devices;
    if (impl_ == nullptr) {
        ALOGE("impl_ is nullptr");
        return devices;
    }
    return impl_->GetDevices(flag);
}

bool AudioManager::IsSupportOffload(AudioOffloadInfo &info)
{
    CHK_NULL_RETURN(impl_, false, "impl_ is nullptr");
    return impl_->IsSupportOffload(info);
}

int32_t AudioManager::SetDeviceConnectionState(AudioDeviceType device, AudioDeviceConnectState state)
{
    CHK_NULL_RETURN(impl_, AUDIO_ERROR, "impl_ is nullptr");
    return impl_->SetDeviceConnectionState(device, state);
}

int32_t AudioManager::ActivateAudioInterrupt(AudioInterrupt &interrupt)
{
    CHK_NULL_RETURN(impl_, AUDIO_ERROR, "impl_ is nullptr");
    return impl_->ActivateAudioInterrupt(interrupt);
}

int32_t AudioManager::DeactivateAudioInterrupt(AudioInterrupt &interrupt)
{
    CHK_NULL_RETURN(impl_, AUDIO_ERROR, "impl_ is nullptr");
    return impl_->DeactivateAudioInterrupt(interrupt);
}

int32_t AudioManager::DumpInfo(AudioManagerDebugInfo &audioManagerInfo)
{
    CHK_NULL_RETURN(impl_, AUDIO_ERROR, "impl_ is nullptr");
    return impl_->DumpInfo(audioManagerInfo);
}

int32_t AudioManager::SetParam(AudioLinkDirection direction, const char *param, uint32_t len)
{
    CHK_NULL_RETURN(impl_, AUDIO_ERROR, "impl_ is nullptr");
    return impl_->SetParam(direction, param, len);
}

int32_t AudioManager::GetParam(AudioLinkDirection direction, char *param, uint32_t len)
{
    CHK_NULL_RETURN(impl_, AUDIO_ERROR, "impl_ is nullptr");
    return impl_->GetParam(direction, param, len);
}
}  // namespace Audio

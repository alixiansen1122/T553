/*
* Copyright (c) @CompanyNameMagicTag. 2021-2021. All rights reserved.
* Description: audio manager implement
* Author: Media Software Group
* Create: 2021-04-21
*/

#ifndef AUDIO_CAPTURER_IMPL_H
#define AUDIO_CAPTURER_IMPL_H

#include "audio_manager.h"
#include "audio_service.h"
#include "audio_callback_dispatcher.h"

namespace Audio {
class AudioManager::AudioManagerImpl {
public:
    AudioManagerImpl();
    virtual ~AudioManagerImpl();
    bool Initialize();
    AudioSession MakeSessionId();
    bool SetVolume(AudioStreamType streamType, int32_t volume);
    int32_t GetVolume(AudioStreamType streamType);
    int32_t GetMinVolume(AudioStreamType streamType);
    int32_t GetMaxVolume(AudioStreamType streamType);
    bool Mute(AudioStreamType streamType);
    bool UnMute(AudioStreamType streamType);
    bool IsMute(AudioStreamType streamType);
    bool SetMasterMute(bool isMute);
    bool IsMasterMute();
    bool SetMicrophoneMute(bool isMute);
    bool IsMicrophoneMute();
    bool SetRingerMode(AudioRingMode mode);
    int32_t GetRingerMode();
    std::vector<AudioDeviceInfo> GetDevices(DeviceFlag flag);
    bool IsSupportOffload(AudioOffloadInfo &info);
    int32_t SetDeviceConnectionState(AudioDeviceType device, AudioDeviceConnectState state);
    int32_t ActivateAudioInterrupt(AudioInterrupt &interrupt);
    int32_t DeactivateAudioInterrupt(AudioInterrupt &interrupt);
    int32_t DumpInfo(AudioManagerDebugInfo &audioManagerInfo);
    int32_t SetParam(AudioLinkDirection direction, const char *param, uint32_t len);
    int32_t GetParam(AudioLinkDirection direction, char *param, uint32_t len);
private:
    static void InterruptListenerImpl(uintptr_t owner, uintptr_t userData, int32_t type, int32_t hint);
    int32_t ConvertIAudioInterrupt(AudioInterrupt &interrupt, AudioInterruptProxy &interruptInternal);
    AudioCallbackDispatcher callbackDispatcher_;
    std::vector<std::shared_ptr<InterruptListener>> interruptListenerInfo_;
};
}  // namespace Audio

#endif  // AUDIO_CAPTURER_IMPL_H

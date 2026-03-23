/*
* Copyright (c) CompanyNameMagicTag. 2021-2021. All rights reserved.
* Description: audio manager impl
* Author: Media Software Group
* Create: 2021-02-28
*/

#include "audio_manager_impl.h"
#include <climits>
#include <list>
#include <sys/time.h>
#ifdef SUPPORT_SYS_PRCTL
#include <sys/prctl.h>
#endif
#include <unistd.h>

#include "audio_utils.h"
#include "audio_errors.h"
#include "securec.h"

#define LOG_MODULE_NAME "AudioManagerImpl"

namespace Audio {
AudioManager::AudioManagerImpl::AudioManagerImpl()
{
}

AudioManager::AudioManagerImpl::~AudioManagerImpl()
{
    AudioServiceDeInit();
}

bool AudioManager::AudioManagerImpl::Initialize()
{
#ifdef AUDIOMANAGER_ENABLE_KPI
    int32_t ret = SetAudioLogEnabledLevel(AUDIO_LOG_LEVEL_ERR);
    if (ret != 0) {
        ALOGE("SetAudioLogEnabledLevel failed");
    }
#endif

    if (AudioServiceInit() != AUDIO_SUCCESS) {
        return false;
    }
    return callbackDispatcher_.Initialize();
}

AudioSession AudioManager::AudioManagerImpl::MakeSessionId()
{
    return AudioServiceAcquireAudioSessionId();
}

bool AudioManager::AudioManagerImpl::SetVolume(AudioStreamType streamType, int32_t volume)
{
    return AudioServiceSetVolume(streamType, volume);
}

int32_t AudioManager::AudioManagerImpl::GetVolume(AudioStreamType streamType)
{
    return AudioServiceGetVolume(streamType);
}

int32_t AudioManager::AudioManagerImpl::GetMinVolume(AudioStreamType streamType)
{
    return AudioServiceGetMinVolume(streamType);
}

int32_t AudioManager::AudioManagerImpl::GetMaxVolume(AudioStreamType streamType)
{
    return AudioServiceGetMaxVolume(streamType);
}

bool AudioManager::AudioManagerImpl::Mute(AudioStreamType streamType)
{
    return AudioServiceMute(streamType);
}

bool AudioManager::AudioManagerImpl::UnMute(AudioStreamType streamType)
{
    return AudioServiceUnMute(streamType);
}

bool AudioManager::AudioManagerImpl::IsMute(AudioStreamType streamType)
{
    return AudioServiceIsMute(streamType);
}

bool AudioManager::AudioManagerImpl::SetMasterMute(bool isMute)
{
    return AudioServiceSetMasterMute(isMute);
}

bool AudioManager::AudioManagerImpl::IsMasterMute()
{
    return AudioServiceIsMasterMute();
}

bool AudioManager::AudioManagerImpl::SetMicrophoneMute(bool isMute)
{
    return AudioServiceSetMicrophoneMute(isMute);
}

bool AudioManager::AudioManagerImpl::IsMicrophoneMute()
{
    return AudioServiceIsMicrophoneMute();
}

bool AudioManager::AudioManagerImpl::SetRingerMode(AudioRingMode mode)
{
    return AudioServiceSetRingerMode(mode);
}

int32_t AudioManager::AudioManagerImpl::GetRingerMode()
{
    return AudioServiceGetRingerMode();
}

std::vector<AudioDeviceInfo> AudioManager::AudioManagerImpl::GetDevices(DeviceFlag flag)
{
    uint32_t deviceCount = 0;
    std::vector<AudioDeviceInfo> devices;
    int32_t ret = AudioServiceGetDeviceCount(flag, &deviceCount);
    if (ret != AUDIO_SUCCESS ||
        deviceCount == 0 ||
        deviceCount == UINT_MAX) {
        return devices;
    }
    AudioDeviceInfo *deviceInfo = new AudioDeviceInfo[deviceCount];
    if (deviceInfo == nullptr) {
        return devices;
    }
    ret = AudioServiceGetDevices(flag, deviceCount, deviceInfo);
    if (ret != AUDIO_SUCCESS) {
        delete []deviceInfo;
        return devices;
    }
    for (uint32_t i = 0; i < deviceCount; i++) {
        devices.push_back(deviceInfo[i]);
    }
    delete []deviceInfo;
    return devices;
}

bool AudioManager::AudioManagerImpl::IsSupportOffload(AudioOffloadInfo &info)
{
    return AudioServiceIsOffloadSupported(&info);
}

int32_t AudioManager::AudioManagerImpl::SetDeviceConnectionState(AudioDeviceType device, AudioDeviceConnectState state)
{
    return AudioServiceSetDeviceConnectionState(device, state);
}

void AudioManager::AudioManagerImpl::InterruptListenerImpl(uintptr_t owner, uintptr_t userData,
                                                           int32_t type, int32_t hint)
{
    AudioCallbackDispatcher *callbackDispatcher = reinterpret_cast<AudioCallbackDispatcher *>(owner);
    if (callbackDispatcher == nullptr) {
        ALOGE("owner is nullptr");
        return;
    }
    Message msg = {};
    msg.userData = userData;
    msg.type = type;
    msg.hint = hint;

    ALOGI("type:%d hint:%d", type, hint);
    callbackDispatcher->Post(msg, true);
}

int32_t AudioManager::AudioManagerImpl::ConvertIAudioInterrupt(AudioInterrupt &interrupt,
                                                               AudioInterruptProxy &interruptInternal)
{
    if (interrupt.interruptListener.get() == nullptr) {
        return AUDIO_INVALID_PARAMS;
    }
    interruptInternal.owner = reinterpret_cast<uintptr_t>(&callbackDispatcher_);
    interruptInternal.interruptListenerCb = InterruptListenerImpl;
    interruptInternal.userData = reinterpret_cast<uintptr_t>(interrupt.interruptListener.get());
    interruptInternal.streamType = interrupt.streamType;
    interruptInternal.sessionID = interrupt.sessionID;

    return AUDIO_SUCCESS;
}

int32_t AudioManager::AudioManagerImpl::ActivateAudioInterrupt(AudioInterrupt &interrupt)
{
    AudioInterruptProxy interruptInternal = {};
    int32_t ret = ConvertIAudioInterrupt(interrupt, interruptInternal);
    if (ret != AUDIO_SUCCESS) {
        return ret;
    }

    interruptListenerInfo_.push_back(interrupt.interruptListener);

    return AudioServiceActivateAudioInterrupt(&interruptInternal);
}

int32_t AudioManager::AudioManagerImpl::DeactivateAudioInterrupt(AudioInterrupt &interrupt)
{
    AudioInterruptProxy interruptInternal = {};
    int32_t ret = ConvertIAudioInterrupt(interrupt, interruptInternal);
    if (ret != AUDIO_SUCCESS) {
        return ret;
    }

    std::vector<std::shared_ptr<InterruptListener>>::iterator iter = interruptListenerInfo_.begin();
    for (; iter != interruptListenerInfo_.end(); iter++) {
        if ((*iter).get() == interrupt.interruptListener.get()) {
            break;
        }
    }

    if (iter != interruptListenerInfo_.end()) {
        interruptListenerInfo_.erase(iter);
    }

    return AudioServiceDeactivateAudioInterrupt(&interruptInternal);
}

int32_t AudioManager::AudioManagerImpl::DumpInfo(AudioManagerDebugInfo &audioManagerInfo)
{
    AudioServiceDebugInfo audioServiceDebugInfo = {};
    int32_t ret = AudioServiceDumpInfo(&audioServiceDebugInfo);
    if (ret != AUDIO_SUCCESS) {
        return ret;
    }
    uint32_t index;
    ALOGD("DumpInfo inputInterruptInfo:%d", audioServiceDebugInfo.audioInterruptInfo.inputInterruptCnt);
    for (index = 0; index < audioServiceDebugInfo.audioInterruptInfo.inputInterruptCnt; index++) {
        audioManagerInfo.inputInterruptInfo.push_back (
            audioServiceDebugInfo.audioInterruptInfo.inputInterruptInfo[index]);
    }
    ALOGD("DumpInfo outputInterruptCnt:%d", audioServiceDebugInfo.audioInterruptInfo.outputInterruptCnt);
    for (index = 0; index < audioServiceDebugInfo.audioInterruptInfo.outputInterruptCnt; index++) {
        audioManagerInfo.outputInterruptInfo.push_back (
            audioServiceDebugInfo.audioInterruptInfo.outputInterruptInfo[index]);
    }
    ALOGD("DumpInfo streamInfoCnt:%d", audioServiceDebugInfo.audioStreamInfo.streamInfoCnt);
    for (index = 0; index < audioServiceDebugInfo.audioStreamInfo.streamInfoCnt; index++) {
        audioManagerInfo.allstreamInfo.push_back (
            audioServiceDebugInfo.audioStreamInfo.allstreamInfo[index]);
    }
    ALOGD("DumpInfo deviceInfoCnt:%d", audioServiceDebugInfo.audioDeviceInfo.deviceInfoCnt);
    for (index = 0; index < audioServiceDebugInfo.audioDeviceInfo.deviceInfoCnt; index++) {
        audioManagerInfo.allDeviceInfo.push_back (
            audioServiceDebugInfo.audioDeviceInfo.devicesInfo[index]);
    }
    return AUDIO_SUCCESS;
}

int32_t AudioManager::AudioManagerImpl::SetParam(AudioLinkDirection direction, const char *param, uint32_t len)
{
    return AudioServiceSetParam(direction, param, len);
}

int32_t AudioManager::AudioManagerImpl::GetParam(AudioLinkDirection direction, char *param, uint32_t len)
{
    return AudioServiceGetParam(direction, param, len);
}
}  // namespace Audio

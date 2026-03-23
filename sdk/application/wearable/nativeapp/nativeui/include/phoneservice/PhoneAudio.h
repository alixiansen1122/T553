/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: PhoneService
 * Create: 2021-09-22
 */

#ifndef PHONE_AUDIO_H
#define PHONE_AUDIO_H

#include <cstdint>
#include "audio_base_type.h"
#include "ohos_types.h"

#ifdef __cplusplus
extern "C" {
#endif

int32 PhoneAudioGetMinVolume(void);
int32 PhoneAudioGetMaxVolume(void);
int32 PhoneAudioGetVolume(void);
bool PhoneAudioSetVolume(int32 volume);

// 设置通话speaker静音/取消静音
bool PhoneAudioSetSpeakerMute(bool isMute);

// 设置通话mic静音/取消静音
bool PhoneAudioSetMicMute(bool isMute);

int32 PhoneAudioManagerInit(void);

int32 PhoneAudioManagerActInterrupt(AudioStreamType streamType);

int32 PhoneAudioManagerSetDeviceConnState(AudioDeviceType deviceType, AudioDeviceConnectState connectState);

int32 PhoneAudioStreamInInit(AudioLinkDirection linkDir);

int32 PhoneAudioStreamInStart(int32 instanceNum);

int32 PhoneAudioStreamInGetChannelId(int32 instanceNum, uint32 *shmId);

int32 PhoneAudioStreamInAttachBackend(int32 instanceNum, uint32 backendId);

int32 PhoneAudioStreamOutInit(AudioLinkDirection linkDir);

int32 PhoneAudioStreamOutStart(int32 instanceNum);

int32 PhoneAudioStreamOutGetChannelId(int32 instanceNum, uint32 *shmId);

int32 PhoneAudioStreamOutAttachFrontend(int32 instanceNum, uint32 frontendId);

int32 PhoneAudioStreamInStop(int32 instanceNum);

int32 PhoneAudioStreamOutStop(int32 instanceNum);

int32 PhoneAudioStreamInDeinit(int32 instanceNum);

int32 PhoneAudioStreamOutDeinit(int32 instanceNum);

int32 PhoneAudioManagerDeactInterrupt(void);

#ifdef __cplusplus
}
#endif

#endif // PHONE_AUDIO_H
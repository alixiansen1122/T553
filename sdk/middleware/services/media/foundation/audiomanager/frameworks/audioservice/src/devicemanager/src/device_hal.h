/*
 * Copyright (c) CompanyNameMagicTag. 2021-2021. All rights reserved.
 * Description: media hal common
 * Author: Media Software Group
 * Create: 2021-03-02
 */

#ifndef DEVICE_HAL_H
#define DEVICE_HAL_H

#include "device_manager.h"
#include "audio_utils.h"

int32_t LoadAudioHal(void);
int32_t UnLoadAudioHal(void);
int32_t OpenInputStream(uint32_t device, AudioStreamConfig attr, Handle *inputStreamHandle);
int32_t CloseInputStream(uint32_t device, Handle inputStreamHandle);
int32_t OpenOutputStream(uint32_t device, AudioStreamConfig attr, Handle *outputStreamHandle);
int32_t CloseOutputStream(uint32_t device, Handle outputStreamHandle);
int32_t GetAdapterCapability(void);
int32_t DeviceHalInvoke(AudioHaidInvokeId audioHaidInvokeId, char *param, uint32_t *paramlength);
int32_t DeviceHalRegCallBack(AudioDeviceHaidEventCallback callBack, void *context);
AudioCapability *GetDeviceCapability(AudioDeviceType device);
#endif  // DEVICE_HAL_H

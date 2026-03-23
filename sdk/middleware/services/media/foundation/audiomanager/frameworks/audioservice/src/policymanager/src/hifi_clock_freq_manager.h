/*
* Copyright (c) CompanyNameMagicTag. 2023-2023. All rights reserved.
* Description: hifi clock freq manager interfaces
* Author: Media Software Group
* Create: 2023-11-09
*/

#ifndef HIFI_CLOCK_FREQ_MANAGER_H
#define HIFI_CLOCK_FREQ_MANAGER_H
#include <stdlib.h>
#include <stdio.h>
#include "audio_base_type.h"

void HifiClockFreqMgrInit(void);
void HifiClockFreqMgrDeInit(void);
int32_t SetHifiClockFreqByStreamTypeAndDeviceType(AudioStreamType streamType,
    AudioDeviceType deviceType, bool isNeedPush);

#endif  // HIFI_CLOCK_FREQ_MANAGER_H
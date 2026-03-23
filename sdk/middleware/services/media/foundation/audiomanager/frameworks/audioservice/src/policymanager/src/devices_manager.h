/*
* Copyright (c) CompanyNameMagicTag. 2021-2021. All rights reserved.
* Description: available devices manager interfaces
* Author: Media Software Group
* Create: 2021-04-07
*/

#ifndef AVAILABLE_DEVICES_MANAGER_H
#define AVAILABLE_DEVICES_MANAGER_H

#include <stdlib.h>
#include <stdio.h>
#include "audio_base_type.h"

/* Device process */
int32_t AvlDevicesMgrInit(void);
int32_t AvlDevicesMgrPushFront(const AudioDeviceInfo *deviceInfo);
int32_t AvlDevicesMgrPop(const AudioDeviceInfo *deviceInfo);
bool AvlDevicesMgrIsAvailable(AudioDeviceType device);
void AvlDevicesMgrDeInit(void);

#endif  // AVAILABLE_DEVICES_MANAGER_H

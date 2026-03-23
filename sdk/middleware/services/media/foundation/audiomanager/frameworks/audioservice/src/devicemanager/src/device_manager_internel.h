/*
 * Copyright (c) CompanyNameMagicTag. 2021-2021. All rights reserved.
 * Description: media hal common
 * Author: Media Software Group
 * Create: 2021-03-02
 */

#ifndef DEVICE_MANAGER_INTERNEL_H
#define DEVICE_MANAGER_INTERNEL_H

#include "device_manager.h"

typedef struct {
    AudioDeviceInfo            deviceInfo;
    AudioDeviceConnectState    state;
} AudioDeviceInfoInternel;

#endif  // DEVICE_MANAGER_INTERNEL_H

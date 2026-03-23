/*
 * Copyright (c) CompanyNameMagicTag. 2021-2021. All rights reserved.
 * Description: media hal common
 * Author: Media Software Group
 * Create: 2021-03-02
 */

#ifndef DEVICE_CONFIG_H
#define DEVICE_CONFIG_H

#include "audio_base_type.h"

#define MAX_ADAPTER_NUM   10

typedef struct {
    char             *adapterName;
    bool             isMatch;
    uint32_t         supportDeviceCount;
    AudioDeviceType  *supportDevice;
} AdapterDeviceMap;


int32_t LoadConfig(void);
int32_t GetAdapterListFromConfig(AdapterDeviceMap **adapterList, int32_t *adapterNum);

#endif  // DEVICE_MANAGER_H

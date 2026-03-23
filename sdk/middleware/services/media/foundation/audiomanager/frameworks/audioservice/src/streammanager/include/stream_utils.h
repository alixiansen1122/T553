/*
* Copyright (c) CompanyNameMagicTag. 2021-2021. All rights reserved.
* Description: stream common
* Author: Media Software Group
* Create: 2021-04-21
*/

#ifndef STREAM_UTILS_H
#define STREAM_UTILS_H

#include "audio_base_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define OUT_DEVICE_NUM 3
#define ONE_DEVICE 1
#define NO_DEVICE 0
#define S2NS 1000000000

typedef enum {
    IDLE = 0,
    RUNNING,
    PAUSE,
    STOP
} StreamState;

typedef struct {
    bool streamMute;
    float typeVolume;
} VolumeInf;

int32_t ConvertOutDeviceToHal(uint32_t deviceId, int32_t *halDevice);

char *StringState(StreamState state);

char *StringType(AudioStreamType streamType);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif

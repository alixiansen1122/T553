/*
* Copyright (c) CompanyNameMagicTag. 2021-2021. All rights reserved.
* Description: Audio debug info
* Author: Media Software Group
* Create: 2021-02-28
*/

#ifndef AUDIO_DEBUG_INFO_H
#define AUDIO_DEBUG_INFO_H

#include <stdint.h>
#include "audio_base_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define MAX_INST_NUM 6

typedef struct {
    AudioStreamType streamType;
    AudioStrategy  currentStrategy;
    AudioSession sessionID;
    int64_t streamCreateTime;
    int64_t streamDestroyTime;
} InterruptDebugInfo;

typedef struct {
    AudioDeviceInfo            deviceInfo;
    AudioDeviceConnectState    deviceState;
} DeviceDebugInfo;

typedef struct {
    uint32_t state;
    int32_t  streamIndex;
    AudioStreamInfo streamInfo;
    float streamVolume; // stream mgr
    float mixWeight;
    uint32_t curDevice;
} StreamDebugInfo;

typedef struct {
    uint32_t inputInterruptCnt;
    InterruptDebugInfo inputInterruptInfo[MAX_INST_NUM];
    uint32_t outputInterruptCnt;
    InterruptDebugInfo outputInterruptInfo[MAX_INST_NUM];
} AudioInterruptDebugInfo;

typedef struct {
    uint32_t streamInfoCnt;
    StreamDebugInfo allstreamInfo[MAX_INST_NUM];
} AudioStreamDebugInfo;

typedef struct {
    uint32_t deviceInfoCnt;
    DeviceDebugInfo devicesInfo[MAX_INST_NUM];
} AudioDeviceDebugInfo;

typedef struct {
    AudioInterruptDebugInfo audioInterruptInfo;
    AudioStreamDebugInfo audioStreamInfo;
    AudioDeviceDebugInfo audioDeviceInfo;
} AudioServiceDebugInfo;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif // AUDIO_UTILS_H
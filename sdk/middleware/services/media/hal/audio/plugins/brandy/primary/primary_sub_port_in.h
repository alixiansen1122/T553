/*
* Copyright (c) @CompanyNameMagicTag. 2021-2021. All rights reserved.
* Description: primary sub port in header
* Author: Media Software Group
* Create: 2021-09-28
*/

#ifndef AUDIO_PRIMARY_SUB_PORT_IN_H
#define AUDIO_PRIMARY_SUB_PORT_IN_H

#include "audio_port_plugin.h"

#ifdef __cplusplus
extern "C" {
#endif

/* sub port in track api */
int32_t AudioPrimarySubPortInGetFrameCount(const AudioHandle trackHandle, uint64_t *count);

int32_t AudioPrimarySubPortInEnableTrack(AudioHandle trackHandle);
int32_t AudioPrimarySubPortInDisableTrack(AudioHandle trackHandle);

int32_t AudioPrimarySubPortInAcquireFrame(AudioHandle trackHandle, uint8_t *buffer,
    uint64_t requestBytes, uint64_t *replyBytes);

int32_t AudioPrimarySubPortInGetPosition(AudioHandle trackHandle,
    uint64_t *frames, struct AudioTimeStamp *time);

int32_t AudioPrimarySubPortInSetMute(AudioHandle trackHandle, bool mute);
int32_t AudioPrimarySubPortInGetMute(AudioHandle trackHandle, bool *mute);

int32_t AudioPrimarySubPortInSetVolume(AudioHandle trackHandle, float volume);
int32_t AudioPrimarySubPortInGetVolume(AudioHandle trackHandle, float *volume);

int32_t AudioPrimarySubPortInInvoke(AudioHandle trackHandle, enum InvokeID invokeID, struct InvokeAttr *invokeAttr);

/* sub port in api */
int32_t AudioPrimarySubPortInCreate(AudioHandle *portHandle);
int32_t AudioPrimarySubPortInDestroy(AudioHandle portHandle);

int32_t AudioPrimarySubPortInGetCapability(const struct AudioPort *port, struct AudioPortCapability *capability);

int32_t AudioPrimarySubPortInCreateTrack(AudioHandle portHandle, const struct PortPluginAttr *inputAttr,
    AudioHandle *trackHandle);
int32_t AudioPrimarySubPortInDestroyTrack(AudioHandle portHandle, AudioHandle trackHandle);

#ifdef __cplusplus
}
#endif

#endif

/*
* Copyright (c) @CompanyNameMagicTag. 2021-2021. All rights reserved.
* Description: primary sub port out header
* Author: Media Software Group
* Create: 2021-09-28
*/

#ifndef AUDIO_PRIMARY_SUB_PORT_OUT_H
#define AUDIO_PRIMARY_SUB_PORT_OUT_H

#include "audio_port_plugin.h"

#ifdef __cplusplus
extern "C" {
#endif

/* sub port out track api */
int32_t AudioPrimarySubPortOutGetLatency(AudioHandle trackHandle, uint32_t *latencyMs);

int32_t AudioPrimarySubPortOutEnableTrack(AudioHandle trackHandle);
int32_t AudioPrimarySubPortOutDisableTrack(AudioHandle trackHandle);

int32_t AudioPrimarySubPortOutPauseTrack(AudioHandle trackHandle);
int32_t AudioPrimarySubPortOutResumeTrack(AudioHandle trackHandle);
int32_t AudioPrimarySubPortOutFlushTrack(const AudioHandle trackHandle);

int32_t AudioPrimarySubPortOutSendFrame(AudioHandle trackHandle, const uint8_t *buffer,
    uint64_t requestBytes, uint64_t *replyBytes);

int32_t AudioPrimarySubPortOutGetPosition(AudioHandle trackHandle,
    uint64_t *frames, struct AudioTimeStamp *time);

int32_t AudioPrimarySubPortOutTrackSetMute(AudioHandle trackHandle, bool mute);
int32_t AudioPrimarySubPortOutTrackGetMute(AudioHandle trackHandle, bool *mute);

int32_t AudioPrimarySubPortOutTrackSetVolume(AudioHandle trackHandle, float volume);
int32_t AudioPrimarySubPortOutTrackGetVolume(AudioHandle trackHandle, float *volume);

int32_t AudioPrimarySubPortOutInvoke(AudioHandle trackHandle,
    enum InvokeID invokeID, struct InvokeAttr *invokeAttr);

int32_t AudioPrimarySubPortOutRegCallback(AudioHandle trackHandle, TrackCallback callback, AudioHandle caller);
/* sub port out api */
int32_t AudioPrimarySubPortOutCreate(AudioHandle *portHandle);
int32_t AudioPrimarySubPortOutDestroy(AudioHandle portHandle);

int32_t AudioPrimarySubPortOutGetCapability(const struct AudioPort *port, struct AudioPortCapability *capability);

int32_t AudioPrimarySubPortOutCreateTrack(AudioHandle portHandle, const struct PortPluginAttr *inputAttr,
    AudioHandle *trackHandle);
int32_t AudioPrimarySubPortOutDestroyTrack(AudioHandle portHandle, AudioHandle trackHandle);

#ifdef __cplusplus
}
#endif

#endif

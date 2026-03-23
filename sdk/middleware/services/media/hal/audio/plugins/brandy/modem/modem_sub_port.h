/*
* Copyright (c) @CompanyNameMagicTag. 2023-2023. All rights reserved.
* Description: modem sub port header
* Author: Media Software Group
* Create: 2023-07-05
*/

#ifndef AUDIO_MODEM_SUB_PORT_H
#define AUDIO_MODEM_SUB_PORT_H

#include "audio_port_plugin.h"

#ifdef __cplusplus
extern "C" {
#endif

/* sub port track api */
int32_t AudioModemSubPortTrackGetLatency(AudioHandle trackHandle, uint32_t *latencyMs);

int32_t AudioModemSubPortEnableTrack(AudioHandle trackHandle);
int32_t AudioModemSubPortDisableTrack(AudioHandle trackHandle);

int32_t AudioModemSubPortPauseTrack(AudioHandle trackHandle);
int32_t AudioModemSubPortResumeTrack(AudioHandle trackHandle);

int32_t AudioModemSubPortAcquireFrame(AudioHandle trackHandle, uint8_t *buffer, uint64_t requestBytes,
    uint64_t *replyBytes);

int32_t AudioModemSubPortGetPosition(AudioHandle trackHandle, uint64_t *frames, struct AudioTimeStamp *time);

int32_t AudioModemSubPortSendFrame(AudioHandle trackHandle, const uint8_t *buffer, uint64_t requestBytes,
    uint64_t *replyBytes);

int32_t AudioModemSubPortTrackSetMute(AudioHandle trackHandle, bool mute);
int32_t AudioModemSubPortTrackGetMute(AudioHandle trackHandle, bool *mute);

int32_t AudioModemSubPortTrackSetVolume(AudioHandle trackHandle, float volume);
int32_t AudioModemSubPortTrackGetVolume(AudioHandle trackHandle, float *volume);

int32_t AudioModemSubPortInvoke(AudioHandle trackHandle, enum InvokeID invokeID, struct InvokeAttr *invokeAttr);

int32_t AudioModemSubPortTrackRegCallback(AudioHandle trackHandle, TrackCallback callback, AudioHandle caller);

/* sub port api */
int32_t AudioModemSubPortCreate(AudioHandle *portHandle);
int32_t AudioModemSubPortDestroy(AudioHandle portHandle);

int32_t AudioModemSubPortGetCapability(const struct AudioPort *port, struct AudioPortCapability *capability);

int32_t AudioModemSubPortCreateTrack(AudioHandle portHandle, const struct PortPluginAttr *inputAttr,
    enum AudioPortPin portPin, AudioHandle *trackHandle);
int32_t AudioModemSubPortDestroyTrack(AudioHandle portHandle, AudioHandle trackHandle);

#ifdef __cplusplus
}
#endif

#endif

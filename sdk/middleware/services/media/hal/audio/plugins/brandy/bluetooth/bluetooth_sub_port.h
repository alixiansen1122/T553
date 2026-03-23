/*
* Copyright (c) @CompanyNameMagicTag. 2021-2021. All rights reserved.
* Description: bluetooth sub port header
* Author: Media Software Group
* Create: 2021-09-28
*/

#ifndef AUDIO_BLUETOOTH_SUB_PORT_H
#define AUDIO_BLUETOOTH_SUB_PORT_H

#include "audio_port_plugin.h"

#ifdef __cplusplus
extern "C" {
#endif

/* sub port track api */
int32_t AudioBluetoothSubPortTrackGetLatency(AudioHandle trackHandle, uint32_t *latencyMs);

int32_t AudioBluetoothSubPortEnableTrack(AudioHandle trackHandle);
int32_t AudioBluetoothSubPortDisableTrack(AudioHandle trackHandle);

int32_t AudioBluetoothSubPortPauseTrack(AudioHandle trackHandle);
int32_t AudioBluetoothSubPortResumeTrack(AudioHandle trackHandle);
int32_t AudioBluetoothSubPortFlushTrack(AudioHandle trackHandle);

int32_t AudioBluetoothSubPortAcquireFrame(AudioHandle trackHandle, uint8_t *buffer, uint64_t requestBytes,
    uint64_t *replyBytes);

int32_t AudioBluetoothSubPortGetPosition(AudioHandle trackHandle, uint64_t *frames, struct AudioTimeStamp *time);

int32_t AudioBluetoothSubPortSendFrame(AudioHandle trackHandle, const uint8_t *buffer, uint64_t requestBytes,
    uint64_t *replyBytes);

int32_t AudioBluetoothSubPortTrackSetMute(AudioHandle trackHandle, bool mute);
int32_t AudioBluetoothSubPortTrackGetMute(AudioHandle trackHandle, bool *mute);

int32_t AudioBluetoothSubPortTrackSetVolume(AudioHandle trackHandle, float volume);
int32_t AudioBluetoothSubPortTrackGetVolume(AudioHandle trackHandle, float *volume);

int32_t AudioBluetoothSubPortInvoke(AudioHandle trackHandle, enum InvokeID invokeID, struct InvokeAttr *invokeAttr);

int32_t AudioBluetoothSubPortTrackRegCallback(AudioHandle trackHandle, TrackCallback callback, AudioHandle caller);

/* sub port api */
int32_t AudioBluetoothSubPortCreate(AudioHandle *portHandle);
int32_t AudioBluetoothSubPortDestroy(AudioHandle portHandle);

int32_t AudioBluetoothSubPortGetCapability(const struct AudioPort *port, struct AudioPortCapability *capability);

int32_t AudioBluetoothSubPortCreateTrack(AudioHandle portHandle, const struct PortPluginAttr *inputAttr,
    enum AudioPortPin portPin, AudioHandle *trackHandle);
int32_t AudioBluetoothSubPortDestroyTrack(AudioHandle portHandle, AudioHandle trackHandle);

#ifdef __cplusplus
}
#endif

#endif

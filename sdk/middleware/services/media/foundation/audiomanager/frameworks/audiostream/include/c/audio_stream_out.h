/*
* Copyright (c) CompanyNameMagicTag. 2023-2023. All rights reserved.
* Description: audio stream interfaces
* Author: Media Software Group
* Create: 2023-05-24
*/

#ifndef AUDIO_STREAM_OUT_H
#define AUDIO_STREAM_OUT_H

#include <time.h>
#include "audio_base_type.h"
#include "audio_debug_info.h"

typedef void *AudioStreamOutHandle;

int32_t AudioStreamOutCreate(AudioStreamOutHandle *handle);
int32_t AudioStreamOutInit(AudioStreamOutHandle handle, const AudioRendererConfig *config,
    RendererCallback cbk, const void *caller);
int32_t AudioStreamOutPlay(AudioStreamOutHandle handle);
int32_t AudioStreamOutPause(AudioStreamOutHandle handle);
int32_t AudioStreamOutStop(AudioStreamOutHandle handle);
int32_t AudioStreamOutRelease(AudioStreamOutHandle handle);
int32_t AudioStreamOutSetPlayBackSpeed(AudioStreamOutHandle handle, float speed);
int32_t AudioStreamOutSetVolume(AudioStreamOutHandle handle, float volume);
int32_t AudioStreamOutGetCurrentDeviceId(AudioStreamOutHandle handle);
int32_t AudioStreamOutGetCurrentChannelId(AudioStreamOutHandle handle, uint32_t *channelId);
int32_t AudioStreamOutAttachFrontend(AudioStreamOutHandle handle, uint32_t frontendId);
int32_t AudioStreamOutStreamWrite(AudioStreamOutHandle handle, const uint8_t *buffer,
    uint64_t userSize, uint64_t *writtenSize);
int32_t AudioStreamOutGetPosition(AudioStreamOutHandle handle, int64_t *position);
int32_t AudioStreamOutGetTimestamp(AudioStreamOutHandle handle, struct timespec *timeStamp);
int32_t AudioStreamOutFlush(AudioStreamOutHandle handle);
int32_t AudioStreamOutSetOutputDevice(AudioStreamOutHandle handle, int32_t deviceId);
int32_t AudioStreamOutDumpInfo(AudioStreamOutHandle handle, StreamDebugInfo *info);
int32_t AudioStreamOutSetParam(AudioStreamOutHandle handle, const char *keyValueList, int32_t len);
int32_t AudioStreamOutSetToggleConfig(AudioStreamOutHandle handle,
    ToggleConfigType type, ToggleConfig toggleConfig);
int32_t AudioStreamOutDestroy(AudioStreamOutHandle handle);

#endif  // AUDIO_STREAM_OUT_H

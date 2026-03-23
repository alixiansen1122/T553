/*
* Copyright (c) CompanyNameMagicTag. 2023-2023. All rights reserved.
* Description: audio stream interfaces
* Author: Media Software Group
* Create: 2023-05-24
*/

#ifndef AUDIO_STREAM_IN_H
#define AUDIO_STREAM_IN_H

#include <time.h>
#include "audio_base_type.h"


/**
 * @brief Enumerates the time base of timespec. Different timing methods are supported.
 */
typedef enum {
    /** Monotonically increasing time, excluding the system sleep time */
    MONOTONIC = 0,
    /** Monotonically increasing time, including the system sleep time */
    BOOTTIME = 1
} Timebase;

typedef void *AudioStreamInHandle;
int32_t AudioStreamInCreate(AudioStreamInHandle *handle);
int32_t AudioStreamInInit(AudioStreamInHandle handle, const CapturerInputConfig *config,
    CapturerCallback cbk, const void *caller);
int32_t AudioStreamInGetCurrentDeviceId(AudioStreamInHandle handle);
int32_t AudioStreamInGetCurrentChannelId(AudioStreamInHandle handle, uint32_t *channelId);
int32_t AudioStreamInAttachBackend(AudioStreamInHandle handle, uint32_t backendId);
int32_t AudioStreamInGetTimestamp(AudioStreamInHandle handle, struct timespec *timestamp, Timebase base);
int32_t AudioStreamInStart(AudioStreamInHandle handle);
int32_t AudioStreamInStop(AudioStreamInHandle handle);
int32_t AudioStreamInSetInputDevice(AudioStreamInHandle handle, int32_t deviceId);
int32_t AudioStreamInObtainBuffer(AudioStreamInHandle handle, const CapturerBuffer *buffer);
int32_t AudioStreamInRelease(AudioStreamInHandle handle);
int32_t AudioStreamInDestroy(AudioStreamInHandle handle);
#endif

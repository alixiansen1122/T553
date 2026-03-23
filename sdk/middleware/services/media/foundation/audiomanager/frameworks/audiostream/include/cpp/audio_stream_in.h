/*
* Copyright (c) CompanyNameMagicTag. 2021-2021. All rights reserved.
* Description: audio stream interfaces
* Author: Media Software Group
* Create: 2021-04-21
*/

#ifndef AUDIO_STREAM_IN_H
#define AUDIO_STREAM_IN_H

#include <ctime>
#include "audio_base_type.h"

namespace Audio {
/**
 * @brief Enumerates the time base of timespec. Different timing methods are supported.
 */
enum Timebase : int32_t {
    /** Monotonically increasing time, excluding the system sleep time */
    MONOTONIC = 0,
    /** Monotonically increasing time, including the system sleep time */
    BOOTTIME = 1
};
class AudioStreamIn {
public:
    AudioStreamIn();
    ~AudioStreamIn();
    int32_t Init(const CapturerInputConfig &config, CapturerCallback cbk = nullptr, const void *caller = nullptr);
    int32_t GetCurrentDeviceId() const;
    int32_t GetCurrentChannelId(uint32_t &channelId) const;
    int32_t AttachBackend(uint32_t &backendId) const;
    int32_t GetTimestamp(struct timespec &timestamp, Timebase base) const;
    int32_t Start() const;
    int32_t Stop() const;
    int32_t SetInputDevice(int32_t deviceId, AudioRendererConfig &renderConfig) const;
    int32_t ObtainBuffer(CapturerBuffer &buffer, bool isBlocking) const;
    int32_t Release() const;
    int32_t SetParam(const char *keyValueList, int32_t len) const;
    int32_t GetParam(char *keyValueList, int32_t len);
private:
    uintptr_t streamHandle_;
    void *mutex;
};
}

#endif

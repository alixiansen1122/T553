
/*
* Copyright (c) CompanyNameMagicTag. 2021-2021. All rights reserved.
* Description: audio stream interfaces
* Author: Media Software Group
* Create: 2021-04-21
*/

#ifndef AUDIO_STREAM_OUT_H
#define AUDIO_STREAM_OUT_H

#include <ctime>
#include <string>
#include "audio_base_type.h"
#include "audio_debug_info.h"

namespace Audio {
class AudioStreamOut {
public:
    AudioStreamOut();
    ~AudioStreamOut();
    int32_t Init(AudioRendererConfig &config, RendererCallback cbk = nullptr, const void *caller = nullptr);
    int32_t Play() const;
    int32_t Pause() const;
    int32_t Stop() const;
    int32_t Release() const;
    int32_t SetPlayBackSpeed(float speed) const;
    int32_t SetMute(bool mute) const;
    int32_t SetVolume(float volume) const;
    int32_t GetCurrentDeviceId() const;
    int32_t GetCurrentChannelId(uint32_t &channelId) const;
    int32_t AttachFrontend(uint32_t &frontendId) const;
    int32_t StreamWrite(const uint8_t *buffer, uint64_t userSize, uint64_t &writtenSize,
        AudioRendererConfig &renderConfig) const;
    int32_t GetPosition(int64_t &position) const;
    int32_t GetTimestamp(struct timespec &timeStamp) const;
    int32_t Flush() const;
    int32_t SetOutputDevice(int32_t deviceId, AudioRendererConfig &renderConfig) const;
    int32_t GetBufferFrameSize() const;
    int32_t GetBufferUnderflowTimes() const; // del DFX
    StreamDebugInfo DumpInfo() const;
    int32_t SetParam(const char *keyValueList, int32_t len) const;
    int32_t GetParam(char *keyValueList, int32_t len);
private:
    uintptr_t streamHandle_;
    void *mutex;
};
}  // namespace Audio
#endif  // AUDIO_STREAM_OUT_H

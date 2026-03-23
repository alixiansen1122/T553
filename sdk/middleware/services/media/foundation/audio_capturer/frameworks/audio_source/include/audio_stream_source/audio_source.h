/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef AUDIO_SOURCE_H_
#define AUDIO_SOURCE_H_

#include <cstddef>
#include <cstdint>
#include <ctime>
#include <memory>
#include <vector>

#include "source_base.h"
#include "audio_manager.h"
#include "audio_stream_in.h"

namespace OHOS {
namespace Audio {
using ::Audio::AudioManager;
using ::Audio::AudioStreamIn;
class AudioSource : public SourceBase {
public:
    AudioSource();
    ~AudioSource() override;

    /**
    * Enumerates currently supported devices by audio source type.
    *
    * @param inputSource the type of source audio.
    * @param devices holds an array of statisfied audio device description, including name and identity.
    * @return Returns SUCCESS if success, other values otherwise.
    */
    int32_t EnumDeviceBySourceType(AudioSourceType inputSource, std::vector<AudioDeviceDesc> &devices) override;

     /**
     * Obtains the minimum frame count (in BytesPerSample) required in the specified conditions.
     *
     * @param sampleRate Indicates the sampling rate (Hz).
     * @param channelCount Indicates the audio channel count.
     * @param audioFormat Indicates the audio data format.
     * @param frameCount the minimum frame count (in BytesPerSample).
     * @return Returns {@code true} if success; returns {@code false} otherwise.
     */
    static bool GetMinFrameCount(int32_t sampleRate, int32_t channelCount,
        AudioCodecFormat audioFormat, size_t &frameCount);

    /**
     * Obtains the frame count (in BytesPerSample) required in the current conditions.
     *
     * @return Returns the frame count (in BytesPerSample); returns {@code -1} if an exception occurs.
     */
    uint64_t GetFrameCount() override;

    /**
    * Initailizes the audio source according to a specific configuration.
    *
    * @param config a configuration of audio source.
    * @return Returns SUCCESS if success, other values otherwise.
    */
    int32_t Initialize(const AudioSourceConfig &config) override;

    /**
    * Sets input device's identity when switching device.
    *
    * @param deviceId indentity to set.
    * @return Returns SUCCESS if set successfully, other values otherwise.
    */
    int32_t SetInputDevice(uint32_t deviceId) override;

    /**
    * Gets current device's identity.
    *
    * @param deviceId holds the identity of current device, if success.
    * @return Returns SUCCESS if success, other values otherwise.
    */
    int32_t GetCurrentDeviceId(uint32_t &deviceId) override;

    /**
    * Starts audio source.
    *
    * @return Returns SUCCESS if success, other values otherwise.
    */
    int32_t Start() override;

    /**
    *
    * Reads frame from source.
    *
    * @param frame, the buffer to storage the info of frame that read from source.
    * @param isBlockingRead reading mode.
    * @return Returns size of data actually read.
    */
    int32_t ReadFrame(AudioFrame &frame, bool isBlockingRead) override;

    /**
    * Stops audio source.
    *
    * @return Returns SUCCESS if success, other values otherwise.
    */
    int32_t Stop() override;

    /**
    * release.
    */
    int32_t Release() override;

private:
    int32_t InitCheck();

private:
    bool initialized_;
    bool started_;
    std::shared_ptr<AudioStreamIn> streamIn_;
    AudioManager& amIntance_ = AudioManager::GetInstance();
};
}  // namespace Audio
}  // namespace OHOS
#endif  // AUDIO_SOURCE_H_

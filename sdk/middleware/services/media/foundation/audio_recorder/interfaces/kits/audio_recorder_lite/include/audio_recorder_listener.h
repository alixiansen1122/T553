/*
* Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
* Description: audio recorder listener
* Author: Media Software Group
* Create: 2025-09-20
*/

#ifndef AUDIO_RECORDER_LISTENER_H
#define AUDIO_RECORDER_LISTENER_H

#include <string>
#include "jsi.h"

namespace OHOS {
namespace ACELite {

class AudioRecorderListener {
public:
    AudioRecorderListener() = default;

    explicit AudioRecorderListener(JSIValue callback, JSIValue thisVal);

    ~AudioRecorderListener();

    void OnTrigger() const;

private:
    JSIValue callback_;
    JSIValue thisVal_;
};

void TriggerRecorderListener(const AudioRecorderListener *listener);
void AsyncExecuteRecorderCallback(void *arg);

} // namespace ACELite
} // namespace OHOS
#endif

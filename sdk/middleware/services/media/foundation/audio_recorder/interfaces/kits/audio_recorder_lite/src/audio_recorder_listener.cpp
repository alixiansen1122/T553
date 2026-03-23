/*
* Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
* Description: audio recorder listener
* Author: Media Software Group
* Create: 2025-09-20
*/

#include "audio_recorder_listener.h"
#include <cerrno>
#include <sys/prctl.h>
#include <sys/time.h>
#include <unistd.h>
#include "js_async_work.h"
#include "media_log.h"

namespace OHOS {
namespace ACELite {

void TriggerRecorderListener(const AudioRecorderListener *listener)
{
    if (listener == nullptr) {
        MEDIA_ERR_LOG("listener is NULL");
        return;
    }
    if (!JsAsyncWork::DispatchAsyncWork(AsyncExecuteRecorderCallback, const_cast<AudioRecorderListener *>(listener))) {
        MEDIA_ERR_LOG("dispatch async work failed.");
    }
}

void AsyncExecuteRecorderCallback(void *arg)
{
    AudioRecorderListener *listener = static_cast<AudioRecorderListener *>(arg);
    if (listener == nullptr) {
        MEDIA_ERR_LOG("async execute callback failed.");
        return;
    }
    listener->OnTrigger();
}

AudioRecorderListener::AudioRecorderListener(JSIValue callback, JSIValue thisVal)
    : callback_(JSI::AcquireValue(callback)),
    thisVal_(thisVal)
{}

AudioRecorderListener::~AudioRecorderListener()
{
    if (!JSI::ValueIsUndefined(callback_)) {
        JSI::ReleaseValue(callback_);
        callback_ = JSI::CreateUndefined();
    }
}

void AudioRecorderListener::OnTrigger() const
{
    if (JSI::ValueIsFunction(callback_)) {
        JSI::CallFunction(callback_, thisVal_, nullptr, 0);
        MEDIA_INFO_LOG("Listener OnTrigger.");
    }
}
}  // namespace ACELite
}  // namespace OHOS

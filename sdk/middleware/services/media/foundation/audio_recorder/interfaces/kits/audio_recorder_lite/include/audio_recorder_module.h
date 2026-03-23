/*
* Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
* Description: audio recorder module
* Author: Media Software Group
* Create: 2025-09-20
*/

#ifndef AUDIO_RECORDER_MODULE_H
#define AUDIO_RECORDER_MODULE_H

#include <cstdint>
#include <string>
#include <memory>
#include "jsi.h"
#include "audio_recorder_listener.h"
#include "audio_manager.h"

namespace OHOS {
namespace ACELite {
class AudioRecorderModule final : public MemoryHeap {
public:
    AudioRecorderModule();
    ~AudioRecorderModule();
    static JSIValue Prepare(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);

    static JSIValue Start(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);

    static JSIValue Pause(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);

    static JSIValue Resume(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);

    static JSIValue Stop(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);

    static JSIValue Release(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);

    static JSIValue RegCallBack(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static void OnTerminate();
private:
    static int32_t ActivateAudioInterrupt(uint32_t &sessionId);
    static int32_t AudioRecorderAllPrepare(void);
    static int32_t AudioRecorderGetInfo(CapturerInputConfig &capturerInfo, std::string &uri,
        const JSIValue *args, uint8_t argsSize);
    static void AudioRecordEventHandle(uint32_t enEvent, const void *data, const void *cookie);
    static JSIValue OnPrepareSetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue OnStartSetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue OnPauseSetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue OnResumeSetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue OnStopSetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue OnReleaseSetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static JSIValue OnErrorSetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
    static AudioRecorderListener *CreateAudioRecorderListener(const JSIValue thisVal,
        const JSIValue *args, uint8_t argsSize);
    static int32_t AudioCaptureGetFormat(std::string format, AudioCodecFormat &audioFormat);
    static AudioRecorderListener *onPrepareListener_;
    static AudioRecorderListener *onStartedListener_;
    static AudioRecorderListener *onPauseListener_;
    static AudioRecorderListener *onResumeListener_;
    static AudioRecorderListener *onStopListener_;
    static AudioRecorderListener *onReleaseListener_;
    static AudioRecorderListener *onErrorListener_;
    static void *recorderHandle_;
    static Audio::AudioInterrupt interrupt_;
};

void InitAudioRecorderModule(JSIValue exports);
}
}


#endif

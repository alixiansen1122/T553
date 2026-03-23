/*
* Copyright (c) CompanyNameMagicTag. 2021-2021. All rights reserved.
* Description: audio capture wrapper
* Author: Media Software Group
* Create: 2021-09-13
*/

#ifndef AUDIOCAPTURER_MODULE_H
#define AUDIOCAPTURER_MODULE_H

#include <cstdint>
#include "jsi.h"

#ifdef __cplusplus
extern "C" {
#endif

int32_t AudioCaptureSample(int32_t argc, const char *argv[]);


#ifdef __cplusplus
};
#endif

namespace OHOS {
namespace ACELite {
class AudioCapturerModule final : public MemoryHeap {
public:
    /**
     * constructor
     */
    AudioCapturerModule() = default;

    /**
     * desconstructor
     */
    ~AudioCapturerModule() = default;
    /**
     * Start
     */
    static JSIValue Start(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);

    /**
     * Stop
     */
    static JSIValue Stop(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);

    /**
     * Start
     * Start XiaoduVoice when args[0] == "XiaoduVoice", no input param
     * Start AudioCaptruer otherwise
     * @inputparam args, include properties buffersize, format, etc
     */
    static JSIValue start(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);

    /**
     * Stop
     */
    static JSIValue stop(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);

    /**
     * ListenerOn
     * @inputparam args, include properties success, fail
     */
    static JSIValue ListenerOn(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);

    /**
     * Cancel
     */
    static JSIValue Cancel(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);
	/**
     * ListenerOff
     */
    static JSIValue ListenerOff(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);

    /**
     handle exception
     */
    static void ExceptionStop(void);
    static void OnDestroy(void);
    static void OnTerminate(void);
};

void InitAudioCapturerModule(JSIValue exports);
}
}


#endif

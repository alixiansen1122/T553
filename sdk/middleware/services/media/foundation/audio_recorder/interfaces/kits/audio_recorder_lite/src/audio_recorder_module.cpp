/*
* Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
* Description: audio recorder module
* Author: Media Software Group
* Create: 2025-09-25
*/

#include "audio_recorder_module.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <climits>
#include "securec.h"
#include "audio_manager.h"
#include "media_log.h"
#include "media_thread_adapt.h"
#include "audio_manager.h"
#include "audio_recorder.h"
#include "nativeapi_fs.h"

#define DELETE_NOT_NULL(pointer) \
    if ((pointer) != nullptr) {  \
        delete (pointer);        \
        (pointer) = nullptr;     \
    }
using namespace::Audio;
using Audio::AudioManager;
namespace OHOS {
namespace ACELite {

 AudioRecorderListener *AudioRecorderModule::onPrepareListener_ = nullptr;
 AudioRecorderListener *AudioRecorderModule::onStartedListener_ = nullptr;
 AudioRecorderListener *AudioRecorderModule::onPauseListener_ = nullptr;
 AudioRecorderListener *AudioRecorderModule::onResumeListener_ = nullptr;
 AudioRecorderListener *AudioRecorderModule::onStopListener_ = nullptr;
 AudioRecorderListener *AudioRecorderModule::onReleaseListener_ = nullptr;
 AudioRecorderListener *AudioRecorderModule::onErrorListener_ = nullptr;
 void *AudioRecorderModule::recorderHandle_ = nullptr;
 Audio::AudioInterrupt AudioRecorderModule::interrupt_ = {};

 const uint64_t PER_FILE_STORAGE_MAX_DURATION = 60;
 const uint64_t DEFAULTE_MAX_STORAGE_THEARHOLD = 90;
 const uint64_t PER_FILE_STORAGE_MAX_SIZE = 2ULL * 1024 * 1024 * 1024;
 const uint8_t INDEX_0 = 0;
 const uint8_t INDEX_1 = 1;
 const uint8_t INDEX_2 = 2;
 const uint8_t INDEX_3 = 3;
 const uint8_t INDEX_4 = 4;
 const uint8_t PARAM_NUM_2 = 2;
 constexpr char AUDIO_ENCODE_BIT_RATE[] = "audioEncodeBitRate";
 constexpr char AUDIO_SAMPLE_RATE[] = "audioSampleRate";
 constexpr char NUMBER_OF_CHANNELS[] = "numberOfChannels";
 constexpr char INPUT_URI[] = "uri";
 constexpr char AUDIO_ENCODER_MIME[] = "audioEncoderMime";
 constexpr char FILE_FORMAT[] = "fileFormat";
 static Audio::AudioManager &g_amInstance = Audio::AudioManager::GetInstance();

 struct NameAudioFormat {
     std::string name;
     AudioCodecFormat audioFormat;
 };

 static NameAudioFormat g_audioCodecFormat[] = {
    {"audio/mp3", MP3},
    {"audio/opus", OPUS},
    {"audio/silk", SILK},
    {"audio/pcm", PCM}
 };

 int32_t AudioRecorderModule::AudioCaptureGetFormat(std::string format, AudioCodecFormat &audioFormat)
 {
     for (size_t i = 0; i < (sizeof(g_audioCodecFormat) / sizeof(g_audioCodecFormat[0])); i++) {
         if (format == g_audioCodecFormat[i].name) {
             audioFormat = g_audioCodecFormat[i].audioFormat;
             MEDIA_INFO_LOG("record [AudioCaptureSample] set audioFormat %s", g_audioCodecFormat[i].name.c_str());
             return MEDIA_OK;
         }
     }
     MEDIA_ERR_LOG("invalid input format[%s]!", format.c_str());
     return MEDIA_INVALID_PARAM;
 }

 class AudioRecorderInterruptListener : public InterruptListener {
     public:
     AudioRecorderInterruptListener() {};
     ~AudioRecorderInterruptListener() override {};

     void OnInterrupt(int32_t type, int32_t hint) override
     {
         MEDIA_INFO_LOG("AudioRecorderInterruptListener OnInterrupt, type:%d, hint:%d", type, hint);
         if (type == INTERRUPT_TYPE_BEGIN && hint == INTERRUPT_HINT_PAUSE) {
             MEDIA_INFO_LOG("OnInterrupt pause not supported");
         }
         if (type == INTERRUPT_TYPE_END && hint == INTERRUPT_HINT_RESUME) {
             MEDIA_INFO_LOG("OnInterrupt resume not supported");
         }
         if (type == INTERRUPT_TYPE_BEGIN && hint == INTERRUPT_HINT_STOP) {
             AudioRecorderModule::Stop(nullptr, nullptr, 0);
             AudioRecorderModule::Release(nullptr, nullptr, 0);
         }
         if (type == INTERRUPT_TYPE_END && hint == INTERRUPT_HINT_STOP) {
             MEDIA_INFO_LOG("OnInterrupt stop not supported");
         }
     }
 };

 AudioRecorderModule::AudioRecorderModule()
 {

 }

 AudioRecorderModule::~AudioRecorderModule()
 {
 }

JSIValue AudioRecorderModule::Prepare(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    CapturerInputConfig capturerInfo = {};
    std::string uri = "";
    errcode_t ret = ActivateAudioInterrupt(capturerInfo.sessionID);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("ActivateAudioInterrupt failed!");
        return JSI::CreateBoolean(false);
    }
    ret = AudioRecorderGetInfo(capturerInfo, uri, args, argsSize);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("AudioRecorderGetInfo failed!");
        goto DEACTIVATE;
    }
    ret = AudioRecorderCreate(&recorderHandle_);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("AudioRecorderCreate failed!");
        goto DESTORY;
    }
    ret = AudioRecorderSetInfo(recorderHandle_, &capturerInfo);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("audio recorder set info failed!");
        goto DESTORY;
    }
    ret = AudioRecorderSetOutputPath(recorderHandle_, uri.c_str(), uri.size() + 1);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("audio recorder set output path failed!");
        goto DESTORY;
    }
    ret = AudioRecorderAllPrepare();
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("audio recorder all prepare failed!");
        goto DESTORY;
    }
    ret = AudioRecorderSetCallback(recorderHandle_, AudioRecordEventHandle, NULL);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("audio recorder set max call back failed!");
        goto DESTORY;
    }
    TriggerRecorderListener(onPrepareListener_);
    return JSI::CreateBoolean(true);
DESTORY:
    AudioRecorderDestroy(recorderHandle_);
    recorderHandle_ = NULL;
DEACTIVATE:
    g_amInstance.DeactivateAudioInterrupt(interrupt_);
    return JSI::CreateBoolean(false);
}

JSIValue AudioRecorderModule::Start(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    errcode_t ret = AudioRecorderStart(recorderHandle_);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("audio recorder start failed!");
        return JSI::CreateBoolean(false);
    }
    TriggerRecorderListener(onStartedListener_);
    return JSI::CreateBoolean(true);
}

JSIValue AudioRecorderModule::Pause(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    MEDIA_UNUSED(thisVal);
    MEDIA_UNUSED(args);
    MEDIA_UNUSED(argsSize);
    MEDIA_ERR_LOG("Not support!");
    TriggerRecorderListener(onPauseListener_);
    return JSI::CreateBoolean(false);
}

JSIValue AudioRecorderModule::Resume(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    MEDIA_UNUSED(thisVal);
    MEDIA_UNUSED(args);
    MEDIA_UNUSED(argsSize);
    MEDIA_ERR_LOG("Not support!");
    TriggerRecorderListener(onResumeListener_);
    return JSI::CreateBoolean(false);
}

JSIValue AudioRecorderModule::Stop(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    MEDIA_UNUSED(thisVal);
    MEDIA_UNUSED(args);
    MEDIA_UNUSED(argsSize);
    errcode_t ret = AudioRecorderStop(recorderHandle_);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("audio recorder stop failed!");
        return JSI::CreateBoolean(false);
    }
    TriggerRecorderListener(onStopListener_);
    return JSI::CreateBoolean(true);
}

JSIValue AudioRecorderModule::Release(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    MEDIA_UNUSED(thisVal);
    MEDIA_UNUSED(args);
    MEDIA_UNUSED(argsSize);
    if (recorderHandle_ == nullptr) {
        return JSI::CreateBoolean(true);
    }
    (void)AudioRecorderDestroy(recorderHandle_);
    recorderHandle_ = nullptr;
    g_amInstance.DeactivateAudioInterrupt(interrupt_);
    TriggerRecorderListener(onReleaseListener_);
    return JSI::CreateBoolean(true);
}

JSIValue AudioRecorderModule::RegCallBack(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    if (argsSize < PARAM_NUM_2) {
        MEDIA_ERR_LOG("The number of parameters is less than 2!");
        return JSI::CreateBoolean(false);
    }
    std::string type = std::string(JSI::ValueToString(args[INDEX_0]));
    if (type == "prepare") {
        return OnPrepareSetter(thisVal, args, argsSize);
    } else if (type == "start") {
        return OnStartSetter(thisVal, args, argsSize);
    } else if (type == "pause") {
        return OnPauseSetter(thisVal, args, argsSize);
    } else if (type == "resume") {
        return OnResumeSetter(thisVal, args, argsSize);
    } else if (type == "stop") {
        return OnStopSetter(thisVal, args, argsSize);
    } else if (type == "release") {
        return OnReleaseSetter(thisVal, args, argsSize);
    } else if (type == "error") {
        return OnErrorSetter(thisVal, args, argsSize);
    }
    return JSI::CreateBoolean(true);
}

int32_t AudioRecorderModule::AudioRecorderAllPrepare(void)
{
    errcode_t ret = AudioRecorderPrepare(recorderHandle_);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("audio recorder prepare failed!");
        return ret;
    }
    ret = AudioRecorderEnableOverwriteFiles(recorderHandle_, false);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("audio recorder enable over write files failed!");
        return ret;
    }
    ret = AudioRecorderSetStorageWarningThreshold(recorderHandle_, DEFAULTE_MAX_STORAGE_THEARHOLD);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("audio recorder set storage warning threshold failed!");
        return ret;
    }
    ret = AudioRecorderSetMaxDuration(recorderHandle_, PER_FILE_STORAGE_MAX_DURATION);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("audio recorder set max duration failed!");
        return ret;
    }
    ret = AudioRecorderSetMaxFileSize(recorderHandle_, PER_FILE_STORAGE_MAX_SIZE);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("audio recorder set max file size failed!");
        return ret;
    }
    return ret;
}

int32_t AudioRecorderModule::ActivateAudioInterrupt(uint32_t &sessionId)
{
    sessionId = g_amInstance.MakeSessionId();
    if (sessionId == AUDIO_SESSION_ID_NONE) {
        MEDIA_ERR_LOG("AUDIO_SESSION_ID_NONE");
        return MEDIA_ERR;
    }
    std::shared_ptr<AudioRecorderInterruptListener> recorderInterruptListener =
        std::make_shared<AudioRecorderInterruptListener>();
    interrupt_ = { AUDIO_STREAM_VOICE_RECORD, sessionId, recorderInterruptListener };
    int32_t ret = g_amInstance.ActivateAudioInterrupt(interrupt_);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("ActivateAudioInterrupt failed");
    }
    return ret;
}

int32_t AudioRecorderModule::AudioRecorderGetInfo(CapturerInputConfig &capturerInfo, std::string &uri,
    const JSIValue *args, uint8_t argsSize)
{
    if (argsSize < 1) {
        MEDIA_ERR_LOG("The number of parameters is less than 1!");
        return MEDIA_ERR;
    }
    int32_t bitRate = static_cast<int32_t>(JSI::ValueToNumber(JSI::GetNamedProperty(args[0], AUDIO_ENCODE_BIT_RATE)));
    capturerInfo.sampleRate = static_cast<int32_t>(JSI::ValueToNumber(JSI::GetNamedProperty(args[0], AUDIO_SAMPLE_RATE)));
    capturerInfo.channelCount = static_cast<int32_t>(JSI::ValueToNumber(JSI::GetNamedProperty(args[0], NUMBER_OF_CHANNELS)));
    uri = std::string(JSI::ValueToString(JSI::GetNamedProperty(args[0], INPUT_URI)));
    MEDIA_INFO_LOG("relative path = %s!", uri.c_str());
    char destFullPath[FILE_NAME_MAX_LEN + 1] = {};
    NativeapiFs::GetFullPath(uri.c_str(), destFullPath, FILE_NAME_MAX_LEN + 1);
    MEDIA_INFO_LOG("absolute path = %s!", destFullPath);
    uri = std::string(destFullPath);
    std::string format = std::string(JSI::ValueToString(JSI::GetNamedProperty(args[0], AUDIO_ENCODER_MIME)));
    int32_t ret = AudioCaptureGetFormat(format, capturerInfo.audioFormat);
    if (ret != MEDIA_OK) {
        return ret;
    }
    capturerInfo.bitWidth = BIT_WIDTH_16;
    capturerInfo.streamType = AUDIO_STREAM_VOICE_RECORD;
    capturerInfo.linkDir = AUDIO_UP_LINK;
    return MEDIA_OK;
}

void AudioRecorderModule::AudioRecordEventHandle(uint32_t enEvent, const void *data, const void *cookie)
{
    MEDIA_UNUSED(data);
    MEDIA_UNUSED(cookie);
    if (enEvent == AUDIO_RECORD_STORAGE_REACHE_MAX_VALUE ||
        enEvent == AUDIO_RECORD_EVENT_ERROR) {
        TriggerRecorderListener(onErrorListener_);
    } else {
        MEDIA_ERR_LOG("not support envent type!");
    }
}

JSIValue AudioRecorderModule::OnPrepareSetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    DELETE_NOT_NULL(onPrepareListener_);
    onPrepareListener_ = CreateAudioRecorderListener(thisVal, args, argsSize);
    if (onPrepareListener_ == nullptr) {
        return JSI::CreateBoolean(false);
    }
    return JSI::CreateBoolean(true);
}

JSIValue AudioRecorderModule::OnStartSetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    DELETE_NOT_NULL(onStartedListener_);
    onStartedListener_ = CreateAudioRecorderListener(thisVal, args, argsSize);
    if (onStartedListener_ == nullptr) {
        return JSI::CreateBoolean(false);
    }
    return JSI::CreateBoolean(true);
}

JSIValue AudioRecorderModule::OnPauseSetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    DELETE_NOT_NULL(onPauseListener_);
    onPauseListener_ = CreateAudioRecorderListener(thisVal, args, argsSize);
    if (onPauseListener_ == nullptr) {
        return JSI::CreateBoolean(false);
    }
    return JSI::CreateBoolean(true);
}

JSIValue AudioRecorderModule::OnResumeSetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    DELETE_NOT_NULL(onResumeListener_);
    onResumeListener_ = CreateAudioRecorderListener(thisVal, args, argsSize);
    if (onResumeListener_ == nullptr) {
        return JSI::CreateBoolean(false);
    }
    return JSI::CreateBoolean(true);
}

JSIValue AudioRecorderModule::OnStopSetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    DELETE_NOT_NULL(onStopListener_);
    onStopListener_ = CreateAudioRecorderListener(thisVal, args, argsSize);
    if (onStopListener_ == nullptr) {
        return JSI::CreateBoolean(false);
    }
    return JSI::CreateBoolean(true);
}

JSIValue AudioRecorderModule::OnReleaseSetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    DELETE_NOT_NULL(onReleaseListener_);
    onReleaseListener_ = CreateAudioRecorderListener(thisVal, args, argsSize);
    if (onReleaseListener_ == nullptr) {
        return JSI::CreateBoolean(false);
    }
    return JSI::CreateBoolean(true);
}

JSIValue AudioRecorderModule::OnErrorSetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    DELETE_NOT_NULL(onErrorListener_);
    onErrorListener_ = CreateAudioRecorderListener(thisVal, args, argsSize);
    if (onErrorListener_ == nullptr) {
        return JSI::CreateBoolean(false);
    }
    return JSI::CreateBoolean(true);
}

AudioRecorderListener *AudioRecorderModule::CreateAudioRecorderListener(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    if (argsSize < PARAM_NUM_2) {
        MEDIA_ERR_LOG("2 argument is required.");
        return nullptr;
    }
    JSIValue callback = args[INDEX_1];
    if (!JSI::ValueIsFunction(callback)) {
        MEDIA_ERR_LOG("a function is required.");
        return nullptr;
    }
    AudioRecorderListener *listener = new AudioRecorderListener(callback, thisVal);
    if (listener == nullptr) {
        MEDIA_ERR_LOG("out of memory.");
    }
    return listener;
}

void AudioRecorderModule::OnTerminate(void)
{
    AudioRecorderModule::Release(nullptr, nullptr, 0);
    DELETE_NOT_NULL(onPrepareListener_);
    DELETE_NOT_NULL(onStartedListener_);
    DELETE_NOT_NULL(onPauseListener_);
    DELETE_NOT_NULL(onStopListener_);
    DELETE_NOT_NULL(onReleaseListener_);
    DELETE_NOT_NULL(onErrorListener_);
}

void InitAudioRecorderModule(JSIValue exports)
{
    g_amInstance.Initialize();
    JSI::SetOnTerminate(exports, AudioRecorderModule::OnTerminate);
    JSI::SetModuleAPI(exports, "prepare", AudioRecorderModule::Prepare);
    JSI::SetModuleAPI(exports, "start", AudioRecorderModule::Start);
    JSI::SetModuleAPI(exports, "pause", AudioRecorderModule::Pause);
    JSI::SetModuleAPI(exports, "resume", AudioRecorderModule::Resume);
    JSI::SetModuleAPI(exports, "stop", AudioRecorderModule::Stop);
    JSI::SetModuleAPI(exports, "reset", AudioRecorderModule::Release);
    JSI::SetModuleAPI(exports, "release", AudioRecorderModule::Release);
    JSI::SetModuleAPI(exports, "on", AudioRecorderModule::RegCallBack);
}

}
}

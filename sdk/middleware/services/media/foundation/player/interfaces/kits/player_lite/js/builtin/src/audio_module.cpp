/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "audio_module.h"
#include "media_log.h"
#include "js_async_work.h"
#ifdef HMF_DECRYPT_DATA_ENABLE
#include "js_ability.h"
#include "audio_decrypt.h"
#endif
namespace OHOS {
namespace ACELite {
constexpr char PROP_SUCCESS[] = "success";
constexpr char PROP_FAIL[] = "fail";
constexpr char PROP_COMPLETE[] = "complete";
constexpr char ALBUMINFO_SOURCE[] = "src";
constexpr char ALBUMINFO_TITLE[] = "title";
constexpr char ALBUMINFO_ARTIST[] = "artist";
constexpr char DECRYPT_FILE_PATH[] = "/user/app/user/ace/data/%s/rawfile/lib/audio_decode.bin";
constexpr uint32_t MAX_PATH_LEN = 256;
constexpr char PATH_PREFIX[] = "internal://";
constexpr char PATH_SANDBOX_KEYWORD[] = "/assets/entry/";

static Audio::AudioManager& g_amInstance = Audio::AudioManager::GetInstance();

static AudioStreamType g_streamType = AUDIO_STREAM_MUSIC;

void AudioModule::SuccessCallBack(const JSIValue thisVal, const JSIValue args, JSIValue outputValue)
{
    if (JSI::ValueIsUndefined(args)) {
        return;
    }
    JSIValue successCB = JSI::GetNamedProperty(args, PROP_SUCCESS);
    if (!JSI::ValueIsUndefined(successCB)) {
        if (!JSI::ValueIsUndefined(outputValue)) {
            JSI::CallFunction(successCB, thisVal, &outputValue, 1);
        } else {
            JSI::CallFunction(successCB, thisVal, nullptr, 1);
        }
    }
    JSI::ReleaseValue(successCB);
}

void AudioModule::FailCallBack(const JSIValue thisVal, const JSIValue args, const char *errStr)
{
    if (JSI::ValueIsUndefined(args)) {
        return;
    }
    JSIValue failCB = JSI::GetNamedProperty(args, PROP_FAIL);
    if (!JSI::ValueIsUndefined(failCB)) {
        JSIValue errLog = JSI::CreateString(errStr);
        JSI::CallFunction(failCB, thisVal, &errLog, 1);
        JSI::ReleaseValue(errLog);
    }
    JSI::ReleaseValue(failCB);
}

void AudioModule::OnTerminate()
{
    AudioPlayer *audioPlayer = AudioPlayer::GetInstance();
    if (audioPlayer == nullptr) {
        MEDIA_ERR_LOG("audio player is null");
        return;
    }
    audioPlayer->ReleaseEventListeners();
    audioPlayer->StopUpdateTimeThread();
}

void AudioModule::DefineProperty(JSIValue target,
                                 const char *propName,
                                 JSIFunctionHandler getter,
                                 JSIFunctionHandler setter)
{
    JSPropertyDescriptor descriptor;
    descriptor.getter = getter;
    descriptor.setter = setter;
    JSI::DefineNamedProperty(target, propName, descriptor);
}

JSIValue AudioModule::Play(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    MEDIA_UNUSED(thisVal);
    MEDIA_UNUSED(args);
    MEDIA_UNUSED(argsSize);
    return JSI::CreateBoolean(AudioPlayer::GetInstance()->Play());
}

JSIValue AudioModule::Pause(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    MEDIA_UNUSED(thisVal);
    MEDIA_UNUSED(args);
    MEDIA_UNUSED(argsSize);
    return JSI::CreateBoolean(AudioPlayer::GetInstance()->Pause());
}

JSIValue AudioModule::Stop(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    MEDIA_UNUSED(thisVal);
    MEDIA_UNUSED(args);
    MEDIA_UNUSED(argsSize);
    return JSI::CreateBoolean(AudioPlayer::GetInstance()->Stop());
}

JSIValue AudioModule::Reset(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    MEDIA_UNUSED(thisVal);
    MEDIA_UNUSED(args);
    MEDIA_UNUSED(argsSize);
    return JSI::CreateBoolean(AudioPlayer::GetInstance()->ResetPlayer());
}

JSIValue AudioModule::GetPlayState(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    if (argsSize < 1) {
        MEDIA_ERR_LOG("1 argument is required.");
        return JSI::CreateBoolean(false);
    }

    JSIValue options = args[0];
    if (!JSI::ValueIsObject(options)) {
        MEDIA_ERR_LOG("invalid parameter.");
        return JSI::CreateBoolean(false);
    }

    JSIValue successCallback = JSI::GetNamedProperty(options, PROP_SUCCESS);
    if (!JSI::ValueIsUndefined(successCallback)) {
        AudioPlayer *audioPlayer = AudioPlayer::GetInstance();
        JSIValue data = JSI::CreateObject();
        JSI::SetStringProperty(data, "status", audioPlayer->GetStatus());
        JSI::SetStringProperty(data, "src", audioPlayer->GetSrc(false));
        JSI::SetStringProperty(data, "srcInner", audioPlayer->GetSrc(true));
        JSI::SetNumberProperty(data, "currentTime", audioPlayer->GetCurrentTime());
        JSI::SetBooleanProperty(data, "autoplay", audioPlayer->GetAutoPlay());
        JSI::SetBooleanProperty(data, "loop", audioPlayer->IsLooping());
        JSI::SetNumberProperty(data, "volume", audioPlayer->GetVolume());
        JSI::SetBooleanProperty(data, "muted", audioPlayer->IsMuted());
        JSIValue params[1] = { data };
        JSI::CallFunction(successCallback, thisVal, params, 1);
        JSI::ReleaseValueList(successCallback, data);
    }

    JSIValue completeCallback = JSI::GetNamedProperty(options, PROP_COMPLETE);
    if (!JSI::ValueIsUndefined(completeCallback)) {
        JSI::CallFunction(completeCallback, thisVal, nullptr, 0);
        JSI::ReleaseValue(completeCallback);
    }

    return JSI::CreateUndefined();
}

JSIValue AudioModule::SrcGetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    MEDIA_UNUSED(thisVal);
    MEDIA_UNUSED(args);
    MEDIA_UNUSED(argsSize);
    char *src;
    if (!AudioPlayer::GetInstance()->IsSysSrc()) {
        char *sysSrc = AudioPlayer::GetInstance()->GetSrc(true);
        std::string sysPath(sysSrc);
        int size = sysPath.find(PATH_SANDBOX_KEYWORD) + strlen(PATH_SANDBOX_KEYWORD);
        std::string path = sysPath.substr(size);
        path.insert(0, PATH_PREFIX);
        src = const_cast<char *>(path.c_str());
    } else {
        src = AudioPlayer::GetInstance()->GetSrc(false);
    }
    return (src == nullptr) ? JSI::CreateUndefined() : JSI::CreateString(src);
}

JSIValue AudioModule::SrcInnerGetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    MEDIA_UNUSED(thisVal);
    MEDIA_UNUSED(args);
    MEDIA_UNUSED(argsSize);
    char *src = AudioPlayer::GetInstance()->GetSrc(true);
    return (src == nullptr) ? JSI::CreateUndefined() : JSI::CreateString(src);
}

JSIValue AudioModule::SrcSetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    MEDIA_UNUSED(thisVal);
    if (argsSize < 1) {
        MEDIA_ERR_LOG("1 argument is required.");
        return JSI::CreateBoolean(false);
    }
    char *src = JSI::ValueToString(args[0]);
    bool autoPlay = AudioPlayer::GetInstance()->GetAutoPlay();
    bool ret = false;
    int size = strlen(PATH_PREFIX);
    std::string strValue(src);
    if (strValue.find(PATH_PREFIX) == 0) {
        std::string strNew = strValue.substr(size);
        char *innerPath = const_cast<char *>(strNew.c_str());
        ret = AudioPlayer::GetInstance()->SetSrc(innerPath, true);
        delete src;
    } else {
        ret = AudioPlayer::GetInstance()->SetSrc(src, false);
    }

    AudioPlayer::GetInstance()->SetStreamType(g_streamType);
    if (autoPlay) {
        AudioPlayer::GetInstance()->Play();
    }
    return JSI::CreateBoolean(ret);
}

JSIValue AudioModule::SrcInnerSetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    MEDIA_UNUSED(thisVal);
    if (argsSize < 1) {
        MEDIA_ERR_LOG("1 argument is required.");
        return JSI::CreateBoolean(false);
    }
    char *src = JSI::ValueToString(args[0]);
    bool autoPlay = AudioPlayer::GetInstance()->GetAutoPlay();
    bool ret = AudioPlayer::GetInstance()->SetSrc(src, true);
    AudioPlayer::GetInstance()->SetStreamType(g_streamType);
    if (autoPlay) {
        AudioPlayer::GetInstance()->Play();
    }
    return JSI::CreateBoolean(ret);
}

JSIValue AudioModule::SetSrcList(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    MEDIA_UNUSED(thisVal);
    if (argsSize < 1 || args == nullptr) {
        MEDIA_ERR_LOG("1 argument is required.");
        return JSI::CreateBoolean(false);
    }
    JSIValue srcListArray = JSI::AcquireValue(args[0]);
    if (!JSI::ValueIsArray(srcListArray)) {
        JSI::ReleaseValue(srcListArray);
        MEDIA_ERR_LOG("src List is required.");
        return JSI::CreateBoolean(false);
    }
    std::vector<std::string> srcList;
    uint32_t srcListLen = JSI::GetArrayLength(srcListArray);
    for (uint32_t index = 0; index < srcListLen; index++) {
        JSIValue src = JSI::GetPropertyByIndex(srcListArray, index);
        std::string strValue = JSI::JSIValueToString(src);
        if (strValue.find(std::string(PATH_PREFIX)) == 0) {
            std::string strNew = strValue.substr(std::string(PATH_PREFIX).size());
            char absPath[MAX_PATH_LEN] = {};
            if (AudioPlayer::GetInstance()->GetSrcInnerPrefix(strNew.c_str(), absPath) != 0) {
                MEDIA_ERR_LOG("fail to get absolute path of srcInner.");
                return JSI::CreateBoolean(false);
            }
            srcList.push_back(std::string(absPath));
        } else {
            srcList.push_back(strValue);
        }
    }
    bool ret = AudioPlayer::GetInstance()->SetSrcList(srcList);
    return JSI::CreateBoolean(ret);
}

bool AudioModule::CheckInputParam(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    if (argsSize < 1 || args == nullptr) {
        MEDIA_ERR_LOG("invalid input parameter.");
        return false;
    }
    if (!JSI::ValueIsObject(args[0])) {
        MEDIA_ERR_LOG("invalid input parameter.");
        return false;
    }
    return true;
}

JSIValue AudioModule::GetAlbumInfo(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    if (!CheckInputParam(thisVal, args, argsSize)) {
         return JSI::CreateBoolean(false);
    }
    JSIValue srcListArray = JSI::AcquireValue(args[1]);
    if (!JSI::ValueIsArray(srcListArray)) {
        FailCallBack(thisVal, args[0], "src List is required.");
        JSI::ReleaseValue(srcListArray);
        return JSI::CreateBoolean(false);
    }
    uint32_t srcListLen = JSI::GetArrayLength(srcListArray);
    JSIValue params = JSI::CreateArray(srcListLen);
    if (!JSI::ValueIsArray(params)) {
        FailCallBack(thisVal, args[0], "get output params failed.");
        JSI::ReleaseValue(params);
        return JSI::CreateBoolean(false);
    }
    for (uint32_t index = 0; index < srcListLen; ++index) {
        AudioPlayerAlbumInfo albumInfo = {"", "", ""};
        JSIValue src = JSI::GetPropertyByIndex(srcListArray, index);
        char *strValue = JSI::JSIValueToString(src);
        JSIValue data = JSI::CreateObject();
        AudioPlayer::GetInstance()->GetAlbumInfo(albumInfo, strValue);
        JSI::SetStringProperty(data, ALBUMINFO_SOURCE, albumInfo.src.c_str());
        JSI::SetStringProperty(data, ALBUMINFO_TITLE, albumInfo.title.c_str());
        JSI::SetStringProperty(data, ALBUMINFO_ARTIST, albumInfo.artist.c_str());
        JSI::SetPropertyByIndex(params, index, data);
        JSI::ReleaseValue(data);
        JSI::ReleaseString(strValue);
    }
    SuccessCallBack(thisVal, args[0], params);
    JSI::ReleaseValueList(params);
    return JSI::CreateBoolean(true);
}

JSIValue AudioModule::CurrentTimeGetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    MEDIA_UNUSED(thisVal);
    MEDIA_UNUSED(args);
    MEDIA_UNUSED(argsSize);
    return JSI::CreateNumber(AudioPlayer::GetInstance()->GetCurrentTime());
}

JSIValue AudioModule::CurrentTimeSetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    MEDIA_UNUSED(thisVal);
    if (argsSize < 1) {
        MEDIA_ERR_LOG("1 argument is required.");
        return JSI::CreateBoolean(false);
    }

    int64_t currentTime = static_cast<int64_t>(JSI::ValueToNumber(args[0]));
    return JSI::CreateBoolean(AudioPlayer::GetInstance()->SetCurrentTime(currentTime));
}

JSIValue AudioModule::DurationGetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    MEDIA_UNUSED(thisVal);
    MEDIA_UNUSED(args);
    MEDIA_UNUSED(argsSize);
    int64_t duation = AudioPlayer::GetInstance()->GetDuration();
    return duation == -1 ? JSI::CreateNumberNaN() : JSI::CreateNumber(static_cast<double>(duation));
}

JSIValue AudioModule::DurationSetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    MEDIA_UNUSED(thisVal);
    MEDIA_UNUSED(args);
    MEDIA_UNUSED(argsSize);
    return JSI::CreateError(JsiErrorType::JSI_ERROR_COMMON, "duration is readonly.");
}

JSIValue AudioModule::AutoPlayGetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    MEDIA_UNUSED(thisVal);
    MEDIA_UNUSED(args);
    MEDIA_UNUSED(argsSize);
    return JSI::CreateBoolean(AudioPlayer::GetInstance()->GetAutoPlay());
}

JSIValue AudioModule::AutoPlaySetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    MEDIA_UNUSED(thisVal);
    if (argsSize < 1) {
        MEDIA_ERR_LOG("1 argument is required.");
        return JSI::CreateBoolean(false);
    }
    bool autoPlay = JSI::ValueToBoolean(args[0]);
    return JSI::CreateBoolean(AudioPlayer::GetInstance()->SetAutoPlay(autoPlay));
}

JSIValue AudioModule::LoopGetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    MEDIA_UNUSED(thisVal);
    MEDIA_UNUSED(args);
    MEDIA_UNUSED(argsSize);
    return JSI::CreateBoolean(AudioPlayer::GetInstance()->IsLooping());
}

JSIValue AudioModule::LoopSetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    MEDIA_UNUSED(thisVal);
    if (argsSize < 1) {
        MEDIA_ERR_LOG("1 argument is required.");
        return JSI::CreateBoolean(false);
    }

    bool loop = JSI::ValueToBoolean(args[0]);
    return JSI::CreateBoolean(AudioPlayer::GetInstance()->SetLoop(loop));
}

JSIValue AudioModule::VolumeGetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    MEDIA_UNUSED(thisVal);
    MEDIA_UNUSED(args);
    MEDIA_UNUSED(argsSize);
    return JSI::CreateNumber(AudioPlayer::GetInstance()->GetVolume());
}

JSIValue AudioModule::VolumeSetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    MEDIA_UNUSED(thisVal);
    if (argsSize < 1) {
        MEDIA_ERR_LOG("1 argument is required.");
        return JSI::CreateBoolean(false);
    }
    float volume = static_cast<float>(JSI::ValueToNumber(args[0]));
    return JSI::CreateBoolean(AudioPlayer::GetInstance()->SetVolume(volume));
}

JSIValue AudioModule::MutedGetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    MEDIA_UNUSED(thisVal);
    MEDIA_UNUSED(args);
    MEDIA_UNUSED(argsSize);
    return JSI::CreateBoolean(AudioPlayer::GetInstance()->IsMuted());
}

JSIValue AudioModule::MutedSetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    MEDIA_UNUSED(thisVal);
    if (argsSize < 1) {
        MEDIA_ERR_LOG("1 argument is required.");
        return JSI::CreateBoolean(false);
    }
    bool muted = JSI::ValueToBoolean(args[0]);
    return JSI::CreateBoolean(AudioPlayer::GetInstance()->SetMuted(muted));
}

JSIValue AudioModule::OnPlayGetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    MEDIA_UNUSED(thisVal);
    MEDIA_UNUSED(args);
    MEDIA_UNUSED(argsSize);
    return GetEventCallback(AudioPlayer::GetInstance()->GetOnPlayListener());
}

JSIValue AudioModule::OnPlaySetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    MEDIA_UNUSED(thisVal);
    AudioEventListener *listener = CreateAudioEventListener(args, argsSize);
    if (listener == nullptr) {
        return JSI::CreateBoolean(false);
    }
    AudioPlayer::GetInstance()->SetOnPlayListener(listener);
    return JSI::CreateBoolean(true);
}

JSIValue AudioModule::OnPauseGetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    MEDIA_UNUSED(thisVal);
    MEDIA_UNUSED(args);
    MEDIA_UNUSED(argsSize);
    return GetEventCallback(AudioPlayer::GetInstance()->GetOnPauseListener());
}

JSIValue AudioModule::OnPauseSetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    MEDIA_UNUSED(thisVal);
    AudioEventListener *listener = CreateAudioEventListener(args, argsSize);
    if (listener == nullptr) {
        return JSI::CreateBoolean(false);
    }
    AudioPlayer::GetInstance()->SetOnPauseListener(listener);
    return JSI::CreateBoolean(true);
}

JSIValue AudioModule::OnStopGetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    MEDIA_UNUSED(thisVal);
    MEDIA_UNUSED(args);
    MEDIA_UNUSED(argsSize);
    return GetEventCallback(AudioPlayer::GetInstance()->GetOnStopListener());
}

JSIValue AudioModule::OnStopSetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    MEDIA_UNUSED(thisVal);
    AudioEventListener *listener = CreateAudioEventListener(args, argsSize);
    if (listener == nullptr) {
        return JSI::CreateBoolean(false);
    }
    AudioPlayer::GetInstance()->SetOnStopListener(listener);
    return JSI::CreateBoolean(true);
}

JSIValue AudioModule::OnLoadedDataGetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    MEDIA_UNUSED(thisVal);
    MEDIA_UNUSED(args);
    MEDIA_UNUSED(argsSize);
    return GetEventCallback(AudioPlayer::GetInstance()->GetOnLoadedDataListener());
}

JSIValue AudioModule::OnLoadedDataSetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    MEDIA_UNUSED(thisVal);
    AudioEventListener *listener = CreateAudioEventListener(args, argsSize);
    if (listener == nullptr) {
        return JSI::CreateBoolean(false);
    }
    AudioPlayer::GetInstance()->SetOnLoadedDataListener(listener);
    return JSI::CreateBoolean(true);
}

JSIValue AudioModule::OnEndedGetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    MEDIA_UNUSED(thisVal);
    MEDIA_UNUSED(args);
    MEDIA_UNUSED(argsSize);
    return GetEventCallback(AudioPlayer::GetInstance()->GetOnEndedListener());
}

JSIValue AudioModule::OnEndedSetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    MEDIA_UNUSED(thisVal);
    AudioEventListener *listener = CreateAudioEventListener(args, argsSize);
    if (listener == nullptr) {
        return JSI::CreateBoolean(false);
    }
    AudioPlayer::GetInstance()->SetOnEndedListener(listener);
    return JSI::CreateBoolean(true);
}

JSIValue AudioModule::OnErrorGetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    MEDIA_UNUSED(thisVal);
    MEDIA_UNUSED(args);
    MEDIA_UNUSED(argsSize);
    return GetEventCallback(AudioPlayer::GetInstance()->GetOnErrorListener());
}

JSIValue AudioModule::OnErrorSetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    MEDIA_UNUSED(thisVal);
    AudioEventListener *listener = CreateAudioEventListener(args, argsSize);
    if (listener == nullptr) {
        return JSI::CreateBoolean(false);
    }
    AudioPlayer::GetInstance()->SetOnErrorListener(listener);
    return JSI::CreateBoolean(true);
}

JSIValue AudioModule::OnTimeUpdateGetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    MEDIA_UNUSED(thisVal);
    MEDIA_UNUSED(args);
    MEDIA_UNUSED(argsSize);
    return GetEventCallback(AudioPlayer::GetInstance()->GetOnTimeUpdateListener());
}

JSIValue AudioModule::OnTimeUpdateSetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    MEDIA_UNUSED(thisVal);
    AudioEventListener *listener = CreateAudioEventListener(args, argsSize);
    if (listener == nullptr) {
        return JSI::CreateBoolean(false);
    }
    AudioPlayer::GetInstance()->SetOnTimeUpdateListener(listener);
    return JSI::CreateBoolean(true);
}

JSIValue AudioModule::StreamTypeGetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    MEDIA_UNUSED(thisVal);
    MEDIA_UNUSED(args);
    MEDIA_UNUSED(argsSize);
    return JSI::CreateNumber((double)g_streamType);
}

JSIValue AudioModule::StreamTypeSetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    if (argsSize < 1) {
        MEDIA_ERR_LOG("1 argument is required.");
        return JSI::CreateBoolean(false);
    }
    double streamType = JSI::ValueToNumber(args[0]);
    g_streamType = (AudioStreamType)streamType;
    AudioPlayer::GetInstance()->SetStreamType(g_streamType);
    return JSI::CreateBoolean(true);
}

JSIValue AudioModule::BackgroundPlaySetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    MEDIA_UNUSED(thisVal);
    if (argsSize < 1) {
        MEDIA_ERR_LOG("1 argument is required.");
        return JSI::CreateBoolean(false);
    }
    bool backgroundPlay = JSI::ValueToBoolean(args[0]);
    return JSI::CreateBoolean(AudioPlayer::GetInstance()->SetBackgroundPlay(backgroundPlay));
}

JSIValue AudioModule::BackgroundPlayGetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    MEDIA_UNUSED(thisVal);
    MEDIA_UNUSED(args);
    MEDIA_UNUSED(argsSize);
    return JSI::CreateBoolean(AudioPlayer::GetInstance()->IsBackgroundPlay());
}

JSIValue AudioModule::GetEventCallback(const AudioEventListener *listener)
{
    return (listener == nullptr) ? JSI::CreateUndefined() : listener->GetCallback();
}

AudioEventListener *AudioModule::CreateAudioEventListener(const JSIValue *args, uint8_t argsSize)
{
    if (argsSize < 1) {
        MEDIA_ERR_LOG("1 argument is required.");
        return nullptr;
    }

    JSIValue callback = args[0];
    if (!JSI::ValueIsFunction(callback)) {
        MEDIA_ERR_LOG("a function is required.");
        return nullptr;
    }

    AudioEventListener *listener = new AudioEventListener(callback);
    if (listener == nullptr) {
        MEDIA_ERR_LOG("out of memory.");
    }

    return listener;
}

static void InitAdioPlayer()
{
    g_amInstance.Initialize();

    AudioPlayer::GetInstance()->ForkUpdateTimeThread();
#ifdef HMF_DECRYPT_DATA_ENABLE
    const char *bundleName = OHOS::ACELite::JSAbility::GetPackageName();
    if (bundleName == nullptr) {
        MEDIA_ERR_LOG("get package name fail");
        return;
    }
    char decfilePath[MAX_PATH_LEN] = {0};
    int32_t ret = snprintf_s(decfilePath, sizeof(decfilePath), sizeof(decfilePath) - 1,
                             DECRYPT_FILE_PATH, bundleName);
    if (ret < 0) {
        MEDIA_ERR_LOG("format decrypt file path fail");
        return;
    }
    AudioPlayer::GetInstance()->SetDecryptLibraryPath(decfilePath, MAX_PATH_LEN);
#endif
}

void InitAudioModule(JSIValue exports)
{
    InitAdioPlayer();

    JSI::SetOnTerminate(exports, AudioModule::OnTerminate);
    JSI::SetModuleAPI(exports, "play", AudioModule::Play);
    JSI::SetModuleAPI(exports, "pause", AudioModule::Pause);
    JSI::SetModuleAPI(exports, "stop", AudioModule::Stop);
    JSI::SetModuleAPI(exports, "reset", AudioModule::Reset);
    JSI::SetModuleAPI(exports, "getPlayState", AudioModule::GetPlayState);
    JSI::SetModuleAPI(exports, "srcList", AudioModule::SetSrcList);
    JSI::SetModuleAPI(exports, "getAlbumInfo", AudioModule::GetAlbumInfo);

    AudioModule::DefineProperty(exports, "backgroundPlay", AudioModule::BackgroundPlayGetter, AudioModule::BackgroundPlaySetter);
    AudioModule::DefineProperty(exports, "streamType", AudioModule::StreamTypeGetter, AudioModule::StreamTypeSetter);
    AudioModule::DefineProperty(exports, "src", AudioModule::SrcGetter, AudioModule::SrcSetter);
    AudioModule::DefineProperty(exports, "srcInner", AudioModule::SrcInnerGetter, AudioModule::SrcInnerSetter);
    AudioModule::DefineProperty(exports, "currentTime", AudioModule::CurrentTimeGetter, AudioModule::CurrentTimeSetter);
    AudioModule::DefineProperty(exports, "duration", AudioModule::DurationGetter, AudioModule::DurationSetter);
    AudioModule::DefineProperty(exports, "autoplay", AudioModule::AutoPlayGetter, AudioModule::AutoPlaySetter);
    AudioModule::DefineProperty(exports, "loop", AudioModule::LoopGetter, AudioModule::LoopSetter);
    AudioModule::DefineProperty(exports, "volume", AudioModule::VolumeGetter, AudioModule::VolumeSetter);
    AudioModule::DefineProperty(exports, "muted", AudioModule::MutedGetter, AudioModule::MutedSetter);

    AudioModule::DefineProperty(exports, "onplay", AudioModule::OnPlayGetter, AudioModule::OnPlaySetter);
    AudioModule::DefineProperty(exports, "onpause", AudioModule::OnPauseGetter, AudioModule::OnPauseSetter);
    AudioModule::DefineProperty(exports, "onstop", AudioModule::OnStopGetter, AudioModule::OnStopSetter);
    AudioModule::DefineProperty(exports, "onloadeddata", AudioModule::OnLoadedDataGetter,
                                AudioModule::OnLoadedDataSetter);
    AudioModule::DefineProperty(exports, "onended", AudioModule::OnEndedGetter, AudioModule::OnEndedSetter);
    AudioModule::DefineProperty(exports, "onerror", AudioModule::OnErrorGetter, AudioModule::OnErrorSetter);
    AudioModule::DefineProperty(exports, "ontimeupdate", AudioModule::OnTimeUpdateGetter,
                                AudioModule::OnTimeUpdateSetter);
}
} // namespace ACELite
} // namespace OHOS

/*
* Copyright (c) CompanyNameMagicTag. 2021-2021. All rights reserved.
* Description: audio capture wrapper
* Author: Media Software Group
* Create: 2021-09-13
*/

#include "recording_module.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <climits>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifdef SUPPORT_SYS_PRCTL
#include <sys/prctl.h>
#endif
#include <string>
#include "securec.h"
#include "media_errors.h"
#include "audio_manager.h"
#include "media_log.h"
#include "media_thread_adapt.h"
#include "xiaodu_voice.h"
#include "msg_center_protocol.h"
#include "cmsis_os2.h"
#include "audio_capturer.h"
#include "js_async_work.h"

#ifdef __cplusplus
extern "C" {
#endif
using namespace OHOS::ACELite;
using namespace OHOS::Media;
using namespace OHOS::Audio;
using namespace::Audio;
using Audio::InterruptListener;
using Audio::AudioInterrupt;
using Audio::AudioManager;

const uint32_t DIRECTORY_PERMISSION = 0755;
const uint32_t PARAM_NUMBER = 6;
const uint32_t PLAYER_CONTROL_CMD_LEN = 20;
const uint32_t READ_RETRY_TIME_US = 10000;
const uint32_t READ_COUNT_MAX = 1000;
const uint32_t GET_AUDIO_TIME_INTERVAL_TIMES = 100;
const uint32_t MAX_ARGC = 5;
const uint32_t PATH_STR_SIZE = 64;
const uint32_t DEFAULT_SAMPLE_RATE = 16000;
const uint32_t DEFAULT_CHANNEL_CNT = 1;
const AudioBitWidth DEFAULT_BIT_WIDTH = BIT_WIDTH_16;
const uint32_t DEFAULT_SAMPLE_TIME = 10; // ms
const uint32_t SECOND_TO_MICROSECOND = 1000;
const uint32_t BYTE_SIZE = 8;
const uint32_t OPUS_HEAD_LEN = 8;
const uint32_t DEFAULT_BUFFER_SIZE = DEFAULT_SAMPLE_RATE *
                                    DEFAULT_BIT_WIDTH / BYTE_SIZE *
                                    DEFAULT_SAMPLE_TIME / SECOND_TO_MICROSECOND *
                                    DEFAULT_CHANNEL_CNT * 2; // equals 2 pcm frames size
const char XIAODU_VOICE[] = "XiaoduVoice";
const char AUDIO_CAPTURER[] = "AudioCapturer";
const char PROP_SUCCESS[] = "success";
const char PROP_FAIL[] = "fail";
const char PROP_BUFFER_SIZE[] = "framesize";
const char PROP_FORMAT[] = "format";
const char PROP_SAMPLE_RATE[] = "samplerate";
const char PROP_BITEWIDTH[] = "bitwidth";
const char PROP_CHANNEL_CNT[] = "channelcnt";

struct SampleAudioCapturerInfo {
    /** Audio source type */
    AudioSourceType inputSource = AUDIO_MIC;
    /** Audio codec format */
    AudioCodecFormat audioFormat = AUDIO_DEFAULT;
    /** Sampling rate */
    int32_t sampleRate = DEFAULT_SAMPLE_RATE;
    /** Bit rate */
    int32_t bitRate = 0;
    /** Number of audio channels */
    int32_t channelCount = DEFAULT_CHANNEL_CNT;
    /** Audio stream type */
    AudioStreamType streamType = TYPE_MEDIA;
    /** Bit width */
    AudioBitWidth bitWidth = DEFAULT_BIT_WIDTH;
    /** Session id */
    AudioSession sessionID = AUDIO_SESSION_ID_NONE;
};

struct NameAudioFormat {
    std::string name;
    AudioCodecFormat audioFormat;
};

typedef struct AudioSourceInput {
    size_t framesize;
    int32_t buffersize = DEFAULT_BUFFER_SIZE;
    uint8_t *buffer;
    uint8_t *storedBuffer;
    AudioCapturer *audioCap;
    AudioCodecFormat audioFormat;
#ifdef SAVE_LOCAL_FILE
    FILE *fd;
#endif
} AudioSourceInput;

struct JsCaptureParams {
    JSIValue thisVal = JSI::CreateUndefined();
    JSIValue successCb = JSI::CreateUndefined();
    JSIValue failCb = JSI::CreateUndefined();
};

typedef enum AudioCapturerState {
    OFF = 0,
    ON,
} AudioCapturerState;

typedef struct JsCapturerContext {
    char control[PLAYER_CONTROL_CMD_LEN];
    bool needStop;
    AudioCapturerState capturerState;
    AudioCapturerState listenerState;
    AudioInterrupt interrupt;
    MediaThreadIdHandle audioCaptureProcess;
} JsCapturerContext;

static NameAudioFormat g_audioCodecFormat[] = {
    {"mp3", MP3},
    {"ape", APE},
    {"msbc", mSBC},
    {"vorbis", VORBIS},
    {"opus", OPUS},
    {"flac", FLAC},
    {"g726", G726},
    {"g711u", G711U},
    {"g711a", G711A},
    {"aaceld", AAC_ELD},
    {"silk", SILK},
    {"pcm", PCM}
};

static bool g_interruptHintStop = false;
static std::shared_ptr<AudioCapturer> g_audioCap;
static MediaMutexHandle g_jsCaptureMutex = nullptr;
class CaptureInterruptListener : public InterruptListener {
public:
    CaptureInterruptListener() {};
    ~CaptureInterruptListener() override {};

    void OnInterrupt(int32_t type, int32_t hint) override
    {
        if (type == INTERRUPT_TYPE_BEGIN && hint == INTERRUPT_HINT_PAUSE) {
            MEDIA_INFO_LOG("record OnInterrupt pause not supported");
        }
        if (type == INTERRUPT_TYPE_END && hint == INTERRUPT_HINT_RESUME) {
            MEDIA_INFO_LOG("record OnInterrupt resume not supported");
        }
        if (type == INTERRUPT_TYPE_BEGIN && hint == INTERRUPT_HINT_STOP) {
            g_interruptHintStop = true;
        }
        if (type == INTERRUPT_TYPE_END && hint == INTERRUPT_HINT_STOP) {
            MEDIA_INFO_LOG("record OnInterrupt stop not supported");
        }
    }
};

static AudioSourceInput g_audioSourceProcessInput;
static AudioManager& g_amInstance = AudioManager::GetInstance();
static JsCapturerContext g_capturerCtx;
static std::shared_ptr<CaptureInterruptListener> g_captureInterruptListener;
static struct JsCaptureParams g_jsCaptureParams;

static void InitResource()
{
    g_jsCaptureParams.thisVal = JSI::CreateUndefined();
    g_jsCaptureParams.successCb = JSI::CreateUndefined();
    g_jsCaptureParams.failCb = JSI::CreateUndefined();
    g_jsCaptureMutex = MediaMutexCreate(nullptr);
}

static void DestroyResource()
{
    JSI::ReleaseValueList(g_jsCaptureParams.thisVal,
        g_jsCaptureParams.successCb,
        g_jsCaptureParams.failCb);
    g_jsCaptureParams.thisVal = JSI::CreateUndefined();
    g_jsCaptureParams.successCb = JSI::CreateUndefined();
    g_jsCaptureParams.failCb = JSI::CreateUndefined();
    MediaMutexDestroy(&g_jsCaptureMutex);
}

static void AudioCaptureExit()
{
    if (!g_audioCap->Stop()) {
        MEDIA_ERR_LOG("record Stop failed");
    }
    if (!g_audioCap->Release()) {
        MEDIA_ERR_LOG("record Release failed");
    }

    delete g_audioSourceProcessInput.buffer;
    g_audioSourceProcessInput.buffer = nullptr;

    if (g_amInstance.DeactivateAudioInterrupt(g_capturerCtx.interrupt) != 0) {
        MEDIA_ERR_LOG("record deactivate audio interrupt failed");
    }
    g_audioCap.reset();
    MEDIA_INFO_LOG("record AudioCaptureProcess end");
}

static void SuccessCallBackDispatch(void *arg)
{
    JSIValue *jsiValue = (JSIValue *)arg;
    if (JSI::ValueIsUndefined(g_jsCaptureParams.successCb)) {
        MEDIA_ERR_LOG("success cbk undefined!");
        return;
    }
    if (JSI::ValueIsUndefined(*jsiValue)) {
        JSI::CallFunction(g_jsCaptureParams.successCb, g_jsCaptureParams.thisVal, nullptr, 0);
    } else {
        JSI::CallFunction(g_jsCaptureParams.successCb, g_jsCaptureParams.thisVal, jsiValue, ARGC_ONE);
    }
}

static void SuccessCallBack(JSIValue *jsiValue)
{
    JsAsyncWork::DispatchAsyncWork(SuccessCallBackDispatch, static_cast<void *>(jsiValue));
}

static void FailCallBack(int ret, const char* errLog)
{
    JSIValue errCode = JSI::CreateNumber(ret);
    JSIValue errInfo = JSI::CreateString(errLog);
    JSIValue argv[ARGC_TWO] = {errInfo, errCode};
    if (!JSI::ValueIsUndefined(g_jsCaptureParams.failCb)) {
        JSI::CallFunction(g_jsCaptureParams.failCb, g_jsCaptureParams.thisVal, argv, ARGC_TWO);
    }
    JSI::ReleaseValueList(errInfo, errCode, ARGS_END);
}

static void* AudioCaptureProcess(void* arg)
{
    MEDIA_UNUSED(arg);
    int32_t readCnt = 0;
    Timestamp timeStamp;
    pcm_stream_queue_msg msg = {MSGCENTER_TYPE_ID_XIAODU_SEND_PCM_STREAM, nullptr, 0};
    while (!g_capturerCtx.needStop && !g_interruptHintStop) {
        int32_t ret = g_audioCap->Read(g_audioSourceProcessInput.buffer, g_audioSourceProcessInput.framesize, false);
        if (ret == -1) {
            MEDIA_ERR_LOG("audio recording failed:0x%x", ret);
            continue;
        }
        if (ret == ERR_RETRY_READ) {
            usleep(READ_RETRY_TIME_US);
            continue;
        }

        if (ret == 0) {
            continue;
        }
        msg.len = ret;
        msg.data = (uint8_t *)malloc(msg.len); /* 消费者释放内存 */
        if (msg.data == nullptr) {
            MEDIA_ERR_LOG("malloc fail. len = %d", msg.len);
            continue;
        }
        (void)memcpy_s(msg.data, msg.len, g_audioSourceProcessInput.buffer, msg.len);

        errcode_t res = osMessageQueuePut(GetPcmMsgQueueId(), &msg, 0, 0);
        if (res != osOK) {
            MEDIA_ERR_LOG("put pcm stream fail, len = %d, res = 0x%x", msg.len, res);
            continue;
        }

        readCnt++;
        if (readCnt % GET_AUDIO_TIME_INTERVAL_TIMES == 0) {
            if (g_audioCap->GetAudioTime(timeStamp, Timestamp::Timebase::MONOTONIC)) {
                MEDIA_INFO_LOG("read %d frames timestamp seconds:%ld, nanoseconds:%ld",
                    readCnt, timeStamp.time.tv_sec, timeStamp.time.tv_nsec);
            }
        }
        MEDIA_INFO_LOG("audioCap Read readCnt = %d, size = %d", readCnt, ret);
    }
    AudioCaptureExit();
    return nullptr;
}

static int32_t SendBuffer(int32_t &bufferOffset, const int32_t readLen)
{
    int32_t outDataSize = 0;
    outDataSize = bufferOffset >=  g_audioSourceProcessInput.buffersize ?
        g_audioSourceProcessInput.buffersize : bufferOffset;
    if (bufferOffset + readLen >= static_cast<int32_t>(g_audioSourceProcessInput.buffersize) ||
        g_capturerCtx.needStop) {
        uint8_t *arrayPtr = nullptr;
        JSIValue result = JSI::CreateObject();
        JSIValue arrayBuffer = JSI::CreateArrayBuffer(bufferOffset, arrayPtr);
        if (memcpy_s(arrayPtr, bufferOffset,
            g_audioSourceProcessInput.storedBuffer,
            bufferOffset) != 0) {
            JSI::ReleaseValueList(result, arrayBuffer);
            FailCallBack(MEDIA_ERR, "memcpy failed!");
            MEDIA_ERR_LOG("memcpy failed!");
            return MEDIA_ERR;
        }
        JSI::SetNumberProperty(result, "ret", MEDIA_OK);
        JSI::SetNumberProperty(result, "dataLen", bufferOffset);
        JSI::SetNamedProperty(result, "data", arrayBuffer);
        SuccessCallBack(&result);
        JSI::ReleaseValueList(result, arrayBuffer);
        if (memset_s(g_audioSourceProcessInput.storedBuffer, outDataSize, 0, outDataSize) != 0) {
            FailCallBack(MEDIA_ERR, "memset_s failed!");
            MEDIA_ERR_LOG("memset_s failed!");
            return MEDIA_ERR;
        }
        bufferOffset = 0;
    }
    int32_t readOffset = g_audioSourceProcessInput.audioFormat == OPUS ?
        OPUS_HEAD_LEN : 0;
    if (memcpy_s(g_audioSourceProcessInput.storedBuffer + bufferOffset,
        g_audioSourceProcessInput.buffersize - bufferOffset,
        g_audioSourceProcessInput.buffer + readOffset,
        readLen - readOffset) != 0) {
        FailCallBack(MEDIA_ERR, "memcpy failed!");
        MEDIA_ERR_LOG("memcpy failed!");
        return MEDIA_ERR;
    }
    bufferOffset += readLen;
    return MEDIA_OK;
}

static void* AudioStreamCaptureProcess(void* arg)
{
    int32_t bufferOffset = 0;
    g_audioSourceProcessInput.storedBuffer = new uint8_t[g_audioSourceProcessInput.buffersize];
    MEDIA_INFO_LOG("start audio capture");
    while (true) {
        MediaMutexLock(g_jsCaptureMutex);
        if (g_capturerCtx.needStop || g_interruptHintStop) {
            MediaMutexUnLock(g_jsCaptureMutex);
            break;
        }
        int32_t readLen = g_audioCap->Read(g_audioSourceProcessInput.buffer,
            g_audioSourceProcessInput.framesize, false);
        if (readLen <= 0) {
            usleep(READ_RETRY_TIME_US);
            MediaMutexUnLock(g_jsCaptureMutex);
            continue;
        }
#ifdef SAVE_LOCAL_FILE
        uint32_t writeLen = fwrite(g_audioSourceProcessInput.buffer, 1, readLen, g_audioSourceProcessInput.fd);
        if (writeLen != (uint32_t)readLen) {
            MEDIA_ERR_LOG("errno:%d, errmsg:%s", errno, strerror(errno));
            MediaMutexUnLock(g_jsCaptureMutex);
            break;
        }
#endif
        if (SendBuffer(bufferOffset, readLen) != MEDIA_OK) {
            MediaMutexUnLock(g_jsCaptureMutex);
            MEDIA_ERR_LOG("SendBuffer failed");
            break;
        }
        MediaMutexUnLock(g_jsCaptureMutex);
    }
    if (bufferOffset != 0) {
        MediaMutexLock(g_jsCaptureMutex);
        SendBuffer(bufferOffset, 0);
        MediaMutexUnLock(g_jsCaptureMutex);
    }
#ifdef SAVE_LOCAL_FILE
    fclose(g_audioSourceProcessInput.fd);
    g_audioSourceProcessInput.fd = nullptr;
#endif
    delete g_audioSourceProcessInput.storedBuffer;
    AudioCaptureExit();
    MEDIA_INFO_LOG("exit audio captrue");
    return nullptr;
}

static int32_t AudioCaptureGetFormat(const char *format, AudioCodecFormat &audioFormat)
{
    std::string inputFormat = format;
    for (size_t i = 0; i < (sizeof(g_audioCodecFormat) / sizeof(g_audioCodecFormat[0])); i++) {
        if (inputFormat == g_audioCodecFormat[i].name) {
            audioFormat = g_audioCodecFormat[i].audioFormat;
            MEDIA_INFO_LOG("record [AudioCaptureSample] set audioFormat %s", g_audioCodecFormat[i].name.c_str());
            return MEDIA_OK;
        }
    }
    MEDIA_ERR_LOG("invalid input format!");
    return MEDIA_INVALID_PARAM;
}

static int32_t AudioStreamCaptureGetInfo(const JSIValue* argv, SampleAudioCapturerInfo &audioCapturerInfo)
{
    JSIValue buffersize = JSI::GetNamedProperty(argv[0], PROP_BUFFER_SIZE);
    if (JSI::ValueIsUndefined(buffersize)) {
        FailCallBack(MEDIA_ERR, "invalid buffer size!");
        JSI::ReleaseValueList(buffersize);
        return MEDIA_ERR;
    }
    int32_t size = static_cast<int32_t>(JSI::ValueToNumber(buffersize));
    /* 1: buffersize */
    g_audioSourceProcessInput.buffersize = size < DEFAULT_BUFFER_SIZE ? DEFAULT_BUFFER_SIZE : size;
    MEDIA_INFO_LOG("record set capture parameter buffersize:%d", g_audioSourceProcessInput.buffersize);
    JSIValue format = JSI::GetNamedProperty(argv[0], PROP_FORMAT);
    if (!JSI::ValueIsUndefined(format)) {
        /* 2: audioFormat */
        if (AudioCaptureGetFormat(JSI::ValueToString(format), audioCapturerInfo.audioFormat) != MEDIA_OK) {
            FailCallBack(MEDIA_ERR, "invalld format!");
            JSI::ReleaseValueList(buffersize, format);
            return MEDIA_ERR;
        }
        MEDIA_INFO_LOG("record set capture parameter audioFormat:%d", audioCapturerInfo.audioFormat);
    }
    JSIValue sampleRate = JSI::GetNamedProperty(argv[0], PROP_SAMPLE_RATE);
    if (!JSI::ValueIsUndefined(sampleRate)) {
        /* 3: samplerate */
        audioCapturerInfo.sampleRate = static_cast<int32_t>(JSI::ValueToNumber(sampleRate));
        MEDIA_INFO_LOG("record set capture parameter sampleRate:%d", audioCapturerInfo.sampleRate);
    }
    JSIValue channelCount = JSI::GetNamedProperty(argv[0], PROP_CHANNEL_CNT);
    if (!JSI::ValueIsUndefined(channelCount)) {
        /* 4: channelCount */
        audioCapturerInfo.channelCount = static_cast<int32_t>(JSI::ValueToNumber(channelCount));
        MEDIA_INFO_LOG("record set capture parameter channelCount:%d", audioCapturerInfo.channelCount);
    }
    JSIValue bitWidth = JSI::GetNamedProperty(argv[0], PROP_BITEWIDTH);
    if (!JSI::ValueIsUndefined(bitWidth)) {
        /* 5: bitWidth */
        audioCapturerInfo.bitWidth = static_cast<AudioBitWidth>((AudioBitWidth)JSI::ValueToNumber(bitWidth));
        MEDIA_INFO_LOG("record set capture parameter bitWidth:%d", audioCapturerInfo.bitWidth);
    }
    JSI::ReleaseValueList(buffersize, format, sampleRate, channelCount, bitWidth);
    return MEDIA_OK;
}

static int32_t AudioCaptureGetInfo(const char **argv, SampleAudioCapturerInfo &audioCapturerInfo, std::string &filePath)
{
    AudioCaptureGetFormat(argv[0x1], audioCapturerInfo.audioFormat);
    audioCapturerInfo.sampleRate = atoi(argv[0x2]); /* 2: samplerate */
    MEDIA_INFO_LOG("record set capture parameter sampleRate:%d", audioCapturerInfo.sampleRate);
    audioCapturerInfo.channelCount = atoi(argv[0x3]); /* 3: channel */
    MEDIA_INFO_LOG("record set capture parameter channel:%d", audioCapturerInfo.channelCount);
    audioCapturerInfo.bitWidth = static_cast<AudioBitWidth>(atoi(argv[0x4])); /* 4: bitWidth */
    MEDIA_INFO_LOG("record set capture parameter bitWidth:%d", audioCapturerInfo.bitWidth);
    MEDIA_INFO_LOG("record input argv[5]:%s.", argv[5]); /* 5: filePath */

    if (std::strlen(argv[0x5]) < PATH_MAX) {
        filePath = argv[0x5];
    } else {
        MEDIA_ERR_LOG("record input path too long");
        return -1;
    }
    return 0;
}

static void ConvertCaptureInfo(AudioCapturerInfo &dts, const SampleAudioCapturerInfo &src)
{
    dts.inputSource = src.inputSource;
    dts.audioFormat = src.audioFormat;
    dts.sampleRate = src.sampleRate;
    dts.bitWidth = src.bitWidth;
    dts.sessionID = src.sessionID;
    dts.channelCount = src.channelCount;
    dts.streamType = src.streamType;
}

static int32_t AudioCaptureSetInfo(SampleAudioCapturerInfo audioCapturerInfo)
{
    AudioCapturerInfo info = {};
    ConvertCaptureInfo(info, audioCapturerInfo);
    if (g_audioCap->SetCapturerInfo(info) != 0) {
        MEDIA_ERR_LOG("record Can't SetCapturerInfo");
        return -1;
    }
    size_t frameCount = g_audioCap->GetFrameCount();
    if (frameCount == 0) {
        MEDIA_ERR_LOG("record Can't GetFrameCount");
        (void)g_audioCap->Release();
        return -1;
    }
    MEDIA_INFO_LOG("GetFrameCount:%d", frameCount);
    g_audioSourceProcessInput.framesize = frameCount * info.channelCount * info.bitWidth / BYTE_SIZE;
    g_audioSourceProcessInput.buffer = new uint8_t[g_audioSourceProcessInput.framesize];
    if (g_audioSourceProcessInput.buffer == nullptr) {
        MEDIA_ERR_LOG("buffer malloc fail");
        (void)g_audioCap->Release();
        return -1;
    }
    if (!g_audioCap->Start()) {
        MEDIA_ERR_LOG("record Can't Start");
        delete g_audioSourceProcessInput.buffer;
        g_audioSourceProcessInput.buffer = nullptr;
        (void)g_audioCap->Release();
        return -1;
    }
    return 0;
}

static int32_t ActivateAudioInterrupt(AudioSession &sessionId, AudioStreamType type)
{
    g_amInstance.Initialize();
    sessionId = g_amInstance.MakeSessionId();
    if (sessionId == AUDIO_SESSION_ID_NONE) {
        MEDIA_ERR_LOG("record session is none");
        return MEDIA_ERR;
    }
    MEDIA_INFO_LOG("sessionId:%d", sessionId);
    g_captureInterruptListener = std::make_shared<CaptureInterruptListener>();
    if (g_captureInterruptListener.get() == nullptr) {
        MEDIA_ERR_LOG("g_captureInterruptListener create failed!");
        return MEDIA_ERR;
    }
    g_capturerCtx.interrupt = { type, sessionId, g_captureInterruptListener };
    if (g_amInstance.ActivateAudioInterrupt(g_capturerCtx.interrupt) == INTERRUPT_FAILED) {
        MEDIA_ERR_LOG("record ActivateAudioInterrupt failed");
        return MEDIA_ERR;
    }
    return MEDIA_OK;
}

static int32_t CheckInputParam(int32_t argc, const char **argv)
{
    MEDIA_UNUSED(argv);
    if (argc != 0x6) { // must be 6 parameters
        MEDIA_ERR_LOG("argc:%d is invalid, must be 6 parameters", argc);
        return -1;
    }

    return 0;
}

int MakeAudioDir(const char *dir)
{
    if (strlen(dir) >= PATH_STR_SIZE) {
        return -1;
    }
    char str[PATH_STR_SIZE];
    if (strncpy_s(str, PATH_STR_SIZE, dir, strlen(dir)) != 0) {
        MEDIA_ERR_LOG("copy path error");
        return -1;
    }
    int len = strlen(str);
    for (int i = 0; i < len; i++) {
        if (str[i] == '/') {
            str[i] = '\0';
            if (access(str, F_OK) != 0) {
                (void)mkdir(str, DIRECTORY_PERMISSION);
            }
            str[i] = '/';
        }
    }
    if (len > 0 && access(str, F_OK) != 0) {
        (void)mkdir(str, DIRECTORY_PERMISSION);
    }
    return 0;
}

static int32_t AudioStreamCaptureInit(const JSIValue *argv)
{
    SampleAudioCapturerInfo audioCapturerInfo;
    int32_t ret = AudioStreamCaptureGetInfo(argv, audioCapturerInfo);
    if (ret != 0) {
        return ret;
    }

    AudioSession sessionId = AUDIO_SESSION_ID_NONE;
    ret = ActivateAudioInterrupt(sessionId, AUDIO_STREAM_VOICE_RECORD);
    if (ret != 0) {
        return ret;
    }
    g_audioCap = std::make_shared<AudioCapturer>();
    if (g_audioCap.get() == nullptr) {
        MEDIA_ERR_LOG("audio capture is nullptr");
        (void)g_amInstance.DeactivateAudioInterrupt(g_capturerCtx.interrupt);
        return -1;
    }
#ifdef SAVE_LOCAL_FILE
    g_audioSourceProcessInput.fd = fopen("/user/js_audio_cap.mp3", "w+");
    if (g_audioSourceProcessInput.fd == nullptr) {
        MEDIA_ERR_LOG("open file failed");
        g_audioCap.reset();
        (void)g_amInstance.DeactivateAudioInterrupt(g_capturerCtx.interrupt);
        return -1;
    }
#endif
    audioCapturerInfo.sessionID = sessionId;
    audioCapturerInfo.inputSource = AUDIO_MIC;
    audioCapturerInfo.streamType = AUDIO_STREAM_VOICE_RECORD;
    ret = AudioCaptureSetInfo(audioCapturerInfo);
    if (ret != 0) {
#ifdef SAVE_LOCAL_FILE
        fclose(g_audioSourceProcessInput.fd);
        g_audioSourceProcessInput.fd = nullptr;
#endif
        g_audioCap.reset();
        (void)g_amInstance.DeactivateAudioInterrupt(g_capturerCtx.interrupt);
        FailCallBack(MEDIA_ERR, "set info failed!");
        return ret;
    }
    return ret;
}

static int32_t AudioCaptureInit(const char **argv)
{
    SampleAudioCapturerInfo audioCapturerInfo = {};
    std::string filePath = "";
    int32_t ret = AudioCaptureGetInfo(argv, audioCapturerInfo, filePath);
    if (ret != 0) {
        return ret;
    }

    AudioSession sessionId = AUDIO_SESSION_ID_NONE;
    ret = ActivateAudioInterrupt(sessionId, AUDIO_STREAM_VOICE_ASSISTANT);
    if (ret != 0) {
        return ret;
    }
    g_audioCap = std::make_shared<AudioCapturer>();
    if (g_audioCap.get() == nullptr) {
        MEDIA_ERR_LOG("audio capture is nullptr");
        (void)g_amInstance.DeactivateAudioInterrupt(g_capturerCtx.interrupt);
        return -1;
    }

    ret = MakeAudioDir("/user/log/xiaodu");
    if (ret != 0) {
        MEDIA_ERR_LOG("dir path is invalid");
        return -1;
    }

    audioCapturerInfo.sessionID = sessionId;
    audioCapturerInfo.inputSource = AUDIO_MIC;
    audioCapturerInfo.streamType = AUDIO_STREAM_VOICE_ASSISTANT;
    ret = AudioCaptureSetInfo(audioCapturerInfo);
    if (ret != 0) {
        g_audioCap.reset();
        (void)g_amInstance.DeactivateAudioInterrupt(g_capturerCtx.interrupt);
        return ret;
    }
    return ret;
}

int32_t AudioCaptureStartSample(int32_t argc, const char **argv)
{
    for (int32_t i = 0; i < argc; i++) {
        MEDIA_INFO_LOG("argv[%d]:%s", i, argv[i]);
    }
    int32_t ret = CheckInputParam(argc, argv);
    if (ret != 0) {
        return ret;
    }

    g_capturerCtx.needStop = false;
    g_interruptHintStop = false;
    ret = AudioCaptureInit(argv);
    if (ret != 0) {
        return ret;
    }


    /* create pcm stream task for transfer */
    ret = pcm_stream_create_task();
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

    MediaThreadattr attr = { "AudioCaptureTask", 0x2000, THREAD_SCHED_INVALID, 0, false };
    g_capturerCtx.audioCaptureProcess = MediaThreadCreate(AudioCaptureProcess, nullptr, &attr);
    if (g_capturerCtx.audioCaptureProcess == nullptr) {
        MEDIA_ERR_LOG("thread create failed");
        return -1;
    }

    /* send record start */
    errcode_t res = msg_center_xiaodu_start_record(MSGCENTER_CMD_XIAODU, MSGCENTER_TYPE_ID_XIAODU_START_RECORD, NULL, 0);
    if (res != ERRCODE_SUCC) {
        MEDIA_ERR_LOG("send start fail.\r\n");
        return res;
    }
    return 0;
}

static int32_t AudioStreamCaptureStart(const JSIValue *args)
{
    g_capturerCtx.needStop = false;
    g_interruptHintStop = false;
    int32_t ret = AudioStreamCaptureInit(args);
    if (ret != 0) {
        return ret;
    }
    MediaThreadattr attr = { "AudioStreamCaptureProcess", 0x4000, THREAD_SCHED_INVALID, 0, false };
    g_capturerCtx.audioCaptureProcess = MediaThreadCreate(AudioStreamCaptureProcess, nullptr, &attr);
    if (g_capturerCtx.audioCaptureProcess == nullptr) {
        MEDIA_ERR_LOG("thread create failed");
        return -1;
    }
    return 0;
}

#ifdef __cplusplus
};

namespace OHOS {
namespace ACELite {

JSIValue AudioCapturerModule::Start(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    MEDIA_UNUSED(thisVal);
    MEDIA_UNUSED(args);
    MEDIA_UNUSED(argsSize);
    const char *params[PARAM_NUMBER] = { "", "opus", "16000", "1", "16", "/user/log/xiaodu/xiaodu.opus" };
    return JSI::CreateBoolean(AudioCaptureStartSample(PARAM_NUMBER, params));
}

JSIValue AudioCapturerModule::start(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    if (argsSize == 0) {
        const char *params[PARAM_NUMBER] = { "", "opus", "16000", "1", "16", "/user/log/xiaodu/xiaodu.opus" };
        return JSI::CreateBoolean(AudioCaptureStartSample(PARAM_NUMBER, params));
    }
    if (g_capturerCtx.capturerState == ON) {
        FailCallBack(MEDIA_OK, "Already start,no need to start!");
        return JSI::CreateBoolean(false);
    }
    if (args == nullptr || JSI::ValueIsUndefined(args[0])) {
        FailCallBack(MEDIA_ERR, "invalid parameter!");
        return JSI::CreateBoolean(false);
    }
    if (g_capturerCtx.listenerState == OFF) {
        InitResource();
    }
    if (AudioStreamCaptureStart(args) != MEDIA_OK) {
        FailCallBack(MEDIA_ERR, "Start AudioCapturer fail!");
        return JSI::CreateBoolean(false);
    }
    g_capturerCtx.capturerState = ON;
    return JSI::CreateBoolean(true);
}

JSIValue AudioCapturerModule::stop(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    if (g_capturerCtx.capturerState == OFF) {
        FailCallBack(MEDIA_ERR, "Already stop, no need to stop");
        return JSI::CreateBoolean(false);
    }
    MEDIA_UNUSED(thisVal);
    MEDIA_UNUSED(args);
    MEDIA_UNUSED(argsSize);
    MediaMutexLock(g_jsCaptureMutex);
    g_capturerCtx.needStop = true;
    MediaMutexUnLock(g_jsCaptureMutex);
    MediaThreadJoin(&g_capturerCtx.audioCaptureProcess);
    if (g_capturerCtx.listenerState == OFF) {
        DestroyResource();
    }
    g_capturerCtx.capturerState = OFF;
    return JSI::CreateBoolean(true);
}

JSIValue AudioCapturerModule::Stop(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    MEDIA_UNUSED(thisVal);
    MEDIA_UNUSED(args);
    MEDIA_UNUSED(argsSize);
    g_capturerCtx.needStop = true;
    MediaThreadJoin(&g_capturerCtx.audioCaptureProcess);
    return JSI::CreateBoolean(true);
}

JSIValue AudioCapturerModule::ListenerOn(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    if (g_capturerCtx.listenerState == ON) {
        FailCallBack(MEDIA_OK, "No need to turn on!");
        return JSI::CreateBoolean(false);
    }
    if (args == nullptr || argsSize == 0) {
        FailCallBack(MEDIA_OK, "invalid parameter!");
        return JSI::CreateBoolean(false);
    }
    if (!JSI::ValueIsObject(args[0])) {
        FailCallBack(MEDIA_OK, "invalid object!");
        return JSI::CreateBoolean(false);
    }
    if (g_capturerCtx.capturerState == OFF) {
        InitResource();
    }
    MediaMutexLock(g_jsCaptureMutex);
    g_jsCaptureParams.thisVal = JSI::AcquireValue(thisVal);
    g_jsCaptureParams.successCb = JSI::GetNamedProperty(args[0], PROP_SUCCESS);
    g_jsCaptureParams.failCb = JSI::GetNamedProperty(args[0], PROP_FAIL);
    g_capturerCtx.listenerState = ON;
    MediaMutexUnLock(g_jsCaptureMutex);
    return JSI::CreateBoolean(SUCCESS);
}

JSIValue AudioCapturerModule::ListenerOff(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    if (g_capturerCtx.listenerState == OFF) {
        FailCallBack(MEDIA_ERR, "Already off, no need to start!");
        return JSI::CreateBoolean(false);
    }
    MediaMutexLock(g_jsCaptureMutex);
    if (!JSI::ValueIsUndefined(g_jsCaptureParams.successCb)) {
        JSI::ReleaseValueList(g_jsCaptureParams.successCb);
        g_jsCaptureParams.successCb = JSI::CreateUndefined();
    }
    MediaMutexUnLock(g_jsCaptureMutex);
    if (g_capturerCtx.capturerState == OFF) {
        DestroyResource();
    }
    g_capturerCtx.listenerState = OFF;
    return JSI::CreateBoolean(SUCCESS);
}

JSIValue AudioCapturerModule::Cancel(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    MEDIA_UNUSED(thisVal);
    MEDIA_UNUSED(args);
    MEDIA_UNUSED(argsSize);
    msg_center_xiaodu_audio_pcm_clear_res();
    return JSI::CreateBoolean(0);
}

void AudioCapturerModule::ExceptionStop(void)
{
    if (!g_capturerCtx.needStop) {
        g_capturerCtx.needStop = true;
    }
}

void AudioCapturerModule::OnDestroy()
{
    if (g_capturerCtx.listenerState == OFF && g_capturerCtx.capturerState == OFF) {
        return;
    }
    MediaMutexLock(g_jsCaptureMutex);
    g_capturerCtx.needStop = true;
    MediaMutexUnLock(g_jsCaptureMutex);
    MediaThreadJoin(&g_capturerCtx.audioCaptureProcess);
    DestroyResource();
    g_capturerCtx.listenerState = OFF;
    g_capturerCtx.capturerState = OFF;
}

void AudioCapturerModule::OnTerminate()
{
    if (g_capturerCtx.listenerState == OFF && g_capturerCtx.capturerState == OFF) {
        return;
    }
    MediaMutexLock(g_jsCaptureMutex);
    g_capturerCtx.needStop = true;
    MediaMutexUnLock(g_jsCaptureMutex);
    MediaThreadJoin(&g_capturerCtx.audioCaptureProcess);
    DestroyResource();
    g_capturerCtx.listenerState = OFF;
    g_capturerCtx.capturerState = OFF;
}

void InitAudioCapturerModule(JSIValue exports)
{
    JSI::SetModuleAPI(exports, "Start", AudioCapturerModule::Start);
    JSI::SetModuleAPI(exports, "Stop", AudioCapturerModule::Stop);
    JSI::SetModuleAPI(exports, "cancel", AudioCapturerModule::Cancel);
    JSI::SetModuleAPI(exports, "start", AudioCapturerModule::start);
    JSI::SetModuleAPI(exports, "stop", AudioCapturerModule::stop);
    JSI::SetModuleAPI(exports, "on", AudioCapturerModule::ListenerOn);
    JSI::SetModuleAPI(exports, "off", AudioCapturerModule::ListenerOff);
    JSI::SetOnDestroy(exports, AudioCapturerModule::OnDestroy);
    JSI::SetOnTerminate(exports, AudioCapturerModule::OnTerminate);
}

}
}

#endif

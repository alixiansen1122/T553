/*
* Copyright (c) CompanyNameMagicTag. 2021-2021. All rights reserved.
* Description: audio capture wrapper
* Author: Media Software Group
* Create: 2021-09-13
*/

#include "audio_capture_wrapper.h"
#include <cstdio>
#include <cstring>
#include <climits>
#include <cstdlib>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifdef SUPPORT_SYS_PRCTL
#include <sys/prctl.h>
#endif
#include <string>
#include <iostream>
#include "securec.h"
#include "audio_capturer.h"
#include "media_errors.h"
#include "audio_manager.h"
#include "media_log.h"
#include "media_mem.h"
#include "media_thread_adapt.h"
#ifdef AUDIO_OGG_SUPPORT
#include "ogg.h"
#endif
#ifdef __cplusplus
extern "C" {
#endif

#define CHK_NULL_RETURN(ptr, printfString) \
do { \
    if ((ptr) == NULL) { \
        MEDIA_ERR_LOG("%s", printfString); \
        return MEDIA_ERR; \
    } \
} while (0)
#define CHK_OK_RETURN(ret, printfString) \
do { \
    if ((ret) != MEDIA_OK || (ret) != 0) { \
        MEDIA_ERR_LOG("%s", printfString); \
        return MEDIA_ERR; \
    } \
} while (0)

using namespace OHOS::Media;
using namespace OHOS::Audio;
using namespace Audio;
using Audio::InterruptListener;
using Audio::AudioInterrupt;
using Audio::AudioManager;
#define PLAYER_CONTROL_CMD_LEN  20
#define READ_RETRY_TIME_US 10000
#define READ_COUNT_MAX 1000
#define GET_AUDIO_TIME_INTERVAL_TIMES 100
#define MAX_ARGC 5
#define BYTE_SIZE 8
const uint32_t MILLSECOND_TO_SECOND = 1000;
const uint32_t HEADER_STRLEN = 9;
const uint32_t OPUS_DELAY_TIME_DIVISOR = 0;
const uint32_t OPUS_DELAY_TIME_DIVIDED = 100;
const uint32_t OPUS_ID_HEADER_LEN = 19;
const uint32_t OPUS_COMMENT_HEADER_LEN = 25;
const uint32_t OPUS_COMMENT_TAIL_LEN = 4;
const uint32_t OPUS_ENCODE_HEADER_SIZE = 0;
const uint32_t OPUS_OPUSTAGS_LEN = 12;
const uint32_t OPUS_SAMPLERATE = 48000;

typedef struct {
    size_t framesize;
    uint8_t *buffer;
    AudioCapturer *audioCap;
    AudioCodecFormat audioFormat;
    bool bThreadRun;
} AudioSourceInput;

struct NameAudioFormat {
    std::string name;
    AudioCodecFormat audioFormat;
};

typedef struct TagTestSample {
    char control[PLAYER_CONTROL_CMD_LEN];
    bool needStop;
    AudioInterrupt interrupt;
    MediaThreadIdHandle audioCaptureProcess;
    MediaMutexHandle mutex;
} TestSample;

struct SampleAudioCapturerInfo {
    /** Audio source type */
    AudioSourceType inputSource = AUDIO_MIC;
    /** Audio codec format */
    AudioCodecFormat audioFormat = AUDIO_DEFAULT;
    /** Sampling rate */
    int32_t sampleRate = 0;
    /** Number of audio channels */
    int32_t channelCount = 0;
    /** Bit rate */
    int32_t bitRate = 0;
    /** Audio stream type */
    AudioStreamType streamType = TYPE_MEDIA;
    /** Bit width */
    AudioBitWidth bitWidth = BIT_WIDTH_16;
    /** Session id */
    AudioSession sessionID = AUDIO_SESSION_ID_NONE;
};

#ifdef AUDIO_OGG_SUPPORT
typedef struct CaptureOggContext {
    ogg_stream_state oggStream = {};
    int64_t sampleRate;
    int64_t channels;
    int64_t granulepos;
    int64_t packetCount;
    int32_t serialno;
    bool isInited;
    ogg_page *page;
    ogg_packet *packet;
    unsigned char *idHeader;
    unsigned char *commentHeader;
} CaptureOggContext;

static CaptureOggContext g_oggCtx = {};
#endif

static bool g_interruptHintStop = false;
static std::shared_ptr<AudioCapturer> g_audioCap;
static NameAudioFormat g_audioCodecFormat[] = {
    {"mp3", MP3},
    {"ape", APE},
    {"msbc", mSBC},
    {"vorbis", VORBIS},
    {"flac", FLAC},
    {"opus", OPUS},
    {"g726", G726},
    {"g711u", G711U},
    {"g711a", G711A},
    {"aaceld", AAC_ELD},
    {"silk", SILK},
    {"pcm", PCM}
};

class AudioCaptureInterruptListener : public InterruptListener {
public:
    AudioCaptureInterruptListener() {};
    ~AudioCaptureInterruptListener() override {};

    void OnInterrupt(int32_t type, int32_t hint) override
    {
        MEDIA_INFO_LOG("AudioCaptureInterruptListener OnInterrupt, type:%d, hint:%d", type, hint);

        if (type == INTERRUPT_TYPE_BEGIN && hint == INTERRUPT_HINT_PAUSE) {
            MEDIA_INFO_LOG("OnInterrupt pause not supported");
        }
        if (type == INTERRUPT_TYPE_END && hint == INTERRUPT_HINT_RESUME) {
            MEDIA_INFO_LOG("OnInterrupt resume not supported");
        }
        if (type == INTERRUPT_TYPE_BEGIN && hint == INTERRUPT_HINT_STOP) {
            MEDIA_INFO_LOG("OnInterrupt stop");
            g_interruptHintStop = true;
        }
        if (type == INTERRUPT_TYPE_END && hint == INTERRUPT_HINT_STOP) {
            MEDIA_INFO_LOG("OnInterrupt stop not supported");
        }
    }
};

static AudioSourceInput g_audioSourceProcessInput;
static AudioManager& g_amInstance = AudioManager::GetInstance();
static TestSample g_sample;
static std::shared_ptr<AudioCaptureInterruptListener> g_audioCaptureInterruptListener;
#ifdef AUDIO_OGG_SUPPORT

static int32_t AudioCapturerOggHeaderInit(SampleAudioCapturerInfo &info)
{
    const char *idHeader = "OpusHead";
    const char *commentHeader = "OpusTags";
    const char *vendor = "libogg-1.3.5";
    uint32_t vendorLen = strlen(vendor);
    uint32_t preSkip = g_oggCtx.sampleRate * OPUS_DELAY_TIME_DIVISOR / OPUS_DELAY_TIME_DIVIDED;
    g_oggCtx.idHeader = static_cast<unsigned char*>(MediaMemCalloc(OPUS_ID_HEADER_LEN));
    uint32_t commentHeaderResLen = OPUS_COMMENT_HEADER_LEN - OPUS_OPUSTAGS_LEN;
    CHK_NULL_RETURN(g_oggCtx.idHeader, "ogg idHeader malloc failed!");
    if (memcpy_s(g_oggCtx.idHeader, OPUS_ID_HEADER_LEN, idHeader, strlen(idHeader)) != 0) {
        MEDIA_ERR_LOG("ogg idHeader memcpy failed!");
        goto FREE_ID_HEADER;
    }
    g_oggCtx.idHeader[0x8] = 0x01;
    g_oggCtx.idHeader[0x9] = info.channelCount;
    g_oggCtx.idHeader[0xA] = preSkip & 0xFF;
    g_oggCtx.idHeader[0xB] = (preSkip >> 0x8); // 8 for Little-endian byte order
    g_oggCtx.idHeader[0xC] = g_oggCtx.sampleRate & 0xFF;
    g_oggCtx.idHeader[0xD] = (g_oggCtx.sampleRate >> 0x8) & 0xFF; // 8 for Little-endian byte order
    g_oggCtx.idHeader[0xE] = (g_oggCtx.sampleRate >> 0x10) & 0xFF; // 16 for Little-endian byte order
    g_oggCtx.idHeader[0xF] = (g_oggCtx.sampleRate >> 0x18) & 0xFF; // 24 for Little-endian byte order

    g_oggCtx.commentHeader = static_cast<unsigned char*>(MediaMemCalloc(OPUS_COMMENT_HEADER_LEN));
    if (g_oggCtx.commentHeader == nullptr) {
        MEDIA_ERR_LOG("ogg commentHeader malloc failed!");
        goto FREE_ID_HEADER;
    }
    if (memcpy_s(g_oggCtx.commentHeader, OPUS_COMMENT_HEADER_LEN, commentHeader, strlen(commentHeader)) != 0) {
        MEDIA_ERR_LOG("ogg commentHeader memcpy failed!");
        goto FREE_COMMENT_HEADER;
    }
    g_oggCtx.commentHeader[0x8] = vendorLen & 0xFF;
    g_oggCtx.commentHeader[0x9] = (vendorLen >> 0x8) & 0xFF; // 8 for Little-endian byte order
    g_oggCtx.commentHeader[0xA] = (vendorLen >> 0x10) & 0xFF; // 16 for Little-endian byte order
    g_oggCtx.commentHeader[0xB] = (vendorLen >> 0x18) & 0xFF; // 24 for Little-endian byte order

    if (memcpy_s(g_oggCtx.commentHeader + OPUS_OPUSTAGS_LEN, commentHeaderResLen, vendor, strlen(vendor)) != 0) {
        MEDIA_ERR_LOG("ogg vendor memcpy failed!");
        goto FREE_COMMENT_HEADER;
    }
    return MEDIA_OK;
FREE_COMMENT_HEADER:
    MediaMemFree((void **)&g_oggCtx.commentHeader);
FREE_ID_HEADER:
    MediaMemFree((void **)&g_oggCtx.idHeader);
    return MEDIA_ERR;
}

static int32_t AudioCaptureOggStreamFlush(FILE *pfd)
{
    while (ogg_stream_flush(&g_oggCtx.oggStream, g_oggCtx.page)) {
        int32_t ret = fwrite(g_oggCtx.page->header, 1, g_oggCtx.page->header_len, pfd);
        if (ret != g_oggCtx.page->header_len) {
            MEDIA_ERR_LOG("errno:%d, errmsg:%s", errno, strerror(errno));
            return MEDIA_ERR;
        }
        ret = fwrite(g_oggCtx.page->body, 1, g_oggCtx.page->body_len, pfd);
        if (ret != g_oggCtx.page->body_len) {
            MEDIA_ERR_LOG("errno:%d, errmsg:%s", errno, strerror(errno));
            return MEDIA_ERR;
        }
    }
    return MEDIA_OK;
}

static void AudioCapturerOggParamInit(SampleAudioCapturerInfo &info)
{
    g_oggCtx.sampleRate = info.sampleRate;
    g_oggCtx.channels = info.channelCount;
    g_oggCtx.granulepos = 0;
    g_oggCtx.packetCount = 0;
    g_oggCtx.serialno = 0x1; // unique serialno
}

static void AudioCapturerOggPacketIn(unsigned char *packet, int64_t bytes)
{
    g_oggCtx.packet->packet = packet;
    g_oggCtx.packet->bytes = bytes;
    g_oggCtx.packet->b_o_s = g_oggCtx.packetCount != 0 ? 0 : 1; // 1 means first page
    g_oggCtx.packet->e_o_s = 0;
    g_oggCtx.packet->granulepos = g_oggCtx.packetCount != 0 ? g_oggCtx.granulepos : -1; // -1 means first page
    g_oggCtx.packet->packetno = g_oggCtx.packetCount++;
    ogg_stream_packetin(&g_oggCtx.oggStream, g_oggCtx.packet);
}

static int32_t AudioCapturerOggCtxInit(FILE *pfd, SampleAudioCapturerInfo &info)
{
    if (g_oggCtx.isInited) {
        MEDIA_ERR_LOG("Ogg context has inited!");
        goto EXIT;
    }
    g_oggCtx.packet = (ogg_packet *)MediaMemCalloc(sizeof(ogg_packet));
    if (g_oggCtx.packet == nullptr) {
        MEDIA_ERR_LOG("ogg context packet malloc failed!");
        goto EXIT;
    }
    g_oggCtx.page = (ogg_page *)MediaMemCalloc(sizeof(ogg_page));
    if (g_oggCtx.page == nullptr) {
        MEDIA_ERR_LOG("ogg context page malloc failed!");
        goto FREE_PACKET;
    }
    AudioCapturerOggParamInit(info);
    if (ogg_stream_init(&g_oggCtx.oggStream, g_oggCtx.serialno) != MEDIA_OK) {
        MEDIA_ERR_LOG("ogg stream init failed!");
        goto FREE_PAGE;
    }
    if (AudioCapturerOggHeaderInit(info) != MEDIA_OK) {
        MEDIA_ERR_LOG("Header init failed!");
        goto STREAM_CLEAR;
    }
    AudioCapturerOggPacketIn(g_oggCtx.idHeader, OPUS_ID_HEADER_LEN);
    if (AudioCaptureOggStreamFlush(pfd) != MEDIA_OK) {
        MEDIA_ERR_LOG("ogg packet flush failed!");
        goto HEADER_FREE;
    }
    AudioCapturerOggPacketIn(g_oggCtx.commentHeader, OPUS_COMMENT_HEADER_LEN);
    if (AudioCaptureOggStreamFlush(pfd) != MEDIA_OK) {
        MEDIA_ERR_LOG("ogg header flush failed!");
        goto HEADER_FREE;
    }
    g_oggCtx.isInited = true;
    return MEDIA_OK;
HEADER_FREE:
    MediaMemFree((void **)&g_oggCtx.idHeader);
    MediaMemFree((void **)&g_oggCtx.commentHeader);
STREAM_CLEAR:
    ogg_stream_clear(&g_oggCtx.oggStream);
FREE_PAGE:
    MediaMemFree((void **)&g_oggCtx.page);
FREE_PACKET:
    MediaMemFree((void **)&g_oggCtx.packet);
EXIT:
    return MEDIA_ERR;
}

static void AudioCapturerOggCtxDeInit()
{
    g_oggCtx.sampleRate = 0;
    g_oggCtx.channels = 0;
    g_oggCtx.granulepos = 0;
    g_oggCtx.packetCount = 0;
    MediaMemFree(g_oggCtx.packet);
    g_oggCtx.packet = nullptr;
    MediaMemFree(g_oggCtx.page);
    g_oggCtx.page = nullptr;
    MediaMemFree(g_oggCtx.idHeader);
    g_oggCtx.idHeader = nullptr;
    MediaMemFree(g_oggCtx.commentHeader);
    g_oggCtx.commentHeader = nullptr;
    ogg_stream_clear(&g_oggCtx.oggStream);
    g_oggCtx.isInited = false;
}

static int32_t AudioCaptureWritePage(FILE *pfd)
{
    if (ogg_stream_pageout(&g_oggCtx.oggStream, g_oggCtx.page)) {
        int32_t ret = fwrite(g_oggCtx.page->header, 1, g_oggCtx.page->header_len, pfd);
        if (ret != g_oggCtx.page->header_len) {
            MEDIA_ERR_LOG("errno:%d, errmsg:%s", errno, strerror(errno));
            return MEDIA_ERR;
        }
        ret = fwrite(g_oggCtx.page->body, 1, g_oggCtx.page->body_len, pfd);
        if (ret != g_oggCtx.page->body_len) {
            MEDIA_ERR_LOG("errno:%d, errmsg:%s", errno, strerror(errno));
            return MEDIA_ERR;
        }
    }
    return MEDIA_OK;
}

static int32_t AudioCapturerWritePacket(uint8_t *buffer, int64_t bufferLen, FILE *pfd)
{
    if (bufferLen < 0) {
        MEDIA_ERR_LOG("input buffer invalid");
        return MEDIA_ERR;
    }
    AudioCapturerOggPacketIn((unsigned char *)(buffer + OPUS_ENCODE_HEADER_SIZE), bufferLen - OPUS_ENCODE_HEADER_SIZE);
    g_oggCtx.granulepos += OPUS_SAMPLERATE * 10 / MILLSECOND_TO_SECOND; // capture frequency is 10ms for each frame
    return AudioCaptureWritePage(pfd);
}

static int32_t AudioCapturerWriteEosPacket(FILE *pfd)
{
    unsigned char emptyData = 0;
    g_oggCtx.packet->packet = &emptyData;
    g_oggCtx.packet->bytes = 1;
    g_oggCtx.packet->b_o_s = 0;
    g_oggCtx.packet->e_o_s = 1;
    g_oggCtx.packet->granulepos = g_oggCtx.granulepos;
    g_oggCtx.packet->packetno = g_oggCtx.packetCount;
    ogg_stream_packetin(&g_oggCtx.oggStream, g_oggCtx.packet);
    return AudioCaptureOggStreamFlush(pfd);
}
#endif

static void AudioCaptureStop()
{
    MediaMutexLock(g_sample.mutex);
    if (!g_audioCap->Stop()) {
        MEDIA_ERR_LOG("Stop failed");
    }
    if (!g_audioCap->Release()) {
        MEDIA_ERR_LOG("Release failed");
    }

    delete g_audioSourceProcessInput.buffer;
    g_audioSourceProcessInput.buffer = nullptr;

    if (g_amInstance.DeactivateAudioInterrupt(g_sample.interrupt) != 0) {
        MEDIA_ERR_LOG("deactivate audio interrupt failed");
    }
    g_audioCap.reset();
    g_audioCap = nullptr;
    g_audioSourceProcessInput.bThreadRun = false;
    MediaMutexUnLock(g_sample.mutex);
    (void)MediaMutexDestroy(&g_sample.mutex);
}

static void* AudioCaptureProcess(void* arg)
{
    FILE *pfd = (FILE *)arg;
    int32_t readCnt = 0;
    Timestamp timeStamp;
    g_audioSourceProcessInput.bThreadRun = true;
    while (true) {
        if (g_sample.needStop || g_interruptHintStop || readCnt > READ_COUNT_MAX) {
            MEDIA_INFO_LOG("quit AudioCapturer Process");
            break;
        }
        int32_t ret = g_audioCap->Read(g_audioSourceProcessInput.buffer, g_audioSourceProcessInput.framesize, false);
        if (ret <= 0) {
            if (ret == ERR_RETRY_READ) {
                usleep(READ_RETRY_TIME_US);
                continue;
            }
            MEDIA_DEBUG_LOG("audioCap Read failed ret %x", ret);
            usleep(READ_RETRY_TIME_US);
            continue;
        }
        readCnt++;
        bool hasWrite = false;
        if (g_audioSourceProcessInput.audioFormat == OPUS) {
#ifdef AUDIO_OGG_SUPPORT
            if (AudioCapturerWritePacket(g_audioSourceProcessInput.buffer, ret, pfd) != MEDIA_OK) {
                MEDIA_ERR_LOG("Write file failed!");
                break;
            }
            hasWrite = true;
#endif
        } 
        if (!hasWrite) {
            int32_t writeLen = fwrite(g_audioSourceProcessInput.buffer, 1, ret, pfd);
            if (writeLen != ret) {
                MEDIA_ERR_LOG("errno:%d, errmsg:%s", errno, strerror(errno));
                break;
            }
        }
        if (readCnt % GET_AUDIO_TIME_INTERVAL_TIMES == 0) {
            if (g_audioCap->GetAudioTime(timeStamp, Timestamp::Timebase::MONOTONIC)) {
                MEDIA_INFO_LOG("read %d frames timestamp seconds:%ld, nanoseconds:%ld",
                    readCnt, timeStamp.time.tv_sec, timeStamp.time.tv_nsec);
            }
        }
        MEDIA_DEBUG_LOG("audioCap Read readCnt = %d, size = %d", readCnt, ret);
    }
#ifdef AUDIO_OGG_SUPPORT
    if (g_audioSourceProcessInput.audioFormat == OPUS) {
        if (AudioCapturerWriteEosPacket(pfd) != MEDIA_OK) {
            MEDIA_INFO_LOG("ogg write eos failed!");
        }
        AudioCapturerOggCtxDeInit();
    }
#endif
    fclose(pfd);
    AudioCaptureStop();
    if (!g_sample.needStop) {
        MediaThreadSetDetach(g_sample.audioCaptureProcess, true);
        g_sample.audioCaptureProcess = nullptr;
    }
    MEDIA_INFO_LOG("AudioCaptureProcess end");
    return nullptr;
}

static void AudioCaptureGetFormat(const char **argv, AudioCodecFormat &audioFormat)
{
    char formatType[10];
    if (strncpy_s(formatType, sizeof(formatType), argv[0x1], strlen(argv[0x1])) != 0) {
        MEDIA_ERR_LOG("strncpy_s failed");
    }
    std::string inputFormat = formatType;
    for (size_t i = 0; i < (sizeof(g_audioCodecFormat) / sizeof(g_audioCodecFormat[0])); i++) {
        if (inputFormat == g_audioCodecFormat[i].name) {
            audioFormat = g_audioCodecFormat[i].audioFormat;
            MEDIA_INFO_LOG("[AudioCaptureSample] set audioFormat %s", g_audioCodecFormat[i].name.c_str());
        }
    }
}

static int32_t AudioCaptureGetInfo(const char **argv, SampleAudioCapturerInfo &audioCapturerInfo, std::string &filePath)
{
    AudioCaptureGetFormat(argv, audioCapturerInfo.audioFormat);
    audioCapturerInfo.sampleRate = atoi(argv[0x2]); /* 2: samplerate */
    MEDIA_INFO_LOG("set capture parameter sampleRate:%d", audioCapturerInfo.sampleRate);
    audioCapturerInfo.channelCount = atoi(argv[0x3]); /* 3: channel */
    MEDIA_INFO_LOG("set capture parameter channel:%d", audioCapturerInfo.channelCount);
    audioCapturerInfo.bitWidth = (AudioBitWidth)atoi(argv[0x4]); /* 4: bitWidth */
    MEDIA_INFO_LOG("set capture parameter bitWidth:%d", audioCapturerInfo.bitWidth);
    MEDIA_INFO_LOG("input argv[5]:%s.", argv[5]); /* 5: filePath */

    if (strlen(argv[0x5]) < PATH_MAX) {
        filePath = argv[0x5];
    } else {
        MEDIA_ERR_LOG("input path too long");
        return -1;
    }
    return 0;
}

static int32_t HandleAudioCaptureCmd(int32_t argc, const char **argv)
{
    if (argc == 1 && argv[0] != nullptr) {
        if (memcpy_s(g_sample.control, PLAYER_CONTROL_CMD_LEN,
            argv[0], strlen(argv[0]) + 1) != EOK) {
            MEDIA_ERR_LOG("memcpy_s failed");
        }
        if ((strncmp(g_sample.control, "stop", strlen("stop")) == 0)) {
            if (g_audioSourceProcessInput.bThreadRun) {
                g_sample.needStop = true;
                MediaThreadJoin(&g_sample.audioCaptureProcess);
                g_audioSourceProcessInput.bThreadRun = false;
                MEDIA_INFO_LOG("set g_sample.needStop:%d", g_sample.needStop);
            } else {
                MEDIA_INFO_LOG("bThreadRun:%d no need stop", g_audioSourceProcessInput.bThreadRun);
            }
            MEDIA_INFO_LOG("stop command excute success");
            return 0;
        } else if ((strncmp(g_sample.control, "mute", strlen("mute")) == 0)) {
            MEDIA_INFO_LOG("mute detected call SetMicrophoneMute(true)");
            if (!g_amInstance.SetMicrophoneMute(true)) {
                MEDIA_ERR_LOG("SetMicrophoneMute failed");
            }
            if (!g_amInstance.IsMicrophoneMute()) {
                MEDIA_ERR_LOG("IsMicrophoneMute return not true");
            }
            MEDIA_INFO_LOG("mute cmd out");
        } else if ((strncmp(g_sample.control, "unmute", strlen("unmute")) == 0)) {
            MEDIA_INFO_LOG("unmute detected call SetMicrophoneMute(false)");
            if (!g_amInstance.SetMicrophoneMute(false)) {
                MEDIA_ERR_LOG("SetMicrophoneMute failed");
            }
            if (g_amInstance.IsMicrophoneMute()) {
                MEDIA_ERR_LOG("IsMicrophoneMute return not false");
            }
            MEDIA_INFO_LOG("unmute cmd out");
        } else if ((strncmp(g_sample.control, "dumpinfo", strlen("dumpinfo")) == 0)) {
            MediaMutexLock(g_sample.mutex);
            if (g_audioCap != nullptr && g_audioCap.get() != nullptr) {
                AudioCapturerDebugInfo info;
                g_audioCap->DumpInfo(info);
            }
            MediaMutexUnLock(g_sample.mutex);
        } else {
            MEDIA_ERR_LOG("[AudioCaptureSample]control cmd not supported:%s do nothing", g_sample.control);
            return 0;
        }
    }
    return 0;
}

static void ConvertCaptureInfo(AudioCapturerInfo &dts, const SampleAudioCapturerInfo &src)
{
    dts.inputSource = src.inputSource;
    dts.audioFormat = src.audioFormat;
    dts.sampleRate = src.sampleRate;
    dts.channelCount = src.channelCount;
    dts.streamType = src.streamType;
    dts.bitWidth = src.bitWidth;
    dts.sessionID = src.sessionID;
}

static int32_t AudioCaptureSetInfo(SampleAudioCapturerInfo audioCapturerInfo)
{
    AudioCapturerInfo info = {};
    ConvertCaptureInfo(info, audioCapturerInfo);
    if (g_audioCap->SetCapturerInfo(info) != 0) {
        MEDIA_ERR_LOG("Can't SetCapturerInfo");
        return -1;
    }
    size_t frameCount = g_audioCap->GetFrameCount();
    if (!frameCount) {
        MEDIA_ERR_LOG("Can't GetFrameCount");
        (void)g_audioCap->Release();
        return -1;
    }
    MEDIA_INFO_LOG("GetFrameCount:%d", frameCount);
    g_audioSourceProcessInput.audioFormat = info.audioFormat;
    g_audioSourceProcessInput.framesize = frameCount * info.channelCount * info.bitWidth / BYTE_SIZE;
    g_audioSourceProcessInput.buffer = new uint8_t[g_audioSourceProcessInput.framesize];
    if (g_audioSourceProcessInput.buffer == nullptr) {
        (void)g_audioCap->Release();
        return -1;
    }
    if (!g_audioCap->Start()) {
        MEDIA_ERR_LOG("Can't Start");
        delete g_audioSourceProcessInput.buffer;
        g_audioSourceProcessInput.buffer = nullptr;
        (void)g_audioCap->Release();
        return -1;
    }
    return 0;
}

static int32_t ActivateAudioInterrupt(AudioSession &sessionId)
{
    g_amInstance = AudioManager::GetInstance();
    g_amInstance.Initialize();
    sessionId = g_amInstance.MakeSessionId();
    if (sessionId == AUDIO_SESSION_ID_NONE) {
        MEDIA_ERR_LOG("session is none");
        return -1;
    }
    MEDIA_INFO_LOG("sessionId:%d", sessionId);
    g_audioCaptureInterruptListener = std::make_shared<AudioCaptureInterruptListener>();
    g_sample.interrupt = { AUDIO_STREAM_VOICE_RECORD, sessionId, g_audioCaptureInterruptListener };
    if (g_amInstance.ActivateAudioInterrupt(g_sample.interrupt) == INTERRUPT_FAILED) {
        MEDIA_ERR_LOG("ActivateAudioInterrupt failed");
        return -1;
    }
    return 0;
}


static int32_t CheckInputParam(int32_t argc, const char **argv)
{
    MEDIA_UNUSED(argv);
    if (argc != 0x6) { // must be 6 parameters
        MEDIA_ERR_LOG("argc:%d is invalid, record start must be 6 parameters", argc);
        return -1;
    }

    return 0;
}

int32_t AudioCaptureSample(int32_t argc, const char **argv)
{
    if (g_audioSourceProcessInput.bThreadRun) {
        return HandleAudioCaptureCmd(argc, argv);
    }
    std::string filePath = "";
    g_sample.needStop = false;
    g_interruptHintStop = false;
    g_sample.mutex = MediaMutexCreate(nullptr);
    g_audioSourceProcessInput.bThreadRun = true;
    if (g_sample.mutex == nullptr) {
        MEDIA_ERR_LOG("create mutex failed");
        g_audioSourceProcessInput.bThreadRun = false;
        return -1;
    }
    SampleAudioCapturerInfo audioCapturerInfo = {};
    AudioSession sessionId = AUDIO_SESSION_ID_NONE;
    FILE *pfd = nullptr;
    MediaThreadattr attr = {"AudioCaptureTask", 0x2000, THREAD_SCHED_INVALID, 0, false};
    int32_t ret = CheckInputParam(argc, argv);
    if (ret != 0) {
        goto EXIT;
    }
    ret = AudioCaptureGetInfo(argv, audioCapturerInfo, filePath);
    if (ret != 0) {
        goto EXIT;
    }
    ret = ActivateAudioInterrupt(sessionId);
    if (ret != 0) {
        goto EXIT;
    }
    g_audioCap = std::make_shared<AudioCapturer>();
    if (g_audioCap == nullptr || g_audioCap.get() == nullptr) {
        MEDIA_ERR_LOG("audio capture is nullptr");
        (void)g_amInstance.DeactivateAudioInterrupt(g_sample.interrupt);
        goto EXIT;
    }
    pfd = fopen(filePath.c_str(), "w+");
    if (pfd == nullptr) {
        MEDIA_ERR_LOG("open file failed");
        g_audioCap.reset();
        (void)g_amInstance.DeactivateAudioInterrupt(g_sample.interrupt);
        goto EXIT;
    }
    audioCapturerInfo.sessionID = sessionId;
    audioCapturerInfo.inputSource = AUDIO_MIC;
    audioCapturerInfo.streamType = AUDIO_STREAM_VOICE_RECORD;
    ret = AudioCaptureSetInfo(audioCapturerInfo);
    if (ret != 0) {
        MEDIA_ERR_LOG("AudioCapture set failed");
        goto FCLOSE_EXIT;
    }
#ifdef AUDIO_OGG_SUPPORT
    if (audioCapturerInfo.audioFormat == OPUS) {
        ret = AudioCapturerOggCtxInit(pfd, audioCapturerInfo);
        if (ret != MEDIA_OK) {
            MEDIA_ERR_LOG("OggCtx init failed");
            goto FCLOSE_EXIT;
        }
    }
#endif
    g_sample.audioCaptureProcess = MediaThreadCreate(AudioCaptureProcess, pfd, &attr);
    if (g_sample.audioCaptureProcess == nullptr) {
        MEDIA_ERR_LOG("thread create failed");
#ifdef AUDIO_OGG_SUPPORT
        AudioCapturerOggCtxDeInit();
#endif
        goto FCLOSE_EXIT;
    }
    return 0;

FCLOSE_EXIT:
    fclose(pfd);
    g_audioCap.reset();
    (void)g_amInstance.DeactivateAudioInterrupt(g_sample.interrupt);
EXIT:
    g_audioSourceProcessInput.bThreadRun = false;
    return -1;
}

#ifdef __cplusplus
};
#endif

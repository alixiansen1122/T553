/*
 * Copyright (c) CompanyNameMagicTag. 2024-2025. All rights reserved.
 * Description: audio player test
 * Author: Media Software Group
 * Create: 2024-12-10
 */

#include "audio_player_test.h"
#include <vector>
#include <string>
#include "audio_player_service.h"
#include "media_log.h"

const int32_t MEDIA_CMD_LEN_MAX = 128;
#define MEDIA_UNUSED(x) ((void)(x))
#define ARRAY_SIZE(array) (sizeof((array)) / sizeof((array)[0]))
#ifdef __cplusplus
extern "C" {
#endif

using OHOS::Media::Player;
using OHOS::AudioPlayerServiceCallback;
using OHOS::AudioPlayerService;
using namespace OHOS;
using namespace OHOS::Media;
using namespace::Audio;
using Audio::AudioManager;
const int32_t NUMBER_BASE = 10;

std::vector<std::string> g_playList = {
    "/user/test/cai.mp3",
    "/user/test/guyongzhe_8k_2ch_8k_0x20000.mp3",
    "/user/test/test_32kHz_16bit_1ch.flac",
    "/user/test/AAC_48kHz_2ch.aac"
};
typedef struct {
    char cmd[MEDIA_CMD_LEN_MAX];
    int32_t (*func)(int32_t argc, const char **param);
} FuncsMap;

class AudioPlayerCallbackImpl : public AudioPlayerServiceCallback {
    public:
    AudioPlayerCallbackImpl() = default;
    ~AudioPlayerCallbackImpl() override {}
    void OnInterrupt(int32_t type, int32_t hint) override
    {
        MEDIA_ERR_LOG("OnInterrupt recv is type[%d], hint[%d].", type, hint);
        if (type == INTERRUPT_TYPE_BEGIN && hint == INTERRUPT_HINT_PAUSE) {
            AudioPlayerService::GetInstance()->Pause();
        } else if (type == INTERRUPT_TYPE_END && hint == INTERRUPT_HINT_RESUME) {
            AudioPlayerService::GetInstance()->Resume();
        } else if (type == INTERRUPT_TYPE_BEGIN && hint == INTERRUPT_HINT_STOP) {
            AudioPlayerService::GetInstance()->Reset();
        }
    }
    void OnError(int32_t errorType, int32_t errorCode) override
    {
        MEDIA_ERR_LOG("OnError errorType[%d]. errorCode[%d]", errorType, errorCode);
        AudioPlayerService::GetInstance()->Reset();
    }

    void OnPlaybackComplete() override
    {
        MEDIA_INFO_LOG("recv OnPlaybackComplete !");
    }
    void OnPlay() override
    {
        MEDIA_INFO_LOG("recv OnPlay !");
    }
    void OnPause() override
    {
        MEDIA_INFO_LOG("recv OnPause !");
    }
    void OnStop() override
    {
        MEDIA_INFO_LOG("recv OnStop !");
    }
};

static int32_t StartAudioPlay(int32_t argc, const char **param)
{
    if (argc <= 1) {
        MEDIA_ERR_LOG("The parameter length[%d] is insufficient.", argc);
        return MEDIA_ERR;
    }
    int64_t index = strtol(param[0x1], nullptr, NUMBER_BASE);
    if (index > g_playList.size()) {
        MEDIA_ERR_LOG("The input start position[%d] is incorrect. It exceeds the length of the playlist.", index);
        return MEDIA_ERR;
    }
    AudioPlayerService::GetInstance()->SetAudioPlayerCallback(std::make_shared<AudioPlayerCallbackImpl>());
    int32_t ret = AudioPlayerService::GetInstance()->SetPlayListSource(g_playList, index);
    if (ret != MEDIA_OK) {
        MEDIA_ERR_LOG("failed to set the playlist!");
        return MEDIA_ERR;
    }
    return AudioPlayerService::GetInstance()->Start();
}

static int32_t StopAudioPlay(int32_t argc, const char **param)
{
    MEDIA_UNUSED(param);
    MEDIA_UNUSED(argc);
    return AudioPlayerService::GetInstance()->Reset();
}
static int32_t SeekAudioPlay(int32_t argc, const char **param)
{
    if (argc <= 1) {
        MEDIA_ERR_LOG("The parameter length[%d] is insufficient.", argc);
        return MEDIA_ERR;
    }
    int32_t seekPos = strtol(param[0x1], nullptr, NUMBER_BASE);
    return AudioPlayerService::GetInstance()->Seek(seekPos);
}

static int32_t PauseAudioPlay(int32_t argc, const char **param)
{
    MEDIA_UNUSED(param);
    MEDIA_UNUSED(argc);
    return AudioPlayerService::GetInstance()->Pause();
}

static int32_t ResumeAudioPlay(int32_t argc, const char **param)
{
    MEDIA_UNUSED(param);
    MEDIA_UNUSED(argc);
    return AudioPlayerService::GetInstance()->Resume();
}

static int32_t PlayNext(int32_t argc, const char **param)
{
    MEDIA_UNUSED(param);
    MEDIA_UNUSED(argc);
    return AudioPlayerService::GetInstance()->PlayNext();
}

static int32_t PlayPrev(int32_t argc, const char **param)
{
    MEDIA_UNUSED(param);
    MEDIA_UNUSED(argc);
    return AudioPlayerService::GetInstance()->PlayPrev();
}

static int32_t GetCurrentTime(int32_t argc, const char **param)
{
    MEDIA_UNUSED(param);
    MEDIA_UNUSED(argc);
    int64_t time = 0;
    int32_t ret =  AudioPlayerService::GetInstance()->GetCurrentTime(time);
    if (ret != 0) {
        MEDIA_ERR_LOG("get current time failed!.");
    }
    MEDIA_INFO_LOG("current time[%lld]!", time);
    return ret;
}

static int32_t GetPlayStatus(int32_t argc, const char **param)
{
    MEDIA_UNUSED(param);
    MEDIA_UNUSED(argc);
    int32_t status = AudioPlayerService::GetInstance()->GetPlayStatus();
    MEDIA_INFO_LOG("current status[%d]!", status);
    return MEDIA_OK;
}

static int32_t SetSingleLoop(int32_t argc, const char **param)
{
    if (argc <= 1) {
        MEDIA_ERR_LOG("The parameter length[%d] is insufficient.", argc);
        return MEDIA_ERR;
    }
    bool isLoop = false;
    int32_t enableValue = strtol(param[0x1], nullptr, NUMBER_BASE);
    if (enableValue != 0) {
        isLoop = true;
    }
    AudioPlayerLoopMode loopMode = AudioPlayerLoopMode::AUDIO_PLAYER_INVAILD_LOOP;
    if (isLoop) {
        loopMode == AudioPlayerLoopMode::AUDIO_PLAYER_SINGLE_LOOP;
    }
    return AudioPlayerService::GetInstance()->SetPlayLoopMode(loopMode);
}

static int32_t GetCurrentPlaySource(int32_t argc, const char **param)
{
    MEDIA_UNUSED(param);
    MEDIA_UNUSED(argc);
    std::string str = AudioPlayerService::GetInstance()->GetCurrentPlaySource();
    MEDIA_ERR_LOG("CurrentPlaySource = %s.", str.c_str());
    return MEDIA_OK;
}

static int32_t GetDuration(int32_t argc, const char **param)
{
    MEDIA_UNUSED(param);
    MEDIA_UNUSED(argc);
    int64_t duration = -1;
    int32_t ret = AudioPlayerService::GetInstance()->GetDuration(duration);
    MEDIA_ERR_LOG("CurrentPlaySource = %lld.", duration);
    return ret;
}

static int32_t SetBackgroundMode(int32_t argc, const char **param)
{
    MEDIA_UNUSED(param);
    MEDIA_UNUSED(argc);
    bool enable = false;
    int32_t enableValue = strtol(param[0x1], nullptr, NUMBER_BASE);
    if (enableValue != 0) {
        enable = true;
    }
    return AudioPlayerService::GetInstance()->SetBackgroundMode(enable);
}

static int32_t GetDumpInfo(int32_t argc, const char **param)
{
    PlayerDebugInfo *playerInfo = new PlayerDebugInfo();
    int32_t ret = AudioPlayerService::GetInstance()->GetDumpInfo(playerInfo);
    if (ret != 0) {
        delete playerInfo;
        MEDIA_ERR_LOG("player get DumpInfo failed:%d", ret);
        return MEDIA_ERR;
    }
    delete playerInfo;
    return MEDIA_OK;
}

static FuncsMap g_audioPlayFuncs[] = {
    { "start", StartAudioPlay },
    { "stop", StopAudioPlay },
    { "seek", SeekAudioPlay },
    { "pause", PauseAudioPlay },
    { "resume", ResumeAudioPlay },
    { "playnext", PlayNext },
    { "playprve", PlayPrev },
    { "getcurrenttime", GetCurrentTime },
    { "getplaystatus", GetPlayStatus },
    { "singleloop", SetSingleLoop },
    { "getcurrentsource", GetCurrentPlaySource },
    { "getduration", GetDuration },
    { "setbackgroundmode", SetBackgroundMode },
    { "dumpinfo", GetDumpInfo},
};

int32_t AudioPlayerSampleTest(int32_t argc, const char *argv[])
{
    MEDIA_UNUSED(argc);
    MEDIA_INFO_LOG("enter");

    MEDIA_INFO_LOG("input param:%s", argv[0]);

    int32_t ret = -1;
    for (uint32_t i = 0; i < ARRAY_SIZE(g_audioPlayFuncs); i++) {
        if (strcmp(g_audioPlayFuncs[i].cmd, argv[0]) == 0) {
            ret = g_audioPlayFuncs[i].func(argc, argv);
            MEDIA_INFO_LOG("%s execute %s", g_audioPlayFuncs[i].cmd, (ret != 0) ? "failed" : "success");
            break;
        }
    }
    MEDIA_INFO_LOG("exit");
    return 0;
}
#ifdef __cplusplus
};
#endif
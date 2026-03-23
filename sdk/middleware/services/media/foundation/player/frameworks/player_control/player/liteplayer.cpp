/*
 * Copyright (c) 2020-2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "liteplayer.h"

#ifdef SUPPORT_SYS_PRCTL
#include <sys/prctl.h>
#endif
#include <unistd.h>
#include "player_check.h"
#include "hi_liteplayer_err.h"
#include "liteplayer_comm.h"
#if !defined(UNSUPPORT_AUDIO_DECODER) || !defined(UNSUPPORT_VIDEO_DECODER)
#include "decoder.h"
#endif
#include "media_mem.h"

#if !defined(UNSUPPORT_AUDIO_DECODER) || !defined(UNSUPPORT_VIDEO_DECODER)
using OHOS::Media::Decoder;
#endif
using OHOS::Media::PlayerSource;
using OHOS::Media::SinkManager;

namespace OHOS {
namespace Media {
/* DEFAULT_VID_BUFSIZE: 2 * 1024 * 1024 */
const uint32_t DEFAULT_VID_BUFSIZE = 2097152;
/* DEFAULT_AUD_BUFSIZE: 256 * 1024 */
const uint32_t DEFAULT_AUD_BUFSIZE = 262144;

const uint32_t GET_BUFFER_TIMEOUT_MS = 0u;
const uint32_t RENDER_FULL_SLEEP_TIME_US = 10000u;
// 1. waiting app response playback complete callback event
// 2. waiting interval time for loop playback
const uint32_t RENDER_EOS_SLEEP_TIME_US = 3000000u;
const uint32_t WAIT_BUFFER_EMPTY_SLEEP_TIME_US = 1000000u;
const uint32_t WAIT_PAUSE_TIME_OUT_US = 1000000u;
const uint32_t DECODER_DEFAULT_WIDTH = 1920;
const uint32_t DECODER_DEFAULT_HEIGHT = 1080;
const uint32_t QUEUE_BUFFER_FULL_SLEEP_TIME_US = 10000u;
const uint32_t NO_DATA_READ_SLEEP_TIME_US = 5000u;
const uint32_t MAX_EVENT_MESSAGE_NUM = 128;

#if defined(ENABLE_UIKIT) || defined(ENABLE_LVGL)
const uint32_t REQUEST_BUFFER_RETRY_WAIT_TIME_US = 10000u;
const float YCBCR420SP_BUFFER_SIZE_COEFFICIENT = 1.5;
// maximum number of consecutive surface buffer request failures retries(converted to time: 100 * 10ms = 1s)
const uint32_t MAX_CONTINOUS_REQUEST_SURFACE_BUFFER_FAIL_COUNT = 100;
#endif
#ifdef DEBUG_LOW_POWER
const uint32_t RENDER_FULL_SLEEP_THRESHOLD_MS_MP3_AACLC_OPUS = 5;
const uint32_t RENDER_FULL_SLEEP_THRESHOLD_MS_PCM = 25;
const uint32_t RENDER_FULL_SLEEP_THRESHOLD_MS_FLAC = 50;
#endif
// In a single-process system, only one thread can work at a time.
// Subthreads need to give CPU resources to the main thread to switch to the playback state.
const uint32_t WAIT_MAIN_THREAD_SWITCH_TO_PLAY_STATUS_TIME_US = 10000u;

static AudioCodecFormat g_fromCodecFormatToAudioManager[CODEC_BUT + 1] = {
    FORMAT_INVALID, FORMAT_INVALID, FORMAT_INVALID, FORMAT_INVALID,
    AAC_LC, G711A, G711U, PCM, MP3, G726, OPUS, FLAC, VORBIS, APE, SILK, SBC, FORMAT_INVALID
};
static EsFrameType g_fromDemuxTypeToSinkType[FRAME_TYPE_BUT + 1] = {
    ESFRAME_TYPE_NONE, ESFRAME_TYPE_AUDIO, ESFRAME_TYPE_VIDEO, ESFRAME_TYPE_IMAGE, ESFRAME_TYPE_SUB,
    ESFRAME_TYPE_DATA, ESFRAME_INVALID_TYPE
};

static CodecFormatAndMimePair g_avCodecFormatInfo[CODEC_BUT + 1] = {
    { CODEC_H264, MEDIA_MIMETYPE_VIDEO_AVC },
    { CODEC_H265, MEDIA_MIMETYPE_VIDEO_HEVC },
    { CODEC_JPEG, MEDIA_MIMETYPE_IMAGE_JPEG },
    { CODEC_MJPEG, MEDIA_MIMETYPE_IMAGE_MJPEG },
    { CODEC_AAC, MEDIA_MIMETYPE_AUDIO_AAC },
    { CODEC_G711A, MEDIA_MIMETYPE_AUDIO_G711A },
    { CODEC_G711U, MEDIA_MIMETYPE_AUDIO_G711U },
    { CODEC_PCM, MEDIA_MIMETYPE_AUDIO_PCM },
    { CODEC_MP3, MEDIA_MIMETYPE_AUDIO_MP3 },
    { CODEC_G726, MEDIA_MIMETYPE_AUDIO_G726 },
    { CODEC_BUT, MEDIA_MIMETYPE_INVALID },
};

namespace {
/* playing position notify interval in ms */
const uint32_t DEFAULT_POS_NOFITY_INTERVAL = 300;
}

#ifdef DEBUG_LOW_POWER
const int32_t US_2_MS = 1000;
#endif

void PlayerControl::RecordTime(struct timeval &timeStart)
{
    gettimeofday(&timeStart, nullptr);
}

uint64_t PlayerControl::CalcTimeDiffUsPrint(const struct timeval &timeStart, struct timeval &timeEnd,
    std::string outString, bool showlog)
{
    gettimeofday(&timeEnd, nullptr);
    const uint64_t timeUs = 1000000;
    uint64_t diffSecToUs = 0;
    bool isOverflow =
        UIntMulIsOverflow64(static_cast<uint64_t>(timeEnd.tv_sec - timeStart.tv_sec), timeUs, &diffSecToUs);
    if (isOverflow) {
        MEDIA_ERR_LOG("PlayerControl::CalcTimeDiffUsPrint calculate reslut overflow!");
        return INVALID_TIME;
    }
    uint64_t diffUsec = static_cast<uint64_t>(timeEnd.tv_usec - timeStart.tv_usec);
    uint64_t diffTimeUS = diffSecToUs + diffUsec;
    if (showlog) {
        MEDIA_INFO_LOG("%s elapsed time: %lld us", outString.c_str(), diffTimeUS);
    }
    return diffTimeUS;
}

int32_t PlayerControl::GetPlayerControlDumpInfo(PlayerControlDebugInfo &playerControlInfo)
{
    playerControlInfo.currentPosition = currentPosition_;
    playerControlInfo.hasRenderAudioEos = hasRenderAudioEos_;
    playerControlInfo.hasRenderVideoEos = hasRenderVideoEos_;
    playerControlInfo.isAudioStarted = isAudioStarted_;
    playerControlInfo.isVideoStarted = isVideoStarted_;
    playerControlInfo.offloadAudio = offloadAudio_;
    playerControlInfo.outDevice = outDeviceId_;
    playerControlInfo.sessionId = audioSession_;
    return 0;
}

int32_t PlayerControl::GetPlayerVideoDumpInfo(PlayerVideoDebugInfo &videoDebugInfo)
{
    videoDebugInfo.frameBufCnt = vdecAttr_.frameBufCnt;
    videoDebugInfo.u32Height = vdecAttr_.maxHeight;
    videoDebugInfo.u32Width = vdecAttr_.maxWidth;
    videoDebugInfo.enVideoType = vdecAttr_.type;
    return 0;
}

int32_t PlayerControl::GetPlayerAudioDumpInfo(PlayerAudioDebugInfo &audioDebugInfo)
{
    audioDebugInfo.bitWidth = adecAttr_.bitWidth;
    audioDebugInfo.enAudioType = adecAttr_.type;
    return 0;
}

int32_t PlayerControl::GetPlayerFileDumpInfo(PlayerFileDebugInfo &fileDebugInfo)
{
    fileDebugInfo.filePath = filePath_;
    fileDebugInfo.fd = fd_;
    fileDebugInfo.offset = offset_;
    return 0;
}

void PlayerControl::PrintKpiTime()
{
#ifdef PLAYER_KPI_ON
    if (!recordFirstFrameCost_ && currentPosition_ >= 0) {
        std::string startPlay("KPI_PlayNewFileTime");
        CalcTimeDiffUsPrint(startPlayStartTime_, startPlayEndTime_, startPlay, KPI_PRINT);
        recordFirstFrameCost_ = true;
    }
#endif
}

PlayerControl::PlayerControl()
    : isInited_(false),
      isPlayEnd_(false),
      isAudPlayEos_(false),
      cachedPacket_(false),
      lastRendPos_(0),
      lastNotifyTime_(0),
      lastReadPktStrmIdx_(-1),
      lastReadPktPts_(0),
      lastSendPktPts_(0),
      speed_(TPLAY_SPEED_BASE),
      isVideoStarted_(false),
      isAudioStarted_(false),
      strmReadEnd_(false),
      isPlayErr_(false),
      lastSendAdecPts_(AV_INVALID_PTS),
      currentPosition_(0),
      paused_(false),
      schThreadExit_(false),
      loop_(false),
      hasRenderAudioEos_(false),
      hasRenderVideoEos_(false),
      renderSleepTime_(0),
      leftVolume_(-1.0f),
      rightVolume_(-1.0f),
      schMutex_(nullptr),
      schCond_(nullptr),
      pauseCond_(nullptr),
      schProcess_(nullptr),
      seekToTimeMs_(-1),
      sourceType_(SOURCE_TYPE_BUT),
      firstAudioFrameAfterSeek_(false),
      firstAudioFrameSend_(false),
      firstVideoFrameSend_(false),
      firstPosAfterSeek_(0),
      fd_(-1),
      offset_(0),
      playerSource_(nullptr),
      sinkManager_(nullptr),
#if defined(ENABLE_UIKIT) || defined(ENABLE_LVGL)
      surface_(nullptr),
      surfaceBuf_(nullptr),
#endif
      curState_(PLAY_STATUS_IDLE),
      offloadAudio_(false),
      audioSession_(0),
      streamType_(AUDIO_STREAM_MUSIC),
      recordFirstFrameCost_(false),
      renderEsTotalTime_(0),
      readPktTotalTime_(0),
      renderEsFrameCnt_(0),
      readPacketCnt_(0),
      outDeviceId_(OUT_BUILTIN_SPEAKER),
      headInSendSuccessInterval_(-1),
      tailInSendSuccessInterval_(-1),
      hasAudio_(false),
      hasVideo_(false),
      isAudio_(false),
      isVideo_(false)
{
    eventCallback_.player = nullptr;
    eventCallback_.callbackFun = nullptr;
    if (memset_s(&fmtFileInfo_, sizeof(fmtFileInfo_), 0, sizeof(FormatFileInfo)) != EOK) {
        MEDIA_ERR_LOG("memset_s failed");
    }
    fmtFileInfo_.s32UsedAudioStreamIndex = HI_DEMUXER_NO_MEDIA_STREAM;
    fmtFileInfo_.s32UsedVideoStreamIndex = HI_DEMUXER_NO_MEDIA_STREAM;
    playerParam_.u32PlayPosNotifyIntervalMs = DEFAULT_POS_NOFITY_INTERVAL;
    playerParam_.u32VideoEsBufSize = DEFAULT_POS_NOFITY_INTERVAL;
    playerParam_.u32AudioEsBufSize = DEFAULT_POS_NOFITY_INTERVAL;
    playerParam_.u32VdecFrameBufCnt = DEFAULT_POS_NOFITY_INTERVAL;
    if (memset_s(&formatPacket_, sizeof(formatPacket_), 0, sizeof(FormatFrame)) != EOK) {
        MEDIA_ERR_LOG("memset_s failed");
    }
    streamAttr_.videoBufSize = 0;
    streamAttr_.audioBufSize = 0;
    adecAttr_.type = 0;
    adecAttr_.sampleRate = 0;
    adecAttr_.channel = 0;
    adecAttr_.bitWidth = 0;
    streamSource_.param.sampleRate = 0;
    streamSource_.param.sampleFormat = 0;
    streamSource_.param.channelCount = 0;
    streamSource_.param.channelLayout = 0;
    streamSource_.stream.ReadData = nullptr;
    streamSource_.stream.GetReadableSize = nullptr;
    streamSource_.stream.handle = nullptr;
    eventQueue_.clear();
    startPlayStartTime_.tv_sec = 0;
    startPlayStartTime_.tv_usec = 0;
    startPlayEndTime_.tv_sec = 0;
    startPlayEndTime_.tv_usec = 0;
    streamEndTime_.tv_sec = 0;
    streamEndTime_.tv_usec = 0;
    metaData_.key = 0;
    metaData_.size = 0;
#if !defined(UNSUPPORT_AUDIO_DECODER)
    audioDecoder_ = nullptr;
#endif
#if !defined(UNSUPPORT_VIDEO_DECODER)
    videoDecoder_ = nullptr;
#endif
}

PlayerControl::~PlayerControl()
{
    (void)Deinit();
    eventQueue_.clear();
}

int32_t PlayerControl::InitAttr(const PlayerControlParam &createParam)
{
    if (memset_s(&fmtFileInfo_, sizeof(fmtFileInfo_), 0x00, sizeof(FormatFileInfo)) != EOK) {
        MEDIA_ERR_LOG("memset_s failed");
        return HI_ERR_PLAYERCONTROL_MEM_SET;
    }
    fmtFileInfo_.s32UsedAudioStreamIndex = HI_DEMUXER_NO_MEDIA_STREAM;
    fmtFileInfo_.s32UsedVideoStreamIndex = HI_DEMUXER_NO_MEDIA_STREAM;
    streamAttr_.playMode = RENDER_MODE_NORMAL;
    streamAttr_.videoBufSize = DEFAULT_VID_BUFSIZE;
    streamAttr_.audioBufSize = DEFAULT_AUD_BUFSIZE;

    lastReadPktPts_ = 0;
    lastSendPktPts_ = 0;
    CHECK_FAILED_PRINT(memcpy_s(&playerParam_, sizeof(playerParam_), &createParam, sizeof(PlayerControlParam)), EOK,
        "copy playerParam_ fail");
    playerParam_.u32PlayPosNotifyIntervalMs = (createParam.u32PlayPosNotifyIntervalMs == 0) ?
        DEFAULT_POS_NOFITY_INTERVAL :
        createParam.u32PlayPosNotifyIntervalMs;
    return HI_SUCCESS;
}

int32_t PlayerControl::Init(const PlayerControlParam &createParam)
{
    MEDIA_INFO_LOG("Init in");
    CHECK_TRUE_CONDITION_RETURN(isInited_, HI_SUCCESS, "already be inited");
    CHECK_FAILED_RETURN(InitAttr(createParam), HI_SUCCESS, HI_FAILURE, "InitAttr failed");
    schMutex_ = MediaMutexCreate(nullptr);
    if (schMutex_ == nullptr) {
        MEDIA_ERR_LOG("create mutex failed");
        return HI_FAILURE;
    }
    schCond_ = MediaThreadCondCreate();
    if (schCond_ == nullptr) {
        MEDIA_ERR_LOG("create cond failed");
        MediaMutexDestroy(&schMutex_);
        return HI_FAILURE;
    }
    pauseCond_ = MediaThreadCondCreate();
    if (pauseCond_ == nullptr) {
        MEDIA_ERR_LOG("create pause cond failed");
        MediaMutexDestroy(&schMutex_);
        return HI_FAILURE;
    }
    isInited_ = true;
#ifdef PLAYER_KPI_ON
    RecordTime(startPlayStartTime_);
#endif
    return HI_SUCCESS;
}

int32_t PlayerControl::Deinit()
{
    if (!isInited_) {
        return HI_SUCCESS;
    }
    DestroyDecoder();
    MediaMutexDestroy(&schMutex_);
    MediaThreadCondDestroy(&schCond_);
    MediaThreadCondDestroy(&pauseCond_);
    isInited_ = false;
    return HI_SUCCESS;
}

int32_t PlayerControl::RegCallback(PlayerCtrlCallbackParam &eventCallback)
{
    MsgInfo msg = {};
    msg.what = PLAYERCONTROL_MSG_REGCALLBACK;
    msg.msgData = static_cast<void *>(&eventCallback);
    msg.msgDataLen = 0;
    return OnMessageReceived(msg);
}

int32_t PlayerControl::SetDataSource(const std::string filepath)
{
    MsgInfo msg = {};
    msg.what = PLAYERCONTROL_MSG_SET_DATASOURCE_URI;
    msg.msgData = const_cast<void *>(static_cast<const void *>(filepath.c_str()));
    msg.msgDataLen = 0;
    return OnMessageReceived(msg);
}

int32_t PlayerControl::SetDataSource(SourceFdParam &fdSrc)
{
    MsgInfo msg = {};
    msg.what = PLAYERCONTROL_MSG_SET_DATASOURCE_FD;
    msg.msgData = static_cast<void *>(&fdSrc);
    msg.msgDataLen = 0;
    return OnMessageReceived(msg);
}

int32_t PlayerControl::SetDataSource(StreamSourceParam &streamSource)
{
    MsgInfo msg = {};
    msg.what = PLAYERCONTROL_MSG_SET_DATASOURCE_STREAM;
    msg.msgData = static_cast<void *>(&streamSource);
    msg.msgDataLen = 0;
    return OnMessageReceived(msg);
}

int32_t PlayerControl::Prepare()
{
    MsgInfo msg = {};
    msg.what = PLAYERCONTROL_MSG_PREPARE;
    return OnMessageReceived(msg);
}

int32_t PlayerControl::Play()
{
    int32_t ret;
    MsgInfo msg = {};
    msg.what = PLAYERCONTROL_MSG_PLAY;
    ret = OnMessageReceived(msg);
    CHECK_FAILED_PRINT(ret, HI_SUCCESS, "play failed");
    return ret;
}

int32_t PlayerControl::SetVolume(const VolumeAttr &volumeAttr)
{
    VolumeAttr attr = volumeAttr;
    MsgInfo msg = {};
    msg.what = PLAYERCONTROL_MSG_SET_VOLUME;
    msg.msgData = static_cast<void *>(&attr);
    msg.msgDataLen = 0;
    return OnMessageReceived(msg);
}

int32_t PlayerControl::Stop()
{
    MsgInfo msg = {};
    msg.what = PLAYERCONTROL_MSG_STOP;
    return OnMessageReceived(msg);
}

int32_t PlayerControl::Pause()
{
    MsgInfo msg = {};
    msg.what = PLAYERCONTROL_MSG_PAUSE;
    return OnMessageReceived(msg);
}

int32_t PlayerControl::Seek(int64_t timeInMs)
{
    if (fmtFileInfo_.enVideoType == CODEC_JPEG) {
        MEDIA_ERR_LOG("seek action not support for play picture");
        return HI_ERR_PLAYERCONTROL_NOT_SUPPORT;
    }
    if ((fmtFileInfo_.s64Duration > 0) && (fmtFileInfo_.s64Duration < timeInMs)) {
        return HI_ERR_PLAYERCONTROL_ILLEGAL_PARAM;
    }
    MediaMutexLock(schMutex_);
    seekToTimeMs_ = timeInMs;
    MediaThreadCondSignal(schCond_);
    MediaMutexUnLock(schMutex_);
    return HI_SUCCESS;
}

int32_t PlayerControl::GetFileInfo(FormatFileInfo &formatInfo)
{
    MsgInfo msg = {};
    msg.what = PLAYERCONTROL_MSG_GETFILEINFO;
    msg.msgData = &formatInfo;
    msg.msgDataLen = sizeof(FormatFileInfo);
    return OnMessageReceived(msg);
}

int32_t PlayerControl::SetMedia(PlayerControlStreamAttr &mediaAttr)
{
    MsgInfo msg = {};
    msg.what = PLAYERCONTROL_MSG_SETATTR;
    msg.msgData = &mediaAttr;
    msg.msgDataLen = sizeof(PlayerControlStreamAttr);
    return OnMessageReceived(msg);
}

#if defined(ENABLE_UIKIT) || defined(ENABLE_LVGL)
#if defined(ENABLE_UIKIT)
int32_t PlayerControl::SetSurface(Surface *surface)
#elif defined(ENABLE_LVGL)
int32_t PlayerControl::SetSurface(LvSurfaceView *surface)
#endif
{
    surface_ = surface;
    return 0;
}
#endif

int32_t PlayerControl::SetPlaybackSpeed(float speed)
{
    if (speed == TPLAY_SPEED_BASE) {
        return Play();
    }
    float setSpeed = speed;
    MsgInfo msg = {};
    msg.what = PLAYERCONTROL_MSG_SETSPEED;
    msg.msgData = static_cast<void *>(&setSpeed);
    msg.msgDataLen = 0;
    return OnMessageReceived(msg);
}

void PlayerControl::ClearCachePacket()
{
    if (cachedPacket_) {
        if (formatPacket_.data != nullptr) {
            if (playerSource_ != nullptr && playerSource_->FreeFrame(formatPacket_) != 0) {
                MEDIA_ERR_LOG("FreeFrame failed \n");
            }
            formatPacket_.data = nullptr;
            formatPacket_.len = 0;
            formatPacket_.trackId = -1;
        }
        /* end packet is null do not need to demux free */
        cachedPacket_ = false;
    }
}

void PlayerControl::OnVideoEndOfStream()
{
    if (!hasAudio_ && (lastSendVdecPts_ > AV_INVALID_PTS)) {
        EventCallback(PLAYERCONTROL_EVENT_PROGRESS, &lastSendVdecPts_);
    }
    isVidPlayEos_ = true;
    isPlayEnd_ = IsPlayEos() ? true : isPlayEnd_;
}

void PlayerControl::EventProcess(EventCbType event)
{
    PlayerControlError playerError;
    MEDIA_DEBUG_LOG("handleEvent %d", event);
    switch (event) {
        case EVNET_VIDEO_PLAY_EOS:
            OnVideoEndOfStream();
            break;
        case EVNET_VIDEO_PLAY_SOS:
            MEDIA_INFO_LOG("video sos recv");
            break;
        case EVNET_AUDIO_PLAY_EOS:
            isAudPlayEos_ = true;
            isPlayEnd_ = IsPlayEos() ? true : isPlayEnd_;
            if (lastSendAdecPts_ > AV_INVALID_PTS) {
                EventCallback(PLAYERCONTROL_EVENT_PROGRESS, &lastSendAdecPts_);
            }
            break;
        case EVNET_VIDEO_RUNNING_ERR:
        case EVNET_AUDIO_RUNNING_ERR:
            playerError = PLAYERCONTROL_ERROR_AUD_PLAY_FAIL;
            isPlayErr_ = true;
            EventCallback(PLAYERCONTROL_EVENT_ERROR, &playerError);
            break;
        default:
            break;
    }
}

void PlayerControl::EventQueueProcess(void)
{
    size_t queSize;
    PalayControlEventItem *item = nullptr;

    queSize = eventQueue_.size();
    if (queSize > MAX_EVENT_MESSAGE_NUM) {
        MEDIA_WARNING_LOG("mesaage except, num:%u", queSize);
    }
    for (size_t i = 0; i < queSize && i < MAX_EVENT_MESSAGE_NUM; i++) {
        item = &eventQueue_[i];
        EventProcess(item->event);
    }
    eventQueue_.clear();
}

int32_t PlayerControl::OnPlayControlEvent(void *priv, const EventCbType event) const
{
    // audio play eos event needs to be handled immediately
    if (event == EVNET_AUDIO_PLAY_EOS) {
        MEDIA_INFO_LOG("Received audio play eos event");
        EventCallback(PLAYERCONTROL_EVENT_EOF, nullptr);
        return HI_SUCCESS;
    } else if (event == EVNET_AUDIO_BUFFER_EMPTY) {
        EventCallback(PLAYERCONTROL_BUFFER_EMPTY, nullptr);
        return HI_SUCCESS;
    } else if (event == EVNET_OUTPUT_DEVICE_CHANGED) {
        PlayerControl *player = static_cast<PlayerControl *>(priv);
        player->RecordLastPosition();
        return HI_SUCCESS;
    } else if (event == EVNET_AUDIO_RUNNING_ERR ||
        event == EVNET_VIDEO_RUNNING_ERR) {
        PlayerControlError playerError = PLAYERCONTROL_ERROR_AUD_PLAY_FAIL;
        EventCallback(PLAYERCONTROL_EVENT_ERROR, &playerError);
        return HI_SUCCESS;
    }

    PalayControlEventItem item;
    PlayerControl *player = static_cast<PlayerControl *>(priv);

    item.event = event;
    player->eventQueue_.push_back(item);
    return HI_SUCCESS;
}

int32_t PlayerControl::PauseResume(void)
{
    CHECK_NULL_RETURN(sinkManager_, HI_ERR_PLAYERCONTROL_NULL_PTR, "sinkManager_ nullptr");
    MediaMutexLock(schMutex_);
    int32_t ret = sinkManager_->Resume();
    if (ret != HI_SUCCESS) {
        if (ret != NOT_FOUND_ACTIVE_STREAME) {
            MEDIA_ERR_LOG("resume failed");
            hasVideo_ ? NotifyError(PLAYERCONTROL_ERROR_VID_PLAY_FAIL) : NotifyError(PLAYERCONTROL_ERROR_AUD_PLAY_FAIL);
        }
        MediaMutexUnLock(schMutex_);
        return ret;
    }
    paused_ = false;
    // played to the end no need to signal
    if (!IsPlayEos()) {
        MediaThreadCondSignal(schCond_);
    }
    MediaMutexUnLock(schMutex_);
    return HI_SUCCESS;
}

int32_t PlayerControl::CheckMediaInfo(void) const
{
    if (fmtFileInfo_.s32UsedAudioStreamIndex >= HI_DEMUXER_RESOLUTION_CNT ||
        fmtFileInfo_.s32UsedVideoStreamIndex >= HI_DEMUXER_RESOLUTION_CNT) {
        MEDIA_ERR_LOG("input file error video track: %d audio %d", fmtFileInfo_.s32UsedVideoStreamIndex,
            fmtFileInfo_.s32UsedAudioStreamIndex);
        return HI_ERR_PLAYERCONTROL_DEMUX_ERROR;
    }
    return HI_SUCCESS;
}

int32_t PlayerControl::GetVideoResolution(int32_t streamIdx, StreamResolution &resolution)
{
    if (streamIdx == HI_DEMUXER_NO_MEDIA_STREAM) {
        MEDIA_ERR_LOG("stream id [%d] in invalid", streamIdx);
        return HI_FAILURE;
    }

    for (uint32_t i = 0; i < HI_DEMUXER_RESOLUTION_CNT; i++) {
        if (fmtFileInfo_.stSteamResolution[i].s32VideoStreamIndex == streamIdx) {
            resolution.enVideoType = fmtFileInfo_.stSteamResolution[i].enVideoType;
            resolution.s32VideoStreamIndex = fmtFileInfo_.stSteamResolution[i].s32VideoStreamIndex;
            resolution.u32Width = fmtFileInfo_.stSteamResolution[i].u32Width;
            resolution.u32Height = fmtFileInfo_.stSteamResolution[i].u32Height;
            return HI_SUCCESS;
        }
    }

    MEDIA_ERR_LOG("not found stream[%d] in file", streamIdx);
    return HI_FAILURE;
}

int32_t PlayerControl::SetVDecAttr(void)
{
    StreamResolution resolution = {};
    if (GetVideoResolution(fmtFileInfo_.s32UsedVideoStreamIndex, resolution) != HI_SUCCESS) {
        MEDIA_ERR_LOG("GetVideoResolution failed");
        return HI_FAILURE;
    }
    vdecAttr_.maxWidth = resolution.u32Width;
    vdecAttr_.maxHeight = resolution.u32Height;
    vdecAttr_.type = resolution.enVideoType;
    vdecAttr_.frameBufCnt = playerParam_.u32VdecFrameBufCnt;
    return HI_SUCCESS;
}

int32_t PlayerControl::SetADecAttr(void)
{
    adecAttr_.bitWidth = AUDIO_BITWIDTH;
    adecAttr_.channel = fmtFileInfo_.u32AudioChannelCnt;
    adecAttr_.sampleRate = fmtFileInfo_.u32SampleRate;
    adecAttr_.type = fmtFileInfo_.enAudioType;
    return HI_SUCCESS;
}

int32_t PlayerControl::SetStreamAttr()
{
    if (hasVideo_) {
        StreamResolution resolution = {};
        if (GetVideoResolution(fmtFileInfo_.s32UsedVideoStreamIndex, resolution) != HI_SUCCESS) {
            MEDIA_ERR_LOG("GetVideoResolution failed");
            return HI_ERR_PLAYERCONTROL_ILLEGAL_PARAM;
        }
        streamAttr_.videoBufSize = playerParam_.u32VideoEsBufSize;
    }

    if (hasAudio_) {
        streamAttr_.audioBufSize =
            (playerParam_.u32AudioEsBufSize) ? (playerParam_.u32AudioEsBufSize) : streamAttr_.audioBufSize;
    }

    return HI_SUCCESS;
}

int32_t PlayerControl::SetDecoderAndStreamAttr(void)
{
    int ret;

    if (hasAudio_) {
        ret = SetADecAttr();
        CHECK_FAILED_RETURN(ret, HI_SUCCESS, ret, "SetADecAttr failed");
    }

    if (hasVideo_) {
        ret = SetVDecAttr();
        CHECK_FAILED_RETURN(ret, HI_SUCCESS, ret, "SetVDecAttr failed");
    }

    ret = SetStreamAttr();
    CHECK_FAILED_RETURN(ret, HI_SUCCESS, ret, "SetStreamAttr failed");
    return HI_SUCCESS;
}

#if !defined(UNSUPPORT_VIDEO_DECODER) || !defined(UNSUPPORT_AUDIO_DECODER)
static AvCodecMime TransformCodecFormatToAvCodecMime(CodecFormat format)
{
    AvCodecMime mime = MEDIA_MIMETYPE_INVALID;
    uint32_t size = sizeof(g_avCodecFormatInfo) / sizeof(CodecFormatAndMimePair);

    for (uint32_t i = 0; i < size; i++) {
        if (g_avCodecFormatInfo[i].format == format) {
            mime = g_avCodecFormatInfo[i].mime;
            break;
        }
    }

    return mime;
}
#endif

#ifndef UNSUPPORT_VIDEO_DECODER
static void GetCurVideoSolution(const FormatFileInfo &info, uint32_t &width, uint32_t &height)
{
    for (int i = 0; i < HI_DEMUXER_RESOLUTION_CNT; i++) {
        if (info.stSteamResolution[i].s32VideoStreamIndex == info.s32UsedVideoStreamIndex) {
            width = info.stSteamResolution[i].u32Width;
            height = info.stSteamResolution[i].u32Height;
            break;
        }
    }
}
#endif

#ifndef UNSUPPORT_AUDIO_DECODER
static std::string GetAudioNameByAvCodecMime(AvCodecMime mime)
{
    std::string audioName = "codec.unknow.soft.decoder";
    switch (mime) {
        case MEDIA_MIMETYPE_AUDIO_AAC:
            audioName = "codec.aac.soft.decoder";
            break;
        case MEDIA_MIMETYPE_AUDIO_MP3:
            audioName = "codec.mp3.soft.decoder";
            break;
        default:
            MEDIA_ERR_LOG("not support codec type:%d", mime);
            break;
    }
    return audioName;
}

int32_t PlayerControl::AudioDecoderStart(void)
{
    AvCodecMime mime = TransformCodecFormatToAvCodecMime(fmtFileInfo_.enAudioType);
    if (mime == MEDIA_MIMETYPE_INVALID) {
        MEDIA_ERR_LOG("DecoderStart not support codec:%d", fmtFileInfo_.enAudioType);
        return -1;
    }
    audioDecoder_ = std::make_shared<Decoder>();
    CHECK_NULL_RETURN(audioDecoder_, -1, "new decoder failed");
    AvAttribute attr;
    attr.type = AUDIO_DECODER;
    attr.adecAttr.mime = mime;
    attr.adecAttr.priv = nullptr;
    attr.adecAttr.bufSize = 0x400; /* 1024 */
    attr.adecAttr.channelCnt = (mime == MEDIA_MIMETYPE_AUDIO_PCM) ? fmtFileInfo_.u32AudioChannelCnt : 0;
    const std::string audioName = GetAudioNameByAvCodecMime(mime);
    int32_t ret = audioDecoder_->CreateHandle(audioName, attr);
    CHECK_FAILED_RETURN(ret, 0, -1, "create audio decoder failed");
    ret = audioDecoder_->StartDec();
    CHECK_FAILED_RETURN(ret, 0, -1, "start audio decoder failed");
    MEDIA_INFO_LOG("audio decoder started");

    return 0;
}
#endif

#if !defined(UNSUPPORT_VIDEO_DECODER)
int32_t PlayerControl::VideoDecoderStart(void)
{
    AvAttribute attr;
    uint32_t width = DECODER_DEFAULT_WIDTH;
    uint32_t height = DECODER_DEFAULT_HEIGHT;

    AvCodecMime mime = TransformCodecFormatToAvCodecMime(fmtFileInfo_.enVideoType);
    if (mime == MEDIA_MIMETYPE_INVALID) {
        MEDIA_ERR_LOG("DecoderStart not support codec:%d", fmtFileInfo_.enVideoType);
        return -1;
    }
    GetCurVideoSolution(fmtFileInfo_, width, height);

    videoDecoder_ = std::make_shared<Decoder>();
    CHECK_NULL_RETURN(videoDecoder_, -1, "new decoder failed");
    attr.type = VIDEO_DECODER;
    attr.vdecAttr.mime = mime;
    attr.vdecAttr.priv = nullptr;
    attr.vdecAttr.bufSize = 0;
    attr.vdecAttr.maxWidth = width;
    attr.vdecAttr.maxHeight = height;
    const std::string videoName = "codec.avc.soft.decoder";
    int32_t ret = videoDecoder_->CreateHandle(videoName, attr);
    CHECK_FAILED_RETURN(ret, 0, -1, "create video decoder failed");
    ret = videoDecoder_->StartDec();
    CHECK_FAILED_RETURN(ret, 0, -1, "start video decoder failed");
    MEDIA_INFO_LOG("video decoder started");

    return 0;
}
#endif

int32_t PlayerControl::DecoderStart(void)
{
    if (!hasAudio_ && !hasVideo_) {
        MEDIA_ERR_LOG("not decoder need start");
        return -1;
    }

    MEDIA_INFO_LOG("audio stream id: %d, video stream id: %d", fmtFileInfo_.s32UsedAudioStreamIndex,
        fmtFileInfo_.s32UsedVideoStreamIndex);

    /* if offload audio is false, need audio decoder, not support */
    if (hasAudio_) {
        if (!offloadAudio_) {
            MEDIA_ERR_LOG("not support offload, need audio decoder, return failed");
            return -1;
        }
    }

    if (hasVideo_) {
#if !defined(UNSUPPORT_VIDEO_DECODER)
        if (VideoDecoderStart() != 0) {
            return -1;
        }
#endif
    }
    return 0;
}

void PlayerControl::DestroyDecoder() const
{
#ifndef UNSUPPORT_AUDIO_DECODER
    if (audioDecoder_ != nullptr) {
        audioDecoder_->DestroyHandle();
    }
#endif
#if !defined(UNSUPPORT_VIDEO_DECODER)
    if (videoDecoder_ != nullptr) {
        videoDecoder_->DestroyHandle();
    }
#endif
}

void PlayerControl::StopSinkAndDecoder()
{
    OutputInfo outInfo = {};
    EsFrameInfo frameInfo = {};
    if (sinkManager_ != nullptr) {
        sinkManager_->Stop();
#ifdef UNSUPPORT_AUDIO_DECODER
        while (sinkManager_->DequeReleaseEsFrame(true, frameInfo) == HI_SUCCESS) {
        }
#else
        if (audioDecoder_ != nullptr) {
            while (sinkManager_->DequeReleaseFrame(true, outInfo) == 0) {
                audioDecoder_->QueueOutputBuffer(outInfo, GET_BUFFER_TIMEOUT_MS);
            }
            audioDecoder_->StopDec();
        }
#endif
#if !defined(UNSUPPORT_VIDEO_DECODER)
        if (videoDecoder_ != nullptr) {
            while (sinkManager_->DequeReleaseFrame(false, outInfo) == 0) {
                videoDecoder_->QueueOutputBuffer(outInfo, GET_BUFFER_TIMEOUT_MS);
            }
            videoDecoder_->StopDec();
        }
#endif
    }

    isVideoStarted_ = false;
    isAudioStarted_ = false;
}

int32_t PlayerControl::AddAudioSink(void)
{
    CHECK_TRUE_CONDITION_RETURN(isAudioStarted_, HI_SUCCESS, "already started");
    SinkAttr attr;
    attr.sinkType = SINK_TYPE_AUDIO;
    attr.trackId = fmtFileInfo_.s32UsedAudioStreamIndex;
    attr.audAttr.format = g_fromCodecFormatToAudioManager[fmtFileInfo_.enAudioType];
    attr.audAttr.sampleRate = adecAttr_.sampleRate;
    attr.audAttr.channel = fmtFileInfo_.u32AudioChannelCnt;
    attr.audAttr.sessionID = audioSession_;
    attr.audAttr.streamType = streamType_;
    attr.audAttr.bitWidth = adecAttr_.bitWidth;
    attr.audAttr.sampleFmt = fmtFileInfo_.sampleFmt;
    MEDIA_INFO_LOG("AddAudioSink, format:%d, sampleRate:%d, channel:%d, session:%d, streamType:%d, bitWidth:%d",
        attr.audAttr.format, attr.audAttr.sampleRate, attr.audAttr.channel, attr.audAttr.sessionID,
        attr.audAttr.streamType, attr.audAttr.bitWidth);
    if (sinkManager_->AddNewSink(attr) != 0) {
        MEDIA_ERR_LOG("AddNewSink  failed");
        return -1;
    }
    isAudioStarted_ = true;
    return HI_SUCCESS;
}

int32_t PlayerControl::GetOffloadSupport(void)
{
    if (sinkManager_ == nullptr) {
        MEDIA_ERR_LOG("sinkManager_ is invalid");
        return -1;
    }
    AudioOffloadParam curOffloadInfo;
    curOffloadInfo.streamType = streamType_;
    curOffloadInfo.format = g_fromCodecFormatToAudioManager[fmtFileInfo_.enAudioType];
    curOffloadInfo.sampleRate = adecAttr_.sampleRate;
    curOffloadInfo.channel = fmtFileInfo_.u32AudioChannelCnt;
    curOffloadInfo.bitWidth = 0;
    MEDIA_INFO_LOG("GetOffloadSupport format:%d, audio:%d", curOffloadInfo.format, fmtFileInfo_.enAudioType);
    offloadAudio_ = sinkManager_->IsOffloadSupported(curOffloadInfo);
    MEDIA_INFO_LOG("%s audio offload", offloadAudio_ ? "supported" : "unsupported");
    return HI_SUCCESS;
}

int32_t PlayerControl::AddVideoSink()
{
    CHECK_TRUE_CONDITION_RETURN(isVideoStarted_, HI_SUCCESS, "already started");
    SinkAttr attr;
    attr.sinkType = SINK_TYPE_VIDEO;
    attr.trackId = fmtFileInfo_.s32UsedVideoStreamIndex;
    attr.vidAttr.width = fmtFileInfo_.u32Width;
    attr.vidAttr.height = fmtFileInfo_.u32Height;
    attr.vidAttr.frameRate = fmtFileInfo_.fFrameRate;
    attr.vidAttr.format = fmtFileInfo_.enVideoType;
#if defined(ENABLE_UIKIT) || defined(ENABLE_LVGL)
    attr.vidAttr.surface = surface_;
#endif
    if (sinkManager_->AddNewSink(attr) != 0) {
        MEDIA_ERR_LOG("AddNewSink  failed");
        return -1;
    }
    isVideoStarted_ = true;
    return HI_SUCCESS;
}

int32_t PlayerControl::SinkStart(void)
{
    int32_t ret;
    sinkManager_ = std::make_shared<SinkManager>();
    CHECK_NULL_RETURN(sinkManager_, HI_FAILURE, "new sinkManager_ nullptr");
    if (hasVideo_ && !isVideoStarted_) {
        ret = AddVideoSink();
        CHECK_FAILED_RETURN(ret, HI_SUCCESS, ret, "AddVideoSink failed");
        isVideoStarted_ = true;
    }
    if (hasAudio_ && !isAudioStarted_) {
        ret = AddAudioSink();
        CHECK_FAILED_RETURN(ret, HI_SUCCESS, ret, "AddAudioSink failed");
        isAudioStarted_ = true;
    }

    PlayEventCallback callback;
    GetPlayElementEventCallBack(callback);
    ret = sinkManager_->RegisterCallBack(callback);
    CHECK_FAILED_RETURN(ret, HI_SUCCESS, ret, "RegisterCallBack failed");
    /* get offload flag */
    GetOffloadSupport();

    if (metaData_.key == KEY_TYPE_EXTRADATA) {
        std::string key("extradata");
        ret = sinkManager_->SetParam(key, DATA_TYPE_PTR, metaData_.value.pValue, metaData_.size);
        CHECK_FAILED_RETURN(ret, HI_SUCCESS, ret, "sinkManager_ SetParam failed");
    }

    ret = sinkManager_->Start();
    CHECK_FAILED_RETURN(ret, HI_SUCCESS, ret, "sinkManager_ Start failed");

    if (leftVolume_ >= 0.0f || rightVolume_ >= 0.0f) {
        sinkManager_->SetVolume(leftVolume_, rightVolume_);
    }
    return HI_SUCCESS;
}

int32_t PlayerControl::ReadFrameFromSource(FormatFrame &fmtFrame)
{
    CHECK_NULL_RETURN(playerSource_, HI_ERR_PLAYERCONTROL_NULL_PTR, "playerSource_ nullptr");
    int ret = playerSource_->ReadFrame(fmtFrame);
    // Check consistency of fmtFrame
    if ((ret == HI_SUCCESS) && (fmtFrame.data != nullptr) && (fmtFrame.len == 0)) {
        MEDIA_ERR_LOG("ReadFrame ERROR : u32Size is invalid");
        ret = HI_FAILURE;
    }
    return ret;
}

// message begin
int32_t PlayerControl::DoRegCallback(const PlayerCtrlCallbackParam &eventCallback)
{
    if (eventCallback_.callbackFun != nullptr) {
        MEDIA_WARNING_LOG("you have reg callback,now the before callback fun pointer will be overwrite");
    }
    eventCallback_ = eventCallback;
    return HI_SUCCESS;
}

int32_t PlayerControl::DoSetDataSource(const char *filepath)
{
    filePath_ = std::string(filepath);
    sourceType_ = SOURCE_TYPE_URI;
    return HI_SUCCESS;
}

int32_t PlayerControl::DoSetDataSource(const SourceFdParam &fdSrc)
{
    fd_ = fdSrc.fd;
    offset_ = fdSrc.offset;
    sourceType_ = SOURCE_TYPE_FD;
    return HI_SUCCESS;
}

int32_t PlayerControl::DoSetDataSource(const StreamSourceParam &streamSource)
{
    streamSource_ = streamSource;
    sourceType_ = SOURCE_TYPE_STREAM;
    return HI_SUCCESS;
}

int32_t PlayerControl::DoPrepare(void)
{
    MEDIA_DEBUG_LOG("Process in");
    CHECK_NULL_RETURN(eventCallback_.callbackFun, HI_ERR_PLAYERCONTROL_CALLBACK_ERROR, "callbackFun nullptr");
    isPlayEnd_ = false;
    int32_t ret = SyncPrepare();
    CHECK_FAILED_RETURN(ret, HI_SUCCESS, HI_ERR_PLAYERCONTROL_OTHER, "SyncPrepare failed");
    if (playerParam_.u32PlayPosNotifyIntervalMs >= fmtFileInfo_.s64Duration) {
        MEDIA_WARNING_LOG("play postion notify interval %d ms oversize file duration, user will never receive notify",
            playerParam_.u32PlayPosNotifyIntervalMs);
    }
    MEDIA_DEBUG_LOG("Process out");
    return HI_SUCCESS;
}

#if !defined(UNSUPPORT_VIDEO_DECODER) || !defined(UNSUPPORT_AUDIO_DECODER)
static void InitOutputBuffer(OutputInfo &outInfo, CodecType type)
{
    if (outInfo.buffers != nullptr) {
        MediaMemFree(outInfo.buffers);
        outInfo.buffers = nullptr;
    }

    outInfo.bufferCnt = 0;
    outInfo.buffers = nullptr;
    outInfo.timeStamp = -1;
    outInfo.sequence = 0;
    outInfo.flag = 0;
    outInfo.type = type;
    outInfo.vendorPrivate = nullptr;
}
#endif

#ifndef UNSUPPORT_AUDIO_DECODER
void PlayerControl::ReleaseADecoderOutputFrame(void)
{
    if (audioDecoder_ == nullptr || sinkManager_ == nullptr) {
        return;
    }
    while (true) {
        OutputInfo outInfo;
        if (sinkManager_->DequeReleaseFrame(true, outInfo) != 0) {
            break;
        }
        audioDecoder_->QueueOutputBuffer(outInfo, GET_BUFFER_TIMEOUT_MS);
    }
}
#endif

void PlayerControl::ReleaseAudioSinkESFrame(void)
{
    CHECK_NULL_RETURN_NONE(sinkManager_, "sink manager is null");

    EsFrameInfo outInfo = {};
    while (sinkManager_->DequeReleaseEsFrame(true, outInfo) == HI_SUCCESS) {
    }
}

#if !defined(UNSUPPORT_VIDEO_DECODER)
void PlayerControl::ReleaseVDecoderOutputFrame(void) const
{
    if (videoDecoder_ == nullptr || sinkManager_ == nullptr) {
        return;
    }
    while (true) {
        OutputInfo outInfo;
        if (sinkManager_->DequeReleaseFrame(false, outInfo) != 0) {
            break;
        }
    }
}
#endif

void PlayerControl::ReportRenderPosition(void)
{
    int64_t position = -1;
    CHECK_NULL_RETURN_NONE(sinkManager_, "sinkManager_ nullptr");
    sinkManager_->GetRenderPosition(position);
    if (position >= 0 && currentPosition_ != position) {
        if (lastRendPos_ >= 0) {
            currentPosition_ = position - lastRendPos_ + firstPosAfterSeek_;
        } else {
            currentPosition_ = position;
        }
        EventCallback(PLAYERCONTROL_EVENT_PROGRESS, &currentPosition_);
    }
}

void PlayerControl::GetRenderFullSleepThresholdTime(AudioCodecFormat codecFormat)
{
#ifdef DEBUG_LOW_POWER
    if (codecFormat == PCM) {
        renderFullSleepThresholdMs_ = RENDER_FULL_SLEEP_THRESHOLD_MS_PCM;
    } else if (codecFormat == MP3 || codecFormat == AAC_LC || codecFormat == OPUS) {
        renderFullSleepThresholdMs_ = RENDER_FULL_SLEEP_THRESHOLD_MS_MP3_AACLC_OPUS;
    } else if (codecFormat == FLAC) {
        renderFullSleepThresholdMs_ = RENDER_FULL_SLEEP_THRESHOLD_MS_FLAC;
    }
#else
    MEDIA_UNUSED(codecFormat);
#endif
}

void PlayerControl::DataSchPreProcess(PlayerControl *play, bool *isContinue)
{
    if (play->IsPlayEos()) {
        int32_t info = PLAYERCONTROL_ERROR_AUD_PLAY_FAIL;
        play->EventCallback(PLAYERCONTROL_EVENT_ERROR, &info);
        MEDIA_ERR_LOG("player waiting for the EOS callback timed out.");
    }
    play->DoSeekIfNeed();

    play->renderSleepTime_ = 0;

    if (play->paused_) {
        // blocking wait ensures that the system goes into deep sleep for reducing power consumption
        int32_t info = PLAYERCONTROL_EVENT_PAUSED;
        play->EventCallback(PLAYERCONTROL_EVENT_PAUSED, &info);
        play->firstAudioFrameSend_ = false;
        play->firstVideoFrameSend_ = false;
        MediaThreadCondSignal(play->pauseCond_);
        MediaThreadCondWait(play->schCond_, play->schMutex_);
        *isContinue = true;
    }
}

void PlayerControl::DataSchPostProcess(PlayerControl *play)
{
    play->EventQueueProcess();

    play->ReportRenderPosition();

    play->PrintKpiTime();

    // process pure video eos
    if (play->hasVideo_ && !play->hasAudio_ && play->hasRenderVideoEos_) {
        MEDIA_INFO_LOG("Received video play eos flag");
        play->EventCallback(PLAYERCONTROL_EVENT_EOF, nullptr);
    }

    if (play->IsPlayEos()) {
        play->renderSleepTime_ = RENDER_EOS_SLEEP_TIME_US;
    }
}

#if !defined(UNSUPPORT_VIDEO_DECODER) || !defined(UNSUPPORT_AUDIO_DECODER)
inline static bool IsValidPacket(FormatFrame &packet)
{
    return (packet.data != nullptr && packet.len != 0) ? true : false;
}
#endif

int32_t PlayerControl::DequeueInputDataFromDemuxer(void)
{
    PlayerStatus state = GetCurState();
    if (state != PLAY_STATUS_PLAY && state != PLAY_STATUS_TPLAY) {
        MEDIA_ERR_LOG("not play state, goto sleep");
        renderSleepTime_ = WAIT_MAIN_THREAD_SWITCH_TO_PLAY_STATUS_TIME_US;
        return HI_FAILURE;
    }

    CHECK_TRUE_CONDITION_RETURN(strmReadEnd_, HI_SUCCESS, "read stream end");

#ifdef PLAYER_KPI_ON
    struct timeval readPacketStartTime;
    RecordTime(readPacketStartTime);
#endif

    int32_t ret;
    if (speed_ != TPLAY_SPEED_BASE) {
        ret = TplayReadPacket();
    } else {
        ret = ReadPacket();
    }
    if (ret == HI_RET_NODATA) {
        renderSleepTime_ = NO_DATA_READ_SLEEP_TIME_US;
        goto RE_SEND;
    } else if (ret != HI_SUCCESS) {
        goto RE_SEND;
    }

#ifdef PLAYER_KPI_ON
    ReadPacketAvgCostTime(readPacketStartTime);
#endif

    if (formatPacket_.data == nullptr && formatPacket_.len == 0) {
        if (speed_ < 0) {
            TPlayRewindToNormal();
            MEDIA_INFO_LOG("rewind end, resume to normal play");
            return HI_FAILURE;
        }
        strmReadEnd_ = true;
        formatPacket_.timestampUs = -1;
        gettimeofday(&streamEndTime_, nullptr);
        if (loop_) {
            seekToTimeMs_ = 0;
            return HI_FAILURE;
        }
    }

    isAudio_ = (formatPacket_.frameType == FRAME_TYPE_AUDIO) ? true : false;
    isVideo_ = (formatPacket_.frameType == FRAME_TYPE_VIDEO) ? true : false;

    return HI_SUCCESS;

RE_SEND:
    ClearCachePacket();
    return ret;
}

int32_t PlayerControl::QueueInputDataToAudioSink(void)
{
    CHECK_NULL_RETURN(sinkManager_, HI_FAILURE, "sink manager is null");
    CHECK_TRUE_CONDITION_RETURN(hasRenderAudioEos_, HI_FAILURE, "has render audio eos");

    if (strmReadEnd_) {
        sinkManager_->RenderEos(true);
    } else {
        CHECK_FALSE_CONDITION_RETURN(cachedPacket_, HI_FAILURE, "no cache packet");
    }

    EsFrameInfo outInfo = {};
    FillAudioFrameInfo(outInfo);
    int32_t ret = sinkManager_->RenderEsFrame(outInfo);
    if (ret == SINK_RENDER_FULL || ret == SINK_RENDER_DELAY) {
#ifdef DEBUG_LOW_POWER
        static uint32_t renderSuccessCount = 0;
        LowPowerProcess(outInfo.pts, renderSuccessCount, false);
#else
        if (ret == SINK_RENDER_FULL && fmtFileInfo_.s32UsedVideoStreamIndex < 0) {
            renderSleepTime_ = WAIT_BUFFER_EMPTY_SLEEP_TIME_US;
        } else {
            renderSleepTime_ = RENDER_FULL_SLEEP_TIME_US;
        }
#endif
        return ret;
    } else {
#ifdef DEBUG_LOW_POWER
        LowPowerProcess(outInfo.pts, renderSuccessCount, true);
#endif
        renderSleepTime_ = 0;
    }
    ReleaseAudioSinkESFrame();
    if (ret == SINK_RENDER_EOS) {
        hasRenderAudioEos_ = true;
        MEDIA_INFO_LOG("have render audio eos");
        if (IsPlayEos()) {
            isPlayEnd_ = true;
        }
    }
    if (firstAudioFrameAfterSeek_) {
        firstAudioFrameAfterSeek_ = false;
        sinkManager_->GetRenderPosition(lastRendPos_);
        firstPosAfterSeek_ = formatPacket_.timestampUs;
        MEDIA_INFO_LOG("push firstAudioFrameAfterSeek_ success, pts:%lld", formatPacket_.timestampUs);
    }
    if (formatPacket_.data != nullptr || formatPacket_.len != 0) {
        lastSendAdecPts_ = formatPacket_.timestampUs;
    }
    ClearCachePacket();
    return ret;
}

#ifndef UNSUPPORT_AUDIO_DECODER
void PlayerControl::QueueInputDataToAudioDecoder(void)
{
    CHECK_NULL_RETURN_NONE(audioDecoder_, "audio decoder is null");
    CHECK_TRUE_CONDITION_RETURN_NONE(cachedPacket_, "no cache packet");

    InputInfo inputData = { 0, nullptr, 0, 0 };
    CodecBufferInfo inBufInfo = {};

    int32_t ret = audioDecoder_->DequeInputBuffer(inputData, GET_BUFFER_TIMEOUT_MS);
    CHECK_FAILED_RETURN_NONE(ret, HI_SUCCESS, "deque input buffer failed");

    inBufInfo.addr = formatPacket_.data;
    inBufInfo.length = formatPacket_.len;
    inputData.bufferCnt = 1;
    inputData.buffers = &inBufInfo;
    inputData.pts = formatPacket_.timestampUs;
    inputData.flag = 0;
    ret = audioDecoder_->QueueInputBuffer(inputData, GET_BUFFER_TIMEOUT_MS);
    if (ret == CODEC_ERR_STREAM_BUF_FULL) {
        renderSleepTime_ = QUEUE_BUFFER_FULL_SLEEP_TIME_US;
        return;
    }
    if (firstAudioFrameAfterSeek_ && IsValidPacket(formatPacket_)) {
        firstAudioFrameAfterSeek_ = false;
        MEDIA_INFO_LOG("push firstAudioFrameAfterSeek_ success, pts:%lld", inputData.pts);
    }
    if (formatPacket_.data != nullptr || formatPacket_.len != 0) {
        lastSendAdecPts_ = formatPacket_.timestampUs;
    }
    ClearCachePacket();
}

void PlayerControl::DequeueOutputBufferAndRenderAudioFrame(void)
{
    CHECK_NULL_RETURN_NONE(audioDecoder_, "audio decoder is null");
    CHECK_NULL_RETURN_NONE(sinkManager_, "sink manager is null");
    CHECK_FAILED_RETURN_NONE(hasRenderAudioEos_, false, "has render audio eos");

    OutputInfo outInfo = {};
    int32_t ret = audioDecoder_->DequeueOutputBuffer(outInfo, GET_BUFFER_TIMEOUT_MS);
    if (ret != HI_SUCCESS) {
        InitOutputBuffer(outInfo, AUDIO_DECODER);
        if (ret == CODEC_RECEIVE_EOS && strmReadEnd_) {
            // all frame have been send to audio sink
            sinkManager_->RenderEos(true);
        }
    }

    ret = sinkManager_->RenderFrame(outInfo);
    if (ret == SINK_RENDER_FULL || ret == SINK_RENDER_DELAY) {
        renderSleepTime_ = RENDER_FULL_SLEEP_TIME_US;
    } else if (ret == SINK_QUE_EMPTY) {
        renderSleepTime_ = 0;
    }
    ReleaseADecoderOutputFrame();
    // all frame in frameCacheQue_ is processed
    if (ret == SINK_RENDER_EOS) {
        hasRenderAudioEos_ = true;
        MEDIA_INFO_LOG("have render audio eos");
        if (IsPlayEos()) {
            isPlayEnd_ = true;
        }
    }
}
#endif

#if !defined(UNSUPPORT_VIDEO_DECODER)
int32_t PlayerControl::QueueOutputBufferToVideoDecoder(void)
{
#if defined(ENABLE_UIKIT) || defined(ENABLE_LVGL)
    CHECK_NULL_RETURN(surface_, HI_FAILURE, "surface is null");
#endif
    CHECK_NULL_RETURN(sinkManager_, HI_FAILURE, "sink manager is null");
    CHECK_NULL_RETURN(videoDecoder_, HI_FAILURE, "video decoder is null");

    // 1. if video sink have cache, means have remain buffer not send to surface, don't need request buffer from surface
    bool haveCache = sinkManager_->HaveCacheFrame(false);
    if (haveCache) {
        MEDIA_DEBUG_LOG("have cache frame skip this");
        return HI_SUCCESS;
    }

    // 2. request video output buffer from surface
#if defined(ENABLE_UIKIT) || defined(ENABLE_LVGL)
#if defined(ENABLE_UIKIT)
    surfaceBuf_ = surface_->RequestBuffer(1); // 1 means RequestBuffer blockly
#elif defined(ENABLE_LVGL)
    surfaceBuf_ = LvRequestBuffer(reinterpret_cast<lv_obj_t *>(surface_), 0);
#endif
    if (surfaceBuf_ == nullptr) {
        if (continousRequestSurfaceBufferFailCount_ == MAX_CONTINOUS_REQUEST_SURFACE_BUFFER_FAIL_COUNT) {
            PalayControlEventItem item = { EVNET_VIDEO_RUNNING_ERR };
            eventQueue_.push_back(item);
            MEDIA_FATAL_LOG("continous request buffer fail count exceed upper limit");
            return REQUEST_SURFACE_BUFFER_FATAL;
        }
        MEDIA_DEBUG_LOG("no free surface buffer retry");
        renderSleepTime_ = REQUEST_BUFFER_RETRY_WAIT_TIME_US;
        continousRequestSurfaceBufferFailCount_++;
        return HI_FAILURE;
    }
#endif
    continousRequestSurfaceBufferFailCount_ = 0;

    // 3. queue output buffer to video decoder
    OutputInfo outInfo = {};
    outInfo.type = VIDEO_DECODER;
    outInfo.bufferCnt = 1;
    outInfo.buffers = (CodecBufferInfo *)MediaMemMalloc(sizeof(CodecBufferInfo));
    if (outInfo.buffers == nullptr) {
        MEDIA_ERR_LOG("malloc failed");
        return HI_FAILURE;
    }
    outInfo.buffers[0].type = BUFFER_TYPE_VIRTUAL;
#if defined(ENABLE_UIKIT) || defined(ENABLE_LVGL)
    outInfo.buffers[0].addr = reinterpret_cast<uint8_t *>(surfaceBuf_->addr);
    outInfo.buffers[0].length = surfaceBuf_->width * surfaceBuf_->height * YCBCR420SP_BUFFER_SIZE_COEFFICIENT;
    outInfo.vendorPrivate = surfaceBuf_;
#endif
    int32_t ret = videoDecoder_->QueueOutputBuffer(outInfo, GET_BUFFER_TIMEOUT_MS);
    if (ret != HI_SUCCESS) {
        MediaMemFree(outInfo.buffers);
        MEDIA_ERR_LOG("queue output buffer to video decoder failed: 0x%x", ret);
        return HI_FAILURE;
    }

    MediaMemFree(outInfo.buffers);
    return HI_SUCCESS;
}

void PlayerControl::QueueInputDataToVideoDecoder(void)
{
    CHECK_NULL_RETURN_NONE(videoDecoder_, "video decoder is null");
    CHECK_NULL_RETURN_NONE(sinkManager_, "sink manager is null");

    bool haveCache = sinkManager_->HaveCacheFrame(false);
    if (haveCache) {
        MEDIA_DEBUG_LOG("have cache frame skip this");
        return;
    }

    if (!strmReadEnd_) {
        CHECK_FALSE_CONDITION_RETURN_NONE(cachedPacket_, "no cache packet");
    }

    InputInfo inputData = { 0, nullptr, 0, 0 };
    CodecBufferInfo inBufInfo = {};

    int32_t ret = videoDecoder_->DequeInputBuffer(inputData, GET_BUFFER_TIMEOUT_MS);
    CHECK_FAILED_RETURN_NONE(ret, HI_SUCCESS, "deque input buffer failed");

    MEDIA_DEBUG_LOG("queue input buffer len: %u, pts: %lld", formatPacket_.len, formatPacket_.timestampUs);

    inBufInfo.addr = formatPacket_.data;
    inBufInfo.length = formatPacket_.len;
    inputData.bufferCnt = 1;
    inputData.buffers = &inBufInfo;
    inputData.pts = formatPacket_.timestampUs;
    inputData.flag = (formatPacket_.data == nullptr && formatPacket_.len == 0) ? STREAM_FLAG_EOS : 0;
    ret = videoDecoder_->QueueInputBuffer(inputData, GET_BUFFER_TIMEOUT_MS);
    if (ret == CODEC_ERR_STREAM_BUF_FULL) {
        renderSleepTime_ = QUEUE_BUFFER_FULL_SLEEP_TIME_US;
        return;
    }
    if (firstVideoFrameAfterSeek_ && IsValidPacket(formatPacket_)) {
        firstVideoFrameAfterSeek_ = false;
        MEDIA_INFO_LOG("push firstVideoFrameAfterSeek_ success, pts:%lld", inputData.pts);
    }
    if (formatPacket_.data != nullptr && formatPacket_.len != 0) {
        lastSendVdecPts_ = formatPacket_.timestampUs;
    }
    ClearCachePacket();
}

void PlayerControl::DequeueOutputBufferAndRenderVideoFrame(void)
{
    CHECK_NULL_RETURN_NONE(videoDecoder_, "video decoder is null");
    CHECK_NULL_RETURN_NONE(sinkManager_, "sink manager is null");
    CHECK_TRUE_CONDITION_RETURN_NONE(hasRenderVideoEos_, "has render video eos");

    OutputInfo outInfo = {};
    outInfo.buffers = (CodecBufferInfo *)MediaMemMalloc(sizeof(CodecBufferInfo));
    if (outInfo.buffers == nullptr) {
        MEDIA_ERR_LOG("malloc failed");
        return;
    }
    int ret = videoDecoder_->DequeueOutputBuffer(outInfo, GET_BUFFER_TIMEOUT_MS);
    if (ret != 0) {
        InitOutputBuffer(outInfo, VIDEO_DECODER);
#if defined(ENABLE_UIKIT) || defined(ENABLE_LVGL)
        if (surface_ != nullptr) {
#if defined(ENABLE_UIKIT)
            surface_->CancelBuffer(surfaceBuf_);
#elif defined(ENABLE_LVGL)
            LvCancelBuffer(reinterpret_cast<lv_obj_t *>(surface_), surfaceBuf_);
#endif
        }
#endif
        if (ret == CODEC_RECEIVE_EOS) {
            sinkManager_->RenderEos(false); /* all frame have been send to video sink */
        } else {
            goto EXIT;
        }
    }

    ret = sinkManager_->RenderFrame(outInfo);
    if (ret == SINK_RENDER_FULL || ret == SINK_RENDER_DELAY) {
        renderSleepTime_ = RENDER_FULL_SLEEP_TIME_US;
    } else if (ret == SINK_QUE_EMPTY) {
        renderSleepTime_ = 0;
    }
EXIT:
    ReleaseVDecoderOutputFrame();
    /* fuction RenderFrame will return SINK_RENDER_EOS when all frame in queue that have been processed */
    if (ret == SINK_RENDER_EOS) {
        hasRenderVideoEos_ = true;
        MEDIA_INFO_LOG("have render video eos");
        if (IsPlayEos()) {
            isPlayEnd_ = true;
        }
    }

    if (outInfo.buffers != nullptr) {
        MediaMemFree(outInfo.buffers);
    }
    return;
}
#endif

int32_t PlayerControl::DealWithInputEos(bool hasAudio, bool hasVideo)
{
    if (hasAudio) {
#ifdef UNSUPPORT_AUDIO_DECODER
        QueueInputDataToAudioSink();
#else
        QueueInputDataToAudioDecoder();
        DequeueOutputBufferAndRenderAudioFrame();
#endif
    }

    if (hasVideo) {
#if !defined(UNSUPPORT_VIDEO_DECODER)
        int32_t ret = QueueOutputBufferToVideoDecoder();
        if (ret != HI_SUCCESS) {
            return ret;
        }
        QueueInputDataToVideoDecoder();
        DequeueOutputBufferAndRenderVideoFrame();
#endif
    }
    return HI_SUCCESS;
}

int32_t PlayerControl::ProcessInputData(bool isAudio, bool isVideo)
{
    if (isAudio) {
#ifdef UNSUPPORT_AUDIO_DECODER
        int32_t retValue = QueueInputDataToAudioSink();
        if (!firstAudioFrameSend_ &&
            !firstVideoFrameSend_ &&
            retValue == HI_SUCCESS) {
            firstAudioFrameSend_ = true;
            int32_t info = PLAYERCONTROL_FIRST_AUDIO_FRAME;
            EventCallback(PLAYERCONTROL_FIRST_AUDIO_FRAME, &info);
        }
#else
        QueueInputDataToAudioDecoder();
        DequeueOutputBufferAndRenderAudioFrame();
#endif
    } else if (isVideo) {
#if !defined(UNSUPPORT_VIDEO_DECODER)
        int32_t retValue = QueueOutputBufferToVideoDecoder();
        if (retValue != HI_SUCCESS) {
            return retValue;
        }
        QueueInputDataToVideoDecoder();
        DequeueOutputBufferAndRenderVideoFrame();
        if (!firstVideoFrameSend_ &&
            !firstAudioFrameSend_ &&
            retValue == HI_SUCCESS) {
            firstVideoFrameSend_ = true;
            int32_t info = PLAYERCONTROL_FIRST_VIDEO_FRAME;
            EventCallback(PLAYERCONTROL_FIRST_VIDEO_FRAME, &info);
        }
#endif
    }
    return HI_SUCCESS;
}

void *PlayerControl::DataSchProcess(void *priv)
{
    CHECK_NULL_RETURN(priv, nullptr, "priv is null");

    PlayerControl *play = (PlayerControl *)priv;

#ifdef SUPPORT_SYS_PRCTL
    prctl(PR_SET_NAME, "PlaySch", 0, 0, 0);
#endif

    MEDIA_INFO_LOG("start work");

    // for low power
    play->GetRenderFullSleepThresholdTime(g_fromCodecFormatToAudioManager[play->fmtFileInfo_.enAudioType]);

    int32_t ret = HI_FAILURE;
    while (true) {
        MediaMutexLock(play->schMutex_);
        if (play->schThreadExit_ || play->isPlayErr_) {
            MediaMutexUnLock(play->schMutex_);
            break;
        }

        bool isContinue = false;
        DataSchPreProcess(play, &isContinue);
        MediaMutexUnLock(play->schMutex_);
        if (isContinue) {
            goto CONTINUE;
        }

        if (play->DequeueInputDataFromDemuxer() != HI_SUCCESS) {
            goto CONTINUE;
        }

        if (play->strmReadEnd_) {
            ret = play->DealWithInputEos(play->hasAudio_, play->hasVideo_);
        } else {
            ret = play->ProcessInputData(play->isAudio_, play->isVideo_);
        }

        if (ret == SINK_RENDER_FATEL || ret == REQUEST_SURFACE_BUFFER_FATAL) {
            play->EventQueueProcess();
            play->renderSleepTime_ = WAIT_BUFFER_EMPTY_SLEEP_TIME_US;
            goto CONTINUE;
        } else if (ret != HI_SUCCESS) {
            goto CONTINUE;
        }

        DataSchPostProcess(play);

    CONTINUE:
        if (play->renderSleepTime_ > 0) {
            MediaMutexLock(play->schMutex_);
            MediaThreadCondTimeWait(play->schCond_, play->schMutex_, play->renderSleepTime_);
            MediaMutexUnLock(play->schMutex_);
        }
    }

    MediaMutexLock(play->schMutex_);
    play->schThreadExit_ = true;
    MediaMutexUnLock(play->schMutex_);

    MEDIA_INFO_LOG("end work");
    return nullptr;
}

int32_t PlayerControl::DoPlay()
{
    MEDIA_DEBUG_LOG("Process in");
    CHECK_NULL_RETURN(playerSource_, HI_ERR_PLAYERCONTROL_NULL_PTR, "playerSource_ nullptr");

    int32_t ret = HI_SUCCESS;
    PlayerStatus playerState = GetCurState();
    CHECK_STATE_SAME_RETURN(playerState, PLAY_STATUS_PLAY, "state same");
    if (playerState == PLAY_STATUS_PREPARED) {
        ret = CheckMediaInfo();
        CHECK_FAILED_RETURN(ret, HI_SUCCESS, ret, "CheckMediaInfo failed");
        ret = SetDecoderAndStreamAttr();
        CHECK_FAILED_RETURN(ret, HI_SUCCESS, ret, "SetDecoderAndStreamAttr failed");
        ret = SinkStart();
        CHECK_FAILED_RETURN(ret, HI_SUCCESS, ret, "SinkStart failed");
        ret = DecoderStart();
        CHECK_FAILED_RETURN(ret, HI_SUCCESS, ret, "DecoderStart failed");
        ret = playerSource_->Start();
        CHECK_FAILED_RETURN(ret, HI_SUCCESS, ret, "playerSource_ Start failed");
        MediaMutexLock(schMutex_);
        paused_ = true;
        MediaThreadattr attr = { "PlaySch", 0x1B00, THREAD_SCHED_INVALID, 80, false };
        if (filePath_.find(".m3u8") != std::string::npos) {
            attr.stackSize = 0x4000;
        }
        schProcess_ = MediaThreadCreate(DataSchProcess, this, &attr);
        if (schProcess_ == nullptr) {
            MEDIA_ERR_LOG("thread create failed %d", ret);
            schThreadExit_ = false;
            MediaMutexUnLock(schMutex_);
            return -1;
        }
        paused_ = false;
        MediaMutexUnLock(schMutex_);
    } else if (playerState == PLAY_STATUS_PAUSE) {
        ret = PauseResume();
        CHECK_FAILED_RETURN(ret, HI_SUCCESS, ret, "PauseResume failed");
    } else if (playerState == PLAY_STATUS_TPLAY) {
        ret = DoSetPlaybackSpeed(TPLAY_SPEED_BASE);  // resume to normal speed
        CHECK_FAILED_RETURN(ret, HI_SUCCESS, ret, "DoSetPlaybackSpeed 1.0 failed");
    } else if (playerState == PLAY_STATUS_PLAY) {
        MEDIA_INFO_LOG("no need to repeat play operation");
        return 0;
    } else {
        return HI_ERR_PLAYERCONTROL_ILLEGAL_STATE_ACTION;
    }
    return ret;
}

int32_t PlayerControl::DoSetVolume(const VolumeAttr &volumeAttr)
{
    leftVolume_ = volumeAttr.leftVolume;
    rightVolume_ = volumeAttr.rightVolume;
    CHECK_NULL_RETURN(sinkManager_, HI_SUCCESS, "sinkManager_ nullptr, it will be setted after play");
    return sinkManager_->SetVolume(volumeAttr.leftVolume, volumeAttr.rightVolume);
}

void PlayerControl::TPlayRewindToNormal()
{
    DecoderAndSinkReset();
    sinkManager_->SetSpeed(TPLAY_SPEED_BASE);
    speed_ = TPLAY_SPEED_BASE;
    curState_ = PLAY_STATUS_PLAY;
    seekToTimeMs_ = 0;
    StateChangeCallback(curState_);
}

int32_t PlayerControl::TPlayBeforeFrameRead()
{
    int32_t ret = HI_SUCCESS;

    if (tplayMode_ != TPLAY_MODE_ONLY_I_FRAME) {
        return ret;
    }

    if (isTplayLastFrame_) {
        MEDIA_INFO_LOG("TPlayBeforeFrameRead TPlayIsFileReadEnd");
        return HI_RET_FILE_EOF;
    }

    int64_t seekTimeInMs = lastReadPktPts_ + curSeekOffset_;
    TplayDirect direction = speed_ < 0 ? TPLAY_DIRECT_BACKWARD : TPLAY_DIRECT_FORWARD;
    FormatSeekMode seekFlag = (speed_ < 0) ? FORMAT_SEEK_MODE_BACKWARD_KEY :
        FORMAT_SEEK_MODE_FORWARD_KEY;
    if (seekTimeInMs <= 0 && direction == TPLAY_DIRECT_BACKWARD) {
        seekTimeInMs = 0;
        isTplayLastFrame_ = true;
    } else if (seekTimeInMs >= fmtFileInfo_.s64Duration && direction == TPLAY_DIRECT_FORWARD) {
        seekTimeInMs = fmtFileInfo_.s64Duration;
        seekFlag = FORMAT_SEEK_MODE_BACKWARD_KEY;
        isTplayLastFrame_ = true;
    } else if (lastReadPktPts_ == 0 && !isTplayStartRead_) {
        seekTimeInMs = 0;
        seekFlag = FORMAT_SEEK_MODE_BACKWARD_KEY;
        isTplayLastFrame_ = false;
    }
    ret = playerSource_->Seek(fmtFileInfo_.s32UsedVideoStreamIndex, seekTimeInMs, seekFlag);
    if (ret != HI_SUCCESS) {
        MEDIA_INFO_LOG("playerSource_ seek failed maybe seek to file end, ret:%d", ret);
        return HI_RET_FILE_EOF;
    }

    return HI_SUCCESS;
}

int32_t PlayerControl::TPlayAfterFrameRead(FormatFrame &packet)
{
    bool isSkipPkt = false;

    if ((packet.timestampUs == lastReadPktPts_) && isTplayStartRead_) {
        lastReadPktPts_ += curSeekOffset_;
        isSkipPkt = true;
    } else {
        lastReadPktPts_ = packet.timestampUs;
    }

    return isSkipPkt ? HI_RET_SKIP_PACKET : HI_SUCCESS;
}

int32_t PlayerControl::TplayReadPacket()
{
    int32_t ret = HI_SUCCESS;
    if (cachedPacket_) {
        return ret;
    }

    ret = TPlayBeforeFrameRead();
    if (ret == HI_RET_FILE_EOF) {
        if (memset_s(&formatPacket_, sizeof(formatPacket_), 0, sizeof(FormatFrame)) != EOK) {
            MEDIA_ERR_LOG("SET  formatPacket_ failed");
            return HI_FAILURE;
        }
        cachedPacket_ = true;  // send eos
        return HI_SUCCESS;
    }
    ret = ReadFrameFromSource(formatPacket_);
    if (ret == HI_RET_FILE_EOF) {
        if (memset_s(&formatPacket_, sizeof(formatPacket_), 0, sizeof(FormatFrame)) != EOK) {
            MEDIA_ERR_LOG("SET  formatPacket_ failed");
            return HI_FAILURE;
        }
    } else if (ret != HI_SUCCESS) {
        MEDIA_ERR_LOG("ReadFrameFromSource failed , ret:%d", ret);
        NotifyError(PLAYERCONTROL_ERROR_DEMUX_FAIL);
        return HI_FAILURE;
    }
    cachedPacket_ = true;
    ret = TPlayAfterFrameRead(formatPacket_);
    if (ret == HI_RET_SKIP_PACKET) {
        ClearCachePacket();
        return ret;
    }
    isTplayStartRead_ = true;
    strmReadEnd_ = false;
    return ret;
}

int32_t PlayerControl::ReadPacket()
{
    int32_t ret = HI_SUCCESS;
    if (cachedPacket_) {
        return ret;
    }
    ret = ReadFrameFromSource(formatPacket_);
    if (ret != HI_RET_FILE_EOF && formatPacket_.data == nullptr && formatPacket_.len == 0) {
        return HI_RET_NODATA;
    }
    if (ret == HI_RET_FILE_EOF) {
        CHECK_FAILED_RETURN(memset_s(&formatPacket_, sizeof(formatPacket_), 0, sizeof(FormatFrame)), EOK, -1,
            "memset_s failed");
        ret = HI_SUCCESS;
    } else if (ret == HI_RET_NODATA) {
        CHECK_FAILED_RETURN(memset_s(&formatPacket_, sizeof(formatPacket_), 0, sizeof(FormatFrame)), EOK, -1,
            "memset_s failed");
        strmReadEnd_ = false;
        return ret;
    } else if (ret != HI_SUCCESS) {
        MEDIA_ERR_LOG("ReadFrameFromSource failed , ret:%d", ret);
        NotifyError(PLAYERCONTROL_ERROR_DEMUX_FAIL);
        return HI_FAILURE;
    }
    strmReadEnd_ = false;
    cachedPacket_ = true;
    return ret;
}

#ifdef PLAYER_KPI_ON
void PlayerControl::PrintRenderEsTime(const struct timeval &esFrameStartTime)
{
    std::string renderEsFrame("KPI_PlayerRenderEsFrame");
    struct timeval esFrameEndTime = {};
    uint64_t renderEsOnceTime = CalcTimeDiffUsPrint(esFrameStartTime, esFrameEndTime, renderEsFrame, false);
    if (renderEsOnceTime != INVALID_TIME) {
        renderEsTotalTime_ += renderEsOnceTime;
        renderEsFrameCnt_++;
    }
    if (renderEsFrameCnt_ == 500) { /* 500:Print Interval */
        MEDIA_INFO_LOG("KPI_PlayerRenderEsFrame avg time: %lld us", renderEsTotalTime_ /= renderEsFrameCnt_);
        renderEsTotalTime_ = 0;
        renderEsFrameCnt_ = 0;
    }
}
#endif

void PlayerControl::FillAudioFrameInfo(EsFrameInfo &outInfo)
{
    if (memset_s(&outInfo, sizeof(outInfo), 0, sizeof(EsFrameInfo)) != EOK) {
        return;
    }

    outInfo.addr = formatPacket_.data;
    outInfo.size = formatPacket_.len;
    outInfo.pts = formatPacket_.timestampUs;
    outInfo.flag = 0;
    EsFrameType frameType = g_fromDemuxTypeToSinkType[formatPacket_.frameType];
    outInfo.type = (frameType == ESFRAME_TYPE_NONE) ? ESFRAME_TYPE_AUDIO : frameType;
}

#ifdef DEBUG_LOW_POWER
void PlayerControl::LowPowerProcess(int64_t pts, uint32_t &renderSuccessCount, bool renderFrameSuccess)
{
    if (renderFrameSuccess) {
        if (headInSendSuccessInterval_ == -1) {
            headInSendSuccessInterval_ = pts;
            tailInSendSuccessInterval_ = -1;
        }
        renderSuccessCount++;
    } else {
        if (tailInSendSuccessInterval_ == -1) {
            tailInSendSuccessInterval_ = pts;
            int64_t diffMs = tailInSendSuccessInterval_ - headInSendSuccessInterval_ - renderFullSleepThresholdMs_;
            renderSleepTime_ = (diffMs > 0) ? (diffMs * US_2_MS) : RENDER_FULL_SLEEP_TIME_US;
        }

        /* MEAIA_XXX_LOG have performance problem, use printf instead */
        printf("render %d audio frames, from %lldms to %lldms, thread wait %ums\n", renderSuccessCount,
            tailInSendSuccessInterval_, headInSendSuccessInterval_, renderSleepTime_ / US_2_MS);

        headInSendSuccessInterval_ = -1;
        renderSuccessCount = 0;
    }
}
#endif

#ifdef PLAYER_KPI_ON
void PlayerControl::ReadPacketAvgCostTime(const struct timeval &readPacketStartTime)
{
    struct timeval readPacketEndTime;
    std::string readPacketCost("KPI_Player_demux_cost_time");
    uint64_t readPktOnceTime = CalcTimeDiffUsPrint(readPacketStartTime, readPacketEndTime, readPacketCost, false);
    if (readPktOnceTime != INVALID_TIME) {
        readPktTotalTime_ += readPktOnceTime;
        readPacketCnt_++;
    }
    if (readPacketCnt_ == 500) { /* 500:Print Interval */
        MEDIA_INFO_LOG("KPI_Player_demux_cost_time avg time: %lld us", readPktTotalTime_ /= readPacketCnt_);
        readPktTotalTime_ = 0;
        readPacketCnt_ = 0;
    }
}
#endif

int32_t PlayerControl::DoStop()
{
    int32_t ret = HI_SUCCESS;
    PlayerStatus playerState = GetCurState();
    CHECK_STATE_SAME_RETURN(playerState, PLAY_STATUS_IDLE, "state same");

    if (schProcess_ != nullptr) {
        MediaMutexLock(schMutex_);
        schThreadExit_ = true;
        MediaThreadCondSignal(schCond_);
        MediaMutexUnLock(schMutex_);
        MediaThreadJoin(&schProcess_);
    }
    StopSinkAndDecoder();
    ClearCachePacket();
    if (playerSource_ != nullptr) {
        ret = playerSource_->Stop();
        CHECK_FAILED_PRINT(ret, HI_SUCCESS, "playerSource_ stop failed");
    }
    isPlayEnd_ = false;
    strmReadEnd_ = false;
    isPlayErr_ = false;
    lastRendPos_ = 0;
    lastReadPktPts_ = 0;
    lastSendPktPts_ = 0;
    isAudPlayEos_ = false;
    hasRenderAudioEos_ = false;
    hasRenderAudioEos_ = false;
    lastNotifyTime_ = 0;
    seekToTimeMs_ = -1;
    if (memset_s(&(fmtFileInfo_), sizeof(fmtFileInfo_), 0, sizeof(FormatFileInfo)) != EOK) {
        MEDIA_ERR_LOG("memset_s failed");
        return HI_ERR_PLAYERCONTROL_MEM_SET;
    }
    return ret;
}

int32_t PlayerControl::DoPause(void)
{
    PlayerStatus playerState = GetCurState();
    CHECK_STATE_SAME_RETURN(playerState, PLAY_STATUS_PAUSE, "state same");
    CHECK_NULL_RETURN(sinkManager_, HI_FAILURE, "sinkManager_ nullptr");

    if (playerState == PLAY_STATUS_PLAY || playerState == PLAY_STATUS_TPLAY) {
        MediaMutexLock(schMutex_);
        int32_t ret = sinkManager_->Pause();
        CHECK_FAILED_PRINT(ret, 0, "sinkManager_ pause failed:%d");
        paused_ = true;
        MediaThreadCondSignal(schCond_);
        MediaThreadCondTimeWait(pauseCond_, schMutex_, WAIT_PAUSE_TIME_OUT_US);
        MediaMutexUnLock(schMutex_);
    } else {
        return HI_ERR_PLAYERCONTROL_ILLEGAL_STATE_ACTION;
    }
    return HI_SUCCESS;
}

int32_t PlayerControl::DoSeekIfNeed(void)
{
    if (seekToTimeMs_ == -1) {
        return HI_SUCCESS;
    }
    MEDIA_INFO_LOG("seek start");
    int32_t ret = AyncSeek(seekToTimeMs_);
    if (ret != HI_SUCCESS) {
        MEDIA_ERR_LOG(" AyncSeek  failed , ret:%d", ret);
        NotifyError(PLAYERCONTROL_ERROR_AUD_PLAY_FAIL);
        return ret;
    }
    strmReadEnd_ = false;
    isAudPlayEos_ = false;
    isVidPlayEos_ = false;
    hasRenderVideoEos_ = false;
    hasRenderAudioEos_ = false;
    isPlayEnd_ = false;
    seekToTimeMs_ = -1;
    firstAudioFrameAfterSeek_ = true;
    firstVideoFrameAfterSeek_ = true;
    MEDIA_INFO_LOG("seek end");
    return HI_SUCCESS;
}

int32_t PlayerControl::DoSeek(int64_t timeInMs)
{
    seekToTimeMs_ = timeInMs;
    return DoSeekIfNeed();
}

int32_t PlayerControl::DoGetFileInfo(FormatFileInfo &fileInfo) const
{
    return memcpy_s(&fileInfo, sizeof(FormatFileInfo), &fmtFileInfo_, sizeof(fmtFileInfo_));
}

int32_t PlayerControl::DoSetMedia(const PlayerControlStreamAttr &mediaAttr)
{
    CHECK_NULL_RETURN(playerSource_, HI_ERR_PLAYERCONTROL_NULL_PTR, "playerSource_ nullptr");

    int32_t ret = HI_SUCCESS;
    if (hasVideo_) {
        ret = playerSource_->SelectTrack(0, mediaAttr.s32VidStreamId);
        CHECK_FAILED_RETURN(ret, 0, HI_ERR_PLAYERCONTROL_DEMUX_ERROR, "select video track failed");
    }
    if (hasAudio_) {
        ret = playerSource_->SelectTrack(0, mediaAttr.s32AudStreamId);
        CHECK_FAILED_RETURN(ret, 0, HI_ERR_PLAYERCONTROL_DEMUX_ERROR, "select audio track failed");
    }

    fmtFileInfo_.s32UsedVideoStreamIndex = mediaAttr.s32VidStreamId;
    fmtFileInfo_.s32UsedAudioStreamIndex = mediaAttr.s32AudStreamId;

    hasAudio_ = (fmtFileInfo_.s32UsedAudioStreamIndex != HI_DEMUXER_NO_MEDIA_STREAM) ? true : false;
    hasVideo_ = (fmtFileInfo_.s32UsedVideoStreamIndex != HI_DEMUXER_NO_MEDIA_STREAM) ? true : false;

    return ret;
}

TplayMode PlayerControl::TPlayGetPlayMode()
{
    if (speed_ < 0) {
        return TPLAY_MODE_ONLY_I_FRAME;
    }

    if ((fmtFileInfo_.u32Width <= 0) || (fmtFileInfo_.u32Height <= 0) || (fmtFileInfo_.fFrameRate <= 0)) {
        MEDIA_ERR_LOG("get stream info failed");
        return TPLAY_MODE_FULL_PLAY;
    }

    TplayMode tplayMode = TPLAY_MODE_ONLY_I_FRAME;
    uint64_t resolution = fmtFileInfo_.u32Width * fmtFileInfo_.u32Height;
    if (resolution <= FULL_TPLAY_RESULITON_LIMIT &&
        fmtFileInfo_.fFrameRate * speed_ <= FULL_TPLAY_FRAMERATE_LIMIT) {
        tplayMode = TPLAY_MODE_FULL_PLAY;
    }

    return tplayMode;
}

/* Calculate the seeking offset in ONLY_I_FRAME mode.
 * offset = (Number of I-frames needed per second) / (Target tplay frame rate) * (I-frame interval)
 * = ((srcFps * speed / gop) / tPlayFps) * ((1000 * gop) / srcFps).
 * Note: It is necessary to set an appropriate GOP_SIZE. For mjpeg format, GOP_SIZE should be set to 1,
 * but for H.264, other values need to be set.
 */
int32_t PlayerControl::TPlayGetSeekOffset(float speed)
{
    int32_t positiveSpeed = (speed < 0 ? -speed : speed);
    float srcFps = fmtFileInfo_.fFrameRate;
    float tPlayFps = FULL_TPLAY_FRAMERATE_LIMIT;

    float gopPts = MS_SCALE * GOP_SIZE / srcFps;
    float skipIFrameNum = (srcFps * positiveSpeed / GOP_SIZE) / tPlayFps;
    float skipPts = skipIFrameNum * gopPts;
    float seekOffset = (speed < 0 ? -skipPts : skipPts);

    return static_cast<int32_t>(seekOffset);
}

int32_t PlayerControl::DoSetPlaybackSpeed(float speed)
{
    MediaMutexLock(schMutex_);
    ClearCachePacket();
    int32_t ret = DecoderAndSinkReset();
    if (ret != HI_SUCCESS) {
        MEDIA_ERR_LOG("DecoderAndSinkReset failed, ret:%d", ret);
        MediaMutexUnLock(schMutex_);
        return ret;
    }
    ret = sinkManager_->SetSpeed(speed);
    if (ret != HI_SUCCESS) {
        MEDIA_ERR_LOG("sink manager set speed failed, ret:%d", ret);
        MediaMutexUnLock(schMutex_);
        return ret;
    }
    speed_ = speed;
    lastReadPktPts_ = currentPosition_;
    isTplayLastFrame_ = false;
    tplayMode_ = TPlayGetPlayMode();
    if (tplayMode_ == TPLAY_MODE_ONLY_I_FRAME) {
        curSeekOffset_ = TPlayGetSeekOffset(speed);
    }
    MEDIA_DEBUG_LOG("DoSetPlaybackSpeed speed_:%f, curSeekOffset_:%d, tplayMode_:%d",
        speed_, curSeekOffset_, tplayMode_);
    MediaMutexUnLock(schMutex_);
    return HI_SUCCESS;
}

void PlayerControl::RecordLastPosition()
{
    sinkManager_->RecordLastPosition();
}

// util bigein
void PlayerControl::EventCallback(PlayerControlEvent event, const void *data) const
{
    CHECK_NULL_RETURN_NONE(eventCallback_.callbackFun, "callbackFun nullptr");
    eventCallback_.callbackFun(eventCallback_.player, event, data);
}

void PlayerControl::PlayerControlCondSignal(void)
{
    MediaMutexLock(schMutex_);
    if (fmtFileInfo_.s32UsedVideoStreamIndex < 0 && !hasRenderAudioEos_ && renderSleepTime_ > 0) {
        MediaThreadCondSignal(schCond_);
    }
    MediaMutexUnLock(schMutex_);
}

void PlayerControl::EnableSingleLooping(bool loop)
{
    MediaMutexLock(schMutex_);
    loop_ = loop;
    MediaMutexUnLock(schMutex_);
}

int32_t PlayerControl::SetParam(const std::string &key, const void *value, uint32_t len)
{
    CHECK_NULL_RETURN(sinkManager_, HI_ERR_PLAYERCONTROL_NULL_PTR, "sinkManager_ nullptr");
    MediaMutexLock(schMutex_);
    int32_t ret = sinkManager_->SetParam(key, DATA_TYPE_STRING, value, len);
    if (ret != HI_SUCCESS) {
        MediaMutexUnLock(schMutex_);
        return ret;
    }
    MediaMutexUnLock(schMutex_);
    return HI_SUCCESS;
}

int32_t PlayerControl::GetParam(const std::string &key, void *value, uint32_t size)
{
    CHECK_NULL_RETURN(sinkManager_, HI_ERR_PLAYERCONTROL_NULL_PTR, "sinkManager_ nullptr");
    MediaMutexLock(schMutex_);
    int32_t ret = sinkManager_->GetParam(key, DATA_TYPE_PTR, value, size);
    if (ret != HI_SUCCESS) {
        MediaMutexUnLock(schMutex_);
        return ret;
    }
    MediaMutexUnLock(schMutex_);
    return HI_SUCCESS;
}

void PlayerControl::NotifyError(PlayerControlError playerError) const
{
    EventCallback(PLAYERCONTROL_EVENT_ERROR, static_cast<void *>(&playerError));
}

void PlayerControl::StateChangeCallback(PlayerStatus state) const
{
    EventCallback(PLAYERCONTROL_EVENT_STATE_CHANGED, static_cast<void *>(&state));
}

void PlayerControl::FlushDecoder(void) const
{
#ifndef UNSUPPORT_AUDIO_DECODER
    ReleaseADecoderOutputFrame();
    if (audioDecoder_ != nullptr) {
        audioDecoder_->FlushDec();
    }
#endif

#if !defined(UNSUPPORT_VIDEO_DECODER)
    ReleaseVDecoderOutputFrame();
    if (videoDecoder_ != nullptr) {
        videoDecoder_->FlushDec();
    }
#endif
}

int32_t PlayerControl::DecoderAndSinkReset(void)
{
    if (strmReadEnd_ && loop_) {
        return HI_SUCCESS;
    }
    int32_t ret;
    bool isNeedResume = false;
    ret = sinkManager_->Pause();
    if ((ret == HI_SUCCESS) && (!paused_)) {
        isNeedResume = true;
    }
    ret = sinkManager_->Reset();
    if (ret != HI_SUCCESS) {
        MEDIA_ERR_LOG("m_render reset failed");
        (void)sinkManager_->Resume();
        return ret;
    }
    FlushDecoder();
    if (isNeedResume) {
        ret = sinkManager_->Resume();
        CHECK_FAILED_RETURN(ret, HI_SUCCESS, HI_FAILURE, "sinkManager_ Resume failed");
    }
    return HI_SUCCESS;
}

int32_t PlayerControl::AyncSeek(int64_t seekTime)
{
    CHECK_NULL_RETURN(playerSource_, HI_FAILURE, "playerSource_ nullptr");
    int64_t seekTimeInMs = seekTime;
    int32_t ret = 0;
    sinkManager_->Reset();
    ClearCachePacket();
    if (fmtFileInfo_.s32UsedVideoStreamIndex != HI_DEMUXER_NO_MEDIA_STREAM) {
        ret = playerSource_->Seek(fmtFileInfo_.s32UsedVideoStreamIndex, seekTimeInMs, FORMAT_SEEK_MODE_BACKWARD_KEY);
        if (ret != HI_SUCCESS) {
            MEDIA_INFO_LOG("exec fmt_seek video stream failed, ret:%d", ret);
            seekTimeInMs = currentPosition_;
        }
    }
    if (fmtFileInfo_.s32UsedAudioStreamIndex != HI_DEMUXER_NO_MEDIA_STREAM) {
        ret = playerSource_->Seek(fmtFileInfo_.s32UsedAudioStreamIndex, seekTimeInMs, FORMAT_SEEK_MODE_BACKWARD_KEY);
        if (ret != HI_SUCCESS) {
            MEDIA_INFO_LOG("exec fmt_seek audio stream failed, ret:%d", ret);
            seekTimeInMs = currentPosition_;
        }
    }
    currentPosition_ = seekTimeInMs;
    if (speed_ != TPLAY_SPEED_BASE) {
        lastReadPktPts_ = currentPosition_;
        isTplayStartRead_ = (currentPosition_ == 0) ? false : true;
        isTplayLastFrame_ = false;
    }
    EventCallback(PLAYERCONTROL_EVENT_PROGRESS, &currentPosition_);
    EventCallback(PLAYERCONTROL_EVENT_SEEK_END, static_cast<void *>(&seekTimeInMs));
    return HI_SUCCESS;
}

void PlayerControl::GetPlayElementEventCallBack(PlayEventCallback &callback)
{
    callback.onEventCallback = PlayerControlOnEvent;
    callback.priv = static_cast<void *>(this);
}
int32_t PlayerControl::SyncPrepare()
{
    int ret;
    playerSource_ = std::make_shared<PlayerSource>();
    CHECK_NULL_RETURN(playerSource_, HI_FAILURE, "new playerSource_ nullptr");
    playerSource_->Init();

    if (sourceType_ == SOURCE_TYPE_FD) {
        playerSource_->SetSource(fd_, offset_);
    } else if (sourceType_ == SOURCE_TYPE_STREAM) {
        playerSource_->SetSource(streamSource_);
    } else {
        playerSource_->SetSource(filePath_.c_str());
    }

    PlayEventCallback callback;
    GetPlayElementEventCallBack(callback);
    ret = playerSource_->SetCallBack(callback);
    CHECK_FAILED_RETURN(ret, HI_SUCCESS, ret, "SetCallBack failed");

    ret = playerSource_->Prepare();
    CHECK_FAILED_RETURN(ret, HI_SUCCESS, ret, "Prepare failed");
    ret = playerSource_->GetFileInfo(fmtFileInfo_);
    CHECK_FAILED_RETURN(ret, HI_SUCCESS, ret, "GetFileInfo failed");
    metaData_.key = KEY_TYPE_EXTRADATA;
    ret = playerSource_->GetParam(0, metaData_);
    if (ret != HI_SUCCESS) {
        MEDIA_INFO_LOG("no extradata metadata");
        metaData_.key = 0;
    }

    hasAudio_ = (fmtFileInfo_.s32UsedAudioStreamIndex != HI_DEMUXER_NO_MEDIA_STREAM) ? true : false;
    hasVideo_ = (fmtFileInfo_.s32UsedVideoStreamIndex != HI_DEMUXER_NO_MEDIA_STREAM) ? true : false;

    MEDIA_INFO_LOG("used audiostream index %d", fmtFileInfo_.s32UsedAudioStreamIndex);
    MEDIA_INFO_LOG("used videostream index %d", fmtFileInfo_.s32UsedVideoStreamIndex);

    return HI_SUCCESS;
}

bool PlayerControl::IsPlayEos() const
{
    bool isEos = false;
    if (hasAudio_) {
        if (hasVideo_) {
            return (!isAudioStarted_ || hasRenderAudioEos_) && (!isVideoStarted_ || hasRenderVideoEos_);
        } else {
            return (!isAudioStarted_ || hasRenderAudioEos_);
        }
    } else {
        if (hasVideo_) {
            return (!isVideoStarted_ || hasRenderVideoEos_);
        } else {
            MEDIA_ERR_LOG("neither audio nor video");
            isEos = false;
        }
    }

    return isEos;
}

int32_t PlayerControl::Invoke(PlayerInvoke invokeId, const void *param)
{
    MsgInfo msg = {};
    InvokeParameter invokeParam;
    invokeParam.id = invokeId;
    invokeParam.param = param;

    msg.what = PLAYERCONTROL_MSG_INVOKE;
    msg.msgData = &invokeParam;
    msg.msgDataLen = sizeof(InvokeParameter);
    return OnMessageReceived(msg);
}

int32_t PlayerControl::DoInvoke(const InvokeParameter &invokeParam)
{
    switch (invokeParam.id) {
        case INVOKE_ENABLE_PAUSE_AFTER_PLAYER:
            if (invokeParam.param == nullptr) {
                return -1;
            }
            break;

        case INVOKE_SET_AUDIO_SESSION:
            if (invokeParam.param == nullptr) {
                return -1;
            }
            audioSession_ = *(static_cast<const uint32_t *>(invokeParam.param));
            MEDIA_INFO_LOG("audio sessionId: %d", audioSession_);
            break;

        case INVOKE_SET_OUTPUT_DEVICE:
            if (invokeParam.param == nullptr) {
                return -1;
            }
            outDeviceId_ = *(static_cast<const uint32_t *>(invokeParam.param));
            MEDIA_INFO_LOG("outputDevice: 0x%x", outDeviceId_);
            if (sinkManager_ != nullptr) {
                return sinkManager_->SetOutputDevice(outDeviceId_);
            }
            break;

        case INVOKE_SET_AUDIOSTREAM_TYPE:
            if (invokeParam.param == nullptr) {
                return -1;
            }
            streamType_ = *(static_cast<const uint32_t *>(invokeParam.param));
            MEDIA_INFO_LOG("streamType: 0x%x", streamType_);
            break;
        default:
            MEDIA_ERR_LOG("unsupport invoke: 0x%x", invokeParam.id);
            return -1;
    }
    return 0;
}

int PlayerControl::HandleMsgWithParam(const MsgInfo &msgInfo)
{
    int32_t ret;
    if (msgInfo.msgData == nullptr) {
        MEDIA_ERR_LOG("data is null, msg:%d", msgInfo.what);
        return HI_ERR_PLAYERCONTROL_NULL_PTR;
    }
    switch (msgInfo.what) {
        case PLAYERCONTROL_MSG_REGCALLBACK:
            ret = DoRegCallback(*static_cast<PlayerCtrlCallbackParam *>(msgInfo.msgData));
            break;
        case PLAYERCONTROL_MSG_SET_DATASOURCE_URI:
            ret = DoSetDataSource(static_cast<const char *>(msgInfo.msgData));
            break;
        case PLAYERCONTROL_MSG_SET_DATASOURCE_FD:
            ret = DoSetDataSource(*static_cast<SourceFdParam *>(msgInfo.msgData));
            break;
        case PLAYERCONTROL_MSG_SET_DATASOURCE_STREAM:
            ret = DoSetDataSource(*static_cast<StreamSourceParam *>(msgInfo.msgData));
            break;
        case PLAYERCONTROL_MSG_SETATTR:
            ret = DoSetMedia(*static_cast<PlayerControlStreamAttr *>(msgInfo.msgData));
            break;
        case PLAYERCONTROL_MSG_SETSPEED:
            ret = DoSetPlaybackSpeed(*static_cast<const float *>(msgInfo.msgData));
            break;
        case PLAYERCONTROL_MSG_SET_VOLUME:
            ret = DoSetVolume(*static_cast<VolumeAttr *>(msgInfo.msgData));
            break;
        case PLAYERCONTROL_MSG_SEEK:
            ret = DoSeek(*static_cast<int64_t *>(msgInfo.msgData));
            break;
        case PLAYERCONTROL_MSG_GETFILEINFO:
            ret = DoGetFileInfo(*static_cast<FormatFileInfo *>(msgInfo.msgData));
            break;
        case PLAYERCONTROL_MSG_INVOKE:
            ret = DoInvoke(*static_cast<InvokeParameter *>(msgInfo.msgData));
            break;
        default:
            ret = HI_ERR_PLAYERCONTROL_ILLEGAL_PARAM;
            break;
    }

    return ret;
}

int PlayerControl::HandleMsgWithNoParam(const MsgInfo &msgInfo)
{
    int32_t ret;
    switch (msgInfo.what) {
        case PLAYERCONTROL_MSG_PREPARE:
            ret = DoPrepare();
            break;
        case PLAYERCONTROL_MSG_PLAY:
            ret = DoPlay();
            break;
        case PLAYERCONTROL_MSG_PAUSE:
            ret = DoPause();
            break;
        case PLAYERCONTROL_MSG_STOP:
            ret = DoStop();
            break;
        case PLAYERCONTROL_MSG_HANDLEDATA:
            ret = HI_SUCCESS;
            break;
        default:
            ret = HI_ERR_PLAYERCONTROL_ILLEGAL_PARAM;
            break;
    }

    return ret;
}

int PlayerControl::HandleMessage(const MsgInfo &msgInfo)
{
    int32_t ret = HI_FAILURE;
    if (!EventValidAtCurState(static_cast<PlayerControlMsgType>(msgInfo.what))) {
        MEDIA_ERR_LOG("invalid event :%d at current state: %d", msgInfo.what, GetCurState());
        return HI_ERR_PLAYERCONTROL_ILLEGAL_STATE_ACTION;
    }

    switch (msgInfo.what) {
        case PLAYERCONTROL_MSG_REGCALLBACK:
        case PLAYERCONTROL_MSG_SET_DATASOURCE_URI:
        case PLAYERCONTROL_MSG_SET_DATASOURCE_FD:
        case PLAYERCONTROL_MSG_SET_DATASOURCE_STREAM:
        case PLAYERCONTROL_MSG_SETATTR:
        case PLAYERCONTROL_MSG_SETSPEED:
        case PLAYERCONTROL_MSG_SET_VOLUME:
        case PLAYERCONTROL_MSG_SEEK:
        case PLAYERCONTROL_MSG_GETFILEINFO:
        case PLAYERCONTROL_MSG_INVOKE:
            ret = HandleMsgWithParam(msgInfo);
            break;
        case PLAYERCONTROL_MSG_PAUSE:
        case PLAYERCONTROL_MSG_PREPARE:
        case PLAYERCONTROL_MSG_PLAY:
        case PLAYERCONTROL_MSG_HANDLEDATA:
        case PLAYERCONTROL_MSG_STOP:
            ret = HandleMsgWithNoParam(msgInfo);
            break;
        default:
            break;
    }
    return ret;
}

void PlayerControl::OnEventHandled(int event, int result) const
{
    if ((result == HI_ERR_PLAYERCONTROL_ILLEGAL_STATE_ACTION) && (event == PLAYERCONTROL_MSG_SEEK)) {
        NotifyError(PLAYERCONTROL_ERROR_ILLEGAL_STATEACTION);
    }
}

int32_t PlayerControl::OnMessageReceived(const MsgInfo &msg)
{
    int32_t ret = HandleMessage(msg);
    if (ret == HI_SUCCESS) {
        PlayerStatus nextState = FindTransition(GetCurState(), msg.what);
        curState_ = nextState;
        StateChangeCallback(curState_);
    }
    OnEventHandled(msg.what, ret);
    return ret;
}

PlayerStatus PlayerControl::GetCurState(void)
{
    return curState_;
}

PlayerStatus PlayerControl::FindTransition(PlayerStatus state, int32_t event) const
{
    PlayerStatus nextState = state;
    for (uint32_t i = 0; i < STATE_TRANS_STATE_SIZE; i++) {
        if (event == STATE_MACHINE[state][i].msgType) {
            nextState = STATE_MACHINE[state][i].newState;
            break;
        }
    }
    if (nextState == PLAY_STATUS_PLAY && speed_ != TPLAY_SPEED_BASE) {
        nextState = PLAY_STATUS_TPLAY;
    }
    return nextState;
}

bool PlayerControl::EventValidAtCurState(PlayerControlMsgType type)
{
    if (type < PLAYERCONTROL_MSG_SETATTR || type >= PLAYERCONTROL_MSG_BUTT) {
        MEDIA_ERR_LOG("EventValidAtCurState MSG type not support %d", type);
        return false;
    }
    if (GetCurState() < 0 || GetCurState() >= PLAY_STATUS_BUTT) {
        MEDIA_ERR_LOG("EventValidAtCurState cur state not support %d", GetCurState());
        return false;
    }
    return LITEPLAY_STATE[type][GetCurState()];
}
bool PlayerControl::UIntMulIsOverflow64(uint64_t a, uint64_t b, uint64_t *value) const
{
    if (a == 0 || b == 0) {
        return false;
    }
    if (UINT64_MAX / a < b) {
        return true;
    }

    uint64_t result = static_cast<uint64_t>((a) * (b));
    if (value != nullptr) {
        *value = result;
    }
    return false;
}
bool PlayerControl::UIntMulIsOverflow32(uint32_t a, uint32_t b, uint32_t *value) const
{
    if (a == 0 || b == 0) {
        return false;
    }
    if (UINT32_MAX / a < b) {
        return true;
    }

    long long result = static_cast<long long>((a) * (b));
    if (value != nullptr) {
        *value = static_cast<uint32_t>(result);
    }
    return false;
}
}
}

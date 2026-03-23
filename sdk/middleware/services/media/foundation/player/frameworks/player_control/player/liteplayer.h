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

#ifndef LITEPLAYER_H
#define LITEPLAYER_H

#include <string>

#include <sys/time.h>
#include "hi_liteplayer.h"
#include "liteplayer_define.h"
#include "liteplayer_msgtype.h"
#include "player_define.h"
#include "player_source.h"
#include "player_sink_manager.h"
#include "hi_demuxer.h"
#include "media_thread_adapt.h"
#include "player_debug_info.h"

#if !defined(UNSUPPORT_AUDIO_DECODER) || !defined(UNSUPPORT_VIDEO_DECODER)
#include "decoder.h"
#endif
#if defined(ENABLE_UIKIT)
#include "surface.h"
#elif defined(ENABLE_LVGL)
#include "lv_surface_view.h"
#endif

// disable PLAYER_KPI_ON for workaround local music playback caton issue
#ifdef PLAYER_KPI_ON
#define KPI_PRINT true
#else
#define KPI_PRINT false
#endif

namespace OHOS {
namespace Media {

const uint32_t INVALID_TIME = 0;

struct PalayControlEventItem {
    EventCbType event;
};

struct MsgInfo {
    int32_t what;
    uint32_t arg1;
    uint32_t arg2;
    void *msgData;
    uint32_t msgDataLen;

    // below is private info,do not change outside
    bool isNeedReply;
    void *privDate;
};

struct CodecFormatAndMimePair {
    CodecFormat format;
    AvCodecMime mime;
};


using StateMap = struct {
    int32_t msgType;
    PlayerStatus newState;
};


/*****************************************************************************
status table of operation, false is not support
*******************************************************************/
const bool LITEPLAY_STATE[PLAYERCONTROL_MSG_BUTT][PLAY_STATUS_BUTT] = {
    /* idle, init,  prepare play, tplay, pause, err */
    { false, false, true,  false, false, false, false }, /* setMedia */
    { true,  true,  true,  true,  false, true,  true },  /* regCallbk */
    { true,  false, false, false, false, false, false }, /* setsrc_fd */
    { true,  false, false, false, false, false, false }, /* setsrc_uri */
    { true,  false, false, false, false, false, false }, /* setsrc_stream */
    { false, true,  true,  false, false, false, false }, /* prepare */
    { false, false, true,  true,  true,  true,  false }, /* play */
    { false, false, false, true,  false, true,  false }, /* seek */
    { false, false, false, true,  true,  true,  false }, /* pause */
    { false, false, false, true,  true,  false, false }, /* setspeed */
    { true,  true,  true,  true,  true,  true,  true },  /* stop */
    { false, false, true,  true,  false, true,  false }, /* getInfo */
    { true,  false, false, true,  false, true,  false }, /* handledata */
    { true,  true,  true,  true,  true,  true,  true },  /* Error */
    { false, true,  true,  true,  false, true,  false }, /* setVolume */
    { true,  true,  true,  true,  false, true,  true },  /* invoke */
};

const uint32_t STATE_TRANS_STATE_SIZE = 4u;
const StateMap STATE_MACHINE[][STATE_TRANS_STATE_SIZE] = {
    { /* idle */
        {PLAYERCONTROL_MSG_SET_DATASOURCE_FD,     PLAY_STATUS_INIT},
        {PLAYERCONTROL_MSG_SET_DATASOURCE_URI,    PLAY_STATUS_INIT},
        {PLAYERCONTROL_MSG_SET_DATASOURCE_STREAM, PLAY_STATUS_INIT},
        {PLAYERCONTROL_MSG_ERROR,                 PLAY_STATUS_ERR}
    },
    { /* init */
        {PLAYERCONTROL_MSG_PREPARE, PLAY_STATUS_PREPARED},
        {PLAYERCONTROL_MSG_ERROR,   PLAY_STATUS_ERR},
        {PLAYERCONTROL_MSG_STOP,    PLAY_STATUS_IDLE},
        {PLAYERCONTROL_MSG_BUTT,    PLAY_STATUS_INIT}
    },
    { /* prepare */
        {PLAYERCONTROL_MSG_PLAY,    PLAY_STATUS_PLAY},
        {PLAYERCONTROL_MSG_STOP,    PLAY_STATUS_IDLE},
        {PLAYERCONTROL_MSG_ERROR,   PLAY_STATUS_ERR},
        {PLAYERCONTROL_MSG_BUTT,    PLAY_STATUS_PREPARED}
    },
    { /* play */
        {PLAYERCONTROL_MSG_STOP,     PLAY_STATUS_IDLE},
        {PLAYERCONTROL_MSG_PAUSE,    PLAY_STATUS_PAUSE},
        {PLAYERCONTROL_MSG_ERROR,    PLAY_STATUS_ERR},
        {PLAYERCONTROL_MSG_SETSPEED, PLAY_STATUS_TPLAY}
    },
    { /* tplay */
        {PLAYERCONTROL_MSG_STOP,     PLAY_STATUS_IDLE},
        {PLAYERCONTROL_MSG_PAUSE,    PLAY_STATUS_PAUSE},
        {PLAYERCONTROL_MSG_PLAY,     PLAY_STATUS_PLAY},
        {PLAYERCONTROL_MSG_ERROR,    PLAY_STATUS_ERR},
    },
    { /* pause */
        {PLAYERCONTROL_MSG_STOP,    PLAY_STATUS_IDLE},
        {PLAYERCONTROL_MSG_PLAY,    PLAY_STATUS_PLAY},
        {PLAYERCONTROL_MSG_ERROR,   PLAY_STATUS_ERR},
        {PLAYERCONTROL_MSG_BUTT,    PLAY_STATUS_PAUSE}
    },
    { /* error */
        {PLAYERCONTROL_MSG_STOP,    PLAY_STATUS_IDLE},
        {PLAYERCONTROL_MSG_BUTT,    PLAY_STATUS_ERR},
        {PLAYERCONTROL_MSG_BUTT,    PLAY_STATUS_ERR},
        {PLAYERCONTROL_MSG_BUTT,    PLAY_STATUS_ERR}
    }
};

class PlayerControl {
public:
    PlayerControl();
    ~PlayerControl();

    int32_t Init(const PlayerControlParam &createParam); // playercontrol create

    int32_t Deinit(void); // playercontrol destroy

    int32_t RegCallback(PlayerCtrlCallbackParam &eventCallback);

    int32_t SetDataSource(const std::string filepath);
    int32_t SetDataSource(SourceFdParam &fdSrc);
    int32_t SetDataSource(StreamSourceParam &streamSource);

    int32_t Prepare(void);

    int32_t Play(void);

    int32_t SetVolume(const VolumeAttr &volumeAttr);

    int32_t Stop(void);

    int32_t Pause(void);

    int32_t Seek(int64_t timeInMs);

    int32_t GetFileInfo(FormatFileInfo &formatInfo);

    int32_t SetMedia(PlayerControlStreamAttr &mediaAttr);

#if defined(ENABLE_UIKIT)
    int32_t SetSurface(Surface *surface);
#elif defined(ENABLE_LVGL)
    int32_t SetSurface(LvSurfaceView *surface);
#endif

    int32_t SetPlaybackSpeed(float speed);

    int32_t Invoke(PlayerInvoke invokeId, const void *param);

    int32_t OnPlayControlEvent(void *priv, const EventCbType event) const;

    void OnVideoEndOfStream(void);

    void StateChangeCallback(PlayerStatus state) const;

    void RecordTime(struct timeval &timeStart);

    uint64_t CalcTimeDiffUsPrint(const struct timeval &timeStart, struct timeval &timeEnd, std::string outString,
        bool showlog);
    int32_t GetPlayerControlDumpInfo(PlayerControlDebugInfo &playerControlInfo);
    
    int32_t GetPlayerVideoDumpInfo(PlayerVideoDebugInfo &videoDebugInfo);
    
    int32_t GetPlayerAudioDumpInfo(PlayerAudioDebugInfo &audioDebugInfo);
    
    int32_t GetPlayerFileDumpInfo(PlayerFileDebugInfo &fileDebugInfo);
    void PlayerControlCondSignal(void);
    void EnableSingleLooping(bool loop);
    int32_t SetParam(const std::string &key, const void *value, uint32_t len);
    int32_t GetParam(const std::string &key, void *value, uint32_t size);
protected:
    int32_t DoRegCallback(const PlayerCtrlCallbackParam &eventCallback);

    int32_t DoSetDataSource(const char *filepath);
    int32_t DoSetDataSource(const SourceFdParam &fdSrc);
    int32_t DoSetDataSource(const StreamSourceParam &streamSource);
    int32_t DoPrepare(void);

    int32_t DoPlay(void);

    int32_t DoSetVolume(const VolumeAttr &volumeAttr);

    int32_t ReadPacketAndPushToDecoder(void);

    int32_t DoStop(void);

    int32_t DoSeek(int64_t timeInMs);

    int32_t DoPause(void);

    int32_t DoGetFileInfo(FormatFileInfo &fileInfo) const;

    int32_t DoSetMedia(const PlayerControlStreamAttr &mediaAttr);

    int32_t DoSetPlaybackSpeed(float speed);

    int32_t DoInvoke(const InvokeParameter &invokeParam);
    void ClearCachePacket(void);
    void NotifyError(PlayerControlError playerError) const;
    void EventCallback(PlayerControlEvent event, const void *data) const;
    void RecordLastPosition();
private:
    int32_t InitAttr(const PlayerControlParam &createParam);
    bool IsPlayEos(void) const;
    int32_t CheckMediaType(const FormatFileInfo &fmtFileInfo);
    int32_t SyncPrepare(void);
    int32_t AyncSeek(int64_t seekTime);
    int32_t ReadPacket(void);
    int32_t ReadFrameFromSource(FormatFrame &fmtFrame);
    int32_t GetVideoResolution(int32_t streamIdx, StreamResolution &resolution);
    void DestroyDecoder(void) const;
    void StopSinkAndDecoder(void);
    void GetPlayElementEventCallBack(PlayEventCallback &callback);
    int32_t PauseResume(void);
    int32_t DecoderAndSinkReset(void);
    int32_t SinkStart(void);
    int32_t DecoderStart(void);
#ifndef UNSUPPORT_AUDIO_DECODER
    int32_t AudioDecoderStart(void);
#endif
    int32_t VideoDecoderStart(void);
    int32_t SetDecoderAndStreamAttr(void);
    int32_t CheckMediaInfo(void) const;
    int32_t AddAudioSink(void);
    int32_t AddVideoSink();
    int32_t SetVDecAttr(void);
    int32_t SetADecAttr(void);
    int32_t SetStreamAttr();
    int32_t DequeueInputDataFromDemuxer(void);
#ifdef UNSUPPORT_AUDIO_DECODER
    int32_t QueueInputDataToAudioSink(void);
#else
    void QueueInputDataToAudioDecoder(void);
    void DequeueOutputBufferAndRenderAudioFrame(void);
#endif
#if !defined(UNSUPPORT_VIDEO_DECODER)
    int32_t QueueOutputBufferToVideoDecoder(void);
    void QueueInputDataToVideoDecoder(void);
    void DequeueOutputBufferAndRenderVideoFrame(void);
#endif
    void ReleaseAudioSinkESFrame(void);
    void ReleaseADecoderOutputFrame(void);
    void ReleaseVDecoderOutputFrame(void) const;
    int32_t DealWithInputEos(bool hasAudio, bool hasVideo);
    int32_t ProcessInputData(bool isAudio, bool isVideo);
    static void DataSchPreProcess(PlayerControl *play, bool *isContinue);
    static void DataSchPostProcess(PlayerControl *play);
    static void *DataSchProcess(void *priv);
    void ReportRenderPosition(void);
    int32_t DoSeekIfNeed(void);
    void FlushDecoder(void) const;
    void EventProcess(EventCbType event);
    void EventQueueProcess(void);
    PlayerStatus GetCurState(void);
    void AddTransition(void);
    PlayerStatus FindTransition(PlayerStatus state, int32_t event) const;
    int32_t OnMessageReceived(const MsgInfo &msg);
    int HandleMessage(const MsgInfo &msgInfo);
    void OnEventHandled(int event, int result) const;
    bool EventValidAtCurState(PlayerControlMsgType type);
    int32_t GetOffloadSupport(void);
    int HandleMsgWithNoParam(const MsgInfo &msgInfo);
    int HandleMsgWithParam(const MsgInfo &msgInfo);
    void PrintKpiTime();
#ifdef PLAYER_KPI_ON
    void PrintRenderEsTime(const struct timeval &esFrameStartTime);
    void ReadPacketAvgCostTime(const struct timeval &esFrameStartTime);
#endif
    bool UIntMulIsOverflow64(uint64_t a, uint64_t b, uint64_t *value) const;
    bool UIntMulIsOverflow32(uint32_t a, uint32_t b, uint32_t *value) const;
#ifdef DEBUG_LOW_POWER
    void LowPowerProcess(int64_t pts, uint32_t &renderSuccessCount, bool renderFrameSuccess);
#endif
    void FillAudioFrameInfo(EsFrameInfo &outInfo);
    void GetRenderFullSleepThresholdTime(AudioCodecFormat codecFormat);
    void TPlayRewindToNormal();
    TplayMode TPlayGetPlayMode(void);
    int32_t TPlayGetSeekOffset(float speed);
    int32_t TplayReadPacket();
    int32_t TPlayBeforeFrameRead(void);
    int32_t TPlayAfterFrameRead(FormatFrame& packet);

private:
    bool isInited_;
    FormatFileInfo fmtFileInfo_;
    PlayerCtrlCallbackParam eventCallback_; /* the regeisted call back function */
    bool isPlayEnd_;
    bool isVidPlayEos_;
    bool isAudPlayEos_;
    PlayerControlParam playerParam_;
    FormatFrame formatPacket_;
    bool cachedPacket_;
    int64_t lastRendPos_;
    uint64_t lastNotifyTime_;
    uint32_t lastReadPktStrmIdx_;
    int64_t lastReadPktPts_;
    int64_t lastSendPktPts_;

    // TPlay
    float speed_;
    TplayMode tplayMode_;
    int32_t curSeekOffset_;
    bool isTplayStartRead_;
    bool isTplayLastFrame_;

    bool isVideoStarted_;
    bool isAudioStarted_;
    bool strmReadEnd_;
    struct timeval streamEndTime_;
    bool isPlayErr_;
    int64_t lastSendVdecPts_;
    int64_t lastSendAdecPts_;
    int64_t currentPosition_;
    PlayControlStreamAttr streamAttr_;
    PlayControlVdecAttr vdecAttr_;
    PlayControlAdecAttr adecAttr_;
    bool paused_;
    bool schThreadExit_;
    bool loop_;
    bool hasRenderAudioEos_;
    bool hasRenderVideoEos_;
    uint32_t renderSleepTime_;
    float leftVolume_;
    float rightVolume_;
    MediaMutexHandle schMutex_;
    MediaThreadCondHandle schCond_;
    MediaThreadCondHandle pauseCond_;
    MediaThreadIdHandle schProcess_;
    int64_t seekToTimeMs_;
    SourceType sourceType_;
    bool firstAudioFrameAfterSeek_;
    bool firstVideoFrameAfterSeek_;
    bool firstAudioFrameSend_;
    bool firstVideoFrameSend_;
    int64_t firstPosAfterSeek_;
    int32_t fd_;
    uint64_t offset_;
    std::string filePath_;
    StreamSourceParam streamSource_;
    std::shared_ptr<PlayerSource> playerSource_;
    std::shared_ptr<SinkManager> sinkManager_;
#if !defined(UNSUPPORT_AUDIO_DECODER)
    std::shared_ptr<Decoder> audioDecoder_;
#endif
#if !defined(UNSUPPORT_VIDEO_DECODER)
    std::shared_ptr<Decoder> videoDecoder_;
#endif
#if defined(ENABLE_UIKIT)
    Surface *surface_;
    SurfaceBuffer *surfaceBuf_;
#elif defined(ENABLE_LVGL)
    LvSurfaceView *surface_;
    LvSurfaceBuffer *surfaceBuf_;
#endif
    std::vector<PalayControlEventItem> eventQueue_;
    PlayerStatus curState_;
    bool offloadAudio_;
    uint32_t audioSession_;
    uint32_t streamType_;
    ParameterItem metaData_;
    bool recordFirstFrameCost_;
    struct timeval startPlayStartTime_;
    struct timeval startPlayEndTime_;
    uint64_t renderEsTotalTime_;
    uint64_t readPktTotalTime_;
    uint32_t renderEsFrameCnt_;
    uint32_t readPacketCnt_;
    int32_t outDeviceId_;
    int64_t headInSendSuccessInterval_;
    int64_t tailInSendSuccessInterval_;
    uint32_t renderFullSleepThresholdMs_;
    bool hasAudio_;
    bool hasVideo_;
    bool isAudio_;
    bool isVideo_;
    uint32_t continousRequestSurfaceBufferFailCount_; // continous requset surface buffer fail count
private:
    PlayerControl(const PlayerControl &);
    PlayerControl &operator = (const PlayerControl &);
};
}
}
#endif // LITEPLAYER_H__

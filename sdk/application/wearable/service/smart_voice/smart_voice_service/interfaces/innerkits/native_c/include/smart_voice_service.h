/*
 * Copyright (c) CompanyNameMagicTag 2021. All rights reserved.
 * Description: smart voice service.
 */
#ifndef SMART_VS_H
#define SMART_VS_H

#include "td_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef enum {
    WAKEUP_ON_VAD,        // keep voice engine in sleep state, until human voice is detected.
    SLEEP_ON_VAD_TIMEOUT, // work linke WAKEUP_ON_VAD, if there is no human voice, go back to sleep again.
    ALWAYS_WAKE,          // no matter whether there is human voice, keep voice engine in wake state.
} AsrPowerMode;

typedef enum {
    AUDIO_STREAM,     // Audio stream that set up by media framework.
    INNER_AUDIO_PORT, // internal Audio port.
    PCM_INJECTION,    // call LocalAsrWriteData to inject pcm data.
} AsrInputMode;

typedef enum {
    TRIGGER_BY_SCREEN_EVENT, // local asr only work at the time screen is on.
    ALWAYS_ON,
    ALWAYS_OFF,
} AsrWorkMode;

typedef enum {
    SMART_VOICE_EVENT_VOICE_BEGIN,
    SMART_VOICE_EVENT_VOICE_END,
    SMART_VOICE_EVENT_VOICE_COMMAND,
    SMART_VOICE_EVENT_MAX
} SmartVoiceEventType;

typedef enum {
    SMART_VOICE_ACCEPT_CALL_CMD,    // 接听电话
    SMART_VOICE_REJECT_CALL_CMD,    // 拒接电话
    SMART_VOICE_PLAY_LAST_CMD,      // 上一首
    SMART_VOICE_PLAY_NEXT_CMD,      // 下一首
    SMART_VOICE_VOLUME_UP_CMD,      // 调大音量
    SMART_VOICE_VOLUME_DOWN_CMD,    // 调小音量
    SMART_VOICE_START_MUSIC_CMD,    // 开始播放
    SMART_VOICE_STOP_MUSIC_CMD,     // 停止播放
    SMART_VOICE_INVALID_CMD_ID
} SmartVoiceCmdId;

typedef struct {
    td_u32 userId;
    SmartVoiceCmdId cmdId;
    td_uchar *cmdArgs;
    td_u32 cmdArgsLen;
} SmartVoiceCmdParam;

typedef struct {
    AsrPowerMode powerMode;
    AsrInputMode inputMode;
    td_s32 enableAEC; // when input mode is AUDIO_STREAM, AEC is always on, enableAEC has no effect.
    td_s32 enableNpu;
} LocalAsrInitParams;

typedef td_void (*LocalAsrCb)(SmartVoiceEventType type, td_void *data, td_s32 size);

td_s32 LocalAsrInit(LocalAsrInitParams *initParams);
td_s32 LocalAsrDeInit(td_void);
td_s32 LocalAsrSetMode(AsrWorkMode workMode);
AsrWorkMode LocalAsrGetMode(td_void);
td_s32 LocalAsrStart(td_void);
td_s32 LocalAsrStop(td_void);
td_u32 LocalAsrReadData(td_uchar *voiceData, td_u32 length);
td_u32 LocalAsrWriteData(td_uchar *voiceData, td_u32 length);
td_s32 LocalAsrSetCallback(LocalAsrCb callback);
td_float LocalAsrCalcRms(td_uchar *voiceData, td_u32 length);
td_void LocalAsrDump(td_void);
td_u32 LocalAsrReadRawData(td_uchar *voiceData, td_u32 length);
td_void LocalAsrSetHifiMode(td_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif // SMART_VS_H
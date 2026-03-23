/**
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides at cmd register func \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2022-07-20， Create file. \n
 */
#ifndef MEDIA_AT_SERVICE_H
#define MEDIA_AT_SERVICE_H

#include <stdint.h>
#include <stdio.h>
#include "errcode.h"
#include "soc_osal.h"
#include "errcode.h"
#include "diag_service.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef int32_t (*MediaProcessFunc)(const uint8_t *data, uint32_t dataLen);

typedef enum {
    MEDIA_AUDIO_PLAY,
    MEDIA_AUDIO_RECORD,
    MEDIA_AUDIO_SCENE_INTERACTION,
    MEDIA_VIDEO_PLAY,
    MEDIA_CAMERA_PREVIEW,
    MEDIA_VOICE_CALL_VOLTE,
    MEDIA_A2DP_SINK,
    MEDIA_AUDIO_RECORDER,
    MEDIA_AUDIO_CMD_MAX,
} MediaAudioCmd;

typedef struct {
    uint8_t id;
    MediaProcessFunc func;
} MediaProcessType;


typedef enum {
    MEDIA_AUDIO_PLAY_START,             // 00
    MEDIA_AUDIO_PLAY_PAUSE,             // 01
    MEDIA_AUDIO_PLAY_RESUME,            // 02
    MEDIA_AUDIO_PLAY_STOP,              // 03
    MEDIA_AUDIO_PLAY_ALBUM_INFO,        // 04
    MEDIA_AUDIO_PLAY_SET_VOLUME,        // 05
    MEDIA_AUDIO_PLAY_GET_MUSIC_VOLUME,  // 06
    MEDIA_AUDIO_PLAY_SET_BT_SCO_VOLUME, // 07
    MEDIA_AUDIO_PLAY_GET_BT_SCO_VOLUME, // 08
    MEDIA_AUDIO_PLAY_SINGLE_LOOP,       // 09
    MEDIA_AUDIO_PLAY_SWITCH_SONG,       // 0A
    MEDIA_AUDIO_PLAY_CURRENT_STATE,     // 0B
    MEDIA_NOTIFY_PLAY_START,            // 0C
    MEDIA_BACKGROUND_AUDIO_PLAY_START,  // 0D
#if (CONFIG_MEDIAAW_AUDIO_ENABLE == FEATURE_ON)
    MEDIA_AW,
#endif
    MEDIA_AUDIO_PLAY_MUTE,
    MEDIA_AUDIO_MAX_PLAY_ID,
} MediaAudioPlayCmd;

typedef enum {
    MEDIA_AUDIO_RECORD_START,
    MEDIA_AUDIO_RECORD_STOP,
    MEDIA_AUDIO_MAX_RECORD_ID,
} MediaAudioRecordCmd;

typedef enum {
    MEDIA_AUDIO_SCENE_INTERACTION_001 = 1,
    MEDIA_AUDIO_SCENE_INTERACTION_002,
    MEDIA_AUDIO_SCENE_INTERACTION_003,
    MEDIA_AUDIO_SCENE_INTERACTION_004,
    MEDIA_AUDIO_SCENE_INTERACTION_005,
    MEDIA_AUDIO_SCENE_INTERACTION_006,
    MEDIA_AUDIO_SCENE_INTERACTION_007,
} MediaAudioSceneInteractionID;

typedef enum {
    MEDIA_VIDEO_PLAY_START,         // 00
    MEDIA_VIDEO_PLAY_PAUSE,         // 01
    MEDIA_VIDEO_PLAY_RESUME,        // 02
    MEDIA_VIDEO_PLAY_STOP,          // 03
    MEDIA_VIDEO_PLAY_SINGLE_LOOP,   // 04
    MEDIA_VIDEO_PLAY_SET_VOLUME,    // 05
    MEDIA_VIDEO_PLAY_GET_VOLUME,    // 06
    MEDIA_VIDEO_PLAY_CURRENT_STATE, // 07
    MEDIA_VIDEO_PLAY_MUTE,          // 08
    MEDIA_VIDEO_MAX_PLAY_ID,
} MediaVideoPlayCmd;

typedef enum {
    MEDIA_CAMERA_PREVIEW_START,    // 00
    MEDIA_CAMERA_PREVIEW_STOP,     // 01
} MediaCameraPreviewCmd;

typedef enum {
    MEDIA_VOICE_CALL_VOLTE_HANDLE,   // 01
} MediaVoiceCallVolteCmd;

typedef enum {
    MEDIA_A2DP_SINK_AEF,             // 00
} MediaA2dpSinkCmd;

typedef enum {
    MEDIA_AUDIO_RECORDER_CMD,             // 00
} MediaAudioRecorderCmd;

void media_at_process(const uint8_t *data, uint32_t dataLen);
uint32_t media_diag_cmd_process(diag_ser_data_t *data);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif

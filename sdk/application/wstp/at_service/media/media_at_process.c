/**
 * Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides media at service. \n
 * Author: CompanyName \n
 * History: \n
 * 2022-11-17， Create file. \n
 */

#include "media_at_process.h"
#include "debug_print.h"
#include "securec.h"
#include "at_cmd_api.h"
#include "common_def.h"
#include "soc_uapi_audio_sys.h"
#ifdef MEMORY_MINI
#include "tone_player_sample_wrapper.h"
#else
#include "audio_capture_wrapper.h"
#include "audio_scene_interaction.h"
#include "video_player_sample_wrapper.h"
#include "camera_preview_sample_wrapper.h"
#include "voice_call_volte_sample_wrapper.h"
#include "audio_player_test.h"
#ifndef MEMORY_MINI
#include "audio_recorder_sample.h"
#endif
#endif
#include "app_at_process.h"
#include "app_msg_manager.h"
#include "media_tx_process.h"
#include "audio_manager_c_wrapper.h"
#include "player_sample_wrapper.h"

#define ARRAY_COUNT(array) (sizeof(array) / sizeof(array[0]))
#define BSP_MEM_ALLOC(y) osal_kmalloc((y), OSAL_GFP_KERNEL)
#define BSP_MEM_FREE(y) osal_kfree(y)

#define MEDIA_SCENE_POS 0
#define MEDIA_SUB_CMD_POS 1
#define MEDIA_PARAM_OFFSET 2
#define MEDIA_MAX_PARAM_CNT 10

#define MEDIA_APP_MSG_MIN_LEN 9
#define MEDIA_APP_CRC_LEN 2
#define MEDIA_APP_SCENE_POS 9
#define MEDIA_APP_REPLY_DATA_LEN 4

bool g_media_diag_flag = false;             // 用于控制执行功能后是否回复diag消息
diag_ser_header_t g_media_diag_header;      // 用于保存diag回复包头内容

uint32_t media_diag_cmd_process(diag_ser_data_t *data)
{
    if (data == NULL) {
        return ERRCODE_FAIL;
    }

    uint8_t* ptr;
    uint32_t media_data_len;
    uint8_t *media_data;
    diag_ser_frame_t* frame = (diag_ser_frame_t*)data->payload;

    parse_tlv_2value(frame->tlv, data->header.length, &ptr, &media_data_len);
    media_data_len += 1;
    media_data = (uint8_t *)BSP_MEM_ALLOC(media_data_len);
    if (media_data == NULL) {
        PRINT("{%s():%d} malloc failed.\r\n", __FUNCTION__, __LINE__);
        return ERRCODE_FAIL;
    }

    (void)memcpy_s(&g_media_diag_header, sizeof(diag_ser_header_t), &(data->header), sizeof(diag_ser_header_t));
    if (memcpy_s(media_data, media_data_len, ptr, (media_data_len - 1)) != EOK) {
        PRINT("memcpy failed.\r\n");
        BSP_MEM_FREE(media_data);
        return ERRCODE_FAIL;
    }
    media_data[media_data_len - 1] = '\0';
    g_media_diag_flag = true;

    media_at_msg_send(media_data, media_data_len);
    BSP_MEM_FREE(media_data);
    return ERRCODE_SUCC;
}

static int32_t ConvertCmdParamToSampleParam(const uint8_t *inParam, uint16_t paramLen, int32_t *outArgc, uint8_t **outArgv)
{
    int ret;
    char *pos;
    uint8_t *strTemp;
    uint32_t paramCnt = 0;

    strTemp = (uint8_t*)BSP_MEM_ALLOC(paramLen);
    if (strTemp == NULL) {
        wstp_print("{%s():%d} malloc failed.\r\n", __FUNCTION__, __LINE__);
        return ERRCODE_FAIL;
    }

    ret = memset_s(strTemp, paramLen, 0, paramLen);
    if (ret != EOK) {
        wstp_print("{%s():%d} memset_s failed.\r\n", __FUNCTION__, __LINE__);
        BSP_MEM_FREE(strTemp);
        return ERRCODE_FAIL;
    }

    ret = memcpy_s(strTemp, paramLen, &inParam[MEDIA_PARAM_OFFSET], paramLen);
    if (ret != EOK) {
        wstp_print("{%s():%d} memcpy_s failed.\r\n", __FUNCTION__, __LINE__);
        BSP_MEM_FREE(strTemp);
        return ERRCODE_FAIL;
    }

    pos = strtok((char*)strTemp, ",");
    for (uint32_t i = 0; pos != NULL; i++) {
        outArgv[i] = (uint8_t*)BSP_MEM_ALLOC(strlen(pos) + 1);
        if (outArgv[i] == NULL) {
            wstp_print("{%s():%d} malloc failed.\r\n", __FUNCTION__, __LINE__);
            BSP_MEM_FREE(strTemp);
            return ERRCODE_FAIL;
        }
        ret = memcpy_s(outArgv[i], strlen(pos) + 1, pos, strlen(pos) + 1);
        if (ret != EOK) {
            wstp_print("{%s():%d} memcpy_s failed.\r\n", __FUNCTION__, __LINE__);
            BSP_MEM_FREE(strTemp);
            return ERRCODE_FAIL;
        }
        paramCnt++;
        if (*outArgc >= MEDIA_MAX_PARAM_CNT) {
            wstp_print("{%s():%d} invalid argc: %d(max: 64).\r\n", __FUNCTION__, __LINE__, *outArgc);
            BSP_MEM_FREE(strTemp);
            return ERRCODE_FAIL;
        }
        pos = strtok(NULL, ",");
    }

    *outArgc = paramCnt;
    BSP_MEM_FREE(strTemp);
    return ERRCODE_SUCC;
}

static int32_t MediaNotifyPlayStart(const uint8_t *data, uint32_t dataLen)
{
    wstp_print("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);

    int32_t argc = 0;
    uint8_t *argv[MEDIA_MAX_PARAM_CNT];
    int32_t ret = ERRCODE_FAIL;

    ret = ConvertCmdParamToSampleParam(data, dataLen, &argc, argv);
    if (ret != ERRCODE_SUCC) {
        goto FREE;
    }

    for (int32_t i = 0; i < argc; i++) {
        wstp_print("{%s():%d} argv[%d]: %s\r\n", __FUNCTION__, __LINE__, i, argv[i]);
    }
    ret = TonePlayerSampleTest(argc, (const char **)argv);
FREE:
    for (int32_t i = 0; i < argc; i++) {
        if (argv[i] != NULL) {
            BSP_MEM_FREE(argv[i]);
            argv[i] = NULL;
        }
    }
    wstp_print("{%s():%d} func out %s.\r\n", __FUNCTION__, __LINE__, (ret == ERRCODE_SUCC) ? "success" : "failure");
    return ret;
}

#ifndef MEMORY_MINI
static int32_t MediaBackgroundAudioPlayStart(const uint8_t *data, uint32_t dataLen)
{
    wstp_print("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);

    int32_t argc = 0;
    uint8_t *argv[MEDIA_MAX_PARAM_CNT];
    int32_t ret = ERRCODE_FAIL;

    ret = ConvertCmdParamToSampleParam(data, dataLen, &argc, argv);
    if (ret != ERRCODE_SUCC) {
        goto FREE;
    }

    for (int32_t i = 0; i < argc; i++) {
        wstp_print("{%s():%d} argv[%d]: %s\r\n", __FUNCTION__, __LINE__, i, argv[i]);
    }
    ret = AudioPlayerSampleTest(argc, (const char **)argv);
FREE:
    for (int32_t i = 0; i < argc; i++) {
        if (argv[i] != NULL) {
            BSP_MEM_FREE(argv[i]);
            argv[i] = NULL;
        }
    }
    wstp_print("{%s():%d} func out %s.\r\n", __FUNCTION__, __LINE__, (ret == ERRCODE_SUCC) ? "success" : "failure");
    return ret;
}
#endif

static int32_t MediaAudioPlayStart(const uint8_t *data, uint32_t dataLen)
{
    wstp_print("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);

    int32_t argc = 0;
    uint8_t *argv[MEDIA_MAX_PARAM_CNT];
    int32_t ret = ERRCODE_FAIL;

    ret = ConvertCmdParamToSampleParam(data, dataLen, &argc, argv);
    if (ret != ERRCODE_SUCC) {
        goto FREE;
    }

    for (int32_t i = 0; i < argc; i++) {
        wstp_print("{%s():%d} argv[%d]: %s\r\n", __FUNCTION__, __LINE__, i, argv[i]);
    }

    ret = PlayerSample(argc, (const char **)argv);

FREE:
    for (int32_t i = 0; i < argc; i++) {
        if (argv[i] != NULL) {
            BSP_MEM_FREE(argv[i]);
            argv[i]  = NULL;
        }
    }
    wstp_print("{%s():%d} func out %s.\r\n", __FUNCTION__, __LINE__, (ret == ERRCODE_SUCC) ? "success" : "failure");
    return ret;
}

static int32_t MediaAudioPlayPause(const uint8_t *data, uint32_t dataLen)
{
    wstp_print("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);

    unused(data);
    unused(dataLen);

    int32_t argc = 1;
    uint8_t *argv = (uint8_t*)"pause";

    PlayerSample(argc, (const char **)&argv);

    wstp_print("{%s():%d} func out success.\r\n", __FUNCTION__, __LINE__);
    return ERRCODE_SUCC;
}

static int32_t MediaAudioPlayResume(const uint8_t *data, uint32_t dataLen)
{
    wstp_print("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);

    unused(data);
    unused(dataLen);

    int32_t argc = 1;
    uint8_t *argv = (uint8_t*)"resume";

    PlayerSample(argc, (const char **)&argv);

    wstp_print("{%s():%d} func out success.\r\n", __FUNCTION__, __LINE__);
    return ERRCODE_SUCC;
}

static int32_t MediaAudioPlaySetVolume(const uint8_t *data, uint32_t dataLen)
{
    wstp_print("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);

    int32_t ret;
    int32_t argc = 0;
    uint8_t *argv[MEDIA_MAX_PARAM_CNT];

    ret = ConvertCmdParamToSampleParam(data, dataLen, &argc, argv);
    if (ret != ERRCODE_SUCC) {
        goto FREE;
    }

    PlayerSample(argc, (const char **)&argv);

FREE:
    for (int32_t i = 0; i < argc; i++) {
        if (argv[i] != NULL) {
            BSP_MEM_FREE(argv[i]);
            argv[i]  = NULL;
        }
    }

    wstp_print("{%s():%d} func out success.\r\n", __FUNCTION__, __LINE__);
    return ERRCODE_SUCC;
}

#ifndef MEMORY_MINI
static int32_t MediaAudioPlayGetMusicVolume(const uint8_t *data, uint32_t dataLen)
{
    wstp_print("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);

    unused(data);
    unused(dataLen);

    int32_t argc = 1;
    uint8_t *argv = (uint8_t*)"getmusicvolume";

    PlayerSample(argc, (const char **)&argv);

    wstp_print("{%s():%d} func out success.\r\n", __FUNCTION__, __LINE__);
    return ERRCODE_SUCC;
}

static int32_t MediaAudioPlaySetBTSCOVolume(const uint8_t *data, uint32_t dataLen)
{
    wstp_print("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);

    int32_t ret;
    int32_t argc = 0;
    uint8_t *argv[MEDIA_MAX_PARAM_CNT];

    ret = ConvertCmdParamToSampleParam(data, dataLen, &argc, argv);

    if (0 < atoi((char*)argv[0]) && atoi((char*)argv[0]) < 100 && argc == 1) {
        argv[1] = (uint8_t *)BSP_MEM_ALLOC(strlen((char*)argv[0]) + 1);
        ret = memcpy_s((char*)argv[1], (MEDIA_MAX_PARAM_CNT - 1), (char*)argv[0], strlen((char*)argv[0]) + 1);
        if (ret != EOK) {
            wstp_print("{%s():%d} memcpy_s failed.\r\n", __FUNCTION__, __LINE__);
            BSP_MEM_FREE(argv[0]);
            BSP_MEM_FREE(argv[1]);
            return ERRCODE_FAIL;
        }
        BSP_MEM_FREE(argv[0]);
        argv[0] = (uint8_t*)"scovolume";
        argc = 2;
    }

    PlayerSample(argc, (const char **)&argv);

    BSP_MEM_FREE(argv[1]);

    wstp_print("{%s():%d} func out success.\r\n", __FUNCTION__, __LINE__);
    return ERRCODE_SUCC;
}

static int32_t MediaAudioPlayGetBTSCOVolume(const uint8_t *data, uint32_t dataLen)
{
    wstp_print("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);

    unused(data);
    unused(dataLen);

    int32_t argc = 1;
    uint8_t *argv = (uint8_t*)"getscovolume";

    PlayerSample(argc, (const char **)&argv);

    wstp_print("{%s():%d} func out success.\r\n", __FUNCTION__, __LINE__);
    return ERRCODE_SUCC;
}
#endif

static int32_t MediaAudioPlaySingleLoop(const uint8_t *data, uint32_t dataLen)
{
    wstp_print("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);

    int32_t ret;
    int32_t argc = 0;
    uint8_t *argv[MEDIA_MAX_PARAM_CNT];

    ret = ConvertCmdParamToSampleParam(data, dataLen, &argc, argv);
    if (ret != ERRCODE_SUCC) {
        goto FREE;
    }

    PlayerSample(argc, (const char **)argv);

FREE:
    for (int32_t i = 0; i < argc; i++) {
        if (argv[i] != NULL) {
            BSP_MEM_FREE(argv[i]);
            argv[i] = NULL;
        }
    }

    wstp_print("{%s():%d} func out success.\r\n", __FUNCTION__, __LINE__);
    return ERRCODE_SUCC;
}

static int32_t MediaAudioPlayMute(const uint8_t *data, uint32_t dataLen)
{
    wstp_print("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);

    unused(data);
    unused(dataLen);

    wstp_print("{%s():%d} func out success.\r\n", __FUNCTION__, __LINE__);
    return ERRCODE_SUCC;
}

static int32_t MediaAudioPlaySwitchSong(const uint8_t *data, uint32_t dataLen)
{
    wstp_print("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);

    int32_t ret;
    int32_t argc = 0;
    uint8_t *argv[MEDIA_MAX_PARAM_CNT];

    ret = ConvertCmdParamToSampleParam(data, dataLen, &argc, argv);
    if (ret != ERRCODE_SUCC) {
        goto FREE;
    }

    PlayerSample(argc, (const char **)argv);

FREE:
    for (int32_t i = 0; i < argc; i++) {
        if (argv[i] != NULL) {
            BSP_MEM_FREE(argv[i]);
            argv[i]  = NULL;
        }
    }

    wstp_print("{%s():%d} func out success.\r\n", __FUNCTION__, __LINE__);
    return ERRCODE_SUCC;
}

static int32_t MediaAudioPlayCurrentState(const uint8_t *data, uint32_t dataLen)
{
    wstp_print("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);

    unused(data);
    unused(dataLen);

    int32_t argc = 1;
    uint8_t *argv = (uint8_t*)"currentstate";

    PlayerSample(argc, (const char **)&argv);

    wstp_print("{%s():%d} func out success.\r\n", __FUNCTION__, __LINE__);
    return ERRCODE_SUCC;
}

#ifndef MEMORY_MINI
static int32_t MediaAudioPlayAlbumInfo(const uint8_t *data, uint32_t dataLen)
{
    wstp_print("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);

    unused(data);
    unused(dataLen);

    int32_t argc = 1;
    uint8_t *argv = (uint8_t*)"albuminfo";

    PlayerSample(argc, (const char **)&argv);

    wstp_print("{%s():%d} func out success.\r\n", __FUNCTION__, __LINE__);
    return ERRCODE_SUCC;
}
#endif

static int32_t MediaAudioPlayStop(const uint8_t *data, uint32_t dataLen)
{
    wstp_print("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);

    unused(data);
    unused(dataLen);

    int32_t argc = 1;
    uint8_t *argv = (uint8_t*)"stop";

    PlayerSample(argc, (const char **)&argv);

    wstp_print("{%s():%d} func out success.\r\n", __FUNCTION__, __LINE__);
    return ERRCODE_SUCC;
}

#ifndef MEMORY_MINI
static int32_t MediaAudioRecordStart(const uint8_t *data, uint32_t dataLen)
{
    wstp_print("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);

    int32_t argc = 0;
    uint8_t *argv[MEDIA_MAX_PARAM_CNT];
    int32_t ret = ERRCODE_FAIL;

    ret = ConvertCmdParamToSampleParam(data, dataLen, &argc, argv);
    if (ret != ERRCODE_SUCC) {
        goto FREE;
    }

    for (int32_t i = 0; i < argc; i++) {
        wstp_print("{%s():%d} argv[%d]: %s.\r\n", __FUNCTION__, __LINE__, i, argv[i]);
    }

    AudioCaptureSample(argc, (const char **)argv);

FREE:
    for (int32_t i = 0; i < argc; i++) {
        if (argv[i] != NULL) {
            BSP_MEM_FREE(argv[i]);
            argv[i]  = NULL;
        }
    }

    wstp_print("{%s():%d} func out %s.\r\n", __FUNCTION__, __LINE__, (ret == ERRCODE_SUCC) ? "success" : "failure");
    return ret;
}

static int32_t MediaAudioRecordStop(const uint8_t *data, uint32_t dataLen)
{
    wstp_print("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);

    unused(data);
    unused(dataLen);

    int32_t argc = 1;
    uint8_t *argv = (uint8_t*)"stop";

    AudioCaptureSample(argc, (const char **)&argv);

    wstp_print("{%s():%d} func out success.\r\n", __FUNCTION__, __LINE__);
    return ERRCODE_SUCC;
}

static int32_t MediaAudioSceneInteraction001(const uint8_t *data, uint32_t dataLen)
{
    wstp_print("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);

    unused(data);
    unused(dataLen);

    (void)AudioSceneInteraction001(0, NULL);

    wstp_print("{%s():%d} func out success.\r\n", __FUNCTION__, __LINE__);
    return ERRCODE_SUCC;
}

static int32_t MediaAudioSceneInteraction002(const uint8_t *data, uint32_t dataLen)
{
    wstp_print("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);

    unused(data);
    unused(dataLen);

    (void)AudioSceneInteraction002(0, NULL);

    wstp_print("{%s():%d} func out success.\r\n", __FUNCTION__, __LINE__);
    return ERRCODE_SUCC;
}

static int32_t MediaAudioSceneInteraction003(const uint8_t *data, uint32_t dataLen)
{
    wstp_print("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);

    unused(data);
    unused(dataLen);

    (void)AudioSceneInteraction003(0, NULL);

    wstp_print("{%s():%d} func out success.\r\n", __FUNCTION__, __LINE__);
    return ERRCODE_SUCC;
}

static int32_t MediaAudioSceneInteraction004(const uint8_t *data, uint32_t dataLen)
{
    wstp_print("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);

    unused(data);
    unused(dataLen);

    (void)AudioSceneInteraction004(0, NULL);

    wstp_print("{%s():%d} func out success.\r\n", __FUNCTION__, __LINE__);
    return ERRCODE_SUCC;
}

static int32_t MediaAudioSceneInteraction005(const uint8_t *data, uint32_t dataLen)
{
    wstp_print("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);

    unused(data);
    unused(dataLen);

    (void)AudioSceneInteraction005(0, NULL);

    wstp_print("{%s():%d} func out success.\r\n", __FUNCTION__, __LINE__);
    return ERRCODE_SUCC;
}

static int32_t MediaAudioSceneInteraction006(const uint8_t *data, uint32_t dataLen)
{
    wstp_print("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);

    unused(data);
    unused(dataLen);

    (void)AudioSceneInteraction006(0, NULL);

    wstp_print("{%s():%d} func out success.\r\n", __FUNCTION__, __LINE__);
    return ERRCODE_SUCC;
}

static int32_t MediaAudioSceneInteraction007(const uint8_t *data, uint32_t dataLen)
{
    wstp_print("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);

    unused(data);
    unused(dataLen);

    (void)AudioSceneInteraction007(0, NULL);

    wstp_print("{%s():%d} func out success.\r\n", __FUNCTION__, __LINE__);
    return ERRCODE_SUCC;
}
#endif

static const MediaProcessType g_mediaAudioPlayFuncs[] = {
    { MEDIA_AUDIO_PLAY_START, MediaAudioPlayStart },
    { MEDIA_AUDIO_PLAY_PAUSE, MediaAudioPlayPause },
    { MEDIA_AUDIO_PLAY_RESUME, MediaAudioPlayResume },
    { MEDIA_AUDIO_PLAY_STOP, MediaAudioPlayStop },
#ifndef MEMORY_MINI
    { MEDIA_AUDIO_PLAY_ALBUM_INFO, MediaAudioPlayAlbumInfo },
#endif
    { MEDIA_AUDIO_PLAY_SET_VOLUME, MediaAudioPlaySetVolume },
#ifndef MEMORY_MINI
    { MEDIA_AUDIO_PLAY_GET_MUSIC_VOLUME, MediaAudioPlayGetMusicVolume },
    { MEDIA_AUDIO_PLAY_SET_BT_SCO_VOLUME, MediaAudioPlaySetBTSCOVolume },
    { MEDIA_AUDIO_PLAY_GET_BT_SCO_VOLUME, MediaAudioPlayGetBTSCOVolume },
#endif
    { MEDIA_AUDIO_PLAY_SINGLE_LOOP, MediaAudioPlaySingleLoop },
    { MEDIA_AUDIO_PLAY_SWITCH_SONG, MediaAudioPlaySwitchSong },

    { MEDIA_AUDIO_PLAY_CURRENT_STATE, MediaAudioPlayCurrentState },
#ifndef MEMORY_MINI
    { MEDIA_AUDIO_PLAY_MUTE, MediaAudioPlayMute },
#endif
    { MEDIA_NOTIFY_PLAY_START, MediaNotifyPlayStart },
#ifndef MEMORY_MINI
    { MEDIA_BACKGROUND_AUDIO_PLAY_START, MediaBackgroundAudioPlayStart },
#endif
};

#ifndef MEMORY_MINI
static int32_t MediaVideoPlayStart(const uint8_t *data, uint32_t dataLen)
{
    wstp_print("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);

    int32_t argc = 0;
    uint8_t *argv[MEDIA_MAX_PARAM_CNT];
    int32_t ret = ERRCODE_FAIL;

    ret = ConvertCmdParamToSampleParam(data, dataLen, &argc, argv);
    if (ret != ERRCODE_SUCC) {
        goto FREE;
    }

    for (int32_t i = 0; i < argc; i++) {
        wstp_print("{%s():%d} argv[%d]: %s\r\n", __FUNCTION__, __LINE__, i, argv[i]);
    }

    ret = VideoPlayerSample(argc, (const char **)argv);

FREE:
    for (int32_t i = 0; i < argc; i++) {
        if (argv[i] != NULL) {
            BSP_MEM_FREE(argv[i]);
            argv[i]  = NULL;
        }
    }
    wstp_print("{%s():%d} func out %s.\r\n", __FUNCTION__, __LINE__, (ret == ERRCODE_SUCC) ? "success" : "failure");
    return ret;
}

static int32_t MediaVideoPlayPause(const uint8_t *data, uint32_t dataLen)
{
    wstp_print("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);

    unused(data);
    unused(dataLen);

    int32_t argc = 1;
    uint8_t *argv = (uint8_t *)"pause";
    int32_t ret = ERRCODE_FAIL;

    ret = VideoPlayerSample(argc, (const char **)&argv);

    wstp_print("{%s():%d} func out %s.\r\n", __FUNCTION__, __LINE__, (ret == ERRCODE_SUCC) ? "success" : "failure");
    return ERRCODE_SUCC;
}

static int32_t MediaVideoPlayResume(const uint8_t *data, uint32_t dataLen)
{
    wstp_print("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);

    unused(data);
    unused(dataLen);

    int32_t argc = 1;
    uint8_t *argv = (uint8_t *)"resume";
    int32_t ret = ERRCODE_FAIL;

    ret = VideoPlayerSample(argc, (const char **)&argv);

    wstp_print("{%s():%d} func out %s.\r\n", __FUNCTION__, __LINE__, (ret == ERRCODE_SUCC) ? "success" : "failure");
    return ERRCODE_SUCC;
}

static int32_t MediaVideoPlayStop(const uint8_t *data, uint32_t dataLen)
{
    wstp_print("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);

    unused(data);
    unused(dataLen);

    int32_t argc = 1;
    uint8_t *argv = (uint8_t *)"stop";
    int32_t ret = ERRCODE_FAIL;

    ret = VideoPlayerSample(argc, (const char **)&argv);

    wstp_print("{%s():%d} func out %s.\r\n", __FUNCTION__, __LINE__, (ret == ERRCODE_SUCC) ? "success" : "failure");
    wstp_print("{%s():%d} exit\n", __FUNCTION__, __LINE__);
    return ERRCODE_SUCC;
}

static int32_t MediaVideoPlaySingleLoop(const uint8_t *data, uint32_t dataLen)
{
    wstp_print("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);

    int32_t argc = 0;
    uint8_t *argv[MEDIA_MAX_PARAM_CNT];
    int32_t ret = ERRCODE_FAIL;

    ret = ConvertCmdParamToSampleParam(data, dataLen, &argc, argv);
    if (ret != ERRCODE_SUCC) {
        goto FREE;
    }

    for (int32_t i = 0; i < argc; i++) {
        wstp_print("{%s():%d} argv[%d]: %s\r\n", __FUNCTION__, __LINE__, i, argv[i]);
    }

    ret = VideoPlayerSample(argc, (const char **)argv);

FREE:
    for (int32_t i = 0; i < argc; i++) {
        if (argv[i] != NULL) {
            BSP_MEM_FREE(argv[i]);
            argv[i] = NULL;
        }
    }
    wstp_print("{%s():%d} func out %s.\r\n", __FUNCTION__, __LINE__, (ret == ERRCODE_SUCC) ? "success" : "failure");
    return ret;
}

static int32_t MediaVideoPlaySetVolume(const uint8_t *data, uint32_t dataLen)
{
    wstp_print("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);

    unused(data);
    unused(dataLen);

    wstp_print("{%s():%d} func out success.\r\n", __FUNCTION__, __LINE__);
    return ERRCODE_SUCC;
}

static int32_t MediaVideoPlayGetVolume(const uint8_t *data, uint32_t dataLen)
{
    wstp_print("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);

    unused(data);
    unused(dataLen);

    wstp_print("{%s():%d} func out success.\r\n", __FUNCTION__, __LINE__);
    return ERRCODE_SUCC;
}

static int32_t MediaVideoPlayCurrentState(const uint8_t *data, uint32_t dataLen)
{
    wstp_print("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);

    unused(data);
    unused(dataLen);

    wstp_print("{%s():%d} func out success.\r\n", __FUNCTION__, __LINE__);
    return ERRCODE_SUCC;
}

static int32_t MediaVideoPlayMute(const uint8_t *data, uint32_t dataLen)
{
    wstp_print("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);

    unused(data);
    unused(dataLen);

    wstp_print("{%s():%d} func out success.\r\n", __FUNCTION__, __LINE__);
    return ERRCODE_SUCC;
}

static int32_t MediaCameraPreviewStart(const uint8_t *data, uint32_t dataLen)
{
    wstp_print("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);
    int32_t ret = ERRCODE_FAIL;
#ifdef ENABLE_UIKIT
    int32_t argc = 0;
    uint8_t *argv[MEDIA_MAX_PARAM_CNT];

    ret = ConvertCmdParamToSampleParam(data, dataLen, &argc, argv);
    if (ret != ERRCODE_SUCC) {
        goto FREE;
    }

    for (int32_t i = 0; i < argc; i++) {
        wstp_print("{%s():%d} argv[%d]: %s\r\n", __FUNCTION__, __LINE__, i, argv[i]);
    }

    ret = CameraPreviewSample(argc, (const char **)&argv);

FREE:
    for (int32_t i = 0; i < argc; i++) {
        if (argv[i] != NULL) {
            BSP_MEM_FREE(argv[i]);
            argv[i] = NULL;
        }
    }
#endif
    unused(data);
    unused(dataLen);
    wstp_print("{%s():%d} func out %s.\r\n", __FUNCTION__, __LINE__, (ret == ERRCODE_SUCC) ? "success" : "failure");
    return ret;
}

static int32_t MediaCameraPreviewStop(const uint8_t *data, uint32_t dataLen)
{
    wstp_print("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);
    int32_t ret = ERRCODE_SUCC;
    unused(data);
    unused(dataLen);
#ifdef ENABLE_UIKIT
    int32_t argc = 1;
    uint8_t *argv = (uint8_t *)"stop";

    ret = CameraPreviewSample(argc, (const char **)&argv);
#endif
    wstp_print("{%s():%d} func out %s.\r\n", __FUNCTION__, __LINE__, (ret == ERRCODE_SUCC) ? "success" : "failure");
    return ret;
}

static int32_t MediaVoiceCallVolteHandle(const uint8_t *data, uint32_t dataLen)
{
    wstp_print("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);

    int32_t ret = ERRCODE_FAIL;
    int32_t argc = 0;
    uint8_t *argv[MEDIA_MAX_PARAM_CNT];

    ret = ConvertCmdParamToSampleParam(data, dataLen, &argc, argv);
    if (ret != ERRCODE_SUCC) {
        goto FREE;
    }

    ret = VoiceCallVolteSample(argc, (const char **)&argv);

FREE:
    for (int32_t i = 0; i < argc; i++) {
        if (argv[i] != NULL) {
            BSP_MEM_FREE(argv[i]);
            argv[i] = NULL;
        }
    }

    wstp_print("{%s():%d} func out success.\r\n", __FUNCTION__, __LINE__);
    return ret;
}
#endif

static int32_t MediaA2dpSinkAef(const uint8_t *data, uint32_t dataLen)
{
    wstp_print("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);

    int32_t ret = ERRCODE_FAIL;
    int32_t argc = 0;
    uint8_t *argv[MEDIA_MAX_PARAM_CNT];

    ret = ConvertCmdParamToSampleParam(data, dataLen, &argc, argv);
    if (ret != ERRCODE_SUCC) {
        goto FREE;
    }
    if (argc >= 1) {
        uint32_t enable = atoi((const char *)argv[0x0]);
        ret = AudioManagerA2dpSinkSetAefEnable(0x190, enable);
    }
    if (argc == 0x2) {
        uint32_t beat = atoi((const char *)argv[0x1]);
        ret = AudioManagerA2dpSinkSetAefParam(0x190, (char *)&beat, sizeof(beat));
    }
    if (argc < 1 || argc > 0x2) {
        wstp_print("{%s():%d} the number of parameters is incorrect.\r\n", __FUNCTION__, __LINE__);
    }

FREE:
    for (int32_t i = 0; i < argc; i++) {
        if (argv[i] != NULL) {
            BSP_MEM_FREE(argv[i]);
            argv[i] = NULL;
        }
    }

    wstp_print("{%s():%d} func out success.\r\n", __FUNCTION__, __LINE__);
    return ret;
}

#ifndef MEMORY_MINI
static int32_t MediaAudioRecorder(const uint8_t *data, uint32_t dataLen)
{
    wstp_print("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);

    int32_t ret = ERRCODE_FAIL;
    int32_t argc = 0;
    uint8_t *argv[MEDIA_MAX_PARAM_CNT];

    ret = ConvertCmdParamToSampleParam(data, dataLen, &argc, argv);
    if (ret != ERRCODE_SUCC) {
        goto FREE;
    }
    ret = AudioRecorderSample(argc, (const char **)&argv);
FREE:
    for (int32_t i = 0; i < argc; i++) {
        if (argv[i] != NULL) {
            BSP_MEM_FREE(argv[i]);
            argv[i] = NULL;
        }
    }

    wstp_print("{%s():%d} func out success.\r\n", __FUNCTION__, __LINE__);
    return ret;
}
#endif

#ifndef MEMORY_MINI
static const MediaProcessType g_mediaAudioRecordFuncs[] = {
    { MEDIA_AUDIO_RECORD_START, MediaAudioRecordStart },
    { MEDIA_AUDIO_RECORD_STOP, MediaAudioRecordStop },
};

static const MediaProcessType g_mediaAudioSceneInteractionFuncs[] = {
    { MEDIA_AUDIO_SCENE_INTERACTION_001, MediaAudioSceneInteraction001 },
    { MEDIA_AUDIO_SCENE_INTERACTION_002, MediaAudioSceneInteraction002 },
    { MEDIA_AUDIO_SCENE_INTERACTION_003, MediaAudioSceneInteraction003 },
    { MEDIA_AUDIO_SCENE_INTERACTION_004, MediaAudioSceneInteraction004 },
    { MEDIA_AUDIO_SCENE_INTERACTION_005, MediaAudioSceneInteraction005 },
    { MEDIA_AUDIO_SCENE_INTERACTION_006, MediaAudioSceneInteraction006 },
    { MEDIA_AUDIO_SCENE_INTERACTION_007, MediaAudioSceneInteraction007 },
};

static const MediaProcessType g_mediaVideoPlayFuncs[] = {
    { MEDIA_VIDEO_PLAY_START, MediaVideoPlayStart },
    { MEDIA_VIDEO_PLAY_PAUSE, MediaVideoPlayPause },
    { MEDIA_VIDEO_PLAY_RESUME, MediaVideoPlayResume },
    { MEDIA_VIDEO_PLAY_STOP, MediaVideoPlayStop },
    { MEDIA_VIDEO_PLAY_SINGLE_LOOP, MediaVideoPlaySingleLoop },
    { MEDIA_VIDEO_PLAY_SET_VOLUME, MediaVideoPlaySetVolume },
    { MEDIA_VIDEO_PLAY_GET_VOLUME, MediaVideoPlayGetVolume },
    { MEDIA_VIDEO_PLAY_CURRENT_STATE, MediaVideoPlayCurrentState },
    { MEDIA_VIDEO_PLAY_MUTE, MediaVideoPlayMute },
};

static const MediaProcessType g_mediaCameraPreviewFuncs[] = {
    { MEDIA_CAMERA_PREVIEW_START, MediaCameraPreviewStart },
    { MEDIA_CAMERA_PREVIEW_STOP, MediaCameraPreviewStop },
};

static const MediaProcessType g_mediaVoiceCallVolteFuncs[] = {
    { MEDIA_VOICE_CALL_VOLTE_HANDLE, MediaVoiceCallVolteHandle },
};

static const MediaProcessType g_mediaAudioRecorderFuncs[] = {
    { MEDIA_AUDIO_RECORDER_CMD, MediaAudioRecorder },
};
#endif

static const MediaProcessType g_mediaA2dpSinkFuncs[] = {
    { MEDIA_A2DP_SINK_AEF, MediaA2dpSinkAef },
};

static int32_t MediaAudioPlayProcess(const uint8_t *data, uint32_t dataLen)
{
    wstp_print("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);

    int32_t ret = ERRCODE_FAIL;

    uint8_t audioCmd = data[MEDIA_SUB_CMD_POS];
    wstp_print("{%s():%d} audio cmd: %d.\r\n", __FUNCTION__, __LINE__, audioCmd);

    for (uint8_t i = 0; i < ARRAY_COUNT(g_mediaAudioPlayFuncs); i++) {
        if (audioCmd == g_mediaAudioPlayFuncs[i].id) {
            ret = g_mediaAudioPlayFuncs[i].func(data, dataLen);
        }
    }

    wstp_print("{%s():%d} func out %s.\r\n", __FUNCTION__, __LINE__, (ret == ERRCODE_SUCC) ? "success" : "failure");
    return ret;
}

#ifndef MEMORY_MINI
static int32_t MediaAudioRecordProcess(const uint8_t *data, uint32_t dataLen)
{
    int32_t ret = ERRCODE_FAIL;

    uint8_t audioCmd = data[MEDIA_SUB_CMD_POS];
    wstp_print("{%s():%d} audio cmd: %d.\r\n", __FUNCTION__, __LINE__, audioCmd);

    for (uint8_t i = 0; i < ARRAY_COUNT(g_mediaAudioRecordFuncs); i++) {
        if (audioCmd == g_mediaAudioRecordFuncs[i].id) {
            ret = g_mediaAudioRecordFuncs[i].func(data, dataLen);
        }
    }

    return ERRCODE_SUCC;
}

static int32_t MediaAudioSceneInteractionProcess(const uint8_t *data, uint32_t dataLen)
{
    int32_t ret = ERRCODE_FAIL;

    uint8_t audioCmd = data[MEDIA_SUB_CMD_POS];
    wstp_print("{%s():%d} audio cmd: %d.\r\n", __FUNCTION__, __LINE__, audioCmd);

    for (uint8_t i = 0; i < ARRAY_COUNT(g_mediaAudioSceneInteractionFuncs); i++) {
        if (audioCmd == g_mediaAudioSceneInteractionFuncs[i].id) {
            ret = g_mediaAudioSceneInteractionFuncs[i].func(data, dataLen);
        }
    }

     return ERRCODE_SUCC;
}

static int32_t MediaVideoPlayProcess(const uint8_t *data, uint32_t dataLen)
{
    wstp_print("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);

    int32_t ret = ERRCODE_FAIL;

    uint8_t videoCmd = data[MEDIA_SUB_CMD_POS];
    wstp_print("{%s():%d} video cmd: %d.\r\n", __FUNCTION__, __LINE__, videoCmd);

    for (uint8_t i = 0; i < ARRAY_COUNT(g_mediaVideoPlayFuncs); i++) {
        if (videoCmd == g_mediaVideoPlayFuncs[i].id) {
            ret = g_mediaVideoPlayFuncs[i].func(data, dataLen);
        }
    }

    wstp_print("{%s():%d} func out %s.\r\n", __FUNCTION__, __LINE__, (ret == ERRCODE_SUCC) ? "success" : "failure");
    return ret;
}

static int32_t MediaCameraPreviewProcess(const uint8_t *data, uint32_t dataLen)
{
    wstp_print("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);

    int32_t ret = ERRCODE_FAIL;

    uint8_t videoCmd = data[MEDIA_SUB_CMD_POS];
    wstp_print("{%s():%d} video cmd: %d.\r\n", __FUNCTION__, __LINE__, videoCmd);

    for (uint8_t i = 0; i < ARRAY_COUNT(g_mediaCameraPreviewFuncs); i++) {
        if (videoCmd == g_mediaCameraPreviewFuncs[i].id) {
            ret = g_mediaCameraPreviewFuncs[i].func(data, dataLen);
        }
    }

    wstp_print("{%s():%d} func out %s.\r\n", __FUNCTION__, __LINE__, (ret == ERRCODE_SUCC) ? "success" : "failure");
    return ret;
}

static int32_t MediaVoiceCallVolteProcess(const uint8_t *data, uint32_t dataLen)
{
    wstp_print("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);

    int32_t ret = ERRCODE_FAIL;

    uint8_t videoCmd = data[MEDIA_SUB_CMD_POS];
    wstp_print("{%s():%d} voice call volte cmd: %d.\r\n", __FUNCTION__, __LINE__, videoCmd);

    for (uint8_t i = 0; i < ARRAY_COUNT(g_mediaVoiceCallVolteFuncs); i++) {
        if (videoCmd == g_mediaVoiceCallVolteFuncs[i].id) {
            ret = g_mediaVoiceCallVolteFuncs[i].func(data, dataLen);
        }
    }

    wstp_print("{%s():%d} func out %s.\r\n", __FUNCTION__, __LINE__, (ret == ERRCODE_SUCC) ? "success" : "failure");
    return ret;
}

static int32_t MediaAudioRecorderProcess(const uint8_t *data, uint32_t dataLen)
{
    wstp_print("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);

    int32_t ret = ERRCODE_FAIL;

    uint8_t videoCmd = data[MEDIA_SUB_CMD_POS];
    wstp_print("{%s():%d} audio recorder cmd: %d.\r\n", __FUNCTION__, __LINE__, videoCmd);

    for (uint8_t i = 0; i < ARRAY_COUNT(g_mediaAudioRecorderFuncs); i++) {
        if (videoCmd == g_mediaAudioRecorderFuncs[i].id) {
            ret = g_mediaAudioRecorderFuncs[i].func(data, dataLen);
        }
    }

    wstp_print("{%s():%d} func out %s.\r\n", __FUNCTION__, __LINE__, (ret == ERRCODE_SUCC) ? "success" : "failure");
    return ret;
}
#endif

static int32_t MediaA2dpSinkProcess(const uint8_t *data, uint32_t dataLen)
{
    wstp_print("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);

    int32_t ret = ERRCODE_FAIL;

    uint8_t videoCmd = data[MEDIA_SUB_CMD_POS];
    wstp_print("{%s():%d} a2dp sink cmd: %d.\r\n", __FUNCTION__, __LINE__, videoCmd);

    for (uint8_t i = 0; i < ARRAY_COUNT(g_mediaA2dpSinkFuncs); i++) {
        if (videoCmd == g_mediaA2dpSinkFuncs[i].id) {
            ret = g_mediaA2dpSinkFuncs[i].func(data, dataLen);
        }
    }

    wstp_print("{%s():%d} func out %s.\r\n", __FUNCTION__, __LINE__, (ret == ERRCODE_SUCC) ? "success" : "failure");
    return ret;
}

static const MediaProcessType g_mediaAudioFuncs[] = {
    { MEDIA_AUDIO_PLAY, MediaAudioPlayProcess },
#ifndef MEMORY_MINI
    { MEDIA_AUDIO_RECORD, MediaAudioRecordProcess },
    { MEDIA_AUDIO_SCENE_INTERACTION, MediaAudioSceneInteractionProcess },
    { MEDIA_VIDEO_PLAY, MediaVideoPlayProcess },
    { MEDIA_CAMERA_PREVIEW, MediaCameraPreviewProcess },
    { MEDIA_VOICE_CALL_VOLTE, MediaVoiceCallVolteProcess },
    { MEDIA_AUDIO_RECORDER, MediaAudioRecorderProcess },
#endif
    { MEDIA_A2DP_SINK, MediaA2dpSinkProcess },
};

void media_at_process(const uint8_t *data, uint32_t dataLen)
{
    wstp_print("{%s():%d} func in.\r\n", __FUNCTION__, __LINE__);
    if (data == NULL) {
        wstp_print("{%s():%d} input data is NULL.\r\n", __FUNCTION__, __LINE__);
        return;
    }

    uint8_t audio_scene;
    int32_t ret = ERRCODE_FAIL;

    // data = { "00000100xx,mp3_48k_caiqing.mp3,1,0" };
    // 第一个字节表示场景: 00(AudioPlay), 01(AudioRecord), 02(AudioSceneInteraction), 03(VideoPlay)
    // 第二个字节表示控制命令或用例编号,和场景类型组合使用

    audio_scene = data[MEDIA_SCENE_POS];
    for (uint32_t i = 0; i < ARRAY_COUNT(g_mediaAudioFuncs); i++) {
        if (audio_scene == g_mediaAudioFuncs[i].id) {
            ret = g_mediaAudioFuncs[i].func(data, dataLen);
            print_at_cmd_ret(ret);
        }
    }

    // phone remote control, need reply to spp
    if (g_media_diag_flag) {
        uint8_t reply_data[MEDIA_APP_REPLY_DATA_LEN] = {0};
        reply_data[0x0] = 0x8C;
        reply_data[0x1] = 0x82;
        reply_data[0x2] = 0;
        reply_data[0x3] = (uint8_t)ret;
        ret = send_spp_diag_msg(g_media_diag_flag, &g_media_diag_header, reply_data, MEDIA_APP_REPLY_DATA_LEN);
    }

    g_media_diag_flag = false;
    wstp_print("{%s():%d} func out %s.\r\n", __FUNCTION__, __LINE__, (ret == ERRCODE_SUCC) ? "success" : "failure");
}

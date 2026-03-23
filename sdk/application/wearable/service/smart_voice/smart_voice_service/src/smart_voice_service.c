/*
 * Copyright (c) CompanyNameMagicTag 2021. All rights reserved.
 * Description: smart voice service.
 */
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include "securec.h"
#include "cmsis_os.h"
#include "soc_errno.h"
#include "audio_stream.h"
#include "audio_base_type.h"
#include "voice_engine_hal.h"
#include "wearable_log.h"
#include "smart_voice_service.h"
#include "pm_clocks.h"

#define SLEEP_TIME 10000

typedef enum {
    STATE_UNKNOWN,
    STATE_VAD,
    STATE_ASR
} work_state;

typedef struct {
    SmartVoiceCmdId id;
    td_uchar cmd[32];
    td_u32 match_cnt;
} voice_command;

static LocalAsrInitParams g_init_params;
static AsrWorkMode g_work_mode = ALWAYS_ON;
static td_s32 is_asr_running = TD_FALSE;
static td_s32 is_state_machine_running = TD_FALSE;
static LocalAsrCb g_asr_cb = TD_NULL;
static osMutexId_t g_mutex;
static osThreadId_t g_thread_id;
static td_s32 is_screen_on = TD_FALSE;
static work_state g_state = STATE_UNKNOWN;
static work_state g_new_state = STATE_UNKNOWN;
static osEventFlagsId_t g_evt_id;
static td_s32 g_init_flag;
static voice_command g_vcmd[] = {
    {SMART_VOICE_ACCEPT_CALL_CMD, "(AnswerCall)接听电话", 0},
    {SMART_VOICE_REJECT_CALL_CMD, "(RejectCall)拒接电话", 0},
    {SMART_VOICE_PLAY_LAST_CMD, "(Previous)上一首", 0},
    {SMART_VOICE_PLAY_NEXT_CMD, "(Next)下一首", 0},
    {SMART_VOICE_VOLUME_UP_CMD, "(VolumeUp)调大音量", 0},
    {SMART_VOICE_VOLUME_DOWN_CMD, "(VolumeDown)调小音量", 0},
    {SMART_VOICE_START_MUSIC_CMD, "(PlayMusic)开始播放", 0},
    {SMART_VOICE_STOP_MUSIC_CMD, "(StopMusic)停止播放", 0},
};

static td_s32 vad_valid_cnt;
static td_s32 vad_timeout_cnt;
static td_s32 vcmd_match_cnt;

static td_void dump_config(td_void)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE, "smart voice service config info:");
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE, "power mode: %d", g_init_params.powerMode);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE, "work mode: %d", g_work_mode);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE, "work state: %d", g_state);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE, "input mode: %d", g_init_params.inputMode);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE, "enable aec: %d", g_init_params.enableAEC);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE, "enable npu: %d", g_init_params.enableNpu);
}

static td_void dump_statis(td_void)
{
    td_u32 num = sizeof(g_vcmd) / sizeof(voice_command);
    td_u32 i;
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE, "smart voice service statis:");
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE, "vad_valid_cnt: %d", vad_valid_cnt);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE, "vad_timeout_cnt: %d", vad_timeout_cnt);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE, "vcmd_match_cnt: %d", vcmd_match_cnt);
    for (i = 0; i < num; i++) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE, "%s : %d", g_vcmd[i].cmd, g_vcmd[i].match_cnt);
    }
}

static td_void dump(td_void)
{
    dump_config();
    dump_statis();
}

static td_void reset_dump_statis(td_void)
{
    td_u32 num = sizeof(g_vcmd) / sizeof(voice_command);
    td_u32 i;
    for (i = 0; i < num; i++) {
        g_vcmd[i].match_cnt = 0;

    }
    vad_valid_cnt = 0;
    vad_timeout_cnt = 0;
    vcmd_match_cnt = 0;
}

static SmartVoiceCmdId find_vcmd_id(td_uchar *cmd, td_u32 len)
{
    (void)len;
    td_u32 num = sizeof(g_vcmd) / sizeof(voice_command);
    td_u32 i;

    for (i = 0; i < num; i++) {
        td_char *vcmd = (td_char *)g_vcmd[i].cmd;
        if (!strncmp(vcmd, (td_char *)cmd, strlen(vcmd))) {
            g_vcmd[i].match_cnt++;
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE, "match key: %s", vcmd);
            return g_vcmd[i].id;
        }
    }
    return SMART_VOICE_INVALID_CMD_ID;
}

static td_void update_state_machine(SmartVoiceEventType event)
{
    if (g_init_params.powerMode == ALWAYS_WAKE) {
        return;
    }
    if (event == SMART_VOICE_EVENT_VOICE_BEGIN) {
        g_new_state = STATE_ASR;
        osEventFlagsSet(g_evt_id, 1);
    }
    if (event == SMART_VOICE_EVENT_VOICE_END) {
        if (g_init_params.powerMode == SLEEP_ON_VAD_TIMEOUT) {
            g_new_state = STATE_VAD;
            osEventFlagsSet(g_evt_id, 1);
        }
    }
}

static td_void v_callback(voice_engine_event_type type, td_void *data, td_u32 size)
{
    (void)size;
    SmartVoiceEventType eventType;
    voice_engine_event_param *vehal_param = (voice_engine_event_param *)data;
    SmartVoiceCmdParam sv_param;
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE, "v_callback event = %d", type);
    osMutexAcquire(g_mutex, osWaitForever);
    switch (type) {
        case VOICE_ENGINE_EVENT_VAD_VALID:
            eventType = SMART_VOICE_EVENT_VOICE_BEGIN;
            vad_valid_cnt++;
            if (g_asr_cb) {
                g_asr_cb(SMART_VOICE_EVENT_VOICE_BEGIN, TD_NULL, 0);
            }
            break;
        case VOICE_ENGINE_EVENT_VAD_TIMEOUT:
            eventType = SMART_VOICE_EVENT_VOICE_END;
            vad_timeout_cnt++;
            if (g_asr_cb) {
                g_asr_cb(SMART_VOICE_EVENT_VOICE_END, TD_NULL, 0);
            }
            break;
        case VOICE_ENGINE_EVENT_VOICE_COMMAND:
            eventType = SMART_VOICE_EVENT_VOICE_COMMAND;
            vcmd_match_cnt++;
            if (vehal_param && g_asr_cb) {
                sv_param.cmdId = find_vcmd_id(vehal_param->cmd, vehal_param->cmd_len);
                if (sv_param.cmdId == SMART_VOICE_INVALID_CMD_ID) {
                    WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE, "can not find cmd id");
                    break;
                }
                sv_param.userId = vehal_param->user_id;
                sv_param.cmdArgs = vehal_param->cmd_args;
                sv_param.cmdArgsLen = vehal_param->cmd_args_len;
                WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE, "voice cmd id: 0x%x", sv_param.cmdId);
                WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE, "voice user id: 0x%x", sv_param.userId);
                if (sv_param.cmdArgs) {
                    WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE, "voice cmd args: %s", sv_param.cmdArgs);
                }
                g_asr_cb(SMART_VOICE_EVENT_VOICE_COMMAND, &sv_param, sizeof(sv_param));
            }
            break;
        default:
            eventType = SMART_VOICE_EVENT_MAX;
            break;
    }
    update_state_machine(eventType);
    osMutexRelease(g_mutex);
}

static td_void audio_interrupt_callback(td_s32 type, td_s32 hint)
{
    if (type == INTERRUPT_TYPE_BEGIN && hint == INTERRUPT_HINT_PAUSE) {
        LocalAsrStop();
    }

    if (type == INTERRUPT_TYPE_END && hint == INTERRUPT_HINT_RESUME) {
        LocalAsrStart();
    }
}

static td_s32 release_audio_stream()
{
    AudioStreamDeactInterrupt();
    AudioStreamDestroy();
    return EXT_SUCCESS;
}

static td_s32 get_audio_stream_channel_id(td_u32 *chan_id)
{
    td_s32 ret;
    td_u32 channel_id;
    ret = AudioManagerActInterrupt(audio_interrupt_callback);
    if (ret != EXT_SUCCESS) {
        return ret;
    }
    ret = AudioStreamCreate();
    if (ret != EXT_SUCCESS) {
        AudioStreamDeactInterrupt();
        return ret;
    }
    ret = AudioStreamGetChannelId(&channel_id);
    if (ret != EXT_SUCCESS) {
        release_audio_stream();
    }
    *chan_id = channel_id;
    return ret;
}

static td_s32 create_asr_inner(td_void)
{
    td_s32 ret;
    td_u32 channel_id;
    voice_engine_params eng_params;
    eng_params.type = ASR_ENGINE;
    eng_params.u.asr_attr.enable_vid = TD_FALSE;
    eng_params.u.asr_attr.enable_nlp = TD_FALSE;
    eng_params.u.asr_attr.enable_aec = g_init_params.enableAEC;
    eng_params.u.asr_attr.enable_npu = g_init_params.enableNpu;

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_SMART_VOICE, "create_asr_inner");

    if (g_init_params.inputMode == PCM_INJECTION) {
        ret = vehal_set_input(DATA_INJECTION, TD_NULL, 0);
    } else if (g_init_params.inputMode == INNER_AUDIO_PORT) {
        ret = vehal_set_input(INNER_PORT, TD_NULL, 0);
    } else {
        ret = get_audio_stream_channel_id(&channel_id);
        if (ret != EXT_SUCCESS) {
            return ret;
        }
        ret = vehal_set_input(INNER_PORT, &channel_id, sizeof(channel_id));
    }
    if (ret != EXT_SUCCESS) {
        if (g_init_params.inputMode == AUDIO_STREAM) {
            release_audio_stream();
        }
        return ret;
    }
    ret = vehal_create_engine(&eng_params);
    if (ret != EXT_SUCCESS) {
        if (g_init_params.inputMode == AUDIO_STREAM) {
            release_audio_stream();
        }
        return ret;
    }
    return ret;
}

static td_s32 start_asr_inner(td_void)
{
    td_s32 ret;

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_SMART_VOICE, "start_asr_inner");
    ret = vehal_start_engine();
    if (ret != EXT_SUCCESS) {
        if (g_init_params.inputMode == AUDIO_STREAM) {
            release_audio_stream();
        }
        return ret;
    }
    if (g_init_params.inputMode == AUDIO_STREAM) {
        ret = AudioStreamStart();
        if (ret != EXT_SUCCESS) {
            vehal_stop_engine();
            release_audio_stream();
        }
    }
    return ret;
}

static td_s32 stop_asr_inner(td_void)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_SMART_VOICE, "stop_asr_inner");
    if (g_init_params.inputMode == AUDIO_STREAM) {
        AudioStreamStop();
    }
    vehal_stop_engine();
    return EXT_SUCCESS;
}

static td_s32 destroy_asr_inner(td_void)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_SMART_VOICE, "destroy_asr_inner");
    vehal_destroy_engine();
    if (g_init_params.inputMode == AUDIO_STREAM) {
        release_audio_stream();
    }
    return EXT_SUCCESS;
}

static td_s32 state_vad_to_asr(td_void)
{
    td_s32 ret;

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_SMART_VOICE, "state_vad_to_asr");
    ret = vehal_stop_vad();
    if (ret != EXT_SUCCESS) {
        return ret;
    }
    start_asr_inner();
    g_state = STATE_ASR;
    return ret;
}

static td_s32 state_asr_to_vad(td_void)
{
    td_s32 ret;

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_SMART_VOICE, "state_asr_to_vad");
    ret = stop_asr_inner();
    if (ret != EXT_SUCCESS) {
        return ret;
    }
    ret = vehal_start_vad();
    if (ret != EXT_SUCCESS) {
        return ret;
    }
    g_state = STATE_VAD;
    return EXT_SUCCESS;
}

static td_void state_machine_task(td_void *data)
{
    (void)data;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_SMART_VOICE, "state_machine_task");
    while (is_state_machine_running) {
        if (osEventFlagsWait(g_evt_id, 1, osFlagsWaitAny, SLEEP_TIME) > 0) {
            if (g_new_state == STATE_ASR && g_state == STATE_VAD) {
                WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE, "voice detected, start asr");
                osMutexAcquire(g_mutex, osWaitForever);
                state_vad_to_asr();
                osMutexRelease(g_mutex);
            } else if (g_new_state == STATE_VAD && g_state == STATE_ASR) {
                osMutexAcquire(g_mutex, osWaitForever);
                state_asr_to_vad();
                osMutexRelease(g_mutex);
            }
        }
    }
    osEventFlagsSet(g_evt_id, 1);
    return;
}

static td_s32 start_state_machine(td_void)
{
    td_s32 ret;
    g_state = STATE_VAD;
    g_new_state = g_state;
    is_state_machine_running = TD_TRUE;

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_SMART_VOICE, "start_state_machine");
    g_thread_id = osThreadNew(state_machine_task, TD_NULL, TD_NULL);
    if (g_thread_id == TD_NULL) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "can not create VE State Machine task");
        return EXT_FAILURE;
    }
    ret = vehal_start_vad();
    if (ret != EXT_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "start VAD failed");
        return EXT_FAILURE;
    }
    return EXT_SUCCESS;
}

static td_s32 stop_state_machine(td_void)
{
    td_s32 ret;
    if (!is_state_machine_running) {
        return EXT_SUCCESS;
    }

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_SMART_VOICE, "stop_state_machine");
    is_state_machine_running = TD_FALSE;
    osEventFlagsSet(g_evt_id, 1);
    usleep(SLEEP_TIME);
    ret = osEventFlagsWait(g_evt_id, 1, osFlagsWaitAny, SLEEP_TIME);
    if (ret < 0) {
        ret = EXT_FAILURE;
    } else {
        ret = EXT_SUCCESS;
    }
    stop_asr_inner();
    vehal_stop_vad();
    return ret;
}

td_s32 LocalAsrInit(LocalAsrInitParams *initParams)
{
    td_s32 ret = EXT_SUCCESS;

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_SMART_VOICE, "LocalAsrInit");
    osMutexAttr_t thread_mutex_attr = { "smart voice mutex", osMutexRecursive | osMutexPrioInherit, TD_NULL, 0U };
    g_init_params.powerMode = initParams->powerMode;
    g_init_params.inputMode = initParams->inputMode;
    g_init_params.enableAEC = initParams->enableAEC;
    g_init_params.enableNpu = initParams->enableNpu;
    g_mutex = osMutexNew(&thread_mutex_attr);
    if (!g_mutex) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "osMutexNew failed");
    }
    vehal_init();
    g_evt_id = osEventFlagsNew(TD_NULL);
    if (g_evt_id == TD_NULL) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "osEventFlagsNew failed");
        return EXT_FAILURE;
    }

    if (g_init_params.inputMode == AUDIO_STREAM) {
        ret = AudioStreamInit();
        if (ret != EXT_SUCCESS) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "AudioStreamInit failed");
            return ret;
        }
    }

    g_init_flag = TD_TRUE;
    return ret;
}

td_s32 LocalAsrDeInit(td_void)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_SMART_VOICE, "LocalAsrDeInit");
    if (g_init_flag) {
        vehal_deinit();
        osEventFlagsDelete(g_evt_id);
        osMutexDelete(g_mutex);
        g_init_flag = TD_FALSE;
    }
    return EXT_SUCCESS;
}

td_s32 LocalAsrSetMode(AsrWorkMode workMode)
{
    td_s32 ret;
    if (!g_init_flag) {
        return EXT_FAILURE;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_SMART_VOICE, "LocalAsrSetMode %d", workMode);
    osMutexAcquire(g_mutex, osWaitForever);
    g_work_mode = workMode;
    osMutexRelease(g_mutex);
    if (g_work_mode == ALWAYS_ON) {
        ret = LocalAsrStart();
    } else if (g_work_mode == ALWAYS_OFF) {
        ret = LocalAsrStop();
    } else {
        if (is_screen_on) {
            ret = LocalAsrStart();
        } else {
            ret = LocalAsrStop();
        }
    }

    return ret;
}

AsrWorkMode LocalAsrGetMode(td_void)
{
    return g_work_mode;
}

td_s32 LocalAsrStart(td_void)
{
    td_s32 ret;
    osMutexAcquire(g_mutex, osWaitForever);
    if (!g_init_flag) {
        osMutexRelease(g_mutex);
        return EXT_FAILURE;
    }
    if (is_asr_running) {
        osMutexRelease(g_mutex);
        return EXT_SUCCESS;
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE, "LocalAsrStart");
    LocalAsrSetHifiMode();
    reset_dump_statis();
    vehal_set_callback(v_callback);
    ret = create_asr_inner();
    if (ret != EXT_SUCCESS) {
        osMutexRelease(g_mutex);
        return ret;
    }
    if (g_init_params.powerMode != ALWAYS_WAKE && g_init_params.inputMode != PCM_INJECTION) {
        ret = start_state_machine();
    } else {
        ret = start_asr_inner();
    }
    if (ret != EXT_SUCCESS) {
        osMutexRelease(g_mutex);
        return ret;
    }
    is_asr_running = TD_TRUE;
    osMutexRelease(g_mutex);
    return ret;
}

td_s32 LocalAsrStop(td_void)
{
    osMutexAcquire(g_mutex, osWaitForever);
    if (!g_init_flag) {
        osMutexRelease(g_mutex);
        return EXT_FAILURE;
    }

    if (!is_asr_running) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE, "local kws already stop");
        osMutexRelease(g_mutex);
        return EXT_SUCCESS;
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE, "LocalAsrStop");
    LocalAsrSetHifiMode();
    if (g_init_params.powerMode != ALWAYS_WAKE) {
        stop_state_machine();
    } else {
        stop_asr_inner();
    }
    vehal_set_callback(TD_NULL);
    destroy_asr_inner();
    is_asr_running = TD_FALSE;
    g_state = STATE_UNKNOWN;
    g_new_state = STATE_UNKNOWN;
    osMutexRelease(g_mutex);
    return EXT_SUCCESS;
}

td_u32 LocalAsrReadData(td_uchar *voiceData, td_u32 length)
{
    td_u32 ret;

    if (voiceData == TD_NULL || length == 0) {
        return 0;
    }

    if (!is_asr_running || !g_init_flag) {
        return 0;
    }
    ret = vehal_capture(voiceData, length);
    return ret;
}

td_u32 LocalAsrReadRawData(td_uchar *voiceData, td_u32 length)
{
    td_u32 ret;

    if (voiceData == TD_NULL || length == 0) {
        return 0;
    }

    if (!is_asr_running || !g_init_flag) {
        return 0;
    }

    ret = vehal_raw_capture(voiceData, length);
    return ret;
}

td_u32 LocalAsrWriteData(td_uchar *voiceData, td_u32 length)
{
    td_u32 ret;

    if (voiceData == TD_NULL || length == 0) {
        return 0;
    }

    if (g_init_params.inputMode != PCM_INJECTION || !is_asr_running || !g_init_flag) {
        return 0;
    }
    ret = vehal_inject(voiceData, length);
    return ret;
}

td_s32 LocalAsrSetCallback(LocalAsrCb callback)
{
    if (!g_init_flag) {
        return EXT_FAILURE;
    }
    osMutexAcquire(g_mutex, osWaitForever);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE, "LocalAsrSetCallback");
    g_asr_cb = callback;
    osMutexRelease(g_mutex);
    return EXT_SUCCESS;
}

td_float LocalAsrCalcRms(td_uchar *voiceData, td_u32 length)
{
    td_float rms = 0.0;
    td_s16 *pcm = (td_s16 *)voiceData;
    td_u32 i, count;

    if (voiceData == TD_NULL || length == 0) {
        return rms;
    }

    count = length / 2; /* 2: sample depth is 16bit */
    for (i = 0; i < count; i++) {
        rms = pcm[i] * pcm[i];
    }
    rms = rms / count;
    rms = sqrt(rms);

    return rms;
}

td_void LocalAsrDump(td_void)
{
    dump();
    if (is_asr_running) {
        vehal_dump();
    }
}

td_void LocalAsrSetHifiMode(td_void)
{
    uapi_set_hifi_mode(HIFI_LPM_CLK_HIGH_LEVEL);
}
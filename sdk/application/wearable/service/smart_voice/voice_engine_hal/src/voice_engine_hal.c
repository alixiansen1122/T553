/*
 * Copyright (c) CompanyNameMagicTag 2021. All rights reserved.
 * Description: voice engine hal.
 */
#include "voice_engine_hal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#include "securec.h"
#include "soc_osal.h"
#include "soc_errno.h"
#include "soc_uapi_adp.h"
#include "soc_uapi_ai.h"
#include "soc_uapi_vad.h"
#include "soc_uapi_sea.h"

#include "wearable_log.h"

#define SEA_PHRASE_COUNT 21
#define WAIT_TIMEOUT_IN_MS 1000
#define WAIT_TASK_EXIT 10000
#define AUDIO_INPUT_ADC
// define CAPTURE_AI_INPUT to enable capture ai input

typedef enum {
    ONLY_AVAD,
    ONLY_MAD,
    AVAD_MAD,
} vad_work_mode;

typedef enum {
    STATE_IDLE,
    STATE_AVAD_DETECTING,
    STATE_MAD_DETECTING,
    STATE_VAD_COMPLETED
} vad_work_state;

static td_handle g_adp_output;
static td_handle g_adp_input;
static td_handle g_adp_raw;
static td_handle g_avad;
static td_handle g_ai_mad;
static td_handle g_ai_kws;
static td_handle g_vad_mode = AVAD_MAD;
static vad_work_state g_vad_state = STATE_IDLE;
static vad_work_state g_vad_new_state = STATE_IDLE;
static osal_wait g_vad_wait;
static td_handle g_sea;
static td_s32 g_lib_id = UAPI_SEA_LIB_MAX;
static voice_engine_input_mode g_input_mode = INNER_PORT;
static voice_engine_cb g_cb;
static osal_mutex g_mutex = { 0 };
static voice_engine_params g_eng_params;
static uapi_sea_item g_phrase[SEA_PHRASE_COUNT] = { 0 };
static td_u32 g_phrase_num;

static td_u32 update_phrase_set(td_void)
{
    td_s32 ret;
    uapi_sea_item_sets sets;
    if (!g_sea) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE, "update_phrase_set failed, g_sea == TD_NULL");
        return 0;
    }
    sets.size = SEA_PHRASE_COUNT;
    sets.array = g_phrase;
    ret = uapi_sea_get_phrase_sets(g_sea, &sets);
    if (ret != EXT_SUCCESS) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE, "uapi_sea_get_phrase_sets ret = 0x%x", ret);
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE, "update_phrase_set: get %d phrases", sets.count);
    return sets.count;
}

static td_s32 find_phrase_by_id(td_u32 id)
{
    td_u32 i;
    for (i = 0; i < g_phrase_num; i++) {
        if (id == (td_u32)g_phrase[i].id) {
            return i;
        }
    }
    return -1;
}

static td_s32 process_kws_match(uapi_sea_kws_event_param *kws_param)
{
    td_s32 index;
    voice_engine_event_param param = { 0 };

    if (kws_param == TD_NULL) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE, "asr match event with param");
        return EXT_FAILURE;
    }
    index = find_phrase_by_id(kws_param->phrase_id);
    if (index < 0 || index >= (td_s32)g_phrase_num) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE, "kws: can not find phrase id %d", kws_param->phrase_id);
        return EXT_FAILURE;
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE, "kws: find phrase %s %d %d", g_phrase[index].name, kws_param->phrase_id, index);
    param.cmd = g_phrase[index].name;
    param.cmd_len = strlen((td_char *)param.cmd) + 1;
    param.cmd_args = (td_uchar *)TD_NULL;
    param.cmd_args_len = 0;
    if (g_cb) {
        g_cb(VOICE_ENGINE_EVENT_VOICE_COMMAND, &param, sizeof(param));
    }
    return EXT_SUCCESS;
}

static td_s32 process_asr_match(uapi_sea_asr_event_param *asr_param)
{
    td_s32 index;
    voice_engine_event_param param;

    if (asr_param == TD_NULL) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE, "asr match event without param");
        return EXT_FAILURE;
    }
    index = find_phrase_by_id(asr_param->skill_id);
    if (index < 0 || index >= (td_s32)g_phrase_num) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE, "asr: can not find phrase id %d", asr_param->skill_id);
        return EXT_FAILURE;
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE, "asr: find phrase %s", g_phrase[index].name);
    param.cmd = g_phrase[index].name;
    param.cmd_len = strlen((td_char *)param.cmd) + 1;
    param.cmd_args = asr_param->data_buf;
    param.cmd_args_len = asr_param->data_size;
    if (g_cb) {
        g_cb(VOICE_ENGINE_EVENT_VOICE_COMMAND, &param, sizeof(param));
    }
    return EXT_SUCCESS;
}

static td_s32 sea_event_proc(td_handle sea, uapi_sea_event_type event, td_void *data, td_void *context)
{
    (void)sea;
    (void)context;
    td_s32 ret = EXT_SUCCESS;

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_SMART_VOICE, "sea_event_proc event = %d", event);
    osal_mutex_lock(&g_mutex);
    switch (event) {
        case UAPI_SEA_EVENT_VAD_END:
            if (g_cb) {
                g_cb(VOICE_ENGINE_EVENT_VAD_TIMEOUT, TD_NULL, 0);
            }
            break;
        case UAPI_SEA_EVENT_KWS_MATCH:
            ret = process_kws_match((uapi_sea_kws_event_param *)data);
            break;
        case UAPI_SEA_EVENT_ASR_MATCH:
            ret = process_asr_match((uapi_sea_asr_event_param *)data);
            break;
        default:
            break;
    }
    osal_mutex_unlock(&g_mutex);
    return ret;
}

static td_s32 vad_event_proc(td_handle ai, uapi_vad_event_type event, const td_void *param, td_void *context)
{
    (void)ai;
    (void)param;
    (void)context;
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE, "vad event type: %d", event);
    osal_mutex_lock(&g_mutex);

    switch (event) {
        case UAPI_VAD_EVENT_VAD_VALID:
            if (g_vad_mode == ONLY_AVAD) {
                if (g_cb) {
                    g_cb(VOICE_ENGINE_EVENT_VAD_VALID, TD_NULL, 0);
                }
                g_vad_state = STATE_VAD_COMPLETED;
                g_vad_new_state = STATE_VAD_COMPLETED;
            } else if (g_vad_mode == AVAD_MAD) {
                g_vad_new_state = STATE_MAD_DETECTING;
                osal_wait_wakeup(&g_vad_wait);
            }
            break;
        case UAPI_VAD_EVENT_VAD_TIMEOUT:
            uapi_vad_reset(g_avad);
            break;
        default:
            break;
    }
    osal_mutex_unlock(&g_mutex);
    return EXT_SUCCESS;
}

static td_s32 ai_event_proc(td_handle ai, uapi_ai_event_type event, const td_void *param, td_void *context)
{
    (void)ai;
    (void)param;
    (void)context;
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE, "ai event type: %d", event);
    osal_mutex_lock(&g_mutex);

    switch (event) {
        case UAPI_AI_EVENT_VAD_VALID:
            if (g_vad_mode == ONLY_MAD ||
                (g_vad_mode == AVAD_MAD && g_vad_state == STATE_MAD_DETECTING)) {
                if (g_cb) {
                    g_cb(VOICE_ENGINE_EVENT_VAD_VALID, TD_NULL, 0);
                }
            }
            break;
        case UAPI_AI_EVENT_VAD_TIMEOUT:
            if (g_vad_mode == AVAD_MAD && g_vad_state == STATE_MAD_DETECTING) {
                g_vad_new_state = STATE_AVAD_DETECTING;
                osal_wait_wakeup(&g_vad_wait);
            } else if (g_vad_mode == ONLY_MAD) {
                uapi_ai_reset_vad(g_ai_mad);
            }
            break;
        default:
            break;
    }
    osal_mutex_unlock(&g_mutex);
    return EXT_SUCCESS;
}

static td_s32 sap_load_aai_engine(td_s32 lib_id)
{
    td_s32 ret;

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_SMART_VOICE, "sap_load_aai_engine: lib_id = %d", lib_id);

    if (g_lib_id == UAPI_SEA_LIB_MAX) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_SMART_VOICE, "sap_load_aai_engine: %d %d", g_lib_id, lib_id);
        ret = uapi_sea_load_engine(lib_id, "imedia_keyword");
        if (ret != EXT_SUCCESS) {
            return EXT_FAILURE;
        }
        // g_lib_id = lib_id;
        return EXIT_SUCCESS;
    }
    if (g_lib_id != lib_id) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_SMART_VOICE, "sap_load_aai_engine: %d %d", g_lib_id, lib_id);
        ret = uapi_sea_unload_engine(g_lib_id, "imedia_keyword");
        if (ret != EXT_SUCCESS) {
            return EXT_FAILURE;
        }
        ret = uapi_sea_load_engine(lib_id, "imedia_keyword");
        if (ret != EXT_SUCCESS) {
            return EXT_FAILURE;
        }
        // g_lib_id = lib_id;
        return EXIT_SUCCESS;
    }
    return EXIT_SUCCESS;
}

static td_s32 sap_init_engine(td_void)
{
    td_s32 ret;

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_SMART_VOICE, "sap_init_engine");
    ret = uapi_adp_init();
    if (ret != EXT_SUCCESS) {
        return EXT_FAILURE;
    }

    ret = uapi_ai_init();
    if (ret != EXT_SUCCESS) {
        return EXT_FAILURE;
    }

    ret = uapi_sea_init();
    if (ret != EXT_SUCCESS) {
        return EXT_FAILURE;
    }

    ret = osal_wait_init(&g_vad_wait);
    if (ret != EXT_SUCCESS) {
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

static td_s32 sap_deinit_engine(td_void)
{
    td_s32 ret;

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_SMART_VOICE, "sap_deinit_engine");
    if (g_lib_id != UAPI_SEA_LIB_MAX) {
        ret = uapi_sea_unload_engine(g_lib_id, "imedia_keyword");
        if (ret != EXT_SUCCESS) {
            return EXT_FAILURE;
        }
        g_lib_id = UAPI_SEA_LIB_MAX;
    }

    ret = uapi_vad_deinit();
    if (ret != EXT_SUCCESS) {
        return EXT_FAILURE;
    }

    ret = uapi_ai_deinit();
    if (ret != EXT_SUCCESS) {
        return EXT_FAILURE;
    }

    ret = uapi_sea_deinit();
    if (ret != EXT_SUCCESS) {
        return EXT_FAILURE;
    }

    ret = uapi_adp_deinit();
    if (ret != EXT_SUCCESS) {
        return EXT_FAILURE;
    }

    osal_wait_destroy(&g_vad_wait);
    return EXT_SUCCESS;
}

static td_s32 sap_open_ai_capture(td_void)
{
#ifdef CAPTURE_AI_INPUT
    td_s32 ret;
    uapi_adp_attr adp_attr;

    uapi_adp_get_def_attr(&adp_attr);
    ret = uapi_adp_create(&g_adp_raw, &adp_attr);
    if (ret != EXT_SUCCESS) {
        uapi_ai_detach_output(g_ai_kws, g_sea);
        uapi_ai_close(g_ai_kws);
        return EXT_FAILURE;
    }

    ret = uapi_ai_attach_output(g_ai_kws, g_adp_raw);
    if (ret != EXT_SUCCESS) {
        uapi_ai_detach_output(g_ai_kws, g_sea);
        uapi_ai_close(g_ai_kws);
        uapi_adp_destroy(g_adp_raw);
        return EXT_FAILURE;
    }
#endif
    return EXT_SUCCESS;
}

static td_s32 sap_close_ai_capture(td_void)
{
#ifdef CAPTURE_AI_INPUT
    td_s32 ret;
    ret = uapi_ai_detach_output(g_ai_kws, g_adp_raw);
    if (ret != EXT_SUCCESS) {
        return EXT_FAILURE;
    }
    uapi_adp_destroy(g_adp_raw);
    g_adp_raw = (td_handle)TD_NULL;
#endif
    return EXT_SUCCESS;
}

static td_s32 sap_attach_audio_port(td_void)
{
    td_s32 ret;
    uapi_ai_port ai_port;
    uapi_ai_attr ai_attr;

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_SMART_VOICE, "sap_attach_audio_port");
    if (g_input_mode == INNER_PORT) {
#ifdef AUDIO_INPUT_ADC
        ai_port = UAPI_AI_PORT_ADC0;
        uapi_ai_get_default_attr(ai_port, &ai_attr);
        ai_attr.port_attr.pdm.i2s_attr.channels = UAPI_AUDIO_CHANNEL_1;
        ai_attr.port_attr.pdm.i2s_attr.bit_depth = UAPI_AUDIO_BIT_DEPTH_16;

        ai_attr.pcm_attr.channels = UAPI_AUDIO_CHANNEL_1;
        ai_attr.pcm_attr.bit_depth = UAPI_AUDIO_BIT_DEPTH_16;
        ai_attr.pcm_attr.sample_rate = UAPI_AUDIO_SAMPLE_RATE_16K;
        ai_attr.pcm_attr.sample_per_frame = UAPI_AUDIO_SAMPLE_RATE_16K / 100; /* 100: ratio */
        if (g_eng_params.u.asr_attr.enable_aec) {
            ai_attr.ref_attr.enable = TD_TRUE;
            ai_attr.ref_attr.port = UAPI_SND_OUT_PORT_I2S1;
        }
#else
        ai_port = UAPI_AI_PORT_PDM0;
        uapi_ai_get_default_attr(ai_port, &ai_attr);
        ai_attr.port_attr.pdm.rx_type = UAPI_AI_RX_MAD;
        ai_attr.vad_attr.enable = TD_TRUE;
        ai_attr.vad_attr.attr.always_on = TD_FALSE;
        ai_attr.port_attr.pdm.i2s_attr.channels = UAPI_AUDIO_CHANNEL_1;
        ai_attr.port_attr.pdm.i2s_attr.bit_depth = UAPI_AUDIO_BIT_DEPTH_16;

        ai_attr.pcm_attr.channels = UAPI_AUDIO_CHANNEL_1;
        ai_attr.pcm_attr.bit_depth = UAPI_AUDIO_BIT_DEPTH_16;
        ai_attr.pcm_attr.sample_rate = UAPI_AUDIO_SAMPLE_RATE_16K;
        ai_attr.pcm_attr.sample_per_frame = UAPI_AUDIO_SAMPLE_RATE_16K / 100; /* 100: ratio */
        ai_attr.ref_attr.enable = TD_FALSE;
#endif
        ret = uapi_ai_open(&g_ai_kws, ai_port, &ai_attr);
        if (ret != EXT_SUCCESS) {
            return EXT_FAILURE;
        }
    }
    ret = uapi_ai_attach_output(g_ai_kws, g_sea);
    if (ret != EXT_SUCCESS) {
        uapi_ai_close(g_ai_kws);
        return EXT_FAILURE;
    }

    sap_open_ai_capture();
    return EXT_SUCCESS;
}

static td_s32 sap_detach_audio_port(td_void)
{
    td_s32 ret;

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_SMART_VOICE, "sap_detach_audio_port");
    if (g_ai_kws == (td_handle)TD_NULL) {
        return EXT_FAILURE;
    }

    ret = uapi_ai_stop(g_ai_kws);
    if (ret != EXT_SUCCESS) {
        return EXT_FAILURE;
    }

    ret = uapi_ai_detach_output(g_ai_kws, g_sea);
    if (ret != EXT_SUCCESS) {
        return EXT_FAILURE;
    }
    sap_close_ai_capture();
    if (g_input_mode == INNER_PORT) {
        ret = uapi_ai_close(g_ai_kws);
        if (ret != EXT_SUCCESS) {
            return EXT_FAILURE;
        }
        g_ai_kws = (td_handle)TD_NULL;
    }
    return EXT_SUCCESS;
}


static td_s32 sap_attach_input_adp(td_void)
{
    td_s32 ret;
    uapi_adp_attr adp_attr;

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_SMART_VOICE, "sap_attach_input_adp");
    uapi_adp_get_def_attr(&adp_attr);
    ret = uapi_adp_create(&g_adp_input, &adp_attr);
    if (ret != EXT_SUCCESS) {
        return EXT_FAILURE;
    }

    ret = uapi_adp_attach_output(g_adp_input, g_sea);
    if (ret != EXT_SUCCESS) {
        return EXT_FAILURE;
    }
    return EXT_SUCCESS;
}

static td_s32 sap_detach_input_adp(td_void)
{
    td_s32 ret;

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_SMART_VOICE, "sap_detach_input_adp");
    if (g_sea == (td_handle)TD_NULL || g_adp_input == (td_handle)TD_NULL) {
        return EXT_FAILURE;
    }

    ret = uapi_adp_detach_output(g_adp_input, g_sea);
    if (ret != EXT_SUCCESS) {
        return EXT_FAILURE;
    }

    ret = uapi_adp_destroy(g_adp_input);
    if (ret != EXT_SUCCESS) {
        return EXT_FAILURE;
    }
    g_adp_input = (td_handle)TD_NULL;
    return EXT_SUCCESS;
}

static td_s32 sap_create_engine(td_void)
{
    uapi_sea_attr sea_attr;
    uapi_adp_attr adp_attr;
    uapi_sea_eng_sel sea_eng;
    td_s32 ret;

    memset_s(&sea_eng, sizeof(sea_eng), 0, sizeof(sea_eng));
    sea_eng.afe.type = UAPI_SEA_AFE_SEE;
    sea_eng.afe.lib_id = UAPI_SEA_LIB_SEE;
    sea_eng.aai[0].type = UAPI_SEA_AAI_KWS;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_SMART_VOICE, "sap_create_engine");
    if (g_eng_params.u.asr_attr.enable_npu) {
        sea_eng.aai[0].lib_id = UAPI_SEA_LIB_ASR;
        sap_load_aai_engine(UAPI_SEA_LIB_ASR);
    } else {
        sea_eng.aai[0].lib_id = UAPI_SEA_LIB_KWS;
        sap_load_aai_engine(UAPI_SEA_LIB_KWS);
    }
    ret = uapi_sea_load_engine(UAPI_SEA_LIB_SEE, "imedia_2mic");
    if (ret != EXT_SUCCESS) {
        return EXT_FAILURE;
    }
    uapi_sea_get_default_attr(&sea_eng, &sea_attr);
#ifdef AUDIO_INPUT_ADC
    if (g_eng_params.u.asr_attr.enable_aec == TD_FALSE) {
        sea_attr.ref_pcm.channels = UAPI_AUDIO_CHANNEL_0;
    } else {
        sea_attr.ref_pcm.channels = UAPI_AUDIO_CHANNEL_1;
    }
#else
    sea_attr.ref_pcm.channels = UAPI_AUDIO_CHANNEL_0;
#endif
    ret = uapi_sea_create(&g_sea, &sea_eng, &sea_attr);
    if (ret != EXT_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "uapi_sea_create failed");
        return EXT_FAILURE;
    }
    g_phrase_num = update_phrase_set();
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_SMART_VOICE, "g_phrase_num: %d phrases", g_phrase_num);
    ret = uapi_sea_register_event_proc(g_sea, sea_event_proc, TD_NULL);
    if (ret != EXT_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "uapi_sea_register_event_proc failed");
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

static td_s32 sap_destroy_engine(td_void)
{
    td_s32 ret;

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_SMART_VOICE, "sap_destroy_engine");

    if (g_sea == (td_handle)TD_NULL) {
        return EXT_FAILURE;
    }

    ret = uapi_sea_destroy(g_sea);
    if (ret != EXT_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "uapi_sea_destroy failed");
        return EXT_FAILURE;
    }

    ret = uapi_sea_unload_engine(UAPI_SEA_LIB_SEE, "imedia_2mic");
    if (ret != EXT_SUCCESS) {
        return EXT_FAILURE;
    }

    g_sea = (td_handle)TD_NULL;
    g_phrase_num = 0;
    return EXT_SUCCESS;
}

static td_s32 sap_start_engine(td_void)
{
    td_s32 ret;
    if (g_sea == (td_handle)TD_NULL) {
        return EXT_FAILURE;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_SMART_VOICE, "sap_start_engine");
    ret = uapi_sea_start(g_sea);
    if (ret != EXT_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "uapi_sea_start failed");
        return EXT_FAILURE;
    }
    return EXT_SUCCESS;
}

static td_s32 sap_stop_engine(td_void)
{
    td_s32 ret;
    if (g_sea == (td_handle)TD_NULL) {
        return EXT_FAILURE;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_SMART_VOICE, "sap_stop_engine");
    ret = uapi_sea_stop(g_sea);
    if (ret != EXT_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "uapi_sea_stop failed");
        return EXT_FAILURE;
    }
    return EXT_SUCCESS;
}

static td_s32 create_voice_data_path(td_void)
{
    td_s32 ret;

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_SMART_VOICE, "create_voice_data_path");
    ret = sap_create_engine();
    if (ret != EXT_SUCCESS) {
        return ret;
    }

    if (g_input_mode == DATA_INJECTION) {
        ret = sap_attach_input_adp();
        if (ret != EXT_SUCCESS) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "sap_attach_input_adp failed");
            return ret;
        }
    } else {
        ret = sap_attach_audio_port();
        if (ret != EXT_SUCCESS) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "sap_attach_audio_port failed");
            return ret;
        }
    }
    return EXT_SUCCESS;
}

static td_s32 start_voice_data_path(td_void)
{
    td_s32 ret;

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_SMART_VOICE, "start_voice_data_path");
    if (g_input_mode != DATA_INJECTION) {
        ret = uapi_ai_start(g_ai_kws);
        if (ret != EXT_SUCCESS) {
            return EXT_FAILURE;
        }
    }
    ret = sap_start_engine();
    if (ret != EXT_SUCCESS) {
        return ret;
    }
    return EXT_SUCCESS;
}

static td_s32 stop_voice_data_path(td_void)
{
    td_s32 ret;

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_SMART_VOICE, "stop_voice_data_path");
    ret = sap_stop_engine();
    if (ret != EXT_SUCCESS) {
        return ret;
    }
    return EXT_SUCCESS;
}

static td_s32 destroy_voice_data_path(td_void)
{
    td_s32 ret;

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_SMART_VOICE, "destroy_voice_data_path");
    if (g_input_mode == DATA_INJECTION) {
        ret = sap_detach_input_adp();
        if (ret != EXT_SUCCESS) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "sap_detach_input_adp failed");
            return ret;
        }
    } else {
        ret = sap_detach_audio_port();
        if (ret != EXT_SUCCESS) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "sap_detach_inner_audio_port failed");
            return ret;
        }
    }
    ret = sap_destroy_engine();
    if (ret != EXT_SUCCESS) {
        return ret;
    }
    return EXT_SUCCESS;
}

static td_s32 start_avad(td_void)
{
    td_s32 ret;
    uapi_vad_attr vad_attr;

    uapi_vad_get_default_attr(&vad_attr);
    vad_attr.always_on = TD_FALSE;
    vad_attr.vad_type = UAPI_VAD_AVAD;

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_SMART_VOICE, "start_avad");

    ret = uapi_vad_open(&g_avad, &vad_attr);
    if (ret != EXT_SUCCESS) {
        return EXT_FAILURE;
    }

    ret = uapi_vad_register_event_proc(g_avad, vad_event_proc, TD_NULL);
    if (ret != EXT_SUCCESS) {
        uapi_vad_close(g_avad);
        g_ai_mad = (td_handle)TD_NULL;
        return EXT_FAILURE;
    }

    ret = uapi_vad_reset(g_avad);
    if (ret != EXT_SUCCESS) {
        uapi_vad_close(g_avad);
        g_avad = (td_handle)TD_NULL;
        return EXT_FAILURE;
    }
    return EXT_SUCCESS;
}

static td_s32 stop_avad(td_void)
{
    td_s32 ret;

    if (g_avad == (td_handle)TD_NULL) {
        return EXT_FAILURE;
    }

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_SMART_VOICE, "stop_avad");
    ret = uapi_vad_close(g_avad);
    if (ret != EXT_SUCCESS) {
        return EXT_FAILURE;
    }
    g_avad = (td_handle)TD_NULL;
    return EXT_SUCCESS;
}

static td_s32 start_mad(td_void)
{
    td_s32 ret;
    uapi_ai_port ai_port;
    uapi_ai_attr ai_attr;

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_SMART_VOICE, "start_mad");
    if (!g_ai_kws) {
        ai_port = UAPI_AI_PORT_PDM0;
        uapi_ai_get_default_attr(ai_port, &ai_attr);
        ai_attr.port_attr.pdm.rx_type = UAPI_AI_RX_MAD;
        ai_attr.vad_attr.enable = TD_TRUE;
        ai_attr.vad_attr.attr.always_on = TD_FALSE;
        ai_attr.port_attr.pdm.i2s_attr.channels = 1;
        ai_attr.port_attr.pdm.i2s_attr.bit_depth = UAPI_AUDIO_BIT_DEPTH_16;

        ai_attr.pcm_attr.channels = 1;
        ai_attr.pcm_attr.bit_depth = UAPI_AUDIO_BIT_DEPTH_16;
        ai_attr.pcm_attr.sample_rate = UAPI_AUDIO_SAMPLE_RATE_16K;
        ai_attr.pcm_attr.sample_per_frame = UAPI_AUDIO_SAMPLE_RATE_16K / 100; /* 100: ratio */
        ai_attr.ref_attr.enable = TD_FALSE;

        ret = uapi_ai_open(&g_ai_mad, ai_port, &ai_attr);
        if (ret != EXT_SUCCESS) {
            return EXT_FAILURE;
        }
    } else {
        g_ai_mad = g_ai_kws;
    }

    ret = uapi_ai_register_event_proc(g_ai_mad, ai_event_proc, TD_NULL);
    if (ret != EXT_SUCCESS) {
        uapi_ai_close(g_ai_mad);
        g_ai_mad = (td_handle)TD_NULL;
        return EXT_FAILURE;
    }

    ret = uapi_ai_reset_vad(g_ai_mad);
    if (ret != EXT_SUCCESS) {
        uapi_ai_close(g_ai_mad);
        g_ai_mad = (td_handle)TD_NULL;
        return EXT_FAILURE;
    }
    return EXT_SUCCESS;
}

static td_s32 stop_mad(td_void)
{
    td_s32 ret;

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_SMART_VOICE, "stop_mad");
    if (g_ai_mad == (td_handle)TD_NULL) {
        return EXT_FAILURE;
    }

    if (g_ai_mad == g_ai_kws) {
        return EXT_SUCCESS;
    }
    ret = uapi_ai_stop(g_ai_mad);
    if (ret != EXT_SUCCESS) {
        return EXT_FAILURE;
    }

    ret = uapi_ai_close(g_ai_mad);
    if (ret != EXT_SUCCESS) {
        return EXT_FAILURE;
    }
    g_ai_mad = (td_handle)TD_NULL;
    return EXT_SUCCESS;
}

static td_s32 is_vad_state_changed(const td_void *param)
{
    (void)param;
    return g_vad_state != g_vad_new_state;
}

static td_s32 is_vad_state_closed(const td_void *param)
{
    (void)param;
    return g_vad_state == STATE_IDLE;
}

static td_s32 vad_state_machine_task(td_void *data)
{
    (void)data;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_SMART_VOICE, "vad_state_machine_task");
    while (g_vad_new_state != STATE_IDLE) {
        if (osal_wait_timeout_interruptible(&g_vad_wait, is_vad_state_changed, TD_NULL, WAIT_TIMEOUT_IN_MS) > 0) {
            if (g_vad_new_state == STATE_MAD_DETECTING && g_vad_state == STATE_AVAD_DETECTING) {
                osal_mutex_lock(&g_mutex);
                stop_avad();
                start_mad();
                g_vad_state = g_vad_new_state;
                osal_mutex_unlock(&g_mutex);
            } else if (g_vad_new_state == STATE_AVAD_DETECTING && g_vad_state == STATE_MAD_DETECTING) {
                osal_mutex_lock(&g_mutex);
                stop_mad();
                start_avad();
                g_vad_state = g_vad_new_state;
                osal_mutex_unlock(&g_mutex);
            }
        }
    }
    g_vad_state = STATE_IDLE;
    osal_wait_wakeup(&g_vad_wait);
    return EXT_SUCCESS;
}

static td_s32 start_vad_state_machine(td_void)
{
    td_s32 ret;

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_SMART_VOICE, "start_vad_state_machine");
    g_vad_state = STATE_AVAD_DETECTING;
    g_vad_new_state = g_vad_state;
    if (osal_kthread_create(vad_state_machine_task, TD_NULL, "AI State Machine", 0x1000) == TD_NULL) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "can not create VE State Machine task");
        return EXT_FAILURE;
    }
    ret = start_avad();
    if (ret != EXT_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "start avad detection failed");
        return EXT_FAILURE;
    }
    return EXT_SUCCESS;
}

static td_s32 stop_vad_state_machine(td_void)
{
    td_s32 ret;

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_SMART_VOICE, "stop_vad_state_machine");
    if (g_vad_state == STATE_IDLE) {
        return EXT_SUCCESS;
    }
    g_vad_new_state = STATE_IDLE;
    osal_wait_wakeup(&g_vad_wait);
    usleep(WAIT_TASK_EXIT);
    ret = osal_wait_timeout_interruptible(&g_vad_wait, is_vad_state_closed, TD_NULL, WAIT_TIMEOUT_IN_MS);
    if (ret > 0) {
        ret = EXT_SUCCESS;
    } else {
        ret = EXT_FAILURE;
    }
    stop_avad();
    stop_mad();
    return ret;
}

td_s32 vehal_init(td_void)
{
    td_s32 ret;

    ret = osal_mutex_init(&g_mutex);
    if (ret != EXT_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "osal_mutex_init in vehal_init failed");
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_SMART_VOICE, "vehal_init");
    ret = sap_init_engine();
    if (ret != EXT_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "init sap voice engine failed");
        return ret;
    }

    return EXT_FAILURE;
}

td_s32 vehal_deinit(td_void)
{
    td_s32 ret;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_SMART_VOICE, "sap_deinit_engine");
    ret = sap_deinit_engine();
    if (ret != EXT_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "deinit sap voice engine failed");
        return ret;
    }
    osal_mutex_destroy(&g_mutex);
    (td_void)memset_s(&g_mutex, sizeof(g_mutex), 0, sizeof(g_mutex));
    return EXT_SUCCESS;
}

td_s32 vehal_set_input(voice_engine_input_mode input_mode, td_void *param, td_u32 length)
{
    (void)length;
    osal_mutex_lock(&g_mutex);
    g_input_mode = input_mode;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_SMART_VOICE, "vehal_set_input input_mode = %d", input_mode);
    if (g_input_mode == EXT_PORT) {
        g_ai_kws = (td_handle)TD_NULL;
        if (param == TD_NULL) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_SMART_VOICE, "set_voice_engine_input params error");
            osal_mutex_unlock(&g_mutex);
            return -1;
        }
        g_ai_kws = *((td_handle *)param);
    }
    osal_mutex_unlock(&g_mutex);
    return EXT_SUCCESS;
}

td_s32 vehal_start_vad(td_void)
{
    td_s32 ret;

    osal_mutex_lock(&g_mutex);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_SMART_VOICE, "vehal_start_vad");
    if (g_vad_mode == ONLY_MAD) {
        ret = start_mad();
    } else if (g_vad_mode == ONLY_AVAD) {
        ret = start_avad();
    } else {
        ret = start_vad_state_machine();
    }
    osal_mutex_unlock(&g_mutex);
    return ret;
}

td_s32 vehal_stop_vad(td_void)
{
    td_s32 ret;
    osal_mutex_lock(&g_mutex);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_SMART_VOICE, "vehal_stop_vad");
    if (g_vad_mode == ONLY_MAD) {
        ret = stop_mad();
    } else if (g_vad_mode == ONLY_AVAD) {
        ret = stop_avad();
    } else {
        ret = stop_vad_state_machine();
    }
    osal_mutex_unlock(&g_mutex);
    return ret;
}

td_s32 vehal_create_engine(voice_engine_params *eng_params)
{
    osal_mutex_lock(&g_mutex);
    g_eng_params.type = ASR_ENGINE;
    g_eng_params.u.asr_attr.enable_vid = eng_params->u.asr_attr.enable_vid;
    g_eng_params.u.asr_attr.enable_nlp = eng_params->u.asr_attr.enable_nlp;
    g_eng_params.u.asr_attr.enable_aec = eng_params->u.asr_attr.enable_aec;
    g_eng_params.u.asr_attr.enable_npu = eng_params->u.asr_attr.enable_npu;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_SMART_VOICE, "vehal_create_engine");
    create_voice_data_path();
    osal_mutex_unlock(&g_mutex);
    return EXT_SUCCESS;
}

td_s32 vehal_start_engine(td_void)
{
    td_s32 ret;
    osal_mutex_lock(&g_mutex);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_SMART_VOICE, "vehal_start_engine");
    ret = start_voice_data_path();
    osal_mutex_unlock(&g_mutex);
    return ret;
}

td_s32 vehal_stop_engine(td_void)
{
    td_s32 ret;
    osal_mutex_lock(&g_mutex);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_SMART_VOICE, "vehal_stop_engine");
    ret = stop_voice_data_path();
    g_vad_state = STATE_IDLE;
    g_vad_new_state = STATE_IDLE;
    osal_mutex_unlock(&g_mutex);
    return ret;
}

td_s32 vehal_destroy_engine(td_void)
{
    osal_mutex_lock(&g_mutex);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_SMART_VOICE, "vehal_destroy_engine");
    destroy_voice_data_path();
    g_cb = (voice_engine_cb)TD_NULL;
    osal_mutex_unlock(&g_mutex);
    return EXT_SUCCESS;
}

td_s32 vehal_set_callback(voice_engine_cb callback)
{
    osal_mutex_lock(&g_mutex);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_SMART_VOICE, "vehal_set_callback");
    g_cb = callback;
    osal_mutex_unlock(&g_mutex);
    return EXT_SUCCESS;
}

td_u32 vehal_inject(td_uchar *data, td_u32 length)
{
    td_s32 ret;
    uapi_stream_buf stream;

    if (g_adp_input == (td_handle)TD_NULL) {
        return 0;
    }

    stream.data = data;
    stream.size = length;
    stream.pts = 0;
    ret = uapi_adp_send_stream(g_adp_input, &stream);
    if (ret != EXT_SUCCESS) {
        WEARABLE_LOGW(WEARABLE_LOG_MODULE_SMART_VOICE, "uapi_adp_send_stream failed");
        return 0;
    }
    return length;
}

td_u32 vehal_capture(td_uchar *data, td_u32 length)
{
    td_s32 ret;
    td_u32 len;
    uapi_stream_buf stream;

    if (g_adp_output == (td_handle)TD_NULL) {
        return 0;
    }

    len = length;
    ret = uapi_adp_acquire_stream(g_adp_output, &stream);
    if (ret != EXT_SUCCESS) {
        return 0;
    }
    if (stream.size < length) {
        len = stream.size;
    }
    memcpy_s(data, length, stream.data, len);
    uapi_adp_release_stream(g_adp_output, &stream);
    return len;
}

td_u32 vehal_raw_capture(td_uchar *data, td_u32 length)
{
    td_s32 ret;
    td_u32 len;
    uapi_stream_buf stream;

    if (g_adp_raw == (td_handle)TD_NULL) {
        return 0;
    }

    len = length;
    ret = uapi_adp_acquire_stream(g_adp_raw, &stream);
    if (ret != EXT_SUCCESS) {
        return 0;
    }
    if (stream.size < length) {
        len = stream.size;
    }
    memcpy_s(data, length, stream.data, len);
    uapi_adp_release_stream(g_adp_raw, &stream);
    return len;
}

td_void vehal_dump(td_void)
{
    td_u32 i;
    WEARABLE_LOGW(WEARABLE_LOG_MODULE_SMART_VOICE, "voice engine dump info:");
    WEARABLE_LOGW(WEARABLE_LOG_MODULE_SMART_VOICE, "engine type: %d", g_eng_params.type);
    WEARABLE_LOGW(WEARABLE_LOG_MODULE_SMART_VOICE, "input mode: %d", g_input_mode);
    WEARABLE_LOGW(WEARABLE_LOG_MODULE_SMART_VOICE, "vad state: %d", g_vad_state);
    if (g_eng_params.type == ASR_ENGINE) {
        WEARABLE_LOGW(WEARABLE_LOG_MODULE_SMART_VOICE, "enable vid: %d", g_eng_params.u.asr_attr.enable_vid);
        WEARABLE_LOGW(WEARABLE_LOG_MODULE_SMART_VOICE, "enable nlp: %d", g_eng_params.u.asr_attr.enable_nlp);
        WEARABLE_LOGW(WEARABLE_LOG_MODULE_SMART_VOICE, "enable aec: %d", g_eng_params.u.asr_attr.enable_aec);
        WEARABLE_LOGW(WEARABLE_LOG_MODULE_SMART_VOICE, "enable npu: %d", g_eng_params.u.asr_attr.enable_npu);
        WEARABLE_LOGW(WEARABLE_LOG_MODULE_SMART_VOICE, "support phrase number: %d", g_phrase_num);
        for (i = 0; i < g_phrase_num; i++) {
            WEARABLE_LOGW(WEARABLE_LOG_MODULE_SMART_VOICE, "phrase: %d %s", g_phrase[i].id, g_phrase[i].name);
        }
    }
}
/*
 * Copyright (c) CompanyNameMagicTag 2019-2020. All rights reserved.
 * Description: sea sample
 * Author: audio
 * Create: 2019-09-17
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "thread_os.h"
#include "chip_type.h"
#include "audio_type.h"
#include "securec.h"
#include "soc_uapi_adp.h"
#include "soc_uapi_vad.h"
#include "soc_uapi_ai.h"
#include "soc_uapi_sea.h"
#include "sample_audio_api.h"
#include "sample_audio_utils.h"

#if (SAP_CHIP_TYPE == brandy)
#include "pm_clocks.h"
#endif /* SAP_CHIP_TYPE */

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define LPWK_PHRASE_COUNT 21
#define LPWK_TASK_NAME_LENGTH 32
#define LPWK_STREAM_MAX_DUR 600  /* 10 minute */
#define LPWK_VAD_RESET_INTERVAL  5
#define LPWK_AI_DEF_PORT UAPI_AI_PORT_PDM0
#define LPWK_VAD_TASK_NAME "lpwk_vad"
#define LPWK_AI_TASK_NAME "lpwk_ai"
#define LPWK_SEA_TASK_NAME "lpwk_sea"

typedef enum {
    LPWK_SVC_VAD = 0,
    LPWK_SVC_AI,
    LPWK_SVC_SEA,
    LPWK_SVC_MAX
} lpwk_svc_type;

typedef enum {
    LPWK_LEVEL_MIN = 0,
    LPWK_LEVEL_1 = LPWK_LEVEL_MIN,
    LPWK_LEVEL_2,
    LPWK_LEVEL_3,
    LPWK_LEVEL_MAX
} lpwk_level_type;

typedef enum {
    LPWK_VAD_MIN = 0,
    LPWK_VAD_AVAD = LPWK_VAD_MIN,
    LPWK_VAD_MAD,
    LPWK_VAD_MAX
} lpwk_vad_type;

typedef enum {
    LPWK_KWS_MIN = 0,
    LPWK_KWS_3A_GRU = LPWK_KWS_MIN,
    LPWK_KWS_3A_ANPU,
    LPWK_KWS_GRU,
    LPWK_KWS_ANPU,
    LPWK_KWS_MAX
} lpwk_kws_type;

typedef struct {
    lpwk_level_type level;
    lpwk_vad_type vad_type;
    lpwk_kws_type kws_type;
    uapi_ai_port ai_port;
    uapi_sea_lib_id afe_id;
    uapi_sea_lib_id aai_id;
    uapi_audio_pcm_format pcm_attr;
    td_bool export_data;
} lpwk_cmd_opt;

typedef struct {
    lpwk_cmd_opt *cmd_opt;
    uapi_audio_pcm_format out_pcm;

    lpwk_svc_type type;
    athread_handle h_task;
    td_bool task_enable;
    td_char task_name[LPWK_TASK_NAME_LENGTH];

    FILE *h_file;
    td_u32 file_size;
    td_u32 file_limit_size;
    td_u32 (*save_frame)(FILE *h_file, const uapi_audio_frame *frame);

    td_handle h_adp;
    td_handle h_mod;
    td_s32 (*close)(td_handle mod);
    td_s32 (*start)(td_handle mod);
    td_s32 (*stop)(td_handle mod);
    td_s32 (*reset)(td_handle mod);
    td_s32 (*attach)(td_handle mod, td_handle output);
    td_s32 (*detach)(td_handle mod, td_handle output);
} lpwk_svc_inst;

typedef struct {
    td_u32 phs_count;
    td_u32 phs_match_cnt[LPWK_PHRASE_COUNT];
    uapi_sea_item phrase[LPWK_PHRASE_COUNT];
} lpwk_phs_sets;

static td_bool g_lpwk_inst_inited = TD_FALSE;
static lpwk_cmd_opt g_lpwk_cmd_opt;
static lpwk_phs_sets g_lpwk_phs_sets;
static lpwk_svc_inst g_lpwk_inst[LPWK_SVC_MAX];

typedef td_s32 (*lpwk_fsm_next)(lpwk_svc_inst *vad_inst, lpwk_svc_inst *ai_inst, lpwk_svc_inst *sea_inst);
typedef td_s32 (*lpwk_event_proc)(td_handle mod, td_s32 event, const td_void *param, td_void *context);
static td_void lpwk_save_frame(lpwk_svc_inst *inst, const uapi_audio_frame *frame);
static td_void vad_reset_proc(td_void *args);
static td_s32 vad_event_proc(td_handle vad, td_s32 event, const td_void *param, td_void *context);
static td_s32 ai_event_proc(td_handle ai, td_s32 event, const td_void *param, td_void *context);
static td_void lpwk_start_task(lpwk_svc_inst *inst);
static td_void lpwk_stop_task(lpwk_svc_inst *inst);
static td_s32 lpwk_open_inst(lpwk_svc_inst *inst, lpwk_event_proc eproc, td_void *context);
static td_void lpwk_close_inst(lpwk_svc_inst *inst);
static td_s32 lpwk_open_kws_inst(lpwk_svc_inst *ai_inst, lpwk_svc_inst *sea_inst);
static td_s32 lpwk_close_kws_inst(lpwk_svc_inst *ai_inst, lpwk_svc_inst *sea_inst);

static td_s32 fsm_l3_start(lpwk_svc_inst *vad_inst, const lpwk_svc_inst *ai_inst, const lpwk_svc_inst *sea_inst)
{
    td_s32 ret;

    ret = lpwk_open_inst(vad_inst, vad_event_proc, (td_void *)vad_inst);
    if (ret != EXT_SUCCESS) {
        return ret;
    }

    if (vad_inst->reset != TD_NULL) {
        vad_inst->reset(vad_inst->h_mod);
    }

    sap_unused(ai_inst);
    sap_unused(sea_inst);
    return EXT_SUCCESS;
}

static td_s32 fsm_l2_start(lpwk_svc_inst *vad_inst, lpwk_svc_inst *ai_inst, lpwk_svc_inst *sea_inst)
{
    td_s32 ret;
    lpwk_cmd_opt *cmd_opt = vad_inst->cmd_opt;

    if (cmd_opt->vad_type == LPWK_VAD_AVAD) {
        ret = lpwk_open_inst(vad_inst, vad_event_proc, (td_void *)vad_inst);
        if (ret != EXT_SUCCESS) {
            return ret;
        }

        if (vad_inst->reset != TD_NULL) {
            vad_inst->reset(vad_inst->h_mod);
        }
    } else {
        ret = lpwk_open_kws_inst(ai_inst, sea_inst);
        if (ret != EXT_SUCCESS) {
            return ret;
        }
    }

    return EXT_SUCCESS;
}

static td_s32 fsm_l1_start(lpwk_svc_inst *vad_inst, lpwk_svc_inst *ai_inst, const lpwk_svc_inst *sea_inst)
{
    td_s32 ret;
    lpwk_cmd_opt *cmd_opt = vad_inst->cmd_opt;

    if (cmd_opt->vad_type == LPWK_VAD_AVAD) {
        ret = lpwk_open_inst(vad_inst, vad_event_proc, (td_void *)vad_inst);
        if (ret != EXT_SUCCESS) {
            return ret;
        }
        lpwk_start_task(vad_inst);
    } else {
        ret = lpwk_open_inst(ai_inst, ai_event_proc, (td_void *)ai_inst);
        if (ret != EXT_SUCCESS) {
            return ret;
        }
        lpwk_start_task(ai_inst);
    }

    audio_unused(sea_inst);
    return EXT_SUCCESS;
}

static td_s32 lpwk_fsm_start(lpwk_level_type level, lpwk_svc_inst *vad_inst, lpwk_svc_inst *ai_inst,
    lpwk_svc_inst *sea_inst)
{
    switch (level) {
        case LPWK_LEVEL_1:
            return fsm_l1_start(vad_inst, ai_inst, sea_inst);
        case LPWK_LEVEL_2:
            return fsm_l2_start(vad_inst, ai_inst, sea_inst);
        case LPWK_LEVEL_3:
            return fsm_l3_start(vad_inst, ai_inst, sea_inst);
        default:
            break;
    }

    return EXT_SUCCESS;
}

static td_s32 fsm_l3_avad_valid_next(lpwk_svc_inst *vad_inst, lpwk_svc_inst *ai_inst, lpwk_svc_inst *sea_inst)
{
    td_s32 ret;

    lpwk_close_inst(vad_inst);
    vad_inst->cmd_opt->vad_type = LPWK_VAD_MAD;
    ret = lpwk_fsm_start(LPWK_LEVEL_2, vad_inst, ai_inst, sea_inst);
    if (ret != EXT_SUCCESS) {
        return ret;
    }

    return EXT_SUCCESS;
}

static td_s32 fsm_l3_mad_valid_next(const lpwk_svc_inst *vad_inst, lpwk_svc_inst *ai_inst, lpwk_svc_inst *sea_inst)
{
    if (ai_inst->start != TD_NULL) {
        ai_inst->start(ai_inst->h_mod);
    }
    if (sea_inst->start != TD_NULL) {
        sea_inst->start(sea_inst->h_mod);
    }

    audio_unused(vad_inst);
    return EXT_SUCCESS;
}

static td_s32 fsm_l3_sea_match_next(lpwk_svc_inst *vad_inst, lpwk_svc_inst *ai_inst, lpwk_svc_inst *sea_inst)
{
    td_s32 ret;

    if (sea_inst->stop != TD_NULL) {
        sea_inst->stop(sea_inst->h_mod);
    }

    if (ai_inst->stop != TD_NULL) {
        ai_inst->stop(ai_inst->h_mod);
    }

    ret = lpwk_close_kws_inst(ai_inst, sea_inst);
    if (ret != EXT_SUCCESS) {
        return ret;
    }

    ret = fsm_l3_start(vad_inst, ai_inst, sea_inst);
    if (ret != EXT_SUCCESS) {
        return ret;
    }

    return EXT_SUCCESS;
}

static td_s32 fsm_l2_avad_valid_next(lpwk_svc_inst *vad_inst, lpwk_svc_inst *ai_inst, lpwk_svc_inst *sea_inst)
{
    td_s32 ret;

    lpwk_close_inst(vad_inst);
    ret = lpwk_open_kws_inst(ai_inst, sea_inst);
    if (ret != EXT_SUCCESS) {
        return ret;
    }

    if (ai_inst->start != TD_NULL) {
        ai_inst->start(ai_inst->h_mod);
    }
    if (sea_inst->start != TD_NULL) {
        sea_inst->start(sea_inst->h_mod);
    }

    return EXT_SUCCESS;
}

static td_s32 fsm_l2_mad_valid_next(const lpwk_svc_inst *vad_inst, lpwk_svc_inst *ai_inst, lpwk_svc_inst *sea_inst)
{
    if (ai_inst->start != TD_NULL) {
        ai_inst->start(ai_inst->h_mod);
    }
    if (sea_inst->start != TD_NULL) {
        sea_inst->start(sea_inst->h_mod);
    }
    sap_unused(vad_inst);
    return EXT_SUCCESS;
}

static td_s32 fsm_l2_sea_match_next(lpwk_svc_inst *vad_inst, lpwk_svc_inst *ai_inst, lpwk_svc_inst *sea_inst)
{
    td_s32 ret;

    if (sea_inst->cmd_opt->vad_type == LPWK_VAD_AVAD) {
        ret = lpwk_close_kws_inst(ai_inst, sea_inst);
        if (ret != EXT_SUCCESS) {
            return ret;
        }

        ret = lpwk_open_inst(vad_inst, vad_event_proc, (td_void *)vad_inst);
        if (ret != EXT_SUCCESS) {
            return ret;
        }

        if (vad_inst->reset != TD_NULL) {
            vad_inst->reset(vad_inst->h_mod);
        }
    } else {
        if (ai_inst->reset != TD_NULL) {
            ai_inst->reset(ai_inst->h_mod);
        }
    }

    return EXT_SUCCESS;
}

static td_s32 fsm_l1_fsm_next(const lpwk_svc_inst *vad_inst, const lpwk_svc_inst *ai_inst,
    const lpwk_svc_inst *sea_inst)
{
    sap_unused(vad_inst);
    sap_unused(ai_inst);
    sap_unused(sea_inst);
    return EXT_SUCCESS;
}

static td_s32 lpwk_fsm_next_vad(lpwk_level_type level, lpwk_svc_inst *vad_inst, lpwk_svc_inst *ai_inst,
    lpwk_svc_inst *sea_inst)
{
    switch (level) {
        case LPWK_LEVEL_1:
            return fsm_l1_fsm_next(vad_inst, ai_inst, sea_inst);
        case LPWK_LEVEL_2:
            return fsm_l2_avad_valid_next(vad_inst, ai_inst, sea_inst);
        case LPWK_LEVEL_3:
            return fsm_l3_avad_valid_next(vad_inst, ai_inst, sea_inst);
        default:
            break;
    }

    return EXT_SUCCESS;
}

static td_s32 lpwk_fsm_next_ai(lpwk_level_type level, lpwk_svc_inst *vad_inst, lpwk_svc_inst *ai_inst,
    lpwk_svc_inst *sea_inst)
{
    switch (level) {
        case LPWK_LEVEL_1:
            return fsm_l1_fsm_next(vad_inst, ai_inst, sea_inst);
        case LPWK_LEVEL_2:
            return fsm_l2_mad_valid_next(vad_inst, ai_inst, sea_inst);
        case LPWK_LEVEL_3:
            return fsm_l3_mad_valid_next(vad_inst, ai_inst, sea_inst);
        default:
            break;
    }

    return EXT_SUCCESS;
}

static td_s32 lpwk_fsm_next_sea(lpwk_level_type level, lpwk_svc_inst *vad_inst, lpwk_svc_inst *ai_inst,
    lpwk_svc_inst *sea_inst)
{
    switch (level) {
        case LPWK_LEVEL_1:
            return fsm_l1_fsm_next(vad_inst, ai_inst, sea_inst);
        case LPWK_LEVEL_2:
            return fsm_l2_sea_match_next(vad_inst, ai_inst, sea_inst);
        case LPWK_LEVEL_3:
            return fsm_l3_sea_match_next(vad_inst, ai_inst, sea_inst);
        default:
            break;
    }

    return EXT_SUCCESS;
}

static td_s32 lpwk_fsm_next_fun(lpwk_svc_type type, lpwk_level_type level, lpwk_svc_inst *vad_inst,
    lpwk_svc_inst *ai_inst, lpwk_svc_inst *sea_inst)
{
    switch (type) {
        case LPWK_SVC_VAD:
            return lpwk_fsm_next_vad(level, vad_inst, ai_inst, sea_inst);
        case LPWK_SVC_AI:
            return lpwk_fsm_next_ai(level, vad_inst, ai_inst, sea_inst);
        case LPWK_SVC_SEA:
            return lpwk_fsm_next_sea(level, vad_inst, ai_inst, sea_inst);
        default:
            break;
    }

    return EXT_SUCCESS;
}

static td_s32 vad_event_proc(td_handle vad, td_s32 event, const td_void *param, td_void *context)
{
    uapi_vad_event_type vad_event = (uapi_vad_event_type)event;
    lpwk_svc_inst *vad_inst = (lpwk_svc_inst *)context;
    lpwk_svc_inst *ai_inst = &g_lpwk_inst[LPWK_SVC_AI];
    lpwk_svc_inst *sea_inst = &g_lpwk_inst[LPWK_SVC_SEA];

    if (vad_event == UAPI_VAD_EVENT_VAD_VALID) {
        sap_err_log_h32(vad_event);
        lpwk_fsm_next_fun(vad_inst->type, vad_inst->cmd_opt->level, vad_inst, ai_inst, sea_inst);
    }

    audio_unused(vad);
    audio_unused(param);
    return EXT_SUCCESS;
}

static td_s32 ai_event_proc(td_handle ai, td_s32 event, const td_void *param, td_void *context)
{
    uapi_ai_event_type ai_event = (uapi_ai_event_type)event;
    lpwk_svc_inst *ai_inst = (lpwk_svc_inst *)context;
    lpwk_svc_inst *vad_inst = &g_lpwk_inst[LPWK_SVC_VAD];
    lpwk_svc_inst *sea_inst = &g_lpwk_inst[LPWK_SVC_SEA];

    if (ai_event == UAPI_AI_EVENT_VAD_VALID) {
#if (SAP_CHIP_TYPE == brandy)
        uapi_set_hifi_mode(HIFI_LPM_CLK_HIGH_LEVEL);
#endif /* SAP_CHIP_TYPE */
        sap_err_log_h32(ai_event);
        lpwk_fsm_next_fun(ai_inst->type, ai_inst->cmd_opt->level, vad_inst, ai_inst, sea_inst);
    } else if (ai_event == UAPI_AI_EVENT_VAD_TIMEOUT) {
        sap_err_log_h32(ai_event);
        lpwk_fsm_next_fun(sea_inst->type, ai_inst->cmd_opt->level, vad_inst, ai_inst, sea_inst);
    } else {
    }

    audio_unused(ai);
    audio_unused(param);
    return EXT_SUCCESS;
}

static td_s32 sea_event_proc(td_handle sea, td_s32 event, const td_void *param, td_void *context)
{
    td_u32 i;
    uapi_sea_event_type sea_event = (uapi_sea_event_type)event;
    uapi_sea_kws_event_param *event_param = (uapi_sea_kws_event_param *)param;
    lpwk_phs_sets *phs_sets = &g_lpwk_phs_sets;
    lpwk_svc_inst *sea_inst = (lpwk_svc_inst *)context;
    lpwk_svc_inst *vad_inst = &g_lpwk_inst[LPWK_SVC_VAD];
    lpwk_svc_inst *ai_inst = &g_lpwk_inst[LPWK_SVC_AI];

    sap_err_log_h32(sea_event);
    if (sea_event == UAPI_SEA_EVENT_KWS_MATCH) {
        if (event_param->phrase_id == 0) {
            return EXT_SUCCESS;
        }
        phs_sets->phs_match_cnt[event_param->phrase_id]++;

        sap_printf("\n--------------------------------------------------");
        sap_printf("cmdid %u\n", event_param->phrase_id);
        for (i = 0; i < phs_sets->phs_count; i++) {
            if (event_param->phrase_id == phs_sets->phrase[i].id) {
                sap_printf("cmd: <%s>, cnt: %u\n", phs_sets->phrase[i].name,
                           phs_sets->phs_match_cnt[phs_sets->phrase[i].id]);
            } else {
                sap_printf("cmd: [%s], cnt: %u\n", phs_sets->phrase[i].name,
                           phs_sets->phs_match_cnt[phs_sets->phrase[i].id]);
            }
        }
        sap_printf("\n--------------------------------------------------");

        lpwk_fsm_next_fun(sea_inst->type, sea_inst->cmd_opt->level, vad_inst, ai_inst, sea_inst);
    } else if (sea_event == UAPI_SEA_EVENT_VAD_END) {
#if (SAP_CHIP_TYPE == brandy)
        uapi_set_hifi_mode(HIFI_LPM_CLK_LEVEL1);
#endif /* SAP_CHIP_TYPE */
        if (ai_inst->stop != TD_NULL) {
            ai_inst->stop(ai_inst->h_mod);
        }
        if (sea_inst->stop != TD_NULL) {
            sea_inst->stop(sea_inst->h_mod);
        }
        lpwk_fsm_next_fun(sea_inst->type, sea_inst->cmd_opt->level, vad_inst, ai_inst, sea_inst);
    }

    audio_unused(sea);
    return EXT_SUCCESS;
}

static td_void adp_data_proc(td_void *args)
{
    td_s32 ret;
    uapi_audio_frame frame;
    lpwk_svc_inst *inst = (lpwk_svc_inst *)args;

    while (inst->task_enable) {
        ret = uapi_adp_acquire_frame(inst->h_adp, &frame);
        if (ret != EXT_SUCCESS) {
            sap_msleep(THREAD_SLEEP_10MS); /*   */
            continue;
        }

        lpwk_save_frame(inst, &frame);

        (td_void)uapi_adp_release_frame(inst->h_adp, &frame);
    }

    athread_set_exit(inst->h_task, TD_TRUE);
}

static td_void vad_reset_proc(td_void *args)
{
    td_s32 ret = EXT_SUCCESS;
    lpwk_svc_inst *inst = (lpwk_svc_inst *)args;

    while (inst->task_enable) {
        sleep(LPWK_VAD_RESET_INTERVAL);
        if (inst->cmd_opt->level == LPWK_LEVEL_1) {
            if (inst->reset != TD_NULL) {
                ret = inst->reset(inst->h_mod);
            }
        }
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(inst->reset, ret);
        }
    }

    athread_set_exit(inst->h_task, TD_TRUE);
}

static td_u32 adp_save_frame(FILE *h_file, const uapi_audio_frame *frame)
{
    td_u32 ret;
    if (h_file == TD_NULL) {
        return 0;
    }

    ret = fwrite((td_void *)frame->bits_buffer, 1, frame->bits_bytes, h_file);
    return ret;
}

static td_s32 sea_attach_output(td_handle sea, td_handle output)
{
    return uapi_sea_attach_output(sea, UAPI_SEA_OUTPUT_ASR_SRC, output);
}

static td_s32 sea_detach_output(td_handle sea, td_handle output)
{
    return uapi_sea_detach_output(sea, UAPI_SEA_OUTPUT_ASR_SRC, output);
}

static td_void opt_set_ai_attr(lpwk_cmd_opt *cmd_opt, uapi_ai_attr *ai_attr)
{
    (td_void)memset_s(ai_attr, sizeof(*ai_attr), 0, sizeof(*ai_attr));
    uapi_ai_get_default_attr(cmd_opt->ai_port, ai_attr);

    (td_void)memcpy_s(&ai_attr->pcm_attr, sizeof(ai_attr->pcm_attr),
                      &cmd_opt->pcm_attr, sizeof(cmd_opt->pcm_attr));
    ai_attr->ref_attr.enable = TD_FALSE;
    if ((cmd_opt->level == LPWK_LEVEL_3) || (cmd_opt->vad_type == LPWK_VAD_MAD)) {
        ai_attr->vad_attr.enable = TD_TRUE;
    } else {
        ai_attr->vad_attr.enable = TD_FALSE;
    }

    switch (cmd_opt->ai_port) {
        case UAPI_AI_PORT_PDM0:
        case UAPI_AI_PORT_PDM1:
            ai_attr->port_attr.pdm.rx_type = UAPI_AI_RX_MAD;
            ai_attr->port_attr.pdm.i2s_attr.channels = ai_attr->pcm_attr.channels;
            ai_attr->port_attr.pdm.i2s_attr.bit_depth = ai_attr->pcm_attr.bit_depth;
            break;

        case UAPI_AI_PORT_ADC0:
        case UAPI_AI_PORT_LPADC0:
            ai_attr->port_attr.adc.rx_type = UAPI_AI_RX_MAD;
            break;

        case UAPI_AI_PORT_I2S0:
        case UAPI_AI_PORT_I2S1:
            ai_attr->port_attr.i2s.i2s_attr.channels = ai_attr->pcm_attr.channels;
            ai_attr->port_attr.i2s.i2s_attr.bit_depth = ai_attr->pcm_attr.bit_depth;
            break;

        default:
            break;
    }
}

static td_void opt_set_sea_attr(lpwk_cmd_opt *cmd_opt, uapi_sea_eng_sel *eng_sel, uapi_sea_attr *sea_attr)
{
    (td_void)memset_s(eng_sel, sizeof(*eng_sel), 0, sizeof(*eng_sel));

    if (cmd_opt->afe_id != UAPI_SEA_LIB_NULL) {
        eng_sel->afe.type = UAPI_SEA_AFE_SEE;
        eng_sel->afe.lib_id = cmd_opt->afe_id;
    }

    if (cmd_opt->aai_id != UAPI_SEA_LIB_NULL) {
        eng_sel->aai[0].type = UAPI_SEA_AAI_KWS;
        eng_sel->aai[0].lib_id = cmd_opt->aai_id;
    }

    uapi_sea_get_default_attr(eng_sel, sea_attr);
    (td_void)memcpy_s(&sea_attr->in_pcm, sizeof(sea_attr->in_pcm),
                      &cmd_opt->pcm_attr, sizeof(cmd_opt->pcm_attr));
    sea_attr->ref_pcm.channels = 0;
}

static td_void opt_load_sea_engine(uapi_sea_lib_id afe_id, uapi_sea_lib_id aai_id)
{
    td_s32 ret;

    if (afe_id != UAPI_SEA_LIB_NULL) {
        ret = uapi_sea_load_engine(afe_id, "imedia_2mic");
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_sea_load_engine, ret);
        }
    }

    if (aai_id != UAPI_SEA_LIB_NULL) {
        ret = uapi_sea_load_engine(aai_id, "imedia_keyword");
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_sea_load_engine, ret);
        }
    }
}

static uapi_ai_port opt_get_ai_port(const td_char *port_name)
{
    td_u32 i;
    const struct {
        const td_char *name;
        uapi_ai_port ai_port;
    } ai_port_name[] = {
        {"pdm0", UAPI_AI_PORT_PDM0},
        {"pdm1", UAPI_AI_PORT_PDM1},
        {"i2s0", UAPI_AI_PORT_I2S0},
        {"adc0", UAPI_AI_PORT_ADC0},
        {"lpadc0", UAPI_AI_PORT_LPADC0},
    };

    for (i = 0; i < sizeof(ai_port_name) / sizeof(ai_port_name[0]); i++) {
        if (strcmp(ai_port_name[i].name, port_name) == 0) {
            return ai_port_name[i].ai_port;
        }
    }

    return LPWK_AI_DEF_PORT;
}

static td_void opt_get_sea_lib_id(lpwk_kws_type type, uapi_sea_lib_id *afe_id, uapi_sea_lib_id *aai_id)
{
    switch (type) {
        case LPWK_KWS_3A_GRU:
            *afe_id = UAPI_SEA_LIB_SEE;
            *aai_id = UAPI_SEA_LIB_KWS;
            break;
        case LPWK_KWS_3A_ANPU:
            *afe_id = UAPI_SEA_LIB_SEE;
            *aai_id = UAPI_SEA_LIB_ASR;
            break;
        case LPWK_KWS_GRU:
            *afe_id = UAPI_SEA_LIB_NULL;
            *aai_id = UAPI_SEA_LIB_KWS;
            break;
        case LPWK_KWS_ANPU:
            *afe_id = UAPI_SEA_LIB_NULL;
            *aai_id = UAPI_SEA_LIB_ASR;
            break;
        default:
            *afe_id = UAPI_SEA_LIB_SEE;
            *aai_id = UAPI_SEA_LIB_KWS;
            break;
    }
}

static td_s32 lpwk_sys_init(td_void)
{
    td_s32 ret;

    ret = uapi_adp_init();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_init, ret);
        return ret;
    }

    ret = uapi_vad_init();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_vad_init, ret);
        return ret;
    }

    ret = uapi_ai_init();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_init, ret);
        return ret;
    }

    ret = uapi_sea_init();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_init, ret);
        return ret;
    }

    return EXT_SUCCESS;
}

static td_void lpwk_sys_deinit(uapi_sea_lib_id afe_id, uapi_sea_lib_id aai_id)
{
    td_s32 ret;

    if (aai_id != UAPI_SEA_LIB_NULL) {
        ret = uapi_sea_unload_engine(aai_id, "imedia_keyword");
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_sea_unload_engine, ret);
        }
    }

    if (aai_id != UAPI_SEA_LIB_NULL) {
        ret = uapi_sea_unload_engine(afe_id, "imedia_2mic");
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_sea_unload_engine, ret);
        }
    }

    ret = uapi_sea_deinit();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_deinit, ret);
    }

    ret = uapi_vad_deinit();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_vad_deinit, ret);
    }

    ret = uapi_ai_deinit();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_deinit, ret);
    }

    ret = uapi_adp_deinit();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_deinit, ret);
    }
}

static td_s32 lpwk_open_file(lpwk_svc_inst *inst, td_char *dir)
{
    td_s32 ret;
    td_char *dir_path = TD_NULL;
    td_char file_path[FILE_PATH_LEN];
    uapi_audio_pcm_format *pcm_attr = &inst->out_pcm;

    if (dir == TD_NULL) {
        dir_path = FILE_STREAM_PATH;
    } else {
        dir_path = dir;
    }

    ret = snprintf_s(file_path, FILE_PATH_LEN - 1, FILE_PATH_LEN - 1,
                     "%s/%s_%uk_%ubit_%uch.pcm", dir_path, inst->task_name,
                     pcm_attr->sample_rate / HZ_NUM_PER_KHZ,
                     pcm_attr->bit_depth, pcm_attr->channels);
    if (ret < 0) {
        sap_err_log_fun(snprintf_s, ret);
        return EXT_FAILURE;
    }

    inst->h_file = fopen(file_path, "a+");
    if (inst->h_file == TD_NULL) {
        sap_printf("open %s failed", file_path);
        return EXT_FAILURE;
    }

    inst->save_frame = adp_save_frame;
    inst->file_limit_size = (pcm_attr->channels * pcm_attr->bit_depth / BITS_NUM_PER_BYTE) *
                            (pcm_attr->sample_rate * LPWK_STREAM_MAX_DUR);
    return EXT_SUCCESS;
}

static td_void lpwk_save_frame(lpwk_svc_inst *inst, const uapi_audio_frame *frame)
{
    inst->file_size += frame->bits_bytes;
    if (inst->file_size >= inst->file_limit_size) {
        return;
    }

    if (inst->save_frame(inst->h_file, frame) == 0) {
        inst->file_size = inst->file_limit_size;
    }
}

static td_void lpwk_opt_init(lpwk_cmd_opt *lpwk_opt)
{
    (td_void)memset_s(lpwk_opt, sizeof(*lpwk_opt), 0, sizeof(*lpwk_opt));

    lpwk_opt->level = LPWK_LEVEL_3;
    lpwk_opt->vad_type = LPWK_VAD_MAD;
    lpwk_opt->kws_type = LPWK_KWS_3A_GRU;
    lpwk_opt->ai_port = LPWK_AI_DEF_PORT;
    lpwk_opt->pcm_attr.channels = UAPI_AUDIO_CHANNEL_1;
    lpwk_opt->pcm_attr.bit_depth = UAPI_AUDIO_BIT_DEPTH_16;
    lpwk_opt->pcm_attr.sample_rate = UAPI_AUDIO_SAMPLE_RATE_16K;
    lpwk_opt->pcm_attr.sample_per_frame = lpwk_opt->pcm_attr.sample_rate / 100; /* 100 --> 10ms */
    lpwk_opt->export_data = TD_FALSE;
}

static td_void lpwk_inst_init(lpwk_svc_inst *inst, const lpwk_cmd_opt *lpwk_opt, const td_char *task_name)
{
    td_s32 ret;

    (td_void)memset_s(inst, sizeof(*inst), 0, sizeof(*inst));
    inst->cmd_opt = (lpwk_cmd_opt *)lpwk_opt;
    inst->out_pcm.channels = UAPI_AUDIO_CHANNEL_1;
    inst->out_pcm.bit_depth = UAPI_AUDIO_BIT_DEPTH_16;
    inst->out_pcm.sample_rate = UAPI_AUDIO_SAMPLE_RATE_16K;
    inst->out_pcm.sample_per_frame = inst->out_pcm.sample_rate / 100; /* 100 --> 10ms */

    if (task_name == TD_NULL) {
        return;
    }

    ret = strncpy_s(inst->task_name, sizeof(inst->task_name), task_name, strlen(task_name));
    if (ret != EOK) {
        sap_err_log_fun(strncpy_s, ret);
    }
}

static td_s32 lpwk_open_vad(lpwk_svc_inst *inst, lpwk_event_proc eproc, td_void *context)
{
    td_s32 ret;
    uapi_vad_attr vad_attr;

    uapi_vad_get_default_attr(&vad_attr);
    ret = uapi_vad_open(&inst->h_mod, &vad_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_vad_open, ret);
        return ret;
    }

    (td_void)uapi_vad_register_event_proc(inst->h_mod, (uapi_vad_event_proc)eproc, context);
    (td_void)memset_s(&inst->out_pcm, sizeof(inst->out_pcm), 0, sizeof(inst->out_pcm));

    inst->close = uapi_vad_close;
    inst->start = TD_NULL;
    inst->stop  = TD_NULL;
    inst->reset = uapi_vad_reset;
    inst->attach = TD_NULL;
    inst->detach = TD_NULL;
    return EXT_SUCCESS;
}

static td_s32 lpwk_open_ai(lpwk_svc_inst *inst, lpwk_event_proc eproc, td_void *context)
{
    td_s32 ret;
    uapi_ai_attr ai_attr;

    opt_set_ai_attr(inst->cmd_opt, &ai_attr);
    ret = uapi_ai_open(&inst->h_mod, inst->cmd_opt->ai_port, &ai_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_open, ret);
        return ret;
    }

    (td_void)uapi_ai_register_event_proc(inst->h_mod, (uapi_ai_event_proc)eproc, context);
    (td_void)memcpy_s(&inst->out_pcm, sizeof(inst->out_pcm), &ai_attr.pcm_attr, sizeof(ai_attr.pcm_attr));

    inst->close = uapi_ai_close;
    inst->start = uapi_ai_start;
    inst->stop  = uapi_ai_stop;
    inst->reset = uapi_ai_reset_vad;
    inst->attach = uapi_ai_attach_output;
    inst->detach = uapi_ai_detach_output;
    return EXT_SUCCESS;
}

static td_s32 lpwk_open_sea(lpwk_svc_inst *inst, lpwk_event_proc eproc, td_void *context)
{
    td_s32 ret;
    uapi_sea_attr sea_attr;
    uapi_sea_eng_sel sea_eng;
    uapi_sea_item_sets item_sets;
    lpwk_phs_sets *phs_sets = &g_lpwk_phs_sets;

    opt_load_sea_engine(inst->cmd_opt->afe_id, inst->cmd_opt->aai_id);
    opt_set_sea_attr(inst->cmd_opt, &sea_eng, &sea_attr);
    ret = uapi_sea_create(&inst->h_mod, &sea_eng, &sea_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_create, ret);
        return ret;
    }

    (td_void)uapi_sea_register_event_proc(inst->h_mod, (uapi_sea_event_proc)eproc, context);
    (td_void)memcpy_s(&inst->out_pcm, sizeof(inst->out_pcm),
                      &sea_attr.out_pcm[UAPI_SEA_OUTPUT_ASR_SRC],
                      sizeof(sea_attr.out_pcm[UAPI_SEA_OUTPUT_ASR_SRC]));

    item_sets.size = LPWK_PHRASE_COUNT;
    item_sets.array = phs_sets->phrase;
    ret = uapi_sea_get_phrase_sets(inst->h_mod, &item_sets);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_get_phrase_sets, ret);
    }
    phs_sets->phs_count = item_sets.count;

    inst->close = uapi_sea_destroy;
    inst->start = uapi_sea_start;
    inst->stop  = uapi_sea_stop;
    inst->reset = TD_NULL;
    inst->attach = sea_attach_output;
    inst->detach = sea_detach_output;
    return EXT_SUCCESS;
}

static td_s32 lpwk_open_inst(lpwk_svc_inst *inst, lpwk_event_proc eproc, td_void *context)
{
    td_s32 ret;
    uapi_adp_attr adp_attr;

    if (strcmp(inst->task_name, LPWK_VAD_TASK_NAME) == 0) {
        inst->type  = LPWK_SVC_VAD;
        ret = lpwk_open_vad(inst, eproc, context);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(lpwk_open_vad, ret);
            return ret;
        }
    } else if (strcmp(inst->task_name, LPWK_AI_TASK_NAME) == 0) {
        inst->type  = LPWK_SVC_AI;
        ret = lpwk_open_ai(inst, eproc, context);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(lpwk_open_ai, ret);
            return ret;
        }
    } else if (strcmp(inst->task_name, LPWK_SEA_TASK_NAME) == 0) {
        inst->type  = LPWK_SVC_SEA;
        ret = lpwk_open_sea(inst, eproc, context);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(lpwk_open_sea, ret);
            return ret;
        }
    } else {
        sap_err_log_ret(EXT_FAILURE);
        return EXT_FAILURE;
    }

    if (inst->cmd_opt->export_data == TD_TRUE) {
        (td_void)memset_s(&adp_attr, sizeof(adp_attr), 0, sizeof(adp_attr));
        ret = uapi_adp_create(&inst->h_adp, &adp_attr);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_adp_create, ret);
            if (inst->close != TD_NULL) {
                inst->close(inst->h_mod);
                inst->h_mod = 0;
            }
            return ret;
        }
    }

    return EXT_SUCCESS;
}

static td_void lpwk_close_inst(lpwk_svc_inst *inst)
{
    if (inst->task_enable) {
        return;
    }

    if (inst->h_mod != 0) {
        if (inst->close != TD_NULL) {
            inst->close(inst->h_mod);
        }
        inst->h_mod = 0;
    }

    if (inst->h_adp != 0) {
        uapi_adp_destroy(inst->h_adp);
        inst->h_adp = 0;
    }
}

static td_void lpwk_start_task(lpwk_svc_inst *inst)
{
    td_s32 ret;
    athread_attr task_attr;

    (td_void)memset_s(&task_attr, sizeof(task_attr), 0, sizeof(task_attr));
    task_attr.priority = ATHREAD_PRIORITY_NORMAL;
    task_attr.stack_size = 0x1000; /* 4k */
    task_attr.name = inst->task_name;

    if (inst->cmd_opt->level == LPWK_LEVEL_1) { /* Level1 no data export */
        inst->task_enable = TD_TRUE;
        ret = athread_create(&inst->h_task, vad_reset_proc, (td_void *)inst, &task_attr);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(athread_create, ret);
        }
    } else { /* L2&L3 support data export */
        if (inst->cmd_opt->export_data == TD_TRUE) {
            ret = lpwk_open_file(inst, TD_NULL);
            if (ret != EXT_SUCCESS) {
                sap_err_log_fun(lpwk_open_file, ret);
                return;
            }

            if (inst->attach == TD_NULL) {
                return;
            }

            ret = inst->attach(inst->h_mod, inst->h_adp);
            if (ret != EXT_SUCCESS) {
                sap_err_log_fun(inst->attach, ret);
                return;
            }

            inst->task_enable = TD_TRUE;
            ret = athread_create(&inst->h_task, adp_data_proc, (td_void *)inst, &task_attr);
            if (ret != EXT_SUCCESS) {
                sap_err_log_fun(athread_create, ret);
            }
        }
    }
}

static td_void lpwk_stop_task(lpwk_svc_inst *inst)
{
    td_s32 ret;

    if (inst->stop != TD_NULL) {
        ret = inst->stop(inst->h_mod);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(inst->stop, ret);
        }
    }

    /* Destory Task to stop ADP instance reading pcm data */
    inst->task_enable = TD_FALSE;
    if (inst->h_task != TD_NULL) {
        athread_exit(inst->h_task);
        inst->h_task = TD_NULL;
    }

    /* Destory Task to stop ADP instance reading pcm data */
    if (inst->cmd_opt->export_data == TD_TRUE) {
        if (inst->detach != TD_NULL) {
            ret = inst->detach(inst->h_mod, inst->h_adp);
            if (ret != EXT_SUCCESS) {
                sap_err_log_fun(inst->detach, ret);
            }
        }
        if (inst->h_file != TD_NULL) {
            fclose(inst->h_file);
            inst->h_file = TD_NULL;
        }
    }
}

static td_s32 lpwk_open_kws_inst(lpwk_svc_inst *ai_inst, lpwk_svc_inst *sea_inst)
{
    td_s32 ret;

    /* IPC wakeup DSP from Deep Standby */

    ret = lpwk_open_inst(ai_inst, ai_event_proc, (td_void *)ai_inst);
    if (ret != EXT_SUCCESS) {
        return ret;
    }

    ret = lpwk_open_inst(sea_inst, sea_event_proc, (td_void *)sea_inst);
    if (ret != EXT_SUCCESS) {
        lpwk_close_inst(ai_inst);
        return ret;
    }

    if (ai_inst->attach != TD_NULL) {
        ret = ai_inst->attach(ai_inst->h_mod, sea_inst->h_mod);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(ai_inst->attach, ret);
        }
    }

    lpwk_start_task(sea_inst);
    lpwk_start_task(ai_inst);

    return EXT_SUCCESS;
}

static td_s32 lpwk_close_kws_inst(lpwk_svc_inst *ai_inst, lpwk_svc_inst *sea_inst)
{
    td_s32 ret;

    lpwk_stop_task(sea_inst);
    lpwk_stop_task(ai_inst);

    if (ai_inst->stop != TD_NULL) {
        ai_inst->stop(ai_inst->h_mod);
    }
    if (sea_inst->stop != TD_NULL) {
        sea_inst->stop(sea_inst->h_mod);
    }

    if (ai_inst->detach != TD_NULL) {
        ret = ai_inst->detach(ai_inst->h_mod, sea_inst->h_mod);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(ai_inst->attach, ret);
        }
    }

    lpwk_close_inst(sea_inst);
    lpwk_close_inst(ai_inst);

    /* DSP auto enter Deep Standby */
    return EXT_SUCCESS;
}

static td_void sample_lpwk_usage(const td_char *name)
{
    sap_printf(" usage: %s -l [level] -p [port] -v [vad_type] -w [kws_type]\n", name);
    sap_printf("        -h     show this usage message and abort\n");
    sap_printf("        -l     level of lowper wakeup(range: 1~3, default: 3)\n"
           "        -v     specify vad type for level 1&2(0:avad, 1:mad, default:0)\n"
           "        -w     specify kws type for level 2&3(0:3a+gru, 1:3a+anpu, 2:gru, 3:anpu, defualt:0)\n"
           "        -p     specify ai port(mad0, adc0, lpadc0, i2s0, i2s1)\n"
           "        -c     channels of ai port(default:1)\n"
           "        -b     bit depth of ai port(default:16)\n"
           "        -s     samplerate of ai port(default:16000)\n"
           "        -d     export data(0:diable, 1:enable, default:0)\n"
           "\n");
    sap_printf(" examples:\n");
    sap_printf(" %s -l 3 -p pdm0\n", name);
    sap_printf(" %s -l 2 -p pdm0 -v 0 -w 0\n", name);
    sap_printf(" %s -l 2 -p pdm0 -v 1 -w 0\n", name);
    sap_printf(" %s -l 1 -p pdm0 -v 0\n", name);
    sap_printf(" %s -l 1 -p pdm0 -v 1\n", name);
    sap_printf(" %s -l 3 -p adc0\n", name);
    sap_printf(" %s -l 2 -p adc0 -v 0 -w 0\n", name);
    sap_printf(" %s -l 2 -p adc0 -v 1 -w 0\n", name);
    sap_printf(" %s -l 1 -p adc0 -v 0\n", name);
    sap_printf(" %s -l 1 -p adc0 -v 1\n", name);
    sap_printf("\n");
}

static td_s32 lpwk_parse_opt(td_s32 opt, const td_char *opt_arg, lpwk_cmd_opt *cmd_opt)
{
    if (opt == 'h') {
        return EXT_FAILURE;
    }

    if (opt_arg == TD_NULL) {
        sap_printf("invalid option -- '%c'\n", (td_char)opt);
        return EXT_FAILURE;
    }

    switch (opt) {
        case 'l':
            cmd_opt->level = (lpwk_level_type)(strtol(opt_arg, TD_NULL, 0) - 1);
            break;
        case 'v':
            cmd_opt->vad_type = (lpwk_vad_type)strtol(opt_arg, TD_NULL, 0);
            break;
        case 'w':
            cmd_opt->kws_type = (lpwk_kws_type)strtol(opt_arg, TD_NULL, 0);
            break;
        case 'p':
            cmd_opt->ai_port = opt_get_ai_port(opt_arg);
            break;
        case 'c':
            cmd_opt->pcm_attr.channels = (td_u32)strtol(opt_arg, TD_NULL, 0);
            break;
        case 'b':
            cmd_opt->pcm_attr.bit_depth = (td_u32)strtol(opt_arg, TD_NULL, 0);
            break;
        case 's':
            cmd_opt->pcm_attr.sample_rate = (td_u32)strtol(opt_arg, TD_NULL, 0);
            break;
        case 'd':
            cmd_opt->export_data = (strtol(opt_arg, TD_NULL, 0) == 0 ? TD_FALSE : TD_TRUE);
            break;
        default:
            sap_printf("invalid command line\n");
            return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

static td_s32 lpwk_parse_cmd_line(td_s32 argc, td_char *argv[], lpwk_cmd_opt *cmd_opt)
{
    td_s32 opt;

    if (argv == TD_NULL) {
        return EXT_FAILURE;
    }

#if (SAP_CHIP_TYPE != socmn1)
    optind = 0;
    opterr = 0;
    while (TD_TRUE) {
        opt = getopt(argc, argv, "l:v:w:p:c:b:s:d:h");
        if (opt == -1) {
            break;
        }

        if (lpwk_parse_opt(opt, optarg, cmd_opt) != EXT_SUCCESS) {
            sample_lpwk_usage(argv[0]);
            return EXT_FAILURE;
        }
    }
#endif

    if ((cmd_opt->level >= LPWK_LEVEL_MAX || cmd_opt->level < LPWK_LEVEL_MIN) ||
        (cmd_opt->vad_type >= LPWK_VAD_MAX || cmd_opt->vad_type < LPWK_VAD_MIN) ||
        (cmd_opt->kws_type >= LPWK_KWS_MAX || cmd_opt->kws_type < LPWK_KWS_MIN)) {
        sample_lpwk_usage(argv[0]);
        return EXT_FAILURE;
    }

    opt_get_sea_lib_id(cmd_opt->kws_type, &cmd_opt->afe_id, &cmd_opt->aai_id);
    cmd_opt->pcm_attr.sample_per_frame = cmd_opt->pcm_attr.sample_rate / 100; /* 100 --> 10ms */
    return EXT_SUCCESS;
}

static td_s32 sample_lpwk_entry(td_s32 argc, td_char *argv[])
{
    td_s32 ret;
    lpwk_cmd_opt *cmd_opt = &g_lpwk_cmd_opt;
    lpwk_phs_sets *phs_sets = &g_lpwk_phs_sets;
    lpwk_svc_inst *vad_inst = &g_lpwk_inst[LPWK_SVC_VAD];
    lpwk_svc_inst *ai_inst = &g_lpwk_inst[LPWK_SVC_AI];
    lpwk_svc_inst *sea_inst = &g_lpwk_inst[LPWK_SVC_SEA];

#if (SAP_CHIP_TYPE == brandy)
    uapi_set_hifi_mode(HIFI_LPM_CLK_LEVEL1);
#endif /* SAP_CHIP_TYPE */

    (td_void)memset_s(phs_sets, sizeof(*phs_sets), 0, sizeof(*phs_sets));
    lpwk_opt_init(cmd_opt);
    lpwk_inst_init(vad_inst, cmd_opt, LPWK_VAD_TASK_NAME);
    lpwk_inst_init(ai_inst, cmd_opt, LPWK_AI_TASK_NAME);
    lpwk_inst_init(sea_inst, cmd_opt, LPWK_SEA_TASK_NAME);
    g_lpwk_inst_inited = TD_TRUE;

    ret = lpwk_parse_cmd_line(argc, argv, cmd_opt);
    if (ret != EXT_SUCCESS) {
        return ret;
    }
    ret = lpwk_sys_init();
    if (ret != EXT_SUCCESS) {
        return ret;
    }
    ret = lpwk_fsm_start(cmd_opt->level, vad_inst, ai_inst, sea_inst);
    if (ret != EXT_SUCCESS) {
        lpwk_sys_deinit(cmd_opt->afe_id, cmd_opt->aai_id);
        return ret;
    }

    return EXT_SUCCESS;
}

static td_s32 sample_lpwk_exit(td_void)
{
    td_s32 ret;
    lpwk_cmd_opt *cmd_opt = &g_lpwk_cmd_opt;
    lpwk_svc_inst *vad_inst = &g_lpwk_inst[LPWK_SVC_VAD];
    lpwk_svc_inst *ai_inst = &g_lpwk_inst[LPWK_SVC_AI];
    lpwk_svc_inst *sea_inst = &g_lpwk_inst[LPWK_SVC_SEA];

    if (g_lpwk_inst_inited == TD_FALSE) {
        return EXT_SUCCESS;
    }

    lpwk_stop_task(sea_inst);
    lpwk_stop_task(ai_inst);
    lpwk_stop_task(vad_inst);

    /* Detach SEA instance from AI instance */
    if ((ai_inst->detach != TD_NULL) && (ai_inst->h_mod != 0) && (sea_inst->h_mod != 0)) {
        ret = ai_inst->detach(ai_inst->h_mod, sea_inst->h_mod);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(ai_inst->detach, ret);
        }
    }

    lpwk_close_inst(sea_inst);
    lpwk_close_inst(ai_inst);
    lpwk_close_inst(vad_inst);

    lpwk_sys_deinit(cmd_opt->afe_id, cmd_opt->aai_id);
    g_lpwk_inst_inited = TD_FALSE;
    return EXT_SUCCESS;
}

td_s32 sample_lpwk(td_s32 argc, td_char *argv[])
{
    if (argc <= 1) {
        sample_lpwk_usage(argv[0]);
        return EXT_SUCCESS;
    }

    if (strcmp(argv[1], "q") == 0) {
        return sample_lpwk_exit();
    } else {
        return sample_lpwk_entry(argc, argv);
    }

    return EXT_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

/*
 * Copyright (c) CompanyNameMagicTag 2019-2022. All rights reserved.
 * Description: ai sample
 * Author: audio
 * Create: 2019-09-17
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "thread_os.h"
#include "osal_list.h"
#include "soc_uapi_adp.h"
#include "soc_uapi_ai.h"
#include "soc_uapi_sea.h"
#include "sample_audio_api.h"
#include "sample_audio_utils.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    td_bool task_enable;
    athread_handle task;
    td_bool sea_enable;
    td_bool mute;

    td_handle h_adp;
    td_handle h_aenc;
    td_handle h_ai;
    td_handle h_sea;
    td_handle h_player;
    uapi_ai_port ai_port;
    uapi_ai_gain volume;
    uapi_acodec_id acodec_id;
    uapi_audio_pcm_format pcm_format;
    struct osal_list_head node;
} sample_ai_aenc_inst;

static OSAL_LIST_HEAD(g_sample_ai_aenc_list);

static sample_ai_aenc_inst *sample_ai_aenc_alloc_inst(td_void)
{
    sample_ai_aenc_inst *inst = (sample_ai_aenc_inst *)malloc(sizeof(sample_ai_aenc_inst));
    if (inst == TD_NULL) {
        return TD_NULL;
    }

    (td_void)memset_s(inst, sizeof(*inst), 0, sizeof(sample_ai_aenc_inst));

    osal_list_add_tail(&inst->node, &g_sample_ai_aenc_list);
    return inst;
}

static td_void sample_ai_aenc_free_inst(sample_ai_aenc_inst *inst)
{
    if (inst == TD_NULL) {
        return;
    }

    osal_list_del(&inst->node);
    free(inst);
}

static sample_ai_aenc_inst *sample_ai_aenc_get_inst(td_void)
{
    if (osal_list_empty(&g_sample_ai_aenc_list) != TD_FALSE) {
        return TD_NULL;
    }

    return osal_list_first_entry(&g_sample_ai_aenc_list, sample_ai_aenc_inst, node);
}

static td_void sample_ai_aenc_thread(td_void *args)
{
    td_s32 ret;
    uapi_stream_buf stream;
    sample_ai_aenc_inst *inst = (sample_ai_aenc_inst *)args;

    if (inst == TD_NULL) {
        sap_err_log_info("ai_aenc_inst is null");
        return;
    }

    while (inst->task_enable) {
        ret = uapi_adp_acquire_stream(inst->h_adp, &stream);
        if (ret != EXT_SUCCESS) {
            sap_msleep(THREAD_SLEEP_10MS); /*   */
            continue;
        }

        while (inst->task_enable) {
            ret = uapi_adp_send_stream(inst->h_player, &stream);
            if (ret != EXT_SUCCESS) {
                sap_msleep(THREAD_SLEEP_10MS); /*   */
                continue;
            }
            break;
        }

        ret = uapi_adp_release_stream(inst->h_adp, &stream);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_adp_release_stream, ret);
        }
    }

    athread_set_exit(inst->task, TD_TRUE);
}

static td_s32 sample_ai_aenc_sys_init(td_void)
{
    td_s32 ret;

    ret = uapi_adp_init();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_init, ret);
        return ret;
    }

    ret = uapi_ai_init();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_init, ret);
        goto out0;
    }

    ret = uapi_sea_init();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_init, ret);
        goto out1;
    }

    ret = uapi_sea_load_engine(UAPI_SEA_LIB_SEE, "imedia_2mic");
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_load_engine, ret);
        goto out2;
    }

    ret = uapi_aenc_init();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_aenc_init, ret);
        goto out3;
    }

    return EXT_SUCCESS;
out3:
    (td_void)uapi_sea_unload_engine(UAPI_SEA_LIB_SEE, "imedia_2mic");
out2:
    (td_void)uapi_sea_deinit();
out1:
    (td_void)uapi_ai_deinit();
out0:
    (td_void)uapi_adp_deinit();
    return ret;
}

static td_void sample_ai_aenc_sys_deinit(td_void)
{
    td_s32 ret;

    ret = uapi_aenc_deinit();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_aenc_deinit, ret);
    }

    ret = uapi_sea_unload_engine(UAPI_SEA_LIB_SEE, "imedia_2mic");
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_unload_engine, ret);
    }

    ret = uapi_sea_deinit();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_deinit, ret);
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

static td_void sample_ai_aenc_close_adp(sample_ai_aenc_inst *inst)
{
    if (inst->h_adp != 0) {
        uapi_adp_destroy(inst->h_adp);
        inst->h_adp = 0;
    }
}

static td_s32 sample_ai_aenc_open_adp(sample_ai_aenc_inst *inst)
{
    td_s32 ret;
    uapi_adp_attr adp_attr;

    inst->h_adp = 0;

    ret = uapi_adp_get_def_attr(&adp_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_get_def_attr, ret);
        return ret;
    }

    ret = uapi_adp_create(&inst->h_adp, &adp_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_create, ret);
        return ret;
    }

    return EXT_SUCCESS;
}

static td_void sample_ai_aenc_close_aenc(sample_ai_aenc_inst *inst)
{
    td_s32 ret;

    if (inst->h_aenc == 0) {
        return;
    }

    ret = uapi_aenc_detach_output(inst->h_aenc, inst->h_adp);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_aenc_detach_output, ret);
    }

    ret = uapi_aenc_destroy(inst->h_aenc);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_aenc_destroy, ret);
    }

    inst->h_aenc = 0;
}

static td_s32 sample_ai_aenc_open_aenc(sample_ai_aenc_inst *inst)
{
    td_s32 ret;
    uapi_aenc_attr aenc_attr;

    inst->h_aenc = 0;
    ret = uapi_aenc_create(&inst->h_aenc);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_aenc_create, ret);
        return ret;
    }

    ret = uapi_aenc_get_attr(inst->h_aenc, &aenc_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_aenc_get_attr, ret);
        goto out;
    }

    aenc_attr.codec_id = inst->acodec_id;
    aenc_attr.param.interleaved = TD_TRUE;
    aenc_attr.param.channels = inst->pcm_format.channels;
    aenc_attr.param.bit_depth = inst->pcm_format.bit_depth;
    aenc_attr.param.sample_rate = inst->pcm_format.sample_rate;
    aenc_attr.param.samples_per_frame = inst->pcm_format.sample_per_frame;
    aenc_attr.param.private_data = TD_NULL;
    aenc_attr.param.private_data_size = 0;

    ret = uapi_aenc_set_attr(inst->h_aenc, &aenc_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_aenc_set_attr, ret);
        goto out;
    }

    ret = uapi_aenc_attach_output(inst->h_aenc, inst->h_adp);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_aenc_attach_output, ret);
        goto out;
    }

    return EXT_SUCCESS;

out:
    (td_void)uapi_aenc_destroy(inst->h_aenc);
    return ret;
}

static td_void sample_ai_aenc_close_sea(sample_ai_aenc_inst *inst)
{
    td_s32 ret;

    if (inst->h_sea == 0) {
        return;
    }

    ret = uapi_sea_detach_output(inst->h_sea, UAPI_SEA_OUTPUT_ASR_SRC, inst->h_aenc);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_detach_output, ret);
    }

    ret = uapi_sea_destroy(inst->h_sea);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_destroy, ret);
    }

    inst->h_sea = 0;
}

static td_s32 sample_ai_aenc_open_sea(sample_ai_aenc_inst *inst)
{
    td_s32 ret;
    uapi_sea_attr sea_attr;
    uapi_sea_eng_sel sea_eng;
    uapi_sea_afe_attr afe_attr = {
        .type = UAPI_SEA_AFE_SEE,
        .mode = UAPI_SEA_NORM_MODE,
        .u.see.options = UAPI_SEA_AFE_OPT_AGC,
    };

    inst->h_sea = 0;
    if (!inst->sea_enable) {
        return EXT_SUCCESS;
    }

    /* Create SEA instance */
    (td_void)memset_s(&sea_eng, sizeof(sea_eng), 0, sizeof(sea_eng));
    sea_eng.afe.type = UAPI_SEA_AFE_SEE;
    sea_eng.afe.lib_id = UAPI_SEA_LIB_SEE;
    ret = uapi_sea_get_default_attr(&sea_eng, &sea_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_get_default_attr, ret);
        return ret;
    }

    sea_attr.in_pcm.bit_depth = inst->pcm_format.bit_depth;
    sea_attr.in_pcm.channels = inst->pcm_format.channels;
    sea_attr.in_pcm.sample_rate = inst->pcm_format.sample_rate;
    sea_attr.in_pcm.sample_per_frame = inst->pcm_format.sample_per_frame;
    sea_attr.ref_pcm.channels = 0;
    ret = uapi_sea_create(&inst->h_sea, &sea_eng, &sea_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_create, ret);
        return ret;
    }

    ret = uapi_sea_set_afe_attr(inst->h_sea, UAPI_SEA_AFE_SEE, &afe_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_set_afe_attr, ret);
        goto out;
    }

    ret = uapi_sea_attach_output(inst->h_sea, UAPI_SEA_OUTPUT_ASR_SRC, inst->h_aenc);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_attach_output, ret);
        goto out;
    }

    return EXT_SUCCESS;
out:
    (td_void)uapi_sea_destroy(inst->h_sea);
    return ret;
}

static td_void sample_ai_aenc_close_ai(sample_ai_aenc_inst *inst)
{
    td_s32 ret;

    if (inst->h_ai == 0) {
        return;
    }

    if (inst->h_sea != 0) {
        ret = uapi_ai_detach_output(inst->h_ai, inst->h_sea);
    } else {
        ret = uapi_ai_detach_output(inst->h_ai, inst->h_aenc);
    }
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_detach_output, ret);
    }

    ret = uapi_ai_close(inst->h_ai);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_close, ret);
    }

    inst->h_ai = 0;
}

static td_void sample_ai_aenc_set_port_attr(const sample_ai_aenc_inst *inst, uapi_ai_attr *ai_attr)
{
    td_s32 ret;

    ret = memcpy_s(&ai_attr->pcm_attr, sizeof(ai_attr->pcm_attr), &inst->pcm_format,
        sizeof(inst->pcm_format));
    if (ret != EOK) {
        return;
    }

    ai_attr->ref_attr.enable = TD_FALSE;

    switch (inst->ai_port) {
        case UAPI_AI_PORT_PDM0:
        case UAPI_AI_PORT_PDM1:
            ai_attr->port_attr.pdm.i2s_attr.channels = inst->pcm_format.channels;
            ai_attr->port_attr.pdm.i2s_attr.bit_depth = inst->pcm_format.bit_depth;
            break;

        case UAPI_AI_PORT_ADC0:
        case UAPI_AI_PORT_I2S0:
        case UAPI_AI_PORT_I2S1:
            ai_attr->port_attr.i2s.i2s_attr.channels = inst->pcm_format.channels;
            ai_attr->port_attr.i2s.i2s_attr.bit_depth = inst->pcm_format.bit_depth;
            break;
        default:
            break;
    }
}

static td_s32 sample_ai_aenc_open_ai(sample_ai_aenc_inst *inst)
{
    td_s32 ret;
    uapi_ai_attr ai_attr;

    ret = uapi_ai_get_default_attr(inst->ai_port, &ai_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_get_default_attr, ret);
        return ret;
    }

    sample_ai_aenc_set_port_attr(inst, &ai_attr);

    ret = uapi_ai_open(&inst->h_ai, inst->ai_port, &ai_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_open, ret);
        return ret;
    }

    ret = uapi_ai_set_volume(inst->h_ai, &inst->volume);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_set_volume, ret);
        goto out;
    }

    ret = uapi_ai_set_mute(inst->h_ai, inst->mute);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_set_mute, ret);
        goto out;
    }

    if (inst->h_sea != 0) {
        ret = uapi_ai_attach_output(inst->h_ai, inst->h_sea);
    } else {
        ret = uapi_ai_attach_output(inst->h_ai, inst->h_aenc);
    }
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_attach_output, ret);
        goto out;
    }

    return EXT_SUCCESS;
out:
    (td_void)uapi_ai_close(inst->h_ai);
    return ret;
}

static td_void sample_ai_aenc_close_player(sample_ai_aenc_inst *inst)
{
    if (inst->h_player == 0) {
        return;
    }

    sample_esplay_close();
    inst->h_player = 0;
}

static td_s32 sample_ai_aenc_open_player(sample_ai_aenc_inst *inst)
{
    td_s32 ret;
    sample_acodec_arg arg;

    inst->h_player = 0;
    arg.file = TD_NULL;
    arg.acodec_id = inst->acodec_id;
    ret = memcpy_s(&arg.pcm_format, sizeof(arg.pcm_format), &inst->pcm_format, sizeof(inst->pcm_format));
    if (ret != EOK) {
        return ret;
    }

    /* esplay播放器是单独的流程，仅仅提供一个播放句柄h_player，不影响sample_ai_aenc流程 */
    ret = sample_esplay_open(&arg, &inst->h_player, UAPI_SND_AEF_PROFILE_RINGTONE);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_esplay_open, ret);
        return ret;
    }

    return EXT_SUCCESS;
}

static td_void sample_ai_aenc_inst_init(sample_ai_aenc_inst *inst)
{
    inst->ai_port = UAPI_AI_PORT_MAX;

    /* ai/aenc的默认属性 */
    inst->acodec_id = UAPI_ACODEC_ID_MSBC;
    inst->pcm_format.channels = UAPI_AUDIO_CHANNEL_1;
    inst->pcm_format.bit_depth = UAPI_AUDIO_BIT_DEPTH_16;
    inst->pcm_format.sample_rate = UAPI_AUDIO_SAMPLE_RATE_16K;
    inst->pcm_format.sample_per_frame = inst->pcm_format.sample_rate / 100; /* 100 --> 10ms */

    inst->volume.integer = AI_VOLUME_DEFAULT;
    inst->volume.decimal = 0;
}

static td_void sample_ai_aenc_close_inst(sample_ai_aenc_inst *inst)
{
    sample_ai_aenc_close_ai(inst);
    sample_ai_aenc_close_sea(inst);
    sample_ai_aenc_close_aenc(inst);
    sample_ai_aenc_close_adp(inst);
    sample_ai_aenc_sys_deinit();
}

static td_s32 sample_ai_aenc_open_inst(sample_ai_aenc_inst *inst)
{
    td_s32 ret;

    ret = sample_ai_aenc_sys_init();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_ai_aenc_sys_init, ret);
        return ret;
    }

    /*
     * 数据流向 ai -->sea--> aenc --> adp
     * 按照数据流向反方向打开实例
     * 按照数据方向attach output
     */
    ret = sample_ai_aenc_open_adp(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_ai_aenc_open_adp, ret);
        goto out0;
    }

    ret = sample_ai_aenc_open_aenc(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_ai_aenc_open_aenc, ret);
        goto out1;
    }

    ret = sample_ai_aenc_open_sea(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_ai_aenc_open_sea, ret);
        goto out2;
    }

    ret = sample_ai_aenc_open_ai(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_ai_aenc_open_ai, ret);
        goto out3;
    }

    return EXT_SUCCESS;
out3:
    sample_ai_aenc_close_sea(inst);
out2:
    sample_ai_aenc_close_aenc(inst);
out1:
    sample_ai_aenc_close_adp(inst);
out0:
    sample_ai_aenc_sys_deinit();
    return ret;
}

static td_void sample_ai_aenc_stop_inst(sample_ai_aenc_inst *inst)
{
    td_s32 ret;

    if (inst->task != TD_NULL) {
        inst->task_enable = TD_FALSE;
        athread_exit(inst->task);
        inst->task = TD_NULL;
    }

    ret = uapi_aenc_stop(inst->h_aenc);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_aenc_stop, ret);
    }

    if (inst->sea_enable) {
        ret = uapi_sea_stop(inst->h_sea);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_sea_stop, ret);
        }
        sample_gui_aef_delete_handle(inst->h_sea);
    }

    ret = uapi_ai_stop(inst->h_ai);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_stop, ret);
    }
}

static td_s32 sample_ai_aenc_start_inst(sample_ai_aenc_inst *inst)
{
    td_s32 ret;
    athread_attr attr;

    ret = uapi_ai_start(inst->h_ai);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_start, ret);
        return ret;
    }

    if (inst->sea_enable) {
        ret = uapi_sea_start(inst->h_sea);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_sea_start, ret);
            goto out0;
        }

        sample_gui_aef_add_handle(GUI_AEF_SEA, inst->h_sea);
    }

    ret = uapi_aenc_start(inst->h_aenc);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_aenc_start, ret);
        goto out1;
    }

    inst->task_enable = TD_TRUE;
    attr.priority = ATHREAD_PRIORITY_NORMAL;
    attr.stack_size = 0x1000; /* 4k */
    attr.name = "sample_ai_aenc";
    ret = athread_create(&inst->task, sample_ai_aenc_thread, (td_void *)inst, &attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(athread_create, ret);
        goto out2;
    }

    return EXT_SUCCESS;
out2:
    (td_void)uapi_aenc_stop(inst->h_aenc);
out1:
    if (inst->sea_enable) {
        (td_void)uapi_sea_stop(inst->h_sea);
        sample_gui_aef_delete_handle(inst->h_sea);
    }
out0:
    (td_void)uapi_ai_stop(inst->h_ai);
    return ret;
}

static td_void sample_ai_aenc_usage(const td_char *name)
{
    sap_printf(" usage: %s -p [port] -c [channels] -b [bit_depth] -s [sample_rate] -v [volume]\n", name);
    sap_printf("        -h     show this usage message and abort\n");
    sap_printf("        -p     specify ai port(mad0/i2s0/i2s1/adc0)\n"
           "        -t     the type of encoder format(mp3/opus/lc3/l2hc/sbc/msbc)\n"
           "        -c     channels of ai port(default: 1)\n"
           "        -b     bit_depth of ai port(default: 16)\n"
           "        -s     samplerate of ai port(default: 16000)\n"
           "        -v     volume of ai port(default: 0dB)\n"
           "        -m     mute ai port(default: false)\n"
           "\n");

    sap_printf(" examples:\n");
    sap_printf(" %s -p mad0 -c 1 -s 16000\n", name);
    sap_printf(" %s -p i2s1 -c 1 -s 16000\n", name);
    sap_printf(" %s -p adc0 -c 1 -s 16000 -v 30\n", name);
    sap_printf(" %s -p adc0 -c 1 -s 16000 -v 30 -t mp3\n", name);
    sap_printf(" %s -p adc0 -c 1 -s 16000 -v 30 -t opus\n", name);
    sap_printf("\n");
}

static td_void sample_ai_aenc_parse_port(const td_char *port_name, sample_ai_aenc_inst *inst)
{
    td_u32 i;
    const struct {
        const td_char *name;
        uapi_ai_port ai_port;
    } ai_port_name[] = {
        {"mad0", UAPI_AI_PORT_PDM0},
        {"pdm0", UAPI_AI_PORT_PDM0},
        {"pdm1", UAPI_AI_PORT_PDM1},
        {"adc0", UAPI_AI_PORT_ADC0},
        {"adc1", UAPI_AI_PORT_ADC1},
        {"lpadc0", UAPI_AI_PORT_LPADC0},
        {"i2s0", UAPI_AI_PORT_I2S0},
        {"i2s1", UAPI_AI_PORT_I2S1},
        {"i2s2", UAPI_AI_PORT_I2S2},
        {"i2s3", UAPI_AI_PORT_I2S3},
    };

    for (i = 0; i < sizeof(ai_port_name) / sizeof(ai_port_name[0]); i++) {
        if (strcmp(ai_port_name[i].name, port_name) == 0) {
            inst->ai_port = ai_port_name[i].ai_port;
            return;
        }
    }
}

static td_s32 sample_ai_aenc_parse_opt(td_char opt, const td_char *opt_arg, sample_ai_aenc_inst *inst)
{
    if (opt == 'h') {
        return EXT_FAILURE;
    }

    if (opt_arg == TD_NULL) {
        sap_printf("invalid option -- '%c'\n", opt);
        return EXT_FAILURE;
    }

    switch (opt) {
        case 's':
            inst->pcm_format.sample_rate = (td_u32)strtoul(opt_arg, TD_NULL, 0);
            break;
        case 'c':
            inst->pcm_format.channels = (td_u32)strtoul(opt_arg, TD_NULL, 0);
            break;
        case 'b':
            inst->pcm_format.bit_depth = (td_u32)strtoul(opt_arg, TD_NULL, 0);
            break;
        case 'p':
            sample_ai_aenc_parse_port(opt_arg, inst);
            break;
        case 'v':
            inst->volume.integer = (td_s32)strtol(opt_arg, TD_NULL, 0);
            break;
        case 't': /* direct return */
            return sample_audio_get_acodec_id(opt_arg, &inst->acodec_id);
        case 'm':
            inst->mute = TD_TRUE;
            break;
        default:
            sap_printf("invalid command line\n");
            return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

static td_s32 sample_ai_aenc_parse_cmd_line(td_s32 argc, td_char *argv[], sample_ai_aenc_inst *inst)
{
    td_s32 ret;
    td_char opt;
    parg_state arg_parse;

    if (argv == TD_NULL) {
        return EXT_FAILURE;
    }

    parg_init(&arg_parse);

    while (1) {
        ret = parg_getopt(&arg_parse, argc, argv, "s:c:b:p:v:t:hm");
        if (ret == -1) {
            break;
        }

        opt = (td_char)ret;
        if (sample_ai_aenc_parse_opt(opt, arg_parse.optarg, inst) != EXT_SUCCESS) {
            sample_ai_aenc_usage(argv[0]);
            return EXT_FAILURE;
        }
    }

    inst->pcm_format.sample_per_frame = inst->pcm_format.sample_rate / 100; /* 100 --> 10ms */

    if (inst->ai_port == UAPI_AI_PORT_MAX) {
        sample_ai_aenc_usage(argv[0]);
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

static td_s32 sample_ai_aenc_entry(td_s32 argc, td_char *argv[], td_bool sea_enable)
{
    td_s32 ret;
    sample_ai_aenc_inst *inst = sample_ai_aenc_get_inst();
    if (inst != TD_NULL) {
        sap_printf("%s is already running", argv[0]);
        return EXT_FAILURE;
    }

    inst = sample_ai_aenc_alloc_inst();
    if (inst == TD_NULL) {
        sap_printf("sample_ai_aenc_alloc_inst failed");
        return EXT_FAILURE;
    }

    sample_ai_aenc_inst_init(inst);
    inst->sea_enable = sea_enable;

    ret = sample_ai_aenc_parse_cmd_line(argc, argv, inst);
    if (ret != EXT_SUCCESS) {
        goto out0;
    }

    /* 按照ai/aenc的属性创建播放器，用于播放aenc输出的es流 */
    ret = sample_ai_aenc_open_player(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_ai_aenc_open_player, ret);
        goto out0;
    }

    ret = sample_ai_aenc_open_inst(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_ai_aenc_open_inst, ret);
        goto out1;
    }

    ret = sample_ai_aenc_start_inst(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_ai_aenc_start_inst, ret);
        goto out2;
    }

    return EXT_SUCCESS;
out2:
    sample_ai_aenc_close_inst(inst);
out1:
    sample_ai_aenc_close_player(inst);
out0:
    sample_ai_aenc_free_inst(inst);
    return ret;
}

static td_s32 sample_ai_aenc_exit(td_void)
{
    sample_ai_aenc_inst *inst = sample_ai_aenc_get_inst();
    if (inst != TD_NULL) {
        sample_ai_aenc_stop_inst(inst);
        sample_ai_aenc_close_inst(inst);
        sample_ai_aenc_close_player(inst);
        sample_ai_aenc_free_inst(inst);
    }

    return EXT_SUCCESS;
}

td_s32 sample_ai_aenc(td_s32 argc, td_char *argv[])
{
    if (argc <= 1) {
        sample_ai_aenc_usage(argv[0]);
        return EXT_FAILURE;
    }

    if (strcmp(argv[1], "q") == 0) {
        return sample_ai_aenc_exit();
    } else {
        return sample_ai_aenc_entry(argc, argv, TD_FALSE);
    }
}

td_s32 sample_ai_sea_aenc(td_s32 argc, td_char *argv[])
{
    if (argc <= 1) {
        sample_ai_aenc_usage(argv[0]);
        return EXT_FAILURE;
    }

    if (strcmp(argv[1], "q") == 0) {
        return sample_ai_aenc_exit();
    } else {
        return sample_ai_aenc_entry(argc, argv, TD_TRUE);
    }
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

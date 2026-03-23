/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: cell call from mobile via BT.
 * Author: audio
 * Create: 2021-07-12
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "chip_type.h"
#include "audio_type.h"
#include "soc_uapi_adp.h"
#include "soc_uapi_ai.h"
#include "soc_uapi_sea.h"

#include "sample_audio_api.h"
#include "sample_audio_utils.h"
#include "sample_phone.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define WAIT_PCORE_INIT 2 /* 2s */

typedef struct {
    td_handle h_ai;
    td_handle h_sea;
    td_handle h_adp;
    td_handle h_aenc;

    td_handle h_player;

    td_bool sea_enable;
    td_bool ai_ref_enable;

    uapi_ai_port ai_port;
    uapi_acodec_id acodec_id;
    uapi_audio_pcm_format pcm_format;
} sample_phone_inst;

static sample_phone_inst g_phone_inst;

static uapi_ai_port sample_phone_get_test_port(td_void)
{
#if (SAP_CHIP_TYPE == brandy)
    return UAPI_AI_PORT_ADC0;
#elif (SAP_CHIP_TYPE == socmn1)
    return UAPI_AI_PORT_LPADC0;
#else
    return UAPI_AI_PORT_I2S0;
#endif /* SAP_CHIP_TYPE */
}

static td_s32 sample_phone_sys_init(const sample_phone_inst *inst)
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

    if (inst->sea_enable == TD_TRUE) {
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
    }

    ret = uapi_aenc_init();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_aenc_init, ret);
        goto out3;
    }

    return EXT_SUCCESS;

out3:
    if (inst->sea_enable == TD_TRUE) {
        (td_void)uapi_sea_unload_engine(UAPI_SEA_LIB_SEE, "imedia_2mic");
    }

out2:
    if (inst->sea_enable == TD_TRUE) {
        (td_void)uapi_sea_deinit();
    }

out1:
    (td_void)uapi_ai_deinit();

out0:
    (td_void)uapi_adp_deinit();

    return ret;
}

static td_void sample_phone_sys_deinit(const sample_phone_inst *inst)
{
    td_s32 ret;

    ret = uapi_aenc_deinit();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_aenc_deinit, ret);
    }

    if (inst->sea_enable == TD_TRUE) {
        ret = uapi_sea_unload_engine(UAPI_SEA_LIB_SEE, "imedia_2mic");
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_sea_unload_engine, ret);
        }

        ret = uapi_sea_deinit();
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_sea_deinit, ret);
        }
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

static td_void sample_phone_close_adp(sample_phone_inst *inst)
{
    if (inst->h_adp != 0) {
        uapi_adp_destroy(inst->h_adp);
        inst->h_adp = 0;
    }
}

static td_s32 sample_phone_open_adp(sample_phone_inst *inst)
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

static td_void sample_phone_close_aenc(sample_phone_inst *inst)
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

static td_s32 sample_phone_open_aenc(sample_phone_inst *inst)
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
    aenc_attr.max_trans_unit_size = BT_SCO_FRAME_SIZE;
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
    sample_phone_close_aenc(inst);
    return ret;
}

static td_void sample_phone_close_sea(sample_phone_inst *inst)
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

static td_s32 sample_phone_open_sea(sample_phone_inst *inst)
{
    td_s32 ret;
    uapi_sea_attr sea_attr;
    uapi_sea_eng_sel sea_eng;

    /* Create SEA instance */
    (td_void)memset_s(&sea_eng, sizeof(sea_eng), 0, sizeof(sea_eng));
    sea_eng.afe.type = UAPI_SEA_AFE_SEE;
    sea_eng.afe.lib_id = UAPI_SEA_LIB_SEE;
    uapi_sea_get_default_attr(&sea_eng, &sea_attr);

    if (inst->ai_ref_enable == TD_FALSE) {
        sea_attr.ref_pcm.channels = 0;
    }

    ret = uapi_sea_create(&inst->h_sea, &sea_eng, &sea_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_create, ret);
        return ret;
    }

    ret = uapi_sea_attach_output(inst->h_sea, UAPI_SEA_OUTPUT_ASR_SRC, inst->h_aenc);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_attach_output, ret);
        goto out0;
    }

    return EXT_SUCCESS;

out0:
    (td_void)uapi_sea_destroy(inst->h_sea);

    return ret;
}

static td_void sample_phone_close_ai(sample_phone_inst *inst)
{
    td_s32 ret;

    if (inst->h_ai == 0) {
        return;
    }

    if (inst->sea_enable == TD_TRUE) {
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

static td_s32 sample_phone_open_ai(sample_phone_inst *inst)
{
    td_s32 ret;
    uapi_ai_attr ai_attr;

    ret = uapi_ai_get_default_attr(inst->ai_port, &ai_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_get_default_attr, ret);
        return ret;
    }

    ret = memcpy_s(&ai_attr.pcm_attr, sizeof(ai_attr.pcm_attr), &inst->pcm_format, sizeof(inst->pcm_format));
    if (ret != EOK) {
        sap_err_log_fun(memcpy_s, ret);
        return ret;
    }

    ai_attr.pcm_attr.sample_rate = UAPI_AUDIO_SAMPLE_RATE_16K;              /* only 16K is supported */
    ai_attr.pcm_attr.sample_per_frame = ai_attr.pcm_attr.sample_rate / 100; /* 100 --> 10ms */
    ai_attr.port_attr.pdm.i2s_attr.channels = inst->pcm_format.channels;
    ai_attr.port_attr.pdm.i2s_attr.bit_depth = inst->pcm_format.bit_depth;
    if (inst->sea_enable == TD_TRUE) {
        ai_attr.ref_attr.enable = inst->ai_ref_enable;
        ai_attr.ref_attr.port = SND_OUT_PORT_DEFAULT;
    } else {
        ai_attr.ref_attr.enable = TD_FALSE;
    }

    ret = uapi_ai_open(&inst->h_ai, inst->ai_port, &ai_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_open, ret);
        return ret;
    }

    if (inst->sea_enable == TD_TRUE) {
        ret = uapi_ai_attach_output(inst->h_ai, inst->h_sea);
    } else {
        ret = uapi_ai_attach_output(inst->h_ai, inst->h_aenc);
    }
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_attach_output, ret);
        goto out0;
    }

    return EXT_SUCCESS;

out0:
    (td_void)uapi_ai_close(inst->h_ai);

    return ret;
}

static td_void sample_phone_close_player(sample_phone_inst *inst)
{
    if (inst->h_player == 0) {
        return;
    }

    sample_esplay_close();
    inst->h_player = 0;
}

static td_s32 sample_phone_open_player(sample_phone_inst *inst)
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

    /* esplay播放器是单独的流程，仅仅提供一个播放句柄h_player，不影响sample_phone流程 */
    ret = sample_esplay_open(&arg, &inst->h_player, UAPI_SND_AEF_PROFILE_VOIP);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_esplay_open, ret);
        return ret;
    }

    return EXT_SUCCESS;
}

static td_void sample_phone_inst_deinit(sample_phone_inst *inst)
{
    sample_phone_close_player(inst);
}

static td_s32 sample_phone_inst_init(sample_phone_inst *inst)
{
    if (inst->acodec_id == UAPI_ACODEC_ID_MSBC) {
        inst->pcm_format.sample_rate = UAPI_AUDIO_SAMPLE_RATE_16K;
    } else if (inst->acodec_id == UAPI_ACODEC_ID_PCM) {
        inst->pcm_format.sample_rate = UAPI_AUDIO_SAMPLE_RATE_8K;
    }
    inst->pcm_format.sample_per_frame = inst->pcm_format.sample_rate / 100; /* 100 --> 10ms */

    /* 按照ai/aenc的属性创建播放器，用于播放aenc输出的es流 */
    return sample_phone_open_player(inst);
}

static td_void sample_phone_close_inst(sample_phone_inst *inst)
{
    sample_phone_close_ai(inst);
    sample_phone_close_sea(inst);
    sample_phone_close_aenc(inst);
    sample_phone_close_adp(inst);
    sample_phone_sys_deinit(inst);
}

static td_s32 sample_phone_open_inst(sample_phone_inst *inst)
{
    td_s32 ret;

    ret = sample_phone_sys_init(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_phone_sys_init, ret);
        return ret;
    }

    /*
     * 数据流向 ai -->sea---> aenc --> adp
     * 按照数据流向反方向打开实例
     * 按照数据方向attach output
     */
    ret = sample_phone_open_adp(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_phone_open_adp, ret);
        goto out0;
    }

    ret = sample_phone_open_aenc(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_phone_open_aenc, ret);
        goto out1;
    }

    if (inst->sea_enable == TD_TRUE) {
        ret = sample_phone_open_sea(inst);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(sample_phone_open_sea, ret);
            goto out2;
        }
    }

    ret = sample_phone_open_ai(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_phone_open_ai, ret);
        goto out3;
    }

    return EXT_SUCCESS;

out3:
    if (inst->sea_enable == TD_TRUE) {
        sample_phone_close_sea(inst);
    }
out2:
    sample_phone_close_aenc(inst);
out1:
    sample_phone_close_adp(inst);
out0:
    sample_phone_sys_deinit(inst);
    return ret;
}

static td_void sample_phone_stop_inst(sample_phone_inst *inst)
{
    td_s32 ret;

    ret = uapi_aenc_stop(inst->h_aenc);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_aenc_stop, ret);
    }

    if (inst->sea_enable == TD_TRUE) {
        ret = uapi_sea_stop(inst->h_sea);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_sea_stop, ret);
        }
    }

    ret = uapi_ai_stop(inst->h_ai);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_stop, ret);
    }
}

static td_s32 sample_phone_start_inst(sample_phone_inst *inst)
{
    td_s32 ret;

    ret = uapi_ai_start(inst->h_ai);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_start, ret);
    }

    if (inst->sea_enable == TD_TRUE) {
        ret = uapi_sea_start(inst->h_sea);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_sea_start, ret);
        }

        sample_gui_aef_add_handle(GUI_AEF_SEA, inst->h_sea);
    }

    ret = uapi_aenc_start(inst->h_aenc);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_aenc_start, ret);
    }

    return EXT_SUCCESS;
}

typedef struct {
    const td_char *name;
    uapi_acodec_id acodec_id;
} acodec_type_map;

static acodec_type_map g_acodec_type_map[] = {
    {"pcm", UAPI_ACODEC_ID_PCM},
    {"msbc", UAPI_ACODEC_ID_MSBC},
};

static td_s32 sample_phone_get_acodec_id(const td_char *name, uapi_acodec_id *acodec_id)
{
    td_u32 i;

    const size_t num = sizeof(g_acodec_type_map) / sizeof(acodec_type_map);

    for (i = 0; i < num; i++) {
        if (strcasecmp(name, g_acodec_type_map[i].name) == 0) {
            *acodec_id = g_acodec_type_map[i].acodec_id;
            sap_err_log_u32(*acodec_id);
            return EXT_SUCCESS;
        }
    }

    *acodec_id = UAPI_ACODEC_ID_MAX;
    return EXT_FAILURE;
}

static td_s32 sample_phone_sea_status(const td_char *sea_status, sample_phone_inst *inst)
{
    /* add para "nosea" to switch sea off */
    if (strcmp(sea_status, "nosea") == 0) {
        inst->sea_enable = TD_FALSE;
    } else {
        inst->sea_enable = TD_TRUE;
    }

    return EXT_SUCCESS;
}

static td_s32 sample_phone_share_adp_handle(sample_phone_inst *inst)
{
    adp_handle_share_set(inst->h_adp, inst->h_player, 1);
    return EXT_SUCCESS;
}

static td_void sample_phone_usage(td_void)
{
    sap_printf("\n");
    sap_printf("usage:  sample_phone -m [SeaStatus] -t [type]\n");
    sap_printf("\n");
    sap_printf("        -h     show this usage message and abort\n");
    sap_printf("        -m     sea status(nosea/sea)\n");
    sap_printf("        -t     the type of play audio(pcm/msbc)\n");
    sap_printf("\n");
    sap_printf("for example: ./sample_phone -m nosea -t msbc\n");
    sap_printf("\n");
}

static td_s32 sample_phone_parse_opt(td_s32 opt, const td_char *opt_arg, sample_phone_inst *inst)
{
    if (opt == 'h') {
        sample_phone_usage();
        return EXT_FAILURE;
    }

    if (opt_arg == TD_NULL) {
        printf("invalid option -- '%c'\n", (td_char)opt);
        return EXT_FAILURE;
    }

    switch (opt) {
        case 'm':
            return sample_phone_sea_status(opt_arg, inst);
        case 't':
            return sample_phone_get_acodec_id(opt_arg, &inst->acodec_id);
        default:
            printf("invalid command line\n");
            return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

static td_s32 sample_phone_parse_cmd_line(td_s32 argc, td_char *argv[], sample_phone_inst *inst)
{
    td_s32 opt;
    parg_state ps;

    if (argv == TD_NULL) {
        return EXT_FAILURE;
    }

    parg_init(&ps);

    while (1) {
        opt = parg_getopt(&ps, argc, argv, "m:t:h");
        if (opt == -1) {
            break;
        }

        if (sample_phone_parse_opt(opt, ps.optarg, inst) != EXT_SUCCESS) {
            sample_phone_usage();
            return EXT_FAILURE;
        }
    }

    if (inst->acodec_id == UAPI_ACODEC_ID_MAX) {
        sample_phone_usage();
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

static td_void sample_phone_init_ctx(sample_phone_inst *inst)
{
    clear_obj(inst);
    inst->ai_port = sample_phone_get_test_port();

    inst->sea_enable = TD_TRUE;
    inst->ai_ref_enable = TD_TRUE;

    /* ai/aenc的属性 */
    inst->acodec_id = UAPI_ACODEC_ID_MSBC;
    inst->pcm_format.channels = UAPI_AUDIO_CHANNEL_1;
    inst->pcm_format.bit_depth = UAPI_AUDIO_BIT_DEPTH_16;
    inst->pcm_format.sample_rate = UAPI_AUDIO_SAMPLE_RATE_16K;
    inst->pcm_format.sample_per_frame = inst->pcm_format.sample_rate / 100; /* 100 --> 10ms */
}

static td_s32 sample_phone_entry(td_s32 argc, td_char *argv[])
{
    td_s32 ret;
    sample_phone_inst *inst = &g_phone_inst;

    sample_phone_init_ctx(inst);

    ret = sample_phone_parse_cmd_line(argc, argv, inst);
    if (ret != EXT_SUCCESS) {
        return ret;
    }

    ret = sample_phone_inst_init(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_phone_inst_init, ret);
        return ret;
    }

    sap_alert_log_bool(inst->sea_enable);
    sap_alert_log_bool(inst->ai_ref_enable);
    sap_alert_log_h32(inst->ai_port);
    sap_alert_log_h32(inst->acodec_id);

    ret = sample_phone_open_inst(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_phone_open_inst, ret);
        goto out0;
    }

    ret = sample_phone_share_adp_handle(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_phone_share_adp_handle, ret);
        goto out1;
    }

    sap_printf("Wait P core to run sample.");
    sleep(WAIT_PCORE_INIT);  /* wait 2s for P core to get adp handle */

    ret = sample_phone_start_inst(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_phone_start_inst, ret);
        goto out1;
    }

    return EXT_SUCCESS;
out1:
    sample_phone_close_inst(inst);
out0:
    sample_phone_inst_deinit(inst);
    return ret;
}

static td_s32 sample_phone_exit(td_void)
{
    sample_phone_inst *inst = &g_phone_inst;

    sample_phone_stop_inst(inst);
    sample_phone_close_inst(inst);
    sample_phone_inst_deinit(inst);

    return EXT_SUCCESS;
}

td_s32 sample_phone_application(td_s32 argc, td_char *argv[])
{
    if (argc <= 1) {
        sample_phone_usage();
        return EXT_SUCCESS;
    }

    if (strcmp(argv[1], "q") == 0) {
        return sample_phone_exit();
    } else {
        return sample_phone_entry(argc, argv);
    }

    return EXT_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

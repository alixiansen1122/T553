/*
 * Copyright (c) CompanyNameMagicTag 2019-2020. All rights reserved.
 * Description: ai sample
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
#include "osal_list.h"
#include "soc_uapi_adp.h"
#include "soc_uapi_ai.h"
#include "sample_audio_api.h"
#include "sample_audio_utils.h"
#include "sample_phone.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define MIC0_TYPE "mic0"
#define MIC1_TYPE "mic1"

typedef enum {
    CHANNEL_L = 0x1,
    CHANNEL_R = 0x2,
} channel_type;

typedef struct {
    td_char *sample;
    td_char *size;
    td_char *file;
} sample_ai_arg;

typedef struct {
    td_char *sample;
    td_char *size;
    td_char *file;
    td_char *port;
    td_char *sample_rate;
    td_char *bit_depth;
    td_char *ch;
    td_char *i2s_master;
} sample_ai_long_arg;

typedef struct sample_ai_inst {
    athread_handle task;
    td_bool task_enable;
    td_bool mute;

    FILE *h_file;
    td_u32 file_size;
    td_u32 file_limit_size;
    td_handle h_adp;
    td_handle h_ai;
    td_handle h_player;
    td_u32 channel_type;
    uapi_ai_port ai_port;
    uapi_audio_pcm_format pcm_format;
    uapi_ai_gain volume;
    td_bool i2s_master;

    /* ao */
    uapi_snd snd_id;
    uapi_snd_out_port ao_port;

    td_u32 (*save_frame)(struct sample_ai_inst *inst, const uapi_audio_frame *frame);
    data_info write_data;
    td_bool run_bt_loopback;
    td_void (*sample_ai_process)(struct sample_ai_inst *inst, const uapi_audio_frame *frame);
    struct osal_list_head node;
} sample_ai_inst;

static OSAL_LIST_HEAD(g_sample_ai_list);

static inline td_bool ai_check_volume_adjust_support(uapi_ai_port ai_port)
{
    if ((ai_port >= UAPI_AI_PORT_ADC0 && ai_port <= UAPI_AI_PORT_ADC3) ||
        (ai_port >= UAPI_AI_PORT_LPADC0 && ai_port <= UAPI_AI_PORT_LPADC1) ||
        (ai_port >= UAPI_AI_PORT_PDM0 && ai_port <= UAPI_AI_PORT_PDM3)) {
        return TD_TRUE;
    }

    return TD_FALSE;
}

static sample_ai_inst *sample_ai_alloc_inst(td_void)
{
    sample_ai_inst *inst = (sample_ai_inst *)malloc(sizeof(sample_ai_inst));
    if (inst == TD_NULL) {
        return TD_NULL;
    }

    (td_void)memset_s(inst, sizeof(*inst), 0, sizeof(sample_ai_inst));

    osal_list_add_tail(&inst->node, &g_sample_ai_list);
    return inst;
}

static td_void sample_ai_free_inst(sample_ai_inst *inst)
{
    if (inst == TD_NULL) {
        return;
    }

    osal_list_del(&inst->node);
    free(inst);
}

static sample_ai_inst *sample_ai_get_inst(td_void)
{
    if (osal_list_empty(&g_sample_ai_list) != TD_FALSE) {
        return TD_NULL;
    }

    return osal_list_first_entry(&g_sample_ai_list, sample_ai_inst, node);
}

static td_bool run_bt_loopback = TD_FALSE;

static td_s32 ai_event_proc(td_handle ai, uapi_ai_event_type event, const td_void *param, td_void *context)
{
    sample_ai_inst *test_inst = (sample_ai_inst *)context;

    sap_err_log_h32(event);

    if (event == UAPI_AI_EVENT_VAD_VALID) {
    } else {
    }

    sap_unused(ai);
    sap_unused(param);
    sap_unused(test_inst);
    return EXT_SUCCESS;
}

static uapi_ai_port sample_ai_get_test_port(td_void)
{
#if (SAP_CHIP_TYPE == brandy)
    return UAPI_AI_PORT_ADC0;
#elif (SAP_CHIP_TYPE == socmn1)
    return UAPI_AI_PORT_LPADC0;
#else
    return UAPI_AI_PORT_I2S0;
#endif /* SAP_CHIP_TYPE */
}

static td_u32 save_frame_to_buf(sample_ai_inst *inst, const uapi_audio_frame *frame)
{
    circ_buf *cb = &inst->write_data.cb;

    if (circ_buf_query_free(cb) <= frame->bits_bytes) {
        sap_printf("Warning: PCM data enough");
        return 0;
    }

    return circ_buf_write(cb, (const td_u8 *)frame->bits_buffer, frame->bits_bytes);
}

static td_u32 save_frame_to_file(sample_ai_inst *inst, const uapi_audio_frame *frame)
{
    td_u32 ret;
    if (inst->h_file == TD_NULL) {
        return 0;
    }

    ret = fwrite((td_void *)frame->bits_buffer, 1, frame->bits_bytes, inst->h_file);
    return ret;
}

static td_void sample_ai_save_frame(sample_ai_inst *inst, const uapi_audio_frame *frame)
{
    inst->file_size += frame->bits_bytes;
    if (inst->file_size >= inst->file_limit_size) {
        return;
    }

    if (inst->save_frame(inst, frame) == 0) {
        inst->file_size = inst->file_limit_size;
    }
}

static td_void sample_ai_ao_parse_mic(const td_char *mic_name, sample_ai_inst *inst)
{
    if (strcmp(MIC0_TYPE, mic_name) == 0) {
        inst->channel_type |= CHANNEL_L;
    } else if (strcmp(MIC1_TYPE, mic_name) == 0) {
        inst->channel_type |= CHANNEL_R;
    }
}

#if (SAP_CHIP_TYPE == socmn1)
static td_void smaple_ai_ao_channel_sel(td_void *pcm, td_u32 samples, td_u32 channel_type)
{
    td_u32 i;
    td_s16 *in = TD_NULL;

    if (pcm == NULL) {
        return;
    }

    in = (td_s16 *)pcm;
    for (i = 0; i < samples; i++) {
        if (channel_type == CHANNEL_L) {
            *in = 0;
        } else if (channel_type == CHANNEL_R) {
            *(in + 1) = 0;
        }
        in += 2; /* 2 is num */
    }
}
#endif

static td_void sample_ai_ao_process(sample_ai_inst *inst, const uapi_audio_frame *frame)
{
    td_s32 ret;

#if (SAP_CHIP_TYPE == socmn1)
    if (inst->channel_type == CHANNEL_L || inst->channel_type == CHANNEL_R) {
        smaple_ai_ao_channel_sel((td_void *)frame->bits_buffer, frame->pcm_samples, inst->channel_type);
    }
#endif

    while (inst->task_enable) {
        ret = uapi_adp_send_frame(inst->h_player, frame);
        if (ret != EXT_SUCCESS) {
            sap_msleep(THREAD_SLEEP_10MS); /*   */
            continue;
        }
        break;
    }
}

static td_void sample_ai_thread(td_void *args)
{
    td_s32 ret;
    uapi_audio_frame frame;
    sample_ai_inst *inst = (sample_ai_inst *)args;

    if (inst == TD_NULL) {
        sap_err_log_info("ai_inst is null");
        return;
    }

    while (inst->task_enable) {
        ret = uapi_adp_acquire_frame(inst->h_adp, &frame);
        if (ret != EXT_SUCCESS) {
            sap_msleep(THREAD_SLEEP_10MS);
            continue;
        }

        if (inst->sample_ai_process != NULL) {
            inst->sample_ai_process(inst, &frame);
        }

        ret = uapi_adp_release_frame(inst->h_adp, &frame);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_adp_release_frame, ret);
        }
    }

    athread_set_exit(inst->task, TD_TRUE);
}

static td_s32 sample_ai_sys_init(td_void)
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
        goto out;
    }

    return EXT_SUCCESS;
out:
    (td_void)uapi_adp_deinit();
    return ret;
}

static td_void sample_ai_sys_deinit(td_void)
{
    td_s32 ret;

    ret = uapi_ai_deinit();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_deinit, ret);
    }

    ret = uapi_adp_deinit();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_deinit, ret);
    }
}

static td_void sample_ai_close_adp(sample_ai_inst *inst)
{
    if (inst->h_adp != 0) {
        uapi_adp_destroy(inst->h_adp);
        inst->h_adp = 0;
    }
}

static td_s32 sample_ai_open_adp(sample_ai_inst *inst)
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

static td_void sample_ai_close_ai(sample_ai_inst *inst)
{
    td_s32 ret;

    if (inst->h_ai == 0) {
        return;
    }

    /* Detach ADP instance from AI instance */
    ret = uapi_ai_detach_output(inst->h_ai, inst->h_adp);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_detach_output, ret);
    }

    ret = uapi_ai_close(inst->h_ai);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_close, ret);
    }

    inst->h_ai = 0;
}

static td_void sample_ai_set_port_attr(const sample_ai_inst *inst, uapi_ai_attr *ai_attr)
{
    td_s32 ret;

    ret = memcpy_s(&ai_attr->pcm_attr, sizeof(ai_attr->pcm_attr), &inst->pcm_format, sizeof(inst->pcm_format));
    if (ret != EOK) {
        return;
    }

    ai_attr->ref_attr.enable = TD_FALSE;

    switch (inst->ai_port) {
        case UAPI_AI_PORT_PDM0:
            ai_attr->port_attr.pdm.rx_type = UAPI_AI_RX_DEFAULT;
            ai_attr->port_attr.pdm.i2s_attr.channels = inst->pcm_format.channels;
            ai_attr->port_attr.pdm.i2s_attr.bit_depth = inst->pcm_format.bit_depth;
            break;

        case UAPI_AI_PORT_ADC0:
        case UAPI_AI_PORT_LPADC0:
        case UAPI_AI_PORT_LPADC1:
            ai_attr->port_attr.adc.rx_type = UAPI_AI_RX_DEFAULT;
            break;

        case UAPI_AI_PORT_I2S0:
        case UAPI_AI_PORT_I2S1:
        case UAPI_AI_PORT_I2S2:
            get_i2s_attr(&ai_attr->port_attr.i2s.i2s_attr, (const uapi_audio_pcm_format *)&inst->pcm_format, TD_FALSE);
            ai_attr->port_attr.i2s.i2s_attr.master = inst->i2s_master;
            break;

        default:
            break;
    }

    if ((inst->ai_port <= UAPI_AI_PORT_I2S2) && (ai_attr->port_attr.i2s.i2s_attr.i2s_mode == UAPI_AUDIO_I2S_TDM_MODE)) {
        ai_attr->port_attr.i2s.i2s_attr.pcm_sample_rise_edge = TD_FALSE;
    }
}

static td_s32 sample_ai_open_ai(sample_ai_inst *inst)
{
    td_s32 ret;
    uapi_ai_attr ai_attr;

    ret = uapi_ai_get_default_attr(inst->ai_port, &ai_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_get_default_attr, ret);
        return ret;
    }

    sample_ai_set_port_attr(inst, &ai_attr);

    ret = uapi_ai_open(&inst->h_ai, inst->ai_port, &ai_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_open, ret);
        return ret;
    }

    /* Register AI event process callback */
    ret = uapi_ai_register_event_proc(inst->h_ai, ai_event_proc, (td_void *)inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_register_event_proc, ret);
        goto out;
    }

    if (ai_check_volume_adjust_support(inst->ai_port) == TD_TRUE) {
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
    }

    /* Attach ADP instance to AI instance */
    ret = uapi_ai_attach_output(inst->h_ai, inst->h_adp);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_attach_output, ret);
        goto out;
    }

    return EXT_SUCCESS;
out:
    (td_void)uapi_ai_close(inst->h_ai);
    return ret;
}

static td_void sample_ai_close_file(sample_ai_inst *inst)
{
    if (inst->h_file != TD_NULL) {
        fclose(inst->h_file);
        inst->h_file = TD_NULL;
    }
}

static td_s32 sample_ai_open_file(sample_ai_inst *inst, const td_char *file)
{
    if (file == TD_NULL || strcmp("null", file) == 0) {
        inst->h_file = TD_NULL;
        data_info_init(&inst->write_data, FPGA_SAVE_DATA_ADDR, FPGA_SAVE_DATA_SIZE);
        inst->save_frame = save_frame_to_buf;
    } else {
        td_s32 ret;
        td_char file_path[FILE_PATH_LEN];
        ret = snprintf_s(file_path, FILE_PATH_LEN - 1, FILE_PATH_LEN - 1, "%s_%uk_%ubit_%uch.pcm", file,
            inst->pcm_format.sample_rate / 1000, /* 1000 Hz to kHz */
            inst->pcm_format.bit_depth, inst->pcm_format.channels);
        if (ret < 0) {
            sap_err_log_fun(snprintf_s, ret);
            return EXT_FAILURE;
        }

        inst->h_file = fopen(file_path, "wb");
        if (inst->h_file == TD_NULL) {
            sap_printf("open %s failed", file_path);
            return EXT_FAILURE;
        }
        inst->save_frame = save_frame_to_file;
    }

    return EXT_SUCCESS;
}

static td_void sample_ai_inst_deinit(sample_ai_inst *inst)
{
    sample_ai_close_file(inst);
}

static td_s32 sample_ai_inst_init(const sample_ai_arg *arg, sample_ai_inst *inst)
{
    inst->ai_port = sample_ai_get_test_port();

    inst->pcm_format.channels = UAPI_AUDIO_CHANNEL_1;
    inst->pcm_format.bit_depth = UAPI_AUDIO_BIT_DEPTH_16;
    inst->pcm_format.sample_rate = UAPI_AUDIO_SAMPLE_RATE_16K;
    inst->pcm_format.sample_per_frame = inst->pcm_format.sample_rate / 100; /* 100 --> 10ms */

    inst->volume.integer = AI_VOLUME_DEFAULT;
    inst->volume.decimal = 0;

    inst->i2s_master = TD_TRUE;
    return EXT_SUCCESS;
}

static td_void sample_ai_close_inst(sample_ai_inst *inst)
{
    sample_ai_close_ai(inst);
    sample_ai_close_adp(inst);
    sample_ai_sys_deinit();
}

static td_s32 sample_ai_open_inst(sample_ai_inst *inst)
{
    td_s32 ret;

    ret = sample_ai_sys_init();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_ai_sys_init, ret);
        return ret;
    }

    /*
     * 数据流向 ai --> adp
     * 按照数据流向反方向打开实例
     * 按照数据方向attach output
     */
    ret = sample_ai_open_adp(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_ai_open_adp, ret);
        goto out0;
    }

    ret = sample_ai_open_ai(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_ai_open_ai, ret);
        goto out1;
    }

    return EXT_SUCCESS;

out1:
    sample_ai_close_adp(inst);
out0:
    sample_ai_sys_deinit();
    return ret;
}

static td_void sample_ai_stop_inst(sample_ai_inst *inst)
{
    td_s32 ret;

    if (inst->task != TD_NULL) {
        /* Destory Task to stop ADP instance reading pcm data */
        inst->task_enable = TD_FALSE;
        athread_exit(inst->task);
        inst->task = TD_NULL;
    }

    /* Stop AI instance output pcm data */
    ret = uapi_ai_stop(inst->h_ai);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_stop, ret);
    }
}

static td_s32 sample_ai_start_inst(sample_ai_inst *inst)
{
    td_s32 ret;
    athread_attr attr;

    /* Start AI instance to write data into ADP instance */
    ret = uapi_ai_start(inst->h_ai);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_start, ret);
        return ret;
    }

    inst->sample_ai_process = sample_ai_save_frame;
    inst->task_enable = TD_TRUE;
    attr.priority = ATHREAD_PRIORITY_NORMAL;
    attr.stack_size = 0x1000; /* 4k */
    attr.name = "sample_ai";
    ret = athread_create(&inst->task, sample_ai_thread, (td_void *)inst, &attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(athread_create, ret);
        goto out;
    }

    return EXT_SUCCESS;
out:
    (td_void)uapi_ai_stop(inst->h_ai);
    return ret;
}

static td_void sample_ai_usage(const td_char *name)
{
    sap_printf(" usage: %s size(MB) file(*.pcm)\n", name);
    sap_printf(" examples:\n");
    sap_printf(" %s 100 /mnt/ai.pcm\n", name);
    sap_printf(" %s 100 null\n", name);
    sap_printf(" usage: %s size(MB) file(*.pcm) port sample_rate bit_depth channel i2s_master\n", name);
    sap_printf(" examples:\n");
    sap_printf(" %s 100 /mnt/ai.pcm i2s1 16000 16 2 1\n", name);
    sap_printf(" %s 100 /mnt/ai.pcm i2s1 16000 16 8 0\n", name);
    sap_printf("\n");
}

static td_void sample_ai_ao_parse_port(const td_char *port_name, sample_ai_inst *inst);

static td_s32 sample_ai_parse_cmd_line(td_s32 argc, td_char *argv[], sample_ai_inst *inst)
{
    td_u32 size;
    sample_ai_long_arg *arg = (sample_ai_long_arg *)argv;

    if (argv == TD_NULL) {
        return EXT_FAILURE;
    }

    size = (td_u32)strtoul(arg->size, TD_NULL, 0);
    if (size == 0 || size > 100) { /* 100 max file size */
        sap_err_log_ret(size);
        return EXT_FAILURE;
    }
    inst->file_limit_size = (size << 20); /* 20: bytes to MB */

    if (argc == (td_s32)sizeof(sample_ai_arg) / (td_s32)sizeof(td_char *)) {
        return AUDIO_SUCCESS;
    }

    if (arg->port != TD_NULL) {
        sample_ai_ao_parse_port(arg->port, inst);
    }

    if (arg->sample_rate != TD_NULL) {
        inst->pcm_format.sample_rate = (td_u32)strtoul(arg->sample_rate, TD_NULL, 0);
    }

    if (arg->bit_depth != TD_NULL) {
        inst->pcm_format.bit_depth = (td_u32)strtoul(arg->bit_depth, TD_NULL, 0);
    }

    if (arg->ch != TD_NULL) {
        inst->pcm_format.channels = (td_u32)strtoul(arg->ch, TD_NULL, 0);
    }

    if (arg->i2s_master != TD_NULL) {
        inst->i2s_master = !!(td_u32)strtol(arg->i2s_master, TD_NULL, 0);
        sap_printf("i2s master: %u", inst->i2s_master);
    }

    return EXT_SUCCESS;
}

static td_s32 sample_ai_entry(td_s32 argc, td_char *argv[])
{
    td_s32 ret;
    sample_ai_arg *arg = (sample_ai_arg *)argv;
    sample_ai_inst *inst = sample_ai_get_inst();
    if (inst != TD_NULL) {
        sap_printf("%s is already running\n", argv[0]);
        return EXT_FAILURE;
    }

    inst = sample_ai_alloc_inst();
    if (inst == TD_NULL) {
        sap_printf("sample_ai_alloc_inst failed\n");
        return EXT_FAILURE;
    }

    ret = sample_ai_inst_init(arg, inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_ai_inst_init, ret);
        goto out0;
    }

    ret = sample_ai_parse_cmd_line(argc, argv, inst);
    if (ret != EXT_SUCCESS) {
        goto out0;
    }

    ret = sample_ai_open_file(inst, arg->file);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_ai_open_file, ret);
        goto out0;
    }

    ret = sample_ai_open_inst(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_ai_open_inst, ret);
        goto out1;
    }

    ret = sample_ai_start_inst(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_ai_start_inst, ret);
        goto out2;
    }

    sap_printf("sample_rate: %u, channels: %u, bit_depth: %u, port: %x\n", inst->pcm_format.sample_rate,
        inst->pcm_format.channels, inst->pcm_format.bit_depth, inst->ai_port);

    return EXT_SUCCESS;
out2:
    sample_ai_close_inst(inst);
out1:
    sample_ai_inst_deinit(inst);
out0:
    sample_ai_free_inst(inst);

    audio_unused(argc);
    return ret;
}

static td_s32 sample_ai_exit(td_void)
{
    sample_ai_inst *inst = sample_ai_get_inst();

    if (inst != NULL) {
        sample_ai_stop_inst(inst);
        sample_ai_close_inst(inst);
        sample_ai_inst_deinit(inst);
        sample_ai_free_inst(inst);
    }

    return EXT_SUCCESS;
}

td_s32 sample_ai(td_s32 argc, td_char *argv[])
{
    if (argc <= 1) {
        sample_ai_usage(argv[0]);
        return EXT_SUCCESS;
    }

    if (strcmp(argv[1], "q") == 0) {
        return sample_ai_exit();
    } else {
        return sample_ai_entry(argc, argv);
    }
}

static td_void sample_ai_ao_usage(const td_char *name)
{
    sap_printf(" usage: %s -p [port] -c [channels] -b [bit_depth] -s [sample_rate] -v [volume]\n", name);
    sap_printf("        -h     show this usage message and abort\n");
    sap_printf("        -p     specify ai port(mad0/i2s0/i2s1/adc0/lpadc0)\n"
           "        -c     channels of ai port(default: 2)\n"
           "        -b     bit_depth of ai port(default: 16)\n"
           "        -s     samplerate of ai port(default: 48000)\n"
           "        -v     volume of ai port(default: 0)\n"
           "        -m     mute ai port(default: false)\n"
           "        -l     specify a long parameter(bt_loopback/mic0/mic1)\n"
           "\n");
    sap_printf(" examples:\n");
    sap_printf(" %s -p mad0 -c 1 -s 16000\n", name);
    sap_printf(" %s -p mad0 -c 2 -s 48000\n", name);
    sap_printf(" %s -p i2s1\n", name);
    sap_printf(" %s -p i2s1 -c 1\n", name);
    sap_printf(" %s -p adc0 -c 2 -s 48000 -v 30\n", name);
    sap_printf("\n");
}

static td_void sample_ai_ao_parse_port(const td_char *port_name, sample_ai_inst *inst)
{
    td_u32 i;
    const struct {
        const td_char *name;
        uapi_ai_port ai_port;
    } ai_port_name[] = {
        {"mad0", UAPI_AI_PORT_PDM0},
        {"pdm0", UAPI_AI_PORT_PDM0},
        {"pdm1", UAPI_AI_PORT_PDM1},
        {"i2s0", UAPI_AI_PORT_I2S0},
        {"i2s1", UAPI_AI_PORT_I2S1},
        {"i2s2", UAPI_AI_PORT_I2S2},
        {"i2s3", UAPI_AI_PORT_I2S3},
        {"adc0", UAPI_AI_PORT_ADC0},
        {"adc1", UAPI_AI_PORT_ADC1},
        {"lpadc0", UAPI_AI_PORT_LPADC0},
        {"lpadc1", UAPI_AI_PORT_LPADC1},
    };

    for (i = 0; i < sizeof(ai_port_name) / sizeof(ai_port_name[0]); i++) {
        if (strcmp(ai_port_name[i].name, port_name) == 0) {
            inst->ai_port = ai_port_name[i].ai_port;
            return;
        }
    }

    inst->ai_port = sample_ai_get_test_port();
}

static td_void sample_ai_ao_parse_snd_port(const td_char *port_name, sample_ai_inst *inst)
{
    td_u32 i;
    const struct {
        const td_char *name;
        uapi_snd_out_port ao_port;
    } ao_port_name[] = {
        {"dac0", UAPI_SND_OUT_PORT_DAC0},
        {"i2s0", UAPI_SND_OUT_PORT_I2S0},
        {"i2s1", UAPI_SND_OUT_PORT_I2S1},
        {"i2s2", UAPI_SND_OUT_PORT_I2S2},
    };

    for (i = 0; i < sizeof(ao_port_name) / sizeof(ao_port_name[0]); i++) {
        if (strcmp(ao_port_name[i].name, port_name) == 0) {
            inst->ao_port = ao_port_name[i].ao_port;
            return;
        }
    }

    inst->ao_port = SND_OUT_PORT_DEFAULT;
}

static td_void sample_ai_ao_parse_snd_id(const td_char *snd_name, sample_ai_inst *inst)
{
    td_u32 i;
    const struct {
        const td_char *name;
        uapi_snd snd_id;
    } ao_snd_name[] = {
        {"snd0", UAPI_SND_0},
        {"snd1", UAPI_SND_1},
        {"snd2", UAPI_SND_2},
    };

    for (i = 0; i < sizeof(ao_snd_name) / sizeof(ao_snd_name[0]); i++) {
        if (strcmp(ao_snd_name[i].name, snd_name) == 0) {
            inst->snd_id = ao_snd_name[i].snd_id;
            return;
        }
    }

    inst->snd_id = UAPI_SND_0;
}

static td_void sample_ai_ao_parse_long_opt(td_char opt, const td_char *opt_arg, sample_ai_inst *inst)
{
    if (opt_arg == TD_NULL) {
        sap_printf("invalid option -- '%c'\n", opt);
        return;
    }

    if (strcmp(opt_arg, "bt_loopback") == 0) {
        inst->run_bt_loopback = TD_TRUE;
        return;
    }

    sample_ai_ao_parse_mic(opt_arg, inst);
}

static td_s32 sample_ai_ao_parse_opt(td_char opt, const td_char *opt_arg, sample_ai_inst *inst)
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
            sample_ai_ao_parse_port(opt_arg, inst);
            break;
        case 'o':
            sample_ai_ao_parse_snd_port(opt_arg, inst);
            break;
        case 'd':
            sample_ai_ao_parse_snd_id(opt_arg, inst);
            break;
        case 'v':
            inst->volume.integer = (td_s32)strtol(opt_arg, TD_NULL, 0);
            break;
        case 'm':
            inst->mute = TD_TRUE;
            break;
        case 'l':
            sample_ai_ao_parse_long_opt(opt, opt_arg, inst);
            break;
        default:
            sap_printf("invalid command line\n");
            return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

static td_s32 sample_ai_ao_parse_cmd_line(td_s32 argc, td_char *argv[], sample_ai_inst *inst)
{
    td_s32 ret;
    td_char opt;
    parg_state ps;

    if (argv == TD_NULL) {
        return EXT_FAILURE;
    }

    parg_init(&ps);

    while (1) {
        ret = parg_getopt(&ps, argc, argv, "s:c:b:p:v:o:d:hml:");
        if (ret == -1) {
            break;
        }

        opt = (td_char)ret;
        if (sample_ai_ao_parse_opt(opt, ps.optarg, inst) != EXT_SUCCESS) {
            sample_ai_ao_usage(argv[0]);
            return EXT_FAILURE;
        }
    }

    inst->pcm_format.sample_per_frame = inst->pcm_format.sample_rate / 100; /* 100 --> 10ms */
    return EXT_SUCCESS;
}

static td_s32 sample_ai_ao_start_inst(sample_ai_inst *inst)
{
    td_s32 ret;
    athread_attr attr;

    /* Start AI instance to write data into ADP instance */
    ret = uapi_ai_start(inst->h_ai);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_start, ret);
        return ret;
    }

    /* Create task for ADP instance to read pcm data */
    inst->task_enable = TD_TRUE;
    inst->sample_ai_process = sample_ai_ao_process;

    if (inst->run_bt_loopback != TD_TRUE) {
        attr.priority = ATHREAD_PRIORITY_NORMAL;
        attr.stack_size = 0x1000; /* 4k */
        attr.name = "sample_ai_ao";
        ret = athread_create(&inst->task, sample_ai_thread, (td_void *)inst, &attr);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(athread_create, ret);
            goto out;
        }
    } else {
        adp_handle_share_set(inst->h_adp, inst->h_player, 0);
        sap_printf("[audio] ul handle = x%x, dl handle = x%x\n", inst->h_adp, inst->h_player);
    }

    return EXT_SUCCESS;
out:
    (td_void)uapi_ai_stop(inst->h_ai);
    return ret;
}

static td_void sample_ai_ao_inst_init(sample_ai_inst *inst)
{
    inst->ai_port = sample_ai_get_test_port();

    inst->snd_id = UAPI_SND_0;
    inst->ao_port = SND_OUT_PORT_DEFAULT;

    inst->pcm_format.channels = UAPI_AUDIO_CHANNEL_1;
    inst->pcm_format.bit_depth = UAPI_AUDIO_BIT_DEPTH_16;
    inst->pcm_format.sample_rate = UAPI_AUDIO_SAMPLE_RATE_16K;
    inst->pcm_format.sample_per_frame = inst->pcm_format.sample_rate / 100; /* 100 --> 10ms */
    inst->run_bt_loopback = TD_FALSE;
#if (SAP_CHIP_TYPE == socmn1)
    inst->channel_type = 0x0;
#endif
}

static td_s32 sample_ai_player_open(sample_ai_inst *inst)
{
    td_s32 ret;
    td_handle h_snd;
    sample_acodec_arg in_arg;
    const uapi_snd_attr snd_attr = {
        .port_num = 1,
        .port_attr[0].out_port = inst->ao_port,
        .channels = SND_OUT_CHANNEL_DEF,
        .bit_depth = SND_OUT_BIT_DEPTH_DEF,
        .sample_rate = SND_OUT_SAMPLE_RATE_DEF,
    };

    sample_escast_arg escast_arg = {
        .player = &inst->h_player,
        .in_arg = &in_arg,
        .snd = &h_snd,
        .snd_attr = &snd_attr,
        .aef_profile = UAPI_SND_AEF_PROFILE_MUSIC,
    };

    /* set track input pcm format matching ai output pcm format */
    ret = memcpy_s(&in_arg.pcm_format, sizeof(in_arg.pcm_format), &inst->pcm_format, sizeof(inst->pcm_format));
    if (ret != EOK) {
        sap_err_log_fun(memcpy_s, ret);
        return ret;
    }

    return sample_ao_player_open(&escast_arg, inst->snd_id);
}

static td_s32 sample_ai_ao_entry(td_s32 argc, td_char *argv[])
{
    td_s32 ret;
    sample_ai_inst *inst = TD_NULL;

    inst = sample_ai_alloc_inst();
    if (inst == TD_NULL) {
        sap_err_log_info("sample_ai_ao_alloc_inst failed");
        return EXT_FAILURE;
    }

    sample_ai_ao_inst_init(inst);

    ret = sample_ai_ao_parse_cmd_line(argc, argv, inst);
    if (ret != EXT_SUCCESS) {
        goto out0;
    }

    ret = sample_ai_player_open(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_ao_player_open, ret);
        goto out0;
    }

    ret = sample_ai_open_inst(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_ai_open_inst, ret);
        goto out1;
    }

    ret = sample_ai_ao_start_inst(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_ai_start_inst, ret);
        goto out2;
    }

    sap_alert_log_u32(inst->pcm_format.sample_rate);
    sap_alert_log_u32(inst->pcm_format.channels);
    sap_alert_log_u32(inst->pcm_format.bit_depth);

    return EXT_SUCCESS;
out2:
    sample_ai_close_inst(inst);
out1:
    sample_ao_player_close();
out0:
    sample_ai_free_inst(inst);
    return ret;
}

static td_s32 sample_ai_ao_exit(td_void)
{
    sample_ai_inst *inst = sample_ai_get_inst();

    if (inst != NULL) {
        sample_ai_stop_inst(inst);
        sample_ai_close_inst(inst);
        sample_ao_player_close();
        sample_ai_free_inst(inst);
    }

    return EXT_SUCCESS;
}

td_s32 sample_ai_ao(td_s32 argc, td_char *argv[])
{
    if (argc <= 1) {
        sample_ai_ao_usage(argv[0]);
        return EXT_FAILURE;
    }

    if (strcmp(argv[1], "q") == 0) {
        return sample_ai_ao_exit();
    } else {
        return sample_ai_ao_entry(argc, argv);
    }
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

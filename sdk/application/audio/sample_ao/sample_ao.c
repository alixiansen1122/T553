/*
 * Copyright (c) CompanyNameMagicTag 2019-2020. All rights reserved.
 * Description: ao sample
 * Author: audio
 * Create: 2019-09-17
 */
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "thread_os.h"
#include "chip_type.h"
#include "osal_list.h"
#include "soc_uapi_adp.h"
#include "soc_uapi_adec.h"
#include "soc_uapi_sound.h"
#include "sample_audio_api.h"
#include "sample_audio_utils.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define MAX_MIXER_NUM 4

typedef struct {
    td_char *in_stream;
    td_char *volume;
    td_char *sample_rate;
    td_char *bit_depth;
    td_char *channels;
} sample_ao_track_arg;

typedef struct {
    td_char *sample;
    sample_ao_track_arg track_arg[MAX_MIXER_NUM];
} sample_ao_arg;

typedef struct sample_ao_track_inst {
    athread_handle task;
    td_bool task_active;

    td_char *in_stream;
    FILE *h_file;
    td_u8 *frame_buf;
    td_u32 frame_size;

    td_handle h_adp;
    td_handle h_track;

    data_info data_info;
    td_u32 (*read_frame)(struct sample_ao_track_inst *inst, uapi_audio_frame *frame);
    td_s32 volume;
    uapi_audio_pcm_format pcm_format;
} sample_ao_track_inst;

typedef struct {
    uapi_snd snd_id;
    td_handle h_snd;
    uapi_snd_attr snd_attr;
    uapi_snd_aef_profile aef_profile;
    td_u32 track_num;
    sample_ao_track_inst track_inst[MAX_MIXER_NUM];
    struct osal_list_head node;
} sample_ao_inst;

static td_u32 g_vector_read = 0;

static OSAL_LIST_HEAD(g_sample_ao_list);

static td_void sample_ao_show_inst(td_void)
{
    td_u32 cnt = 0;
    sample_ao_inst *inst = TD_NULL;

    if (osal_list_empty(&g_sample_ao_list) != TD_FALSE) {
        sap_alert_log_info("sample_ao all exit!");
        return;
    }

    osal_list_for_each_entry(inst, &g_sample_ao_list, node) {
        cnt++;
        sap_trace_log_u32(cnt);
        sap_trace_log_u32(inst->snd_id);
        sap_trace_log_u32(inst->track_num);
    }
}

static sample_ao_inst *sample_ao_alloc_inst(td_void)
{
    sample_ao_inst *inst = (sample_ao_inst *)malloc(sizeof(sample_ao_inst));
    if (inst == TD_NULL) {
        return TD_NULL;
    }

    memset_s(inst, sizeof(*inst), 0, sizeof(sample_ao_inst));

    osal_list_add_tail(&inst->node, &g_sample_ao_list);
    return inst;
}

static td_void sample_ao_free_inst(sample_ao_inst *inst)
{
    if (inst == TD_NULL) {
        return;
    }

    osal_list_del(&inst->node);
    free(inst);
}

static sample_ao_inst *sample_ao_get_inst(td_void)
{
    if (osal_list_empty(&g_sample_ao_list) != TD_FALSE) {
        return TD_NULL;
    }

    return osal_list_first_entry(&g_sample_ao_list, sample_ao_inst, node);
}

static td_u32 sample_ao_track_read_file(sample_ao_track_inst *inst, uapi_audio_frame *frame)
{
    td_u32 read_len;

    if (inst->h_file == TD_NULL) {
        return 0;
    }

    read_len = fread(frame->bits_buffer, 1, frame->bits_bytes, inst->h_file);
    if (read_len == 0) {
        sap_printf("reset read\n");
        rewind(inst->h_file);
        return 0;
    }

    return read_len;
}

static td_u32 sample_ao_track_read_flash(sample_ao_track_inst *inst, uapi_audio_frame *frame)
{
    td_u32 len = inst->frame_size;
    circ_buf *cb = &inst->data_info.cb;

    if (cb == TD_NULL) {
        sap_err_log_info("Fatal Error: circ_buf pointer is null");
        return 0;
    }

    if (len > cb->size) {
        sap_err_log_info("Fatal Error: data not enough");
        return 0;
    }

    if (circ_buf_query_busy(cb) < len) {
        sap_err_log_info("reset read");
        circ_buf_flush(cb);
        circ_buf_update_write_pos(cb, cb->size - sizeof(td_s16));
    }

    if (circ_buf_read(cb, (td_u8 *)frame->bits_buffer, len) != len) {
        sap_err_log_info("Fatal Error: circ_buf_read error");
        return 0;
    }

    circ_buf_update_write_pos(cb, len);
    return len;
}

static td_u32 sample_ao_track_read_array(sample_ao_track_inst *inst, uapi_audio_frame *frame)
{
    td_u32 i;
    td_u32 ch;
    td_u16 *buf = (td_u16 *)(td_uintptr_t)frame->bits_buffer;
    const td_u16 *vector_16k_1ch_16bit = sample_get_vector_16k_1ch_16bit();
    const td_u32 verctor_size = sample_get_vector_size();

    for (i = 0; i < inst->pcm_format.sample_per_frame; i++) {
        for (ch = 0; ch < inst->pcm_format.channels; ch++) {
            *buf++ = vector_16k_1ch_16bit[g_vector_read];
        }

        g_vector_read = saturate_add(g_vector_read, 1, verctor_size);
    }

    return inst->frame_size;
}

static td_void sample_ao_track_build_frame(const sample_ao_track_inst *inst, uapi_audio_frame *frame)
{
    clear_obj(frame);

    frame->interleaved = TD_TRUE;
    frame->channels = inst->pcm_format.channels;
    frame->bit_depth = inst->pcm_format.bit_depth;
    frame->sample_rate = inst->pcm_format.sample_rate;
    frame->pcm_samples = inst->pcm_format.sample_per_frame;
    frame->bits_buffer = (td_s32 *)inst->frame_buf;
    frame->bits_bytes = inst->frame_size;
    frame->pkg_loss = TD_FALSE;
}

static td_void sample_ao_track_thread(td_void *args)
{
    td_s32 ret;
    td_u32 len;
    uapi_audio_frame frame;
    sample_ao_track_inst *inst = (sample_ao_track_inst *)args;

    inst->frame_buf = (td_u8 *)malloc(inst->frame_size);
    if (inst->frame_buf == TD_NULL) {
        sap_err_log_u32(inst->frame_size);
        sap_err_log_info("malloc buffer failed");
        return;
    }

    sample_ao_track_build_frame(inst, &frame);
    while (inst->task_active) {
        uapi_adp_query_free(inst->h_adp, &len);
        if (len < FILE_READ_LEN_MIN) {
            sap_msleep(THREAD_SLEEP_5MS);
            continue;
        }

        len = circ_buf_min(len, inst->frame_size);
        frame.bits_bytes = len;

        len = inst->read_frame(inst, &frame);
        if (len == 0) {
            continue;
        }
        frame.bits_bytes = len;

        while (inst->task_active) {
            ret = uapi_adp_send_frame(inst->h_adp, &frame);
            if (ret != EXT_SUCCESS) {
                sap_msleep(THREAD_SLEEP_10MS);
                continue;
            }
            break;
        }
    }

    free(inst->frame_buf);
    inst->frame_buf = TD_NULL;
    athread_set_exit(inst->task, TD_TRUE);
}

static td_s32 sample_ao_sys_init(td_void)
{
    td_s32 ret;

    ret = uapi_adp_init();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_init, ret);
        return ret;
    }

    ret = uapi_snd_init();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_init, ret);
        goto out;
    }

    return EXT_SUCCESS;
out:
    (td_void)uapi_adp_deinit();
    return ret;
}

static td_void sample_ao_sys_deinit(td_void)
{
    td_s32 ret;

    ret = uapi_snd_deinit();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_deinit, ret);
    }

    ret = uapi_adp_deinit();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_deinit, ret);
    }
}

static td_void sample_ao_close_adp(sample_ao_track_inst *inst)
{
    td_s32 ret;

    if (inst->h_adp == 0) {
        return;
    }

    ret = uapi_adp_detach_output(inst->h_adp, inst->h_track);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_detach_output, ret);
    }

    ret = uapi_adp_destroy(inst->h_adp);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_destroy, ret);
    }

    inst->h_adp = 0;
}

static td_s32 sample_ao_open_adp(sample_ao_track_inst *inst)
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

    ret = uapi_adp_attach_output(inst->h_adp, inst->h_track);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_attach_output, ret);
        goto out;
    }

    return EXT_SUCCESS;
out:
    (td_void)uapi_adp_destroy(inst->h_adp);
    return ret;
}

static td_void sample_ao_close_snd(sample_ao_inst *inst)
{
    td_s32 ret;

    if (inst->h_snd == 0) {
        return;
    }

    ret = uapi_snd_close(inst->h_snd);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_close, ret);
    }

    inst->h_snd = 0;
}

static td_void sample_ao_set_i2s_attr(uapi_snd_attr *snd_attr)
{
    td_u32 bclk_hz = 0;
    td_bool local_out_port;
    const uapi_audio_pcm_format pcm_fmt = {
        .sample_rate = snd_attr->sample_rate,
        .channels = snd_attr->channels,
        .bit_depth = snd_attr->bit_depth,
    };
    uapi_audio_i2s_attr *i2s_attr = TD_NULL;

    for (td_u32 i = 0; i < snd_attr->port_num; ++i) {
        if (snd_attr->port_attr[i].out_port < UAPI_SND_OUT_PORT_I2S0 ||
            snd_attr->port_attr[i].out_port > UAPI_SND_OUT_PORT_I2S4) {
            continue; /* Set i2s attr when port is i2s */
        }

        i2s_attr = &snd_attr->port_attr[i].u.i2s_attr.attr;
        local_out_port = (snd_attr->port_attr[i].out_port == SND_OUT_PORT_DEFAULT) ? TD_TRUE : TD_FALSE;

        get_i2s_attr(i2s_attr, &pcm_fmt, local_out_port);
    }
}

static td_s32 sample_ao_open_snd(sample_ao_inst *inst)
{
    td_s32 ret;
    uapi_snd_attr snd_attr;

    inst->h_snd = 0;
    ret = uapi_snd_get_default_attr(inst->snd_id, &snd_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_get_default_attr, ret);
        return ret;
    }

    snd_attr.port_num = 1;
    snd_attr.port_attr[0].out_port = inst->snd_attr.port_attr[0].out_port;
    snd_attr.channels = inst->snd_attr.channels;
    snd_attr.bit_depth = inst->snd_attr.bit_depth;
    snd_attr.sample_rate = inst->snd_attr.sample_rate;

    sample_ao_set_i2s_attr(&snd_attr);

    ret = uapi_snd_open(&inst->h_snd, inst->snd_id, &snd_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_open, ret);
        return ret;
    }

    ret = uapi_snd_set_aef_profile(inst->h_snd, inst->aef_profile);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_set_aef_profile, ret);
    }

    return EXT_SUCCESS;
}

static td_void sample_ao_close_track(sample_ao_track_inst *inst)
{
    td_s32 ret;

    if (inst->h_track == 0) {
        return;
    }

    ret = uapi_snd_destroy_track(inst->h_track);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_destroy_track, ret);
    }

    inst->h_track = 0;
}


static td_s32 sample_ao_open_track(td_handle h_snd, sample_ao_track_inst *inst)
{
    td_s32 ret;
    uapi_snd_track_attr track_attr;

    uapi_snd_gain gain = {
        .integer = inst->volume,
        .decimal = 0,
    };

    inst->h_track = 0;
    ret = uapi_snd_get_track_default_attr(&track_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_get_track_default_attr, ret);
        return ret;
    }

    ret = uapi_snd_create_track(&inst->h_track, h_snd, &track_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_create_track, ret);
        return ret;
    }

    ret = uapi_snd_set_track_volume(inst->h_track, &gain, TD_NULL);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_set_track_volume, ret);
        goto out;
    }

    return EXT_SUCCESS;
out:
    (td_void)uapi_snd_destroy_track(inst->h_track);
    return ret;
}

static td_void sample_ao_track_stop_inst(sample_ao_track_inst *inst)
{
    td_s32 ret;

    if (inst->task != TD_NULL) {
        inst->task_active = TD_FALSE;
        athread_exit(inst->task);
        inst->task = TD_NULL;
    }

    ret = uapi_snd_track_stop(inst->h_track);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_track_stop, ret);
    }
}

static td_s32 sample_ao_track_start_inst(sample_ao_track_inst *inst)
{
    td_s32 ret;
    athread_attr attr;

    ret = uapi_snd_track_start(inst->h_track, TD_NULL);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_track_start, ret);
    }

    inst->task_active = TD_TRUE;
    attr.priority = ATHREAD_PRIORITY_NORMAL;
    attr.stack_size = 0x1000; /* 4k */
    attr.name = "sample_ao_track";

    ret = athread_create(&inst->task, sample_ao_track_thread, (td_void *)inst, &attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(athread_create, ret);
        goto out;
    }

    return EXT_SUCCESS;
out:
    (td_void)uapi_snd_track_stop(inst->h_track);
    return ret;
}

static td_void sample_ao_track_close_inst(sample_ao_track_inst *inst)
{
    sample_ao_close_adp(inst);
    sample_ao_close_track(inst);
}

static td_s32 sample_ao_track_open_inst(td_handle h_snd, sample_ao_track_inst *inst)
{
    td_s32 ret;

    /*
     * 数据流向 adp --> track(snd)
     * 按照数据流向反方向打开实例
     * 按照数据方向attach output
     */
    ret = sample_ao_open_track(h_snd, inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_ao_open_track, ret);
        return ret;
    }

    ret = sample_ao_open_adp(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_ao_open_adp, ret);
        goto out0;
    }

    return EXT_SUCCESS;
out0:
    sample_ao_close_track(inst);
    return ret;
}

static td_void sample_ao_inst_init(sample_ao_inst *inst)
{
    inst->aef_profile = UAPI_SND_AEF_PROFILE_MUSIC; /* aef music scene in default */
    inst->snd_attr.port_attr[0].out_port = SND_OUT_PORT_DEFAULT;
    inst->snd_attr.channels = SND_OUT_CHANNEL_DEF;
    inst->snd_attr.bit_depth = SND_OUT_BIT_DEPTH_DEF;
    inst->snd_attr.sample_rate = SND_OUT_SAMPLE_RATE_DEF;
}

static td_void sample_ao_close_inst(sample_ao_inst *inst)
{
    td_u32 i;

    for (i = 0; i < inst->track_num; i++) {
        sample_ao_track_close_inst(&inst->track_inst[i]);
    }

    sample_ao_close_snd(inst);
    sample_ao_sys_deinit();
}

static td_s32 sample_ao_open_inst(sample_ao_inst *inst)
{
    td_s32 ret;
    td_u32 i;

    ret = sample_ao_sys_init();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_ao_sys_init, ret);
        return ret;
    }

    ret = sample_ao_open_snd(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_ao_open_snd, ret);
        goto out0;
    }

    sample_gui_aef_add_handle(GUI_AEF_AO, inst->h_snd);

    for (i = 0; i < inst->track_num; i++) {
        ret = sample_ao_track_open_inst(inst->h_snd, &inst->track_inst[i]);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(sample_ao_track_open_inst, ret);
            goto out0;
        }
    }

    return EXT_SUCCESS;
out0:
    sample_ao_close_inst(inst);
    return ret;
}

static td_void sample_ao_stop_inst(sample_ao_inst *inst)
{
    td_u32 i;

    for (i = 0; i < inst->track_num; i++) {
        sample_ao_track_stop_inst(&inst->track_inst[i]);
    }
}

static td_s32 sample_ao_start_inst(sample_ao_inst *inst)
{
    td_s32 ret;
    td_u32 i;

    for (i = 0; i < inst->track_num; i++) {
        ret = sample_ao_track_start_inst(&inst->track_inst[i]);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(sample_ao_track_start_inst, ret);
            sample_ao_stop_inst(inst);
            return ret;
        }
    }

    return EXT_SUCCESS;
}

static td_void sample_ao_track_close_file(sample_ao_track_inst *inst)
{
    if (inst->h_file != TD_NULL) {
        fclose(inst->h_file);
        inst->h_file = TD_NULL;
    }
}

static td_s32 sample_ao_track_open_file(sample_ao_track_inst *inst)
{
    if (strcmp(inst->in_stream, "array") == 0 || inst->in_stream == TD_NULL) {
        inst->h_file = TD_NULL;
        inst->read_frame = sample_ao_track_read_array;
        sap_alert_log_info("sample_ao read data from array.");
    } else if (strcmp(inst->in_stream, "flash") == 0 || strcmp(inst->in_stream, "null") == 0) {
        data_info_init(&inst->data_info, FPGA_FLASH_PCM_DATA_ADDR, FPGA_FLASH_PCM_SIZE);
        circ_buf_update_write_pos(&inst->data_info.cb, inst->data_info.cb.size - sizeof(td_s16));

        inst->h_file = TD_NULL;
        inst->read_frame = sample_ao_track_read_flash;
        sap_alert_log_info("sample_ao read data from flash.");
    } else {
        inst->read_frame = sample_ao_track_read_file;
        inst->h_file = sample_audio_open_input_stream(inst->in_stream);
        if (inst->h_file == TD_NULL) {
            sap_err_log_info("open file error!");
            sap_err_log_str(inst->in_stream);
            return EXT_FAILURE;
        }
        sap_alert_log_info("sample_ao read data from file.");
    }
    sap_alert_log_u32(inst->pcm_format.sample_rate);
    sap_alert_log_u32(inst->pcm_format.channels);
    sap_alert_log_u32(inst->pcm_format.bit_depth);

    return EXT_SUCCESS;
}

static td_void sample_ao_close_file(sample_ao_inst *inst)
{
    td_u32 i;

    for (i = 0; i < inst->track_num; i++) {
        sample_ao_track_close_file(&inst->track_inst[i]);
    }
}

static td_s32 sample_ao_open_file(sample_ao_inst *inst)
{
    td_u32 i;
    td_s32 ret;

    for (i = 0; i < inst->track_num; i++) {
        ret = sample_ao_track_open_file(&inst->track_inst[i]);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(sample_ao_track_open_file, ret);
            sample_ao_close_file(inst);
            return ret;
        }
    }

    return EXT_SUCCESS;
}

static td_void sample_ao_usage(const td_char *name)
{
    sap_printf("usage:  %s pcm_file0 volume0 sample_rate0 bit_width0 channels0 "
        "pcm_file1 volume1 sample_rate1 bit_width1 channels1 ...\n",
        name);
    sap_printf("examples:\n");
    sap_printf("  %s /mnt/data/test0.pcm 100 48000 16 2 /mnt/data/test1.pcm 100 44100 16 1\n", name);
    sap_printf("  %s null 100 48000 16 2 null 100 44100 16 1\n", name);
}

static inline td_void sample_ao_parse_pcm_format(const sample_ao_track_arg *arg, uapi_audio_pcm_format *pcm_format)
{
    pcm_format->sample_rate = (td_u32)strtoul(arg->sample_rate, TD_NULL, 0);
    pcm_format->bit_depth = (td_u32)strtoul(arg->bit_depth, TD_NULL, 0);
    pcm_format->channels = (td_u32)strtoul(arg->channels, TD_NULL, 0);
    /* 100 --> 10ms pcm data */
    pcm_format->sample_per_frame = pcm_format->sample_rate / 100;
}

static td_s32 sample_ao_parse_cmd_line(td_s32 argc, td_char *argv[], sample_ao_inst *inst)
{
    td_u32 i;
    sample_ao_arg *arg = (sample_ao_arg *)argv;
    const td_u32 track_arg_size = (td_u32)(sizeof(sample_ao_track_arg) / sizeof(td_char *));
    const td_u32 track_num = (td_u32)(argc - 1) / track_arg_size;
    if (argc <= 1 || track_num <= 0) {
        return EXT_FAILURE;
    }

    inst->snd_id = UAPI_SND_0;
    inst->track_num = track_num;
    for (i = 0; i < track_num; i++) {
        inst->track_inst[i].in_stream = arg->track_arg[i].in_stream;
        inst->track_inst[i].volume = (td_s32)strtol(arg->track_arg[i].volume, TD_NULL, 0);
        sample_ao_parse_pcm_format(&arg->track_arg[i], &inst->track_inst[i].pcm_format);
        inst->track_inst[i].frame_size = calc_pcm_frame_size(&inst->track_inst[i].pcm_format);
    }

    return EXT_SUCCESS;
}

static td_s32 sample_ao_entry(td_s32 argc, td_char *argv[])
{
    td_s32 ret;
    sample_ao_inst *inst = sample_ao_alloc_inst();
    if (inst == TD_NULL) {
        sap_err_log_info("sample_ao_alloc_inst failed");
        return EXT_FAILURE;
    }

    sample_ao_inst_init(inst);

    ret = sample_ao_parse_cmd_line(argc, argv, inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_ao_parse_cmd_line, ret);
        goto out0;
    }

    ret = sample_ao_open_file(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_ao_open_file, ret);
        goto out0;
    }

    ret = sample_ao_open_inst(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_ao_open_inst, ret);
        goto out1;
    }

    ret = sample_ao_start_inst(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_ao_start_inst, ret);
        goto out2;
    }

    sample_ao_show_inst();
    return EXT_SUCCESS;
out2:
    sample_ao_close_inst(inst);
out1:
    sample_ao_close_file(inst);
out0:
    sample_ao_free_inst(inst);
    return ret;
}

static td_s32 sample_ao_exit(td_void)
{
    sample_ao_inst *inst = sample_ao_get_inst();
    if (inst == TD_NULL) {
        sap_err_log_info("sample_ao_get_inst failed");
        return EXT_FAILURE;
    }

    sample_gui_aef_delete_handle(inst->h_snd);
    sample_ao_stop_inst(inst);
    sample_ao_close_inst(inst);
    sample_ao_close_file(inst);
    sample_ao_free_inst(inst);
    sample_ao_show_inst();
    return EXT_SUCCESS;
}

td_s32 sample_ao(td_s32 argc, td_char *argv[])
{
    if (argc <= 1) {
        sample_ao_usage(argv[0]);
        return EXT_SUCCESS;
    }

    if (strcmp(argv[1], "q") == 0) {
        return sample_ao_exit();
    } else {
        return sample_ao_entry(argc, argv);
    }
}

static td_void sample_ao_player_inst_init_cat1(sample_ao_inst *inst)
{
    if (inst->snd_attr.port_attr[0].out_port == SND_OUT_PORT_DEFAULT) { /* SmartPA output use default pcm format */
        return;
    }
    /* pcm format for CAT1 */
    inst->snd_attr.channels = SIO_CAT1_I2S_CHANNEL_DEF;
    inst->snd_attr.sample_rate = SIO_CAT1_I2S_SAMPLE_RATE_DEF;
    inst->aef_profile = UAPI_SND_AEF_PROFILE_NONE;
}

td_s32 sample_ao_player_open(sample_escast_arg *escast_arg, uapi_snd snd_id)
{
    td_s32 ret;
    sample_ao_inst *inst = sample_ao_alloc_inst();
    if (inst == TD_NULL) {
        sap_err_log_info("sample_ao_alloc_inst failed");
        return EXT_FAILURE;
    }

    if (escast_arg == TD_NULL || escast_arg->player == TD_NULL ||
        escast_arg->in_arg == TD_NULL || escast_arg->snd_attr == TD_NULL) {
        return EXT_FAILURE;
    }

    sample_ao_inst_init(inst);

    *escast_arg->player = 0;

    inst->snd_id = snd_id;
    inst->track_num = 1;

    /* Set track input PCM format */
    (td_void)memcpy_s(&inst->track_inst[0].pcm_format, sizeof(uapi_audio_pcm_format),
                      &escast_arg->in_arg->pcm_format, sizeof(escast_arg->in_arg->pcm_format));

    /* Set track output PCM format(sound output format) */
    (td_void)memcpy_s(&inst->snd_attr, sizeof(inst->snd_attr), escast_arg->snd_attr, sizeof(*escast_arg->snd_attr));

    /* Set aef profile type */
    inst->aef_profile = escast_arg->aef_profile;

    sample_ao_player_inst_init_cat1(inst); /* Set pcm/i2s attr for cat1 */

    ret = sample_ao_open_inst(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_ao_open_inst, ret);
        goto out0;
    }

    ret = uapi_snd_track_start(inst->track_inst[0].h_track, TD_NULL);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_track_start, ret);
        goto out1;
    }

    *escast_arg->player = inst->track_inst[0].h_adp;
    *escast_arg->snd = inst->h_snd;

    sample_ao_show_inst();
    return EXT_SUCCESS;
out1:
    sample_ao_close_inst(inst);
out0:
    sample_ao_free_inst(inst);
    return ret;
}

td_void sample_ao_player_close(td_void)
{
    td_s32 ret;
    sample_ao_inst *inst = sample_ao_get_inst();
    if (inst == TD_NULL) {
        sap_err_log_info("sample_ao_get_inst failed");
        return;
    }

    ret = uapi_snd_track_stop(inst->track_inst[0].h_track);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_track_stop, ret);
        return;
    }

    sample_ao_close_inst(inst);
    sample_ao_free_inst(inst);
    sample_ao_show_inst();
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

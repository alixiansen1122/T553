/*
 * Copyright (c) CompanyNameMagicTag 2019-2020. All rights reserved.
 * Description: decode sample
 * Author: audio
 * Create: 2019-09-17
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "thread_os.h"
#include "osal_list.h"
#include "securec.h"
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

#define SEND_DATA_MODE 1 /* 0: Use get/put buffer interface; 1: Use send_stream interface; */
#define MAX_ADEC_NUM 2
#define ESPLAY_SND_IDX UAPI_SND_0
#define ESCAST_SND_IDX UAPI_SND_1
#define HEX_FORMATE_STEP 16
#define AEF_ARGC_NUM 4
#define SAID_BEAT_ARGC_NUM 3
#define AEF_TYPE_POS 2
#define SAID_BEAT_POS 2
#define AEF_ENABLE_POS 3

typedef union {
    uapi_acodec_opus_dec_priv_cfg opus_priv_cfg;
} sample_decode_priv_cfg;

typedef struct {
    td_bool task_active;
    athread_handle task;
    td_u8 frame_buf[2048]; /* 2048 frame size */

    const td_char *in_stream;
    const td_char *out_stream;
    FILE *in_file;
    FILE *out_file;
    td_slong in_file_len;

    sample_acodec_arg acodec_arg;

    td_u32 stream_addr;
    td_u32 stream_size;
    td_handle h_adp_in;
    td_handle h_adp_out;
    td_handle h_adec;
    td_handle h_track;
    data_info reader;
    td_bool play_once;
    sample_decode_priv_cfg priv_cfg;
} sample_decode_inst;

typedef struct {
    td_bool play_once;
    td_handle h_snd;
    uapi_snd snd_id;
    uapi_snd_attr snd_attr;
    uapi_snd_gain gain;
    uapi_snd_aef_profile aef_profile;
    td_u32 decode_num;
    sample_decode_inst decode_inst[MAX_ADEC_NUM];
    struct osal_list_head node;
} sample_decode_ao_inst;

static OSAL_LIST_HEAD(g_sample_decode_list);

typedef struct {
    const td_char *name;
    uapi_aef_type aef_type;
} aef_type_map;

static aef_type_map g_aef_type_map[] = {
    {"sws1", UAPI_AEF_TYPE_SWS1},
    {"said", UAPI_AEF_TYPE_SAID},
};

static td_void sample_decode_show_inst(td_void)
{
    td_u32 cnt = 0;
    sample_decode_ao_inst *inst = TD_NULL;

    if (osal_list_empty(&g_sample_decode_list) != TD_FALSE) {
        sap_alert_log_info("sample_decode all exit!");
        return;
    }

    osal_list_for_each_entry(inst, &g_sample_decode_list, node) {
        cnt++;
        sap_alert_log_u32(cnt);
        sap_alert_log_u32(inst->snd_id);
        sap_alert_log_u32(inst->decode_num);
    }
}

static sample_decode_ao_inst *sample_decode_alloc_inst(td_void)
{
    sample_decode_ao_inst *inst = (sample_decode_ao_inst *)malloc(sizeof(sample_decode_ao_inst));
    if (inst == TD_NULL) {
        return TD_NULL;
    }

    memset_s(inst, sizeof(*inst), 0, sizeof(sample_decode_ao_inst));

    osal_list_add_tail(&inst->node, &g_sample_decode_list);
    return inst;
}

static td_void sample_decode_free_inst(sample_decode_ao_inst *inst)
{
    if (inst == TD_NULL) {
        return;
    }

    osal_list_del(&inst->node);
    free(inst);
}

static sample_decode_ao_inst *sample_decode_get_inst(uapi_snd snd_id)
{
    sample_decode_ao_inst *inst = TD_NULL;

    osal_list_for_each_entry(inst, &g_sample_decode_list, node) {
        if (inst->snd_id == snd_id) {
            return inst;
        }
    }

    return TD_NULL;
}

static td_s32 sample_decode_parse_addr(td_s32 opt, const td_char *opt_arg, sample_decode_inst *inst)
{
    if (strncasecmp(opt_arg, "0x", 2) != 0) { /* 2 is the number of comparison */
        sap_printf("invalid address prefix, should be '0x' or '0X'\n");
        return EXT_FAILURE;
    }

    inst->stream_addr = (td_u32)strtoul(opt_arg, NULL, HEX_FORMATE_STEP); /* 16 represents Hexadecimal */
    sap_printf("get input addr 0x%x\n", inst->stream_addr);

    sap_unused(opt);
    return EXT_SUCCESS;
}

static td_void sample_decode_reader_init(sample_decode_inst *inst)
{
    data_info_init(&inst->reader, inst->stream_addr, inst->stream_size);
    circ_buf_update_write_pos(&inst->reader.cb, inst->reader.cb.size - sizeof(td_s16));
}

static td_void sample_decode_close_file(sample_decode_inst *inst)
{
    if (inst->out_file != TD_NULL) {
        fclose(inst->out_file);
        inst->out_file = TD_NULL;
    }

    if (inst->in_file != TD_NULL) {
        fclose(inst->in_file);
        inst->in_file = TD_NULL;
    }
}

static td_s32 sample_decode_open_file(sample_decode_inst *inst)
{
    sample_decode_reader_init(inst);

    /* open input file */
    inst->in_file = TD_NULL;

    if (inst->in_stream != TD_NULL && strcmp(inst->in_stream, "null") != 0) {
        inst->in_file = sample_audio_open_input_stream(inst->in_stream);
        if (inst->in_file == TD_NULL) {
            sap_printf("open file %s error!\n", inst->in_stream);
            return EXT_FAILURE;
        }
        fseek(inst->in_file, 0, SEEK_END);
        inst->in_file_len = ftell(inst->in_file);
        rewind(inst->in_file);
    } else {
        /* check read flash addr and len set */
        if (inst->stream_addr == 0 || inst->stream_size == 0) {
            sap_printf("need to set flash addr and len to read first!\n");
            goto out;
        }
    }

    /* open output file */
    inst->out_file = TD_NULL;

    if (inst->out_stream != TD_NULL && strcmp(inst->out_stream, "null") != 0) {
        inst->out_file = fopen(inst->out_stream, "wb");
        if (inst->out_file == TD_NULL) {
            sap_printf("open file %s error!\n", inst->out_stream);
            goto out;
        }
    }

    return 0;
out:
    if (inst->in_file != TD_NULL) {
        fclose(inst->in_file);
        inst->in_file = TD_NULL;
    }

    return EXT_FAILURE;
}

static td_void sample_decode_ao_close_file(sample_decode_ao_inst *inst)
{
    td_u32 i;

    for (i = 0; i < inst->decode_num; i++) {
        sample_decode_close_file(&inst->decode_inst[i]);
    }
}

static td_s32 sample_decode_ao_open_file(sample_decode_ao_inst *inst)
{
    td_u32 i;
    td_s32 ret;

    for (i = 0; i < inst->decode_num; i++) {
        ret = sample_decode_open_file(&inst->decode_inst[i]);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(sample_decode_open_file, ret);
            sample_decode_ao_close_file(inst);
            return ret;
        }
    }

    return EXT_SUCCESS;
}

static td_u32 read_es_from_flash(sample_decode_inst *inst, td_u8 *to, td_u32 len)
{
    circ_buf *cb = &inst->reader.cb;

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

    if (circ_buf_read(cb, to, len) != len) {
        sap_err_log_info("Fatal Error: circ_buf_read error");
        return 0;
    }

    circ_buf_update_write_pos(cb, len);
    return len;
}

static td_void sample_decode_rewind_file(sample_decode_inst *inst)
{
    rewind(inst->in_file);

    if (inst->acodec_arg.acodec_id == UAPI_ACODEC_ID_FLAC) {
        flac_skip_header(inst->in_file);
    }
}

static td_u32 sample_decode_read_file(sample_decode_inst *inst, uapi_stream_buf *stream)
{
    td_u32 read_len;

    read_len = fread(stream->data, 1, stream->size, inst->in_file);
    if (read_len == 0) {
        if (inst->play_once != TD_TRUE) {
            sample_decode_rewind_file(inst);
            sap_printf("rewind file %s\n", inst->in_stream);
        }
        return 0;
    } else {
        stream->size = read_len;
        stream->pts = 0LL;
        stream->eos = ((inst->in_file_len == ftell(inst->in_file)) && (inst->play_once == TD_TRUE));
        return stream->size;
    }
}

static td_u32 sample_decode_read_es(sample_decode_inst *inst, uapi_stream_buf *stream)
{
    if (inst->in_file != TD_NULL) {
        return sample_decode_read_file(inst, stream);
    } else {
        return read_es_from_flash(inst, stream->data, stream->size);
    }
}

#if SEND_DATA_MODE
static td_void sample_decode_play_task(td_void *arg)
{
    td_s32 ret;
    uapi_stream_buf stream;
    sample_decode_inst *inst = (sample_decode_inst *)arg;
    td_u32 read_len = (td_u32)sizeof(inst->frame_buf);

    stream.data = inst->frame_buf;
    stream.pts = 0LL;
    stream.eos = TD_FALSE;
    stream.pkg_loss = TD_FALSE;

    sap_alert_log_info("thread enter.");
    while (inst->task_active) {
        uapi_adp_query_free(inst->h_adp_in, &read_len);
        read_len = circ_buf_min(read_len, sizeof(inst->frame_buf));
        if (read_len < FILE_READ_LEN_MIN) {
            sap_msleep(THREAD_SLEEP_10MS);
            continue;
        }

        stream.size = read_len;
        stream.size = sample_decode_read_es(inst, &stream);
        if (stream.size == 0) {
            if (inst->play_once == TD_TRUE) {
                sap_printf("end of file %s", inst->in_stream);
                break;
            } else {
                sap_msleep(THREAD_SLEEP_10MS);
                continue;
            }
        }

        while (inst->task_active) {
            ret = uapi_adp_send_stream(inst->h_adp_in, &stream);
            if (ret != EXT_SUCCESS) {
                sap_msleep(THREAD_SLEEP_10MS);
                continue;
            }
            break;
        }
    }

    sap_alert_log_info("thread exit.");
    athread_set_exit(inst->task, TD_TRUE);
}

#else

static td_void sample_decode_play_task(td_void *arg)
{
    td_s32 ret;
    td_u32 read_len;
    uapi_stream_buf stream;
    sample_decode_inst *inst = (sample_decode_inst *)arg;

    stream.data = inst->frame_buf;
    stream.size = sizeof(inst->frame_buf);
    stream.pts = 0LL;
    stream.eos = TD_FALSE;
    stream.pkg_loss = TD_FALSE;

    while (inst->task_active) {
        /* Use get/put buffer to send data */
        while (inst->task_active) {
            uapi_adp_query_free(inst->h_adp_in, &read_len);
            if (read_len < sizeof(inst->frame_buf)) {
                sap_msleep(THREAD_SLEEP_10MS);
                continue;
            }

            ret = uapi_adp_get_buffer(inst->h_adp_in, &stream);
            if (ret != EXT_SUCCESS) {
                sap_msleep(THREAD_SLEEP_10MS);
                continue;
            }
            break;
        }

        read_len = sample_decode_read_es(inst, &stream);
        if (read_len == 0) {
            continue;
        }

        ret = uapi_adp_put_buffer(inst->h_adp_in, &stream);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_adp_put_buffer, ret);
        }
    }

    athread_set_exit(inst->task, TD_TRUE);
}
#endif

static td_s32 sample_decode_sys_init(td_void)
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
        goto out0;
    }

    ret = uapi_adec_init();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adec_init, ret);
        goto out1;
    }

    return EXT_SUCCESS;
out1:
    (td_void)uapi_snd_deinit();
out0:
    (td_void)uapi_adp_deinit();
    return ret;
}

static td_void sample_decode_sys_deinit(td_void)
{
    td_s32 ret;

    ret = uapi_adec_deinit();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adec_deinit, ret);
    }

    ret = uapi_snd_deinit();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_deinit, ret);
    }

    ret = uapi_adp_deinit();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_deinit, ret);
    }
}

static td_void sample_decode_close_adp(sample_decode_inst *inst)
{
    td_s32 ret;

    if (inst->h_adp_in == 0) {
        return;
    }

    ret = uapi_adp_detach_output(inst->h_adp_in, inst->h_adec);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_detach_output, ret);
    }

    ret = uapi_adp_destroy(inst->h_adp_in);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_destroy, ret);
    }

    inst->h_adp_in = 0;
}

static td_s32 sample_decode_adp_event_proc(td_handle adp, uapi_adp_event_type event,
    const td_void *param, const td_void *context)
{
    sap_trace_log_h32(adp);
    sap_trace_log_u32(event);

    sap_unused(adp);
    sap_unused(event);
    sap_unused(param);
    sap_unused(context);
    return EXT_SUCCESS;
}

static td_s32 sample_decode_open_adp(sample_decode_inst *inst)
{
    td_s32 ret;
    uapi_adp_attr adp_attr;

    ret = uapi_adp_get_def_attr(&adp_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_get_def_attr, ret);
        return ret;
    }

    inst->h_adp_in = 0;
    ret = uapi_adp_create(&inst->h_adp_in, &adp_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_create, ret);
        return ret;
    }

    ret = uapi_adp_register_event_proc(inst->h_adp_in, (uapi_adp_event_proc)sample_decode_adp_event_proc, TD_NULL);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_register_event_proc, ret);
        goto out;
    }

    ret = uapi_adp_attach_output(inst->h_adp_in, inst->h_adec);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_attach_output, ret);
        goto out;
    }

    return EXT_SUCCESS;
out:
    (td_void)uapi_adp_destroy(inst->h_adp_in);
    return ret;
}

static td_void sample_decode_close_snd(sample_decode_ao_inst *inst)
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

static td_void sample_decode_get_def_attr_snd(sample_decode_ao_inst *inst)
{
    inst->snd_attr.port_num = 1;
    inst->snd_attr.port_attr[0].out_port = SND_OUT_PORT_DEFAULT;
    inst->snd_attr.channels = SND_OUT_CHANNEL_DEF;
    inst->snd_attr.bit_depth = SND_OUT_BIT_DEPTH_DEF;
    inst->snd_attr.sample_rate = SND_OUT_SAMPLE_RATE_DEF;
}

static td_void sample_decode_set_i2s_attr(uapi_snd_attr *snd_attr)
{
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

static td_s32 sample_decode_open_snd(sample_decode_ao_inst *inst)
{
    td_s32 ret;
    uapi_snd_attr snd_attr;

    inst->h_snd = 0;
    ret = uapi_snd_get_default_attr(inst->snd_id, &snd_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_get_default_attr, ret);
        return ret;
    }

    snd_attr.port_num = inst->snd_attr.port_num;
    snd_attr.port_attr[0].out_port = inst->snd_attr.port_attr[0].out_port;
    snd_attr.channels = inst->snd_attr.channels;
    snd_attr.bit_depth = inst->snd_attr.bit_depth;
    snd_attr.sample_rate = inst->snd_attr.sample_rate;

    sample_decode_set_i2s_attr(&snd_attr);

    ret = uapi_snd_open(&inst->h_snd, inst->snd_id, &snd_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_open, ret);
        return ret;
    }

    ret = uapi_snd_set_volume(inst->h_snd, snd_attr.port_attr[0].out_port, &inst->gain, TD_NULL);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_set_volume, ret);
        goto out;
    }

    ret = uapi_snd_set_aef_profile(inst->h_snd, inst->aef_profile);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_set_aef_profile, ret);
        goto out;
    }

    return EXT_SUCCESS;
out:
    (td_void)uapi_snd_close(inst->h_snd);
    return ret;
}

static td_void sample_decode_close_track(sample_decode_inst *inst)
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

static td_s32 sample_decode_open_track(td_handle h_snd, sample_decode_inst *inst)
{
    td_s32 ret;
    uapi_snd_track_attr track_attr;

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

    return EXT_SUCCESS;
}

static td_void sample_decode_close_adec(sample_decode_inst *inst)
{
    td_s32 ret;

    if (inst->h_adec == 0) {
        return;
    }

    ret = uapi_adec_detach_output(inst->h_adec, inst->h_track);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adec_detach_output, ret);
    }

    ret = uapi_adec_destroy(inst->h_adec);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adec_destroy, ret);
    }

    inst->h_adec = 0;
}

static td_void sample_decode_adec_attr_cfg(const sample_decode_inst *inst, uapi_adec_attr *adec_attr)
{
    adec_attr->codec_id = inst->acodec_arg.acodec_id;
    adec_attr->param.sample_rate = inst->acodec_arg.pcm_format.sample_rate;
    adec_attr->param.channels = inst->acodec_arg.pcm_format.channels;
    adec_attr->param.bit_depth = inst->acodec_arg.pcm_format.bit_depth;

    sap_alert_log_u32(inst->acodec_arg.pcm_format.sample_rate);
    sap_alert_log_u32(inst->acodec_arg.pcm_format.channels);
    sap_alert_log_u32(inst->acodec_arg.pcm_format.bit_depth);

    if (inst->acodec_arg.acodec_id == UAPI_ACODEC_ID_OPUS) {
        if (inst->priv_cfg.opus_priv_cfg.frame_length != 0) {
            adec_attr->param.private_data = &(inst->priv_cfg.opus_priv_cfg);
            adec_attr->param.private_data_size = sizeof(inst->priv_cfg.opus_priv_cfg);
        }
    }
}

static td_s32 sample_decode_open_adec(sample_decode_inst *inst)
{
    td_s32 ret;
    uapi_adec_attr adec_attr;

    inst->h_adec = 0;
    ret = uapi_adec_create(&inst->h_adec);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adec_create, ret);
        return ret;
    }

    ret = uapi_adec_get_attr(inst->h_adec, &adec_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adec_get_attr, ret);
        goto out;
    }

    sample_decode_adec_attr_cfg(inst, &adec_attr);

    ret = uapi_adec_set_attr(inst->h_adec, &adec_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adec_set_attr, ret);
        goto out;
    }

    ret = uapi_adec_attach_output(inst->h_adec, inst->h_track);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adec_attach_output, ret);
        goto out;
    }

    return EXT_SUCCESS;
out:
    (td_void)uapi_adec_destroy(inst->h_adec);
    return ret;
}

static td_void sample_decode_close_inst(sample_decode_inst *inst)
{
    sample_decode_close_adp(inst);
    sample_decode_close_adec(inst);
    sample_decode_close_track(inst);
}

static td_s32 sample_decode_open_inst(td_handle h_snd, sample_decode_inst *inst)
{
    td_s32 ret;

    /*
     * 数据流向 adp --> adec --> track(snd)
     * 按照数据流向反方向打开实例
     * 按照数据方向attach output
     */
    ret = sample_decode_open_track(h_snd, inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_decode_open_track, ret);
        return ret;
    }

    ret = sample_decode_open_adec(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_decode_open_adec, ret);
        goto out0;
    }

    ret = sample_decode_open_adp(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_decode_open_adp, ret);
        goto out1;
    }

    return EXT_SUCCESS;
out1:
    sample_decode_close_adec(inst);
out0:
    sample_decode_close_track(inst);
    return ret;
}

static td_void sample_decode_ao_close_inst(sample_decode_ao_inst *inst)
{
    td_u32 i;

    for (i = 0; i < inst->decode_num; i++) {
        sample_decode_close_inst(&inst->decode_inst[i]);
    }

    sample_decode_close_snd(inst);
    sample_decode_sys_deinit();
}

static td_s32 sample_decode_ao_open_inst(sample_decode_ao_inst *inst)
{
    td_u32 i;
    td_s32 ret;

    ret = sample_decode_sys_init();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_decode_sys_init, ret);
        return ret;
    }

    ret = sample_decode_open_snd(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_decode_open_snd, ret);
        goto out0;
    }

    for (i = 0; i < inst->decode_num; i++) {
        ret = sample_decode_open_inst(inst->h_snd, &inst->decode_inst[i]);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(sample_decode_open_inst, ret);
            goto out0;
        }

        inst->decode_inst[i].play_once = inst->play_once;
    }

    return EXT_SUCCESS;
out0:
    sample_decode_ao_close_inst(inst);
    return ret;
}

static td_void sample_decode_stop_inst(sample_decode_inst *inst)
{
    td_s32 ret;

    if (inst->task != TD_NULL) {
        if (inst->task_active) {
            inst->task_active = TD_FALSE;
            athread_exit(inst->task);
        }

        inst->task = TD_NULL;
    }

    ret = uapi_adec_stop(inst->h_adec);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adec_stop, ret);
    }

    ret = uapi_snd_track_stop(inst->h_track);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_track_stop, ret);
    }
}

static td_s32 sample_decode_start_inst(sample_decode_inst *inst)
{
    td_s32 ret;
    athread_attr attr;

    ret = uapi_snd_track_start(inst->h_track, TD_NULL);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_track_start, ret);
        return ret;
    }

    ret = uapi_adec_start(inst->h_adec);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adec_start, ret);
        goto out0;
    }

    inst->task_active = TD_TRUE;
    attr.priority = ATHREAD_PRIORITY_NORMAL;
    attr.stack_size = 0x1000; /* 4k */
    attr.name = "sample_adec_play";
    ret = athread_create(&inst->task, sample_decode_play_task, (td_void *)inst, &attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(athread_create, ret);
        goto out1;
    }

    return EXT_SUCCESS;
out1:
    uapi_adec_stop(inst->h_adec);
out0:
    uapi_snd_track_stop(inst->h_track);
    return ret;
}

static td_void sample_decode_ao_stop_inst(sample_decode_ao_inst *inst)
{
    td_u32 i;

    for (i = 0; i < inst->decode_num; i++) {
        sample_decode_stop_inst(&inst->decode_inst[i]);
    }
}

static td_s32 sample_decode_ao_start_inst(sample_decode_ao_inst *inst)
{
    td_u32 i;
    td_s32 ret;

    for (i = 0; i < inst->decode_num; i++) {
        ret = sample_decode_start_inst(&inst->decode_inst[i]);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(sample_decode_start_inst, ret);
            sample_decode_ao_stop_inst(inst);
            return ret;
        }
    }

    return EXT_SUCCESS;
}

static td_void sample_decode_usage(td_void)
{
    sap_printf("\n"
           "usage:  sample_decode -i [InputFileName] [OPTIONS]\n"
           "\n");
    sap_printf("        -h     show this usage message and abort\n"
           "        -i     input file name[read flash when no file specified]\n"
           "        -a     specify addr to read from flash\n"
           "        -L     specify stream len to read from flash(Btye)\n"
           "        -f     opus dec frame length(25/50/100/200/400/600/800/1000/1200)\n"
           "\n");
    sap_printf("        -t     the type of play audio(aac/mp3/opus/lc3/l2hc/sbc/msbc/flac)\n"
           "        -c     channels of the input stream(default: 2)\n"
           "        -b     bit_depth of the input stream(default: 16)\n"
           "        -s     samplerate of the input stream(default: 48000)\n"
           "        -o     the output file name to save\n"
           "        -v     set play volume(default: 0dB)\n"
           "\n");
    sap_printf("        -l          specify one long argument\n"
           "        -l once     play input stream once and deal eos event\n");
    sap_printf("\n"
           "for example: ./sample_decode -i /mnt/dada.sbc -t sbc -c 2 -o /mnt/dada.pcm\n"
           "             ./sample_decode -i /mnt/dada.aac -t aac\n"
           "             ./sample_decode -a 0x105D8000 -L 500036 -t mp3 -b 32 (read mp3 data from flash)\n"
           "\n");
}

td_void sample_decode_get_frame_length(const td_char *opt_arg, sample_decode_inst *inst)
{
    /* opus dec frame length */
    inst->priv_cfg.opus_priv_cfg.frame_length = (td_u32)strtoul(opt_arg, TD_NULL, 0);
    sap_printf("opus dec frame length %u\n", inst->priv_cfg.opus_priv_cfg.frame_length);
}

static td_s32 sample_decode_parse_opt(td_char opt, const td_char *opt_arg, sample_decode_inst *inst)
{
    if (opt == 'h') {
        sample_decode_usage();
        return EXT_FAILURE;
    }

    if (opt_arg == TD_NULL) {
        sap_printf("invalid option -- '%c'\n", opt);
        return EXT_FAILURE;
    }

    switch (opt) {
        case 'i':
            inst->in_stream = opt_arg;
            parse_audio_params_from_file(inst->in_stream, &inst->acodec_arg.pcm_format);
            break;
        case 'o':
            inst->out_stream = opt_arg;
            break;
        case 's':
            inst->acodec_arg.pcm_format.sample_rate = (td_u32)strtoul(opt_arg, TD_NULL, 0);
            break;
        case 'c':
            inst->acodec_arg.pcm_format.channels = (td_u32)strtoul(opt_arg, TD_NULL, 0);
            break;
        case 'b':
            inst->acodec_arg.pcm_format.bit_depth = (td_u32)strtoul(opt_arg, TD_NULL, 0);
            break;
        case 't': /* direct return */
            return sample_audio_get_acodec_id(opt_arg, &inst->acodec_arg.acodec_id);
        case 'a':
            return sample_decode_parse_addr(opt, opt_arg, inst);
        case 'L':
            inst->stream_size = (td_u32)strtoul(opt_arg, TD_NULL, 0);
            break;
        case 'f':
            sample_decode_get_frame_length(opt_arg, inst);
        case 'v':
        case 'l':
            break;
        default:
            sap_printf("invalid command line\n");
            sample_decode_usage();
            return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

static td_void sample_decode_parse_volume(td_char opt, const td_char *opt_arg, sample_decode_ao_inst *inst)
{
    td_double volume;
    td_s32 volume_integer;
    td_s32 volume_decimal;

    if (opt != 'v') {
        return;
    }

    if (opt_arg == TD_NULL) {
        sap_printf("invalid option -- '%c'\n", opt);
        return;
    }

    volume = strtod(opt_arg, TD_NULL);

    volume_integer = (td_s32)volume;

    /* Multiply by 1000.0f, decimal part of high preicision gain */
    volume = (volume - (td_double)volume_integer) * 1000.0f;
    volume_decimal = (td_s32)volume;

    inst->gain.integer = volume_integer;
    inst->gain.decimal = volume_decimal;
}

static td_void sample_decode_parse_long_opt(td_char opt, const td_char *opt_arg, sample_decode_ao_inst *inst)
{
    if (opt != 'l') {
        return;
    }

    if (opt_arg == TD_NULL) {
        sap_printf("invalid option -- '%c'\n", opt);
        return;
    }

    if (strcmp(opt_arg, "once") == 0) {
        inst->play_once = TD_TRUE;
    }
}

/* sample_decode_parse_cmd_line: parses the command-line input */
static td_s32 sample_decode_parse_cmd_line(td_s32 argc, td_char *argv[], sample_decode_ao_inst *inst)
{
    parg_state arg_parse;
    td_s32 ret;
    td_u32 i;
    td_u32 decode_num = 0;
    td_u32 step = 0;
    td_char opt;

    /* check input arguments */
    if ((argv == TD_NULL) || (argc < 0x2)) {
        goto out;
    }

    parg_init(&arg_parse);
    while (1) {
        ret = parg_getopt(&arg_parse, argc, argv, "i:o:s:c:b:t:v:l:a:L:f:h");
        if (ret == -1) {
            break;
        }

        opt = (td_char)ret;
        sample_decode_parse_volume(opt, arg_parse.optarg, inst);
        sample_decode_parse_long_opt(opt, arg_parse.optarg, inst);

        if (opt == 'i') {
            decode_num += step;
            step = 1;
        }

        if (sample_decode_parse_opt(opt, arg_parse.optarg, &inst->decode_inst[decode_num]) != EXT_SUCCESS) {
            return EXT_FAILURE;
        }
    }

    inst->decode_num = decode_num + 1;
    for (i = 0; i < inst->decode_num; i++) {
        if (inst->decode_inst[i].acodec_arg.acodec_id == UAPI_ACODEC_ID_MAX) {
            goto out;
        }
    }

    return EXT_SUCCESS;
out:
    sample_decode_usage();
    return EXT_FAILURE;
}

static td_void sample_decode_codec_arg_init(sample_acodec_arg *acodec_arg)
{
    acodec_arg->pcm_format.sample_rate = UAPI_AUDIO_SAMPLE_RATE_16K;
    acodec_arg->pcm_format.channels = UAPI_AUDIO_CHANNEL_2;
    acodec_arg->pcm_format.bit_depth = UAPI_AUDIO_BIT_DEPTH_16;
    acodec_arg->acodec_id = UAPI_ACODEC_ID_MAX;
}

static td_void sample_decode_priv_cfg_init(sample_decode_priv_cfg *priv_cfg)
{
    priv_cfg->opus_priv_cfg.frame_length = 0;
}

static td_void sample_decode_inst_init(sample_decode_ao_inst *inst)
{
    td_u32 i;

    inst->snd_id = ESPLAY_SND_IDX;
    inst->decode_num = 0;
    inst->gain.integer = 0;
    inst->gain.decimal = 0;
    inst->aef_profile = UAPI_SND_AEF_PROFILE_MUSIC; /* aef not bypass in default */

    for (i = 0; i < MAX_ADEC_NUM; i++) {
        sample_decode_codec_arg_init(&inst->decode_inst[i].acodec_arg);
        sample_decode_priv_cfg_init(&inst->decode_inst[i].priv_cfg);
        inst->decode_inst[i].stream_addr = 0x0;
        inst->decode_inst[i].stream_size = 0x0;
    }
}

static td_s32 sample_decode_entry(td_s32 argc, td_char *argv[])
{
    td_s32 ret;
    sample_decode_ao_inst *inst = sample_decode_alloc_inst();
    if (inst == TD_NULL) {
        sap_printf("sample_decode_alloc_inst failed\n");
        return EXT_FAILURE;
    }

    sample_decode_inst_init(inst);

    ret = sample_decode_parse_cmd_line(argc, argv, inst);
    if (ret != EXT_SUCCESS) {
        goto out0;
    }

    ret = sample_decode_ao_open_file(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_decode_ao_open_file, ret);
        goto out0;
    }

    sample_decode_get_def_attr_snd(inst);

    ret = sample_decode_ao_open_inst(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_decode_ao_open_inst, ret);
        goto out1;
    }

    ret = sample_decode_ao_start_inst(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_decode_ao_start_inst, ret);
        goto out2;
    }

    sample_decode_show_inst();
    return EXT_SUCCESS;
out2:
    sample_decode_ao_close_inst(inst);
out1:
    sample_decode_ao_close_file(inst);
out0:
    sample_decode_free_inst(inst);
    return ret;
}

static td_s32 sample_decode_exit(td_void)
{
    sample_decode_ao_inst *inst = sample_decode_get_inst(ESPLAY_SND_IDX);
    if (inst != TD_NULL) {
        sample_decode_ao_stop_inst(inst);
        sample_decode_ao_close_inst(inst);
        sample_decode_ao_close_file(inst);
        sample_decode_free_inst(inst);
        sample_decode_show_inst();
    }

    return EXT_SUCCESS;
}

static td_s32 sample_decode_set_aef_enable(td_s32 argc, td_char *argv[])
{
    td_s32 ret;
    td_u32 i;
    size_t num;
    uapi_aef_type aef_type;
    td_bool enable = TD_TRUE;
    sample_decode_ao_inst *inst = TD_NULL;

    if (argc != AEF_ARGC_NUM) {
        return EXT_FAILURE;
    }

    inst = sample_decode_get_inst(ESPLAY_SND_IDX);
    if (inst == TD_NULL) {
        return AUDIO_FAILURE;
    }

    num = sizeof(g_aef_type_map) / sizeof(g_aef_type_map[0]);
    aef_type = UAPI_AEF_TYPE_MAX;

    for (i = 0; i < num; i++) {
        if (strcmp(argv[AEF_TYPE_POS], g_aef_type_map[i].name) == 0) {
            aef_type = g_aef_type_map[i].aef_type;
            break;
        }
    }

    if (aef_type == UAPI_AEF_TYPE_MAX) {
        sap_err_log_info("invalid aef type");
        return AUDIO_FAILURE;
    }

    if ((strcmp(argv[AEF_ENABLE_POS], "on") == 0)) {
        enable = TD_TRUE;
    } else if ((strcmp(argv[AEF_ENABLE_POS], "off") == 0)) {
        enable = TD_FALSE;
    }

    ret = uapi_snd_set_port_aef_enable(inst->h_snd, inst->snd_attr.port_attr[0].out_port, aef_type, enable);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_set_port_aef_enable, ret);
        return ret;
    }
    return EXT_SUCCESS;
}

static td_s32 sample_decode_set_said_beat(td_s32 argc, td_char *argv[])
{
    td_s32 ret;
    td_u32 beat;
    td_u32 size;
    td_u32 i;
    sample_decode_ao_inst *inst = TD_NULL;

    if (argc != SAID_BEAT_ARGC_NUM) {
        return EXT_FAILURE;
    }

    inst = sample_decode_get_inst(ESPLAY_SND_IDX);
    if (inst == TD_NULL) {
        return AUDIO_FAILURE;
    }

    beat = (td_u32)strtoul(argv[SAID_BEAT_POS], TD_NULL, 0);
    size = sizeof(beat);

    ret = uapi_snd_set_aef_param(inst->h_snd, inst->snd_attr.port_attr[0].out_port, UAPI_AEF_TYPE_SAID, &beat, size);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_set_aef_param, ret);
        return ret;
    }
    return EXT_SUCCESS;
}

td_s32 sample_decode(td_s32 argc, td_char *argv[])
{
    if (argc <= 1) {
        sample_decode_usage();
        return EXT_SUCCESS;
    }

    if (strcmp(argv[1], "q") == 0) {
        return sample_decode_exit();
    } else if (strcmp(argv[1], "aef") == 0) {
        return sample_decode_set_aef_enable(argc, argv);
    } else if (strcmp(argv[1], "beat") == 0) {
        return sample_decode_set_said_beat(argc, argv);
    } else {
        return sample_decode_entry(argc, argv);
    }
}

static td_void sample_esplay_stop_inst(sample_decode_inst *inst)
{
    td_s32 ret;

    ret = uapi_adec_stop(inst->h_adec);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adec_stop, ret);
    }

    ret = uapi_snd_track_stop(inst->h_track);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_track_stop, ret);
    }
}

static td_s32 sample_esplay_start_inst(sample_decode_inst *inst)
{
    td_s32 ret;

    ret = uapi_snd_track_start(inst->h_track, TD_NULL);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_track_start, ret);
        return ret;
    }

    ret = uapi_adec_start(inst->h_adec);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adec_start, ret);
        goto out;
    }

    return EXT_SUCCESS;
out:
    (td_void)uapi_snd_track_stop(inst->h_track);
    return ret;
}

static td_s32 sample_esplay_init(sample_escast_arg *escast_arg, uapi_snd snd_id)
{
    td_s32 ret;
    sample_decode_inst *inst = TD_NULL;
    sample_decode_ao_inst *ao_inst = sample_decode_alloc_inst();
    if (ao_inst == TD_NULL) {
        sap_printf("sample_decode_alloc_inst failed\n");
        return EXT_FAILURE;
    }

    inst = &ao_inst->decode_inst[0];
    sample_decode_inst_init(ao_inst);
    ao_inst->snd_id = snd_id;
    ao_inst->decode_num = 1;

    /* 指定 adec 输入码流属性 */
    (td_void)memcpy_s(&inst->acodec_arg, sizeof(inst->acodec_arg), escast_arg->in_arg, sizeof(*escast_arg->in_arg));

    /* 指定 track 输出码流属性 */
    (td_void)memcpy_s(&ao_inst->snd_attr, sizeof(ao_inst->snd_attr),
        escast_arg->snd_attr, sizeof(*escast_arg->snd_attr));

    /* Set aef profile type */
    ao_inst->aef_profile = escast_arg->aef_profile;

    ret = sample_decode_ao_open_inst(ao_inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_decode_ao_open_inst, ret);
        goto out1;
    }

    ret = sample_esplay_start_inst(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_decode_start_inst, ret);
        goto out2;
    }

    *escast_arg->player = inst->h_adp_in;
    *escast_arg->snd = ao_inst->h_snd;

    sample_decode_show_inst();
    return EXT_SUCCESS;
out2:
    sample_decode_ao_close_inst(ao_inst);
out1:
    sample_decode_free_inst(ao_inst);
    return ret;
}

static td_void sample_esplay_deinit(uapi_snd snd_id)
{
    sample_decode_ao_inst *ao_inst = sample_decode_get_inst(snd_id);
    if (ao_inst != TD_NULL) {
        sample_decode_inst *inst = &ao_inst->decode_inst[0];

        sample_esplay_stop_inst(inst);
        sample_decode_ao_close_inst(ao_inst);
        sample_decode_ao_close_file(ao_inst);
        sample_decode_free_inst(ao_inst);
        sample_decode_show_inst();
    }
}

td_s32 sample_escast_open(sample_escast_arg *escast_arg)
{
    return sample_esplay_init(escast_arg, ESCAST_SND_IDX);
}

td_void sample_escast_close(td_void)
{
    sample_esplay_deinit(ESCAST_SND_IDX);
}

td_s32 sample_esplay_open(const sample_acodec_arg *acodec_arg, const td_handle *player,
                          uapi_snd_aef_profile aef_profile)
{
    td_handle h_snd;
    const uapi_snd_attr snd_attr = {
        .port_num = 1,
        .port_attr[0].out_port = SND_OUT_PORT_DEFAULT,
        .channels = SND_OUT_CHANNEL_DEF,
        .bit_depth = SND_OUT_BIT_DEPTH_DEF,
        .sample_rate = SND_OUT_SAMPLE_RATE_DEF,
    };

    sample_escast_arg escast_arg = {
        .player = (td_handle *)player,
        .in_arg = acodec_arg,
        .snd = &h_snd,
        .snd_attr = &snd_attr,
        .aef_profile = aef_profile,
    };

    return sample_esplay_init(&escast_arg, ESPLAY_SND_IDX);
}

td_void sample_esplay_close(td_void)
{
    sample_esplay_deinit(ESPLAY_SND_IDX);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

/*
 * Copyright (c) CompanyNameMagicTag 2019-2020. All rights reserved.
 * Description: encode sample
 * Author: audio
 * Create: 2019-09-17
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "thread_os.h"
#include "osal_list.h"
#include "securec.h"
#include "soc_uapi_adp.h"
#include "soc_uapi_aenc.h"
#include "soc_uapi_sound.h"
#include "sample_phone.h"
#include "sample_audio_api.h"
#include "sample_audio_utils.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef enum intact_mode {
    INTACT_MODE_ACORE_LOOP,
    INTACT_MODE_PCORE_LOOP,
    INTACT_MODE_PCORE_ONLY,
}intact_mode;

typedef struct {
    athread_handle task_in;
    athread_handle task_out;
    td_bool task_active;
    intact_mode data_loop_mode;
    FILE *h_file_in;
    FILE *h_file_out;

    td_u32 in_data_addr;
    td_u32 in_data_size;
    td_u32 out_data_addr;
    td_u32 out_data_size;

    td_u8 frame_buf[0x1000];
    td_u32 frame_size;

    td_handle h_adp_in;
    td_handle h_adp_out;
    td_handle h_adp_play;
    td_handle h_aenc;
    td_handle h_snd;

    sample_acodec_arg adec_arg;
    sample_acodec_arg aenc_arg;
    data_info read_data;
    data_info write_data;
    struct osal_list_head node;
} sample_cast_aenc_play_inst;

static OSAL_LIST_HEAD(g_sample_cast_aenc_play_list);

static sample_cast_aenc_play_inst *sample_cast_aenc_play_alloc_inst(td_void)
{
    sample_cast_aenc_play_inst *inst = (sample_cast_aenc_play_inst *)malloc(sizeof(sample_cast_aenc_play_inst));
    if (inst == TD_NULL) {
        return TD_NULL;
    }

    (td_void)memset_s(inst, sizeof(*inst), 0, sizeof(sample_cast_aenc_play_inst));

    osal_list_add_tail(&inst->node, &g_sample_cast_aenc_play_list);
    return inst;
}

static td_void sample_cast_aenc_play_free_inst(sample_cast_aenc_play_inst *inst)
{
    if (inst == TD_NULL) {
        return;
    }

    osal_list_del(&inst->node);
    free(inst);
}

static sample_cast_aenc_play_inst *sample_cast_aenc_play_get_inst(td_void)
{
    if (osal_list_empty(&g_sample_cast_aenc_play_list) != TD_FALSE) {
        return TD_NULL;
    }

    return osal_list_first_entry(&g_sample_cast_aenc_play_list, sample_cast_aenc_play_inst, node);
}

static sample_cast_aenc_play_inst g_sample_cast_aenc_play_inst;


static td_void cast_aenc_play_reader_reset(sample_cast_aenc_play_inst *aenc)
{
    circ_buf *cb = &aenc->read_data.cb;
    circ_buf_flush(cb);
    circ_buf_update_write_pos(cb, cb->size - sizeof(td_s16));
}

static td_u32 sample_cast_aenc_play_read_flash(sample_cast_aenc_play_inst *inst, uapi_stream_buf *stream)
{
    const td_u32 len = stream->size;
    circ_buf *cb = &inst->read_data.cb;

    if (len > cb->size) {
        sap_printf("Fatal Error: data not enough\n");
        return 0;
    }

    if (circ_buf_query_busy(cb) < len) {
        sap_printf("reset read\n");
        cast_aenc_play_reader_reset(inst);
    }

    return circ_buf_read(cb, (td_u8 *)stream->data, len);
}

static td_u32 sample_cast_aenc_play_read_file(sample_cast_aenc_play_inst *inst, uapi_stream_buf *stream)
{
    td_u32 len;

    len = fread(stream->data, 1, stream->size, inst->h_file_in);
    if (len == 0) {
        sap_printf("reset read\n");
        rewind(inst->h_file_in);
        return 0;
    }

    return len;
}

static td_u32 sample_cast_aenc_play_read(sample_cast_aenc_play_inst *inst, uapi_stream_buf *stream)
{
    if (inst->h_file_in != TD_NULL) {
        return sample_cast_aenc_play_read_file(inst, stream);
    } else {
        return sample_cast_aenc_play_read_flash(inst, stream);
    }
}

static td_void cast_aenc_play_adp_data_play_proc(td_void *args)
{
    td_s32 ret;
    uapi_stream_buf stream;
    sample_cast_aenc_play_inst *inst = (sample_cast_aenc_play_inst *)args;

    stream.pts = 0LL;
    stream.eos = TD_FALSE;
    stream.pkg_loss = TD_FALSE;

    sap_err_log_info("thread enter.");
    while (inst->task_active) {
        ret = uapi_adp_acquire_stream(inst->h_adp_out, &stream);
        if (ret != EXT_SUCCESS) {
            sap_msleep(THREAD_SLEEP_10MS);
            continue;
        }

        if (stream.size >= FILE_READ_LEN) {
            stream.size = FILE_READ_LEN;
        }

        while (inst->task_active) {
            ret = uapi_adp_send_stream(inst->h_adp_play, &stream);
            if (ret != EXT_SUCCESS) {
                sap_msleep(THREAD_SLEEP_10MS);
                continue;
            }
            break;
        }

        ret = uapi_adp_release_stream(inst->h_adp_out, &stream);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_adp_release_stream, ret);
        }
    }

    sap_err_log_info("thread exit.");
    athread_set_exit(inst->task_out, TD_TRUE);
}

static td_s32 sample_cast_aenc_play_sys_init(td_void)
{
    td_s32 ret;

    ret = uapi_adp_init();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_init, ret);
        return ret;
    }

    ret = uapi_aenc_init();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_aenc_init, ret);
        (td_void)uapi_adp_deinit();
        return ret;
    }

    return EXT_SUCCESS;
}

static td_void sample_cast_aenc_play_sys_deinit(td_void)
{
    td_s32 ret;

    ret = uapi_aenc_deinit();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_aenc_deinit, ret);
    }

    ret = uapi_adp_deinit();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_deinit, ret);
    }
}

static td_void sample_cast_aenc_play_close_adp_output(sample_cast_aenc_play_inst *inst)
{
    td_s32 ret;

    if (inst->h_adp_out == 0) {
        return;
    }

    ret = uapi_adp_destroy(inst->h_adp_out);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_destroy, ret);
    }

    inst->h_adp_out = 0;
}

static td_s32 sample_cast_aenc_play_open_adp_output(sample_cast_aenc_play_inst *inst)
{
    td_s32 ret;
    uapi_adp_attr adp_attr;

    inst->h_adp_out = 0;
    ret = uapi_adp_get_def_attr(&adp_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_get_def_attr, ret);
        return ret;
    }

    ret = uapi_adp_create(&inst->h_adp_out, &adp_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_create, ret);
        return ret;
    }

    return EXT_SUCCESS;
}

static td_void sample_cast_aenc_play_close_aenc(sample_cast_aenc_play_inst *inst)
{
    td_s32 ret;

    if (inst->h_aenc == 0) {
        return;
    }

    ret = uapi_aenc_detach_output(inst->h_aenc, inst->h_adp_out);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_aenc_detach_output, ret);
    }

    ret = uapi_aenc_destroy(inst->h_aenc);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_aenc_destroy, ret);
    }

    inst->h_aenc = 0;
}

static td_s32 sample_cast_aenc_play_open_aenc(sample_cast_aenc_play_inst *inst)
{
    td_s32 ret;
    td_s32 err;
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

    aenc_attr.codec_id = inst->aenc_arg.acodec_id;
    aenc_attr.param.interleaved = TD_TRUE;
    aenc_attr.param.channels = inst->aenc_arg.pcm_format.channels;
    aenc_attr.param.bit_depth = inst->aenc_arg.pcm_format.bit_depth;
    aenc_attr.param.sample_rate = inst->aenc_arg.pcm_format.sample_rate;
    aenc_attr.param.samples_per_frame = inst->aenc_arg.pcm_format.sample_per_frame;
    aenc_attr.param.private_data = TD_NULL;
    aenc_attr.param.private_data_size = 0;

    if (aenc_attr.codec_id == UAPI_ACODEC_ID_SBC) {
        aenc_attr.max_trans_unit_size = BT_A2DP_MTU_SIZE;
    }

    ret = uapi_aenc_set_attr(inst->h_aenc, &aenc_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_aenc_set_attr, ret);
        goto out;
    }

    ret = uapi_aenc_attach_output(inst->h_aenc, inst->h_adp_out);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_aenc_attach_output, ret);
        goto out;
    }

    return EXT_SUCCESS;
out:
    err = uapi_aenc_destroy(inst->h_aenc);
    if (err != EXT_SUCCESS) {
        sap_err_log_fun(uapi_aenc_destroy, err);
    }

    inst->h_aenc = 0;
    return ret;
}

typedef struct {
    td_char *sample_encode;
    td_char *in_data;
    td_char *in_size;
    td_char *decode_type;
    td_char *in_sample_rate;
    td_char *in_bit_depth;
    td_char *in_channels;
    td_char *out_data;
    td_char *out_size;
    td_char *encode_type;
    td_char *out_sample_rate;
    td_char *out_bit_depth;
    td_char *out_channels;
    td_char *data_loop_mode;
} sample_cast_aenc_play_arg;

static td_void cast_aenc_play_adp_data_send_proc(td_void *args)
{
    td_s32 ret;
    uapi_stream_buf stream;
    sample_cast_aenc_play_inst *inst = (sample_cast_aenc_play_inst *)args;
    td_u32 read_len = (td_u32)sizeof(inst->frame_buf);

    if (read_len >= FILE_READ_LEN) {
        read_len = FILE_READ_LEN;
    }

    stream.data = inst->frame_buf;
    stream.pts = 0LL;
    stream.eos = TD_FALSE;
    stream.pkg_loss = TD_FALSE;

    cast_aenc_play_reader_reset(inst);

    sap_err_log_info("thread enter.");
    while (inst->task_active) {
        stream.size = read_len;
        stream.size = sample_cast_aenc_play_read(inst, &stream);
        if (stream.size == 0) {
            sap_msleep(THREAD_SLEEP_10MS);
            sap_err_log_info("read es stream failed.");
            continue;
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

    sap_err_log_info("thread exit.");
    athread_set_exit(inst->task_in, TD_TRUE);
}

static td_s32 sample_cast_aenc_play_ctx_init(const sample_cast_aenc_play_arg *arg, sample_acodec_arg *adec_ctx,
    sample_acodec_arg *aenc_ctx, td_bool *data_loop_mode)
{
    td_s32 ret;
    clear_obj(adec_ctx);
    clear_obj(aenc_ctx);

    if (strcmp(arg->in_data, "null") == 0) {
        adec_ctx->file = TD_NULL;
    } else {
        adec_ctx->file = arg->in_data;
    }

    if (strcmp(arg->out_data, "null") == 0) {
        aenc_ctx->file = TD_NULL;
    } else {
        aenc_ctx->file = arg->out_data;
    }

    if (strcmp(arg->data_loop_mode, "pcore_only") == 0) {
        *data_loop_mode = INTACT_MODE_PCORE_ONLY;
    } else if (strcmp(arg->data_loop_mode, "pcore_loop") == 0) {
        *data_loop_mode = INTACT_MODE_PCORE_LOOP;
    } else {
        *data_loop_mode = INTACT_MODE_ACORE_LOOP;
    }

    adec_ctx->acodec_id = UAPI_ACODEC_ID_MAX;
    aenc_ctx->acodec_id = UAPI_ACODEC_ID_MAX;

    ret = sample_audio_get_acodec_id(arg->encode_type, &aenc_ctx->acodec_id);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_audio_get_acodec_id, ret);
        return ret;
    }

    ret = sample_audio_get_acodec_id(arg->decode_type, &adec_ctx->acodec_id);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_audio_get_acodec_id, ret);
        return ret;
    }

    adec_ctx->pcm_format.channels = (td_u32)strtoul(arg->in_channels, TD_NULL, 0);
    adec_ctx->pcm_format.bit_depth = (td_u32)strtoul(arg->in_bit_depth, TD_NULL, 0);
    adec_ctx->pcm_format.sample_rate = (td_u32)strtoul(arg->in_sample_rate, TD_NULL, 0);
    adec_ctx->pcm_format.sample_per_frame = adec_ctx->pcm_format.sample_rate / 100; /* 100 --> 10ms */

    aenc_ctx->pcm_format.channels = (td_u32)strtoul(arg->out_channels, TD_NULL, 0);
    aenc_ctx->pcm_format.bit_depth = (td_u32)strtoul(arg->out_bit_depth, TD_NULL, 0);
    aenc_ctx->pcm_format.sample_rate = (td_u32)strtoul(arg->out_sample_rate, TD_NULL, 0);
    aenc_ctx->pcm_format.sample_per_frame = aenc_ctx->pcm_format.sample_rate / 100; /* 100 --> 10ms */

    return EXT_SUCCESS;
}

static td_void sample_cast_aenc_play_usage(const td_char *name)
{
    sap_printf("usage: %s in_data in_size adec rate bit ch out_data out_size aenc rate bit ch data_loop\n", name);
    sap_printf("examples:\n");
    sap_printf("  %s /mnt/test.mp3 0x0 mp3 16000 16 1 /mnt/test.pcm 0x0 sbc 16000 16 1 acore_loop\n", name);
    sap_printf("  %s 0x5825c000 0x40000 mp3 16000 16 1 0x0 0 sbc 16000 16 1 pcore_loop\n", name);
    sap_printf("  %s /mnt/test.mp3 0x0 mp3 16000 16 1 /mnt/test.pcm 0x0 sbc 16000 16 1 pcore_only\n", name);
    sap_printf("  %s 0x0 0x0 mp3 16000 16 1 0x0 0x0 sbc 16000 16 1 pcore_loop\n", name);
    sap_printf("  %s 0x0 0x0 mp3 16000 16 2 0x0 0x0 sbc 16000 16 2 pcore_only\n", name);
}

static td_void sample_cast_aenc_play_close_file(sample_cast_aenc_play_inst *inst)
{
    if (inst->h_file_out != TD_NULL) {
        fclose(inst->h_file_out);
        inst->h_file_out = TD_NULL;
    }

    if (inst->h_file_in != TD_NULL) {
        fclose(inst->h_file_in);
        inst->h_file_in = TD_NULL;
    }
}

static td_void sample_cast_aenc_play_reader_init(sample_cast_aenc_play_inst *inst)
{
    td_u32 buf;
    td_u32 size;

    if (inst->in_data_addr != 0x0) {
        buf = inst->in_data_addr;
        size = inst->in_data_size;
    } else {
        switch (inst->adec_arg.acodec_id) {
            case UAPI_ACODEC_ID_SBC:
            case UAPI_ACODEC_ID_MSBC:
                buf = FPGA_FLASH_ES_DATA_SBC_ADDR;
                break;
            case UAPI_ACODEC_ID_MP3:
            case UAPI_ACODEC_ID_AAC:
            case UAPI_ACODEC_ID_LC3:
            case UAPI_ACODEC_ID_SILK:
            case UAPI_ACODEC_ID_OPUS:
            case UAPI_ACODEC_ID_FLAC:
            case UAPI_ACODEC_ID_L2HC:
                buf = FPGA_FLASH_ES_DATA_MP3_ADDR;
                break;

            default:
                buf = FPGA_FLASH_ES_DATA_MP3_ADDR;
                break;
        }
        size = FPGA_FLASH_ES_SIZE;
    }

    data_info_init(&inst->read_data, buf, size);
    circ_buf_update_write_pos(&inst->read_data.cb, inst->read_data.cb.size - sizeof(td_s16));
}

static td_void sample_cast_aenc_play_storer_init(sample_cast_aenc_play_inst *inst)
{
    td_u32 buf;
    td_u32 size;

    if (inst->out_data_addr != 0x0) {
        buf = inst->out_data_addr;
        size = inst->out_data_size;
    } else {
        buf = FPGA_SAVE_DATA_ADDR;
        size = FPGA_SAVE_DATA_SIZE;
    }

    data_info_init(&inst->write_data, buf, size);
}

/* Check data position via the prefix of addr in hex form.
    ret:
        TD_TRUE:  data in addr
        TD_FALSE: data in file
 */
static td_bool sample_cast_aenc_play_check_data_addr(td_char *data)
{
    td_u32 ret;
    sap_printf("data= %s\n", data);
    if (strncasecmp((const char *)data, "0x", 2) != 0) {
        ret = TD_TRUE;
    } else {
        ret = TD_FALSE;
    }
    sap_err_log_u32(ret);
    return ret;
}

static td_s32 sample_cast_aenc_play_open_file(sample_cast_aenc_play_inst *inst, const sample_cast_aenc_play_arg *arg)
{
    inst->frame_size = calc_pcm_frame_size(&inst->adec_arg.pcm_format);

    /* open input file */
    inst->h_file_in = TD_NULL;
    if (sample_cast_aenc_play_check_data_addr(arg->in_data)) {
        inst->h_file_in = fopen(arg->in_data, "rb");
        if (inst->h_file_in == TD_NULL) {
            sap_printf("open file %s error!\n", arg->in_data);
            return -1;
        }
    } else {
        inst->in_data_addr = (td_u32)strtoul(arg->in_data, NULL, 16); /* 16 represents Hexadecimal */
        inst->in_data_size = (td_u32)strtoul(arg->in_size, NULL, 16); /* 16 represents Hexadecimal */
        sap_err_log_h32(inst->in_data_addr);
        sap_err_log_u32(inst->in_data_size);
    }

    /* open output file */
    inst->h_file_out = TD_NULL;
    if (sample_cast_aenc_play_check_data_addr(arg->in_data)) {
        td_s32 ret;
        td_char file_path[FILE_PATH_LEN];
        ret = snprintf_s(file_path, FILE_PATH_LEN - 1, FILE_PATH_LEN - 1, "%s.%s", arg->in_data, arg->encode_type);
        if (ret < 0) {
            sap_err_log_fun(snprintf_s, ret);
            if (inst->h_file_in != TD_NULL) {
                fclose(inst->h_file_in);
                inst->h_file_in = TD_NULL;
            }

            return EXT_FAILURE;
        }

        inst->h_file_out = fopen(file_path, "wb");
        if (inst->h_file_out == TD_NULL) {
            sap_printf("open file %s error!\n", file_path);
            if (inst->h_file_in != TD_NULL) {
                fclose(inst->h_file_in);
                inst->h_file_in = TD_NULL;
            }

            return EXT_FAILURE;
        }
    } else {
        inst->out_data_addr = (td_u32)strtoul(arg->out_data, NULL, 16); /* 16 represents Hexadecimal */
        inst->out_data_size = (td_u32)strtoul(arg->out_size, NULL, 16); /* 16 represents Hexadecimal */
        sap_err_log_h32(inst->out_data_addr);
        sap_err_log_u32(inst->out_data_size);
    }

    sample_cast_aenc_play_reader_init(inst);
    sample_cast_aenc_play_storer_init(inst);
    return 0;
}

static td_s32 sample_cast_aenc_play_open_cast_player(sample_cast_aenc_play_inst *inst)
{
    td_s32 ret;
    const uapi_snd_attr snd_attr = {
        .port_num = 1,
        .port_attr[0].out_port = UAPI_SND_OUT_PORT_CAST0,
        .channels = inst->aenc_arg.pcm_format.channels,
        .bit_depth = inst->aenc_arg.pcm_format.bit_depth,
        .sample_rate = inst->aenc_arg.pcm_format.sample_rate,
    };
    sample_escast_arg escast_arg = {
        .player = &inst->h_adp_in,
        .in_arg = &inst->adec_arg,
        .snd = &inst->h_snd,
        .snd_attr = &snd_attr,
        .aef_profile = UAPI_SND_AEF_PROFILE_NONE,
    };

    /* 按照输入的采样率、声道、位宽，打开一个escast播放器，解码ES码流，返回输出 snd */
    ret = sample_escast_open(&escast_arg);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_escast_open, ret);
        return ret;
    }

    /* 将 cast_player 的输出 pcm attach 到 aenc 做输入 */
    ret = uapi_snd_attach_output(inst->h_snd, UAPI_SND_OUT_PORT_CAST0, inst->h_aenc);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_attach_output, ret);
        goto out0;
    }

    return EXT_SUCCESS;
out0:
    sample_escast_close();
    return ret;
}

static td_s32 sample_cast_aenc_play_close_cast_player(sample_cast_aenc_play_inst *inst)
{
    td_s32 ret;

    ret = uapi_snd_detach_output(inst->h_snd, UAPI_SND_OUT_PORT_CAST0, inst->h_aenc);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_detach_output, ret);
        return ret;
    }

    sample_escast_close();
    return EXT_SUCCESS;
}

static td_s32 sample_cast_aenc_play_open_inst(sample_cast_aenc_play_inst *inst)
{
    td_s32 ret;

    ret = sample_cast_aenc_play_sys_init();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_cast_aenc_play_sys_init, ret);
        return ret;
    }

    /*
     * 数据流向 cast_player --> aenc --> adp_out
     * 按照数据流向反方向打开实例
     * 按照数据方向attach output
     */
    ret = sample_cast_aenc_play_open_adp_output(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_cast_aenc_play_open_adp_output, ret);
        goto out0;
    }

    ret = sample_cast_aenc_play_open_aenc(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_cast_aenc_play_open_aenc, ret);
        goto out1;
    }

    ret = sample_cast_aenc_play_open_cast_player(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_cast_aenc_play_open_cast_player, ret);
        goto out2;
    }

    return EXT_SUCCESS;
out2:
    sample_cast_aenc_play_close_aenc(inst);
out1:
    sample_cast_aenc_play_close_adp_output(inst);
out0:
    sample_cast_aenc_play_sys_deinit();
    return ret;
}

static td_void sample_cast_aenc_play_close_inst(sample_cast_aenc_play_inst *inst)
{
    td_s32 ret;

    ret = sample_cast_aenc_play_close_cast_player(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_cast_aenc_play_close_cast_player, ret);
    }
    sample_cast_aenc_play_close_aenc(inst);
    sample_cast_aenc_play_close_adp_output(inst);
    sample_cast_aenc_play_sys_deinit();
}

static td_s32 sample_cast_aenc_play_adp_share(sample_cast_aenc_play_inst *inst)
{
    /* bt use these two adp handle to get aenc output and send stream to dsp player */
    sap_err_log_h32(inst->h_adp_out);
    sap_err_log_h32(inst->h_adp_play);
    adp_handle_share_set(inst->h_adp_out, inst->h_adp_play, 1);
    return EXT_SUCCESS;
}

static td_void sample_cast_aenc_play_stop_inst(sample_cast_aenc_play_inst *inst)
{
    td_s32 ret;

    inst->task_active = TD_FALSE;
    if (inst->task_in != TD_NULL) {
        athread_exit(inst->task_in);
        inst->task_in = TD_NULL;
    }

    if (inst->task_out != TD_NULL) {
        athread_exit(inst->task_out);
        inst->task_out = TD_NULL;
    }

    ret = uapi_aenc_stop(inst->h_aenc);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_aenc_stop, ret);
    }
}

static td_s32 sample_cast_aenc_play_start_inst(sample_cast_aenc_play_inst *inst)
{
    td_s32 ret;
    athread_attr attr;

    ret = uapi_aenc_start(inst->h_aenc);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_aenc_start, ret);
    }

    /* 创建线程为adec输入adp输送ES数据 */
    inst->task_active = TD_TRUE;
    attr.priority = ATHREAD_PRIORITY_NORMAL;
    attr.stack_size = 0x1000; /* 4k */
    attr.name = "sample_cast_send";
    ret = athread_create(&inst->task_in, cast_aenc_play_adp_data_send_proc, (td_void *)inst, &attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(athread_create, ret);
    }

    /* when running bt_task to get aenc output and send to esplayer, no need to create sample_cast_play task in acore */
    if (inst->data_loop_mode != INTACT_MODE_ACORE_LOOP) {
        /* share adp handle to BT core */
        return sample_cast_aenc_play_adp_share(inst);
    }

    /* create a task to looopback aenc output to esplayer */
    attr.priority = ATHREAD_PRIORITY_NORMAL;
    attr.stack_size = 0x1000; /* 4k */
    attr.name = "sample_cast_play";
    /* 创建线程获取编码后的es流，并送给esplay播放器去播放 */
    ret = athread_create(&inst->task_out, cast_aenc_play_adp_data_play_proc, (td_void *)inst, &attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(athread_create, ret);
    }
    return EXT_SUCCESS;
}

static td_void sample_cast_aenc_play_close(sample_cast_aenc_play_inst *inst)
{
    sample_cast_aenc_play_close_inst(inst);
    sample_cast_aenc_play_close_file(inst);
    sample_esplay_close();
}

static td_s32 sample_cast_aenc_play_open(sample_cast_aenc_play_inst *inst, const sample_cast_aenc_play_arg *arg)
{
    td_s32 ret;

    /* when running in BT music demo mode, we don't need to proc the loopback data via esplayer */
    if (inst->data_loop_mode != INTACT_MODE_PCORE_ONLY) {
        /* 按照最终编码输出的采样率、声道、位宽，打开一个esplay播放器，将最终编码输出的ES码流本地播放出来 */
        ret = sample_esplay_open(&inst->aenc_arg, &inst->h_adp_play, UAPI_SND_AEF_PROFILE_MUSIC);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(sample_esplay_open, ret);
            return ret;
        }
    }

    ret = sample_cast_aenc_play_open_file(inst, arg);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_cast_aenc_play_open_file, ret);
        goto out0;
    }

    /* 打开播放cast编码通路所有实例 */
    ret = sample_cast_aenc_play_open_inst(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_cast_aenc_play_open_inst, ret);
        goto out1;
    }

    return EXT_SUCCESS;
out1:
    sample_cast_aenc_play_close_file(inst);
out0:
    sample_esplay_close();
    return ret;
}

static td_s32 sample_cast_aenc_play_entry(td_s32 argc, td_char *argv[])
{
    td_s32 ret;
    sample_cast_aenc_play_arg *arg = (sample_cast_aenc_play_arg *)argv;
    sample_cast_aenc_play_inst *inst = sample_cast_aenc_play_get_inst();
    if (inst != TD_NULL) {
        sap_printf("%s is already running\n", argv[0]);
        return EXT_FAILURE;
    }

    if (argc < (td_s32)(sizeof(*arg) / sizeof(td_char *))) {
        sample_cast_aenc_play_usage(argv[0]);
        return EXT_FAILURE;
    }

    inst = sample_cast_aenc_play_alloc_inst();
    if (inst == TD_NULL) {
        sap_printf("sample_cast_aenc_play_alloc_inst failed\n");
        return EXT_FAILURE;
    }

    ret = sample_cast_aenc_play_ctx_init(arg, &inst->adec_arg, &inst->aenc_arg, &inst->data_loop_mode);
    if (ret != EXT_SUCCESS) {
        goto out0;
    }

    ret = sample_cast_aenc_play_open(inst, arg);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_cast_aenc_play_open, ret);
        goto out0;
    }

    /* start播放cast编码通道所有实例 */
    ret = sample_cast_aenc_play_start_inst(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_cast_aenc_play_start_inst, ret);
        goto out1;
    }

    return EXT_SUCCESS;
out1:
    sample_cast_aenc_play_close(inst);
out0:
    sample_cast_aenc_play_free_inst(inst);
    return ret;
}

static td_s32 sample_cast_aenc_play_exit(td_void)
{
    sample_cast_aenc_play_inst *inst = sample_cast_aenc_play_get_inst();

    if (inst != NULL) {
        sample_cast_aenc_play_stop_inst(inst);
        sample_cast_aenc_play_close(inst);
        sample_cast_aenc_play_free_inst(inst);
    }

    return EXT_SUCCESS;
}

/*
 * 编码同时播放sample
 * 从文件或者内存读入ES流解码成pcm数据，执行编码操作
 * 编码后的ES流通过一个esplay播放器播放
 */
td_s32 sample_cast_aenc_play(td_s32 argc, td_char *argv[])
{
    if (argc <= 1) {
        sample_cast_aenc_play_usage(argv[0]);
        return EXT_SUCCESS;
    }

    if (strcmp(argv[1], "q") == 0) {
        return sample_cast_aenc_play_exit();
    } else {
        return sample_cast_aenc_play_entry(argc, argv);
    }
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

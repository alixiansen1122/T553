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
#include "sample_audio_api.h"
#include "sample_audio_utils.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define DEFAULT_AMRWB_BIT_RATE    0

#define DYNAMIC_SET_PARAM_ARG_NUM 3
#define PARAM_TYPE_INDEX          1
#define PARAM_VALUE_INDEX         2

#define PLC_TEST_ARG_NUM          9
#define PKG_LOSS_MODE_INDEX       6
#define PKG_LOSS_PERIOD_INDEX     7
#define PKG_LOSS_RATE_INDEX       8

typedef enum {
    SAMPLE_PKG_LOSS_OFF,
    SAMPLE_PKG_LOSS_INTERMITTENT,
    SAMPLE_PKG_LOSS_CONTINUAL
} sample_pkg_loss_mode;

typedef struct {
    sample_pkg_loss_mode pkg_loss_mode;
    td_u32 pkg_loss_period;
    td_u32 pkg_loss_rate;
} sample_encode_play_plc_arg;

typedef struct {
    td_char *sample_encode;
    td_char *in_stream;
    td_char *encode_type;
    td_char *sample_rate;
    td_char *bit_depth;
    td_char *channels;
} sample_encode_arg;

typedef struct sample_encode_inst {
    td_bool task_active;
    td_bool task_out_active;
    athread_handle task_in;
    athread_handle task_out;

    FILE *h_file_in;
    FILE *h_file_out;
    td_u8 frame_buf[0x1000];
    td_u32 frame_size;
    data_info read_data;
    data_info write_data;
    td_u32 (*read_frame)(struct sample_encode_inst *inst, uapi_audio_frame *frame);
    td_u32 (*write_frame)(struct sample_encode_inst *inst, const td_u8 *from, td_u32 len);

    td_handle h_adp_in;
    td_handle h_adp_out;
    td_handle h_adp_play;
    td_handle h_aenc;

    td_u32 plc_count;
    sample_encode_play_plc_arg plc_arg;

    sample_acodec_arg aenc_arg;
    struct osal_list_head node;
} sample_encode_inst;

static OSAL_LIST_HEAD(g_sample_encode_list);

static sample_encode_inst *sample_encode_alloc_inst(td_void)
{
    sample_encode_inst *inst = (sample_encode_inst *)malloc(sizeof(sample_encode_inst));
    if (inst == TD_NULL) {
        return TD_NULL;
    }

    (td_void)memset_s(inst, sizeof(*inst), 0, sizeof(sample_encode_inst));

    osal_list_add_tail(&inst->node, &g_sample_encode_list);
    return inst;
}

static td_void sample_encode_free_inst(sample_encode_inst *inst)
{
    if (inst == TD_NULL) {
        return;
    }

    osal_list_del(&inst->node);
    free(inst);
}

static sample_encode_inst *sample_encode_get_inst(td_void)
{
    if (osal_list_empty(&g_sample_encode_list) != TD_FALSE) {
        return TD_NULL;
    }

    return osal_list_first_entry(&g_sample_encode_list, sample_encode_inst, node);
}

static td_u32 sample_encode_write_flash(sample_encode_inst *aenc, const td_u8 *from, td_u32 len)
{
    circ_buf *cb = &aenc->write_data.cb;
    if (circ_buf_query_free(cb) <= len) {
        sap_printf("Warning: ES data enough\n");
        return 0;
    }

    return circ_buf_write(cb, from, len);
}

static td_u32 sample_encode_write_file(sample_encode_inst *inst, const td_u8 *from, td_u32 len)
{
    td_u32 write;

    write = fwrite(from, 1, len, inst->h_file_out);
    return write;
}

static td_void encode_reader_reset(sample_encode_inst *aenc)
{
    circ_buf *cb = &aenc->read_data.cb;
    circ_buf_flush(cb);
    circ_buf_update_write_pos(cb, cb->size - sizeof(td_s16));
}

static td_u32 sample_encode_read_flash(sample_encode_inst *inst, uapi_audio_frame *frame)
{
    const td_u32 len = frame->bits_bytes;
    circ_buf *cb = &inst->read_data.cb;

    if (len > cb->size) {
        sap_printf("Fatal Error: data not enough\n");
        return 0;
    }

    if (circ_buf_query_busy(cb) < len) {
        sap_printf("reset read\n");
        encode_reader_reset(inst);
    }

    return circ_buf_read(cb, (td_u8 *)frame->bits_buffer, len);
}

static td_u32 sample_encode_read_file(sample_encode_inst *inst, uapi_audio_frame *frame)
{
    td_u32 len;

    len = fread(frame->bits_buffer, 1, frame->bits_bytes, inst->h_file_in);
    if (len == 0) {
        sap_printf("reset read\n");
        rewind(inst->h_file_in);
        return 0;
    }

    return len;
}

static td_u32 g_vector_read = 0;

static td_u32 sample_encode_read_array(sample_encode_inst *inst, uapi_audio_frame *frame)
{
    td_u32 i, j;
    td_u16 *buf = (td_u16 *)(td_uintptr_t)frame->bits_buffer;
    const td_u16 *vector_16k_1ch_16bit = sample_get_vector_16k_1ch_16bit();
    const td_u32 verctor_size = sample_get_vector_size();

    for (i = 0; i < inst->aenc_arg.pcm_format.sample_per_frame; i++) {
        for (j = 0; j < inst->aenc_arg.pcm_format.channels; ++j) {
            *buf++ = vector_16k_1ch_16bit[g_vector_read]; /* duplicate each channel */
        }
        g_vector_read = saturate_add(g_vector_read, 1, verctor_size);
    }

    return inst->frame_size;
}

static td_void sample_encode_build_frame(const sample_encode_inst *inst, uapi_audio_frame *frame)
{
    clear_obj(frame);
    frame->interleaved = TD_TRUE;
    frame->channels = inst->aenc_arg.pcm_format.channels;
    frame->bit_depth = inst->aenc_arg.pcm_format.bit_depth;
    frame->sample_rate = inst->aenc_arg.pcm_format.sample_rate;
    frame->bits_buffer = (td_s32 *)inst->frame_buf;
    frame->bits_bytes = inst->frame_size;
    frame->pkg_loss = TD_FALSE;
}

static td_void encode_adp_data_send_proc(td_void *args)
{
    td_s32 ret;
    td_u32 len;
    td_u32 read_len;
    uapi_audio_frame input_frame;
    sample_encode_inst *inst = (sample_encode_inst *)args;

    sample_encode_build_frame(inst, &input_frame);
    encode_reader_reset(inst);

    sap_err_log_info("thread enter.");
    while (inst->task_active) {
        uapi_adp_query_free(inst->h_adp_in, &read_len);
        if (read_len < FILE_READ_LEN_MIN) {
            sap_msleep(THREAD_SLEEP_5MS);
            continue;
        }

        read_len = circ_buf_min(read_len, inst->frame_size);
        input_frame.bits_bytes = read_len;

        len = inst->read_frame(inst, &input_frame);
        if (len == 0) {
            continue;
        }
        input_frame.bits_bytes = len;

        while (inst->task_active) {
            ret = uapi_adp_send_frame(inst->h_adp_in, &input_frame);
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

static td_void encode_adp_data_recv_proc(td_void *args)
{
    td_s32 ret;
    td_u32 len;
    uapi_stream_buf stream;
    sample_encode_inst *inst = (sample_encode_inst *)args;

    sap_err_log_info("thread enter.");
    while (inst->task_out_active) {
        ret = uapi_adp_acquire_stream(inst->h_adp_out, &stream);
        if (ret != EXT_SUCCESS) {
            sap_msleep(THREAD_SLEEP_10MS);
            continue;
        }

        len = inst->write_frame(inst, (const td_u8 *)stream.data, stream.size);
        if (len == 0) {
            inst->task_out_active = TD_FALSE;
            break;
        }

        ret = uapi_adp_release_stream(inst->h_adp_out, &stream);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_adp_release_stream, ret);
        }

        sap_msleep(THREAD_SLEEP_10MS);
    }

    sap_err_log_info("thread exit.");
    athread_set_exit(inst->task_out, TD_TRUE);
}

static td_void sample_encode_play_start_pkg_loss(sample_encode_inst *inst, uapi_stream_buf *stream)
{
    if (inst->plc_arg.pkg_loss_mode == SAMPLE_PKG_LOSS_OFF || inst->plc_arg.pkg_loss_rate == 0) {
        return;
    }

    if (inst->plc_arg.pkg_loss_mode == SAMPLE_PKG_LOSS_INTERMITTENT) {
        if (inst->plc_count >= inst->plc_arg.pkg_loss_period + inst->plc_arg.pkg_loss_rate) {
            inst->plc_count -= inst->plc_arg.pkg_loss_period;
        } else if (inst->plc_count >= inst->plc_arg.pkg_loss_period) {
            stream->data = TD_NULL;
            stream->pkg_loss = TD_TRUE;
        }
    } else if (inst->plc_arg.pkg_loss_mode == SAMPLE_PKG_LOSS_CONTINUAL) {
        if (inst->plc_count < inst->plc_arg.pkg_loss_rate) {
            stream->data = TD_NULL;
            stream->pkg_loss = TD_TRUE;
        } else if (inst->plc_count >= inst->plc_arg.pkg_loss_period) {
            inst->plc_count = 0;
        }
    }
}

static td_void sample_encode_play_stop_pkg_loss(sample_encode_inst *inst, uapi_stream_buf *stream)
{
    stream->pkg_loss = TD_FALSE;
    if (inst->plc_arg.pkg_loss_mode == SAMPLE_PKG_LOSS_OFF || inst->plc_arg.pkg_loss_rate == 0) {
        return;
    }

    if (inst->plc_arg.pkg_loss_mode == SAMPLE_PKG_LOSS_INTERMITTENT) {
        inst->plc_count += inst->plc_arg.pkg_loss_rate;
    } else if (inst->plc_arg.pkg_loss_mode == SAMPLE_PKG_LOSS_CONTINUAL) {
        inst->plc_count++;
    }
}

static td_void encode_adp_data_play_proc(td_void *args)
{
    td_s32 ret;
    td_u32 read_len;
    uapi_stream_buf stream;
    sample_encode_inst *inst = (sample_encode_inst *)args;

    sap_err_log_info("thread enter.");
    while (inst->task_out_active) {
        ret = uapi_adp_acquire_stream(inst->h_adp_out, &stream);
        if (ret != EXT_SUCCESS) {
            sap_msleep(THREAD_SLEEP_10MS);
            continue;
        }

        uapi_adp_query_free(inst->h_adp_play, &read_len);
        if (read_len < FILE_READ_LEN_MIN) {
            sap_msleep(THREAD_SLEEP_5MS);
            continue;
        }
        stream.size = circ_buf_min(stream.size, read_len);

        while (inst->task_out_active) {
            sample_encode_play_start_pkg_loss(inst, &stream);
            ret = uapi_adp_send_stream(inst->h_adp_play, &stream);
            if (ret != EXT_SUCCESS) {
                sap_msleep(THREAD_SLEEP_10MS);
                continue;
            }
            sample_encode_play_stop_pkg_loss(inst, &stream);
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

static td_s32 sample_encode_sys_init(td_void)
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

static td_void sample_encode_sys_deinit(td_void)
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

static td_void sample_encode_close_adp_input(sample_encode_inst *inst)
{
    td_s32 ret;

    if (inst->h_adp_in == 0) {
        return;
    }

    ret = uapi_adp_detach_output(inst->h_adp_in, inst->h_aenc);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_detach_output, ret);
    }

    ret = uapi_adp_destroy(inst->h_adp_in);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_destroy, ret);
    }

    inst->h_adp_in = 0;
}

static td_s32 sample_encode_open_adp_input(sample_encode_inst *inst)
{
    td_s32 ret;
    td_s32 err;
    uapi_adp_attr adp_attr;

    inst->h_adp_in = 0;
    ret = uapi_adp_get_def_attr(&adp_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_get_def_attr, ret);
        return ret;
    }

    ret = uapi_adp_create(&inst->h_adp_in, &adp_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_create, ret);
        return ret;
    }

    ret = uapi_adp_attach_output(inst->h_adp_in, inst->h_aenc);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_attach_output, ret);
        goto out;
    }

    return EXT_SUCCESS;
out:
    err = uapi_adp_destroy(inst->h_adp_in);
    if (err != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_destroy, err);
    }

    inst->h_adp_in = 0;
    return ret;
}

static td_void sample_encode_close_adp_output(sample_encode_inst *inst)
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

static td_s32 sample_encode_open_adp_output(sample_encode_inst *inst)
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

static td_void sample_encode_close_aenc(sample_encode_inst *inst)
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

static td_s32 sample_encode_open_aenc(sample_encode_inst *inst)
{
    td_s32 ret;
    td_s32 err;
    td_u32 bit_rate;
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

    ret = uapi_aenc_set_attr(inst->h_aenc, &aenc_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_aenc_set_attr, ret);
        goto out;
    }

    if (aenc_attr.codec_id == UAPI_ACODEC_ID_AMRWB) {
        bit_rate = DEFAULT_AMRWB_BIT_RATE;
        ret = uapi_aenc_set_param(inst->h_aenc, UAPI_AENC_PARAM_BIT_RATE, &bit_rate, sizeof(bit_rate));
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_aenc_set_param, ret);
            goto out;
        }
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

static td_void sample_encode_close_inst(sample_encode_inst *inst)
{
    sample_encode_close_adp_input(inst);
    sample_encode_close_aenc(inst);
    sample_encode_close_adp_output(inst);
    sample_encode_sys_deinit();
}

static td_s32 sample_encode_open_inst(sample_encode_inst *inst)
{
    td_s32 ret;

    ret = sample_encode_sys_init();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_encode_sys_init, ret);
        return ret;
    }

    /*
     * 数据流向 adp_in --> aenc --> adp_out
     * 按照数据流向反方向打开实例
     * 按照数据方向attach output
     */
    ret = sample_encode_open_adp_output(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_encode_open_adp_output, ret);
        goto out0;
    }

    ret = sample_encode_open_aenc(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_encode_open_aenc, ret);
        goto out1;
    }

    ret = sample_encode_open_adp_input(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_encode_open_adp_input, ret);
        goto out2;
    }

    return EXT_SUCCESS;
out2:
    sample_encode_close_aenc(inst);
out1:
    sample_encode_close_adp_output(inst);
out0:
    sample_encode_sys_deinit();
    return ret;
}

static td_void sample_encode_stop_inst(sample_encode_inst *inst)
{
    td_s32 ret;

    inst->task_active = TD_FALSE;
    if (inst->task_in != TD_NULL) {
        athread_exit(inst->task_in);
        inst->task_in = TD_NULL;
    }

    inst->task_out_active = TD_FALSE;
    if (inst->task_out != TD_NULL) {
        athread_exit(inst->task_out);
        inst->task_out = TD_NULL;
    }

    ret = uapi_aenc_stop(inst->h_aenc);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_aenc_stop, ret);
    }
}

static td_s32 sample_encode_start_inst(sample_encode_inst *inst)
{
    td_s32 ret;
    athread_attr attr;

    ret = uapi_aenc_start(inst->h_aenc);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_aenc_start, ret);
    }

    /* Create Task for ADP instance to send audio data */
    inst->task_active = TD_TRUE;
    attr.priority = ATHREAD_PRIORITY_NORMAL;
    attr.stack_size = 0x1000; /* 4k */
    attr.name = "sample_aenc_send";
    ret = athread_create(&inst->task_in, encode_adp_data_send_proc, (td_void *)inst, &attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(athread_create, ret);
    }

    inst->task_out_active = TD_TRUE;
    attr.priority = ATHREAD_PRIORITY_NORMAL;
    attr.stack_size = 0x1000; /* 4k */
    attr.name = "sample_aenc_recv";
    ret = athread_create(&inst->task_out, encode_adp_data_recv_proc, (td_void *)inst, &attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(athread_create, ret);
    }
    return EXT_SUCCESS;
}

static td_void sample_encode_close_file(sample_encode_inst *inst)
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

static td_s32 sample_encode_open_file(sample_encode_inst *inst, const sample_encode_arg *arg)
{
    /* open input file */
    inst->h_file_in = TD_NULL;
    if (strcmp(arg->in_stream, "null") != 0) {
        inst->h_file_in = fopen(arg->in_stream, "rb");
        if (inst->h_file_in == TD_NULL) {
            sap_printf("open file %s error!\n", arg->in_stream);
            return -1;
        }
    }

    /* open output file */
    inst->h_file_out = TD_NULL;
    if (strcmp(arg->sample_encode, "sample_encode_play") == 0) {
        return 0;
    }

    if (strcmp(arg->in_stream, "null") != 0) {
        td_s32 ret;
        td_char file_path[FILE_PATH_LEN];
        ret = snprintf_s(file_path, FILE_PATH_LEN - 1, FILE_PATH_LEN - 1, "%s.%s", arg->in_stream, arg->encode_type);
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
    }

    return 0;
}

static td_s32 sample_encode_open_in_stream(sample_encode_inst *inst, const sample_encode_arg *arg)
{
    td_s32 ret;
    inst->frame_size = calc_pcm_frame_size(&inst->aenc_arg.pcm_format);

    inst->h_file_in = TD_NULL;
    inst->h_file_out = TD_NULL;
    data_info_init(&inst->read_data, FPGA_FLASH_PCM_DATA_ADDR, FPGA_FLASH_PCM_SIZE);
    data_info_init(&inst->write_data, FPGA_SAVE_DATA_ADDR, FPGA_SAVE_DATA_SIZE);

    if (strcmp(arg->in_stream, "array") == 0 || arg->in_stream == TD_NULL) {
        inst->read_frame = sample_encode_read_array;
        inst->write_frame = sample_encode_write_flash;
        sap_err_log_info("sample_ao read data from array.");
    } else if (strcmp(arg->in_stream, "flash") == 0 || strcmp(arg->in_stream, "null") == 0) {
        inst->read_frame = sample_encode_read_flash;
        inst->write_frame = sample_encode_write_flash;
        sap_err_log_info("sample_ao read data from flash.");
    } else {
        ret = sample_encode_open_file(inst, arg);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(sample_encode_open_file, ret);
            return ret;
        }

        inst->read_frame = sample_encode_read_file;
        inst->write_frame = sample_encode_write_file;
        sap_err_log_info("sample_ao read data from file.");
    }

    return 0;
}

static td_s32 sample_encode_ctx_init(const sample_encode_arg *arg, sample_acodec_arg *ctx)
{
    td_s32 ret;
    clear_obj(ctx);

    if (strcmp(arg->in_stream, "null") == 0) {
        ctx->file = TD_NULL;
    } else {
        ctx->file = arg->in_stream;
    }

    ctx->acodec_id = UAPI_ACODEC_ID_MAX;

    ret = sample_audio_get_acodec_id(arg->encode_type, &ctx->acodec_id);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_audio_get_acodec_id, ret);
        return ret;
    }

    ctx->pcm_format.channels = (td_u32)strtoul(arg->channels, TD_NULL, 0);
    ctx->pcm_format.bit_depth = (td_u32)strtoul(arg->bit_depth, TD_NULL, 0);
    ctx->pcm_format.sample_rate = (td_u32)strtoul(arg->sample_rate, TD_NULL, 0);
    ctx->pcm_format.sample_per_frame = ctx->pcm_format.sample_rate / 100; /* 100 --> 10ms */

    sap_err_log_u32(ctx->pcm_format.channels);
    sap_err_log_u32(ctx->pcm_format.bit_depth);
    sap_err_log_u32(ctx->pcm_format.sample_rate);
    sap_err_log_u32(ctx->pcm_format.sample_per_frame);
    return EXT_SUCCESS;
}

static td_void sample_encode_usage(const td_char *name)
{
    sap_printf("if you won't to test PLC:\n");
    sap_printf("usage:  %s pcm_file codec sample_rate bit_depth channels\n", name);
    sap_printf("examples:\n");
    sap_printf("  %s /mnt/test.pcm sbc 48000 16 2\n", name);
    sap_printf("  %s null msbc 16000 16 1\n", name);
    sap_printf("\n");
    sap_printf("if you want to test PLC:\n");
    sap_printf("usage:  %s pcm_file codec sample_rate bit_depth channels plc_mode plc_period plc_rate\n", name);
    sap_printf("examples:\n");
    sap_printf("  %s /user/stream/PCM_8k_1ch_16bit.pcm pcm 8000 16 1 off 100 10\n", name);
    sap_printf("  %s /user/stream/PCM_8k_1ch_16bit.pcm pcm 8000 16 1 int 10 1\n", name);
    sap_printf("  %s /user/stream/PCM_8k_1ch_16bit.pcm pcm 8000 16 1 con 100 20\n", name);
    sap_printf("\n");
    sap_printf("if you want to set param dynamically:\n");
    sap_printf("usage:  %s param_type param_value\n", name);
    sap_printf("examples:\n");
    sap_printf("  %s bit_rate 6\n", name);
}

static td_s32 sample_encode_entry(td_s32 argc, td_char *argv[])
{
    td_s32 ret;
    sample_encode_arg *arg = (sample_encode_arg *)argv;
    sample_encode_inst *encode_inst = sample_encode_get_inst();
    if (encode_inst != TD_NULL) {
        sap_printf("%s is already running", argv[0]);
        return EXT_FAILURE;
    }

    if (argc < (td_s32)(sizeof(*arg) / sizeof(td_char *))) {
        sample_encode_usage(argv[0]);
        return EXT_FAILURE;
    }

    encode_inst = sample_encode_alloc_inst();
    if (encode_inst == TD_NULL) {
        sap_printf("sample_encode_alloc_inst failed");
        return EXT_FAILURE;
    }

    ret = sample_encode_ctx_init(arg, &encode_inst->aenc_arg);
    if (ret != EXT_SUCCESS) {
        goto out0;
    }

    ret = sample_encode_open_in_stream(encode_inst, arg);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_encode_open_in_stream, ret);
        goto out0;
    }

    ret = sample_encode_open_inst(encode_inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_encode_open_inst, ret);
        goto out1;
    }

    ret = sample_encode_start_inst(encode_inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_encode_start_inst, ret);
        goto out2;
    }

    return EXT_SUCCESS;
out2:
    sample_encode_close_inst(encode_inst);
out1:
    sample_encode_close_file(encode_inst);
out0:
    sample_encode_free_inst(encode_inst);
    return ret;
}

static td_s32 sample_encode_exit(td_void)
{
    sample_encode_inst *inst = sample_encode_get_inst();
    if (inst != TD_NULL) {
        sample_encode_stop_inst(inst);
        sample_encode_close_inst(inst);
        sample_encode_close_file(inst);
        sample_encode_free_inst(inst);
    }

    return EXT_SUCCESS;
}

/*
 * 编码sample
 * 从文件或者内存读入pcm数据，执行编码操作
 * 编码后的ES流写入文件或者写入内存
 */
td_s32 sample_encode(td_s32 argc, td_char *argv[])
{
    if (argc <= 1) {
        sample_encode_usage(argv[0]);
        return EXT_SUCCESS;
    }

    if (strcmp(argv[1], "q") == 0) {
        return sample_encode_exit();
    } else {
        return sample_encode_entry(argc, argv);
    }
}

static td_s32 sample_encode_play_parse_plc_mode(td_char *argv[], sample_encode_inst *inst)
{
    td_u32 i;
    const struct {
        td_char *mode_name;
        sample_pkg_loss_mode pkg_loss_mode;
    } optional_pkg_loss_mode[] = {
        {"off", SAMPLE_PKG_LOSS_OFF},
        {"int", SAMPLE_PKG_LOSS_INTERMITTENT},
        {"con", SAMPLE_PKG_LOSS_CONTINUAL}
    };

    for (i = 0; i < sizeof(optional_pkg_loss_mode) / sizeof(optional_pkg_loss_mode[0]); i++) {
        if (strcmp(argv[PKG_LOSS_MODE_INDEX], optional_pkg_loss_mode[i].mode_name) == 0) {
            inst->plc_arg.pkg_loss_mode = optional_pkg_loss_mode[i].pkg_loss_mode;
            return EXT_SUCCESS;
        }
    }
    sap_printf("invalid pkg_loss mode : %s\n", argv[PKG_LOSS_MODE_INDEX]);
    return EXT_FAILURE;
}

static td_s32 sample_encode_play_parse_plc_arg(td_s32 argc, td_char *argv[], sample_encode_inst *inst)
{
    td_s32 ret;

    if (argc < PLC_TEST_ARG_NUM) {
        sample_encode_usage(argv[0]);
        return EXT_FAILURE;
    }

    ret = sample_encode_play_parse_plc_mode(argv, inst);
    if (ret != EXT_SUCCESS) {
        return ret;
    }

    inst->plc_arg.pkg_loss_period = (td_u32)strtoul(argv[PKG_LOSS_PERIOD_INDEX], TD_NULL, 0);
    inst->plc_arg.pkg_loss_rate = (td_u32)strtoul(argv[PKG_LOSS_RATE_INDEX], TD_NULL, 0);
    if (inst->plc_arg.pkg_loss_period < inst->plc_arg.pkg_loss_rate) {
        sap_printf("pkg_loss period(%u) < rate(%u)!\n", inst->plc_arg.pkg_loss_period, inst->plc_arg.pkg_loss_rate);
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

static td_s32 sample_encode_play_parse_param_type(td_char *argv[], uapi_aenc_param_type *param_type)
{
    td_u32 i;
    const struct {
        td_char *param_name;
        uapi_aenc_param_type param_type;
    } optional_param_type[] = {
        {"bit_rate", UAPI_AENC_PARAM_BIT_RATE},
    };

    for (i = 0; i < sizeof(optional_param_type) / sizeof(optional_param_type[0]); i++) {
        if (strcmp(argv[PARAM_TYPE_INDEX], optional_param_type[i].param_name) == 0) {
            *param_type = optional_param_type[i].param_type;
            return EXT_SUCCESS;
        }
    }
    sap_printf("invalid param type : %s\n", argv[PARAM_TYPE_INDEX]);
    return EXT_FAILURE;
}

static td_s32 sample_encode_dynamic_set_param(td_char *argv[], sample_encode_inst *inst)
{
    td_s32 ret;
    td_u32 bit_rate;
    uapi_aenc_param_type param_type;

    ret = sample_encode_play_parse_param_type(argv, &param_type);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_encode_play_parse_param_type, ret);
        return ret;
    }

    switch (param_type) {
        case UAPI_AENC_PARAM_BIT_RATE:
            bit_rate = (td_u32)strtoul(argv[PARAM_VALUE_INDEX], TD_NULL, 0);
            ret = uapi_aenc_set_param(inst->h_aenc, param_type, &bit_rate, sizeof(bit_rate));
            if (ret != EXT_SUCCESS) {
                sap_err_log_fun(uapi_aenc_set_param, ret);
                return ret;
            }
            break;

        default:
            break;
    }

    return EXT_SUCCESS;
}

static td_s32 sample_encode_play_start_inst(sample_encode_inst *inst)
{
    td_s32 ret;
    athread_attr attr;

    ret = uapi_aenc_start(inst->h_aenc);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_aenc_start, ret);
    }

    /* 创建线程为aenc输入adp输送pcm数据 */
    inst->task_active = TD_TRUE;
    attr.priority = ATHREAD_PRIORITY_NORMAL;
    attr.stack_size = 0x1000; /* 4k */
    attr.name = "sample_aenc_send";
    ret = athread_create(&inst->task_in, encode_adp_data_send_proc, (td_void *)inst, &attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(athread_create, ret);
    }

    inst->task_out_active = TD_TRUE;
    attr.priority = ATHREAD_PRIORITY_NORMAL;
    attr.stack_size = 0x1000; /* 4k */
    attr.name = "sample_aenc_play";
    /* 创建线程获取编码后的es流，并送给esplay播放器去播放 */
    ret = athread_create(&inst->task_out, encode_adp_data_play_proc, (td_void *)inst, &attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(athread_create, ret);
    }

    return EXT_SUCCESS;
}

static td_s32 sample_encode_play_open(sample_encode_inst *inst, const sample_encode_arg *arg)
{
    td_s32 ret;

    /* 按照输入的采样率、声道、位宽，打开一个esplay播放器 */
    ret = sample_esplay_open(&inst->aenc_arg, &inst->h_adp_play, UAPI_SND_AEF_PROFILE_MUSIC);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_esplay_open, ret);
        return ret;
    }

    ret = sample_encode_open_in_stream(inst, arg);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_encode_open_in_stream, ret);
        goto out0;
    }

    /* 打开编码通路所有实例 */
    ret = sample_encode_open_inst(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_encode_open_inst, ret);
        goto out1;
    }

    return EXT_SUCCESS;
out1:
    sample_encode_close_file(inst);
out0:
    sample_esplay_close();
    return ret;
}

static td_void sample_encode_play_close(sample_encode_inst *inst)
{
    sample_encode_close_inst(inst);
    sample_encode_close_file(inst);
    sample_esplay_close();
}

static td_s32 sample_encode_play_entry(td_s32 argc, td_char *argv[])
{
    td_s32 ret;
    sample_encode_arg *arg = (sample_encode_arg *)argv;
    sample_encode_inst *inst = sample_encode_get_inst();
    if (inst != TD_NULL) {
        if (argc == DYNAMIC_SET_PARAM_ARG_NUM) {
            ret = sample_encode_dynamic_set_param(argv, inst);
            if (ret != EXT_SUCCESS) {
                sap_err_log_fun(sample_encode_dynamic_set_param, ret);
            }
            return ret;
        } else {
            sap_printf("%s is already running", argv[0]);
            return EXT_FAILURE;
        }
    }

    if (argc < (td_s32)(sizeof(*arg) / sizeof(td_char *))) {
        sample_encode_usage(argv[0]);
        return EXT_FAILURE;
    }

    inst = sample_encode_alloc_inst();
    if (inst == TD_NULL) {
        sap_printf("sample_encode_alloc_inst failed");
        return EXT_FAILURE;
    }

    if (argc > (td_s32)(sizeof(*arg) / sizeof(td_char *))) { /* parse plc arg when arg num greater than 6 */
        ret = sample_encode_play_parse_plc_arg(argc, argv, inst);
        if (ret != EXT_SUCCESS) {
            goto out0;
        }
    }

    ret = sample_encode_ctx_init(arg, &inst->aenc_arg);
    if (ret != EXT_SUCCESS) {
        goto out0;
    }

    ret = sample_encode_play_open(inst, arg);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_encode_play_open, ret);
        goto out0;
    }

    /* start编码通道所有实例 */
    ret = sample_encode_play_start_inst(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_encode_play_start_inst, ret);
        goto out1;
    }

    return EXT_SUCCESS;
out1:
    sample_encode_play_close(inst);
out0:
    sample_encode_free_inst(inst);
    return ret;
}

static td_s32 sample_encode_play_exit(td_void)
{
    sample_encode_inst *inst = sample_encode_get_inst();
    if (inst != TD_NULL) {
        sample_encode_stop_inst(inst);
        sample_encode_close_inst(inst);
        sample_encode_close_file(inst);
        sample_esplay_close();
        sample_encode_free_inst(inst);
    }

    return EXT_SUCCESS;
}

/*
 * 编码同时播放sample
 * 从文件或者内存读入pcm数据，执行编码操作
 * 编码后的ES流通过一个esplay播放器播放
 */
td_s32 sample_encode_play(td_s32 argc, td_char *argv[])
{
    if (argc <= 1) {
        sample_encode_usage(argv[0]);
        return EXT_SUCCESS;
    }

    if (strcmp(argv[1], "q") == 0) {
        return sample_encode_play_exit();
    } else {
        return sample_encode_play_entry(argc, argv);
    }
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

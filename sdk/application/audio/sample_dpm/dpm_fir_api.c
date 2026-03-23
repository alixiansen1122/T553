/*
 * Copyright (c) CompanyNameMagicTag 2019-2020. All rights reserved.
 * Description: dpm sample
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
#include "soc_uapi_dpm.h"

#include "dpm_fir_api.h"

/*
 define DPM_PRINT_IN_OUT_DATA // macro to print input/output data
 define DPM_PRINT_INFO_LOG // macro to print info log
 define DPM_PRINT_PROC_TIME // macro to print proc cost time
 */

#ifndef EXT_SUCCESS
#define EXT_SUCCESS 0
#define EXT_FAILURE (-1)
#endif

#define fir_printf printf
#define fir_err_log_fun_err(func, err_code) fir_printf("[A][Error]Call %s Failed, Error Code: %#X\n", #func, err_code)
#define fir_err_log_u32(val)                fir_printf("[A][Error]%s = %u\n", #val, val)

#ifndef DPM_PRINT_INFO_LOG
#define fir_info_log_info(val)
#define fir_info_log_u32(val)
#else
#define fir_info_log_info(val)  fir_printf("[A][Info]<%s>\n", val)
#define fir_info_log_u32(val)   fir_printf("[A][Info]%s = %u\n", #val, val)
#endif

#ifndef DPM_PRINT_IN_OUT_DATA
#define fir_log_s32(val)
#define fir_log_u32(val)
#define fir_log_dbl(val)
#else
#define fir_log_s32(val)   fir_printf("%s = %d\n", #val, val)
#define fir_log_u32(val)   fir_printf("%s = %u\n", #val, val)
#define fir_log_dbl(val)   fir_printf("%s = %lf\n", #val, val)
#endif

#define DPM_FIR_DATA_SAMPLES_ALIGN 4 /* 4 : Requirement of fir algorithm process data num */
#define DPM_FIR_PROC_LOOP_SCALE_SHIFT 2 /* 2 : 2bits(4) for for loop process data num */
#define DPM_FIR_DEFAULT_FILTER_SAMPLES 1001
#define DPM_FIR_DEFAULT_DATA_SAMPLES 3001
#define DPM_ADP_BUF_RESERVED_SIZE 128 /* FIR input/output adp buf size, 128: reserved */

#define fir_align_up(size, align) (((size) + ((align) - 1)) & ~((align) - 1))

#include "dpm_fir_stat.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    unsigned int block_len; /* length of sample block, should be a multiple of 4 */
    unsigned int filter_len; /* length of filter, should be a multiple of 4 */
    int *filter_coef;
} dpm_fir_attr;

typedef struct dpm_fir_inst {
    uapi_dpm_id dpm_id;

    unsigned int h_dpm;
    unsigned int h_adp_in;
    unsigned int h_adp_out;

    unsigned int data_samples; /* in/out data samples, x samples */
    unsigned int coef_samples; /* filter coef samples, b samples */
    struct osal_list_head node;
} dpm_fir_inst;

static OSAL_LIST_HEAD(g_dpm_list);

static inline dpm_fir_inst *dpm_fir_alloc_inst(void)
{
    dpm_fir_inst *inst = (dpm_fir_inst *)malloc(sizeof(dpm_fir_inst));
    if (inst == NULL) {
        return NULL;
    }

    (void)memset_s(inst, sizeof(*inst), 0, sizeof(dpm_fir_inst));

    osal_list_add_tail(&inst->node, &g_dpm_list);
    return inst;
}

static inline void dpm_fir_free_inst(dpm_fir_inst *inst)
{
    if (inst == NULL) {
        return;
    }

    osal_list_del(&inst->node);
    free(inst);
}

static inline dpm_fir_inst *dpm_fir_get_inst(const dpm_fir_handle fir)
{
    dpm_fir_inst *inst = TD_NULL;

    osal_list_for_each_entry(inst, &g_dpm_list, node) {
        if (inst == (dpm_fir_inst *)fir) {
            return inst;
        }
    }

    return TD_NULL;
}

#define FIR_B_QUANTIZER_NUM 2147483648 /* b参数定标31：放大2^31之后强转成int */
#define FIR_RAW_QUANTIZER_NUM 8192 /* x参数定标13：放大2^13之后强转成int */
static inline int dpm_fir_double_to_int(double in_data, unsigned int quantizer_num)
{
    return (int)(in_data * (double)quantizer_num);
}

static inline double dpm_fir_int_to_double(int in_data, unsigned int quantizer_num)
{
    return (double)in_data / (double)quantizer_num;
}

static int dpm_fir_sys_init(void)
{
    int ret;

    ret = uapi_adp_init();
    if (ret != EXT_SUCCESS) {
        fir_err_log_fun_err(uapi_adp_init, ret);
        return ret;
    }

    ret = uapi_dpm_init();
    if (ret != EXT_SUCCESS) {
        fir_err_log_fun_err(uapi_dpm_init, ret);
        goto out;
    }

    return EXT_SUCCESS;
out:
    (void)uapi_adp_deinit();
    return ret;
}

static void dpm_fir_sys_deinit(void)
{
    int ret;

    ret = uapi_dpm_deinit();
    if (ret != EXT_SUCCESS) {
        fir_err_log_fun_err(uapi_dpm_deinit, ret);
    }

    ret = uapi_adp_deinit();
    if (ret != EXT_SUCCESS) {
        fir_err_log_fun_err(uapi_adp_deinit, ret);
    }
}

static void dpm_fir_close_adp_input(dpm_fir_inst *inst)
{
    int ret;

    if (inst->h_adp_in == 0) {
        return;
    }

    ret = uapi_adp_destroy(inst->h_adp_in);
    if (ret != EXT_SUCCESS) {
        fir_err_log_fun_err(uapi_adp_destroy, ret);
    }

    inst->h_adp_in = 0;
}

static int dpm_fir_open_adp_input(dpm_fir_inst *inst)
{
    int ret;
    int err;
    uapi_adp_attr adp_attr;

    inst->h_adp_in = 0;
    ret = uapi_adp_get_def_attr(&adp_attr);
    if (ret != EXT_SUCCESS) {
        fir_err_log_fun_err(uapi_adp_get_def_attr, ret);
        goto out;
    }

    ret = uapi_adp_create(&inst->h_adp_in, &adp_attr);
    if (ret != EXT_SUCCESS) {
        fir_err_log_fun_err(uapi_adp_create, ret);
        goto out;
    }

    return EXT_SUCCESS;
out:
    inst->h_adp_in = 0;
    return ret;
}

static void dpm_close_adp_output(dpm_fir_inst *inst)
{
    int ret;

    if (inst->h_adp_out == 0) {
        return;
    }

    ret = uapi_adp_destroy(inst->h_adp_out);
    if (ret != EXT_SUCCESS) {
        fir_err_log_fun_err(uapi_adp_destroy, ret);
    }

    inst->h_adp_out = 0;
}

static int dpm_open_adp_output(dpm_fir_inst *inst)
{
    int ret;
    uapi_adp_attr adp_attr;

    inst->h_adp_out = 0;
    ret = uapi_adp_get_def_attr(&adp_attr);
    if (ret != EXT_SUCCESS) {
        fir_err_log_fun_err(uapi_adp_get_def_attr, ret);
        goto out;
    }

    ret = uapi_adp_create(&inst->h_adp_out, &adp_attr);
    if (ret != EXT_SUCCESS) {
        fir_err_log_fun_err(uapi_adp_create, ret);
        goto out;
    }

    return EXT_SUCCESS;
out:
    inst->h_adp_out = 0;
    return ret;
}

static void dpm_fir_close_dpm(dpm_fir_inst *inst)
{
    int ret;

    if (inst->h_dpm == 0) {
        return;
    }

    ret = uapi_dpm_destroy(inst->h_dpm);
    if (ret != EXT_SUCCESS) {
        fir_err_log_fun_err(uapi_dpm_destroy, ret);
    }

    inst->h_dpm = 0;
}

static int dpm_fir_attr_init(uapi_dpm_attr *dpm_attr, const dpm_fir_frame *coef_b,
                             unsigned int coef_b_samples, unsigned int in_data_samples)
{
    unsigned int i, loop_scale, data_cnt;
    int *fir_b = NULL;
    double *fir_b_dbl = NULL;
    dpm_fir_attr *fir_attr = NULL;

    dpm_attr->param.private_data_size = coef_b_samples * (unsigned int)sizeof(int) + (unsigned int)sizeof(dpm_fir_attr);
    dpm_attr->param.private_data = malloc(dpm_attr->param.private_data_size);
    if (dpm_attr->param.private_data == NULL) {
        fir_err_log_fun_err(malloc, EXT_ERR_DPM_FIR_MEM_ALLOC);
        return EXT_ERR_DPM_FIR_MEM_ALLOC;
    }

    fir_info_log_u32(dpm_attr->param.private_data_size);

    fir_attr = (dpm_fir_attr *)dpm_attr->param.private_data;
    fir_attr->block_len = in_data_samples;
    fir_attr->filter_len = coef_b_samples;

    if (coef_b->data_buffer == NULL) {
        return EXT_SUCCESS;
    }

    fir_b = (int *)&(fir_attr->filter_coef);
    fir_b_dbl = coef_b->data_buffer;

    /* Large for loop process data num : Align down to 4 */
    data_cnt = coef_b->data_samples >> DPM_FIR_PROC_LOOP_SCALE_SHIFT << DPM_FIR_PROC_LOOP_SCALE_SHIFT;
    loop_scale = 1 << DPM_FIR_PROC_LOOP_SCALE_SHIFT; /* 1 : Initial num of for loop process data num */

    fir_time_consume_start();
    for (i = 0; i < data_cnt; i = i + loop_scale) {
        /* Trans from double to int */
        fir_b[i] = dpm_fir_double_to_int(fir_b_dbl[i], FIR_B_QUANTIZER_NUM);
        fir_b[i + 1] = dpm_fir_double_to_int(fir_b_dbl[i + 1], FIR_B_QUANTIZER_NUM); /* 1 : Index shift of array */
        fir_b[i + 2] = dpm_fir_double_to_int(fir_b_dbl[i + 2], FIR_B_QUANTIZER_NUM); /* 2 : Index shift of array */
        fir_b[i + 3] = dpm_fir_double_to_int(fir_b_dbl[i + 3], FIR_B_QUANTIZER_NUM); /* 3 : Index shift of array */
    }
    for (; i < coef_b->data_samples; i++) {
        /* Trans from double to int */
        fir_b[i] = dpm_fir_double_to_int(fir_b_dbl[i], FIR_B_QUANTIZER_NUM);
    }
    data_cnt = (coef_b_samples - i) * (unsigned int)sizeof(*fir_b);
    (void)memset_s(&fir_b[i], data_cnt, 0, data_cnt); /* Add zero to align up to 4 */

    fir_time_consume_stop("Coef b double to int");

#ifdef DPM_PRINT_IN_OUT_DATA
    for (i = 0; i < coef_b->data_samples; i++) {
        /* Logging coef b param of filter after transfer from double to int */
        fir_log_s32(fir_b[i]);
    }
#endif

    return EXT_SUCCESS;
}

static void dpm_fir_attr_deinit(uapi_dpm_attr *dpm_attr)
{
    if (dpm_attr->param.private_data != NULL) {
        free(dpm_attr->param.private_data);
    }
}

static int dpm_fir_set_attr(dpm_fir_inst *inst, const dpm_fir_frame *coef_b)
{
    int ret;
    uapi_dpm_attr dpm_attr;
    const uapi_adp_attr adp_attr = {
        .buf_size = inst->data_samples * (unsigned int)sizeof(int) + DPM_ADP_BUF_RESERVED_SIZE,
    };

    ret = uapi_adp_set_attr(inst->h_adp_in, &adp_attr);
    if (ret != EXT_SUCCESS) {
        fir_err_log_fun_err(uapi_adp_set_attr, ret);
        return ret;
    }

    ret = uapi_adp_set_attr(inst->h_adp_out, &adp_attr);
    if (ret != EXT_SUCCESS) {
        fir_err_log_fun_err(uapi_adp_set_attr, ret);
        return ret;
    }

    dpm_attr.dpm_id = inst->dpm_id;
    dpm_attr.param.private_data_size = 0;
    dpm_attr.param.private_data = NULL;

    ret = dpm_fir_attr_init(&dpm_attr, coef_b, inst->coef_samples, inst->data_samples);
    if (ret != EXT_SUCCESS) {
        fir_err_log_fun_err(dpm_fir_attr_init, ret);
        return ret;
    }

    fir_time_consume_start();
    ret = uapi_dpm_set_attr(inst->h_dpm, &dpm_attr);
    if (ret != EXT_SUCCESS) {
        fir_err_log_fun_err(uapi_dpm_set_attr, ret);
        return ret;
    }
    fir_time_consume_stop("uapi_dpm_set_attr");

    dpm_fir_attr_deinit(&dpm_attr);

    return ret;
}

static void dpm_fir_data_prepare(int *fir_x, unsigned int x_samples, const dpm_fir_frame *in_frame)
{
    int ret;
    unsigned int i, loop_scale, data_cnt;
    double *vector_x = in_frame->data_buffer;

    /* Large for loop process data num : Align down to 4 */
    data_cnt = in_frame->data_samples >> DPM_FIR_PROC_LOOP_SCALE_SHIFT << DPM_FIR_PROC_LOOP_SCALE_SHIFT;
    loop_scale = 1 << DPM_FIR_PROC_LOOP_SCALE_SHIFT; /* 1 : Initial num of for loop process data num */

    fir_time_consume_start();
    for (i = 0; i < data_cnt; i = i + loop_scale) {
        fir_x[i] = dpm_fir_double_to_int(vector_x[i], FIR_RAW_QUANTIZER_NUM);
        fir_x[i + 1] = dpm_fir_double_to_int(vector_x[i + 1], FIR_RAW_QUANTIZER_NUM); /* 1 : Index shift of array */
        fir_x[i + 2] = dpm_fir_double_to_int(vector_x[i + 2], FIR_RAW_QUANTIZER_NUM); /* 2 : Index shift of array */
        fir_x[i + 3] = dpm_fir_double_to_int(vector_x[i + 3], FIR_RAW_QUANTIZER_NUM); /* 3 : Index shift of array */
    }
    for (; i < in_frame->data_samples; i++) {
        fir_x[i] = dpm_fir_double_to_int(vector_x[i], FIR_RAW_QUANTIZER_NUM);
    }

    data_cnt = (x_samples - i) * (unsigned int)sizeof(*fir_x);
    (void)memset_s(&fir_x[i], data_cnt, 0, data_cnt); /* Add zero to align up to 4 */

    fir_time_consume_stop("Input data double to int");

#ifdef DPM_PRINT_IN_OUT_DATA
    for (i = 0; i < in_frame->data_samples; i++) {
        /* Logging input data after transfer from double to int */
        fir_log_s32(fir_x[i]);
    }
#endif
}

static int dpm_fir_send_data(dpm_fir_inst *inst, const dpm_fir_frame *in_frame)
{
    int ret;
    int *fir_x = NULL;
    double *vector_x = NULL;
    uapi_stream_buf stream = { 0 };
    unsigned int i, loop_scale, data_cnt;
    const unsigned int in_data_size = inst->data_samples * (unsigned int)sizeof(int);

    if (in_frame->data_buffer == NULL) {
        fir_printf("in_frame->data_buffer null pointer\n");
        return EXT_ERR_DPM_FIR_NULL_PTR;
    }

    ret = uapi_adp_get_buffer(inst->h_adp_in, &stream);
    if (ret != EXT_SUCCESS || stream.size < in_data_size) {
        fir_err_log_u32(stream.size);
        fir_err_log_u32(in_data_size);
        fir_err_log_fun_err(uapi_adp_get_buffer, ret);
        return ret;
    }

    /* Transfer input data from double to int && padding zeros to the end of input data */
    dpm_fir_data_prepare((int *)stream.data, inst->data_samples, in_frame);

    stream.size = in_data_size;

    fir_info_log_u32(in_data_size);

    ret = uapi_adp_put_buffer(inst->h_adp_in, &stream);
    if (ret != EXT_SUCCESS) {
        fir_err_log_fun_err(uapi_adp_put_buffer, ret);
        return ret;
    }

    return ret;
}

static int dpm_fir_data_post_proc(unsigned int coef_samples,
                                  unsigned char *out_buf,
                                  unsigned int out_buf_size,
                                  dpm_fir_frame *out_frame)
{
    int *buf = NULL;
    double *fir_res = NULL;
    unsigned int i, loop_scale;
    unsigned int data_cnt = coef_samples / 2; /* 2 : Half of the coef b samples */
    unsigned int busy_size = data_cnt * (unsigned int)sizeof(int); /* Data size of data cnt, samples to bytes */
    const unsigned int out_data_size = out_frame->data_samples * (unsigned int)sizeof(int);

    if (out_frame->data_buffer == NULL) {
        fir_printf("out_frame->data_buffer null pointer\n");
        return EXT_ERR_DPM_FIR_NULL_PTR;
    }

    if (busy_size >= out_buf_size) {
        fir_err_log_u32(busy_size);
        fir_err_log_u32(out_buf_size);
        fir_printf("Out data size && coef b size not matching\n");
        return EXT_ERR_DPM_FIR_INVALID_PARAM;
    }

    buf = (int *)out_buf + data_cnt; /* Shift output data by skipping data_cnt samples at the begginning */
    busy_size = out_buf_size - busy_size; /* Update out buf size for skipping busy_size at the begginning */

    busy_size = busy_size < out_data_size ? busy_size : out_data_size; /* Set limit of output data size */
    out_frame->data_samples = busy_size / (unsigned int)sizeof(int); /* Actual output data samples, bytes to samples */

    /* Largest samples cnt for loop process data num : Align down to 4 */
    data_cnt = out_frame->data_samples >> DPM_FIR_PROC_LOOP_SCALE_SHIFT << DPM_FIR_PROC_LOOP_SCALE_SHIFT;
    loop_scale = 1 << DPM_FIR_PROC_LOOP_SCALE_SHIFT; /* 1 : Initial num of for loop process data num */

    fir_res = out_frame->data_buffer;

    fir_time_consume_start();
    for (i = 0; i < data_cnt; i = i + loop_scale) {
        fir_res[i] = dpm_fir_int_to_double(buf[i], FIR_RAW_QUANTIZER_NUM);
        fir_res[i + 1] = dpm_fir_int_to_double(buf[i + 1], FIR_RAW_QUANTIZER_NUM); /* 1 : Index shift of array */
        fir_res[i + 2] = dpm_fir_int_to_double(buf[i + 2], FIR_RAW_QUANTIZER_NUM); /* 2 : Index shift of array */
        fir_res[i + 3] = dpm_fir_int_to_double(buf[i + 3], FIR_RAW_QUANTIZER_NUM); /* 3 : Index shift of array */
    }
    for (; i < out_frame->data_samples; i++) {
        fir_res[i] = dpm_fir_int_to_double(buf[i], FIR_RAW_QUANTIZER_NUM);
    }
    fir_time_consume_stop("Output data int to double");

    fir_info_log_u32(out_frame->data_samples);

#ifdef DPM_PRINT_IN_OUT_DATA
    for (i = 0; i < out_frame->data_samples; i++) {
        /* Logging output data after transfer from int to double */
        fir_log_dbl(fir_res[i]);
    }
#endif

    return EXT_SUCCESS;
}

static int dpm_fir_recv_data(dpm_fir_inst *inst, dpm_fir_frame *out_frame)
{
    int ret;
    unsigned int busy_size = 0;
    uapi_stream_buf stream = { 0 };
    const unsigned int exp_data_size = inst->data_samples * (unsigned int)sizeof(int);

    fir_time_consume_start();
    while (busy_size < exp_data_size) {
        ret = uapi_adp_query_busy(inst->h_adp_out, &busy_size);
        if (ret != EXT_SUCCESS) {
            fir_err_log_fun_err(uapi_adp_query_busy, ret);
            return ret;
        }
    }
    fir_time_consume_stop("Wait for output data");

    ret = uapi_adp_acquire_stream(inst->h_adp_out, &stream);
    if (ret != EXT_SUCCESS) {
        fir_err_log_fun_err(uapi_adp_acquire_stream, ret);
        return ret;
    }

    /* Adjust data delay shift && transfer output from int to double */
    ret = dpm_fir_data_post_proc(inst->coef_samples, stream.data, stream.size, out_frame);
    if (ret != EXT_SUCCESS) {
        fir_err_log_fun_err(dpm_fir_data_post_proc, ret);
    }

    ret = uapi_adp_release_stream(inst->h_adp_out, &stream);
    if (ret != EXT_SUCCESS) {
        fir_err_log_fun_err(uapi_adp_release_stream, ret);
    }

    return ret;
}

static void dpm_fir_param_update(dpm_fir_inst *inst, unsigned int coef_samples, unsigned int data_samples)
{
    inst->coef_samples = fir_align_up(coef_samples, DPM_FIR_DATA_SAMPLES_ALIGN); /* Aligned to 4 */
    /* 2 : Add half of coef b samples zeros to input data */
    inst->data_samples = fir_align_up(data_samples, DPM_FIR_DATA_SAMPLES_ALIGN) + inst->coef_samples / 2;
}

static int dpm_fir_open_dpm(dpm_fir_inst *inst)
{
    int ret;

    inst->h_dpm = 0;
    ret = uapi_dpm_create(&inst->h_dpm);
    if (ret != EXT_SUCCESS) {
        fir_err_log_fun_err(uapi_dpm_create, ret);
        goto out;
    }

    return EXT_SUCCESS;
out:
    inst->h_dpm = 0;
    return ret;
}

static void dpm_fir_close_inst(dpm_fir_inst *inst)
{
    dpm_fir_close_adp_input(inst);
    dpm_fir_close_dpm(inst);
    dpm_close_adp_output(inst);
    dpm_fir_sys_deinit();
}

static int dpm_fir_open_inst(dpm_fir_inst *inst)
{
    int ret;

    ret = dpm_fir_sys_init();
    if (ret != EXT_SUCCESS) {
        fir_err_log_fun_err(dpm_fir_sys_init, ret);
        return ret;
    }

    /* Calc data/filter_coef samples with default config */
    dpm_fir_param_update(inst, DPM_FIR_DEFAULT_FILTER_SAMPLES, DPM_FIR_DEFAULT_DATA_SAMPLES);

    /*
     * 数据流向 adp_in --> dpm --> adp_out
     * 按照数据流向反方向打开实例
     * 按照数据方向attach output
     */
    ret = dpm_open_adp_output(inst);
    if (ret != EXT_SUCCESS) {
        fir_err_log_fun_err(dpm_open_adp_output, ret);
        goto out0;
    }

    ret = dpm_fir_open_dpm(inst);
    if (ret != EXT_SUCCESS) {
        fir_err_log_fun_err(dpm_fir_open_dpm, ret);
        goto out1;
    }

    ret = dpm_fir_open_adp_input(inst);
    if (ret != EXT_SUCCESS) {
        fir_err_log_fun_err(dpm_fir_open_adp_input, ret);
        goto out2;
    }

    return EXT_SUCCESS;
out2:
    dpm_fir_close_dpm(inst);
out1:
    dpm_close_adp_output(inst);
out0:
    dpm_fir_sys_deinit();
    return ret;
}

static void dpm_fir_stop_inst(dpm_fir_inst *inst)
{
    int ret;

    ret = uapi_dpm_stop(inst->h_dpm);
    if (ret != EXT_SUCCESS) {
        fir_err_log_fun_err(uapi_dpm_stop, ret);
    }
}

static int dpm_fir_start_inst(dpm_fir_inst *inst)
{
    int ret;

    ret = uapi_dpm_start(inst->h_dpm);
    if (ret != EXT_SUCCESS) {
        fir_err_log_fun_err(uapi_dpm_start, ret);
    }

    return ret;
}

static int dpm_fir_attach_inst(dpm_fir_inst *inst)
{
    int ret;

    ret = uapi_adp_attach_output(inst->h_adp_in, inst->h_dpm);
    if (ret != EXT_SUCCESS) {
        fir_err_log_fun_err(uapi_adp_attach_output, ret);
        return ret;
    }

    ret = uapi_dpm_attach_output(inst->h_dpm, inst->h_adp_out);
    if (ret != EXT_SUCCESS) {
        fir_err_log_fun_err(uapi_dpm_attach_output, ret);
        return ret;
    }

    return ret;
}

static int dpm_fir_detach_inst(dpm_fir_inst *inst)
{
    int ret;

    ret = uapi_adp_detach_output(inst->h_adp_in, inst->h_dpm);
    if (ret != EXT_SUCCESS) {
        fir_err_log_fun_err(uapi_adp_detach_output, ret);
        return ret;
    }

    ret = uapi_dpm_detach_output(inst->h_dpm, inst->h_adp_out);
    if (ret != EXT_SUCCESS) {
        fir_err_log_fun_err(uapi_dpm_detach_output, ret);
        return ret;
    }

    return ret;
}

int dpm_fir_init(dpm_fir_handle *fir)
{
    int ret;
    dpm_fir_frame coef_b = {
        .data_buffer = NULL,
    };
    dpm_fir_inst *inst = dpm_fir_get_inst(*fir);
    if (inst != NULL) {
        fir_printf("dpm fir is already running\n");
        *fir = (dpm_fir_handle)inst;
        return EXT_ERR_DPM_FIR_INST_BUSY;
    }

    inst = dpm_fir_alloc_inst();
    if (inst == NULL) {
        fir_printf("dpm_fir_alloc_inst failed\n");
        return EXT_ERR_DPM_FIR_MEM_ALLOC;
    }

    inst->dpm_id = UAPI_DPM_ID_FIR;

    ret = dpm_fir_open_inst(inst);
    if (ret != EXT_SUCCESS) {
        fir_err_log_fun_err(dpm_fir_open_inst, ret);
        goto out0;
    }

    coef_b.data_samples = inst->coef_samples;
    ret = dpm_fir_set_attr(inst, &coef_b);
    if (ret != EXT_SUCCESS) {
        fir_err_log_fun_err(dpm_fir_set_attr, ret);
        return ret;
    }

    ret = dpm_fir_attach_inst(inst);
    if (ret != EXT_SUCCESS) {
        fir_err_log_fun_err(dpm_fir_attach_inst, ret);
        return ret;
    }

    *fir = (dpm_fir_handle)inst;
    return EXT_SUCCESS;
out0:
    dpm_fir_free_inst(inst);
    return ret;
}

int dpm_fir_deinit(const dpm_fir_handle fir)
{
    int ret;
    dpm_fir_inst *inst = dpm_fir_get_inst(fir);
    if (inst != NULL) {
        dpm_fir_stop_inst(inst);

        ret = dpm_fir_detach_inst(inst);
        if (ret != EXT_SUCCESS) {
            fir_err_log_fun_err(dpm_fir_detach_inst, ret);
            return ret;
        }

        dpm_fir_close_inst(inst);
        dpm_fir_free_inst(inst);
    }

    return EXT_SUCCESS;
}

int dpm_fir_set_coef(const dpm_fir_handle fir,
                     const dpm_fir_frame *coef_a,
                     const dpm_fir_frame *coef_b,
                     unsigned int input_data_samples)
{
    int ret;
    dpm_fir_inst *inst = dpm_fir_get_inst(fir);
    if (inst == NULL) {
        fir_printf("dpm fir is not running\n");
        return EXT_ERR_DPM_FIR_NOT_AVAILABLE;
    }

    if (coef_b == NULL) {
        fir_printf("input param has null pointer\n");
        return EXT_ERR_DPM_FIR_NULL_PTR;
    }

    dpm_fir_stop_inst(inst);

    ret = dpm_fir_detach_inst(inst);
    if (ret != EXT_SUCCESS) {
        fir_err_log_fun_err(dpm_fir_detach_inst, ret);
        return ret;
    }

    /* Calc data/filter_coef samples with default config */
    dpm_fir_param_update(inst, coef_b->data_samples, input_data_samples);

    fir_info_log_u32(inst->coef_samples);
    fir_info_log_u32(inst->data_samples);

    ret = dpm_fir_set_attr(inst, coef_b);
    if (ret != EXT_SUCCESS) {
        fir_err_log_fun_err(dpm_fir_set_attr, ret);
        return ret;
    }

    ret = dpm_fir_attach_inst(inst);
    if (ret != EXT_SUCCESS) {
        fir_err_log_fun_err(dpm_fir_attach_inst, ret);
        return ret;
    }

    (void)coef_a;
    return EXT_SUCCESS;
}

int dpm_fir_proc(const dpm_fir_handle fir, const dpm_fir_frame *in_frame, dpm_fir_frame *out_frame)
{
    int ret;

    dpm_fir_inst *inst = dpm_fir_get_inst(fir);
    if (inst == NULL) {
        fir_printf("dpm fir is not running\n");
        return EXT_ERR_DPM_FIR_NOT_AVAILABLE;
    }

    if (in_frame == NULL || out_frame == NULL) {
        fir_printf("input param has null pointer\n");
        return EXT_ERR_DPM_FIR_NULL_PTR;
    }

    ret = dpm_fir_start_inst(inst);
    if (ret != EXT_SUCCESS) {
        fir_err_log_fun_err(dpm_fir_start_inst, ret);
        return ret;
    }

    /* Update samples of input data according to in_frame; 2 : Add half of coef b samples zeros to input data */
    inst->data_samples = fir_align_up(in_frame->data_samples, DPM_FIR_DATA_SAMPLES_ALIGN) + inst->coef_samples / 2;

    ret = dpm_fir_send_data(inst, in_frame);
    if (ret != EXT_SUCCESS) {
        fir_err_log_fun_err(dpm_fir_send_data, ret);
        return ret;
    }

    ret = dpm_fir_recv_data(inst, out_frame);
    if (ret != EXT_SUCCESS) {
        fir_err_log_fun_err(dpm_fir_recv_data, ret);
        return ret;
    }

    dpm_fir_stop_inst(inst);

    return EXT_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

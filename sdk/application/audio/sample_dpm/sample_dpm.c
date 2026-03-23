/*
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: dpm sample
 * Author: audio
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "osal_list.h"
#include "dpm_fir_api.h"
#include "sample_audio_api.h"
#include "sample_audio_utils.h"

#define DPM_PRINT_PROC_TIME

#define TST_CNT 10

#define M 1001 /* length of filter */
#define N 3001 /* length of sample block */

#include "dpm_fir_stat.h"

typedef struct {
    const td_char *in_stream;
    const td_char *out_stream;
    const td_char *coef_b_stream;

    FILE *in_file;
    FILE *out_file;
    FILE *coef_b_file;

    dpm_fir_frame in_frame;
    dpm_fir_frame coef_a;
    dpm_fir_frame coef_b;
    dpm_fir_frame out_frame;

    dpm_fir_handle fir;

    struct osal_list_head node;
} sample_dpm_inst;

static OSAL_LIST_HEAD(g_sample_dpm_list);

static td_void sample_dpm_show_inst(td_void)
{
    td_u32 cnt = 0;
    sample_dpm_inst *inst = TD_NULL;

    if (osal_list_empty(&g_sample_dpm_list) != TD_FALSE) {
        sap_alert_log_info("sample_dpm all exit!");
        return;
    }

    osal_list_for_each_entry(inst, &g_sample_dpm_list, node)
    {
        cnt++;
        sap_alert_log_u32(cnt);
    }
}

static sample_dpm_inst *sample_dpm_alloc_inst(td_void)
{
    sample_dpm_inst *inst = (sample_dpm_inst *)malloc(sizeof(sample_dpm_inst));
    if (inst == TD_NULL) {
        return TD_NULL;
    }

    memset_s(inst, sizeof(*inst), 0, sizeof(sample_dpm_inst));

    osal_list_add_tail(&inst->node, &g_sample_dpm_list);
    return inst;
}

static td_void sample_dpm_free_inst(sample_dpm_inst *inst)
{
    if (inst == TD_NULL) {
        return;
    }

    osal_list_del(&inst->node);
    free(inst);
}

static td_void sample_dpm_inst_init(sample_dpm_inst *inst)
{
    inst->in_stream = TD_NULL;
    inst->out_stream = TD_NULL;
    inst->coef_b_stream = TD_NULL;

    inst->in_file = TD_NULL;
    inst->out_file = TD_NULL;
    inst->coef_b_file = TD_NULL;

    inst->in_frame.data_samples = N;
    inst->coef_b.data_samples = M;
    inst->out_frame.data_samples = N;
}

static td_void sample_dpm_usage(td_void)
{
    sap_printf("\n"
               "usage: sample_dpm -i[InDataFile] -x[InDataSamples] -b[CoefBDataFile] -w[CoefBDataSamples] [OPTIONS]\n"
               "\n");
    sap_printf("                  -h     show this usage message and abort\n"
               "                  -i     input file name\n"
               "                  -o     output file name, if not specify, generate automatically\n"
               "                  -b     coef b file name\n"
               "\n");
    sap_printf("                  -x     input/output data samples, default 3001\n"
               "                  -w     coef b data samples, default 1001\n"
               "\n");
    sap_printf("for example: sample_dpm -i /user/fir_test0_x_3001.txt -x 3001 -b /user/fir_test0_b_1001.txt -w 1001\n"
               "             sample_dpm -i /user/fir_test1_x_3001.txt -x 3001 -b /user/fir_test1_b_25.txt -w 25\n"
               "             sample_dpm -i /user/fir_test2_x_3001.txt -x 3001 -b /user/fir_test2_b_79.txt -w 79\n"
               "             sample_dpm -i /user/fir_test3_x_3001.txt -x 3001 -b /user/fir_test3_b_25.txt -w 25\n"
               "             sample_dpm -i /user/fir_test4_x_3001.txt -x 3001 -b /user/fir_test4_b_79.txt -w 79\n"
               "             sample_dpm -i /user/fir_x.txt -b /user/fir_b.txt\n"
               "\n");
}

static td_s32 sample_dpm_parse_opt(td_char opt, const td_char *opt_arg, sample_dpm_inst *inst)
{
    if (opt == 'h') {
        sample_dpm_usage();
        return EXT_FAILURE;
    }

    if (opt_arg == TD_NULL) {
        sap_printf("invalid option -- '%c'\n", opt);
        return EXT_FAILURE;
    }

    switch (opt) {
        case 'i':
            inst->in_stream = opt_arg;
            sap_trace_log_info(inst->in_stream);
            break;
        case 'o':
            inst->out_stream = opt_arg;
            sap_trace_log_info(inst->out_stream);
            break;
        case 'b':
            inst->coef_b_stream = opt_arg;
            sap_trace_log_info(inst->coef_b_stream);
            break;
        case 'x':
            inst->in_frame.data_samples = (td_u32)strtoul(opt_arg, TD_NULL, 0);
            inst->out_frame.data_samples = inst->in_frame.data_samples;
            sap_trace_log_u32(inst->in_frame.data_samples);
            sap_trace_log_u32(inst->out_frame.data_samples);
            break;
        case 'w':
            inst->coef_b.data_samples = (td_u32)strtoul(opt_arg, TD_NULL, 0);
            sap_trace_log_u32(inst->coef_b.data_samples);
            break;
        default:
            sap_printf("invalid command line\n");
            sample_dpm_usage();
            return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

/* sample_dpm_parse_cmd_line: parses the command-line input */
static td_s32 sample_dpm_parse_cmd_line(td_s32 argc, td_char *argv[], sample_dpm_inst *inst)
{
    td_u32 i;
    td_s32 ret;
    td_char opt;
    parg_state arg_parse;

    /* check input arguments */
    if ((argv == TD_NULL) || (argc < 0x2)) {
        goto out;
    }

    parg_init(&arg_parse);
    while (1) {
        ret = parg_getopt(&arg_parse, argc, argv, "i:o:b:x:w:h");
        if (ret == -1) {
            break;
        }

        opt = (td_char)ret;
        if (sample_dpm_parse_opt(opt, arg_parse.optarg, inst) != EXT_SUCCESS) {
            return EXT_FAILURE;
        }
    }

    return EXT_SUCCESS;
out:
    sample_dpm_usage();
    return EXT_FAILURE;
}

static td_s32 sample_dpm_open_file(sample_dpm_inst *inst)
{
    /* open input file */
    inst->in_file = TD_NULL;

    if (inst->in_stream != TD_NULL && strcmp(inst->in_stream, "null") != 0) {
        inst->in_file = sample_audio_open_input_stream(inst->in_stream);
        if (inst->in_file == TD_NULL) {
            sap_printf("open file %s error!\n", inst->in_stream);
            return EXT_FAILURE;
        }
    }

    /* open output file */
    inst->out_file = TD_NULL;

    if (inst->out_stream == TD_NULL) {
        td_s32 ret;
        td_char file_path[FILE_PATH_LEN];
        ret = snprintf_s(file_path, FILE_PATH_LEN - 1, FILE_PATH_LEN - 1, "%s_res.txt", inst->in_stream);
        if (ret < 0) {
            sap_err_log_fun(snprintf_s, ret);
            goto out1;
        }
        inst->out_stream = file_path;
        sap_trace_log_info(inst->out_stream);
    }

    if (inst->out_stream != TD_NULL && strcmp(inst->out_stream, "null") != 0) {
        inst->out_file = fopen(inst->out_stream, "wb");
        if (inst->out_file == TD_NULL) {
            sap_printf("open file %s error!\n", inst->out_stream);
            goto out1;
        }
    }

    /* open coef_b file */
    inst->coef_b_file = TD_NULL;

    if (inst->coef_b_stream != TD_NULL && strcmp(inst->coef_b_stream, "null") != 0) {
        inst->coef_b_file = fopen(inst->coef_b_stream, "rb");
        if (inst->coef_b_file == TD_NULL) {
            sap_printf("open file %s error!\n", inst->coef_b_stream);
            goto out0;
        }
    }

    return EXT_SUCCESS;
out0:
    if (inst->out_file != TD_NULL) {
        fclose(inst->out_file);
        inst->out_file = TD_NULL;
    }

out1:
    if (inst->in_file != TD_NULL) {
        fclose(inst->in_file);
        inst->in_file = TD_NULL;
    }

    return EXT_FAILURE;
}

static td_void sample_dpm_close_file(sample_dpm_inst *inst)
{
    if (inst->in_file != TD_NULL) {
        fclose(inst->in_file);
        inst->in_file = TD_NULL;
    }

    if (inst->out_file != TD_NULL) {
        fclose(inst->out_file);
        inst->out_file = TD_NULL;
    }

    if (inst->coef_b_file != TD_NULL) {
        fclose(inst->coef_b_file);
        inst->coef_b_file = TD_NULL;
    }
}

static td_s32 sample_dpm_data_init(sample_dpm_inst *inst)
{
    td_u32 n;

    /* 1.1 Buffer for fir input */
    inst->in_frame.data_buffer = (double *)malloc(inst->in_frame.data_samples * sizeof(double));
    if (inst->in_frame.data_buffer == TD_NULL) {
        sap_err_log_fun(malloc, EXT_ERR_DPM_FIR_NULL_PTR);
        return EXT_ERR_DPM_FIR_NULL_PTR;
    }

    /* 1.2 Read input data from input file(-i) */
    n = 0;
    while (fscanf_s(inst->in_file, "%lf", &inst->in_frame.data_buffer[n]) == 1 && n < inst->in_frame.data_samples) {
        n++;
    }

    /* 2.1 Buffer for fir coef_b */
    inst->coef_b.data_buffer = (double *)malloc(inst->coef_b.data_samples * sizeof(double));
    if (inst->coef_b.data_buffer == TD_NULL) {
        sap_err_log_fun(malloc, EXT_ERR_DPM_FIR_NULL_PTR);
        return EXT_ERR_DPM_FIR_NULL_PTR;
    }

    /* 2.2 Read coef b data from coef b file(-b) */
    n = 0;
    while (fscanf_s(inst->coef_b_file, "%lf", &inst->coef_b.data_buffer[n]) == 1 && n < inst->coef_b.data_samples) {
        n++;
    }

    /* 3.1 Buffer for fir output */
    inst->out_frame.data_buffer = (double *)malloc(inst->out_frame.data_samples * sizeof(double));
    if (inst->out_frame.data_buffer == TD_NULL) {
        sap_err_log_fun(malloc, EXT_ERR_DPM_FIR_NULL_PTR);
        return EXT_ERR_DPM_FIR_NULL_PTR;
    }

    return EXT_SUCCESS;
}

static td_void sample_dpm_data_save(sample_dpm_inst *inst)
{
    td_s32 ret;
    if (inst->out_file == TD_NULL) {
        return;
    }
    for (td_u32 i = 0; i < inst->out_frame.data_samples; ++i) {
        ret = fprintf(inst->out_file, "%lf\n", inst->out_frame.data_buffer[i]);
        if (ret < 0) {
            sap_err_log_fun(fprintf, EXT_ERR_DPM_FIR_MEM_EXCEPTION);
        }
    }
}

static td_void sample_dpm_data_deinit(sample_dpm_inst *inst)
{
    if (inst->in_frame.data_buffer != TD_NULL) {
        free(inst->in_frame.data_buffer);
        inst->in_frame.data_buffer = TD_NULL;
    }

    if (inst->coef_b.data_buffer != TD_NULL) {
        free(inst->coef_b.data_buffer);
        inst->coef_b.data_buffer = TD_NULL;
    }

    if (inst->out_frame.data_buffer != TD_NULL) {
        free(inst->out_frame.data_buffer);
        inst->out_frame.data_buffer = TD_NULL;
    }
}

static td_s32 sample_dpm_init(sample_dpm_inst *inst)
{
    dpm_fir_handle *fir = &inst->fir;
    td_s32 ret = dpm_fir_init(fir);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(dpm_fir_init, ret);
        return ret;
    }

    /* Call set coef when coef change */
    ret = dpm_fir_set_coef(*fir, &inst->coef_a, &inst->coef_b, inst->in_frame.data_samples);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(dpm_fir_set_coef, ret);
        goto out;
    }

    return EXT_SUCCESS;
out:
    ret = dpm_fir_deinit(*fir);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(dpm_fir_deinit, ret);
        return ret;
    }

    return ret;
}

static td_s32 sample_dpm_deinit(const sample_dpm_inst *inst)
{
    td_s32 ret = dpm_fir_deinit(inst->fir);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(dpm_fir_deinit, ret);
        return ret;
    }

    return ret;
}

static td_s32 sample_dpm_proc(sample_dpm_inst *inst)
{
    td_s32 ret;

    sap_alert_log_info("sample dpm fir processing...");
    for (int i = 0; i < TST_CNT; ++i) {
        if (i % 2 == 0) { /* 2 : frequency to set coef of filter */
            fir_time_consume_start();
            /* Call dpm_fir_set_coef only when coef_a/coef_b changed. */
            ret = dpm_fir_set_coef(inst->fir, &inst->coef_a, &inst->coef_b, inst->in_frame.data_samples);
            if (ret != EXT_SUCCESS) {
                sap_err_log_fun(dpm_fir_set_coef, ret);
                return ret;
            }
            fir_time_consume_stop("dpm_fir_set_coef");
        }

        fir_time_consume_start();
        ret = dpm_fir_proc(inst->fir, &inst->in_frame, &inst->out_frame);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(dpm_fir_proc, ret);
            return ret;
        }
        fir_time_consume_stop("dpm_fir_proc");
    }

    return ret;
}

static td_s32 sample_dpm_entry(td_s32 argc, td_char *argv[])
{
    td_s32 ret;

    sample_dpm_inst *inst = sample_dpm_alloc_inst();
    if (inst == TD_NULL) {
        sap_printf("sample_dpm_alloc_inst failed\n");
        return EXT_FAILURE;
    }

    sample_dpm_inst_init(inst);

    ret = sample_dpm_parse_cmd_line(argc, argv, inst);
    if (ret != EXT_SUCCESS) {
        goto out0;
    }

    ret = sample_dpm_open_file(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_dpm_open_file, ret);
        return ret;
    }

    ret = sample_dpm_data_init(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_dpm_data_init, ret);
        return ret;
    }

    ret = sample_dpm_init(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_dpm_init, ret);
        return ret;
    }

    ret = sample_dpm_proc(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_dpm_proc, ret);
    }

    ret = sample_dpm_deinit(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_dpm_deinit, ret);
        return ret;
    }

    sample_dpm_data_save(inst);

    sample_dpm_data_deinit(inst);

    sample_dpm_close_file(inst);

    sample_dpm_show_inst();
out0:
    sample_dpm_free_inst(inst);
    return ret;
}

static td_s32 sample_dpm_exit(td_void)
{
    return EXT_SUCCESS;
}

/*
 * DPM sample
 * 从文件或者内存读入原始数据，执行DPM算子处理
 * DPM算子处理后的ES流写入文件或者写入内存
 */
td_s32 sample_dpm(td_s32 argc, td_char *argv[])
{
    if (strcmp(argv[1], "q") == 0) {
        return sample_dpm_exit();
    } else {
        return sample_dpm_entry(argc, argv);
    }
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

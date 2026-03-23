/*
 * Copyright (c) CompanyNameMagicTag 2020-2020. All rights reserved.
 * Description: source for sea extend algorithm
 * Author: audio
 */

#include <stdio.h>
#include <stdlib.h>
#include "securec.h"
#include "component.h"
#include "plugin_errno.h"
#include "audio_alg.h"
#include "audio_sea.h"
#include "audio_type.h"
#include "sea_alg0_api.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* defines the bit depth during audio sampling. */
typedef enum {
    BIT_DEPTH_NULL = 0, /* unknown */
    BIT_DEPTH_8 = 8,    /* 8-bit depth */
    BIT_DEPTH_16X2_LH = 15,  /* 16-bit depth */
    BIT_DEPTH_16 = 16,  /* 16-bit depth */
    BIT_DEPTH_18 = 18,  /* 18-bit depth */
    BIT_DEPTH_20 = 20,  /* 20-bit depth */
    BIT_DEPTH_24 = 24,  /* 24-bit depth */
    BIT_DEPTH_32 = 32,  /* 32-bit depth */
    BIT_DEPTH_64 = 64,  /* 64-bit depth */

    BIT_DEPTH_MAX
} sea_bit_depth;

#define SEA_LIB_FPS 50

typedef struct {
    sea_afe_type afe_type;
    sea_aai_type aai_type;
    sea_output_type out_type;
    td_u8 ref_chan_num;
    sea_pcm_format in_pcm;
    sea_pcm_format out_pcm;
} sea_lib_caps;

typedef struct {
    td_void *handle;
    td_bool enable;
    sea_lib_caps *caps;
    sea_frame_buf frame_buf;
    audio_alg_component *entry;

    td_void *data_context;
    td_void *event_context;
    sea_data_output data_output;
    sea_event_report event_report;
} sea_lib_inst;

static sea_lib_caps g_sea_caps = {
    .afe_type = SEA_AFE_SEE,
    .aai_type = SEA_AAI_ASR,
    .out_type = SEA_OUTPUT_VC_SRC,
    .ref_chan_num = 0,
    .in_pcm = {
        .channels = 1,
        .bit_depth = 16,
        .sample_rate = 16000,
        /* The frame length should match the algorithm process cycle */
        .samples_per_frame = 16000 / SEA_LIB_FPS,
    },
    .out_pcm = {
        .channels = 1,
        .bit_depth = 16,
        .sample_rate = 16000,
        .samples_per_frame = 0,
    },
};

static inline td_u32 sea_min(td_u32 x, td_u32 y)
{
    return (x <= y) ? x : y;
}

static td_u32 calc_sample_size(td_u32 channels, td_u32 bit_depth)
{
    td_u32 depth_size;

    if (bit_depth > BIT_DEPTH_NULL && bit_depth <= BIT_DEPTH_8) {
        depth_size = sizeof(td_u8);
    } else if (bit_depth > BIT_DEPTH_8 && bit_depth <= BIT_DEPTH_16) {
        depth_size = sizeof(td_u16);
    } else if (bit_depth > BIT_DEPTH_16 && bit_depth <= BIT_DEPTH_32) {
        depth_size = sizeof(td_u32);
    } else {
        return 0;
    }

    return (td_u32)(depth_size * channels);
}


/* how many samples per frame */
static td_u32 calc_samples_per_frame(td_u32 frame_size, td_u32 channels, td_u32 bit_depth)
{
    td_u32 sample_size = calc_sample_size(channels, bit_depth);
    if (sample_size == 0) {
        return 0;
    }

    return (frame_size / sample_size);
}

static td_void write_frame_buf(sea_frame_buf *frame_buf, const td_void *pcm_buf, td_u32 pcm_size, td_bool *is_full)
{
    td_void *dst_buf = TD_NULL;
    td_u32 copy_size = 0;
    td_u32 free_size = 0;

    if (frame_buf->used >= frame_buf->size) {
        frame_buf->used = 0;
    }

    free_size = frame_buf->size - frame_buf->used;
    copy_size = sea_min(pcm_size, free_size);
    dst_buf = frame_buf->addr + frame_buf->used;
    (td_void)memcpy_s(dst_buf, copy_size, pcm_buf, copy_size);
    frame_buf->used += copy_size;

    *is_full = (frame_buf->used >= frame_buf->size ? TD_TRUE : TD_FALSE);
}

static td_s32 sea_lib_get_eng_caps(sea_eng_sel *eng_sel)
{
    /* If Algorithm is AFE (Audio Front-End) type, should set one of following options:
     * 1) eng_sel->eng_ind[SEA_ECH_AFE_IND].type = SEA_AFE_SEE
     * 2) eng_sel->eng_ind[SEA_ECH_AFE_IND].type = SEA_AFE_VQE
     * 3) eng_sel->eng_ind[SEA_ECH_AFE_IND].type = SEA_AFE_AHE
     */
    eng_sel->eng_ind[SEA_ECH_AFE_IND].type = g_sea_caps.afe_type;

    /* If Algorithm is AAI (Audio AI) type, should set one of following options:
     * 1) eng_sel->eng_ind[SEA_ECH_AAI_IND].type = SEA_AAI_KWS
     * 2) eng_sel->eng_ind[SEA_ECH_AAI_IND].type = SEA_AAI_ASR
     * 3) eng_sel->eng_ind[SEA_ECH_AAI_IND].type = SEA_AAI_VID
     * 4) eng_sel->eng_ind[SEA_ECH_AAI_IND].type = SEA_AAI_ASD
     */
    eng_sel->eng_ind[SEA_ECH_AAI_IND].type = g_sea_caps.aai_type;

    return PLUGIN_SUCCESS;
}

static td_s32 sea_lib_get_def_pcm_attr(sea_pcm_attr *attr)
{
    /* Set MIC input chanel attributes */
    attr->in_pcm = g_sea_caps.in_pcm;

    /* Set reference chanel attributes */
    attr->ref_pcm = g_sea_caps.in_pcm;
    attr->ref_pcm.channels = g_sea_caps.ref_chan_num;

    /* Set output chanel attributes */
    attr->out_pcm[g_sea_caps.out_type] = g_sea_caps.out_pcm;

    return PLUGIN_SUCCESS;
}

static td_s32 sea_lib_create(sea_handle *handle, const sea_eng_sel *eng_sel, const sea_pcm_attr *attr)
{
    td_s32 ret;
    td_u32 api_size = 0;
    td_u32 buf_size = 0;
    sea_lib_inst *lib_inst = TD_NULL;
    audio_alg_component *lib_entry = TD_NULL;

    if ((eng_sel->eng_ind[SEA_ECH_AFE_IND].type != SEA_AFE_NULL) &&
        (eng_sel->eng_ind[SEA_ECH_AFE_IND].type != g_sea_caps.afe_type)) {
        return PLG_ERR_NOT_SUPPORTED;
    }

    if ((eng_sel->eng_ind[SEA_ECH_AAI_IND].type != SEA_AAI_NULL) &&
        (eng_sel->eng_ind[SEA_ECH_AAI_IND].type != g_sea_caps.aai_type)) {
        return PLG_ERR_NOT_SUPPORTED;
    }

    lib_entry = sea_alg0_report_entry();
    lib_entry->get_config(TD_NULL, ALG_COMMON_CMD_GET_API_SIZE, (td_void *)&api_size);
    lib_inst = (sea_lib_inst *)calloc(1, (sizeof(sea_lib_inst) + api_size + buf_size));
    if (lib_inst == TD_NULL) {
        return PLG_ERR_MEM_ALLOC;
    }
    lib_inst->handle = (td_void *)lib_inst + sizeof(sea_lib_inst);
    lib_inst->frame_buf.addr = lib_inst->handle + api_size;
    lib_inst->frame_buf.size = buf_size;
    lib_inst->frame_buf.samples = calc_samples_per_frame(buf_size, attr->in_pcm.channels, attr->in_pcm.bit_depth);

    (td_void)lib_entry->set_config(lib_inst->handle, ALG_COMMON_CMD_API_INIT, TD_NULL);
    ret = lib_entry->create(lib_inst->handle, attr);
    if (ret != PLUGIN_SUCCESS) {
        free((td_void *)lib_inst);
        return ret;
    }

    lib_inst->entry = lib_entry;
    lib_inst->caps = &g_sea_caps;
    *handle = (sea_handle)lib_inst;
    audio_unused(eng_sel);
    return PLUGIN_SUCCESS;
}

static td_s32 sea_lib_destroy(sea_handle handle)
{
    sea_lib_inst *lib_inst = (sea_lib_inst *)handle;

    if (lib_inst->handle != TD_NULL) {
        lib_inst->entry->destroy(lib_inst->handle);
        lib_inst->handle = TD_NULL;
    }

    free((td_void *)lib_inst);
    return PLUGIN_SUCCESS;
}

static td_s32 sea_lib_proc_frame(sea_handle handle, sea_pcm_frame *frame)
{
    td_s32 ret;
    td_bool is_full = TD_FALSE;
    sea_pcm_frame out_frm;
    sea_lib_inst *lib_inst = (sea_lib_inst *)handle;

    if (lib_inst->enable == TD_FALSE) {
        return PLG_ERR_INST_STOP;
    }

    if (frame->size > lib_inst->frame_buf.size) {
    }

    write_frame_buf(&lib_inst->frame_buf, (td_void *)frame->data, frame->size, &is_full);
    if (is_full == TD_FALSE) {
        return PLUGIN_SUCCESS;
    }

    ret = lib_inst->entry->process(lib_inst->handle, (td_void *)frame, &out_frm);
    if (ret != PLUGIN_SUCCESS) {
        return ret;
    }
    lib_inst->data_output(handle, &out_frm, lib_inst->data_context);

    return PLUGIN_SUCCESS;
}

static td_s32 sea_lib_set_enable(sea_handle handle, td_bool enable)
{
    sea_lib_inst *lib_inst = (sea_lib_inst *)handle;

    if (enable == TD_FALSE) {
        lib_inst->entry->reset(lib_inst->handle);
        lib_inst->enable = TD_FALSE;
    }

    lib_inst->enable = enable;
    return PLUGIN_SUCCESS;
}

static td_s32 sea_lib_set_scr_buf(sea_handle handle, td_void *scr_buf, td_u32 scr_size)
{
    td_u32 size = 0;
    sea_lib_inst *lib_inst = (sea_lib_inst *)handle;

    (td_void)lib_inst->entry->get_config(lib_inst->handle, SEA_ALG_CMD_GET_SCR_BUF_SIZE, (td_void *)&size);
    if ((scr_buf == TD_NULL) || (scr_size < size)) {
        return PLG_ERR_BUF_OVERFLOW;
    }

    (td_void)lib_inst->entry->set_config(lib_inst->handle, SEA_ALG_CMD_SET_SCR_BUF, scr_buf);
    return PLUGIN_SUCCESS;
}

static td_s32 sea_lib_reg_data_output(sea_handle handle, sea_data_output data_output, td_void *context)
{
    sea_lib_inst *lib_inst = (sea_lib_inst *)handle;

    lib_inst->data_context = context;
    lib_inst->data_output = data_output;
    return PLUGIN_SUCCESS;
}

static td_s32 sea_lib_reg_event_report(sea_handle handle, sea_event_report event_report, td_void *context)
{
    sea_lib_inst *lib_inst = (sea_lib_inst *)handle;

    lib_inst->event_context = context;
    lib_inst->event_report = event_report;
    return PLUGIN_SUCCESS;
}

static sea_entry ha_sea_lib_entry = {
    .name = (const td_char *)"sea_lib0",
    .sea_id = SEA_LIB_ID_EXT0,
    .version.u32 = 0x10000001,
    .description = (const td_char *)"Sea lib0 developed by XXX",
    .create = sea_lib_create,
    .destroy = sea_lib_destroy,
    .proc_frame = sea_lib_proc_frame,
    .set_enable = sea_lib_set_enable,
    .set_scr_buf = sea_lib_set_scr_buf,
    .get_eng_caps = sea_lib_get_eng_caps,
    .get_def_pcm_attr = sea_lib_get_def_pcm_attr,
    .get_item_sets = TD_NULL,
    .get_eng_attr = TD_NULL,
    .set_eng_attr = TD_NULL,
    .get_parameter = TD_NULL,
    .set_parameter = TD_NULL,
    .set_input_attr = TD_NULL,
    .set_output_attr = TD_NULL,
    .reg_data_output = sea_lib_reg_data_output,
    .reg_event_report = sea_lib_reg_event_report
};

define_component(SEA, ha_sea_lib_entry);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */


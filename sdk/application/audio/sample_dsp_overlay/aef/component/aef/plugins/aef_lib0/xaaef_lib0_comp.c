/*
 * Copyright (c) CompanyNameMagicTag 2020-2020. All rights reserved.
 * Description: source for aef extend algorithm
 * Author: audio
 */

#include <stdlib.h>
#include "component.h"
#include "core_overlay.h"
#include "xaaef_lib0_comp.h"

#define AUDIO_CH_1 1
#define BIT_DEPTH_16 16
#define SAMPLE_RATE_48K 48000

#define AW_EFFECT_CHANNEL_DEF AUDIO_CH_1
#define AW_EFFECT_BIT_DEPTH_DEF BIT_DEPTH_16
#define AW_EFFECT_SAMPLE_RATE_DEF SAMPLE_RATE_48K

typedef struct {
    td_void *aef_lib_handle;
} aef_lib_inst;

static td_s32 aef_lib_create(const td_void *aef_attr, td_void **aef)
{
    aef_lib_inst *lib_inst = TD_NULL;

    lib_inst = (aef_lib_inst *)calloc(1, sizeof(aef_lib_inst));
    if (lib_inst == TD_NULL) {
        return -1;
    }

    lib_inst->aef_lib_handle = (td_char *)calloc(1, sizeof(td_char));
    if (lib_inst->aef_lib_handle == TD_NULL) {
        free(lib_inst);
        return -1;
    }

    *aef = (td_void *)lib_inst;
    (void)aef_attr;

    return 0;
}

static td_s32 aef_lib_destroy(td_void *aef)
{
    aef_lib_inst *lib_inst = (aef_lib_inst *)aef;

    if (lib_inst == TD_NULL || lib_inst->aef_lib_handle == TD_NULL) {
        return -1;
    }

    free(lib_inst->aef_lib_handle);
    free(lib_inst);

    return 0;
}

static td_s32 aef_lib_get_input_pcm_attr(td_void *aef, audio_pcm_format *pcm_attr)
{
    pcm_attr->channels = AW_EFFECT_CHANNEL_DEF;    /* Number of channels */
    pcm_attr->bit_depth = AW_EFFECT_BIT_DEPTH_DEF;   /* ::audio_bit_depth */
    pcm_attr->sample_rate = AW_EFFECT_SAMPLE_RATE_DEF; /* ::audio_sample_rate */
    (void)(aef);
    return 0;
}

static td_s32 aef_lib_proc_frame(td_void *aef, audio_frame *in_frame, audio_frame *out_frame)
{
    td_u32 i;
    aef_lib_inst *lib_inst = (aef_lib_inst *)aef;

    if ((lib_inst == TD_NULL) || (in_frame == TD_NULL) || (out_frame == TD_NULL) ||
        (in_frame->bits_buffer == TD_NULL) || (out_frame->bits_buffer == TD_NULL)) {
        return -1;
    }

    if (out_frame->bits_bytes < in_frame->bits_bytes) {
        return -1;
    }

    for (i = 0; i < in_frame->bits_bytes; i++) {
        out_frame->bits_buffer[i] = in_frame->bits_buffer[i];
    }

    return 0;
}

static aef_component ha_aef_lib_entry = {
    .name = (const td_char *)"aef_lib0",
    .type = AEF_TYPE_SMARTPA,
    .version = 1,
    .create = aef_lib_create,
    .destroy = aef_lib_destroy,
    .set_config = TD_NULL,
    .get_config = TD_NULL,
    .set_parameter = TD_NULL,
    .get_parameter = TD_NULL,
    .set_enable = TD_NULL,
    .get_enable = TD_NULL,
    .get_max_pcm_in_size = TD_NULL,
    .get_max_pcm_out_size = TD_NULL,
    .get_input_pcm_attr = aef_lib_get_input_pcm_attr,
    .proc_frame = aef_lib_proc_frame,
};

define_component(AEF, ha_aef_lib_entry);

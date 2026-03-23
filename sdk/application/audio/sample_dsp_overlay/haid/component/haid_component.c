/*
 * Copyright (c) CompanyNameMagicTag 2022-2023. All rights reserved.
 * Description: adaptation layer for haid
 * Author: audio
 */

#include <stdlib.h>

#include "td_type.h"
#include "component.h"
#include "core_overlay.h"

#include "hearing_aid_dummy.h"
#include "haid_component.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    hearing_aid_data_buffer buffer;
    td_s8 *handle;
    td_u32 samples;
} haid_context;

static td_s32 haid_create_inst(haid_context *ctx)
{
    td_void *hearing_aids_handle = TD_NULL;
    hearing_aid_dummy_cfg cfg = {
        .bit_depth = BIT_DEPTH_16,
        .pcm_samples = HEARINGAIDS_FRAME_LENGTH_96K,
        .sample_rate = SAMPLE_RATE_96K,
        .mic_channels = AUDIO_CH_2,
        .out_channels = AUDIO_CH_1,
    };

    hearing_aids_handle = hearing_aid_open(&cfg);
    if (hearing_aids_handle == TD_NULL) {
        return -1;
    }

    hearing_aid_reset(hearing_aids_handle);

    ctx->handle = hearing_aids_handle;
    return 0;
}

static td_s32 haid_create(haid_handle *haid)
{
    td_s32 ret;
    haid_context *ctx = TD_NULL;

    ctx = (haid_context *)calloc(sizeof(haid_context), 1);
    if (ctx == TD_NULL) {
        return -1;
    }

    ctx->samples = HEARINGAIDS_FRAME_LENGTH_96K;

    ret = haid_create_inst(ctx);
    if (ret != 0) {
        free((td_void *)ctx);
        return ret;
    }

    *haid = (haid_handle)ctx;
    return 0;
}

static td_s32 haid_destroy(haid_handle haid)
{
    haid_context *ctx = (haid_context *)haid;
    if (haid == TD_NULL) {
        return -1;
    }

    hearing_aid_close(ctx->handle);

    free((td_void *)ctx);
    return 0;
}

static td_void haid_pcm_deinterlace(haid_context *inst, audio_frame *frame)
{
    td_u32 i;
    td_s16 *src = (td_s16 *)frame->bits_buffer;
    td_s16 *mic1_data = (td_s16 *)inst->buffer.mic1_data;
    td_s16 *mic2_data = (td_s16 *)inst->buffer.mic2_data;
    const td_u32 frame_len = inst->samples;
    const td_u32 len = 2;

    for (i = 0; i < frame_len; i++) {
        mic1_data[i] = src[i * len];
        mic2_data[i] = src[i * len + 1];
    }
}

static td_s32 haid_proc_frame(haid_handle handle, audio_frame *frame)
{
    haid_context *ctx = (haid_context *)handle;

    if (handle == TD_NULL || frame == TD_NULL) {
        return -1;
    }

    haid_pcm_deinterlace(ctx, frame);

    hearing_aid_process(ctx->handle, &ctx->buffer, &ctx->buffer);

    /* frame for process frame directly */
    frame->sample_rate = SAMPLE_RATE_96K;
    frame->pcm_samples = HEARINGAIDS_FRAME_LENGTH_96K;
    frame->channels = AUDIO_CH_1;
    frame->bits_bytes = (HEARINGAIDS_FRAME_LENGTH_96K << 1);
    frame->bits_buffer = (td_s32 *)ctx->buffer.out_data;

    return 0;
}

static td_s32 haid_set_param(haid_handle haid, const audio_haid_param *haid_param)
{
    haid_context *ctx = (haid_context *)haid;

    if (ctx == TD_NULL) {
        return 0;
    }

    return hearing_aid_set_config(ctx->handle, (td_void *)haid_param->param, (td_s32 *)&(haid_param->param_len));
}

static td_s32 haid_set_config(haid_handle haid, td_u32 cmd, const td_void *cfg)
{
    switch (cmd) {
        case HAID_CMD_ID_SET_PARAM:
            return haid_set_param(haid, (const audio_haid_param *)cfg);

        default:
            break;
    }

    return 0;
}

static td_s32 haid_get_param(haid_handle haid, audio_haid_param *haid_param)
{
    haid_context *ctx = (haid_context *)haid;
    if (ctx == TD_NULL) {
        return 0;
    }

    return hearing_aid_get_config(ctx->handle, (td_void *)haid_param->param, (td_s32 *)&haid_param->param_len);
}

static td_s32 haid_get_config(haid_handle haid, td_u32 cmd, td_void *cfg)
{
    switch (cmd) {
        case HAID_CMD_ID_GET_PARAM:
            return haid_get_param(haid, (audio_haid_param *)cfg);

        default:
            break;
    }

    return 0;
}

audio_haid_component g_audio_haid_component = {
    .create = haid_create,
    .destroy = haid_destroy,

    .set_config = haid_set_config,
    .get_config = haid_get_config,

    .process = haid_proc_frame,
};

define_component(HAID, g_audio_haid_component);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

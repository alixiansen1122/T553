/*
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: header file for haid lib
 * Author: audio
 */

#ifndef __HAID_COMPONENT_H__
#define __HAID_COMPONENT_H__

#include "td_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef td_void *haid_handle;

enum haid_cmd_id {
    HAID_CMD_ID_RESET,
    HAID_CMD_ID_GET_PARAM,
    HAID_CMD_ID_SET_PARAM,

    HAID_CMD_ID_MAX,
};

/* defines the audio frame. */
typedef struct {
    td_bool interleaved;  /* whether the data is interleaved */
    td_u32 channels;     /* Number of Channels */
    td_u32 bit_depth;    /* Bit Depth */
    td_u32 sample_rate;  /* Sampling Rate */
    td_s64 pts;          /* presentation time stamp (PTS, unit: us) */
    td_s32 *pcm_buffer;  /* pointer to the buffer for storing the pulse code modulation (PCM) data */
    td_s32 *bits_buffer; /* pointer to the buffer for storing the stream data */
    td_u32 bits_bytes;   /* IEC61937 data size */
    td_u32 pcm_samples;  /* number of sampling points of the PCM data */
    td_u32 frame_index;  /* Frame ID */

    /* eos */
    /* CNcomment: eos */
    td_bool eos;
    /* the packet loss */
    /* CNcomment: 是否丢包 CNend */
    td_bool pkg_loss;
} audio_frame;

typedef struct {
    const td_s8 *param;
    td_u32 param_len;
} audio_haid_param;

typedef struct {
    td_s32 (*create)(haid_handle *haid);
    td_s32 (*destroy)(haid_handle haid);

    td_s32 (*set_config)(haid_handle haid, td_u32 cmd, const td_void *cfg);
    td_s32 (*get_config)(haid_handle haid, td_u32 cmd, td_void *cfg);

    td_s32 (*process)(haid_handle haid, audio_frame *frame);
} audio_haid_component;

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* __HAID_COMPONENT_H__ */

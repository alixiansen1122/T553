/*
 * Copyright (c) CompanyNameMagicTag 2020-2020. All rights reserved.
 * Description: header file for aef lib
 * Author: audio
 */

#ifndef __XAAEF_LIB0_COMP_H__
#define __XAAEF_LIB0_COMP_H__

#include "td_type.h"

typedef enum {
    /* invalid audio effect */
    /* CNcomment: 无效音效 */
    AEF_TYPE_NULL = 0x000,

    /* Dolby audio effect */
    /* CNcomment: Dolby音效 */
    AEF_TYPE_DOLBY = 0x001,

    /* SRS audio effect */
    /* CNcomment: SRS音效 */
    AEF_TYPE_SRS3D = 0x010,

    /* Customer audio effect */
    /* CNcomment: 客户自研音效 */
    AEF_TYPE_CUSTOMER = 0x020,

    /* Customer A audio effect */
    /* CNcomment: 客户A的自研音效 */
    AEF_TYPE_CUSTOMER_A = 0x021,

    /* LP audio effect */
    /* CNcomment: LP音效 */
    AEF_TYPE_LP = 0x100,

    /* DBX audio effect */
    /* CNcomment: DBX音效 */
    AEF_TYPE_DBX = 0x110,

    /* DBX2 audio effect */
    /* CNcomment: DBX2音效 */
    AEF_TYPE_DBX2 = 0x111,

    /* SPECTRUM */
    /* CNcomment: SPECTRUM组件 */
    AEF_TYPE_SPECTRUM = 0x120,

    /* SWS3D V1.0 audio effect */
    /* CNcomment: SWS3D V1.0音效 */
    AEF_TYPE_SWS1 = 0x130,

    /* SWS3D V2.0 audio effect */
    /* CNcomment: SWS3D V2.0音效 */
    AEF_TYPE_SWS2 = 0x131,

    /* SWS3D V3.0 audio effect */
    /* CNcomment: SWS3D V3.0音 */
    AEF_TYPE_SWS3 = 0x132,

    /* KDE audio effect */
    /* CNcomment: 卡拉OK音效 */
    AEF_TYPE_KDE = 0x150,

    /* SOOVEN audio effect */
    /* CNcomment: SOOVEN音效 */
    AEF_TYPE_SOOVEN = 0x160,

    /* SOOVEN2 audio effect */
    /* CNcomment: SOOVEN2音效 */
    AEF_TYPE_SOOVEN2 = 0x161,

    /* SOOVEN3 audio effect */
    /* CNcomment: SOOVEN3音效 */
    AEF_TYPE_SOOVEN3 = 0x162,

    /* global audio effect */
    /* CNcomment: 全局音效 */
    SND_AEF_TYPE_GLOBAL = 0x170,

    /* supplemental1 audio effect */
    /* CNcomment: 补充1音效，补充全局音效的单一特性§ */
    SND_AEF_TYPE_SUPPLEMENTAL1 = 0x171,

    /* supplemental2 audio effect */
    /* CNcomment: 补充2音效，补充全局音效的单一特性，Reserved */
    SND_AEF_TYPE_SUPPLEMENTAL2 = 0x172,

    /* SMARTPA audio effect */
    /* CNcomment: SMARTPA音效 */
    AEF_TYPE_SMARTPA = 0x180,
} aef_type;

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

/* defines audio format */
typedef struct {
    td_u32 channels;    /* Number of channels */
    td_u32 bit_depth;   /* ::audio_bit_depth */
    td_u32 sample_rate; /* ::audio_sample_rate */
    td_u32 samples_per_frame;
} audio_pcm_format;

typedef struct {
    td_u64 reserved;
    const td_char *name;
    aef_type type;
    td_u32 version;
    td_s32 (*get_default_open_param)(td_void *open_param, td_u32 param_size);
    td_s32 (*create)(const td_void *aef_attr, td_void **aef);
    td_s32 (*destroy)(td_void *aef);
    td_s32 (*set_config)(td_void *aef, td_u32 cmd, const td_void *cfg);
    td_s32 (*get_config)(td_void *aef, td_u32 cmd, td_void *cfg);
    td_s32 (*set_parameter)(td_void *aef, td_u32 cmd, const td_void *parameter, td_u32 param_size);
    td_s32 (*get_parameter)(td_void *aef, td_u32 cmd, td_void *parameter, td_u32 param_size);

    td_s32 (*set_enable)(td_void *aef, td_bool enable);
    td_s32 (*get_enable)(td_void *aef, td_bool *enable);
    td_s32 (*get_max_pcm_in_size)(td_void *aef, td_u32 *in_size);
    td_s32 (*get_max_pcm_out_size)(td_void *aef, td_u32 *out_size);
    td_s32 (*get_input_pcm_attr)(td_void *aef, audio_pcm_format *pcm_attr);
    td_s32 (*proc_frame)(td_void *aef, audio_frame *in_frame, audio_frame *out_frame);
} aef_component;

#endif

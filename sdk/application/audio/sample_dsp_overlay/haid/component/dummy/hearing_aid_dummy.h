/*
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: header file for haid lib
 * Author: audio
 */

#ifndef __HEARING_AID_DUMMY_H__
#define __HEARING_AID_DUMMY_H__

#include "td_type.h"

typedef enum {
    SAMPLE_RATE_NULL = 0,      /* unknown */
    SAMPLE_RATE_4K = 4000,     /* 4 kHz sampling rate */
    SAMPLE_RATE_8K = 8000,     /* 8 kHz sampling rate */
    SAMPLE_RATE_11K = 11025,   /* 11.025 kHz sampling rate */
    SAMPLE_RATE_12K = 12000,   /* 12 kHz sampling rate */
    SAMPLE_RATE_16K = 16000,   /* 16 kHz sampling rate */
    SAMPLE_RATE_22K = 22050,   /* 22.050 kHz sampling rate */
    SAMPLE_RATE_24K = 24000,   /* 24 kHz sampling rate */
    SAMPLE_RATE_32K = 32000,   /* 32 kHz sampling rate */
    SAMPLE_RATE_44K = 44100,   /* 44.1 kHz sampling rate */
    SAMPLE_RATE_48K = 48000,   /* 48 kHz sampling rate */
    SAMPLE_RATE_88K = 88200,   /* 88.2 kHz sampling rate */
    SAMPLE_RATE_96K = 96000,   /* 96 kHz sampling rate */
    SAMPLE_RATE_176K = 176400, /* 176 kHz sampling rate */
    SAMPLE_RATE_192K = 192000, /* 192 kHz sampling rate */
    SAMPLE_RATE_288K = 288000, /* 288 kHz sampling rate */
    SAMPLE_RATE_384K = 384000, /* 384 kHz sampling rate */
    SAMPLE_RATE_768K = 768000, /* 768 kHz sampling rate */
    SAMPLE_RATE_MAX
} audio_sample_rate;

/* defines the bit depth during audio sampling. */
typedef enum {
    BIT_DEPTH_NULL = 0, /* unknown */
    BIT_DEPTH_8 = 8,    /* 8-bit depth */
    BIT_DEPTH_16 = 16,  /* 16-bit depth */
    BIT_DEPTH_18 = 18,  /* 18-bit depth */
    BIT_DEPTH_20 = 20,  /* 20-bit depth */
    BIT_DEPTH_24 = 24,  /* 24-bit depth */
    BIT_DEPTH_32 = 32,  /* 32-bit depth */
    BIT_DEPTH_64 = 64,  /* 64-bit depth */

    BIT_DEPTH_MAX
} audio_bit_depth;

/* defines audio channels. */
typedef enum {
    AUDIO_CH_NULL = 0,
    AUDIO_CH_1 = 1,
    AUDIO_CH_MONO = AUDIO_CH_1,
    AUDIO_CH_2 = 2,
    AUDIO_CH_STEREO = AUDIO_CH_2,
    AUDIO_CH_4 = 4,
    AUDIO_CH_6 = 6,
    AUDIO_CH_8 = 8,
    AUDIO_CH_16 = 16,
    AUDIO_CH_MAX
} audio_channel;

typedef struct {
    td_u32 bit_depth;   /* Bit Depth */
    td_u32 pcm_samples; /* number of sampling points of the PCM data */
    td_u32 sample_rate; /* Sampling Rate */

    td_u32 mic_channels; /* Number of Mic Channels */
    td_u32 out_channels; /* Number of Out Channels */
} hearing_aid_dummy_cfg;

#define HEARINGAIDS_FRAME_LENGTH_96K 192

typedef struct {
    td_s16 mic1_data[HEARINGAIDS_FRAME_LENGTH_96K];
    td_s16 mic2_data[HEARINGAIDS_FRAME_LENGTH_96K];
    td_s16 out_data[HEARINGAIDS_FRAME_LENGTH_96K];
} hearing_aid_data_buffer;

void *hearing_aid_open(const hearing_aid_dummy_cfg *config);

void hearing_aid_process(void *ins, void *in, void *out);

void hearing_aid_close(void *ins);

void hearing_aid_reset(void *ins);

int hearing_aid_set_config(void *ins, const void *value, int *length);

int hearing_aid_get_config(void *ins, void *value, int *length);

#endif /* __HEARING_AID_DUMMY_H__ */

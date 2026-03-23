/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: sample audio utils
 * Author: audio
 * Create: 2021-07-27
 */

#ifndef __SAMPLE_AUDIO_UTILS_H__
#define __SAMPLE_AUDIO_UTILS_H__

#include <stdio.h>
#include "soc_uapi_audio.h"
#include "soc_uapi_acodec.h"

typedef struct {
    const char *optarg;   /* pointer to option argument, if any */
    int optind;           /* next index in argv to process */
    int optopt;           /* option value resulting in error, if any */
    const char *nextchar; /* next character to process */
} parg_state;

td_void parg_init(parg_state *ps);

td_s32 parg_getopt(parg_state *ps, int argc, td_char *const argv[], const td_char *optstring);

td_void parse_audio_params_from_file(const td_char *file, uapi_audio_pcm_format *pcm_format);

FILE *sample_audio_open_input_stream(const td_char *stream);

td_u32 calc_pcm_frame_size(const uapi_audio_pcm_format *pcm_fmt);

td_u32 calc_audio_frame_size(const uapi_audio_frame *frame);

td_s32 sample_audio_get_acodec_id(const td_char *name, uapi_acodec_id *acodec_id);

td_void get_i2s_attr(uapi_audio_i2s_attr *i2s_attr, const uapi_audio_pcm_format *pcm_fmt, td_bool local_out_port);

td_void flac_skip_header(FILE *in);

#endif

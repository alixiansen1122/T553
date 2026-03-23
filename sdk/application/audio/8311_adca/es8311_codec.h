/*
 * Copyright (c) CompanyNameMagicTag 2020-2021. All rights reserved.
 * Description: source file for module operation
 */


#ifndef __ES8311_CODEC_H__
#define __ES8311_CODEC_H__

#include "td_type.h"

typedef enum {
    SOC_CODEC_SAMPLE_RATE_8K = 8,
    SOC_CODEC_SAMPLE_RATE_16K = 16,
    SOC_CODEC_SAMPLE_RATE_32K = 32,
    SOC_CODEC_SAMPLE_RATE_48K = 48,
} soc_codec_sample_rate;

typedef enum {
    SOC_CODEC_RESOLUTION_16BIT = 16,
    SOC_CODEC_RESOLUTION_24BIT = 24,
} soc_codec_resolution;

typedef struct {
    soc_codec_sample_rate sample_rate;
    soc_codec_resolution resolution;
} soc_codec_attribute;

td_s32 soc_codec_init(const soc_codec_attribute *codec_attr);

#endif

/*
 * Copyright (c) @CompanyNameMagicTag. 2021-2021. All rights reserved.
 * Description: bluetooth sub port
 * Author: Media Software Group
 * Create: 2021-09-28
 */

#ifndef LATM_SIMPLE_MUXER_H
#define LATM_SIMPLE_MUXER_H

#include <stdint.h>

#define BIT_NUM_ONE_BYTE 8
#define BIT_NUM_OF_INT 32

#define MAX_OF_BYTE 255

typedef enum {
    AOT_NULL,
                               // Support?                Name
    AOT_AAC_MAIN,              ///< Y                       Main
    AOT_AAC_LC,                ///< Y                       Low Complexity
    AOT_AAC_SSR,               ///< N (code in SoC repo)    Scalable Sample Rate
    AOT_AAC_LTP,               ///< Y                       Long Term Prediction
    AOT_SBR,                   ///< Y                       Spectral Band Replication
} AudioObjectType;

typedef enum {
    SAMPLE_FREQ_INDEX_96000 = 0x0,
    SAMPLE_FREQ_INDEX_88200 = 0x1,
    SAMPLE_FREQ_INDEX_64000 = 0x2,
    SAMPLE_FREQ_INDEX_48000 = 0x3,
    SAMPLE_FREQ_INDEX_44100 = 0x4,
    SAMPLE_FREQ_INDEX_32100 = 0x5,
    SAMPLE_FREQ_INDEX_24000 = 0x6,
    SAMPLE_FREQ_INDEX_22050 = 0x7,
    SAMPLE_FREQ_INDEX_16000 = 0x8,
    SAMPLE_FREQ_INDEX_12000 = 0x9,
    SAMPLE_FREQ_INDEX_11025 = 0xa,
    SAMPLE_FREQ_INDEX_8000 = 0xb,
    SAMPLE_FREQ_INDEX_7350 = 0xc,
    SAMPLE_FREQ_INDEX_INVALID = 0xf,
} SampleFreqIndex;

typedef enum {
    SINGLE_CHANNEL = 1,
    PAIR_CHANNEL = 2,
    INVALID_CHANNEL_COUNT = 16
} ChannelConfiguration;

typedef struct {
    AudioObjectType audioObjType;
    SampleFreqIndex sampleFreqIndex;
    ChannelConfiguration channelConfig;
} AudioBasicParam;

void ConstructLatmHeader12Byte(const AudioBasicParam *audioparam, uint32_t size,
    uint32_t *bitNumOfHeader, uint8_t *headerBuf, uint32_t headBufSize);

#endif
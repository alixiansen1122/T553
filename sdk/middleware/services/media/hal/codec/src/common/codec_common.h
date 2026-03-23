/*
 * Copyright (c) @CompanyNameMagicTag. 2017-2019. All rights reserved.
 * Description: avplay common
 * Author: Media Software Group
 * Create: 2017-12-22
 */
#ifndef CODEC_COMMON_H
#define CODEC_COMMON_H

#include <sys/time.h>
#include "codec_type.h"
#include "media_hal_thread_adapt.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

enum {
    VID_BITRATE_LEVEL1 = 1,
    VID_BITRATE_LEVEL2 = 52428800,
    VID_BITRATE_LEVEL3 = 83886080,
    VID_BITRATE_LEVEL4 = 104857600,
    AUD_BITRATE_LEVEL1 = 10000000,
};

#define ALGIN_LEVEL_2 2
#define ALGIN_LEVEL_4 4
#define ALGIN_LEVEL_16 16
#define ALGIN_LEVEL_INVALID (-1)

#define MS_SCALE   1000

#define AV_INVALID_PTS (-1)

#define NO_WAIT  0

#define ZERO_ARG  0

#define MAX_MSG_QUEUE_SIZE  50

#define MAX_MSG_QUEUE_PAYLOAD_SIZE  128

#define AVPLAY_SPEED_DECIMAL_PRECISION  1000

// 1024 * 16 (16k)
#define VIDEO_WIDTH_HEIGHT_MAX 16384

// 268435456 = 256M = 1024 * 1024 * 256
#define AVPLAY_MAX_VBBUFFER_SIZE 268435456

// comes from ffmpeg, more detail pls refer to ffmpeg doc
// avc decoding in ffmepg needs 64 padding bytes for input buffer
#ifndef AV_INPUT_BUFFER_PADDING_SIZE
#define AV_INPUT_BUFFER_PADDING_SIZE 64
#endif

int32_t Int32Multiple(int32_t firstNumber, int32_t secondeNumner, int32_t *result);

uint32_t Uint32Multiple(uint32_t firstNumber, uint32_t secondeNumner, uint32_t *result);

AvCodecMime GetCodecMime(const Param *attr, int len);

const char* GetCodecTypeName(CodecType typeEnum);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* CODEC_COMMON_H */

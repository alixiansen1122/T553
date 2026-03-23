/*
 * Copyright (c) @CompanyNameMagicTag. 2020-2021. All rights reserved.
 * Description: codec module
 * Author: Media Software Group
 * Create: 2020-6-20
 */

#ifndef CODEC_CONFIG_H
#define CODEC_CONFIG_H

#include <stdint.h>
#include "buffer_channel.h"
#include "codec_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

int32_t SetDevice(CodecBufferChannel *channelCtx, CodecType type, DirectionType dirType,
    const Param *params);

int32_t CodecVencSetParameter(const CodecBufferChannel *channelCtx, CodecType type,
    const Param *params);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* CODEC_CONFIG_H */
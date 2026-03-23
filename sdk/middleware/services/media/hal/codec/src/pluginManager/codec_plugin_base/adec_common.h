/*
 * Copyright (c) @CompanyNameMagicTag. 2017-2019. All rights reserved.
 * Description: video decoder comm
 * Author: Media Software Group
 * Create: 2017-12-22
 */
#ifndef AV_ADEC_COMMON_H
#define AV_ADEC_COMMON_H

#include "codec_plugin_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define ADEC_ERR_BASE 0x1000

#define ADEC_ERR_ILLEGAL_PARA    (ADEC_ERR_BASE + 1)
#define ADEC_ERR_STREAM_BUF_FULL (ADEC_ERR_BASE + 2)
#define ADEC_ERR_FRAME_BUF_EMPTY (ADEC_ERR_BASE + 3)
#define ADEC_ERR_ADEC_CREATE_ERR (ADEC_ERR_BASE + 4)
#define ADEC_RECEIVE_EOS         (ADEC_ERR_BASE + 10)

#define MAX_FF_LIB_NAME_LEN  1024

#define MAX_PLUGIN_NAME_LEN  256

#define MAX_ADEC_CAPBILITY_NUM  20

typedef enum {
    AV_AUDIO_BITRATE_16K = 0,
    AV_AUDIO_BITRATE_24K,
    AV_AUDIO_BITRATE_32K,
    AV_AUDIO_BITRATE_40K,
    AV_AUDIO_BITRATE_48K,
    AV_AUDIO_BITRATE_96K,
    AV_AUDIO_BITRATE_BUTT
} AvAudioBitrate;

typedef struct {
    CodecPluginCommon pluginCommon;
    int32_t (*pfnQueueInput)(void *adecHdl, const InputInfo *adecStream, uint32_t frameIndex, bool block);
    int32_t (*pfnDequeueOutput)(void *adecHdl, OutputInfo *outInfo, bool block);
    int32_t (*pfnQueueOutput)(void *adecHdl, OutputInfo *outInfo);
} AvAudioDecoder;

void *ADecoderGetImpl(void);

int32_t ADecoderImplUnRegister(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* AV_ADEC_COMMON_H */


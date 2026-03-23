/*
 * Copyright (c) @CompanyNameMagicTag. 2017-2019. All rights reserved.
 * Description: video decoder comm
 * Author: Media Software Group
 * Create: 2017-12-22
 */
#ifndef AV_AENC_COMMON_H
#define AV_AENC_COMMON_H

#include "codec_plugin_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define AENC_ERR_BASE 0x1000

#define AENC_ERR_INVALID_OP (AENC_ERR_BASE + 1)
#define AENC_ERR_NOTREG (AENC_ERR_BASE + 2)
#define AENC_ERR_NOMEM (AENC_ERR_BASE + 3)
#define AENC_ERR_NOFREE_CHANNEL (AENC_ERR_BASE + 4)
#define AENC_ERR_CHANNEL_NOT_CREATED (AENC_ERR_BASE + 5)
#define AENC_ERR_CHANNEL_NOT_STARTED (AENC_ERR_BASE + 6)
#define AENC_ERR_AENC_ERR (AENC_ERR_BASE + 7)
#define AENC_ERR_STREAM_BUF_FULL (AENC_ERR_BASE + 8)
#define AENC_ERR_FRAME_BUF_EMPTY (AENC_ERR_BASE + 9)
#define AENC_RECEIVE_EOS         (AENC_ERR_BASE + 10)
#define AENC_ERR_INVALID_STATE   (AENC_ERR_BASE + 11)
#define AENC_ERR_UNKNOWN (AENC_ERR_BASE + 100)

#define MAX_FF_LIB_NAME_LEN  1024

#define MAX_PLUGIN_NAME_LEN  256

#define MAX_AENC_CAPBILITY_NUM  20

typedef struct {
    CodecPluginCommon pluginCommon;
    int32_t (*pfnQueueInput)(void *aencHdl, const InputInfo *inputInfo, uint32_t frameIndex, bool block);
    int32_t (*pfnDequeueOutput)(void *aencHdl, OutputInfo *outInfo, bool block);
    int32_t (*pfnQueueOutput)(void *aencHdl, OutputInfo *outInfo);
} AvAudioEncoder;

void *AEncoderGetImpl(void);

int32_t AEncoderImplUnRegister(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* AV_AENC_COMMON_H */


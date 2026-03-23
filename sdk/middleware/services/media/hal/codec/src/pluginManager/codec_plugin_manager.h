/*
 * Copyright (c) @CompanyNameMagicTag. 2017-2019. All rights reserved.
 * Description: video decoder comm
 * Author: Media Software Group
 * Create: 2017-12-22
 */
#ifndef CODEC_PLUGIN_MANAGER_H
#define CODEC_PLUGIN_MANAGER_H

#include "adec_common.h"
#include "aenc_common.h"
#include "codec_list.h"
#include "vdec_common.h"
#include "venc_common.h"
#include "securec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef struct {
    CodecListHead listPtr;
    union {
        AvVideoDecoder videoDecoder;
        AvAudioDecoder audioDecoder;
        AvVideoEncoder videoEncoder;
        AvAudioEncoder audioEncoder;
    };
} CodecPluginNode;

int SearchLoadValidPlugin(const char *dirPath, const char *getImplSymbol, CodecType type);

void* FindSpecificPlugin(AvCodecMime mime, bool hardwareFirst, CodecType type);

int UnLoadPlugin(CodecType type);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* CODEC_PLUGIN_MANAGER_H */

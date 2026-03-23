/*
 * Copyright (c) @CompanyNameMagicTag. 2017-2019. All rights reserved.
 * Description: video decoder comm
 * Author: Media Software Group
 * Create: 2017-12-22
 */
#ifndef CODEC_PLUGIN_BASE_H
#define CODEC_PLUGIN_BASE_H

#include <stdbool.h>
#include "codec_type.h"
#include "codec_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define MAX_PLUGIN_NAME_LEN  256
#define MAX_CODEC_CAPBILITY_NUM  20

typedef struct {
    void *module;
    int8_t pluginName[MAX_PLUGIN_NAME_LEN];
    CodecCapbility capbilites[MAX_CODEC_CAPBILITY_NUM];
    int validCapbilityNum;
    bool isHardwarePlugin;
    int32_t (*pfnCreate)(void **pluginHandle, const Param *attr, int len);
    int32_t (*pfnDestroy)(void *pluginHandle);
    int32_t (*pfnStart)(void *pluginHandle);
    int32_t (*pfnStop)(void *pluginHandle);
    int32_t (*pfnReset)(void *pluginHandle);
    int32_t (*pfnGetParameter)(void *pluginHandle, Param *attr);
    int32_t (*pfnSetParameter)(void *pluginHandle, const Param *attr);
    int32_t (*pfnInvoke)(void *pluginHandle, uint32_t cmd, void *arg1, void *arg2);
} CodecPluginCommon;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* CODEC_PLUGIN_BASE_H */


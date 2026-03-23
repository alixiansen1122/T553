/*
 * Copyright (c) @CompanyNameMagicTag. 2017-2019. All rights reserved.
 * Description: avplay common
 * Author: Media Software Group
 * Create: 2017-12-22
 */
#include "codec_common.h"
#include <limits.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include "media_hal_common.h"
#include "securec.h"

#define MODULE_NAME "CODEC_COMMON"

#define CODEC_PATH_MAX_LEN     (PATH_MAX + 1)
#define AVPLAY_MAX_PRINT_STR_LEN  1024

typedef struct {
    CodecType codecType;
    const char *typeName;
} CodecTypeMap;

static CodecTypeMap g_typeMaps[] = {
    {VIDEO_DECODER, "vdec"},
    {VIDEO_ENCODER, "venc"},
    {AUDIO_DECODER, "adec"},
    {AUDIO_ENCODER, "aenc"},
    {INVALID_TYPE, "invalid type"},
};

int32_t Int32Multiple(int32_t firstNumber, int32_t secondeNumner, int32_t *result)
{
    if (result == NULL) {
        return MEDIA_HAL_ERR;
    }
    if (secondeNumner == 0) {
        *result = 0;
        return MEDIA_HAL_OK;
    }

    const int32_t intMax = 0x7fffffff;
    if (firstNumber > (intMax / secondeNumner)) {
        return MEDIA_HAL_ERR;
    }
    *result = firstNumber * secondeNumner;
    return MEDIA_HAL_OK;
}

uint32_t Uint32Multiple(uint32_t firstNumber, uint32_t secondeNumner, uint32_t *result)
{
    if (result == NULL) {
        return MEDIA_HAL_ERR;
    }
    if (secondeNumner == 0) {
        *result = 0;
        return MEDIA_HAL_OK;
    }

    const uint32_t uintMax = 0xffffffff;
    if (firstNumber > (uintMax / secondeNumner)) {
        return MEDIA_HAL_ERR;
    }
    *result = firstNumber * secondeNumner;
    return MEDIA_HAL_OK;
}

AvCodecMime GetCodecMime(const Param *attr, int len)
{
    CHK_NULL_RETURN(attr, MEDIA_HAL_ERR, "attr null");
    for (int i = 0; i < len; i++) {
        if (attr[i].key == KEY_MIMETYPE) {
            if (attr[i].size != sizeof(AvCodecMime)) {
                MEDIA_HAL_LOGE(MODULE_NAME, "param kParamIndexMimeType size wrong");
                return MEDIA_MIMETYPE_INVALID;
            }
            CHK_NULL_RETURN(attr[i].val, MEDIA_HAL_ERR, "attr val null");
            AvCodecMime upperMime = *((AvCodecMime *)(attr[i].val));
            return upperMime;
        }
    }
    return MEDIA_MIMETYPE_INVALID;
}

const char* GetCodecTypeName(CodecType typeEnum)
{
    if (typeEnum > INVALID_TYPE || typeEnum < VIDEO_DECODER) {
        return g_typeMaps[INVALID_TYPE].typeName;
    }
    return g_typeMaps[typeEnum].typeName;
}
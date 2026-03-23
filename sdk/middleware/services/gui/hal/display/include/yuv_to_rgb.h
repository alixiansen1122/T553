/*
 * Copyright (c) @CompanyNameMagicTag 2023. All rights reserved.
 * Description: yuv to rgb
 */

#ifndef YUV_TO_RGB_H
#define YUV_TO_RGB_H

#include "td_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    td_u8 *yAddr;
    td_u8 *uvAddr;
    td_u32 width;
    td_u32 height;
    td_u32 yStride;
    td_u32 uvStride;
} CscYuvBuf;

typedef struct {
    td_u8 *addr;
    td_u32 width;
    td_u32 height;
    td_u32 stride;
} CscRgbBuf;

td_s32 CscConvertYuvToRgbWithSkipLP(CscRgbBuf *dst, CscYuvBuf *src);
td_s32 CscConvertYuvToRgb(CscRgbBuf *dst, CscYuvBuf *src);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* YUV_TO_RGB_H */
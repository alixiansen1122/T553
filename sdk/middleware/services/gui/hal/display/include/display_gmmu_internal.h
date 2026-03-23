/*
Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
*/

#ifndef DISPLAY_GMMU_INTERNAL_H
#define DISPLAY_GMMU_INTERNAL_H
#include "display_type.h"

#define INTERNAL_GMMU_BUF_CNT 2

typedef struct {
    td_u32 stride;                           /* Gmmu buffer line spacing. */
    td_u32 size;                             /* Gmmu buffer Size. */
    td_u32 buffer[INTERNAL_GMMU_BUF_CNT];    /* Gmmu buffer Addr. */
} GmmuBuf;

typedef struct {
    td_u8 bpp;        /* Pixel Depth. */
    td_u32 width;     /* Memory Width. */
    td_u32 height;    /* height. */
    td_u8  offset;    /* Pixel offset for fine tuning lut contours. */
} GmmuAttr;

td_s32 GmmuInit(const GmmuAttr *gmmu_attr);
td_void GmmuAlloc(GmmuBuf *gmmu_buf);
td_void GmmuFree(GmmuBuf *gmmu_buf);
td_void GmmuDeinit(td_void);

#endif /* DISPLAY_GMMU_INTERNAL_H */

/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description : drv gfx type file
 */

#ifndef DRV_GFX_TYPE_H
#define DRV_GFX_TYPE_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef enum {
    DRV_GFX_FMT_ARGB8888 = 0x00,
    DRV_GFX_FMT_KRGB8888 = 0x01,
    DRV_GFX_FMT_ARGB4444 = 0x02,
    DRV_GFX_FMT_ARGB1555 = 0x03,
    DRV_GFX_FMT_ARGB8565 = 0x04,
    DRV_GFX_FMT_RGB888   = 0x05,
    DRV_GFX_FMT_RGB444   = 0x06,
    DRV_GFX_FMT_RGB555   = 0x07,
    DRV_GFX_FMT_RGB565   = 0x08,
    DRV_GFX_FMT_A1       = 0x09,
    DRV_GFX_FMT_A8       = 0x0A,
    DRV_GFX_FMT_A1B      = 0x0D,
    DRV_GFX_FMT_CLUT1    = 0x10,
    DRV_GFX_FMT_CLUT2    = 0x11,
    DRV_GFX_FMT_CLUT4    = 0x12,
    DRV_GFX_FMT_CLUT8    = 0x13,
    DRV_GFX_FMT_ACLUT44  = 0x14,
    DRV_GFX_FMT_ACLUT88  = 0x15,
    DRV_GFX_FMT_CLUT1B   = 0x16,
    DRV_GFX_FMT_CLUT2B   = 0x17,
    DRV_GFX_FMT_CLUT4B   = 0x18,
    DRV_GFX_FMT_A2       = 0x19,
    DRV_GFX_FMT_A4       = 0x1A,
    DRV_GFX_FMT_A2B      = 0x1B,
    DRV_GFX_FMT_A4B      = 0x1C,
    DRV_GFX_FMT_YUV420SP = 0x1D,
    DRV_GFX_FMT_MAX
} drv_gfx_fmt;

typedef enum {
    DRV_GFX_FMT_ORDER_ARGB = 0x00,
    DRV_GFX_FMT_ORDER_ARBG = 0x01,
    DRV_GFX_FMT_ORDER_AGRB = 0x02,
    DRV_GFX_FMT_ORDER_AGBR = 0x03,
    DRV_GFX_FMT_ORDER_ABRG = 0x04,
    DRV_GFX_FMT_ORDER_ABGR = 0x05,

    DRV_GFX_FMT_ORDER_RAGB = 0x06,
    DRV_GFX_FMT_ORDER_RABG = 0x07,
    DRV_GFX_FMT_ORDER_RGAB = 0x08,
    DRV_GFX_FMT_ORDER_RGBA = 0x09,
    DRV_GFX_FMT_ORDER_RBAG = 0x0a,
    DRV_GFX_FMT_ORDER_RBGA = 0x0b,

    DRV_GFX_FMT_ORDER_GRAB = 0x0c,
    DRV_GFX_FMT_ORDER_GRBA = 0x0d,
    DRV_GFX_FMT_ORDER_GARB = 0x0e,
    DRV_GFX_FMT_ORDER_GABR = 0x0f,
    DRV_GFX_FMT_ORDER_GBRA = 0x10,
    DRV_GFX_FMT_ORDER_GBAR = 0x11,

    DRV_GFX_FMT_ORDER_BRGA = 0x12,
    DRV_GFX_FMT_ORDER_BRAG = 0x13,
    DRV_GFX_FMT_ORDER_BGRA = 0x14,
    DRV_GFX_FMT_ORDER_BGAR = 0x15,
    DRV_GFX_FMT_ORDER_BARG = 0x16,
    DRV_GFX_FMT_ORDER_BAGR = 0x17,
    DRV_GFX_FMT_ORDER_MAX
} drv_gfx_argb_order;

typedef enum {
    DRV_GFX_COMPRESS_NONE = 0,
    DRV_GFX_COMPRESS_HFBC,
    DRV_GFX_COMPRESS_HFBC_ABYPASS,
    DRV_GFX_COMPRESS_MAX
} drv_gfx_compress_mode;

typedef enum {
    DRV_GFX_ALPHA_RANDOM  = 0,
    DRV_GFX_ALPHA_ALL_0   = 1,
    DRV_GFX_ALPHA_ALL_255 = 2,
    DRV_GFX_ALPHA_0_255   = 3,
    DRV_GFX_ALPHA_MAX
} drv_gfx_alpha_value;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* DRV_GFX_TYPE_H */

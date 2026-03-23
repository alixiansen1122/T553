/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description : drv dpu type file
 */

#ifndef DRV_GRAPHIC_INCLUDE_DRV_DPU_TYPE_H
#define DRV_GRAPHIC_INCLUDE_DRV_DPU_TYPE_H

#include "td_base.h"
#include "drv_gfx_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define DRV_DPU_INVALID_HANDLE 0xFFFFFFFF

typedef enum {
    DRV_DPU_LAYER_ID1 = 0,
    DRV_DPU_LAYER_ID2,
    DRV_DPU_LAYER_ID3,
    DRV_DPU_LAYER_ID4,
    DRV_DPU_LAYER_ID_MAX
} drv_dpu_layer_id;

#define DRV_DPU_MAX_LAYER_ID     DRV_DPU_LAYER_ID2

typedef enum {
    DRV_DPU_ZORDER_MODE_MOVETOP = 0,
    DRV_DPU_ZORDER_MODE_MOVEUP,
    DRV_DPU_ZORDER_MODE_MOVEBOTTOM,
    DRV_DPU_ZORDER_MODE_MOVEDOWN,
    DRV_DPU_ZORDER_MODE_MAX
} drv_dpu_zorder_mode;

typedef enum {
    DRV_DPU_BUF_MODE_NORMAL = 0,
    DRV_DPU_BUF_MODE_SINGLE,
    DRV_DPU_BUF_MODE_MAX
} drv_dpu_buf_mode;

typedef struct {
    td_bool key_en;
    td_u32 key_value;
} drv_dpu_colorkey;

typedef struct {
    td_u32 width;
    td_u32 height;
    drv_gfx_fmt layer_fmt;
    drv_dpu_layer_id layer_id;
    drv_dpu_buf_mode buf_mode;
    drv_gfx_compress_mode compress_mode;
} drv_dpu_layer_info;

typedef struct {
    td_u32 width;
    td_u32 height;
    td_u32 stride;
    td_u32 uv_stride;
    td_u32 uv_offset;
    td_u32 size;
    td_u32 phy_addr;
    td_uchar *vir_addr;
    ext_rect *update_rect;
    drv_gfx_fmt color_fmt;
    td_bool premulti_en;
    drv_gfx_compress_mode compress_mode;
} drv_dpu_surface;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

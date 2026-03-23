/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: dpu layer
 */

#ifndef DRV_GRAPHICS_LAYER_DPU_LAYER_H
#define DRV_GRAPHICS_LAYER_DPU_LAYER_H

#include "soc_osal.h"
#include "drv_dpu_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    td_bool show;
    td_u8 alpha0;
    td_u8 alpha1;
    td_u8 global_alpha;
    td_u32 width;
    td_u32 height;
    td_u32 stride;
    td_u32 current_addr;
    td_s32 pos_x;
    td_s32 pos_y;
    ext_rect disp_rect;
    ext_rect crop_rect;
    ext_rect update_region;
    drv_gfx_fmt fmt;
    drv_dpu_colorkey colorkey;
    td_bool premulti_en;
    drv_gfx_compress_mode compress_mode;
} dpu_layer_run_info;

typedef struct {
    td_bool show;
    td_bool alpha0_alpha1;
    td_bool global_alpha;
    td_bool width;
    td_bool height;
    td_bool stride;
    td_bool addr;
    td_bool disp_rect;
    td_bool crop_rect;
    td_bool fmt;
    td_bool colorkey;
    td_bool premulti_en;
    td_bool compress_mode;
} dpu_layer_run_info_change;

typedef struct {
    td_bool is_create;
    td_bool update_flag;
    td_bool vblank;
    td_u32 create_cnt;
    td_u32 screen_width;
    td_u32 screen_height;
    drv_dpu_layer_id layer_id;
    osal_spinlock layer_lock;
    dpu_layer_run_info run_info;
    dpu_layer_run_info_change run_info_change;
    drv_dpu_surface canvas_surface;
} dpu_layer_dev;

td_void dpu_layer_suspend(td_void);
td_void dpu_layer_resume(td_void);

td_s32  dpu_layer_init(void);
td_void dpu_layer_deinit(void);

td_s32 dpu_layer_create(const drv_dpu_layer_info *layer_info);
td_s32 dpu_layer_destroy(drv_dpu_layer_id layer_id);

td_s32 dpu_layer_set_alpha(drv_dpu_layer_id layer_id, td_u8 alpha);
td_s32 dpu_layer_get_alpha(drv_dpu_layer_id layer_id, td_u8 *alpha);

td_s32 dpu_layer_set_pos(drv_dpu_layer_id layer_id, td_s32 pos_x, td_s32 pos_y);
td_s32 dpu_layer_get_pos(drv_dpu_layer_id layer_id, td_s32 *pos_x, td_s32 *pos_y);

td_s32 dpu_layer_set_zorder(drv_dpu_layer_id layer_id, drv_dpu_zorder_mode zorder);
td_s32 dpu_layer_get_zorder(drv_dpu_layer_id layer_id, td_u32 *zorder);

td_s32 dpu_layer_show(drv_dpu_layer_id layer_id);
td_s32 dpu_layer_hide(drv_dpu_layer_id layer_id);

td_s32 dpu_layer_set_info(drv_dpu_layer_id layer_id, const drv_dpu_surface *surface);
td_s32 dpu_layer_get_info(drv_dpu_layer_id layer_id, drv_dpu_surface *surface);

td_s32 dpu_layer_set_colorkey(drv_dpu_layer_id layer_id, const drv_dpu_colorkey *colorkey);
td_s32 dpu_layer_get_colorkey(drv_dpu_layer_id layer_id, drv_dpu_colorkey *colorkey);

td_s32 dpu_layer_refresh(void);
td_s32 dpu_layer_wait_vsync(void);

td_s32 dpu_layer_set_vblank(drv_dpu_layer_id layer_id, td_bool vblank);
td_s32 dpu_layer_get_vblank(drv_dpu_layer_id layer_id, td_bool *vblank);

td_s32 dpu_layer_query_state(drv_dpu_layer_id layer_id);
dpu_layer_dev *dpu_layer_get_dev(drv_dpu_layer_id layer_id);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
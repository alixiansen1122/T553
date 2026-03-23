/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description : dpu ioctl head file
 */

#ifndef DRV_GRAPHICS_DRV_DPU_STRUCT_H
#define DRV_GRAPHICS_DRV_DPU_STRUCT_H

#include "drv_dpu_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef struct {
    drv_dpu_layer_id layer_id;
    td_u8 alpha;
} dpu_layer_alpha_cmd;

typedef struct {
    drv_dpu_layer_id layer_id;
    td_s32 pos_x;
    td_s32 pos_y;
} dpu_layer_pos_cmd;

typedef struct {
    drv_dpu_layer_id layer_id;
    td_u32 zorder;
} dpu_layer_zorder_cmd;

typedef struct {
    drv_dpu_layer_id layer_id;
    drv_dpu_surface surface;
} dpu_layer_info_cmd;

typedef struct {
    drv_dpu_layer_id layer_id;
    drv_dpu_colorkey colorkey;
} dpu_layer_colokey_cmd;

typedef struct {
    drv_dpu_layer_id layer_id;
    td_bool vblank;
} dpu_layer_vlank_cmd;

typedef struct {
    drv_dpu_layer_id layer_id;
    td_s32 state;
} dpu_layer_state_cmd;

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif /* DRV_GRAPHICS_DRV_DPU_STRUCT_H */

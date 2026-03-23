/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description : dpu ioctl header
 */

#ifndef DRV_GRAPHICS_IOCTL_DPU_IOCTL_H
#define DRV_GRAPHICS_IOCTL_DPU_IOCTL_H

#include "td_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

td_s32 dpu_ioctl_create_layer(td_u32 cmd, td_void *arg, td_void *file);
td_s32 dpu_ioctl_destroy_layer(td_u32 cmd, td_void *arg, td_void *file);

td_s32 dpu_ioctl_set_layer_alpha(td_u32 cmd, td_void *arg, td_void *file);
td_s32 dpu_ioctl_get_layer_alpha(td_u32 cmd, td_void *arg, td_void *file);

td_s32 dpu_ioctl_set_layer_pos(td_u32 cmd, td_void *arg, td_void *file);
td_s32 dpu_ioctl_get_layer_pos(td_u32 cmd, td_void *arg, td_void *file);

td_s32 dpu_ioctl_set_layer_zorder(td_u32 cmd, td_void *arg, td_void *file);
td_s32 dpu_ioctl_get_layer_zorder(td_u32 cmd, td_void *arg, td_void *file);

td_s32 dpu_ioctl_show_layer(td_u32 cmd, td_void *arg, td_void *file);
td_s32 dpu_ioctl_hide_layer(td_u32 cmd, td_void *arg, td_void *file);

td_s32 dpu_ioctl_set_layer_info(td_u32 cmd, td_void *arg, td_void *file);
td_s32 dpu_ioctl_get_layer_info(td_u32 cmd, td_void *arg, td_void *file);

td_s32 dpu_ioctl_set_layer_colorkey(td_u32 cmd, td_void *arg, td_void *file);
td_s32 dpu_ioctl_get_layer_colorkey(td_u32 cmd, td_void *arg, td_void *file);

td_s32 dpu_ioctl_refresh_layers(td_u32 cmd, td_void *arg, td_void *file);
td_s32 dpu_ioctl_wait_layer_vsync(td_u32 cmd, td_void *arg, td_void *file);

td_s32 dpu_ioctl_set_layer_vblank(td_u32 cmd, td_void *arg, td_void *file);
td_s32 dpu_ioctl_get_layer_vblank(td_u32 cmd, td_void *arg, td_void *file);

td_s32 dpu_ioctl_query_layer_state(td_u32 cmd, td_void *arg, td_void *file);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif /* DRV_GRAPHICS_IOCTL_DPU_IOCTL_H */
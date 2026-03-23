/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description : dpu ioctl head file
 */

#ifndef DRV_GRAPHICS_DRV_VAU_STRUCT_H
#define DRV_GRAPHICS_DRV_VAU_STRUCT_H

#include "drv_vau_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef struct {
    td_handle hanlde;
    drv_vau_surface_list surface_list;
} drv_vau_blit_cmd;

typedef struct {
    td_handle handle;
    td_bool is_sync;
    td_u32 time_out;
} drv_vau_submit_cmd;

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif /* DRV_GRAPHICS_DRV_VAU_STRUCT_H */

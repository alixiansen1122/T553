/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: vau job
 */

#ifndef DRV_GRAPHIC_VAU_JOB_VAU_JOB_H
#define DRV_GRAPHIC_VAU_JOB_VAU_JOB_H

#include "drv_vau_type.h"
#include "vau_node.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

td_s32 drv_vau_begin_job(td_handle *handle, td_void *private_data);
td_s32 drv_vau_end_job(td_handle handle, td_bool is_sync, td_u32 time_out, drv_vau_callback_func callback_func);
td_s32 drv_vau_force_cancel_job(td_handle handle);
td_s32 drv_vau_wait_all_done(td_void);
td_s32 drv_vau_fill(td_handle handle, drv_vau_surface_list *surface_list);
td_s32 drv_vau_compose(td_handle handle, drv_vau_surface_list *surface_list);
td_s32 drv_vau_blit(td_handle handle, drv_vau_surface_list *surface_list);
td_s32 drv_vau_wait_for_done(td_handle handle, td_u32 timeout);
td_s32 drv_vau_import_node(td_handle handle, const vau_hardware_node *node, const td_u32 info[][2], td_u32 size);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: vau hal vector draw file
 */

#ifndef __VAU_HAL_VECTOR_DRAW_H__
#define __VAU_HAL_VECTOR_DRAW_H__

#include "td_base.h"
#include "drv_vau_type.h"
#include "vau_hal_node.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

td_void vau_hal_vector_draw_config(vau_hardware_node *node, const drv_vau_surface *src_surface,
                                   const drv_vau_surface *dst_surface);
td_void vau_hal_vector_draw_mem_init(td_void);
td_void vau_hal_vector_draw_mem_deinit(td_void);
td_void vau_hal_set_et_buffer(vau_hardware_node *node);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __VAU_HAL_VECTOR_DRAW_H__ */

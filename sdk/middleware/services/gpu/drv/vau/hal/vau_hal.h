/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description : vau node adp file
 */

#ifndef DRV_GRAPHIC_HAL_VAU_HAL_H
#define DRV_GRAPHIC_HAL_VAU_HAL_H

#include "drv_vau_type.h"
#include "vau_hal_node.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

td_void vau_hal_set_src1(vau_hardware_node *node, const drv_vau_surface *surface, td_bool is_dma);
td_void vau_hal_set_src2(vau_hardware_node *node, const drv_vau_surface *src_surface,
                         const drv_vau_surface *dst_surface);
td_void vau_hal_set_dst(vau_hardware_node *node, const drv_vau_surface *surface, td_bool is_dma);
td_void vau_hal_set_blend_mode(vau_hardware_node *node, drv_vau_blit_opt *opt);
td_void vau_hal_set_gaussian(vau_hardware_node *node, drv_vau_blit_opt *opt);
td_void vau_hal_set_set_rop(vau_hardware_node *node, const drv_vau_rop_opt *drv_rop);
td_void vau_hal_set_colorkey(vau_hardware_node *node, const drv_vau_colorkey_opt *drv_colorkey);
td_void vau_hal_set_cbm(vau_hardware_node *node, const ext_rect *cbm_rect, td_u32 alu_mode);
td_void vau_hal_set_tile_en(const vau_hardware_node *node);
td_void vau_hal_add_next_node(vau_hardware_node *node, td_u32 phy_addr);
td_void vau_hal_enable_complete_int(vau_hardware_node *node);
td_void vau_hal_enable_lowdelay_int(vau_hardware_node *node);
td_void vau_hal_set_tunl_enable(vau_hardware_node *node);

td_void vau_hal_init(td_void);
td_void vau_hal_deinit(td_void);
td_void vau_hal_suspend(td_void);
td_void vau_hal_resume(td_void);
td_bool vau_hal_check_is_idle(td_void);
td_void vau_hal_clear_int_state(td_u32 mask);
td_void vau_hal_set_start_up(td_void);
td_s32 vau_hal_execute_node(td_u32 start_node);
td_u32 vau_hal_get_current_node(td_void);
td_u32 vau_hal_get_int_state(td_void);
td_s32 vau_hal_reset(td_bool reset);
td_void vau_hal_free_vector_mem(vau_hardware_node *node);
td_void vau_hal_apb_print_reg_cfg(const vau_hardware_node *hw_node);
td_void vau_hal_set_alpha_strb(vau_hardware_node *node, td_bool enable);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif /* DRV_GRAPHIC_HAL_VAU_HAL_H */

/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: vau_interface
 */

#ifndef DRV_GRAPHIC_NODE_VAU_NODE_H
#define DRV_GRAPHIC_NODE_VAU_NODE_H

#include "drv_vau_type.h"
#include "vau_hal_node.h"
#include "soc_osal.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* Data struct of software list node */
typedef struct {
    vau_hardware_node hw_node; /* put at start position for bytes align */
    td_handle handle;          /* job handle */
    td_u32 phy_addr;
    struct osal_list_head list_head;
} vau_sw_node;

vau_sw_node *vau_node_create(td_void);
td_s32 vau_node_add(td_handle handle, vau_sw_node *sw_node, td_bool is_display_node);
td_s32 vau_node_free(vau_sw_node *sw_node);

td_void vau_node_set_src1(vau_hardware_node *node, const drv_vau_surface *surface, td_bool is_dma);
td_void vau_node_set_src2(vau_hardware_node *node, const drv_vau_surface *src_surface,
                          const drv_vau_surface *dst_surface);
td_void vau_node_set_dst(vau_hardware_node *node, const drv_vau_surface *surface, td_bool is_dma);
td_void vau_node_set_tile_en(vau_hardware_node *node);
td_void vau_node_set_blend_mode(vau_hardware_node *node, drv_vau_blit_opt *opt);
td_void vau_node_set_gaussian(vau_hardware_node *node, drv_vau_blit_opt *opt);
td_void vau_node_set_cbm(vau_hardware_node *node, const drv_vau_surface *dst_surface,
                         const drv_vau_blit_opt *opt);
td_void vau_node_add_next_node(vau_hardware_node *node, td_u32 phy_addr);
td_void vau_node_enable_complete_int(vau_hardware_node *node);
td_void vau_node_enable_lowdelay_int(vau_hardware_node *node);
td_void vau_node_set_tunl_enable(vau_hardware_node *node);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

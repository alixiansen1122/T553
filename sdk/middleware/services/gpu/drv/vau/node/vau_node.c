/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: vau ioctl file
 */

#include "vau_node.h"
#include <securec.h>
#include "vau_mem.h"
#include "vau_list.h"
#include "vau_dfx.h"
#include "vau_hal.h"
#include "vau_hal_apb.h"

td_u32 g_vau_node_id = 0;
vau_sw_node *vau_node_create(td_void)
{
    vau_sw_node *sw_node = TD_NULL;

    sw_node = (vau_sw_node *)vau_mem_alloc(VAU_MEM_TYPE_NODE, sizeof(vau_sw_node));
    if (sw_node == TD_NULL) {
        vau_err("call vau_mem_alloc failed");
        return TD_NULL;
    }

    (td_void)memset_s(sw_node, sizeof(*sw_node), 0, sizeof(vau_sw_node));

    g_vau_node_id++;
    sw_node->hw_node.vau_node_id.u32 = g_vau_node_id;

    return sw_node;
}

td_s32 vau_node_add(td_handle handle, vau_sw_node *sw_node, td_bool is_display_node)
{
    td_s32 ret;

    ret = vau_list_set_node_finish(handle, sw_node, is_display_node);
    if (ret != EXT_SUCCESS) {
        vau_err("vau_list_set_node_finish failed: 0x%x", ret);
        return ret;
    }
#ifdef CONFIG_VAU_DFX_DEBUG
    if (vau_dfx_check_mask(VAU_DBG_MASK_PRINT_NODE_NOZERO)) {
        vau_hal_print_node_nozero(&sw_node->hw_node);
    }

    if (vau_dfx_check_mask(VAU_DBG_MASK_PRINT_NODE_ALL)) {
        vau_print("//frm \n//NodeStartAddr %#x\n", &sw_node->hw_node);
        vau_hal_print_node_all(&sw_node->hw_node, 0, sizeof(vau_hardware_node) - 0x4, 0x0);
    }

    if (vau_dfx_check_mask(VAU_DBG_MASK_PRINT_REG_CFG)) {
        vau_print("//reg config\n");
        vau_hal_print_node_all(&sw_node->hw_node, 0, sizeof(vau_hardware_node) - 0x4, 0x0); // 0~0x164
        vau_hal_apb_print_reg_cfg(&sw_node->hw_node);
    }
#endif
    return EXT_SUCCESS;
}

td_s32 vau_node_free(vau_sw_node *sw_node)
{
    vau_osi_list_free_node(sw_node);
    return EXT_SUCCESS;
}

td_void vau_node_set_src1(vau_hardware_node *node, const drv_vau_surface *surface, td_bool is_dma)
{
    vau_hal_set_src1(node, surface, is_dma);
}

td_void vau_node_set_src2(vau_hardware_node *node, const drv_vau_surface *src_surface,
                          const drv_vau_surface *dst_surface)
{
    vau_hal_set_src2(node, src_surface, dst_surface);
}

td_void vau_node_set_dst(vau_hardware_node *node, const drv_vau_surface *surface, td_bool is_dma)
{
    vau_hal_set_dst(node, surface, is_dma);
}

td_void vau_node_set_blend_mode(vau_hardware_node *node, drv_vau_blit_opt *opt)
{
    if (opt == NULL) {
        return;
    }
    vau_hal_set_blend_mode(node, opt);
}

td_void vau_node_set_gaussian(vau_hardware_node *node, drv_vau_blit_opt *opt)
{
    if (opt == NULL) {
        return;
    }
    if (opt->blur_opt.blur_en == TD_TRUE) {
        vau_hal_set_gaussian(node, opt);
    }
}

td_void vau_node_set_cbm(vau_hardware_node *node, const drv_vau_surface *dst_surface,
                         const drv_vau_blit_opt *opt)
{
    drv_vau_alu_mode alu_mode;

    if (opt == TD_NULL) {
        alu_mode = DRV_VAU_ALU_MODE_SRC2_BYPASS;
    } else {
        if (opt->rop_opt.rop_enable) {
            alu_mode = DRV_VAU_ALU_MODE_ROP;
        } else if (opt->blend_opt.blend_enable) {
            alu_mode = DRV_VAU_ALU_MODE_BLEND;
        } else {
            alu_mode = DRV_VAU_ALU_MODE_SRC2_BYPASS;
        }

        if (opt->rop_opt.rop_enable == TD_TRUE) {
            vau_hal_set_set_rop(node, &opt->rop_opt);
        }

        if (opt->colorkey_opt.colorkey_enable == TD_TRUE) {
            vau_hal_set_colorkey(node, &opt->colorkey_opt);
        }
    }

    if (dst_surface->attr.crop.crop_en) {
        vau_hal_set_cbm(node, &dst_surface->attr.crop.crop_rect, alu_mode);
    } else {
        vau_hal_set_cbm(node, &dst_surface->rect, alu_mode);
    }

    return;
}

td_void vau_node_set_tile_en(vau_hardware_node *node)
{
    vau_hal_set_tile_en(node);
}

td_void vau_node_add_next_node(vau_hardware_node *node, td_u32 phy_addr)
{
    vau_hal_add_next_node(node, phy_addr);
}

td_void vau_node_enable_complete_int(vau_hardware_node *node)
{
    g_vau_node_id = 0;
    vau_hal_enable_complete_int(node);
}

td_void vau_node_enable_lowdelay_int(vau_hardware_node *node)
{
    vau_hal_enable_lowdelay_int(node);
}

td_void vau_node_set_tunl_enable(vau_hardware_node *node)
{
    vau_hal_set_tunl_enable(node);
}

/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: vau job
 */

#include "vau_job.h"
#include "vau_dfx.h"
#include "vau_list.h"
#include "vau_handle.h"
#include "vau_hal.h"
#include "vau_hal_capability.h"
#include "pm.h"

/*-------------------------------- macro define -------------------------------------------*/

/*-------------------------------- struct define ------------------------------------------*/

/*-------------------------------- func declares ------------------------------------------*/

static td_s32 vau_job_check_surface_list(const drv_vau_surface_list *surface_list);
static td_s32 vau_job_check_every_surface(const drv_vau_surface_list *surface_list);
static td_bool vau_job_check_is_dma_mode(const drv_vau_surface_list *surface_list);
static td_bool vau_job_check_dma2d_all0mode(drv_vau_surface_list *surface_list);
static td_bool vau_job_check_dma2d_onlysrc2mode(const drv_vau_surface_list *surface_list);
static td_void vau_job_dma2d_config(vau_hardware_node *node, drv_vau_surface_list *surface_list);

/*-------------------------------- par define ---------------------------------------------*/

/*-------------------------------- func release -------------------------------------------*/

static td_s32 vau_job_add_node(td_handle handle, vau_sw_node *node)
{
    td_bool is_display_node;

    /* only check dest addr */
    is_display_node = TD_TRUE; /* double FB buffer should check */
    return vau_node_add(handle, node, is_display_node);
}

static td_void vau_job_revise_dst_rect(drv_vau_surface *dst_sur, ext_rect *refer_region)
{
    td_s32 surface_w, surface_h;
    td_s32 rect_x, rect_y, rect_w, rect_h;

    rect_x = dst_sur->rect.x;
    rect_y = dst_sur->rect.y;
    rect_w = (td_s32)dst_sur->rect.width;  /* for cmpare u32 to s32 */
    rect_h = (td_s32)dst_sur->rect.height; /* for cmpare u32 to s32 */

    vau_dbg("dst_rect_before:[%d, %d, %u, %u]",
            dst_sur->rect.x, dst_sur->rect.y, dst_sur->rect.width, dst_sur->rect.height);

    surface_w = (td_s32)dst_sur->width;    /* for cmpare u32 to s32 */
    surface_h = (td_s32)dst_sur->height;   /* for cmpare u32 to s32 */

    if (rect_x < 0) {
        rect_w = rect_x + rect_w;
        rect_x = 0;
    }

    if (rect_y < 0) {
        rect_h = rect_y + rect_h;
        rect_y = 0;
    }

    if (rect_x + rect_w > surface_w) {
        rect_w = surface_w - rect_x;
    }

    if (rect_y + rect_h > surface_h) {
        rect_h = surface_h - rect_y;
    }

    /* refer_region decide how to revise src rect */
    refer_region->x = rect_x - dst_sur->rect.x; /* x offset */
    refer_region->y = rect_y - dst_sur->rect.y; /* y offset */
    refer_region->width  = (td_u32)rect_w;              /* revise w */
    refer_region->height = (td_u32)rect_h;              /* revise h */

    dst_sur->rect.x = rect_x;
    dst_sur->rect.y = rect_y;
    dst_sur->rect.width  = (td_u32)rect_w;
    dst_sur->rect.height = (td_u32)rect_h;

    vau_dbg("dst_rect_after:[%d, %d, %u, %u]",
            dst_sur->rect.x, dst_sur->rect.y, dst_sur->rect.width, dst_sur->rect.height);

    return;
}

static td_void vau_job_revise_src_rect(drv_vau_surface *src_sur, ext_rect *refer_region)
{
    if ((src_sur->attr.affine.affine_en == TD_TRUE) ||
        (src_sur->attr.vector.enable == TD_TRUE)) {
        return; /* affine or vector not need revise */
    }

    vau_dbg("src_rect_before:[%d, %d, %u, %u]",
            src_sur->rect.x, src_sur->rect.y, src_sur->rect.width, src_sur->rect.height);

    src_sur->rect.x = src_sur->rect.x + refer_region->x;
    src_sur->rect.y = src_sur->rect.y + refer_region->y;
    src_sur->rect.width  = refer_region->width;
    src_sur->rect.height = refer_region->height;

    vau_dbg("src_rect_after:[%d, %d, %u, %u]",
            src_sur->rect.x, src_sur->rect.y, src_sur->rect.width, src_sur->rect.height);

    return;
}

static td_void vau_job_revise_rect_in_range(drv_vau_surface_list *surface_list)
{
    ext_rect refer_region = {0};

    vau_job_revise_dst_rect(surface_list->dst_surface, &refer_region);
    vau_job_revise_src_rect(&surface_list->src_surface[0], &refer_region);

    if (surface_list->src_surface_cnt == 2) { /* 2: src cnt */
        vau_job_revise_src_rect(&surface_list->src_surface[1], &refer_region);
    }

    return;
}

static td_s32 vau_check_bkg_and_dst_surface(const drv_vau_surface *bkg_sur,
                                            const drv_vau_surface *dst_sur)
{
    td_bool is_same_buf = (bkg_sur->addr == dst_sur->addr) ? TD_TRUE : TD_FALSE;

    if (is_same_buf) {
        if ((bkg_sur->fmt != dst_sur->fmt) ||
            (memcmp(&bkg_sur->rect, &dst_sur->rect, sizeof(ext_rect)) != 0) ||
            (bkg_sur->compress_info.compress_mode != dst_sur->compress_info.compress_mode)) {
            vau_err("bkg and dst surface not equal!"
                    "bkg:fmt[%d] cmp[%d] rect[%d,%d,%u,%u] dst:fmt[%d] cmp[%d] rect[%d,%d,%u,%u]!",
                    bkg_sur->fmt, bkg_sur->compress_info.compress_mode, bkg_sur->rect.x, bkg_sur->rect.y,
                    bkg_sur->rect.width, bkg_sur->rect.height, dst_sur->fmt, dst_sur->compress_info.compress_mode,
                    dst_sur->rect.x, dst_sur->rect.y, dst_sur->rect.width, dst_sur->rect.height);
            return EXT_FAILURE;
        }
    } else {
        /* if not same buf, src1 xy not support < 0 */
        if ((bkg_sur->rect.x < 0) || (bkg_sur->rect.y < 0) ||
            (bkg_sur->rect.width != dst_sur->rect.width) ||
            (bkg_sur->rect.height != dst_sur->rect.height)) {
            vau_err("bkg and dst rect_wh not same buf, bkg_xywh[%d,%d,%u,%u], dst_wh[%u,%u]!",
                    bkg_sur->rect.x, bkg_sur->rect.y, bkg_sur->rect.width, bkg_sur->rect.height,
                    dst_sur->rect.width, dst_sur->rect.height);
            return DRV_ERR_VAU_INVLIAD_RECT;
        }
    }

    return EXT_SUCCESS;
}

static td_s32 vau_check_frg_and_dst_surface(const drv_vau_surface *frg_sur,
                                            const drv_vau_surface *dst_sur)
{
    if ((frg_sur->attr.affine.affine_en == TD_TRUE) ||
        (frg_sur->attr.vector.enable == TD_TRUE)) {
        return EXT_SUCCESS;
    }

    if ((frg_sur->rect.width != dst_sur->rect.width) ||
        (frg_sur->rect.height != dst_sur->rect.height)) {
        vau_err("foreground and dst rect_wh not equal, frg_wh[%u,%u], dst_wh[%u,%u]!",
                frg_sur->rect.width, frg_sur->rect.height,
                dst_sur->rect.width, dst_sur->rect.height);
        return DRV_ERR_VAU_INVLIAD_RECT;
    }

    return EXT_SUCCESS;
}

td_s32 drv_vau_begin_job(td_handle *handle, td_void *private_data)
{
    td_s32 ret;

    ret = vau_list_beg_job(handle, private_data);
    if (ret != EXT_SUCCESS) {
        vau_err("vau_list_beg_job failed!");
        return ret;
    }

    uapi_pm_add_sleep_veto(PM_ID_VAU);

    return EXT_SUCCESS;
}

td_s32 drv_vau_end_job(td_handle handle, td_bool is_sync, td_u32 time_out, drv_vau_callback_func callback_func)
{
    td_s32 ret;
    drv_vau_notify_mode notify_type = (is_sync == TD_TRUE) ? (DRV_VAU_JOB_WAKE_NOTIFY) : (DRV_VAU_JOB_COMPL_NOTIFY);
    vau_dfx_info *vau_info = vau_dfx_get_info();

    if ((is_sync == TD_TRUE) && (osal_in_interrupt() != 0)) {
        vau_err("unsupport in interrupt");
        return DRV_ERR_VAU_UNSUPPORTED_OPERATION;
    }

    vau_info->func_info.func_cnt.submit_cnt++;
    ret = vau_list_submit_job(handle, time_out, &callback_func, notify_type);

    return ret;
}

td_s32 drv_vau_force_cancel_job(td_handle handle)
{
    td_s32 ret;
    ret = vau_list_cancel_job(handle);

    vau_handle_destroy(handle);

    uapi_pm_remove_sleep_veto(PM_ID_VAU);

    return ret;
}

td_s32 drv_vau_wait_all_done(td_void)
{
    td_s32 ret;

    if ((td_bool)osal_in_interrupt()) {
        vau_err("unsupport in interrupt");
        return DRV_ERR_VAU_UNSUPPORTED_OPERATION;
    }
    ret = vau_list_wait_all_done();

    return ret;
}

td_s32 drv_vau_wait_for_done(td_handle handle, td_u32 timeout)
{
    return vau_list_wait_for_done(handle, timeout);
}

td_s32 drv_vau_fill(td_handle handle, drv_vau_surface_list *surface_list)
{
    return drv_vau_blit(handle, surface_list);
}

td_s32 drv_vau_compose(td_handle handle, drv_vau_surface_list *surface_list)
{
    td_s32 ret;
    td_u32 surface_cnt;
    drv_vau_surface tmp_sur0 = { 0 };
    drv_vau_surface tmp_sur1 = { 0 };
    drv_vau_surface src_sur = { 0 };
    drv_vau_surface dst_sur = { 0 };

    if ((surface_list == NULL) ||
        (surface_list->src_surface == NULL) ||
        (surface_list->dst_surface == NULL) ||
        (surface_list->src_surface_cnt <= 0)) {
        vau_err("input surface_list or src surface or dst surface is null");
        return EXT_FAILURE;
    }

    surface_cnt = surface_list->src_surface_cnt;
    surface_list->src_surface_cnt = 2; /* every for 2 layers */

    if (surface_cnt == 1) { /* only 1 layers */
        tmp_sur0 = surface_list->src_surface[0];
    } else { /* max to 2 layers */
        tmp_sur0 = surface_list->src_surface[0];
        tmp_sur1 = surface_list->src_surface[1];
    }
    dst_sur = *(surface_list->dst_surface);

    for (td_u32 i = 0; i < surface_cnt; i++) {
        src_sur = surface_list->src_surface[i];
        if (i == 0) {
            dst_sur.rect = tmp_sur0.out_rect;
            surface_list->dst_surface->rect = tmp_sur0.out_rect;
            surface_list->src_surface[0] = dst_sur;
            surface_list->src_surface[1] = tmp_sur0;
        } else if (i == 1) {
            dst_sur.rect = tmp_sur1.out_rect;
            surface_list->dst_surface->rect = tmp_sur1.out_rect;
            surface_list->src_surface[0] = dst_sur;
            surface_list->src_surface[1] = tmp_sur1;
        } else {
            dst_sur.rect = src_sur.out_rect;
            surface_list->dst_surface->rect = src_sur.out_rect;
            surface_list->src_surface[0] = dst_sur;
            surface_list->src_surface[1] = src_sur;
        }

        ret = drv_vau_blit(handle, surface_list);
        if (ret != EXT_SUCCESS) {
            vau_err("call drv_vau_blit failed");
            return EXT_FAILURE;
        }
    }

    return EXT_SUCCESS;
}

static td_void drv_vau_blit_config_node(vau_hardware_node *hw_node, drv_vau_surface_list *surface_list)
{
    td_bool is_dma = vau_job_check_is_dma_mode(surface_list);
    if (is_dma) {
        vau_node_set_src1(hw_node, &surface_list->src_surface[0], is_dma);
    } else {
        if (surface_list->src_surface_cnt == 2) { /* 2:double src surface */
            if (vau_job_check_dma2d_onlysrc2mode(surface_list)) {
                vau_job_dma2d_config(hw_node, surface_list);
            } else {
                vau_node_set_src1(hw_node, &surface_list->src_surface[0], is_dma);
                vau_node_set_blend_mode(hw_node, surface_list->opt);
            }
            vau_node_set_src2(hw_node, &surface_list->src_surface[1], surface_list->dst_surface);
            vau_node_set_gaussian(hw_node, surface_list->opt);
        } else {
            vau_node_set_src2(hw_node, &surface_list->src_surface[0], surface_list->dst_surface);
            vau_node_set_gaussian(hw_node, surface_list->opt);
        }
        vau_node_set_cbm(hw_node, surface_list->dst_surface, surface_list->opt);
    }

    vau_node_set_dst(hw_node, surface_list->dst_surface, is_dma);
    vau_node_set_tile_en(hw_node);
    return;
}

td_s32 drv_vau_blit(td_handle handle, drv_vau_surface_list *surface_list)
{
    td_s32 ret;
    vau_sw_node *sw_node = TD_NULL;

    ret = vau_job_check_surface_list(surface_list);
    if (ret != EXT_SUCCESS) {
        vau_err("call vau_job_check_surface_list fail");
        return EXT_FAILURE;
    }

    vau_job_revise_rect_in_range(surface_list);

    sw_node = vau_node_create();
    if (sw_node == TD_NULL) {
        vau_err("call vau_node_create failed");
        return DRV_ERR_VAU_NO_MEM;
    }

    if (vau_job_check_dma2d_all0mode(surface_list)) {
        return EXT_SUCCESS;
    }

    drv_vau_blit_config_node(&sw_node->hw_node, surface_list);

    ret = vau_job_add_node(handle, sw_node);
    if (ret != EXT_SUCCESS) {
        vau_osi_list_free_node(sw_node);
        vau_err("call vau_job_add_node failed");
        return ret;
    }

    return EXT_SUCCESS;
}

static td_s32 vau_job_check_surface_list(const drv_vau_surface_list *surface_list)
{
    td_s32 ret;

    if (surface_list == TD_NULL) { /* only support 2 layers */
        vau_err("surface list is null");
        return EXT_FAILURE;
    }

    if ((surface_list->src_surface_cnt == 0) ||
        (surface_list->src_surface_cnt > 2)) { /* only support 2 layers */
        vau_err("surface list is null or surface cnt [%u] is not support", surface_list->src_surface_cnt);
        return EXT_FAILURE;
    }

    if ((surface_list->src_surface == TD_NULL) || (surface_list->dst_surface == TD_NULL)) {
        vau_err("null pointer");
        return EXT_FAILURE;
    }

    ret = vau_job_check_every_surface(surface_list);
    if (ret != EXT_SUCCESS) {
        vau_err("vau_check_every_surface failed");
        return ret;
    }

    return EXT_SUCCESS;
}

static td_s32 vau_job_check_every_surface(const drv_vau_surface_list *surface_list)
{
    td_s32 ret;

    ret = vau_hal_check_dst(surface_list->dst_surface);
    if (ret != EXT_SUCCESS) {
        return ret;
    }

    if (surface_list->src_surface_cnt == 1) {
        if (vau_job_check_is_dma_mode(surface_list) == TD_TRUE) {
            ret = vau_hal_check_src1(&surface_list->src_surface[0], TD_TRUE);
        } else {
            ret = vau_hal_check_src2(&surface_list->src_surface[0]);
        }
        if (ret != EXT_SUCCESS) {
            vau_err("vau check single src failed!");
            return ret;
        }

        /* check foreground and dst */
        ret = vau_check_frg_and_dst_surface(&surface_list->src_surface[0], surface_list->dst_surface);
        if (ret != EXT_SUCCESS) {
            vau_err("vau_check_frg_and_dst_surface failed!");
            return ret;
        }
    } else { /* src cnt = 2 */
        ret = vau_hal_check_src1(&surface_list->src_surface[0], TD_FALSE);
        if (ret != EXT_SUCCESS) {
            vau_err("call vau_hal_check_src1 failed!");
            return ret;
        }

        ret = vau_hal_check_src2(&surface_list->src_surface[1]);
        if (ret != EXT_SUCCESS) {
            vau_err("call vau_hal_check_src2 failed!");
            return ret;
        }

        ret = vau_hal_check_blit_opt(surface_list->opt);
        if (ret != EXT_SUCCESS) {
            vau_err("call vau_hal_check_blit_opt failed!");
            return ret;
        }

        /* check foreground and dst */
        ret = vau_check_frg_and_dst_surface(&surface_list->src_surface[1], surface_list->dst_surface);
        if (ret != EXT_SUCCESS) {
            vau_err("vau_check_frg_and_dst_surface failed!");
            return ret;
        }

        /* check background and dst */
        ret = vau_check_bkg_and_dst_surface(&surface_list->src_surface[0], surface_list->dst_surface);
        if (ret != EXT_SUCCESS) {
            vau_err("vau_check_bkg_and_dst_surface failed!");
            return ret;
        }
    }

    return EXT_SUCCESS;
}

static td_bool vau_job_check_is_dma_mode(const drv_vau_surface_list *surface_list)
{
    if ((surface_list->src_surface_cnt == 1) &&
        (surface_list->opt == TD_NULL) &&
        (!surface_list->src_surface[0].attr.affine.affine_en) &&
        (!surface_list->src_surface[0].attr.crop.crop_en) &&
        (!surface_list->src_surface[0].attr.trans.enable) &&
        (!surface_list->src_surface[0].attr.vector.enable) &&
        (surface_list->src_surface[0].fmt == surface_list->dst_surface->fmt) &&
        (surface_list->src_surface[0].rect.width == surface_list->dst_surface->rect.width) &&
        (surface_list->src_surface[0].rect.height == surface_list->dst_surface->rect.height) &&
        (surface_list->src_surface[0].argb_order == surface_list->dst_surface->argb_order) &&
        (surface_list->src_surface[0].compress_info.compress_mode == DRV_GFX_COMPRESS_NONE) &&
        (surface_list->dst_surface->compress_info.compress_mode == DRV_GFX_COMPRESS_NONE)) {
        return TD_TRUE;
    }

    return TD_FALSE;
}

static td_bool vau_job_check_dma2d_all0mode(drv_vau_surface_list *surface_list)
{
    if (vau_dfx_check_mask(VAU_DBG_MASK_DMA2D_DISABLE)) {
        return TD_FALSE;
    }
    /* 2 surfaces blend */
    if ((surface_list->src_surface_cnt == 2) &&
        (surface_list->src_surface[1].compress_info.alpha_value == DRV_GFX_ALPHA_ALL_0)) {
        if ((surface_list->src_surface[0].fmt == surface_list->dst_surface->fmt) &&
            (surface_list->src_surface[0].addr == surface_list->dst_surface->addr)) {
            return TD_TRUE; /* no need logic run */
        } else {
            surface_list->src_surface_cnt = 1; /* gon to check dma */
        }
    }

    return TD_FALSE;
}

static td_bool vau_job_check_dma2d_onlysrc2mode(const drv_vau_surface_list *surface_list)
{
    if (vau_dfx_check_mask(VAU_DBG_MASK_DMA2D_DISABLE)) {
        return TD_FALSE;
    }
    if ((surface_list->src_surface[1].compress_info.alpha_value == DRV_GFX_ALPHA_ALL_255) ||
        (surface_list->src_surface[1].compress_info.alpha_value == DRV_GFX_ALPHA_0_255)) {
        return TD_TRUE;
    }

    return TD_FALSE;
}

static td_void vau_job_dma2d_config(vau_hardware_node *node, drv_vau_surface_list *surface_list)
{
    td_bool strb_en = TD_FALSE;
    if (surface_list->opt == TD_NULL) {
        return;
    }
    if ((surface_list->src_surface[1].compress_info.alpha_value == DRV_GFX_ALPHA_ALL_255) ||
        (surface_list->src_surface[1].compress_info.alpha_value == DRV_GFX_ALPHA_0_255)) {
        surface_list->opt->rop_opt.rop_enable = TD_FALSE;
        surface_list->opt->blend_opt.blend_enable = TD_FALSE;
        strb_en = (surface_list->src_surface[1].compress_info.alpha_value == DRV_GFX_ALPHA_0_255) ? TD_TRUE : TD_FALSE;
        if (!vau_dfx_check_mask(VAU_DBG_MASK_ALPHA_STRB_DISABLE)) {
            vau_hal_set_alpha_strb(node, strb_en);
        }
    } else {
        vau_hal_set_alpha_strb(node, TD_FALSE);
    }

    return;
}

td_s32 drv_vau_import_node(td_handle handle, const vau_hardware_node *node, const td_u32 info[][2], td_u32 size)
{
    td_s32 ret;
    vau_sw_node *sw_node = TD_NULL;

    sw_node = vau_node_create();
    if (sw_node == TD_NULL) {
        vau_err("call vau_node_create failed");
        return DRV_ERR_VAU_NO_MEM;
    }
    if (node != TD_NULL) {
        sw_node->hw_node = *node;
    } else {
        td_u32 hw_node_size = (td_u32)sizeof(vau_hardware_node);
        td_u32 *base = (td_u32 *)(uintptr_t)&sw_node->hw_node;

        td_bool is_draw = (sw_node->hw_node.vau_draw_ctrl.bits.draw_en == 1) ? TD_TRUE : TD_FALSE;
        for (td_u32 i = 0; i < size; i++) {
            /* part1 common regs 0-0x100 */
            if (info[i][0] < 0x100) {
                base[info[i][0] / 4] = info[i][1]; /* 1 reg 4 bytes */
            /* part2 common regs 0x100-0x200 */
            } else if ((info[i][0] < 0x200) && (info[i][0] < hw_node_size) && (is_draw == TD_TRUE)) {
                base[info[i][0] / 4] = info[i][1]; /* 1 reg 4 bytes */
            /* part3 common regs 0x200-0x300 */
            } else if (((info[i][0] - 0x100) < hw_node_size) && (is_draw == TD_FALSE)) {
                base[(info[i][0] - 0x100) / 4] = info[i][1]; /* 0x100: offset for mix regs, 1 reg 4 bytes */
            }
        }
    }

    ret = vau_job_add_node(handle, sw_node);
    if (ret != EXT_SUCCESS) {
        vau_osi_list_free_node(sw_node);
        vau_err("call vau_job_add_node failed");
        return ret;
    }

    return EXT_SUCCESS;
}

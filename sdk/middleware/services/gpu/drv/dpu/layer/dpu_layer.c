/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: dpu layer
 */

#include "dpu_layer.h"
#include "dpu_disp.h"
#include "dpu_isr.h"
#include "dpu_hal.h"
#include "dpu_dfx.h"
#include "pm.h"
#include "mipi_tx.h"

#define DRV_DPU_MAX_LAYER_WIDTH  640
#define DRV_DPU_MAX_LAYER_HEIGHT 640

#define DRV_DPU_MIN_LAYER_WIDTH  32
#define DRV_DPU_MIN_LAYER_HEIGHT 32

typedef struct {
    td_bool is_create[DRV_DPU_MAX_LAYER_ID + 1];
    drv_dpu_layer_info layer_info[DRV_DPU_MAX_LAYER_ID + 1];
} dpu_context;

static dpu_context g_dpu_context = { 0 };

static dpu_layer_dev *g_dpu_layer_dev[DRV_DPU_MAX_LAYER_ID + 1] = { 0 };

#define get_dpu_layer_device(layer_id, layer_dev) do {  \
    layer_dev = g_dpu_layer_dev[layer_id];              \
    if ((layer_dev) == TD_NULL) {                       \
        dpu_err("layer%d is not inited!", layer_id); \
        return DRV_ERR_DPU_NULL_POINTER;                \
    }                                                   \
} while (0)

dpu_layer_dev *dpu_layer_get_dev(drv_dpu_layer_id layer_id)
{
    return g_dpu_layer_dev[layer_id];
}

static td_s32 dpu_layer_check_layer_info(const drv_dpu_layer_info *layer_info)
{
    if ((layer_info->width < DRV_DPU_MIN_LAYER_WIDTH) ||
        (layer_info->width > DRV_DPU_MAX_LAYER_WIDTH)) {
        dpu_err("layer width not support:%u", layer_info->width);
        return DRV_ERR_DPU_LAYER_NOT_SUPPORT;
    }
    if ((layer_info->height < DRV_DPU_MIN_LAYER_HEIGHT) ||
        (layer_info->height > DRV_DPU_MAX_LAYER_HEIGHT)) {
        dpu_err("layer height not support:%u", layer_info->height);
        return DRV_ERR_DPU_LAYER_NOT_SUPPORT;
    }
    if ((layer_info->layer_fmt != DRV_GFX_FMT_ARGB8888) &&
        (layer_info->layer_fmt != DRV_GFX_FMT_ARGB4444) &&
        (layer_info->layer_fmt != DRV_GFX_FMT_ARGB1555) &&
        (layer_info->layer_fmt != DRV_GFX_FMT_RGB888) &&
        (layer_info->layer_fmt != DRV_GFX_FMT_RGB565) &&
        (layer_info->layer_fmt != DRV_GFX_FMT_YUV420SP)) {
        dpu_err("layer fmt not support:%u", (td_u32)layer_info->layer_fmt);
        return DRV_ERR_DPU_LAYER_NOT_SUPPORT;
    }
    if ((layer_info->layer_id < DRV_DPU_LAYER_ID1) ||
        (layer_info->layer_id > DRV_DPU_MAX_LAYER_ID)) {
        dpu_err("layer not support:%u", (td_u32)layer_info->layer_id);
        return DRV_ERR_DPU_LAYER_NOT_SUPPORT;
    }
    if (layer_info->buf_mode != DRV_DPU_BUF_MODE_NORMAL) {
        dpu_err("flush mode not support:%u", (td_u32)layer_info->buf_mode);
        return DRV_ERR_DPU_LAYER_NOT_SUPPORT;
    }
    if (layer_info->compress_mode >= DRV_GFX_COMPRESS_MAX) {
        dpu_err("compress mode not support:%u", (td_u32)layer_info->compress_mode);
        return DRV_ERR_DPU_LAYER_NOT_SUPPORT;
    }

    if ((layer_info->layer_fmt == DRV_GFX_FMT_YUV420SP) &&
        (layer_info->layer_id != DRV_DPU_LAYER_ID2)) {
        dpu_err("only layer_id[%u] support yuv fmt[%u]!", (td_u32)layer_info->layer_id,
                (td_u32)layer_info->layer_fmt);
        return DRV_ERR_DPU_LAYER_NOT_SUPPORT;
    }

    return EXT_SUCCESS;
}

static td_void dpu_layer_set_run_info(dpu_layer_run_info *run_info, const drv_dpu_layer_info *layer_info)
{
    run_info->show          = TD_TRUE;
    run_info->alpha0        = 0x0;
    run_info->alpha1        = 0xFF;
    run_info->global_alpha  = 0xFF;
    run_info->width         = layer_info->width;
    run_info->height        = layer_info->height;
    run_info->fmt           = layer_info->layer_fmt;
    run_info->premulti_en   = 0;
    run_info->compress_mode = layer_info->compress_mode;

    run_info->pos_x = 0;
    run_info->pos_y = 0;

    run_info->disp_rect.x = 0;
    run_info->disp_rect.y = 0;
    run_info->disp_rect.width  = layer_info->width;
    run_info->disp_rect.height = layer_info->height;

    run_info->crop_rect.x = 0;
    run_info->crop_rect.y = 0;
    run_info->crop_rect.width  = layer_info->width;  /* crop_w means ireso_w */
    run_info->crop_rect.height = layer_info->height; /* crop_h means ireso_h */

    run_info->update_region.x = 0;
    run_info->update_region.y = 0;
    run_info->update_region.width  = layer_info->width;
    run_info->update_region.height = layer_info->height;

    return;
}

static td_void dpu_layer_init_run_info_change(dpu_layer_run_info_change *run_info_change)
{
    run_info_change->show          = TD_TRUE;
    run_info_change->alpha0_alpha1 = TD_TRUE;
    run_info_change->global_alpha  = TD_TRUE;
    run_info_change->width         = TD_TRUE;
    run_info_change->height        = TD_TRUE;
    run_info_change->stride        = TD_TRUE;
    run_info_change->addr          = TD_TRUE;
    run_info_change->disp_rect     = TD_TRUE;
    run_info_change->crop_rect     = TD_TRUE;
    run_info_change->fmt           = TD_TRUE;
    run_info_change->colorkey      = TD_TRUE;
    run_info_change->compress_mode = TD_TRUE;

    return;
}

static dpu_hal_fmt dpu_layer_trans_fmt_drv_to_hal(drv_gfx_fmt fmt)
{
    dpu_hal_fmt hal_fmt;

    switch (fmt) {
        case DRV_GFX_FMT_ARGB8888:
            hal_fmt = DPU_HAL_FMT_ARGB8888;
            break;
        case DRV_GFX_FMT_ARGB1555:
            hal_fmt = DPU_HAL_FMT_ARGB1555;
            break;
        case DRV_GFX_FMT_ARGB4444:
            hal_fmt = DPU_HAL_FMT_ARGB4444;
            break;
        case DRV_GFX_FMT_RGB888:
            hal_fmt = DPU_HAL_FMT_RGB888;
            break;
        case DRV_GFX_FMT_RGB565:
            hal_fmt = DPU_HAL_FMT_RGB565;
            break;
        case DRV_GFX_FMT_YUV420SP:
            hal_fmt = DPU_HAL_FMT_YUV420SP;
            break;
        default:
            hal_fmt = DPU_HAL_FMT_RGB888;
            break;
    }

    return hal_fmt;
}

static td_void dpu_layer_yuv_base_config(dpu_layer_dev *layer_dev)
{
    dpu_hal_yuv_info yuv_info = {0};

    if (layer_dev->run_info.fmt != DRV_GFX_FMT_YUV420SP) {
        return;
    }

    yuv_info.y_addr     = layer_dev->run_info.current_addr;
    yuv_info.out_height = layer_dev->run_info.height;
    yuv_info.uv_stride  = layer_dev->canvas_surface.uv_stride;
    yuv_info.uv_offset  = layer_dev->canvas_surface.uv_offset;

    /* soft support yuv crop */
    if (layer_dev->run_info.crop_rect.y != 0) {
        yuv_info.y_addr    += (td_u32)layer_dev->run_info.crop_rect.y * yuv_info.uv_stride;
        yuv_info.uv_offset -= (td_u32)layer_dev->run_info.crop_rect.y * yuv_info.uv_stride / 2; /* 2:index */

        layer_dev->run_info.crop_rect.y = 0;
        dpu_hal_layer_set_crop_rect(layer_dev->layer_id, &layer_dev->run_info.crop_rect);
    }

    dpu_hal_layer_set_addr(layer_dev->layer_id, yuv_info.y_addr);
    dpu_hal_layer_set_yuv_base_info(layer_dev->layer_id, &yuv_info);

    return;
}

static void dpu_layer_disp_rect_update(dpu_layer_dev *layer_dev)
{
    dpu_layer_run_info *run_info = &layer_dev->run_info;

    if (run_info->disp_rect.x + (td_s32)run_info->disp_rect.width > (td_s32)layer_dev->screen_width) {
        run_info->disp_rect.width = (td_u32)((td_s32)layer_dev->screen_width - run_info->disp_rect.x);
        run_info->crop_rect.width = run_info->disp_rect.width;
    }

    if (run_info->disp_rect.y + (td_s32)run_info->disp_rect.height > (td_s32)layer_dev->screen_height) {
        run_info->disp_rect.height = (td_u32)((td_s32)layer_dev->screen_height - run_info->disp_rect.y);
        run_info->crop_rect.height = run_info->disp_rect.height;
    }

    dpu_hal_layer_set_disp_rect(layer_dev->layer_id, &run_info->disp_rect);
}

static td_void dpu_layer_config_update(dpu_layer_dev *layer_dev)
{
    td_bool dcmp_en, alpha_bypass;

    if (layer_dev->run_info_change.global_alpha) {
        dpu_hal_layer_set_global_alpha(layer_dev->layer_id, layer_dev->run_info.global_alpha);
        layer_dev->run_info_change.global_alpha = TD_FALSE;
    }
    if (layer_dev->run_info_change.disp_rect) {
        dpu_layer_disp_rect_update(layer_dev);
        layer_dev->run_info_change.disp_rect = TD_FALSE;
    }
    if (layer_dev->run_info_change.crop_rect) {
        dpu_hal_layer_set_crop_rect(layer_dev->layer_id, &layer_dev->run_info.crop_rect);
        layer_dev->run_info_change.crop_rect = TD_FALSE;
    }
    if (layer_dev->run_info_change.alpha0_alpha1) {
        dpu_hal_layer_set_alpha0_alpha1(layer_dev->layer_id, layer_dev->run_info.alpha0, layer_dev->run_info.alpha1);
        layer_dev->run_info_change.alpha0_alpha1 = TD_FALSE;
    }
    if (layer_dev->run_info_change.fmt) {
        dpu_hal_fmt hal_fmt = dpu_layer_trans_fmt_drv_to_hal(layer_dev->run_info.fmt);
        dpu_hal_layer_set_fmt(layer_dev->layer_id, hal_fmt);
        layer_dev->run_info_change.fmt = TD_FALSE;
    }
    if (layer_dev->run_info_change.width) {
        dpu_hal_layer_set_width(layer_dev->layer_id, layer_dev->run_info.width);
        layer_dev->run_info_change.width = TD_FALSE;
    }
    if (layer_dev->run_info_change.height) {
        dpu_hal_layer_set_height(layer_dev->layer_id, layer_dev->run_info.height);
        layer_dev->run_info_change.height = TD_FALSE;
    }
    if (layer_dev->run_info_change.stride) {
        dpu_hal_layer_set_stride(layer_dev->layer_id, layer_dev->run_info.stride);
        layer_dev->run_info_change.stride = TD_FALSE;
    }
    if (layer_dev->run_info_change.addr) {
        dpu_hal_layer_set_addr(layer_dev->layer_id, layer_dev->run_info.current_addr);
        layer_dev->run_info_change.addr = TD_FALSE;
    }
    if (layer_dev->run_info_change.compress_mode) {
        dcmp_en = (layer_dev->run_info.compress_mode != DRV_GFX_COMPRESS_NONE) ? TD_TRUE : TD_FALSE;
        alpha_bypass = (layer_dev->run_info.compress_mode == DRV_GFX_COMPRESS_HFBC_ABYPASS) ? TD_TRUE : TD_FALSE;
        if (layer_dev->layer_id == DRV_DPU_LAYER_ID2) {
        /* if G1 input cmp format, need bind share mem to G1 */
            dpu_hal_disp_set_mem_link(dcmp_en);
        }
        dpu_hal_layer_set_dcmp(layer_dev->layer_id, dcmp_en);
        dpu_hal_layer_set_alpha_bypass(layer_dev->layer_id, alpha_bypass);
        layer_dev->run_info_change.compress_mode = TD_FALSE;
    }
    if (layer_dev->run_info_change.show) {
        dpu_hal_layer_set_enable(layer_dev->layer_id, layer_dev->run_info.show);
        layer_dev->run_info_change.show = TD_FALSE;
    }
    if (layer_dev->run_info_change.colorkey) {
        dpu_hal_layer_set_colorkey(layer_dev->layer_id, layer_dev->run_info.colorkey.key_en,
                                   layer_dev->run_info.colorkey.key_value);
        layer_dev->run_info_change.colorkey = TD_FALSE;
    }
    if (layer_dev->run_info_change.premulti_en) {
        dpu_hal_layer_set_premulti_en(layer_dev->layer_id, layer_dev->canvas_surface.premulti_en);
        layer_dev->run_info_change.premulti_en = TD_FALSE;
    }

    dpu_layer_yuv_base_config(layer_dev);
    dpu_hal_layer_set_reg_up(layer_dev->layer_id);
}

static td_void dpu_layer_calculate_gp_region(ext_rect *gp_region)
{
    td_u32 screen_width  = dpu_disp_get_srceen_width();
    td_u32 screen_height = dpu_disp_get_srceen_height();
#ifdef GPU_SUPPORT_VIDEO_MODE
    gp_region->x = 0;
    gp_region->y = 0;
    gp_region->width  = (td_u32)screen_width;
    gp_region->height = (td_u32)screen_height;

    return;
#endif
    td_s32 end_x = 0;
    td_s32 end_y = 0;
    td_s32 start_x = (td_s32)screen_width;
    td_s32 start_y = (td_s32)screen_height;

    for (td_u32 layer_id = 0; layer_id <= (td_u32)DRV_DPU_MAX_LAYER_ID; layer_id++) {
        if ((g_dpu_layer_dev[layer_id] == TD_NULL) || (g_dpu_layer_dev[layer_id]->is_create != TD_TRUE)) {
            continue;
        }

        dpu_layer_run_info *run_info = &g_dpu_layer_dev[layer_id]->run_info;
        /* if exists full screen layer or g1, gp_reion is full */
        if (((run_info->update_region.width == screen_width) && (run_info->update_region.height == screen_height)) ||
            (layer_id == DRV_DPU_LAYER_ID2)) {
            gp_region->x = 0;
            gp_region->y = 0;
            gp_region->width  = screen_width;
            gp_region->height = screen_height;
            return;
        }

        if (start_x > run_info->pos_x + run_info->update_region.x) {
            start_x = run_info->pos_x + run_info->update_region.x;
            start_x = (start_x < 0) ? 0 : start_x;
        }

        if (start_y > run_info->pos_y + run_info->update_region.y) {
            start_y = run_info->pos_y + run_info->update_region.y;
            start_y = (start_y < 0) ? 0 : start_y;
        }

        if (end_x < run_info->pos_x + run_info->update_region.x + (td_s32)run_info->update_region.width) {
            end_x = run_info->pos_x + run_info->update_region.x + (td_s32)run_info->update_region.width;
            end_x = (end_x > (td_s32)screen_width) ? (td_s32)screen_width : end_x;
        }

        if (end_y < run_info->pos_y + run_info->update_region.y + (td_s32)run_info->update_region.height) {
            end_y = run_info->pos_y + run_info->update_region.y + (td_s32)run_info->update_region.height;
            end_y = (end_y > (td_s32)screen_height) ? (td_s32)screen_height : end_y;
        }

        if ((end_x < start_x) || (end_y < start_y)) {
            dpu_err("invalid param, layer[%u] start[%d,%d] end[%d,%d] pos[%d,%d] region[%d,%d,%u,%u]",
                    layer_id, start_x, start_y, end_x, end_y, run_info->pos_x, run_info->pos_y,
                    run_info->update_region.x, run_info->update_region.y,
                    run_info->update_region.width, run_info->update_region.height);
        }
    }

    gp_region->x = start_x;
    gp_region->y = start_y;
    gp_region->width  = (td_u32)(end_x - start_x);
    gp_region->height = (td_u32)(end_y - start_y);
}

static td_void dpu_layer_revise_update_region(dpu_layer_dev *layer_dev, const ext_rect *gp_region)
{
    ext_rect common_area;
    dpu_layer_run_info *run_info = &layer_dev->run_info;

    /* calculate the layer region and gp_region comon area */
    common_area.x = (run_info->pos_x > gp_region->x) ? run_info->pos_x : gp_region->x;
    common_area.y = (run_info->pos_y > gp_region->y) ? run_info->pos_y : gp_region->y;
    common_area.width  = (td_u32)(((run_info->pos_x + (td_s32)run_info->width) <
                         (gp_region->x + (td_s32)gp_region->width)) ?
                         (run_info->pos_x + (td_s32)run_info->width - common_area.x) :
                         (gp_region->x + (td_s32)gp_region->width - common_area.x));
    common_area.height = (td_u32)(((run_info->pos_y + (td_s32)run_info->height) <
                         (gp_region->y + (td_s32)gp_region->height)) ?
                         (run_info->pos_y + (td_s32)run_info->height - common_area.y) :
                         (gp_region->y + (td_s32)gp_region->height - common_area.y));

    /* calculate the layer display position in the gp region */
    run_info->disp_rect.x      = common_area.x - gp_region->x;
    run_info->disp_rect.y      = common_area.y - gp_region->y;
    run_info->disp_rect.width  = common_area.width;
    run_info->disp_rect.height = common_area.height;

    run_info->crop_rect.x      = common_area.x - run_info->pos_x;
    run_info->crop_rect.y      = common_area.y - run_info->pos_y;
    run_info->crop_rect.width  = common_area.width;
    run_info->crop_rect.height = common_area.height;

    /* revise the update region */
    run_info->update_region = run_info->crop_rect;

    layer_dev->run_info_change.disp_rect = TD_TRUE;
    layer_dev->run_info_change.crop_rect = TD_TRUE;

    return;
}

static td_s32 dpu_check_update_rect(dpu_layer_run_info *run_info, const drv_dpu_surface *surface)
{
#ifdef GPU_SUPPORT_VIDEO_MODE
    run_info->update_region.x = 0;
    run_info->update_region.y = 0;
    run_info->update_region.width  = surface->width;
    run_info->update_region.height = surface->height;
#else
    if (surface->update_rect == TD_NULL) {
        run_info->update_region.x = 0;
        run_info->update_region.y = 0;
        run_info->update_region.width  = surface->width;
        run_info->update_region.height = surface->height;
        return EXT_SUCCESS;
    }

    /* check surface update rect */
    if ((surface->update_rect->x < 0) || (surface->update_rect->y < 0) ||
        (surface->update_rect->width == 0) || (surface->update_rect->height == 0) ||
        ((td_u32)surface->update_rect->x + surface->update_rect->width > surface->width) ||
        ((td_u32)surface->update_rect->y + surface->update_rect->height > surface->height)) {
        dpu_err("invalid param, update_rect[%d,%d,%u,%u], surface_wh[%u,%u]!",
                surface->update_rect->x, surface->update_rect->y,
                surface->update_rect->width, surface->update_rect->height,
                surface->width, surface->height);
        return DRV_ERR_DPU_OPERATION_NOT_SUPPORT;
    }

    /* revise update region */
    run_info->update_region = *surface->update_rect;
    if (run_info->compress_mode != DRV_GFX_COMPRESS_NONE) {
        if (run_info->update_region.x != 0) {
            dpu_dbg("cmp mode not support horizotal crop, x revise to zero!");
            run_info->update_region.x = 0;
        }

        if (run_info->update_region.width != run_info->width) {
            dpu_dbg("cmp mode not support horizotal crop, w revise to surface_w!");
            run_info->update_region.width = run_info->width;
        }
    }
#endif
    return EXT_SUCCESS;
}

static td_void dpu_layer_update_layer_info(dpu_layer_dev *layer_dev, const drv_dpu_surface *surface)
{
    layer_dev->update_flag = TD_TRUE;
    layer_dev->canvas_surface = *surface;

    if (surface->width != layer_dev->run_info.width) {
        layer_dev->run_info.width = surface->width;
        layer_dev->run_info_change.width = TD_TRUE;
    }

    if (surface->height != layer_dev->run_info.height) {
        layer_dev->run_info.height = surface->height;
        layer_dev->run_info_change.height = TD_TRUE;
    }

    if (surface->stride != layer_dev->run_info.stride) {
        layer_dev->run_info.stride = surface->stride;
        layer_dev->run_info_change.stride = TD_TRUE;
    }

    if (surface->phy_addr != layer_dev->run_info.current_addr) {
        layer_dev->run_info.current_addr  = surface->phy_addr;
        layer_dev->run_info_change.addr = TD_TRUE;
    }

    if (surface->color_fmt != layer_dev->run_info.fmt) {
        layer_dev->run_info.fmt = surface->color_fmt;
        layer_dev->run_info_change.fmt = TD_TRUE;
    }

    if (surface->premulti_en != layer_dev->run_info.premulti_en) {
        layer_dev->run_info.premulti_en = surface->premulti_en;
        layer_dev->run_info_change.premulti_en = TD_TRUE;
    }

    if (surface->compress_mode != layer_dev->run_info.compress_mode) {
        layer_dev->run_info.compress_mode = surface->compress_mode;
        layer_dev->run_info_change.compress_mode = TD_TRUE;
    }

    return;
}

static td_void dpu_layer_hal_init(td_u32 layer_id)
{
    dpu_hal_layer_set_ck_gt_en(layer_id, TD_TRUE);
    dpu_hal_layer_set_palpha_en(layer_id, TD_TRUE);
    dpu_hal_layer_set_fdr_chksum_en(layer_id, TD_TRUE);
}

static td_void dpu_layer_hal_deinit(drv_dpu_layer_id layer_id)
{
    dpu_hal_layer_set_ck_gt_en(layer_id, TD_FALSE);
}

td_s32 dpu_layer_init(void)
{
    td_s32 ret;
    td_u32 layer_id;

    for (layer_id = (td_u32)DRV_DPU_LAYER_ID1; layer_id <= (td_u32)DRV_DPU_MAX_LAYER_ID; layer_id++) {
        g_dpu_layer_dev[layer_id] = (dpu_layer_dev *)osal_vmalloc(sizeof(dpu_layer_dev));
        if (g_dpu_layer_dev[layer_id] == TD_NULL) {
            dpu_layer_deinit();
            dpu_err("osal_vmalloc %u failed", (td_u32)sizeof(dpu_layer_dev));
            return DRV_ERR_DPU_MALLOC_FAILED;
        }
        (void)memset_s(g_dpu_layer_dev[layer_id], sizeof(dpu_layer_dev), 0, sizeof(dpu_layer_dev));
        ret = osal_spin_lock_init(&g_dpu_layer_dev[layer_id]->layer_lock);
        if (ret != EXT_SUCCESS) {
            dpu_layer_deinit();
            dpu_err("osal_spin_lock_init failed");
            return DRV_ERR_DPU_OSAL_FAILED;
        }
        dpu_layer_hal_init(layer_id);
    }

    return EXT_SUCCESS;
}

td_void dpu_layer_deinit(void)
{
    td_u32 layer_id;

    for (layer_id = (td_u32)DRV_DPU_LAYER_ID1; layer_id <= (td_u32)DRV_DPU_MAX_LAYER_ID; layer_id++) {
        if (g_dpu_layer_dev[layer_id] != TD_NULL) {
            dpu_layer_hal_deinit((drv_dpu_layer_id)layer_id);

            if (g_dpu_layer_dev[layer_id]->layer_lock.lock != TD_NULL) {
                osal_spin_lock_destroy(&g_dpu_layer_dev[layer_id]->layer_lock);
            }

            osal_vfree((td_void *)g_dpu_layer_dev[layer_id]);
            g_dpu_layer_dev[layer_id] = TD_NULL;
        }
    }

    return;
}

td_void dpu_layer_suspend(td_void)
{
    td_u32 layer_id;

    for (layer_id = 0; layer_id <= (td_u32)DRV_DPU_MAX_LAYER_ID; layer_id++) {
        g_dpu_context.is_create[layer_id] = g_dpu_layer_dev[layer_id]->is_create;
        if (g_dpu_context.is_create[layer_id]) {
            dpu_layer_destroy(layer_id);
        }
    }
}

td_void dpu_layer_resume(td_void)
{
    td_u32 layer_id;

    for (layer_id = 0; layer_id <= (td_u32)DRV_DPU_MAX_LAYER_ID; layer_id++) {
        if (g_dpu_context.is_create[layer_id]) {
            dpu_layer_create(&g_dpu_context.layer_info[layer_id]);
            dpu_layer_hal_init(layer_id);
        }
    }
}

td_s32 dpu_layer_create(const drv_dpu_layer_info *layer_info)
{
    td_s32 ret;
    unsigned long flag = 0;
    dpu_layer_dev *layer_dev = TD_NULL;

    if (layer_info == TD_NULL) {
        dpu_err("layer_info is null");
        return DRV_ERR_DPU_NULL_POINTER;
    }

    get_dpu_layer_device(layer_info->layer_id, layer_dev);

    ret = dpu_layer_check_layer_info(layer_info);
    if (ret != EXT_SUCCESS) {
        return ret;
    }

    osal_spin_lock_irqsave(&layer_dev->layer_lock, &flag);
    if (layer_dev->create_cnt > 0) {
        layer_dev->create_cnt++;
        osal_spin_unlock_irqrestore(&layer_dev->layer_lock, &flag);
        return EXT_SUCCESS;
    }

    layer_dev->layer_id = layer_info->layer_id;
    dpu_layer_set_run_info(&layer_dev->run_info, layer_info);
    dpu_layer_init_run_info_change(&layer_dev->run_info_change);

    layer_dev->create_cnt++;
    layer_dev->is_create = TD_TRUE;
    layer_dev->update_flag = TD_TRUE;

    osal_spin_unlock_irqrestore(&layer_dev->layer_lock, &flag);

    dpu_disp_open();

    layer_dev->screen_width  = dpu_disp_get_srceen_width();
    layer_dev->screen_height = dpu_disp_get_srceen_height();

    g_dpu_context.layer_info[layer_info->layer_id] = *layer_info;

    return EXT_SUCCESS;
}

td_s32 dpu_layer_destroy(drv_dpu_layer_id layer_id)
{
    unsigned long flag = 0;
    dpu_layer_dev *layer_dev = TD_NULL;

    get_dpu_layer_device(layer_id, layer_dev);

    osal_spin_lock_irqsave(&layer_dev->layer_lock, &flag);
    /* the layer has not been created */
    if (layer_dev->is_create != TD_TRUE) {
        osal_spin_unlock_irqrestore(&layer_dev->layer_lock, &flag);
        return EXT_SUCCESS;
    }

    /* the layer has been created by two or more objs */
    if (layer_dev->create_cnt > 1) {
        layer_dev->create_cnt--;
        osal_spin_unlock_irqrestore(&layer_dev->layer_lock, &flag);
        return EXT_SUCCESS;
    }

    layer_dev->create_cnt--;
    layer_dev->is_create = TD_FALSE;
    osal_spin_unlock_irqrestore(&layer_dev->layer_lock, &flag);

    dpu_disp_close();

    return EXT_SUCCESS;
}

td_s32 dpu_layer_set_alpha(drv_dpu_layer_id layer_id, td_u8 alpha)
{
    unsigned long flag = 0;
    dpu_layer_dev *layer_dev = TD_NULL;

    get_dpu_layer_device(layer_id, layer_dev);

    osal_spin_lock_irqsave(&layer_dev->layer_lock, &flag);
    layer_dev->update_flag = TD_TRUE;
    layer_dev->run_info.global_alpha = alpha;
    layer_dev->run_info_change.global_alpha = TD_TRUE;
    osal_spin_unlock_irqrestore(&layer_dev->layer_lock, &flag);

    return EXT_SUCCESS;
}

td_s32 dpu_layer_get_alpha(drv_dpu_layer_id layer_id, td_u8 *alpha)
{
    unsigned long flag = 0;
    dpu_layer_dev *layer_dev = TD_NULL;

    get_dpu_layer_device(layer_id, layer_dev);

    osal_spin_lock_irqsave(&layer_dev->layer_lock, &flag);
    *alpha = layer_dev->run_info.global_alpha;
    osal_spin_unlock_irqrestore(&layer_dev->layer_lock, &flag);

    return EXT_SUCCESS;
}

td_s32 dpu_layer_set_pos(drv_dpu_layer_id layer_id, td_s32 pos_x, td_s32 pos_y)
{
    unsigned long flag = 0;
    dpu_layer_dev *layer_dev = TD_NULL;

    get_dpu_layer_device(layer_id, layer_dev);

    if ((pos_x > (td_s32)layer_dev->screen_width) || (pos_y > (td_s32)layer_dev->screen_height) ||
        (pos_x % 2 != 0) || (pos_y % 2 != 0)) { /* 2: yuv data pos_xy is even */
        dpu_err("Invalid param, xy[%d,%d], screen_wh[%u,%u]\n",
                pos_x, pos_y, layer_dev->screen_width, layer_dev->screen_height);
        return DRV_ERR_DPU_OPERATION_NOT_SUPPORT;
    }

    if ((layer_dev->run_info.pos_x == pos_x) && (layer_dev->run_info.pos_y == pos_y)) {
        return EXT_SUCCESS;
    }

    osal_spin_lock_irqsave(&layer_dev->layer_lock, &flag);
    layer_dev->update_flag = TD_TRUE;
    layer_dev->run_info.pos_x = pos_x;
    layer_dev->run_info.pos_y = pos_y;

    /* position setting need change update region to full layer */
    layer_dev->run_info.update_region.x = 0;
    layer_dev->run_info.update_region.y = 0;
    layer_dev->run_info.update_region.width  = layer_dev->run_info.width;
    layer_dev->run_info.update_region.height = layer_dev->run_info.height;
    osal_spin_unlock_irqrestore(&layer_dev->layer_lock, &flag);

    return EXT_SUCCESS;
}

td_s32 dpu_layer_get_pos(drv_dpu_layer_id layer_id, td_s32 *pos_x, td_s32 *pos_y)
{
    unsigned long flag = 0;
    dpu_layer_dev *layer_dev = TD_NULL;

    get_dpu_layer_device(layer_id, layer_dev);

    osal_spin_lock_irqsave(&layer_dev->layer_lock, &flag);
    *pos_x = layer_dev->run_info.pos_x;
    *pos_y = layer_dev->run_info.pos_y;
    osal_spin_unlock_irqrestore(&layer_dev->layer_lock, &flag);

    return EXT_SUCCESS;
}

td_s32 dpu_layer_set_zorder(drv_dpu_layer_id layer_id, drv_dpu_zorder_mode zorder)
{
    unsigned long flag = 0;
    dpu_layer_dev *layer_dev = TD_NULL;

    get_dpu_layer_device(layer_id, layer_dev);

    osal_spin_lock_irqsave(&layer_dev->layer_lock, &flag);
    layer_dev->update_flag = TD_TRUE;
    dpu_disp_set_cbm_priority(layer_id, zorder);

    /* position setting need change update region to full layer */
    layer_dev->run_info.update_region.x = 0;
    layer_dev->run_info.update_region.y = 0;
    layer_dev->run_info.update_region.width  = layer_dev->run_info.width;
    layer_dev->run_info.update_region.height = layer_dev->run_info.height;
    osal_spin_unlock_irqrestore(&layer_dev->layer_lock, &flag);

    return EXT_SUCCESS;
}

td_s32 dpu_layer_get_zorder(drv_dpu_layer_id layer_id, td_u32 *zorder)
{
    unsigned long flag = 0;
    dpu_layer_dev *layer_dev = TD_NULL;

    get_dpu_layer_device(layer_id, layer_dev);

    osal_spin_lock_irqsave(&layer_dev->layer_lock, &flag);
    *zorder = dpu_disp_get_layer_zorder();
    osal_spin_unlock_irqrestore(&layer_dev->layer_lock, &flag);

    return EXT_SUCCESS;
}

td_s32 dpu_layer_show(drv_dpu_layer_id layer_id)
{
    unsigned long flag = 0;
    dpu_layer_dev *layer_dev = TD_NULL;

    get_dpu_layer_device(layer_id, layer_dev);

    osal_spin_lock_irqsave(&layer_dev->layer_lock, &flag);
    layer_dev->update_flag = TD_TRUE;
    layer_dev->run_info.show = TD_TRUE;
    layer_dev->run_info_change.show = TD_TRUE;
    osal_spin_unlock_irqrestore(&layer_dev->layer_lock, &flag);

    return EXT_SUCCESS;
}

td_s32 dpu_layer_hide(drv_dpu_layer_id layer_id)
{
    unsigned long flag = 0;
    dpu_layer_dev *layer_dev = TD_NULL;

    get_dpu_layer_device(layer_id, layer_dev);

    osal_spin_lock_irqsave(&layer_dev->layer_lock, &flag);
    layer_dev->update_flag = TD_TRUE;
    layer_dev->run_info.show = TD_FALSE;
    layer_dev->run_info_change.show = TD_TRUE;
    osal_spin_unlock_irqrestore(&layer_dev->layer_lock, &flag);

    return EXT_SUCCESS;
}

td_s32 dpu_layer_set_info(drv_dpu_layer_id layer_id, const drv_dpu_surface *surface)
{
    td_s32 ret;
    unsigned long flag = 0;
    dpu_layer_dev *layer_dev = TD_NULL;

    if (surface == TD_NULL) {
        dpu_err("null pointer!");
        return DRV_ERR_DPU_NULL_POINTER;
    }

    get_dpu_layer_device(layer_id, layer_dev);

    osal_spin_lock_irqsave(&layer_dev->layer_lock, &flag);
    if (layer_dev->is_create != TD_TRUE) {
        dpu_err("layer%d is not created!\n", layer_id);
        osal_spin_unlock_irqrestore(&layer_dev->layer_lock, &flag);
        return DRV_ERR_DPU_LAYER_NOT_CREATED;
    }

    ret = dpu_check_update_rect(&layer_dev->run_info, surface);
    if (ret != EXT_SUCCESS) {
        dpu_err("update_rect is invalid, ret=%#x!\n", ret);
        osal_spin_unlock_irqrestore(&layer_dev->layer_lock, &flag);
        return ret;
    }

    dpu_layer_update_layer_info(layer_dev, surface);
    osal_spin_unlock_irqrestore(&layer_dev->layer_lock, &flag);

    dpu_dfx_info *dfx_info = dpu_dfx_get_info();
    dfx_info->layer_info[layer_id].phy_addr = surface->phy_addr;
    dfx_info->layer_info[layer_id].width = surface->width;
    dfx_info->layer_info[layer_id].height = surface->height;
    dfx_info->layer_info[layer_id].stride = surface->stride;

    return EXT_SUCCESS;
}

td_s32 dpu_layer_get_info(drv_dpu_layer_id layer_id, drv_dpu_surface *surface)
{
    unsigned long flag = 0;
    dpu_layer_dev *layer_dev = TD_NULL;

    if (surface == TD_NULL) {
        dpu_err("null pointer!");
        return DRV_ERR_DPU_NULL_POINTER;
    }

    get_dpu_layer_device(layer_id, layer_dev);

    osal_spin_lock_irqsave(&layer_dev->layer_lock, &flag);
    *surface = layer_dev->canvas_surface;
    osal_spin_unlock_irqrestore(&layer_dev->layer_lock, &flag);

    return EXT_SUCCESS;
}

td_s32 dpu_layer_set_colorkey(drv_dpu_layer_id layer_id, const drv_dpu_colorkey *colorkey)
{
    unsigned long flag = 0;
    dpu_layer_dev *layer_dev = TD_NULL;

    if (colorkey == TD_NULL) {
        dpu_err("null pointer!");
        return DRV_ERR_DPU_NULL_POINTER;
    }

    get_dpu_layer_device(layer_id, layer_dev);

    osal_spin_lock_irqsave(&layer_dev->layer_lock, &flag);
    layer_dev->update_flag = TD_TRUE;
    layer_dev->run_info.colorkey = *colorkey;
    layer_dev->run_info_change.colorkey = TD_TRUE;
    osal_spin_unlock_irqrestore(&layer_dev->layer_lock, &flag);

    return EXT_SUCCESS;
}

td_s32 dpu_layer_get_colorkey(drv_dpu_layer_id layer_id, drv_dpu_colorkey *colorkey)
{
    unsigned long flag = 0;
    dpu_layer_dev *layer_dev = TD_NULL;

    if (colorkey == TD_NULL) {
        dpu_err("null pointer!");
        return DRV_ERR_DPU_NULL_POINTER;
    }

    get_dpu_layer_device(layer_id, layer_dev);

    osal_spin_lock_irqsave(&layer_dev->layer_lock, &flag);
    *colorkey = layer_dev->run_info.colorkey;
    osal_spin_unlock_irqrestore(&layer_dev->layer_lock, &flag);

    return EXT_SUCCESS;
}

td_s32 dpu_layer_refresh(void)
{
#ifdef GPU_SUPPORT_VIDEO_MODE
    td_u32 layer_id;
    unsigned long flag = 0;
    ext_rect gp_region = { 0 };

    td_u32 time_start = 0;
    td_u32 time_end = 0;
    dpu_dfx_info *dfx_info = dpu_dfx_get_info();

    dfx_info->refresh_cnt++;
    dfx_info->refresh_total++;
    dpu_dfx_get_time_us(&time_start);

    dpu_disp_wait_vthd_percent_signal();
    dpu_isr_clear_finish_flag();
    dpu_isr_clear_vthd_percent_flag();
    dpu_layer_calculate_gp_region(&gp_region);

    for (layer_id = (td_u32)DRV_DPU_LAYER_ID1; layer_id <= (td_u32)DRV_DPU_MAX_LAYER_ID; layer_id++) {
        if (g_dpu_layer_dev[layer_id] == TD_NULL) {
            continue;
        }

        if ((g_dpu_layer_dev[layer_id]->is_create != TD_TRUE) ||
            (g_dpu_layer_dev[layer_id]->update_flag != TD_TRUE)) {
            continue;
        }

        osal_spin_lock_irqsave(&g_dpu_layer_dev[layer_id]->layer_lock, &flag);
        dpu_layer_revise_update_region(g_dpu_layer_dev[layer_id], &gp_region);
        dpu_layer_config_update(g_dpu_layer_dev[layer_id]);
        g_dpu_layer_dev[layer_id]->update_flag = TD_FALSE;
        osal_spin_unlock_irqrestore(&g_dpu_layer_dev[layer_id]->layer_lock, &flag);
    }
    dpu_disp_set_chnl_reg_up();

    dpu_disp_wait_frame_done();
    dpu_dfx_get_time_us(&time_end);
    dfx_info->refresh_intf_cost = (td_float)(time_end - time_start);
#else
    td_u32 layer_id;
    unsigned long flag = 0;
    ext_rect gp_region = { 0 };
    td_bool need_cur_done = TD_FALSE;

    td_u32 time_start = 0;
    td_u32 time_end = 0;
    dpu_dfx_info *dfx_info = dpu_dfx_get_info();

    dfx_info->refresh_cnt++;
    dfx_info->refresh_total++;
    dpu_dfx_get_time_us(&time_start);

    uapi_pm_add_sleep_veto(PM_ID_DPU);

    dpu_layer_calculate_gp_region(&gp_region);

    for (layer_id = (td_u32)DRV_DPU_LAYER_ID1; layer_id <= (td_u32)DRV_DPU_MAX_LAYER_ID; layer_id++) {
        if (g_dpu_layer_dev[layer_id] == TD_NULL) {
            continue;
        }

        if ((g_dpu_layer_dev[layer_id]->is_create != TD_TRUE) ||
            (g_dpu_layer_dev[layer_id]->update_flag != TD_TRUE)) {
            continue;
        }

        if (g_dpu_layer_dev[layer_id]->vblank == TD_TRUE) {
            need_cur_done = TD_TRUE;
        } else {
            dpu_disp_wait_frame_done(); /* wait last frame done */
        }

        osal_spin_lock_irqsave(&g_dpu_layer_dev[layer_id]->layer_lock, &flag);
        dpu_layer_revise_update_region(g_dpu_layer_dev[layer_id], &gp_region);
        dpu_layer_config_update(g_dpu_layer_dev[layer_id]);
        g_dpu_layer_dev[layer_id]->update_flag = TD_FALSE;
        osal_spin_unlock_irqrestore(&g_dpu_layer_dev[layer_id]->layer_lock, &flag);
    }

    dpu_disp_update_dhd_region(&gp_region);
    dpu_disp_set_timing_and_screen_region();
    dpu_disp_set_chnl_reg_up();
    dpu_isr_clear_finish_flag();

#ifdef MIPI_ULPS_SUPPORT
    if (uapi_mipi_tx_is_in_ulps() == true) {
        dpu_err("mipi_tx_is_not_exit_ulps, can not flush\n");
    }
#endif
    dpu_hal_disp_set_frame_ready();

    if (need_cur_done == TD_TRUE) {
        dpu_disp_wait_frame_done(); /* wait current frame done */
    }

    dpu_dfx_get_time_us(&time_end);
    dfx_info->refresh_intf_cost = (td_float)(time_end - time_start);
#endif
    return EXT_SUCCESS;
}

td_s32 dpu_layer_wait_vsync(void)
{
    dpu_disp_wait_te_signal();
    return EXT_SUCCESS;
}

td_s32 dpu_layer_set_vblank(drv_dpu_layer_id layer_id, td_bool vblank)
{
    unsigned long flag = 0;
    dpu_layer_dev *layer_dev = TD_NULL;

    get_dpu_layer_device(layer_id, layer_dev);

    osal_spin_lock_irqsave(&layer_dev->layer_lock, &flag);
    layer_dev->vblank = vblank;
    osal_spin_unlock_irqrestore(&layer_dev->layer_lock, &flag);

    return EXT_SUCCESS;
}

td_s32 dpu_layer_get_vblank(drv_dpu_layer_id layer_id, td_bool *vblank)
{
    unsigned long flag = 0;
    dpu_layer_dev *layer_dev = TD_NULL;

    get_dpu_layer_device(layer_id, layer_dev);

    osal_spin_lock_irqsave(&layer_dev->layer_lock, &flag);
    *vblank = layer_dev->vblank;
    osal_spin_unlock_irqrestore(&layer_dev->layer_lock, &flag);

    return  EXT_SUCCESS;
}

td_s32 dpu_layer_query_state(drv_dpu_layer_id layer_id)
{
    uapi_unused(layer_id);

    return TD_TRUE;
}

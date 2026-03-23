/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: vau adp node ctrl file
 */

#include "vau_hal.h"
#include "vau_dfx.h"
#include "vau_hal_type.h"
#include "vau_hal_apb.h"
#include "vau_hal_affine.h"
#include "vau_mem.h"
#include "vau_hal_vector_draw.h"

#define DCMP_TILE_WIDTH_4 4
#define VAU_CMP_HEADER_SIZE 16

typedef struct {
    drv_gfx_fmt drv_fmt;
    hal_vau_fmt hal_fmt;
} vau_fmt_mapping;

typedef struct {
    drv_gfx_argb_order drv_order;
    hal_vau_argb_order hal_order;
} vau_fmt_order_mapping;

typedef struct {
    drv_vau_rop_mode drv_mode;
    hal_vau_rop_mode hal_mode;
} vau_rop_mode_mapping;

static vau_fmt_mapping g_vau_fmt_mapping[] = {
    { DRV_GFX_FMT_ARGB8888, HAL_VAU_FMT_ARGB8888 },
    { DRV_GFX_FMT_KRGB8888, HAL_VAU_FMT_KRGB8888 },
    { DRV_GFX_FMT_ARGB4444, HAL_VAU_FMT_ARGB4444 },
    { DRV_GFX_FMT_ARGB1555, HAL_VAU_FMT_ARGB1555 },
    { DRV_GFX_FMT_ARGB8565, HAL_VAU_FMT_ARGB8565 },
    { DRV_GFX_FMT_RGB888, HAL_VAU_FMT_RGB888 },
    { DRV_GFX_FMT_RGB444, HAL_VAU_FMT_RGB444 },
    { DRV_GFX_FMT_RGB555, HAL_VAU_FMT_RGB555 },
    { DRV_GFX_FMT_RGB565, HAL_VAU_FMT_RGB565 },
    { DRV_GFX_FMT_A1, HAL_VAU_FMT_A1 },
    { DRV_GFX_FMT_A8, HAL_VAU_FMT_A8 },
    { DRV_GFX_FMT_A1B, HAL_VAU_FMT_A1B },
    { DRV_GFX_FMT_CLUT1, HAL_VAU_FMT_CLUT1 },
    { DRV_GFX_FMT_CLUT2, HAL_VAU_FMT_CLUT2 },
    { DRV_GFX_FMT_CLUT4, HAL_VAU_FMT_CLUT4 },
    { DRV_GFX_FMT_CLUT8, HAL_VAU_FMT_CLUT8 },
    { DRV_GFX_FMT_ACLUT44, HAL_VAU_FMT_ACLUT44 },
    { DRV_GFX_FMT_ACLUT88, HAL_VAU_FMT_ACLUT88 },
    { DRV_GFX_FMT_CLUT1B, HAL_VAU_FMT_CLUT1B },
    { DRV_GFX_FMT_CLUT2B, HAL_VAU_FMT_CLUT2B },
    { DRV_GFX_FMT_CLUT4B, HAL_VAU_FMT_CLUT4B },
    { DRV_GFX_FMT_A2, HAL_VAU_FMT_A2 },
    { DRV_GFX_FMT_A4, HAL_VAU_FMT_A4 },
    { DRV_GFX_FMT_A2B, HAL_VAU_FMT_A2B },
    { DRV_GFX_FMT_A4B, HAL_VAU_FMT_A4B },
};

static vau_fmt_order_mapping g_vau_fmt_order_mapping[] = {
    { DRV_GFX_FMT_ORDER_ARGB, HAL_VAU_FMT_ORDER_ARGB },
    { DRV_GFX_FMT_ORDER_ARBG, HAL_VAU_FMT_ORDER_ARBG },
    { DRV_GFX_FMT_ORDER_AGRB, HAL_VAU_FMT_ORDER_AGRB },
    { DRV_GFX_FMT_ORDER_AGBR, HAL_VAU_FMT_ORDER_AGBR },
    { DRV_GFX_FMT_ORDER_ABRG, HAL_VAU_FMT_ORDER_ABRG },
    { DRV_GFX_FMT_ORDER_ABGR, HAL_VAU_FMT_ORDER_ABGR },

    { DRV_GFX_FMT_ORDER_RAGB, HAL_VAU_FMT_ORDER_RAGB },
    { DRV_GFX_FMT_ORDER_RABG, HAL_VAU_FMT_ORDER_RABG },
    { DRV_GFX_FMT_ORDER_RGAB, HAL_VAU_FMT_ORDER_RGAB },
    { DRV_GFX_FMT_ORDER_RGBA, HAL_VAU_FMT_ORDER_RGBA },
    { DRV_GFX_FMT_ORDER_RBAG, HAL_VAU_FMT_ORDER_RBAG },
    { DRV_GFX_FMT_ORDER_RBGA, HAL_VAU_FMT_ORDER_RBGA },

    { DRV_GFX_FMT_ORDER_GRAB, HAL_VAU_FMT_ORDER_GRAB },
    { DRV_GFX_FMT_ORDER_GRBA, HAL_VAU_FMT_ORDER_GRBA },
    { DRV_GFX_FMT_ORDER_GARB, HAL_VAU_FMT_ORDER_GARB },
    { DRV_GFX_FMT_ORDER_GABR, HAL_VAU_FMT_ORDER_GABR },
    { DRV_GFX_FMT_ORDER_GBRA, HAL_VAU_FMT_ORDER_GBRA },
    { DRV_GFX_FMT_ORDER_GBAR, HAL_VAU_FMT_ORDER_GBAR },

    { DRV_GFX_FMT_ORDER_BRGA, HAL_VAU_FMT_ORDER_BRGA },
    { DRV_GFX_FMT_ORDER_BRAG, HAL_VAU_FMT_ORDER_BRAG },
    { DRV_GFX_FMT_ORDER_BGRA, HAL_VAU_FMT_ORDER_BGRA },
    { DRV_GFX_FMT_ORDER_BGAR, HAL_VAU_FMT_ORDER_BGAR },
    { DRV_GFX_FMT_ORDER_BARG, HAL_VAU_FMT_ORDER_BARG },
    { DRV_GFX_FMT_ORDER_BAGR, HAL_VAU_FMT_ORDER_BAGR },
};

static vau_rop_mode_mapping g_vau_rop_mode_mapping[] = {
    { DRV_VAU_ROP_BLACK,       HAL_VAU_ROP_BLACK },
    { DRV_VAU_ROP_NOTMERGEPEN, HAL_VAU_ROP_NOTMERGEPEN },
    { DRV_VAU_ROP_MASKNOTPEN,  HAL_VAU_ROP_MASKNOTPEN },
    { DRV_VAU_ROP_NOTCOPYPEN,  HAL_VAU_ROP_NOTCOPYPEN },
    { DRV_VAU_ROP_MASKPENNOT,  HAL_VAU_ROP_MASKPENNOT },
    { DRV_VAU_ROP_NOT,         HAL_VAU_ROP_NOT },
    { DRV_VAU_ROP_XORPEN,      HAL_VAU_ROP_XORPEN },
    { DRV_VAU_ROP_NOTMASKPEN,  HAL_VAU_ROP_NOTMASKPEN },
    { DRV_VAU_ROP_MASKPEN,     HAL_VAU_ROP_MASKPEN },
    { DRV_VAU_ROP_NOTXORPEN,   HAL_VAU_ROP_NOTXORPEN },
    { DRV_VAU_ROP_NOP,         HAL_VAU_ROP_NOP },
    { DRV_VAU_ROP_MERGENOTPEN, HAL_VAU_ROP_MERGENOTPEN },
    { DRV_VAU_ROP_COPYPEN,     HAL_VAU_ROP_COPYPEN },
    { DRV_VAU_ROP_MERGEPENNOT, HAL_VAU_ROP_MERGEPENNOT },
    { DRV_VAU_ROP_MERGEPEN,    HAL_VAU_ROP_MERGEPEN },
    { DRV_VAU_ROP_WHITE,       HAL_VAU_ROP_WHITE },
};

static hal_vau_blend_mode g_vau_src1_blend_cmd_mode[DRV_VAU_BLENDCMD_MAX] = {
    HAL_VAU_BLEND_INVSRC2ALPHA,
    HAL_VAU_BLEND_ZERO,
    HAL_VAU_BLEND_ZERO,
    HAL_VAU_BLEND_INVSRC2ALPHA,
    HAL_VAU_BLEND_ONE,
    HAL_VAU_BLEND_ZERO,
    HAL_VAU_BLEND_SRC2ALPHA,
    HAL_VAU_BLEND_ZERO,
    HAL_VAU_BLEND_INVSRC2ALPHA,
    HAL_VAU_BLEND_INVSRC2ALPHA,
    HAL_VAU_BLEND_SRC2ALPHA,
    HAL_VAU_BLEND_ONE,
    HAL_VAU_BLEND_INVSRC2ALPHA,
    HAL_VAU_BLEND_ONE,
    0
};

static hal_vau_blend_mode g_vau_src2_blend_cmd_mode[DRV_VAU_BLENDCMD_MAX] = {
    HAL_VAU_BLEND_SRC2ALPHA,
    HAL_VAU_BLEND_ZERO,
    HAL_VAU_BLEND_ONE,
    HAL_VAU_BLEND_ONE,
    HAL_VAU_BLEND_INVSRC1ALPHA,
    HAL_VAU_BLEND_SRC1ALPHA,
    HAL_VAU_BLEND_ZERO,
    HAL_VAU_BLEND_INVSRC1ALPHA,
    HAL_VAU_BLEND_ZERO,
    HAL_VAU_BLEND_SRC1ALPHA,
    HAL_VAU_BLEND_INVSRC1ALPHA,
    HAL_VAU_BLEND_ONE,
    HAL_VAU_BLEND_INVSRC1ALPHA,
    HAL_VAU_BLEND_ZERO,
    0
};

static hal_vau_fmt hal_trans_fmt_drv2hal(drv_gfx_fmt drv_fmt)
{
    td_u8 i;
    hal_vau_fmt hal_fmt = HAL_VAU_FMT_ARGB8888;
    for (i = 0; i < sizeof(g_vau_fmt_mapping) / sizeof(g_vau_fmt_mapping[0]); i++) {
        if (g_vau_fmt_mapping[i].drv_fmt == drv_fmt) {
            hal_fmt = g_vau_fmt_mapping[i].hal_fmt;
        }
    }
    return hal_fmt;
}

static hal_vau_argb_order hal_trans_argb_order_drv2hal(drv_gfx_argb_order drv_order)
{
    td_u8 i;
    hal_vau_argb_order hal_order = HAL_VAU_FMT_ORDER_ARGB;
    for (i = 0; i < sizeof(g_vau_fmt_order_mapping) / sizeof(g_vau_fmt_order_mapping[0]); i++) {
        if (g_vau_fmt_order_mapping[i].drv_order == drv_order) {
            hal_order = g_vau_fmt_order_mapping[i].hal_order;
        }
    }
    return hal_order;
}

static hal_vau_compress_mode hal_trans_compress_mode_drv2hal(drv_gfx_compress_mode drv_compress_mode)
{
    if (drv_compress_mode == DRV_GFX_COMPRESS_HFBC) {
        return HAL_VAU_COMPRESS_HFBC;
    } else if (drv_compress_mode == DRV_GFX_COMPRESS_HFBC_ABYPASS) {
        return HAL_VAU_COMPRESS_HFBC_ABYPASS;
    } else {
        return HAL_VAU_COMPRESS_NONE;
    }
}

static td_void hal_trans_alpha_info_drv2hal(const drv_vau_alpha_info *drv_alpha_info,
                                            hal_vau_alpha_info *hal_alpha_info)
{
    hal_alpha_info->is_alpha_max_128  = drv_alpha_info->is_alpha_max_128;
    hal_alpha_info->alpha0            = drv_alpha_info->alpha0;
    hal_alpha_info->alpha1            = drv_alpha_info->alpha1;
    hal_alpha_info->alpha_thd         = drv_alpha_info->alpha_thd; /* for output fmt 1555 */
    hal_alpha_info->global_alpha      = drv_alpha_info->global_alpha;
    hal_alpha_info->golbal_premult_en = drv_alpha_info->golbal_premult_en;
    hal_alpha_info->global_alpha_en   = drv_alpha_info->global_alpha_en;
    hal_alpha_info->premult_en        = drv_alpha_info->premult_en;
    hal_alpha_info->pixel_alpha_en    = drv_alpha_info->pixel_alpha_en;
}

static td_void hal_trans_surface_drv2hal(const drv_vau_surface *drv_surface, hal_vau_surface *hal_surface)
{
    td_s32 ret;
    hal_surface->width            = drv_surface->width;
    hal_surface->height           = drv_surface->height;
    hal_surface->stride           = drv_surface->stride;
    hal_surface->fmt              = hal_trans_fmt_drv2hal(drv_surface->fmt);
    hal_surface->argb_order       = hal_trans_argb_order_drv2hal(drv_surface->argb_order);
    hal_surface->compress_mode    = hal_trans_compress_mode_drv2hal(drv_surface->compress_info.compress_mode);
    if (hal_surface->compress_mode != HAL_VAU_COMPRESS_NONE) {
        ret = memcpy_s(hal_surface->compress_header, sizeof(hal_surface->compress_header),
                       (td_void *)(uintptr_t)drv_surface->addr, VAU_CMP_HEADER_SIZE);
        if (ret != EOK) {
            vau_err("compress_info copy fail!\n");
        }
        hal_surface->addr             = drv_surface->addr + VAU_CMP_HEADER_SIZE;
    } else {
        hal_surface->addr             = drv_surface->addr;
    }
    hal_surface->clut_addr        = drv_surface->clut_addr;
    hal_surface->is_color_surface = drv_surface->is_color_surface;
    hal_surface->color            = drv_surface->color;
    hal_surface->rect             = drv_surface->rect;
    hal_surface->affine_en        = drv_surface->attr.affine.affine_en;

    hal_trans_alpha_info_drv2hal(&drv_surface->alpha_info, &hal_surface->alpha_info);
}

static hal_vau_rop_mode hal_trans_rop_mode_drv2hal(drv_vau_rop_mode drv_mode)
{
    td_u8 i;
    hal_vau_rop_mode hal_mode = HAL_VAU_ROP_MERGEPEN;
    for (i = 0; i < sizeof(g_vau_rop_mode_mapping) / sizeof(g_vau_rop_mode_mapping[0]); i++) {
        if (g_vau_rop_mode_mapping[i].drv_mode == drv_mode) {
            hal_mode = g_vau_rop_mode_mapping[i].hal_mode;
        }
    }
    return hal_mode;
}

static td_void hal_trans_rop_opt_drv2hal(const drv_vau_rop_opt *drv_rop, hal_vau_rop_opt *hal_rop)
{
    hal_rop->rop_enable     = drv_rop->rop_enable;
    hal_rop->alpha_rop_mode = hal_trans_rop_mode_drv2hal(drv_rop->alpha_rop_mode);
    hal_rop->color_rop_mode = hal_trans_rop_mode_drv2hal(drv_rop->color_rop_mode);
}

static td_void hal_trans_colorkey_component_drv2hal(const drv_vau_colorkey_component *drv_component,
                                                    hal_vau_colorkey_component *hal_component)
{
    if (drv_component->is_component_ignore == TD_TRUE) {
        hal_component->key_mode = HAL_VAU_COLORKEY_VALUE_IGNORE;
    } else {
        if (drv_component->is_component_out == TD_TRUE) {
            hal_component->key_mode = HAL_VAU_COLORKEY_VALUE_OUT;
        } else {
            hal_component->key_mode = HAL_VAU_COLORKEY_VALUE_IN;
        }
    }

    hal_component->key_min  = drv_component->component_min;
    hal_component->key_max  = drv_component->component_max;
    hal_component->key_mask = 0xFF; /* default all bits used for key decision */
}

static td_void hal_trans_colorkey_opt_drv2hal(const drv_vau_colorkey_opt *drv_colorkey,
                                              hal_vau_colorkey_opt *hal_colorkey)
{
    hal_colorkey->key_en = drv_colorkey->colorkey_enable;

    switch (drv_colorkey->colorkey_mode) {
        case DRV_VAU_COLORKEY_NONE:
        case DRV_VAU_COLORKEY_BACKGROUND:
            hal_colorkey->key_sel = HAL_VAU_COLORKEY_BACKGROUND;
            break;
        case DRV_VAU_COLORKEY_FOREGROUND:
            hal_colorkey->key_sel = HAL_VAU_COLORKEY_FOREGROUND_AFTER_CLUT;
            break;
        default:
            hal_colorkey->key_sel = HAL_VAU_COLORKEY_BACKGROUND;
    }

    hal_trans_colorkey_component_drv2hal(&drv_colorkey->colorkey_value.colorkey_argb.alpha,
                                         &hal_colorkey->component[HAL_VAU_COMPONENT_ALPHA]);
    hal_trans_colorkey_component_drv2hal(&drv_colorkey->colorkey_value.colorkey_argb.red,
                                         &hal_colorkey->component[HAL_VAU_COMPONENT_RED]);
    hal_trans_colorkey_component_drv2hal(&drv_colorkey->colorkey_value.colorkey_argb.green,
                                         &hal_colorkey->component[HAL_VAU_COMPONENT_GREEN]);
    hal_trans_colorkey_component_drv2hal(&drv_colorkey->colorkey_value.colorkey_argb.blue,
                                         &hal_colorkey->component[HAL_VAU_COMPONENT_BLUE]);
}

static td_void vau_hal_set_src1_draw_attr(vau_hardware_node *node, const drv_vau_surface *surface)
{
    if (surface->attr.crop.crop_en) {
        vau_hal_node_set_src1_crop(node, &surface->attr.crop.crop_rect);
    }
}

static td_void vau_hal_set_src2_draw_attr(vau_hardware_node *node,
                                          const drv_vau_surface *src_surface,
                                          const drv_vau_surface *dst_surface)
{
    hal_vau_t2r_info info = { 0 };

    if (src_surface->attr.vector.enable) {
        vau_hal_vector_draw_config(node, src_surface, dst_surface);
    } else if (src_surface->attr.affine.affine_en) {
        vau_hal_affine_config(node, src_surface, dst_surface);
    }

    if (vau_dfx_check_mask(VAU_DBG_MASK_T2R_DISABLE)) {
        vau_dbg("disable t2r\n");
        return;
    }
    if (src_surface->attr.vector.enable) {
        return ;
    }

    if (src_surface->attr.affine.affine_en || (src_surface->compress_info.compress_mode != DRV_GFX_COMPRESS_NONE)) {
        info.t2r_en = 1;
        info.t2r_ck_gt_en = 1;
#ifdef VAU_T2R_USER_CFG
        info.cfg_en = 1;
        info.tile_width = src_surface->attr.affine.affine_en ? \
        DCMP_TILE_WIDTH_4 : src_surface->tile_width;
#else
        info.cfg_en = 0;
#endif
    }

    vau_hal_node_set_src2_t2r(node, &info);
}

static td_void vau_hal_set_dst_draw_attr(vau_hardware_node *node, const drv_vau_surface *surface)
{
    if (surface->attr.crop.crop_en) {
        vau_hal_node_set_dst_crop(node, &surface->attr.crop.crop_rect);
    }
}

td_void vau_hal_set_src1(vau_hardware_node *node, const drv_vau_surface *surface, td_bool is_dma)
{
    hal_vau_src_mode src_mode;
    hal_vau_surface hal_surface = {0};

    hal_trans_surface_drv2hal(surface, &hal_surface);

    vau_hal_node_set_src1_base_info(node, &hal_surface);

    if (is_dma) {
        src_mode = surface->is_color_surface ? HAL_VAU_MODE_TYPE_REG_DMA : HAL_VAU_MODE_TYPE_DMA;
    } else {
        src_mode = surface->is_color_surface ? HAL_VAU_MODE_TYPE_REG : HAL_VAU_MODE_TYPE_MEM;
        vau_hal_node_set_src1_alpha(node, &hal_surface);
        vau_hal_set_src1_draw_attr(node, surface);
    }
    vau_hal_node_set_src1_mode(node, src_mode);
}

td_void vau_hal_set_src2(vau_hardware_node *node, const drv_vau_surface *src_surface,
                         const drv_vau_surface *dst_surface)
{
    hal_vau_surface hal_surface = {0};

    hal_trans_surface_drv2hal(src_surface, &hal_surface);

    if (src_surface->is_color_surface) {
        vau_hal_node_set_src2_mode(node, HAL_VAU_MODE_TYPE_REG);
    }

    if (!src_surface->attr.vector.enable) {
        vau_hal_node_set_src2_base_info(node, &hal_surface);
        vau_hal_node_set_src2_clut(node, &hal_surface);
    }

    vau_hal_node_set_src2_alpha(node, &hal_surface);
    vau_hal_set_src2_draw_attr(node, src_surface, dst_surface);
}

td_void vau_hal_set_dst(vau_hardware_node *node, const drv_vau_surface *surface, td_bool is_dma)
{
    hal_vau_surface hal_surface = {0};

    hal_trans_surface_drv2hal(surface, &hal_surface);

    vau_hal_node_set_dst_base_info(node, &hal_surface);
    if (is_dma == TD_FALSE) {
        vau_hal_set_dst_draw_attr(node, surface);
    }

    return;
}

td_void vau_hal_set_blend_mode(vau_hardware_node *node, drv_vau_blit_opt *opt)
{
    hal_vau_blend_mode src1_blend_mode;
    hal_vau_blend_mode src2_blend_mode;

    if (opt->blend_opt.blend_cmd == DRV_VAU_BLENDCMD_CONFIG) {
        vau_hal_node_set_blend_mode(node, (hal_vau_blend_mode)opt->blend_opt.background_blend_mode,
            (hal_vau_blend_mode)opt->blend_opt.foreground_blend_mode);
    } else {
        src1_blend_mode =
            g_vau_src1_blend_cmd_mode[(td_u32)opt->blend_opt.blend_cmd % DRV_VAU_BLENDCMD_MAX];
        src2_blend_mode =
            g_vau_src2_blend_cmd_mode[(td_u32)opt->blend_opt.blend_cmd % DRV_VAU_BLENDCMD_MAX];
        vau_hal_node_set_blend_mode(node, src1_blend_mode, src2_blend_mode);
    }

    vau_hal_node_set_blend_alpha_mode(node, (hal_vau_blend_alpha_mode)opt->blend_opt.out_alpha_mode);
}

td_void vau_hal_set_gaussian(vau_hardware_node *node, drv_vau_blit_opt *opt)
{
    td_u32 k;
    td_u32 size;
    td_float x_sigma;
    td_float x_sum = 0.0f;
    const td_float sigma0 = 100.0f / 3.1415926;
    const td_s32 radius[3] = {76, 46, 23};
    const td_float weights[3] = {0.1618f, 0.5502f, 0.9495f};
    td_u32 radius_hal[3] = { 0 };
    td_float weights_hal[3] = { 0 };

    x_sigma = opt->blur_opt.gaussian_attr.x_sigma;
    if (fabs(x_sigma - 0) <= (1e-6)) {
        x_sigma = opt->blur_opt.gaussian_attr.y_sigma;
    }

    size = (td_u32)(sizeof(radius) / sizeof(td_s32));
    for (k = 0; k < size; k++) {
        radius_hal[k] = (td_u32)(radius[k] * (x_sigma / sigma0) + 0.5f);
        x_sum += weights[k] * (2 * radius_hal[k] + 1); // 2 times
    }

    if (fabs(x_sigma - 0) <= (1e-6)) {
        return;
    }

    size = (td_u32)(sizeof(weights) / sizeof(td_float));
    for (k = 0; k < size; k++) {
        weights_hal[k] = weights[k] / x_sum;
    }

    node->mix.src2.vau_gauss_filter_ctrl.bits.gauss_filter_en = 1;
    node->mix.src2.vau_gauss_filter_ctrl.bits.gauss_filter_ck_gt_en = 1;
    node->mix.src2.vau_gauss_filter_radius0.bits.x_radius0 = radius_hal[0];
    node->mix.src2.vau_gauss_filter_radius1.bits.x_radius1 = radius_hal[1];
    node->mix.src2.vau_gauss_filter_radius2.bits.x_radius2 = radius_hal[2]; // 2 index
    node->mix.src2.vau_gauss_filter_weight0.bits.x_weight0 = weights_hal[0];
    node->mix.src2.vau_gauss_filter_weight1.bits.x_weight1 = weights_hal[1];
    node->mix.src2.vau_gauss_filter_weight2.bits.x_weight2 = weights_hal[2]; // 2 index
}

td_void vau_hal_set_set_rop(vau_hardware_node *node, const drv_vau_rop_opt *drv_rop)
{
    hal_vau_rop_opt hal_rop = {0};

    hal_trans_rop_opt_drv2hal(drv_rop, &hal_rop);
    vau_hal_node_set_rop(node, &hal_rop);

    return;
}

td_void vau_hal_set_colorkey(vau_hardware_node *node, const drv_vau_colorkey_opt *drv_colorkey)
{
    hal_vau_colorkey_opt hal_colorkey = {0};

    hal_trans_colorkey_opt_drv2hal(drv_colorkey, &hal_colorkey);
    vau_hal_node_set_colorkey(node, &hal_colorkey);

    return;
}

td_void vau_hal_set_cbm(vau_hardware_node *node, const ext_rect *cbm_rect, td_u32 alu_mode)
{
    vau_hal_node_set_cbm(node, cbm_rect, alu_mode);
}

td_void vau_hal_set_tile_en(const vau_hardware_node *node)
{
    uapi_unused(node);

    return;
}

td_void vau_hal_add_next_node(vau_hardware_node *node, td_u32 phy_addr)
{
    vau_hal_node_add_next_node(node, phy_addr);
}

td_void vau_hal_enable_complete_int(vau_hardware_node *node)
{
    vau_hal_node_enable_complete_int(node);
}

td_void vau_hal_enable_lowdelay_int(vau_hardware_node *node)
{
    vau_hal_node_enable_lowdelay_int(node);
}

td_void vau_hal_set_tunl_enable(vau_hardware_node *node)
{
    vau_hal_node_set_tunl_enable(node);
}

/* APB SET */
td_void vau_hal_init(td_void)
{
    td_u32 start, end;
    vau_hal_vector_draw_mem_init();
    vau_hal_apb_set_clock(TD_TRUE);
    vau_hal_apb_reset(TD_TRUE);
    vau_hal_apb_reset(TD_FALSE);
    vau_hal_apb_set_video_ckdiv(0);
    vau_hal_apb_init_queue();
    vau_hal_apb_vector_draw_init();
    vau_hal_apb_sram_ctrl(TD_TRUE);

    vau_mem_get_range(VAU_MEM_TYPE_PATH_CMD, &start, &end);
    vau_hal_apb_vector_cmddata_range(VAU_VECTOR_RANGE_CMD, start, end);
    vau_mem_get_range(VAU_MEM_TYPE_PATH_DATA, &start, &end);
    vau_hal_apb_vector_cmddata_range(VAU_VECTOR_RANGE_DATA, start, end);
}

td_void vau_hal_deinit(td_void)
{
    vau_hal_vector_draw_mem_deinit();
    vau_hal_apb_reset(TD_TRUE);
    vau_hal_apb_reset(TD_FALSE);
    vau_hal_apb_sram_ctrl(TD_FALSE);
    vau_hal_apb_set_clock(TD_FALSE);
    return;
}

td_void vau_hal_suspend(td_void)
{
    vau_hal_apb_clear_int_state(0xFF); /* 0xF: clear all int bits */
    vau_hal_deinit();
}

td_void vau_hal_resume(td_void)
{
    vau_hal_init();
}

td_bool vau_hal_check_is_idle(td_void)
{
    td_u32 i;
    /* get the state of tde one more time ,make sure it's idle */
    for (i = 0; i < 10; i++) {  /* 10:index */
        if (!vau_hal_apb_is_idle()) {
            return TD_FALSE;
        }
    }
    return TD_TRUE;
}

td_s32 vau_hal_execute_node(td_u32 start_node)
{
    if (vau_hal_check_is_idle()) {
        vau_hal_apb_clear_int_state(0xFF); /* 0xF: clear all int bits */
        vau_hal_apb_set_start_node(start_node);
        vau_hal_apb_set_start_up();
    } else {
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

td_u32 vau_hal_get_current_node(td_void)
{
    return vau_hal_apb_get_current_ndoe();
}

td_u32 vau_hal_get_int_state(td_void)
{
    return vau_hal_apb_get_int_state();
}

td_void vau_hal_clear_int_state(td_u32 mask)
{
    return vau_hal_apb_clear_int_state(mask);
}

td_void vau_hal_set_start_up(td_void)
{
    return vau_hal_apb_set_start_up();
}

td_s32 vau_hal_reset(td_bool reset)
{
    return vau_hal_apb_reset(reset);
}

td_void vau_hal_free_vector_mem(vau_hardware_node *node)
{
    td_bool inner_cmd;
    td_bool inner_data;
    td_u32 start, end;

    if (node->vau_draw_ctrl.bits.draw_en != 1) {
        return;
    }

    vau_mem_get_range(VAU_MEM_TYPE_PATH_CMD, &start, &end);
    inner_cmd = (node->vau_draw_cmd_addr.u32 <= end) && (node->vau_draw_cmd_addr.u32 >= start);

    vau_mem_get_range(VAU_MEM_TYPE_PATH_DATA, &start, &end);
    inner_data = (node->vau_draw_path_addr.u32 <= end) && (node->vau_draw_path_addr.u32 >= start);

    if (inner_data) {
        vau_mem_ref_dec(VAU_MEM_TYPE_PATH_DATA, (td_uchar *)(uintptr_t)node->vau_draw_path_addr.u32);
    }
    if (inner_cmd) {
        vau_mem_ref_dec(VAU_MEM_TYPE_PATH_CMD, (td_uchar *)(uintptr_t)node->vau_draw_cmd_addr.u32);
    }
    return;
}

td_void vau_hal_apb_print_reg_cfg(const vau_hardware_node *hw_node)
{
#ifdef CONFIG_VAU_DFX_DEBUG
    vau_hal_apb_print_reg(HAL_VAU_REG_BASEADDR, 0x168, 0x1fc);
    vau_hal_print_node_all(hw_node, 0x100, 0x154, 0x200); // reg:0x200~0x254 is node 0x100~0x154
    vau_hal_apb_print_reg(HAL_VAU_REG_BASEADDR, 0x258, 0x40ac);
#else
    uapi_unused(hw_node);
#endif
    return;
}

td_void vau_hal_set_alpha_strb(vau_hardware_node *node, td_bool enable)
{
    vau_hal_node_set_alpha_strb(node, enable);
    return;
}


/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: capability
 */

#include "vau_hal_capability.h"
#include "vau_dfx.h"

/*-------------------------------- macro define -------------------------------------------*/

#define VAU_RESOLUTION_WIDTH_MIN      1
#define VAU_RESOLUTION_HEIGHT_MIN     1
#define VAU_RESOLUTION_STRIDE_MIN     16
#define VAU_RESOLUTION_WIDTH_MAX      640
#define VAU_RESOLUTION_HEIGHT_MAX     640
#define VAU_RESOLUTION_STRIDE_MAX     4096 /* 640x4=2560, 2^12 = 4096 */

#define VAU_CMP_TILE_BS_WIDTH         16
#define VAU_CMP_TILE_BS_HEIGHT        4

/*-------------------------------- struct define ------------------------------------------*/

typedef enum {
    VAU_LYAER_SRC1 = 0,
    VAU_LYAER_SRC2,
    VAU_LYAER_DST,
} vau_layer_index;

typedef union {
    struct {
        td_u32 affine     : 1;
        td_u32 crop       : 1;
        td_u32 dma        : 1;
        td_u32 trans      : 1;
        td_u32 vector_draw : 1;
        td_u32 reserved   : 27;
    } bits;

    unsigned int u32;
} vau_draw_capability;

typedef union {
    struct {
        td_u32 rop         : 1;
        td_u32 color_key   : 1;
        td_u32 blend       : 1;
        td_u32 reserved    : 29;
    } bits;

    unsigned int u32;
} vau_blit_capability;

typedef union {
    struct {
        td_u32 support_fmt_argb8888 : 1;  /* [0] */
        td_u32 support_fmt_krgb8888 : 1;  /* [1] */
        td_u32 support_fmt_argb4444 : 1;  /* [2] */
        td_u32 support_fmt_argb1555 : 1;  /* [3] */
        td_u32 support_fmt_argb8565 : 1;  /* [4] */
        td_u32 support_fmt_rgb888   : 1;  /* [5] */
        td_u32 support_fmt_rgb444   : 1;  /* [6] */
        td_u32 support_fmt_rgb555   : 1;  /* [7] */
        td_u32 support_fmt_rgb565   : 1;  /* [8] */
        td_u32 support_fmt_a1       : 1;  /* [9] */
        td_u32 support_fmt_a8       : 1;  /* [10] */
        td_u32 reserved1            : 2;  /* [12:11] */
        td_u32 support_fmt_a1b      : 1;  /* [13] */
        td_u32 reserved2            : 2;  /* [15:14] */
        td_u32 support_fmt_clut1    : 1;  /* [16] */
        td_u32 support_fmt_clut2    : 1;  /* [17] */
        td_u32 support_fmt_clut4    : 1;  /* [18] */
        td_u32 support_fmt_clut8    : 1;  /* [19] */
        td_u32 support_fmt_aclut44  : 1;  /* [20] */
        td_u32 support_fmt_aclut88  : 1;  /* [21] */
        td_u32 support_fmt_clut1b   : 1;  /* [22] */
        td_u32 support_fmt_clut2b   : 1;  /* [23] */
        td_u32 support_fmt_clut4b   : 1;  /* [24] */
        td_u32 support_fmt_a2       : 1;  /* [25] */
        td_u32 support_fmt_a4       : 1;  /* [26] */
        td_u32 support_fmt_a2b      : 1;  /* [27] */
        td_u32 support_fmt_a4b      : 1;  /* [28] */
        td_u32 reserved3            : 3;  /* [31:29] */
    } bits;

    td_u32 all_bits;
} vau_support_fmts;

typedef union {
    struct {
        td_u32 support_fmt_order_argb : 1;
        td_u32 support_fmt_order_arbg : 1;
        td_u32 support_fmt_order_agrb : 1;
        td_u32 support_fmt_order_agbr : 1;
        td_u32 support_fmt_order_abrg : 1;
        td_u32 support_fmt_order_abgr : 1;

        td_u32 support_fmt_order_ragb : 1;
        td_u32 support_fmt_order_rabg : 1;
        td_u32 support_fmt_order_rbab : 1;
        td_u32 support_fmt_order_rgba : 1;
        td_u32 support_fmt_order_rbag : 1;
        td_u32 support_fmt_order_rbga : 1;

        td_u32 support_fmt_order_grab : 1;
        td_u32 support_fmt_order_grba : 1;
        td_u32 support_fmt_order_garb : 1;
        td_u32 support_fmt_order_gabr : 1;
        td_u32 support_fmt_order_gbra : 1;
        td_u32 support_fmt_order_gbar : 1;

        td_u32 support_fmt_order_brga : 1;
        td_u32 support_fmt_order_brag : 1;
        td_u32 support_fmt_order_bgra : 1;
        td_u32 support_fmt_order_bgar : 1;
        td_u32 support_fmt_order_barg : 1;
        td_u32 support_fmt_order_bagr : 1;
    } bits;

    td_u32 all_bits;
} vau_support_fmt_order;

typedef struct {
    vau_support_fmts support_fmts;
    vau_support_fmts support_fmts_dma;
    vau_support_fmts support_fmts_affine;

    vau_support_fmts support_fmts_cmp;
    vau_support_fmt_order support_fmt_orders_cmp;

    td_u32 min_width;
    td_u32 min_height;
    td_u32 min_stride;

    td_u32 max_width;
    td_u32 max_height;
    td_u32 max_stride;

    const vau_draw_capability *support_draw_attr_list;
    td_u32 support_draw_attr_list_size;
} vau_support_features;

/*-------------------------------- func declares ------------------------------------------*/

static td_s32 vau_check_surface(const drv_vau_surface *surface, vau_layer_index index, td_bool is_dma);
static inline td_s32 vau_check_resolution(const drv_vau_surface *surface, vau_layer_index index, td_bool is_dma);
static td_s32 vau_check_stride(const drv_vau_surface *surface);
static td_s32 vau_check_fmt(const drv_vau_surface *surface, vau_layer_index index, td_bool is_dma);
static td_s32 vau_check_addr(const drv_vau_surface *surface);
static td_s32 vau_check_draw_attr(const drv_vau_surface *surface, vau_layer_index index, td_bool is_dma);

/*-------------------------------- par define ---------------------------------------------*/

const static vau_draw_capability g_vau_src1_draw_capability_list[] = {
    /* no operation */
    {},
    { .bits.crop = 1 },
    { .bits.dma  = 1 }
};

const static vau_draw_capability g_vau_src2_draw_capability_list[] = {
    /* no operation */
    {},
    { .bits.affine = 1 },
    { .bits.vector_draw = 1}
};

const static vau_draw_capability g_vau_dst_draw_capability_list[] = {
    /* no operation */
    {},
    { .bits.crop = 1 },
};

const static vau_blit_capability g_vau_blit_capability_list[] = {
    /* no operation */
    {},
    /* double src and one operation */
    { .bits.rop = 1 },
    { .bits.color_key = 1 },
    { .bits.blend = 1 },
    /* double src and more than one operation */
    {
        .bits.rop = 1,
        .bits.color_key = 1
    },
    {
        .bits.blend = 1,
        .bits.color_key = 1
    },
};

const static vau_support_features g_vau_capability[] = {
    /* src1 */
    {
        .support_fmts.bits.support_fmt_argb8888 = 1,
        .support_fmts.bits.support_fmt_krgb8888 = 1,
        .support_fmts.bits.support_fmt_argb4444 = 1,
        .support_fmts.bits.support_fmt_argb1555 = 1,
        .support_fmts.bits.support_fmt_argb8565 = 1,
        .support_fmts.bits.support_fmt_rgb888   = 1,
        .support_fmts.bits.support_fmt_rgb444   = 1,
        .support_fmts.bits.support_fmt_rgb555   = 1,
        .support_fmts.bits.support_fmt_rgb565   = 1,

        .support_fmts_dma.bits.support_fmt_argb8888 = 1,
        .support_fmts_dma.bits.support_fmt_krgb8888 = 1,
        .support_fmts_dma.bits.support_fmt_argb4444 = 1,
        .support_fmts_dma.bits.support_fmt_argb1555 = 1,
        .support_fmts_dma.bits.support_fmt_argb8565 = 1,
        .support_fmts_dma.bits.support_fmt_rgb888   = 1,
        .support_fmts_dma.bits.support_fmt_rgb444   = 1,
        .support_fmts_dma.bits.support_fmt_rgb555   = 1,
        .support_fmts_dma.bits.support_fmt_rgb565   = 1,
        .support_fmts_dma.bits.support_fmt_a1       = 1,
        .support_fmts_dma.bits.support_fmt_a8       = 1,
        .support_fmts_dma.bits.support_fmt_a1b      = 1,
        .support_fmts_dma.bits.support_fmt_clut1    = 1,
        .support_fmts_dma.bits.support_fmt_clut2    = 1,
        .support_fmts_dma.bits.support_fmt_clut4    = 1,
        .support_fmts_dma.bits.support_fmt_clut8    = 1,
        .support_fmts_dma.bits.support_fmt_aclut44  = 1,
        .support_fmts_dma.bits.support_fmt_aclut88  = 1,
        .support_fmts_dma.bits.support_fmt_clut1b   = 1,
        .support_fmts_dma.bits.support_fmt_clut2b   = 1,
        .support_fmts_dma.bits.support_fmt_clut4b   = 1,

        .min_width  = VAU_RESOLUTION_WIDTH_MIN,
        .min_height = VAU_RESOLUTION_HEIGHT_MIN,
        .min_stride = VAU_RESOLUTION_STRIDE_MIN,
        .max_width  = VAU_RESOLUTION_WIDTH_MAX,
        .max_height = VAU_RESOLUTION_HEIGHT_MAX,
        .max_stride = VAU_RESOLUTION_STRIDE_MAX,

        .support_draw_attr_list = g_vau_src1_draw_capability_list,
        .support_draw_attr_list_size = sizeof(g_vau_src1_draw_capability_list) / \
                                       sizeof(g_vau_src1_draw_capability_list[0]),
    },
    /* src2 */
    {
        .support_fmts.bits.support_fmt_argb8888 = 1,
        .support_fmts.bits.support_fmt_krgb8888 = 1,
        .support_fmts.bits.support_fmt_argb4444 = 1,
        .support_fmts.bits.support_fmt_argb1555 = 1,
        .support_fmts.bits.support_fmt_argb8565 = 1,
        .support_fmts.bits.support_fmt_rgb888   = 1,
        .support_fmts.bits.support_fmt_rgb444   = 1,
        .support_fmts.bits.support_fmt_rgb555   = 1,
        .support_fmts.bits.support_fmt_rgb565   = 1,
        .support_fmts.bits.support_fmt_a1       = 1,
        .support_fmts.bits.support_fmt_a8       = 1,
        .support_fmts.bits.support_fmt_a1b      = 1,
        .support_fmts.bits.support_fmt_clut1    = 1,
        .support_fmts.bits.support_fmt_clut2    = 1,
        .support_fmts.bits.support_fmt_clut4    = 1,
        .support_fmts.bits.support_fmt_clut8    = 1,
        .support_fmts.bits.support_fmt_aclut44  = 1,
        .support_fmts.bits.support_fmt_aclut88  = 1,
        .support_fmts.bits.support_fmt_clut1b   = 1,
        .support_fmts.bits.support_fmt_clut2b   = 1,
        .support_fmts.bits.support_fmt_clut4b   = 1,

        .support_fmts.bits.support_fmt_a2       = 1,
        .support_fmts.bits.support_fmt_a4       = 1,
        .support_fmts.bits.support_fmt_a2b      = 1,
        .support_fmts.bits.support_fmt_a4b      = 1,

        .support_fmts_affine.bits.support_fmt_argb8888 = 1,
        .support_fmts_affine.bits.support_fmt_krgb8888 = 1,
        .support_fmts_affine.bits.support_fmt_argb4444 = 1,
        .support_fmts_affine.bits.support_fmt_argb1555 = 1,
        .support_fmts_affine.bits.support_fmt_argb8565 = 1,
        .support_fmts_affine.bits.support_fmt_rgb888   = 1,
        .support_fmts_affine.bits.support_fmt_rgb444   = 1,
        .support_fmts_affine.bits.support_fmt_rgb555   = 1,
        .support_fmts_affine.bits.support_fmt_rgb565   = 1,

        .support_fmts_cmp.bits.support_fmt_argb8888 = 1,
        .support_fmts_cmp.bits.support_fmt_rgb888   = 1,
        .support_fmts_cmp.bits.support_fmt_rgb565   = 1,
        .support_fmt_orders_cmp.bits.support_fmt_order_argb = 1,

        .min_width  = VAU_RESOLUTION_WIDTH_MIN,
        .min_height = VAU_RESOLUTION_HEIGHT_MIN,
        .min_stride = VAU_RESOLUTION_STRIDE_MIN,
        .max_width  = VAU_RESOLUTION_WIDTH_MAX,
        .max_height = VAU_RESOLUTION_HEIGHT_MAX,
        .max_stride = VAU_RESOLUTION_STRIDE_MAX,

        .support_draw_attr_list = g_vau_src2_draw_capability_list,
        .support_draw_attr_list_size = sizeof(g_vau_src2_draw_capability_list) / \
                                       sizeof(g_vau_src2_draw_capability_list[0]),
    },
    /* dst */
    {
        .support_fmts.bits.support_fmt_argb8888 = 1,
        .support_fmts.bits.support_fmt_krgb8888 = 1,
        .support_fmts.bits.support_fmt_argb4444 = 1,
        .support_fmts.bits.support_fmt_argb1555 = 1,
        .support_fmts.bits.support_fmt_argb8565 = 1,
        .support_fmts.bits.support_fmt_rgb888   = 1,
        .support_fmts.bits.support_fmt_rgb444   = 1,
        .support_fmts.bits.support_fmt_rgb555   = 1,
        .support_fmts.bits.support_fmt_rgb565   = 1,

        .min_width  = VAU_RESOLUTION_WIDTH_MIN,
        .min_height = VAU_RESOLUTION_HEIGHT_MIN,
        .min_stride = VAU_RESOLUTION_STRIDE_MIN,
        .max_width  = VAU_RESOLUTION_WIDTH_MAX,
        .max_height = VAU_RESOLUTION_HEIGHT_MAX,
        .max_stride = VAU_RESOLUTION_STRIDE_MAX,

        .support_draw_attr_list = g_vau_dst_draw_capability_list,
        .support_draw_attr_list_size = sizeof(g_vau_dst_draw_capability_list) / \
                                       sizeof(g_vau_dst_draw_capability_list[0]),
    }
};

/*-------------------------------- func release -------------------------------------------*/

td_s32 vau_hal_check_src1(const drv_vau_surface *surface, td_bool is_dma)
{
    td_s32 ret = vau_check_surface(surface, VAU_LYAER_SRC1, is_dma);
    if (ret != EXT_SUCCESS) {
        vau_err("vau_check_surface SRC1 failed!");
        return ret;
    }
    return EXT_SUCCESS;
}

td_s32 vau_hal_check_src2(const drv_vau_surface *surface)
{
    td_s32 ret = vau_check_surface(surface, VAU_LYAER_SRC2, TD_FALSE);
    if (ret != EXT_SUCCESS) {
        vau_err("vau_check_surface SRC2 failed!");
        return ret;
    }
    return EXT_SUCCESS;
}

td_s32 vau_hal_check_dst(const drv_vau_surface *surface)
{
    td_s32 ret = vau_check_surface(surface, VAU_LYAER_DST, TD_FALSE);
    if (ret != EXT_SUCCESS) {
        vau_err("vau_check_surface DST failed!");
        return ret;
    }
    return EXT_SUCCESS;
}

static td_s32 vau_check_surface(const drv_vau_surface *surface, vau_layer_index index, td_bool is_dma)
{
    td_s32 ret;

    if (surface->attr.vector.enable) {
        return EXT_SUCCESS;
    }

    ret = vau_check_resolution(surface, index, is_dma);
    if (ret != EXT_SUCCESS) {
        return ret;
    }

    ret = vau_check_stride(surface);
    if (ret != EXT_SUCCESS) {
        return ret;
    }

    ret = vau_check_fmt(surface, index, is_dma);
    if (ret != EXT_SUCCESS) {
        return ret;
    }

    ret = vau_check_addr(surface);
    if (ret != EXT_SUCCESS) {
        return ret;
    }

    ret = vau_check_draw_attr(surface, index, is_dma);
    if (ret != EXT_SUCCESS) {
        return ret;
    }

    return EXT_SUCCESS;
}

static td_s32 vau_check_resolution(const drv_vau_surface *surface, vau_layer_index index, td_bool is_dma)
{
    if ((surface->width < g_vau_capability[index].min_width) ||
        (surface->width > g_vau_capability[index].max_width) ||
        (surface->height < g_vau_capability[index].min_height) ||
        (surface->height > g_vau_capability[index].max_height) ||
        (surface->stride < g_vau_capability[index].min_stride) ||
        (surface->stride > g_vau_capability[index].max_stride)) {
        vau_err("invalid resolution: width[%u], height[%u], stride[%u]!",
                surface->width, surface->height, surface->stride);
        return EXT_FAILURE;
    }

    td_s32 rect_x, rect_y, rect_w, rect_h;
    rect_x = surface->rect.x;
    rect_y = surface->rect.y;
    rect_w = (td_s32)surface->rect.width;  /* for cmpare u32 to s32 */
    rect_h = (td_s32)surface->rect.height; /* for cmpare u32 to s32 */

    td_s32 surface_w, surface_h;
    surface_w = (td_s32)surface->width;    /* for cmpare u32 to s32 */
    surface_h = (td_s32)surface->height;   /* for cmpare u32 to s32 */

    if ((is_dma == TD_TRUE) || (index == VAU_LYAER_SRC2)) {
        if ((rect_x < 0) || (rect_y < 0)) {
            vau_err("invalid input rect, src[%d], rect[%d,%d,%u,%u]!", index,
                    surface->rect.x, surface->rect.y, surface->rect.width, surface->rect.height);
            return EXT_FAILURE;
        }
    }

    if (index == VAU_LYAER_DST) {
        if ((rect_x + rect_w <= 0) || (rect_y + rect_h <= 0)) {
            vau_err("invalid dst rect, rect[%d,%d,%u,%u]!",
                    surface->rect.x, surface->rect.y, surface->rect.width, surface->rect.height);
            return EXT_FAILURE;
        }
    }

    if ((rect_x >= surface_w) || (rect_y >= surface_h) ||
        (rect_w == 0) || (rect_h == 0) || (rect_w > surface_w) || (rect_h > surface_h)) {
        vau_err("invalid input rect, index[%d], rect[%d,%d,%u,%u]!", index,
                surface->rect.x, surface->rect.y, surface->rect.width, surface->rect.height);
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

static td_s32 vau_check_stride(const drv_vau_surface *surface)
{
    if (surface->compress_info.compress_mode == DRV_GFX_COMPRESS_NONE) {
        return EXT_SUCCESS;
    }

    if (uapi_is_byte_align(surface->stride, 8) != TD_TRUE) {  /* 8: bytes align */
        vau_err("cmp mode[%u] stride[%u] should align to 8!",
                (td_u32)surface->compress_info.compress_mode, surface->stride);
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

static td_s32 vau_check_fmt(const drv_vau_surface *surface, vau_layer_index index, td_bool is_dma)
{
    td_u32 check_bits;

    if (((g_vau_capability[index].support_fmts.all_bits >> (td_u32)surface->fmt) & 0x1) != 0x1) {
        vau_err("surface fmt [%d] is not support", surface->fmt);
        return EXT_FAILURE;
    }

    if ((surface->is_color_surface) &&
        ((surface->fmt != DRV_GFX_FMT_ARGB8888) && (surface->fmt != DRV_GFX_FMT_RGB888))) {
        vau_err("src[%d] fill color fmt must be argb8888/rgb888, cur_fmt=%d", index, surface->fmt);
        return EXT_FAILURE;
    }

    check_bits = g_vau_capability[index].support_fmts_dma.all_bits;
    if ((is_dma == TD_TRUE) && (((check_bits >> (td_u32)surface->fmt) & 0x1) != 0x1)) {
        vau_err("surface[%d] fmt [%d] for dma is not support", ((td_s32)index + 1), surface->fmt);
        return EXT_FAILURE;
    }

    /* src1 not support matrix */
    check_bits = g_vau_capability[index].support_fmts_affine.all_bits;
    if ((surface->attr.affine.affine_en == TD_TRUE) && (((check_bits >> (td_u32)surface->fmt) & 0x1) != 0x1)) {
        vau_err("surface[%d] fmt [%d] for matrix is not support", ((td_s32)index + 1), surface->fmt);
        return EXT_FAILURE;
    }

    if ((surface->compress_info.compress_mode > DRV_GFX_COMPRESS_NONE) &&
        (surface->compress_info.compress_mode < DRV_GFX_COMPRESS_MAX)) {
        check_bits = g_vau_capability[index].support_fmts_cmp.all_bits;
        if (((check_bits >> (td_u32)surface->fmt) & 0x1) != 0x1) {
            vau_err("invalid cmp fmt: 0x%x", surface->fmt);
            return EXT_FAILURE;
        }

        check_bits = g_vau_capability[index].support_fmt_orders_cmp.all_bits;
        if (((check_bits >> (td_u32)surface->argb_order) & 0x1) != 0x1) {
            vau_err("invalid cmp fmt order: 0x%x", surface->argb_order);
            return EXT_FAILURE;
        }
    }

    return EXT_SUCCESS;
}

static td_s32 vau_check_addr(const drv_vau_surface *surface)
{
    if (surface->is_color_surface == TD_TRUE) { /* fill color no need src addr */
        return EXT_SUCCESS;
    }

    if (surface->addr == 0x0) {
        vau_err("surface addr is zero!");
        return EXT_FAILURE;
    }

    if (uapi_is_byte_align(surface->addr, 16) == TD_FALSE) { /* 16bytes align */
        vau_err("surface addr is not 16 byte align!");
        return EXT_FAILURE;
    }

    if ((surface->fmt >= DRV_GFX_FMT_CLUT1) && (surface->fmt <= DRV_GFX_FMT_CLUT4B)) {
        if (surface->clut_addr == 0x0) {
            vau_err("clut addr is zero!");
            return EXT_FAILURE;
        }

        if (uapi_is_byte_align(surface->clut_addr, 16) == TD_FALSE) { /* 16 bytes align */
            vau_err("clut addr is not 16 byte align!");
            return EXT_FAILURE;
        }
    }

    return EXT_SUCCESS;
}

static td_s32 vau_check_draw_attr(const drv_vau_surface *surface, vau_layer_index index, td_bool is_dma)
{
    td_u32 i;
    vau_draw_capability draw_attr = { 0 };

    draw_attr.bits.crop   = surface->attr.crop.crop_en;
    draw_attr.bits.trans  = surface->attr.trans.enable;
    draw_attr.bits.affine = surface->attr.affine.affine_en;
    draw_attr.bits.dma    = is_dma;

    for (i = 0; i < g_vau_capability[index].support_draw_attr_list_size; i++) {
        if (draw_attr.u32 == g_vau_capability[index].support_draw_attr_list[i].u32) {
            return EXT_SUCCESS;
        }
    }

    vau_err("unsupported draw attr: 0x%x", draw_attr.u32);
    return EXT_FAILURE;
}

td_s32 vau_hal_check_blit_opt(const drv_vau_blit_opt *opt)
{
    vau_blit_capability blit_opt = {0};

    if (opt == TD_NULL) {
        return EXT_SUCCESS;
    }

    blit_opt.bits.rop       = opt->rop_opt.rop_enable;
    blit_opt.bits.color_key = opt->colorkey_opt.colorkey_enable;
    blit_opt.bits.blend     = opt->blend_opt.blend_enable;

    td_u32 list_size = (td_u32)(sizeof(g_vau_blit_capability_list) / sizeof(g_vau_blit_capability_list[0]));
    for (td_u32 i = 0; i < list_size; i++) {
        if (blit_opt.u32 == g_vau_blit_capability_list[i].u32) {
            return EXT_SUCCESS;
        }
    }
    vau_err("unsupported blit opt: 0x%x", blit_opt.u32);
    return EXT_FAILURE;
}

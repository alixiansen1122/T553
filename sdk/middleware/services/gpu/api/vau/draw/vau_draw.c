/*
 * Copyright (c) @CompanyNameMagicTag 2021-2022. All rights reserved.
 * Description: direct draw
 */

#include "vau_draw.h"
#include "vau_arc.h"
#include "vau_debug.h"
#include "vau_mem.h"
#include "vau_hal_type.h"
#include "vau_node.h"
#include "vau_hal_vector_draw.h"
#include "vau_stroke_dash.h"
#include "vau_stroke_path.h"

#define VAU_DOUBLE_SURFACE 2

typedef struct {
    ext_gfx_fmt api_fmt;
    hal_vau_fmt hal_fmt;
} vau_fmt_mapping;

typedef struct {
    ext_gfx_argb_order api_order;
    hal_vau_argb_order hal_order;
} vau_fmt_order_mapping;

static vau_fmt_mapping g_vau_fmt_mapping_api2hal[] = {
    { EXT_GFX_FMT_ARGB8888, HAL_VAU_FMT_ARGB8888 },
    { EXT_GFX_FMT_KRGB8888, HAL_VAU_FMT_KRGB8888 },
    { EXT_GFX_FMT_ARGB4444, HAL_VAU_FMT_ARGB4444 },
    { EXT_GFX_FMT_ARGB1555, HAL_VAU_FMT_ARGB1555 },
    { EXT_GFX_FMT_ARGB8565, HAL_VAU_FMT_ARGB8565 },
    { EXT_GFX_FMT_RGB888, HAL_VAU_FMT_RGB888 },
    { EXT_GFX_FMT_RGB444, HAL_VAU_FMT_RGB444 },
    { EXT_GFX_FMT_RGB555, HAL_VAU_FMT_RGB555 },
    { EXT_GFX_FMT_RGB565, HAL_VAU_FMT_RGB565 },
    { EXT_GFX_FMT_A1, HAL_VAU_FMT_A1 },
    { EXT_GFX_FMT_A8, HAL_VAU_FMT_A8 },
    { EXT_GFX_FMT_A1B, HAL_VAU_FMT_A1B },
    { EXT_GFX_FMT_CLUT1, HAL_VAU_FMT_CLUT1 },
    { EXT_GFX_FMT_CLUT2, HAL_VAU_FMT_CLUT2 },
    { EXT_GFX_FMT_CLUT4, HAL_VAU_FMT_CLUT4 },
    { EXT_GFX_FMT_CLUT8, HAL_VAU_FMT_CLUT8 },
    { EXT_GFX_FMT_ACLUT44, HAL_VAU_FMT_ACLUT44 },
    { EXT_GFX_FMT_ACLUT88, HAL_VAU_FMT_ACLUT88 },
    { EXT_GFX_FMT_CLUT1B, HAL_VAU_FMT_CLUT1B },
    { EXT_GFX_FMT_CLUT2B, HAL_VAU_FMT_CLUT2B },
    { EXT_GFX_FMT_CLUT4B, HAL_VAU_FMT_CLUT4B },
    { EXT_GFX_FMT_A2, HAL_VAU_FMT_A2 },
    { EXT_GFX_FMT_A4, HAL_VAU_FMT_A4 },
    { EXT_GFX_FMT_A2B, HAL_VAU_FMT_A2B },
    { EXT_GFX_FMT_A4B, HAL_VAU_FMT_A4B },
};

static vau_fmt_order_mapping g_vau_fmt_order_mapping_api2hal[] = {
    { EXT_GFX_FMT_ORDER_ARGB, HAL_VAU_FMT_ORDER_ARGB },
    { EXT_GFX_FMT_ORDER_ARBG, HAL_VAU_FMT_ORDER_ARBG },
    { EXT_GFX_FMT_ORDER_AGRB, HAL_VAU_FMT_ORDER_AGRB },
    { EXT_GFX_FMT_ORDER_AGBR, HAL_VAU_FMT_ORDER_AGBR },
    { EXT_GFX_FMT_ORDER_ABRG, HAL_VAU_FMT_ORDER_ABRG },
    { EXT_GFX_FMT_ORDER_ABGR, HAL_VAU_FMT_ORDER_ABGR },

    { EXT_GFX_FMT_ORDER_RAGB, HAL_VAU_FMT_ORDER_RAGB },
    { EXT_GFX_FMT_ORDER_RABG, HAL_VAU_FMT_ORDER_RABG },
    { EXT_GFX_FMT_ORDER_RGAB, HAL_VAU_FMT_ORDER_RGAB },
    { EXT_GFX_FMT_ORDER_RGBA, HAL_VAU_FMT_ORDER_RGBA },
    { EXT_GFX_FMT_ORDER_RBAG, HAL_VAU_FMT_ORDER_RBAG },
    { EXT_GFX_FMT_ORDER_RBGA, HAL_VAU_FMT_ORDER_RBGA },

    { EXT_GFX_FMT_ORDER_GRAB, HAL_VAU_FMT_ORDER_GRAB },
    { EXT_GFX_FMT_ORDER_GRBA, HAL_VAU_FMT_ORDER_GRBA },
    { EXT_GFX_FMT_ORDER_GARB, HAL_VAU_FMT_ORDER_GARB },
    { EXT_GFX_FMT_ORDER_GABR, HAL_VAU_FMT_ORDER_GABR },
    { EXT_GFX_FMT_ORDER_GBRA, HAL_VAU_FMT_ORDER_GBRA },
    { EXT_GFX_FMT_ORDER_GBAR, HAL_VAU_FMT_ORDER_GBAR },

    { EXT_GFX_FMT_ORDER_BRGA, HAL_VAU_FMT_ORDER_BRGA },
    { EXT_GFX_FMT_ORDER_BRAG, HAL_VAU_FMT_ORDER_BRAG },
    { EXT_GFX_FMT_ORDER_BGRA, HAL_VAU_FMT_ORDER_BGRA },
    { EXT_GFX_FMT_ORDER_BGAR, HAL_VAU_FMT_ORDER_BGAR },
    { EXT_GFX_FMT_ORDER_BARG, HAL_VAU_FMT_ORDER_BARG },
    { EXT_GFX_FMT_ORDER_BAGR, HAL_VAU_FMT_ORDER_BAGR },
};

static hal_vau_blend_mode g_vau_src1_blend_cmd_mode[EXT_VAU_BLENDCMD_MAX] = {
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

static hal_vau_blend_mode g_vau_src2_blend_cmd_mode[EXT_VAU_BLENDCMD_MAX] = {
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

static td_s32 vau_draw_matrix_x(const ext_vau_matrix *m, td_s32 x, td_s32 y)
{
    return (td_s32)((td_float)x * m->matrix[0][0] + (td_float)y * m->matrix[0][1] + m->matrix[0][2]); /* 0, 2:index */
}
static td_s32 vau_draw_matrix_y(const ext_vau_matrix *m, td_s32 x, td_s32 y)
{
    return (td_s32)((td_float)x * m->matrix[1][0] + (td_float)y * m->matrix[1][1] + m->matrix[1][2]); /* 1, 2:index */
}

static hal_vau_fmt hal_trans_fmt_api2hal(ext_gfx_fmt fmt)
{
    td_u8 i;
    for (i = 0; i < sizeof(g_vau_fmt_mapping_api2hal) / sizeof(g_vau_fmt_mapping_api2hal[0]); i++) {
        if (g_vau_fmt_mapping_api2hal[i].api_fmt == fmt) {
            return g_vau_fmt_mapping_api2hal[i].hal_fmt;
        }
    }
    return HAL_VAU_FMT_ARGB8888;
}

static hal_vau_argb_order hal_trans_argb_order_api2hal(ext_gfx_argb_order order)
{
    td_u8 i;
    for (i = 0; i < sizeof(g_vau_fmt_order_mapping_api2hal) / sizeof(g_vau_fmt_order_mapping_api2hal[0]); i++) {
        if (g_vau_fmt_order_mapping_api2hal[i].api_order == order) {
            return g_vau_fmt_order_mapping_api2hal[i].hal_order;
        }
    }
    return HAL_VAU_FMT_ORDER_ARGB;
}

static td_void vau_draw_make_src1(const ext_vau_surface *surface, vau_hardware_node *node)
{
    if (surface == TD_NULL) {
        return;
    }
    node->vau_src1_ctrl.bits.src1_en = 1;
    node->vau_src1_ctrl.bits.src1_mode = surface->is_color_surface ? HAL_VAU_MODE_TYPE_REG : HAL_VAU_MODE_TYPE_MEM;
    node->vau_src1_fill.u32 = surface->color;
    node->vau_src1_ctrl.bits.src1_fmt = hal_trans_fmt_api2hal(surface->color_fmt);
    node->vau_src1_ctrl.bits.src1_argb_order = hal_trans_argb_order_api2hal(surface->argb_order);
    node->vau_src1_ctrl.bits.src1_rgb_exp = HAL_VAU_RGB_EXP_0;

    node->vau_src1_imgsize.bits.src1_width = surface->width - 1;
    node->vau_src1_imgsize.bits.src1_height = surface->height - 1;
    node->vau_src1_stride.bits.src1_stride = surface->stride;
    node->vau_src1_addr.u32 = surface->phy_addr;

    node->vau_src1_rect_xy.bits.src1_rect_x = (td_u16)surface->rect.x;
    node->vau_src1_rect_xy.bits.src1_rect_y = (td_u16)surface->rect.y;
    node->vau_src1_rect_wh.bits.src1_rect_width = surface->rect.width - 1;
    node->vau_src1_rect_wh.bits.src1_rect_height = surface->rect.height - 1;
    return;
}

static td_s32 vau_draw_make_path(const ext_vau_path *path, vau_hardware_node *node)
{
    if ((path->cmds == TD_NULL) || (path->datas == TD_NULL)) {
        vau_err("null pointer");
        return EXT_ERR_GFX_NULL_POINTER;
    }
    if (path->cmds[0] != VAU_PATH_MOVE_TO_ABS) {
        vau_err("path should start width move to abs, now:%u", path->cmds[0]);
        return EXT_ERR_GFX_INVALID_PARAM;
    }
    if (uapi_is_byte_align((td_u32)(uintptr_t)path->datas, 4) != TD_TRUE) { // need 4 byte align
        vau_err("path data addr align err");
        return EXT_ERR_GFX_INVALID_ADDR;
    }

    td_bool is_internal_buff = vau_mem_is_internal(VAU_MEM_TYPE_PATH_CMD, path->cmds);
    if (is_internal_buff == TD_TRUE) {
        vau_mem_ref_inc(VAU_MEM_TYPE_PATH_CMD, path->cmds);
        node->vau_draw_ctrl.bits.cmd_return_en  = 1;
    }
    is_internal_buff = vau_mem_is_internal(VAU_MEM_TYPE_PATH_DATA, (td_uchar *)path->datas);
    if (is_internal_buff == TD_TRUE) {
        vau_mem_ref_inc(VAU_MEM_TYPE_PATH_DATA, (td_uchar *)path->datas);
        node->vau_draw_ctrl.bits.path_return_en  = 1;
    }
    node->vau_draw_cmd_addr.u32 = (td_u32)(uintptr_t)path->cmds;
    node->vau_draw_path_addr.u32 = (td_u32)(uintptr_t)path->datas;
    node->vau_draw_path_num.bits.data_num = path->data_num;
    node->vau_draw_path_num.bits.cmd_num = path->cmd_num;

    vau_hal_set_et_buffer(node);

    node->vau_draw_ctrl.bits.draw_en = 1;
    return EXT_SUCCESS;
}

static td_void vau_draw_make_matrix(const ext_vau_matrix *m, vau_hardware_node *node)
{
    node->vau_draw_matrix_00.bits.matrix_00 = m->matrix[0][0];
    node->vau_draw_matrix_01.bits.matrix_01 = m->matrix[0][1];
    node->vau_draw_matrix_02.bits.matrix_02 = m->matrix[0][2]; /* 0: row, 2: col */
    node->vau_draw_matrix_10.bits.matrix_10 = m->matrix[1][0];
    node->vau_draw_matrix_11.bits.matrix_11 = m->matrix[1][1];
    node->vau_draw_matrix_12.bits.matrix_12 = m->matrix[1][2]; /* 1: row, 2: col */
}

static td_s32 vau_draw_make_gradient_stops(const ext_vau_paint_attr *paint, u_vau_mix_node *g)
{
    if (paint->gradient.stops.num < 2) { /* 2: at least 2 points: start and end */
        vau_err("invalid offset num: %u", paint->gradient.stops.num);
        return EXT_ERR_GFX_INVALID_PARAM;
    }
    for (int i = 0; i < paint->gradient.stops.num; i++) {
        if ((paint->gradient.stops.offsets[i] < 0) || (paint->gradient.stops.offsets[i] > 1)) {
            vau_err("invalid offset at %d: %f", i, paint->gradient.stops.offsets[i]);
            return EXT_ERR_GFX_INVALID_PARAM;
        }
        if (i < 4) { /* 4 offsets in offset0 */
            g->gradient.vau_gradient_stops_offset0.u32 |=
                // 8: 8 bits per offset
                (td_u32)(paint->gradient.stops.offsets[i] * HAL_VAU_GRADIENT_STOP_RATIO) << (td_u32)(i * 8);
        } else {
            g->gradient.vau_gradient_stops_offset1.u32 |=
                // 8: 8 bits per offset; 4: index to [0, 3]
                (td_u32)(paint->gradient.stops.offsets[i] * HAL_VAU_GRADIENT_STOP_RATIO) << (td_u32)((i - 4) * 8);
        }
    }

    for (td_s32 i = 0; i < paint->gradient.stops.num - 1; i++) {
        if (paint->gradient.stops.offsets[i + 1] > paint->gradient.stops.offsets[i]) {
            g->gradient.vau_gradient_color_reci[i].bits.color_reci01 =
                (1.0 / ((paint->gradient.stops.offsets[i + 1] - paint->gradient.stops.offsets[i]) *
                    HAL_VAU_GRADIENT_AUXILIARY));
        } else {
            vau_err("offset need to be incremental at %d: %f, %d: %f", i, paint->gradient.stops.offsets[i],
                i + 1, paint->gradient.stops.offsets[i + 1]);
            return EXT_ERR_GFX_INVALID_PARAM;
        }
        g->gradient.vau_gradient_stops_color[i].bits.stops_color0 = paint->gradient.stops.colors[i];
    }
    g->gradient.vau_gradient_stops_color[paint->gradient.stops.num - 1].bits.stops_color0 =
        paint->gradient.stops.colors[paint->gradient.stops.num - 1];
    g->gradient.vau_gradient_stops_num.bits.stops_num = paint->gradient.stops.num;

    if (paint->gradient.tile_mode == EXT_VAU_PAINT_TILE_MODE_CLAMP) {
        g->gradient.vau_gradient_tile_mode.bits.tile_mode = HAL_VAU_PAINT_TILE_MODE_CLAMP;
    } else if (paint->gradient.tile_mode == EXT_VAU_PAINT_TILE_MODE_REPEAT) {
        g->gradient.vau_gradient_tile_mode.bits.tile_mode = HAL_VAU_PAINT_TILE_MODE_REPEAT;
    } else if (paint->gradient.tile_mode == EXT_VAU_PAINT_TILE_MODE_MIRROR) {
        g->gradient.vau_gradient_tile_mode.bits.tile_mode = HAL_VAU_PAINT_TILE_MODE_MIRROR;
    }
    return EXT_SUCCESS;
}

static td_void vau_draw_make_gadient_linear(const ext_vau_linear_gradient_attr *linear,
    const ext_vau_matrix *m, vau_hardware_node *node)
{
    ext_vau_point start_point = { vau_draw_matrix_x(m, linear->start_point.x, linear->start_point.y),
        vau_draw_matrix_y(m, linear->start_point.x, linear->start_point.y) };
    ext_vau_point end_point = { vau_draw_matrix_x(m, linear->end_point.x, linear->end_point.y),
        vau_draw_matrix_y(m, linear->end_point.x, linear->end_point.y) };

    node->vau_draw_ctrl.bits.fill_effect = HAL_VAU_FILL_EFFECT_LINEAR_GRADIENT;
    node->mix.gradient.vau_gradient_linear_start.bits.linear_start_x = (td_s16)start_point.x;
    node->mix.gradient.vau_gradient_linear_start.bits.linear_start_y = (td_s16)start_point.y;
    td_s32 dx = end_point.x - start_point.x;
    td_s32 dy = end_point.y - start_point.y;
    td_s32 usq = dx * dx + dy * dy;
    if ((dx == 0) && (dy == 0)) {
        node->mix.gradient.vau_gradient_const_mode.bits.linear_const_mode = 0;
        node->mix.gradient.vau_gradient_linear_const_x.bits.linear_const_x = 0;
        node->mix.gradient.vau_gradient_linear_const_y.bits.linear_const_y = 0;
    } else if ((dx == 0) && (dy != 0)) {
        node->mix.gradient.vau_gradient_const_mode.bits.linear_const_mode = 1; // 1: horizon
        node->mix.gradient.vau_gradient_linear_const_x.bits.linear_const_x = 0;
        node->mix.gradient.vau_gradient_linear_const_y.bits.linear_const_y =
            HAL_VAU_GRADIENT_AUXILIARY * (td_float)dy / usq;
    } else if ((dx != 0) && (dy == 0)) {
        node->mix.gradient.vau_gradient_const_mode.bits.linear_const_mode = 2; // 2: vertical
        node->mix.gradient.vau_gradient_linear_const_x.bits.linear_const_x =
            HAL_VAU_GRADIENT_AUXILIARY * (td_float)dx / usq;
        node->mix.gradient.vau_gradient_linear_const_y.bits.linear_const_y = 0;
    } else {
        node->mix.gradient.vau_gradient_const_mode.bits.linear_const_mode = 3; // 3: slash
        node->mix.gradient.vau_gradient_linear_const_x.bits.linear_const_x =
            HAL_VAU_GRADIENT_AUXILIARY * (td_float)dx / usq;
        node->mix.gradient.vau_gradient_linear_const_y.bits.linear_const_y =
            HAL_VAU_GRADIENT_AUXILIARY * (td_float)dy / usq;
    }
}

static td_void vau_draw_make_gadient_radial(const ext_vau_radial_gradient_attr *radial,
    const ext_vau_matrix *m, vau_hardware_node *node)
{
    node->vau_draw_ctrl.bits.fill_effect = HAL_VAU_FILL_EFFECT_RADIAL_GRADIENT;
    node->mix.gradient.vau_gradient_radial_center.bits.radial_center_x =
        (td_u32)vau_draw_matrix_x(m, radial->center.x, radial->center.y);
    node->mix.gradient.vau_gradient_radial_center.bits.radial_center_y =
        (td_u32)vau_draw_matrix_y(m, radial->center.x, radial->center.y);
    node->mix.gradient.vau_gradient_radial_const_r.bits.radial_const_r =
        HAL_VAU_GRADIENT_AUXILIARY / (td_float)radial->radius;
}

static td_void vau_draw_make_gadient_sweep(const ext_vau_sweep_gradient_attr *sweep,
    const ext_vau_matrix *m, vau_hardware_node *node)
{
    node->vau_draw_ctrl.bits.fill_effect = HAL_VAU_FILL_EFFECT_SWEEP_GRADIENT;
    node->mix.gradient.vau_gradient_sweep_center.bits.sweep_center_x =
        (td_u32)vau_draw_matrix_x(m, sweep->center.x, sweep->center.y);
    node->mix.gradient.vau_gradient_sweep_center.bits.sweep_center_y =
        (td_u32)vau_draw_matrix_y(m, sweep->center.x, sweep->center.y);
    node->mix.gradient.vau_gradient_sweep_start_angle.bits.sweep_start_angle = sweep->start_angle;
}

static td_s32 vau_draw_make_paint(const ext_vau_paint_attr *paint, ext_vau_fill_type fill_rule,
    ext_vau_render_quality render_quality, const ext_vau_matrix *m, vau_hardware_node *node)
{
    node->vau_draw_ctrl.bits.msaa_mode = (render_quality == EXT_VAU_RENDER_QUALITY_NONE_AA) ?
        HAL_VAU_MSAA_MODE_NONE : HAL_VAU_MSAA_MODE_8X_CFG;
    node->vau_draw_ctrl.bits.fill_mode = (fill_rule == EXT_VAU_EVEN_ODD_FILL) ?
        HAL_VAU_FILL_MODE_EVEN_ODD : HAL_VAU_FILL_MODE_NONE_ZERO;
    switch (paint->paint_type) {
        case EXT_VAU_PAINT_TYPE_COLOR:
            node->vau_draw_ctrl.bits.fill_effect = HAL_VAU_FILL_EFFECT_PURE_COLOR;
            node->mix.gradient.vau_gradient_typical_color.u32 = paint->solid.color;
            return EXT_SUCCESS;
        case EXT_VAU_PAINT_TYPE_COLOR_ALPHA_FF:
            node->vau_draw_ctrl.bits.fill_effect = HAL_VAU_FILL_EFFECT_ALPHA_FF;
            node->mix.gradient.vau_gradient_typical_color.u32 = paint->solid.color;
            return EXT_SUCCESS;
        case EXT_VAU_PAINT_TYPE_LINEAR_GRADIENT:
            vau_draw_make_gadient_linear(&paint->gradient.linear, m, node);
            break;
        case EXT_VAU_PAINT_TYPE_RADIAL_GRADIENT:
            if (paint->gradient.radial.radius == 0) {
                vau_err("radial radius should not be 0");
                return EXT_ERR_GFX_INVALID_PARAM;
            }
            vau_draw_make_gadient_radial(&paint->gradient.radial, m, node);
            break;
        case EXT_VAU_PAINT_TYPE_SWEEP_GRADIENT:
            vau_draw_make_gadient_sweep(&paint->gradient.sweep, m, node);
            break;
        default:
            vau_err("invalid paint_type:0x%x", paint->paint_type);
            return EXT_ERR_GFX_INVALID_PARAM;
    }
    return vau_draw_make_gradient_stops(paint, &node->mix);
}

static td_void vau_draw_make_dst(const ext_vau_surface *surface, vau_hardware_node *node)
{
    node->vau_des_ctrl.bits.des_en = 1;
    node->vau_des_ctrl.bits.des_fmt = hal_trans_fmt_api2hal(surface->color_fmt);
    node->vau_des_ctrl.bits.des_argb_order = hal_trans_argb_order_api2hal(surface->argb_order);
    node->vau_des_imgsize.bits.des_width = surface->width - 1;
    node->vau_des_imgsize.bits.des_height = surface->height - 1;
    node->vau_des_stride.bits.des_stride = surface->stride;
    node->vau_des_addr.u32 = surface->phy_addr;

    node->vau_des_rect_xy.bits.des_rect_x = (td_u32)surface->rect.x;
    node->vau_des_rect_xy.bits.des_rect_y = (td_u32)surface->rect.y;
    node->vau_des_rect_wh.bits.des_rect_width = surface->rect.width - 1;
    node->vau_des_rect_wh.bits.des_rect_height = surface->rect.height - 1;

    node->vau_des_alpha.bits.des_alpha_thd = 0x7F;

    node->vau_draw_ctrl.bits.dyn_bdbx_en = 1;
    node->vau_draw_ctrl.bits.bdbx_adj_en = 1;
    node->vau_draw_st.bits.draw_x_start = (td_u32)surface->rect.x;
    node->vau_draw_st.bits.draw_y_start = (td_u32)surface->rect.y;
    node->vau_draw_end.bits.draw_x_end = surface->rect.x + surface->rect.width - 1;
    node->vau_draw_end.bits.draw_y_end = surface->rect.y + surface->rect.height - 1;
}

static td_void vau_draw_make_cbm(const ext_vau_surface_list *surface_list, vau_hardware_node *node)
{
    node->vau_cbm_ctrl.bits.cbm_en = 1;
    if (surface_list->src_surface_cnt == 1 || surface_list->opt == TD_NULL) {
        node->vau_cbm_ctrl.bits.alu_mode = DRV_VAU_ALU_MODE_SRC2_BYPASS;
        return;
    }
    node->vau_cbm_ctrl.bits.alu_mode = DRV_VAU_ALU_MODE_BLEND;

    node->vau_cbm_src1_cbm_para.bits.s1_premult_en = surface_list->src_surface[0].alpha_info.premult_en;
    node->vau_cbm_src1_cbm_para.bits.s1_palpha_en = surface_list->src_surface[0].alpha_info.pixel_alpha_en;
    node->vau_cbm_src2_cbm_para.bits.s2_premult_en = surface_list->src_surface[1].alpha_info.premult_en;
    node->vau_cbm_src2_cbm_para.bits.s2_palpha_en = surface_list->src_surface[1].alpha_info.pixel_alpha_en;

    hal_vau_blend_mode src1, src2;
    if (surface_list->opt->blend_opt.blend_cmd == EXT_VAU_BLENDCMD_CONFIG) {
        src1 = (hal_vau_blend_mode)surface_list->opt->blend_opt.background_blend_mode;
        src2 = (hal_vau_blend_mode)surface_list->opt->blend_opt.foreground_blend_mode;
    } else {
        src1 = g_vau_src1_blend_cmd_mode[(td_u32)surface_list->opt->blend_opt.blend_cmd % EXT_VAU_BLENDCMD_MAX];
        src2 = g_vau_src2_blend_cmd_mode[(td_u32)surface_list->opt->blend_opt.blend_cmd % EXT_VAU_BLENDCMD_MAX];
    }
    node->vau_cbm_src1_cbm_para.bits.s1_blend_mode = src1;
    node->vau_cbm_src2_cbm_para.bits.s2_blend_mode = src2;

    node->vau_cbm_alu_para.bits.alpha_from =
        (hal_vau_blend_alpha_mode)surface_list->opt->blend_opt.out_alpha_mode;
}

static td_s32 vau_draw_hardware(td_handle handle, const ext_vau_surface_list *surface_list,
    const ext_vau_path *path, const ext_vau_paint_attr *paint, ext_vau_fill_type type)
{
    td_s32 ret;
    ext_vau_surface *src1 = (surface_list->src_surface_cnt == VAU_DOUBLE_SURFACE) ?
        &surface_list->src_surface[0] : TD_NULL;
    ext_vau_surface *src2 = (surface_list->src_surface_cnt == VAU_DOUBLE_SURFACE) ?
        &surface_list->src_surface[1] : &surface_list->src_surface[0];
    ext_vau_matrix identity = {1, 0, 0, 0, 1, 0, 0, 0, 1};
    ext_vau_matrix *m = (src2->matrix == TD_NULL) ? &identity : src2->matrix;

    if (path->cmd_num == 0 || path->data_num == 0) {
        return EXT_SUCCESS;
    }
    vau_sw_node *sw_node = vau_node_create();
    if (sw_node == TD_NULL) {
        vau_err("vau_node_create failed");
        return DRV_ERR_VAU_NO_MEM;
    }
    ret = vau_draw_make_path(path, &sw_node->hw_node);
    if (ret != EXT_SUCCESS) {
        vau_node_free(sw_node);
        vau_err("vau_draw_make_path failed");
        return ret;
    }
    ret = vau_draw_make_paint(paint, type, src2->draw->paint->render_cfg.render_quality, m, &sw_node->hw_node);
    if (ret != EXT_SUCCESS) {
        vau_node_free(sw_node);
        vau_err("vau_draw_make_paint failed");
        return ret;
    }

    vau_draw_make_matrix(m, &sw_node->hw_node);
    vau_draw_make_src1(src1, &sw_node->hw_node);
    vau_draw_make_dst(surface_list->dst_surface, &sw_node->hw_node);
    vau_draw_make_cbm(surface_list, &sw_node->hw_node);
    ret = vau_node_add(handle, sw_node, TD_FALSE);
    if (ret != EXT_SUCCESS) {
        vau_node_free(sw_node);
        vau_err("vau_node_add failed");
        return ret;
    }
    return EXT_SUCCESS;
}

static td_s32 vau_draw_fill_path(td_handle handle, const ext_vau_surface_list *surface_list, const ext_vau_path *path)
{
    ext_vau_surface *src2 = (surface_list->src_surface_cnt == VAU_DOUBLE_SURFACE) ?
        &surface_list->src_surface[1] : &surface_list->src_surface[0];
    if ((src2->draw == TD_NULL) || (src2->draw->paint == TD_NULL) || (src2->draw->paint->fill_attr == TD_NULL)) {
        vau_err("fill attr is null");
        return DRV_ERR_VAU_INVALID_PARA;
    }

    return vau_draw_hardware(handle, surface_list, path, &src2->draw->paint->fill_attr->paint_attr,
        src2->draw->paint->fill_attr->type);
}

static td_s32 vau_draw_stroke_path(td_handle handle, const ext_vau_surface_list *surface_list, const ext_vau_path *path)
{
    td_s32 ret;
    ext_vau_path dash_path = { 0 };
    ext_vau_path fill_path = { 0 };
    ext_vau_fill_attr fill_attr = {0};
    ext_vau_surface *src2 = (surface_list->src_surface_cnt == VAU_DOUBLE_SURFACE) ?
        &surface_list->src_surface[1] : &surface_list->src_surface[0];
    if (src2->draw->paint->stroke_attr == TD_NULL) {
        vau_err("stroke attr is null");
        return DRV_ERR_VAU_INVALID_PARA;
    }

    const ext_vau_path *to_fill_path = path;

    if (src2->draw->paint->stroke_attr->dash.enable) {
        ret = vau_create_stroke_dash_path(path, src2->draw->paint->stroke_attr, &dash_path);
        if (ret != EXT_SUCCESS) {
            vau_err("call vau_create_stroke_dash_path failed\n");
            return ret;
        }
        to_fill_path = &dash_path;
    }

    ext_vau_bezier_stroke_split_mode mode =
        (src2->draw->paint->render_cfg.render_quality == EXT_VAU_RENDER_QUALITY_BETTER) ?
        EXT_BEZIER_MODE_LINE_AVG : EXT_BEZIER_MODE_BEZIER_4;

    ret = vau_create_stroke_to_fill_path(to_fill_path, src2->draw->paint->stroke_attr, &fill_path, &fill_attr, mode);
    if (ret != EXT_SUCCESS) {
        vau_err("call vau_create_stroke_to_fill_path failed\n");
        goto destroy_dash;
    }

    ret = vau_draw_hardware(handle, surface_list, &fill_path, &src2->draw->paint->stroke_attr->paint_attr,
        EXT_VAU_NON_ZERO_FILL);
    if (ret != EXT_SUCCESS) {
        vau_err("vau_draw_hardware failed");
    }
    vau_destroy_stroke_to_fill_path(&fill_path);
destroy_dash:
    if (src2->draw->paint->stroke_attr->dash.enable) {
        vau_destroy_stroke_dash_path(&dash_path);
    }
    return ret;
}

static td_bool vau_draw_has_arc(const ext_vau_path *path, ext_vau_paint_mode paint_mode)
{
    if (((unsigned int)paint_mode & EXT_VAU_PAINT_MODE_STROKE) != EXT_VAU_PAINT_MODE_STROKE) {
        return TD_FALSE; /* current only support stroke ecllipse arc */
    }

    for (td_u32 i = 0; i < path->cmd_num; i++) {
        if (path->cmds[i] >= VAU_PATH_SCCWARC_TO_ABS && path->cmds[i] <= VAU_PATH_LCWARC_TO_REL) {
            return TD_TRUE;
        }
    }

    return TD_FALSE;
}

td_s32 vau_draw_directly(td_handle handle, const ext_vau_surface_list *surface_list)
{
    td_s32 ret = EXT_SUCCESS;
    td_u8 index = (surface_list->src_surface_cnt == 1) ? 0 : 1;
    ext_vau_paint_mode paint_mode = surface_list->src_surface[index].draw->paint->paint_mode;
    const ext_vau_path *path = surface_list->src_surface[index].draw->path;
    ext_vau_path fit_path = { 0 };
    td_bool has_arc = vau_draw_has_arc(path, paint_mode);
    if (has_arc) {
        ret = vau_draw_create_arc_fit_path(path, &fit_path);
        if (ret != EXT_SUCCESS) {
            vau_err("fit arc path failed:0x%x", ret);
            return ret;
        }
        path = &fit_path;
    }

    if (((unsigned int)paint_mode & EXT_VAU_PAINT_MODE_FILL) == EXT_VAU_PAINT_MODE_FILL) {
        ret = vau_draw_fill_path(handle, surface_list, path);
        if (ret != EXT_SUCCESS) {
            vau_err("call vau_draw_fill_path failed");
            goto out;
        }
    }

    if (((unsigned int)paint_mode & EXT_VAU_PAINT_MODE_STROKE) == EXT_VAU_PAINT_MODE_STROKE) {
        ret = vau_draw_stroke_path(handle, surface_list, path);
        if (ret != EXT_SUCCESS) {
            vau_err("call vau_draw_stroke_path failed");
            goto out;
        }
    }
out:
    if (has_arc) {
        vau_draw_destroy_arc_fit_path(&fit_path);
    }
    return ret;
}

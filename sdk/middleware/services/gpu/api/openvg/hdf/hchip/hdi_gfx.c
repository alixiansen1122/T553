/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: hdi gfx source file
 * Author: @CompanyNameTag
 * Create: 2021-08-26
 */

#include "hdi_gfx.h"

#include <securec.h>
#include "VG/openvg.h"
#include "soc_vau_api.h"
#include "vau_debug.h"
#include "vg_paint.h"
#include "vg_context.h"
#include "vg_common.h"
#include "vau_stroke_dash.h"
#include "vau_stroke_path.h"

VGHandle vg_draw_create_handle(void)
{
    return (VGHandle)uapi_vau_create();
}

static ext_vau_blend_cmd convert_blend_cmd_vg_to_vau(VGBlendMode mode)
{
    switch (mode) {
        case VG_BLEND_SRC:
            return EXT_VAU_BLENDCMD_SRC;
        case VG_BLEND_SRC_OVER:
            return EXT_VAU_BLENDCMD_SRCOVER;
        case VG_BLEND_DST_OVER:
            return EXT_VAU_BLENDCMD_DSTOVER;
        case VG_BLEND_SRC_IN:
            return EXT_VAU_BLENDCMD_SRCIN;
        case VG_BLEND_DST_IN:
            return EXT_VAU_BLENDCMD_DSTIN;
        // need to do something
        default:
            return EXT_VAU_BLENDCMD_SRC;
    }
}

void set_blend(ext_vau_blend_opt *blend, VGBlendMode mode)
{
    blend->blend_enable = TD_TRUE;
    blend->blend_cmd = convert_blend_cmd_vg_to_vau(mode);
    blend->out_alpha_mode = EXT_VAU_OUTALPHA_FROM_NORM;
}

static ext_gfx_fmt convert_vg_fmt_to_gfx_fmt(VGImageFormat v)
{
    switch (v) {
        case VG_sARGB_8888_PRE:
        case VG_sARGB_8888:
        case VG_sABGR_8888_PRE:
        case VG_sABGR_8888:
            return EXT_GFX_FMT_ARGB8888;
        case VG_sRGB_565:
        case VG_sBGR_565:
            return EXT_GFX_FMT_RGB565;
        case VG_EXT_sRGB_888:
        case VG_EXT_sBGR_888:
            return EXT_GFX_FMT_RGB888;
        default:
            return EXT_GFX_FMT_ARGB8888;
    }
}

static ext_gfx_argb_order convert_vg_fmt_to_gfx_order(VGImageFormat v)
{
    switch (v) {
        case VG_sARGB_8888_PRE:
        case VG_sARGB_8888:
        case VG_sRGB_565:
        case VG_EXT_sRGB_888:
            return EXT_GFX_FMT_ORDER_ARGB;
        case VG_sABGR_8888_PRE:
        case VG_sABGR_8888:
        case VG_sBGR_565:
        case VG_EXT_sBGR_888:
            return EXT_GFX_FMT_ORDER_ABGR;
        default:
            return EXT_GFX_FMT_ORDER_ARGB;
    }
}

td_u32 convert_vg_color_to_gfx_color(const VGfloat *color)
{
    // rgba
    td_u32 r = (td_u32)(color[0] * 0xFF) & 0xFF;
    td_u32 g = (td_u32)(color[1] * 0xFF) & 0xFF;
    td_u32 b = (td_u32)(color[2] * 0xFF) & 0xFF;
    td_u32 a = (td_u32)(color[3] * 0xFF) & 0xFF;
    return (a << 24) | (r << 16) | (g << 8) | (b << 0); /* shift 24 16 8 bits for argb8888 */
}

static td_bool fmt_is_premulted(VGImageFormat v)
{
    switch (v) {
        case VG_sARGB_8888_PRE:
        case VG_sABGR_8888_PRE:
            return TD_TRUE;
        case VG_sARGB_8888:
        case VG_sABGR_8888:
            return TD_FALSE;
        default:
            return TD_FALSE;
    }
}

void convert_to_vau_alpha_info(ext_vau_alpha_info *info, VGImageFormat v)
{
    info->is_alpha_max_128 = TD_FALSE;

    info->alpha0 = 0;
    info->alpha1 = 0xFF;
    info->alpha_thd = 0xFF / 2; /* for output fmt 1555, 2 is one half */

    info->global_alpha = 0xFF;
    info->golbal_premult_en = TD_FALSE;
    info->global_alpha_en = TD_FALSE;

    info->premult_en = fmt_is_premulted(v) == TD_TRUE ? TD_FALSE : TD_TRUE;
    info->pixel_alpha_en = TD_TRUE;
}

void convert_vg_image_to_vau_surface(const vg_image *v, ext_vau_surface *s)
{
    const vg_image *real = (v->parent != NULL) ? v->parent : v;
    // parent info
    s->width = real->width;
    s->height = real->height;
    s->stride = real->stride;
    s->phy_addr = real->phy_addr;
    s->vir_addr = real->vir_addr;
    s->clut_addr = real->clut_addr;
    s->color_fmt = convert_vg_fmt_to_gfx_fmt(real->fmt);
    s->argb_order = convert_vg_fmt_to_gfx_order(real->fmt);
    s->is_color_surface = real->buffer_type == VG_IMAGE_EXT_TYPE_COLOR;
    s->color = convert_vg_color_to_gfx_color(real->color);
    s->compress_mode = real->compress_mode;
    convert_to_vau_alpha_info(&s->alpha_info, real->fmt);

    // self info
    s->rect.x = (td_s32)v->offset_x;
    s->rect.y = (td_s32)v->offset_y;
    s->rect.width = v->width;
    s->rect.height = v->height;
}

static void convert_vg_path_to_vau_path(const vg_path *path, ext_vau_path *vau_path)
{
    vau_path->cmds = path->segments;
    vau_path->cmd_num = path->segment_num;
    vau_path->datas = path->datas;
    vau_path->data_num = path->data_num;
    return;
}

static td_s32 convert_vg_line_cap_to_vau(ext_vau_stroke_attr *stroke, VGCapStyle stroke_cap_style)
{
    switch (stroke_cap_style) {
        case VG_CAP_BUTT:
            stroke->cap = EXT_VAU_STROKE_LINE_CAP_BUTT;
            break;
        case VG_CAP_ROUND:
            stroke->cap = EXT_VAU_STROKE_LINE_CAP_ROUND;
            break;
        case VG_CAP_SQUARE:
            stroke->cap = EXT_VAU_STROKE_LINE_CAP_SQUARE;
            break;
        default:
            vg_err("invalid stroke cap:0x%x", stroke_cap_style);
            return EXT_FAILURE;
    }
    return EXT_SUCCESS;
}

static td_s32 convert_vg_line_join_to_vau(ext_vau_stroke_attr *stroke, VGJoinStyle stroke_join_style)
{
    switch (stroke_join_style) {
        case VG_JOIN_MITER:
            stroke->join = EXT_VAU_STROKE_LINE_JOIN_MITER;
            break;
        case VG_JOIN_ROUND:
            stroke->join = EXT_VAU_STROKE_LINE_JOIN_ROUND;
            break;
        case VG_JOIN_BEVEL:
            stroke->join = EXT_VAU_STROKE_LINE_JOIN_BEVEL;
            break;
        default:
            vg_err("invalid stroke join:0x%x", stroke_join_style);
            return EXT_FAILURE;
    }
    return EXT_SUCCESS;
}

static td_s32 convert_vg_dash_to_vau(ext_vau_stroke_attr *stroke, vg_context *ctx)
{
    if (ctx->stroke_dash_pattern_num != 0 && ctx->stroke_dash_pattern != NULL) {
        stroke->dash.enable = TD_TRUE;
        stroke->dash.array = ctx->stroke_dash_pattern;
        stroke->dash.count = ctx->stroke_dash_pattern_num;
        stroke->dash.offset = ctx->stroke_dash_phase;
        stroke->dash.reset = ctx->stroke_dash_phase_reset;
    } else {
        stroke->dash.enable = TD_FALSE;
    }
    return EXT_SUCCESS;
}

static td_s32 covert_vg_tilemode_to_vau(ext_vau_paint_tile_mode *tile_mode,
                                        VGColorRampSpreadMode colro_ramp_spread_mode)
{
    switch (colro_ramp_spread_mode) {
        case VG_COLOR_RAMP_SPREAD_PAD:
            *tile_mode = EXT_VAU_PAINT_TILE_MODE_CLAMP;
            break;
        case VG_COLOR_RAMP_SPREAD_REPEAT:
            *tile_mode = EXT_VAU_PAINT_TILE_MODE_REPEAT;
            break;
        case VG_COLOR_RAMP_SPREAD_REFLECT:
            *tile_mode = EXT_VAU_PAINT_TILE_MODE_MIRROR;
            break;
        default:
            vg_err("invalid tile mode:0x%x", colro_ramp_spread_mode);
            return EXT_FAILURE;
    }
    return EXT_SUCCESS;
}

static td_s32 convert_vg_aa_mode_vau(ext_vau_paint *paint, VGRenderingQuality render_quality)
{
    switch (render_quality) {
        case VG_RENDERING_QUALITY_NONANTIALIASED:
            paint->render_cfg.render_quality = EXT_VAU_RENDER_QUALITY_NONE_AA;
            break;
        case VG_RENDERING_QUALITY_FASTER:
            paint->render_cfg.render_quality = EXT_VAU_RENDER_QUALITY_BETTER;
            break;
        case VG_RENDERING_QUALITY_BETTER:
            paint->render_cfg.render_quality = EXT_VAU_RENDER_QUALITY_BETTER;
            break;
        default:
            vg_err("invalid render quality:0x%x", render_quality);
            return EXT_FAILURE;
    }
    return EXT_SUCCESS;
}

static td_s32 convert_vg_fill_rule_to_vau(ext_vau_fill_attr *fill_attr, VGFillRule fill_rule)
{
    switch (fill_rule) {
        case VG_EVEN_ODD:
            fill_attr->type = EXT_VAU_EVEN_ODD_FILL;
            break;
        case VG_NON_ZERO:
            fill_attr->type = EXT_VAU_NON_ZERO_FILL;
            break;
        default:
            vg_err("invalid fill rule:0x%x", fill_rule);
            return EXT_FAILURE;
    }
    return EXT_SUCCESS;
}

static void convert_scissor_vg_to_vau(const vg_context *context, ext_vau_surface *surface)
{
    if ((context->scissoring != VG_FALSE) && (context->scissor != NULL)) {
        surface->rect.x = context->scissor[0].x;
        surface->rect.y = context->scissor[0].y;
        surface->rect.width = context->scissor[0].width;
        surface->rect.height = context->scissor[0].height;
    } else {
        surface->rect.x = 0;
        surface->rect.y = 0;
        surface->rect.width = surface->width;
        surface->rect.height = surface->height;
    }
}

static td_void convert_stop_vg_to_vau(ext_vau_stops *vau, vg_paint_stop *vg)
{
    vau->colors = vg->colors;
    vau->offsets = vg->offsets;
    vau->num = vg->num;
}

static td_s32 convert_vg_paint_to_vau_paint(vg_paint *paint, ext_vau_paint_attr *paint_attr)
{
    switch (paint->paint_type) {
        case VG_PAINT_TYPE_COLOR:
            paint_attr->paint_type = EXT_VAU_PAINT_TYPE_COLOR;
            paint_attr->solid.color = convert_vg_color_to_gfx_color(paint->paint_color);
            break;
        case VG_PAINT_TYPE_EXT_COLOR_ALPHA_FF:
            paint_attr->paint_type = EXT_VAU_PAINT_TYPE_COLOR_ALPHA_FF;
            paint_attr->solid.color = convert_vg_color_to_gfx_color(paint->paint_color);
            break;
        case VG_PAINT_TYPE_LINEAR_GRADIENT:
            paint_attr->paint_type = EXT_VAU_PAINT_TYPE_LINEAR_GRADIENT;
            paint_attr->gradient.linear.start_point.x = (td_s32)paint->linear_gradient_point0[0];
            paint_attr->gradient.linear.start_point.y = (td_s32)paint->linear_gradient_point0[1];
            paint_attr->gradient.linear.end_point.x = (td_s32)paint->linear_gradient_point1[0];
            paint_attr->gradient.linear.end_point.y = (td_s32)paint->linear_gradient_point1[1];

            convert_stop_vg_to_vau(&paint_attr->gradient.stops, &paint->stop);
            covert_vg_tilemode_to_vau(&paint_attr->gradient.tile_mode,
                                      paint->colro_ramp_spread_mode);
            break;
        case VG_PAINT_TYPE_RADIAL_GRADIENT:
            paint_attr->paint_type = EXT_VAU_PAINT_TYPE_RADIAL_GRADIENT;
            paint_attr->gradient.radial.center.x = (td_s32)paint->radial_gradient_center[0];
            paint_attr->gradient.radial.center.y = (td_s32)paint->radial_gradient_center[1];
            paint_attr->gradient.radial.radius = (td_u16)paint->radial_gradient_radius;

            convert_stop_vg_to_vau(&paint_attr->gradient.stops, &paint->stop);
            covert_vg_tilemode_to_vau(&paint_attr->gradient.tile_mode,
                                      paint->colro_ramp_spread_mode);
            break;
        case VG_PAINT_TYPE_EXT_SWEEP_GRADIENT:
            paint_attr->paint_type = EXT_VAU_PAINT_TYPE_SWEEP_GRADIENT;
            paint_attr->gradient.sweep.center.x = (td_s32)paint->sweep_gradient_center[0];
            paint_attr->gradient.sweep.center.y = (td_s32)paint->sweep_gradient_center[1];
            paint_attr->gradient.sweep.start_angle = (td_u32)paint->sweep_gradient_angle;

            convert_stop_vg_to_vau(&paint_attr->gradient.stops, &paint->stop);
            covert_vg_tilemode_to_vau(&paint_attr->gradient.tile_mode,
                                      paint->colro_ramp_spread_mode);
            break;
        case VG_PAINT_TYPE_PATTERN:
            paint_attr->paint_type = EXT_VAU_PAINT_TYPE_PATTERN;
            break;
        default:
            vg_err("invalid paint_type:0x%x", paint->paint_type);
            return EXT_FAILURE;
    }
    return EXT_SUCCESS;
}

static td_s32 hdf_gfx_draw_path_hardware(ext_vau_surface *dst_surface, ext_vau_path *path, ext_vau_paint *paint,
    vg_context *context)
{
    ext_vau_surface src_surface[2] = {0};
    ext_vau_draw_attr draw = {0};
    ext_vau_surface_list surface_list = { 0 };
    ext_vau_blit_opt opt = { 0 };

    if (context->draw_handle == VG_INVALID_HANDLE) {
        context->draw_handle = vg_draw_create_handle();
    }

    if (context->draw_handle == VG_INVALID_HANDLE) {
        vg_err("invalid handle");
        return EXT_FAILURE;
    }

    src_surface[1].alpha_info.premult_en = 1;
    src_surface[1].alpha_info.pixel_alpha_en = 1;
    src_surface[1].matrix = &context->path_user_to_surface;
    draw.path = path;
    draw.paint = paint;
    src_surface[1].draw = &draw;

    surface_list.src_surface_cnt = 2; /* 2 srcs */
    surface_list.src_surface = src_surface;
    surface_list.dst_surface = dst_surface;
    surface_list.opt = &opt;

    src_surface[0] = *dst_surface;

    set_blend(&opt.blend_opt, context->blend_mode);

    return uapi_vau_render((td_handle)context->draw_handle, &surface_list);
}

static VGErrorCode hdf_gfx_fill_path(ext_vau_surface *surface, ext_vau_path *vau_path, ext_vau_paint *paint,
    vg_context *context)
{
    if (context->fill_paint == VG_INVALID_HANDLE) {
        vg_err("invalid fill paint handle");
        return VG_ILLEGAL_ARGUMENT_ERROR;
    }
    convert_vg_fill_rule_to_vau(paint->fill_attr, context->fill_rule);
    convert_vg_paint_to_vau_paint((vg_paint *)(uintptr_t)context->fill_paint, &paint->fill_attr->paint_attr);
    paint->paint_mode = EXT_VAU_PAINT_MODE_FILL;

    td_s32 ret = hdf_gfx_draw_path_hardware(surface, vau_path, paint, context);
    if (ret != EXT_SUCCESS) {
        vg_err("hardware draw path failed, ret:0x%x", ret);
        return VG_ILLEGAL_ARGUMENT_ERROR;
    }
    return VG_NO_ERROR;
}

static VGErrorCode hdf_gfx_stroke_path(ext_vau_surface *surface, ext_vau_path *vau_path, ext_vau_paint *paint,
    vg_context *context)
{
    td_s32 ret;
    ext_vau_stroke_attr stroke = {0};
    if (context->stroke_paint == VG_INVALID_HANDLE) {
        vg_err("invalid stroke paint handle");
        return VG_ILLEGAL_ARGUMENT_ERROR;
    }
    stroke.width = context->stroke_line_width;
    stroke.miter_limit = context->stroke_miter_limit;
    convert_vg_line_cap_to_vau(&stroke, context->stroke_cap_style);
    convert_vg_line_join_to_vau(&stroke, context->stroke_join_style);
    convert_vg_dash_to_vau(&stroke, context);
    convert_vg_paint_to_vau_paint((vg_paint *)(uintptr_t)context->stroke_paint, &stroke.paint_attr);

    ext_vau_path dash_path = { 0 };
    ext_vau_path fill_path = { 0 };
    const ext_vau_path *to_fill_path = vau_path;

    if (stroke.dash.enable) {
        ret = vau_create_stroke_dash_path(vau_path, &stroke, &dash_path);
        if (ret != EXT_SUCCESS) {
            vg_err("call vau_create_stroke_dash_path failed\n");
            return ret;
        }
        to_fill_path = &dash_path;
    }

    ext_vau_bezier_stroke_split_mode mode = (context->render_quality == VG_RENDERING_QUALITY_BETTER) ?
        EXT_BEZIER_MODE_LINE_AVG : EXT_BEZIER_MODE_BEZIER_4;

    ret = vau_create_stroke_to_fill_path(to_fill_path, &stroke, &fill_path, paint->fill_attr, mode);
    if (ret != EXT_SUCCESS) {
        vau_err("call vau_create_stroke_to_fill_path failed, ret:0x%x", ret);
        return ret;
    }
    paint->paint_mode = EXT_VAU_PAINT_MODE_FILL;

    ret = hdf_gfx_draw_path_hardware(surface, &fill_path, paint, context);
    vau_destroy_stroke_to_fill_path(&fill_path);
    if (stroke.dash.enable) {
        vau_destroy_stroke_dash_path(&dash_path);
    }
    if (ret != EXT_SUCCESS) {
        vg_err("hardware draw path failed, ret:0x%x", ret);
        return VG_ILLEGAL_ARGUMENT_ERROR;
    }
    return VG_NO_ERROR;
}

VGErrorCode hdf_gfx_draw_path(const vg_path *path, const vg_image *image,
                              VGbitfield paint_modes, vg_context *context)
{
    VGErrorCode ret;
    ext_vau_surface surface = {0};
    ext_vau_path vau_path = {0};
    ext_vau_paint paint = {0};
    ext_vau_fill_attr fill = {0};

    convert_vg_aa_mode_vau(&paint, context->render_quality);
    convert_vg_path_to_vau_path(path, &vau_path);
    convert_vg_image_to_vau_surface(image, &surface);
    convert_scissor_vg_to_vau(context, &surface);

    paint.fill_attr = &fill;

    // fill
    if (((unsigned int)paint_modes & VG_FILL_PATH) == VG_FILL_PATH) {
        ret = hdf_gfx_fill_path(&surface, &vau_path, &paint, context);
        if (ret != VG_NO_ERROR) {
            vau_err("call hdf_gfx_fill_path failed");
            return ret;
        }
    }

    // stroke
    if (((unsigned int)paint_modes & VG_STROKE_PATH) == VG_STROKE_PATH) {
        ret = hdf_gfx_stroke_path(&surface, &vau_path, &paint, context);
        if (ret != VG_NO_ERROR) {
            vau_err("call hdf_gfx_stroke_path failed");
            return ret;
        }
    }

    return VG_NO_ERROR;
}

/*
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description : vau debug file
 */

#include "vau_debug.h"

#ifdef CONFIG_VAU_DFX_DEBUG
static td_void vau_debug_print_surface_opt(const ext_vau_blit_opt *opt)
{
    if (opt == TD_NULL) {
        return;
    }
    vau_print("rop_opt:enable[%u] alpha_mode[%u] color_mode[%u]\n",
              opt->rop_opt.rop_enable, opt->rop_opt.alpha_rop_mode, opt->rop_opt.color_rop_mode);

    vau_print("colorkey_opt:enable[%u] mode[%u] value[%#x]\n",
              opt->colorkey_opt.colorkey_enable, opt->colorkey_opt.colorkey_mode, opt->colorkey_opt.colorkey_value);

    vau_print("blend_opt:enable[%u] cmd[%u] back_mode[%u] fore_mode[%u] out_alpha_mode[%u]\n",
              opt->blend_opt.blend_enable, opt->blend_opt.blend_cmd, opt->blend_opt.background_blend_mode,
              opt->blend_opt.foreground_blend_mode, opt->blend_opt.out_alpha_mode);

    vau_print("blur_opt:enable[%u] mode[%u] gaussian_attr: xy_sigma[%d.%d, %d.%d] alpha_gaussian[%u]\n",
              opt->blur_opt.blur_en, opt->blur_opt.blur_mode, (td_s32)opt->blur_opt.gaussian_attr.x_sigma,
              fractional_part(opt->blur_opt.gaussian_attr.x_sigma), (td_s32)opt->blur_opt.gaussian_attr.y_sigma,
              fractional_part(opt->blur_opt.gaussian_attr.y_sigma), opt->blur_opt.gaussian_attr.alpha_gaussian);
}

static td_void vau_debug_print_path(const ext_vau_path *path)
{
    if (path == TD_NULL) {
        return;
    }

    if (path->cmd_num != 0) {
        vau_print("cmd[%u] = { ", path->cmd_num);
        for (td_u32 i = 0; i < path->cmd_num; i++) {
            vau_print("%u, ", (td_u32)path->cmds[i]);
        }
        vau_print("};\n");
    }

    if (path->data_num != 0) {
        vau_print("data[%u] = { ", path->data_num);
        for (td_u32 i = 0; i < path->data_num; i++) {
            vau_print("%d.%d, ", (td_s32)path->datas[i], fractional_part(path->datas[i]));
        }
        vau_print("};\n");
    }
}

static td_void vau_debug_print_paint_attr(const ext_vau_paint_attr *paint_attr)
{
    vau_print("paint_type:%u\n", paint_attr->paint_type);
    if (paint_attr->paint_type <= EXT_VAU_PAINT_TYPE_COLOR_ALPHA_FF) {
        vau_print("color: %#x\n", paint_attr->solid.color);
        return;
    }
    if (paint_attr->paint_type <= EXT_VAU_PAINT_TYPE_SWEEP_GRADIENT &&
        paint_attr->paint_type >= EXT_VAU_PAINT_TYPE_LINEAR_GRADIENT) {
        vau_print("gradient_tile_mode: %u, stops:colors_offsets:\n",
                  paint_attr->gradient.tile_mode);
        for (td_u32 i = 0; i < paint_attr->gradient.stops.num; i++) {
            vau_print("[%#x, %d.%d]", paint_attr->gradient.stops.colors[i],
                      (td_s32)paint_attr->gradient.stops.offsets[i],
                      fractional_part(paint_attr->gradient.stops.offsets[i]));
        }
        if (paint_attr->paint_type == EXT_VAU_PAINT_TYPE_LINEAR_GRADIENT) {
            vau_print("\nLINEAR: startXY:[%d, %d], endXY:[%d, %d]\n", paint_attr->gradient.linear.start_point.x,
                      paint_attr->gradient.linear.start_point.y,
                      paint_attr->gradient.linear.end_point.x, paint_attr->gradient.linear.end_point.y);
        } else if (paint_attr->paint_type == EXT_VAU_PAINT_TYPE_RADIAL_GRADIENT) {
            vau_print("\nRADIAL: radius:%u, center[%d, %d]\n", paint_attr->gradient.radial.radius,
                      paint_attr->gradient.radial.center.x, paint_attr->gradient.radial.center.y);
        } else {
            vau_print("\nSWEEP: start_angle:%u, center[%d, %d]\n", paint_attr->gradient.sweep.start_angle,
                      paint_attr->gradient.sweep.center.x, paint_attr->gradient.sweep.center.y);
        }
    }
}

static td_void vau_debug_print_paint(const ext_vau_paint *paint)
{
    if (paint == TD_NULL) {
        return;
    }

    vau_print("render_quality[%u] paint_mode[%u]\n", paint->render_cfg.render_quality, paint->paint_mode);
    if (paint->fill_attr != TD_NULL) {
        vau_print("fill:-- type: %u, ", paint->fill_attr->type);
        vau_debug_print_paint_attr(&paint->fill_attr->paint_attr);
    }

    if (paint->stroke_attr != TD_NULL) {
        vau_print("stroke:--width[%d.%d] limit[%d.%d] cap[%u] join[%u]\n",
                  (td_s32)paint->stroke_attr->width, fractional_part(paint->stroke_attr->width),
                  (td_s32)paint->stroke_attr->miter_limit, fractional_part(paint->stroke_attr->miter_limit),
                  paint->stroke_attr->cap, paint->stroke_attr->join);

        vau_debug_print_paint_attr(&paint->stroke_attr->paint_attr);
        if (paint->stroke_attr->dash.enable == TD_TRUE) {
            vau_print("dash:count-offset-reset[%d, %d.%d, %d], array: \n",
                      paint->stroke_attr->dash.count, (td_s32)paint->stroke_attr->dash.offset,
                      fractional_part(paint->stroke_attr->dash.offset), paint->stroke_attr->dash.reset);
            for (td_u32 i = 0; i < paint->stroke_attr->dash.count; i++) {
                vau_print("%d.%d, ", (td_s32)paint->stroke_attr->dash.array[i],
                          fractional_part(paint->stroke_attr->dash.array[i]));
            }
            vau_print("\n");
        }
    }
}

static td_void vau_debug_print_surface(const ext_vau_surface *surface)
{
    vau_print("wh[%ux%u] stride[%u] fmt[%u] order[%d] is_color[%d] color[%#x] cmp-mode[%u]\n",
              surface->width, surface->height, surface->stride, surface->color_fmt,
              surface->argb_order, surface->is_color_surface, surface->color, surface->compress_mode);

    vau_print("rect[%d %d %u %u] out_rect[%d %d %u %u]\n",
              surface->rect.x, surface->rect.y, surface->rect.width, surface->rect.height,
              surface->out_rect.x, surface->out_rect.y, surface->out_rect.width, surface->out_rect.height);
    
    vau_print("g_alpha[%u] g_premult_en[%d], g_alpha_en[%d] premult_en[%d], pix_alpha_en[%d]\n",
              surface->alpha_info.global_alpha, surface->alpha_info.golbal_premult_en,
              surface->alpha_info.global_alpha_en,
              surface->alpha_info.premult_en, surface->alpha_info.pixel_alpha_en);

    if (surface->matrix != TD_NULL) {
        vau_print("matrix(1000):\n %d %d %d\n %d %d %d\n %d %d %d\n",
                  (td_s32)(surface->matrix->matrix[0][0] * VAU_THOUSAND),
                  (td_s32)(surface->matrix->matrix[0][1] * VAU_THOUSAND),
                  (td_s32)(surface->matrix->matrix[0][2] * VAU_THOUSAND),  /* 2:index */
                  (td_s32)(surface->matrix->matrix[1][0] * VAU_THOUSAND),
                  (td_s32)(surface->matrix->matrix[1][1] * VAU_THOUSAND),
                  (td_s32)(surface->matrix->matrix[1][2] * VAU_THOUSAND),  /* 2:index */
                  (td_s32)(surface->matrix->matrix[2][0] * VAU_THOUSAND),  /* 2:index */
                  (td_s32)(surface->matrix->matrix[2][1] * VAU_THOUSAND),  /* 2:index */
                  (td_s32)(surface->matrix->matrix[2][2] * VAU_THOUSAND)); /* 2:index */
    }

    if (surface->pattern != TD_NULL) {
        vau_print("pattern:mode[%u], argb[%u]\n", surface->pattern->mode, surface->pattern->argb);
    }

    if (surface->draw != TD_NULL) {
        vau_debug_print_path(surface->draw->path);
        vau_debug_print_paint(surface->draw->paint);
    }
}
#endif

td_void vau_debug_print_surface_list(const ext_vau_surface_list *surface_list)
{
    if (surface_list == TD_NULL) {
        return;
    }
#ifdef CONFIG_VAU_DFX_DEBUG
    for (td_u32 i = 0; i < surface_list->src_surface_cnt; i++) {
        vau_print("src[%u]--------------------\n", i);
        vau_debug_print_surface(&surface_list->src_surface[i]);
    }
    vau_print("dst-----------------------\n");
    vau_debug_print_surface(surface_list->dst_surface);
    vau_print("opt-----------------------\n");
    vau_debug_print_surface_opt(surface_list->opt);
#endif
}

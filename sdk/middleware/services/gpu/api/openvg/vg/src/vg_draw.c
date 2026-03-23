/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: openvg draw file
 * Author: @CompanyNameTag
 * Create: 2021-07-20
 */

#include "vg_draw.h"
#include <stdlib.h>
#include <string.h>
#include <securec.h>
#include "vg_debug.h"
#include "vg_math.h"
#include "soc_vau_api.h"
#include "hdi_gfx.h"

#define VG_DRAW_IS_SYNC TD_TRUE
#define VG_DRAW_TIMEOUT 1000

static td_bool calc_dst_rect(ext_vau_surface *src, ext_vau_surface *dst, const ext_vau_matrix *m)
{
    if (vg_float_is_equal(m->matrix[0][0], 1) && vg_float_is_equal(m->matrix[0][1], 0) &&
        vg_float_is_equal(m->matrix[1][0], 0) && vg_float_is_equal(m->matrix[1][1], 1) &&
        vg_float_is_equal(m->matrix[2][0], 0) && vg_float_is_equal(m->matrix[2][1], 0) && /* 2: index */
        vg_float_is_equal(m->matrix[2][2], 1)) {                                          /* 2: index */
        dst->rect.x = (td_s32)m->matrix[0][2]; /* trans matrix index is 0 2 for x */
        dst->rect.y = (td_s32)m->matrix[1][2]; /* trans matrix index is 1 2 for y */
        dst->rect.width = src->rect.width;
        dst->rect.height = src->rect.height;

        td_u32 x_offset = (td_u32)((dst->rect.x > 0) ? dst->rect.x : -(dst->rect.x));
        td_u32 y_offset = (td_u32)((dst->rect.y > 0) ? dst->rect.y : -(dst->rect.y));

        if (dst->rect.x + (td_s32)dst->rect.width < 0 || x_offset > dst->width ||
            dst->rect.y + (td_s32)dst->rect.height < 0 || y_offset > dst->height) {
            vg_dbg("invalid rect[%d %d %d %d], width:%d, height:%d\n",
                   dst->rect.x, dst->rect.y, dst->rect.width, dst->rect.height, dst->width, dst->height);
            return TD_TRUE;
        }

        if (dst->rect.x + (td_s32)dst->rect.width > (td_s32)dst->width) {
            dst->rect.width = (td_u32)((td_s32)dst->width - dst->rect.x);
        }
        if (dst->rect.y + (td_s32)dst->rect.height > (td_s32)dst->height) {
            dst->rect.height = (td_u32)((td_s32)dst->height - dst->rect.y);
        }
        if (dst->rect.x < 0) {
            src->rect.x = src->rect.x + (td_s32)x_offset;
            dst->rect.width = (td_u32)((td_s32)dst->width + dst->rect.x);
        }
        if (dst->rect.y < 0) {
            src->rect.y = src->rect.y + (td_s32)y_offset;
            dst->rect.height = (td_u32)((td_s32)dst->height + dst->rect.y);
        }
        src->rect.width = dst->rect.width;
        src->rect.height = dst->rect.height;
    }

    return TD_FALSE;
}

static td_bool set_affine(ext_vau_matrix *matrix, const ext_vau_matrix *m)
{
    if ((matrix == TD_NULL) || (m == TD_NULL)) {
        vg_err("Null pointer ");
        return TD_FALSE;
    }

    if (!(vg_float_is_equal(m->matrix[0][0], 1) && vg_float_is_equal(m->matrix[0][1], 0) &&
        vg_float_is_equal(m->matrix[1][0], 0) && vg_float_is_equal(m->matrix[1][1], 1) &&
        vg_float_is_equal(m->matrix[2][0], 0) && vg_float_is_equal(m->matrix[2][1], 0) && /* 2 is array index */
        vg_float_is_equal(m->matrix[2][2], 1))) { /* 2 is array index */
        *matrix = *m;
        return TD_TRUE;
    }

    return TD_FALSE;
}

static void set_global_alpha(ext_vau_surface *s, VGboolean color_transform, VGfloat alpha_scale)
{
    s->alpha_info.global_alpha_en = (color_transform == VG_TRUE) ? TD_TRUE : TD_FALSE;
    s->alpha_info.global_alpha = (td_u8)(alpha_scale * 0xFF);
}

static void make_color_surface(ext_vau_surface *s, const ext_rect *rect, const VGfloat *color)
{
    s->width = rect->width;
    s->height = rect->height;
    s->stride = uapi_byte_align(rect->width, 16) * 4; /* 16 align * 4 bytes for argb8888's stride */

    s->color_fmt = EXT_GFX_FMT_ARGB8888;
    s->argb_order = EXT_GFX_FMT_ORDER_ARGB;
    s->is_color_surface = TD_TRUE;
    s->color = convert_vg_color_to_gfx_color(color);
    s->compress_mode = EXT_GFX_COMPRESS_NONE;
    convert_to_vau_alpha_info(&s->alpha_info, VG_sARGB_8888_PRE);
    s->rect.x = 0;
    s->rect.y = 0;
    s->rect.width = rect->width;
    s->rect.height = rect->height;
    s->matrix = NULL;
    s->draw = NULL;
}

VGErrorCode vg_draw_image(const vg_image *src, const vg_image *dst, vg_context *context)
{
    ext_vau_matrix matrix = {{{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}};
    if (context->draw_handle == VG_INVALID_HANDLE) {
        context->draw_handle = vg_draw_create_handle();
    }

    if (context->draw_handle == VG_INVALID_HANDLE) {
        vg_err("invalid handle");
        return VG_BAD_HANDLE_ERROR;
    }
    ext_vau_surface src_surface[2] = {0};
    ext_vau_surface dst_surface = { 0 };
    ext_vau_surface_list surface_list = { 0 };
    ext_vau_blit_opt opt = { 0 };

    src_surface[1].matrix = &matrix;
    surface_list.src_surface_cnt = 2; /* 2 srcs */
    surface_list.src_surface = src_surface;
    surface_list.dst_surface = &dst_surface;
    surface_list.opt = &opt;

    convert_vg_image_to_vau_surface(src, &src_surface[1]);
    convert_vg_image_to_vau_surface(dst, &dst_surface);

    if (calc_dst_rect(&src_surface[1], &dst_surface, &context->image_user_to_surface) == TD_TRUE) {
        return VG_NO_ERROR;
    }

    src_surface[0] = dst_surface;

    if (set_affine(src_surface[1].matrix, &context->image_user_to_surface) == TD_FALSE) {
        src_surface[1].matrix = NULL;
    }
    set_blend(&opt.blend_opt, context->blend_mode);
    /* index 3 is alpha scale */
    set_global_alpha(&src_surface[1], context->color_transform, context->color_transform_values[3]);

    td_s32 ret = uapi_vau_render((td_handle)context->draw_handle, &surface_list);
    if (ret != EXT_SUCCESS) {
        vg_err("call uapi_vau_render failed, ret:0x%x", ret);
        return VG_ERROR_CODE_FORCE_SIZE;
    }
    return VG_NO_ERROR;
}

VGErrorCode vg_draw_clear_image(const vg_image *dst, const ext_rect *rect, vg_context *context)
{
    if (context->draw_handle == VG_INVALID_HANDLE) {
        context->draw_handle = vg_draw_create_handle();
    }

    if (context->draw_handle == VG_INVALID_HANDLE) {
        vg_err("invalid handle");
        return VG_BAD_HANDLE_ERROR;
    }
    ext_vau_surface src_surface[1] = { 0 };
    ext_vau_surface dst_surface = { 0 };
    ext_vau_surface_list surface_list = { 0 };

    surface_list.src_surface_cnt = 1;
    surface_list.src_surface = src_surface;
    surface_list.dst_surface = &dst_surface;
    surface_list.opt = NULL;

    make_color_surface(&src_surface[0], rect, context->clear_color);
    convert_vg_image_to_vau_surface(dst, &dst_surface);
    dst_surface.rect = *rect;

    td_s32 ret = uapi_vau_render((td_handle)context->draw_handle, &surface_list);
    if (ret != EXT_SUCCESS) {
        vg_err("call uapi_vau_render failed, ret:0x%x", ret);
        return VG_ERROR_CODE_FORCE_SIZE;
    }
    return VG_NO_ERROR;
}

void vg_draw_flush(VGHandle handle)
{
    td_s32 ret;
    if (handle == VG_INVALID_HANDLE) {
        return;
    }
    ret = uapi_vau_submit(handle, VG_DRAW_IS_SYNC, VG_DRAW_TIMEOUT);
    if (ret != EXT_SUCCESS) {
        uapi_vau_cancel(handle);
    }
    ret = uapi_vau_destroy(handle);
    if (ret != EXT_SUCCESS) {
        vg_err("call uapi_vau_destroy failed, ret:0x%x", ret);
    }
}

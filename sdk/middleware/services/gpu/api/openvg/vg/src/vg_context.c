/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: openvg context adapter source file
 */

#include "vg_context.h"
#include <securec.h>
#include "vg_debug.h"
#include "vg_math.h"
#include "vg_draw.h"
#include "vg_paint.h"

static VGboolean vg_context_is_valid_image(vg_context *thiz, VGImage image)
{
    return thiz->image_manager->is_valid(thiz->image_manager, (vg_resource *)(uintptr_t)image);
}

static VGboolean vg_context_is_valid_path(vg_context *thiz, VGPath path)
{
    return thiz->path_manager->is_valid(thiz->image_manager, (vg_resource *)(uintptr_t)path);
}

static VGboolean vg_context_is_valid_paint(vg_context *thiz, VGPaint paint)
{
    return thiz->paint_manager->is_valid(thiz->image_manager, (vg_resource *)(uintptr_t)paint);
}

static VGboolean vg_context_is_valid_font(vg_context *thiz, VGFont font)

{
    return thiz->font_manager->is_valid(thiz->image_manager, (vg_resource *)(uintptr_t)font);
}

static VGboolean vg_context_is_valid_mask_layer(vg_context *thiz, VGMaskLayer layer)

{
    return thiz->mask_layer_manager->is_valid(thiz->image_manager, (vg_resource *)(uintptr_t)layer);
}

static void vg_context_release_paint(vg_context *thiz, VGbitfield paint_mode)
{
    if ((paint_mode & VG_FILL_PATH) != 0) {
        // release previous paint
        vg_paint *prev = (vg_paint *)(uintptr_t)thiz->fill_paint;
        if (prev != NULL) {
            if (remove_ref(prev) == 0) {
                delete_object(prev);
            }
        }
        thiz->fill_paint = VG_INVALID_HANDLE;
    }
    if ((paint_mode & VG_STROKE_PATH) != 0) {
        // release previous paint
        vg_paint *prev = (vg_paint *)(uintptr_t)thiz->stroke_paint;
        if (prev != NULL) {
            if (remove_ref(prev) == 0) {
                delete_object(prev);
            }
        }
        thiz->stroke_paint = VG_INVALID_HANDLE;
    }
}

static void vg_context_set_error(vg_context *thiz, VGErrorCode error)
{
    if (thiz->error == VG_NO_ERROR) {
        thiz->error = error;
    }
}

static void vg_context_flush(vg_context *context)
{
    vg_draw_flush(context->draw_handle);
    context->draw_handle = VG_INVALID_HANDLE;
}

static void vg_context_deinit_resource_manager(vg_context *context)
{
    delete_object(context->image_manager);
    delete_object(context->path_manager);
    delete_object(context->paint_manager);
    delete_object(context->font_manager);
    delete_object(context->mask_layer_manager);

    context->image_manager = NULL;
    context->path_manager = NULL;
    context->paint_manager = NULL;
    context->font_manager = NULL;
    context->mask_layer_manager = NULL;
}

static VGErrorCode vg_context_init_resource_manager(vg_context *context)
{
    context->image_manager = NULL;
    context->path_manager = NULL;
    context->paint_manager = NULL;
    context->font_manager = NULL;
    context->mask_layer_manager = NULL;

    context->image_manager = new_object(vg_resource_manager);
    if (context->image_manager == NULL) {
        vg_err("new object failed");
        return VG_OUT_OF_MEMORY_ERROR;
    }
    context->path_manager = new_object(vg_resource_manager);
    if (context->path_manager == NULL) {
        vg_context_deinit_resource_manager(context);
        vg_err("new object failed");
        return VG_OUT_OF_MEMORY_ERROR;
    }
    context->paint_manager = new_object(vg_resource_manager);
    if (context->paint_manager == NULL) {
        vg_context_deinit_resource_manager(context);
        vg_err("new object failed");
        return VG_OUT_OF_MEMORY_ERROR;
    }
    context->font_manager = new_object(vg_resource_manager);
    if (context->font_manager == NULL) {
        vg_context_deinit_resource_manager(context);
        vg_err("new object failed");
        return VG_OUT_OF_MEMORY_ERROR;
    }
    context->mask_layer_manager = new_object(vg_resource_manager);
    if (context->mask_layer_manager == NULL) {
        vg_context_deinit_resource_manager(context);
        vg_err("new object failed");
        return VG_OUT_OF_MEMORY_ERROR;
    }

    return VG_NO_ERROR;
}

void vg_context_delete(vg_context *context)
{
    context->release_paint(context, VG_FILL_PATH | VG_STROKE_PATH);
    vg_context_deinit_resource_manager(context);
    free(context);
}

static void vg_context_init_comm(vg_context *context)
{
    context->matrix_mode = VG_MATRIX_PATH_USER_TO_SURFACE;

    // Scissor rectangles
    context->scissor = NULL; // not support
    context->scissor_num = 0; // not support
    context->masking = VG_FALSE;
    context->scissoring = VG_FALSE; // not support
    context->pixel_layout = VG_PIXEL_LAYOUT_UNKNOWN; // not support
    context->filter_format_linear = VG_FALSE; // not support
    context->filter_format_premulted = VG_FALSE; // not support
    context->filter_channel_mask = VG_RED | VG_GREEN | VG_BLUE | VG_ALPHA; // not support

    vg_matrix_identity(&context->path_user_to_surface);
    vg_matrix_identity(&context->image_user_to_surface);
    vg_matrix_identity(&context->glyph_user_to_surface);
    vg_matrix_identity(&context->fill_paint_to_user);
    vg_matrix_identity(&context->stroke_paint_to_user);

    context->color_transform = VG_FALSE;
    context->color_transform_values[0] = 1.0f; /* defaut value for index 0 */
    context->color_transform_values[1] = 1.0f; /* defaut value for index 1 */
    context->color_transform_values[2] = 1.0f; /* defaut value for index 2 */
    context->color_transform_values[3] = 1.0f; /* defaut value for index 3 */
    context->color_transform_values[4] = 0.0f; /* defaut value for index 4 */
    context->color_transform_values[5] = 0.0f; /* defaut value for index 5 */
    context->color_transform_values[6] = 0.0f; /* defaut value for index 6 */
    context->color_transform_values[7] = 0.0f; /* defaut value for index 7 */

    context->error = VG_NO_ERROR;
}

static void vg_context_init_path(vg_context *context)
{
    context->fill_rule = VG_NON_ZERO;
    context->render_quality = VG_RENDERING_QUALITY_BETTER;
    // Stroke parameters
    context->stroke_line_width = 1.0f;
    context->stroke_cap_style = VG_CAP_BUTT;
    context->stroke_join_style = VG_JOIN_BEVEL;
    context->stroke_miter_limit = 4.0f;
    context->stroke_dash_pattern = NULL;
    context->stroke_dash_pattern_num = 0;
    context->stroke_dash_phase = 0.0f;
    context->stroke_dash_phase_reset = VG_FALSE;

    context->fill_paint = VG_INVALID_HANDLE;
    context->stroke_paint = VG_INVALID_HANDLE;
}

static void vg_context_init_image(vg_context *context)
{
    context->image_quality = VG_IMAGE_QUALITY_FASTER;
    context->blend_mode = VG_BLEND_SRC_OVER;
    context->image_mode = VG_DRAW_IMAGE_NORMAL;
}

int vg_context_init(vg_context *context)
{
    if (context == NULL) {
        vg_err("null ptr");
        return VG_BAD_HANDLE_ERROR;
    }

    vg_context_init_comm(context);
    vg_context_init_path(context);
    vg_context_init_image(context);

    context->is_valid_image = vg_context_is_valid_image;
    context->is_valid_path = vg_context_is_valid_path;
    context->is_valid_paint = vg_context_is_valid_paint;
    context->is_valid_font = vg_context_is_valid_font;
    context->is_valid_mask_layer = vg_context_is_valid_mask_layer;

    context->release_paint = vg_context_release_paint;
    context->set_error = vg_context_set_error;
    context->flush = vg_context_flush;

    return vg_context_init_resource_manager(context);
}

vg_context *vg_context_new(void)
{
    vg_context *context = (vg_context *)malloc(sizeof(vg_context));
    if (context == NULL) {
        vg_err("malloc %d failed", sizeof(vg_context));
        return NULL;
    }
    (void)memset_s(context, sizeof(*context), 0, sizeof(vg_context));

    int ret = vg_context_init(context);
    if (ret != VG_NO_ERROR) {
        vg_context_delete(context);
        vg_err("vg_context init failed, ret:0x%x", ret);
        return NULL;
    }
    return context;
}

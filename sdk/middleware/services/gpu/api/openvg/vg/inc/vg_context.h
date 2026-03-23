/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description : openvg context file
 */

#ifndef API_OPENVG_CONTEXT_H
#define API_OPENVG_CONTEXT_H

#include "VG/openvg.h"
#include "osal_list.h"
#include "EGL/egl.h"
#include "egl_mutex.h"
#include "vg_resource.h"
#include "soc_vau_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef struct vg_context_tag vg_context;
struct vg_context_tag {
    // Mode settings
    VGMatrixMode                    matrix_mode;
    VGFillRule                      fill_rule;
    VGImageQuality                  image_quality;
    VGRenderingQuality              render_quality;
    VGBlendMode                     blend_mode;
    VGImageMode                     image_mode;

    // Scissor rectangles
    ext_rect                        *scissor; // not support
    td_u8                           scissor_num; // not support

    // Stroke parameters
    VGfloat                         stroke_line_width;
    VGCapStyle                      stroke_cap_style;
    VGJoinStyle                     stroke_join_style;
    VGfloat                         stroke_miter_limit;
    VGfloat                        *stroke_dash_pattern;
    td_u8                           stroke_dash_pattern_num;
    VGfloat                         stroke_dash_phase;
    VGboolean                       stroke_dash_phase_reset;

    // Edge fill color for vgConvolve and pattern paint
    VGfloat                         tile_fill_color[4];

    // Color for vgClear
    VGfloat                         clear_color[4];

    //
    VGfloat                         glyph_origin[2];

    VGboolean                       masking;
    VGboolean                       scissoring; // not support
    VGPixelLayout                   pixel_layout; // not support
    VGboolean                       filter_format_linear; // not support
    VGboolean                       filter_format_premulted; // not support
    VGbitfield                      filter_channel_mask; // not support

    // Matrix
    ext_vau_matrix                  path_user_to_surface;
    ext_vau_matrix                  image_user_to_surface;
    ext_vau_matrix                  glyph_user_to_surface;
    ext_vau_matrix                  fill_paint_to_user;
    ext_vau_matrix                  stroke_paint_to_user;

    // Paint
    VGPaint                         fill_paint;
    VGPaint                         stroke_paint;

    // Color transform
    VGboolean                       color_transform;
    VGfloat                         color_transform_values[8];

    VGErrorCode                     error;

    // resources
    vg_resource_manager             *image_manager;
    vg_resource_manager             *path_manager;
    vg_resource_manager             *paint_manager;
    vg_resource_manager             *font_manager;
    vg_resource_manager             *mask_layer_manager;

    VGHandle                        draw_handle;

    VGboolean (*is_valid_image)(vg_context *thiz, VGImage image);
    VGboolean (*is_valid_path)(vg_context *thiz, VGPath path);
    VGboolean (*is_valid_paint)(vg_context *thiz, VGPaint paint);
    VGboolean (*is_valid_font)(vg_context *thiz, VGFont font);
    VGboolean (*is_valid_mask_layer)(vg_context *thiz, VGMaskLayer layer);

    void (*release_paint)(vg_context *thiz, VGbitfield paintModes);

    void (*set_error)(vg_context *thiz, VGErrorCode error);

    void (*flush)(vg_context *thiz);
};

int vg_context_init(vg_context *context);
vg_context *vg_context_new(void);
void vg_context_delete(vg_context *context);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif /* API_OPENVG_CONTEXT_H */
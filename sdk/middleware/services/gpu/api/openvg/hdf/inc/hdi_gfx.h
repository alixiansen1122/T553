/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description : hdi_gfx file
 * Author: @CompanyNameTag
 * Create: 2021-08-16
 */

#ifndef HDI_GFX_H
#define HDI_GFX_H

#include "vg_path.h"
#include "vg_image.h"
#include "soc_vau_type.h"
#include "vg_context.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define VAU_COMPRESS_HEADER 16

VGHandle vg_draw_create_handle(void);

void set_blend(ext_vau_blend_opt *blend, VGBlendMode mode);
td_u32 convert_vg_color_to_gfx_color(const VGfloat *color);
void convert_to_vau_alpha_info(ext_vau_alpha_info *info, VGImageFormat v);
void convert_vg_image_to_vau_surface(const vg_image *v, ext_vau_surface *s);
VGErrorCode hdf_gfx_draw_path(const vg_path *path, const vg_image *image, VGbitfield paint_modes, vg_context *context);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* HDI_GFX_H */

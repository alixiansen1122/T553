/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description : openvg draw file
 */

#ifndef API_OPENVG_DRAW_H
#define API_OPENVG_DRAW_H

#include "vg_object.h"
#include "vg_image.h"
#include "vg_context.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

VGErrorCode vg_draw_image(const vg_image *src, const vg_image *dst, vg_context *context);
VGErrorCode vg_draw_clear_image(const vg_image *dst, const ext_rect *rect, vg_context *context);
void vg_draw_flush(VGHandle handle);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif /* API_OPENVG_DRAW_H */
/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description : openvg paint file
 * Author: @CompanyNameTag
 * Create: 2021-08-16
 */

#ifndef API_OPENVG_PAINT_H
#define API_OPENVG_PAINT_H

#include "VG/openvg.h"
#include "vg_object.h"
#include "vg_image.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef struct {
    VGuint colors[7];  /* include alpha */
    VGfloat offsets[7]; /* 0 to 1 */
    VGubyte num;      /* at least 2 stop points at 0 and 100 */
} vg_paint_stop;

typedef struct vg_paint_tag vg_paint;
struct vg_paint_tag {
    vg_magic magic;
    struct osal_list_head node;
    int ref_count;
    void (*add_ref)(vg_paint *thiz);
    int  (*remove_ref)(vg_paint *thiz);
    void (*free)(vg_paint *thiz);

    VGPaintType paint_type;
    VGfloat paint_color[4];
    VGColorRampSpreadMode colro_ramp_spread_mode;
    vg_paint_stop stop;
    VGboolean color_ramp_premultiplied;
    VGfloat linear_gradient_point0[2];
    VGfloat linear_gradient_point1[2];
    VGfloat radial_gradient_center[2];
    VGfloat radial_gradient_focus[2];
    VGfloat radial_gradient_radius;
    VGfloat sweep_gradient_center[2];
    VGfloat sweep_gradient_angle;

    VGErrorCode (*seti)(vg_paint *thiz, VGint type, VGint value);
    VGErrorCode (*geti)(const vg_paint *thiz, VGint type, VGint *value);

    VGErrorCode (*setfv)(vg_paint *thiz, VGint type, VGint count, const VGfloat *values);
    VGErrorCode (*get_size)(const vg_paint *thiz, VGint type, VGint *count);
    VGErrorCode (*getfv)(const vg_paint *thiz, VGint type, VGint count, VGfloat *values);

    VGErrorCode (*set_color)(vg_paint *thiz, VGuint rgba);
    VGuint(*get_color)(const vg_paint *thiz);
    VGErrorCode(*paint_pattern)(const vg_paint *thiz, const vg_image *pattern);
};

vg_paint *vg_paint_new(void);
void vg_paint_delete(vg_paint *p);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif /* API_OPENVG_PAINT_H */


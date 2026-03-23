/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description : openvg image file
 */

#ifndef API_OPENVG_IMAGE_H
#define API_OPENVG_IMAGE_H

#include "soc_gfx_type.h"
#include "VG/openvg.h"
#include "VG/vgext.h"
#include "soc_gfx_type.h"
#include "vg_object.h"
#include "vg_resource.h"
#include "vg_context.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef struct vg_image_tag vg_image;
struct vg_image_tag {
    vg_magic magic;
    struct osal_list_head node;
    int ref_count;
    void (*add_ref)(vg_image *thiz);
    int  (*remove_ref)(vg_image *thiz);
    void (*free)(vg_image *thiz);

    VGImageFormat fmt;
    VGuint width;
    VGuint height;
    VGbitfield allowed_quality;

    VGuint offset_x;
    VGuint offset_y;

    vg_image *parent;

    VGImageBufferType buffer_type;
    VGuint stride;
    VGuint phy_addr;
    VGubyte *vir_addr;
    VGfloat color[4];
    VGuint clut_addr;
    ext_gfx_compress_mode compress_mode;

    VGErrorCode (*seti)(vg_image *thiz, VGint type, VGint value);
    VGErrorCode (*geti)(const vg_image *thiz, VGint type, VGint *value);

    VGErrorCode (*setfv)(vg_image *thiz, VGint type, VGint count, const VGfloat *values);
    VGErrorCode (*get_size)(const vg_image *thiz, VGint type, VGint *count);
    VGErrorCode (*getfv)(const vg_image *thiz, VGint type, VGint count, VGfloat *values);

    VGErrorCode (*draw_image)(const vg_image *thiz, const vg_image *src, vg_context *context);
    VGErrorCode (*clear_image)(const vg_image *thiz, const ext_rect *rect, vg_context *context);
};

int vg_image_init(vg_image *m, VGImageFormat fmt, VGint width, VGint height, VGbitfield allowed_quality);
vg_image *vg_image_new(VGImageFormat fmt, VGint width, VGint height, VGbitfield allowed_quality);
void vg_image_delete(vg_image *m);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif /* API_OPENVG_IMAGE_H */
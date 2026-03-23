/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description : egl surface file
 */

#ifndef API_OPENVG_EGL_SURFACE_H
#define API_OPENVG_EGL_SURFACE_H

#include "egl_config.h"
#include "vg_image.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef struct egl_surface_tag egl_surface;
struct egl_surface_tag {
    vg_magic magic;
    struct osal_list_head node;
    int ref_count;
    void (*add_ref)(egl_surface *thiz);
    int  (*remove_ref)(egl_surface *thiz);
    void (*free)(egl_surface *thiz);

    EGLConfig config;
    vg_image *image;
};

int egl_surface_init(egl_surface *s, const EGLConfig config);
egl_surface *egl_surface_new(const EGLConfig config);
void egl_surface_delete(egl_surface *s);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif /* API_OPENVG_EGL_SURFACE_H */

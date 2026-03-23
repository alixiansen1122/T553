/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description : egl display file
 */

#ifndef API_OPENVG_EGL_DISPLAY_H
#define API_OPENVG_EGL_DISPLAY_H

#include "egl_context.h"
#include "egl_surface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef struct egl_display_tag egl_display;
struct egl_display_tag {
    vg_magic magic;
    struct osal_list_head node;
    int ref_count;
    void (*add_ref)(egl_display *thiz);
    int  (*remove_ref)(egl_display *thiz);
    void (*free)(egl_display *thiz);

    EGLDisplay id;
    const EGLConfig config;

    egl_context context_list;
    egl_surface surface_list;

    void (*add_context)(egl_display *thiz, egl_context *ctx);
    void (*remove_context)(const egl_display *thiz, egl_context *ctx);

    void (*add_surface)(egl_display *thiz, egl_surface *srf);
    void (*remove_surface)(const egl_display *thiz, egl_surface *srf);

    EGLBoolean (*context_exists)(egl_display *thiz, const EGLContext ctx);
    EGLBoolean (*surface_exists)(egl_display *thiz, const EGLSurface srf);
    EGLBoolean (*config_exists)(egl_display *thiz, const EGLConfig cfg);
};

int egl_display_init(egl_display *d, EGLDisplay id);
egl_display *egl_display_new(EGLDisplay id);
void egl_display_delete(egl_display *d);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif /* API_OPENVG_EGL_DISPLAY_H */

/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description : egl inner file
 */

#ifndef API_OPENVG_EGL_EGL_H
#define API_OPENVG_EGL_EGL_H

#include "EGL/egl.h"
#include "vg_object.h"
#include "egl_surface.h"
#include "egl_display.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef struct egl_tag egl;
struct egl_tag {
    vg_magic magic;
    struct osal_list_head node;
    int ref_count;
    void (*add_ref)(egl *thiz);
    int  (*remove_ref)(egl *thiz);
    void (*free)(egl *thiz);

    EGLint error;
    egl_display display_list;
    egl_context *current_context;
    egl_surface *current_surface;

    void (*add_display)(egl *thiz, egl_display *disp);
    void (*remove_display)(const egl *thiz, egl_display *disp);
    egl_display *(*get_display)(egl *thiz, const EGLDisplay *disp_id);
    EGLDisplay (*find_dispaly)(egl *thiz, const EGLContext ctx);

    void (*set_error)(egl *e, EGLint error);
    EGLint (*get_error)(egl *e);

    void (*set_current_context)(egl *e, egl_context *c);
    void (*set_current_surface)(egl *e, egl_surface *s);
    egl_context *(*get_current_context)(egl *e);
    egl_surface *(*get_current_surface)(egl *e);
};

egl *get_egl(void);
void release_egl(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif /* API_OPENVG_EGL_EGL_H */

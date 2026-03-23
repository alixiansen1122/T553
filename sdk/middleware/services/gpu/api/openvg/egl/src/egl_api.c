/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: egl adapter source file
 */


#include <stdlib.h>
#include <string.h>
#include "EGL/egl.h"
#include "osal_list.h"
#include "egl_egl.h"
#include "egl_mutex.h"
#include "egl_context.h"
#include "egl_surface.h"
#include "egl_display.h"
#include "vg_debug.h"
#include "vg_object.h"
#include "vg_context.h"
#include "vg_image.h"

#define EGL_VERSION_MAJOR 1
#define EGL_VERSION_MINOR 3

#define egl_get_display(disp_id, ret_val) do { \
    egl_acquire_mutex(); \
    e = get_egl(); \
    if (e == NULL) { \
        egl_release_mutex(); \
        vg_err("egl not inited"); \
        return ret_val; \
    } \
    display = e->get_display(e, disp_id); \
} while (0)

#define egl_get_egl(ret_val) do { \
    egl_acquire_mutex(); \
    e = get_egl(); \
    if (e == NULL) { \
        egl_release_mutex(); \
        vg_err("egl not inited"); \
        return ret_val; \
    } \
} while (0)

#define egl_if_error_return(cond, err_code, ret_value) do { \
    if (cond) { \
        e->set_error(e, err_code); \
        egl_release_mutex(); \
        vg_err("in condition: %s,err code:0x%x", #cond, err_code); \
        return ret_value; \
    } \
} while (0)

#define egl_return(err_code, ret_value) do { \
        e->set_error(e, err_code); \
        egl_release_mutex(); \
        return ret_value; \
    } while (0)

EGLint eglGetError(void)
{
    egl *e = NULL;
    egl_get_egl(EGL_NOT_INITIALIZED);

    EGLint ret = e->get_error(e);

    egl_return(EGL_SUCCESS, ret);
}

EGLDisplay eglGetDisplay(NativeDisplayType display)
{
    return (EGLDisplay)((EGLint)(uintptr_t)display + 1);
}

EGLBoolean eglInitialize(EGLDisplay dpy, EGLint *major, EGLint *minor)
{
    egl *e = NULL;
    egl_display *display = NULL;
    egl_get_display(dpy, EGL_FALSE);
    egl_if_error_return(display != NULL, EGL_SUCCESS, EGL_TRUE); // already inited

    display = new_object(egl_display, dpy);
    egl_if_error_return(display == NULL, EGL_BAD_ALLOC, EGL_FALSE);

    e->add_display(e, display);

    if (major != NULL) {
        *major = EGL_VERSION_MAJOR;
    }
    if (minor != NULL) {
        *minor = EGL_VERSION_MINOR;
    }
    egl_return(EGL_SUCCESS, EGL_TRUE);
}

EGLBoolean eglTerminate(EGLDisplay dpy)
{
    egl *e = NULL;
    egl_display *display = NULL;
    egl_get_display(dpy, EGL_FALSE);
    egl_if_error_return(display == NULL, EGL_SUCCESS, EGL_TRUE); // no display

    e->remove_display(e, display);
    delete_object(display);

    egl_return(EGL_SUCCESS, EGL_TRUE);
}

const char *eglQueryString(EGLDisplay dpy, EGLint name)
{
    egl *e = NULL;
    egl_display *display = NULL;
    egl_get_display(dpy, NULL);
    egl_if_error_return(display == NULL, EGL_NOT_INITIALIZED, NULL); // no display

    static const char apis[] = "OpenVG";
    static const char extensions[] = "";
    static const char vendor[] = "SDK";
    static const char version[] = "1.1";

    const char *ret = NULL;
    switch (name) {
        case EGL_CLIENT_APIS:
            ret = apis;
            break;

        case EGL_EXTENSIONS:
            ret = extensions;
            break;

        case EGL_VENDOR:
            ret = vendor;
            break;

        case EGL_VERSION:
            ret = version;
            break;

        default:
            egl_return(EGL_BAD_PARAMETER, NULL);
    }
    egl_return(EGL_SUCCESS, ret);
}

EGLBoolean eglGetConfigs(EGLDisplay dpy, EGLConfig *configs,
                         EGLint config_size, EGLint *num_config)
{
    egl *e = NULL;
    egl_display *display = NULL;

    uapi_unused(configs);
    uapi_unused(config_size);
    uapi_unused(num_config);

    egl_get_display(dpy, EGL_FALSE);
    egl_if_error_return(display == NULL, EGL_NOT_INITIALIZED, EGL_FALSE); // no display

    egl_return(EGL_SUCCESS, EGL_TRUE);
}

EGLBoolean eglChooseConfig(EGLDisplay dpy, const EGLint *attrib_list,
                           EGLConfig *configs, EGLint config_size,
                           EGLint *num_config)
{
    egl *e = NULL;
    egl_display *display = NULL;

    uapi_unused(attrib_list);
    uapi_unused(configs);
    uapi_unused(config_size);
    uapi_unused(num_config);

    egl_get_display(dpy, EGL_FALSE);
    egl_if_error_return(display == NULL, EGL_NOT_INITIALIZED, EGL_FALSE); // no display

    egl_return(EGL_SUCCESS, EGL_TRUE);
}

EGLBoolean eglGetConfigAttrib(EGLDisplay dpy, EGLConfig config,
                              EGLint attribute, EGLint *value)
{
    egl *e = NULL;
    egl_display *display = NULL;

    uapi_unused(config);
    uapi_unused(attribute);
    uapi_unused(value);

    egl_get_display(dpy, EGL_FALSE);
    egl_if_error_return(display == NULL, EGL_NOT_INITIALIZED, EGL_FALSE); // no display

    egl_return(EGL_SUCCESS, EGL_TRUE);
}

EGLSurface eglCreateWindowSurface(EGLDisplay dpy, EGLConfig config,
                                  NativeWindowType win,
                                  const EGLint *attrib_list);
EGLSurface eglCreatePbufferSurface(EGLDisplay dpy, EGLConfig config,
                                   const EGLint *attrib_list);
EGLSurface eglCreatePixmapSurface(EGLDisplay dpy, EGLConfig config,
                                  NativePixmapType pixmap,
                                  const EGLint *attrib_list);

EGLBoolean eglDestroySurface(EGLDisplay dpy, EGLSurface surface)
{
    egl *e = NULL;
    egl_display *display = NULL;
    egl_get_display(dpy, EGL_FALSE);
    egl_if_error_return(display == NULL, EGL_NOT_INITIALIZED, EGL_FALSE);
    egl_if_error_return(surface == NULL, EGL_BAD_SURFACE, EGL_FALSE);
    egl_if_error_return(display->surface_exists(display, surface) == TD_FALSE, EGL_BAD_SURFACE, EGL_FALSE);

    egl_surface *s = (egl_surface*)surface;

    if (s->image != NULL) {
        if (remove_ref(s->image) == 0) {
            delete_object(s->image);
        }
    }

    display->remove_surface(display, s);
    if ((remove_ref(s)) == 0) {
        delete_object(s);
    }

    egl_return(EGL_SUCCESS, EGL_TRUE);
}

EGLBoolean eglQuerySurface(EGLDisplay dpy, EGLSurface surface,
                           EGLint attribute, EGLint *value);

EGLBoolean eglBindAPI(EGLenum api)
{
    egl *e = NULL;
    egl_get_egl(EGL_NONE);
    egl_if_error_return(api != EGL_OPENVG_API && api != EGL_OPENGL_ES_API, EGL_BAD_PARAMETER, EGL_FALSE);
    egl_return(EGL_SUCCESS, EGL_TRUE);
}

EGLenum eglQueryAPI(void)
{
    egl *e = NULL;
    egl_get_egl(EGL_NONE);
    egl_return(EGL_SUCCESS, EGL_OPENVG_API);
}

EGLBoolean eglWaitClient(void);

EGLBoolean eglReleaseThread(void)
{
    egl *e = NULL;
    egl_get_egl(EGL_FALSE);

    egl_release_mutex();
    egl_deinit_mutex();

    release_egl();
    return EGL_TRUE;
}

EGLSurface eglCreatePbufferFromClientBuffer(EGLDisplay dpy, EGLenum buftype, EGLClientBuffer buffer,
    EGLConfig config, const EGLint *attrib_list)
{
    egl *e = NULL;
    egl_display *display = NULL;

    uapi_unused(attrib_list);

    egl_get_display(dpy, EGL_NO_SURFACE);
    egl_if_error_return(display == NULL, EGL_NOT_INITIALIZED, EGL_NO_SURFACE);
    egl_if_error_return(buftype != EGL_OPENVG_IMAGE, EGL_BAD_PARAMETER, EGL_NO_SURFACE);
    egl_if_error_return(buffer == VG_INVALID_HANDLE, EGL_BAD_PARAMETER, EGL_NO_SURFACE);

    vg_image *image = (vg_image *)(uintptr_t)buffer;
    vg_resource_type resource_type = get_resource_type((vg_resource *)(uintptr_t)buffer);
    egl_if_error_return(resource_type != VG_TYPE_RESOURCE_IMAGE, EGL_BAD_PARAMETER, EGL_NO_SURFACE);

    egl_surface *surface = new_object(egl_surface, config);
    egl_if_error_return(surface == NULL, EGL_BAD_ALLOC, EGL_NO_SURFACE);

    surface->image = image;

    add_ref(image);
    add_ref(surface);
    display->add_surface(display, surface);
    egl_return(EGL_SUCCESS, surface);
}

EGLBoolean eglSurfaceAttrib(EGLDisplay dpy, EGLSurface surface,
                            EGLint attribute, EGLint value);
EGLBoolean eglBindTexImage(EGLDisplay dpy, EGLSurface surface, EGLint buffer);
EGLBoolean eglReleaseTexImage(EGLDisplay dpy, EGLSurface surface, EGLint buffer);

EGLBoolean eglSwapInterval(EGLDisplay dpy, EGLint interval);

EGLContext eglCreateContext(EGLDisplay dpy, EGLConfig config,
                            EGLContext share_context,
                            const EGLint *attrib_list)
{
    egl *e = NULL;
    egl_display *display = NULL;

    uapi_unused(share_context);
    uapi_unused(attrib_list);

    egl_get_display(dpy, EGL_NO_CONTEXT);
    egl_if_error_return(display == NULL, EGL_NOT_INITIALIZED, EGL_NO_CONTEXT);

    egl_context *c = new_object(egl_context, config);
    egl_if_error_return(c == NULL, EGL_BAD_ALLOC, EGL_NO_CONTEXT);

    add_ref(c);
    display->add_context(display, c);

    if (e->get_current_context(e) == NULL) {
        e->set_current_context(e, c);
        add_ref(c);
    }
    egl_return(EGL_SUCCESS, (EGLContext)c);
}

EGLBoolean eglDestroyContext(EGLDisplay dpy, EGLContext ctx)
{
    egl *e = NULL;
    egl_display *display = NULL;
    egl_get_display(dpy, EGL_FALSE);
    egl_if_error_return(display == NULL, EGL_NOT_INITIALIZED, EGL_FALSE);
    egl_if_error_return(ctx == NULL, EGL_BAD_CONFIG, EGL_FALSE);
    egl_if_error_return(display->context_exists(display, ctx) == TD_FALSE, EGL_BAD_CONFIG, EGL_FALSE);

    egl_context *c = (egl_context *)ctx;
    display->remove_context(display, c);
    if (remove_ref(c) == 0) {
        delete_object(c);
    }
    egl_return(EGL_SUCCESS, EGL_TRUE);
}

EGLBoolean eglMakeCurrent(EGLDisplay dpy, EGLSurface draw,
                          EGLSurface read, EGLContext ctx)
{
    egl *e = NULL;
    egl_display *display = NULL;
    egl_get_display(dpy, EGL_FALSE);
    egl_if_error_return(display == NULL, EGL_NOT_INITIALIZED, EGL_FALSE);
    egl_if_error_return(draw != read, EGL_BAD_MATCH, EGL_FALSE);
    egl_if_error_return(ctx != EGL_NO_CONTEXT && display->context_exists(display, ctx) == VG_FALSE,
        EGL_BAD_CONFIG, EGL_FALSE);

    egl_context *c = e->get_current_context(e);
    egl_surface *s = e->get_current_surface(e);

    egl_context *cx = (egl_context *)(uintptr_t)ctx;
    egl_surface *sx = (egl_surface *)(uintptr_t)draw;

    if (c != cx) {
        if ((c != NULL) && (remove_ref(c) == 0)) {
            delete_object(c);
        }
        e->set_current_context(e, cx);
        if (cx != NULL) {
            add_ref(cx);
        }
    }
    if (s != sx) {
        if ((s != NULL) && (remove_ref(s) == 0)) {
            delete_object(s);
        }
        e->set_current_surface(e, sx);
        if (sx != NULL) {
            add_ref(sx);
        }
    }

    egl_return(EGL_SUCCESS, EGL_TRUE);
}

EGLContext eglGetCurrentContext(void);
EGLSurface eglGetCurrentSurface(EGLint readdraw);
EGLDisplay eglGetCurrentDisplay(void);
EGLBoolean eglQueryContext(EGLDisplay dpy, EGLContext ctx,
                           EGLint attribute, EGLint *value);

EGLBoolean eglWaitGL(void);
EGLBoolean eglWaitNative(EGLint engine);
EGLBoolean eglSwapBuffers(EGLDisplay dpy, EGLSurface surface);
EGLBoolean eglCopyBuffers(EGLDisplay dpy, EGLSurface surface,
                          NativePixmapType target);

void       (*eglGetProcAddress(const char *procname))(void);

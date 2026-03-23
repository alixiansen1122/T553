/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: egl display file
 */

#include "egl_display.h"
#include <stdlib.h>
#include <string.h>
#include <securec.h>
#include "vg_debug.h"

static void egl_display_add_reference(egl_display *thiz)
{
    thiz->ref_count++;
}

static int egl_display_remove_reference(egl_display *thiz)
{
    thiz->ref_count--;
    if (thiz->ref_count < 0) {
        thiz->ref_count = 0;
    }
    return thiz->ref_count;
}

static void egl_display_add_surface(egl_display *thiz, egl_surface *srf)
{
    osal_list_add_tail(&thiz->surface_list.node, &srf->node);
}

static void egl_display_remove_surface(const egl_display *thiz, egl_surface *srf)
{
    uapi_unused(thiz);

    osal_list_del(&srf->node);
}

static void egl_display_add_context(egl_display *thiz, egl_context *ctx)
{
    osal_list_add_tail(&thiz->context_list.node, &ctx->node);
}

static void egl_display_remove_context(const egl_display *thiz, egl_context *ctx)
{
    uapi_unused(thiz);

    osal_list_del(&ctx->node);
}

static EGLBoolean egl_display_context_exists(egl_display *disp, const EGLContext ctx)
{
    egl_context *c = NULL;
    osal_list_for_each_entry(c, &disp->context_list.node, node) {
        if (c == ctx) {
            return EGL_TRUE;
        }
    }
    return EGL_FALSE;
}

static EGLBoolean egl_display_surface_exists(egl_display *disp, const EGLSurface srf)
{
    egl_surface *s = NULL;
    osal_list_for_each_entry(s, &disp->surface_list.node, node) {
        if (s == srf) {
            return EGL_TRUE;
        }
    }
    return EGL_FALSE;
}

int egl_display_init(egl_display *d, EGLDisplay id)
{
    if (d == NULL) {
        vg_err("null ptr");
        return EGL_BAD_PARAMETER;
    }
    init_object_common_private_member(egl_display, d, VG_MAGIC_EGL_DISPLAY);
    d->id = id;
    OSAL_INIT_LIST_HEAD(&d->surface_list.node);
    OSAL_INIT_LIST_HEAD(&d->context_list.node);

    d->add_surface = egl_display_add_surface;
    d->remove_surface = egl_display_remove_surface;
    d->add_context = egl_display_add_context;
    d->remove_context = egl_display_remove_context;

    d->context_exists = egl_display_context_exists;
    d->surface_exists = egl_display_surface_exists;
    return EGL_SUCCESS;
}

static void egl_display_deinit_surface_list(egl_display *d)
{
    egl_surface *s = NULL;
    egl_surface *n = NULL;
    osal_list_for_each_entry_safe(s, n, &d->surface_list.node, node) {
        osal_list_del(&s->node);
        delete_object(s);
    }
}

static void egl_display_deinit_context_list(egl_display *d)
{
    egl_context *c = NULL;
    egl_context *n = NULL;
    osal_list_for_each_entry_safe(c, n, &d->context_list.node, node) {
        osal_list_del(&c->node);
        delete_object(c);
    }
}

static void egl_display_deinit(egl_display *d)
{
    egl_display_deinit_surface_list(d);
    egl_display_deinit_context_list(d);
}

egl_display *egl_display_new(EGLDisplay id)
{
    egl_display *d = (egl_display *)malloc(sizeof(egl_display));
    if (d == NULL) {
        vg_err("malloc failed");
        return NULL;
    }
    (void)memset_s(d, sizeof(*d), 0, sizeof(egl_display));
    egl_display_init(d, id);
    return d;
}

void egl_display_delete(egl_display *d)
{
    if (d == NULL) {
        return;
    }
    egl_display_deinit(d);

    if (d->ref_count != 0) {
        vg_err("display ref count not zero:%d\n", d->ref_count);
    }
    free(d);
}

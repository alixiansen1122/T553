/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: egl inner file
 */

#include "egl_egl.h"
#include <stdlib.h>
#include <string.h>
#include <securec.h>
#include "vg_debug.h"

static void egl_delete(egl *e);

static egl *g_egl = NULL;

static void egl_add_reference(egl *thiz)
{
    thiz->ref_count++;
}

static int egl_remove_reference(egl *thiz)
{
    thiz->ref_count--;
    if (thiz->ref_count < 0) {
        thiz->ref_count = 0;
    }
    return thiz->ref_count;
}

static void egl_add_display(egl *thiz, egl_display *disp)
{
    osal_list_add_tail(&thiz->display_list.node, &disp->node);
}

static void egl_remove_display(const egl *thiz, egl_display *disp)
{
    uapi_unused(thiz);

    osal_list_del(&disp->node);
}

static egl_display *egl_get_egl_display(egl *thiz, const EGLDisplay *disp_id)
{
    egl_display *d = NULL;
    osal_list_for_each_entry(d, &thiz->display_list.node, node) {
        if (d->id == disp_id) {
            return d;
        }
    }
    return NULL;
}

/* return EGLDisplay for the current context */
static EGLDisplay egl_find_dispaly(egl *thiz, const EGLContext ctx)
{
    egl_display *d = NULL;
    osal_list_for_each_entry(d, &thiz->display_list.node, node) {
        if (d->context_exists(d, ctx)) {
            return d->id;
        }
    }
    return EGL_NO_DISPLAY;
}

static void egl_set_error(egl *e, EGLint error)
{
    e->error = error;
}

static EGLint egl_get_error(egl *e)
{
    return e->error;
}

static void egl_set_current_context(egl *e, egl_context *c)
{
    e->current_context = c;
}

static void egl_set_current_surface(egl *e, egl_surface *s)
{
    e->current_surface = s;
}

static egl_context *egl_get_current_context(egl *e)
{
    return e->current_context;
}

static egl_surface *egl_get_current_surface(egl *e)
{
    return e->current_surface;
}

static void egl_init(egl *e)
{
    init_object_common_private_member(egl, e, VG_MAGIC_EGL_EGL);

    OSAL_INIT_LIST_HEAD(&e->display_list.node);

    e->add_display = egl_add_display;
    e->remove_display = egl_remove_display;
    e->get_display = egl_get_egl_display;
    e->find_dispaly = egl_find_dispaly;
    e->set_error = egl_set_error;
    e->get_error = egl_get_error;
    e->set_current_context = egl_set_current_context;
    e->set_current_surface = egl_set_current_surface;
    e->get_current_context = egl_get_current_context;
    e->get_current_surface = egl_get_current_surface;

    e->current_context = NULL;
    e->current_surface = NULL;

    e->error = EGL_SUCCESS;
}

static void egl_deinit(egl *e)
{
    egl_display *d = NULL;
    egl_display *n = NULL;
    osal_list_for_each_entry_safe(d, n, &e->display_list.node, node) {
        osal_list_del(&d->node);
        delete_object(d);
    }
}

static egl *egl_new(void)
{
    egl *e = (egl *)malloc(sizeof(egl));
    if (e == NULL) {
        vg_err("malloc failed");
        return e;
    }
    (void)memset_s(e, sizeof(*e), 0, sizeof(egl));
    egl_init(e);
    return e;
}

static void egl_delete(egl *e)
{
    if (e == NULL) {
        return;
    }
    egl_deinit(e);

    if (e->ref_count != 0) {
        vg_err("display ref count not zero:%d\n", e->ref_count);
    }
    free(e);
}

void release_egl(void)
{
    if (g_egl != NULL) {
        if (remove_ref(g_egl) == 0) {
            delete_object(g_egl);
            g_egl = NULL;
        }
    }
}

egl *get_egl(void)
{
    if (g_egl == NULL) {
        g_egl = new_object(egl);
        if (g_egl != NULL) {
            add_ref(g_egl);
        }
    }
    return g_egl;
}

/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: egl context file
 */

#include "egl_context.h"
#include <stdlib.h>
#include <string.h>
#include <securec.h>
#include "vg_debug.h"

static void egl_context_add_reference(egl_context *thiz)
{
    thiz->ref_count++;
}

static EGLint egl_context_remove_reference(egl_context *thiz)
{
    thiz->ref_count--;
    if (thiz->ref_count < 0) {
        thiz->ref_count = 0;
    }
    return thiz->ref_count;
}

EGLint egl_context_init(egl_context *c, const egl_config *config)
{
    if (c == NULL) {
        vg_err("null ptr");
        return EGL_BAD_PARAMETER;
    }
    init_object_common_private_member(egl_context, c, VG_MAGIC_EGL_CONTEXT);
    c->context = new_object(vg_context);
    if (c->context == NULL) {
        vg_err("malloc failed");
        return EGL_BAD_ALLOC;
    }
    c->config = config;
    return EGL_SUCCESS;
}

egl_context *egl_context_new(const egl_config *config)
{
    egl_context *c = (egl_context *)malloc(sizeof(egl_context));
    if (c == NULL) {
        vg_err("malloc failed");
        return NULL;
    }
    memset_s(c, sizeof(*c), 0, sizeof(egl_context));
    int ret = egl_context_init(c, config);
    if (ret != EGL_SUCCESS) {
        free(c);
        vg_err("call egl_context init failed");
        return NULL;
    }
    return c;
}

void egl_context_delete(egl_context *c)
{
    if (c == NULL) {
        return;
    }

    if (c->ref_count != 0) {
        vg_err("context ref count not zero:%d\n", c->ref_count);
    }
    vg_context_delete(c->context);
    free(c);
}

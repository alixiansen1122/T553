/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: egl surface file
 */

#include "egl_surface.h"
#include <stdlib.h>
#include <string.h>
#include <securec.h>
#include "vg_debug.h"

static void egl_surface_add_reference(egl_surface *thiz)
{
    thiz->ref_count++;
}

static int egl_surface_remove_reference(egl_surface *thiz)
{
    thiz->ref_count--;
    if (thiz->ref_count < 0) {
        thiz->ref_count = 0;
    }
    return thiz->ref_count;
}

int egl_surface_init(egl_surface *s, const EGLConfig config)
{
    if (s == NULL) {
        vg_err("null ptr");
        return EGL_BAD_PARAMETER;
    }
    init_object_common_private_member(egl_surface, s, VG_MAGIC_EGL_SURFACE);
    s->config = config;
    return EGL_SUCCESS;
}

egl_surface *egl_surface_new(const EGLConfig config)
{
    egl_surface *s = (egl_surface *)malloc(sizeof(egl_surface));
    if (s == NULL) {
        vg_err("malloc failed");
        return NULL;
    }
    (void)memset_s(s, sizeof(*s), 0, sizeof(egl_surface));
    egl_surface_init(s, config);
    return s;
}

void egl_surface_delete(egl_surface *s)
{
    if (s == NULL) {
        return;
    }

    if (s->ref_count != 0) {
        vg_err("surface ref count not zero:%d\n", s->ref_count);
    }
    free(s);
}

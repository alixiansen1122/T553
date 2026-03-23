/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description : egl context file
 */

#ifndef API_OPENVG_EGL_CONTEXT_H
#define API_OPENVG_EGL_CONTEXT_H

#include "egl_config.h"
#include "vg_context.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef struct egl_context_tag egl_context;
struct egl_context_tag {
    vg_magic magic;
    struct osal_list_head node;
    int ref_count;
    void (*add_ref)(egl_context *thiz);
    int  (*remove_ref)(egl_context *thiz);
    void (*free)(egl_context *thiz);
    vg_context *context;
    const egl_config *config;
};

int egl_context_init(egl_context *c, const egl_config *config);
egl_context *egl_context_new(const egl_config *config);
void egl_context_delete(egl_context *c);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif /* API_OPENVG_EGL_CONTEXT_H */

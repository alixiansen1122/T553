/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description : openvg common file
 */

#ifndef API_OPENVG_COMMON_H
#define API_OPENVG_COMMON_H

#include <stdio.h>
#include "egl_mutex.h"
#include "osal_list.h"
#include "egl_egl.h"
#include "egl_context.h"
#include "vg_debug.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define VG_NO_RETVAL

#define vg_get_context(ret_val) do { \
    egl_acquire_mutex(); \
    e = get_egl(); \
    if (e == NULL) { \
        egl_release_mutex(); \
        vg_err("egl not inited"); \
        return ret_val; \
    } \
    egl_context *pc = (egl_context*)e->get_current_context(e); \
    if (pc == NULL) { \
        egl_release_mutex(); \
        vg_err("no egl context"); \
        return ret_val; \
    } \
    context = pc->context; \
} while (0)

#define vg_if_error_return(cond, error_code, ret_val) do { \
    if (cond) { \
        context->set_error(context, error_code); \
        egl_release_mutex(); \
        vg_err("in condition: %s,err code:0x%x", #cond, error_code); \
        return ret_val; \
    } \
} while (0)

#define vg_return(ret_val) do { \
    egl_release_mutex(); \
    return ret_val; \
} while (0)

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif /* API_OPENVG_COMMON_H */

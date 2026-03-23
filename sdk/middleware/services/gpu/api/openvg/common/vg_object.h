/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description : openvg common file
 */

#ifndef API_OPENVG_COMMON_OBJECT_H
#define API_OPENVG_COMMON_OBJECT_H

#include "osal_list.h"
#include "egl_mutex.h"
#include "vg_debug.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define VG_MAGIC_MASK 0xFFFFFF00
#define VG_TYPE_MASK  0xFF

typedef enum {
    VG_TYPE_RESOURCE_IMAGE,
    VG_TYPE_RESOURCE_PATH,
    VG_TYPE_RESOURCE_PAINT,
    VG_TYPE_RESOURCE_FONT,

    VG_TYPE_RESOURCE_MANAGER,

    VG_TYPE_RESOURCE_MAX,
} vg_resource_type;

typedef enum {
    VG_TYPE_OBJ_EGL,
    VG_TYPE_OBJ_EGL_CONTEXT,
    VG_TYPE_OBJ_EGL_SURFACE,
    VG_TYPE_OBJ_EGL_DISPLAY,

    VG_TYPE_OBJ_VG_CONTEXT,

    VG_TYPE_OBJ_MAX,
} vg_object_type;

#define VG_MAGIC_EGL          ((('E') << 24) | (('G') << 16) | (('L') << 8))
#define VG_MAGIC_VG           ((('O' + 'P') << 24) | (('E' + 'N') << 16) | (('V' + 'G') << 8))
#define VG_MAGIC_RESOURCE     ((('R' + 'E') << 24) | (('S' + 'O' + 'U') << 16) | (('R' + 'C' + 'E') << 8))

typedef enum {
    VG_MAGIC_RESOURCE_IMAGE = VG_MAGIC_RESOURCE | VG_TYPE_RESOURCE_IMAGE,
    VG_MAGIC_RESOURCE_PATH  = VG_MAGIC_RESOURCE | VG_TYPE_RESOURCE_PATH,
    VG_MAGIC_RESOURCE_PAINT = VG_MAGIC_RESOURCE | VG_TYPE_RESOURCE_PAINT,
    VG_MAGIC_RESOURCE_FONT  = VG_MAGIC_RESOURCE | VG_TYPE_RESOURCE_FONT,
    VG_MAGIC_RESOURCE_MANAGER  = VG_MAGIC_RESOURCE | VG_TYPE_RESOURCE_MANAGER,

    VG_MAGIC_EGL_EGL        = VG_MAGIC_EGL | VG_TYPE_OBJ_EGL,
    VG_MAGIC_EGL_CONTEXT    = VG_MAGIC_EGL | VG_TYPE_OBJ_EGL_CONTEXT,
    VG_MAGIC_EGL_SURFACE    = VG_MAGIC_EGL | VG_TYPE_OBJ_EGL_SURFACE,
    VG_MAGIC_EGL_DISPLAY    = VG_MAGIC_EGL | VG_TYPE_OBJ_EGL_DISPLAY,

    VG_MAGIC_VG_CONTEXT     = VG_MAGIC_VG  | VG_TYPE_OBJ_VG_CONTEXT,
} vg_magic;

#define interface_add_reference_name(t) t##_add_reference
#define interface_remove_reference_name(t) t##_remove_reference
#define interface_free_name(t) t##_delete

#define init_object_common_private_member(t, obj, m) do { \
    (obj)->magic = m; \
    OSAL_INIT_LIST_HEAD(&(obj)->node); \
    (obj)->add_ref  = interface_add_reference_name(t); \
    (obj)->remove_ref = interface_remove_reference_name(t); \
    (obj)->free = interface_free_name(t); \
} while (0)

#define new_object(t, ...)  t##_new(__VA_ARGS__)
#define delete_object(o)    (o)->free(o)

#define add_ref(o)    (vg_dbg("add ref: %d", ((o)->ref_count) + 1), (o)->add_ref(o))
#define remove_ref(o) (vg_dbg("rem ref: %d", ((o)->ref_count) - 1), (o)->remove_ref(o))

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif /* API_OPENVG_COMMON_OBJECT_H */

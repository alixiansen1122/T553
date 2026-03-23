/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description : openvg resource manager file
 */

#ifndef API_OPENVG_RESOURCE_MANAGER_H
#define API_OPENVG_RESOURCE_MANAGER_H

#include "osal_list.h"
#include "VG/openvg.h"
#include "vg_object.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef struct vg_resource_tag vg_resource;
struct vg_resource_tag {
    vg_magic magic;
    struct osal_list_head node;
    int ref_count;
    void (*add_ref)(vg_resource *thiz);
    int  (*remove_ref)(vg_resource *thiz);
    void (*free)(vg_resource *thiz);
};

typedef struct vg_resource_manager_tag vg_resource_manager;
struct vg_resource_manager_tag {
    vg_magic magic;
    struct osal_list_head node;
    int ref_count;
    void (*add_ref)(vg_resource_manager *thiz);
    int  (*remove_ref)(vg_resource_manager *thiz);
    void (*free)(vg_resource_manager *thiz);

    vg_resource resource_list;

    void (*add_resource)(vg_resource_manager *thiz, vg_resource *resource);
    void  (*remove_resource)(vg_resource_manager *thiz, vg_resource *resource);
    VGboolean (*is_valid)(vg_resource_manager *thiz, vg_resource *resource);
    vg_resource *(*get_first_resource)(vg_resource_manager *thiz);
};

vg_resource_type get_resource_type(const vg_resource *resource);
int vg_resource_manager_init(vg_resource_manager *m);
vg_resource_manager *vg_resource_manager_new(void);
void vg_resource_manager_delete(vg_resource_manager *manager);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif /* API_OPENVG_RESOURCE_MANAGER_H */
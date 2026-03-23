/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: openvg context adapter source file
 */

#include "vg_resource.h"
#include <stdlib.h>
#include <string.h>
#include <securec.h>
#include "VG/openvg.h"
#include "vg_debug.h"
#include "vg_object.h"

static void vg_resource_manager_add_reference(vg_resource_manager *thiz)
{
    thiz->ref_count++;
}

static int vg_resource_manager_remove_reference(vg_resource_manager *thiz)
{
    thiz->ref_count--;
    if (thiz->ref_count < 0) {
        thiz->ref_count = 0;
    }
    return thiz->ref_count;
}

static void resource_manager_add_resource(vg_resource_manager *manager, vg_resource *resource)
{
    if (manager == NULL) {
        vg_err("manager is null");
        return;
    }
    osal_list_add(&resource->node, &manager->resource_list.node);
    add_ref(resource);
}

static void resource_manager_remove_resource(vg_resource_manager *manager, vg_resource *resource)
{
    if (manager == NULL) {
        vg_err("manager is null");
        return;
    }
    VGboolean found =  VG_FALSE;
    vg_resource *r = NULL;
    vg_resource *n = NULL;
    osal_list_for_each_entry_safe(r, n, &manager->resource_list.node, node) {
        if (r == resource) {
            osal_list_del(&r->node);
            found = VG_TRUE;
            break;
        }
    }
    if (found != VG_FALSE) {
        if (remove_ref(r) == 0) {
            delete_object(r);
        }
    }
}

static VGboolean resource_manager_is_valid_resource(vg_resource_manager *manager, vg_resource *resource)
{
    if (manager == NULL) {
        vg_err("manager is null");
        return VG_FALSE;
    }
    vg_resource *r = NULL;
    osal_list_for_each_entry(r, &manager->resource_list.node, node) {
        if (r == resource) {
            return VG_TRUE;
        }
    }
    return VG_FALSE;
}

static vg_resource *resource_manager_get_first_resource(vg_resource_manager *manager)
{
    if (manager == NULL) {
        vg_err("manager is null");
        return NULL;
    }
    vg_resource *r = NULL;
    osal_list_for_each_entry(r, &manager->resource_list.node, node) {
        return r;
    }
    return NULL;
}

int vg_resource_manager_init(vg_resource_manager *m)
{
    if (m == NULL) {
        vg_err("manager is null");
        return VG_ILLEGAL_ARGUMENT_ERROR;
    }

    init_object_common_private_member(vg_resource_manager, m, VG_MAGIC_RESOURCE_MANAGER);

    OSAL_INIT_LIST_HEAD(&m->resource_list.node);

    m->add_resource = resource_manager_add_resource;
    m->remove_resource = resource_manager_remove_resource;
    m->is_valid = resource_manager_is_valid_resource;
    m->get_first_resource = resource_manager_get_first_resource;

    return VG_NO_ERROR;
}

vg_resource_manager *vg_resource_manager_new(void)
{
    vg_resource_manager *m = (vg_resource_manager *)malloc(sizeof(vg_resource_manager));
    if (m == NULL) {
        vg_err("malloc %d failed", sizeof(vg_resource_manager));
        return NULL;
    }
    (void)memset_s(m, sizeof(*m), 0, sizeof(vg_resource_manager));

    vg_resource_manager_init(m);

    return m;
}

void vg_resource_manager_delete(vg_resource_manager *manager)
{
    if (manager == NULL) {
        vg_err("manager is null");
        return;
    }
    vg_resource *i;
    while ((i = manager->get_first_resource(manager)) != NULL) {
        manager->remove_resource(manager, i);
    }
    if (manager->ref_count == 0) {
        free(manager);
    }
}

vg_resource_type get_resource_type(const vg_resource *resource)
{
    switch (resource->magic) {
        case VG_MAGIC_RESOURCE_IMAGE:
            return VG_TYPE_RESOURCE_IMAGE;
        case VG_MAGIC_RESOURCE_PATH:
            return VG_TYPE_RESOURCE_PATH;
        case VG_MAGIC_RESOURCE_PAINT:
            return VG_TYPE_RESOURCE_PAINT;
        case VG_MAGIC_RESOURCE_FONT:
            return VG_TYPE_RESOURCE_FONT;
        default:
            vg_err("invalid resource:0x%x", resource->magic);
            return VG_TYPE_RESOURCE_MAX;
    }
}
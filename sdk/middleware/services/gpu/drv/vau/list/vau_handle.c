/*
 * Copyright (c) @CompanyNameMagicTag 2021-2023. All rights reserved.
 * Description: handle manage
 */

#include "vau_handle.h"
#include "vau_mem.h"
#include "vau_dfx.h"

/*-------------------------------- macro define -------------------------------------------*/

#define VAU_MAX_HANDLE_VALUE 0x7fffffff

/*-------------------------------- struct define ------------------------------------------*/

/*-------------------------------- func declares ------------------------------------------*/

/*-------------------------------- par define ---------------------------------------------*/

static vau_handle_mgr *g_vau_handle_list = TD_NULL;

/*-------------------------------- func release -------------------------------------------*/

/* run only once for insmod ko */
td_s32 vau_handle_list_init(td_void)
{
    td_s32 ret;

    if (g_vau_handle_list != TD_NULL) {
        return EXT_FAILURE;
    }

    g_vau_handle_list = (vau_handle_mgr *)osal_kmalloc(sizeof(vau_handle_mgr), OSAL_GFP_KERNEL);
    if (g_vau_handle_list == TD_NULL) {
        vau_err("call osal_kmalloc failed");
        return EXT_FAILURE;
    }
    (td_void)memset_s(g_vau_handle_list, sizeof(*g_vau_handle_list), 0x0, sizeof(vau_handle_mgr));

    OSAL_INIT_LIST_HEAD(&g_vau_handle_list->list_head);

    ret = osal_spin_lock_init(&g_vau_handle_list->lock);
    if (ret != 0) {
        osal_kfree(g_vau_handle_list);
        g_vau_handle_list = TD_NULL;
        vau_err("call osal_spin_lock_init failed");
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

/* run only once for rmmod ko */
td_void vau_handle_list_deinit(td_void)
{
    if (g_vau_handle_list == TD_NULL) {
        return;
    }

    osal_spin_lock_destroy(&g_vau_handle_list->lock);

    osal_kfree(g_vau_handle_list);

    g_vau_handle_list = TD_NULL;

    return;
}

vau_handle_mgr *vau_handle_list_get(td_void)
{
    return g_vau_handle_list;
}

vau_handle_mgr *vau_handle_create(td_u32 size)
{
    td_size_t lockflags = 0;
    vau_handle_mgr *handle_mgr = TD_NULL;

    handle_mgr = (vau_handle_mgr *)vau_mem_alloc(VAU_MEM_TYPE_HANDLE_MNG, size);
    if (handle_mgr == TD_NULL) {
        vau_err("call vau_mem_alloc failed");
        return TD_NULL;
    }
    (td_void)memset_s(handle_mgr, size, 0x0, size);

    osal_spin_lock_irqsave(&g_vau_handle_list->lock, &lockflags);
    {
        g_vau_handle_list->handle = (g_vau_handle_list->handle + 1) % VAU_MAX_HANDLE_VALUE;
        g_vau_handle_list->handle = (g_vau_handle_list->handle == 0) ? (1) : (g_vau_handle_list->handle);
        handle_mgr->handle = g_vau_handle_list->handle;
        osal_list_add_tail(&handle_mgr->list_head, &g_vau_handle_list->list_head);
    }
    osal_spin_unlock_irqrestore(&g_vau_handle_list->lock, &lockflags);

    return handle_mgr;
}

vau_handle_mgr *vau_handle_search(td_handle handle)
{
    td_size_t lockflags = 0;
    vau_handle_mgr *handle_mgr = TD_NULL;

    osal_spin_lock_irqsave(&g_vau_handle_list->lock, &lockflags);
    osal_list_for_each_entry(handle_mgr, &g_vau_handle_list->list_head, list_head) {
        if ((handle_mgr != TD_NULL) && (handle == handle_mgr->handle)) {
            osal_spin_unlock_irqrestore(&g_vau_handle_list->lock, &lockflags);
            return handle_mgr;
        }
    }
    osal_spin_unlock_irqrestore(&g_vau_handle_list->lock, &lockflags);

    return TD_NULL;
}

td_void vau_handle_del(td_handle handle)
{
    vau_handle_mgr *handle_mgr = vau_handle_search(handle);
    if (handle_mgr != TD_NULL) {
        osal_list_del_init(&handle_mgr->list_head);
        vau_mem_free(VAU_MEM_TYPE_HANDLE_MNG, (td_uchar *)handle_mgr);
    }
    return;
}

td_void vau_handle_destroy(td_handle handle)
{
    vau_handle_del(handle);
    return;
}

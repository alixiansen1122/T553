/*
 * Copyright (c) @CompanyNameMagicTag 2021-2023. All rights reserved.
 * Description: handle manage
 */

#ifndef GPU_DRV_VAU_HANDLE_H
#define GPU_DRV_VAU_HANDLE_H

#include "td_base.h"
#include "soc_osal.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*-------------------------------- macro define -------------------------------------------*/

#define VAU_HANDLE_INVALID  0xFFFFFFFF

/*-------------------------------- struct define ------------------------------------------*/

typedef struct {
    struct osal_list_head list_head;
    td_void *res;
    td_handle handle;
    osal_spinlock lock;
} vau_handle_mgr;

/*-------------------------------- func declares ------------------------------------------*/
td_s32 vau_handle_list_init(td_void);
td_void vau_handle_list_deinit(td_void);
vau_handle_mgr *vau_handle_list_get(td_void);

vau_handle_mgr *vau_handle_create(td_u32 size);
vau_handle_mgr *vau_handle_search(td_handle handle);
td_void vau_handle_del(td_handle handle);
td_void vau_handle_destroy(td_handle handle);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* GPU_DRV_VAU_HANDLE_H */

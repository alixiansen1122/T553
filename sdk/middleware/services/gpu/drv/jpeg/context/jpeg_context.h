/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: jpeg context
 */

#ifndef DRV_GRAPHICS_JPEG_CONTEXT_H
#define DRV_GRAPHICS_JPEG_CONTEXT_H

#include "soc_osal.h"
#include "jpeg_dfx.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*-------------------------------- macro define -------------------------------------------*/

/*-------------------------------- struct define ------------------------------------------*/

typedef struct {
    td_bool is_lock;
    td_u32 int_type;
    td_u32 init_count;
    osal_atomic ref_count;
    volatile td_u32 *reg_base;
    osal_spinlock lock;
    osal_wait wait_int;
} drv_jpeg_context;

/*-------------------------------- func declares ------------------------------------------*/

td_void jpeg_context_init(td_void);
td_void jpeg_context_deinit(td_void);
td_void jpeg_context_lock(td_void);
td_void jpeg_context_unlock(td_void);

drv_jpeg_context *jpeg_context_get(td_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* DRV_GRAPHICS_JPEG_CONTEXT_H */

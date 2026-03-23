/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: jpeg context
 */

#include "jpeg_context.h"
#include <securec.h>
#include "soc_osal.h"
#include "jpeg_mem.h"
#include "jpeg_reg_define.h"
#include "jpeg_hal.h"

/*-------------------------------- macro define -------------------------------------------*/

/*-------------------------------- struct define ------------------------------------------*/

/*-------------------------------- func declares ------------------------------------------*/

/*-------------------------------- par define ---------------------------------------------*/

static osal_semaphore g_jpeg_mutex;

static drv_jpeg_context *g_jpeg_context = TD_NULL;

/*-------------------------------- func release -------------------------------------------*/

td_void jpeg_context_init(td_void)
{
    td_s32 ret;

    if (g_jpeg_context != TD_NULL) {
        g_jpeg_context->init_count = 1;
        return;
    }

    g_jpeg_context = (drv_jpeg_context *)osal_kmalloc(sizeof(drv_jpeg_context), OSAL_GFP_KERNEL);
    if (g_jpeg_context == TD_NULL) {
        jpeg_err("call jpeg_mem_small_alloc failure\n");
        return;
    }

    ret = memset_s(g_jpeg_context, sizeof(drv_jpeg_context), 0x0, sizeof(drv_jpeg_context));
    if (ret != EOK) {
        osal_kfree((td_void *)g_jpeg_context);
        g_jpeg_context = TD_NULL;
        jpeg_err("call memset_s failure\n");
        return;
    }

    g_jpeg_context->reg_base = (volatile td_u32 *)osal_ioremap_nocache(JPEG_REG_BASEADDR, JPEG_REG_LENGTH);
    if (g_jpeg_context->reg_base == TD_NULL) {
        osal_kfree((td_void *)g_jpeg_context);
        g_jpeg_context = TD_NULL;
        jpeg_err("call ioremap_nocache failure\n");
        return;
    }

    jpeg_hal_init(g_jpeg_context->reg_base);

    g_jpeg_context->is_lock = TD_FALSE;
    g_jpeg_context->init_count = 0;

    osal_sem_init(&g_jpeg_mutex, 1);
    osal_wait_init(&g_jpeg_context->wait_int);
    osal_spin_lock_init(&g_jpeg_context->lock);

    return;
}

td_void jpeg_context_deinit(td_void)
{
    if (g_jpeg_context == TD_NULL) {
        return;
    }

    if (g_jpeg_context->reg_base != TD_NULL) {
        osal_iounmap((td_void*)g_jpeg_context->reg_base, JPEG_REG_LENGTH);
    }

    jpeg_hal_deinit();

    osal_kfree((td_void *)g_jpeg_context);
    g_jpeg_context = TD_NULL;
    return;
}

td_void jpeg_context_lock(td_void)
{
    td_ulong lock_flag = 0;

    if (osal_sem_down_interruptible(&g_jpeg_mutex) != 0) {
        jpeg_err("call down_interruptible failure\n");
        return;
    }

    osal_spin_lock_irqsave(&g_jpeg_context->lock, &lock_flag);
    g_jpeg_context->is_lock = TD_TRUE;
    osal_spin_unlock_irqrestore(&g_jpeg_context->lock, &lock_flag);
    return;
}

td_void jpeg_context_unlock(td_void)
{
    td_ulong lock_flag = 0;

    osal_spin_lock_irqsave(&g_jpeg_context->lock, &lock_flag);
    if (g_jpeg_context->is_lock == TD_FALSE) {
        osal_spin_unlock_irqrestore(&g_jpeg_context->lock, &lock_flag);
        return;
    }
    osal_spin_unlock_irqrestore(&g_jpeg_context->lock, &lock_flag);

    osal_sem_up(&g_jpeg_mutex);

    osal_spin_lock_irqsave(&g_jpeg_context->lock, &lock_flag);
    g_jpeg_context->is_lock = TD_FALSE;
    osal_spin_unlock_irqrestore(&g_jpeg_context->lock, &lock_flag);

    return;
}

drv_jpeg_context *jpeg_context_get(td_void)
{
    return g_jpeg_context;
}

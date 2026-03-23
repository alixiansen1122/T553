/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: isr
 */

#include "vau_isr.h"
#include "drv_gfx_port.h"
#include "vau_dfx.h"
#include "vau_list.h"
#include "vau_hal.h"

/*-------------------------------- macro define -------------------------------------------*/

/*-------------------------------- struct define ------------------------------------------*/

typedef struct {
    td_u32 data;
    osal_spinlock isr_lock;
    osal_workqueue task_workqueue;
} vau_isr_info;

/*-------------------------------- func declares ------------------------------------------*/

static td_void vau_isr_dfx_inc_cnt(const td_u32 int_status);

/*-------------------------------- par define ---------------------------------------------*/

static vau_isr_info g_vau_isr_info = {0};

/*-------------------------------- func release -------------------------------------------*/

static td_void vau_tasklet_func(osal_workqueue *work)
{
    td_u32 int_status;
    unsigned long flags = 0;

    uapi_unused(work);

    vau_dfx_info *vau_info = vau_dfx_get_info();
    vau_info->func_info.func_cnt.tasklet_func_cnt++;

    osal_spin_lock_irqsave(&g_vau_isr_info.isr_lock, &flags);
    int_status = g_vau_isr_info.data;
    g_vau_isr_info.data &= (~int_status);
    osal_spin_unlock_irqrestore(&g_vau_isr_info.isr_lock, &flags);

    vau_list_node_end_process(int_status);
#ifndef CONFIG_VAU_DEAL_INT_ONCE
    if (vau_is_err_int(int_status)) {
        osal_irq_enable(VAU_INT_NUM);
    }
#endif

    return;
}

static td_void vau_isr_main(td_u32 param)
{
    td_u32 int_status;
    unsigned long flags = 0;

    uapi_unused(param);

    int_status = vau_hal_get_int_state();
    vau_hal_clear_int_state(int_status);
    vau_isr_dfx_inc_cnt(int_status);

    if (int_status == DRV_VAU_INT_NONE) {
        /* maybe exist problem */
        return;
    } else if ((int_status & DRV_VAU_INT_TUNL_DONE) == DRV_VAU_INT_TUNL_DONE) {
        vau_list_node_lowdelay_ready();
    }
#ifndef CONFIG_VAU_DEAL_INT_ONCE
    if (vau_is_err_int(int_status)) {
        osal_irq_disable(VAU_INT_NUM);
    }
#endif

    osal_spin_lock_irqsave(&g_vau_isr_info.isr_lock, &flags);
    g_vau_isr_info.data = g_vau_isr_info.data | (int_status);
    osal_spin_unlock_irqrestore(&g_vau_isr_info.isr_lock, &flags);

    osal_workqueue_schedule(&g_vau_isr_info.task_workqueue);
}

static td_void vau_isr_dfx_inc_cnt(const td_u32 int_status)
{
    vau_dfx_info *vau_info = vau_dfx_get_info();
    vau_info->func_info.func_cnt.isr_cnt++;

    if (int_status == 0x0) {
        vau_info->func_info.isr_cnt.isr_status_zero_value_cnt++;
    }

    if ((int_status & DRV_VAU_INT_NODE_COMPLETE) != 0) {
        vau_info->func_info.isr_cnt.isr_node_end++;
    }

    if ((int_status & DRV_VAU_INT_TIMEOUT) != 0) {
        vau_info->func_info.isr_cnt.isr_timeout++;
    }

    if ((int_status & DRV_VAU_INT_BUS_ERROR) != 0) {
        vau_info->func_info.isr_cnt.isr_bus_err++;
    }

    if ((int_status & DRV_VAU_INT_LIST_END) != 0) {
        vau_info->func_info.isr_cnt.isr_list_end++;
    }

    if ((int_status & DRV_VAU_INT_DISPLAY_DONE) != 0) {
        vau_info->func_info.isr_cnt.isr_disp_done++;
    }

    if ((int_status & DRV_VAU_INT_TUNL_DONE) != 0) {
        vau_info->func_info.isr_cnt.isr_tunl_done++;
    }

    if ((int_status & DRV_VAU_INT_CONFLICT) != 0) {
        vau_info->func_info.isr_cnt.isr_conflict++;
    }
}

td_s32 vau_isr_init(void)
{
    td_s32 ret;

    (void)memset_s(&g_vau_isr_info, sizeof(g_vau_isr_info), 0, sizeof(vau_isr_info));

    ret = osal_spin_lock_init(&g_vau_isr_info.isr_lock);
    if (ret != EXT_SUCCESS) {
        vau_err("osal_spin_lock_init failed, ret=%#x", ret);
        return ret;
    }

    ret = osal_workqueue_init(&(g_vau_isr_info.task_workqueue), vau_tasklet_func);
    if (ret != EXT_SUCCESS) {
        osal_spin_lock_destroy(&g_vau_isr_info.isr_lock);
        vau_err("osal_workqueue_init failed, ret=%#x", ret);
        return ret;
    }

    ret = osal_irq_request(VAU_INT_NUM, (osal_irq_handler)vau_isr_main, TD_NULL, TD_NULL, TD_NULL);
    if (ret != EXT_SUCCESS) {
        osal_workqueue_destroy(&(g_vau_isr_info.task_workqueue));
        osal_spin_lock_destroy(&g_vau_isr_info.isr_lock);
        vau_err("osal_irq_request failed, ret=%#x", ret);
        return ret;
    }

    osal_irq_enable(VAU_INT_NUM);

    return EXT_SUCCESS;
}

td_void vau_isr_deinit(void)
{
    osal_irq_disable(VAU_INT_NUM);
    osal_irq_free(VAU_INT_NUM, TD_NULL);
    osal_workqueue_destroy(&(g_vau_isr_info.task_workqueue));
    osal_spin_lock_destroy(&g_vau_isr_info.isr_lock);
}

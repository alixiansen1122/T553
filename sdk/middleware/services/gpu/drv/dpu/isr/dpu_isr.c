/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: dpu isr file
 */

#include "dpu_isr.h"
#include "drv_gfx_port.h"
#include "dpu_dfx.h"
#include "dpu_hal.h"
#include "pm.h"

typedef struct {
    osal_spinlock isr_lock;
    osal_wait finish_event;
    osal_wait te_signal_event;
#ifdef GPU_SUPPORT_VIDEO_MODE
    osal_wait vthd_percent_event;
    volatile td_s32 vthd_percent_done;
#endif
    volatile td_s32 finish_done;
    volatile td_s32 te_signal_done;
} dpu_isr_info;

static dpu_isr_info g_dpu_isr_info = {0};

static td_void isr_disp_main(td_u32 param)
{
    td_u32 int_type;
    td_u32 cur_time_us = 0;
    unsigned long flags = 0;

    uapi_unused(param);

    dpu_dfx_info *dfx_info = dpu_dfx_get_info();

    int_type = dpu_hal_disp_get_int_state();
    dpu_hal_disp_clear_int_state(int_type);

    osal_spin_lock_irqsave(&g_dpu_isr_info.isr_lock, &flags);
    if ((int_type & DPU_DISP_INT_TYPE_LOWBAND) == DPU_DISP_INT_TYPE_LOWBAND) {
        dfx_info->lowband_cnt++;
    }
    if ((int_type & DPU_DISP_INT_TYPE_BUS_ERR) == DPU_DISP_INT_TYPE_BUS_ERR) {
        dfx_info->bus_err_cnt++;
    }
#ifdef GPU_SUPPORT_VIDEO_MODE
    if ((int_type & DPU_DISP_INT_TYPE_SETTING) == DPU_DISP_INT_TYPE_SETTING) {
        g_dpu_isr_info.vthd_percent_done = 1;
    }
#endif
    if ((int_type & DPU_DISP_INT_TYPE_TE_INT) == DPU_DISP_INT_TYPE_TE_INT) {
        dpu_dfx_get_time_us(&cur_time_us);
        dfx_info->te_cost_us = (cur_time_us - dfx_info->last_te_time_us);
        dfx_info->last_te_time_us = cur_time_us;
        dfx_info->te_signal_cnt++;
        dfx_info->te_signal_total++;
        g_dpu_isr_info.te_signal_done = 1;
    }

    if ((int_type & DPU_DISP_INT_TYPE_FRM_DONE) == DPU_DISP_INT_TYPE_FRM_DONE) {
        g_dpu_isr_info.finish_done = 1;
        dfx_info->frm_done_cnt++;
        dfx_info->frm_done_total++;
        uapi_pm_remove_sleep_veto(PM_ID_DPU);
    }

    dpu_dfx_get_time_us(&cur_time_us);
    if ((cur_time_us - dfx_info->one_sec_ago) >= 1000000) { /* 1000000 us */
        dfx_info->one_sec_ago   = cur_time_us;
        dfx_info->draw_fps      = dfx_info->refresh_cnt;
        dfx_info->flip_fps      = dfx_info->frm_done_cnt;
        dfx_info->te_fps        = dfx_info->te_signal_cnt;

        dfx_info->refresh_cnt   = 0;
        dfx_info->frm_done_cnt  = 0;
        dfx_info->te_signal_cnt = 0;

        dpu_dfx_print_info();
    }
    osal_spin_unlock_irqrestore(&g_dpu_isr_info.isr_lock, &flags);
#ifdef GPU_SUPPORT_VIDEO_MODE
    if (g_dpu_isr_info.vthd_percent_done == 1) {
        osal_wait_wakeup(&g_dpu_isr_info.vthd_percent_event);
    }
#endif

    if (g_dpu_isr_info.finish_done == 1) {
        osal_wait_wakeup(&g_dpu_isr_info.finish_event);
    }

    if (g_dpu_isr_info.te_signal_done == 1) {
        osal_wait_wakeup(&g_dpu_isr_info.te_signal_event);
    }

    return;
}

static td_s32 isr_get_te_signal_done(const td_void *param)
{
    uapi_unused(param);

    return g_dpu_isr_info.te_signal_done;
}

static td_s32 isr_get_finish_done(const td_void *param)
{
    uapi_unused(param);

    return g_dpu_isr_info.finish_done;
}

#ifdef GPU_SUPPORT_VIDEO_MODE
static td_s32 isr_get_vthd_percent_signal_done(const td_void *param)
{
    uapi_unused(param);

    return g_dpu_isr_info.vthd_percent_done;
}

static td_s32 dpu_init_vvthd_percent_event(void)
{
    td_s32 ret;
    unsigned long flag = 0;

    ret = osal_wait_init(&g_dpu_isr_info.vthd_percent_event);
    if (ret != EXT_SUCCESS) {
        dpu_err("dpu_init_vvthd_percent failed");
        return DRV_ERR_DPU_OSAL_FAILED;
    }

    osal_spin_lock_irqsave(&g_dpu_isr_info.isr_lock, &flag);
    g_dpu_isr_info.vthd_percent_done = 1;
    osal_spin_unlock_irqrestore(&g_dpu_isr_info.isr_lock, &flag);

    return EXT_SUCCESS;
}
#endif

td_s32 dpu_isr_init(void)
{
    td_s32 ret;
    unsigned long flag = 0;

    (void)memset_s(&g_dpu_isr_info, sizeof(g_dpu_isr_info), 0, sizeof(dpu_isr_info));

    ret = osal_spin_lock_init(&g_dpu_isr_info.isr_lock);
    if (ret != EXT_SUCCESS) {
        dpu_err("osal_spin_lock_init failed, ret:0x%x", ret);
        return ret;
    }

    ret = osal_wait_init(&g_dpu_isr_info.finish_event);
    if (ret != EXT_SUCCESS) {
        osal_spin_lock_destroy(&g_dpu_isr_info.isr_lock);
        dpu_err("dpu_layer_init_wait_event failed");
        return DRV_ERR_DPU_OSAL_FAILED;
    }

    ret = osal_wait_init(&g_dpu_isr_info.te_signal_event);
    if (ret != EXT_SUCCESS) {
        osal_spin_lock_destroy(&g_dpu_isr_info.isr_lock);
        osal_wait_destroy(&g_dpu_isr_info.finish_event);
        dpu_err("dpu_layer_init_wait_event failed");
        return DRV_ERR_DPU_OSAL_FAILED;
    }

    osal_spin_lock_irqsave(&g_dpu_isr_info.isr_lock, &flag);
    g_dpu_isr_info.te_signal_done = 1;
    g_dpu_isr_info.finish_done = 1;
    osal_spin_unlock_irqrestore(&g_dpu_isr_info.isr_lock, &flag);

#ifdef GPU_SUPPORT_VIDEO_MODE
    ret = dpu_init_vvthd_percent_event();
    if (ret != EXT_SUCCESS) {
        osal_spin_lock_destroy(&g_dpu_isr_info.isr_lock);
        osal_wait_destroy(&g_dpu_isr_info.finish_event);
        osal_wait_destroy(&g_dpu_isr_info.te_signal_event);
        return DRV_ERR_DPU_OSAL_FAILED;
    }
#endif

    ret = osal_irq_request(DPU_INT_DISP, (osal_irq_handler)isr_disp_main, TD_NULL, TD_NULL, TD_NULL);
    if (ret != EXT_SUCCESS) {
        osal_spin_lock_destroy(&g_dpu_isr_info.isr_lock);
        osal_wait_destroy(&g_dpu_isr_info.finish_event);
        osal_wait_destroy(&g_dpu_isr_info.te_signal_event);
#ifdef GPU_SUPPORT_VIDEO_MODE
        osal_wait_destroy(&g_dpu_isr_info.vthd_percent_event);
#endif
        dpu_err("osal_irq_request failed, ret:0x%x", ret);
        return ret;
    }
    osal_irq_enable(DPU_INT_DISP);

    return EXT_SUCCESS;
}

td_void dpu_isr_deinit(void)
{
    osal_irq_disable(DPU_INT_DISP);
    osal_irq_free(DPU_INT_DISP, TD_NULL);
    osal_spin_lock_destroy(&g_dpu_isr_info.isr_lock);
}

td_void dpu_isr_wait_finish(void)
{
    td_s32 ret;
    static td_u32 dpu_timeout_cnt = 0;
    dpu_dfx_info *dfx_info = dpu_dfx_get_info();

    ret = osal_wait_timeout_interruptible(&g_dpu_isr_info.finish_event,
                                          isr_get_finish_done, TD_NULL, 500); /* 500 ms */
    if (ret <= 0) {
        if (dpu_timeout_cnt < 0x3) {
            dpu_dfx_proc_read();
            print_mipi_and_dpu_reg(0, TD_NULL);
        }
        dpu_timeout_cnt++;
        dfx_info->refresh_total = 0;
        dfx_info->frm_done_total = 0;
        dpu_err("osal_wait_timeout!!!!");
        return;
    }
}

td_void dpu_isr_wait_te_signal(void)
{
    td_s32 ret;

    ret = osal_wait_timeout_interruptible(&g_dpu_isr_info.te_signal_event,
                                          isr_get_te_signal_done, TD_NULL, 500); /* 500 ms */
    if (ret <= 0) {
        dpu_err("osal_wait_timeout!!!!");
        return;
    }

    g_dpu_isr_info.te_signal_done = 0;

    return;
}

#ifdef GPU_SUPPORT_VIDEO_MODE
td_void dpu_isr_wait_vthd_percent_signal(void)
{
    td_s32 ret;

    ret = osal_wait_timeout_interruptible(&g_dpu_isr_info.vthd_percent_event,
                                          isr_get_vthd_percent_signal_done, TD_NULL, 500); /* 500 ms */
    if (ret <= 0) {
        dpu_err("osal_wait_timeout!!!!");
        return;
    }

    return;
}

td_void dpu_isr_clear_vthd_percent_flag(void)
{
    g_dpu_isr_info.vthd_percent_done = 0;
}
#endif

td_void dpu_isr_clear_finish_flag(void)
{
    g_dpu_isr_info.finish_done = 0;
}

td_s32 dpu_wait_all_finish(td_void)
{
#ifndef GPU_SUPPORT_VIDEO_MODE
    unsigned long flag = 0;
    dpu_dfx_info *dfx_info = dpu_dfx_get_info();
    const td_u32 wait_time = 200;
    td_u32 cnt = 0;
    static td_u32 dpu_timeout_cnt = 0;

    while (cnt++ < wait_time) {
        osal_spin_lock_irqsave(&g_dpu_isr_info.isr_lock, &flag);
        if (dfx_info->refresh_total == dfx_info->frm_done_total) {
            osal_spin_unlock_irqrestore(&g_dpu_isr_info.isr_lock, &flag);
            break;
        }
        osal_spin_unlock_irqrestore(&g_dpu_isr_info.isr_lock, &flag);
        osal_udelay(100); // 100us
    }
    if (cnt >= wait_time) {
        dpu_err("dpu_wait_all_finish timeout!!!!");
        if (dpu_timeout_cnt < 0x2) {
            dpu_dfx_proc_read();
            print_mipi_and_dpu_reg(0, TD_NULL);
        }
        dpu_timeout_cnt++;
        return EXT_FAILURE;
    }
#endif
    return EXT_SUCCESS;
}

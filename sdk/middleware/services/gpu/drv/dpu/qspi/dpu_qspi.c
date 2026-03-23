/*
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved.
 * Description: dpu qspi file
 */

#include "dpu_qspi.h"
#include "dpu_dfx.h"
#include "drv_dpu_ioctl.h"
#include "platform_core.h"

typedef struct {
    td_bool has_new_frame;
    td_bool vblank;
    td_u32 qspi_addr;
    lcd_bus_api *bus_api;
    osal_wait qspi_te_event;
    osal_wait frm_done_event;
    volatile td_s32 qspi_te_done;
    volatile td_s32 send_frm_done;
} dpu_qspi_info;

static dpu_qspi_info g_qspi_info = {0};

static td_void qspi_frm_done_callback(td_void);


static td_s32 qspi_get_te_done(const td_void *param)
{
    uapi_unused(param);

    return g_qspi_info.qspi_te_done;
}

static td_void qspi_te_isr_callback(pin_t pin, uintptr_t param)
{
    uapi_unused(pin);
    uapi_unused(param);
    g_qspi_info.qspi_te_done = TD_TRUE;
    osal_wait_wakeup(&g_qspi_info.qspi_te_event);

    if (g_qspi_info.has_new_frame == TD_TRUE) { /* the func of bus_send_frame should be async */
        g_qspi_info.has_new_frame = TD_FALSE;
        g_qspi_info.bus_api->bus_send_frame(g_qspi_info.qspi_addr, qspi_frm_done_callback);
    }
}

static td_void qspi_wait_te_signal(td_void)
{
    td_s32 ret;

    ret = osal_wait_timeout_interruptible(&g_qspi_info.qspi_te_event,
                                          qspi_get_te_done, TD_NULL, 500); /* 500 ms */
    if (ret <= 0) {
        dpu_err("qspi osal_wait_timeout!!!!");
        return;
    }

    g_qspi_info.qspi_te_done = 0;

    return;
}

static td_s32 qspi_get_frm_done(const td_void *param)
{
    uapi_unused(param);

    return g_qspi_info.send_frm_done;
}

static td_void qspi_frm_done_callback(td_void)
{
    g_qspi_info.send_frm_done = 1;
    osal_wait_wakeup(&g_qspi_info.frm_done_event);
}

static td_void qspi_wait_last_frm_done(td_void)
{
    td_s32 ret;

    ret = osal_wait_timeout_interruptible(&g_qspi_info.frm_done_event,
                                          qspi_get_frm_done, TD_NULL, 500); /* 500 ms */
    if (ret <= 0) {
        dpu_err("qspi osal_wait_timeout!!!!");
        return;
    }

    g_qspi_info.send_frm_done = 0;

    return;
}

static td_void qspi_set_addr(td_u32 addr)
{
    g_qspi_info.qspi_addr = addr;
}

static td_void qspi_send_frame(td_void)
{
    if (g_qspi_info.vblank != TD_TRUE) { // double buffer refresh
        qspi_wait_last_frm_done();
    }
    g_qspi_info.has_new_frame = TD_TRUE;
}

static td_void qspi_set_vblank(td_bool vblank)
{
    g_qspi_info.vblank = vblank;
}

td_bool dpu_qspi_check_is_qspi(td_void)
{
    lcd_bus_type_t display_tpye  = lcd_get_bus_type();

    return (display_tpye == BUS_DISPLAY_QSPI);
}

td_s32 dpu_qspi_init(td_void)
{
    td_s32 ret;

    g_qspi_info.bus_api = lcd_get_bus_api();
    if (g_qspi_info.bus_api == TD_NULL) {
        dpu_err("call lcd_get_bus_api failed!");
        return EXT_FAILURE;
    }

    if (g_qspi_info.bus_api->bus_send_frame == TD_NULL) {
        dpu_err("bus_send_frame is NULL!");
        return EXT_FAILURE;
    }

    if (g_qspi_info.bus_api->bus_register_te_isr == TD_NULL) {
        dpu_err("bus_register_te_isr is NULL!");
        return EXT_FAILURE;
    }

    ret = g_qspi_info.bus_api->bus_register_te_isr(qspi_te_isr_callback);
    if (ret != EXT_SUCCESS) {
        dpu_err("bus_register_te_isr failed, ret = %#x!", ret);
        return ret;
    }

    ret = osal_wait_init(&g_qspi_info.qspi_te_event);
    if (ret != EXT_SUCCESS) {
        dpu_err("qspi_te_event init failed");
        return EXT_FAILURE;
    }

    ret = osal_wait_init(&g_qspi_info.frm_done_event);
    if (ret != EXT_SUCCESS) {
        osal_wait_destroy(&g_qspi_info.qspi_te_event);
        dpu_err("qspi_te_event init failed");
        return EXT_FAILURE;
    }

    g_qspi_info.send_frm_done = TD_TRUE;

    return EXT_SUCCESS;
}

td_s32 dpu_qspi_ioctl(td_ulong ioctl_cmd, td_void *para)
{
    if (ioctl_cmd == DRV_DPU_CMD_PUTLAYERINFO) {
        dpu_layer_info_cmd *layer_info_cmd = (dpu_layer_info_cmd *)para;
        if (layer_info_cmd == TD_NULL) {
            dpu_err("arg is null!");
            return DRV_ERR_DPU_NULL_POINTER;
        }
        qspi_set_addr(layer_info_cmd->surface.phy_addr);
    } else if (ioctl_cmd == DRV_DPU_CMD_PUTLAYERVBLANK) {
        dpu_layer_vlank_cmd *vlank_cmd = (dpu_layer_vlank_cmd *)para;
        if (vlank_cmd == TD_NULL) {
            dpu_err("arg is null!");
            return DRV_ERR_DPU_NULL_POINTER;
        }
        qspi_set_vblank(vlank_cmd->vblank);
    } else if (ioctl_cmd == DRV_DPU_CMD_WAITLAYERVSYNC) {
        qspi_wait_te_signal();
    } else if (ioctl_cmd == DRV_DPU_CMD_REFRESHLAYERS) {
        qspi_send_frame();
    }

    return EXT_SUCCESS;
}

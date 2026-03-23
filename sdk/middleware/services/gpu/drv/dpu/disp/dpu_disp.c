/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: dpu disp process
 */
#include "dpu_disp.h"
#include "dpu_dfx.h"
#include "dpu_isr.h"
#include "dpu_clock.h"
#include "lcd_bus.h"

#define DPU_VTHD_PERCENT_10  10
#define DPU_VTHD_PERCENT_90  90
#define DPU_VTHD_PERCENT_100 100

static dpu_disp_info g_dpu_disp_info = { 0 };

dpu_disp_info *dpu_disp_get_info(td_void)
{
    return &g_dpu_disp_info;
}

td_u32 dpu_disp_get_srceen_width(td_void)
{
    return g_dpu_disp_info.screen_width;
}

td_u32 dpu_disp_get_srceen_height(td_void)
{
    return g_dpu_disp_info.screen_height;
}

static td_s32 disp_get_panel_timing(const ext_rect *update_rect, dpu_disp_timing *drv_timing)
{
    td_s32 ret;
    lcd_bus_api *bus_api = TD_NULL;
    bus_panel_info_t panel_timing = { 0 };

    bus_api = lcd_get_bus_api();
    if ((bus_api == TD_NULL) || (bus_api->bus_get_panel_timing == TD_NULL)) {
        dpu_err("bus_api/bus_get_panel_timing is NULL!");
        return EXT_FAILURE;
    }

    ret = bus_api->bus_get_panel_timing(update_rect, &panel_timing);
    if (ret != EXT_SUCCESS) {
        dpu_err("uapi_panel_get_timing failed, ret = %#x!", ret);
        return ret;
    }

    drv_timing->is_cmd_mode       = panel_timing.is_cmd_mode;
    drv_timing->refresh_rate      = panel_timing.refresh_rate;

    drv_timing->vfb  = panel_timing.sync_info.vfp_lines;
    drv_timing->vbb  = panel_timing.sync_info.vbp_lines + panel_timing.sync_info.vsa_lines;
    drv_timing->vact = panel_timing.sync_info.vact_lines;

    drv_timing->hfb  = panel_timing.sync_info.hfp_pixels;
    drv_timing->hbb  = panel_timing.sync_info.hbp_pixels + panel_timing.sync_info.hsa_pixels;
    drv_timing->hact = panel_timing.sync_info.hact_pixels;

    drv_timing->vpw  = panel_timing.sync_info.vsa_lines;
    drv_timing->hpw  = panel_timing.sync_info.hsa_pixels;

    drv_timing->iop = 1;

    drv_timing->pixel_clk = panel_timing.pixel_clk;

    return EXT_SUCCESS;
}

static td_void disp_set_interrupt_thd(dpu_disp_timing *timing)
{
#ifdef GPU_SUPPORT_VIDEO_MODE
    td_u32 vtotal = timing->vact + timing->vbb + timing->vfb;
    td_u32 vthd_percent_10 = vtotal * DPU_VTHD_PERCENT_10 / DPU_VTHD_PERCENT_100;

    dpu_hal_disp_set_int_thd(DPU_DISP_INT_TYPE_SETTING, vthd_percent_10);
#else
    td_u32 setting_thd;
    td_u32 finish_thd;
    td_u32 reserved_thd;
    td_u32 vtotal = timing->vact + timing->vbb + timing->vfb;

    setting_thd  = vtotal * DPU_VTHD_PERCENT_10 / DPU_VTHD_PERCENT_100;
    finish_thd   = vtotal - 2; /* 2:line */
    reserved_thd = timing->vbb + timing->vfb + 2; /* 2:line */

    dpu_hal_disp_set_int_thd(DPU_DISP_INT_TYPE_SETTING, setting_thd);
    dpu_hal_disp_set_int_thd(DPU_DISP_INT_TYPE_FINISH, finish_thd);
    dpu_hal_disp_set_int_thd(DPU_DISP_INT_TYPE_RESERVE, reserved_thd);
#endif
}

static td_s32 disp_config_timing(td_void)
{
    td_s32 ret;

    ret = disp_get_panel_timing(TD_NULL, &g_dpu_disp_info.timing);
    if (ret != EXT_SUCCESS) {
        return ret;
    }

    g_dpu_disp_info.disp_region.x = 0;
    g_dpu_disp_info.disp_region.y = 0;
    g_dpu_disp_info.disp_region.width  = g_dpu_disp_info.timing.hact;
    g_dpu_disp_info.disp_region.height = g_dpu_disp_info.timing.vact;

    g_dpu_disp_info.screen_width  = g_dpu_disp_info.timing.hact;
    g_dpu_disp_info.screen_height = g_dpu_disp_info.timing.vact;

    dpu_clock_set_start_pos();
    dpu_clock_set_timing(&g_dpu_disp_info.timing);
    dpu_clock_set_chnl_clk(g_dpu_disp_info.timing.pixel_clk);
    disp_set_interrupt_thd(&g_dpu_disp_info.timing);

    return EXT_SUCCESS;
}

static td_void disp_set_chnl_enable(td_bool enable)
{
    dpu_hal_disp_set_chnl_enable(enable);
}

static td_void disp_set_cbm_default_priority(td_void)
{
    dpu_hal_disp_set_cbm_priority(DPU_DISP_CBM_MIX_LAYER_G0, DPU_DISP_CBM_MIX_PRIORITY_1);
    dpu_hal_disp_set_cbm_priority(DPU_DISP_CBM_MIX_LAYER_G1, DPU_DISP_CBM_MIX_PRIORITY_0);
}

static td_void disp_set_isr_enable(td_bool enable)
{
#ifdef GPU_SUPPORT_VIDEO_MODE
    dpu_hal_disp_set_dhd0_te_mode(0);      /* set dpu work video mode */
    dpu_hal_disp_set_int_enable(DPU_DISP_INT_TYPE_SETTING, enable);
#else
    dpu_hal_disp_set_dhd0_te_mode(enable); /* set dpu work te mode */
#endif
    dpu_hal_disp_set_int_enable(DPU_DISP_INT_TYPE_LOWBAND, enable);
    dpu_hal_disp_set_int_enable(DPU_DISP_INT_TYPE_BUS_ERR, enable);
    dpu_hal_disp_set_int_enable(DPU_DISP_INT_TYPE_FRM_DONE, enable);
}

td_s32 dpu_disp_init(td_void)
{
    td_s32 ret;

    (td_void)memset_s(&g_dpu_disp_info, sizeof(g_dpu_disp_info), 0, sizeof(dpu_disp_info));

    ret = osal_spin_lock_init(&g_dpu_disp_info.disp_lock);
    if (ret != EXT_SUCCESS) {
        dpu_err("osal_spin_lock_init failed");
        return DRV_ERR_DPU_OSAL_FAILED;
    }

    disp_set_cbm_default_priority();

    return EXT_SUCCESS;
}

td_void dpu_disp_deinit(td_void)
{
    dpu_disp_close();

    if (g_dpu_disp_info.disp_lock.lock != TD_NULL) {
        osal_spin_lock_destroy(&g_dpu_disp_info.disp_lock);
    }

    return;
}

td_void dpu_disp_chl_cfg(td_void)
{
    disp_config_timing();
    disp_set_chnl_enable(TD_TRUE);
    disp_set_isr_enable(TD_TRUE);
    disp_set_cbm_default_priority();
    g_dpu_disp_info.region_change = TD_TRUE;
}

td_void dpu_disp_open(td_void)
{
    unsigned long flag = 0;

    osal_spin_lock_irqsave(&g_dpu_disp_info.disp_lock, &flag);

    if (g_dpu_disp_info.open_cnt > 0) {
        g_dpu_disp_info.open_cnt++;
        osal_spin_unlock_irqrestore(&g_dpu_disp_info.disp_lock, &flag);
        return;
    }

    dpu_disp_chl_cfg();

    g_dpu_disp_info.open_cnt++;
    g_dpu_disp_info.is_open = TD_TRUE;

    osal_spin_unlock_irqrestore(&g_dpu_disp_info.disp_lock, &flag);

    return;
}

td_void dpu_disp_close(td_void)
{
    unsigned long flag = 0;

    osal_spin_lock_irqsave(&g_dpu_disp_info.disp_lock, &flag);

    if (g_dpu_disp_info.is_open != TD_TRUE) {
        osal_spin_unlock_irqrestore(&g_dpu_disp_info.disp_lock, &flag);
        return;
    }

    if (g_dpu_disp_info.open_cnt > 1) {
        g_dpu_disp_info.open_cnt--;
        osal_spin_unlock_irqrestore(&g_dpu_disp_info.disp_lock, &flag);
        return;
    }

    g_dpu_disp_info.open_cnt--;
    g_dpu_disp_info.is_open = TD_FALSE;
    osal_spin_unlock_irqrestore(&g_dpu_disp_info.disp_lock, &flag);

    return;
}

td_void dpu_disp_set_cbm_priority(drv_dpu_layer_id layer_id, drv_dpu_zorder_mode zorder)
{
    td_bool is_g0_up = TD_TRUE;

    if (layer_id == DRV_DPU_LAYER_ID1) {
        if ((zorder == DRV_DPU_ZORDER_MODE_MOVETOP) ||
            (zorder == DRV_DPU_ZORDER_MODE_MOVEUP)) {
            is_g0_up = TD_TRUE;
        } else {
            is_g0_up = TD_FALSE;
        }
    } else {
        if ((zorder == DRV_DPU_ZORDER_MODE_MOVETOP) ||
            (zorder == DRV_DPU_ZORDER_MODE_MOVEUP)) {
            is_g0_up = TD_FALSE;
        } else {
            is_g0_up = TD_TRUE;
        }
    }

    if (is_g0_up) {
        dpu_hal_disp_set_cbm_priority(DPU_DISP_CBM_MIX_LAYER_G0, DPU_DISP_CBM_MIX_PRIORITY_1);
        dpu_hal_disp_set_cbm_priority(DPU_DISP_CBM_MIX_LAYER_G1, DPU_DISP_CBM_MIX_PRIORITY_0);
    } else {
        dpu_hal_disp_set_cbm_priority(DPU_DISP_CBM_MIX_LAYER_G0, DPU_DISP_CBM_MIX_PRIORITY_0);
        dpu_hal_disp_set_cbm_priority(DPU_DISP_CBM_MIX_LAYER_G1, DPU_DISP_CBM_MIX_PRIORITY_1);
    }

    return;
}

td_u32 dpu_disp_get_layer_zorder(td_void)
{
    return dpu_hal_disp_get_cbm_priority();
}

td_void dpu_disp_wait_frame_done(td_void)
{
    td_u32 time_start = 0;
    td_u32 time_end   = 0;
    dpu_dfx_info *dfx_info = dpu_dfx_get_info();

    dpu_dfx_get_time_us(&time_start);
    dpu_isr_wait_finish();
    dpu_dfx_get_time_us(&time_end);

    dfx_info->wait_frm_done = (td_float)(time_end - time_start);

    return;
}

#ifdef GPU_SUPPORT_VIDEO_MODE
td_void dpu_disp_wait_vthd_percent_signal(td_void)
{
    dpu_isr_wait_vthd_percent_signal();
}
#endif

td_void dpu_disp_wait_te_signal(td_void)
{
    dpu_isr_wait_te_signal();
}

td_s32 dpu_disp_update_dhd_region(ext_rect *gp_region)
{
    /* 1.check whether gp_region and disp_region is equal */
    if (memcmp(&g_dpu_disp_info.disp_region, gp_region, sizeof(ext_rect)) == 0) {
        return EXT_SUCCESS;
    }

    /* 2.if disp_region need update, first update mipi timing */
    if (disp_get_panel_timing(gp_region, &g_dpu_disp_info.timing) != EXT_SUCCESS) {
        dpu_err("disp_get_panel_timing failed, gp_region[%d %d %u %u]",
                gp_region->x, gp_region->y, gp_region->width, gp_region->height);
        return EXT_FAILURE;
    }

    /* 3.check panel timing whether match gp_region */
    if ((gp_region->width != g_dpu_disp_info.timing.hact) || (gp_region->height != g_dpu_disp_info.timing.vact)) {
        dpu_err("panel timing not match gp_region, gp_wh[%u,%u], timing_hact/vact[%u,%u]!",
                gp_region->width, gp_region->height, g_dpu_disp_info.timing.hact, g_dpu_disp_info.timing.vact);
        return EXT_FAILURE;
    }

    /* 4.update disp_region by gp_region */
    g_dpu_disp_info.disp_region = *gp_region;
    g_dpu_disp_info.region_change = TD_TRUE;

    return EXT_SUCCESS;
}

#ifdef SUPPORT_LOGIC_SEND_FRM
static td_void disp_set_partial_cmd_info(td_void)
{
    td_s32 ret;
    partial_display_cmd_t lcd_cmd_info = {0};
    dpu_hal_partial_cmd_info dpu_cmd_info = {0};

    lcd_bus_api *bus_api = lcd_get_bus_api();
    if ((bus_api == TD_NULL) || (bus_api->bus_get_partial_display_cmd == TD_NULL)) {
        dpu_err("bus_api/bus_get_partial_display_cmd is NULL!");
        return;
    }

    ret = bus_api->bus_get_partial_display_cmd(&g_dpu_disp_info.disp_region, &lcd_cmd_info);
    if (ret != EXT_SUCCESS) {
        dpu_err("uapi_panel_get_cmd_info failed !");
        return;
    }

    dpu_cmd_info.hdr0       = lcd_cmd_info.cmd_set_col_h;
    dpu_cmd_info.pld_data00 = lcd_cmd_info.set_col_p0;
    dpu_cmd_info.pld_data01 = lcd_cmd_info.set_col_p1;

    dpu_cmd_info.hdr1       = lcd_cmd_info.cmd_set_raw_h;
    dpu_cmd_info.pld_data10 = lcd_cmd_info.set_raw_p0;
    dpu_cmd_info.pld_data11 = lcd_cmd_info.set_raw_p1;

    dpu_hal_disp_set_partial_cmd_info(&dpu_cmd_info);

    return;
}
#endif

td_void dpu_disp_set_timing_and_screen_region(td_void)
{
    lcd_bus_api *bus_api = lcd_get_bus_api();
    if ((bus_api == TD_NULL) || (bus_api->bus_update_panel == TD_NULL)) {
        dpu_err("bus_api/bus_update_panel is NULL!");
        return;
    }

#ifdef SUPPORT_LOGIC_SEND_FRM
    dpu_hal_disp_set_partial_cfg_vld(g_dpu_disp_info.region_change);

    if (g_dpu_disp_info.region_change == TD_TRUE) {
        dpu_clock_set_timing(&g_dpu_disp_info.timing);  /* update dpu timing */
        bus_api->bus_update_panel(PANEL_UPDATE_TIMING); /* update mipi timing */
        disp_set_partial_cmd_info();                    /* update screen region by dpu logic */
        g_dpu_disp_info.region_change = TD_FALSE;
    }
#else
    if (g_dpu_disp_info.region_change == TD_TRUE) {
        dpu_clock_set_timing(&g_dpu_disp_info.timing);  /* update dpu timing */
        bus_api->bus_update_panel(PANEL_UPDATE_TIMING); /* update mipi timing */
        bus_api->bus_update_panel(PANEL_UPDATE_SCREEN); /* update screen region by mipi software */
        g_dpu_disp_info.region_change = TD_FALSE;
    }
#endif

    return;
}

td_void dpu_disp_set_chnl_reg_up(td_void)
{
    dpu_hal_disp_set_chnl_regup();
}

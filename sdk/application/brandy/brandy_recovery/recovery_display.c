/*
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description:  Application Core OS Initialize Interface for Standard.
 * Author:
 * Create: 2023-12-13
 */
#include "recovery_display.h"
#include "pm_control.h"
#include "lcd_mipi.h"
#include "lcd_bus.h"
#include "soc_dpu_type.h"
#include "soc_dpu_api.h"
#include "debug_print.h"
#include "errno.h"
#include "tcxo.h"
#include "lcd_qspi.h"
#include "pinctrl_porting.h"
#include "product_evb4_standard.h"

#ifdef QSPI_DISPLAY
#define  DPU_FRAME_BUFF_ADDR     0x60280000
#else
#define  DPU_FRAME_BUFF_ADDR     0x60260000
#endif
#define  DPU_BYTE_ALIGN          16
#define  DPU_BPP_LEN             24  // 和layer_fmt相关，目前默认为EXT_GFX_FMT_RGB888，BPP为24
#define  DPU_STRIDER_DIVIDER     8
#define  DISP_POWER_ON_DELAY_MS  10

static const lcd_bus_api g_recovery_display_api_mipi = {
    .bus_init = mipi_bus_init,
    .bus_deinit = mipi_bus_deinit,
    .bus_enable = mipi_bus_enable,
    .bus_disable = mipi_bus_disable,
    .bus_write_cmd = mipi_bus_write,
    .bus_read_cmd = mipi_bus_read,
    .bus_lp_ctrl = mipi_bus_lp_ctrl,
    .bus_register_panel_info = mipi_bus_register_panel_info,
    .bus_get_panel_timing = mipi_bus_get_panel_timing,
    .bus_get_partial_display_cmd = mipi_bus_get_partial_display_cmd,
    .bus_update_panel = mipi_bus_update_panel,
    .bus_refresh_area = mipi_bus_refresh_area,
    .bus_suspend = mipi_bus_suspend,
    .bus_resume = mipi_bus_resume,
};

static const lcd_bus_api g_display_api_qspi = {
    .bus_init = lcd_qspi_init,
    .bus_deinit = lcd_qspi_deinit,
    .bus_enable = lcd_qspi_enable,
    .bus_disable = lcd_qspi_disable,
    .bus_write_cmd = lcd_qspi_write_cmd,
    .bus_read_cmd = lcd_qspi_read_cmd,
    .bus_send_frame = lcd_qspi_send_frame,
    .bus_suspend = lcd_qspi_suspend,
    .bus_resume = lcd_qspi_resume,
    .bus_register_te_isr = lcd_qspi_register_te_isr,
};
td_s32 uapi_dpu_module_init(void);

static errcode_t recovery_dispay_read_image_and_savein_surface(const char *path, ext_dpu_surface *surface)
{
    FILE *fp = fopen(path, "r");
    if (fp == TD_NULL) {
        PRINT("open [%s] file failed\n", path);
        return ERRCODE_FAIL;
    }

    if (fseek(fp, DPU_HEADER_LENTH, SEEK_SET) != 0) {
        PRINT("fseek [%s] file failed\n", path);
        (void)fclose(fp);
        return ERRCODE_FAIL;
    }

    (void)memset_s(surface->vir_addr, surface->size, 0, surface->size);
    if (fread((void *)surface->vir_addr, 1, surface->size, fp) != surface->size) {
        PRINT("read [%s] file failed, size = [%u]\n", path, surface->size);
        (void)fclose(fp);
        return ERRCODE_FAIL;
    }

    (void)fclose(fp);
    return ERRCODE_SUCC;
}

static errcode_t recovery_dispay_layer_and_surface_init(ext_dpu_surface *surface,
                                                        ext_dpu_layer_info *layer_info, ext_rect *update_rect)
{
    if (surface == NULL || layer_info == NULL) {
        return ERRCODE_FAIL;
    }

    layer_info->width = RESOLUTION_WIDTH;
    layer_info->height = RESOLUTION_HEIGHT;
    layer_info->layer_id = EXT_DPU_LAYER_ID1;
    layer_info->layer_fmt = EXT_GFX_FMT_RGB888;
    layer_info->compress_mode = EXT_GFX_COMPRESS_NONE;

    surface->width = layer_info->width;
    surface->height = layer_info->height;
    surface->stride = recovery_align_byte(RESOLUTION_WIDTH, DPU_BYTE_ALIGN) * DPU_BPP_LEN / DPU_STRIDER_DIVIDER;
    surface->size = surface->stride * surface->height;
    surface->phy_addr = (td_u32)DPU_FRAME_BUFF_ADDR;
    surface->vir_addr = (td_uchar *)surface->phy_addr;
    surface->update_rect = update_rect;
    surface->color_fmt = layer_info->layer_fmt;
    surface->compress_mode = EXT_GFX_COMPRESS_NONE;

    return ERRCODE_SUCC;
}

void recovery_display_sample(void)
{
    errcode_t ret;
    td_handle dpu_handle;
    ext_dpu_surface surface = {0};
    ext_dpu_layer_info layer_info = {0};
#ifndef IMG_DISPLAY
    /* 局部显示白色示例 */
    ext_rect update_rect = {0, 133, 454, 200}; // 定义一个局部区域
    recovery_dispay_layer_and_surface_init(&surface, &layer_info, &update_rect);
    // 设置layer全白色
    memset_s((void *)surface.phy_addr, surface.size, 0xff, surface.size);
#else
    /* 全屏显示图片示例 */
    /* 图片压缩参数如下：
       <imageRes compress="NONE" format="RGB888" align="16" tile="6"
                 alpha="0" rgb="0" autocmpmode="0" highqualitymode="0"
                 highcmpratiomode="0" ver="1">
       </imageRes>
    */
    #define  DISP_IMAGE "/update/demo.bin"
    recovery_dispay_layer_and_surface_init(&surface, &layer_info, NULL);
    recovery_dispay_read_image_and_savein_surface(DISP_IMAGE, &surface);
#endif
    
    ret = uapi_dpu_create_layer(&layer_info, &dpu_handle);
    if (ret != ERRCODE_SUCC) {
        return;
    }
    ret = uapi_dpu_set_layer_info(dpu_handle, &surface);
    if (ret != ERRCODE_SUCC) {
        return;
    }
    ret = uapi_dpu_refresh_layers();
    if (ret != ERRCODE_SUCC) {
        return;
    }
    ret = uapi_dpu_destroy_layer(dpu_handle);
    if (ret != ERRCODE_SUCC) {
        return;
    }
}

void recovery_display_init(void)
{
    uint32_t ret;
    uapi_pm_control(PM_DISPLAY_POWER_UP_AND_RUN);
    uapi_tcxo_delay_ms(DISP_POWER_ON_DELAY_MS);
#ifdef QSPI_DISPLAY
    PRINT("qspi init \r\n");
    lcd_bus_register_api(&g_display_api_qspi, BUS_DISPLAY_QSPI);
#else
    PRINT("mipi init \r\n");
    lcd_bus_register_api(&g_recovery_display_api_mipi, BUS_DISPLAY_MIPI);
#endif
    ret = uapi_lcd_init();
    if (ret != ERRCODE_SUCC) {
        PRINT("lcd init fail, ret = %x \r\n", ret);
        return;
    }

    ret = (uint32_t)uapi_dpu_module_init();
    if (ret != ERRCODE_SUCC) {
        uapi_lcd_deinit();
        return;
    }
}

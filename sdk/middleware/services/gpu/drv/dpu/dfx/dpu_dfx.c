/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: dpu intf file
 */

#include "dpu_dfx.h"
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "drv_gfx_port.h"

#include "dpu_isr.h"
#include "dpu_disp.h"
#include "dpu_layer.h"
#include "dpu_hal.h"
#include "drv_dpu.h"

#define SAVE_FILE_DIR "/user"

static dpu_dfx_info g_dpu_dfx_info = {0};
static  td_u32 g_save_cnt = 0;

dpu_dfx_info *dpu_dfx_get_info(td_void)
{
    return &g_dpu_dfx_info;
}

td_void dpu_dfx_print_info(void)
{
    if (g_dpu_dfx_info.print_en != TD_TRUE) {
        return;
    }
    dpu_print("-------------------------------------\n");
    dpu_print("wait_frm_done(us)         :%u\n", (td_u32)(g_dpu_dfx_info.wait_frm_done));
    dpu_print("fresh_intf_cost(us)       :%u\n", (td_u32)(g_dpu_dfx_info.refresh_intf_cost));
    dpu_print("te_interrupt(us)          :%u\n", (td_u32)(g_dpu_dfx_info.te_cost_us));
    dpu_print("draw/flip/te(fps)         :%-6u/%-6u/%-6u\n", g_dpu_dfx_info.draw_fps,
              g_dpu_dfx_info.flip_fps, g_dpu_dfx_info.te_fps);

    return;
}

#if defined(SUPPORT_AT_CMD) || defined(SUPPORT_TEST_COMMON) || defined(CONFIG_DIAG_GPU_PROC_SUPPORT)
static td_void dfx_disp_proc_read(void)
{
    dpu_disp_info *disp_dev  = dpu_disp_get_info();

    dpu_print("\n-----------DISP proc-----------\n");
    dpu_print("work_mode           :%-6d\n", disp_dev->timing.is_cmd_mode ? 1 : 0);
    dpu_print("disp_open           :%-6d\n", disp_dev->is_open ? 1 : 0);
    dpu_print("open_cnt            :%-6u\n", disp_dev->open_cnt);

    dpu_print("pixel_clk           :%-6u\n", disp_dev->timing.pixel_clk);
    dpu_print("refresh_rate        :%-6u\n", disp_dev->timing.refresh_rate);

    dpu_print("hact/vact           :%-6u/%-6u\n", disp_dev->timing.hact, disp_dev->timing.vact);
    dpu_print("hfb/vfb             :%-6u/%-6u\n", disp_dev->timing.hfb, disp_dev->timing.vfb);
    dpu_print("hbb/vbb             :%-6u/%-6u\n", disp_dev->timing.hbb, disp_dev->timing.vbb);
    dpu_print("hpw/vpw             :%-6u/%-6u\n", disp_dev->timing.hpw, disp_dev->timing.vpw);

    dpu_print("dhd0_region         :%-6d/%-6d/%-6u/%-6u\n",
              disp_dev->disp_region.x, disp_dev->disp_region.y,
              disp_dev->disp_region.width, disp_dev->disp_region.height);

    dpu_print("lowband_cnt         :%-6u\n", g_dpu_dfx_info.lowband_cnt);
    dpu_print("bus_err_cnt         :%-6u\n", g_dpu_dfx_info.bus_err_cnt);
    dpu_print("te_signal           :%-6u\n", g_dpu_dfx_info.te_signal_total);
    dpu_print("fresh/frm_done      :%-6u/%-6u\n", g_dpu_dfx_info.refresh_total, g_dpu_dfx_info.frm_done_total);
    dpu_print("draw/flip/te(t)     :%-6u/%-6u/%-6u\n", g_dpu_dfx_info.refresh_cnt,
              g_dpu_dfx_info.frm_done_cnt, g_dpu_dfx_info.te_signal_cnt);

    dpu_print("suspend/resume      :%-6u/%-6u\n", g_dpu_dfx_info.suspend_cnt, g_dpu_dfx_info.resume_cnt);

    td_u32 chksum_y = 0;
    td_u32 chksum_u = 0;
    td_u32 chksum_v = 0;
    if (drv_dpu_get_suspend_flag() == TD_FALSE) {
        dpu_hal_disp_get_chksum(&chksum_y, &chksum_u, &chksum_v);
    } else {
        dpu_print("dpu is in lowpower state, chksum_out is invalid!");
    }
    dpu_print("chksum[RGB]         :%#x/%#x/%#x\n", chksum_y, chksum_u, chksum_v);

    return;
}

static td_void dfx_layer_proc_read(td_u32 layer_id)
{
    dpu_layer_dev *layer_dev = TD_NULL;

    if (layer_id < DRV_DPU_LAYER_ID1 || layer_id > DRV_DPU_MAX_LAYER_ID) {
        dpu_err("invalid layer id = %u, max is 1!", layer_id);
        return;
    }

    layer_dev = dpu_layer_get_dev(layer_id);
    if (layer_dev != TD_NULL) {
        dpu_print("-----------LAYER%u proc---------\n", layer_id);
        dpu_print("create          :%-6d\n", layer_dev->is_create ? 1 : 0);
        dpu_print("create_cnt      :%-6u\n", layer_dev->create_cnt);
        dpu_print("width           :%-6u\n", layer_dev->run_info.width);
        dpu_print("height          :%-6u\n", layer_dev->run_info.height);
        dpu_print("stride          :%-6u\n", layer_dev->run_info.stride);
        dpu_print("uv_stride       :%-6u\n", layer_dev->canvas_surface.uv_stride);
        dpu_print("uv_offset       :%-6u\n", layer_dev->canvas_surface.uv_offset);
        dpu_print("format          :%-6u\n", layer_dev->run_info.fmt);
        dpu_print("cmp_mode        :%-6u\n", layer_dev->run_info.compress_mode);
        dpu_print("vblank          :%-6d\n", layer_dev->vblank ? 1 : 0);

        dpu_print("disp_rect       :%-6d/%-6d/%-6u/%-6u\n",
            layer_dev->run_info.disp_rect.x, layer_dev->run_info.disp_rect.y,
            layer_dev->run_info.disp_rect.width, layer_dev->run_info.disp_rect.height);
        dpu_print("crop_rect       :%-6d/%-6d/%-6u/%-6u\n",
            layer_dev->run_info.crop_rect.x, layer_dev->run_info.crop_rect.y,
            layer_dev->run_info.crop_rect.width, layer_dev->run_info.crop_rect.height);
    }

    td_u32 chksum_ar = 0;
    td_u32 chksum_gb = 0;
    if (drv_dpu_get_suspend_flag() == TD_FALSE) {
        dpu_hal_layer_get_fdr_chksum(layer_id, &chksum_ar, &chksum_gb);
    } else {
        dpu_print("dpu is in lowpower state, chksum_in is invalid!");
    }
    dpu_print("chksum[ar/gb]   :%#x/%#x\n", chksum_ar, chksum_gb);

    return;
}

td_s32 dfx_get_proc_info(int argc, const char *argv[])
{
    td_u32 layer_id;

    if (argc < 1) {
        dpu_print("usage: dpu_proc layer_id. range:0~2");
        return EXT_FAILURE;
    }

    layer_id = (td_u32)strtoul(argv[0], TD_NULL, 10); /* 10: index */
    dfx_layer_proc_read(layer_id);
    dfx_disp_proc_read();

    return EXT_SUCCESS;
}

td_s32 dfx_dpu_print(int argc, const char *argv[])
{
    if (argc < 1) {
        dpu_print("usage: dpu_print 0/1\n");
        return EXT_FAILURE;
    }

    g_dpu_dfx_info.print_en = (td_bool)strtoul(argv[0], TD_NULL, 10); /* 10: index */

    return EXT_SUCCESS;
}

td_s32 dfx_dpu_save_fb_enable(int argc, const char *argv[])
{
    if (argc < 0x4) {
        dpu_print("usage: layer_id: 0/1, save_en 0/1, save_all 0/1, save_cnt\n");
        return EXT_FAILURE;
    }

    g_dpu_dfx_info.save_layer = (td_u32)strtoul(argv[0x0], TD_NULL, 10); /* 10: index */
    g_dpu_dfx_info.save_en    = (td_bool)strtoul(argv[0x1], TD_NULL, 10); /* 10: index */
    g_dpu_dfx_info.save_all   = (td_bool)strtoul(argv[0x2], TD_NULL, 10); /* 10: index */
    g_dpu_dfx_info.save_cnt   = (td_u32)strtoul(argv[0x3], TD_NULL, 10); /* 10: index */

    return EXT_SUCCESS;
}
#endif

td_void dpu_dfx_proc_read(td_void)
{
#if defined(SUPPORT_AT_CMD) || defined(SUPPORT_TEST_COMMON) || defined(CONFIG_DIAG_GPU_PROC_SUPPORT)
    dfx_layer_proc_read(0);
    dfx_layer_proc_read(1);
    dfx_disp_proc_read();
#endif
}

static td_void dpu_dfx_save_fb_data_all(const drv_dpu_surface *surface)
{
    if (surface == TD_NULL) {
        dpu_print("input surface is null\n");
        return;
    }

    if ((surface->phy_addr == 0) || (surface->stride == 0) || (surface->height == 0)) {
        dpu_print("invalid input[%#X][%u][%u]\n", surface->phy_addr, surface->stride, surface->height);
        return;
    }

    int fd = open("/user/dpu_save_fb_data.bin", O_RDWR | O_CREAT, 0666);
    if (fd < 0) {
        dpu_print("open dpu_save_fb_data.bin failure\n");
        return;
    }
    lseek(fd, 0, SEEK_END);
    write(fd, (td_void *)(uintptr_t)surface->phy_addr, surface->stride * surface->height);
    close(fd);

    dpu_print("save /user/dpu_save_fb_data.bin add[%#X] stride[%u] height[%u] success!\n",
              surface->phy_addr, surface->stride, surface->height);
}

static td_void dpu_dfx_save_fb_data_part(const drv_dpu_surface *surface)
{
    if (surface == TD_NULL) {
        dpu_print("input surface is null\n");
        return;
    }

    if ((surface->phy_addr == 0) || (surface->stride == 0) || (surface->height == 0)) {
        dpu_print("invalid input[%#X][%u][%u]\n", surface->phy_addr, surface->stride, surface->height);
        return;
    }

    td_u32 pixel_byte;
    if (surface->color_fmt == DRV_GFX_FMT_ARGB8888) {
        pixel_byte = 0x4;
    } else if (surface->color_fmt == DRV_GFX_FMT_RGB888) {
        pixel_byte = 0x3;
    } else if (surface->color_fmt == DRV_GFX_FMT_RGB565) {
        pixel_byte = 0x2;
    } else {
        dpu_print("unsupport fmt: %#X", surface->color_fmt);
        return;
    }

    ext_rect rect = {0, 0, surface->width, surface->height};
    ext_rect *update_rect = (surface->update_rect == TD_NULL ? &rect : surface->update_rect);
    td_char file_name[0xff] = {'\0'};
    td_s32 ret = snprintf_s(file_name, sizeof(file_name), sizeof(file_name), "%s/%u_width_%u_height_%u.bin",
             SAVE_FILE_DIR, g_save_cnt, update_rect->width, update_rect->height);
    if (ret < 0) {
        dpu_print("write file_name err\n");
        return;
    }

    int fd = open(file_name, O_RDWR | O_CREAT, 0666);
    if (fd < 0) {
        dpu_print("open file failure\n");
        return;
    }
    td_uchar *start_addr = (td_uchar *)(uintptr_t)surface->phy_addr +
        (td_u32)update_rect->y * surface->stride + (td_u32)update_rect->x * pixel_byte;
    for (td_u32 i = 0; i < update_rect->height; i++) {
        write(fd, (td_void *)start_addr, update_rect->width * pixel_byte);
        start_addr += surface->stride;
    }
    close(fd);

    dpu_print("save %u_width_%u_height_%u.bin, addr[%#X] success!\n",
              g_save_cnt, surface->width, surface->height, surface->phy_addr);
}

td_void dfx_dpu_save_fb_to_file(drv_dpu_layer_id layer_id, drv_dpu_surface *surface)
{
    if ((g_dpu_dfx_info.save_en == TD_FALSE) || (layer_id != g_dpu_dfx_info.save_layer)) {
        return;
    }
    if (g_save_cnt >= g_dpu_dfx_info.save_cnt) {
        g_save_cnt = 0;
        g_dpu_dfx_info.save_en = TD_FALSE;
        return;
    }
    g_save_cnt++;
    if (g_dpu_dfx_info.save_all) {
        dpu_dfx_save_fb_data_all(surface);
    } else {
        dpu_dfx_save_fb_data_part(surface);
    }
}

static td_void read_reg_with_cnt(td_u32 addr, td_u32 cnt)
{
    for (uint32_t i = 0; i < cnt; i++) {
        volatile uint32_t value;
        uapi_reg_read(addr + i * 4, value); // 4: 4byte
        if (i % 4 == 0) { // 4: 4byte
            if (i != 0) {
                dpu_print("\n");
            }
            dpu_print("%X =", addr + i * 4); // 4: 4byte
        }
        dpu_print(" %08X", value);
        if (i == cnt - 1) {
            dpu_print("\n");
        }
    }
}

td_void print_mipi_and_dpu_reg(int argc, const char *argv[])
{
    uapi_unused(argc);
    uapi_unused(argv);

    printf("----------------mipi reg--------------------\n");
    read_reg_with_cnt(0x56000000, 0x100);
    printf("----------------dpu  reg--------------------\n");
    read_reg_with_cnt(0x56020000, 0x24);
    read_reg_with_cnt(0x56020300, 0x10);
    read_reg_with_cnt(0x56020400, 0x20);
    read_reg_with_cnt(0x56020c00, 0x10);
    read_reg_with_cnt(0x56027000, 0x50);
    read_reg_with_cnt(0x56027800, 0x50);
    read_reg_with_cnt(0x5602d000, 0x100);
    read_reg_with_cnt(0x56030e00, 0x100);
    read_reg_with_cnt(0x56031000, 0x100);
}

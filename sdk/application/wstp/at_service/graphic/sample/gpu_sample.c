/**
 * Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved. \n
 *
 * Description: Provides gpu sample. \n
 * Author: CompanyName \n
 * History: \n
 * 2022-11-17， Create file. \n
 */

#include "gpu_sample.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "soc_osal.h"
#include "soc_vau_api.h"
#include "soc_dpu_api.h"
#include "soc_mipi_tx.h"
#include "dpu_dfx.h"

#ifdef SUPPORT_GPU_TEST
#include "test_gpu_comm.h"
#endif

#define TEST_DPU_LAYER_DRAW_BOX_WIDTH  (454 / 2)
#define TEST_DPU_LAYER_DRAW_BOX_HEIGHT (454 / 2)
#define TEST_DPU_LAYER_BLUE   0xff0000ff
#define TEST_DPU_LAYER_RED    0xffff0000
#define TEST_DPU_LAYER_GREEN  0xff00ff00
#define TEST_DPU_LAYER_WHITE  0xffffffff
#define TEST_DPU_LAYER_YELLOW 0xffffff00

extern td_s32 vau_dfx_proc_read(int argc, const char *argv[]);
extern td_s32 jpeg_dfx_proc_read(int argc, const char *argv[]);

static td_void wstp_draw_box(ext_rect *rect, td_u32 stride, td_char *mem_start, td_u32 color, td_u32 bpp)
{
    td_char *current_mem = TD_NULL;
    td_u32 column, row;
    td_u32 convert_color = color;

    for (column = rect->y; column < (rect->y + rect->height); column++) {
        current_mem = mem_start + column * stride;
        for (row = rect->x; row < (rect->x + rect->width); row++) {
            *(td_u32*)(current_mem + row * bpp) = convert_color;
        }
    }
}

static td_void wstp_draw_ui(td_char *mem, td_u32 width, td_u32 height, td_u32 stride, td_u32 bpp)
{
    ext_rect rect;

    rect.x = 0;
    rect.y = 0;
    rect.width  = TEST_DPU_LAYER_DRAW_BOX_WIDTH;
    rect.height = TEST_DPU_LAYER_DRAW_BOX_HEIGHT;

    wstp_draw_box(&rect, stride, mem, TEST_DPU_LAYER_BLUE, bpp);
    rect.x = width - TEST_DPU_LAYER_DRAW_BOX_WIDTH - 1;
    rect.y = 0;
    wstp_draw_box(&rect, stride, mem, TEST_DPU_LAYER_RED, bpp);
    rect.x = 0;
    rect.y = height - TEST_DPU_LAYER_DRAW_BOX_HEIGHT - 1;
    wstp_draw_box(&rect, stride, mem, TEST_DPU_LAYER_GREEN, bpp);
    rect.x = width - TEST_DPU_LAYER_DRAW_BOX_WIDTH - 1;
    rect.y = height - TEST_DPU_LAYER_DRAW_BOX_HEIGHT - 1;
    wstp_draw_box(&rect, stride, mem, TEST_DPU_LAYER_YELLOW, bpp);
}

static td_void wstp_vau_fmt_trans(td_u32 src_addr, td_u32 dst_addr)
{
    td_handle handle;
    ext_vau_surface src_surface = {0};
    ext_vau_surface dst_surface = {0};
    ext_vau_surface_list surface_list = {0};

    src_surface.width          = 454;
    src_surface.height         = 454;
    src_surface.stride         = 1856; /* 1856: 464x4 */
    src_surface.phy_addr       = src_addr;
    src_surface.vir_addr       = (td_uchar *)src_surface.phy_addr;
    src_surface.color_fmt      = EXT_GFX_FMT_ARGB8888;
    src_surface.compress_mode  = EXT_GFX_COMPRESS_NONE;
    src_surface.argb_order     = EXT_GFX_FMT_ORDER_ARGB;
    src_surface.rect.x         = 0;
    src_surface.rect.y         = 0;
    src_surface.rect.width     = src_surface.width;
    src_surface.rect.height    = src_surface.height;
    src_surface.out_rect       = src_surface.rect;

    dst_surface.width          = 454;
    dst_surface.height         = 454;
    dst_surface.stride         = 1392; /* 1392: 464x3 */
    dst_surface.phy_addr       = dst_addr;
    dst_surface.vir_addr       = (td_uchar *)dst_surface.phy_addr;
    dst_surface.color_fmt      = EXT_GFX_FMT_RGB888;
    dst_surface.compress_mode  = EXT_GFX_COMPRESS_NONE;
    dst_surface.argb_order     = EXT_GFX_FMT_ORDER_ARGB;
    dst_surface.rect.x         = 0;
    dst_surface.rect.y         = 0;
    dst_surface.rect.width     = dst_surface.width;
    dst_surface.rect.height    = dst_surface.height;

    surface_list.src_surface_cnt = 1;
    surface_list.src_surface = &src_surface;
    surface_list.dst_surface = &dst_surface;
    surface_list.opt = NULL;

    handle = uapi_vau_create();
    uapi_vau_render(handle, &surface_list);
    uapi_vau_submit(handle, TD_TRUE, 1000); /* 1000 ms timeout */
    uapi_vau_destroy(handle);

    return;
}

td_void wstp_gpu_smoke_display(td_void)
{
    td_handle dpu_handle;
    ext_dpu_surface surface = { 0 };
    ext_dpu_layer_info layer_info = { 0 };

    const td_u32 src_addr = 0x6d200000;
    const td_u32 dst_addr = 0x60280000;

#ifdef MIPI_ULPS_SUPPORT
    uapi_mipi_tx_exit_ulps();
#endif

    /* 1.init layer info & create layer */
    layer_info.width           = 454;
    layer_info.height          = 454;
    layer_info.layer_id        = EXT_DPU_LAYER_ID1;
    layer_info.layer_fmt       = EXT_GFX_FMT_ARGB8888;
    layer_info.compress_mode   = EXT_GFX_COMPRESS_NONE;

    uapi_dpu_create_layer(&layer_info, &dpu_handle);

    /* 2.init surface */
    surface.width         = layer_info.width;
    surface.height        = layer_info.height;
    surface.stride        = 1856;
    surface.size          = surface.stride * surface.height;
    surface.phy_addr      = src_addr;
    surface.vir_addr      = (td_uchar *)surface.phy_addr;
    surface.update_rect   = TD_NULL;
    surface.color_fmt     = layer_info.layer_fmt;
    surface.compress_mode = EXT_GFX_COMPRESS_NONE;

    /* 3.draw picture */
    wstp_draw_ui((td_char *)surface.vir_addr, surface.width, surface.height, surface.stride, 0x4);

    /* 4.vau process argb888 --> rgb888 */
    wstp_vau_fmt_trans(src_addr, dst_addr);
    layer_info.layer_fmt = EXT_GFX_FMT_RGB888;
    surface.color_fmt    = EXT_GFX_FMT_RGB888;
    surface.stride       = 1392;
    surface.size         = surface.stride * surface.height;
    surface.phy_addr     = dst_addr;
    surface.vir_addr     = (td_uchar *)surface.phy_addr;

    /* 5.dpu display */
    uapi_dpu_set_layer_info(dpu_handle, &surface);
    uapi_dpu_refresh_layers();

    return;
}

static td_void wstp_gpu_dpu_cbar(int argc, const char *argv[])
{
    if (argc < 1) {
        return;
    }

#ifdef MIPI_ULPS_SUPPORT
    uapi_mipi_tx_exit_ulps();
    osal_msleep(1);
#endif

    td_bool cbar_en = (td_bool)strtoul(argv[0], TD_NULL, 10); /* 10: index */
    td_u32 value = cbar_en ? 0xc0000011 : 0x80000011;
    uapi_reg_write(0x5602d000, value);
    uapi_reg_write(0x5602d07c, 0x1);
}

static td_void wstp_dpu_screen_cap_g0(td_void)
{
    td_u32 addr, stride, height;
    dpu_dfx_info *dfx_info = dpu_dfx_get_info();

    addr = dfx_info->layer_info[0].phy_addr;
    height = dfx_info->layer_info[0].height;
    stride = dfx_info->layer_info[0].stride;

    int fd = open("/user/dpu_screen_cap_g0.bin", O_RDWR | O_CREAT, 0666);
    if (fd < 0) {
        printf("open file failure!\n");
        return;
    }

    write(fd, (td_void *)(uintptr_t)addr, stride * height);
    close(fd);

    printf("save /user/dpu_screen_cap_g0.bin stride[%u] height[%u] success!\n", stride, height);
}

static td_void wstp_dpu_screen_cap_g1_yuv_sp420(td_u32 y_addr, td_u32 y_h, td_u32 y_s)
{
    td_u32 y_size = y_s * y_h;
    td_u32 uv_size = y_s * uapi_byte_align(y_h, 16) / 2; // 16:index, 2:index
    td_u32 total_size = y_size + uv_size;

    int fd = open("/user/dpu_g1_sp420.yuv", O_RDWR | O_CREAT, 0666);
    if (fd < 0) {
        printf("open file dpu_g1_sp420 failure!\n");
        return;
    }

    write(fd, (td_void *)(uintptr_t)y_addr, total_size);
    close(fd);
    printf("save /user/dpu_g1_sp420.yuv stride[%u] height[%u] success!\n", y_s, y_h);
}

static td_void wstp_dpu_screen_cap_g1_yuv_p420(td_u32 y_addr, td_u32 y_w, td_u32 y_h, td_u32 y_s)
{
    td_u32 uv_addr = y_addr + y_s * y_h;
    td_u32 uv_w, uv_h, uv_s;
    uv_w = y_w / 2; // 2:index
    uv_h = uapi_byte_align(y_h, 16) / 2; // 16: index 2:index
    uv_s = y_s;

    int fd = open("/user/dpu_g1_planar420.yuv", O_RDWR | O_CREAT, 0666);
    if (fd < 0) {
        printf("open file dpu_g1_planar420 failure!\n");
        return;
    }

    // save_y_data
    td_char *addr = (td_char *)(uintptr_t)y_addr;
    for (td_u32 i = 0; i < y_h; i++) {
        write(fd, addr, y_w);
        addr += y_s;
    }

    // save_u_data
    const td_u32 uv_max_len = 640;
    char *uv_data = malloc(uv_w);
    if (uv_data == NULL) {
        printf("malloc fail for screen_cap g1 size = %u", uv_w);
        return;
    }

    addr = (char *)(uintptr_t)uv_addr;
    for (td_u32 i = 0; i < uv_h; i++) {
        for (td_u32 j = 0; j < uv_w; j++) {
            uv_data[j] = addr[2 * j + 1]; /* 2 is index */
        }
        write(fd, uv_data, uv_w);
        addr += uv_s;
    }

    // save_v_data
    addr = (char *)(uintptr_t)uv_addr;
    for (td_u32 i = 0; i < uv_h; i++) {
        for (td_u32 j = 0; j < uv_w; j++) {
            uv_data[j] = addr[2 * j + 0]; /* 2 is index */
        }
        write(fd, uv_data, uv_w);
        addr += uv_s;
    }

    close(fd);
    free(uv_data);
    printf("save /user/dpu_g1_planar420.yuv y_w/h/s:[%u/%u/%u] uv_w/h/s:[%u/%u/%u]success!\n",
           y_w, y_h, y_s, uv_w, uv_h, uv_s);
}

static td_void wstp_dpu_screen_cap_g1(td_void)
{
    td_u32 y_addr, y_w, y_h, y_s;
    dpu_dfx_info *dfx_info = dpu_dfx_get_info();

    y_addr = dfx_info->layer_info[1].phy_addr;
    y_w    = dfx_info->layer_info[1].width;
    y_h    = dfx_info->layer_info[1].height;
    y_s = dfx_info->layer_info[1].stride;

    wstp_dpu_screen_cap_g1_yuv_sp420(y_addr, y_h, y_s);
    wstp_dpu_screen_cap_g1_yuv_p420(y_addr, y_w, y_h, y_s);
}

static td_void wstp_gpu_screen_cap(int argc, const char *argv[])
{
    if (argc < 1) {
        printf("need input layer_id: 0/1\n");
        return;
    }

    td_u32 layer_id = (td_u32)strtoul(argv[0], TD_NULL, 10); /* 10: index */
    if (layer_id == 0) {
        wstp_dpu_screen_cap_g0();
    } else {
        wstp_dpu_screen_cap_g1();
    }
}

static td_void dfx_hide_layer_proc(int argc, const char *argv[])
{
    if (argc < 1) {
        printf("need input layer_id: 0/1\n");
        return;
    }

    td_u32 layer_id = (td_u32)strtoul(argv[0], TD_NULL, 10); /* 10: index */
    uapi_dpu_hide_layer(layer_id);
}

static td_void dfx_show_layer_proc(int argc, const char *argv[])
{
    if (argc < 1) {
        printf("need input layer_id: 0/1\n");
        return;
    }

    td_u32 layer_id = (td_u32)strtoul(argv[0], TD_NULL, 10); /* 10: index */
    uapi_dpu_show_layer(layer_id);
}

td_void wstp_gpu_sample_process(int argc, char *argv[])
{
    if (argc < 1) {
        return;
    }

    if (strcmp((char *)argv[0], "smoke") == 0) {
        wstp_gpu_smoke_display();
    } else if (strcmp((char *)argv[0], "dpu_cbar") == 0) {
        wstp_gpu_dpu_cbar(argc - 1, argv + 1);
    } else if (strcmp((char *)argv[0], "screen_cap") == 0) {
        wstp_gpu_screen_cap(argc - 1, argv + 1);
    } else if (strcmp((char *)argv[0], "dpu_proc") == 0) {
        dfx_get_proc_info(argc - 1, argv + 1);
    } else if (strcmp((char *)argv[0], "dpu_hide") == 0) {
        dfx_hide_layer_proc(argc - 1, argv + 1);
    } else if (strcmp((char *)argv[0], "dpu_show") == 0) {
        dfx_show_layer_proc(argc - 1, argv + 1);
    } else if (strcmp((char *)argv[0], "dpu_print") == 0) {
        dfx_dpu_print(argc - 1, argv + 1);
    } else if (strcmp((char *)argv[0], "dpu_print_reg") == 0) {
        print_mipi_and_dpu_reg(argc - 1, argv + 1);
    } else if (strcmp((char *)argv[0], "dpu_save_fb") == 0) {
        dfx_dpu_save_fb_enable(argc - 1, argv + 1);
    } else if (strcmp((char *)argv[0], "vau_print") == 0) {
        vau_dfx_print(argc - 1, argv + 1);
    } else if (strcmp((char *)argv[0], "vau_proc") == 0) {
        vau_dfx_proc_read(argc - 1, argv + 1);
    } else if (strcmp((char *)argv[0], "vau_api_print") == 0) {
        vau_dfx_enable_api_print(argc - 1, argv + 1);
    } else if (strcmp((char *)argv[0], "jpeg_proc") == 0) {
#ifdef SUPPORT_GPU_JPEG
        jpeg_dfx_proc_read(argc - 1, argv + 1);
#endif
    } else {
#ifdef SUPPORT_GPU_TEST
        test_gpu_testcase_process(argc, argv);
#endif
    }

    return;
}

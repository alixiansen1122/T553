/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: dpu_interface
 */

#ifndef DRV_GRAPHICS_DFX_DPU_DFX_H
#define DRV_GRAPHICS_DFX_DPU_DFX_H

#include <stdio.h>
#include <securec.h>
#include "td_base.h"
#include "soc_osal.h"
#include "drv_dpu_type.h"

#ifdef NO_UART_DEBUG
#include "soc_diag_util.h"
#include "log_oam_logger.h"
#include "log_def.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define DRV_ERR_DPU_BASE ((td_s32)(((0x80UL + 0x20UL) << 24) | (100 << 16) | (4 << 13) | 1))

enum {
    DRV_ERR_DPU_LAYER_NOT_CREATED = DRV_ERR_DPU_BASE,
    DRV_ERR_DPU_NULL_POINTER,
    DRV_ERR_DPU_LAYER_NOT_SUPPORT,
    DRV_ERR_DPU_OSAL_FAILED,
    DRV_ERR_DPU_OPERATION_NOT_SUPPORT,
    DRV_ERR_DPU_MALLOC_FAILED,
    DRV_ERR_DPU_MEMCPY_FALIED,
};

#ifdef NO_UART_DEBUG
#define dpu_print(fmt, args...) uapi_diag_debug_log(0, fmt, ##args)
#else
#define dpu_print(fmt, args...) printf(fmt, ##args)
#endif

#if defined(CONFIG_DPU_DFX_DEBUG)
#ifdef NO_UART_DEBUG
#define dpu_err(fmt, args...)  uapi_diag_error_log(0, fmt, ##args)
#define dpu_info(fmt, args...) uapi_diag_info_log(0, fmt, ##args)
#define dpu_dbg(fmt, args...)  uapi_diag_debug_log(0, fmt, ##args)
#else
#define dpu_err(fmt, args...)  printf("[DPU_ERROR][%s %d]"fmt"\n", __func__, __LINE__, ##args)
#define dpu_info(fmt, args...) printf("[DPU_INFO][%s %d]"fmt"\n", __func__, __LINE__, ##args)
#define dpu_dbg(fmt, args...)  printf("[DPU_DEBUG][%s %d]"fmt"\n", __func__, __LINE__, ##args)
#endif
#elif defined(CONFIG_DPU_DFX_ERROR)
#ifdef NO_UART_DEBUG
#define dpu_err(fmt, args...)  uapi_diag_error_log(0, fmt, ##args)
#define dpu_info(fmt, args...) uapi_diag_info_log(0, fmt, ##args)
#else
#define dpu_err(fmt, args...)  printf("[DPU_ERROR][%s %d]"fmt"\n", __func__, __LINE__, ##args)
#define dpu_info(fmt, args...) printf("[DPU_INFO][%s %d]"fmt"\n", __func__, __LINE__, ##args)
#endif
#define dpu_dbg(fmt, args...)
#else
#define dpu_err(fmt, args...)
#define dpu_info(fmt, args...)
#define dpu_dbg(fmt, args...)
#endif

typedef struct {
    td_u32 phy_addr;
    td_u32 width;
    td_u32 height;
    td_u32 stride;
} dpu_dfx_layer_info;

typedef struct {
    td_bool print_en;
    td_bool save_en;
    td_bool save_all;
    td_u32  save_layer;
    td_u32  save_cnt;

    td_float wait_frm_done;
    td_float refresh_intf_cost;

    td_u32 suspend_cnt;
    td_u32 resume_cnt;

    td_u32 lowband_cnt;
    td_u32 bus_err_cnt;

    td_u32 refresh_cnt;
    td_u32 frm_done_cnt;
    td_u32 te_signal_cnt;

    td_u32 refresh_total;
    td_u32 frm_done_total;
    td_u32 te_signal_total;

    td_u32 draw_fps; /* refresh fps */
    td_u32 flip_fps; /* display fps */
    td_u32 te_fps;   /* te signal fps */

    td_float te_cost_us;
    td_u32 last_te_time_us;

    td_u32 one_sec_ago; /* for fps calculate */

    dpu_dfx_layer_info layer_info[DRV_DPU_MAX_LAYER_ID + 1];
} dpu_dfx_info;

td_void dpu_dfx_print_info(void);
dpu_dfx_info *dpu_dfx_get_info(td_void);
td_s32 dfx_get_proc_info(int argc, const char *argv[]);
td_s32 dfx_dpu_print(int argc, const char *argv[]);
td_s32 dfx_dpu_save_fb_enable(int argc, const char *argv[]);
td_void dfx_dpu_save_fb_to_file(drv_dpu_layer_id layer_id, drv_dpu_surface *surface);
td_void dpu_dfx_proc_read(td_void);
td_void print_mipi_and_dpu_reg(int argc, const char *argv[]);

static inline td_void dpu_dfx_get_time_us(td_u32 *time_us)
{
    osal_timeval cur_time = { 0 };
    osal_gettimeofday(&cur_time);
    *time_us = (td_u32)(cur_time.tv_sec * 1000000 + cur_time.tv_usec); /* 1000000: index */
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

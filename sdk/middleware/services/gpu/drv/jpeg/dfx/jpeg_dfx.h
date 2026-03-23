/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: jpeg dfx
 */

#ifndef DRV_GRAPHICS_JPEG_DFX_H
#define DRV_GRAPHICS_JPEG_DFX_H

#include <stdio.h>
#include "td_base.h"
#include "soc_osal.h"
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

/*-------------------------------- macro define -------------------------------------------*/
#ifdef NO_UART_DEBUG
#define jpeg_err(fmt, args...)  uapi_diag_error_log(0, fmt, ##args)
#define jpeg_print(fmt, args...) uapi_diag_debug_log(0, fmt, ##args)
#else
#define jpeg_print(fmt, args...) printf(fmt, ##args)
#define jpeg_err(fmt, args...)  printf("[JPEG_ERROR][%s %d]"#fmt"\n", __func__, __LINE__, ##args)
#endif
/*-------------------------------- struct define ------------------------------------------*/

typedef struct {
    td_u32 open_times;
    td_u32 close_times;
    td_u32 stream_buf_size;
    td_u64 stream_phy_buf;
    td_u32 data_buf_size;
    td_u64 data_phy_buf;
    td_u64 y_phy_buf;
    td_u32 y_buf_stride;
    td_u32 y_mcu_height;
    td_u64 uv_phy_buf;
    td_u32 uv_buf_stride;
    td_u32 uv_mcu_height;
    td_u32 jpeg_color_space;
    td_u32 output_color_space;
    td_u32 scale;
    td_float start_cost;
    td_float get_status_cost;
    td_float init_cost;
    td_u32 create_cnt;
    td_u32 destroy_cnt;
} drv_jpeg_dfx_info;


/*-------------------------------- par define ---------------------------------------------*/


/*-------------------------------- func declares ------------------------------------------*/
td_void jpeg_dfx_get_time_us(td_u32 *time_us);
drv_jpeg_dfx_info *jpeg_dfx_get_info(td_void);
td_s32 jpeg_dfx_proc_read(int argc, const char *argv[]);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* DRV_GRAPHICS_JPEG_DFX_H */

/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: dfx manage
 */

#ifndef DRV_GRAPHICS_VAU_DFX_H
#define DRV_GRAPHICS_VAU_DFX_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <securec.h>
#include <math.h>
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
#define fractional_part(val) ((td_u32)((val) * 1000) % 1000)
#ifdef NO_UART_DEBUG
#define vau_print(fmt, args...) uapi_diag_debug_log(0, fmt, ##args)
#else
#define vau_print(fmt, args...) printf(fmt, ##args)
#endif

#if defined(CONFIG_VAU_DFX_DEBUG)
#ifdef NO_UART_DEBUG
#define vau_err(fmt, args...)      uapi_diag_error_log(0, fmt, ##args)
#define vau_info(fmt, args...)     uapi_diag_info_log(0, fmt, ##args)
#define vau_dbg(fmt, args...)      uapi_diag_debug_log(0, fmt, ##args)
#else
#define vau_err(fmt, args...)      printf("[VAU_ERROR][%s %d]"fmt"\n", __func__, __LINE__, ##args)
#define vau_info(fmt, args...)     printf("[VAU_INFO] [%s %d]"fmt"\n", __func__, __LINE__, ##args)
#define vau_dbg(fmt, args...)      printf("[VAU_DEBUG][%s %d]"fmt"\n", __func__, __LINE__, ##args)
#endif
#elif defined(CONFIG_VAU_DFX_ERROR)
#ifdef NO_UART_DEBUG
#define vau_err(fmt, args...)      uapi_diag_error_log(0, fmt, ##args)
#define vau_info(fmt, args...)     uapi_diag_info_log(0, fmt, ##args)
#else
#define vau_err(fmt, args...)      printf("[VAU_ERROR][%s %d]"fmt"\n", __func__, __LINE__, ##args)
#define vau_info(fmt, args...)     printf("[VAU_INFO] [%s %d]"fmt"\n", __func__, __LINE__, ##args)
#endif
#define vau_dbg(fmt, args...)
#else
#define vau_err(fmt, args...)
#define vau_info(fmt, args...)
#define vau_dbg(fmt, args...)
#endif

#define vau_check_left_larger_right_return_value(left_value, right_value, ret) do { \
    if ((left_value) > (right_value)) { \
        vau_err("%s:%d, %s:%d", #left_value, left_value, #right_value, right_value); \
        return ret; \
    } \
} while (0)

#define vau_check_left_equal_right_return_novalue(left_value, right_value) do { \
    if ((left_value) == (right_value)) { \
        vau_err("%s:%d, %s:%d", #left_value, left_value, #right_value, right_value); \
        return; \
    } \
} while (0)

#define vau_check_left_equal_right_return_value(left_value, right_value, ret) do { \
    if ((left_value) == (right_value)) { \
        vau_err("%s:%d, %s:%d", #left_value, left_value, #right_value, right_value); \
        return ret; \
    } \
} while (0)

#define vau_check_nullpointer_return_value(pointer, ret) do { \
    if ((pointer) == TD_NULL) {                               \
        vau_err("%s is null", #pointer);                      \
        return ret;                                           \
    }                                                         \
} while (0)

#define vau_check_nullpointer_return_novalue(pointer) do { \
    if ((pointer) == TD_NULL) {                            \
        vau_err("%s is null", #pointer);                   \
        return;                                            \
    }                                                      \
} while (0)


/*-------------------------------- struct define ------------------------------------------*/

enum {
    VAU_DBG_MASK_NONE                   = 0x0,
    VAU_DBG_MASK_PRINT_NODE_ALL         = 0x1,
    VAU_DBG_MASK_PRINT_NODE_NOZERO      = 0x2,
    VAU_DBG_MASK_PRINT_PATH             = 0x4,
    VAU_DBG_MASK_PRINT_REG_CFG          = 0x10,
    VAU_DBG_MASK_PRINT_ERROR_REG        = 0x20,
    VAU_DBG_MASK_PRINT_END_REG          = 0x40,
    VAU_DBG_MASK_PRINT_SURFACE          = 0x100,
    VAU_DBG_MASK_MATRIX_TILE_SEL4X4     = 0x1000,
    VAU_DBG_MASK_DMA2D_DISABLE          = 0x2000,
    VAU_DBG_MASK_ALPHA_STRB_DISABLE     = 0x4000,
    VAU_DBG_MASK_DRAW_CMD_RTN_DISABLE   = 0x10000,
    VAU_DBG_MASK_DRAW_PATH_RTN_DISABLE  = 0x20000,
    VAU_DBG_MASK_DRAW_BBOX_DISABLE      = 0x40000,
    VAU_DBG_MASK_T2R_DISABLE            = 0x80000,
    VAU_DBG_MASK_AFFINE_TRANS_DISABLE   = 0x100000,
};

typedef struct {
    td_u32 handle_num;
    td_u32 handle_size;
    td_u32 handle_addr;
    td_bool handle_busy[40];
    td_u32 job_num;
    td_u32 job_size;
} dfx_mem_info;

typedef struct {
    td_float create_cost;
    td_float destroy_cost;
    td_float blit_cost;
    td_float compose_cost;
    td_float submit_cost;
    td_float wait_for_done_cost;
    td_u32 list_pfcnt;
} dfx_time_info;

typedef struct {
    td_u32 create_cnt;
    td_u32 cancel_cnt;
    td_u32 destroy_cnt;
    td_u32 blit_cnt;
    td_u32 compose_cnt;
    td_u32 submit_cnt;
    td_u32 submit_usr_cnt;
    td_u32 wait_for_done_cnt;
    td_u32 submit_from_int;
    td_u32 submit_from_user;
    td_u32 isr_cnt;
    td_u32 tasklet_func_cnt;
    td_u32 execute_node_cnt;
} dfx_func_cnt;

typedef struct {
    td_u32 isr_status_zero_value_cnt;
    td_u32 isr_node_end;
    td_u32 isr_timeout;
    td_u32 isr_bus_err;
    td_u32 isr_list_end;
    td_u32 isr_disp_done;
    td_u32 isr_tunl_done;
    td_u32 isr_conflict;
} dfx_isr_cnt;

typedef struct {
    dfx_time_info time_info;
    dfx_func_cnt func_cnt;
    dfx_isr_cnt isr_cnt;
} dfx_func_info;

typedef struct {
    td_u32 suspend_cnt;
    td_u32 resume_cnt;
    dfx_mem_info mem_info;
    dfx_func_info func_info;
    td_bool is_print_vau_api;
    td_bool print_en;
} vau_dfx_info;

/*-------------------------------- func declares ------------------------------------------*/

static inline td_void vau_dfx_get_time_us(td_u32 *time_us)
{
    osal_timeval cur_time = { 0 };
    osal_gettimeofday(&cur_time);
    *time_us = (td_u32)(cur_time.tv_sec * 1000000 + cur_time.tv_usec); /* 1000000: index */
}

vau_dfx_info *vau_dfx_get_info(td_void);
td_bool vau_dfx_check_mask(td_u32 mask);
td_void vau_dfx_print_path(const td_u8 *cmds, const td_float *datas, td_u16 cmd_num, td_u16 data_num);
td_s32 vau_dfx_print(int argc, const char *argv[]);
td_s32 vau_dfx_proc_read(int argc, const char *argv[]);
td_s32 vau_dfx_proc_write(int argc, const char *argv[]);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* DRV_GRAPHICS_VAU_DFX_H */

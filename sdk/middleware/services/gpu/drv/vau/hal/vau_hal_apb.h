/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description : dpu reg common file
 */

#ifndef DRV_GRAPHIC_HAL_VAU_HAL_APB_H
#define DRV_GRAPHIC_HAL_VAU_HAL_APB_H

#include "td_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
#define VAU_VECTOR_RANGE_CMD 0
#define VAU_VECTOR_RANGE_DATA 1

td_void vau_hal_apb_init_queue(td_void);
td_bool vau_hal_apb_is_idle(td_void);
td_s32 vau_hal_apb_reset(td_bool reset);
td_u32 vau_hal_apb_get_current_ndoe(td_void);
td_u32 vau_hal_apb_get_softrst_state(td_void);
td_void vau_hal_apb_set_clock(td_bool enable);
td_u32 vau_hal_apb_get_int_state(td_void);
td_void vau_hal_apb_clear_int_state(td_u32 mask);
td_void vau_hal_apb_set_start_node(td_u32 start_node);
td_void vau_hal_apb_set_start_up(td_void);
td_void vau_hal_apb_set_video_ckdiv(td_u32 vedio_dpu_vau_ckdiv);
td_void vau_hal_apb_vector_draw_init(td_void);
td_void vau_hal_apb_print_reg(td_u32 base, td_u32 start, td_u32 end);
td_void vau_hal_apb_print_all_reg(td_void);
td_void vau_hal_apb_vector_cmddata_range(td_u32 type, td_u32 start, td_u32 end);
td_void vau_hal_apb_sram_ctrl(td_bool lp_en);
td_void vau_hal_apb_set_cubic_threshold(td_float cubic_threshold);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif

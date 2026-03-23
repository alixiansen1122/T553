/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description : dpu clock header
 */

#ifndef DRV_GRAPHICS_CLOCK_DPU_CLOCK_H
#define DRV_GRAPHICS_CLOCK_DPU_CLOCK_H

#include "td_base.h"
#include "dpu_hal.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

td_void dpu_clock_set_start_pos(void);
td_void dpu_clock_set_chnl_clk(td_u32 pixel_clk);
td_void dpu_clock_set_timing(dpu_disp_timing *disp_timing);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif /* DRV_GRAPHICS_CLOCK_DPU_CLOCK_H */
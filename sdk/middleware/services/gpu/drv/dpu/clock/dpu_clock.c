/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: dpu clock
 */
#include "dpu_clock.h"

#define DPU_CLOCK_START_POS  5

td_void dpu_clock_set_start_pos(void)
{
    dpu_hal_disp_set_start_pos(DPU_CLOCK_START_POS);
}

td_void dpu_clock_set_chnl_clk(td_u32 pixel_clk)
{
    dpu_hal_crg_set_chnl_clk(pixel_clk);
}

td_void dpu_clock_set_timing(dpu_disp_timing *disp_timing)
{
    dpu_hal_disp_set_timing(disp_timing);
}


/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: dpu disp header
 */

#ifndef DRV_GRAPHIC_DPU_DISP_DPU_DISP_H
#define DRV_GRAPHIC_DPU_DISP_DPU_DISP_H

#include "drv_dpu_type.h"
#include "soc_osal.h"
#include "dpu_hal.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    td_bool is_open;
    td_bool region_change;
    td_u32 open_cnt;
    td_u32 screen_width;
    td_u32 screen_height;
    ext_rect disp_region;
    dpu_disp_timing timing;
    osal_spinlock disp_lock;
} dpu_disp_info;

td_s32 dpu_disp_init(td_void);
td_void dpu_disp_deinit(td_void);

td_void dpu_disp_open(td_void);
td_void dpu_disp_close(td_void);

td_void dpu_disp_chl_cfg(td_void);
#ifdef GPU_SUPPORT_VIDEO_MODE
td_void dpu_disp_wait_vthd_percent_signal(td_void);
#endif
td_void dpu_disp_wait_te_signal(td_void);
td_void dpu_disp_wait_frame_done(td_void);
td_u32 dpu_disp_get_srceen_width(td_void);
td_u32 dpu_disp_get_srceen_height(td_void);
td_s32 dpu_disp_update_dhd_region(ext_rect *gp_region);
td_u32 dpu_disp_get_layer_zorder(td_void);
td_void dpu_disp_set_cbm_priority(drv_dpu_layer_id layer_id, drv_dpu_zorder_mode zorder);
td_void dpu_disp_set_timing_and_screen_region(td_void);
td_void dpu_disp_set_chnl_reg_up(td_void);

dpu_disp_info *dpu_disp_get_info(td_void);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

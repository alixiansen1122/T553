/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: dpu isr
 */

#ifndef DRV_GRAPHICS_ISR_DPU_ISR_H
#define DRV_GRAPHICS_ISR_DPU_ISR_H

#include "td_base.h"
#include "soc_osal.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

td_s32 dpu_isr_init(void);
td_void dpu_isr_deinit(void);

#ifdef GPU_SUPPORT_VIDEO_MODE
td_void dpu_isr_wait_vthd_percent_signal(void);
td_void dpu_isr_clear_vthd_percent_flag(void);
#endif

td_void dpu_isr_wait_finish(void);
td_void dpu_isr_wait_te_signal(void);
td_void dpu_isr_clear_finish_flag(void);
td_s32 dpu_wait_all_finish(td_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

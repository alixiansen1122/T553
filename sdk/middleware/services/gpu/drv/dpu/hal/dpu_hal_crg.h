/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description : dpu crg hal header
 */

#ifndef DRV_GRAPHICS_HAL_DPU_HAL_CRG_H
#define DRV_GRAPHICS_HAL_DPU_HAL_CRG_H

#include "td_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

td_void dpu_crg_reset(td_bool reset);
td_void dpu_crg_set_dpu_ppc_hd0_cken(td_u32 dpu_ppc_hd0_cken);
td_void dpu_crg_set_dpu_cfg_cken(td_u32 dpu_cfg_cken);
td_void dpu_crg_set_dpu_hd0_cken(td_u32 dpu_hd0_cken);
td_void dpu_crg_set_dpu_axi_cken(td_u32 dpu_axi_cken);
td_void dpu_crg_set_vedio_dpu_ppc_hd0_ckdiv(td_u32 dpu_ppc_hd0_ckdiv);

td_void dpu_crg_set_vedio_mclk_div_ctl(td_bool mclk_cken);
td_void dpu_crg_set_aon_ctrl_video_cken(td_bool aon_ctrl_video_cken);
td_void dpu_crg_set_video_160m_cken(td_bool video_160m_cken);
td_void dpu_crg_set_dpu_apb_cken(td_bool dpu_apb_cken);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif /* DRV_GRAPHICS_HAL_DPU_HAL_CRG_H */
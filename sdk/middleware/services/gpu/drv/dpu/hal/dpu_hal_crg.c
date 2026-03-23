/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: dpu crg reg
 */
#include "dpu_hal_crg.h"
#include "crg_reg_define.h"

td_void dpu_crg_reset(td_bool reset)
{
    volatile u_video_srst_reg video_srst_reg;

    if (reset) {
        uapi_reg_read(CRG_VEDIO_SRST_REG, video_srst_reg.u32);
        video_srst_reg.bits.dpu_srst_req = 1;
        uapi_reg_write(CRG_VEDIO_SRST_REG, video_srst_reg.u32);
    }

    uapi_reg_read(CRG_VEDIO_SRST_REG, video_srst_reg.u32);
    video_srst_reg.bits.dpu_srst_req = 0;
    uapi_reg_write(CRG_VEDIO_SRST_REG, video_srst_reg.u32);

    return;
}

td_void dpu_crg_set_dpu_ppc_hd0_cken(td_u32 dpu_ppc_hd0_cken)
{
    volatile u_vedio_crg_cken_ctl vedio_crg_cken_ctl;
    uapi_reg_read(CRG_VEDIO_CRG_CKEN_CTL, vedio_crg_cken_ctl.u32);
    vedio_crg_cken_ctl.bits.dpu_ppc_hd0_cken = dpu_ppc_hd0_cken;
    uapi_reg_write(CRG_VEDIO_CRG_CKEN_CTL, vedio_crg_cken_ctl.u32);
}

td_void dpu_crg_set_dpu_cfg_cken(td_u32 dpu_cfg_cken)
{
    volatile u_vedio_crg_cken_ctl vedio_crg_cken_ctl;
    uapi_reg_read(CRG_VEDIO_CRG_CKEN_CTL, vedio_crg_cken_ctl.u32);
    vedio_crg_cken_ctl.bits.dpu_cfg_cken = dpu_cfg_cken;
    uapi_reg_write(CRG_VEDIO_CRG_CKEN_CTL, vedio_crg_cken_ctl.u32);
}

td_void dpu_crg_set_dpu_hd0_cken(td_u32 dpu_hd0_cken)
{
    volatile u_vedio_crg_cken_ctl vedio_crg_cken_ctl;
    uapi_reg_read(CRG_VEDIO_CRG_CKEN_CTL, vedio_crg_cken_ctl.u32);
    vedio_crg_cken_ctl.bits.dpu_hd0_cken = dpu_hd0_cken;
    uapi_reg_write(CRG_VEDIO_CRG_CKEN_CTL, vedio_crg_cken_ctl.u32);
}

td_void dpu_crg_set_dpu_axi_cken(td_u32 dpu_axi_cken)
{
    volatile u_vedio_crg_cken_ctl vedio_crg_cken_ctl;
    uapi_reg_read(CRG_VEDIO_CRG_CKEN_CTL, vedio_crg_cken_ctl.u32);
    vedio_crg_cken_ctl.bits.dpu_axi_cken = dpu_axi_cken;
    uapi_reg_write(CRG_VEDIO_CRG_CKEN_CTL, vedio_crg_cken_ctl.u32);
}

td_void dpu_crg_set_vedio_dpu_ppc_hd0_ckdiv(td_u32 dpu_ppc_hd0_ckdiv)
{
    volatile u_vedio_crg_div_ctl vedio_crg_div_ctl;
    uapi_reg_read(CRG_VEDIO_CRG_DIV_CTL, vedio_crg_div_ctl.u32);
    vedio_crg_div_ctl.bits.vedio_dpu_ppc_hd0_ckdiv = dpu_ppc_hd0_ckdiv;
    uapi_reg_write(CRG_VEDIO_CRG_DIV_CTL, vedio_crg_div_ctl.u32);
}

td_void dpu_crg_set_vedio_mclk_div_ctl(td_bool mclk_cken)
{
    /* bit[26]:enable, bit[25:13]:div_denom,  bit[12:0]:div_num */
    volatile u_vedio_crg_mclk_div_ctl mclk_div_ctl;
    uapi_reg_read(CRG_VEDIO_CRG_MCLK_DIV_CTL, mclk_div_ctl.u32);
#ifdef GPU_SUPPORT_VIDEO_MODE
    mclk_div_ctl.bits.dpu_mclk_div  = 0x412600C; /* 147M * 12/147 = 12M */
#else
    mclk_div_ctl.bits.dpu_mclk_div  = 0x4126014; /* 147M * 20/147 = 20M */
#endif
    mclk_div_ctl.bits.dpu_mclk_cken = mclk_cken;
    uapi_reg_write(CRG_VEDIO_CRG_MCLK_DIV_CTL, mclk_div_ctl.u32);
}

td_void dpu_crg_set_aon_ctrl_video_cken(td_bool aon_ctrl_video_cken)
{
    volatile u_video_clken_cfg video_clken_cfg;
    uapi_reg_read(CRG_VEDIO_CLKEN_CFG, video_clken_cfg.u32);
    video_clken_cfg.bits.aon_ctrl_video_cken = aon_ctrl_video_cken;
    uapi_reg_write(CRG_VEDIO_CLKEN_CFG, video_clken_cfg.u32);
}

td_void dpu_crg_set_video_160m_cken(td_bool video_160m_cken)
{
    volatile u_video_clken_cfg video_clken_cfg;
    uapi_reg_read(CRG_VEDIO_CLKEN_CFG, video_clken_cfg.u32);
    video_clken_cfg.bits.vedio_pll2cbb_160m_cken = video_160m_cken;
    uapi_reg_write(CRG_VEDIO_CLKEN_CFG, video_clken_cfg.u32);
}

td_void dpu_crg_set_dpu_apb_cken(td_bool dpu_apb_cken)
{
    volatile u_video_clken_cfg video_clken_cfg;
    uapi_reg_read(CRG_VEDIO_CLKEN_CFG, video_clken_cfg.u32);
    video_clken_cfg.bits.dpu_apb_cken = dpu_apb_cken;
    uapi_reg_write(CRG_VEDIO_CLKEN_CFG, video_clken_cfg.u32);
}

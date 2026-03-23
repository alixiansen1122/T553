/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: dpu layer hal
 */

#include "dpu_hal_layer.h"

td_void dpu_reg_set_g0_ctrl_surface_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 surface_en)
{
    volatile u_g0_ctrl g0_ctrl;
    g0_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g0_ctrl.u32) + offset));
    g0_ctrl.bits.surface_en = surface_en;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g0_ctrl.u32) + offset), g0_ctrl.u32);

    return;
}

td_void dpu_reg_set_g0_ctrl_depremult(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 depremult)
{
    volatile u_g0_ctrl g0_ctrl;
    g0_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g0_ctrl.u32) + offset));
    g0_ctrl.bits.depremult = depremult;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g0_ctrl.u32) + offset), g0_ctrl.u32);

    return;
}

td_void dpu_reg_set_g0_ctrl_galpha(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 galpha)
{
    volatile u_g0_ctrl g0_ctrl;
    g0_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g0_ctrl.u32) + offset));
    g0_ctrl.bits.galpha = galpha;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g0_ctrl.u32) + offset), g0_ctrl.u32);

    return;
}

td_void dpu_reg_set_g0_upd_regup(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 regup)
{
    volatile u_g0_upd g0_upd;
    g0_upd.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g0_upd.u32) + offset));
    g0_upd.bits.regup = regup;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g0_upd.u32) + offset), g0_upd.u32);

    return;
}

td_u32 dpu_reg_get_g0_0reso_read_oh(const volatile dpu_regs_type *dpu_reg, td_u32 offset)
{
    volatile u_g0_0reso_read g0_0reso_read;
    g0_0reso_read.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g0_0reso_read.u32) + offset));
    return g0_0reso_read.bits.oh;
}

td_u32 dpu_reg_get_g0_0reso_read_ow(const volatile dpu_regs_type *dpu_reg, td_u32 offset)
{
    volatile u_g0_0reso_read g0_0reso_read;
    g0_0reso_read.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g0_0reso_read.u32) + offset));
    return g0_0reso_read.bits.ow;
}

td_void dpu_reg_set_g0_dfpos_disp_yfpos(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 disp_yfpos)
{
    volatile u_g0_dfpos g0_dfpos;
    g0_dfpos.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g0_dfpos.u32) + offset));
    g0_dfpos.bits.disp_yfpos = disp_yfpos;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g0_dfpos.u32) + offset), g0_dfpos.u32);

    return;
}

td_void dpu_reg_set_g0_dfpos_disp_xfpos(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 disp_xfpos)
{
    volatile u_g0_dfpos g0_dfpos;
    g0_dfpos.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g0_dfpos.u32) + offset));
    g0_dfpos.bits.disp_xfpos = disp_xfpos;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g0_dfpos.u32) + offset), g0_dfpos.u32);

    return;
}

td_void dpu_reg_set_g0_dlpos_disp_ylpos(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 disp_ylpos)
{
    volatile u_g0_dlpos g0_dlpos;
    g0_dlpos.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g0_dlpos.u32) + offset));
    g0_dlpos.bits.disp_ylpos = disp_ylpos;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g0_dlpos.u32) + offset), g0_dlpos.u32);

    return;
}

td_void dpu_reg_set_g0_dlpos_disp_xlpos(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 disp_xlpos)
{
    volatile u_g0_dlpos g0_dlpos;
    g0_dlpos.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g0_dlpos.u32) + offset));
    g0_dlpos.bits.disp_xlpos = disp_xlpos;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g0_dlpos.u32) + offset), g0_dlpos.u32);

    return;
}

td_void dpu_reg_set_g0_vfpos_video_yfpos(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 video_yfpos)
{
    volatile u_g0_vfpos g0_vfpos;
    g0_vfpos.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g0_vfpos.u32) + offset));
    g0_vfpos.bits.video_yfpos = video_yfpos;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g0_vfpos.u32) + offset), g0_vfpos.u32);

    return;
}

td_void dpu_reg_set_g0_vfpos_video_xfpos(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 video_xfpos)
{
    volatile u_g0_vfpos g0_vfpos;
    g0_vfpos.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g0_vfpos.u32) + offset));
    g0_vfpos.bits.video_xfpos = video_xfpos;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g0_vfpos.u32) + offset), g0_vfpos.u32);

    return;
}

td_void dpu_reg_set_g0_vlpos_video_ylpos(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 video_ylpos)
{
    volatile u_g0_vlpos g0_vlpos;
    g0_vlpos.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g0_vlpos.u32) + offset));
    g0_vlpos.bits.video_ylpos = video_ylpos;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g0_vlpos.u32) + offset), g0_vlpos.u32);

    return;
}

td_void dpu_reg_set_g0_vlpos_video_xlpos(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 video_xlpos)
{
    volatile u_g0_vlpos g0_vlpos;
    g0_vlpos.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g0_vlpos.u32) + offset));
    g0_vlpos.bits.video_xlpos = video_xlpos;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g0_vlpos.u32) + offset), g0_vlpos.u32);

    return;
}

td_void dpu_reg_set_g0_bk_vbk_y(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 vbk_y)
{
    volatile u_g0_bk g0_bk;
    g0_bk.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g0_bk.u32) + offset));
    g0_bk.bits.vbk_y = vbk_y;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g0_bk.u32) + offset), g0_bk.u32);

    return;
}

td_void dpu_reg_set_g0_bk_vbk_cb(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 vbk_cb)
{
    volatile u_g0_bk g0_bk;
    g0_bk.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g0_bk.u32) + offset));
    g0_bk.bits.vbk_cb = vbk_cb;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g0_bk.u32) + offset), g0_bk.u32);

    return;
}

td_void dpu_reg_set_g0_bk_vbk_cr(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 vbk_cr)
{
    volatile u_g0_bk g0_bk;
    g0_bk.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g0_bk.u32) + offset));
    g0_bk.bits.vbk_cr = vbk_cr;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g0_bk.u32) + offset), g0_bk.u32);

    return;
}

td_void dpu_reg_set_g0_alpha_vbk_alpha(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 vbk_alpha)
{
    volatile u_g0_alpha g0_alpha;
    g0_alpha.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g0_alpha.u32) + offset));
    g0_alpha.bits.vbk_alpha = vbk_alpha;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g0_alpha.u32) + offset), g0_alpha.u32);

    return;
}

td_void dpu_reg_set_g0_mute_bk_mute_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 mute_en)
{
    volatile u_g0_mute_bk g0_mute_bk;
    g0_mute_bk.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g0_mute_bk.u32) + offset));
    g0_mute_bk.bits.mute_en = mute_en;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g0_mute_bk.u32) + offset), g0_mute_bk.u32);

    return;
}

td_void dpu_reg_set_g0_mute_bk_mute_y(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 mute_y)
{
    volatile u_g0_mute_bk g0_mute_bk;
    g0_mute_bk.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g0_mute_bk.u32) + offset));
    g0_mute_bk.bits.mute_y = mute_y;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g0_mute_bk.u32) + offset), g0_mute_bk.u32);

    return;
}

td_void dpu_reg_set_g0_mute_bk_mute_cb(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 mute_cb)
{
    volatile u_g0_mute_bk g0_mute_bk;
    g0_mute_bk.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g0_mute_bk.u32) + offset));
    g0_mute_bk.bits.mute_cb = mute_cb;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g0_mute_bk.u32) + offset), g0_mute_bk.u32);

    return;
}

td_void dpu_reg_set_g0_mute_bk_mute_cr(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 mute_cr)
{
    volatile u_g0_mute_bk g0_mute_bk;
    g0_mute_bk.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g0_mute_bk.u32) + offset));
    g0_mute_bk.bits.mute_cr = mute_cr;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g0_mute_bk.u32) + offset), g0_mute_bk.u32);

    return;
}

td_void dpu_reg_set_g1_csc_ctrl_ck_gt_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 gt_en)
{
    volatile u_g1_csc_ctrl g1_csc_ctrl;
    g1_csc_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_csc_ctrl.u32) + offset));
    g1_csc_ctrl.bits.csc_ck_gt_en = gt_en;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_csc_ctrl.u32) + offset), g1_csc_ctrl.u32);

    return;
}

td_void dpu_reg_set_g1_csc_ctrl_demo_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 demo_en)
{
    volatile u_g1_csc_ctrl g1_csc_ctrl;
    g1_csc_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_csc_ctrl.u32) + offset));
    g1_csc_ctrl.bits.csc_demo_en = demo_en;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_csc_ctrl.u32) + offset), g1_csc_ctrl.u32);

    return;
}

td_void dpu_reg_set_g1_csc_ctrl_csc_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 csc_en)
{
    volatile u_g1_csc_ctrl g1_csc_ctrl;
    g1_csc_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_csc_ctrl.u32) + offset));
    g1_csc_ctrl.bits.csc_en = csc_en;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_csc_ctrl.u32) + offset), g1_csc_ctrl.u32);

    return;
}

td_void dpu_reg_set_g1_csc_coef00(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 coef00)
{
    volatile u_g1_csc_coef00 g1_csc_coef00;
    g1_csc_coef00.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_csc_coef00.u32) + offset));
    g1_csc_coef00.bits.csc_coef00 = coef00;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_csc_coef00.u32) + offset), g1_csc_coef00.u32);

    return;
}

td_void dpu_reg_set_g1_csc_coef01(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 coef01)
{
    volatile u_g1_csc_coef01 g1_csc_coef01;
    g1_csc_coef01.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_csc_coef01.u32) + offset));
    g1_csc_coef01.bits.csc_coef01 = coef01;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_csc_coef01.u32) + offset), g1_csc_coef01.u32);

    return;
}

td_void dpu_reg_set_g1_csc_coef02(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 coef02)
{
    volatile u_g1_csc_coef02 g1_csc_coef02;
    g1_csc_coef02.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_csc_coef02.u32) + offset));
    g1_csc_coef02.bits.csc_coef02 = coef02;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_csc_coef02.u32) + offset), g1_csc_coef02.u32);

    return;
}

td_void dpu_reg_set_g1_csc_coef10(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 coef10)
{
    volatile u_g1_csc_coef10 g1_csc_coef10;
    g1_csc_coef10.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_csc_coef10.u32) + offset));
    g1_csc_coef10.bits.csc_coef10 = coef10;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_csc_coef10.u32) + offset), g1_csc_coef10.u32);

    return;
}

td_void dpu_reg_set_g1_csc_coef11(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 coef11)
{
    volatile u_g1_csc_coef11 g1_csc_coef11;
    g1_csc_coef11.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_csc_coef11.u32) + offset));
    g1_csc_coef11.bits.csc_coef11 = coef11;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_csc_coef11.u32) + offset), g1_csc_coef11.u32);

    return;
}

td_void dpu_reg_set_g1_csc_coef12(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 coef12)
{
    volatile u_g1_csc_coef12 g1_csc_coef12;
    g1_csc_coef12.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_csc_coef12.u32) + offset));
    g1_csc_coef12.bits.csc_coef12 = coef12;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_csc_coef12.u32) + offset), g1_csc_coef12.u32);

    return;
}

td_void dpu_reg_set_g1_csc_coef20(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 coef20)
{
    volatile u_g1_csc_coef20 g1_csc_coef20;
    g1_csc_coef20.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_csc_coef20.u32) + offset));
    g1_csc_coef20.bits.csc_coef20 = coef20;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_csc_coef20.u32) + offset), g1_csc_coef20.u32);

    return;
}

td_void dpu_reg_set_g1_csc_coef21(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 coef21)
{
    volatile u_g1_csc_coef21 g1_csc_coef21;
    g1_csc_coef21.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_csc_coef21.u32) + offset));
    g1_csc_coef21.bits.csc_coef21 = coef21;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_csc_coef21.u32) + offset), g1_csc_coef21.u32);

    return;
}

td_void dpu_reg_set_g1_csc_coef22(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 coef22)
{
    volatile u_g1_csc_coef22 g1_csc_coef22;
    g1_csc_coef22.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_csc_coef22.u32) + offset));
    g1_csc_coef22.bits.csc_coef22 = coef22;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_csc_coef22.u32) + offset), g1_csc_coef22.u32);

    return;
}

td_void dpu_reg_set_g1_csc_scale(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 csc_scale)
{
    volatile u_g1_csc_scale g1_csc_scale;
    g1_csc_scale.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_csc_scale.u32) + offset));
    g1_csc_scale.bits.csc_scale = csc_scale;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_csc_scale.u32) + offset), g1_csc_scale.u32);

    return;
}

td_void dpu_reg_set_g1_csc_idc0(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 csc_idc0)
{
    volatile u_g1_csc_idc0 g1_csc_idc0;
    g1_csc_idc0.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_csc_idc0.u32) + offset));
    g1_csc_idc0.bits.csc_idc0 = csc_idc0;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_csc_idc0.u32) + offset), g1_csc_idc0.u32);

    return;
}

td_void dpu_reg_set_g1_csc_idc1(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 csc_idc1)
{
    volatile u_g1_csc_idc1 g1_csc_idc1;
    g1_csc_idc1.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_csc_idc1.u32) + offset));
    g1_csc_idc1.bits.csc_idc1 = csc_idc1;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_csc_idc1.u32) + offset), g1_csc_idc1.u32);

    return;
}

td_void dpu_reg_set_g1_csc_idc2(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 csc_idc2)
{
    volatile u_g1_csc_idc2 g1_csc_idc2;
    g1_csc_idc2.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_csc_idc2.u32) + offset));
    g1_csc_idc2.bits.csc_idc2 = csc_idc2;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_csc_idc2.u32) + offset), g1_csc_idc2.u32);

    return;
}

td_void dpu_reg_set_g1_csc_odc0(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 csc_odc0)
{
    volatile u_g1_csc_odc0 g1_csc_odc0;
    g1_csc_odc0.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_csc_odc0.u32) + offset));
    g1_csc_odc0.bits.csc_odc0 = csc_odc0;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_csc_odc0.u32) + offset), g1_csc_odc0.u32);

    return;
}

td_void dpu_reg_set_g1_csc_odc1(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 csc_odc1)
{
    volatile u_g1_csc_odc1 g1_csc_odc1;
    g1_csc_odc1.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_csc_odc1.u32) + offset));
    g1_csc_odc1.bits.csc_odc1 = csc_odc1;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_csc_odc1.u32) + offset), g1_csc_odc1.u32);

    return;
}

td_void dpu_reg_set_g1_csc_odc2(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 csc_odc2)
{
    volatile u_g1_csc_odc2 g1_csc_odc2;
    g1_csc_odc2.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_csc_odc2.u32) + offset));
    g1_csc_odc2.bits.csc_odc2 = csc_odc2;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_csc_odc2.u32) + offset), g1_csc_odc2.u32);

    return;
}

td_void dpu_reg_set_g1_csc_min_y(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 csc_min_y)
{
    volatile u_g1_csc_min_y g1_csc_min_y;
    g1_csc_min_y.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_csc_min_y.u32) + offset));
    g1_csc_min_y.bits.csc_min_y = csc_min_y;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_csc_min_y.u32) + offset), g1_csc_min_y.u32);

    return;
}

td_void dpu_reg_set_g1_csc_min_c(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 csc_min_c)
{
    volatile u_g1_csc_min_c g1_csc_min_c;
    g1_csc_min_c.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_csc_min_c.u32) + offset));
    g1_csc_min_c.bits.csc_min_c = csc_min_c;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_csc_min_c.u32) + offset), g1_csc_min_c.u32);

    return;
}

td_void dpu_reg_set_g1_csc_max_y(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 csc_max_y)
{
    volatile u_g1_csc_max_y g1_csc_max_y;
    g1_csc_max_y.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_csc_max_y.u32) + offset));
    g1_csc_max_y.bits.csc_max_y = csc_max_y;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_csc_max_y.u32) + offset), g1_csc_max_y.u32);

    return;
}

td_void dpu_reg_set_g1_csc_max_c(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 csc_max_c)
{
    volatile u_g1_csc_max_c g1_csc_max_c;
    g1_csc_max_c.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_csc_max_c.u32) + offset));
    g1_csc_max_c.bits.csc_max_c = csc_max_c;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_csc_max_c.u32) + offset), g1_csc_max_c.u32);

    return;
}

td_void dpu_reg_set_g1_csc2_coef00(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 coef00)
{
    volatile u_g1_csc2_coef00 g1_csc2_coef00;
    g1_csc2_coef00.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_csc2_coef00.u32) + offset));
    g1_csc2_coef00.bits.csc2_coef00 = coef00;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_csc2_coef00.u32) + offset), g1_csc2_coef00.u32);

    return;
}

td_void dpu_reg_set_g1_csc2_coef01(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 coef01)
{
    volatile u_g1_csc2_coef01 g1_csc2_coef01;
    g1_csc2_coef01.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_csc2_coef01.u32) + offset));
    g1_csc2_coef01.bits.csc2_coef01 = coef01;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_csc2_coef01.u32) + offset), g1_csc2_coef01.u32);

    return;
}

td_void dpu_reg_set_g1_csc2_coef02(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 coef02)
{
    volatile u_g1_csc2_coef02 g1_csc2_coef02;
    g1_csc2_coef02.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_csc2_coef02.u32) + offset));
    g1_csc2_coef02.bits.csc2_coef02 = coef02;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_csc2_coef02.u32) + offset), g1_csc2_coef02.u32);

    return;
}

td_void dpu_reg_set_g1_csc2_coef10(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 coef10)
{
    volatile u_g1_csc2_coef10 g1_csc2_coef10;
    g1_csc2_coef10.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_csc2_coef10.u32) + offset));
    g1_csc2_coef10.bits.csc2_coef10 = coef10;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_csc2_coef10.u32) + offset), g1_csc2_coef10.u32);

    return;
}

td_void dpu_reg_set_g1_csc2_coef11(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 coef11)
{
    volatile u_g1_csc2_coef11 g1_csc2_coef11;
    g1_csc2_coef11.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_csc2_coef11.u32) + offset));
    g1_csc2_coef11.bits.csc2_coef11 = coef11;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_csc2_coef11.u32) + offset), g1_csc2_coef11.u32);

    return;
}

td_void dpu_reg_set_g1_csc2_coef12(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 coef12)
{
    volatile u_g1_csc2_coef12 g1_csc2_coef12;
    g1_csc2_coef12.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_csc2_coef12.u32) + offset));
    g1_csc2_coef12.bits.csc2_coef12 = coef12;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_csc2_coef12.u32) + offset), g1_csc2_coef12.u32);

    return;
}

td_void dpu_reg_set_g1_csc2_coef20(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 coef20)
{
    volatile u_g1_csc2_coef20 g1_csc2_coef20;
    g1_csc2_coef20.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_csc2_coef20.u32) + offset));
    g1_csc2_coef20.bits.csc2_coef20 = coef20;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_csc2_coef20.u32) + offset), g1_csc2_coef20.u32);

    return;
}

td_void dpu_reg_set_g1_csc2_coef21(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 coef21)
{
    volatile u_g1_csc2_coef21 g1_csc2_coef21;
    g1_csc2_coef21.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_csc2_coef21.u32) + offset));
    g1_csc2_coef21.bits.csc2_coef21 = coef21;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_csc2_coef21.u32) + offset), g1_csc2_coef21.u32);

    return;
}

td_void dpu_reg_set_g1_csc2_coef22(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 coef22)
{
    volatile u_g1_csc2_coef22 g1_csc2_coef22;
    g1_csc2_coef22.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_csc2_coef22.u32) + offset));
    g1_csc2_coef22.bits.csc2_coef22 = coef22;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_csc2_coef22.u32) + offset), g1_csc2_coef22.u32);

    return;
}

td_void dpu_reg_set_g1_csc2_scale(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 csc2_scale)
{
    volatile u_g1_csc2_scale g1_csc2_scale;
    g1_csc2_scale.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_csc2_scale.u32) + offset));
    g1_csc2_scale.bits.csc2_scale = csc2_scale;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_csc2_scale.u32) + offset), g1_csc2_scale.u32);

    return;
}

td_void dpu_reg_set_g1_csc2_idc0(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 csc2_idc0)
{
    volatile u_g1_csc2_idc0 g1_csc2_idc0;
    g1_csc2_idc0.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_csc2_idc0.u32) + offset));
    g1_csc2_idc0.bits.csc2_idc0 = csc2_idc0;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_csc2_idc0.u32) + offset), g1_csc2_idc0.u32);

    return;
}

td_void dpu_reg_set_g1_csc2_idc1(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 csc2_idc1)
{
    volatile u_g1_csc2_idc1 g1_csc2_idc1;
    g1_csc2_idc1.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_csc2_idc1.u32) + offset));
    g1_csc2_idc1.bits.csc2_idc1 = csc2_idc1;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_csc2_idc1.u32) + offset), g1_csc2_idc1.u32);

    return;
}

td_void dpu_reg_set_g1_csc2_idc2(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 csc2_idc2)
{
    volatile u_g1_csc2_idc2 g1_csc2_idc2;
    g1_csc2_idc2.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_csc2_idc2.u32) + offset));
    g1_csc2_idc2.bits.csc2_idc2 = csc2_idc2;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_csc2_idc2.u32) + offset), g1_csc2_idc2.u32);

    return;
}

td_void dpu_reg_set_g1_csc2_odc0(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 csc2_odc0)
{
    volatile u_g1_csc2_odc0 g1_csc2_odc0;
    g1_csc2_odc0.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_csc2_odc0.u32) + offset));
    g1_csc2_odc0.bits.csc2_odc0 = csc2_odc0;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_csc2_odc0.u32) + offset), g1_csc2_odc0.u32);

    return;
}

td_void dpu_reg_set_g1_csc2_odc1(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 csc2_odc1)
{
    volatile u_g1_csc2_odc1 g1_csc2_odc1;
    g1_csc2_odc1.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_csc2_odc1.u32) + offset));
    g1_csc2_odc1.bits.csc2_odc1 = csc2_odc1;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_csc2_odc1.u32) + offset), g1_csc2_odc1.u32);

    return;
}

td_void dpu_reg_set_g1_csc2_odc2(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 csc2_odc2)
{
    volatile u_g1_csc2_odc2 g1_csc2_odc2;
    g1_csc2_odc2.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_csc2_odc2.u32) + offset));
    g1_csc2_odc2.bits.csc2_odc2 = csc2_odc2;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_csc2_odc2.u32) + offset), g1_csc2_odc2.u32);

    return;
}

td_void dpu_reg_set_g1_csc2_min_y(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 csc2_min_y)
{
    volatile u_g1_csc2_min_y g1_csc2_min_y;
    g1_csc2_min_y.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_csc2_min_y.u32) + offset));
    g1_csc2_min_y.bits.csc2_min_y = csc2_min_y;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_csc2_min_y.u32) + offset), g1_csc2_min_y.u32);

    return;
}

td_void dpu_reg_set_g1_csc2_min_c(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 csc2_min_c)
{
    volatile u_g1_csc2_min_c g1_csc2_min_c;
    g1_csc2_min_c.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_csc2_min_c.u32) + offset));
    g1_csc2_min_c.bits.csc2_min_c = csc2_min_c;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_csc2_min_c.u32) + offset), g1_csc2_min_c.u32);

    return;
}

td_void dpu_reg_set_g1_csc2_max_y(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 csc2_max_y)
{
    volatile u_g1_csc2_max_y g1_csc2_max_y;
    g1_csc2_max_y.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_csc2_max_y.u32) + offset));
    g1_csc2_max_y.bits.csc2_max_y = csc2_max_y;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_csc2_max_y.u32) + offset), g1_csc2_max_y.u32);

    return;
}

td_void dpu_reg_set_g1_csc2_max_c(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 csc2_max_c)
{
    volatile u_g1_csc2_max_c g1_csc2_max_c;
    g1_csc2_max_c.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_csc2_max_c.u32) + offset));
    g1_csc2_max_c.bits.csc2_max_c = csc2_max_c;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_csc2_max_c.u32) + offset), g1_csc2_max_c.u32);

    return;
}

td_void dpu_reg_set_g1_csc_ink_ctrl_color_mode(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 mode)
{
    volatile u_g1_csc_ink_ctrl g1_csc_ink_ctrl;
    g1_csc_ink_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_csc_ink_ctrl.u32) + offset));
    g1_csc_ink_ctrl.bits.color_mode = mode;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_csc_ink_ctrl.u32) + offset), g1_csc_ink_ctrl.u32);

    return;
}

td_void dpu_reg_set_g1_csc_ink_ctrl_cross_enable(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 enable)
{
    volatile u_g1_csc_ink_ctrl g1_csc_ink_ctrl;
    g1_csc_ink_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_csc_ink_ctrl.u32) + offset));
    g1_csc_ink_ctrl.bits.cross_enable = enable;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_csc_ink_ctrl.u32) + offset), g1_csc_ink_ctrl.u32);

    return;
}

td_void dpu_reg_set_g1_csc_ink_ctrl_data_fmt(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 data_fmt)
{
    volatile u_g1_csc_ink_ctrl g1_csc_ink_ctrl;
    g1_csc_ink_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_csc_ink_ctrl.u32) + offset));
    g1_csc_ink_ctrl.bits.data_fmt = data_fmt;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_csc_ink_ctrl.u32) + offset), g1_csc_ink_ctrl.u32);

    return;
}

td_void dpu_reg_set_g1_csc_ink_ctrl_ink_sel(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 ink_sel)
{
    volatile u_g1_csc_ink_ctrl g1_csc_ink_ctrl;
    g1_csc_ink_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_csc_ink_ctrl.u32) + offset));
    g1_csc_ink_ctrl.bits.ink_sel = ink_sel;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_csc_ink_ctrl.u32) + offset), g1_csc_ink_ctrl.u32);

    return;
}

td_void dpu_reg_set_g1_csc_ink_ctrl_ink_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 ink_en)
{
    volatile u_g1_csc_ink_ctrl g1_csc_ink_ctrl;
    g1_csc_ink_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_csc_ink_ctrl.u32) + offset));
    g1_csc_ink_ctrl.bits.ink_en = ink_en;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_csc_ink_ctrl.u32) + offset), g1_csc_ink_ctrl.u32);

    return;
}

td_void dpu_reg_set_g1_csc_ink_pos_y_pos(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 y_pos)
{
    volatile u_g1_csc_ink_pos g1_csc_ink_pos;
    g1_csc_ink_pos.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_csc_ink_pos.u32) + offset));
    g1_csc_ink_pos.bits.y_pos = y_pos;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_csc_ink_pos.u32) + offset), g1_csc_ink_pos.u32);

    return;
}

td_void dpu_reg_set_g1_csc_ink_pos_x_pos(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 x_pos)
{
    volatile u_g1_csc_ink_pos g1_csc_ink_pos;
    g1_csc_ink_pos.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_csc_ink_pos.u32) + offset));
    g1_csc_ink_pos.bits.x_pos = x_pos;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_csc_ink_pos.u32) + offset), g1_csc_ink_pos.u32);

    return;
}

td_void dpu_reg_set_g1_csc_ink_data_ink_data(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 ink_data)
{
    volatile u_g1_csc_ink_data g1_csc_ink_data;
    g1_csc_ink_data.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_csc_ink_data.u32) + offset));
    g1_csc_ink_data.bits.ink_data = ink_data;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_csc_ink_data.u32) + offset), g1_csc_ink_data.u32);

    return;
}

td_void dpu_reg_set_g1_csc_ink_data2_ink_data2(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 ink_data2)
{
    volatile u_g1_csc_ink_data2 g1_csc_ink_data2;
    g1_csc_ink_data2.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_csc_ink_data2.u32) + offset));
    g1_csc_ink_data2.bits.ink_data2 = ink_data2;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_csc_ink_data2.u32) + offset), g1_csc_ink_data2.u32);

    return;
}

td_void dpu_reg_set_g1_cvfir_vinfo_vzme_ck_gt_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 gt_en)
{
    volatile u_g1_cvfir_vinfo g1_cvfir_vinfo;
    g1_cvfir_vinfo.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_cvfir_vinfo.u32) + offset));
    g1_cvfir_vinfo.bits.vzme_ck_gt_en = gt_en;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_cvfir_vinfo.u32) + offset), g1_cvfir_vinfo.u32);

    return;
}

td_void dpu_reg_set_g1_cvfir_vinfo_out_pro(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 out_pro)
{
    volatile u_g1_cvfir_vinfo g1_cvfir_vinfo;
    g1_cvfir_vinfo.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_cvfir_vinfo.u32) + offset));
    g1_cvfir_vinfo.bits.out_pro = out_pro;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_cvfir_vinfo.u32) + offset), g1_cvfir_vinfo.u32);

    return;
}

td_void dpu_reg_set_g1_cvfir_vinfo_out_fmt(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 out_fmt)
{
    volatile u_g1_cvfir_vinfo g1_cvfir_vinfo;
    g1_cvfir_vinfo.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_cvfir_vinfo.u32) + offset));
    g1_cvfir_vinfo.bits.out_fmt = out_fmt;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_cvfir_vinfo.u32) + offset), g1_cvfir_vinfo.u32);

    return;
}

td_void dpu_reg_set_g1_cvfir_vinfo_out_height(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 out_height)
{
    volatile u_g1_cvfir_vinfo g1_cvfir_vinfo;
    g1_cvfir_vinfo.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_cvfir_vinfo.u32) + offset));
    g1_cvfir_vinfo.bits.out_height = out_height;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_cvfir_vinfo.u32) + offset), g1_cvfir_vinfo.u32);

    return;
}

td_void dpu_reg_set_g1_cvfir_vsp_cvfir_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 cvfir_en)
{
    volatile u_g1_cvfir_vsp g1_cvfir_vsp;
    g1_cvfir_vsp.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_cvfir_vsp.u32) + offset));
    g1_cvfir_vsp.bits.cvfir_en = cvfir_en;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_cvfir_vsp.u32) + offset), g1_cvfir_vsp.u32);

    return;
}

td_void dpu_reg_set_g1_cvfir_vsp_cvmid_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 cvmid_en)
{
    volatile u_g1_cvfir_vsp g1_cvfir_vsp;
    g1_cvfir_vsp.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_cvfir_vsp.u32) + offset));
    g1_cvfir_vsp.bits.cvmid_en = cvmid_en;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_cvfir_vsp.u32) + offset), g1_cvfir_vsp.u32);

    return;
}

td_void dpu_reg_set_g1_cvfir_vsp_cvfir_mode(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 cvfir_mode)
{
    volatile u_g1_cvfir_vsp g1_cvfir_vsp;
    g1_cvfir_vsp.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_cvfir_vsp.u32) + offset));
    g1_cvfir_vsp.bits.cvfir_mode = cvfir_mode;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_cvfir_vsp.u32) + offset), g1_cvfir_vsp.u32);

    return;
}

td_void dpu_reg_set_g1_cvfir_vsp_vratio(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 vratio)
{
    volatile u_g1_cvfir_vsp g1_cvfir_vsp;
    g1_cvfir_vsp.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_cvfir_vsp.u32) + offset));
    g1_cvfir_vsp.bits.vratio = vratio;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_cvfir_vsp.u32) + offset), g1_cvfir_vsp.u32);

    return;
}

td_void dpu_reg_set_g1_cvfir_vchroma_offset(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 vchroma_offset)
{
    volatile u_g1_cvfir_voffset g1_cvfir_voffset;
    g1_cvfir_voffset.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_cvfir_voffset.u32) + offset));
    g1_cvfir_voffset.bits.vchroma_offset = vchroma_offset;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_cvfir_voffset.u32) + offset), g1_cvfir_voffset.u32);

    return;
}

td_void dpu_reg_set_g1_cvfir_vbchroma_offset(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 vbchroma_offset)
{
    volatile u_g1_cvfir_vboffset g1_cvfir_vboffset;
    g1_cvfir_vboffset.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_cvfir_vboffset.u32) + offset));
    g1_cvfir_vboffset.bits.vbchroma_offset = vbchroma_offset;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_cvfir_vboffset.u32) + offset), g1_cvfir_vboffset.u32);

    return;
}

td_void dpu_reg_set_g1_cvfir_vcoef0_vccoef00(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 vccoef00)
{
    volatile u_g1_cvfir_vcoef0 g1_cvfir_vcoef0;
    g1_cvfir_vcoef0.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_cvfir_vcoef0.u32) + offset));
    g1_cvfir_vcoef0.bits.vccoef00 = vccoef00;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_cvfir_vcoef0.u32) + offset), g1_cvfir_vcoef0.u32);

    return;
}

td_void dpu_reg_set_g1_cvfir_vcoef0_vccoef01(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 vccoef01)
{
    volatile u_g1_cvfir_vcoef0 g1_cvfir_vcoef0;
    g1_cvfir_vcoef0.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_cvfir_vcoef0.u32) + offset));
    g1_cvfir_vcoef0.bits.vccoef01 = vccoef01;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_cvfir_vcoef0.u32) + offset), g1_cvfir_vcoef0.u32);

    return;
}

td_void dpu_reg_set_g1_cvfir_vcoef0_vccoef02(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 vccoef02)
{
    volatile u_g1_cvfir_vcoef0 g1_cvfir_vcoef0;
    g1_cvfir_vcoef0.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_cvfir_vcoef0.u32) + offset));
    g1_cvfir_vcoef0.bits.vccoef02 = vccoef02;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_cvfir_vcoef0.u32) + offset), g1_cvfir_vcoef0.u32);

    return;
}

td_void dpu_reg_set_g1_cvfir_vcoef1_vccoef03(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 vccoef03)
{
    volatile u_g1_cvfir_vcoef1 g1_cvfir_vcoef1;
    g1_cvfir_vcoef1.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_cvfir_vcoef1.u32) + offset));
    g1_cvfir_vcoef1.bits.vccoef03 = vccoef03;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_cvfir_vcoef1.u32) + offset), g1_cvfir_vcoef1.u32);

    return;
}

td_void dpu_reg_set_g1_cvfir_vcoef1_vccoef10(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 vccoef10)
{
    volatile u_g1_cvfir_vcoef1 g1_cvfir_vcoef1;
    g1_cvfir_vcoef1.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_cvfir_vcoef1.u32) + offset));
    g1_cvfir_vcoef1.bits.vccoef10 = vccoef10;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_cvfir_vcoef1.u32) + offset), g1_cvfir_vcoef1.u32);

    return;
}

td_void dpu_reg_set_g1_cvfir_vcoef1_vccoef11(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 vccoef11)
{
    volatile u_g1_cvfir_vcoef1 g1_cvfir_vcoef1;
    g1_cvfir_vcoef1.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_cvfir_vcoef1.u32) + offset));
    g1_cvfir_vcoef1.bits.vccoef11 = vccoef11;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_cvfir_vcoef1.u32) + offset), g1_cvfir_vcoef1.u32);

    return;
}

td_void dpu_reg_set_g1_cvfir_vcoef2_vccoef12(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 vccoef12)
{
    volatile u_g1_cvfir_vcoef2 g1_cvfir_vcoef2;
    g1_cvfir_vcoef2.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_cvfir_vcoef2.u32) + offset));
    g1_cvfir_vcoef2.bits.vccoef12 = vccoef12;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_cvfir_vcoef2.u32) + offset), g1_cvfir_vcoef2.u32);

    return;
}

td_void dpu_reg_set_g1_cvfir_vcoef2_vccoef13(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 vccoef13)
{
    volatile u_g1_cvfir_vcoef2 g1_cvfir_vcoef2;
    g1_cvfir_vcoef2.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_cvfir_vcoef2.u32) + offset));
    g1_cvfir_vcoef2.bits.vccoef13 = vccoef13;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_cvfir_vcoef2.u32) + offset), g1_cvfir_vcoef2.u32);

    return;
}

td_void dpu_reg_set_g1_hfir_ctrl_ck_gt_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 ck_gt_en)
{
    volatile u_g1_hfir_ctrl g1_hfir_ctrl;
    g1_hfir_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_hfir_ctrl.u32) + offset));
    g1_hfir_ctrl.bits.ck_gt_en = ck_gt_en;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_hfir_ctrl.u32) + offset), g1_hfir_ctrl.u32);

    return;
}

td_void dpu_reg_set_g1_hfir_ctrl_mid_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 mid_en)
{
    volatile u_g1_hfir_ctrl g1_hfir_ctrl;
    g1_hfir_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_hfir_ctrl.u32) + offset));
    g1_hfir_ctrl.bits.mid_en = mid_en;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_hfir_ctrl.u32) + offset), g1_hfir_ctrl.u32);

    return;
}

td_void dpu_reg_set_g1_hfir_ctrl_hfir_mode(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 hfir_mode)
{
    volatile u_g1_hfir_ctrl g1_hfir_ctrl;
    g1_hfir_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_hfir_ctrl.u32) + offset));
    g1_hfir_ctrl.bits.hfir_mode = hfir_mode;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_hfir_ctrl.u32) + offset), g1_hfir_ctrl.u32);

    return;
}

td_void dpu_reg_set_g1_hfircoef01_coef1(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 coef1)
{
    volatile u_g1_hfircoef01 g1_hfircoef01;
    g1_hfircoef01.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_hfircoef01.u32) + offset));
    g1_hfircoef01.bits.coef1 = coef1;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_hfircoef01.u32) + offset), g1_hfircoef01.u32);

    return;
}

td_void dpu_reg_set_g1_hfircoef01_coef0(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 coef0)
{
    volatile u_g1_hfircoef01 g1_hfircoef01;
    g1_hfircoef01.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_hfircoef01.u32) + offset));
    g1_hfircoef01.bits.coef0 = coef0;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_hfircoef01.u32) + offset), g1_hfircoef01.u32);

    return;
}

td_void dpu_reg_set_g1_hfircoef23_coef3(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 coef3)
{
    volatile u_g1_hfircoef23 g1_hfircoef23;
    g1_hfircoef23.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_hfircoef23.u32) + offset));
    g1_hfircoef23.bits.coef3 = coef3;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_hfircoef23.u32) + offset), g1_hfircoef23.u32);

    return;
}

td_void dpu_reg_set_g1_hfircoef23_coef2(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 coef2)
{
    volatile u_g1_hfircoef23 g1_hfircoef23;
    g1_hfircoef23.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_hfircoef23.u32) + offset));
    g1_hfircoef23.bits.coef2 = coef2;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_hfircoef23.u32) + offset), g1_hfircoef23.u32);

    return;
}

td_void dpu_reg_set_g1_hfircoef45_coef5(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 coef5)
{
    volatile u_g1_hfircoef45 g1_hfircoef45;
    g1_hfircoef45.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_hfircoef45.u32) + offset));
    g1_hfircoef45.bits.coef5 = coef5;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_hfircoef45.u32) + offset), g1_hfircoef45.u32);

    return;
}

td_void dpu_reg_set_g1_hfircoef45_coef4(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 coef4)
{
    volatile u_g1_hfircoef45 g1_hfircoef45;
    g1_hfircoef45.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_hfircoef45.u32) + offset));
    g1_hfircoef45.bits.coef4 = coef4;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_hfircoef45.u32) + offset), g1_hfircoef45.u32);

    return;
}

td_void dpu_reg_set_g1_hfircoef67_coef7(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 coef7)
{
    volatile u_g1_hfircoef67 g1_hfircoef67;
    g1_hfircoef67.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_hfircoef67.u32) + offset));
    g1_hfircoef67.bits.coef7 = coef7;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_hfircoef67.u32) + offset), g1_hfircoef67.u32);

    return;
}

td_void dpu_reg_set_g1_hfircoef67_coef6(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 coef6)
{
    volatile u_g1_hfircoef67 g1_hfircoef67;
    g1_hfircoef67.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->g1_hfircoef67.u32) + offset));
    g1_hfircoef67.bits.coef6 = coef6;
    dpu_reg_write((uintptr_t)(&(dpu_reg->g1_hfircoef67.u32) + offset), g1_hfircoef67.u32);

    return;
}
td_void dpu_reg_set_gfx_read_ctrl_fdr_ck_gt_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 fdr_ck_gt_en)
{
    volatile u_gfx_read_ctrl gfx_read_ctrl;
    gfx_read_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_read_ctrl.u32) + offset));
    gfx_read_ctrl.bits.fdr_ck_gt_en = fdr_ck_gt_en;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_read_ctrl.u32) + offset), gfx_read_ctrl.u32);

    return;
}

td_void dpu_reg_set_gfx_read_ctrl_mute_req_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 mute_req_en)
{
    volatile u_gfx_read_ctrl gfx_read_ctrl;
    gfx_read_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_read_ctrl.u32) + offset));
    gfx_read_ctrl.bits.mute_req_en = mute_req_en;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_read_ctrl.u32) + offset), gfx_read_ctrl.u32);

    return;
}

td_void dpu_reg_set_gfx_read_ctrl_mute_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 mute_en)
{
    volatile u_gfx_read_ctrl gfx_read_ctrl;
    gfx_read_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_read_ctrl.u32) + offset));
    gfx_read_ctrl.bits.mute_en = mute_en;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_read_ctrl.u32) + offset), gfx_read_ctrl.u32);

    return;
}

td_void dpu_reg_set_gfx_read_ctrl_flip_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 flip_en)
{
    volatile u_gfx_read_ctrl gfx_read_ctrl;
    gfx_read_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_read_ctrl.u32) + offset));
    gfx_read_ctrl.bits.flip_en = flip_en;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_read_ctrl.u32) + offset), gfx_read_ctrl.u32);

    return;
}

td_void dpu_reg_set_gfx_read_ctrl_draw_mode(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 draw_mode)
{
    volatile u_gfx_read_ctrl gfx_read_ctrl;
    gfx_read_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_read_ctrl.u32) + offset));
    gfx_read_ctrl.bits.draw_mode = draw_mode;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_read_ctrl.u32) + offset), gfx_read_ctrl.u32);

    return;
}

td_void dpu_reg_set_gfx_read_ctrl_read_mode(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 read_mode)
{
    volatile u_gfx_read_ctrl gfx_read_ctrl;
    gfx_read_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_read_ctrl.u32) + offset));
    gfx_read_ctrl.bits.read_mode = read_mode;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_read_ctrl.u32) + offset), gfx_read_ctrl.u32);

    return;
}

td_void dpu_reg_set_gfx_mac_ctrl_req_len(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 req_len)
{
    volatile u_gfx_mac_ctrl gfx_mac_ctrl;
    gfx_mac_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_mac_ctrl.u32) + offset));
    gfx_mac_ctrl.bits.req_len = req_len;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_mac_ctrl.u32) + offset), gfx_mac_ctrl.u32);

    return;
}

td_void dpu_reg_set_gfx_mac_ctrl_req_ctrl(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 req_ctrl)
{
    volatile u_gfx_mac_ctrl gfx_mac_ctrl;
    gfx_mac_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_mac_ctrl.u32) + offset));
    gfx_mac_ctrl.bits.req_ctrl = req_ctrl;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_mac_ctrl.u32) + offset), gfx_mac_ctrl.u32);

    return;
}

td_void dpu_reg_set_gfx_out_ctrl_testpattern_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 en)
{
    volatile u_gfx_out_ctrl gfx_out_ctrl;
    gfx_out_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_out_ctrl.u32) + offset));
    gfx_out_ctrl.bits.testpattern_en = en;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_out_ctrl.u32) + offset), gfx_out_ctrl.u32);

    return;
}

td_void dpu_reg_set_gfx_out_ctrl_premulti_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 premulti_en)
{
    volatile u_gfx_out_ctrl gfx_out_ctrl;
    gfx_out_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_out_ctrl.u32) + offset));
    gfx_out_ctrl.bits.premulti_en = premulti_en;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_out_ctrl.u32) + offset), gfx_out_ctrl.u32);

    return;
}

td_void dpu_reg_set_gfx_out_ctrl_bitext(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 bitext)
{
    volatile u_gfx_out_ctrl gfx_out_ctrl;
    gfx_out_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_out_ctrl.u32) + offset));
    gfx_out_ctrl.bits.bitext = bitext;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_out_ctrl.u32) + offset), gfx_out_ctrl.u32);

    return;
}

td_void dpu_reg_set_gfx_out_ctrl_key_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 key_en)
{
    volatile u_gfx_out_ctrl gfx_out_ctrl;
    gfx_out_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_out_ctrl.u32) + offset));
    gfx_out_ctrl.bits.key_en = key_en;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_out_ctrl.u32) + offset), gfx_out_ctrl.u32);

    return;
}

td_void dpu_reg_set_gfx_out_ctrl_key_mode(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 key_mode)
{
    volatile u_gfx_out_ctrl gfx_out_ctrl;
    gfx_out_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_out_ctrl.u32) + offset));
    gfx_out_ctrl.bits.key_mode = key_mode;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_out_ctrl.u32) + offset), gfx_out_ctrl.u32);

    return;
}

td_void dpu_reg_set_gfx_out_ctrl_palpha_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 palpha_en)
{
    volatile u_gfx_out_ctrl gfx_out_ctrl;
    gfx_out_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_out_ctrl.u32) + offset));
    gfx_out_ctrl.bits.palpha_en = palpha_en;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_out_ctrl.u32) + offset), gfx_out_ctrl.u32);

    return;
}

td_void dpu_reg_set_gfx_out_ctrl_palpha_range(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 palpha_range)
{
    volatile u_gfx_out_ctrl gfx_out_ctrl;
    gfx_out_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_out_ctrl.u32) + offset));
    gfx_out_ctrl.bits.palpha_range = palpha_range;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_out_ctrl.u32) + offset), gfx_out_ctrl.u32);

    return;
}

td_void dpu_reg_set_gfx_mute_alpha_mute_alpha(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 mute_alpha)
{
    volatile u_gfx_mute_alpha gfx_mute_alpha;
    gfx_mute_alpha.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_mute_alpha.u32) + offset));
    gfx_mute_alpha.bits.mute_alpha = mute_alpha;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_mute_alpha.u32) + offset), gfx_mute_alpha.u32);

    return;
}

td_void dpu_reg_set_gfx_mute_bk_mute_y(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 mute_y)
{
    volatile u_gfx_mute_bk gfx_mute_bk;
    gfx_mute_bk.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_mute_bk.u32) + offset));
    gfx_mute_bk.bits.mute_y = mute_y;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_mute_bk.u32) + offset), gfx_mute_bk.u32);

    return;
}

td_void dpu_reg_set_gfx_mute_bk_mute_cb(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 mute_cb)
{
    volatile u_gfx_mute_bk gfx_mute_bk;
    gfx_mute_bk.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_mute_bk.u32) + offset));
    gfx_mute_bk.bits.mute_cb = mute_cb;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_mute_bk.u32) + offset), gfx_mute_bk.u32);

    return;
}

td_void dpu_reg_set_gfx_mute_bk_mute_cr(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 mute_cr)
{
    volatile u_gfx_mute_bk gfx_mute_bk;
    gfx_mute_bk.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_mute_bk.u32) + offset));
    gfx_mute_bk.bits.mute_cr = mute_cr;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_mute_bk.u32) + offset), gfx_mute_bk.u32);

    return;
}

td_void dpu_reg_set_gfx_1555_alpha_alpha_1(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 alpha_1)
{
    volatile u_gfx_1555_alpha gfx_1555_alpha;
    gfx_1555_alpha.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_1555_alpha.u32) + offset));
    gfx_1555_alpha.bits.alpha_1 = alpha_1;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_1555_alpha.u32) + offset), gfx_1555_alpha.u32);

    return;
}

td_void dpu_reg_set_gfx_1555_alpha_alpha_0(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 alpha_0)
{
    volatile u_gfx_1555_alpha gfx_1555_alpha;
    gfx_1555_alpha.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_1555_alpha.u32) + offset));
    gfx_1555_alpha.bits.alpha_0 = alpha_0;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_1555_alpha.u32) + offset), gfx_1555_alpha.u32);

    return;
}

td_void dpu_reg_set_gfx_src_info_dcmp_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 dcmp_en)
{
    volatile u_gfx_src_info gfx_src_info;
    gfx_src_info.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_src_info.u32) + offset));
    gfx_src_info.bits.dcmp_en = dcmp_en;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_src_info.u32) + offset), gfx_src_info.u32);

    return;
}

td_void dpu_reg_set_gfx_src_info_disp_mode(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 disp_mode)
{
    volatile u_gfx_src_info gfx_src_info;
    gfx_src_info.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_src_info.u32) + offset));
    gfx_src_info.bits.disp_mode = disp_mode;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_src_info.u32) + offset), gfx_src_info.u32);

    return;
}

td_void dpu_reg_set_gfx_src_info_ifmt(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 ifmt)
{
    volatile u_gfx_src_info gfx_src_info;
    gfx_src_info.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_src_info.u32) + offset));
    gfx_src_info.bits.ifmt = ifmt;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_src_info.u32) + offset), gfx_src_info.u32);

    return;
}

td_void dpu_reg_set_gfx_src_reso_src_h(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 src_h)
{
    volatile u_gfx_src_reso gfx_src_reso;
    gfx_src_reso.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_src_reso.u32) + offset));
    gfx_src_reso.bits.src_h = src_h;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_src_reso.u32) + offset), gfx_src_reso.u32);

    return;
}

td_void dpu_reg_set_gfx_src_reso_src_w(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 src_w)
{
    volatile u_gfx_src_reso gfx_src_reso;
    gfx_src_reso.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_src_reso.u32) + offset));
    gfx_src_reso.bits.src_w = src_w;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_src_reso.u32) + offset), gfx_src_reso.u32);

    return;
}

td_void dpu_reg_set_gfx_src_crop_src_crop_y(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 src_crop_y)
{
    volatile u_gfx_src_crop gfx_src_crop;
    gfx_src_crop.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_src_crop.u32) + offset));
    gfx_src_crop.bits.src_crop_y = src_crop_y;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_src_crop.u32) + offset), gfx_src_crop.u32);

    return;
}

td_void dpu_reg_set_gfx_src_crop_src_crop_x(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 src_crop_x)
{
    volatile u_gfx_src_crop gfx_src_crop;
    gfx_src_crop.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_src_crop.u32) + offset));
    gfx_src_crop.bits.src_crop_x = src_crop_x;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_src_crop.u32) + offset), gfx_src_crop.u32);

    return;
}

td_void dpu_reg_set_gfx_ireso_ireso_h(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 ireso_h)
{
    volatile u_gfx_ireso gfx_ireso;
    gfx_ireso.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_ireso.u32) + offset));
    gfx_ireso.bits.ireso_h = ireso_h;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_ireso.u32) + offset), gfx_ireso.u32);

    return;
}

td_void dpu_reg_set_gfx_ireso_ireso_w(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 ireso_w)
{
    volatile u_gfx_ireso gfx_ireso;
    gfx_ireso.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_ireso.u32) + offset));
    gfx_ireso.bits.ireso_w = ireso_w;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_ireso.u32) + offset), gfx_ireso.u32);

    return;
}

td_void dpu_reg_set_gfx_addr_h_gfx_addr_h(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 addr_h)
{
    volatile u_gfx_addr_h gfx_addr_h;
    gfx_addr_h.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_addr_h.u32) + offset));
    gfx_addr_h.bits.gfx_addr_h = addr_h;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_addr_h.u32) + offset), gfx_addr_h.u32);

    return;
}

td_void dpu_reg_set_gfx_addr_l_gfx_addr_l(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 addr_l)
{
    volatile u_gfx_addr_l gfx_addr_l;
    gfx_addr_l.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_addr_l.u32) + offset));
    gfx_addr_l.bits.gfx_addr_l = addr_l;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_addr_l.u32) + offset), gfx_addr_l.u32);

    return;
}

td_void dpu_reg_set_gfx_stride_surface_stride(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 stride)
{
    volatile u_gfx_stride gfx_stride;
    gfx_stride.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_stride.u32) + offset));
    gfx_stride.bits.surface_stride = stride;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_stride.u32) + offset), gfx_stride.u32);

    return;
}

td_void dpu_reg_set_gfx_addr_h_c_gfx_addr_h_c(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 addr_h_c)
{
    volatile u_gfx_addr_h_c gfx_addr_h_c;
    gfx_addr_h_c.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_addr_h_c.u32) + offset));
    gfx_addr_h_c.bits.gfx_addr_h_c = addr_h_c;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_addr_h_c.u32) + offset), gfx_addr_h_c.u32);

    return;
}

td_void dpu_reg_set_gfx_addr_l_c_gfx_addr_l_c(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 addr_l_c)
{
    volatile u_gfx_addr_l_c gfx_addr_l_c;
    gfx_addr_l_c.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_addr_l_c.u32) + offset));
    gfx_addr_l_c.bits.gfx_addr_l_c = addr_l_c;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_addr_l_c.u32) + offset), gfx_addr_l_c.u32);

    return;
}

td_void dpu_reg_set_gfx_stride_c_surface_stride_c(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 stride_c)
{
    volatile u_gfx_stride_c gfx_stride_c;
    gfx_stride_c.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_stride_c.u32) + offset));
    gfx_stride_c.bits.surface_stride_c = stride_c;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_stride_c.u32) + offset), gfx_stride_c.u32);

    return;
}

td_void dpu_reg_set_gfx_dcmp_addr_h_dcmp_addr_h(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 dcmp_addr_h)
{
    volatile u_gfx_dcmp_addr_h gfx_dcmp_addr_h;
    gfx_dcmp_addr_h.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_dcmp_addr_h.u32) + offset));
    gfx_dcmp_addr_h.bits.dcmp_addr_h = dcmp_addr_h;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_dcmp_addr_h.u32) + offset), gfx_dcmp_addr_h.u32);

    return;
}

td_void dpu_reg_set_gfx_dcmp_addr_l_dcmp_addr_l(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 dcmp_addr_l)
{
    volatile u_gfx_dcmp_addr_l gfx_dcmp_addr_l;
    gfx_dcmp_addr_l.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_dcmp_addr_l.u32) + offset));
    gfx_dcmp_addr_l.bits.dcmp_addr_l = dcmp_addr_l;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_dcmp_addr_l.u32) + offset), gfx_dcmp_addr_l.u32);

    return;
}

td_void dpu_reg_set_gfx_testpat_cfg_tp_mode(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 tp_mode)
{
    volatile u_gfx_testpat_cfg gfx_testpat_cfg;
    gfx_testpat_cfg.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_testpat_cfg.u32) + offset));
    gfx_testpat_cfg.bits.tp_mode = tp_mode;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_testpat_cfg.u32) + offset), gfx_testpat_cfg.u32);

    return;
}

td_void dpu_reg_set_gfx_testpat_cfg_tp_color_mode(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 mode)
{
    volatile u_gfx_testpat_cfg gfx_testpat_cfg;
    gfx_testpat_cfg.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_testpat_cfg.u32) + offset));
    gfx_testpat_cfg.bits.tp_color_mode = mode;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_testpat_cfg.u32) + offset), gfx_testpat_cfg.u32);

    return;
}

td_void dpu_reg_set_gfx_testpat_cfg_tp_line_w(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 tp_line_w)
{
    volatile u_gfx_testpat_cfg gfx_testpat_cfg;
    gfx_testpat_cfg.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_testpat_cfg.u32) + offset));
    gfx_testpat_cfg.bits.tp_line_w = tp_line_w;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_testpat_cfg.u32) + offset), gfx_testpat_cfg.u32);

    return;
}

td_void dpu_reg_set_gfx_testpat_cfg_tp_speed(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 tp_speed)
{
    volatile u_gfx_testpat_cfg gfx_testpat_cfg;
    gfx_testpat_cfg.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_testpat_cfg.u32) + offset));
    gfx_testpat_cfg.bits.tp_speed = tp_speed;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_testpat_cfg.u32) + offset), gfx_testpat_cfg.u32);

    return;
}

td_void dpu_reg_set_gfx_testpat_seed_tp_seed(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 tp_seed)
{
    volatile u_gfx_testpat_seed gfx_testpat_seed;
    gfx_testpat_seed.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_testpat_seed.u32) + offset));
    gfx_testpat_seed.bits.tp_seed = tp_seed;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_testpat_seed.u32) + offset), gfx_testpat_seed.u32);

    return;
}

td_void dpu_reg_set_gfx_dcmp_ctrl_alpha_bypass(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 alpha_bypass)
{
    volatile u_gfx_dcmp_ctrl gfx_dcmp_ctrl;
    gfx_dcmp_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_dcmp_ctrl.u32) + offset));
    gfx_dcmp_ctrl.bits.alpha_bypass = alpha_bypass;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_dcmp_ctrl.u32) + offset), gfx_dcmp_ctrl.u32);

    return;
}

td_void dpu_reg_set_gfx_dcmp_ctrl_dcmp_fmt(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 dcmp_fmt)
{
    volatile u_gfx_dcmp_ctrl gfx_dcmp_ctrl;
    gfx_dcmp_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_dcmp_ctrl.u32) + offset));
    gfx_dcmp_ctrl.bits.dcmp_fmt = dcmp_fmt;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_dcmp_ctrl.u32) + offset), gfx_dcmp_ctrl.u32);

    return;
}

td_void dpu_reg_set_gfx_dcmp_ctrl_cmp_mode(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 cmp_mode)
{
    volatile u_gfx_dcmp_ctrl gfx_dcmp_ctrl;
    gfx_dcmp_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_dcmp_ctrl.u32) + offset));
    gfx_dcmp_ctrl.bits.cmp_mode = cmp_mode;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_dcmp_ctrl.u32) + offset), gfx_dcmp_ctrl.u32);

    return;
}

td_void dpu_reg_set_gfx_dcmp_ctrl_ice_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 ice_en)
{
    volatile u_gfx_dcmp_ctrl gfx_dcmp_ctrl;
    gfx_dcmp_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_dcmp_ctrl.u32) + offset));
    gfx_dcmp_ctrl.bits.ice_en = ice_en;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_dcmp_ctrl.u32) + offset), gfx_dcmp_ctrl.u32);

    return;
}

td_void dpu_reg_set_gfx_debug_ctrl_checksum_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 checksum_en)
{
    volatile u_gfx_debug_ctrl gfx_debug_ctrl;
    gfx_debug_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_debug_ctrl.u32) + offset));
    gfx_debug_ctrl.bits.checksum_en = checksum_en;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_debug_ctrl.u32) + offset), gfx_debug_ctrl.u32);

    return;
}

td_u32 dpu_reg_get_gfx_in_ar_checksum0_ar_checksum0(const volatile dpu_regs_type *dpu_reg, td_u32 offset)
{
    volatile u_gfx_in_ar_checksum0 gfx_in_ar_checksum0;
    gfx_in_ar_checksum0.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_in_ar_checksum0.u32) + offset));
    return gfx_in_ar_checksum0.bits.ar_checksum0;
}

td_u32 dpu_reg_get_gfx_in_ar_checksum1_ar_checksum1(const volatile dpu_regs_type *dpu_reg, td_u32 offset)
{
    volatile u_gfx_in_ar_checksum1 gfx_in_ar_checksum1;
    gfx_in_ar_checksum1.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_in_ar_checksum1.u32) + offset));
    return gfx_in_ar_checksum1.bits.ar_checksum1;
}

td_u32 dpu_reg_get_gfx_in_gb_checksum0_gb_checksum0(const volatile dpu_regs_type *dpu_reg, td_u32 offset)
{
    volatile u_gfx_in_gb_checksum0 gfx_in_gb_checksum0;
    gfx_in_gb_checksum0.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_in_gb_checksum0.u32) + offset));
    return gfx_in_gb_checksum0.bits.gb_checksum0;
}

td_u32 dpu_reg_get_gfx_in_gb_checksum1_gb_checksum1(const volatile dpu_regs_type *dpu_reg, td_u32 offset)
{
    volatile u_gfx_in_gb_checksum1 gfx_in_gb_checksum1;
    gfx_in_gb_checksum1.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_in_gb_checksum1.u32) + offset));
    return gfx_in_gb_checksum1.bits.gb_checksum1;
}

td_void dpu_reg_set_gfx_ckey_max_key_r_max(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 key_r_max)
{
    volatile u_gfx_ckey_max gfx_ckey_max;
    gfx_ckey_max.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_ckey_max.u32) + offset));
    gfx_ckey_max.bits.key_r_max = key_r_max;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_ckey_max.u32) + offset), gfx_ckey_max.u32);

    return;
}

td_void dpu_reg_set_gfx_ckey_max_key_g_max(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 key_g_max)
{
    volatile u_gfx_ckey_max gfx_ckey_max;
    gfx_ckey_max.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_ckey_max.u32) + offset));
    gfx_ckey_max.bits.key_g_max = key_g_max;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_ckey_max.u32) + offset), gfx_ckey_max.u32);

    return;
}

td_void dpu_reg_set_gfx_ckey_max_key_b_max(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 key_b_max)
{
    volatile u_gfx_ckey_max gfx_ckey_max;
    gfx_ckey_max.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_ckey_max.u32) + offset));
    gfx_ckey_max.bits.key_b_max = key_b_max;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_ckey_max.u32) + offset), gfx_ckey_max.u32);

    return;
}

td_void dpu_reg_set_gfx_ckey_min_key_r_min(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 key_r_min)
{
    volatile u_gfx_ckey_min gfx_ckey_min;
    gfx_ckey_min.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_ckey_min.u32) + offset));
    gfx_ckey_min.bits.key_r_min = key_r_min;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_ckey_min.u32) + offset), gfx_ckey_min.u32);

    return;
}

td_void dpu_reg_set_gfx_ckey_min_key_g_min(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 key_g_min)
{
    volatile u_gfx_ckey_min gfx_ckey_min;
    gfx_ckey_min.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_ckey_min.u32) + offset));
    gfx_ckey_min.bits.key_g_min = key_g_min;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_ckey_min.u32) + offset), gfx_ckey_min.u32);

    return;
}

td_void dpu_reg_set_gfx_ckey_min_key_b_min(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 key_b_min)
{
    volatile u_gfx_ckey_min gfx_ckey_min;
    gfx_ckey_min.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_ckey_min.u32) + offset));
    gfx_ckey_min.bits.key_b_min = key_b_min;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_ckey_min.u32) + offset), gfx_ckey_min.u32);

    return;
}

td_void dpu_reg_set_gfx_ckey_mask_key_r_msk(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 key_r_msk)
{
    volatile u_gfx_ckey_mask gfx_ckey_mask;
    gfx_ckey_mask.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_ckey_mask.u32) + offset));
    gfx_ckey_mask.bits.key_r_msk = key_r_msk;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_ckey_mask.u32) + offset), gfx_ckey_mask.u32);

    return;
}

td_void dpu_reg_set_gfx_ckey_mask_key_g_msk(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 key_g_msk)
{
    volatile u_gfx_ckey_mask gfx_ckey_mask;
    gfx_ckey_mask.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_ckey_mask.u32) + offset));
    gfx_ckey_mask.bits.key_g_msk = key_g_msk;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_ckey_mask.u32) + offset), gfx_ckey_mask.u32);

    return;
}

td_void dpu_reg_set_gfx_ckey_mask_key_b_msk(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 key_b_msk)
{
    volatile u_gfx_ckey_mask gfx_ckey_mask;
    gfx_ckey_mask.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_ckey_mask.u32) + offset));
    gfx_ckey_mask.bits.key_b_msk = key_b_msk;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_ckey_mask.u32) + offset), gfx_ckey_mask.u32);

    return;
}

td_void dpu_reg_set_gfx_htc_dcmp_is_a_offline(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 is_a_offline)
{
    volatile u_gfx_v5r1_htc_dcmp_glb_info gfx_v5r1_htc_dcmp_glb_info;
    gfx_v5r1_htc_dcmp_glb_info.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_v5r1_htc_dcmp_glb_info.u32) + offset));
    gfx_v5r1_htc_dcmp_glb_info.bits.is_a_offline = is_a_offline;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_v5r1_htc_dcmp_glb_info.u32) + offset), gfx_v5r1_htc_dcmp_glb_info.u32);

    return;
}

td_void dpu_reg_set_gfx_htc_dcmp_is_a_bypass(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 is_a_bypass)
{
    volatile u_gfx_v5r1_htc_dcmp_glb_info gfx_v5r1_htc_dcmp_glb_info;
    gfx_v5r1_htc_dcmp_glb_info.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_v5r1_htc_dcmp_glb_info.u32) + offset));
    gfx_v5r1_htc_dcmp_glb_info.bits.is_a_bypass = is_a_bypass;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_v5r1_htc_dcmp_glb_info.u32) + offset), gfx_v5r1_htc_dcmp_glb_info.u32);

    return;
}

td_void dpu_reg_set_gfx_htc_dcmp_pix_format(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 pix_format)
{
    volatile u_gfx_v5r1_htc_dcmp_glb_info gfx_v5r1_htc_dcmp_glb_info;
    gfx_v5r1_htc_dcmp_glb_info.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_v5r1_htc_dcmp_glb_info.u32) + offset));
    gfx_v5r1_htc_dcmp_glb_info.bits.pix_format = pix_format;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_v5r1_htc_dcmp_glb_info.u32) + offset), gfx_v5r1_htc_dcmp_glb_info.u32);

    return;
}

td_void dpu_reg_set_gfx_htc_dcmp_cmp_mode(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 cmp_mode)
{
    volatile u_gfx_v5r1_htc_dcmp_glb_info gfx_v5r1_htc_dcmp_glb_info;
    gfx_v5r1_htc_dcmp_glb_info.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_v5r1_htc_dcmp_glb_info.u32) + offset));
    gfx_v5r1_htc_dcmp_glb_info.bits.cmp_mode = cmp_mode;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_v5r1_htc_dcmp_glb_info.u32) + offset), gfx_v5r1_htc_dcmp_glb_info.u32);

    return;
}

td_void dpu_reg_set_gfx_htc_dcmp_ice_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 ice_en)
{
    volatile u_gfx_v5r1_htc_dcmp_glb_info gfx_v5r1_htc_dcmp_glb_info;
    gfx_v5r1_htc_dcmp_glb_info.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_v5r1_htc_dcmp_glb_info.u32) + offset));
    gfx_v5r1_htc_dcmp_glb_info.bits.ice_en = ice_en;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_v5r1_htc_dcmp_glb_info.u32) + offset), gfx_v5r1_htc_dcmp_glb_info.u32);

    return;
}

td_void dpu_reg_set_gfx_htc_dcmp_dbg_reg_dcmp_debug(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 dcmp_debug)
{
    volatile u_gfx_v5r1_htc_dcmp_dbg_reg gfx_v5r1_htc_dcmp_dbg_reg;
    gfx_v5r1_htc_dcmp_dbg_reg.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->gfx_v5r1_htc_dcmp_dbg_reg.u32) + offset));
    gfx_v5r1_htc_dcmp_dbg_reg.bits.dcmp_debug = dcmp_debug;
    dpu_reg_write((uintptr_t)(&(dpu_reg->gfx_v5r1_htc_dcmp_dbg_reg.u32) + offset), gfx_v5r1_htc_dcmp_dbg_reg.u32);

    return;
}

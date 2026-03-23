/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: dpu hal disp
 */
#include "dpu_hal_disp.h"

td_void dpu_reg_set_voctrl_vo_ck_gt_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 vo_ck_gt_en)
{
    volatile u_voctrl voctrl;
    voctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->voctrl.u32) + offset));
    voctrl.bits.vo_ck_gt_en = vo_ck_gt_en;
    dpu_reg_write((uintptr_t)(&(dpu_reg->voctrl.u32) + offset), voctrl.u32);

    return;
}

td_void dpu_reg_set_voctrl_intf_chk_sum_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 intf_chk_sum_en)
{
    volatile u_voctrl voctrl;
    voctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->voctrl.u32) + offset));
    voctrl.bits.intf_chk_sum_en = intf_chk_sum_en;
    dpu_reg_write((uintptr_t)(&(dpu_reg->voctrl.u32) + offset), voctrl.u32);

    return;
}

td_void dpu_reg_set_voctrl_g0_ck_gt_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 g0_ck_gt_en)
{
    volatile u_voctrl voctrl;
    voctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->voctrl.u32) + offset));
    voctrl.bits.g0_ck_gt_en = g0_ck_gt_en;
    dpu_reg_write((uintptr_t)(&(dpu_reg->voctrl.u32) + offset), voctrl.u32);

    return;
}

td_void dpu_reg_set_voctrl_g1_ck_gt_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 g1_ck_gt_en)
{
    volatile u_voctrl voctrl;
    voctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->voctrl.u32) + offset));
    voctrl.bits.g1_ck_gt_en = g1_ck_gt_en;
    dpu_reg_write((uintptr_t)(&(dpu_reg->voctrl.u32) + offset), voctrl.u32);

    return;
}

td_u32 dpu_reg_get_vointsta(const volatile dpu_regs_type *dpu_reg)
{
    volatile u_vointsta vointsta;
    vointsta.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->vointsta.u32)));
    return vointsta.u32;
}

td_void dpu_reg_set_vomskintsta(volatile dpu_regs_type *dpu_reg, td_u32 intmask)
{
    dpu_reg_write((uintptr_t)(&(dpu_reg->vomskintsta.u32)), intmask);
    return;
}

td_u32 dpu_reg_get_vomskintsta(const volatile dpu_regs_type *dpu_reg)
{
    volatile u_vomskintsta vomskintsta;
    vomskintsta.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->vomskintsta.u32)));
    return vomskintsta.u32;
}

td_void dpu_reg_set_vointmsk_enable(volatile dpu_regs_type *dpu_reg, td_u32 intmask)
{
    volatile u_vointmsk vointmsk;
    vointmsk.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->vointmsk.u32)));
    vointmsk.u32 = vointmsk.u32 | intmask;
    dpu_reg_write((uintptr_t)(&(dpu_reg->vointmsk.u32)), vointmsk.u32);
    return;
}

td_void dpu_reg_set_vointmsk_disable(volatile dpu_regs_type *dpu_reg, td_u32 intmask)
{
    volatile u_vointmsk vointmsk;
    vointmsk.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->vointmsk.u32)));
    vointmsk.u32 = vointmsk.u32 & (~intmask);
    dpu_reg_write((uintptr_t)(&(dpu_reg->vointmsk.u32)), vointmsk.u32);
    return;
}

td_u32 dpu_reg_get_vocmd_intsta(const volatile dpu_regs_type *dpu_reg)
{
    volatile u_vocmd_intsta vocmd_intsta;
    vocmd_intsta.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->vocmd_intsta.u32)));
    return vocmd_intsta.u32;
}

td_void dpu_reg_set_vocmd_mskintsta(volatile dpu_regs_type *dpu_reg, td_u32 intmask)
{
    dpu_reg_write((uintptr_t)(&(dpu_reg->vocmd_mskintsta.u32)), intmask);
    return;
}

td_u32 dpu_reg_get_vocmd_mskintsta(const volatile dpu_regs_type *dpu_reg)
{
    volatile u_vocmd_mskintsta vocmd_mskintsta;
    vocmd_mskintsta.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->vocmd_mskintsta.u32)));
    return vocmd_mskintsta.u32;
}

td_void dpu_reg_set_vocmd_intmsk_enable(volatile dpu_regs_type *dpu_reg, td_u32 intmask)
{
    volatile u_vocmd_intmsk vocmd_intmsk;
    vocmd_intmsk.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->vocmd_intmsk.u32)));
    vocmd_intmsk.u32 = vocmd_intmsk.u32 | intmask;
    dpu_reg_write((uintptr_t)(&(dpu_reg->vocmd_intmsk.u32)), vocmd_intmsk.u32);
    return;
}

td_void dpu_reg_set_vocmd_intmsk_disable(volatile dpu_regs_type *dpu_reg, td_u32 intmask)
{
    volatile u_vocmd_intmsk vocmd_intmsk;
    vocmd_intmsk.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->vocmd_intmsk.u32)));
    vocmd_intmsk.u32 = vocmd_intmsk.u32 & (~intmask);
    dpu_reg_write((uintptr_t)(&(dpu_reg->vocmd_intmsk.u32)), vocmd_intmsk.u32);
    return;
}

td_u32 dpu_reg_get_voufsta_g1_uf_sta(const volatile dpu_regs_type *dpu_reg, td_u32 offset)
{
    volatile u_voufsta voufsta;
    voufsta.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->voufsta.u32) + offset));

    return voufsta.bits.g1_uf_sta;
}

td_u32 dpu_reg_get_voufsta_g0_uf_sta(const volatile dpu_regs_type *dpu_reg, td_u32 offset)
{
    volatile u_voufsta voufsta;
    voufsta.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->voufsta.u32) + offset));

    return voufsta.bits.g0_uf_sta;
}

td_void dpu_reg_set_voufclr_g1_uf_clr(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 g1_uf_clr)
{
    volatile u_voufclr voufclr;
    voufclr.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->voufclr.u32) + offset));
    voufclr.bits.g1_uf_clr = g1_uf_clr;
    dpu_reg_write((uintptr_t)(&(dpu_reg->voufclr.u32) + offset), voufclr.u32);

    return;
}

td_void dpu_reg_set_voufclr_g0_uf_clr(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 g0_uf_clr)
{
    volatile u_voufclr voufclr;
    voufclr.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->voufclr.u32) + offset));
    voufclr.bits.g0_uf_clr = g0_uf_clr;
    dpu_reg_write((uintptr_t)(&(dpu_reg->voufclr.u32) + offset), voufclr.u32);

    return;
}

td_void dpu_reg_set_cbm_bkg_cbm_bkgy(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 cbm_bkgy)
{
    volatile u_cbm_bkg cbm_bkg;
    cbm_bkg.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->cbm_bkg.u32) + offset));
    cbm_bkg.bits.cbm_bkgy = cbm_bkgy;
    dpu_reg_write((uintptr_t)(&(dpu_reg->cbm_bkg.u32) + offset), cbm_bkg.u32);

    return;
}

td_void dpu_reg_set_cbm_bkg_cbm_bkgcb(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 cbm_bkgcb)
{
    volatile u_cbm_bkg cbm_bkg;
    cbm_bkg.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->cbm_bkg.u32) + offset));
    cbm_bkg.bits.cbm_bkgcb = cbm_bkgcb;
    dpu_reg_write((uintptr_t)(&(dpu_reg->cbm_bkg.u32) + offset), cbm_bkg.u32);

    return;
}

td_void dpu_reg_set_cbm_bkg_cbm_bkgcr(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 cbm_bkgcr)
{
    volatile u_cbm_bkg cbm_bkg;
    cbm_bkg.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->cbm_bkg.u32) + offset));
    cbm_bkg.bits.cbm_bkgcr = cbm_bkgcr;
    dpu_reg_write((uintptr_t)(&(dpu_reg->cbm_bkg.u32) + offset), cbm_bkg.u32);

    return;
}

td_void dpu_reg_set_cbm_mix_mixer_prio3(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 mixer_prio3)
{
    volatile u_cbm_mix cbm_mix;
    cbm_mix.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->cbm_mix.u32) + offset));
    cbm_mix.bits.mixer_prio3 = mixer_prio3;
    dpu_reg_write((uintptr_t)(&(dpu_reg->cbm_mix.u32) + offset), cbm_mix.u32);

    return;
}

td_void dpu_reg_set_cbm_mix_mixer_prio2(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 mixer_prio2)
{
    volatile u_cbm_mix cbm_mix;
    cbm_mix.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->cbm_mix.u32) + offset));
    cbm_mix.bits.mixer_prio2 = mixer_prio2;
    dpu_reg_write((uintptr_t)(&(dpu_reg->cbm_mix.u32) + offset), cbm_mix.u32);

    return;
}

td_void dpu_reg_set_cbm_mix_mixer_prio1(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 mixer_prio1)
{
    volatile u_cbm_mix cbm_mix;
    cbm_mix.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->cbm_mix.u32) + offset));
    cbm_mix.bits.mixer_prio1 = mixer_prio1;
    dpu_reg_write((uintptr_t)(&(dpu_reg->cbm_mix.u32) + offset), cbm_mix.u32);

    return;
}

td_void dpu_reg_set_cbm_mix_mixer_prio0(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 mixer_prio0)
{
    volatile u_cbm_mix cbm_mix;
    cbm_mix.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->cbm_mix.u32) + offset));
    cbm_mix.bits.mixer_prio0 = mixer_prio0;
    dpu_reg_write((uintptr_t)(&(dpu_reg->cbm_mix.u32) + offset), cbm_mix.u32);

    return;
}

td_u32 dpu_reg_get_cbm_mix_priority(const volatile dpu_regs_type *dpu_reg, td_u32 offset)
{
    volatile u_cbm_mix cbm_mix;
    cbm_mix.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->cbm_mix.u32) + offset));
    return cbm_mix.u32;
}

td_void dpu_reg_set_cbm_ctrl_bypass_mode(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 bypass_mode)
{
    volatile u_cbm_ctrl cbm_ctrl;
    cbm_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->cbm_ctrl.u32) + offset));
    cbm_ctrl.bits.bypass_mode = bypass_mode;
    dpu_reg_write((uintptr_t)(&(dpu_reg->cbm_ctrl.u32) + offset), cbm_ctrl.u32);

    return;
}

td_void dpu_reg_set_cbm_ctrl_layer0_bypass_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 bypass_en)
{
    volatile u_cbm_ctrl cbm_ctrl;
    cbm_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->cbm_ctrl.u32) + offset));
    cbm_ctrl.bits.layer0_bypass_en = bypass_en;
    dpu_reg_write((uintptr_t)(&(dpu_reg->cbm_ctrl.u32) + offset), cbm_ctrl.u32);

    return;
}

td_void dpu_reg_set_partial_cfg_vld(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_bool is_valid)
{
    volatile u_dpu_partial_ctrl dpu_partial_ctrl;
    dpu_partial_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dpu_partial_ctrl.u32) + offset));
    dpu_partial_ctrl.bits.dpu_partial_cfg_vld = is_valid;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dpu_partial_ctrl.u32) + offset), dpu_partial_ctrl.u32);

    return;
}

td_void dpu_reg_set_partial_hdr0(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 partial_hdr0)
{
    volatile u_dpu_partial_gen_hdr0 dpu_partial_gen_hdr0;
    dpu_partial_gen_hdr0.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dpu_partial_gen_hdr0.u32) + offset));
    dpu_partial_gen_hdr0.u32 = partial_hdr0;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dpu_partial_gen_hdr0.u32) + offset), dpu_partial_gen_hdr0.u32);

    return;
}

td_void dpu_reg_set_partial_pld_data00(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 pld_data00)
{
    volatile u_dpu_partial_gen_pld_data00 dpu_partial_gen_pld_data00;
    dpu_partial_gen_pld_data00.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dpu_partial_gen_pld_data00.u32) + offset));
    dpu_partial_gen_pld_data00.u32 = pld_data00;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dpu_partial_gen_pld_data00.u32) + offset), dpu_partial_gen_pld_data00.u32);

    return;
}

td_void dpu_reg_set_partial_pld_data01(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 pld_data01)
{
    volatile u_dpu_partial_gen_pld_data01 dpu_partial_gen_pld_data01;
    dpu_partial_gen_pld_data01.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dpu_partial_gen_pld_data01.u32) + offset));
    dpu_partial_gen_pld_data01.u32 = pld_data01;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dpu_partial_gen_pld_data01.u32) + offset), dpu_partial_gen_pld_data01.u32);

    return;
}

td_void dpu_reg_set_partial_hdr1(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 partial_hdr1)
{
    volatile u_dpu_partial_gen_hdr1 dpu_partial_gen_hdr1;
    dpu_partial_gen_hdr1.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dpu_partial_gen_hdr1.u32) + offset));
    dpu_partial_gen_hdr1.u32 = partial_hdr1;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dpu_partial_gen_hdr1.u32) + offset), dpu_partial_gen_hdr1.u32);

    return;
}

td_void dpu_reg_set_partial_pld_data10(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 pld_data10)
{
    volatile u_dpu_partial_gen_pld_data10 dpu_partial_gen_pld_data10;
    dpu_partial_gen_pld_data10.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dpu_partial_gen_pld_data10.u32) + offset));
    dpu_partial_gen_pld_data10.u32 = pld_data10;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dpu_partial_gen_pld_data10.u32) + offset), dpu_partial_gen_pld_data10.u32);

    return;
}

td_void dpu_reg_set_partial_pld_data11(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 pld_data11)
{
    volatile u_dpu_partial_gen_pld_data11 dpu_partial_gen_pld_data11;
    dpu_partial_gen_pld_data11.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dpu_partial_gen_pld_data11.u32) + offset));
    dpu_partial_gen_pld_data11.u32 = pld_data11;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dpu_partial_gen_pld_data11.u32) + offset), dpu_partial_gen_pld_data11.u32);

    return;
}

td_void dpu_reg_set_link_ctrl_g0_bypass_dhd0_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 bypass_en)
{
    volatile u_link_ctrl link_ctrl;
    link_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->link_ctrl.u32) + offset));
    link_ctrl.bits.g0_bypass_dhd0_en = bypass_en;
    dpu_reg_write((uintptr_t)(&(dpu_reg->link_ctrl.u32) + offset), link_ctrl.u32);

    return;
}

td_void dpu_reg_set_link_ctrl_format_link(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 format_link)
{
    volatile u_link_ctrl link_ctrl;
    link_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->link_ctrl.u32) + offset));
    link_ctrl.bits.format_link = format_link;
    dpu_reg_write((uintptr_t)(&(dpu_reg->link_ctrl.u32) + offset), link_ctrl.u32);

    return;
}

td_void dpu_reg_set_link_ctrl_mem_link(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 mem_link)
{
    volatile u_link_ctrl link_ctrl;
    link_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->link_ctrl.u32) + offset));
    link_ctrl.bits.mem_link = mem_link;
    dpu_reg_write((uintptr_t)(&(dpu_reg->link_ctrl.u32) + offset), link_ctrl.u32);

    return;
}

td_void dpu_reg_set_dhd0_ctrl_intf_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 intf_en)
{
    volatile u_dhd0_ctrl dhd0_ctrl;
    dhd0_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_ctrl.u32) + offset));
    dhd0_ctrl.bits.intf_en = intf_en;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_ctrl.u32) + offset), dhd0_ctrl.u32);

    return;
}

td_void dpu_reg_set_dhd0_ctrl_cbar_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 cbar_en)
{
    volatile u_dhd0_ctrl dhd0_ctrl;
    dhd0_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_ctrl.u32) + offset));
    dhd0_ctrl.bits.cbar_en = cbar_en;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_ctrl.u32) + offset), dhd0_ctrl.u32);

    return;
}

td_void dpu_reg_set_dhd0_ctrl_cbar_sel(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 cbar_sel)
{
    volatile u_dhd0_ctrl dhd0_ctrl;
    dhd0_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_ctrl.u32) + offset));
    dhd0_ctrl.bits.cbar_sel = cbar_sel;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_ctrl.u32) + offset), dhd0_ctrl.u32);

    return;
}

td_void dpu_reg_set_dhd0_ctrl_cbar_mode(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 cbar_mode)
{
    volatile u_dhd0_ctrl dhd0_ctrl;
    dhd0_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_ctrl.u32) + offset));
    dhd0_ctrl.bits.cbar_mode = cbar_mode;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_ctrl.u32) + offset), dhd0_ctrl.u32);

    return;
}

td_void dpu_reg_set_dhd0_ctrl_uf_offline_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 uf_offline_en)
{
    volatile u_dhd0_ctrl dhd0_ctrl;
    dhd0_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_ctrl.u32) + offset));
    dhd0_ctrl.bits.uf_offline_en = uf_offline_en;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_ctrl.u32) + offset), dhd0_ctrl.u32);

    return;
}

td_void dpu_reg_set_dhd0_ctrl_intf_idv(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 intf_idv)
{
    volatile u_dhd0_ctrl dhd0_ctrl;
    dhd0_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_ctrl.u32) + offset));
    dhd0_ctrl.bits.intf_idv = intf_idv;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_ctrl.u32) + offset), dhd0_ctrl.u32);

    return;
}

td_void dpu_reg_set_dhd0_ctrl_intf_ihs(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 intf_ihs)
{
    volatile u_dhd0_ctrl dhd0_ctrl;
    dhd0_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_ctrl.u32) + offset));
    dhd0_ctrl.bits.intf_ihs = intf_ihs;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_ctrl.u32) + offset), dhd0_ctrl.u32);

    return;
}

td_void dpu_reg_set_dhd0_ctrl_intf_ivs(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 intf_ivs)
{
    volatile u_dhd0_ctrl dhd0_ctrl;
    dhd0_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_ctrl.u32) + offset));
    dhd0_ctrl.bits.intf_ivs = intf_ivs;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_ctrl.u32) + offset), dhd0_ctrl.u32);

    return;
}

td_void dpu_reg_set_dhd0_ctrl_iop(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 iop)
{
    volatile u_dhd0_ctrl dhd0_ctrl;
    dhd0_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_ctrl.u32) + offset));
    dhd0_ctrl.bits.iop = iop;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_ctrl.u32) + offset), dhd0_ctrl.u32);

    return;
}

td_void dpu_reg_set_dhd0_ctrl_disp_mode(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 disp_mode)
{
    volatile u_dhd0_ctrl dhd0_ctrl;
    dhd0_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_ctrl.u32) + offset));
    dhd0_ctrl.bits.disp_mode = disp_mode;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_ctrl.u32) + offset), dhd0_ctrl.u32);

    return;
}

td_void dpu_reg_set_dhd0_ctrl_regup(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 regup)
{
    volatile u_dhd0_ctrl dhd0_ctrl;
    dhd0_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_ctrl.u32) + offset));
    dhd0_ctrl.bits.regup = regup;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_ctrl.u32) + offset), dhd0_ctrl.u32);

    return;
}

td_void dpu_reg_set_dhd0_vsync1_vbb(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 vbb)
{
    volatile u_dhd0_vsync1 dhd0_vsync1;
    dhd0_vsync1.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_vsync1.u32) + offset));
    dhd0_vsync1.bits.vbb = vbb;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_vsync1.u32) + offset), dhd0_vsync1.u32);

    return;
}

td_void dpu_reg_set_dhd0_vsync1_vact(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 vact)
{
    volatile u_dhd0_vsync1 dhd0_vsync1;
    dhd0_vsync1.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_vsync1.u32) + offset));
    dhd0_vsync1.bits.vact = vact;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_vsync1.u32) + offset), dhd0_vsync1.u32);

    return;
}

td_void dpu_reg_set_dhd0_vsync2_vfb(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 vfb)
{
    volatile u_dhd0_vsync2 dhd0_vsync2;
    dhd0_vsync2.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_vsync2.u32) + offset));
    dhd0_vsync2.bits.vfb = vfb;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_vsync2.u32) + offset), dhd0_vsync2.u32);

    return;
}

td_void dpu_reg_set_dhd0_hsync1_hbb(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 hbb)
{
    volatile u_dhd0_hsync1 dhd0_hsync1;
    dhd0_hsync1.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_hsync1.u32) + offset));
    dhd0_hsync1.bits.hbb = hbb;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_hsync1.u32) + offset), dhd0_hsync1.u32);

    return;
}

td_void dpu_reg_set_dhd0_hsync1_hact(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 hact)
{
    volatile u_dhd0_hsync1 dhd0_hsync1;
    dhd0_hsync1.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_hsync1.u32) + offset));
    dhd0_hsync1.bits.hact = hact;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_hsync1.u32) + offset), dhd0_hsync1.u32);

    return;
}

td_void dpu_reg_set_dhd0_hsync2_hmid(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 hmid)
{
    volatile u_dhd0_hsync2 dhd0_hsync2;
    dhd0_hsync2.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_hsync2.u32) + offset));
    dhd0_hsync2.bits.hmid = hmid;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_hsync2.u32) + offset), dhd0_hsync2.u32);

    return;
}

td_void dpu_reg_set_dhd0_hsync2_hfb(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 hfb)
{
    volatile u_dhd0_hsync2 dhd0_hsync2;
    dhd0_hsync2.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_hsync2.u32) + offset));
    dhd0_hsync2.bits.hfb = hfb;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_hsync2.u32) + offset), dhd0_hsync2.u32);

    return;
}

td_void dpu_reg_set_dhd0_vplus1_bvbb(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 bvbb)
{
    volatile u_dhd0_vplus1 dhd0_vplus1;
    dhd0_vplus1.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_vplus1.u32) + offset));
    dhd0_vplus1.bits.bvbb = bvbb;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_vplus1.u32) + offset), dhd0_vplus1.u32);

    return;
}

td_void dpu_reg_set_dhd0_vplus1_bvact(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 bvact)
{
    volatile u_dhd0_vplus1 dhd0_vplus1;
    dhd0_vplus1.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_vplus1.u32) + offset));
    dhd0_vplus1.bits.bvact = bvact;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_vplus1.u32) + offset), dhd0_vplus1.u32);

    return;
}

td_void dpu_reg_set_dhd0_vplus2_bvfb(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 bvfb)
{
    volatile u_dhd0_vplus2 dhd0_vplus2;
    dhd0_vplus2.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_vplus2.u32) + offset));
    dhd0_vplus2.bits.bvfb = bvfb;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_vplus2.u32) + offset), dhd0_vplus2.u32);

    return;
}

td_void dpu_reg_set_dhd0_pwr_multichn_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 multichn_en)
{
    volatile u_dhd0_pwr dhd0_pwr;
    dhd0_pwr.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_pwr.u32) + offset));
    dhd0_pwr.bits.multichn_en = multichn_en;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_pwr.u32) + offset), dhd0_pwr.u32);

    return;
}

td_void dpu_reg_set_dhd0_pwr_vpw(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 vpw)
{
    volatile u_dhd0_pwr dhd0_pwr;
    dhd0_pwr.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_pwr.u32) + offset));
    dhd0_pwr.bits.vpw = vpw;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_pwr.u32) + offset), dhd0_pwr.u32);

    return;
}

td_void dpu_reg_set_dhd0_pwr_hpw(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 hpw)
{
    volatile u_dhd0_pwr dhd0_pwr;
    dhd0_pwr.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_pwr.u32) + offset));
    dhd0_pwr.bits.hpw = hpw;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_pwr.u32) + offset), dhd0_pwr.u32);

    return;
}

td_void dpu_reg_set_dhd0_vtthd3_thd4_mode(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 thd4_mode)
{
    volatile u_dhd0_vtthd3 dhd0_vtthd3;
    dhd0_vtthd3.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_vtthd3.u32) + offset));
    dhd0_vtthd3.bits.thd4_mode = thd4_mode;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_vtthd3.u32) + offset), dhd0_vtthd3.u32);

    return;
}

td_void dpu_reg_set_dhd0_vtthd3_vtmgthd4(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 vtmgthd4)
{
    volatile u_dhd0_vtthd3 dhd0_vtthd3;
    dhd0_vtthd3.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_vtthd3.u32) + offset));
    dhd0_vtthd3.bits.vtmgthd4 = vtmgthd4;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_vtthd3.u32) + offset), dhd0_vtthd3.u32);

    return;
}

td_void dpu_reg_set_dhd0_vtthd3_thd3_mode(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 thd3_mode)
{
    volatile u_dhd0_vtthd3 dhd0_vtthd3;
    dhd0_vtthd3.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_vtthd3.u32) + offset));
    dhd0_vtthd3.bits.thd3_mode = thd3_mode;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_vtthd3.u32) + offset), dhd0_vtthd3.u32);

    return;
}

td_void dpu_reg_set_dhd0_vtthd3_vtmgthd3(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 vtmgthd3)
{
    volatile u_dhd0_vtthd3 dhd0_vtthd3;
    dhd0_vtthd3.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_vtthd3.u32) + offset));
    dhd0_vtthd3.bits.vtmgthd3 = vtmgthd3;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_vtthd3.u32) + offset), dhd0_vtthd3.u32);

    return;
}

td_void dpu_reg_set_dhd0_vtthd_thd2_mode(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 thd2_mode)
{
    volatile u_dhd0_vtthd dhd0_vtthd;
    dhd0_vtthd.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_vtthd.u32) + offset));
    dhd0_vtthd.bits.thd2_mode = thd2_mode;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_vtthd.u32) + offset), dhd0_vtthd.u32);

    return;
}

td_void dpu_reg_set_dhd0_vtthd_vtmgthd2(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 vtmgthd2)
{
    volatile u_dhd0_vtthd dhd0_vtthd;
    dhd0_vtthd.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_vtthd.u32) + offset));
    dhd0_vtthd.bits.vtmgthd2 = vtmgthd2;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_vtthd.u32) + offset), dhd0_vtthd.u32);

    return;
}

td_void dpu_reg_set_dhd0_vtthd_thd1_mode(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 thd1_mode)
{
    volatile u_dhd0_vtthd dhd0_vtthd;
    dhd0_vtthd.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_vtthd.u32) + offset));
    dhd0_vtthd.bits.thd1_mode = thd1_mode;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_vtthd.u32) + offset), dhd0_vtthd.u32);

    return;
}

td_void dpu_reg_set_dhd0_vtthd_vtmgthd1(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 vtmgthd1)
{
    volatile u_dhd0_vtthd dhd0_vtthd;
    dhd0_vtthd.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_vtthd.u32) + offset));
    dhd0_vtthd.bits.vtmgthd1 = vtmgthd1;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_vtthd.u32) + offset), dhd0_vtthd.u32);

    return;
}

td_void dpu_reg_set_dhd0_parathd_para_thd(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 para_thd)
{
    volatile u_dhd0_parathd dhd0_parathd;
    dhd0_parathd.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_parathd.u32) + offset));
    dhd0_parathd.bits.para_thd = para_thd;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_parathd.u32) + offset), dhd0_parathd.u32);

    return;
}

td_void dpu_reg_set_dhd0_precharge_thd_vsync_te_mode(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 mode)
{
    volatile u_dhd0_te_ctrl dhd0_te_ctrl;
    dhd0_te_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_te_ctrl.u32) + offset));
    dhd0_te_ctrl.bits.vsync_te_mode = mode;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_te_ctrl.u32) + offset), dhd0_te_ctrl.u32);

    return;
}

td_void dpu_reg_set_dhd0_start_pos_fi_start_pos(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 fi_start_pos)
{
    volatile u_dhd0_start_pos dhd0_start_pos;
    dhd0_start_pos.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_start_pos.u32) + offset));
    dhd0_start_pos.bits.fi_start_pos = fi_start_pos;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_start_pos.u32) + offset), dhd0_start_pos.u32);

    return;
}

td_void dpu_reg_set_dhd0_start_pos_timing_start_pos(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 pos)
{
    volatile u_dhd0_start_pos dhd0_start_pos;
    dhd0_start_pos.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_start_pos.u32) + offset));
    dhd0_start_pos.bits.timing_start_pos = pos;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_start_pos.u32) + offset), dhd0_start_pos.u32);

    return;
}

td_void dpu_reg_set_dhd0_start_pos_start_pos(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 start_pos)
{
    volatile u_dhd0_start_pos dhd0_start_pos;
    dhd0_start_pos.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_start_pos.u32) + offset));
    dhd0_start_pos.bits.start_pos = start_pos;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_start_pos.u32) + offset), dhd0_start_pos.u32);

    return;
}

td_void dpu_reg_set_dhd0_clk_dv_ctrl_no_active_area_pos(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 pos)
{
    volatile u_dhd0_clk_dv_ctrl dhd0_clk_dv_ctrl;
    dhd0_clk_dv_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_clk_dv_ctrl.u32) + offset));
    dhd0_clk_dv_ctrl.bits.no_active_area_pos = pos;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_clk_dv_ctrl.u32) + offset), dhd0_clk_dv_ctrl.u32);

    return;
}

td_void dpu_reg_set_dhd0_clk_dv_ctrl_intf_dv_mux(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 intf_dv_mux)
{
    volatile u_dhd0_clk_dv_ctrl dhd0_clk_dv_ctrl;
    dhd0_clk_dv_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_clk_dv_ctrl.u32) + offset));
    dhd0_clk_dv_ctrl.bits.intf_dv_mux = intf_dv_mux;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_clk_dv_ctrl.u32) + offset), dhd0_clk_dv_ctrl.u32);

    return;
}

td_void dpu_reg_set_dhd0_clk_dv_ctrl_intf_clk_mux(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 intf_clk_mux)
{
    volatile u_dhd0_clk_dv_ctrl dhd0_clk_dv_ctrl;
    dhd0_clk_dv_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_clk_dv_ctrl.u32) + offset));
    dhd0_clk_dv_ctrl.bits.intf_clk_mux = intf_clk_mux;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_clk_dv_ctrl.u32) + offset), dhd0_clk_dv_ctrl.u32);

    return;
}

td_void dpu_reg_set_dhd0_rgb_fix_ctrl_rgb_fix_mux(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 rgb_fix_mux)
{
    volatile u_dhd0_rgb_fix_ctrl dhd0_rgb_fix_ctrl;
    dhd0_rgb_fix_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_rgb_fix_ctrl.u32) + offset));
    dhd0_rgb_fix_ctrl.bits.rgb_fix_mux = rgb_fix_mux;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_rgb_fix_ctrl.u32) + offset), dhd0_rgb_fix_ctrl.u32);

    return;
}

td_void dpu_reg_set_dhd0_rgb_fix_ctrl_fix_r(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 fix_r)
{
    volatile u_dhd0_rgb_fix_ctrl dhd0_rgb_fix_ctrl;
    dhd0_rgb_fix_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_rgb_fix_ctrl.u32) + offset));
    dhd0_rgb_fix_ctrl.bits.fix_r = fix_r;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_rgb_fix_ctrl.u32) + offset), dhd0_rgb_fix_ctrl.u32);

    return;
}

td_void dpu_reg_set_dhd0_rgb_fix_ctrl_fix_g(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 fix_g)
{
    volatile u_dhd0_rgb_fix_ctrl dhd0_rgb_fix_ctrl;
    dhd0_rgb_fix_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_rgb_fix_ctrl.u32) + offset));
    dhd0_rgb_fix_ctrl.bits.fix_g = fix_g;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_rgb_fix_ctrl.u32) + offset), dhd0_rgb_fix_ctrl.u32);

    return;
}

td_void dpu_reg_set_dhd0_rgb_fix_ctrl_fix_b(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 fix_b)
{
    volatile u_dhd0_rgb_fix_ctrl dhd0_rgb_fix_ctrl;
    dhd0_rgb_fix_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_rgb_fix_ctrl.u32) + offset));
    dhd0_rgb_fix_ctrl.bits.fix_b = fix_b;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_rgb_fix_ctrl.u32) + offset), dhd0_rgb_fix_ctrl.u32);

    return;
}

td_u32 dpu_reg_get_dhd0_intf_chksum_y_check_sum(const volatile dpu_regs_type *dpu_reg, td_u32 offset)
{
    volatile u_dhd0_intf_chksum_y dhd0_intf_chksum_y;
    dhd0_intf_chksum_y.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_intf_chksum_y.u32) + offset));
    return dhd0_intf_chksum_y.bits.check_sum;
}

td_u32 dpu_reg_get_dhd0_intf_chksum_u_check_sum(const volatile dpu_regs_type *dpu_reg, td_u32 offset)
{
    volatile u_dhd0_intf_chksum_u dhd0_intf_chksum_u;
    dhd0_intf_chksum_u.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_intf_chksum_u.u32) + offset));
    return dhd0_intf_chksum_u.bits.check_sum;
}

td_u32 dpu_reg_get_dhd0_intf_chksum_v_check_sum(const volatile dpu_regs_type *dpu_reg, td_u32 offset)
{
    volatile u_dhd0_intf_chksum_v dhd0_intf_chksum_v;
    dhd0_intf_chksum_v.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_intf_chksum_v.u32) + offset));
    return dhd0_intf_chksum_v.bits.check_sum;
}

td_u32 dpu_reg_get_dhd0_intf1_chksum_y_check_sum(const volatile dpu_regs_type *dpu_reg, td_u32 offset)
{
    volatile u_dhd0_intf1_chksum_y dhd0_intf1_chksum_y;
    dhd0_intf1_chksum_y.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_intf1_chksum_y.u32) + offset));
    return dhd0_intf1_chksum_y.bits.check_sum;
}

td_u32 dpu_reg_get_dhd0_intf1_chksum_u_check_sum(const volatile dpu_regs_type *dpu_reg, td_u32 offset)
{
    volatile u_dhd0_intf1_chksum_u dhd0_intf1_chksum_u;
    dhd0_intf1_chksum_u.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_intf1_chksum_u.u32) + offset));
    return dhd0_intf1_chksum_u.bits.check_sum;
}

td_u32 dpu_reg_get_dhd0_intf1_chksum_v_check_sum(const volatile dpu_regs_type *dpu_reg, td_u32 offset)
{
    volatile u_dhd0_intf1_chksum_v dhd0_intf1_chksum_v;
    dhd0_intf1_chksum_v.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_intf1_chksum_v.u32) + offset));
    return dhd0_intf1_chksum_v.bits.check_sum;
}

td_u32 dpu_reg_get_dhd0_intf_chksum_high1_b0_sum_high(const volatile dpu_regs_type *dpu_reg, td_u32 offset)
{
    volatile u_dhd0_intf_chksum_high1 dhd0_intf_chksum_high1;
    dhd0_intf_chksum_high1.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_intf_chksum_high1.u32) + offset));
    return dhd0_intf_chksum_high1.bits.b0_sum_high;
}

td_u32 dpu_reg_get_dhd0_intf_chksum_high1_g0_sum_high(const volatile dpu_regs_type *dpu_reg, td_u32 offset)
{
    volatile u_dhd0_intf_chksum_high1 dhd0_intf_chksum_high1;
    dhd0_intf_chksum_high1.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_intf_chksum_high1.u32) + offset));
    return dhd0_intf_chksum_high1.bits.g0_sum_high;
}

td_u32 dpu_reg_get_dhd0_intf_chksum_high1_r0_sum_high(const volatile dpu_regs_type *dpu_reg, td_u32 offset)
{
    volatile u_dhd0_intf_chksum_high1 dhd0_intf_chksum_high1;
    dhd0_intf_chksum_high1.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_intf_chksum_high1.u32) + offset));
    return dhd0_intf_chksum_high1.bits.r0_sum_high;
}

td_u32 dpu_reg_get_dhd0_intf_chksum_high2_b1_sum_high(const volatile dpu_regs_type *dpu_reg, td_u32 offset)
{
    volatile u_dhd0_intf_chksum_high2 dhd0_intf_chksum_high2;
    dhd0_intf_chksum_high2.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_intf_chksum_high2.u32) + offset));
    return dhd0_intf_chksum_high2.bits.b1_sum_high;
}

td_u32 dpu_reg_get_dhd0_intf_chksum_high2_g1_sum_high(const volatile dpu_regs_type *dpu_reg, td_u32 offset)
{
    volatile u_dhd0_intf_chksum_high2 dhd0_intf_chksum_high2;
    dhd0_intf_chksum_high2.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_intf_chksum_high2.u32) + offset));
    return dhd0_intf_chksum_high2.bits.g1_sum_high;
}

td_u32 dpu_reg_get_dhd0_intf_chksum_high2_r1_sum_high(const volatile dpu_regs_type *dpu_reg, td_u32 offset)
{
    volatile u_dhd0_intf_chksum_high2 dhd0_intf_chksum_high2;
    dhd0_intf_chksum_high2.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_intf_chksum_high2.u32) + offset));
    return dhd0_intf_chksum_high2.bits.r1_sum_high;
}

td_void dpu_reg_set_dhd0_frm_rdy_mode(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 frm_rdy_mode)
{
    volatile u_dhd0_intf_cmdcfg dhd0_intf_cmdcfg;
    dhd0_intf_cmdcfg.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_intf_cmdcfg.u32) + offset));
    dhd0_intf_cmdcfg.bits.dhd0_tde_frm_rdy_mode = frm_rdy_mode;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_intf_cmdcfg.u32) + offset), dhd0_intf_cmdcfg.u32);

    return;
}

td_void dpu_reg_set_dhd0_te_dly_cfg(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 dhd0_td_dly_cfg)
{
    volatile u_dhd0_intf_cmdcfg dhd0_intf_cmdcfg;
    dhd0_intf_cmdcfg.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_intf_cmdcfg.u32) + offset));
    dhd0_intf_cmdcfg.bits.dhd0_te_dly_cfg = dhd0_td_dly_cfg;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_intf_cmdcfg.u32) + offset), dhd0_intf_cmdcfg.u32);

    return;
}

td_void dpu_reg_set_dhd0_tde_frm_rdy_dhd0_tde_frm_rdy(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 frm_rdy)
{
    volatile u_dhd0_tde_frm_rdy dhd0_tde_frm_rdy;
    dhd0_tde_frm_rdy.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_tde_frm_rdy.u32) + offset));
    dhd0_tde_frm_rdy.bits.dhd0_tde_frm_rdy = frm_rdy;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_tde_frm_rdy.u32) + offset), dhd0_tde_frm_rdy.u32);

    return;
}

td_void dpu_reg_set_dhd0_te_timeout_dhd_te_timeout_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 timeout)
{
    volatile u_dhd0_te_timeout dhd0_te_timeout;
    dhd0_te_timeout.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_te_timeout.u32) + offset));
    dhd0_te_timeout.bits.dhd_te_timeout_en = timeout;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_te_timeout.u32) + offset), dhd0_te_timeout.u32);

    return;
}

td_void dpu_reg_set_dhd0_te_timeout_dhd_te_timeout_thd(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 thd)
{
    volatile u_dhd0_te_timeout dhd0_te_timeout;
    dhd0_te_timeout.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_te_timeout.u32) + offset));
    dhd0_te_timeout.bits.dhd_te_timeout_thd = thd;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_te_timeout.u32) + offset), dhd0_te_timeout.u32);

    return;
}

td_void dpu_reg_set_dhd0_afifo_pre_thd_afifo_pre_uf_thd(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 thd)
{
    volatile u_dhd0_afifo_pre_thd dhd0_afifo_pre_thd;
    dhd0_afifo_pre_thd.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_afifo_pre_thd.u32) + offset));
    dhd0_afifo_pre_thd.bits.afifo_pre_uf_thd = thd;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dhd0_afifo_pre_thd.u32) + offset), dhd0_afifo_pre_thd.u32);

    return;
}

td_u32 dpu_reg_get_dhd0_state_dhd_even(const volatile dpu_regs_type *dpu_reg, td_u32 offset)
{
    volatile u_dhd0_state dhd0_state;
    dhd0_state.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_state.u32) + offset));
    return dhd0_state.bits.dhd_even;
}

td_u32 dpu_reg_get_dhd0_state_count_int(const volatile dpu_regs_type *dpu_reg, td_u32 offset)
{
    volatile u_dhd0_state dhd0_state;
    dhd0_state.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_state.u32) + offset));
    return dhd0_state.bits.count_int;
}

td_u32 dpu_reg_get_dhd0_state_vcnt(const volatile dpu_regs_type *dpu_reg, td_u32 offset)
{
    volatile u_dhd0_state dhd0_state;
    dhd0_state.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_state.u32) + offset));
    return dhd0_state.bits.vcnt;
}

td_u32 dpu_reg_get_dhd0_state_bottom_field(const volatile dpu_regs_type *dpu_reg, td_u32 offset)
{
    volatile u_dhd0_state dhd0_state;
    dhd0_state.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_state.u32) + offset));
    return dhd0_state.bits.bottom_field;
}

td_u32 dpu_reg_get_dhd0_state_vblank(const volatile dpu_regs_type *dpu_reg, td_u32 offset)
{
    volatile u_dhd0_state dhd0_state;
    dhd0_state.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_state.u32) + offset));
    return dhd0_state.bits.vblank;
}

td_u32 dpu_reg_get_dhd0_state_vback_blank(const volatile dpu_regs_type *dpu_reg, td_u32 offset)
{
    volatile u_dhd0_state dhd0_state;
    dhd0_state.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_state.u32) + offset));
    return dhd0_state.bits.vback_blank;
}

td_u32 dpu_reg_get_dhd0_uf_state_start_pos(const volatile dpu_regs_type *dpu_reg, td_u32 offset)
{
    volatile u_dhd0_uf_state dhd0_uf_state;
    dhd0_uf_state.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_uf_state.u32) + offset));
    return dhd0_uf_state.bits.start_pos;
}

td_u32 dpu_reg_get_dhd0_uf_state_ud_first_cnt(const volatile dpu_regs_type *dpu_reg, td_u32 offset)
{
    volatile u_dhd0_uf_state dhd0_uf_state;
    dhd0_uf_state.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_uf_state.u32) + offset));
    return dhd0_uf_state.bits.ud_first_cnt;
}

td_u32 dpu_reg_get_dhd0_vsync_te_state_vsync_te_end_sta(const volatile dpu_regs_type *dpu_reg, td_u32 offset)
{
    volatile u_dhd0_vsync_te_state dhd0_vsync_te_state;
    dhd0_vsync_te_state.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_vsync_te_state.u32) + offset));
    return dhd0_vsync_te_state.bits.vsync_te_end_sta;
}

td_u32 dpu_reg_get_dhd0_vsync_te_state_vsync_te_start_sta1(const volatile dpu_regs_type *dpu_reg, td_u32 offset)
{
    volatile u_dhd0_vsync_te_state dhd0_vsync_te_state;
    dhd0_vsync_te_state.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_vsync_te_state.u32) + offset));
    return dhd0_vsync_te_state.bits.vsync_te_start_sta1;
}

td_u32 dpu_reg_get_dhd0_vsync_te_state_vsync_te_start_sta(const volatile dpu_regs_type *dpu_reg, td_u32 offset)
{
    volatile u_dhd0_vsync_te_state dhd0_vsync_te_state;
    dhd0_vsync_te_state.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_vsync_te_state.u32) + offset));
    return dhd0_vsync_te_state.bits.vsync_te_start_sta;
}

td_u32 dpu_reg_get_dhd0_vsync_te_state1_vsync_te_width(const volatile dpu_regs_type *dpu_reg, td_u32 offset)
{
    volatile u_dhd0_vsync_te_state1 dhd0_vsync_te_state1;
    dhd0_vsync_te_state1.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_vsync_te_state1.u32) + offset));
    return dhd0_vsync_te_state1.bits.vsync_te_width;
}

td_u32 dpu_reg_get_dhd0_vsync_te_state1_vsync_te_vfb(const volatile dpu_regs_type *dpu_reg, td_u32 offset)
{
    volatile u_dhd0_vsync_te_state1 dhd0_vsync_te_state1;
    dhd0_vsync_te_state1.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dhd0_vsync_te_state1.u32) + offset));
    return dhd0_vsync_te_state1.bits.vsync_te_vfb;
}

td_void dpu_reg_set_intf0_ulps_ctrl_ulps_lp_mode_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 mode_en)
{
    volatile u_intf0_ulps_ctrl intf0_ulps_ctrl;
    intf0_ulps_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->intf0_ulps_ctrl.u32) + offset));
    intf0_ulps_ctrl.bits.ulps_lp_mode_en = mode_en;
    dpu_reg_write((uintptr_t)(&(dpu_reg->intf0_ulps_ctrl.u32) + offset), intf0_ulps_ctrl.u32);

    return;
}

td_void dpu_reg_set_intf0_ulps_ctrl_ulps_lp_mode_thd(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 mode_thd)
{
    volatile u_intf0_ulps_ctrl intf0_ulps_ctrl;
    intf0_ulps_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->intf0_ulps_ctrl.u32) + offset));
    intf0_ulps_ctrl.bits.ulps_lp_mode_thd = mode_thd;
    dpu_reg_write((uintptr_t)(&(dpu_reg->intf0_ulps_ctrl.u32) + offset), intf0_ulps_ctrl.u32);

    return;
}

td_void dpu_reg_set_intf0_ulps_delay_ulps_lp_end_delay(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 delay)
{
    volatile u_intf0_ulps_delay intf0_ulps_delay;
    intf0_ulps_delay.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->intf0_ulps_delay.u32) + offset));
    intf0_ulps_delay.bits.ulps_lp_end_delay = delay;
    dpu_reg_write((uintptr_t)(&(dpu_reg->intf0_ulps_delay.u32) + offset), intf0_ulps_delay.u32);

    return;
}

td_void dpu_reg_set_intf0_ulps_delay_ulps_lp_start_delay(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 delay)
{
    volatile u_intf0_ulps_delay intf0_ulps_delay;
    intf0_ulps_delay.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->intf0_ulps_delay.u32) + offset));
    intf0_ulps_delay.bits.ulps_lp_start_delay = delay;
    dpu_reg_write((uintptr_t)(&(dpu_reg->intf0_ulps_delay.u32) + offset), intf0_ulps_delay.u32);

    return;
}

td_void dpu_reg_set_intf0_ulps_stop_soft_stop_lp_mode(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 lp_mode)
{
    volatile u_intf0_ulps_stop intf0_ulps_stop;
    intf0_ulps_stop.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->intf0_ulps_stop.u32) + offset));
    intf0_ulps_stop.bits.soft_stop_lp_mode = lp_mode;
    dpu_reg_write((uintptr_t)(&(dpu_reg->intf0_ulps_stop.u32) + offset), intf0_ulps_stop.u32);

    return;
}

td_u32 dpu_reg_get_intf0_ulps_sta_mipi_lp_mode_en(const volatile dpu_regs_type *dpu_reg, td_u32 offset)
{
    volatile u_intf0_ulps_sta intf0_ulps_sta;
    intf0_ulps_sta.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->intf0_ulps_sta.u32) + offset));
    return intf0_ulps_sta.u32;
}

td_void dpu_reg_set_sram_ctrl_lp_mode(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 lp_mode)
{
    volatile u_dpu_sram_ctrl dpu_sram_ctrl;
    dpu_sram_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dpu_sram_ctrl.u32) + offset));
    dpu_sram_ctrl.bits.dpu_lp_mode = lp_mode;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dpu_sram_ctrl.u32) + offset), dpu_sram_ctrl.u32);

    return;
}

td_void dpu_reg_set_sram_ctrl_block_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 block_en)
{
    volatile u_dpu_sram_ctrl dpu_sram_ctrl;
    dpu_sram_ctrl.u32 = dpu_reg_read((uintptr_t)(&(dpu_reg->dpu_sram_ctrl.u32) + offset));
    dpu_sram_ctrl.bits.dpu_block_en = block_en;
    dpu_reg_write((uintptr_t)(&(dpu_reg->dpu_sram_ctrl.u32) + offset), dpu_sram_ctrl.u32);

    return;
}

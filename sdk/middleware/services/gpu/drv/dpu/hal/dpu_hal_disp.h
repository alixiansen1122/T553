/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description : dpu disp hal chnl header file
 */

#ifndef DRV_GRAPHICS_HAL_DPU_HAL_DISP_H
#define DRV_GRAPHICS_HAL_DPU_HAL_DISP_H

#include "td_base.h"
#include "dpu_reg_define.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

td_void dpu_reg_set_voctrl_vo_ck_gt_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 vo_ck_gt_en);
td_void dpu_reg_set_voctrl_intf_chk_sum_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 intf_chk_sum_en);
td_void dpu_reg_set_voctrl_g0_ck_gt_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 g0_ck_gt_en);
td_void dpu_reg_set_voctrl_g1_ck_gt_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 g1_ck_gt_en);
td_u32 dpu_reg_get_vointsta(const volatile dpu_regs_type *dpu_reg);
td_void dpu_reg_set_vomskintsta(volatile dpu_regs_type *dpu_reg, td_u32 intmask);
td_u32 dpu_reg_get_vomskintsta(const volatile dpu_regs_type *dpu_reg);
td_void dpu_reg_set_vointmsk_enable(volatile dpu_regs_type *dpu_reg, td_u32 intmask);
td_void dpu_reg_set_vointmsk_disable(volatile dpu_regs_type *dpu_reg, td_u32 intmask);
td_u32 dpu_reg_get_vocmd_intsta(const volatile dpu_regs_type *dpu_reg);
td_void dpu_reg_set_vocmd_mskintsta(volatile dpu_regs_type *dpu_reg, td_u32 intmask);
td_u32 dpu_reg_get_vocmd_mskintsta(const volatile dpu_regs_type *dpu_reg);
td_void dpu_reg_set_vocmd_intmsk_enable(volatile dpu_regs_type *dpu_reg, td_u32 intmask);
td_void dpu_reg_set_vocmd_intmsk_disable(volatile dpu_regs_type *dpu_reg, td_u32 intmask);
td_u32 dpu_reg_get_voufsta_g1_uf_sta(const volatile dpu_regs_type *dpu_reg, td_u32 offset);
td_u32 dpu_reg_get_voufsta_g0_uf_sta(const volatile dpu_regs_type *dpu_reg, td_u32 offset);
td_void dpu_reg_set_voufclr_g1_uf_clr(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 g1_uf_clr);
td_void dpu_reg_set_voufclr_g0_uf_clr(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 g0_uf_clr);
td_void dpu_reg_set_cbm_bkg_cbm_bkgy(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 cbm_bkgy);
td_void dpu_reg_set_cbm_bkg_cbm_bkgcb(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 cbm_bkgcb);
td_void dpu_reg_set_cbm_bkg_cbm_bkgcr(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 cbm_bkgcr);
td_void dpu_reg_set_cbm_mix_mixer_prio3(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 mixer_prio3);
td_void dpu_reg_set_cbm_mix_mixer_prio2(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 mixer_prio2);
td_void dpu_reg_set_cbm_mix_mixer_prio1(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 mixer_prio1);
td_void dpu_reg_set_cbm_mix_mixer_prio0(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 mixer_prio0);
td_u32 dpu_reg_get_cbm_mix_priority(const volatile dpu_regs_type *dpu_reg, td_u32 offset);
td_void dpu_reg_set_cbm_ctrl_bypass_mode(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 bypass_mode);
td_void dpu_reg_set_cbm_ctrl_layer0_bypass_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 bypass_en);
td_void dpu_reg_set_partial_cfg_vld(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_bool is_valid);
td_void dpu_reg_set_partial_hdr0(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 partial_hdr0);
td_void dpu_reg_set_partial_pld_data00(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 pld_data00);
td_void dpu_reg_set_partial_pld_data01(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 pld_data01);
td_void dpu_reg_set_partial_hdr1(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 partial_hdr1);
td_void dpu_reg_set_partial_pld_data10(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 pld_data10);
td_void dpu_reg_set_partial_pld_data11(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 pld_data11);
td_void dpu_reg_set_link_ctrl_g0_bypass_dhd0_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 bypass_en);
td_void dpu_reg_set_link_ctrl_format_link(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 format_link);
td_void dpu_reg_set_link_ctrl_mem_link(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 mem_link);
td_void dpu_reg_set_dhd0_ctrl_intf_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 intf_en);
td_void dpu_reg_set_dhd0_ctrl_cbar_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 cbar_en);
td_void dpu_reg_set_dhd0_ctrl_cbar_sel(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 cbar_sel);
td_void dpu_reg_set_dhd0_ctrl_cbar_mode(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 cbar_mode);
td_void dpu_reg_set_dhd0_ctrl_uf_offline_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 uf_offline_en);
td_void dpu_reg_set_dhd0_ctrl_intf_idv(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 intf_idv);
td_void dpu_reg_set_dhd0_ctrl_intf_ihs(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 intf_ihs);
td_void dpu_reg_set_dhd0_ctrl_intf_ivs(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 intf_ivs);
td_void dpu_reg_set_dhd0_ctrl_iop(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 iop);
td_void dpu_reg_set_dhd0_ctrl_disp_mode(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 disp_mode);
td_void dpu_reg_set_dhd0_ctrl_regup(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 regup);
td_void dpu_reg_set_dhd0_vsync1_vbb(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 vbb);
td_void dpu_reg_set_dhd0_vsync1_vact(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 vact);
td_void dpu_reg_set_dhd0_vsync2_vfb(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 vfb);
td_void dpu_reg_set_dhd0_hsync1_hbb(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 hbb);
td_void dpu_reg_set_dhd0_hsync1_hact(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 hact);
td_void dpu_reg_set_dhd0_hsync2_hmid(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 hmid);
td_void dpu_reg_set_dhd0_hsync2_hfb(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 hfb);
td_void dpu_reg_set_dhd0_vplus1_bvbb(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 bvbb);
td_void dpu_reg_set_dhd0_vplus1_bvact(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 bvact);
td_void dpu_reg_set_dhd0_vplus2_bvfb(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 bvfb);
td_void dpu_reg_set_dhd0_pwr_multichn_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 multichn_en);
td_void dpu_reg_set_dhd0_pwr_vpw(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 vpw);
td_void dpu_reg_set_dhd0_pwr_hpw(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 hpw);
td_void dpu_reg_set_dhd0_vtthd3_thd4_mode(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 thd4_mode);
td_void dpu_reg_set_dhd0_vtthd3_vtmgthd4(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 vtmgthd4);
td_void dpu_reg_set_dhd0_vtthd3_thd3_mode(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 thd3_mode);
td_void dpu_reg_set_dhd0_vtthd3_vtmgthd3(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 vtmgthd3);
td_void dpu_reg_set_dhd0_vtthd_thd2_mode(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 thd2_mode);
td_void dpu_reg_set_dhd0_vtthd_vtmgthd2(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 vtmgthd2);
td_void dpu_reg_set_dhd0_vtthd_thd1_mode(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 thd1_mode);
td_void dpu_reg_set_dhd0_vtthd_vtmgthd1(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 vtmgthd1);
td_void dpu_reg_set_dhd0_parathd_para_thd(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 para_thd);
td_void dpu_reg_set_dhd0_precharge_thd_vsync_te_mode(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 mode);
td_void dpu_reg_set_dhd0_start_pos_fi_start_pos(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 fi_start_pos);
td_void dpu_reg_set_dhd0_start_pos_timing_start_pos(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 pos);
td_void dpu_reg_set_dhd0_start_pos_start_pos(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 start_pos);
td_void dpu_reg_set_dhd0_clk_dv_ctrl_no_active_area_pos(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 pos);
td_void dpu_reg_set_dhd0_clk_dv_ctrl_intf_dv_mux(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 intf_dv_mux);
td_void dpu_reg_set_dhd0_clk_dv_ctrl_intf_clk_mux(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 intf_clk_mux);
td_void dpu_reg_set_dhd0_rgb_fix_ctrl_rgb_fix_mux(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 rgb_fix_mux);
td_void dpu_reg_set_dhd0_rgb_fix_ctrl_fix_r(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 fix_r);
td_void dpu_reg_set_dhd0_rgb_fix_ctrl_fix_g(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 fix_g);
td_void dpu_reg_set_dhd0_rgb_fix_ctrl_fix_b(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 fix_b);
td_u32 dpu_reg_get_dhd0_intf_chksum_y_check_sum(const volatile dpu_regs_type *dpu_reg, td_u32 offset);
td_u32 dpu_reg_get_dhd0_intf_chksum_u_check_sum(const volatile dpu_regs_type *dpu_reg, td_u32 offset);
td_u32 dpu_reg_get_dhd0_intf_chksum_v_check_sum(const volatile dpu_regs_type *dpu_reg, td_u32 offset);
td_u32 dpu_reg_get_dhd0_intf1_chksum_y_check_sum(const volatile dpu_regs_type *dpu_reg, td_u32 offset);
td_u32 dpu_reg_get_dhd0_intf1_chksum_u_check_sum(const volatile dpu_regs_type *dpu_reg, td_u32 offset);
td_u32 dpu_reg_get_dhd0_intf1_chksum_v_check_sum(const volatile dpu_regs_type *dpu_reg, td_u32 offset);
td_u32 dpu_reg_get_dhd0_intf_chksum_high1_b0_sum_high(const volatile dpu_regs_type *dpu_reg, td_u32 offset);
td_u32 dpu_reg_get_dhd0_intf_chksum_high1_g0_sum_high(const volatile dpu_regs_type *dpu_reg, td_u32 offset);
td_u32 dpu_reg_get_dhd0_intf_chksum_high1_r0_sum_high(const volatile dpu_regs_type *dpu_reg, td_u32 offset);
td_u32 dpu_reg_get_dhd0_intf_chksum_high2_b1_sum_high(const volatile dpu_regs_type *dpu_reg, td_u32 offset);
td_u32 dpu_reg_get_dhd0_intf_chksum_high2_g1_sum_high(const volatile dpu_regs_type *dpu_reg, td_u32 offset);
td_u32 dpu_reg_get_dhd0_intf_chksum_high2_r1_sum_high(const volatile dpu_regs_type *dpu_reg, td_u32 offset);
td_void dpu_reg_set_dhd0_frm_rdy_mode(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 frm_rdy_mode);
td_void dpu_reg_set_dhd0_te_dly_cfg(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 dhd0_td_dly_cfg);
td_void dpu_reg_set_dhd0_tde_frm_rdy_dhd0_tde_frm_rdy(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 frm_rdy);
td_void dpu_reg_set_dhd0_te_timeout_dhd_te_timeout_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 timeout);
td_void dpu_reg_set_dhd0_te_timeout_dhd_te_timeout_thd(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 thd);
td_void dpu_reg_set_dhd0_afifo_pre_thd_afifo_pre_uf_thd(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 thd);
td_u32 dpu_reg_get_dhd0_state_dhd_even(const volatile dpu_regs_type *dpu_reg, td_u32 offset);
td_u32 dpu_reg_get_dhd0_state_count_int(const volatile dpu_regs_type *dpu_reg, td_u32 offset);
td_u32 dpu_reg_get_dhd0_state_vcnt(const volatile dpu_regs_type *dpu_reg, td_u32 offset);
td_u32 dpu_reg_get_dhd0_state_bottom_field(const volatile dpu_regs_type *dpu_reg, td_u32 offset);
td_u32 dpu_reg_get_dhd0_state_vblank(const volatile dpu_regs_type *dpu_reg, td_u32 offset);
td_u32 dpu_reg_get_dhd0_state_vback_blank(const volatile dpu_regs_type *dpu_reg, td_u32 offset);
td_u32 dpu_reg_get_dhd0_uf_state_start_pos(const volatile dpu_regs_type *dpu_reg, td_u32 offset);
td_u32 dpu_reg_get_dhd0_uf_state_ud_first_cnt(const volatile dpu_regs_type *dpu_reg, td_u32 offset);
td_u32 dpu_reg_get_dhd0_vsync_te_state_vsync_te_end_sta(const volatile dpu_regs_type *dpu_reg, td_u32 offset);
td_u32 dpu_reg_get_dhd0_vsync_te_state_vsync_te_start_sta1(const volatile dpu_regs_type *dpu_reg, td_u32 offset);
td_u32 dpu_reg_get_dhd0_vsync_te_state_vsync_te_start_sta(const volatile dpu_regs_type *dpu_reg, td_u32 offset);
td_u32 dpu_reg_get_dhd0_vsync_te_state1_vsync_te_width(const volatile dpu_regs_type *dpu_reg, td_u32 offset);
td_u32 dpu_reg_get_dhd0_vsync_te_state1_vsync_te_vfb(const volatile dpu_regs_type *dpu_reg, td_u32 offset);
td_void dpu_reg_set_intf0_ulps_ctrl_ulps_lp_mode_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 mode_en);
td_void dpu_reg_set_intf0_ulps_ctrl_ulps_lp_mode_thd(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 mode_thd);
td_void dpu_reg_set_intf0_ulps_delay_ulps_lp_end_delay(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 delay);
td_void dpu_reg_set_intf0_ulps_delay_ulps_lp_start_delay(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 delay);
td_void dpu_reg_set_intf0_ulps_stop_soft_stop_lp_mode(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 lp_mode);
td_u32 dpu_reg_get_intf0_ulps_sta_mipi_lp_mode_en(const volatile dpu_regs_type *dpu_reg, td_u32 offset);
td_void dpu_reg_set_sram_ctrl_lp_mode(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 lp_mode);
td_void dpu_reg_set_sram_ctrl_block_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 block_en);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif /* DRV_GRAPHICS_HAL_DPU_HAL_DISP_H */

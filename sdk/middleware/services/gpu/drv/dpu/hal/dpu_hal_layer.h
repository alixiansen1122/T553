/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description : dpu layer hal header file
 */

#ifndef DRV_GRAPHICS_HAL_DPU_HAL_LAYER_H
#define DRV_GRAPHICS_HAL_DPU_HAL_LAYER_H

#include "td_base.h"
#include "dpu_reg_define.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

td_void dpu_reg_set_g0_ctrl_surface_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 surface_en);
td_void dpu_reg_set_g0_ctrl_depremult(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 depremult);
td_void dpu_reg_set_g0_ctrl_galpha(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 galpha);
td_void dpu_reg_set_g0_upd_regup(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 regup);
td_u32 dpu_reg_get_g0_0reso_read_oh(const volatile dpu_regs_type *dpu_reg, td_u32 offset);
td_u32 dpu_reg_get_g0_0reso_read_ow(const volatile dpu_regs_type *dpu_reg, td_u32 offset);
td_void dpu_reg_set_g0_dfpos_disp_yfpos(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 disp_yfpos);
td_void dpu_reg_set_g0_dfpos_disp_xfpos(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 disp_xfpos);
td_void dpu_reg_set_g0_dlpos_disp_ylpos(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 disp_ylpos);
td_void dpu_reg_set_g0_dlpos_disp_xlpos(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 disp_xlpos);
td_void dpu_reg_set_g0_vfpos_video_yfpos(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 video_yfpos);
td_void dpu_reg_set_g0_vfpos_video_xfpos(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 video_xfpos);
td_void dpu_reg_set_g0_vlpos_video_ylpos(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 video_ylpos);
td_void dpu_reg_set_g0_vlpos_video_xlpos(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 video_xlpos);
td_void dpu_reg_set_g0_bk_vbk_y(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 vbk_y);
td_void dpu_reg_set_g0_bk_vbk_cb(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 vbk_cb);
td_void dpu_reg_set_g0_bk_vbk_cr(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 vbk_cr);
td_void dpu_reg_set_g0_alpha_vbk_alpha(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 vbk_alpha);
td_void dpu_reg_set_g0_mute_bk_mute_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 mute_en);
td_void dpu_reg_set_g0_mute_bk_mute_y(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 mute_y);
td_void dpu_reg_set_g0_mute_bk_mute_cb(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 mute_cb);
td_void dpu_reg_set_g0_mute_bk_mute_cr(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 mute_cr);
td_void dpu_reg_set_g1_csc_ctrl_ck_gt_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 gt_en);
td_void dpu_reg_set_g1_csc_ctrl_demo_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 demo_en);
td_void dpu_reg_set_g1_csc_ctrl_csc_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 csc_en);
td_void dpu_reg_set_g1_csc_coef00(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 coef00);
td_void dpu_reg_set_g1_csc_coef01(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 coef01);
td_void dpu_reg_set_g1_csc_coef02(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 coef02);
td_void dpu_reg_set_g1_csc_coef10(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 coef10);
td_void dpu_reg_set_g1_csc_coef11(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 coef11);
td_void dpu_reg_set_g1_csc_coef12(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 coef12);
td_void dpu_reg_set_g1_csc_coef20(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 coef20);
td_void dpu_reg_set_g1_csc_coef21(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 coef21);
td_void dpu_reg_set_g1_csc_coef22(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 coef22);
td_void dpu_reg_set_g1_csc_scale(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 csc_scale);
td_void dpu_reg_set_g1_csc_idc0(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 csc_idc0);
td_void dpu_reg_set_g1_csc_idc1(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 csc_idc1);
td_void dpu_reg_set_g1_csc_idc2(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 csc_idc2);
td_void dpu_reg_set_g1_csc_odc0(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 csc_odc0);
td_void dpu_reg_set_g1_csc_odc1(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 csc_odc1);
td_void dpu_reg_set_g1_csc_odc2(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 csc_odc2);
td_void dpu_reg_set_g1_csc_min_y(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 csc_min_y);
td_void dpu_reg_set_g1_csc_min_c(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 csc_min_c);
td_void dpu_reg_set_g1_csc_max_y(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 csc_max_y);
td_void dpu_reg_set_g1_csc_max_c(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 csc_max_c);
td_void dpu_reg_set_g1_csc2_coef00(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 coef00);
td_void dpu_reg_set_g1_csc2_coef01(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 coef01);
td_void dpu_reg_set_g1_csc2_coef02(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 coef02);
td_void dpu_reg_set_g1_csc2_coef10(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 coef10);
td_void dpu_reg_set_g1_csc2_coef11(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 coef11);
td_void dpu_reg_set_g1_csc2_coef12(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 coef12);
td_void dpu_reg_set_g1_csc2_coef20(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 coef20);
td_void dpu_reg_set_g1_csc2_coef21(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 coef21);
td_void dpu_reg_set_g1_csc2_coef22(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 coef22);
td_void dpu_reg_set_g1_csc2_scale(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 csc2_scale);
td_void dpu_reg_set_g1_csc2_idc0(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 csc2_idc0);
td_void dpu_reg_set_g1_csc2_idc1(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 csc2_idc1);
td_void dpu_reg_set_g1_csc2_idc2(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 csc2_idc2);
td_void dpu_reg_set_g1_csc2_odc0(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 csc2_odc0);
td_void dpu_reg_set_g1_csc2_odc1(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 csc2_odc1);
td_void dpu_reg_set_g1_csc2_odc2(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 csc2_odc2);
td_void dpu_reg_set_g1_csc2_min_y(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 csc2_min_y);
td_void dpu_reg_set_g1_csc2_min_c(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 csc2_min_c);
td_void dpu_reg_set_g1_csc2_max_y(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 csc2_max_y);
td_void dpu_reg_set_g1_csc2_max_c(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 csc2_max_c);
td_void dpu_reg_set_g1_csc_ink_ctrl_color_mode(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 mode);
td_void dpu_reg_set_g1_csc_ink_ctrl_cross_enable(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 enable);
td_void dpu_reg_set_g1_csc_ink_ctrl_data_fmt(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 data_fmt);
td_void dpu_reg_set_g1_csc_ink_ctrl_ink_sel(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 ink_sel);
td_void dpu_reg_set_g1_csc_ink_ctrl_ink_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 ink_en);
td_void dpu_reg_set_g1_csc_ink_pos_y_pos(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 y_pos);
td_void dpu_reg_set_g1_csc_ink_pos_x_pos(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 x_pos);
td_void dpu_reg_set_g1_csc_ink_data_ink_data(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 ink_data);
td_void dpu_reg_set_g1_csc_ink_data2_ink_data2(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 ink_data2);
td_void dpu_reg_set_g1_cvfir_vinfo_vzme_ck_gt_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 gt_en);
td_void dpu_reg_set_g1_cvfir_vinfo_out_pro(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 out_pro);
td_void dpu_reg_set_g1_cvfir_vinfo_out_fmt(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 out_fmt);
td_void dpu_reg_set_g1_cvfir_vinfo_out_height(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 out_height);
td_void dpu_reg_set_g1_cvfir_vsp_cvfir_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 cvfir_en);
td_void dpu_reg_set_g1_cvfir_vsp_cvmid_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 cvmid_en);
td_void dpu_reg_set_g1_cvfir_vsp_cvfir_mode(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 cvfir_mode);
td_void dpu_reg_set_g1_cvfir_vsp_vratio(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 vratio);
td_void dpu_reg_set_g1_cvfir_vchroma_offset(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 vchroma_offset);
td_void dpu_reg_set_g1_cvfir_vbchroma_offset(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 vbchroma_offset);
td_void dpu_reg_set_g1_cvfir_vcoef0_vccoef00(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 vccoef00);
td_void dpu_reg_set_g1_cvfir_vcoef0_vccoef01(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 vccoef01);
td_void dpu_reg_set_g1_cvfir_vcoef0_vccoef02(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 vccoef02);
td_void dpu_reg_set_g1_cvfir_vcoef1_vccoef03(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 vccoef03);
td_void dpu_reg_set_g1_cvfir_vcoef1_vccoef10(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 vccoef10);
td_void dpu_reg_set_g1_cvfir_vcoef1_vccoef11(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 vccoef11);
td_void dpu_reg_set_g1_cvfir_vcoef2_vccoef12(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 vccoef12);
td_void dpu_reg_set_g1_cvfir_vcoef2_vccoef13(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 vccoef13);
td_void dpu_reg_set_g1_hfir_ctrl_ck_gt_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 ck_gt_en);
td_void dpu_reg_set_g1_hfir_ctrl_mid_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 mid_en);
td_void dpu_reg_set_g1_hfir_ctrl_hfir_mode(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 hfir_mode);
td_void dpu_reg_set_g1_hfircoef01_coef1(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 coef1);
td_void dpu_reg_set_g1_hfircoef01_coef0(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 coef0);
td_void dpu_reg_set_g1_hfircoef23_coef3(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 coef3);
td_void dpu_reg_set_g1_hfircoef23_coef2(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 coef2);
td_void dpu_reg_set_g1_hfircoef45_coef5(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 coef5);
td_void dpu_reg_set_g1_hfircoef45_coef4(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 coef4);
td_void dpu_reg_set_g1_hfircoef67_coef7(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 coef7);
td_void dpu_reg_set_g1_hfircoef67_coef6(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 coef6);
td_void dpu_reg_set_gfx_read_ctrl_fdr_ck_gt_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 fdr_ck_gt_en);
td_void dpu_reg_set_gfx_read_ctrl_mute_req_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 mute_req_en);
td_void dpu_reg_set_gfx_read_ctrl_mute_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 mute_en);
td_void dpu_reg_set_gfx_read_ctrl_flip_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 flip_en);
td_void dpu_reg_set_gfx_read_ctrl_draw_mode(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 draw_mode);
td_void dpu_reg_set_gfx_read_ctrl_read_mode(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 read_mode);
td_void dpu_reg_set_gfx_mac_ctrl_req_len(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 req_len);
td_void dpu_reg_set_gfx_mac_ctrl_req_ctrl(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 req_ctrl);
td_void dpu_reg_set_gfx_out_ctrl_testpattern_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 en);
td_void dpu_reg_set_gfx_out_ctrl_premulti_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 premulti_en);
td_void dpu_reg_set_gfx_out_ctrl_bitext(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 bitext);
td_void dpu_reg_set_gfx_out_ctrl_key_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 key_en);
td_void dpu_reg_set_gfx_out_ctrl_key_mode(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 key_mode);
td_void dpu_reg_set_gfx_out_ctrl_palpha_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 palpha_en);
td_void dpu_reg_set_gfx_out_ctrl_palpha_range(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 palpha_range);
td_void dpu_reg_set_gfx_mute_alpha_mute_alpha(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 mute_alpha);
td_void dpu_reg_set_gfx_mute_bk_mute_y(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 mute_y);
td_void dpu_reg_set_gfx_mute_bk_mute_cb(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 mute_cb);
td_void dpu_reg_set_gfx_mute_bk_mute_cr(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 mute_cr);
td_void dpu_reg_set_gfx_1555_alpha_alpha_1(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 alpha_1);
td_void dpu_reg_set_gfx_1555_alpha_alpha_0(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 alpha_0);
td_void dpu_reg_set_gfx_src_info_dcmp_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 dcmp_en);
td_void dpu_reg_set_gfx_src_info_disp_mode(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 disp_mode);
td_void dpu_reg_set_gfx_src_info_ifmt(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 ifmt);
td_void dpu_reg_set_gfx_src_reso_src_h(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 src_h);
td_void dpu_reg_set_gfx_src_reso_src_w(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 src_w);
td_void dpu_reg_set_gfx_src_crop_src_crop_y(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 src_crop_y);
td_void dpu_reg_set_gfx_src_crop_src_crop_x(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 src_crop_x);
td_void dpu_reg_set_gfx_ireso_ireso_h(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 ireso_h);
td_void dpu_reg_set_gfx_ireso_ireso_w(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 ireso_w);
td_void dpu_reg_set_gfx_addr_h_gfx_addr_h(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 addr_h);
td_void dpu_reg_set_gfx_addr_l_gfx_addr_l(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 addr_l);
td_void dpu_reg_set_gfx_stride_surface_stride(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 stride);
td_void dpu_reg_set_gfx_addr_h_c_gfx_addr_h_c(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 addr_h_c);
td_void dpu_reg_set_gfx_addr_l_c_gfx_addr_l_c(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 addr_l_c);
td_void dpu_reg_set_gfx_stride_c_surface_stride_c(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 stride_c);
td_void dpu_reg_set_gfx_dcmp_addr_h_dcmp_addr_h(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 dcmp_addr_h);
td_void dpu_reg_set_gfx_dcmp_addr_l_dcmp_addr_l(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 dcmp_addr_l);
td_void dpu_reg_set_gfx_testpat_cfg_tp_mode(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 tp_mode);
td_void dpu_reg_set_gfx_testpat_cfg_tp_color_mode(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 mode);
td_void dpu_reg_set_gfx_testpat_cfg_tp_line_w(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 tp_line_w);
td_void dpu_reg_set_gfx_testpat_cfg_tp_speed(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 tp_speed);
td_void dpu_reg_set_gfx_testpat_seed_tp_seed(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 tp_seed);
td_void dpu_reg_set_gfx_dcmp_ctrl_alpha_bypass(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 alpha_bypass);
td_void dpu_reg_set_gfx_dcmp_ctrl_dcmp_fmt(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 dcmp_fmt);
td_void dpu_reg_set_gfx_dcmp_ctrl_cmp_mode(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 cmp_mode);
td_void dpu_reg_set_gfx_dcmp_ctrl_ice_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 ice_en);
td_void dpu_reg_set_gfx_debug_ctrl_checksum_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 checksum_en);
td_u32 dpu_reg_get_gfx_in_ar_checksum0_ar_checksum0(const volatile dpu_regs_type *dpu_reg, td_u32 offset);
td_u32 dpu_reg_get_gfx_in_ar_checksum1_ar_checksum1(const volatile dpu_regs_type *dpu_reg, td_u32 offset);
td_u32 dpu_reg_get_gfx_in_gb_checksum0_gb_checksum0(const volatile dpu_regs_type *dpu_reg, td_u32 offset);
td_u32 dpu_reg_get_gfx_in_gb_checksum1_gb_checksum1(const volatile dpu_regs_type *dpu_reg, td_u32 offset);
td_void dpu_reg_set_gfx_ckey_max_key_r_max(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 key_r_max);
td_void dpu_reg_set_gfx_ckey_max_key_g_max(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 key_g_max);
td_void dpu_reg_set_gfx_ckey_max_key_b_max(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 key_b_max);
td_void dpu_reg_set_gfx_ckey_min_key_r_min(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 key_r_min);
td_void dpu_reg_set_gfx_ckey_min_key_g_min(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 key_g_min);
td_void dpu_reg_set_gfx_ckey_min_key_b_min(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 key_b_min);
td_void dpu_reg_set_gfx_ckey_mask_key_r_msk(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 key_r_msk);
td_void dpu_reg_set_gfx_ckey_mask_key_g_msk(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 key_g_msk);
td_void dpu_reg_set_gfx_ckey_mask_key_b_msk(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 key_b_msk);
td_void dpu_reg_set_gfx_htc_dcmp_is_a_offline(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 is_a_offline);
td_void dpu_reg_set_gfx_htc_dcmp_is_a_bypass(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 is_a_bypass);
td_void dpu_reg_set_gfx_htc_dcmp_pix_format(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 pix_format);
td_void dpu_reg_set_gfx_htc_dcmp_cmp_mode(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 cmp_mode);
td_void dpu_reg_set_gfx_htc_dcmp_ice_en(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 ice_en);
td_void dpu_reg_set_gfx_htc_dcmp_dbg_reg_dcmp_debug(volatile dpu_regs_type *dpu_reg, td_u32 offset, td_u32 dcmp_debug);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif /* DRV_GRAPHICS_HAL_DPU_HAL_LAYER_H */
/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: dpu reg common
 */

#include "dpu_hal.h"
#include "soc_osal.h"

#include "dpu_hal_disp.h"
#include "dpu_hal_layer.h"
#include "dpu_hal_crg.h"

#define DPU_REG_BASEADDR 0x56020000

static volatile dpu_regs_type *g_dpu_reg;

static td_s32 dpu_hal_reg_map(void)
{
    if (g_dpu_reg == TD_NULL) {
        g_dpu_reg = (dpu_regs_type *)osal_ioremap_nocache(DPU_REG_BASEADDR, sizeof(dpu_regs_type));
        if (g_dpu_reg == TD_NULL) {
            return EXT_FAILURE;
        }
    }

    return EXT_SUCCESS;
}

static td_void dpu_hal_reg_unmap(void)
{
    if (g_dpu_reg != TD_NULL) {
        osal_iounmap((void *)g_dpu_reg, sizeof(dpu_regs_type));
        g_dpu_reg = TD_NULL;
    }
    return;
}

td_s32 dpu_hal_init(void)
{
    dpu_hal_reg_map();
    dpu_hal_crg_init();
    hal_disp_init();

    return EXT_SUCCESS;
}

td_void dpu_hal_deinit(void)
{
    hal_disp_deinit();
    dpu_hal_crg_deinit();
    dpu_hal_reg_unmap();
}

/* ======================DISP INTERFACE CALLING============== */

#define HAL_DISP_OFFSET 0

td_void hal_disp_init(void)
{
    dpu_reg_set_voctrl_intf_chk_sum_en(g_dpu_reg, HAL_DISP_OFFSET, 1);
    dpu_reg_set_voctrl_vo_ck_gt_en(g_dpu_reg, HAL_DISP_OFFSET, 0);
    dpu_reg_set_voctrl_g0_ck_gt_en(g_dpu_reg, HAL_DISP_OFFSET, 0);
    dpu_reg_set_voctrl_g1_ck_gt_en(g_dpu_reg, HAL_DISP_OFFSET, 0);

    dpu_reg_set_intf0_ulps_ctrl_ulps_lp_mode_en(g_dpu_reg, HAL_DISP_OFFSET, 0);
    dpu_reg_set_intf0_ulps_ctrl_ulps_lp_mode_thd(g_dpu_reg, HAL_DISP_OFFSET, 400); /* 400:lp thd */
    dpu_reg_set_intf0_ulps_delay_ulps_lp_end_delay(g_dpu_reg, HAL_DISP_OFFSET, 0);
    dpu_reg_set_intf0_ulps_delay_ulps_lp_start_delay(g_dpu_reg, HAL_DISP_OFFSET, 5); /* 5:delay lines */

    dpu_reg_set_sram_ctrl_lp_mode(g_dpu_reg, HAL_DISP_OFFSET, 0x2);     /* 2:lp default mode */
    dpu_reg_set_sram_ctrl_block_en(g_dpu_reg, HAL_DISP_OFFSET, 0x3ff);  /* 3ff:default block */
}

td_void hal_disp_deinit(void)
{
    dpu_reg_set_voctrl_intf_chk_sum_en(g_dpu_reg, HAL_DISP_OFFSET, 0);
    dpu_reg_set_voctrl_vo_ck_gt_en(g_dpu_reg, HAL_DISP_OFFSET, 1);
    dpu_reg_set_voctrl_g0_ck_gt_en(g_dpu_reg, HAL_DISP_OFFSET, 1);
    dpu_reg_set_voctrl_g1_ck_gt_en(g_dpu_reg, HAL_DISP_OFFSET, 1);

    dpu_reg_set_intf0_ulps_ctrl_ulps_lp_mode_en(g_dpu_reg, HAL_DISP_OFFSET, 0);
    dpu_reg_set_sram_ctrl_block_en(g_dpu_reg, HAL_DISP_OFFSET, 0);
}

td_u32 dpu_hal_disp_get_ulps_mipi_state(void)
{
    return dpu_reg_get_intf0_ulps_sta_mipi_lp_mode_en(g_dpu_reg, HAL_DISP_OFFSET);
}

td_void dpu_hal_disp_set_timing(dpu_disp_timing *timing)
{
    dpu_reg_set_dhd0_vsync2_vfb(g_dpu_reg, HAL_DISP_OFFSET, timing->vfb - 1);
    dpu_reg_set_dhd0_vsync1_vbb(g_dpu_reg, HAL_DISP_OFFSET, timing->vbb - 1);
    dpu_reg_set_dhd0_vsync1_vact(g_dpu_reg, HAL_DISP_OFFSET, timing->vact - 1);

    dpu_reg_set_dhd0_hsync2_hfb(g_dpu_reg, HAL_DISP_OFFSET, timing->hfb - 1);
    dpu_reg_set_dhd0_hsync1_hbb(g_dpu_reg, HAL_DISP_OFFSET, timing->hbb - 1);
    dpu_reg_set_dhd0_hsync1_hact(g_dpu_reg, HAL_DISP_OFFSET, timing->hact - 1);

    dpu_reg_set_dhd0_ctrl_intf_idv(g_dpu_reg, HAL_DISP_OFFSET, timing->idv);
    dpu_reg_set_dhd0_ctrl_intf_ihs(g_dpu_reg, HAL_DISP_OFFSET, timing->ihs);
    dpu_reg_set_dhd0_ctrl_intf_ivs(g_dpu_reg, HAL_DISP_OFFSET, timing->ivs);
    dpu_reg_set_dhd0_ctrl_iop(g_dpu_reg, HAL_DISP_OFFSET, timing->iop);

    dpu_reg_set_dhd0_pwr_vpw(g_dpu_reg, HAL_DISP_OFFSET, timing->vpw - 1);
    dpu_reg_set_dhd0_pwr_hpw(g_dpu_reg, HAL_DISP_OFFSET, timing->hpw - 1);
}

td_void dpu_hal_disp_set_int_thd(td_u32 int_type, td_u32 thd)
{
    if ((int_type & DPU_DISP_INT_TYPE_SETTING) == DPU_DISP_INT_TYPE_SETTING) {
        dpu_reg_set_dhd0_vtthd_vtmgthd1(g_dpu_reg, HAL_DISP_OFFSET, thd);
    }
    if ((int_type & DPU_DISP_INT_TYPE_FINISH) == DPU_DISP_INT_TYPE_FINISH) {
        dpu_reg_set_dhd0_vtthd_vtmgthd2(g_dpu_reg, HAL_DISP_OFFSET, thd);
    }
    if ((int_type & DPU_DISP_INT_TYPE_RESERVE) == DPU_DISP_INT_TYPE_RESERVE) {
        dpu_reg_set_dhd0_vtthd3_vtmgthd3(g_dpu_reg, HAL_DISP_OFFSET, thd);
    }
}

td_u32 dpu_hal_disp_get_int_mipi_state(void)
{
    return dpu_reg_get_vocmd_mskintsta(g_dpu_reg);
}

td_void dpu_hal_disp_clear_int_mipi_state(void)
{
    return dpu_reg_set_vocmd_mskintsta(g_dpu_reg, 1);
}

td_void dpu_hal_disp_set_int_mipi_enable(td_bool enable)
{
    if (enable) {
        dpu_reg_set_vocmd_intmsk_enable(g_dpu_reg, 1);
    } else {
        dpu_reg_set_vocmd_intmsk_disable(g_dpu_reg, 1);
    }
}

td_void dpu_hal_disp_set_dhd0_te_mode(td_bool enable)
{
    dpu_reg_set_dhd0_precharge_thd_vsync_te_mode(g_dpu_reg, HAL_DISP_OFFSET, enable);
}

td_void dpu_hal_disp_set_dhd_te_timeout_en(td_bool enable)
{
    dpu_reg_set_dhd0_te_timeout_dhd_te_timeout_en(g_dpu_reg, HAL_DISP_OFFSET, enable);
}

td_void dpu_hal_disp_set_dhd_te_timeout_thd(td_u32 thd)
{
    dpu_reg_set_dhd0_te_timeout_dhd_te_timeout_thd(g_dpu_reg, HAL_DISP_OFFSET, thd);
}

td_void dpu_hal_disp_set_start_pos(td_u32 start_pos)
{
    dpu_reg_set_dhd0_start_pos_start_pos(g_dpu_reg, HAL_DISP_OFFSET, start_pos);
}

td_void dpu_hal_disp_set_chnl_regup(void)
{
    dpu_reg_set_dhd0_ctrl_regup(g_dpu_reg, HAL_DISP_OFFSET, 1);
}

td_void dpu_hal_disp_set_chnl_enable(td_bool enable)
{
    dpu_reg_set_dhd0_ctrl_intf_en(g_dpu_reg, HAL_DISP_OFFSET, enable);
}

td_void dpu_hal_disp_set_int_enable(td_u32 int_type, td_bool enable)
{
    if (enable) {
        dpu_reg_set_vointmsk_enable(g_dpu_reg, int_type);
    } else {
        dpu_reg_set_vointmsk_disable(g_dpu_reg, int_type);
    }
}

td_u32 dpu_hal_disp_get_int_state(void)
{
    return dpu_reg_get_vomskintsta(g_dpu_reg);
}

td_void dpu_hal_disp_clear_int_state(td_u32 int_type)
{
    dpu_reg_set_vomskintsta(g_dpu_reg, int_type);
}

td_void dpu_hal_disp_set_cbm_priority(dpu_disp_cbm_mix_layer layer, dpu_disp_cbm_mix_priority prio)
{
    if (prio == DPU_DISP_CBM_MIX_PRIORITY_0) {
        dpu_reg_set_cbm_mix_mixer_prio0(g_dpu_reg, HAL_DISP_OFFSET, layer);
    } else if (prio == DPU_DISP_CBM_MIX_PRIORITY_1) {
        dpu_reg_set_cbm_mix_mixer_prio1(g_dpu_reg, HAL_DISP_OFFSET, layer);
    }
}

td_u32 dpu_hal_disp_get_cbm_priority(td_void)
{
    return dpu_reg_get_cbm_mix_priority(g_dpu_reg, HAL_DISP_OFFSET);
}

td_void dpu_hal_disp_set_partial_cfg_vld(td_bool is_valid)
{
    dpu_reg_set_partial_cfg_vld(g_dpu_reg, HAL_DISP_OFFSET, is_valid);
}

td_void dpu_hal_disp_set_partial_cmd_info(dpu_hal_partial_cmd_info *cmd_info)
{
    dpu_reg_set_partial_hdr0(g_dpu_reg, HAL_DISP_OFFSET, cmd_info->hdr0);
    dpu_reg_set_partial_pld_data00(g_dpu_reg, HAL_DISP_OFFSET, cmd_info->pld_data00);
    dpu_reg_set_partial_pld_data01(g_dpu_reg, HAL_DISP_OFFSET, cmd_info->pld_data01);

    dpu_reg_set_partial_hdr1(g_dpu_reg, HAL_DISP_OFFSET, cmd_info->hdr1);
    dpu_reg_set_partial_pld_data10(g_dpu_reg, HAL_DISP_OFFSET, cmd_info->pld_data10);
    dpu_reg_set_partial_pld_data11(g_dpu_reg, HAL_DISP_OFFSET, cmd_info->pld_data11);
}

td_void dpu_hal_disp_set_mem_link(td_u32 mem_link)
{
    dpu_reg_set_link_ctrl_mem_link(g_dpu_reg, HAL_DISP_OFFSET, mem_link);
}

td_void dpu_hal_disp_set_frame_ready_mode(td_u32 frm_rdy_mode)
{
    dpu_reg_set_dhd0_frm_rdy_mode(g_dpu_reg, HAL_DISP_OFFSET, frm_rdy_mode);
}

td_void dpu_hal_disp_set_te_isr_dly_lines(td_u32 delay_lines)
{
    dpu_reg_set_dhd0_te_dly_cfg(g_dpu_reg, HAL_DISP_OFFSET, delay_lines);
}

td_void dpu_hal_disp_set_frame_ready(void)
{
    dpu_reg_set_dhd0_tde_frm_rdy_dhd0_tde_frm_rdy(g_dpu_reg, HAL_DISP_OFFSET, 1);
}

td_void dpu_hal_disp_set_cbar(td_bool enable)
{
    dpu_reg_set_dhd0_ctrl_cbar_en(g_dpu_reg, HAL_DISP_OFFSET, enable);
}

td_bool dpu_hal_disp_get_vblank(void)
{
    return dpu_reg_get_dhd0_state_vblank(g_dpu_reg, HAL_DISP_OFFSET);
}

td_void dpu_hal_disp_get_chksum(td_u32 *chksum_y, td_u32 *chksum_u, td_u32 *chksum_v)
{
    *chksum_y = dpu_reg_get_dhd0_intf_chksum_y_check_sum(g_dpu_reg, HAL_DISP_OFFSET);
    *chksum_u = dpu_reg_get_dhd0_intf_chksum_u_check_sum(g_dpu_reg, HAL_DISP_OFFSET);
    *chksum_v = dpu_reg_get_dhd0_intf_chksum_v_check_sum(g_dpu_reg, HAL_DISP_OFFSET);
}


/* ======================LAYER INTERFACE CALLING============== */

#define HAL_LAYER_CTRL_OFFSET  0x200  /* g0_ctrl:0x7000  g1_ctrl:0x7800   g2_ctrl:0x8000  */
#define HAL_LAYER_FDR_OFFSET   0x80   /* g0_read:0x10e00 g1_read:0x11000  g2_read:0x11200 */

td_void dpu_hal_layer_set_enable(td_u32 layer_id, td_bool show)
{
    dpu_reg_set_g0_ctrl_surface_en(g_dpu_reg, HAL_LAYER_CTRL_OFFSET * layer_id, show);
}

td_void dpu_hal_layer_set_global_alpha(td_u32 layer_id, td_u8 global_alpha)
{
    dpu_reg_set_g0_ctrl_galpha(g_dpu_reg, HAL_LAYER_CTRL_OFFSET * layer_id, global_alpha);
}

td_void dpu_hal_layer_set_premulti_en(td_u32 layer_id, td_bool premulti_en)
{
    dpu_reg_set_gfx_out_ctrl_premulti_en(g_dpu_reg, HAL_LAYER_CTRL_OFFSET * layer_id, premulti_en);
}

td_void dpu_hal_layer_set_disp_rect(td_u32 layer_id, const ext_rect *disp_rect)
{
    td_u32 x_start = (td_u32)disp_rect->x;
    td_u32 x_end   = (td_u32)(disp_rect->x + disp_rect->width - 1);
    td_u32 y_start = (td_u32)disp_rect->y;
    td_u32 y_end   = (td_u32)(disp_rect->y + disp_rect->height - 1);

    dpu_reg_set_g0_dfpos_disp_xfpos(g_dpu_reg, HAL_LAYER_CTRL_OFFSET * layer_id, x_start);
    dpu_reg_set_g0_dlpos_disp_xlpos(g_dpu_reg, HAL_LAYER_CTRL_OFFSET * layer_id, x_end);
    dpu_reg_set_g0_dfpos_disp_yfpos(g_dpu_reg, HAL_LAYER_CTRL_OFFSET * layer_id, y_start);
    dpu_reg_set_g0_dlpos_disp_ylpos(g_dpu_reg, HAL_LAYER_CTRL_OFFSET * layer_id, y_end);

    dpu_reg_set_g0_vfpos_video_xfpos(g_dpu_reg, HAL_LAYER_CTRL_OFFSET * layer_id, x_start);
    dpu_reg_set_g0_vlpos_video_xlpos(g_dpu_reg, HAL_LAYER_CTRL_OFFSET * layer_id, x_end);
    dpu_reg_set_g0_vfpos_video_yfpos(g_dpu_reg, HAL_LAYER_CTRL_OFFSET * layer_id, y_start);
    dpu_reg_set_g0_vlpos_video_ylpos(g_dpu_reg, HAL_LAYER_CTRL_OFFSET * layer_id, y_end);
}

td_void dpu_hal_layer_set_reg_up(td_u32 layer_id)
{
    dpu_reg_set_g0_upd_regup(g_dpu_reg, HAL_LAYER_CTRL_OFFSET * layer_id, 1);
}

td_void dpu_hal_layer_set_ck_gt_en(td_u32 layer_id, td_bool enable)
{
    dpu_reg_set_gfx_read_ctrl_fdr_ck_gt_en(g_dpu_reg, HAL_LAYER_FDR_OFFSET * layer_id, enable);
}

td_void dpu_hal_layer_set_palpha_en(td_u32 layer_id, td_bool enable)
{
    dpu_reg_set_gfx_out_ctrl_palpha_en(g_dpu_reg, HAL_LAYER_FDR_OFFSET * layer_id, enable);
}

td_void dpu_hal_layer_set_fdr_chksum_en(td_u32 layer_id, td_bool chksum_en)
{
    dpu_reg_set_gfx_debug_ctrl_checksum_en(g_dpu_reg, HAL_LAYER_FDR_OFFSET * layer_id, chksum_en);
}

td_void dpu_hal_layer_get_fdr_chksum(td_u32 layer_id, td_u32 *chksum_ar, td_u32 *chksum_gb)
{
    *chksum_ar = dpu_reg_get_gfx_in_ar_checksum1_ar_checksum1(g_dpu_reg, HAL_LAYER_FDR_OFFSET * layer_id);
    *chksum_gb = dpu_reg_get_gfx_in_gb_checksum1_gb_checksum1(g_dpu_reg, HAL_LAYER_FDR_OFFSET * layer_id);
}

td_void dpu_hal_layer_set_crop_rect(td_u32 layer_id, const ext_rect *crop_rect)
{
    dpu_reg_set_gfx_src_crop_src_crop_x(g_dpu_reg, HAL_LAYER_FDR_OFFSET * layer_id, (td_u32)crop_rect->x);
    dpu_reg_set_gfx_src_crop_src_crop_y(g_dpu_reg, HAL_LAYER_FDR_OFFSET * layer_id, (td_u32)crop_rect->y);

    dpu_reg_set_gfx_ireso_ireso_h(g_dpu_reg, HAL_LAYER_FDR_OFFSET * layer_id, crop_rect->height - 1);
    dpu_reg_set_gfx_ireso_ireso_w(g_dpu_reg, HAL_LAYER_FDR_OFFSET * layer_id, crop_rect->width - 1);
}

td_void dpu_hal_layer_set_alpha0_alpha1(td_u32 layer_id, td_u8 alpha0, td_u8 alpha1)
{
    dpu_reg_set_gfx_1555_alpha_alpha_0(g_dpu_reg, HAL_LAYER_FDR_OFFSET * layer_id, alpha0);
    dpu_reg_set_gfx_1555_alpha_alpha_1(g_dpu_reg, HAL_LAYER_FDR_OFFSET * layer_id, alpha1);
}

td_void dpu_hal_layer_set_yuv_base_info(td_u32 layer_id, dpu_hal_yuv_info *yuv_info)
{
    dpu_reg_set_gfx_addr_l_c_gfx_addr_l_c(g_dpu_reg, HAL_LAYER_FDR_OFFSET * layer_id,
                                          yuv_info->y_addr + yuv_info->uv_offset);
    dpu_reg_set_gfx_stride_c_surface_stride_c(g_dpu_reg, HAL_LAYER_FDR_OFFSET * layer_id, yuv_info->uv_stride);

    /* cvfir/hfir/csc ip en control */
    dpu_reg_set_g1_cvfir_vsp_cvfir_en(g_dpu_reg, 0, 1);
    dpu_reg_set_g1_hfir_ctrl_hfir_mode(g_dpu_reg, 0, 1);
    dpu_reg_set_g1_csc_ctrl_csc_en(g_dpu_reg, 0, 1);

    /* cvfir */
    dpu_reg_set_g1_cvfir_vinfo_vzme_ck_gt_en(g_dpu_reg, 0, 0);
    dpu_reg_set_g1_cvfir_vinfo_out_pro(g_dpu_reg, 0, 1);
    dpu_reg_set_g1_cvfir_vinfo_out_fmt(g_dpu_reg, 0, 1);
    dpu_reg_set_g1_cvfir_vinfo_out_height(g_dpu_reg, 0, yuv_info->out_height - 1);
    dpu_reg_set_g1_cvfir_vsp_vratio(g_dpu_reg, 0, 1 << 12); /* 12:vratio */

    dpu_reg_set_g1_cvfir_vcoef0_vccoef00(g_dpu_reg, 0, 0);
    dpu_reg_set_g1_cvfir_vcoef0_vccoef01(g_dpu_reg, 0, 63); /* 63:cvfir ceof */
    dpu_reg_set_g1_cvfir_vcoef0_vccoef02(g_dpu_reg, 0, 0);
    dpu_reg_set_g1_cvfir_vcoef1_vccoef03(g_dpu_reg, 0, 0);
    dpu_reg_set_g1_cvfir_vcoef1_vccoef10(g_dpu_reg, 0, -6); /* -6:cvfir ceof */
    dpu_reg_set_g1_cvfir_vcoef1_vccoef11(g_dpu_reg, 0, 12); /* 12:cvfir ceof */
    dpu_reg_set_g1_cvfir_vcoef2_vccoef12(g_dpu_reg, 0, 51); /* 51:cvfir ceof */
    dpu_reg_set_g1_cvfir_vcoef2_vccoef13(g_dpu_reg, 0, 7);  /* 7:cvfir ceof */

    /* hfir */
    dpu_reg_set_g1_hfir_ctrl_ck_gt_en(g_dpu_reg, 0, 0);
    dpu_reg_set_g1_hfircoef01_coef0(g_dpu_reg, 0, -11);  /* -11:hfir ceof */
    dpu_reg_set_g1_hfircoef01_coef1(g_dpu_reg, 0, 15);   /* 15:hfir ceof */
    dpu_reg_set_g1_hfircoef23_coef2(g_dpu_reg, 0, -20);  /* -20:hfir ceof */
    dpu_reg_set_g1_hfircoef23_coef3(g_dpu_reg, 0, 28);   /* 28:hfir ceof */
    dpu_reg_set_g1_hfircoef45_coef4(g_dpu_reg, 0, -40);  /* -40:hfir ceof */
    dpu_reg_set_g1_hfircoef45_coef5(g_dpu_reg, 0, 61);   /* 61:hfir ceof */
    dpu_reg_set_g1_hfircoef67_coef6(g_dpu_reg, 0, -107); /* -107:hfir ceof */
    dpu_reg_set_g1_hfircoef67_coef7(g_dpu_reg, 0, 330);  /* 330:hfir ceof */

    /* CSC */
    dpu_reg_set_g1_csc_coef00(g_dpu_reg, 0, 1024);  /* 1024:csc ceof */
    dpu_reg_set_g1_csc_coef01(g_dpu_reg, 0, 0);
    dpu_reg_set_g1_csc_coef02(g_dpu_reg, 0, 1576); /* 1576:csc ceof */
    dpu_reg_set_g1_csc_coef10(g_dpu_reg, 0, 1024); /* 1024:csc ceof */
    dpu_reg_set_g1_csc_coef11(g_dpu_reg, 0, -187); /* -187:csc ceof */
    dpu_reg_set_g1_csc_coef12(g_dpu_reg, 0, -470); /* -470:csc ceof */
    dpu_reg_set_g1_csc_coef20(g_dpu_reg, 0, 1024); /* 1024:csc ceof */
    dpu_reg_set_g1_csc_coef21(g_dpu_reg, 0, 1859); /* 1859:csc ceof */
    dpu_reg_set_g1_csc_coef22(g_dpu_reg, 0, 0);

    dpu_reg_set_g1_csc_scale(g_dpu_reg, 0, 10);  /* 10:csc scale */

    dpu_reg_set_g1_csc_idc0(g_dpu_reg, 0, -64);  /* -64:idc index */
    dpu_reg_set_g1_csc_idc1(g_dpu_reg, 0, -512); /* -512:idc index */
    dpu_reg_set_g1_csc_idc2(g_dpu_reg, 0, -512); /* -512:idc index */
    dpu_reg_set_g1_csc_odc0(g_dpu_reg, 0, 0);
    dpu_reg_set_g1_csc_odc1(g_dpu_reg, 0, 0);
    dpu_reg_set_g1_csc_odc2(g_dpu_reg, 0, 0);

    dpu_reg_set_g1_csc_min_y(g_dpu_reg, 0, 0);
    dpu_reg_set_g1_csc_min_c(g_dpu_reg, 0, 0);
    dpu_reg_set_g1_csc_max_y(g_dpu_reg, 0, 4095); /* 4095:max y */
    dpu_reg_set_g1_csc_max_c(g_dpu_reg, 0, 4095); /* 4095:max c */
}

td_void dpu_hal_layer_set_fmt(td_u32 layer_id, dpu_hal_fmt fmt)
{
    dpu_reg_set_gfx_src_info_ifmt(g_dpu_reg, HAL_LAYER_FDR_OFFSET * layer_id, fmt);

    if (layer_id == 1) { /* only G1 valid */
        if (fmt == DPU_HAL_FMT_YUV420SP) {
            dpu_reg_set_link_ctrl_format_link(g_dpu_reg, HAL_DISP_OFFSET, TD_TRUE);
        } else {
            dpu_reg_set_link_ctrl_format_link(g_dpu_reg, HAL_DISP_OFFSET, TD_FALSE);
        }
    }

    /* dcmp rgb/argb mode config */
    if (fmt == DPU_HAL_FMT_RGB888) {
        dpu_reg_set_gfx_dcmp_ctrl_dcmp_fmt(g_dpu_reg, HAL_LAYER_FDR_OFFSET * layer_id, 1);
        dpu_reg_set_gfx_htc_dcmp_pix_format(g_dpu_reg, HAL_LAYER_FDR_OFFSET * layer_id, 0);
    } else {
        dpu_reg_set_gfx_dcmp_ctrl_dcmp_fmt(g_dpu_reg, HAL_LAYER_FDR_OFFSET * layer_id, 0);
        dpu_reg_set_gfx_htc_dcmp_pix_format(g_dpu_reg, HAL_LAYER_FDR_OFFSET * layer_id, 1);
    }
}

td_void dpu_hal_layer_set_width(td_u32 layer_id, td_u32 width)
{
    dpu_reg_set_gfx_src_reso_src_w(g_dpu_reg, HAL_LAYER_FDR_OFFSET * layer_id, width - 1);
}

td_void dpu_hal_layer_set_height(td_u32 layer_id, td_u32 height)
{
    dpu_reg_set_gfx_src_reso_src_h(g_dpu_reg, HAL_LAYER_FDR_OFFSET * layer_id, height - 1);
}

td_void dpu_hal_layer_set_stride(td_u32 layer_id, td_u32 stride)
{
    dpu_reg_set_gfx_stride_surface_stride(g_dpu_reg, HAL_LAYER_FDR_OFFSET * layer_id, stride);
}

td_void dpu_hal_layer_set_addr(td_u32 layer_id, td_u32 addr)
{
    dpu_reg_set_gfx_addr_l_gfx_addr_l(g_dpu_reg, HAL_LAYER_FDR_OFFSET * layer_id, addr);
    dpu_reg_set_gfx_dcmp_addr_l_dcmp_addr_l(g_dpu_reg, HAL_LAYER_FDR_OFFSET * layer_id, addr);
}

td_void dpu_hal_layer_set_dcmp(td_u32 layer_id, td_bool dcmp)
{
    td_u32 cmp_mode = dcmp ? 0 : 1;
    td_u32 ice_en   = dcmp ? 1 : 0;

    dpu_reg_set_gfx_dcmp_ctrl_cmp_mode(g_dpu_reg, HAL_LAYER_FDR_OFFSET * layer_id, cmp_mode);
    dpu_reg_set_gfx_dcmp_ctrl_ice_en(g_dpu_reg, HAL_LAYER_FDR_OFFSET * layer_id, ice_en);

    dpu_reg_set_gfx_htc_dcmp_cmp_mode(g_dpu_reg, HAL_LAYER_FDR_OFFSET * layer_id, cmp_mode);
    dpu_reg_set_gfx_htc_dcmp_ice_en(g_dpu_reg, HAL_LAYER_FDR_OFFSET * layer_id, ice_en);
}

td_void dpu_hal_layer_set_alpha_bypass(td_u32 layer_id, td_bool alpha_bypass)
{
    dpu_reg_set_gfx_dcmp_ctrl_alpha_bypass(g_dpu_reg, HAL_LAYER_FDR_OFFSET * layer_id, alpha_bypass);
    dpu_reg_set_gfx_htc_dcmp_is_a_bypass(g_dpu_reg, HAL_LAYER_FDR_OFFSET * layer_id, alpha_bypass);
}

td_void dpu_hal_layer_set_colorkey(td_u32 layer_id, td_bool key_en, td_u32 key_value)
{
    td_u32 key_r, key_g, key_b;
    const td_u32 default_mode = 0;
    const td_u32 default_mask = 0xff;

    key_r = (key_value & 0xFF0000) >> 16; /* 16:index */
    key_g = (key_value & 0xFF00) >> 8;    /* 8:index */
    key_b = (key_value & 0xFF);

    dpu_reg_set_gfx_out_ctrl_key_en(g_dpu_reg, HAL_LAYER_FDR_OFFSET * layer_id, key_en);
    dpu_reg_set_gfx_out_ctrl_key_mode(g_dpu_reg, HAL_LAYER_FDR_OFFSET * layer_id, default_mode);

    dpu_reg_set_gfx_ckey_min_key_r_min(g_dpu_reg, HAL_LAYER_FDR_OFFSET * layer_id, key_r);
    dpu_reg_set_gfx_ckey_max_key_r_max(g_dpu_reg, HAL_LAYER_FDR_OFFSET * layer_id, key_r);

    dpu_reg_set_gfx_ckey_min_key_g_min(g_dpu_reg, HAL_LAYER_FDR_OFFSET * layer_id, key_g);
    dpu_reg_set_gfx_ckey_max_key_g_max(g_dpu_reg, HAL_LAYER_FDR_OFFSET * layer_id, key_g);

    dpu_reg_set_gfx_ckey_min_key_b_min(g_dpu_reg, HAL_LAYER_FDR_OFFSET * layer_id, key_b);
    dpu_reg_set_gfx_ckey_max_key_b_max(g_dpu_reg, HAL_LAYER_FDR_OFFSET * layer_id, key_b);

    dpu_reg_set_gfx_ckey_mask_key_r_msk(g_dpu_reg, HAL_LAYER_FDR_OFFSET * layer_id, default_mask);
    dpu_reg_set_gfx_ckey_mask_key_g_msk(g_dpu_reg, HAL_LAYER_FDR_OFFSET * layer_id, default_mask);
    dpu_reg_set_gfx_ckey_mask_key_b_msk(g_dpu_reg, HAL_LAYER_FDR_OFFSET * layer_id, default_mask);
}

td_void dpu_hal_layer_set_testpattern(td_u32 layer_id, td_bool on)
{
    dpu_reg_set_gfx_out_ctrl_testpattern_en(g_dpu_reg, HAL_LAYER_FDR_OFFSET * layer_id, on);
}

td_void dpu_hal_layer_set_mute_en(td_u32 layer_id, td_bool mute_en)
{
    dpu_reg_set_gfx_read_ctrl_mute_en(g_dpu_reg, HAL_LAYER_FDR_OFFSET * layer_id, mute_en);
    dpu_reg_set_gfx_mute_alpha_mute_alpha(g_dpu_reg, HAL_LAYER_FDR_OFFSET * layer_id, 0xff);
}

td_void dpu_hal_layer_set_mute_color(td_u32 layer_id, td_u32 mute_y, td_u32 mute_u, td_u32 mute_v)
{
    dpu_reg_set_gfx_mute_bk_mute_y(g_dpu_reg, HAL_LAYER_FDR_OFFSET * layer_id, mute_y);
    dpu_reg_set_gfx_mute_bk_mute_cb(g_dpu_reg, HAL_LAYER_FDR_OFFSET * layer_id, mute_u);
    dpu_reg_set_gfx_mute_bk_mute_cr(g_dpu_reg, HAL_LAYER_FDR_OFFSET * layer_id, mute_v);
}

/* =======FOR INTERFACE CALL====== */
td_void dpu_hal_crg_init(void)
{
    dpu_crg_reset(TD_TRUE);

    /* brandy aon/apb need config first, ano also influent vau */
    dpu_crg_set_aon_ctrl_video_cken(TD_TRUE);
    dpu_crg_set_dpu_apb_cken(TD_TRUE);

    dpu_crg_set_vedio_mclk_div_ctl(TD_TRUE);
    dpu_crg_set_video_160m_cken(TD_TRUE);
    dpu_crg_set_vedio_dpu_ppc_hd0_ckdiv(4); /* 4: 147M * 4/16 > 20M */

    dpu_crg_set_dpu_ppc_hd0_cken(1);
    dpu_crg_set_dpu_cfg_cken(1);
    dpu_crg_set_dpu_hd0_cken(1);
    dpu_crg_set_dpu_axi_cken(1);
}

td_void dpu_hal_crg_deinit(void)
{
    dpu_crg_set_dpu_ppc_hd0_cken(0);
    dpu_crg_set_dpu_cfg_cken(0);
    dpu_crg_set_dpu_hd0_cken(0);
    dpu_crg_set_dpu_axi_cken(0);

    dpu_crg_set_vedio_mclk_div_ctl(TD_FALSE);
    dpu_crg_set_video_160m_cken(TD_FALSE);
    dpu_crg_set_dpu_apb_cken(TD_FALSE);
}

td_void dpu_hal_crg_set_chnl_clk(td_u32 div)
{
    uapi_unused(div);

    return;
}

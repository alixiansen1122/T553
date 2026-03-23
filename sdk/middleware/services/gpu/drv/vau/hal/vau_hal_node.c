/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: vau hal node ctrl file
 */

#include "vau_dfx.h"
#include "vau_hal_node.h"

#define VAU_FLOAT_TRANS_SHIFT 8
#ifdef CONFIG_VAU_DFX_DEBUG
#define vau_print_node_member(member) do {                      \
    if ((member).u32 != 0) {                                    \
        vau_print("(%-40s): 0x%08x\n", #member, (member).u32); \
    }                                                           \
} while (0)

static td_void vau_hal_print_path(const vau_hardware_node *node)
{
    if (vau_dfx_check_mask(VAU_DBG_MASK_PRINT_PATH) == TD_FALSE) {
        return;
    }
    td_u8 *cmds = (td_u8 *)(uintptr_t)node->vau_draw_cmd_addr.u32;
    td_float *datas = (td_float *)(uintptr_t)node->vau_draw_path_addr.u32;

    td_u32 data_index = 0;
    const char name[] = { 'Z', 'z', 'M', 'm', 'L', 'l', 'H', 'h', 'V', 'v', 'Q', 'q', 'C', 'c', 'T', 't', 'S', 's' };
    const td_u8 count[] = { 0, 0, 2, 2, 2, 2, 1, 1, 1, 1, 4, 4, 6, 6, 2, 2, 4, 4 };
    const td_u8 max_cmd_index = sizeof(name) / sizeof(name[0]);

    vau_dfx_print_path(cmds, datas, node->vau_draw_path_num.bits.cmd_num, node->vau_draw_path_num.bits.data_num);

    vau_print("<g transform=\"matrix(%d %d %d %d %d %d)\">\n",
        (td_s32)node->vau_draw_matrix_00.bits.matrix_00, (td_s32)node->vau_draw_matrix_10.bits.matrix_10,
        (td_s32)node->vau_draw_matrix_01.bits.matrix_01, (td_s32)node->vau_draw_matrix_11.bits.matrix_11,
        (td_s32)node->vau_draw_matrix_02.bits.matrix_02, (td_s32)node->vau_draw_matrix_12.bits.matrix_12);

    vau_print("<path d=\"");
    for (td_u32 i = 0; i < node->vau_draw_path_num.bits.cmd_num; i++) {
        if (cmds[i] >= max_cmd_index) {
            vau_err("invalid cmd at %d: %d", i, cmds[i]);
            return;
        }
        vau_print("%c ", name[cmds[i]]);
        for (td_u32 j = 0; j < count[cmds[i]]; j++) {
            if (data_index >= node->vau_draw_path_num.bits.data_num) {
                vau_err("data num not enough at cmd %d: %d, data index:%d", i, cmds[i], data_index);
                return;
            }
            vau_print("%d.%d ", (td_s32)datas[data_index], fractional_part(datas[data_index]));
            data_index++;
        }
    }

    td_u32 color = ((node->vau_draw_ctrl.bits.fill_effect == HAL_VAU_FILL_EFFECT_PURE_COLOR) ||
        (node->vau_draw_ctrl.bits.fill_effect == HAL_VAU_FILL_EFFECT_PURE_COLOR)) ?
        node->mix.gradient.vau_gradient_typical_color.u32 : 0xFFFFFF00;
    td_float alpha = ((color >> 24) & 0xFF) / 255.0f; /* 24: alpha; / 255.0f to normalized */
    vau_print("\" fill=#%06x opacity=%d.%d fill-rule=%u />\n", color & 0xFFFFFF,
              (td_u32)alpha, fractional_part(alpha),
              node->vau_draw_ctrl.bits.fill_mode);
    vau_print("</g>\n");
}

static td_void vau_hal_print_mix_gradient_nozero(const u_vau_mix_node_gradient *gradient)
{
    vau_print_node_member(gradient->vau_gradient_typical_color);
    vau_print_node_member(gradient->vau_gradient_linear_const_x);
    vau_print_node_member(gradient->vau_gradient_linear_const_y);
    vau_print_node_member(gradient->vau_gradient_linear_start);
    vau_print_node_member(gradient->vau_gradient_const_mode);
    vau_print_node_member(gradient->vau_gradient_radial_const_r);
    vau_print_node_member(gradient->vau_gradient_radial_center);
    vau_print_node_member(gradient->vau_gradient_sweep_center);
    vau_print_node_member(gradient->vau_gradient_sweep_start_angle);
    vau_print_node_member(gradient->vau_gradient_tile_mode);
    vau_print_node_member(gradient->vau_gradient_stops_offset0);
    vau_print_node_member(gradient->vau_gradient_stops_offset1);
    vau_print_node_member(gradient->vau_gradient_stops_num);
    vau_print_node_member(gradient->vau_gradient_stops_color[0]);
    vau_print_node_member(gradient->vau_gradient_stops_color[1]);
    vau_print_node_member(gradient->vau_gradient_stops_color[2]);
    vau_print_node_member(gradient->vau_gradient_stops_color[3]);
    vau_print_node_member(gradient->vau_gradient_stops_color[4]);
    vau_print_node_member(gradient->vau_gradient_stops_color[5]);
    vau_print_node_member(gradient->vau_gradient_stops_color[6]);
    vau_print_node_member(gradient->vau_gradient_color_reci[0]);
    vau_print_node_member(gradient->vau_gradient_color_reci[1]);
    vau_print_node_member(gradient->vau_gradient_color_reci[2]);
    vau_print_node_member(gradient->vau_gradient_color_reci[3]);
    vau_print_node_member(gradient->vau_gradient_color_reci[4]);
    vau_print_node_member(gradient->vau_gradient_color_reci[5]);
}

static td_void vau_hal_print_draw_nozero(const vau_hardware_node *node)
{
    vau_print_node_member(node->vau_draw_ctrl);
    vau_print_node_member(node->vau_draw_st);
    vau_print_node_member(node->vau_draw_end);
    vau_print_node_member(node->vau_draw_cmd_addr);
    vau_print_node_member(node->vau_draw_path_addr);
    vau_print_node_member(node->vau_draw_et_addr);
    vau_print_node_member(node->vau_draw_link_start_addr);
    vau_print_node_member(node->vau_draw_link_end_addr);
    vau_print_node_member(node->vau_draw_et_bk_addr);
    vau_print_node_member(node->vau_draw_path_num);
    vau_print_node_member(node->vau_draw_matrix_00);
    vau_print_node_member(node->vau_draw_matrix_01);
    vau_print_node_member(node->vau_draw_matrix_02);
    vau_print_node_member(node->vau_draw_matrix_10);
    vau_print_node_member(node->vau_draw_matrix_11);
    vau_print_node_member(node->vau_draw_matrix_12);
    vau_print_node_member(node->vau_draw_et_bkend_addr);

    if (node->vau_draw_ctrl.bits.draw_en) {
        vau_hal_print_mix_gradient_nozero(&node->mix.gradient);
        vau_hal_print_path(node);
    }
}

static td_void vau_hal_print_mix_src2_nozero(const u_vau_mix_node_src2 *src2)
{
    vau_print_node_member(src2->vau_src2_crop_xy);
    vau_print_node_member(src2->vau_src2_crop_wh);
    vau_print_node_member(src2->vau_t2r_ctrl);
    vau_print_node_member(src2->vau_matrix_ctrl);
    vau_print_node_member(src2->vau_matrix_coef00);
    vau_print_node_member(src2->vau_matrix_coef01);
    vau_print_node_member(src2->vau_matrix_coef02);
    vau_print_node_member(src2->vau_matrix_coef10);
    vau_print_node_member(src2->vau_matrix_coef11);
    vau_print_node_member(src2->vau_matrix_coef12);
    vau_print_node_member(src2->vau_matrix_coef20);
    vau_print_node_member(src2->vau_matrix_coef21);
    vau_print_node_member(src2->vau_matrix_coef22);
    vau_print_node_member(src2->vau_matrix_crop_xy);
    vau_print_node_member(src2->vau_matrix_crop_wh);
    vau_print_node_member(src2->vau_matrix_pattern_fill_color);
    vau_print_node_member(src2->vau_gauss_filter_radius0);
    vau_print_node_member(src2->vau_gauss_filter_radius1);
    vau_print_node_member(src2->vau_gauss_filter_radius2);
    vau_print_node_member(src2->vau_gauss_filter_weight0);
    vau_print_node_member(src2->vau_gauss_filter_weight1);
    vau_print_node_member(src2->vau_gauss_filter_weight2);
}

static td_void vau_hal_print_cbm_info(const vau_hardware_node *node)
{
    vau_print_node_member(node->vau_cbm_ctrl);
    vau_print_node_member(node->vau_cbm_colorize);
    vau_print_node_member(node->vau_cbm_alu_para);
    vau_print_node_member(node->vau_cbm_key_para);
    vau_print_node_member(node->vau_cbm_key_min);
    vau_print_node_member(node->vau_cbm_key_max);
    vau_print_node_member(node->vau_cbm_key_mask);
    vau_print_node_member(node->vau_cbm_src1_cbm_para);
    vau_print_node_member(node->vau_cbm_src2_cbm_para);
}

td_void vau_hal_print_node_nozero(const vau_hardware_node *node)
{
    vau_print_node_member(node->vau_src1_ctrl);
    vau_print_node_member(node->vau_src1_addr);
    vau_print_node_member(node->vau_src1_stride);
    vau_print_node_member(node->vau_src1_imgsize);
    vau_print_node_member(node->vau_src1_rect_xy);
    vau_print_node_member(node->vau_src1_rect_wh);
    vau_print_node_member(node->vau_src1_fill);
    vau_print_node_member(node->vau_src1_alpha);
    vau_print_node_member(node->vau_src1_pix_rgb);
    vau_print_node_member(node->vau_src2_ctrl);
    vau_print_node_member(node->vau_src2_addr);
    vau_print_node_member(node->vau_src2_stride);
    vau_print_node_member(node->vau_src2_imgsize);
    vau_print_node_member(node->vau_src2_rect_xy);
    vau_print_node_member(node->vau_src2_rect_wh);
    vau_print_node_member(node->vau_src2_fill);
    vau_print_node_member(node->vau_src2_alpha);
    vau_print_node_member(node->vau_src2_pix_rgb);
    vau_print_node_member(node->vau_src2_dcmp_cfg0);
    vau_print_node_member(node->vau_src2_dcmp_cfg1);
    vau_print_node_member(node->vau_src2_dcmp_cfg2);
    vau_print_node_member(node->vau_src2_dcmp_cfg3);
    vau_print_node_member(node->vau_des_ctrl);
    vau_print_node_member(node->vau_des_addr);
    vau_print_node_member(node->vau_des_stride);
    vau_print_node_member(node->vau_des_imgsize);
    vau_print_node_member(node->vau_des_rect_xy);
    vau_print_node_member(node->vau_des_rect_wh);
    vau_print_node_member(node->vau_des_alpha);
    vau_print_node_member(node->vau_src1_crop_xy);
    vau_print_node_member(node->vau_src1_crop_wh);
    vau_print_node_member(node->vau_clut_addr);
    vau_print_node_member(node->vau_node_id);
    vau_print_node_member(node->vau_intmask);
    vau_print_node_member(node->vau_pnext);

    vau_hal_print_cbm_info(node);
    vau_hal_print_draw_nozero(node);

    if (node->vau_draw_ctrl.bits.draw_en == TD_FALSE) {
        vau_hal_print_mix_src2_nozero(&node->mix.src2);
    }

    vau_print("--------------------------------\n");
}

td_void vau_hal_print_node_all(const vau_hardware_node *node, td_u32 start, td_u32 end, td_u32 print_offset)
{
    td_u32 real_end = (td_u32)node + end;
    td_u32 addr = print_offset;
    for (td_u32 cur = (td_u32)node + start; cur <= real_end; cur += 0x4, addr += 0x4) {
        vau_print("%08x  %08x\n", addr, *((td_u32*)cur));
    }
    if (node->vau_draw_ctrl.bits.draw_en) {
        vau_hal_print_path(node);
    }
}
#endif

td_void vau_hal_node_set_src1_base_info(vau_hardware_node *node, const hal_vau_surface *surface)
{
    node->vau_src1_ctrl.bits.src1_en            = 1;
    node->vau_src1_ctrl.bits.src1_fmt           = surface->fmt;
    node->vau_src1_ctrl.bits.src1_argb_order    = surface->argb_order;
    node->vau_src1_ctrl.bits.src1_rgb_exp       = HAL_VAU_RGB_EXP_HIGH_BITS;
    node->vau_src1_ctrl.bits.src1_alpha_range   = surface->alpha_info.is_alpha_max_128;

    node->vau_src1_addr.bits.src1_addr          = surface->addr;
    node->vau_src1_stride.bits.src1_stride      = surface->stride;
    node->vau_src1_imgsize.bits.src1_width      = surface->width - 1;
    node->vau_src1_imgsize.bits.src1_height     = surface->height - 1;

    if (surface->is_color_surface == TD_TRUE) {
        node->vau_src1_fill.bits.src1_color_fill = surface->color;
    }

    /* fdr crop */
    node->vau_src1_rect_xy.bits.src1_rect_x      = (td_u32)surface->rect.x;
    node->vau_src1_rect_xy.bits.src1_rect_y      = (td_u32)surface->rect.y;
    node->vau_src1_rect_wh.bits.src1_rect_width  = surface->rect.width - 1;
    node->vau_src1_rect_wh.bits.src1_rect_height = surface->rect.height - 1;

    if (surface->fmt == HAL_VAU_FMT_A1 || surface->fmt == HAL_VAU_FMT_A1B) {
        node->vau_src1_pix_rgb.bits.src1_pix_rgb = surface->color;
        node->vau_src1_alpha.bits.src1_alpha0 = 0x00; /* decide A1 alpha pix=0 */
        node->vau_src1_alpha.bits.src1_alpha1 = 0xff; /* decide A1 alpha pix=1 */
    } else if (surface->fmt == HAL_VAU_FMT_A8) {
        node->vau_src1_pix_rgb.bits.src1_pix_rgb = surface->color;
    }

    return;
}

td_void vau_hal_node_set_src1_mode(vau_hardware_node *node, hal_vau_src_mode mode)
{
    node->vau_src1_ctrl.bits.src1_mode = mode;
}

td_void vau_hal_node_set_src1_alpha(vau_hardware_node *node, const hal_vau_surface *surface)
{
    if (surface->fmt == HAL_VAU_FMT_ARGB1555) {
        node->vau_src1_alpha.bits.src1_alpha0 = surface->alpha_info.alpha0;
        node->vau_src1_alpha.bits.src1_alpha1 = surface->alpha_info.alpha1;
    }

    /* s1_blend_mode config in cbm */
    node->vau_cbm_src1_cbm_para.bits.s1_multi_global_en = surface->alpha_info.golbal_premult_en;
    node->vau_cbm_src1_cbm_para.bits.s1_premult_en      = surface->alpha_info.premult_en;
    node->vau_cbm_src1_cbm_para.bits.s1_palpha_en       = surface->alpha_info.pixel_alpha_en;
    node->vau_cbm_src1_cbm_para.bits.s1_galpha_en       = surface->alpha_info.global_alpha_en;
}

td_void vau_hal_node_set_src1_crop(vau_hardware_node *node, const ext_rect *rect)
{
    node->vau_src1_crop_xy.bits.src1_crop_en     = 1;
    node->vau_src1_crop_xy.bits.src1_crop_x      = (td_u32)rect->x;
    node->vau_src1_crop_xy.bits.src1_crop_y      = (td_u32)rect->y;
    node->vau_src1_crop_wh.bits.src1_crop_width  = rect->width;
    node->vau_src1_crop_wh.bits.src1_crop_height = rect->height;
}

td_void vau_hal_node_set_src2_base_info(vau_hardware_node *node, const hal_vau_surface *surface)
{
    node->vau_src2_ctrl.bits.src2_en            = 1;
    node->vau_src2_ctrl.bits.src2_fmt           = surface->fmt;
    node->vau_src2_ctrl.bits.src2_argb_order    = surface->argb_order;
    node->vau_src2_ctrl.bits.src2_rgb_exp       = HAL_VAU_RGB_EXP_HIGH_BITS;
    node->vau_src2_ctrl.bits.src2_alpha_range   = surface->alpha_info.is_alpha_max_128;

    node->vau_src2_addr.bits.src2_addr          = surface->addr;
    node->vau_src2_stride.bits.src2_stride      = surface->stride;
    node->vau_src2_imgsize.bits.src2_width      = surface->width - 1;
    node->vau_src2_imgsize.bits.src2_height     = surface->height - 1;

    if (surface->is_color_surface == TD_TRUE) {
        node->vau_src2_fill.bits.src2_color_fill = surface->color;
    }

    /* fdr crop */
    node->vau_src2_rect_xy.bits.src2_rect_x      = (td_u16)surface->rect.x;
    node->vau_src2_rect_xy.bits.src2_rect_y      = (td_u16)surface->rect.y;
    node->vau_src2_rect_wh.bits.src2_rect_width  = surface->rect.width - 1;
    node->vau_src2_rect_wh.bits.src2_rect_height = surface->rect.height - 1;

    /* cmp info config */
    if (surface->compress_mode != HAL_VAU_COMPRESS_NONE) {
        td_s32 ret;
        node->vau_src2_ctrl.bits.src2_ice_en       = 1;
        ret = memcpy_s(&node->vau_src2_dcmp_cfg0, sizeof(node->vau_src2_dcmp_cfg0) * HAL_VAU_CMP_INFO_CNT,
                       surface->compress_header, sizeof(surface->compress_header));
        if (ret != EOK) {
            vau_err("compress_info copy fail!\n");
        }
    }

    if (surface->fmt == HAL_VAU_FMT_A1 || surface->fmt == HAL_VAU_FMT_A1B || surface->fmt == HAL_VAU_FMT_A2 ||
        surface->fmt == HAL_VAU_FMT_A4 || surface->fmt == HAL_VAU_FMT_A2B || surface->fmt == HAL_VAU_FMT_A4B) {
        node->vau_src2_pix_rgb.bits.src2_pix_rgb = surface->color;
        node->vau_src2_alpha.bits.src2_alpha0 = 0x00; /* decide A1 alpha pix=0 */
        node->vau_src2_alpha.bits.src2_alpha1 = 0xff; /* decide A1 alpha pix=1 */
    } else if (surface->fmt == HAL_VAU_FMT_A8) {
        node->vau_src2_pix_rgb.bits.src2_pix_rgb = surface->color;
    }

    return;
}

td_void vau_hal_node_set_src2_mode(vau_hardware_node *node, hal_vau_src_mode mode)
{
    node->vau_src2_ctrl.bits.src2_mode = mode;
}

td_void vau_hal_node_set_src2_alpha(vau_hardware_node *node, const hal_vau_surface *surface)
{
    if (surface->fmt == HAL_VAU_FMT_ARGB1555) {
        node->vau_src2_alpha.bits.src2_alpha0 = surface->alpha_info.alpha0;
        node->vau_src2_alpha.bits.src2_alpha1 = surface->alpha_info.alpha1;
    }

    if (surface->affine_en) {
        /* matrix */
        node->vau_src2_ctrl.bits.src2_premult_en            = surface->alpha_info.premult_en;
    } else {
        /* s2_blend_mode config in cbm */
        node->vau_cbm_src2_cbm_para.bits.s2_premult_en      = surface->alpha_info.premult_en;
    }
    node->vau_cbm_src2_cbm_para.bits.s2_palpha_en       = surface->alpha_info.pixel_alpha_en;
    node->vau_cbm_src2_cbm_para.bits.s2_galpha          = surface->alpha_info.global_alpha;
    node->vau_cbm_src2_cbm_para.bits.s2_multi_global_en = surface->alpha_info.golbal_premult_en;
    node->vau_cbm_src2_cbm_para.bits.s2_galpha_en       = surface->alpha_info.global_alpha_en;
}

td_void vau_hal_node_set_src2_crop(vau_hardware_node *node, const ext_rect *rect)
{
    node->mix.src2.vau_src2_crop_xy.bits.src2_crop_en     = 1;
    node->mix.src2.vau_src2_crop_xy.bits.src2_crop_x      = (td_u32)rect->x;
    node->mix.src2.vau_src2_crop_xy.bits.src2_crop_y      = (td_u32)rect->y;
    node->mix.src2.vau_src2_crop_wh.bits.src2_crop_width  = rect->width;
    node->mix.src2.vau_src2_crop_wh.bits.src2_crop_height = rect->height;
}


td_void vau_hal_node_set_src2_clut(vau_hardware_node *node, const hal_vau_surface *surface)
{
    if ((surface->fmt >= HAL_VAU_FMT_CLUT1) && (surface->fmt <= HAL_VAU_FMT_CLUT4B)) {
        node->vau_clut_addr.u32 = surface->clut_addr;
    }
}

td_void vau_hal_node_set_affine_info(vau_hardware_node *node, hal_vau_affine_cfg *hal_cfg)
{
    node->vau_src2_ctrl.bits.src2_aff_en       = 1;
    node->mix.src2.vau_matrix_ctrl.bits.req_tile_sel = vau_dfx_check_mask(VAU_DBG_MASK_MATRIX_TILE_SEL4X4) ? 0x0 : 0x1;
    node->mix.src2.vau_matrix_ctrl.bits.matrix_en    = 1;
    node->mix.src2.vau_matrix_ctrl.bits.transform_mode  = hal_cfg->transform_mode;
    node->mix.src2.vau_matrix_ctrl.bits.alpha_trans_mode = 1;
    node->mix.src2.vau_matrix_ctrl.bits.tile_linear_sel = 0;
    node->mix.src2.vau_matrix_ctrl.bits.pattern_cfg = hal_cfg->pattern_mode;
    node->mix.src2.vau_matrix_pattern_fill_color.u32 = hal_cfg->pattern_argb;

    node->mix.src2.vau_matrix_coef00.bits.matrix_coef00 = hal_cfg->inv_m[0][0];
    node->mix.src2.vau_matrix_coef01.bits.matrix_coef01 = hal_cfg->inv_m[0][1];
    node->mix.src2.vau_matrix_coef02.bits.matrix_coef02 = hal_cfg->inv_m[0][2]; /* 2:index */
    node->mix.src2.vau_matrix_coef10.bits.matrix_coef10 = hal_cfg->inv_m[1][0];
    node->mix.src2.vau_matrix_coef11.bits.matrix_coef11 = hal_cfg->inv_m[1][1];
    node->mix.src2.vau_matrix_coef12.bits.matrix_coef12 = hal_cfg->inv_m[1][2]; /* 2:index */
    node->mix.src2.vau_matrix_coef20.bits.matrix_coef20 = hal_cfg->inv_m[2][0]; /* 2:index */
    node->mix.src2.vau_matrix_coef21.bits.matrix_coef21 = hal_cfg->inv_m[2][1]; /* 2:index */
    node->mix.src2.vau_matrix_coef22.bits.matrix_coef22 = hal_cfg->inv_m[2][2]; /* 2:index */

    return;
}

td_void vau_hal_node_set_src2_t2r(vau_hardware_node *node, hal_vau_t2r_info *info)
{
    node->mix.src2.vau_t2r_ctrl.bits.cch_copy_en  = info->cch_copy_en;
    node->mix.src2.vau_t2r_ctrl.bits.cfg_en       = info->cfg_en;
    node->mix.src2.vau_t2r_ctrl.bits.delay_en     = info->delay_en;
    node->mix.src2.vau_t2r_ctrl.bits.flip_en      = info->flip_en;
    node->mix.src2.vau_t2r_ctrl.bits.t2r_ck_gt_en = info->t2r_ck_gt_en;
    node->mix.src2.vau_t2r_ctrl.bits.t2r_en       = info->t2r_en;
    node->mix.src2.vau_t2r_ctrl.bits.tile_wth     = info->tile_width;
}

td_void vau_hal_node_set_dst_base_info(vau_hardware_node *node, const hal_vau_surface *surface)
{
    node->vau_des_ctrl.bits.des_en             = 1;
    node->vau_des_ctrl.bits.des_fmt            = surface->fmt;
    node->vau_des_ctrl.bits.des_argb_order     = surface->argb_order;
    node->vau_des_ctrl.bits.des_alpha_range    = surface->alpha_info.is_alpha_max_128;

    node->vau_des_addr.bits.des_addr           = surface->addr;
    node->vau_des_stride.bits.des_stride       = surface->stride;
    node->vau_des_imgsize.bits.des_width       = surface->width - 1;
    node->vau_des_imgsize.bits.des_height      = surface->height - 1;

    node->vau_des_alpha.bits.des_alpha_thd     = surface->alpha_info.alpha_thd;

    /* dst rect */
    node->vau_des_rect_xy.bits.des_rect_x      = (td_u32)surface->rect.x;
    node->vau_des_rect_xy.bits.des_rect_y      = (td_u32)surface->rect.y;
    node->vau_des_rect_wh.bits.des_rect_width  = surface->rect.width - 1;
    node->vau_des_rect_wh.bits.des_rect_height = surface->rect.height - 1;
}

td_void vau_hal_node_set_dst_crop(const vau_hardware_node *node, const ext_rect *rect)
{
    uapi_unused(node);
    uapi_unused(rect);

    return;
}

td_void vau_hal_node_set_cbm(vau_hardware_node *node, const ext_rect *cbm_rect, td_u32 alu_mode)
{
    if (node->vau_src1_ctrl.bits.src1_mode == HAL_VAU_MODE_TYPE_DMA) {
        return;
    }
    uapi_unused(cbm_rect);

    node->vau_cbm_ctrl.bits.cbm_en   = 1;
    node->vau_cbm_ctrl.bits.alu_mode = alu_mode;
}

td_void vau_hal_node_set_rop(vau_hardware_node *node, hal_vau_rop_opt *rop_opt)
{
    node->vau_cbm_alu_para.bits.blend_rop_en = rop_opt->rop_enable;
    node->vau_cbm_alu_para.bits.a_rop        = rop_opt->alpha_rop_mode;
    node->vau_cbm_alu_para.bits.rgb_rop      = rop_opt->color_rop_mode;
}

td_void vau_hal_node_set_colorkey(vau_hardware_node *node, hal_vau_colorkey_opt *colorkey_opt)
{
    node->vau_cbm_key_para.bits.key_en  = colorkey_opt->key_en;
    node->vau_cbm_key_para.bits.key_sel = colorkey_opt->key_sel;

    node->vau_cbm_key_para.bits.key_a_mode = colorkey_opt->component[HAL_VAU_COMPONENT_ALPHA].key_mode;
    node->vau_cbm_key_min.bits.key_a_min   = colorkey_opt->component[HAL_VAU_COMPONENT_ALPHA].key_min;
    node->vau_cbm_key_max.bits.key_a_max   = colorkey_opt->component[HAL_VAU_COMPONENT_ALPHA].key_max;
    node->vau_cbm_key_mask.bits.key_a_mask = colorkey_opt->component[HAL_VAU_COMPONENT_ALPHA].key_mask;

    node->vau_cbm_key_para.bits.key_r_mode = colorkey_opt->component[HAL_VAU_COMPONENT_RED].key_mode;
    node->vau_cbm_key_min.bits.key_r_min   = colorkey_opt->component[HAL_VAU_COMPONENT_RED].key_min;
    node->vau_cbm_key_max.bits.key_r_max   = colorkey_opt->component[HAL_VAU_COMPONENT_RED].key_max;
    node->vau_cbm_key_mask.bits.key_r_mask = colorkey_opt->component[HAL_VAU_COMPONENT_RED].key_mask;

    node->vau_cbm_key_para.bits.key_g_mode = colorkey_opt->component[HAL_VAU_COMPONENT_GREEN].key_mode;
    node->vau_cbm_key_min.bits.key_g_min   = colorkey_opt->component[HAL_VAU_COMPONENT_GREEN].key_min;
    node->vau_cbm_key_max.bits.key_g_max   = colorkey_opt->component[HAL_VAU_COMPONENT_GREEN].key_max;
    node->vau_cbm_key_mask.bits.key_g_mask = colorkey_opt->component[HAL_VAU_COMPONENT_GREEN].key_mask;

    node->vau_cbm_key_para.bits.key_b_mode = colorkey_opt->component[HAL_VAU_COMPONENT_BLUE].key_mode;
    node->vau_cbm_key_min.bits.key_b_min   = colorkey_opt->component[HAL_VAU_COMPONENT_BLUE].key_min;
    node->vau_cbm_key_max.bits.key_b_max   = colorkey_opt->component[HAL_VAU_COMPONENT_BLUE].key_max;
    node->vau_cbm_key_mask.bits.key_b_mask = colorkey_opt->component[HAL_VAU_COMPONENT_BLUE].key_mask;
}

td_void vau_hal_node_set_blend_mode(vau_hardware_node *node, td_u32 src1_mode, td_u32 src2_mode)
{
    node->vau_cbm_src1_cbm_para.bits.s1_blend_mode = src1_mode;
    node->vau_cbm_src2_cbm_para.bits.s2_blend_mode = src2_mode;
}

td_void vau_hal_node_set_blend_alpha_mode(vau_hardware_node *node, hal_vau_blend_alpha_mode alpha_from)
{
    node->vau_cbm_alu_para.bits.alpha_from = alpha_from;
}

td_void vau_hal_node_enable_complete_int(vau_hardware_node *node)
{
    node->vau_intmask.bits.node_end_mask = 0x1;
    node->vau_intmask.bits.timeout_mask  = 0x1;
    node->vau_intmask.bits.bus_err_mask  = 0x1;
#ifndef CONFIG_VAU_DEAL_INT_ONCE
    node->vau_intmask.bits.draw_err_mask = 0x1;
    node->vau_intmask.bits.et_oversize_mask = 0x1;
    node->vau_intmask.bits.draw_oversize_mask = 0x1;
    node->vau_intmask.bits.dcmp_cfg_err_mask = 0x1;
    node->vau_intmask.bits.dcmp_bs_err_mask = 0x1;
#else
    node->vau_intmask.bits.once_draw_err_mask = 0x1;
    node->vau_intmask.bits.once_et_oversize_mask = 0x1;
    node->vau_intmask.bits.once_draw_oversize_mask = 0x1;
    node->vau_intmask.bits.once_dcmp_cfg_err_mask = 0x1;
    node->vau_intmask.bits.once_dcmp_bs_err_mask = 0x1;
#endif
}

td_void vau_hal_node_enable_lowdelay_int(const vau_hardware_node *node)
{
    uapi_unused(node);

    return;
}

td_void vau_hal_node_set_tunl_enable(const vau_hardware_node *node)
{
    /* CS lowdelay config */
    uapi_unused(node);

    return;
}

td_void vau_hal_node_add_next_node(vau_hardware_node *node, td_u32 phy_addr)
{
    node->vau_pnext.bits.p_next = phy_addr;
}

static td_void vau_hal_node_set_gradient(vau_hardware_node *node, const hal_vau_vector_cfg *cfg)
{
    node->mix.gradient.vau_gradient_typical_color.u32  = cfg->gradient.color.typical_color;
    node->mix.gradient.vau_gradient_linear_const_x.bits.linear_const_x = cfg->gradient.linear.linear_const_x;
    node->mix.gradient.vau_gradient_linear_const_y.bits.linear_const_y = cfg->gradient.linear.linear_const_y;
    node->mix.gradient.vau_gradient_linear_start.bits.linear_start_x = (td_s16)cfg->gradient.linear.linear_start_x;
    node->mix.gradient.vau_gradient_linear_start.bits.linear_start_y = (td_s16)cfg->gradient.linear.linear_start_y;
    node->mix.gradient.vau_gradient_const_mode.bits.linear_const_mode  = cfg->gradient.linear.linear_const_mode;
    node->mix.gradient.vau_gradient_radial_const_r.bits.radial_const_r = cfg->gradient.radial.radial_const_r;
    node->mix.gradient.vau_gradient_radial_center.bits.radial_center_x = cfg->gradient.radial.radial_center_x;
    node->mix.gradient.vau_gradient_radial_center.bits.radial_center_y = cfg->gradient.radial.radial_center_y;
    node->mix.gradient.vau_gradient_sweep_center.bits.sweep_center_x   = cfg->gradient.sweep.sweep_center_x;
    node->mix.gradient.vau_gradient_sweep_center.bits.sweep_center_y   = cfg->gradient.sweep.sweep_center_y;
    node->mix.gradient.vau_gradient_sweep_start_angle.bits.sweep_start_angle = cfg->gradient.sweep.sweep_start_angle;
    node->mix.gradient.vau_gradient_tile_mode.bits.tile_mode           = cfg->tile_mode;
    node->mix.gradient.vau_gradient_stops_offset0.u32                  = cfg->stops_offset0;
    node->mix.gradient.vau_gradient_stops_offset1.u32                  = cfg->stops_offset1;
    node->mix.gradient.vau_gradient_stops_num.bits.stops_num           = cfg->stops_num;

    if (cfg->stops_num > 0) {
        if (memcpy_s(node->mix.gradient.vau_gradient_stops_color,
                     sizeof(node->mix.gradient.vau_gradient_stops_color),
                     cfg->stops_color, sizeof(cfg->stops_color)) != EOK) {
            vau_err("memcpy stops_color fail!\n");
        }
        if (memcpy_s(node->mix.gradient.vau_gradient_color_reci,
                     sizeof(node->mix.gradient.vau_gradient_color_reci),
                     cfg->color_reci, sizeof(cfg->color_reci)) != EOK) {
            vau_err("memcpy color_reci0 fail!\n");
        }
    }
}

td_void vau_hal_node_set_vector_info(vau_hardware_node *node, hal_vau_vector_cfg *cfg)
{
    node->vau_draw_ctrl.bits.draw_en        = cfg->draw_en;
    node->vau_draw_ctrl.bits.fill_mode      = cfg->fill_mode;
    node->vau_draw_ctrl.bits.fill_effect    = cfg->fill_effect;
    node->vau_draw_ctrl.bits.msaa_mode      = cfg->msaa_mode;
    node->vau_draw_ctrl.bits.dyn_bdbx_en    = cfg->dyn_bdbx_en;
    node->vau_draw_ctrl.bits.cmd_return_en  = cfg->cmd_return_en;
    node->vau_draw_ctrl.bits.path_return_en = cfg->path_return_en;
    node->vau_draw_ctrl.bits.bdbx_adj_en    = 1;

    node->vau_draw_st.bits.draw_x_start     = cfg->draw_start_x;
    node->vau_draw_st.bits.draw_y_start     = cfg->draw_start_y;

    node->vau_draw_end.bits.draw_x_end      = cfg->draw_end_x;
    node->vau_draw_end.bits.draw_y_end      = cfg->draw_end_y;

    node->vau_draw_cmd_addr.u32             = cfg->draw_path_cmd_addr;
    node->vau_draw_path_addr.u32            = cfg->draw_path_data_addr;
    node->vau_draw_path_num.bits.cmd_num    = cfg->draw_path_cmd_cnt;
    node->vau_draw_path_num.bits.data_num   = cfg->draw_path_data_cnt;

    node->vau_draw_matrix_00.bits.matrix_00 = cfg->matrix[0][0];
    node->vau_draw_matrix_01.bits.matrix_01 = cfg->matrix[0][1];
    node->vau_draw_matrix_02.bits.matrix_02 = cfg->matrix[0][2]; /* 2 is index */
    node->vau_draw_matrix_10.bits.matrix_10 = cfg->matrix[1][0];
    node->vau_draw_matrix_11.bits.matrix_11 = cfg->matrix[1][1];
    node->vau_draw_matrix_12.bits.matrix_12 = cfg->matrix[1][2]; /* 2 is index */

    vau_hal_node_set_gradient(node, cfg);

    return;
}

td_void vau_hal_node_set_alpha_strb(vau_hardware_node *node, td_bool enable)
{
    node->vau_des_ctrl.bits.alpha_strb_en = enable;
    return;
}


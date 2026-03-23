/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description : vau hal node ctrl file
 */

#ifndef DRV_GRAPHIC_HAL_VAU_HAL_NODE_H
#define DRV_GRAPHIC_HAL_VAU_HAL_NODE_H

#include "vau_reg_define.h"
#include "vau_hal_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define VAU_CMP_TILE_BS_WIDTH         16
#define VAU_CMP_TILE_BS_HEIGHT        4
#define VAU_GRADIENT_STOPS_COLOR_CNT  7
#define VAU_GRADIENT_COLOR_RECI_CNT   6

typedef struct {
    u_vau_gradient_typical_color         vau_gradient_typical_color;          /* 100 */
    u_vau_gradient_linear_const_x        vau_gradient_linear_const_x;         /* 104 */
    u_vau_gradient_linear_const_y        vau_gradient_linear_const_y;         /* 108 */
    u_vau_gradient_linear_start          vau_gradient_linear_start;           /* 10c */
    u_vau_gradient_const_mode            vau_gradient_const_mode;             /* 110 */
    u_vau_gradient_radial_const_r        vau_gradient_radial_const_r;         /* 114 */
    u_vau_gradient_radial_center         vau_gradient_radial_center;          /* 118 */
    u_vau_gradient_sweep_center          vau_gradient_sweep_center;           /* 11c */
    u_vau_gradient_sweep_start_angle     vau_gradient_sweep_start_angle;      /* 120 */
    u_vau_gradient_tile_mode             vau_gradient_tile_mode;              /* 124 */
    u_vau_gradient_stops_offset0         vau_gradient_stops_offset0;          /* 128 */
    u_vau_gradient_stops_offset1         vau_gradient_stops_offset1;          /* 12c */
    u_vau_gradient_stops_color0          vau_gradient_stops_color[VAU_GRADIENT_STOPS_COLOR_CNT]; /* 130~148 */
    u_vau_gradient_color_reci0           vau_gradient_color_reci[VAU_GRADIENT_COLOR_RECI_CNT];   /* 14c~160 */
    u_vau_gradient_stops_num             vau_gradient_stops_num;              /* 164 */
} u_vau_mix_node_gradient;

typedef struct {
    u_vau_src2_crop_xy                   vau_src2_crop_xy;                    /* 200 */
    u_vau_src2_crop_wh                   vau_src2_crop_wh;                    /* 204 */
    unsigned int                         rsr3[2];                             /* 208 ~ 20c */
    u_vau_t2r_ctrl                       vau_t2r_ctrl;                        /* 210 */
    u_vau_matrix_ctrl                    vau_matrix_ctrl;                     /* 214 */
    u_vau_matrix_coef00                  vau_matrix_coef00;                   /* 218 */
    u_vau_matrix_coef01                  vau_matrix_coef01;                   /* 21c */
    u_vau_matrix_coef02                  vau_matrix_coef02;                   /* 220 */
    u_vau_matrix_coef10                  vau_matrix_coef10;                   /* 224 */
    u_vau_matrix_coef11                  vau_matrix_coef11;                   /* 228 */
    u_vau_matrix_coef12                  vau_matrix_coef12;                   /* 22c */
    u_vau_matrix_coef20                  vau_matrix_coef20;                   /* 230 */
    u_vau_matrix_coef21                  vau_matrix_coef21;                   /* 234 */
    u_vau_matrix_coef22                  vau_matrix_coef22;                   /* 238 */
    u_vau_matrix_crop_xy                 vau_matrix_crop_xy;                  /* 23c */
    u_vau_matrix_crop_wh                 vau_matrix_crop_wh;                  /* 240 */
    u_vau_matrix_pattern_fill_color      vau_matrix_pattern_fill_color;       /* 244 */
    u_vau_gauss_filter_ctrl              vau_gauss_filter_ctrl;               /* 248 */
    u_vau_gauss_filter_radius0           vau_gauss_filter_radius0;            /* 24c */
    u_vau_gauss_filter_radius1           vau_gauss_filter_radius1;            /* 250 */
    u_vau_gauss_filter_radius2           vau_gauss_filter_radius2;            /* 254 */
    u_vau_gauss_filter_weight0           vau_gauss_filter_weight0;            /* 258 */
    u_vau_gauss_filter_weight1           vau_gauss_filter_weight1;            /* 25c */
    u_vau_gauss_filter_weight2           vau_gauss_filter_weight2;            /* 260 */
    unsigned int                         rsr4;                                /* 264 */
} u_vau_mix_node_src2;

typedef union {
    u_vau_mix_node_gradient gradient;
    u_vau_mix_node_src2 src2;
} u_vau_mix_node;

typedef struct {
    u_vau_src1_ctrl                      vau_src1_ctrl;                       /* 0 */
    u_vau_src1_addr                      vau_src1_addr;                       /* 4 */
    u_vau_src1_stride                    vau_src1_stride;                     /* 8 */
    u_vau_src1_imgsize                   vau_src1_imgsize;                    /* c */
    u_vau_src1_rect_xy                   vau_src1_rect_xy;                    /* 10 */
    u_vau_src1_rect_wh                   vau_src1_rect_wh;                    /* 14 */
    u_vau_src1_fill                      vau_src1_fill;                       /* 18 */
    u_vau_src1_alpha                     vau_src1_alpha;                      /* 1c */
    u_vau_src1_pix_rgb                   vau_src1_pix_rgb;                    /* 20 */
    u_vau_src2_ctrl                      vau_src2_ctrl;                       /* 24 */
    u_vau_src2_addr                      vau_src2_addr;                       /* 28 */
    u_vau_src2_stride                    vau_src2_stride;                     /* 2c */
    u_vau_src2_imgsize                   vau_src2_imgsize;                    /* 30 */
    u_vau_src2_rect_xy                   vau_src2_rect_xy;                    /* 34 */
    u_vau_src2_rect_wh                   vau_src2_rect_wh;                    /* 38 */
    u_vau_src2_fill                      vau_src2_fill;                       /* 3c */
    u_vau_src2_alpha                     vau_src2_alpha;                      /* 40 */
    u_vau_src2_pix_rgb                   vau_src2_pix_rgb;                    /* 44 */
    u_vau_src2_dcmp_cfg0                 vau_src2_dcmp_cfg0;                  /* 48 */
    u_vau_src2_dcmp_cfg1                 vau_src2_dcmp_cfg1;                  /* 4c */
    u_vau_src2_dcmp_cfg2                 vau_src2_dcmp_cfg2;                  /* 50 */
    u_vau_src2_dcmp_cfg3                 vau_src2_dcmp_cfg3;                  /* 54 */
    u_vau_des_ctrl                       vau_des_ctrl;                        /* 58 */
    u_vau_des_addr                       vau_des_addr;                        /* 5c */
    u_vau_des_stride                     vau_des_stride;                      /* 60 */
    u_vau_des_imgsize                    vau_des_imgsize;                     /* 64 */
    u_vau_des_rect_xy                    vau_des_rect_xy;                     /* 68 */
    u_vau_des_rect_wh                    vau_des_rect_wh;                     /* 6c */
    u_vau_des_alpha                      vau_des_alpha;                       /* 70 */
    u_vau_cbm_ctrl                       vau_cbm_ctrl;                        /* 74 */
    u_vau_cbm_colorize                   vau_cbm_colorize;                    /* 78 */
    u_vau_cbm_alu_para                   vau_cbm_alu_para;                    /* 7c */
    u_vau_cbm_key_para                   vau_cbm_key_para;                    /* 80 */
    u_vau_cbm_key_min                    vau_cbm_key_min;                     /* 84 */
    u_vau_cbm_key_max                    vau_cbm_key_max;                     /* 88 */
    u_vau_cbm_key_mask                   vau_cbm_key_mask;                    /* 8c */
    u_vau_cbm_src1_cbm_para              vau_cbm_src1_cbm_para;               /* 90 */
    u_vau_cbm_src2_cbm_para              vau_cbm_src2_cbm_para;               /* 94 */
    unsigned int                         rsr0;                                /* 98 */
    u_vau_src1_crop_xy                   vau_src1_crop_xy;                    /* 9c */
    u_vau_src1_crop_wh                   vau_src1_crop_wh;                    /* a0 */
    u_vau_draw_ctrl                      vau_draw_ctrl;                       /* a4 */
    u_vau_draw_st                        vau_draw_st;                         /* a8 */
    u_vau_draw_end                       vau_draw_end;                        /* ac */
    u_vau_draw_cmd_addr                  vau_draw_cmd_addr;                   /* b0 */
    u_vau_draw_path_addr                 vau_draw_path_addr;                  /* b4 */
    u_vau_draw_et_addr                   vau_draw_et_addr;                    /* b8 */
    u_vau_draw_link_start_addr           vau_draw_link_start_addr;            /* bc */
    u_vau_draw_link_end_addr             vau_draw_link_end_addr;              /* c0 */
    u_vau_draw_et_bk_addr                vau_draw_et_bk_addr;                 /* c4 */
    u_vau_draw_path_num                  vau_draw_path_num;                   /* c8 */
    u_vau_draw_matrix_00                 vau_draw_matrix_00;                  /* cc */
    u_vau_draw_matrix_01                 vau_draw_matrix_01;                  /* d0 */
    u_vau_draw_matrix_02                 vau_draw_matrix_02;                  /* d4 */
    u_vau_draw_matrix_10                 vau_draw_matrix_10;                  /* d8 */
    u_vau_draw_matrix_11                 vau_draw_matrix_11;                  /* dc */
    u_vau_draw_matrix_12                 vau_draw_matrix_12;                  /* e0 */
    u_vau_draw_et_bkend_addr             vau_draw_et_bkend_addr;              /* e4 */
    unsigned int                         rsr1[2];                             /* e8 ~ ec */
    u_vau_clut_addr                      vau_clut_addr;                       /* f0 */
    u_vau_node_id                        vau_node_id;                         /* f4 */
    u_vau_intmask                        vau_intmask;                         /* f8 */
    u_vau_pnext                          vau_pnext;                           /* fc */
    u_vau_mix_node                       mix;                                 /* 100~164 or 200~264 */
} vau_hardware_node;

/* debug */
td_void vau_hal_print_node_nozero(const vau_hardware_node *node);
td_void vau_hal_print_node_all(const vau_hardware_node *node, td_u32 start, td_u32 end, td_u32 print_offset);

/* src1 */
td_void vau_hal_node_set_src1_base_info(vau_hardware_node *node, const hal_vau_surface *surface);
td_void vau_hal_node_set_src1_mode(vau_hardware_node *node, hal_vau_src_mode mode);
td_void vau_hal_node_set_src1_alpha(vau_hardware_node *node, const hal_vau_surface *surface);
td_void vau_hal_node_set_src1_crop(vau_hardware_node *node, const ext_rect *rect);

/* src2 */
td_void vau_hal_node_set_src2_base_info(vau_hardware_node *node, const hal_vau_surface *surface);
td_void vau_hal_node_set_src2_mode(vau_hardware_node *node, hal_vau_src_mode mode);
td_void vau_hal_node_set_src2_alpha(vau_hardware_node *node, const hal_vau_surface *surface);
td_void vau_hal_node_set_src2_crop(vau_hardware_node *node, const ext_rect *rect);
td_void vau_hal_node_set_src2_clut(vau_hardware_node *node, const hal_vau_surface *surface);
td_void vau_hal_node_set_affine_info(vau_hardware_node *node, hal_vau_affine_cfg *hal_cfg); /* only src2 */
td_void vau_hal_node_set_src2_t2r(vau_hardware_node *node, hal_vau_t2r_info *info);

/* dst */
td_void vau_hal_node_set_dst_base_info(vau_hardware_node *node, const hal_vau_surface *surface);
td_void vau_hal_node_set_dst_crop(const vau_hardware_node *node, const ext_rect *rect);

/* cbm */
td_void vau_hal_node_set_cbm(vau_hardware_node *node, const ext_rect *cbm_rect, td_u32 alu_mode);
td_void vau_hal_node_set_rop(vau_hardware_node *node, hal_vau_rop_opt *rop_opt);
td_void vau_hal_node_set_colorkey(vau_hardware_node *node, hal_vau_colorkey_opt *colorkey_opt);
td_void vau_hal_node_set_blend_mode(vau_hardware_node *node, td_u32 src1_mode, td_u32 src2_mode);
td_void vau_hal_node_set_blend_alpha_mode(vau_hardware_node *node, hal_vau_blend_alpha_mode alpha_from);

/* isr */
td_void vau_hal_node_enable_complete_int(vau_hardware_node *node);
td_void vau_hal_node_enable_lowdelay_int(const vau_hardware_node *node);
td_void vau_hal_node_set_tunl_enable(const vau_hardware_node *node);

/* next node */
td_void vau_hal_node_add_next_node(vau_hardware_node *node, td_u32 phy_addr);

/* vector draw */
td_void vau_hal_node_set_vector_info(vau_hardware_node *node, hal_vau_vector_cfg *cfg);

td_void vau_hal_node_set_alpha_strb(vau_hardware_node *node, td_bool enable);
#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif /* DRV_GRAPHIC_HAL_VAU_HAL_NODE_H */

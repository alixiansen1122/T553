/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description : vau cs reg define file
 */

#ifndef DRV_GRAPHIC_HAL_VAU_REG_DEFINE_H
#define DRV_GRAPHIC_HAL_VAU_REG_DEFINE_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define HAL_VAU_REG_BASEADDR 0x56011000

#define HAL_VAU_REG_START           (HAL_VAU_REG_BASEADDR + 0x0500)
#define HAL_VAU_REG_INT_STATE       (HAL_VAU_REG_BASEADDR + 0x0504)
#define HAL_VAU_REG_INT_CLR         (HAL_VAU_REG_BASEADDR + 0x0508)
#define HAL_VAU_REG_PNEXT           (HAL_VAU_REG_BASEADDR + 0x00fc)
#define HAL_VAU_REG_STATUS          (HAL_VAU_REG_BASEADDR + 0x4000)
#define HAL_VAU_REG_AQ_ADDR         (HAL_VAU_REG_BASEADDR + 0x4004)
#define HAL_VAU_REG_MISCELLANEOUS   (HAL_VAU_REG_BASEADDR + 0x0514)
#define HAL_VAU_REG_OUTSTANDING     (HAL_VAU_REG_BASEADDR + 0x0600)
#define HAL_VAU_SOFTRST_STATE       (HAL_VAU_REG_BASEADDR + 0x052C)
#define HAL_VAU_SRAM_CTRL           (HAL_VAU_REG_BASEADDR + 0x0b00)
#define HAL_VAU_LIST_PFCNT          (HAL_VAU_REG_BASEADDR + 0x40a0)
#define HAL_VAU_START_CNT           (HAL_VAU_REG_BASEADDR + 0x4060)
#define HAL_VAU_LIST_FINISH_CNT     (HAL_VAU_REG_BASEADDR + 0x4064)
#define HAL_VAU_INT_CNT             (HAL_VAU_REG_BASEADDR + 0x4068)

#define HAL_VAU_DRAW_PATH_CMD_ADDR   (HAL_VAU_REG_BASEADDR + 0xB0)
#define HAL_VAU_DRAW_PATH_DATA_ADDR  (HAL_VAU_REG_BASEADDR + 0xB4)
#define HAL_VAU_DRAW_PATH_NUM_ADDR   (HAL_VAU_REG_BASEADDR + 0xC8)

#define HAL_VAU_DRAW_THRESHOLD_QUAD  (HAL_VAU_REG_BASEADDR + 0xA00)
#define HAL_VAU_DRAW_THRESHOLD_CUBIC (HAL_VAU_REG_BASEADDR + 0xA04)
#define HAL_VAU_DRAW_SAMPLERADIUS    (HAL_VAU_REG_BASEADDR + 0xA08)
#define HAL_VAU_DRAW_CVRG_WEIGHT0    (HAL_VAU_REG_BASEADDR + 0xA4C)
#define HAL_VAU_DRAW_CVRG_WEIGHT1    (HAL_VAU_REG_BASEADDR + 0xA50)

#define HAL_VAU_DRAW_CMDBUF_START_ADDR  (HAL_VAU_REG_BASEADDR + 0xA54)
#define HAL_VAU_DRAW_CMDBUF_END_ADDR    (HAL_VAU_REG_BASEADDR + 0xA58)
#define HAL_VAU_DRAW_PATHBUF_START_ADDR (HAL_VAU_REG_BASEADDR + 0xA5C)
#define HAL_VAU_DRAW_PATHBUF_END_ADDR   (HAL_VAU_REG_BASEADDR + 0xA60)
/* Define the union vau_src1_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src1_fmt                : 6;       /* [5:0] */
        unsigned int src1_argb_order         : 5;       /* [10:6] */
        unsigned int src1_premult_en         : 1;       /* [11] */
        unsigned int src1_rgb_exp            : 2;       /* [13:12] */
        unsigned int reserved0               : 4;       /* [17:14] */
        unsigned int src1_alpha_range        : 1;       /* [18] */
        unsigned int src1_v_scan_ord         : 1;       /* [19] */
        unsigned int src1_h_scan_ord         : 1;       /* [20] */
        unsigned int reserved1               : 8;       /* [28:21] */
        unsigned int src1_mode               : 2;       /* [30:29] */
        unsigned int src1_en                 : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src1_ctrl;

/* Define the union vau_src1_addr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src1_addr               : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src1_addr;

/* Define the union vau_src1_stride */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src1_stride             : 20;      /* [19:0] */
        unsigned int reserved                : 12;      /* [31:20] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src1_stride;

/* Define the union vau_src1_imgsize */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src1_width              : 16;      /* [15:0] */
        unsigned int src1_height             : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src1_imgsize;

/* Define the union vau_src1_rect_xy */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src1_rect_x             : 16;      /* [15:0] */
        unsigned int src1_rect_y             : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src1_rect_xy;

/* Define the union vau_src1_rect_wh */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src1_rect_width         : 16;      /* [15:0] */
        unsigned int src1_rect_height        : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src1_rect_wh;

/* Define the union vau_src1_fill */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src1_color_fill         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src1_fill;

/* Define the union vau_src1_alpha */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src1_alpha0             : 8;       /* [7:0] */
        unsigned int src1_alpha1             : 8;       /* [15:8] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src1_alpha;

/* Define the union vau_src1_pix_rgb */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src1_pix_rgb            : 24;      /* [23:0] */
        unsigned int reserved                : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src1_pix_rgb;

/* Define the union vau_src2_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2_fmt                : 6;       /* [5:0] */
        unsigned int src2_argb_order         : 5;       /* [10:6] */
        unsigned int src2_premult_en         : 1;       /* [11] */
        unsigned int src2_rgb_exp            : 2;       /* [13:12] */
        unsigned int src2_clut_mode          : 1;       /* [14] */
        unsigned int reserved0               : 3;       /* [17:15] */
        unsigned int src2_alpha_range        : 1;       /* [18] */
        unsigned int src2_v_scan_ord         : 1;       /* [19] */
        unsigned int src2_h_scan_ord         : 1;       /* [20] */
        unsigned int reserved1               : 3;       /* [23:21] */
        unsigned int src2_aff_en             : 1;       /* [24] */
        unsigned int reserved2               : 2;       /* [26:25] */
        unsigned int src2_ice_en             : 1;       /* [27] */
        unsigned int reserved3               : 1;       /* [28] */
        unsigned int src2_mode               : 2;       /* [30:29] */
        unsigned int src2_en                 : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src2_ctrl;

/* Define the union vau_src2_addr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2_addr               : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src2_addr;

/* Define the union vau_src2_stride */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2_stride             : 20;      /* [19:0] */
        unsigned int reserved                : 12;      /* [31:20] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src2_stride;

/* Define the union vau_src2_imgsize */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2_width              : 16;      /* [15:0] */
        unsigned int src2_height             : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src2_imgsize;

/* Define the union vau_src2_rect_xy */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2_rect_x             : 16;      /* [15:0] */
        unsigned int src2_rect_y             : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src2_rect_xy;

/* Define the union vau_src2_rect_wh */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2_rect_width         : 16;      /* [15:0] */
        unsigned int src2_rect_height        : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src2_rect_wh;

/* Define the union vau_src2_fill */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2_color_fill         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src2_fill;

/* Define the union vau_src2_alpha */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2_alpha0             : 8;       /* [7:0] */
        unsigned int src2_alpha1             : 8;       /* [15:8] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src2_alpha;

/* Define the union vau_src2_pix_rgb */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2_pix_rgb            : 24;      /* [23:0] */
        unsigned int reserved                : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src2_pix_rgb;

/* Define the union vau_src2_dcmp_cfg0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2_dcmp_cfg0          : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src2_dcmp_cfg0;

/* Define the union vau_src2_dcmp_cfg1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2_dcmp_cfg1          : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src2_dcmp_cfg1;

/* Define the union vau_src2_dcmp_cfg2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2_dcmp_cfg2          : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src2_dcmp_cfg2;

/* Define the union vau_src2_dcmp_cfg3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2_dcmp_cfg3          : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src2_dcmp_cfg3;

/* Define the union vau_des_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int des_fmt                 : 6;       /* [5:0] */
        unsigned int des_argb_order          : 5;       /* [10:6] */
        unsigned int reserved0               : 1;       /* [11] */
        unsigned int des_bind_en             : 1;       /* [12] */
        unsigned int des_bind_mode           : 1;       /* [13] */
        unsigned int display_node            : 1;       /* [14] */
        unsigned int tunl_en                 : 1;       /* [15] */
        unsigned int reserved1               : 1;       /* [16] */
        unsigned int des_rgb_round           : 1;       /* [17] */
        unsigned int des_alpha_range         : 1;       /* [18] */
        unsigned int des_v_scan_ord          : 1;       /* [19] */
        unsigned int des_h_scan_ord          : 1;       /* [20] */
        unsigned int alpha_strb_en           : 1;       /* [21] */
        unsigned int reserved2               : 9;       /* [30:22] */
        unsigned int des_en                  : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_des_ctrl;

/* Define the union vau_des_addr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int des_addr                : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_des_addr;

/* Define the union vau_des_stride */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int des_stride              : 20;      /* [19:0] */
        unsigned int reserved                : 12;      /* [31:20] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_des_stride;

/* Define the union vau_des_imgsize */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int des_width               : 16;      /* [15:0] */
        unsigned int des_height              : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_des_imgsize;

/* Define the union vau_des_rect_xy */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int des_rect_x              : 16;      /* [15:0] */
        unsigned int des_rect_y              : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_des_rect_xy;

/* Define the union vau_des_rect_wh */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int des_rect_width          : 16;      /* [15:0] */
        unsigned int des_rect_height         : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_des_rect_wh;

/* Define the union vau_des_alpha */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int des_alpha_thd           : 8;       /* [7:0] */
        unsigned int reserved                : 24;      /* [31:8] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_des_alpha;

/* Define the union vau_cbm_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved0               : 21;      /* [20:0] */
        unsigned int alu_mode                : 4;       /* [24:21] */
        unsigned int reserved1               : 6;       /* [30:25] */
        unsigned int cbm_en                  : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_cbm_ctrl;

/* Define the union vau_cbm_colorize */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int colorize_b              : 8;       /* [7:0] */
        unsigned int colorize_g              : 8;       /* [15:8] */
        unsigned int colorize_r              : 8;       /* [23:16] */
        unsigned int reserved                : 7;       /* [30:24] */
        unsigned int colorize_en             : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_cbm_colorize;

/* Define the union vau_cbm_alu_para */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int rgb_rop                 : 4;       /* [3:0] */
        unsigned int a_rop                   : 4;       /* [7:4] */
        unsigned int reserved0               : 8;       /* [15:8] */
        unsigned int alpha_from              : 2;       /* [17:16] */
        unsigned int right_alpha_border_en   : 1;       /* [18] */
        unsigned int left_alpha_border_en    : 1;       /* [19] */
        unsigned int bottom_alpha_border_en  : 1;       /* [20] */
        unsigned int top_alpha_border_en     : 1;       /* [21] */
        unsigned int reserved1               : 9;       /* [30:22] */
        unsigned int blend_rop_en            : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_cbm_alu_para;

/* Define the union vau_cbm_key_para */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int key_b_mode              : 2;       /* [1:0] */
        unsigned int key_g_mode              : 2;       /* [3:2] */
        unsigned int key_r_mode              : 2;       /* [5:4] */
        unsigned int key_a_mode              : 2;       /* [7:6] */
        unsigned int key_sel                 : 2;       /* [9:8] */
        unsigned int reserved                : 21;      /* [30:10] */
        unsigned int key_en                  : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_cbm_key_para;

/* Define the union vau_cbm_key_min */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int key_b_min               : 8;       /* [7:0] */
        unsigned int key_g_min               : 8;       /* [15:8] */
        unsigned int key_r_min               : 8;       /* [23:16] */
        unsigned int key_a_min               : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_cbm_key_min;

/* Define the union vau_cbm_key_max */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int key_b_max               : 8;       /* [7:0] */
        unsigned int key_g_max               : 8;       /* [15:8] */
        unsigned int key_r_max               : 8;       /* [23:16] */
        unsigned int key_a_max               : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_cbm_key_max;

/* Define the union vau_cbm_key_mask */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int key_b_mask              : 8;       /* [7:0] */
        unsigned int key_g_mask              : 8;       /* [15:8] */
        unsigned int key_r_mask              : 8;       /* [23:16] */
        unsigned int key_a_mask              : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_cbm_key_mask;

/* Define the union vau_cbm_src1_cbm_para */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int s1_galpha_en            : 1;       /* [0] */
        unsigned int s1_palpha_en            : 1;       /* [1] */
        unsigned int s1_premult_en           : 1;       /* [2] */
        unsigned int s1_multi_global_en      : 1;       /* [3] */
        unsigned int s1_blend_mode           : 4;       /* [7:4] */
        unsigned int reserved                : 24;      /* [31:8] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_cbm_src1_cbm_para;

/* Define the union vau_cbm_src2_cbm_para */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int s2_galpha_en            : 1;       /* [0] */
        unsigned int s2_palpha_en            : 1;       /* [1] */
        unsigned int s2_premult_en           : 1;       /* [2] */
        unsigned int s2_multi_global_en      : 1;       /* [3] */
        unsigned int s2_blend_mode           : 4;       /* [7:4] */
        unsigned int s2_galpha               : 8;       /* [15:8] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_cbm_src2_cbm_para;

/* Define the union vau_src1_crop_xy */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src1_crop_x             : 13;      /* [12:0] */
        unsigned int reserved0               : 3;       /* [15:13] */
        unsigned int src1_crop_y             : 13;      /* [28:16] */
        unsigned int reserved1               : 1;       /* [29] */
        unsigned int src1_crop_cfg_en        : 1;       /* [30] */
        unsigned int src1_crop_en            : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src1_crop_xy;

/* Define the union vau_src1_crop_wh */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src1_crop_width         : 16;      /* [15:0] */
        unsigned int src1_crop_height        : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src1_crop_wh;

/* Define the union vau_draw_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved0               : 16;      /* [15:0] */
        unsigned int bdbx_adj_en             : 1;       /* [16] */
        unsigned int path_return_en          : 1;       /* [17] */
        unsigned int cmd_return_en           : 1;       /* [18] */
        unsigned int dyn_bdbx_en             : 1;       /* [19] */
        unsigned int msaa_mode               : 4;       /* [23:20] */
        unsigned int fill_effect             : 4;       /* [27:24] */
        unsigned int fill_mode               : 1;       /* [28] */
        unsigned int reserved1               : 2;       /* [30:29] */
        unsigned int draw_en                 : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_ctrl;

/* Define the union vau_draw_st */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int draw_x_start            : 16;      /* [15:0] */
        unsigned int draw_y_start            : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_st;

/* Define the union vau_draw_end */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int draw_x_end              : 16;      /* [15:0] */
        unsigned int draw_y_end              : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_end;

/* Define the union vau_draw_cmd_addr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int tes_cmd_addr            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_cmd_addr;

/* Define the union vau_draw_path_addr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int tes_data_addr           : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_path_addr;

/* Define the union vau_draw_et_addr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int tes_et_addr             : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_et_addr;

/* Define the union vau_draw_link_start_addr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int tes_link_start_addr     : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_link_start_addr;

/* Define the union vau_draw_link_end_addr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int tes_link_end_addr       : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_link_end_addr;

/* Define the union vau_draw_et_bk_addr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int tes_et_bk_addr          : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_et_bk_addr;

/* Define the union vau_draw_path_num */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cmd_num                 : 16;      /* [15:0] */
        unsigned int data_num                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_path_num;

/* Define the union vau_draw_matrix_00 */
typedef union {
    /* Define the struct bits */
    struct {
        float matrix_00;
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_matrix_00;

/* Define the union vau_draw_matrix_01 */
typedef union {
    /* Define the struct bits */
    struct {
        float matrix_01;
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_matrix_01;

/* Define the union vau_draw_matrix_02 */
typedef union {
    /* Define the struct bits */
    struct {
        float matrix_02;
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_matrix_02;

/* Define the union vau_draw_matrix_10 */
typedef union {
    /* Define the struct bits */
    struct {
        float matrix_10;
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_matrix_10;

/* Define the union vau_draw_matrix_11 */
typedef union {
    /* Define the struct bits */
    struct {
        float matrix_11;
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_matrix_11;

/* Define the union vau_draw_matrix_12 */
typedef union {
    /* Define the struct bits */
    struct {
        float matrix_12;
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_matrix_12;

/* Define the union vau_draw_et_bkend_addr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int tes_et_bkend_addr       : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_et_bkend_addr;

/* Define the union vau_clut_addr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int clut_addr               : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_clut_addr;

/* Define the union vau_node_id */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int node_id                 : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_node_id;

/* Define the union vau_intmask */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int node_end_mask           : 1;       /* [0] */
        unsigned int timeout_mask            : 1;       /* [1] */
        unsigned int bus_err_mask            : 1;       /* [2] */
        unsigned int list_end_mask           : 1;       /* [3] */
        unsigned int display_done_mask       : 1;       /* [4] */
        unsigned int reserved0               : 1;       /* [5] */
        unsigned int reserved1               : 1;       /* [6] */
        unsigned int draw_err_mask           : 1;       /* [7] */
        unsigned int et_oversize_mask        : 1;       /* [8] */
        unsigned int draw_oversize_mask      : 1;       /* [9] */
        unsigned int dcmp_cfg_err_mask       : 1;       /* [10] */
        unsigned int dcmp_bs_err_mask        : 1;       /* [11] */
        unsigned int once_draw_err_mask      : 1;       /* [12] */
        unsigned int once_et_oversize_mask   : 1;       /* [13] */
        unsigned int once_draw_oversize_mask : 1;       /* [14] */
        unsigned int once_dcmp_cfg_err_mask  : 1;       /* [15] */
        unsigned int once_dcmp_bs_err_mask   : 1;       /* [16] */
        unsigned int reserved2               : 15;      /* [31:17] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_intmask;

/* Define the union vau_pnext */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int p_next                  : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_pnext;

/* Define the union vau_gradient_typical_color */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int typical_color           : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_gradient_typical_color;

/* Define the union vau_gradient_linear_const_x */
typedef union {
    /* Define the struct bits */
    struct {
        float linear_const_x;      /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_gradient_linear_const_x;

/* Define the union vau_gradient_linear_const_y */
typedef union {
    /* Define the struct bits */
    struct {
        float linear_const_y;      /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_gradient_linear_const_y;

/* Define the union vau_gradient_linear_start */
typedef union {
    /* Define the struct bits */
    struct {
        signed short linear_start_y          : 16;      /* [15:0] */
        signed short linear_start_x          : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_gradient_linear_start;

/* Define the union vau_gradient_const_mode */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int linear_const_mode       : 2;       /* [1:0] */
        unsigned int reserved                : 30;      /* [31:2] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_gradient_const_mode;

/* Define the union vau_gradient_radial_const_r */
typedef union {
    /* Define the struct bits */
    struct {
        float radial_const_r;      /* only [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_gradient_radial_const_r;

/* Define the union vau_gradient_radial_center */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int radial_center_y         : 10;      /* [9:0] */
        unsigned int reserved0               : 6;       /* [15:10] */
        unsigned int radial_center_x         : 10;      /* [25:16] */
        unsigned int reserved1               : 6;       /* [31:26] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_gradient_radial_center;

/* Define the union vau_gradient_sweep_center */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sweep_center_y          : 10;      /* [9:0] */
        unsigned int reserved0               : 6;       /* [15:10] */
        unsigned int sweep_center_x          : 10;      /* [25:16] */
        unsigned int reserved1               : 6;       /* [31:26] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_gradient_sweep_center;

/* Define the union vau_gradient_sweep_start_angle */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sweep_start_angle       : 9;       /* [8:0] */
        unsigned int reserved                : 23;      /* [31:9] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_gradient_sweep_start_angle;

/* Define the union vau_gradient_tile_mode */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int tile_mode               : 2;       /* [1:0] */
        unsigned int reserved                : 30;      /* [31:2] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_gradient_tile_mode;

/* Define the union vau_gradient_stops_offset0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int stops_offset0           : 8;       /* [7:0] */
        unsigned int stops_offset1           : 8;       /* [15:8] */
        unsigned int stops_offset2           : 8;       /* [23:16] */
        unsigned int stops_offset3           : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_gradient_stops_offset0;

/* Define the union vau_gradient_stops_offset1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int stops_offset4           : 8;       /* [7:0] */
        unsigned int stops_offset5           : 8;       /* [15:8] */
        unsigned int stops_offset6           : 8;       /* [23:16] */
        unsigned int reserved                : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_gradient_stops_offset1;

/* Define the union vau_gradient_stops_color0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int stops_color0            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_gradient_stops_color0;

/* Define the union vau_gradient_stops_color1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int stops_color1            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_gradient_stops_color1;

/* Define the union vau_gradient_stops_color2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int stops_color2            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_gradient_stops_color2;

/* Define the union vau_gradient_stops_color3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int stops_color3            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_gradient_stops_color3;

/* Define the union vau_gradient_stops_color4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int stops_color4            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_gradient_stops_color4;

/* Define the union vau_gradient_stops_color5 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int stops_color5            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_gradient_stops_color5;

/* Define the union vau_gradient_stops_color6 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int stops_color6            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_gradient_stops_color6;

/* Define the union vau_gradient_color_reci0 */
typedef union {
    /* Define the struct bits */
    struct {
        float color_reci01;
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_gradient_color_reci0;

/* Define the union vau_gradient_color_reci1 */
typedef union {
    /* Define the struct bits */
    struct {
        float color_reci12;
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_gradient_color_reci1;

/* Define the union vau_gradient_color_reci2 */
typedef union {
    /* Define the struct bits */
    struct {
        float color_reci23;
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_gradient_color_reci2;

/* Define the union vau_gradient_color_reci3 */
typedef union {
    /* Define the struct bits */
    struct {
        float color_reci34;
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_gradient_color_reci3;

/* Define the union vau_gradient_color_reci4 */
typedef union {
    /* Define the struct bits */
    struct {
        float color_reci45;
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_gradient_color_reci4;

/* Define the union vau_gradient_color_reci5 */
typedef union {
    /* Define the struct bits */
    struct {
        float color_reci56;
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_gradient_color_reci5;

/* Define the union vau_gradient_stops_num */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int stops_num               : 3;       /* [2:0] */
        unsigned int reserved                : 29;      /* [31:3] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_gradient_stops_num;

/* Define the union vau_src2_crop_xy */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2_crop_x             : 13;      /* [12:0] */
        unsigned int reserved0               : 3;       /* [15:13] */
        unsigned int src2_crop_y             : 13;      /* [28:16] */
        unsigned int reserved1               : 1;       /* [29] */
        unsigned int src2_crop_cfg_en        : 1;       /* [30] */
        unsigned int src2_crop_en            : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src2_crop_xy;

/* Define the union vau_src2_crop_wh */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src2_crop_width         : 16;      /* [15:0] */
        unsigned int src2_crop_height        : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src2_crop_wh;

/* Define the union vau_t2r_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int t2r_en                  : 1;       /* [0] */
        unsigned int flip_en                 : 1;       /* [1] */
        unsigned int cch_copy_en             : 1;       /* [2] */
        unsigned int delay_en                : 1;       /* [3] */
        unsigned int cfg_en                  : 1;       /* [4] */
        unsigned int reserved0               : 3;       /* [7:5] */
        unsigned int tile_wth                : 8;       /* [15:8] */
        unsigned int reserved1               : 15;      /* [30:16] */
        unsigned int t2r_ck_gt_en            : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_t2r_ctrl;

/* Define the union vau_matrix_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved                : 24;      /* [23:0] */
        unsigned int pattern_cfg             : 2;       /* [25:24] */
        unsigned int cfg_en                  : 1;       /* [26] */
        unsigned int req_tile_sel            : 1;       /* [27] */
        unsigned int alpha_trans_mode        : 1;       /* [28] */
        unsigned int tile_linear_sel         : 1;       /* [29] */
        unsigned int transform_mode          : 1;       /* [30] */
        unsigned int matrix_en               : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_matrix_ctrl;

/* Define the union vau_matrix_coef00 */
typedef union {
    /* Define the struct bits */
    struct {
        float matrix_coef00;      /* bits [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_matrix_coef00;

/* Define the union vau_matrix_coef01 */
typedef union {
    /* Define the struct bits */
    struct {
        float matrix_coef01;      /* bits [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_matrix_coef01;

/* Define the union vau_matrix_coef02 */
typedef union {
    /* Define the struct bits */
    struct {
        float matrix_coef02;      /* bits [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_matrix_coef02;

/* Define the union vau_matrix_coef10 */
typedef union {
    /* Define the struct bits */
    struct {
        float matrix_coef10;      /* bits [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_matrix_coef10;

/* Define the union vau_matrix_coef11 */
typedef union {
    /* Define the struct bits */
    struct {
        float matrix_coef11;      /* bits [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_matrix_coef11;

/* Define the union vau_matrix_coef12 */
typedef union {
    /* Define the struct bits */
    struct {
        float matrix_coef12;      /* bits [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_matrix_coef12;

/* Define the union vau_matrix_coef20 */
typedef union {
    /* Define the struct bits */
    struct {
        float matrix_coef20;      /* bits [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_matrix_coef20;

/* Define the union vau_matrix_coef21 */
typedef union {
    /* Define the struct bits */
    struct {
        float matrix_coef21;      /* bits [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_matrix_coef21;

/* Define the union vau_matrix_coef22 */
typedef union {
    /* Define the struct bits */
    struct {
        float matrix_coef22;      /* bits [31:8] is valid */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_matrix_coef22;

/* Define the union vau_matrix_crop_xy */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int matrix_crop_x           : 16;      /* [15:0] */
        unsigned int matrix_crop_y           : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_matrix_crop_xy;

/* Define the union vau_matrix_crop_wh */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int matrix_crop_width       : 16;      /* [15:0] */
        unsigned int matrix_crop_height      : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_matrix_crop_wh;

/* Define the union vau_matrix_pattern_fill_color */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int fill_color_b            : 8;       /* [7:0] */
        unsigned int fill_color_g            : 8;       /* [15:8] */
        unsigned int fill_color_r            : 8;       /* [23:16] */
        unsigned int fill_color_a            : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_matrix_pattern_fill_color;

/* Define the union vau_gauss_filter_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int gauss_filter_en         : 1;       /* [0] */
        unsigned int reserved                : 30;      /* [30:1] */
        unsigned int gauss_filter_ck_gt_en   : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_gauss_filter_ctrl;

/* Define the union vau_gauss_filter_radius0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int x_radius0               : 8;       /* [7:0] */
        unsigned int reserved                : 24;      /* [31:8] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_gauss_filter_radius0;

/* Define the union vau_gauss_filter_radius1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int x_radius1               : 8;       /* [7:0] */
        unsigned int reserved                : 24;      /* [31:8] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_gauss_filter_radius1;

/* Define the union vau_gauss_filter_radius2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int x_radius2               : 8;       /* [7:0] */
        unsigned int reserved                : 24;      /* [31:8] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_gauss_filter_radius2;

/* Define the union vau_gauss_filter_weight0 */
typedef union {
    /* Define the struct bits */
    struct {
        float x_weight0;
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_gauss_filter_weight0;

/* Define the union vau_gauss_filter_weight1 */
typedef union {
    /* Define the struct bits */
    struct {
        float x_weight1;
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_gauss_filter_weight1;

/* Define the union vau_gauss_filter_weight2 */
typedef union {
    /* Define the struct bits */
    struct {
        float x_weight2;
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_gauss_filter_weight2;

/* Define the union vau_start */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int start                   : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_start;

/* Define the union vau_intstate */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int node_end_state          : 1;       /* [0] */
        unsigned int timeout_state           : 1;       /* [1] */
        unsigned int bus_err                 : 1;       /* [2] */
        unsigned int list_end_state          : 1;       /* [3] */
        unsigned int display_done_state      : 1;       /* [4] */
        unsigned int reserved0               : 1;       /* [5] */
        unsigned int reserved1               : 1;       /* [6] */
        unsigned int draw_err                : 1;       /* [7] */
        unsigned int et_oversize_state       : 1;       /* [8] */
        unsigned int draw_oversize           : 1;       /* [9] */
        unsigned int dcmp_cfg_err            : 1;       /* [10] */
        unsigned int dcmp_bs_err             : 1;       /* [11] */
        unsigned int once_draw_err           : 1;       /* [12] */
        unsigned int once_et_oversize        : 1;       /* [13] */
        unsigned int once_draw_oversize      : 1;       /* [14] */
        unsigned int once_dcmp_cfg_err       : 1;       /* [15] */
        unsigned int once_dcmp_bs_err        : 1;       /* [16] */
        unsigned int reserved2               : 15;      /* [31:17] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_intstate;

/* Define the union vau_intclr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int node_end_clr            : 1;       /* [0] */
        unsigned int timeout_clr             : 1;       /* [1] */
        unsigned int bus_err_clr             : 1;       /* [2] */
        unsigned int list_end_clr            : 1;       /* [3] */
        unsigned int display_done_clr        : 1;       /* [4] */
        unsigned int reserved0               : 1;       /* [5] */
        unsigned int reserved1               : 1;       /* [6] */
        unsigned int draw_err_clr            : 1;       /* [7] */
        unsigned int et_oversize_clr         : 1;       /* [8] */
        unsigned int draw_oversize_clr       : 1;       /* [9] */
        unsigned int dcmp_cfg_err_clr        : 1;       /* [10] */
        unsigned int dcmp_bs_err_clr         : 1;       /* [11] */
        unsigned int once_draw_err_clr       : 1;       /* [12] */
        unsigned int once_et_oversize_clr    : 1;       /* [13] */
        unsigned int once_draw_oversize_clr  : 1;       /* [14] */
        unsigned int once_dcmp_cfg_err_clr   : 1;       /* [15] */
        unsigned int once_dcmp_bs_err_clr    : 1;       /* [16] */
        unsigned int reserved2               : 15;      /* [31:17] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_intclr;

/* Define the union vau_rawint */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int raw_node_end            : 1;       /* [0] */
        unsigned int raw_timeout             : 1;       /* [1] */
        unsigned int raw_bus_err             : 1;       /* [2] */
        unsigned int raw_list_end            : 1;       /* [3] */
        unsigned int raw_display_done        : 1;       /* [4] */
        unsigned int reserved0               : 1;       /* [5] */
        unsigned int reserved1               : 1;       /* [6] */
        unsigned int raw_draw_err            : 1;       /* [7] */
        unsigned int raw_et_oversize         : 1;       /* [8] */
        unsigned int raw_draw_oversize       : 1;       /* [9] */
        unsigned int raw_dcmp_cfg_err        : 1;       /* [10] */
        unsigned int raw_dcmp_bs_err         : 1;       /* [11] */
        unsigned int reserved2               : 20;      /* [31:12] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_rawint;

/* Define the union vau_pfcnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pfcnt                   : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_pfcnt;

/* Define the union vau_miscellaneous */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved0               : 8;       /* [7:0] */
        unsigned int init_timer              : 16;      /* [23:8] */
        unsigned int ck_gt_en                : 1;       /* [24] */
        unsigned int ck_gt_en_calc           : 1;       /* [25] */
        unsigned int ck_gt_en_cfg            : 1;       /* [26] */
        unsigned int ck_gt_en_axi            : 1;       /* [27] */
        unsigned int reserved1               : 4;       /* [31:28] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_miscellaneous;

/* Define the union vau_timeout */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int timeout                 : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_timeout;

/* Define the union vau_node_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int node_cnt                : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_node_cnt;

/* Define the union vau_memctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int llrfshd_ema             : 3;       /* [2:0] */
        unsigned int llrfshd_emas            : 1;       /* [3] */
        unsigned int llrfshd_emaw            : 2;       /* [5:4] */
        unsigned int llrfshd_rawl            : 1;       /* [6] */
        unsigned int llrfshd_rawlm           : 2;       /* [8:7] */
        unsigned int llrfshd_ret1n           : 1;       /* [9] */
        unsigned int llrfshd_wabl            : 1;       /* [10] */
        unsigned int llrfshd_wablm           : 2;       /* [12:11] */
        unsigned int llrasuhd_ema            : 3;       /* [15:13] */
        unsigned int llrasuhd_emas           : 1;       /* [16] */
        unsigned int llrasuhd_emaw           : 2;       /* [18:17] */
        unsigned int llrasuhd_rawl           : 1;       /* [19] */
        unsigned int llrasuhd_rawlm          : 2;       /* [21:20] */
        unsigned int llrasuhd_ret1n          : 1;       /* [22] */
        unsigned int llrasuhd_stov           : 1;       /* [23] */
        unsigned int llrasuhd_wabl           : 1;       /* [24] */
        unsigned int llrasuhd_wablm          : 3;       /* [27:25] */
        unsigned int lgrfthd_emaa            : 3;       /* [30:28] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_memctrl;

/* Define the union vau_memctrl1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int lgrfthd_emab            : 3;       /* [2:0] */
        unsigned int lgrfthd_emasa           : 1;       /* [3] */
        unsigned int lgrfthd_ret1n           : 1;       /* [4] */
        unsigned int lgrfthd_stov            : 1;       /* [5] */
        unsigned int llrfthd_emaa            : 3;       /* [8:6] */
        unsigned int llrfthd_emab            : 3;       /* [11:9] */
        unsigned int llrfthd_emasa           : 1;       /* [12] */
        unsigned int llrfthd_ret1n           : 1;       /* [13] */
        unsigned int llrfthd_stov            : 1;       /* [14] */
        unsigned int llrfthd_wabl            : 1;       /* [15] */
        unsigned int llrfthd_wablm           : 2;       /* [17:16] */
        unsigned int reserved                : 14;      /* [31:18] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_memctrl1;

/* Define the union vau_soft_rst_state */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int soft_rst_state          : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_soft_rst_state;

/* Define the union vau_memctrl_09v */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int llrfshd_ema_09v         : 3;       /* [2:0] */
        unsigned int llrfshd_emas_09v        : 1;       /* [3] */
        unsigned int llrfshd_emaw_09v        : 2;       /* [5:4] */
        unsigned int llrfshd_rawl_09v        : 1;       /* [6] */
        unsigned int llrfshd_rawlm_09v       : 2;       /* [8:7] */
        unsigned int llrfshd_ret1n_09v       : 1;       /* [9] */
        unsigned int llrfshd_wabl_09v        : 1;       /* [10] */
        unsigned int llrfshd_wablm_09v       : 2;       /* [12:11] */
        unsigned int llrasuhd_ema_09v        : 3;       /* [15:13] */
        unsigned int llrasuhd_emas_09v       : 1;       /* [16] */
        unsigned int llrasuhd_emaw_09v       : 2;       /* [18:17] */
        unsigned int llrasuhd_rawl_09v       : 1;       /* [19] */
        unsigned int llrasuhd_rawlm_09v      : 2;       /* [21:20] */
        unsigned int llrasuhd_ret1n_09v      : 1;       /* [22] */
        unsigned int llrasuhd_stov_09v       : 1;       /* [23] */
        unsigned int llrasuhd_wabl_09v       : 1;       /* [24] */
        unsigned int llrasuhd_wablm_09v      : 3;       /* [27:25] */
        unsigned int lgrfthd_emaa_09v        : 3;       /* [30:28] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_memctrl_09v;

/* Define the union vau_memctrl1_09v */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int lgrfthd_emab_09v        : 3;       /* [2:0] */
        unsigned int lgrfthd_emasa_09v       : 1;       /* [3] */
        unsigned int lgrfthd_ret1n_09v       : 1;       /* [4] */
        unsigned int lgrfthd_stov_09v        : 1;       /* [5] */
        unsigned int llrfthd_emaa_09v        : 3;       /* [8:6] */
        unsigned int llrfthd_emab_09v        : 3;       /* [11:9] */
        unsigned int llrfthd_emasa_09v       : 1;       /* [12] */
        unsigned int llrfthd_ret1n_09v       : 1;       /* [13] */
        unsigned int llrfthd_stov_09v        : 1;       /* [14] */
        unsigned int llrfthd_wabl_09v        : 1;       /* [15] */
        unsigned int llrfthd_wablm_09v       : 2;       /* [17:16] */
        unsigned int reserved                : 14;      /* [31:18] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_memctrl1_09v;

/* Define the union vau_mst_outstanding */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mstr_routstanding       : 4;       /* [3:0] */
        unsigned int mstr_woutstanding       : 4;       /* [7:4] */
        unsigned int reserved                : 24;      /* [31:8] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mst_outstanding;

/* Define the union vau_mst_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int split_mode              : 4;       /* [3:0] */
        unsigned int arb_mode                : 4;       /* [7:4] */
        unsigned int mid_enable              : 1;       /* [8] */
        unsigned int reserved                : 23;      /* [31:9] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mst_ctrl;

/* Define the union vau_mst_rchn_prio */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int rchn0_prio              : 1;       /* [0] */
        unsigned int rchn1_prio              : 1;       /* [1] */
        unsigned int rchn2_prio              : 1;       /* [2] */
        unsigned int rchn3_prio              : 1;       /* [3] */
        unsigned int rchn4_prio              : 1;       /* [4] */
        unsigned int rchn5_prio              : 1;       /* [5] */
        unsigned int rchn6_prio              : 1;       /* [6] */
        unsigned int rchn7_prio              : 1;       /* [7] */
        unsigned int rchn8_prio              : 1;       /* [8] */
        unsigned int rchn9_prio              : 1;       /* [9] */
        unsigned int rchn10_prio             : 1;       /* [10] */
        unsigned int rchn11_prio             : 1;       /* [11] */
        unsigned int rchn12_prio             : 1;       /* [12] */
        unsigned int rchn13_prio             : 1;       /* [13] */
        unsigned int rchn14_prio             : 1;       /* [14] */
        unsigned int rchn15_prio             : 1;       /* [15] */
        unsigned int rchn16_prio             : 1;       /* [16] */
        unsigned int rchn17_prio             : 1;       /* [17] */
        unsigned int rchn18_prio             : 1;       /* [18] */
        unsigned int rchn19_prio             : 1;       /* [19] */
        unsigned int rchn20_prio             : 1;       /* [20] */
        unsigned int rchn21_prio             : 1;       /* [21] */
        unsigned int rchn22_prio             : 1;       /* [22] */
        unsigned int rchn23_prio             : 1;       /* [23] */
        unsigned int rchn24_prio             : 1;       /* [24] */
        unsigned int rchn25_prio             : 1;       /* [25] */
        unsigned int rchn26_prio             : 1;       /* [26] */
        unsigned int rchn27_prio             : 1;       /* [27] */
        unsigned int rchn28_prio             : 1;       /* [28] */
        unsigned int rchn29_prio             : 1;       /* [29] */
        unsigned int rchn30_prio             : 1;       /* [30] */
        unsigned int rchn31_prio             : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mst_rchn_prio;

/* Define the union vau_mst_wchn_prio */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int wchn0_prio              : 1;       /* [0] */
        unsigned int wchn1_prio              : 1;       /* [1] */
        unsigned int wchn2_prio              : 1;       /* [2] */
        unsigned int wchn3_prio              : 1;       /* [3] */
        unsigned int wchn4_prio              : 1;       /* [4] */
        unsigned int wchn5_prio              : 1;       /* [5] */
        unsigned int wchn6_prio              : 1;       /* [6] */
        unsigned int wchn7_prio              : 1;       /* [7] */
        unsigned int wchn8_prio              : 1;       /* [8] */
        unsigned int wchn9_prio              : 1;       /* [9] */
        unsigned int wchn10_prio             : 1;       /* [10] */
        unsigned int wchn11_prio             : 1;       /* [11] */
        unsigned int wchn12_prio             : 1;       /* [12] */
        unsigned int wchn13_prio             : 1;       /* [13] */
        unsigned int wchn14_prio             : 1;       /* [14] */
        unsigned int wchn15_prio             : 1;       /* [15] */
        unsigned int wchn16_prio             : 1;       /* [16] */
        unsigned int wchn17_prio             : 1;       /* [17] */
        unsigned int wchn18_prio             : 1;       /* [18] */
        unsigned int wchn19_prio             : 1;       /* [19] */
        unsigned int wchn20_prio             : 1;       /* [20] */
        unsigned int wchn21_prio             : 1;       /* [21] */
        unsigned int wchn22_prio             : 1;       /* [22] */
        unsigned int wchn23_prio             : 1;       /* [23] */
        unsigned int wchn24_prio             : 1;       /* [24] */
        unsigned int wchn25_prio             : 1;       /* [25] */
        unsigned int wchn26_prio             : 1;       /* [26] */
        unsigned int wchn27_prio             : 1;       /* [27] */
        unsigned int wchn28_prio             : 1;       /* [28] */
        unsigned int wchn29_prio             : 1;       /* [29] */
        unsigned int wchn30_prio             : 1;       /* [30] */
        unsigned int wchn31_prio             : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mst_wchn_prio;

/* Define the union vau_mst_bus_err_clr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int bus_error_clr           : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mst_bus_err_clr;

/* Define the union vau_mst_bus_err */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mst_r_error             : 1;       /* [0] */
        unsigned int mst_w_error             : 1;       /* [1] */
        unsigned int reserved                : 30;      /* [31:2] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mst_bus_err;

/* Define the union vau_mst_src0_status0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src0_status0            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mst_src0_status0;

/* Define the union vau_mst_src0_status1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src0_status1            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mst_src0_status1;

/* Define the union vau_mst_debug_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int axi_det_enable          : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mst_debug_ctrl;

/* Define the union vau_mst_debug_clr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int axi_det_clr             : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mst_debug_clr;

/* Define the union vau_mst_det_latency */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int axi_mst_latency         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mst_det_latency;

/* Define the union vau_mst_rd_info */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mst_rd_info             : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mst_rd_info;

/* Define the union vau_mst_wr_info */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mst_wr_info             : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mst_wr_info;

/* Define the union vau_mst_det_latency0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mst_rd_max              : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mst_det_latency0;

/* Define the union vau_mst_det_latency1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mst_rd_aver             : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mst_det_latency1;

/* Define the union vau_mst_det_latency2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mst_rd_cmd              : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mst_det_latency2;

/* Define the union vau_mst_det_latency3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mst_wr_max              : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mst_det_latency3;

/* Define the union vau_mst_det_latency4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mst_wr_aver             : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mst_det_latency4;

/* Define the union vau_mst_det_latency5 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mst_wr_cmd              : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_mst_det_latency5;

/* Define the union vau_src_cmd_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src_cmd_interval        : 16;      /* [15:0] */
        unsigned int reserved                : 15;      /* [30:16] */
        unsigned int src_cmd_ctrl_en         : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_src_cmd_ctrl;

/* Define the union vau_des_cmd_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int des_cmd_interval        : 16;      /* [15:0] */
        unsigned int reserved                : 15;      /* [30:16] */
        unsigned int des_cmd_ctrl_en         : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_des_cmd_ctrl;

/* Define the union vau_des_safe_dist */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int des_safe_dist           : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_des_safe_dist;

/* Define the union vau_des_safe_dist_inverse */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int des_safe_dist_inverse   : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_des_safe_dist_inverse;

/* Define the union vau_des_tunl_dist */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int des_tunl_dist           : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_des_tunl_dist;

/* Define the union vau_matrix_debug0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pix_cnt_dfx             : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_matrix_debug0;

/* Define the union vau_t2r_err_clr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int of_err_clr              : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_t2r_err_clr;

/* Define the union vau_t2r_err_sta */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int of_err                  : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_t2r_err_sta;

/* Define the union vau_t2r_debug1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int t2r_debug1              : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_t2r_debug1;

/* Define the union vau_t2r_debug2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int t2r_debug2              : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_t2r_debug2;

/* Define the union vau_t2r_debug3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int t2r_debug3              : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_t2r_debug3;

/* Define the union vau_t2r_debug4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int t2r_debug4              : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_t2r_debug4;

/* Define the union vau_draw_threshold_quad */
typedef union {
    /* Define the struct bits */
    struct {
        float threshold_quad;
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_threshold_quad;

/* Define the union vau_draw_threshold_cubic */
typedef union {
    /* Define the struct bits */
    struct {
        float threshold_cubic;
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_threshold_cubic;

/* Define the union vau_draw_sampleradius */
typedef union {
    /* Define the struct bits */
    struct {
        float m_sampleradious;
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_sampleradius;

/* Define the union vau_draw_rndr_aa_smpl_x0 */
typedef union {
    /* Define the struct bits */
    struct {
        float sample_x_0;
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_rndr_aa_smpl_x0;

/* Define the union vau_draw_rndr_aa_smpl_x1 */
typedef union {
    /* Define the struct bits */
    struct {
        float sample_x_1;
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_rndr_aa_smpl_x1;

/* Define the union vau_draw_rndr_aa_smpl_x2 */
typedef union {
    /* Define the struct bits */
    struct {
        float sample_x_2;
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_rndr_aa_smpl_x2;

/* Define the union vau_draw_rndr_aa_smpl_x3 */
typedef union {
    /* Define the struct bits */
    struct {
        float sample_x_3;
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_rndr_aa_smpl_x3;

/* Define the union vau_draw_rndr_aa_smpl_x4 */
typedef union {
    /* Define the struct bits */
    struct {
        float sample_x_4;
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_rndr_aa_smpl_x4;

/* Define the union vau_draw_rndr_aa_smpl_x5 */
typedef union {
    /* Define the struct bits */
    struct {
        float sample_x_5;
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_rndr_aa_smpl_x5;

/* Define the union vau_draw_rndr_aa_smpl_x6 */
typedef union {
    /* Define the struct bits */
    struct {
        float sample_x_6;
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_rndr_aa_smpl_x6;

/* Define the union vau_draw_rndr_aa_smpl_x7 */
typedef union {
    /* Define the struct bits */
    struct {
        float sample_x_7;
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_rndr_aa_smpl_x7;

/* Define the union vau_draw_rndr_aa_smpl_y0 */
typedef union {
    /* Define the struct bits */
    struct {
        float sample_y_0;
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_rndr_aa_smpl_y0;

/* Define the union vau_draw_rndr_aa_smpl_y1 */
typedef union {
    /* Define the struct bits */
    struct {
        float sample_y_1;
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_rndr_aa_smpl_y1;

/* Define the union vau_draw_rndr_aa_smpl_y2 */
typedef union {
    /* Define the struct bits */
    struct {
        float sample_y_2;
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_rndr_aa_smpl_y2;

/* Define the union vau_draw_rndr_aa_smpl_y3 */
typedef union {
    /* Define the struct bits */
    struct {
        float sample_y_3;
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_rndr_aa_smpl_y3;

/* Define the union vau_draw_rndr_aa_smpl_y4 */
typedef union {
    /* Define the struct bits */
    struct {
        float sample_y_4;
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_rndr_aa_smpl_y4;

/* Define the union vau_draw_rndr_aa_smpl_y5 */
typedef union {
    /* Define the struct bits */
    struct {
        float sample_y_5;
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_rndr_aa_smpl_y5;

/* Define the union vau_draw_rndr_aa_smpl_y6 */
typedef union {
    /* Define the struct bits */
    struct {
        float sample_y_6;
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_rndr_aa_smpl_y6;

/* Define the union vau_draw_rndr_aa_smpl_y7 */
typedef union {
    /* Define the struct bits */
    struct {
        float sample_y_7;
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_rndr_aa_smpl_y7;

/* Define the union vau_draw_cvrg_weight0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int weight3                 : 8;       /* [7:0] */
        unsigned int weight2                 : 8;       /* [15:8] */
        unsigned int weight1                 : 8;       /* [23:16] */
        unsigned int weight0                 : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_cvrg_weight0;

/* Define the union vau_draw_cvrg_weight1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int weight7                 : 8;       /* [7:0] */
        unsigned int weight6                 : 8;       /* [15:8] */
        unsigned int weight5                 : 8;       /* [23:16] */
        unsigned int weight4                 : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_cvrg_weight1;

/* Define the union vau_draw_cmdbuf_start_addr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cmdbuf_start_addr       : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_cmdbuf_start_addr;

/* Define the union vau_draw_cmdbuf_end_addr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cmdbuf_end_addr         : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_cmdbuf_end_addr;

/* Define the union vau_draw_pathbuf_start_addr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pathbuf_start_addr      : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_pathbuf_start_addr;

/* Define the union vau_draw_pathbuf_end_addr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pathbuf_end_addr        : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_draw_pathbuf_end_addr;

/* Define the union vau_sram_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vau_block_en            : 10;      /* [9:0] */
        unsigned int reserved                : 20;      /* [29:10] */
        unsigned int vau_lp_mode             : 2;       /* [31:30] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_sram_ctrl;

/* Define the union vau_work_state */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int debug0                  : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_work_state;

/* Define the union vau_debug1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int p_last                  : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_debug1;

/* Define the union vau_debug2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int nodeid_last             : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_debug2;

/* Define the union vau_debug3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int out_src1_y_drdy         : 1;       /* [0] */
        unsigned int out_src1_y_dneed        : 1;       /* [1] */
        unsigned int out_src1_c_drdy         : 1;       /* [2] */
        unsigned int out_src1_c_dneed        : 1;       /* [3] */
        unsigned int out_src2_y_drdy         : 1;       /* [4] */
        unsigned int out_src2_y_dneed        : 1;       /* [5] */
        unsigned int out_src2_c_drdy         : 1;       /* [6] */
        unsigned int out_src2_c_dneed        : 1;       /* [7] */
        unsigned int out_src3_y_drdy         : 1;       /* [8] */
        unsigned int out_src3_y_dneed        : 1;       /* [9] */
        unsigned int out_src4_y_drdy         : 1;       /* [10] */
        unsigned int out_src4_y_dneed        : 1;       /* [11] */
        unsigned int out_src5_y_drdy         : 1;       /* [12] */
        unsigned int out_src5_y_dneed        : 1;       /* [13] */
        unsigned int out_src6_y_drdy         : 1;       /* [14] */
        unsigned int out_src6_y_dneed        : 1;       /* [15] */
        unsigned int out_src7_y_drdy         : 1;       /* [16] */
        unsigned int out_src7_y_dneed        : 1;       /* [17] */
        unsigned int r_req                   : 1;       /* [18] */
        unsigned int r_ack                   : 1;       /* [19] */
        unsigned int w_req                   : 1;       /* [20] */
        unsigned int w_ack                   : 1;       /* [21] */
        unsigned int w_valid                 : 1;       /* [22] */
        unsigned int w_ready                 : 1;       /* [23] */
        unsigned int r_wr_ready              : 1;       /* [24] */
        unsigned int r_valid                 : 1;       /* [25] */
        unsigned int reserved                : 6;       /* [31:26] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_debug3;

/* Define the union vau_debug4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ch_en_src1              : 1;       /* [0] */
        unsigned int ch_en_src1c             : 1;       /* [1] */
        unsigned int ch_en_src2              : 1;       /* [2] */
        unsigned int ch_en_src2c             : 1;       /* [3] */
        unsigned int ch_en_gdc3              : 1;       /* [4] */
        unsigned int ch_en_gdc4              : 1;       /* [5] */
        unsigned int ch_en_gdc5              : 1;       /* [6] */
        unsigned int ch_en_gdc6              : 1;       /* [7] */
        unsigned int ch_en_gdc7              : 1;       /* [8] */
        unsigned int ch_en_des               : 1;       /* [9] */
        unsigned int ch_en_desc              : 1;       /* [10] */
        unsigned int ch_en_cmp               : 1;       /* [11] */
        unsigned int read_done_src1          : 1;       /* [12] */
        unsigned int read_done_src1c         : 1;       /* [13] */
        unsigned int read_done_src2          : 1;       /* [14] */
        unsigned int read_done_src2c         : 1;       /* [15] */
        unsigned int read_done_gdc3          : 1;       /* [16] */
        unsigned int read_done_gdc4          : 1;       /* [17] */
        unsigned int read_done_gdc5          : 1;       /* [18] */
        unsigned int read_done_gdc6          : 1;       /* [19] */
        unsigned int read_done_gdc7          : 1;       /* [20] */
        unsigned int write_done_des          : 1;       /* [21] */
        unsigned int write_done_desc         : 1;       /* [22] */
        unsigned int write_done_cmp          : 1;       /* [23] */
        unsigned int reserved                : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_debug4;

/* Define the union vau_debug5 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int out_src1_dneed          : 1;       /* [0] */
        unsigned int out_src1_drdy           : 1;       /* [1] */
        unsigned int out_src2_dneed          : 1;       /* [2] */
        unsigned int out_src2_drdy           : 1;       /* [3] */
        unsigned int out_src3_dneed          : 1;       /* [4] */
        unsigned int out_src3_drdy           : 1;       /* [5] */
        unsigned int out_src4_dneed          : 1;       /* [6] */
        unsigned int out_src4_drdy           : 1;       /* [7] */
        unsigned int out_src5_dneed          : 1;       /* [8] */
        unsigned int out_src5_drdy           : 1;       /* [9] */
        unsigned int out_src6_dneed          : 1;       /* [10] */
        unsigned int out_src6_drdy           : 1;       /* [11] */
        unsigned int out_src7_dneed          : 1;       /* [12] */
        unsigned int out_src7_drdy           : 1;       /* [13] */
        unsigned int reserved                : 18;      /* [31:14] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_debug5;

/* Define the union vau_debug6 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int in_src1_y_drdy          : 1;       /* [0] */
        unsigned int in_src1_y_dneed         : 1;       /* [1] */
        unsigned int in_src1_c_drdy          : 1;       /* [2] */
        unsigned int in_src1_c_dneed         : 1;       /* [3] */
        unsigned int hpzme_out_y_drdy        : 1;       /* [4] */
        unsigned int hpzme_out_y_dneed       : 1;       /* [5] */
        unsigned int hpzme_out_c_drdy        : 1;       /* [6] */
        unsigned int hpzme_out_c_dneed       : 1;       /* [7] */
        unsigned int chfir_out_y_drdy        : 1;       /* [8] */
        unsigned int chfir_out_y_dneed       : 1;       /* [9] */
        unsigned int chfir_out_c_drdy        : 1;       /* [10] */
        unsigned int chfir_out_c_dneed       : 1;       /* [11] */
        unsigned int out_src1_drdy           : 1;       /* [12] */
        unsigned int out_src1_dneed          : 1;       /* [13] */
        unsigned int reserved                : 18;      /* [31:14] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_debug6;

/* Define the union vau_debug7 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int in_src2_y_drdy          : 1;       /* [0] */
        unsigned int in_src2_y_dneed         : 1;       /* [1] */
        unsigned int in_src2_c_drdy          : 1;       /* [2] */
        unsigned int in_src2_c_dneed         : 1;       /* [3] */
        unsigned int hpzme_out_y_drdy        : 1;       /* [4] */
        unsigned int hpzme_out_y_dneed       : 1;       /* [5] */
        unsigned int hpzme_out_c_drdy        : 1;       /* [6] */
        unsigned int hpzme_out_c_dneed       : 1;       /* [7] */
        unsigned int zme_out_y_drdy          : 1;       /* [8] */
        unsigned int zme_out_y_dneed         : 1;       /* [9] */
        unsigned int zme_out_c_drdy          : 1;       /* [10] */
        unsigned int zme_out_c_dneed         : 1;       /* [11] */
        unsigned int out_src2_drdy           : 1;       /* [12] */
        unsigned int out_src2_dneed          : 1;       /* [13] */
        unsigned int reserved                : 18;      /* [31:14] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_debug7;

/* Define the union vau_debug8 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int in_des_drdy             : 1;       /* [0] */
        unsigned int in_des_dneed            : 1;       /* [1] */
        unsigned int csc_out_drdy            : 1;       /* [2] */
        unsigned int csc_out_dneed           : 1;       /* [3] */
        unsigned int hdfir_out_y_drdy        : 1;       /* [4] */
        unsigned int hdfir_out_y_dneed       : 1;       /* [5] */
        unsigned int hdfir_out_c_drdy        : 1;       /* [6] */
        unsigned int hdfir_out_c_dneed       : 1;       /* [7] */
        unsigned int vdfir_out_y_drdy        : 1;       /* [8] */
        unsigned int vdfir_out_y_dneed       : 1;       /* [9] */
        unsigned int vdfir_out_c_drdy        : 1;       /* [10] */
        unsigned int vdfir_out_c_dneed       : 1;       /* [11] */
        unsigned int out_des_y_drdy          : 1;       /* [12] */
        unsigned int out_des_y_dneed         : 1;       /* [13] */
        unsigned int out_des_c_drdy          : 1;       /* [14] */
        unsigned int out_des_c_dneed         : 1;       /* [15] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_debug8;

/* Define the union vau_debug9 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src1_drdy               : 1;       /* [0] */
        unsigned int src1_dneed              : 1;       /* [1] */
        unsigned int src2_drdy               : 1;       /* [2] */
        unsigned int src2_dneed              : 1;       /* [3] */
        unsigned int src3_drdy               : 1;       /* [4] */
        unsigned int src3_dneed              : 1;       /* [5] */
        unsigned int src4_drdy               : 1;       /* [6] */
        unsigned int src4_dneed              : 1;       /* [7] */
        unsigned int src5_drdy               : 1;       /* [8] */
        unsigned int src5_dneed              : 1;       /* [9] */
        unsigned int src6_drdy               : 1;       /* [10] */
        unsigned int src6_dneed              : 1;       /* [11] */
        unsigned int src7_drdy               : 1;       /* [12] */
        unsigned int src7_dneed              : 1;       /* [13] */
        unsigned int out_drdy                : 1;       /* [14] */
        unsigned int out_dneed               : 1;       /* [15] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_debug9;

/* Define the union vau_debug10 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int bready                  : 1;       /* [0] */
        unsigned int bvalid                  : 1;       /* [1] */
        unsigned int wready                  : 1;       /* [2] */
        unsigned int wvalid                  : 1;       /* [3] */
        unsigned int awready                 : 1;       /* [4] */
        unsigned int awvalid                 : 1;       /* [5] */
        unsigned int rready                  : 1;       /* [6] */
        unsigned int rvalid                  : 1;       /* [7] */
        unsigned int arready                 : 1;       /* [8] */
        unsigned int arvalid                 : 1;       /* [9] */
        unsigned int reserved                : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_debug10;

/* Define the union vau_debug11 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int debug11                 : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_debug11;

/* Define the union vau_debug12 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int debug12                 : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_debug12;

/* Define the union vau_debug13 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int debug13                 : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_debug13;

/* Define the union vau_debug14 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int debug14                 : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_debug14;

/* Define the union vau_debug15 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int debug15                 : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_debug15;

/* Define the union vau_debug16 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int debug16                 : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_debug16;

/* Define the union vau_debug17 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int debug17                 : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_debug17;

/* Define the union vau_debug18 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int debug18                 : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_debug18;

/* Define the union vau_debug19 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int debug19                 : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_debug19;

/* Define the union vau_debug20 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int debug20                 : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_debug20;

/* Define the union vau_debug21 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int debug21                 : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_debug21;

/* Define the union vau_debug22 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int debug22                 : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_debug22;

/* Define the union vau_debug23 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int debug23                 : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_debug23;

/* Define the union vau_debug24 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int debug24                 : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_debug24;

/* Define the union vau_debug25 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int debug25                 : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_debug25;

/* Define the union vau_debug26 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int debug26                 : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_debug26;

/* Define the union vau_debug38 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int p_last_done             : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_debug38;

/* Define the union vau_debug39 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int nodeid_last_done        : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_debug39;

/* Define the union vau_pfcnt_list */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pfcnt_list              : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_pfcnt_list;

/* Define the union vau_tunl_sta */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int tunl_sta                : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_tunl_sta;

/* Define the union vau_tunl_wr_ptr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int tunl_wr_ptr             : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_tunl_wr_ptr;

/* Define the union vau_tunl_rd_ptr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int tunl_rd_ptr             : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vau_tunl_rd_ptr;


/*
 * DEFINE GLOBAL STRUCT
 */
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
    u_vau_gradient_stops_color0          vau_gradient_stops_color0;           /* 130 */
    u_vau_gradient_stops_color1          vau_gradient_stops_color1;           /* 134 */
    u_vau_gradient_stops_color2          vau_gradient_stops_color2;           /* 138 */
    u_vau_gradient_stops_color3          vau_gradient_stops_color3;           /* 13c */
    u_vau_gradient_stops_color4          vau_gradient_stops_color4;           /* 140 */
    u_vau_gradient_stops_color5          vau_gradient_stops_color5;           /* 144 */
    u_vau_gradient_stops_color6          vau_gradient_stops_color6;           /* 148 */
    u_vau_gradient_color_reci0           vau_gradient_color_reci0;            /* 14c */
    u_vau_gradient_color_reci1           vau_gradient_color_reci1;            /* 150 */
    u_vau_gradient_color_reci2           vau_gradient_color_reci2;            /* 154 */
    u_vau_gradient_color_reci3           vau_gradient_color_reci3;            /* 158 */
    u_vau_gradient_color_reci4           vau_gradient_color_reci4;            /* 15c */
    u_vau_gradient_color_reci5           vau_gradient_color_reci5;            /* 160 */
    u_vau_gradient_stops_num             vau_gradient_stops_num;              /* 164 */
    unsigned int                         rsr2[38];                            /* 168 ~ 1fc */
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
    unsigned int                         rsr4[167];                           /* 264 ~ 4fc */
    u_vau_start                          vau_start;                           /* 500 */
    u_vau_intstate                       vau_intstate;                        /* 504 */
    u_vau_intclr                         vau_intclr;                          /* 508 */
    u_vau_rawint                         vau_rawint;                          /* 50c */
    u_vau_pfcnt                          vau_pfcnt;                           /* 510 */
    u_vau_miscellaneous                  vau_miscellaneous;                   /* 514 */
    unsigned int                         rsr5;                                /* 518 */
    u_vau_timeout                        vau_timeout;                         /* 51c */
    u_vau_node_cnt                       vau_node_cnt;                        /* 520 */
    u_vau_memctrl                        vau_memctrl;                         /* 524 */
    u_vau_memctrl1                       vau_memctrl1;                        /* 528 */
    u_vau_soft_rst_state                 vau_soft_rst_state;                  /* 52c */
    u_vau_memctrl_09v                    vau_memctrl_09v;                     /* 530 */
    u_vau_memctrl1_09v                   vau_memctrl1_09v;                    /* 534 */
    unsigned int                         rsr6[50];                            /* 538 ~ 5fc */
    u_vau_mst_outstanding                vau_mst_outstanding;                 /* 600 */
    u_vau_mst_ctrl                       vau_mst_ctrl;                        /* 604 */
    unsigned int                         rsr7[2];                             /* 608 ~ 60c */
    u_vau_mst_rchn_prio                  vau_mst_rchn_prio;                   /* 610 */
    unsigned int                         rsr8;                                /* 614 */
    u_vau_mst_wchn_prio                  vau_mst_wchn_prio;                   /* 618 */
    unsigned int                         rsr9[9];                             /* 61c ~ 63c */
    u_vau_mst_bus_err_clr                vau_mst_bus_err_clr;                 /* 640 */
    u_vau_mst_bus_err                    vau_mst_bus_err;                     /* 644 */
    unsigned int                         rsr10[2];                            /* 648 ~ 64c */
    u_vau_mst_src0_status0               vau_mst_src0_status0;                /* 650 */
    u_vau_mst_src0_status1               vau_mst_src0_status1;                /* 654 */
    unsigned int                         rsr11[6];                            /* 658 ~ 66c */
    u_vau_mst_debug_ctrl                 vau_mst_debug_ctrl;                  /* 670 */
    u_vau_mst_debug_clr                  vau_mst_debug_clr;                   /* 674 */
    unsigned int                         rsr12[2];                            /* 678 ~ 67c */
    u_vau_mst_det_latency                vau_mst_det_latency;                 /* 680 */
    unsigned int                         rsr13[3];                            /* 684 ~ 68c */
    u_vau_mst_rd_info                    vau_mst_rd_info;                     /* 690 */
    u_vau_mst_wr_info                    vau_mst_wr_info;                     /* 694 */
    unsigned int                         rsr14[6];                            /* 698 ~ 6ac */
    u_vau_mst_det_latency0               vau_mst_det_latency0;                /* 6b0 */
    u_vau_mst_det_latency1               vau_mst_det_latency1;                /* 6b4 */
    u_vau_mst_det_latency2               vau_mst_det_latency2;                /* 6b8 */
    u_vau_mst_det_latency3               vau_mst_det_latency3;                /* 6bc */
    u_vau_mst_det_latency4               vau_mst_det_latency4;                /* 6c0 */
    u_vau_mst_det_latency5               vau_mst_det_latency5;                /* 6c4 */
    unsigned int                         rsr15[14];                           /* 6c8 ~ 6fc */
    u_vau_src_cmd_ctrl                   vau_src_cmd_ctrl;                    /* 700 */
    u_vau_des_cmd_ctrl                   vau_des_cmd_ctrl;                    /* 704 */
    u_vau_des_safe_dist                  vau_des_safe_dist;                   /* 708 */
    u_vau_des_safe_dist_inverse          vau_des_safe_dist_inverse;           /* 70c */
    u_vau_des_tunl_dist                  vau_des_tunl_dist;                   /* 710 */
    unsigned int                         rsr16[59];                           /* 714 ~ 7fc */
    u_vau_matrix_debug0                  vau_matrix_debug0;                   /* 800 */
    unsigned int                         rsr17[63];                           /* 804 ~ 8fc */
    u_vau_t2r_err_clr                    vau_t2r_err_clr;                     /* 900 */
    u_vau_t2r_err_sta                    vau_t2r_err_sta;                     /* 904 */
    unsigned int                         rsr18[2];                            /* 908 ~ 90c */
    u_vau_t2r_debug1                     vau_t2r_debug1;                      /* 910 */
    u_vau_t2r_debug2                     vau_t2r_debug2;                      /* 914 */
    u_vau_t2r_debug3                     vau_t2r_debug3;                      /* 918 */
    u_vau_t2r_debug4                     vau_t2r_debug4;                      /* 91c */
    unsigned int                         rsr19[56];                           /* 920 ~ 9fc */
    u_vau_draw_threshold_quad            vau_draw_threshold_quad;             /* a00 */
    u_vau_draw_threshold_cubic           vau_draw_threshold_cubic;            /* a04 */
    u_vau_draw_sampleradius              vau_draw_sampleradius;               /* a08 */
    u_vau_draw_rndr_aa_smpl_x0           vau_draw_rndr_aa_smpl_x0;            /* a0c */
    u_vau_draw_rndr_aa_smpl_x1           vau_draw_rndr_aa_smpl_x1;            /* a10 */
    u_vau_draw_rndr_aa_smpl_x2           vau_draw_rndr_aa_smpl_x2;            /* a14 */
    u_vau_draw_rndr_aa_smpl_x3           vau_draw_rndr_aa_smpl_x3;            /* a18 */
    u_vau_draw_rndr_aa_smpl_x4           vau_draw_rndr_aa_smpl_x4;            /* a1c */
    u_vau_draw_rndr_aa_smpl_x5           vau_draw_rndr_aa_smpl_x5;            /* a20 */
    u_vau_draw_rndr_aa_smpl_x6           vau_draw_rndr_aa_smpl_x6;            /* a24 */
    u_vau_draw_rndr_aa_smpl_x7           vau_draw_rndr_aa_smpl_x7;            /* a28 */
    u_vau_draw_rndr_aa_smpl_y0           vau_draw_rndr_aa_smpl_y0;            /* a2c */
    u_vau_draw_rndr_aa_smpl_y1           vau_draw_rndr_aa_smpl_y1;            /* a30 */
    u_vau_draw_rndr_aa_smpl_y2           vau_draw_rndr_aa_smpl_y2;            /* a34 */
    u_vau_draw_rndr_aa_smpl_y3           vau_draw_rndr_aa_smpl_y3;            /* a38 */
    u_vau_draw_rndr_aa_smpl_y4           vau_draw_rndr_aa_smpl_y4;            /* a3c */
    u_vau_draw_rndr_aa_smpl_y5           vau_draw_rndr_aa_smpl_y5;            /* a40 */
    u_vau_draw_rndr_aa_smpl_y6           vau_draw_rndr_aa_smpl_y6;            /* a44 */
    u_vau_draw_rndr_aa_smpl_y7           vau_draw_rndr_aa_smpl_y7;            /* a48 */
    u_vau_draw_cvrg_weight0              vau_draw_cvrg_weight0;               /* a4c */
    u_vau_draw_cvrg_weight1              vau_draw_cvrg_weight1;               /* a50 */
    u_vau_draw_cmdbuf_start_addr         vau_draw_cmdbuf_start_addr;          /* a54 */
    u_vau_draw_cmdbuf_end_addr           vau_draw_cmdbuf_end_addr;            /* a58 */
    u_vau_draw_pathbuf_start_addr        vau_draw_pathbuf_start_addr;         /* a5c */
    u_vau_draw_pathbuf_end_addr          vau_draw_pathbuf_end_addr;           /* a60 */
    unsigned int                         rsr20[39];                           /* a64 ~ afc */
    u_vau_sram_ctrl                      vau_sram_ctrl;                       /* b00 */
    unsigned int                         rsr21[3391];                         /* b04 ~ 3ffc */
    u_vau_work_state                     vau_work_state;                      /* 4000 */
    u_vau_debug1                         vau_debug1;                          /* 4004 */
    u_vau_debug2                         vau_debug2;                          /* 4008 */
    u_vau_debug3                         vau_debug3;                          /* 400c */
    u_vau_debug4                         vau_debug4;                          /* 4010 */
    u_vau_debug5                         vau_debug5;                          /* 4014 */
    u_vau_debug6                         vau_debug6;                          /* 4018 */
    u_vau_debug7                         vau_debug7;                          /* 401c */
    u_vau_debug8                         vau_debug8;                          /* 4020 */
    u_vau_debug9                         vau_debug9;                          /* 4024 */
    u_vau_debug10                        vau_debug10;                         /* 4028 */
    u_vau_debug11                        vau_debug11;                         /* 402c */
    u_vau_debug12                        vau_debug12;                         /* 4030 */
    u_vau_debug13                        vau_debug13;                         /* 4034 */
    u_vau_debug14                        vau_debug14;                         /* 4038 */
    u_vau_debug15                        vau_debug15;                         /* 403c */
    u_vau_debug16                        vau_debug16;                         /* 4040 */
    u_vau_debug17                        vau_debug17;                         /* 4044 */
    u_vau_debug18                        vau_debug18;                         /* 4048 */
    u_vau_debug19                        vau_debug19;                         /* 404c */
    u_vau_debug20                        vau_debug20;                         /* 4050 */
    u_vau_debug21                        vau_debug21;                         /* 4054 */
    u_vau_debug22                        vau_debug22;                         /* 4058 */
    u_vau_debug23                        vau_debug23;                         /* 405c */
    u_vau_debug24                        vau_debug24;                         /* 4060 */
    u_vau_debug25                        vau_debug25;                         /* 4064 */
    u_vau_debug26                        vau_debug26;                         /* 4068 */
    unsigned int                         rsr22[11];                           /* 406c ~ 4094 */
    u_vau_debug38                        vau_debug38;                         /* 4098 */
    u_vau_debug39                        vau_debug39;                         /* 409c */
    u_vau_pfcnt_list                     vau_pfcnt_list;                      /* 40a0 */
    u_vau_tunl_sta                       vau_tunl_sta;                        /* 40a4 */
    u_vau_tunl_wr_ptr                    vau_tunl_wr_ptr;                     /* 40a8 */
    u_vau_tunl_rd_ptr                    vau_tunl_rd_ptr;                     /* 40ac */
} vau_regs_type;

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif /* DRV_GRAPHIC_HAL_VAU_REG_DEFINE_H */

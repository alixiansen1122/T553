/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: jpeg reg
 */

#ifndef DRV_GRAPHICS_JPEG_REG_DEFINE_H
#define DRV_GRAPHICS_JPEG_REG_DEFINE_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*-------------------------------- macro define -------------------------------------------*/
#define JPEG_REG_BASEADDR          (0x56001000)
#define JPEG_REG_LENGTH            (0x6F0)

/*-------------------------------- struct define ------------------------------------------*/

/* Define the union jpeg_dec_start */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int jpeg_dec_start          : 1;       /* [0] */
        unsigned int rst_busy                : 1;       /* [1] */
        unsigned int axi_4k_bypass           : 1;       /* [2] */
        unsigned int jcfg2idct_lowdly_en     : 1;       /* [3] */
        unsigned int axi_sep_typ             : 1;       /* [4] */
        unsigned int reserved                : 27;      /* [31:5] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_jpeg_dec_start;

/* Define the union jpeg_resume_start */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int jpeg_resume_start       : 1;       /* [0] */
        unsigned int last_resume_in_pic      : 1;       /* [1] */
        unsigned int reserved                : 30;      /* [31:2] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_jpeg_resume_start;

/* Define the union pic_vld_num */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pic_vld_num             : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_pic_vld_num;

/* Define the union jpeg_stride */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int y_stride                : 16;      /* [15:0] */
        unsigned int uv_stride               : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_jpeg_stride;

/* Define the union picture_size */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pic_width_in_mcu        : 12;      /* [11:0] */
        unsigned int reserved0               : 4;       /* [15:12] */
        unsigned int pic_height_in_mcu       : 12;      /* [27:16] */
        unsigned int reserved1               : 4;       /* [31:28] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_picture_size;

/* Define the union picture_type */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pic_type                : 3;       /* [2:0] */
        unsigned int reserved                : 29;      /* [31:3] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_picture_type;

/* Define the union picdec_time */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int picdec_time             : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_picdec_time;

/* Define the union rgb_out_stride */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int jcfg2jdo_out_stride     : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_rgb_out_stride;

/* Define the union bitbuffer_staddr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved                : 6;       /* [5:0] */
        unsigned int bb_staddr               : 26;      /* [31:6] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_bitbuffer_staddr;

/* Define the union bitbuffer_endaddr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved                : 6;       /* [5:0] */
        unsigned int bb_endaddr              : 26;      /* [31:6] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_bitbuffer_endaddr;

/* Define the union bitstreams_staddr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int bs_staddr               : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_bitstreams_staddr;

/* Define the union bitstreams_endaddr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int bs_endaddr              : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_bitstreams_endaddr;

/* Define the union picture_ystaddr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pic_ystaddr             : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_picture_ystaddr;

/* Define the union picture_uvstaddr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pic_uvstaddr            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_picture_uvstaddr;

/* Define the union pd_sum_cfg_cbcr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int jcfg2jvld_pd_cb         : 16;      /* [15:0] */
        unsigned int jcfg2jvld_pd_cr         : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_pd_sum_cfg_cbcr;

/* Define the union pd_sum_cfg_y */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int jcfg2jvld_pd_y          : 16;      /* [15:0] */
        unsigned int jcfg2jbs_flag_cur       : 1;       /* [16] */
        unsigned int reserved                : 15;      /* [31:17] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_pd_sum_cfg_y;

/* Define the union freq_scale */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int freq_scale              : 2;       /* [1:0] */
        unsigned int jidct_emar_en           : 1;       /* [2] */
        unsigned int ck_gt_en                : 1;       /* [3] */
        unsigned int outstanding             : 4;       /* [7:4] */
        unsigned int axi_id                  : 4;       /* [11:8] */
        unsigned int reserved                : 20;      /* [31:12] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_freq_scale;

/* Define the union out_type */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int out_rgb_type_old        : 2;       /* [1:0] */
        unsigned int jidct_yuv420_en         : 1;       /* [2] */
        unsigned int hor_med_en              : 1;       /* [3] */
        unsigned int ver_med_en              : 1;       /* [4] */
        unsigned int dither_en               : 1;       /* [5] */
        unsigned int uv_reserved             : 1;       /* [6] */
        unsigned int reserved0               : 1;       /* [7] */
        unsigned int out_rgb_type            : 3;       /* [10:8] */
        unsigned int reserved1               : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_out_type;

/* Define the union jpgd_alpha */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int jpgd_a                  : 8;       /* [7:0] */
        unsigned int reserved                : 24;      /* [31:8] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_jpgd_alpha;

/* Define the union hor_phase0_coef01 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hor_phase0_coef0        : 10;      /* [9:0] */
        unsigned int reserved0               : 6;       /* [15:10] */
        unsigned int hor_phase0_coef1        : 10;      /* [25:16] */
        unsigned int reserved1               : 6;       /* [31:26] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hor_phase0_coef01;

/* Define the union hor_phase0_coef23 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hor_phase0_coef2        : 10;      /* [9:0] */
        unsigned int reserved0               : 6;       /* [15:10] */
        unsigned int hor_phase0_coef3        : 10;      /* [25:16] */
        unsigned int reserved1               : 6;       /* [31:26] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hor_phase0_coef23;

/* Define the union hor_phase0_coef45 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hor_phase0_coef4        : 10;      /* [9:0] */
        unsigned int reserved0               : 6;       /* [15:10] */
        unsigned int hor_phase0_coef5        : 10;      /* [25:16] */
        unsigned int reserved1               : 6;       /* [31:26] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hor_phase0_coef45;

/* Define the union hor_phase0_coef67 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hor_phase0_coef6        : 10;      /* [9:0] */
        unsigned int reserved0               : 6;       /* [15:10] */
        unsigned int hor_phase0_coef7        : 10;      /* [25:16] */
        unsigned int reserved1               : 6;       /* [31:26] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hor_phase0_coef67;

/* Define the union hor_phase2_coef01 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hor_phase2_coef0        : 10;      /* [9:0] */
        unsigned int reserved0               : 6;       /* [15:10] */
        unsigned int hor_phase2_coef1        : 10;      /* [25:16] */
        unsigned int reserved1               : 6;       /* [31:26] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hor_phase2_coef01;

/* Define the union hor_phase2_coef23 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hor_phase2_coef2        : 10;      /* [9:0] */
        unsigned int reserved0               : 6;       /* [15:10] */
        unsigned int hor_phase2_coef3        : 10;      /* [25:16] */
        unsigned int reserved1               : 6;       /* [31:26] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hor_phase2_coef23;

/* Define the union hor_phase2_coef45 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hor_phase2_coef4        : 10;      /* [9:0] */
        unsigned int reserved0               : 6;       /* [15:10] */
        unsigned int hor_phase2_coef5        : 10;      /* [25:16] */
        unsigned int reserved1               : 6;       /* [31:26] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hor_phase2_coef45;

/* Define the union hor_phase2_coef67 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hor_phase2_coef6        : 10;      /* [9:0] */
        unsigned int reserved0               : 6;       /* [15:10] */
        unsigned int hor_phase2_coef7        : 10;      /* [25:16] */
        unsigned int reserved1               : 6;       /* [31:26] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hor_phase2_coef67;

/* Define the union ver_phase0_coef01 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ver_phase0_coef0        : 10;      /* [9:0] */
        unsigned int reserved0               : 6;       /* [15:10] */
        unsigned int ver_phase0_coef1        : 10;      /* [25:16] */
        unsigned int reserved1               : 6;       /* [31:26] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_ver_phase0_coef01;

/* Define the union ver_phase0_coef23 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ver_phase0_coef2        : 10;      /* [9:0] */
        unsigned int reserved0               : 6;       /* [15:10] */
        unsigned int ver_phase0_coef3        : 10;      /* [25:16] */
        unsigned int reserved1               : 6;       /* [31:26] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_ver_phase0_coef23;

/* Define the union ver_phase2_coef01 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ver_phase2_coef0        : 10;      /* [9:0] */
        unsigned int reserved0               : 6;       /* [15:10] */
        unsigned int ver_phase2_coef1        : 10;      /* [25:16] */
        unsigned int reserved1               : 6;       /* [31:26] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_ver_phase2_coef01;

/* Define the union ver_phase2_coef23 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ver_phase2_coef2        : 10;      /* [9:0] */
        unsigned int reserved0               : 6;       /* [15:10] */
        unsigned int ver_phase2_coef3        : 10;      /* [25:16] */
        unsigned int reserved1               : 6;       /* [31:26] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_ver_phase2_coef23;

/* Define the union csc_in_dc_coef */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_in_dc_coef0         : 9;       /* [8:0] */
        unsigned int reserved0               : 1;       /* [9] */
        unsigned int csc_in_dc_coef1         : 9;       /* [18:10] */
        unsigned int reserved1               : 1;       /* [19] */
        unsigned int csc_in_dc_coef2         : 9;       /* [28:20] */
        unsigned int reserved2               : 3;       /* [31:29] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_csc_in_dc_coef;

/* Define the union csc_out_dc_coef */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_out_dc_coef0        : 9;       /* [8:0] */
        unsigned int reserved0               : 1;       /* [9] */
        unsigned int csc_out_dc_coef1        : 9;       /* [18:10] */
        unsigned int reserved1               : 1;       /* [19] */
        unsigned int csc_out_dc_coef2        : 9;       /* [28:20] */
        unsigned int reserved2               : 3;       /* [31:29] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_csc_out_dc_coef;

/* Define the union csc_trans_coef0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_p_00                : 15;      /* [14:0] */
        unsigned int reserved0               : 1;       /* [15] */
        unsigned int csc_p_01                : 15;      /* [30:16] */
        unsigned int reserved1               : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_csc_trans_coef0;

/* Define the union csc_trans_coef1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_p_02                : 15;      /* [14:0] */
        unsigned int reserved0               : 1;       /* [15] */
        unsigned int csc_p_10                : 15;      /* [30:16] */
        unsigned int reserved1               : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_csc_trans_coef1;

/* Define the union csc_trans_coef2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_p_11                : 15;      /* [14:0] */
        unsigned int reserved0               : 1;       /* [15] */
        unsigned int csc_p_12                : 15;      /* [30:16] */
        unsigned int reserved1               : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_csc_trans_coef2;

/* Define the union csc_trans_coef3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_p_20                : 15;      /* [14:0] */
        unsigned int reserved0               : 1;       /* [15] */
        unsigned int csc_p_21                : 15;      /* [30:16] */
        unsigned int reserved1               : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_csc_trans_coef3;

/* Define the union csc_trans_coef4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_p_22                : 15;      /* [14:0] */
        unsigned int reserved                : 17;      /* [31:15] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_csc_trans_coef4;

/* Define the union mtn_address0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mtn_address0            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_mtn_address0;

/* Define the union mtn_address1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mtn_address1            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_mtn_address1;

/* Define the union mcu_pro_startpos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int jcfg2jvld_mcu_starty    : 10;      /* [9:0] */
        unsigned int reserved                : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_mcu_pro_startpos;

/* Define the union piccut_startpos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int jcfg2jytr_pic_startx    : 13;      /* [12:0] */
        unsigned int reserved0               : 3;       /* [15:13] */
        unsigned int jcfg2jytr_pic_starty    : 13;      /* [28:16] */
        unsigned int reserved1               : 3;       /* [31:29] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_piccut_startpos;

/* Define the union piccut_endpos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int jcfg2jytr_pic_endx      : 13;      /* [12:0] */
        unsigned int reserved0               : 3;       /* [15:13] */
        unsigned int jcfg2jytr_pic_endy      : 13;      /* [28:16] */
        unsigned int reserved1               : 3;       /* [31:29] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_piccut_endpos;

/* Define the union mcu_pro_curpos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int jvld2jcfg_mcu_y         : 10;      /* [9:0] */
        unsigned int reserved                : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_mcu_pro_curpos;

/* Define the union bs_consu */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int jvld2jcfg_bs_consu      : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_bs_consu;

/* Define the union bs_resume_data0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int jbs2jcfg_bs_data0       : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_bs_resume_data0;

/* Define the union bs_resume_data1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int jbs2jcfg_bs_data1       : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_bs_resume_data1;

/* Define the union bs_resume_bit */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int jbs2jcfg_bs_remain      : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_bs_resume_bit;

/* Define the union bs_res_data_cfg0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int jcfg2jbs_bs_data0       : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_bs_res_data_cfg0;

/* Define the union bs_res_data_cfg1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int jcfg2jbs_bs_data1       : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_bs_res_data_cfg1;

/* Define the union bs_res_bit_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int jcfg2jbs_bs_remain      : 7;       /* [6:0] */
        unsigned int reserved                : 25;      /* [31:7] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_bs_res_bit_cfg;

/* Define the union jpeg_int */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int int_dec_fnsh            : 1;       /* [0] */
        unsigned int int_dec_err             : 1;       /* [1] */
        unsigned int int_bs_res              : 1;       /* [2] */
        unsigned int debug_state             : 1;       /* [3] */
        unsigned int int_over_time           : 1;       /* [4] */
        unsigned int int_line_eql            : 1;       /* [5] */
        unsigned int reserved                : 26;      /* [31:6] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_jpeg_int;

/* Define the union int_mask */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int intm_dec_fnsh           : 1;       /* [0] */
        unsigned int intm_dec_err            : 1;       /* [1] */
        unsigned int intm_bs_res             : 1;       /* [2] */
        unsigned int reserved0               : 1;       /* [3] */
        unsigned int intm_over_time          : 1;       /* [4] */
        unsigned int intm_line_eql           : 1;       /* [5] */
        unsigned int reserved1               : 26;      /* [31:6] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_int_mask;

/* Define the union over_time_thd */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int over_time_thr           : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_over_time_thd;

/* Define the union pd_sum_y */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int jvld2jcfg_pd_y          : 16;      /* [15:0] */
        unsigned int jbs2jcfg_flag_cur       : 1;       /* [16] */
        unsigned int reserved                : 15;      /* [31:17] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_pd_sum_y;

/* Define the union pd_sum_cbcr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int jvld2jcfg_pd_cb         : 16;      /* [15:0] */
        unsigned int jvld2jcfg_pd_cr         : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_pd_sum_cbcr;

/* Define the union luma_pix_sum0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int luma_pix_sum0           : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_luma_pix_sum0;

/* Define the union luma_pix_sum1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int luma_pix_sum1           : 4;       /* [3:0] */
        unsigned int reserved                : 27;      /* [30:4] */
        unsigned int jidct_luma_sum_en       : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_luma_pix_sum1;

/* Define the union axi_4k_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int wr_over_4k_cnt          : 16;      /* [15:0] */
        unsigned int rd_over_4k_cnt          : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_axi_4k_cnt;

/* Define the union axi_16m_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int srcw_cur_fsm            : 2;       /* [1:0] */
        unsigned int srcr_cur_fsm            : 2;       /* [3:2] */
        unsigned int wr_cnt                  : 4;       /* [7:4] */
        unsigned int rd_cnt                  : 4;       /* [11:8] */
        unsigned int wr_over_16m_cnt         : 10;      /* [21:12] */
        unsigned int rd_over_16m_cnt         : 10;      /* [31:22] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_axi_16m_cnt;

/* Define the union sampling_factor */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int v_fac                   : 8;       /* [7:0] */
        unsigned int u_fac                   : 8;       /* [15:8] */
        unsigned int y_fac                   : 8;       /* [23:16] */
        unsigned int reserved                : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_sampling_factor;

/* Define the union dri */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int jcfg2jvld_ri            : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dri;

/* Define the union dri_cnt_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int jcfg2jvld_dri_cnt       : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dri_cnt_cfg;

/* Define the union dri_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int jvld2jcfg_dri_cnt       : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dri_cnt;

/* Define the union mmu_bypass_chn */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ar_bs_bypass            : 1;       /* [0] */
        unsigned int aw_idct_y_bypass        : 1;       /* [1] */
        unsigned int aw_idct_c_bypass        : 1;       /* [2] */
        unsigned int ar_mtn0_bypass          : 1;       /* [3] */
        unsigned int ar_mtn1_bypass          : 1;       /* [4] */
        unsigned int aw_mtn0_bypass          : 1;       /* [5] */
        unsigned int aw_mtn1_bypass          : 1;       /* [6] */
        unsigned int aw_rgb_bypass           : 1;       /* [7] */
        unsigned int aw_tunl_bypass          : 1;       /* [8] */
        unsigned int reserved                : 23;      /* [31:9] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_mmu_bypass_chn;

/* Define the union jpgd_line_num */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int jcfg2idct_line_num      : 10;      /* [9:0] */
        unsigned int reserved                : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_jpgd_line_num;

/* Define the union jpgd_line_cnt_addr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int jcfg2idct_line_cnt_addr : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_jpgd_line_cnt_addr;

/* Define the union mem_ctrl_ras_rfs */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mem_ctrl_rfs            : 16;      /* [15:0] */
        unsigned int mem_ctrl_ras            : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_mem_ctrl_ras_rfs;

/* Define the union mem_ctrl_rft */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mem_ctrl_rft            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_mem_ctrl_rft;

/* Define the union bitbuffer_staddr_msb */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int bb_staddr_msb           : 2;       /* [1:0] */
        unsigned int reserved                : 30;      /* [31:2] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_bitbuffer_staddr_msb;

/* Define the union bitbuffer_endaddr_msb */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int bb_endaddr_msb          : 2;       /* [1:0] */
        unsigned int reserved                : 30;      /* [31:2] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_bitbuffer_endaddr_msb;

/* Define the union bitstream_staddr_msb */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int bs_staddr_msb           : 2;       /* [1:0] */
        unsigned int reserved                : 30;      /* [31:2] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_bitstream_staddr_msb;

/* Define the union bitstream_endaddr_msb */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int bs_endaddr_msb          : 2;       /* [1:0] */
        unsigned int reserved                : 30;      /* [31:2] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_bitstream_endaddr_msb;

/* Define the union picture_ystaddr_msb */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pic_ystaddr_msb         : 2;       /* [1:0] */
        unsigned int reserved                : 30;      /* [31:2] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_picture_ystaddr_msb;

/* Define the union picture_uvstaddr_msb */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pic_uvstaddr_msb        : 2;       /* [1:0] */
        unsigned int reserved                : 30;      /* [31:2] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_picture_uvstaddr_msb;

/* Define the union mtn_address0_msb */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mtn_address0_msb        : 2;       /* [1:0] */
        unsigned int reserved                : 30;      /* [31:2] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_mtn_address0_msb;

/* Define the union mtn_address1_msb */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mtn_address1_msb        : 2;       /* [1:0] */
        unsigned int reserved                : 30;      /* [31:2] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_mtn_address1_msb;

/* Define the union out_height_pix */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int yout_height_pix         : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_out_height_pix;

/* Define the union jpgd_dfx */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dc_err                  : 1;       /* [0] */
        unsigned int ac_err                  : 1;       /* [1] */
        unsigned int dri_err                 : 1;       /* [2] */
        unsigned int bs_eat_over             : 1;       /* [3] */
        unsigned int reserved                : 28;      /* [31:4] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_jpgd_dfx;

/* Define the union quant_table */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int qtab_y                  : 8;       /* [7:0] */
        unsigned int qtab_cb                 : 8;       /* [15:8] */
        unsigned int qtab_cr                 : 8;       /* [23:16] */
        unsigned int reserved                : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_quant_table;

/* Define the union hdc_table */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hdc_lu                  : 12;      /* [11:0] */
        unsigned int hdc_ch                  : 12;      /* [23:12] */
        unsigned int reserved                : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hdc_table;

/* Define the union hac_min_table */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hac_lu_mincode_even     : 8;       /* [7:0] */
        unsigned int hac_lu_mincode_odd      : 8;       /* [15:8] */
        unsigned int hac_ch_mincode_even     : 8;       /* [23:16] */
        unsigned int hac_ch_mincode_odd      : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hac_min_table;

/* Define the union hac_base_table */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hac_lu_base_even        : 8;       /* [7:0] */
        unsigned int hac_lu_base_odd         : 8;       /* [15:8] */
        unsigned int hac_ch_base_even        : 8;       /* [23:16] */
        unsigned int hac_ch_base_odd         : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hac_base_table;

/* Define the union hac_symbol_table */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int rs_luma                 : 8;       /* [7:0] */
        unsigned int rs_chroma               : 8;       /* [15:8] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_hac_symbol_table;

typedef struct {
    u_jpeg_dec_start                     jpeg_dec_start;                      /* 0 */
    u_jpeg_resume_start                  jpeg_resume_start;                   /* 4 */
    u_pic_vld_num                        pic_vld_num;                         /* 8 */
    u_jpeg_stride                        jpeg_stride;                         /* c */
    u_picture_size                       picture_size;                        /* 10 */
    u_picture_type                       picture_type;                        /* 14 */
    u_picdec_time                        picdec_time;                         /* 18 */
    u_rgb_out_stride                     rgb_out_stride;                      /* 1c */
    u_bitbuffer_staddr                   bitbuffer_staddr;                    /* 20 */
    u_bitbuffer_endaddr                  bitbuffer_endaddr;                   /* 24 */
    u_bitstreams_staddr                  bitstreams_staddr;                   /* 28 */
    u_bitstreams_endaddr                 bitstreams_endaddr;                  /* 2c */
    u_picture_ystaddr                    picture_ystaddr;                     /* 30 */
    u_picture_uvstaddr                   picture_uvstaddr;                    /* 34 */
    u_pd_sum_cfg_cbcr                    pd_sum_cfg_cbcr;                     /* 38 */
    u_pd_sum_cfg_y                       pd_sum_cfg_y;                        /* 3c */
    u_freq_scale                         freq_scale;                          /* 40 */
    u_out_type                           out_type;                            /* 44 */
    u_jpgd_alpha                         jpgd_alpha;                          /* 48 */
    u_hor_phase0_coef01                  hor_phase0_coef01;                   /* 4c */
    u_hor_phase0_coef23                  hor_phase0_coef23;                   /* 50 */
    u_hor_phase0_coef45                  hor_phase0_coef45;                   /* 54 */
    u_hor_phase0_coef67                  hor_phase0_coef67;                   /* 58 */
    unsigned int                         rsr0[4];                             /* 5c ~ 68 */
    u_hor_phase2_coef01                  hor_phase2_coef01;                   /* 6c */
    u_hor_phase2_coef23                  hor_phase2_coef23;                   /* 70 */
    u_hor_phase2_coef45                  hor_phase2_coef45;                   /* 74 */
    u_hor_phase2_coef67                  hor_phase2_coef67;                   /* 78 */
    unsigned int                         rsr1[4];                             /* 7c ~ 88 */
    u_ver_phase0_coef01                  ver_phase0_coef01;                   /* 8c */
    u_ver_phase0_coef23                  ver_phase0_coef23;                   /* 90 */
    unsigned int                         rsr2[2];                             /* 94 ~ 98 */
    u_ver_phase2_coef01                  ver_phase2_coef01;                   /* 9c */
    u_ver_phase2_coef23                  ver_phase2_coef23;                   /* a0 */
    unsigned int                         rsr3[2];                             /* a4 ~ a8 */
    u_csc_in_dc_coef                     csc_in_dc_coef;                      /* ac */
    u_csc_out_dc_coef                    csc_out_dc_coef;                     /* b0 */
    u_csc_trans_coef0                    csc_trans_coef0;                     /* b4 */
    u_csc_trans_coef1                    csc_trans_coef1;                     /* b8 */
    u_csc_trans_coef2                    csc_trans_coef2;                     /* bc */
    u_csc_trans_coef3                    csc_trans_coef3;                     /* c0 */
    u_csc_trans_coef4                    csc_trans_coef4;                     /* c4 */
    u_mtn_address0                       mtn_address0;                        /* c8 */
    u_mtn_address1                       mtn_address1;                        /* cc */
    unsigned int                         rsr4;                                /* d0 */
    u_mcu_pro_startpos                   mcu_pro_startpos;                    /* d4 */
    u_piccut_startpos                    piccut_startpos;                     /* d8 */
    u_piccut_endpos                      piccut_endpos;                       /* dc */
    u_mcu_pro_curpos                     mcu_pro_curpos;                      /* e0 */
    u_bs_consu                           bs_consu;                            /* e4 */
    u_bs_resume_data0                    bs_resume_data0;                     /* e8 */
    u_bs_resume_data1                    bs_resume_data1;                     /* ec */
    u_bs_resume_bit                      bs_resume_bit;                       /* f0 */
    u_bs_res_data_cfg0                   bs_res_data_cfg0;                    /* f4 */
    u_bs_res_data_cfg1                   bs_res_data_cfg1;                    /* f8 */
    u_bs_res_bit_cfg                     bs_res_bit_cfg;                      /* fc */
    u_jpeg_int                           jpeg_int;                            /* 100 */
    u_int_mask                           int_mask;                            /* 104 */
    u_over_time_thd                      over_time_thd;                       /* 108 */
    u_pd_sum_y                           pd_sum_y;                            /* 10c */
    u_pd_sum_cbcr                        pd_sum_cbcr;                         /* 110 */
    u_luma_pix_sum0                      luma_pix_sum0;                       /* 114 */
    u_luma_pix_sum1                      luma_pix_sum1;                       /* 118 */
    u_axi_4k_cnt                         axi_4k_cnt;                          /* 11c */
    u_axi_16m_cnt                        axi_16m_cnt;                         /* 120 */
    u_sampling_factor                    sampling_factor;                     /* 124 */
    u_dri                                dri;                                 /* 128 */
    u_dri_cnt_cfg                        dri_cnt_cfg;                         /* 12c */
    u_dri_cnt                            dri_cnt;                             /* 130 */
    u_mmu_bypass_chn                     mmu_bypass_chn;                      /* 134 */
    u_jpgd_line_num                      jpgd_line_num;                       /* 138 */
    u_jpgd_line_cnt_addr                 jpgd_line_cnt_addr;                  /* 13c */
    u_mem_ctrl_ras_rfs                   mem_ctrl_ras_rfs;                    /* 140 */
    u_mem_ctrl_rft                       mem_ctrl_rft;                        /* 144 */
    u_bitbuffer_staddr_msb               bitbuffer_staddr_msb;                /* 148 */
    u_bitbuffer_endaddr_msb              bitbuffer_endaddr_msb;               /* 14c */
    u_bitstream_staddr_msb               bitstream_staddr_msb;                /* 150 */
    u_bitstream_endaddr_msb              bitstream_endaddr_msb;               /* 154 */
    u_picture_ystaddr_msb                picture_ystaddr_msb;                 /* 158 */
    u_picture_uvstaddr_msb               picture_uvstaddr_msb;                /* 15c */
    u_mtn_address0_msb                   mtn_address0_msb;                    /* 160 */
    u_mtn_address1_msb                   mtn_address1_msb;                    /* 164 */
    u_out_height_pix                     out_height_pix;                      /* 168 */
    u_jpgd_dfx                           jpgd_dfx;                            /* 16c */
    unsigned int                         rsr5[36];                            /* 170 ~ 1fc */
    u_quant_table                        quant_table[64];                     /* 200 ~ 2fc */
    u_hdc_table                          hdc_table[12];                       /* 300 ~ 32c */
    unsigned int                         rsr6[4];                             /* 330 ~ 33c */
    u_hac_min_table                      hac_min_table[8];                    /* 340 ~ 35c */
    u_hac_base_table                     hac_base_table[8];                   /* 360 ~ 37c */
    unsigned int                         rsr7[32];                            /* 380 ~ 3fc */
    u_hac_symbol_table                   hac_symbol_table[256];               /* 400 ~ 7fc */
} jpeg_regs_struct, *jpeg_regs_ptr;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* DRV_GRAPHICS_JPEG_REG_DEFINE_H */

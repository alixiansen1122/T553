/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description : dpu reg define file
 */

#ifndef DRV_GRAPHIC_HAL_DPU_REG_DEFINE_H
#define DRV_GRAPHIC_HAL_DPU_REG_DEFINE_H

#include "td_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define dpu_reg_read(addr)         (*(volatile unsigned int *)(uintptr_t)(addr))
#define dpu_reg_write(addr, val)   (*(volatile unsigned int *)(uintptr_t)(addr) = (val))

/* Define the union voctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved0               : 26;      /* [25:0] */
        unsigned int g1_ck_gt_en             : 1;       /* [26] */
        unsigned int g0_ck_gt_en             : 1;       /* [27] */
        unsigned int reserved1               : 2;       /* [29:28] */
        unsigned int intf_chk_sum_en         : 1;       /* [30] */
        unsigned int vo_ck_gt_en             : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_voctrl;

/* Define the union vointsta */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd0vtthd1_int          : 1;       /* [0] */
        unsigned int dhd0vtthd2_int          : 1;       /* [1] */
        unsigned int dhd0vtthd3_int          : 1;       /* [2] */
        unsigned int dhd0uf_int              : 1;       /* [3] */
        unsigned int reserved0               : 8;       /* [11:4] */
        unsigned int bus_err_int             : 1;       /* [12] */
        unsigned int reserved1               : 7;       /* [19:13] */
        unsigned int dhd0_te_int             : 1;       /* [20] */
        unsigned int dhd0_frm_end_int        : 1;       /* [21] */
        unsigned int reserved2               : 10;      /* [31:22] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vointsta;

/* Define the union vomskintsta */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd0vtthd1_clr          : 1;       /* [0] */
        unsigned int dhd0vtthd2_clr          : 1;       /* [1] */
        unsigned int dhd0vtthd3_clr          : 1;       /* [2] */
        unsigned int dhd0uf_clr              : 1;       /* [3] */
        unsigned int reserved0               : 8;       /* [11:4] */
        unsigned int bus_err_clr             : 1;       /* [12] */
        unsigned int reserved1               : 7;       /* [19:13] */
        unsigned int dhd0_te_clr             : 1;       /* [20] */
        unsigned int dhd0_frm_end_clr        : 1;       /* [21] */
        unsigned int reserved2               : 10;      /* [31:22] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vomskintsta;

/* Define the union vointmsk */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd0vtthd1_intmask      : 1;       /* [0] */
        unsigned int dhd0vtthd2_intmask      : 1;       /* [1] */
        unsigned int dhd0vtthd3_intmask      : 1;       /* [2] */
        unsigned int dhd0uf_intmask          : 1;       /* [3] */
        unsigned int reserved0               : 8;       /* [11:4] */
        unsigned int bus_err_intmask         : 1;       /* [12] */
        unsigned int reserved1               : 7;       /* [19:13] */
        unsigned int dhd0_te_intmask         : 1;       /* [20] */
        unsigned int dhd0_frm_end_intmask    : 1;       /* [21] */
        unsigned int reserved2               : 10;      /* [31:22] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vointmsk;

/* Define the union vodebug */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int rm_en_chn               : 4;       /* [3:0] */
        unsigned int dhd0_ff_info            : 2;       /* [5:4] */
        unsigned int dhd1_ff_info            : 2;       /* [7:6] */
        unsigned int reserved                : 24;      /* [31:8] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vodebug;

/* Define the union vocmd_intsta */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd0_vocmd_int          : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vocmd_intsta;

/* Define the union vocmd_mskintsta */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd0_vocmd_clr          : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vocmd_mskintsta;

/* Define the union vocmd_intmsk */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd0_vocmd_intmask      : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vocmd_intmsk;

/* Define the union voversion1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int version1                : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_voversion1;

/* Define the union voversion2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int version2                : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_voversion2;

/* Define the union volowpower_ctrl */
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
} u_volowpower_ctrl;

/* Define the union voufsta */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved0               : 8;       /* [7:0] */
        unsigned int g0_uf_sta               : 1;       /* [8] */
        unsigned int g1_uf_sta               : 1;       /* [9] */
        unsigned int reserved1               : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_voufsta;

/* Define the union voufclr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved0               : 8;       /* [7:0] */
        unsigned int g0_uf_clr               : 1;       /* [8] */
        unsigned int g1_uf_clr               : 1;       /* [9] */
        unsigned int reserved1               : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_voufclr;

/* Define the union vointproc_tim */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vointproc_time          : 24;      /* [23:0] */
        unsigned int reserved                : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vointproc_tim;

/* Define the union vofpgatest */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved                : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vofpgatest;

/* Define the union volowpower_ctrl_09v */
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
} u_volowpower_ctrl_09v;

/* Define the union volowpower_ctrl1_09v */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int lgrfthd_emab_09v        : 3;       /* [2:0] */
        unsigned int lgrfthd_emasa_09v       : 1;       /* [3] */
        unsigned int lgrfthd_ret1n_09v       : 1;       /* [4] */
        unsigned int lgrfthd_stov_09v        : 1;       /* [5] */
        unsigned int reserved                : 26;      /* [31:6] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_volowpower_ctrl1_09v;

/* Define the union volowpower_ctrl1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int lgrfthd_emab            : 3;       /* [2:0] */
        unsigned int lgrfthd_emasa           : 1;       /* [3] */
        unsigned int lgrfthd_ret1n           : 1;       /* [4] */
        unsigned int lgrfthd_stov            : 1;       /* [5] */
        unsigned int reserved                : 26;      /* [31:6] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_volowpower_ctrl1;

/* Define the union dpu_sram_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dpu_block_en            : 10;      /* [9:0] */
        unsigned int reserved                : 20;      /* [29:10] */
        unsigned int dpu_lp_mode             : 2;       /* [31:30] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dpu_sram_ctrl;

/* Define the union cbm_bkg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cbm_bkgcr               : 8;       /* [7:0] */
        unsigned int cbm_bkgcb               : 8;       /* [15:8] */
        unsigned int cbm_bkgy                : 8;       /* [23:16] */
        unsigned int reserved                : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_cbm_bkg;

/* Define the union cbm_mix */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mixer_prio0             : 4;       /* [3:0] */
        unsigned int mixer_prio1             : 4;       /* [7:4] */
        unsigned int mixer_prio2             : 4;       /* [11:8] */
        unsigned int mixer_prio3             : 4;       /* [15:12] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_cbm_mix;

/* Define the union cbm_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved0               : 1;       /* [0] */
        unsigned int layer0_bypass_en        : 1;       /* [1] */
        unsigned int reserved1               : 13;      /* [14:2] */
        unsigned int bypass_mode             : 1;       /* [15] */
        unsigned int reserved2               : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_cbm_ctrl;

/* Define the union dpu_partial_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dpu_partial_cfg_vld     : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dpu_partial_ctrl;

/* Define the union dpu_partial_gen_hdr0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int command0_datatype       : 6;       /* [5:0] */
        unsigned int command0_virtualchannel : 2;       /* [7:6] */
        unsigned int command0_wordcount_low  : 8;       /* [15:8] */
        unsigned int command0_wordcount_hi   : 8;       /* [23:16] */
        unsigned int reserved                : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dpu_partial_gen_hdr0;

/* Define the union dpu_partial_gen_pld_data00 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int command0_pld_b1         : 8;       /* [7:0] */
        unsigned int command0_pld_b2         : 8;       /* [15:8] */
        unsigned int command0_pld_b3         : 8;       /* [23:16] */
        unsigned int command0_pld_b4         : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dpu_partial_gen_pld_data00;

/* Define the union dpu_partial_gen_pld_data01 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int command0_pld_b5         : 8;       /* [7:0] */
        unsigned int command0_pld_b6         : 8;       /* [15:8] */
        unsigned int command0_pld_b7         : 8;       /* [23:16] */
        unsigned int command0_pld_b8         : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dpu_partial_gen_pld_data01;

/* Define the union dpu_partial_gen_hdr1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int command1_datatype       : 6;       /* [5:0] */
        unsigned int command1_virtualchannel : 2;       /* [7:6] */
        unsigned int command1_wordcount_low  : 8;       /* [15:8] */
        unsigned int command1_wordcount_hi   : 8;       /* [23:16] */
        unsigned int reserved                : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dpu_partial_gen_hdr1;

/* Define the union dpu_partial_gen_pld_data10 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int command1_pld_b1         : 8;       /* [7:0] */
        unsigned int command1_pld_b2         : 8;       /* [15:8] */
        unsigned int command1_pld_b3         : 8;       /* [23:16] */
        unsigned int command1_pld_b4         : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dpu_partial_gen_pld_data10;

/* Define the union dpu_partial_gen_pld_data11 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int command1_pld_b5         : 8;       /* [7:0] */
        unsigned int command1_pld_b6         : 8;       /* [15:8] */
        unsigned int command1_pld_b7         : 8;       /* [23:16] */
        unsigned int command1_pld_b8         : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dpu_partial_gen_pld_data11;

/* Define the union link_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mem_link                : 2;       /* [1:0] */
        unsigned int format_link             : 2;       /* [3:2] */
        unsigned int reserved                : 27;      /* [30:4] */
        unsigned int g0_bypass_dhd0_en       : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_link_ctrl;

/* Define the union g0_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int galpha                  : 8;       /* [7:0] */
        unsigned int reserved0               : 19;      /* [26:8] */
        unsigned int depremult               : 1;       /* [27] */
        unsigned int rupd_field              : 1;       /* [28] */
        unsigned int rgup_mode               : 1;       /* [29] */
        unsigned int reserved1               : 1;       /* [30] */
        unsigned int surface_en              : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g0_ctrl;

/* Define the union g0_upd */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int regup                   : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g0_upd;

/* Define the union g0_0reso_read */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ow                      : 16;      /* [15:0] */
        unsigned int oh                      : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g0_0reso_read;

/* Define the union g0_dfpos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int disp_xfpos              : 16;      /* [15:0] */
        unsigned int disp_yfpos              : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g0_dfpos;

/* Define the union g0_dlpos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int disp_xlpos              : 16;      /* [15:0] */
        unsigned int disp_ylpos              : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g0_dlpos;

/* Define the union g0_vfpos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int video_xfpos             : 16;      /* [15:0] */
        unsigned int video_yfpos             : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g0_vfpos;

/* Define the union g0_vlpos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int video_xlpos             : 16;      /* [15:0] */
        unsigned int video_ylpos             : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g0_vlpos;

/* Define the union g0_bk */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved0               : 2;       /* [1:0] */
        unsigned int vbk_cr                  : 8;       /* [9:2] */
        unsigned int reserved1               : 2;       /* [11:10] */
        unsigned int vbk_cb                  : 8;       /* [19:12] */
        unsigned int reserved2               : 2;       /* [21:20] */
        unsigned int vbk_y                   : 8;       /* [29:22] */
        unsigned int reserved3               : 2;       /* [31:30] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g0_bk;

/* Define the union g0_alpha */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vbk_alpha               : 8;       /* [7:0] */
        unsigned int reserved                : 24;      /* [31:8] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g0_alpha;

/* Define the union g0_mute_bk */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mute_cr                 : 10;      /* [9:0] */
        unsigned int mute_cb                 : 10;      /* [19:10] */
        unsigned int mute_y                  : 10;      /* [29:20] */
        unsigned int reserved                : 1;       /* [30] */
        unsigned int mute_en                 : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g0_mute_bk;

/* Define the union g1_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int galpha                  : 8;       /* [7:0] */
        unsigned int reserved0               : 19;      /* [26:8] */
        unsigned int depremult               : 1;       /* [27] */
        unsigned int rupd_field              : 1;       /* [28] */
        unsigned int rgup_mode               : 1;       /* [29] */
        unsigned int reserved1               : 1;       /* [30] */
        unsigned int surface_en              : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_ctrl;

/* Define the union g1_upd */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int regup                   : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_upd;

/* Define the union g1_0reso_read */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ow                      : 16;      /* [15:0] */
        unsigned int oh                      : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_0reso_read;

/* Define the union g1_dfpos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int disp_xfpos              : 16;      /* [15:0] */
        unsigned int disp_yfpos              : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_dfpos;

/* Define the union g1_dlpos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int disp_xlpos              : 16;      /* [15:0] */
        unsigned int disp_ylpos              : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_dlpos;

/* Define the union g1_vfpos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int video_xfpos             : 16;      /* [15:0] */
        unsigned int video_yfpos             : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_vfpos;

/* Define the union g1_vlpos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int video_xlpos             : 16;      /* [15:0] */
        unsigned int video_ylpos             : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_vlpos;

/* Define the union g1_bk */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved0               : 2;       /* [1:0] */
        unsigned int vbk_cr                  : 8;       /* [9:2] */
        unsigned int reserved1               : 2;       /* [11:10] */
        unsigned int vbk_cb                  : 8;       /* [19:12] */
        unsigned int reserved2               : 2;       /* [21:20] */
        unsigned int vbk_y                   : 8;       /* [29:22] */
        unsigned int reserved3               : 2;       /* [31:30] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_bk;

/* Define the union g1_alpha */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vbk_alpha               : 8;       /* [7:0] */
        unsigned int reserved                : 24;      /* [31:8] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_alpha;

/* Define the union g1_mute_bk */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mute_cr                 : 10;      /* [9:0] */
        unsigned int mute_cb                 : 10;      /* [19:10] */
        unsigned int mute_y                  : 10;      /* [29:20] */
        unsigned int reserved                : 1;       /* [30] */
        unsigned int mute_en                 : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_mute_bk;

/* Define the union g1_csc_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_en             : 1;       /* [0] */
        unsigned int csc_demo_en        : 1;       /* [1] */
        unsigned int csc_ck_gt_en       : 1;       /* [2] */
        unsigned int reserved                : 29;      /* [31:3] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc_ctrl;

/* Define the union g1_csc_coef00 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_coef00         : 12;      /* [11:0] */
        unsigned int reserved                : 20;      /* [31:12] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc_coef00;

/* Define the union g1_csc_coef01 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_coef01         : 12;      /* [11:0] */
        unsigned int reserved                : 20;      /* [31:12] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc_coef01;

/* Define the union g1_csc_coef02 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_coef02         : 12;      /* [11:0] */
        unsigned int reserved                : 20;      /* [31:12] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc_coef02;

/* Define the union g1_csc_coef10 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_coef10         : 12;      /* [11:0] */
        unsigned int reserved                : 20;      /* [31:12] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc_coef10;

/* Define the union g1_csc_coef11 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_coef11         : 12;      /* [11:0] */
        unsigned int reserved                : 20;      /* [31:12] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc_coef11;

/* Define the union g1_csc_coef12 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_coef12         : 12;      /* [11:0] */
        unsigned int reserved                : 20;      /* [31:12] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc_coef12;

/* Define the union g1_csc_coef20 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_coef20         : 12;      /* [11:0] */
        unsigned int reserved                : 20;      /* [31:12] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc_coef20;

/* Define the union g1_csc_coef21 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_coef21         : 12;      /* [11:0] */
        unsigned int reserved                : 20;      /* [31:12] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc_coef21;

/* Define the union g1_csc_coef22 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_coef22         : 12;      /* [11:0] */
        unsigned int reserved                : 20;      /* [31:12] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc_coef22;

/* Define the union g1_csc_scale */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_scale          : 4;       /* [3:0] */
        unsigned int reserved                : 28;      /* [31:4] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc_scale;

/* Define the union g1_csc_idc0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_idc0           : 11;      /* [10:0] */
        unsigned int reserved                : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc_idc0;

/* Define the union g1_csc_idc1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_idc1           : 11;      /* [10:0] */
        unsigned int reserved                : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc_idc1;

/* Define the union g1_csc_idc2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_idc2           : 11;      /* [10:0] */
        unsigned int reserved                : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc_idc2;

/* Define the union g1_csc_odc0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_odc0           : 11;      /* [10:0] */
        unsigned int reserved                : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc_odc0;

/* Define the union g1_csc_odc1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_odc1           : 11;      /* [10:0] */
        unsigned int reserved                : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc_odc1;

/* Define the union g1_csc_odc2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_odc2           : 11;      /* [10:0] */
        unsigned int reserved                : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc_odc2;

/* Define the union g1_csc_min_y */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_min_y          : 10;      /* [9:0] */
        unsigned int reserved                : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc_min_y;

/* Define the union g1_csc_min_c */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_min_c          : 10;      /* [9:0] */
        unsigned int reserved                : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc_min_c;

/* Define the union g1_csc_max_y */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_max_y          : 10;      /* [9:0] */
        unsigned int reserved                : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc_max_y;

/* Define the union g1_csc_max_c */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc_max_c          : 10;      /* [9:0] */
        unsigned int reserved                : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc_max_c;

/* Define the union g1_csc2_coef00 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc2_coef00        : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc2_coef00;

/* Define the union g1_csc2_coef01 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc2_coef01        : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc2_coef01;

/* Define the union g1_csc2_coef02 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc2_coef02        : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc2_coef02;

/* Define the union g1_csc2_coef10 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc2_coef10        : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc2_coef10;

/* Define the union g1_csc2_coef11 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc2_coef11        : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc2_coef11;

/* Define the union g1_csc2_coef12 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc2_coef12        : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc2_coef12;

/* Define the union g1_csc2_coef20 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc2_coef20        : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc2_coef20;

/* Define the union g1_csc2_coef21 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc2_coef21        : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc2_coef21;

/* Define the union g1_csc2_coef22 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc2_coef22        : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc2_coef22;

/* Define the union g1_csc2_scale */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc2_scale         : 4;       /* [3:0] */
        unsigned int reserved                : 28;      /* [31:4] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc2_scale;

/* Define the union g1_csc2_idc0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc2_idc0          : 11;      /* [10:0] */
        unsigned int reserved                : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc2_idc0;

/* Define the union g1_csc2_idc1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc2_idc1          : 11;      /* [10:0] */
        unsigned int reserved                : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc2_idc1;

/* Define the union g1_csc2_idc2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc2_idc2          : 11;      /* [10:0] */
        unsigned int reserved                : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc2_idc2;

/* Define the union g1_csc2_odc0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc2_odc0          : 11;      /* [10:0] */
        unsigned int reserved                : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc2_odc0;

/* Define the union g1_csc2_odc1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc2_odc1          : 11;      /* [10:0] */
        unsigned int reserved                : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc2_odc1;

/* Define the union g1_csc2_odc2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc2_odc2          : 11;      /* [10:0] */
        unsigned int reserved                : 21;      /* [31:11] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc2_odc2;

/* Define the union g1_csc2_min_y */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc2_min_y         : 10;      /* [9:0] */
        unsigned int reserved                : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc2_min_y;

/* Define the union g1_csc2_min_c */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc2_min_c         : 10;      /* [9:0] */
        unsigned int reserved                : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc2_min_c;

/* Define the union g1_csc2_max_y */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc2_max_y         : 10;      /* [9:0] */
        unsigned int reserved                : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc2_max_y;

/* Define the union g1_csc2_max_c */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int csc2_max_c         : 10;      /* [9:0] */
        unsigned int reserved                : 22;      /* [31:10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc2_max_c;

/* Define the union g1_csc_ink_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ink_en                  : 1;       /* [0] */
        unsigned int ink_sel                 : 1;       /* [1] */
        unsigned int data_fmt                : 1;       /* [2] */
        unsigned int cross_enable            : 1;       /* [3] */
        unsigned int color_mode              : 2;       /* [5:4] */
        unsigned int reserved                : 26;      /* [31:6] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc_ink_ctrl;

/* Define the union g1_csc_ink_pos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int x_pos                   : 16;      /* [15:0] */
        unsigned int y_pos                   : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc_ink_pos;

/* Define the union g1_csc_ink_data */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ink_data                : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc_ink_data;

/* Define the union g1_csc_ink_data2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ink_data2               : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_csc_ink_data2;

/* Define the union g1_cvfir_vinfo */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int out_height              : 16;      /* [15:0] */
        unsigned int out_fmt                 : 2;       /* [17:16] */
        unsigned int out_pro                 : 1;       /* [18] */
        unsigned int vzme_ck_gt_en           : 1;       /* [19] */
        unsigned int reserved                : 12;      /* [31:20] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_cvfir_vinfo;

/* Define the union g1_cvfir_vsp */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vratio                  : 16;      /* [15:0] */
        unsigned int reserved0               : 1;       /* [16] */
        unsigned int reserved1               : 8;       /* [24:17] */
        unsigned int cvfir_mode              : 1;       /* [25] */
        unsigned int reserved2               : 1;       /* [26] */
        unsigned int reserved3               : 1;       /* [27] */
        unsigned int cvmid_en                : 1;       /* [28] */
        unsigned int reserved4               : 1;       /* [29] */
        unsigned int cvfir_en                : 1;       /* [30] */
        unsigned int reserved5               : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_cvfir_vsp;

/* Define the union g1_cvfir_voffset */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vchroma_offset          : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_cvfir_voffset;

/* Define the union g1_cvfir_vboffset */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vbchroma_offset         : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_cvfir_vboffset;

/* Define the union g1_cvfir_vcoef0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vccoef02                : 10;      /* [9:0] */
        unsigned int vccoef01                : 10;      /* [19:10] */
        unsigned int vccoef00                : 10;      /* [29:20] */
        unsigned int reserved                : 2;       /* [31:30] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_cvfir_vcoef0;

/* Define the union g1_cvfir_vcoef1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vccoef11                : 10;      /* [9:0] */
        unsigned int vccoef10                : 10;      /* [19:10] */
        unsigned int vccoef03                : 10;      /* [29:20] */
        unsigned int reserved                : 2;       /* [31:30] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_cvfir_vcoef1;

/* Define the union g1_cvfir_vcoef2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vccoef13                : 10;      /* [9:0] */
        unsigned int vccoef12                : 10;      /* [19:10] */
        unsigned int reserved                : 12;      /* [31:20] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_cvfir_vcoef2;

/* Define the union g1_hfir_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved0               : 1;       /* [0] */
        unsigned int hfir_mode               : 2;       /* [2:1] */
        unsigned int mid_en                  : 1;       /* [3] */
        unsigned int ck_gt_en                : 1;       /* [4] */
        unsigned int reserved1               : 27;      /* [31:5] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_hfir_ctrl;

/* Define the union g1_hfircoef01 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int coef0                   : 10;      /* [9:0] */
        unsigned int reserved0               : 6;       /* [15:10] */
        unsigned int coef1                   : 10;      /* [25:16] */
        unsigned int reserved1               : 6;       /* [31:26] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_hfircoef01;

/* Define the union g1_hfircoef23 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int coef2                   : 10;      /* [9:0] */
        unsigned int reserved0               : 6;       /* [15:10] */
        unsigned int coef3                   : 10;      /* [25:16] */
        unsigned int reserved1               : 6;       /* [31:26] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_hfircoef23;

/* Define the union g1_hfircoef45 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int coef4                   : 10;      /* [9:0] */
        unsigned int reserved0               : 6;       /* [15:10] */
        unsigned int coef5                   : 10;      /* [25:16] */
        unsigned int reserved1               : 6;       /* [31:26] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_hfircoef45;

/* Define the union g1_hfircoef67 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int coef6                   : 10;      /* [9:0] */
        unsigned int reserved0               : 6;       /* [15:10] */
        unsigned int coef7                   : 10;      /* [25:16] */
        unsigned int reserved1               : 6;       /* [31:26] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_hfircoef67;

/* Define the union dhd0_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int regup                   : 1;       /* [0] */
        unsigned int disp_mode               : 3;       /* [3:1] */
        unsigned int iop                     : 1;       /* [4] */
        unsigned int intf_ivs                : 1;       /* [5] */
        unsigned int intf_ihs                : 1;       /* [6] */
        unsigned int intf_idv                : 1;       /* [7] */
        unsigned int reserved0               : 3;       /* [10:8] */
        unsigned int uf_offline_en           : 1;       /* [11] */
        unsigned int reserved1               : 6;       /* [17:12] */
        unsigned int cbar_mode               : 1;       /* [18] */
        unsigned int reserved2               : 10;      /* [28:19] */
        unsigned int cbar_sel                : 1;       /* [29] */
        unsigned int cbar_en                 : 1;       /* [30] */
        unsigned int intf_en                 : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_ctrl;

/* Define the union dhd0_vsync1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vact                    : 16;      /* [15:0] */
        unsigned int vbb                     : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_vsync1;

/* Define the union dhd0_vsync2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vfb                     : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_vsync2;

/* Define the union dhd0_hsync1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hact                    : 16;      /* [15:0] */
        unsigned int hbb                     : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_hsync1;

/* Define the union dhd0_hsync2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hfb                     : 16;      /* [15:0] */
        unsigned int hmid                    : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_hsync2;

/* Define the union dhd0_vplus1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int bvact                   : 16;      /* [15:0] */
        unsigned int bvbb                    : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_vplus1;

/* Define the union dhd0_vplus2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int bvfb                    : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_vplus2;

/* Define the union dhd0_pwr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int hpw                     : 16;      /* [15:0] */
        unsigned int vpw                     : 8;       /* [23:16] */
        unsigned int reserved0               : 3;       /* [26:24] */
        unsigned int multichn_en             : 2;       /* [28:27] */
        unsigned int reserved1               : 3;       /* [31:29] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_pwr;

/* Define the union dhd0_vtthd3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vtmgthd3                : 13;      /* [12:0] */
        unsigned int reserved0               : 2;       /* [14:13] */
        unsigned int thd3_mode               : 1;       /* [15] */
        unsigned int vtmgthd4                : 13;      /* [28:16] */
        unsigned int reserved1               : 2;       /* [30:29] */
        unsigned int thd4_mode               : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_vtthd3;

/* Define the union dhd0_vtthd */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vtmgthd1                : 13;      /* [12:0] */
        unsigned int reserved0               : 2;       /* [14:13] */
        unsigned int thd1_mode               : 1;       /* [15] */
        unsigned int vtmgthd2                : 13;      /* [28:16] */
        unsigned int reserved1               : 2;       /* [30:29] */
        unsigned int thd2_mode               : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_vtthd;

/* Define the union dhd0_parathd */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int para_thd                : 8;       /* [7:0] */
        unsigned int reserved                : 24;      /* [31:8] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_parathd;

/* Define the union dhd0_precharge_thd */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved0               : 20;      /* [19:0] */
        unsigned int vsync_te_mode           : 1;       /* [20] */
        unsigned int vsync_te_from           : 1;       /* [21] */
        unsigned int reserved1               : 10;      /* [31:22] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_te_ctrl;

/* Define the union dhd0_start_pos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int start_pos               : 8;       /* [7:0] */
        unsigned int timing_start_pos        : 8;       /* [15:8] */
        unsigned int fi_start_pos            : 4;       /* [19:16] */
        unsigned int req_start_pos           : 12;      /* [31:20] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_start_pos;

/* Define the union dhd0_clk_dv_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int intf_clk_mux            : 1;       /* [0] */
        unsigned int intf_dv_mux             : 1;       /* [1] */
        unsigned int no_active_area_pos      : 16;      /* [17:2] */
        unsigned int reserved                : 14;      /* [31:18] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_clk_dv_ctrl;

/* Define the union dhd0_rgb_fix_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int fix_b                   : 10;      /* [9:0] */
        unsigned int fix_g                   : 10;      /* [19:10] */
        unsigned int fix_r                   : 10;      /* [29:20] */
        unsigned int rgb_fix_mux             : 1;       /* [30] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_rgb_fix_ctrl;

/* Define the union dhd0_intf_chksum_y */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int check_sum               : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_intf_chksum_y;

/* Define the union dhd0_intf_chksum_u */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int check_sum               : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_intf_chksum_u;

/* Define the union dhd0_intf_chksum_v */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int check_sum               : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_intf_chksum_v;

/* Define the union dhd0_intf1_chksum_y */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int check_sum               : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_intf1_chksum_y;

/* Define the union dhd0_intf1_chksum_u */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int check_sum               : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_intf1_chksum_u;

/* Define the union dhd0_intf1_chksum_v */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int check_sum               : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_intf1_chksum_v;

/* Define the union dhd0_intf_chksum_high1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int r0_sum_high             : 8;       /* [7:0] */
        unsigned int g0_sum_high             : 8;       /* [15:8] */
        unsigned int b0_sum_high             : 8;       /* [23:16] */
        unsigned int reserved                : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_intf_chksum_high1;

/* Define the union dhd0_intf_chksum_high2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int r1_sum_high             : 8;       /* [7:0] */
        unsigned int g1_sum_high             : 8;       /* [15:8] */
        unsigned int b1_sum_high             : 8;       /* [23:16] */
        unsigned int reserved                : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_intf_chksum_high2;

/* Define the union dhd0_intf_cmdcfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd0_te_dly_cfg         : 16;      /* [15:0] */
        unsigned int reserved                : 15;      /* [30:16] */
        unsigned int dhd0_tde_frm_rdy_mode   : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_intf_cmdcfg;

/* Define the union dhd0_tde_frm_rdy */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd0_tde_frm_rdy        : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_tde_frm_rdy;

/* Define the union dhd0_te_timeout */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dhd_te_timeout_thd      : 31;      /* [30:0] */
        unsigned int dhd_te_timeout_en       : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_te_timeout;

/* Define the union dhd0_afifo_pre_thd */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int afifo_pre_uf_thd        : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_afifo_pre_thd;

/* Define the union dhd0_state */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vback_blank             : 1;       /* [0] */
        unsigned int vblank                  : 1;       /* [1] */
        unsigned int bottom_field            : 1;       /* [2] */
        unsigned int vcnt                    : 13;      /* [15:3] */
        unsigned int count_int               : 8;       /* [23:16] */
        unsigned int dhd_even                : 1;       /* [24] */
        unsigned int reserved                : 7;       /* [31:25] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_state;

/* Define the union dhd0_uf_state */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ud_first_cnt            : 13;      /* [12:0] */
        unsigned int reserved0               : 3;       /* [15:13] */
        unsigned int start_pos               : 8;       /* [23:16] */
        unsigned int reserved1               : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_uf_state;

/* Define the union dhd0_vsync_te_state */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vsync_te_start_sta      : 8;       /* [7:0] */
        unsigned int vsync_te_start_sta1     : 8;       /* [15:8] */
        unsigned int vsync_te_end_sta        : 8;       /* [23:16] */
        unsigned int reserved                : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_vsync_te_state;

/* Define the union dhd0_vsync_te_state1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vsync_te_vfb            : 16;      /* [15:0] */
        unsigned int vsync_te_width          : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_dhd0_vsync_te_state1;

/* Define the union intf0_dither_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither_tap_mode         : 2;       /* [1:0] */
        unsigned int dither_domain_mode      : 1;       /* [2] */
        unsigned int dither_round            : 1;       /* [3] */
        unsigned int dither_mode             : 1;       /* [4] */
        unsigned int dither_en               : 1;       /* [5] */
        unsigned int dither_round_unlim      : 1;       /* [6] */
        unsigned int i_data_width_dither     : 3;       /* [9:7] */
        unsigned int o_data_width_dither     : 3;       /* [12:10] */
        unsigned int reserved                : 19;      /* [31:13] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_intf0_dither_ctrl;

/* Define the union intf0_dither_sed_y0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither_sed_y0           : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_intf0_dither_sed_y0;

/* Define the union intf0_dither_sed_u0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither_sed_u0           : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_intf0_dither_sed_u0;

/* Define the union intf0_dither_sed_v0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither_sed_v0           : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_intf0_dither_sed_v0;

/* Define the union intf0_dither_sed_w0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither_sed_w0           : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_intf0_dither_sed_w0;

/* Define the union intf0_dither_sed_y1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither_sed_y1           : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_intf0_dither_sed_y1;

/* Define the union intf0_dither_sed_u1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither_sed_u1           : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_intf0_dither_sed_u1;

/* Define the union intf0_dither_sed_v1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither_sed_v1           : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_intf0_dither_sed_v1;

/* Define the union intf0_dither_sed_w1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither_sed_w1           : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_intf0_dither_sed_w1;

/* Define the union intf0_dither_sed_y2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither_sed_y2           : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_intf0_dither_sed_y2;

/* Define the union intf0_dither_sed_u2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither_sed_u2           : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_intf0_dither_sed_u2;

/* Define the union intf0_dither_sed_v2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither_sed_v2           : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_intf0_dither_sed_v2;

/* Define the union intf0_dither_sed_w2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither_sed_w2           : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_intf0_dither_sed_w2;

/* Define the union intf0_dither_sed_y3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither_sed_y3           : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_intf0_dither_sed_y3;

/* Define the union intf0_dither_sed_u3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither_sed_u3           : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_intf0_dither_sed_u3;

/* Define the union intf0_dither_sed_v3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither_sed_v3           : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_intf0_dither_sed_v3;

/* Define the union intf0_dither_sed_w3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither_sed_w3           : 31;      /* [30:0] */
        unsigned int reserved                : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_intf0_dither_sed_w3;

/* Define the union intf0_dither_thr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dither_thr_min          : 16;      /* [15:0] */
        unsigned int dither_thr_max          : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_intf0_dither_thr;

/* Define the union intf0_ulps_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ulps_lp_mode_thd        : 16;      /* [15:0] */
        unsigned int reserved                : 15;      /* [30:16] */
        unsigned int ulps_lp_mode_en         : 1;       /* [31] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_intf0_ulps_ctrl;

/* Define the union intf0_ulps_delay */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ulps_lp_start_delay     : 16;      /* [15:0] */
        unsigned int ulps_lp_end_delay       : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_intf0_ulps_delay;

/* Define the union intf0_ulps_stop */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int soft_stop_lp_mode       : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_intf0_ulps_stop;

/* Define the union intf0_ulps_sta */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mipi_lp_mode_en         : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_intf0_ulps_sta;

/* Define the union intf_mipi_upd */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int regup                   : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_intf_mipi_upd;

/* Define the union intf_mipi_sync_inv */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dv_inv                  : 1;       /* [0] */
        unsigned int hs_inv                  : 1;       /* [1] */
        unsigned int vs_inv                  : 1;       /* [2] */
        unsigned int reserved                : 29;      /* [31:3] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_intf_mipi_sync_inv;

/* Define the union mac_outstanding */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mstr_routstanding       : 4;       /* [3:0] */
        unsigned int mstr_woutstanding       : 4;       /* [7:4] */
        unsigned int reserved                : 24;      /* [31:8] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_mac_outstanding;

/* Define the union mac_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int split_mode              : 4;       /* [3:0] */
        unsigned int arb_mode                : 4;       /* [7:4] */
        unsigned int mid_enable              : 1;       /* [8] */
        unsigned int reserved0               : 7;       /* [15:9] */
        unsigned int reserved1               : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_mac_ctrl;

/* Define the union mac_rchn_prio */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved0               : 9;       /* [8:0] */
        unsigned int g0_prio                 : 1;       /* [9] */
        unsigned int reserved1               : 1;       /* [10] */
        unsigned int g1_prio                 : 1;       /* [11] */
        unsigned int reserved2               : 5;       /* [16:12] */
        unsigned int g3_prio                 : 1;       /* [17] */
        unsigned int reserved3               : 1;       /* [18] */
        unsigned int g2_prio                 : 1;       /* [19] */
        unsigned int reserved4               : 12;      /* [31:20] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_mac_rchn_prio;

/* Define the union mac_bus_err_clr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int bus_error_clr           : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_mac_bus_err_clr;

/* Define the union mac_bus_err */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mst_r_error             : 1;       /* [0] */
        unsigned int mst_w_error             : 1;       /* [1] */
        unsigned int reserved                : 30;      /* [31:2] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_mac_bus_err;

/* Define the union mac_src0_status0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src0_status0            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_mac_src0_status0;

/* Define the union mac_src0_status1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src0_status1            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_mac_src0_status1;

/* Define the union mac_debug_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int axi_det_enable          : 1;       /* [0] */
        unsigned int reserved0               : 3;       /* [3:1] */
        unsigned int fifo_det_mode           : 4;       /* [7:4] */
        unsigned int reserved1               : 24;      /* [31:8] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_mac_debug_ctrl;

/* Define the union mac_debug_clr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int axi_det_clr             : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_mac_debug_clr;

/* Define the union mac_debug_info */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mac_debug_info          : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_mac_debug_info;

/* Define the union mac_rd_info */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mst_rd_info             : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_mac_rd_info;

/* Define the union mac_wr_info */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mst_wr_info             : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_mac_wr_info;

/* Define the union mac_det_latency0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mst_rd_max              : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_mac_det_latency0;

/* Define the union mac_det_latency1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mst_rd_aver             : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_mac_det_latency1;

/* Define the union mac_det_latency2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mst_rd_cmd              : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_mac_det_latency2;

/* Define the union mac_det_latency3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mst_wr_max              : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_mac_det_latency3;

/* Define the union mac_det_latency4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mst_wr_aver             : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_mac_det_latency4;

/* Define the union mac_det_latency5 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mst_wr_cmd              : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_mac_det_latency5;

/* Define the union gfx_read_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int read_mode               : 2;       /* [1:0] */
        unsigned int reserved0               : 2;       /* [3:2] */
        unsigned int draw_mode               : 2;       /* [5:4] */
        unsigned int reserved1               : 2;       /* [7:6] */
        unsigned int flip_en                 : 1;       /* [8] */
        unsigned int reserved2               : 1;       /* [9] */
        unsigned int mute_en                 : 1;       /* [10] */
        unsigned int mute_req_en             : 1;       /* [11] */
        unsigned int fdr_ck_gt_en            : 1;       /* [12] */
        unsigned int reserved3               : 19;      /* [31:13] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_read_ctrl;

/* Define the union gfx_mac_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int req_ctrl                : 2;       /* [1:0] */
        unsigned int req_len                 : 2;       /* [3:2] */
        unsigned int reserved                : 28;      /* [31:4] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_mac_ctrl;

/* Define the union gfx_out_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int palpha_range            : 1;       /* [0] */
        unsigned int palpha_en               : 1;       /* [1] */
        unsigned int reserved0               : 2;       /* [3:2] */
        unsigned int key_mode                : 1;       /* [4] */
        unsigned int key_en                  : 1;       /* [5] */
        unsigned int reserved1               : 2;       /* [7:6] */
        unsigned int bitext                  : 2;       /* [9:8] */
        unsigned int premulti_en             : 1;       /* [10] */
        unsigned int testpattern_en          : 1;       /* [11] */
        unsigned int reserved2               : 20;      /* [31:12] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_out_ctrl;

/* Define the union gfx_mute_alpha */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mute_alpha              : 8;       /* [7:0] */
        unsigned int reserved                : 24;      /* [31:8] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_mute_alpha;

/* Define the union gfx_mute_bk */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mute_cr                 : 10;      /* [9:0] */
        unsigned int mute_cb                 : 10;      /* [19:10] */
        unsigned int mute_y                  : 10;      /* [29:20] */
        unsigned int reserved                : 2;       /* [31:30] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_mute_bk;

/* Define the union gfx_1555_alpha */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int alpha_0                 : 8;       /* [7:0] */
        unsigned int alpha_1                 : 8;       /* [15:8] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_1555_alpha;

/* Define the union gfx_src_info */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ifmt                    : 8;       /* [7:0] */
        unsigned int reserved0               : 8;       /* [15:8] */
        unsigned int disp_mode               : 4;       /* [19:16] */
        unsigned int dcmp_en                 : 1;       /* [20] */
        unsigned int reserved1               : 11;      /* [31:21] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_src_info;

/* Define the union gfx_src_reso */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src_w                   : 16;      /* [15:0] */
        unsigned int src_h                   : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_src_reso;

/* Define the union gfx_src_crop */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src_crop_x              : 16;      /* [15:0] */
        unsigned int src_crop_y              : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_src_crop;

/* Define the union gfx_ireso */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ireso_w                 : 16;      /* [15:0] */
        unsigned int ireso_h                 : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_ireso;

/* Define the union gfx_addr_h */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int gfx_addr_h              : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_addr_h;

/* Define the union gfx_addr_l */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int gfx_addr_l              : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_addr_l;

/* Define the union gfx_stride */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int surface_stride          : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_stride;

/* Define the union gfx_addr_h_c */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int gfx_addr_h_c            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_addr_h_c;

/* Define the union gfx_addr_l_c */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int gfx_addr_l_c            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_addr_l_c;

/* Define the union gfx_stride_c */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int surface_stride_c        : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_stride_c;

/* Define the union gfx_dcmp_addr_h */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dcmp_addr_h             : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_dcmp_addr_h;

/* Define the union gfx_dcmp_addr_l */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dcmp_addr_l             : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_dcmp_addr_l;

/* Define the union gfx_testpat_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int tp_speed                : 10;      /* [9:0] */
        unsigned int reserved0               : 2;       /* [11:10] */
        unsigned int tp_line_w               : 1;       /* [12] */
        unsigned int tp_color_mode           : 1;       /* [13] */
        unsigned int reserved1               : 2;       /* [15:14] */
        unsigned int tp_mode                 : 2;       /* [17:16] */
        unsigned int reserved2               : 14;      /* [31:18] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_testpat_cfg;

/* Define the union gfx_testpat_seed */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int tp_seed                 : 30;      /* [29:0] */
        unsigned int reserved                : 2;       /* [31:30] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_testpat_seed;

/* Define the union gfx_dcmp_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ice_en                  : 1;       /* [0] */
        unsigned int cmp_mode                : 1;       /* [1] */
        unsigned int dcmp_fmt                : 2;       /* [3:2] */
        unsigned int alpha_bypass            : 1;       /* [4] */
        unsigned int reserved                : 27;      /* [31:5] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_dcmp_ctrl;

/* Define the union gfx_debug_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int checksum_en             : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_debug_ctrl;

/* Define the union gfx_in_ar_checksum0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ar_checksum0            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_in_ar_checksum0;

/* Define the union gfx_in_ar_checksum1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ar_checksum1            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_in_ar_checksum1;

/* Define the union gfx_in_gb_checksum0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int gb_checksum0            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_in_gb_checksum0;

/* Define the union gfx_in_gb_checksum1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int gb_checksum1            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_in_gb_checksum1;

/* Define the union gfx_ckey_max */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int key_b_max               : 8;       /* [7:0] */
        unsigned int key_g_max               : 8;       /* [15:8] */
        unsigned int key_r_max               : 8;       /* [23:16] */
        unsigned int reserved                : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_ckey_max;

/* Define the union gfx_ckey_min */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int key_b_min               : 8;       /* [7:0] */
        unsigned int key_g_min               : 8;       /* [15:8] */
        unsigned int key_r_min               : 8;       /* [23:16] */
        unsigned int reserved                : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_ckey_min;

/* Define the union gfx_ckey_mask */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int key_b_msk               : 8;       /* [7:0] */
        unsigned int key_g_msk               : 8;       /* [15:8] */
        unsigned int key_r_msk               : 8;       /* [23:16] */
        unsigned int reserved                : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_ckey_mask;

/* Define the union gfx_v5r1_htc_dcmp_glb_info */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ice_en                  : 1;       /* [0] */
        unsigned int cmp_mode                : 1;       /* [1] */
        unsigned int pix_format              : 1;       /* [2] */
        unsigned int is_a_bypass             : 1;       /* [3] */
        unsigned int is_a_offline            : 1;       /* [4] */
        unsigned int reserved                : 27;      /* [31:5] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_v5r1_htc_dcmp_glb_info;

/* Define the union gfx_v5r1_htc_dcmp_dbg_reg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dcmp_debug              : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_gfx_v5r1_htc_dcmp_dbg_reg;

/* Define the union g1_gfx_read_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int read_mode               : 2;       /* [1:0] */
        unsigned int reserved0               : 2;       /* [3:2] */
        unsigned int draw_mode               : 2;       /* [5:4] */
        unsigned int reserved1               : 2;       /* [7:6] */
        unsigned int flip_en                 : 1;       /* [8] */
        unsigned int reserved2               : 1;       /* [9] */
        unsigned int mute_en                 : 1;       /* [10] */
        unsigned int mute_req_en             : 1;       /* [11] */
        unsigned int fdr_ck_gt_en            : 1;       /* [12] */
        unsigned int reserved3               : 19;      /* [31:13] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_gfx_read_ctrl;

/* Define the union g1_gfx_mac_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int req_ctrl                : 2;       /* [1:0] */
        unsigned int req_len                 : 2;       /* [3:2] */
        unsigned int reserved                : 28;      /* [31:4] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_gfx_mac_ctrl;

/* Define the union g1_gfx_out_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int palpha_range            : 1;       /* [0] */
        unsigned int palpha_en               : 1;       /* [1] */
        unsigned int reserved0               : 2;       /* [3:2] */
        unsigned int key_mode                : 1;       /* [4] */
        unsigned int key_en                  : 1;       /* [5] */
        unsigned int reserved1               : 2;       /* [7:6] */
        unsigned int bitext                  : 2;       /* [9:8] */
        unsigned int premulti_en             : 1;       /* [10] */
        unsigned int testpattern_en          : 1;       /* [11] */
        unsigned int reserved2               : 20;      /* [31:12] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_gfx_out_ctrl;

/* Define the union g1_gfx_mute_alpha */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mute_alpha              : 8;       /* [7:0] */
        unsigned int reserved                : 24;      /* [31:8] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_gfx_mute_alpha;

/* Define the union g1_gfx_mute_bk */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mute_cr                 : 10;      /* [9:0] */
        unsigned int mute_cb                 : 10;      /* [19:10] */
        unsigned int mute_y                  : 10;      /* [29:20] */
        unsigned int reserved                : 2;       /* [31:30] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_gfx_mute_bk;

/* Define the union g1_gfx_1555_alpha */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int alpha_0                 : 8;       /* [7:0] */
        unsigned int alpha_1                 : 8;       /* [15:8] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_gfx_1555_alpha;

/* Define the union g1_gfx_src_info */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ifmt                    : 8;       /* [7:0] */
        unsigned int reserved0               : 8;       /* [15:8] */
        unsigned int disp_mode               : 4;       /* [19:16] */
        unsigned int dcmp_en                 : 1;       /* [20] */
        unsigned int reserved1               : 11;      /* [31:21] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_gfx_src_info;

/* Define the union g1_gfx_src_reso */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src_w                   : 16;      /* [15:0] */
        unsigned int src_h                   : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_gfx_src_reso;

/* Define the union g1_gfx_src_crop */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int src_crop_x              : 16;      /* [15:0] */
        unsigned int src_crop_y              : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_gfx_src_crop;

/* Define the union g1_gfx_ireso */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ireso_w                 : 16;      /* [15:0] */
        unsigned int ireso_h                 : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_gfx_ireso;

/* Define the union g1_gfx_addr_h */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int gfx_addr_h              : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_gfx_addr_h;

/* Define the union g1_gfx_addr_l */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int gfx_addr_l              : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_gfx_addr_l;

/* Define the union g1_gfx_stride */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int surface_stride          : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_gfx_stride;

/* Define the union g1_gfx_addr_h_c */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int gfx_addr_h_c            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_gfx_addr_h_c;

/* Define the union g1_gfx_addr_l_c */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int gfx_addr_l_c            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_gfx_addr_l_c;

/* Define the union g1_gfx_stride_c */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int surface_stride_c        : 16;      /* [15:0] */
        unsigned int reserved                : 16;      /* [31:16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_gfx_stride_c;

/* Define the union g1_gfx_dcmp_addr_h */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dcmp_addr_h             : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_gfx_dcmp_addr_h;

/* Define the union g1_gfx_dcmp_addr_l */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dcmp_addr_l             : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_gfx_dcmp_addr_l;

/* Define the union g1_gfx_testpat_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int tp_speed                : 10;      /* [9:0] */
        unsigned int reserved0               : 2;       /* [11:10] */
        unsigned int tp_line_w               : 1;       /* [12] */
        unsigned int tp_color_mode           : 1;       /* [13] */
        unsigned int reserved1               : 2;       /* [15:14] */
        unsigned int tp_mode                 : 2;       /* [17:16] */
        unsigned int reserved2               : 14;      /* [31:18] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_gfx_testpat_cfg;

/* Define the union g1_gfx_testpat_seed */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int tp_seed                 : 30;      /* [29:0] */
        unsigned int reserved                : 2;       /* [31:30] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_gfx_testpat_seed;

/* Define the union g1_gfx_dcmp_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ice_en                  : 1;       /* [0] */
        unsigned int cmp_mode                : 1;       /* [1] */
        unsigned int dcmp_fmt                : 2;       /* [3:2] */
        unsigned int alpha_bypass            : 1;       /* [4] */
        unsigned int reserved                : 27;      /* [31:5] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_gfx_dcmp_ctrl;

/* Define the union g1_gfx_debug_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int checksum_en             : 1;       /* [0] */
        unsigned int reserved                : 31;      /* [31:1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_gfx_debug_ctrl;

/* Define the union g1_gfx_in_ar_checksum0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ar_checksum0            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_gfx_in_ar_checksum0;

/* Define the union g1_gfx_in_ar_checksum1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ar_checksum1            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_gfx_in_ar_checksum1;

/* Define the union g1_gfx_in_gb_checksum0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int gb_checksum0            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_gfx_in_gb_checksum0;

/* Define the union g1_gfx_in_gb_checksum1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int gb_checksum1            : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_gfx_in_gb_checksum1;

/* Define the union g1_gfx_ckey_max */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int key_b_max               : 8;       /* [7:0] */
        unsigned int key_g_max               : 8;       /* [15:8] */
        unsigned int key_r_max               : 8;       /* [23:16] */
        unsigned int reserved                : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_gfx_ckey_max;

/* Define the union g1_gfx_ckey_min */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int key_b_min               : 8;       /* [7:0] */
        unsigned int key_g_min               : 8;       /* [15:8] */
        unsigned int key_r_min               : 8;       /* [23:16] */
        unsigned int reserved                : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_gfx_ckey_min;

/* Define the union g1_gfx_ckey_mask */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int key_b_msk               : 8;       /* [7:0] */
        unsigned int key_g_msk               : 8;       /* [15:8] */
        unsigned int key_r_msk               : 8;       /* [23:16] */
        unsigned int reserved                : 8;       /* [31:24] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_gfx_ckey_mask;

/* Define the union g1_gfx_v5r1_htc_dcmp_glb_info */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ice_en                  : 1;       /* [0] */
        unsigned int cmp_mode                : 1;       /* [1] */
        unsigned int pix_format              : 1;       /* [2] */
        unsigned int is_a_bypass             : 1;       /* [3] */
        unsigned int is_a_offline            : 1;       /* [4] */
        unsigned int reserved                : 27;      /* [31:5] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_gfx_v5r1_htc_dcmp_glb_info;

/* Define the union g1_gfx_v5r1_htc_dcmp_dbg_reg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dcmp_debug              : 32;      /* [31:0] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_g1_gfx_v5r1_htc_dcmp_dbg_reg;


/*
 * DEFINE GLOBAL STRUCT
 */
typedef struct {
    u_voctrl                             voctrl;                              /* 0 */
    u_vointsta                           vointsta;                            /* 4 */
    u_vomskintsta                        vomskintsta;                         /* 8 */
    u_vointmsk                           vointmsk;                            /* c */
    u_vodebug                            vodebug;                             /* 10 */
    u_vocmd_intsta                       vocmd_intsta;                        /* 14 */
    u_vocmd_mskintsta                    vocmd_mskintsta;                     /* 18 */
    u_vocmd_intmsk                       vocmd_intmsk;                        /* 1c */
    u_voversion1                         voversion1;                          /* 20 */
    u_voversion2                         voversion2;                          /* 24 */
    u_volowpower_ctrl                    volowpower_ctrl;                     /* 28 */
    u_voufsta                            voufsta;                             /* 2c */
    u_voufclr                            voufclr;                             /* 30 */
    u_vointproc_tim                      vointproc_tim;                       /* 34 */
    u_vofpgatest                         vofpgatest;                          /* 38 */
    unsigned int                         rsr0;                                /* 3c */
    u_volowpower_ctrl_09v                volowpower_ctrl_09v;                 /* 40 */
    u_volowpower_ctrl1_09v               volowpower_ctrl1_09v;                /* 44 */
    u_volowpower_ctrl1                   volowpower_ctrl1;                    /* 48 */
    unsigned int                         rsr1;                                /* 4c */
    u_dpu_sram_ctrl                      dpu_sram_ctrl;                       /* 50 */
    unsigned int                         rsr2[171];                           /* 54 ~ 2fc */
    u_cbm_bkg                            cbm_bkg;                             /* 300 */
    unsigned int                         rsr3;                                /* 304 */
    u_cbm_mix                            cbm_mix;                             /* 308 */
    u_cbm_ctrl                           cbm_ctrl;                            /* 30c */
    unsigned int                         rsr4[60];                            /* 310 ~ 3fc */
    u_dpu_partial_ctrl                   dpu_partial_ctrl;                    /* 400 */
    u_dpu_partial_gen_hdr0               dpu_partial_gen_hdr0;                /* 404 */
    u_dpu_partial_gen_pld_data00         dpu_partial_gen_pld_data00;          /* 408 */
    u_dpu_partial_gen_pld_data01         dpu_partial_gen_pld_data01;          /* 40c */
    u_dpu_partial_gen_hdr1               dpu_partial_gen_hdr1;                /* 410 */
    u_dpu_partial_gen_pld_data10         dpu_partial_gen_pld_data10;          /* 414 */
    u_dpu_partial_gen_pld_data11         dpu_partial_gen_pld_data11;          /* 418 */
    unsigned int                         rsr5[505];                           /* 41c ~ bfc */
    u_link_ctrl                          link_ctrl;                           /* c00 */
    unsigned int                         rsr6[6399];                          /* c04 ~ 6ffc */
    u_g0_ctrl                            g0_ctrl;                             /* 7000 */
    u_g0_upd                             g0_upd;                              /* 7004 */
    unsigned int                         rsr7;                                /* 7008 */
    u_g0_0reso_read                      g0_0reso_read;                       /* 700c */
    unsigned int                         rsr8[28];                            /* 7010 ~ 707c */
    u_g0_dfpos                           g0_dfpos;                            /* 7080 */
    u_g0_dlpos                           g0_dlpos;                            /* 7084 */
    u_g0_vfpos                           g0_vfpos;                            /* 7088 */
    u_g0_vlpos                           g0_vlpos;                            /* 708c */
    u_g0_bk                              g0_bk;                               /* 7090 */
    u_g0_alpha                           g0_alpha;                            /* 7094 */
    u_g0_mute_bk                         g0_mute_bk;                          /* 7098 */
    unsigned int                         rsr9[473];                           /* 709c ~ 77fc */
    u_g1_ctrl                            g1_ctrl;                             /* 7800 */
    u_g1_upd                             g1_upd;                              /* 7804 */
    unsigned int                         rsr10;                               /* 7808 */
    u_g1_0reso_read                      g1_0reso_read;                       /* 780c */
    unsigned int                         rsr11[28];                           /* 7810 ~ 787c */
    u_g1_dfpos                           g1_dfpos;                            /* 7880 */
    u_g1_dlpos                           g1_dlpos;                            /* 7884 */
    u_g1_vfpos                           g1_vfpos;                            /* 7888 */
    u_g1_vlpos                           g1_vlpos;                            /* 788c */
    u_g1_bk                              g1_bk;                               /* 7890 */
    u_g1_alpha                           g1_alpha;                            /* 7894 */
    u_g1_mute_bk                         g1_mute_bk;                          /* 7898 */
    unsigned int                         rsr12[25];                           /* 789c ~ 78fc */
    u_g1_csc_ctrl                        g1_csc_ctrl;                         /* 7900 */
    u_g1_csc_coef00                      g1_csc_coef00;                       /* 7904 */
    u_g1_csc_coef01                      g1_csc_coef01;                       /* 7908 */
    u_g1_csc_coef02                      g1_csc_coef02;                       /* 790c */
    u_g1_csc_coef10                      g1_csc_coef10;                       /* 7910 */
    u_g1_csc_coef11                      g1_csc_coef11;                       /* 7914 */
    u_g1_csc_coef12                      g1_csc_coef12;                       /* 7918 */
    u_g1_csc_coef20                      g1_csc_coef20;                       /* 791c */
    u_g1_csc_coef21                      g1_csc_coef21;                       /* 7920 */
    u_g1_csc_coef22                      g1_csc_coef22;                       /* 7924 */
    u_g1_csc_scale                       g1_csc_scale;                        /* 7928 */
    u_g1_csc_idc0                        g1_csc_idc0;                         /* 792c */
    u_g1_csc_idc1                        g1_csc_idc1;                         /* 7930 */
    u_g1_csc_idc2                        g1_csc_idc2;                         /* 7934 */
    u_g1_csc_odc0                        g1_csc_odc0;                         /* 7938 */
    u_g1_csc_odc1                        g1_csc_odc1;                         /* 793c */
    u_g1_csc_odc2                        g1_csc_odc2;                         /* 7940 */
    u_g1_csc_min_y                       g1_csc_min_y;                        /* 7944 */
    u_g1_csc_min_c                       g1_csc_min_c;                        /* 7948 */
    u_g1_csc_max_y                       g1_csc_max_y;                        /* 794c */
    u_g1_csc_max_c                       g1_csc_max_c;                        /* 7950 */
    u_g1_csc2_coef00                     g1_csc2_coef00;                      /* 7954 */
    u_g1_csc2_coef01                     g1_csc2_coef01;                      /* 7958 */
    u_g1_csc2_coef02                     g1_csc2_coef02;                      /* 795c */
    u_g1_csc2_coef10                     g1_csc2_coef10;                      /* 7960 */
    u_g1_csc2_coef11                     g1_csc2_coef11;                      /* 7964 */
    u_g1_csc2_coef12                     g1_csc2_coef12;                      /* 7968 */
    u_g1_csc2_coef20                     g1_csc2_coef20;                      /* 796c */
    u_g1_csc2_coef21                     g1_csc2_coef21;                      /* 7970 */
    u_g1_csc2_coef22                     g1_csc2_coef22;                      /* 7974 */
    u_g1_csc2_scale                      g1_csc2_scale;                       /* 7978 */
    u_g1_csc2_idc0                       g1_csc2_idc0;                        /* 797c */
    u_g1_csc2_idc1                       g1_csc2_idc1;                        /* 7980 */
    u_g1_csc2_idc2                       g1_csc2_idc2;                        /* 7984 */
    u_g1_csc2_odc0                       g1_csc2_odc0;                        /* 7988 */
    u_g1_csc2_odc1                       g1_csc2_odc1;                        /* 798c */
    u_g1_csc2_odc2                       g1_csc2_odc2;                        /* 7990 */
    u_g1_csc2_min_y                      g1_csc2_min_y;                       /* 7994 */
    u_g1_csc2_min_c                      g1_csc2_min_c;                       /* 7998 */
    u_g1_csc2_max_y                      g1_csc2_max_y;                       /* 799c */
    u_g1_csc2_max_c                      g1_csc2_max_c;                       /* 79a0 */
    unsigned int                         rsr13[19];                           /* 79a4 ~ 79ec */
    u_g1_csc_ink_ctrl                    g1_csc_ink_ctrl;                     /* 79f0 */
    u_g1_csc_ink_pos                     g1_csc_ink_pos;                      /* 79f4 */
    u_g1_csc_ink_data                    g1_csc_ink_data;                     /* 79f8 */
    u_g1_csc_ink_data2                   g1_csc_ink_data2;                    /* 79fc */
    unsigned int                         rsr14[64];                           /* 7a00 ~ 7afc */
    u_g1_cvfir_vinfo                     g1_cvfir_vinfo;                      /* 7b00 */
    u_g1_cvfir_vsp                       g1_cvfir_vsp;                        /* 7b04 */
    u_g1_cvfir_voffset                   g1_cvfir_voffset;                    /* 7b08 */
    u_g1_cvfir_vboffset                  g1_cvfir_vboffset;                   /* 7b0c */
    unsigned int                         rsr15[8];                            /* 7b10 ~ 7b2c */
    u_g1_cvfir_vcoef0                    g1_cvfir_vcoef0;                     /* 7b30 */
    u_g1_cvfir_vcoef1                    g1_cvfir_vcoef1;                     /* 7b34 */
    u_g1_cvfir_vcoef2                    g1_cvfir_vcoef2;                     /* 7b38 */
    unsigned int                         rsr16[49];                           /* 7b3c ~ 7bfc */
    u_g1_hfir_ctrl                       g1_hfir_ctrl;                        /* 7c00 */
    u_g1_hfircoef01                      g1_hfircoef01;                       /* 7c04 */
    u_g1_hfircoef23                      g1_hfircoef23;                       /* 7c08 */
    u_g1_hfircoef45                      g1_hfircoef45;                       /* 7c0c */
    u_g1_hfircoef67                      g1_hfircoef67;                       /* 7c10 */
    unsigned int                         rsr17[5371];                         /* 7c14 ~ cffc */
    u_dhd0_ctrl                          dhd0_ctrl;                           /* d000 */
    u_dhd0_vsync1                        dhd0_vsync1;                         /* d004 */
    u_dhd0_vsync2                        dhd0_vsync2;                         /* d008 */
    u_dhd0_hsync1                        dhd0_hsync1;                         /* d00c */
    u_dhd0_hsync2                        dhd0_hsync2;                         /* d010 */
    u_dhd0_vplus1                        dhd0_vplus1;                         /* d014 */
    u_dhd0_vplus2                        dhd0_vplus2;                         /* d018 */
    u_dhd0_pwr                           dhd0_pwr;                            /* d01c */
    u_dhd0_vtthd3                        dhd0_vtthd3;                         /* d020 */
    u_dhd0_vtthd                         dhd0_vtthd;                          /* d024 */
    u_dhd0_parathd                       dhd0_parathd;                        /* d028 */
    u_dhd0_te_ctrl                       dhd0_te_ctrl;                        /* d02c */
    u_dhd0_start_pos                     dhd0_start_pos;                      /* d030 */
    unsigned int                         rsr18[3];                            /* d034 ~ d03c */
    u_dhd0_clk_dv_ctrl                   dhd0_clk_dv_ctrl;                    /* d040 */
    u_dhd0_rgb_fix_ctrl                  dhd0_rgb_fix_ctrl;                   /* d044 */
    unsigned int                         rsr19[4];                            /* d048 ~ d054 */
    u_dhd0_intf_chksum_y                 dhd0_intf_chksum_y;                  /* d058 */
    u_dhd0_intf_chksum_u                 dhd0_intf_chksum_u;                  /* d05c */
    u_dhd0_intf_chksum_v                 dhd0_intf_chksum_v;                  /* d060 */
    u_dhd0_intf1_chksum_y                dhd0_intf1_chksum_y;                 /* d064 */
    u_dhd0_intf1_chksum_u                dhd0_intf1_chksum_u;                 /* d068 */
    u_dhd0_intf1_chksum_v                dhd0_intf1_chksum_v;                 /* d06c */
    u_dhd0_intf_chksum_high1             dhd0_intf_chksum_high1;              /* d070 */
    u_dhd0_intf_chksum_high2             dhd0_intf_chksum_high2;              /* d074 */
    u_dhd0_intf_cmdcfg                   dhd0_intf_cmdcfg;                    /* d078 */
    u_dhd0_tde_frm_rdy                   dhd0_tde_frm_rdy;                    /* d07c */
    u_dhd0_te_timeout                    dhd0_te_timeout;                     /* d080 */
    u_dhd0_afifo_pre_thd                 dhd0_afifo_pre_thd;                  /* d084 */
    u_dhd0_state                         dhd0_state;                          /* d088 */
    u_dhd0_uf_state                      dhd0_uf_state;                       /* d08c */
    unsigned int                         rsr20[4];                            /* d090 ~ d09c */
    u_dhd0_vsync_te_state                dhd0_vsync_te_state;                 /* d0a0 */
    u_dhd0_vsync_te_state1               dhd0_vsync_te_state1;                /* d0a4 */
    unsigned int                         rsr21[2];                            /* d0a8 ~ d0ac */
    u_intf0_dither_ctrl                  intf0_dither_ctrl;                   /* d0b0 */
    u_intf0_dither_sed_y0                intf0_dither_sed_y0;                 /* d0b4 */
    u_intf0_dither_sed_u0                intf0_dither_sed_u0;                 /* d0b8 */
    u_intf0_dither_sed_v0                intf0_dither_sed_v0;                 /* d0bc */
    u_intf0_dither_sed_w0                intf0_dither_sed_w0;                 /* d0c0 */
    u_intf0_dither_sed_y1                intf0_dither_sed_y1;                 /* d0c4 */
    u_intf0_dither_sed_u1                intf0_dither_sed_u1;                 /* d0c8 */
    u_intf0_dither_sed_v1                intf0_dither_sed_v1;                 /* d0cc */
    u_intf0_dither_sed_w1                intf0_dither_sed_w1;                 /* d0d0 */
    u_intf0_dither_sed_y2                intf0_dither_sed_y2;                 /* d0d4 */
    u_intf0_dither_sed_u2                intf0_dither_sed_u2;                 /* d0d8 */
    u_intf0_dither_sed_v2                intf0_dither_sed_v2;                 /* d0dc */
    u_intf0_dither_sed_w2                intf0_dither_sed_w2;                 /* d0e0 */
    u_intf0_dither_sed_y3                intf0_dither_sed_y3;                 /* d0e4 */
    u_intf0_dither_sed_u3                intf0_dither_sed_u3;                 /* d0e8 */
    u_intf0_dither_sed_v3                intf0_dither_sed_v3;                 /* d0ec */
    u_intf0_dither_sed_w3                intf0_dither_sed_w3;                 /* d0f0 */
    u_intf0_dither_thr                   intf0_dither_thr;                    /* d0f4 */
    unsigned int                         rsr22[30];                           /* d0f8 ~ d16c */
    u_intf0_ulps_ctrl                    intf0_ulps_ctrl;                     /* d170 */
    u_intf0_ulps_delay                   intf0_ulps_delay;                    /* d174 */
    u_intf0_ulps_stop                    intf0_ulps_stop;                     /* d178 */
    u_intf0_ulps_sta                     intf0_ulps_sta;                      /* d17c */
    unsigned int                         rsr23;                               /* d180 */
    u_intf_mipi_upd                      intf_mipi_upd;                       /* d184 */
    u_intf_mipi_sync_inv                 intf_mipi_sync_inv;                  /* d188 */
    unsigned int                         rsr24[2973];                         /* d18c ~ fffc */
    u_mac_outstanding                    mac_outstanding;                     /* 10000 */
    u_mac_ctrl                           mac_ctrl;                            /* 10004 */
    unsigned int                         rsr25[2];                            /* 10008 ~ 1000c */
    u_mac_rchn_prio                      mac_rchn_prio;                       /* 10010 */
    unsigned int                         rsr26[11];                           /* 10014 ~ 1003c */
    u_mac_bus_err_clr                    mac_bus_err_clr;                     /* 10040 */
    u_mac_bus_err                        mac_bus_err;                         /* 10044 */
    unsigned int                         rsr27[2];                            /* 10048 ~ 1004c */
    u_mac_src0_status0                   mac_src0_status0;                    /* 10050 */
    u_mac_src0_status1                   mac_src0_status1;                    /* 10054 */
    unsigned int                         rsr28[6];                            /* 10058 ~ 1006c */
    u_mac_debug_ctrl                     mac_debug_ctrl;                      /* 10070 */
    u_mac_debug_clr                      mac_debug_clr;                       /* 10074 */
    unsigned int                         rsr29[2];                            /* 10078 ~ 1007c */
    u_mac_debug_info                     mac_debug_info;                      /* 10080 */
    unsigned int                         rsr30[3];                            /* 10084 ~ 1008c */
    u_mac_rd_info                        mac_rd_info;                         /* 10090 */
    u_mac_wr_info                        mac_wr_info;                         /* 10094 */
    unsigned int                         rsr31[6];                            /* 10098 ~ 100ac */
    u_mac_det_latency0                   mac_det_latency0;                    /* 100b0 */
    u_mac_det_latency1                   mac_det_latency1;                    /* 100b4 */
    u_mac_det_latency2                   mac_det_latency2;                    /* 100b8 */
    u_mac_det_latency3                   mac_det_latency3;                    /* 100bc */
    u_mac_det_latency4                   mac_det_latency4;                    /* 100c0 */
    u_mac_det_latency5                   mac_det_latency5;                    /* 100c4 */
    unsigned int                         rsr32[846];                          /* 100c8 ~ 10dfc */
    u_gfx_read_ctrl                      gfx_read_ctrl;                       /* 10e00 */
    u_gfx_mac_ctrl                       gfx_mac_ctrl;                        /* 10e04 */
    u_gfx_out_ctrl                       gfx_out_ctrl;                        /* 10e08 */
    unsigned int                         rsr33;                               /* 10e0c */
    u_gfx_mute_alpha                     gfx_mute_alpha;                      /* 10e10 */
    u_gfx_mute_bk                        gfx_mute_bk;                         /* 10e14 */
    unsigned int                         rsr34[4];                            /* 10e18 ~ 10e24 */
    u_gfx_1555_alpha                     gfx_1555_alpha;                      /* 10e28 */
    unsigned int                         rsr35[5];                            /* 10e2c ~ 10e3c */
    u_gfx_src_info                       gfx_src_info;                        /* 10e40 */
    u_gfx_src_reso                       gfx_src_reso;                        /* 10e44 */
    u_gfx_src_crop                       gfx_src_crop;                        /* 10e48 */
    u_gfx_ireso                          gfx_ireso;                           /* 10e4c */
    u_gfx_addr_h                         gfx_addr_h;                          /* 10e50 */
    u_gfx_addr_l                         gfx_addr_l;                          /* 10e54 */
    unsigned int                         rsr36[2];                            /* 10e58 ~ 10e5c */
    u_gfx_stride                         gfx_stride;                          /* 10e60 */
    u_gfx_addr_h_c                       gfx_addr_h_c;                        /* 10e64 */
    u_gfx_addr_l_c                       gfx_addr_l_c;                        /* 10e68 */
    u_gfx_stride_c                       gfx_stride_c;                        /* 10e6c */
    u_gfx_dcmp_addr_h                    gfx_dcmp_addr_h;                     /* 10e70 */
    u_gfx_dcmp_addr_l                    gfx_dcmp_addr_l;                     /* 10e74 */
    unsigned int                         rsr37[14];                           /* 10e78 ~ 10eac */
    u_gfx_testpat_cfg                    gfx_testpat_cfg;                     /* 10eb0 */
    u_gfx_testpat_seed                   gfx_testpat_seed;                    /* 10eb4 */
    u_gfx_dcmp_ctrl                      gfx_dcmp_ctrl;                       /* 10eb8 */
    unsigned int                         rsr38;                               /* 10ebc */
    u_gfx_debug_ctrl                     gfx_debug_ctrl;                      /* 10ec0 */
    unsigned int                         rsr39[7];                            /* 10ec4 ~ 10edc */
    u_gfx_in_ar_checksum0                gfx_in_ar_checksum0;                 /* 10ee0 */
    u_gfx_in_ar_checksum1                gfx_in_ar_checksum1;                 /* 10ee4 */
    u_gfx_in_gb_checksum0                gfx_in_gb_checksum0;                 /* 10ee8 */
    u_gfx_in_gb_checksum1                gfx_in_gb_checksum1;                 /* 10eec */
    unsigned int                         rsr40[4];                            /* 10ef0 ~ 10efc */
    u_gfx_ckey_max                       gfx_ckey_max;                        /* 10f00 */
    u_gfx_ckey_min                       gfx_ckey_min;                        /* 10f04 */
    u_gfx_ckey_mask                      gfx_ckey_mask;                       /* 10f08 */
    unsigned int                         rsr41[29];                           /* 10f0c ~ 10f7c */
    u_gfx_v5r1_htc_dcmp_glb_info         gfx_v5r1_htc_dcmp_glb_info;          /* 10f80 */
    unsigned int                         rsr42[3];                            /* 10f84 ~ 10f8c */
    u_gfx_v5r1_htc_dcmp_dbg_reg          gfx_v5r1_htc_dcmp_dbg_reg;           /* 10f90 */
    unsigned int                         rsr43[27];                           /* 10f94 ~ 10ffc */
    u_g1_gfx_read_ctrl                   g1_gfx_read_ctrl;                    /* 11000 */
    u_g1_gfx_mac_ctrl                    g1_gfx_mac_ctrl;                     /* 11004 */
    u_g1_gfx_out_ctrl                    g1_gfx_out_ctrl;                     /* 11008 */
    unsigned int                         rsr44;                               /* 1100c */
    u_g1_gfx_mute_alpha                  g1_gfx_mute_alpha;                   /* 11010 */
    u_g1_gfx_mute_bk                     g1_gfx_mute_bk;                      /* 11014 */
    unsigned int                         rsr45[4];                            /* 11018 ~ 11024 */
    u_g1_gfx_1555_alpha                  g1_gfx_1555_alpha;                   /* 11028 */
    unsigned int                         rsr46[5];                            /* 1102c ~ 1103c */
    u_g1_gfx_src_info                    g1_gfx_src_info;                     /* 11040 */
    u_g1_gfx_src_reso                    g1_gfx_src_reso;                     /* 11044 */
    u_g1_gfx_src_crop                    g1_gfx_src_crop;                     /* 11048 */
    u_g1_gfx_ireso                       g1_gfx_ireso;                        /* 1104c */
    u_g1_gfx_addr_h                      g1_gfx_addr_h;                       /* 11050 */
    u_g1_gfx_addr_l                      g1_gfx_addr_l;                       /* 11054 */
    unsigned int                         rsr47[2];                            /* 11058 ~ 1105c */
    u_g1_gfx_stride                      g1_gfx_stride;                       /* 11060 */
    u_g1_gfx_addr_h_c                    g1_gfx_addr_h_c;                     /* 11064 */
    u_g1_gfx_addr_l_c                    g1_gfx_addr_l_c;                     /* 11068 */
    u_g1_gfx_stride_c                    g1_gfx_stride_c;                     /* 1106c */
    u_g1_gfx_dcmp_addr_h                 g1_gfx_dcmp_addr_h;                  /* 11070 */
    u_g1_gfx_dcmp_addr_l                 g1_gfx_dcmp_addr_l;                  /* 11074 */
    unsigned int                         rsr48[14];                           /* 11078 ~ 110ac */
    u_g1_gfx_testpat_cfg                 g1_gfx_testpat_cfg;                  /* 110b0 */
    u_g1_gfx_testpat_seed                g1_gfx_testpat_seed;                 /* 110b4 */
    u_g1_gfx_dcmp_ctrl                   g1_gfx_dcmp_ctrl;                    /* 110b8 */
    unsigned int                         rsr49;                               /* 110bc */
    u_g1_gfx_debug_ctrl                  g1_gfx_debug_ctrl;                   /* 110c0 */
    unsigned int                         rsr50[7];                            /* 110c4 ~ 110dc */
    u_g1_gfx_in_ar_checksum0             g1_gfx_in_ar_checksum0;              /* 110e0 */
    u_g1_gfx_in_ar_checksum1             g1_gfx_in_ar_checksum1;              /* 110e4 */
    u_g1_gfx_in_gb_checksum0             g1_gfx_in_gb_checksum0;              /* 110e8 */
    u_g1_gfx_in_gb_checksum1             g1_gfx_in_gb_checksum1;              /* 110ec */
    unsigned int                         rsr51[4];                            /* 110f0 ~ 110fc */
    u_g1_gfx_ckey_max                    g1_gfx_ckey_max;                     /* 11100 */
    u_g1_gfx_ckey_min                    g1_gfx_ckey_min;                     /* 11104 */
    u_g1_gfx_ckey_mask                   g1_gfx_ckey_mask;                    /* 11108 */
    unsigned int                         rsr52[29];                           /* 1110c ~ 1117c */
    u_g1_gfx_v5r1_htc_dcmp_glb_info      g1_gfx_v5r1_htc_dcmp_glb_info;       /* 11180 */
    unsigned int                         rsr53[3];                            /* 11184 ~ 1118c */
    u_g1_gfx_v5r1_htc_dcmp_dbg_reg       g1_gfx_v5r1_htc_dcmp_dbg_reg;        /* 11190 */
    unsigned int                         rsr54[27];                           /* 11190 ~ 111fc */
} dpu_regs_type;

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif /* DRV_GRAPHIC_HAL_DPU_REG_DEFINE_H */
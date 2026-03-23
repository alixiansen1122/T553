/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description : gfx crg file
 */

#ifndef COMM_CRG_REG_DEFINE_H
#define COMM_CRG_REG_DEFINE_H

#include "td_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define CRG_VEDIO_CRG_CKEN_CTL     0x56010000
#define CRG_VEDIO_CRG_DIV_CTL      0x56010004
#define CRG_VEDIO_CRG_MCLK_DIV_CTL 0x56010008
#define CRG_VEDIO_CLKEN_CFG        0x52000200
#define CRG_VEDIO_SRST_REG         0x52000204

/* Define the union u_vedio_crg_cken_ctl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved_0              : 2;       /* [1:0] */
        unsigned int dpu_axi_cken            : 1;       /* [1] */
        unsigned int reserved_1              : 1;       /* [3] */
        unsigned int vau_cken                : 1;       /* [4] */
        unsigned int dpu_hd0_cken            : 1;       /* [5] */
        unsigned int dpu_cfg_cken            : 1;       /* [6] */
        unsigned int dpu_ppc_hd0_cken        : 1;       /* [7] */
        unsigned int reserved_2              : 24;      /* [31:8] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vedio_crg_cken_ctl;

/* Define the union u_vedio_crg_div_ctl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved_0              : 8;       /* [7:0] */
        unsigned int vedio_dpu_ppc_hd0_ckdiv : 4;       /* [11:8] */
        unsigned int reserved_1              : 20;      /* [31:12] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vedio_crg_div_ctl;

/* Define the union u_vedio_crg_mclk_div_ctl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dpu_mclk_div            : 27;      /* [26:0] */
        unsigned int dpu_mclk_cken           : 1;       /* [27] */
        unsigned int reserved_0              : 4;       /* [31:28] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_vedio_crg_mclk_div_ctl;

/* Define the union u_video_clken_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved_0              : 1;       /* [0] */
        unsigned int jpgd_clken              : 1;       /* [1] */
        unsigned int mipitx_apb_cken         : 1;       /* [2] */
        unsigned int vau_apb_cken            : 1;       /* [3] */
        unsigned int dpu_apb_cken            : 1;       /* [4] */
        unsigned int vedio_pll2cbb_160m_cken : 1;       /* [5] */
        unsigned int aon_ctrl_mipitx_cken    : 1;       /* [6] */
        unsigned int aon_ctrl_video_cken     : 1;       /* [7] */
        unsigned int reserved_1              : 24;      /* [31..8] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_video_clken_cfg;

/* Define the union u_video_srst_reg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved_0              : 1;       /* [0] */
        unsigned int mipitx_srst_req         : 1;       /* [1] */
        unsigned int vau_srst_req            : 1;       /* [2] */
        unsigned int dpu_srst_req            : 1;       /* [3] */
        unsigned int jpg_srst_req            : 1;       /* [4] */
        unsigned int reserved_1              : 27;      /* [31..5] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_video_srst_reg;

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif /* COMM_CRG_REG_DEFINE_H */

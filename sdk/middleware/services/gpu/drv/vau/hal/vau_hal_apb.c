/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: vau hal ctrl settings
 */
#include "vau_hal_apb.h"
#include "crg_reg_define.h"
#include "vau_reg_define.h"
#include "vau_dfx.h"

static td_float g_cubic_threshold = 0.125f;
static td_void hal_apb_set_vau_srst_req(td_bool reset)
{
    volatile u_video_srst_reg video_srst_reg;

    if (reset) {
        uapi_reg_read(CRG_VEDIO_SRST_REG, video_srst_reg.u32);
        video_srst_reg.bits.vau_srst_req = 1;
        uapi_reg_write(CRG_VEDIO_SRST_REG, video_srst_reg.u32);
    } else {
        uapi_reg_read(CRG_VEDIO_SRST_REG, video_srst_reg.u32);
        video_srst_reg.bits.vau_srst_req = 0;
        uapi_reg_write(CRG_VEDIO_SRST_REG, video_srst_reg.u32);
    }

    return;
}

static td_void hal_apb_set_vau_axi_cken(td_u32 vau_axi_cken)
{
    uapi_unused(vau_axi_cken);
}

static td_void hal_apb_set_vau_work_cken(td_u32 vau_cken)
{
    td_s32 cnt = 0;
    volatile u_vedio_crg_cken_ctl vau_crg_cken_ctl;

    uapi_reg_read(CRG_VEDIO_CRG_CKEN_CTL, vau_crg_cken_ctl.u32);
    vau_crg_cken_ctl.bits.vau_cken = vau_cken;
    uapi_reg_write(CRG_VEDIO_CRG_CKEN_CTL, vau_crg_cken_ctl.u32);

    do {
        cnt++;
        uapi_reg_read(CRG_VEDIO_CRG_CKEN_CTL, vau_crg_cken_ctl.u32);
        if (vau_crg_cken_ctl.bits.vau_cken == vau_cken) {
            break;
        }
    } while (cnt < 10); /* try 10 times to ensure take inffect. */

    return;
}

static td_void hal_apb_set_vau_apb_cken(td_u32 vau_apb_cken)
{
    volatile u_video_clken_cfg video_clken_cfg;
    uapi_reg_read(CRG_VEDIO_CLKEN_CFG, video_clken_cfg.u32);
    video_clken_cfg.bits.vau_apb_cken = vau_apb_cken;
    uapi_reg_write(CRG_VEDIO_CLKEN_CFG, video_clken_cfg.u32);
}

td_void vau_hal_apb_set_video_ckdiv(td_u32 vedio_dpu_vau_ckdiv)
{
    uapi_unused(vedio_dpu_vau_ckdiv);
}

td_void vau_hal_apb_init_queue(td_void)
{
    volatile u_vau_pnext vau_next;
    volatile u_vau_miscellaneous vau_miscellaneous;
    volatile u_vau_mst_outstanding out_standing;

    uapi_reg_read(HAL_VAU_REG_PNEXT, vau_next.u32);
    vau_next.bits.p_next = 0;
    uapi_reg_write(HAL_VAU_REG_PNEXT, vau_next.u32);

    uapi_reg_read(HAL_VAU_REG_MISCELLANEOUS, vau_miscellaneous.u32);
    vau_miscellaneous.bits.ck_gt_en_axi  = 1;
    vau_miscellaneous.bits.ck_gt_en_cfg  = 1;
    vau_miscellaneous.bits.ck_gt_en_calc = 1;
    vau_miscellaneous.bits.ck_gt_en      = 1;
    vau_miscellaneous.bits.init_timer    = 0;
    uapi_reg_write(HAL_VAU_REG_MISCELLANEOUS, vau_miscellaneous.u32);

    uapi_reg_read(HAL_VAU_REG_OUTSTANDING, out_standing.u32);
    out_standing.bits.mstr_woutstanding = 0x7;
    out_standing.bits.mstr_routstanding = 0xF;
    uapi_reg_write(HAL_VAU_REG_OUTSTANDING, out_standing.u32);
}

td_bool vau_hal_apb_is_idle(td_void)
{
    volatile u_vau_work_state status;

    uapi_reg_read(HAL_VAU_REG_STATUS, status.u32);

    return ((status.bits.debug0 == 0) ? TD_TRUE : TD_FALSE);
}

td_s32 vau_hal_apb_reset(td_bool reset)
{
    td_u32 retry = 0;

    hal_apb_set_vau_srst_req(reset);

    while (vau_hal_apb_get_softrst_state() != reset) {
        if (retry++ > 0xFFFF) {
            vau_err("vau hal reset fail!");
            return EXT_FAILURE;
        }
    }

    return EXT_SUCCESS;
}

td_u32 vau_hal_apb_get_current_ndoe(td_void)
{
    volatile u_vau_debug1 current_node;

    uapi_reg_read(HAL_VAU_REG_AQ_ADDR, current_node.u32);

    return current_node.bits.p_last;
}

td_u32 vau_hal_apb_get_softrst_state(td_void)
{
    volatile u_vau_soft_rst_state rst_state;

    uapi_reg_read(HAL_VAU_SOFTRST_STATE, rst_state.u32);

    return rst_state.bits.soft_rst_state;
}

td_void vau_hal_apb_set_clock(td_bool enable)
{
    if (enable) {
        /* brandy aon/apb need config first, also influent dpu */
        hal_apb_set_vau_apb_cken(TD_TRUE);
        hal_apb_set_vau_axi_cken(TD_TRUE);
        hal_apb_set_vau_work_cken(TD_TRUE);
    } else {
        /* brandy aon/apb need config last */
        hal_apb_set_vau_work_cken(TD_FALSE);
        hal_apb_set_vau_axi_cken(TD_FALSE);
    }
}

td_u32 vau_hal_apb_get_int_state(td_void)
{
    volatile u_vau_intstate int_state;
    uapi_reg_read(HAL_VAU_REG_INT_STATE, int_state.u32);
    return int_state.u32;
}

td_void vau_hal_apb_clear_int_state(td_u32 mask)
{
    volatile u_vau_intclr int_clr;
    int_clr.u32 = mask; /* Write directly without read-modify-write to avoid losing new interrupt */
    uapi_reg_write(HAL_VAU_REG_INT_CLR, int_clr.u32);
    return;
}

td_void vau_hal_apb_set_start_node(td_u32 start_node)
{
    volatile u_vau_pnext next_node;

    uapi_reg_read(HAL_VAU_REG_PNEXT, next_node.u32);
    next_node.bits.p_next = start_node;
    uapi_reg_write(HAL_VAU_REG_PNEXT, next_node.u32);
}

td_void vau_hal_apb_set_start_up(td_void)
{
    volatile u_vau_start start_reg;
    uapi_reg_read(HAL_VAU_REG_START, start_reg.u32);
    start_reg.bits.start = 1;
    uapi_reg_write(HAL_VAU_REG_START, start_reg.u32);
}

static td_void vau_hal_apb_vector_draw_init_sample(td_void)
{
    u_vau_draw_sampleradius sample_radius;
    u_vau_draw_rndr_aa_smpl_x0 x[8]; /* 8 sample pos */
    u_vau_draw_rndr_aa_smpl_y0 y[8]; /* 8 sample pos */

    sample_radius.bits.m_sampleradious = 0.5; /* 0.5 : default sample radius for 8x */

    x[0].bits.sample_x_0 = 0.4375; /* 0 , 0.4375 : default sample pos for 8x */
    x[1].bits.sample_x_0 = 0.9375; /* 1 , 0.9375 : default sample pos for 8x */
    x[2].bits.sample_x_0 = 0.0625; /* 2 , 0.0625 : default sample pos for 8x */
    x[3].bits.sample_x_0 = 0.3125; /* 3 , 0.3125 : default sample pos for 8x */
    x[4].bits.sample_x_0 = 0.6875; /* 4 , 0.6875 : default sample pos for 8x */
    x[5].bits.sample_x_0 = 0.1875; /* 5 , 0.1875 : default sample pos for 8x */
    x[6].bits.sample_x_0 = 0.8125; /* 6 , 0.8125 : default sample pos for 8x */
    x[7].bits.sample_x_0 = 0.5625; /* 7 , 0.5625 : default sample pos for 8x */
    y[0].bits.sample_y_0 = 0.0625; /* 0 , 0.0625 : default sample pos for 8x */
    y[1].bits.sample_y_0 = 0.1875; /* 1 , 0.1875 : default sample pos for 8x */
    y[2].bits.sample_y_0 = 0.3125; /* 2 , 0.3125 : default sample pos for 8x */
    y[3].bits.sample_y_0 = 0.4375; /* 3 , 0.4375 : default sample pos for 8x */
    y[4].bits.sample_y_0 = 0.5625; /* 4 , 0.5625 : default sample pos for 8x */
    y[5].bits.sample_y_0 = 0.6875; /* 5 , 0.6875 : default sample pos for 8x */
    y[6].bits.sample_y_0 = 0.8125; /* 6 , 0.8125 : default sample pos for 8x */
    y[7].bits.sample_y_0 = 0.9375; /* 7 , 0.9375 : default sample pos for 8x */

    uapi_reg_write(HAL_VAU_DRAW_SAMPLERADIUS, sample_radius.u32);
    uapi_reg_write((HAL_VAU_DRAW_SAMPLERADIUS + 0x4), x[0].u32);  /* index 0 */
    uapi_reg_write((HAL_VAU_DRAW_SAMPLERADIUS + 0x8), x[1].u32);  /* index 1 */
    uapi_reg_write((HAL_VAU_DRAW_SAMPLERADIUS + 0xc), x[2].u32);  /* index 2 */
    uapi_reg_write((HAL_VAU_DRAW_SAMPLERADIUS + 0x10), x[3].u32); /* index 3 */
    uapi_reg_write((HAL_VAU_DRAW_SAMPLERADIUS + 0x14), x[4].u32); /* index 4 */
    uapi_reg_write((HAL_VAU_DRAW_SAMPLERADIUS + 0x18), x[5].u32); /* index 5 */
    uapi_reg_write((HAL_VAU_DRAW_SAMPLERADIUS + 0x1c), x[6].u32); /* index 6 */
    uapi_reg_write((HAL_VAU_DRAW_SAMPLERADIUS + 0x20), x[7].u32); /* index 7 */
    uapi_reg_write((HAL_VAU_DRAW_SAMPLERADIUS + 0x24), y[0].u32); /* index 0 */
    uapi_reg_write((HAL_VAU_DRAW_SAMPLERADIUS + 0x28), y[1].u32); /* index 1 */
    uapi_reg_write((HAL_VAU_DRAW_SAMPLERADIUS + 0x2c), y[2].u32); /* index 2 */
    uapi_reg_write((HAL_VAU_DRAW_SAMPLERADIUS + 0x30), y[3].u32); /* index 3 */
    uapi_reg_write((HAL_VAU_DRAW_SAMPLERADIUS + 0x34), y[4].u32); /* index 4 */
    uapi_reg_write((HAL_VAU_DRAW_SAMPLERADIUS + 0x38), y[5].u32); /* index 5 */
    uapi_reg_write((HAL_VAU_DRAW_SAMPLERADIUS + 0x3c), y[6].u32); /* index 6 */
    uapi_reg_write((HAL_VAU_DRAW_SAMPLERADIUS + 0x40), y[7].u32); /* index 7 */
}

td_void vau_hal_apb_vector_draw_init(td_void)
{
    u_vau_draw_threshold_quad quad;
    u_vau_draw_threshold_cubic cubic;
    u_vau_draw_cvrg_weight0 weight0;
    u_vau_draw_cvrg_weight1 weight1;

    quad.bits.threshold_quad = 0.125; /* 0.125 : default threshold for quad split */
    cubic.bits.threshold_cubic = g_cubic_threshold; /* 0.125 : default threshold for cubic split */

    weight0.bits.weight0 = 32; /* 32 default weithg for 8x */
    weight0.bits.weight1 = 32; /* 32 default weithg for 8x */
    weight0.bits.weight2 = 32; /* 32 default weithg for 8x */
    weight0.bits.weight3 = 32; /* 32 default weithg for 8x */
    weight1.bits.weight4 = 32; /* 32 default weithg for 8x */
    weight1.bits.weight5 = 32; /* 32 default weithg for 8x */
    weight1.bits.weight6 = 32; /* 32 default weithg for 8x */
    weight1.bits.weight7 = 31; /* 31 default weithg for 8x */

    uapi_reg_write(HAL_VAU_DRAW_THRESHOLD_QUAD, quad.u32);
    uapi_reg_write(HAL_VAU_DRAW_THRESHOLD_CUBIC, cubic.u32);

    uapi_reg_write(HAL_VAU_DRAW_CVRG_WEIGHT0, weight0.u32);
    uapi_reg_write(HAL_VAU_DRAW_CVRG_WEIGHT1, weight1.u32);

    vau_hal_apb_vector_draw_init_sample();
}

td_void vau_hal_apb_set_cubic_threshold(td_float cubic_threshold)
{
    u_vau_draw_threshold_cubic cubic;

    cubic.bits.threshold_cubic = cubic_threshold;
    uapi_reg_write(HAL_VAU_DRAW_THRESHOLD_CUBIC, cubic.u32);

    g_cubic_threshold = cubic_threshold;
}

td_void vau_hal_apb_vector_cmddata_range(td_u32 type, td_u32 start, td_u32 end)
{
    if (type == VAU_VECTOR_RANGE_CMD) {
        uapi_reg_write(HAL_VAU_DRAW_CMDBUF_START_ADDR, start);
        uapi_reg_write(HAL_VAU_DRAW_CMDBUF_END_ADDR, end);
    } else {
        uapi_reg_write(HAL_VAU_DRAW_PATHBUF_START_ADDR, start);
        uapi_reg_write(HAL_VAU_DRAW_PATHBUF_END_ADDR, end);
    }
    return;
}

td_void vau_hal_apb_sram_ctrl(td_bool lp_en)
{
    u_vau_sram_ctrl sram_ctrl;

    if (lp_en) {
        sram_ctrl.bits.vau_lp_mode  = 0x2;   /* lp work mode */
        sram_ctrl.bits.vau_block_en = 0x3ff; /* block en */
    } else {
        sram_ctrl.u32 = 0x0;
    }

    uapi_reg_write(HAL_VAU_SRAM_CTRL, sram_ctrl.u32);

    return;
}

static td_void vau_print_reg(td_u32 base, td_u32 start, td_u32 end)
{
    td_u32 size  = end - start;
    td_u32 *addr = (td_u32 *)(base + start);

    for (td_u32 i = 0; i <= size / 16; i++) {                                       /* 16:index */
        vau_print("%04x:  %08x %08x %08x %08x\n", ((td_u32)addr + i * 16),         /* 16:index */
                  addr[i * 4], addr[i * 4 + 1], addr[i * 4 + 2], addr[i * 4 + 3]); /* 2,3,4:index */
    }
}

td_void vau_hal_apb_print_reg(td_u32 base, td_u32 start, td_u32 end)
{
    volatile td_u32 value;

    vau_print_reg(base, start, end);
    uapi_reg_read(HAL_VAU_DRAW_PATH_CMD_ADDR, value);
    td_u8 *cmds = (td_u8 *)(uintptr_t)value;
    uapi_reg_read(HAL_VAU_DRAW_PATH_DATA_ADDR, value);
    td_float *datas = (td_float *)(uintptr_t)value;
    uapi_reg_read(HAL_VAU_DRAW_PATH_NUM_ADDR, value);
    u_vau_draw_path_num num = { .u32 = value };

    vau_dfx_print_path(cmds, datas, num.bits.cmd_num, num.bits.data_num);

    return;
}

td_void vau_hal_apb_print_all_reg(td_void)
{
    vau_print_reg(HAL_VAU_REG_BASEADDR, 0, 0x164); /* reg:0~0x164 */
    vau_print_reg(HAL_VAU_REG_BASEADDR, 0x200, 0x260); /* reg:0x200~0x260 */
    vau_print_reg(HAL_VAU_REG_BASEADDR, 0x500, 0x534); /* reg:0x500~0x534 */
    vau_print_reg(HAL_VAU_REG_BASEADDR, 0xA00, 0xA60); /* reg:0xA00~0xA60 */
    vau_hal_apb_print_reg(HAL_VAU_REG_BASEADDR, 0x4000, 0x40AC); /* reg:0x4000~0x40AC */

    return;
}

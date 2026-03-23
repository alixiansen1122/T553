/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: jpeg hal
 */

#include "jpeg_hal.h"
#include "drv_jpeg_struct.h"
#include "jpeg_reg_define.h"

/*-------------------------------- macro define -------------------------------------------*/

/*-------------------------------- struct define ------------------------------------------*/

/*-------------------------------- func declares ------------------------------------------*/

/*-------------------------------- par define ---------------------------------------------*/

static volatile jpeg_regs_ptr g_jpeg_reg = TD_NULL;

/*-------------------------------- func release -------------------------------------------*/

td_void jpeg_hal_init(volatile td_u32 *reg_base)
{
    g_jpeg_reg = (jpeg_regs_ptr)reg_base;
}

td_void jpeg_hal_deinit(td_void)
{
    g_jpeg_reg = TD_NULL;
}

static td_u32 jpeg_hal_read(uintptr_t addr)
{
    return *((volatile td_u32 *)addr);
}

static td_void jpeg_hal_write(uintptr_t addr, td_u32 value)
{
    *((volatile td_u32 *)addr) = value;
}

td_void jpeg_hal_set_ck_enable(td_void)
{
    volatile u_freq_scale freq_scale;
    freq_scale.u32 = jpeg_hal_read((uintptr_t)(&(g_jpeg_reg->freq_scale.u32)));
    freq_scale.bits.ck_gt_en = 0x1;
    jpeg_hal_write((uintptr_t)(&(g_jpeg_reg->freq_scale.u32)), freq_scale.u32);
}

td_void jpeg_hal_set_ck_disable(td_void)
{
    volatile u_freq_scale freq_scale;
    freq_scale.u32 = jpeg_hal_read((uintptr_t)(&(g_jpeg_reg->freq_scale.u32)));
    freq_scale.bits.ck_gt_en = 0;
    jpeg_hal_write((uintptr_t)(&(g_jpeg_reg->freq_scale.u32)), freq_scale.u32);
}

td_void jpeg_hal_set_outstanding(td_void)
{
    volatile u_freq_scale freq_scale;
    freq_scale.u32 = jpeg_hal_read((uintptr_t)(&(g_jpeg_reg->freq_scale.u32)));
    freq_scale.bits.outstanding = 0x3;
    jpeg_hal_write((uintptr_t)(&(g_jpeg_reg->freq_scale.u32)), freq_scale.u32);
}

td_void jpeg_hal_get_resume_state(td_u32 *state)
{
    volatile u_jpeg_dec_start jpeg_dec_start;
    jpeg_dec_start.u32 = jpeg_hal_read((uintptr_t)(&(g_jpeg_reg->jpeg_dec_start.u32)));
    *state = jpeg_dec_start.bits.rst_busy;
}

td_void jpeg_hal_set_start(td_u32 value)
{
    volatile u_jpeg_dec_start jpeg_dec_start;
    jpeg_dec_start.u32 = jpeg_hal_read((uintptr_t)(&(g_jpeg_reg->jpeg_dec_start.u32)));
    jpeg_dec_start.bits.jpeg_dec_start = value;
    jpeg_hal_write((uintptr_t)(&(g_jpeg_reg->jpeg_dec_start.u32)), jpeg_dec_start.u32);
}

td_void jpeg_hal_set_resume(td_u32 value)
{
    volatile u_jpeg_resume_start jpeg_resume_start;
    jpeg_resume_start.u32 = jpeg_hal_read((uintptr_t)(&(g_jpeg_reg->jpeg_resume_start.u32)));
    jpeg_resume_start.bits.jpeg_resume_start = value;
    jpeg_hal_write((uintptr_t)(&(g_jpeg_reg->jpeg_resume_start.u32)), jpeg_resume_start.u32);
}

td_void jpeg_hal_set_eoi(td_u32 value)
{
    volatile u_jpeg_resume_start jpeg_resume_start;
    jpeg_resume_start.u32 = jpeg_hal_read((uintptr_t)(&(g_jpeg_reg->jpeg_resume_start.u32)));
    jpeg_resume_start.bits.last_resume_in_pic = value;
    jpeg_hal_write((uintptr_t)(&(g_jpeg_reg->jpeg_resume_start.u32)), jpeg_resume_start.u32);
}

td_void jpeg_hal_set_picture_size(td_u32 width, td_u32 height)
{
    volatile u_picture_size picture_size;
    picture_size.u32 = jpeg_hal_read((uintptr_t)(&(g_jpeg_reg->picture_size.u32)));
    picture_size.bits.pic_width_in_mcu  = width;
    picture_size.bits.pic_height_in_mcu = height;
    jpeg_hal_write((uintptr_t)(&(g_jpeg_reg->picture_size.u32)), picture_size.u32);
}

td_void jpeg_hal_set_picture_fmt(td_u32 fmt)
{
    volatile u_picture_type picture_type;
    picture_type.u32 = jpeg_hal_read((uintptr_t)(&(g_jpeg_reg->picture_type.u32)));
    picture_type.bits.pic_type = fmt;
    jpeg_hal_write((uintptr_t)(&(g_jpeg_reg->picture_type.u32)), picture_type.u32);
}

td_void jpeg_hal_set_stride(td_u32 y_stride, td_u32 uv_stride)
{
    volatile u_jpeg_stride jpeg_stride;
    jpeg_stride.u32 = jpeg_hal_read((uintptr_t)(&(g_jpeg_reg->jpeg_stride.u32)));
    jpeg_stride.bits.y_stride  = y_stride;
    jpeg_stride.bits.uv_stride = uv_stride;
    jpeg_hal_write((uintptr_t)(&(g_jpeg_reg->jpeg_stride.u32)), jpeg_stride.u32);
}

td_void jpeg_hal_set_scale(td_u32 scale_vale)
{
    volatile u_freq_scale freq_scale;
    freq_scale.u32 = jpeg_hal_read((uintptr_t)(&(g_jpeg_reg->freq_scale.u32)));
    freq_scale.bits.freq_scale = scale_vale;
    jpeg_hal_write((uintptr_t)(&(g_jpeg_reg->freq_scale.u32)), freq_scale.u32);
}

td_void jpeg_hal_set_output_size(td_u32 height)
{
    volatile u_out_height_pix out_height_pix;
    out_height_pix.u32 = jpeg_hal_read((uintptr_t)(&(g_jpeg_reg->out_height_pix.u32)));
    out_height_pix.bits.yout_height_pix = height;
    jpeg_hal_write((uintptr_t)(&(g_jpeg_reg->out_height_pix.u32)), out_height_pix.u32);
}

td_void jpeg_hal_set_output_fmt(td_u32 fmt)
{
    volatile u_out_type out_type;
    out_type.u32 = jpeg_hal_read((uintptr_t)(&(g_jpeg_reg->out_type.u32)));
    out_type.bits.jidct_yuv420_en = fmt;
    jpeg_hal_write((uintptr_t)(&(g_jpeg_reg->out_type.u32)), out_type.u32);
}

td_void jpeg_hal_set_intmask(td_void)
{
    volatile u_int_mask int_mask;
    int_mask.u32 = jpeg_hal_read((uintptr_t)(&(g_jpeg_reg->int_mask.u32)));
    int_mask.bits.intm_dec_fnsh   = 0;
    int_mask.bits.intm_dec_err    = 0;
    int_mask.bits.intm_bs_res     = 0;
    int_mask.bits.intm_over_time  = 0;
    int_mask.bits.intm_line_eql   = 1;
    jpeg_hal_write((uintptr_t)(&(g_jpeg_reg->int_mask.u32)), int_mask.u32);
}

td_void jpeg_hal_set_inttype(td_u32 inttype)
{
    jpeg_hal_write((uintptr_t)(&(g_jpeg_reg->jpeg_int.u32)), inttype);
}

td_void jpeg_hal_get_inttype(td_u32 *inttype)
{
    *inttype = jpeg_hal_read((uintptr_t)(&(g_jpeg_reg->jpeg_int.u32)));
}

td_void jpeg_hal_set_factor(td_u32 y_fac, td_u32 u_fac, td_u32 v_fac)
{
    volatile u_sampling_factor sampling_factor;
    sampling_factor.u32 = jpeg_hal_read((uintptr_t)(&(g_jpeg_reg->sampling_factor.u32)));
    sampling_factor.bits.y_fac = y_fac;
    sampling_factor.bits.u_fac = u_fac;
    sampling_factor.bits.v_fac = v_fac;
    jpeg_hal_write((uintptr_t)(&(g_jpeg_reg->sampling_factor.u32)), sampling_factor.u32);
}

td_void jpeg_hal_set_dri(td_u32 dri_value)
{
    volatile u_dri dri;
    dri.u32 = jpeg_hal_read((uintptr_t)(&(g_jpeg_reg->dri.u32)));
    dri.bits.jcfg2jvld_ri = dri_value;
    jpeg_hal_write((uintptr_t)(&(g_jpeg_reg->dri.u32)), dri.u32);
}

td_void jpeg_hal_set_dqt_y(const td_u8 *table, td_u32 table_size)
{
    td_u32 i;
    volatile u_quant_table quant_table;

    if ((table_size != DRV_JPEG_MAX_QTB_SIZE) ||
        ((sizeof(g_jpeg_reg->quant_table) / sizeof(u_quant_table)) != DRV_JPEG_MAX_QTB_SIZE)) {
        return;
    }
    for (i = 0; i < table_size; i++) {
        quant_table.u32 = jpeg_hal_read((uintptr_t)(&(g_jpeg_reg->quant_table[i].u32)));
        quant_table.bits.qtab_y = table[i];
        jpeg_hal_write((uintptr_t)(&(g_jpeg_reg->quant_table[i].u32)), quant_table.u32);
    }
}

td_void jpeg_hal_set_dqt_cb(const td_u8 *table, td_u32 table_size)
{
    td_u32 i;
    volatile u_quant_table quant_table;

    if ((table_size != DRV_JPEG_MAX_QTB_SIZE) ||
        ((sizeof(g_jpeg_reg->quant_table) / sizeof(u_quant_table)) != DRV_JPEG_MAX_QTB_SIZE)) {
        return;
    }
    for (i = 0; i < table_size; i++) {
        quant_table.u32 = jpeg_hal_read((uintptr_t)(&(g_jpeg_reg->quant_table[i].u32)));
        quant_table.bits.qtab_cb = table[i];
        jpeg_hal_write((uintptr_t)(&(g_jpeg_reg->quant_table[i].u32)), quant_table.u32);
    }
}

td_void jpeg_hal_set_dqt_cr(const td_u8 *table, td_u32 table_size)
{
    td_u32 i;
    volatile u_quant_table quant_table;

    if ((table_size != DRV_JPEG_MAX_QTB_SIZE) ||
        ((sizeof(g_jpeg_reg->quant_table) / sizeof(u_quant_table)) != DRV_JPEG_MAX_QTB_SIZE)) {
        return;
    }
    for (i = 0; i < table_size; i++) {
        quant_table.u32 = jpeg_hal_read((uintptr_t)(&(g_jpeg_reg->quant_table[i].u32)));
        quant_table.bits.qtab_cr = table[i];
        jpeg_hal_write((uintptr_t)(&(g_jpeg_reg->quant_table[i].u32)), quant_table.u32);
    }
}

td_void jpeg_hal_set_dht_dc_lu_table(const td_u16 *table, td_u32 table_size)
{
    td_u32 i;
    volatile u_hdc_table hdc_table;

    if ((table_size != DRV_JPEG_MAX_HTB_DC_SIZE) ||
        ((sizeof(g_jpeg_reg->hdc_table) / sizeof(u_hdc_table)) != DRV_JPEG_MAX_HTB_DC_SIZE)) {
        return;
    }
    for (i = 0; i < table_size; i++) {
        hdc_table.u32 = jpeg_hal_read((uintptr_t)(&(g_jpeg_reg->hdc_table[i].u32)));
        hdc_table.bits.hdc_lu = table[i];
        jpeg_hal_write((uintptr_t)(&(g_jpeg_reg->hdc_table[i].u32)), hdc_table.u32);
    }
}

td_void jpeg_hal_set_dht_dc_ch_table(const td_u16 *table, td_u32 table_size)
{
    td_u32 i;
    volatile u_hdc_table hdc_table;

    if ((table_size != DRV_JPEG_MAX_HTB_DC_SIZE) ||
        ((sizeof(g_jpeg_reg->hdc_table) / sizeof(u_hdc_table)) != DRV_JPEG_MAX_HTB_DC_SIZE)) {
        return;
    }
    for (i = 0; i < table_size; i++) {
        hdc_table.u32 = jpeg_hal_read((uintptr_t)(&(g_jpeg_reg->hdc_table[i].u32)));
        hdc_table.bits.hdc_ch = table[i];
        jpeg_hal_write((uintptr_t)(&(g_jpeg_reg->hdc_table[i].u32)), hdc_table.u32);
    }
}

td_void jpeg_hal_set_dht_ac_lu_mincode_even(const td_u8 *table, td_u32 table_size)
{
    td_u32 i;
    volatile u_hac_min_table hac_min_table;

    if ((table_size != DRV_JPEG_MAX_HTB_AC_MIN_SIZE) ||
        ((sizeof(g_jpeg_reg->hac_min_table) / sizeof(u_hac_min_table)) != DRV_JPEG_MAX_HTB_AC_MIN_SIZE)) {
        return;
    }
    for (i = 0; i < table_size; i++) {
        hac_min_table.u32 = jpeg_hal_read((uintptr_t)(&(g_jpeg_reg->hac_min_table[i].u32)));
        hac_min_table.bits.hac_lu_mincode_even = table[i];
        jpeg_hal_write((uintptr_t)(&(g_jpeg_reg->hac_min_table[i].u32)), hac_min_table.u32);
    }
}

td_void jpeg_hal_set_dht_ac_lu_mincode_odd(const td_u8 *table, td_u32 table_size)
{
    td_u32 i;
    volatile u_hac_min_table hac_min_table;

    if ((table_size != DRV_JPEG_MAX_HTB_AC_MIN_SIZE) ||
        ((sizeof(g_jpeg_reg->hac_min_table) / sizeof(u_hac_min_table)) != DRV_JPEG_MAX_HTB_AC_MIN_SIZE)) {
        return;
    }
    for (i = 0; i < table_size; i++) {
        hac_min_table.u32 = jpeg_hal_read((uintptr_t)(&(g_jpeg_reg->hac_min_table[i].u32)));
        hac_min_table.bits.hac_lu_mincode_odd = table[i];
        jpeg_hal_write((uintptr_t)(&(g_jpeg_reg->hac_min_table[i].u32)), hac_min_table.u32);
    }
}

td_void jpeg_hal_set_dht_ac_ch_mincode_even(const td_u8 *table, td_u32 table_size)
{
    td_u32 i;
    volatile u_hac_min_table hac_min_table;

    if ((table_size != DRV_JPEG_MAX_HTB_AC_MIN_SIZE) ||
        ((sizeof(g_jpeg_reg->hac_min_table) / sizeof(u_hac_min_table)) != DRV_JPEG_MAX_HTB_AC_MIN_SIZE)) {
        return;
    }
    for (i = 0; i < table_size; i++) {
        hac_min_table.u32 = jpeg_hal_read((uintptr_t)(&(g_jpeg_reg->hac_min_table[i].u32)));
        hac_min_table.bits.hac_ch_mincode_even = table[i];
        jpeg_hal_write((uintptr_t)(&(g_jpeg_reg->hac_min_table[i].u32)), hac_min_table.u32);
    }
}

td_void jpeg_hal_set_dht_ac_ch_mincode_odd(const td_u8 *table, td_u32 table_size)
{
    td_u32 i;
    volatile u_hac_min_table hac_min_table;

    if ((table_size != DRV_JPEG_MAX_HTB_AC_MIN_SIZE) ||
        ((sizeof(g_jpeg_reg->hac_min_table) / sizeof(u_hac_min_table)) != DRV_JPEG_MAX_HTB_AC_MIN_SIZE)) {
        return;
    }
    for (i = 0; i < table_size; i++) {
        hac_min_table.u32 = jpeg_hal_read((uintptr_t)(&(g_jpeg_reg->hac_min_table[i].u32)));
        hac_min_table.bits.hac_ch_mincode_odd = table[i];
        jpeg_hal_write((uintptr_t)(&(g_jpeg_reg->hac_min_table[i].u32)), hac_min_table.u32);
    }
}

td_void jpeg_hal_set_dht_ac_lu_base_even(const td_u8 *table, td_u32 table_size)
{
    td_u32 i;
    volatile u_hac_base_table hac_base_table;

    if ((table_size != DRV_JPEG_MAX_HTB_AC_BASE_SIZE) ||
        ((sizeof(g_jpeg_reg->hac_base_table) / sizeof(u_hac_base_table)) != DRV_JPEG_MAX_HTB_AC_BASE_SIZE)) {
        return;
    }
    for (i = 0; i < table_size; i++) {
        hac_base_table.u32 = jpeg_hal_read((uintptr_t)(&(g_jpeg_reg->hac_base_table[i].u32)));
        hac_base_table.bits.hac_lu_base_even = table[i];
        jpeg_hal_write((uintptr_t)(&(g_jpeg_reg->hac_base_table[i].u32)), hac_base_table.u32);
    }
}

td_void jpeg_hal_set_dht_ac_lu_base_odd(const td_u8 *table, td_u32 table_size)
{
    td_u32 i;
    volatile u_hac_base_table hac_base_table;

    if ((table_size != DRV_JPEG_MAX_HTB_AC_BASE_SIZE) ||
        ((sizeof(g_jpeg_reg->hac_base_table) / sizeof(u_hac_base_table)) != DRV_JPEG_MAX_HTB_AC_BASE_SIZE)) {
        return;
    }
    for (i = 0; i < table_size; i++) {
        hac_base_table.u32 = jpeg_hal_read((uintptr_t)(&(g_jpeg_reg->hac_base_table[i].u32)));
        hac_base_table.bits.hac_lu_base_odd = table[i];
        jpeg_hal_write((uintptr_t)(&(g_jpeg_reg->hac_base_table[i].u32)), hac_base_table.u32);
    }
}

td_void jpeg_hal_set_dht_ac_ch_base_even(const td_u8 *table, td_u32 table_size)
{
    td_u32 i;
    volatile u_hac_base_table hac_base_table;

    if ((table_size != DRV_JPEG_MAX_HTB_AC_BASE_SIZE) ||
        ((sizeof(g_jpeg_reg->hac_base_table) / sizeof(u_hac_base_table)) != DRV_JPEG_MAX_HTB_AC_BASE_SIZE)) {
        return;
    }
    for (i = 0; i < table_size; i++) {
        hac_base_table.u32 = jpeg_hal_read((uintptr_t)(&(g_jpeg_reg->hac_base_table[i].u32)));
        hac_base_table.bits.hac_ch_base_even = table[i];
        jpeg_hal_write((uintptr_t)(&(g_jpeg_reg->hac_base_table[i].u32)), hac_base_table.u32);
    }
}

td_void jpeg_hal_set_dht_ac_ch_base_odd(const td_u8 *table, td_u32 table_size)
{
    td_u32 i;
    volatile u_hac_base_table hac_base_table;

    if ((table_size != DRV_JPEG_MAX_HTB_AC_BASE_SIZE) ||
        ((sizeof(g_jpeg_reg->hac_base_table) / sizeof(u_hac_base_table)) != DRV_JPEG_MAX_HTB_AC_BASE_SIZE)) {
        return;
    }
    for (i = 0; i < table_size; i++) {
        hac_base_table.u32 = jpeg_hal_read((uintptr_t)(&(g_jpeg_reg->hac_base_table[i].u32)));
        hac_base_table.bits.hac_ch_base_odd = table[i];
        jpeg_hal_write((uintptr_t)(&(g_jpeg_reg->hac_base_table[i].u32)), hac_base_table.u32);
    }
}

td_void jpeg_hal_set_dht_ac_lu_symblo(const td_u8 *table, td_u32 table_size)
{
    td_u32 i;
    volatile u_hac_symbol_table hac_symbol_table;

    if ((table_size != DRV_JPEG_MAX_HTB_AC_SYMBOL_SIZE) ||
        ((sizeof(g_jpeg_reg->hac_symbol_table) / sizeof(u_hac_symbol_table)) != DRV_JPEG_MAX_HTB_AC_SYMBOL_SIZE)) {
        return;
    }
    for (i = 0; i < table_size; i++) {
        hac_symbol_table.u32 = jpeg_hal_read((uintptr_t)(&(g_jpeg_reg->hac_symbol_table[i].u32)));
        hac_symbol_table.bits.rs_luma = table[i];
        jpeg_hal_write((uintptr_t)(&(g_jpeg_reg->hac_symbol_table[i].u32)), hac_symbol_table.u32);
    }
}

td_void jpeg_hal_set_dht_ac_ch_symblo(const td_u8 *table, td_u32 table_size)
{
    td_u32 i;
    volatile u_hac_symbol_table hac_symbol_table;

    if ((table_size != DRV_JPEG_MAX_HTB_AC_SYMBOL_SIZE) ||
        ((sizeof(g_jpeg_reg->hac_symbol_table) / sizeof(u_hac_symbol_table)) != DRV_JPEG_MAX_HTB_AC_SYMBOL_SIZE)) {
        return;
    }
    for (i = 0; i < table_size; i++) {
        hac_symbol_table.u32 = jpeg_hal_read((uintptr_t)(&(g_jpeg_reg->hac_symbol_table[i].u32)));
        hac_symbol_table.bits.rs_chroma = table[i];
        jpeg_hal_write((uintptr_t)(&(g_jpeg_reg->hac_symbol_table[i].u32)), hac_symbol_table.u32);
    }
}

td_void jpeg_hal_set_stream_start_addr(td_u32 addr)
{
    jpeg_hal_write((uintptr_t)(&(g_jpeg_reg->bitbuffer_staddr)), addr);
}

td_void jpeg_hal_set_stream_end_addr(td_u32 addr)
{
    jpeg_hal_write((uintptr_t)(&(g_jpeg_reg->bitbuffer_endaddr)), addr);
}

td_void jpeg_hal_set_data_start_addr(td_u32 addr)
{
    jpeg_hal_write((uintptr_t)(&(g_jpeg_reg->bitstreams_staddr)), addr);
}

td_void jpeg_hal_set_data_end_addr(td_u32 addr)
{
    jpeg_hal_write((uintptr_t)(&(g_jpeg_reg->bitstreams_endaddr)), addr);
}

td_void jpeg_hal_set_y_addr(td_u32 addr)
{
    jpeg_hal_write((uintptr_t)(&(g_jpeg_reg->picture_ystaddr)), addr);
}

td_void jpeg_hal_set_uv_addr(td_u32 addr)
{
    jpeg_hal_write((uintptr_t)(&(g_jpeg_reg->picture_uvstaddr)), addr);
}

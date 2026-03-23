/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: jpeg hal
 */

#ifndef DRV_GRAPHICS_JPEG_HAL_H
#define DRV_GRAPHICS_JPEG_HAL_H

#include "td_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*-------------------------------- macro define -------------------------------------------*/

/*-------------------------------- struct define ------------------------------------------*/

/*-------------------------------- func declares ------------------------------------------*/

td_void jpeg_hal_init(volatile td_u32 *reg_base);
td_void jpeg_hal_deinit(td_void);

td_void jpeg_hal_set_ck_enable(td_void);
td_void jpeg_hal_set_ck_disable(td_void);
td_void jpeg_hal_set_outstanding(td_void);
td_void jpeg_hal_get_resume_state(td_u32 *state);

td_void jpeg_hal_set_start(td_u32 value);
td_void jpeg_hal_set_resume(td_u32 value);
td_void jpeg_hal_set_eoi(td_u32 value);

td_void jpeg_hal_set_picture_size(td_u32 width, td_u32 height);
td_void jpeg_hal_set_picture_fmt(td_u32 fmt);

td_void jpeg_hal_set_stride(td_u32 y_stride, td_u32 uv_stride);
td_void jpeg_hal_set_scale(td_u32 scale_vale);
td_void jpeg_hal_set_output_size(td_u32 height);
td_void jpeg_hal_set_output_fmt(td_u32 fmt);

td_void jpeg_hal_set_intmask(td_void);
td_void jpeg_hal_set_inttype(td_u32 inttype);
td_void jpeg_hal_get_inttype(td_u32 *inttype);

td_void jpeg_hal_set_factor(td_u32 y_fac, td_u32 u_fac, td_u32 v_fac);
td_void jpeg_hal_set_dri(td_u32 dri_value);

td_void jpeg_hal_set_dqt_y(const td_u8 *table, td_u32 table_size);
td_void jpeg_hal_set_dqt_cb(const td_u8 *table, td_u32 table_size);
td_void jpeg_hal_set_dqt_cr(const td_u8 *table, td_u32 table_size);

td_void jpeg_hal_set_dht_dc_lu_table(const td_u16 *table, td_u32 table_size);
td_void jpeg_hal_set_dht_dc_ch_table(const td_u16 *table, td_u32 table_size);

td_void jpeg_hal_set_dht_ac_lu_mincode_even(const td_u8 *table, td_u32 table_size);
td_void jpeg_hal_set_dht_ac_lu_mincode_odd(const td_u8 *table, td_u32 table_size);
td_void jpeg_hal_set_dht_ac_ch_mincode_even(const td_u8 *table, td_u32 table_size);
td_void jpeg_hal_set_dht_ac_ch_mincode_odd(const td_u8 *table, td_u32 table_size);

td_void jpeg_hal_set_dht_ac_lu_base_even(const td_u8 *table, td_u32 table_size);
td_void jpeg_hal_set_dht_ac_lu_base_odd(const td_u8 *table, td_u32 table_size);
td_void jpeg_hal_set_dht_ac_ch_base_even(const td_u8 *table, td_u32 table_size);
td_void jpeg_hal_set_dht_ac_ch_base_odd(const td_u8 *table, td_u32 table_size);

td_void jpeg_hal_set_dht_ac_lu_symblo(const td_u8 *table, td_u32 table_size);
td_void jpeg_hal_set_dht_ac_ch_symblo(const td_u8 *table, td_u32 table_size);

td_void jpeg_hal_set_stream_start_addr(td_u32 addr);
td_void jpeg_hal_set_stream_end_addr(td_u32 addr);
td_void jpeg_hal_set_data_start_addr(td_u32 addr);
td_void jpeg_hal_set_data_end_addr(td_u32 addr);
td_void jpeg_hal_set_y_addr(td_u32 addr);
td_void jpeg_hal_set_uv_addr(td_u32 addr);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* DRV_GRAPHICS_JPEG_HAL_H */

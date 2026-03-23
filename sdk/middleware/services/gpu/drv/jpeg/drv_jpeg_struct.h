/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: drv jpeg struct
 */

#ifndef DRV_JPEG_STRUCT_H
#define DRV_JPEG_STRUCT_H

#include "td_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*-------------------------------- macro define -------------------------------------------*/

#define DRV_JPEG_MASK                     0x0123456789ABCDEF
#define DRV_JPEG_MAX_QTB_SIZE             64
#define DRV_JPEG_MAX_HTB_DC_SIZE          12
#define DRV_JPEG_MAX_HTB_AC_MIN_SIZE      8
#define DRV_JPEG_MAX_HTB_AC_BASE_SIZE     8
#define DRV_JPEG_MAX_HTB_AC_SYMBOL_SIZE   256

/*-------------------------------- struct define ------------------------------------------*/

typedef enum {
    DRV_JPEG_INT_TYPE_NONE = 0,
    DRV_JPEG_INT_TYPE_CONTINUE,
    DRV_JPEG_INT_TYPE_LOWDEALY,
    DRV_JPEG_INT_TYPE_FINISH,
    DRV_JPEG_INT_TYPE_ERROR,
    DRV_JPEG_INT_TYPE_STREAM_ERROR,
    DRV_JPEG_INT_TYPE_MAX
} jpeg_int_type;

typedef struct {
    td_u64 mask;
    jpeg_int_type int_type;
} drv_jpeg_intstatus;

typedef struct {
    td_u64 mask;
    td_bool is_start;
    td_u8 is_eoi;
    td_u8 start_value;
    td_u8 resume_value;
    td_u32 restart_interval;
    td_u32 stream_buf_size;
    td_u64 stream_phy_buf;
    td_u32 data_buf_size;
    td_u64 data_phy_buf;
    td_char *y_vir_buf;
    td_u64 y_phy_buf;
    td_u64 uv_phy_buf;
    td_u32 y_buf_stride;
    td_u32 uv_buf_stride;
    td_u8 y_fac;
    td_u8 u_fac;
    td_u8 v_fac;
    td_u8 uv_order;
    td_u32 jpeg_color_space;
    td_u32 output_color_space;
    td_u32 scale;
    td_u32 image_mcu_width;
    td_u32 image_mcu_height;
    td_u32 y_mcu_height;
    td_u32 uv_mcu_height;
    td_u8 quant_ytable[DRV_JPEG_MAX_QTB_SIZE];
    td_u8 quant_cbtable[DRV_JPEG_MAX_QTB_SIZE];
    td_u8 quant_crtable[DRV_JPEG_MAX_QTB_SIZE];
    td_u16 dht_dc_lu_table[DRV_JPEG_MAX_HTB_DC_SIZE];
    td_u16 dht_dc_ch_table[DRV_JPEG_MAX_HTB_DC_SIZE];
    td_u8 dht_ac_lu_mincode_even[DRV_JPEG_MAX_HTB_AC_MIN_SIZE];
    td_u8 dht_ac_lu_mincode_odd[DRV_JPEG_MAX_HTB_AC_MIN_SIZE];
    td_u8 dht_ac_ch_mincode_even[DRV_JPEG_MAX_HTB_AC_MIN_SIZE];
    td_u8 dht_ac_ch_mincode_odd[DRV_JPEG_MAX_HTB_AC_MIN_SIZE];
    td_u8 dht_ac_lu_base_even[DRV_JPEG_MAX_HTB_AC_BASE_SIZE];
    td_u8 dht_ac_lu_base_odd[DRV_JPEG_MAX_HTB_AC_BASE_SIZE];
    td_u8 dht_ac_ch_base_even[DRV_JPEG_MAX_HTB_AC_BASE_SIZE];
    td_u8 dht_ac_ch_base_odd[DRV_JPEG_MAX_HTB_AC_BASE_SIZE];
    td_u8 dht_ac_lu_symblo[DRV_JPEG_MAX_HTB_AC_SYMBOL_SIZE];
    td_u8 dht_ac_ch_symblo[DRV_JPEG_MAX_HTB_AC_SYMBOL_SIZE];
} drv_jpeg_mgr;

/*-------------------------------- func declares ------------------------------------------*/

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* DRV_JPEG_STRUCT_H */

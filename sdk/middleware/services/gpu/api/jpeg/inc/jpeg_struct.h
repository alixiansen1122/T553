/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: jpeg struct interface
 */

#ifndef API_GRAPHICS_JPEG_STRUCT_H
#define API_GRAPHICS_JPEG_STRUCT_H

#include <stdio.h>
#include "td_base.h"
#include "drv_jpeg_struct.h"
#ifdef NO_UART_DEBUG
#include "soc_diag_util.h"
#include "log_oam_logger.h"
#include "log_def.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*-------------------------------- macro define -------------------------------------------*/

#define JPEG_MARK_TAG     0xFF
#define JPEG_MARK_SOI     0xD8
#define JPEG_MARK_EOI     0xD9

#define JPEG_1BYTE        1
#define JPEG_2BYTES       2
#define JPEG_4BYTES       4
#define JPEG_8BYTES       8
#define JPEG_SHIFT_1BIT   1
#define JPEG_SHIFT_2BITS  2
#define JPEG_SHIFT_4BITS  4
#define JPEG_SHIFT_8BITS  8
#define JPEG_SHIFT_16BITS 16
#define JPEG_SHIFT_24BITS 24

#define JPEG_MAX_COMPONENTS  4

/* huff */
#define JPEG_HUFF_LENGTH      16
#define JPEG_HUFF_TABLES      4
#define JPEG_HUFF_DC_TABLES   2
#define JPEG_HUFF_BITS_SIZE   17
#define JPEG_HUFF_CODE_SIZE   256
/* quant */
#define JPEG_DCT_LENGTH       16
#define JPEG_DCT_SIZE         8
#define JPEG_DCT_SIZE2        64
#define JPEG_QUANT_TABLES     4

#ifdef NO_UART_DEBUG
#define jpeg_err(fmt, args...) uapi_diag_error_log(0, fmt, ##args)
#else
#define jpeg_err(fmt, args...)  printf("[JPEG_ERROR][%s %d]"#fmt"\n", __func__, __LINE__, ##args)
#endif
/*-------------------------------- struct define ------------------------------------------*/

typedef enum {
    MY_JPEG_SCALE_1 = 1,
    MY_JPEG_SCALE_2 = 2,
    MY_JPEG_SCALE_4 = 4,
    MY_JPEG_SCALE_8 = 8,
    MY_JPEG_SCALE_MAX
} my_jpeg_scale_multiple;

typedef enum {
    MY_JPEG_COLOR_SPACE_YUV400 = 0,
    MY_JPEG_COLOR_SPACE_YUV420,
    MY_JPEG_COLOR_SPACE_YUV422_21,
    MY_JPEG_COLOR_SPACE_YUV422_12,
    MY_JPEG_COLOR_SPACE_YUV444,
    MY_JPEG_COLOR_SPACE_ARGB8888,
    MY_JPEG_COLOR_SPACE_RGB888,
    MY_JPEG_COLOR_SPACE_MAX
} my_jpeg_color_space;

typedef struct {
    td_u8 bits[JPEG_HUFF_BITS_SIZE];
    td_u8 huff_value[JPEG_HUFF_CODE_SIZE];
    td_bool is_has_huff_table;
} my_jpeg_huff_table;

typedef struct {
    td_bool is_has_quant_table;
    td_u16 quant_value[JPEG_DCT_SIZE2];
} my_jpeg_quant_table;

typedef struct {
    td_u8 h_samp_factor;
    td_u8 v_samp_factor;
    td_s32 component_id;
    td_s32 component_index;
    td_s32 quant_table_no;
    td_s32 dc_table_no;
    td_s32 ac_table_no;
} my_jpeg_component_info;

typedef struct {
    td_u16 year;
    td_u8 month;
    td_u8 day;
    td_u8 hour;
    td_u8 minute;
    td_u8 second;
} my_jpeg_timeval;

typedef struct {
    td_char *buffer;
    td_u32 phy_buf;
    td_u32 size;
    td_char *base_vir_buf;
    td_u32 base_phy_buf;
} my_jpeg_exif;

typedef struct {
    FILE *infile;
    td_char *vir_buf;
    td_u32 phy_buf;
    td_u32 size;
    td_bool is_mem;
    td_u32 cur_offset;
    td_u32 size_in_buffer;
} my_jpeg_src;

typedef struct {
    td_u32 width[JPEG_MAX_COMPONENTS];
    td_u32 height[JPEG_MAX_COMPONENTS];
    td_u32 stride[JPEG_MAX_COMPONENTS];
    td_u32 size[JPEG_MAX_COMPONENTS];
} my_jpeg_output;

typedef struct jpeg_decompress_par {
    td_bool is_stop;
    td_bool thumbnail_en;
    td_bool has_sof;
    td_bool is_progressive;
    td_bool arith_code;
    td_bool is_hdr;
    td_bool out_uv_order;
    td_bool output_yuv420sp;
    td_u8 num_components;
    td_u8 components_in_scan;
    td_s32 dev_fd;
    td_u32 image_width;
    td_u32 image_height;
    td_u32 output_width;
    td_u32 output_height;
    td_u32 output_stride;
    my_jpeg_scale_multiple scale;
    my_jpeg_color_space image_color_space;
    my_jpeg_color_space output_color_space;
    my_jpeg_src src;
    my_jpeg_exif exif;
    my_jpeg_timeval time;
    my_jpeg_output output;
    my_jpeg_component_info component_info[JPEG_MAX_COMPONENTS];
    my_jpeg_huff_table dc_huff_table[JPEG_HUFF_TABLES];
    my_jpeg_huff_table ac_huff_table[JPEG_HUFF_TABLES];
    my_jpeg_quant_table quant_table[JPEG_QUANT_TABLES];
    drv_jpeg_mgr reg_mgr;
    drv_jpeg_intstatus status;
} *jpeg_decompress_par_ptr;

/*-------------------------------- func declares ------------------------------------------*/

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* API_GRAPHICS_JPEG_STRUCT_H */

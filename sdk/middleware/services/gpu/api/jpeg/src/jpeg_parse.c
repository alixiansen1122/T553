/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: jpeg parse
 */

#include "jpeg_parse.h"
#include "securec.h"
#include "jpeg_stream.h"
#include "jpeg_table.h"
#include "jpeg_exif.h"

/*-------------------------------------- macro define -------------------------------------*/
#define MARK_TAG   (JPEG_MARK_TAG)
#define MARK_SOI   (JPEG_MARK_SOI)
#define MARK_EOI   (JPEG_MARK_EOI)
/* tem mark */
#define MARK_TEM   0x01
/* sofn mark */
#define MARK_SOF0  0xc0
#define MARK_SOF1  0xc1
#define MARK_SOF2  0xc2
#define MARK_SOF3  0xc3
#define MARK_SOF5  0xc5
#define MARK_SOF6  0xc6
#define MARK_SOF7  0xc7
#define MARK_JPG   0xc8
#define MARK_SOF9  0xc9
#define MARK_SOF10 0xca
#define MARK_SOF11 0xcb
#define MARK_SOF13 0xcd
#define MARK_SOF14 0xce
#define MARK_SOF15 0xcf
/* dht mark */
#define MARK_DHT   0xc4
/* dac mark */
#define MARK_DAC   0xcc
/* rst mark */
#define MARK_RST0  0xd0
#define MARK_RST1  0xd1
#define MARK_RST2  0xd2
#define MARK_RST3  0xd3
#define MARK_RST4  0xd4
#define MARK_RST5  0xd5
#define MARK_RST6  0xd6
#define MARK_RST7  0xd7
/* compress data */
#define MARK_SOS   0xda
/* dqt mark */
#define MARK_DQT   0xdb
/* dnl mark */
#define MARK_DNL   0xdc
/* dri mark */
#define MARK_DRI   0xdd
/* appn mark */
#define MARK_APP0  0xe0
#define MARK_APP1  0xe1
#define MARK_APP2  0xe2
#define MARK_APP3  0xe3
#define MARK_APP4  0xe4
#define MARK_APP5  0xe5
#define MARK_APP6  0xe6
#define MARK_APP7  0xe7
#define MARK_APP8  0xe8
#define MARK_APP9  0xe9
#define MARK_APP10 0xea
#define MARK_APP11 0xeb
#define MARK_APP12 0xec
#define MARK_APP13 0xed
#define MARK_APP14 0xee
#define MARK_APP15 0xef
/* com mark */
#define MARK_COM   0xfe

#define JPEG_DATA_PRECISION  8
#define JPEG_YUV_COMPONENTS  3
#define JPEG_MAX_SAMP_FACTOR 4
#define JPEG_MIN_WIDTH       10
#define JPEG_MIN_HEIGHT      10
#define JPEG_MAX_WIDTH       8096
#define JPEG_MAX_HEIGHT      8096

static td_s32 jpeg_max(td_s32 a, td_s32 b)
{
    return (a < b) ? b : a;
}

/*-------------------------------------- struct info --------------------------------------*/
typedef enum {
    JPEG_PARSE_NOT_JPEG = 0,
    JPEG_PARSE_SOFN,
    JPEG_PARSE_SOS,
    JPEG_PARSE_DQT,
    JPEG_PARSE_DHT,
    JPEG_PARSE_DRI,
    JPEG_PARSE_CONTINUE,
    JPEG_PARSE_ERROR,
    JPEG_PARSE_FINISH,
    JPEG_PARSE_MAX
} jpeg_parse_state;

/*-------------------------------------- func define --------------------------------------*/
/* parse mark */
static td_s32 jpeg_parse_fist_marker(jpeg_decompress_par_ptr par);
static td_s32 jpeg_parse_next_marker(jpeg_decompress_par_ptr par);
static td_s32 jpeg_parse_every_mark(jpeg_decompress_par_ptr par, td_s32 mark);
/* check image support */
static td_s32 jpeg_parse_sofn_support(jpeg_decompress_par_ptr par, td_s32 mark);
static td_s32 jpeg_parse_appn_support(jpeg_decompress_par_ptr par, td_s32 mark);
static td_s32 jpeg_parse_rst_support(td_s32 mark);
static td_s32 jpeg_parse_dax_support(td_s32 mark);
/* parse image information */
static td_s32 jpeg_parse_header(jpeg_decompress_par_ptr par, td_s32 mark);
/* image sofn information */
static td_s32 jpeg_parse_sofn(jpeg_decompress_par_ptr par);
static td_s32 jpeg_parse_component_info(jpeg_decompress_par_ptr par);
static td_s32 jpeg_parse_sample_fac(jpeg_decompress_par_ptr par);
static td_s32 jpeg_parse_fmt(jpeg_decompress_par_ptr par);
/* image sos information */
static td_s32 jpeg_parse_sos(jpeg_decompress_par_ptr par);
/* image dht information */
static td_s32 jpeg_parse_dht(jpeg_decompress_par_ptr par);
/* image dqt information */
static td_s32 jpeg_parse_dqt(jpeg_decompress_par_ptr par);
/* image dri information */
static td_s32 jpeg_parse_dri(jpeg_decompress_par_ptr par);
/* image com information */
static td_s32 jpeg_parse_com(jpeg_decompress_par_ptr par);
/* parse decompress information */
static td_s32  jpeg_parse_decompress_info(jpeg_decompress_par_ptr par);
static td_s32  jpeg_parse_calc_scale(jpeg_decompress_par_ptr par);
static td_s32  jpeg_parse_calc_input_color_space(jpeg_decompress_par_ptr par);
static td_void jpeg_parse_calc_output_color_space(jpeg_decompress_par_ptr par);
static td_void jpeg_parse_calc_output_resolution(jpeg_decompress_par_ptr par);
static td_void jpeg_parse_calc_output_wh(jpeg_decompress_par_ptr par);
static td_void jpeg_parse_calc_output_mcuwh(jpeg_decompress_par_ptr par);
static td_void jpeg_parse_calc_output_yuvwh(jpeg_decompress_par_ptr par);
static td_void jpeg_parse_calc_output_stride(jpeg_decompress_par_ptr par);

/*-------------------------------------- func release -------------------------------------*/
td_s32 jpeg_parse_start(jpeg_decompress_par_ptr par)
{
    td_s32 ret;
    td_s32 mark;

    ret = jpeg_parse_fist_marker(par);
    if (ret != EXT_SUCCESS) {
        jpeg_err("call jpeg_parse_fist_marker failure");
        return EXT_FAILURE;
    }

    do {
        mark = jpeg_parse_next_marker(par);
        if (mark == (td_s32)EXT_FAILURE) {
            jpeg_err("call jpeg_parse_next_marker failure");
            return EXT_FAILURE;
        }

        ret = jpeg_parse_every_mark(par, mark);
        if (ret == JPEG_PARSE_FINISH) {
            break;
        }

        if (ret == JPEG_PARSE_ERROR) {
            jpeg_err("parse failure");
            return EXT_FAILURE;
        }
    } while (TD_TRUE);

    ret = jpeg_parse_decompress_info(par);
    if (ret != EXT_SUCCESS) {
        jpeg_err("call jpeg_parse_decompress_info failure");
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

static td_s32 jpeg_parse_fist_marker(jpeg_decompress_par_ptr par)
{
    td_s32 code, code1;

    code = jpeg_stream_read_one_byte(par);
    if (code < 0) {
        jpeg_err("call jpeg_stream_read_one_byte failure");
        return EXT_FAILURE;
    }

    code1 = jpeg_stream_read_one_byte(par);
    if (code1 < 0) {
        jpeg_err("call jpeg_stream_read_one_byte failure");
        return EXT_FAILURE;
    }

    if ((code == MARK_TAG) && (code1 == MARK_SOI)) {
        return EXT_SUCCESS;
    }

    return EXT_FAILURE;
}

static td_s32 jpeg_parse_next_marker(jpeg_decompress_par_ptr par)
{
    td_s32 mark;

    do {
        mark = jpeg_stream_read_one_byte(par);
    } while ((mark != MARK_TAG) && (mark >= 0));

    do {
        mark = jpeg_stream_read_one_byte(par);
    } while (mark == MARK_TAG);

    return mark;
}

static td_s32 jpeg_parse_every_mark(jpeg_decompress_par_ptr par, td_s32 mark)
{
    td_s32 ret;

    ret = jpeg_parse_sofn_support(par, mark);
    if (ret != EXT_SUCCESS) {
        jpeg_err("call jpeg_parse_sofn_support failure");
        return JPEG_PARSE_ERROR;
    }

    ret = jpeg_parse_appn_support(par, mark);
    if (ret != EXT_SUCCESS) {
        jpeg_err("call jpeg_parse_appn_support failure");
        return JPEG_PARSE_ERROR;
    }

    ret = jpeg_parse_rst_support(mark);
    if (ret != EXT_SUCCESS) {
        jpeg_err("call jpeg_parse_rst_support failure");
        return JPEG_PARSE_ERROR;
    }

    ret = jpeg_parse_dax_support(mark);
    if (ret != EXT_SUCCESS) {
        jpeg_err("call jpeg_parse_dax_support failure");
        return JPEG_PARSE_ERROR;
    }

    return jpeg_parse_header(par, mark);
}

static td_s32 jpeg_parse_sofn_support(jpeg_decompress_par_ptr par, td_s32 mark)
{
    td_s32 len;

    par->arith_code = ((mark == MARK_SOF9) || (mark == MARK_SOF10)) ? (TD_TRUE) : (TD_FALSE);
    par->is_progressive = ((mark == MARK_SOF2) || (mark == MARK_SOF10)) ? (TD_TRUE) : (TD_FALSE);
    if ((par->arith_code == TD_TRUE) || (par->is_progressive == TD_TRUE)) {
        len = jpeg_stream_read_two_byte(par);
        len = (len >= JPEG_2BYTES) ? (len - JPEG_2BYTES) : 0;
        jpeg_stream_skip_bytes(par, (td_u32)len);
        jpeg_err("is progressive");
        return EXT_FAILURE;
    }

    if ((mark == MARK_SOF3)  || (mark == MARK_SOF5)  ||
        (mark == MARK_SOF6)  || (mark == MARK_SOF7)  ||
        (mark == MARK_JPG)   || (mark == MARK_SOF11) ||
        (mark == MARK_SOF13) || (mark == MARK_SOF14) ||
        (mark == MARK_SOF15)) {
        len = jpeg_stream_read_two_byte(par);
        len = (len >= JPEG_2BYTES) ? (len - JPEG_2BYTES) : 0;
        jpeg_stream_skip_bytes(par, (td_u32)len);
        jpeg_err("sof not support");
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

static td_s32 jpeg_parse_appn_support(jpeg_decompress_par_ptr par, td_s32 mark)
{
    td_s32 len;
    if ((mark == MARK_APP0)  ||
        (mark == MARK_APP2)  || (mark == MARK_APP3)  ||
        (mark == MARK_APP4)  || (mark == MARK_APP5)  ||
        (mark == MARK_APP6)  || (mark == MARK_APP7)  ||
        (mark == MARK_APP8)  || (mark == MARK_APP9)  ||
        (mark == MARK_APP10) || (mark == MARK_APP11) ||
        (mark == MARK_APP12) || (mark == MARK_APP13) ||
        (mark == MARK_APP14) || (mark == MARK_APP15)) {
        len = jpeg_stream_read_two_byte(par);
        len = (len >= JPEG_2BYTES) ? (len - JPEG_2BYTES) : 0;
        jpeg_stream_skip_bytes(par, (td_u32)len);
    }

    return EXT_SUCCESS;
}

static td_s32 jpeg_parse_rst_support(td_s32 mark)
{
    if ((mark == MARK_RST0) || (mark == MARK_RST1) ||
        (mark == MARK_RST2) || (mark == MARK_RST3) ||
        (mark == MARK_RST4) || (mark == MARK_RST5) ||
        (mark == MARK_RST6) || (mark == MARK_RST7)) {
        jpeg_err("rst not support");
        return EXT_FAILURE;
    }
    return EXT_SUCCESS;
}

static td_s32 jpeg_parse_dax_support(td_s32 mark)
{
    if ((mark == MARK_DAC) || (mark == MARK_TEM) || (mark == MARK_DNL)) {
        jpeg_err("dax not support");
        return EXT_FAILURE;
    }
    return EXT_SUCCESS;
}

static td_s32 jpeg_parse_header(jpeg_decompress_par_ptr par, td_s32 mark)
{
    switch (mark) {
        case MARK_SOF0:
        case MARK_SOF1:
            if (jpeg_parse_sofn(par) != EXT_SUCCESS) {
                jpeg_err("call jpeg_parse_sofn failure");
                return JPEG_PARSE_ERROR;
            }
            break;
        case MARK_APP1:
            jpeg_exif_parse(par);
            break;
        case MARK_SOS:
            if (jpeg_parse_sos(par) != EXT_SUCCESS) {
                jpeg_err("call jpeg_parse_sos failure");
                return JPEG_PARSE_ERROR;
            }
            return JPEG_PARSE_FINISH;
        case MARK_DHT:
            if (jpeg_parse_dht(par) != EXT_SUCCESS) {
                jpeg_err("call jpeg_parse_dht failure");
                return JPEG_PARSE_ERROR;
            }
            break;
        case MARK_DQT:
            if (jpeg_parse_dqt(par) != EXT_SUCCESS) {
                jpeg_err("call jpeg_parse_dqt failure");
                return JPEG_PARSE_ERROR;
            }
            break;
        case MARK_DRI:
            if (jpeg_parse_dri(par) != EXT_SUCCESS) {
                jpeg_err("call jpeg_parse_dri failure");
                return JPEG_PARSE_ERROR;
            }
            break;
        case MARK_COM:
            if (jpeg_parse_com(par) != EXT_SUCCESS) {
                jpeg_err("call jpeg_parse_com failure");
                return JPEG_PARSE_ERROR;
            }
            break;
        case MARK_EOI:
            return JPEG_PARSE_FINISH;
        default:
            return JPEG_PARSE_CONTINUE;
    }

    return JPEG_PARSE_CONTINUE;
}

static td_s32 jpeg_parse_sofn(jpeg_decompress_par_ptr par)
{
    td_s32 data_precision, len;

    len = jpeg_stream_read_two_byte(par);
    data_precision = jpeg_stream_read_one_byte(par);
    if (data_precision != JPEG_DATA_PRECISION) {
        jpeg_err("data precision is not support");
        return EXT_FAILURE;
    }

    par->image_height   = (td_u32)jpeg_stream_read_two_byte(par);
    par->image_width    = (td_u32)jpeg_stream_read_two_byte(par);
    par->num_components = (td_u8)jpeg_stream_read_one_byte(par);

    len -= JPEG_8BYTES;
    if ((par->num_components * JPEG_YUV_COMPONENTS) != len) {
        jpeg_err("component not support");
        return EXT_FAILURE;
    }

    if ((par->num_components > JPEG_MAX_COMPONENTS) ||
        (par->image_width < JPEG_MIN_WIDTH) || (par->image_height < JPEG_MIN_HEIGHT) ||
        (par->image_width > JPEG_MAX_WIDTH) || (par->image_height > JPEG_MAX_HEIGHT)) {
        jpeg_err("size is not support");
        return EXT_FAILURE;
    }

    if (jpeg_parse_component_info(par) != EXT_SUCCESS) {
        jpeg_err("call jpeg_parse_component_info failure");
        return EXT_FAILURE;
    }

    if (jpeg_parse_sample_fac(par) != EXT_SUCCESS) {
        jpeg_err("call jpeg_parse_sample_fac failure");
        return EXT_FAILURE;
    }

    if (jpeg_parse_fmt(par) != EXT_SUCCESS) {
        jpeg_err("call jpeg_parse_fmt failure");
        return EXT_FAILURE;
    }

    par->has_sof = TD_TRUE;

    return EXT_SUCCESS;
}

static td_s32 jpeg_parse_component_info(jpeg_decompress_par_ptr par)
{
    td_s32 i;
    td_u32 code;
    td_s32 max_h_samp_factor = 1;
    td_s32 max_v_samp_factor = 1;

    for (i = 0; i < par->num_components; i++) {
        par->component_info[i].component_index = i;
        par->component_info[i].component_id = jpeg_stream_read_one_byte(par);

        code = (td_u32)jpeg_stream_read_one_byte(par);
        par->component_info[i].h_samp_factor = (code >> JPEG_SHIFT_4BITS) & 0xf;
        par->component_info[i].v_samp_factor = code & 0xf;

        par->component_info[i].quant_table_no = jpeg_stream_read_one_byte(par);

        if ((par->component_info[i].h_samp_factor <= 0) ||
            (par->component_info[i].h_samp_factor > JPEG_MAX_SAMP_FACTOR) ||
            (par->component_info[i].v_samp_factor <= 0) ||
            (par->component_info[i].v_samp_factor > JPEG_MAX_SAMP_FACTOR)) {
            jpeg_err("sample factor not support");
            return EXT_FAILURE;
        }

        max_h_samp_factor = jpeg_max(max_h_samp_factor, par->component_info[i].h_samp_factor);
        max_v_samp_factor = jpeg_max(max_v_samp_factor, par->component_info[i].v_samp_factor);
    }

    return EXT_SUCCESS;
}

static td_s32 jpeg_parse_sample_fac(jpeg_decompress_par_ptr par)
{
    td_u8 component_y_h = par->component_info[0].h_samp_factor;
    td_u8 component_y_v = par->component_info[0].v_samp_factor;
    td_u8 component_cb_h = par->component_info[1].h_samp_factor;
    td_u8 component_cb_v = par->component_info[1].v_samp_factor;
    td_u8 component_cr_h = par->component_info[2].h_samp_factor;
    td_u8 component_cr_v = par->component_info[2].v_samp_factor;

    if (par->num_components == 1) {
        if (component_y_h == component_y_v) {
            par->reg_mgr.y_fac = (td_u8)(((1 << 4) | 1) & 0xff); /* shift 4 for h, no shift for v */
        }
        return EXT_SUCCESS;
    }

    if ((par->num_components == JPEG_YUV_COMPONENTS) &&
        (component_cb_h == component_cr_h) &&
        (component_cb_v == component_cr_v)) {
        par->reg_mgr.y_fac = ((component_y_h  << 4) | component_y_v)  & 0xff; /* index: 4 */
        par->reg_mgr.u_fac = ((component_cb_h << 4) | component_cb_v) & 0xff; /* index: 4 */
        par->reg_mgr.v_fac = ((component_cr_h << 4) | component_cr_v) & 0xff; /* index: 4 */
        return EXT_SUCCESS;
    }

    return EXT_FAILURE;
}

static td_s32 jpeg_parse_fmt(jpeg_decompress_par_ptr par)
{
    td_s32 component_cb_h = par->component_info[1].h_samp_factor;
    td_s32 component_cb_v = par->component_info[1].v_samp_factor;
    td_s32 component_cr_h = par->component_info[2].h_samp_factor;
    td_s32 component_cr_v = par->component_info[2].v_samp_factor;

    if (par->num_components == 1) {
        if (par->component_info[0].h_samp_factor == par->component_info[0].v_samp_factor) {
            par->image_color_space = MY_JPEG_COLOR_SPACE_YUV400;
            return EXT_SUCCESS;
        }
    }

    if ((par->num_components != JPEG_YUV_COMPONENTS) || (component_cb_h != component_cr_h) ||
        (component_cb_v != component_cr_v)) {
        par->image_color_space = MY_JPEG_COLOR_SPACE_MAX;
        jpeg_err("color space is not support");
        return EXT_FAILURE;
    }

    if (par->component_info[0].h_samp_factor == ((par->component_info[1].h_samp_factor) << 1)) {
        if (par->component_info[0].v_samp_factor == ((par->component_info[1].v_samp_factor) << 1)) {
            par->image_color_space = MY_JPEG_COLOR_SPACE_YUV420;
            return EXT_SUCCESS;
        } else if (par->component_info[0].v_samp_factor == par->component_info[1].v_samp_factor) {
            par->image_color_space = MY_JPEG_COLOR_SPACE_YUV422_21;
            return EXT_SUCCESS;
        }
    } else if (par->component_info[0].h_samp_factor == par->component_info[1].h_samp_factor) {
        if (par->component_info[0].v_samp_factor == ((par->component_info[1].v_samp_factor) << 1)) {
            par->image_color_space = MY_JPEG_COLOR_SPACE_YUV422_12;
            return EXT_SUCCESS;
        } else if (par->component_info[0].v_samp_factor == par->component_info[1].v_samp_factor) {
            par->image_color_space = MY_JPEG_COLOR_SPACE_YUV444;
            return EXT_SUCCESS;
        }
    }

    par->image_color_space = MY_JPEG_COLOR_SPACE_MAX;

    jpeg_err("color space is not support");
    return EXT_FAILURE;
}

static td_s32 jpeg_parse_sos(jpeg_decompress_par_ptr par)
{
    td_s32 len, num_component, i, j, code, code1;
    td_u32 ss, se, ah, al;

    if (par->has_sof != TD_TRUE) {
        return EXT_SUCCESS;
    }

    len = jpeg_stream_read_two_byte(par);
    num_component = jpeg_stream_read_one_byte(par);
    if ((len != (num_component * 2 + 6)) || (num_component < 1) || /* 2,6:index */
        (num_component > JPEG_MAX_COMPONENTS) || (par->num_components > JPEG_MAX_COMPONENTS)) {
        jpeg_err("sos not support");
        return EXT_FAILURE;
    }

    par->components_in_scan = (td_u8)num_component;

    for (i = 0; i < num_component; i++) {
        code1 = jpeg_stream_read_one_byte(par);
        code = jpeg_stream_read_one_byte(par);
        for (j = 0; j < par->num_components; j++) {
            if (code1 == par->component_info[j].component_id) {
                goto id_found;
            }
        }
        return EXT_FAILURE;
        id_found:
            par->component_info[j].dc_table_no = ((td_u32)code >> 4) & 15; /* 4 shif val, 15 index */
            par->component_info[j].ac_table_no = (td_u32)code & 15;        /* 15 index */
    }

    ss = (td_u32)jpeg_stream_read_one_byte(par);
    se = (td_u32)jpeg_stream_read_one_byte(par);
    code = jpeg_stream_read_one_byte(par);
    ah = ((td_u32)code >> 4) & 15; /* 4 shif val, 15 index */
    al = (td_u32)code & 15;        /* 15 index */

    code = (td_s32)(ss | se | ah | al); /* there is no point */

    return EXT_SUCCESS;
}

static td_s32 jpeg_parse_dht(jpeg_decompress_par_ptr par)
{
    td_s32 err_ret, len, i, count, hor_table_index;
    td_u8 bits[JPEG_HUFF_BITS_SIZE] = {0};
    td_u8 huffval[JPEG_HUFF_CODE_SIZE] = {0};
    my_jpeg_huff_table *htblptr = TD_NULL;

    len = jpeg_stream_read_two_byte(par);
    len -= 2; /* 2: index */

    while (len > JPEG_HUFF_LENGTH) {
        hor_table_index = jpeg_stream_read_one_byte(par);
        count = bits[0] = 0;
        for (i = 1; i <= JPEG_HUFF_LENGTH; i++) {
            bits[i] = (td_u8)jpeg_stream_read_one_byte(par);
            count += bits[i];
        }
        len -= 1 + JPEG_HUFF_LENGTH;

        if ((count > JPEG_HUFF_CODE_SIZE) || (count > len) || (count == 0)) {
            return EXT_FAILURE;
        }

        for (i = 0; i < count; i++) {
            huffval[i] = (td_u8)jpeg_stream_read_one_byte(par);
        }
        len -= count;

        if (((td_u32)hor_table_index & 0x10) != 0) {
            hor_table_index -= 0x10;
            if ((hor_table_index < 0) || (hor_table_index >= JPEG_HUFF_TABLES)) {
                return EXT_FAILURE;
            }
            htblptr = &par->ac_huff_table[hor_table_index];
            par->ac_huff_table[hor_table_index].is_has_huff_table = TD_TRUE;
        } else {
            if ((hor_table_index < 0) || (hor_table_index >= JPEG_HUFF_TABLES)) {
                return EXT_FAILURE;
            }
            htblptr = &par->dc_huff_table[hor_table_index];
            par->dc_huff_table[hor_table_index].is_has_huff_table = TD_TRUE;
        }
        err_ret = memcpy_s(htblptr->bits, sizeof(htblptr->bits), bits, sizeof(bits));
        if (err_ret != EOK) {
            return EXT_FAILURE;
        }
        err_ret = memcpy_s(htblptr->huff_value, sizeof(htblptr->huff_value), huffval, sizeof(huffval));
        if (err_ret != EOK) {
            return EXT_FAILURE;
        }
    }
    return EXT_SUCCESS;
}

static td_s32 jpeg_parse_dqt(jpeg_decompress_par_ptr par)
{
    td_u32 i, len, code, precision;
    my_jpeg_quant_table *quantptr = TD_NULL;
    td_s32 zorder[JPEG_DCT_SIZE2 + JPEG_DCT_LENGTH] = {
        0,  1,  8,  16, 9,  2,  3,  10, 17, 24, 32, 25, 18, 11, 4,  5,  12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13,
        6,  7,  14, 21, 28, 35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51, 58, 59, 52, 45, 38, 31,
        39, 46, 53, 60, 61, 54, 47, 55, 62, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63
    };

    len = (td_u32)jpeg_stream_read_two_byte(par);
    len -= 2; /* 2: index */

    while (len > 0) {
        code = (td_u32)jpeg_stream_read_one_byte(par);
        precision = code >> 4; /* 4: shift val */
        code &= 0x0F;
        if (code >= JPEG_QUANT_TABLES) {
            return EXT_FAILURE;
        }

        par->quant_table[code].is_has_quant_table = TD_TRUE;
        quantptr = &par->quant_table[code];

        for (i = 0; i < JPEG_DCT_SIZE2; i++) {
            if (precision != 0) {
                code = (td_u32)jpeg_stream_read_two_byte(par);
            } else {
                code = (td_u32)jpeg_stream_read_one_byte(par);
            }
            quantptr->quant_value[zorder[i]] = (td_u16)code;
        }

        len -= JPEG_DCT_SIZE2 + 1;
        if (precision != 0) {
            len -= JPEG_DCT_SIZE2;
        }
    }

    if (len != 0) {
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

static td_s32 jpeg_parse_dri(jpeg_decompress_par_ptr par)
{
    td_u32 len, code;

    len = (td_u32)jpeg_stream_read_two_byte(par);
    if (len != 4) { /* 4: length */
        return EXT_FAILURE;
    }

    code = (td_u32)jpeg_stream_read_two_byte(par);
    par->reg_mgr.restart_interval = code;

    return EXT_SUCCESS;
}

static td_s32 jpeg_parse_com(jpeg_decompress_par_ptr par)
{
    td_u32 len;
    len = (td_u32)jpeg_stream_read_two_byte(par);
    len = (len >= JPEG_2BYTES) ? (len - JPEG_2BYTES) : 0;
    jpeg_stream_skip_bytes(par, len);
    return EXT_SUCCESS;
}

static td_s32 jpeg_parse_decompress_info(jpeg_decompress_par_ptr par)
{
    td_s32 ret;

    ret = jpeg_table_decompress(par);
    if (ret != EXT_SUCCESS) {
        jpeg_err("call jpeg_table_decompress failure");
        return EXT_FAILURE;
    }

    ret = jpeg_parse_calc_scale(par);
    if (ret != EXT_SUCCESS) {
        jpeg_err("call jpeg_parse_calc_scale failure");
        return EXT_FAILURE;
    }

    ret = jpeg_parse_calc_input_color_space(par);
    if (ret != EXT_SUCCESS) {
        jpeg_err("call jpeg_parse_calc_input_color_space failure");
        return EXT_FAILURE;
    }

    jpeg_parse_calc_output_color_space(par);

    jpeg_parse_calc_output_resolution(par);

    return EXT_SUCCESS;
}

static td_s32 jpeg_parse_calc_scale(jpeg_decompress_par_ptr par)
{
    if (par->scale == MY_JPEG_SCALE_1) {
        par->reg_mgr.scale = 0;
    } else if (par->scale == MY_JPEG_SCALE_2) {
        par->reg_mgr.scale = 1;
    } else if (par->scale == MY_JPEG_SCALE_4) {
        par->reg_mgr.scale = 2; /* 2: sacle val */
    } else if (par->scale == MY_JPEG_SCALE_8) {
        par->reg_mgr.scale = 3; /* 3: sacle val */
    } else {
        jpeg_err("scale is not support");
        return EXT_FAILURE;
    }
    return EXT_SUCCESS;
}

static td_s32 jpeg_parse_calc_input_color_space(jpeg_decompress_par_ptr par)
{
    if (par->image_color_space == MY_JPEG_COLOR_SPACE_YUV400) {
        par->reg_mgr.jpeg_color_space = 0; /* 0 for set reg yuv400 */
    } else if (par->image_color_space == MY_JPEG_COLOR_SPACE_YUV420) {
        par->reg_mgr.jpeg_color_space = 3; /* 3 for set reg yuv420 */
    } else if (par->image_color_space == MY_JPEG_COLOR_SPACE_YUV422_21) {
        par->reg_mgr.jpeg_color_space = 4; /* 4 for set reg yuv422_2 */
    } else if (par->image_color_space == MY_JPEG_COLOR_SPACE_YUV422_12) {
        par->reg_mgr.jpeg_color_space = 5; /* 5 for set reg yuv422_12 */
    } else if (par->image_color_space == MY_JPEG_COLOR_SPACE_YUV444) {
        par->reg_mgr.jpeg_color_space = 6; /* 6 for set reg yuv444 */
    } else {
        jpeg_err("jpeg color space is not support");
        return EXT_FAILURE;
    }
    return EXT_SUCCESS;
}

static td_void jpeg_parse_calc_output_color_space(jpeg_decompress_par_ptr par)
{
    if ((par->output_color_space != MY_JPEG_COLOR_SPACE_ARGB8888) &&
        (par->output_color_space != MY_JPEG_COLOR_SPACE_RGB888)) {
        par->output_color_space = par->image_color_space;
    }

    if ((par->output_yuv420sp == TD_TRUE) && (par->image_color_space != MY_JPEG_COLOR_SPACE_YUV400)) {
        par->output_color_space = MY_JPEG_COLOR_SPACE_YUV420;
        par->reg_mgr.output_color_space = 1;
    }

    par->reg_mgr.uv_order = (par->out_uv_order == TD_TRUE) ? (0) : (1);

    return;
}

static td_void jpeg_parse_calc_output_resolution(jpeg_decompress_par_ptr par)
{
    jpeg_parse_calc_output_wh(par);
    jpeg_parse_calc_output_stride(par);
    jpeg_parse_calc_output_mcuwh(par);
    jpeg_parse_calc_output_yuvwh(par);
    return;
}

static td_void jpeg_parse_calc_output_wh(jpeg_decompress_par_ptr par)
{
    if ((par->scale >= MY_JPEG_SCALE_1) && (par->scale <= MY_JPEG_SCALE_8)) {
        par->output_width  = (td_u32)((par->image_width  + (td_u32)par->scale - 1L) / (td_u32)par->scale);
        par->output_height = (td_u32)((par->image_height + (td_u32)par->scale - 1L) / (td_u32)par->scale);
    }
}

static td_void jpeg_parse_calc_output_mcuwh(jpeg_decompress_par_ptr par)
{
    td_u8 y_h_fac = (td_u8)((par->reg_mgr.y_fac >> 4) & 0xf); /* shift 4 is h */
    td_u8 y_v_fac = (td_u8)(par->reg_mgr.y_fac & 0xf); /* no shift is v */

    if (y_h_fac == 1) {
        par->reg_mgr.image_mcu_width = (par->image_width + 8 - 1) >> 3;  /* 8 align index, 3 shif val */
    } else {
        par->reg_mgr.image_mcu_width = (par->image_width + 16 - 1) >> 4; /* 16 align index, 4 shif val */
    }

    if (y_v_fac == 1) {
        par->reg_mgr.image_mcu_height = (par->image_height + 8 - 1) >> 3; /* 8 align index, 3 shif val */
        par->reg_mgr.y_mcu_height = (par->output_height + 8 - 1) & (~(8 - 1)); /* 8 align index */
    } else {
        par->reg_mgr.image_mcu_height = (par->image_height + 16 - 1) >> 4;  /* 16 align index, 4 shif val */
        par->reg_mgr.y_mcu_height = (par->output_height + 16 - 1) & (~(16 - 1)); /* 16 align index */
    }
}

static td_void jpeg_parse_calc_output_yuvwh(jpeg_decompress_par_ptr par)
{
    par->reg_mgr.uv_mcu_height = 0;
    par->output.width[0]  = par->output_width;
    par->output.height[0] = par->output_height;
    if (par->image_color_space == MY_JPEG_COLOR_SPACE_YUV400) {
        par->output.width[0]  = (par->output.width[0] >> 1) << 1;
        par->output.height[0] = (par->output.height[0] >> 1) << 1;
    } else if (par->image_color_space == MY_JPEG_COLOR_SPACE_YUV420) {
        par->output.width[0]  = (par->output.width[0] >> 1) << 1;
        par->output.height[0] = (par->output.height[0] >> 1) << 1;
        par->output.width[1]  = par->output.width[0] >> 1;
        par->output.height[1] = par->output.height[0] >> 1;
        par->reg_mgr.uv_mcu_height = par->reg_mgr.y_mcu_height >> 1;
    } else if (par->image_color_space == MY_JPEG_COLOR_SPACE_YUV422_21) {
        par->output.width[0]  = (par->output.width[0] >> 1) << 1;
        par->output.width[1]  = par->output.width[0] >> 1;
        par->output.height[1] = par->output.height[0];
        par->reg_mgr.uv_mcu_height = par->reg_mgr.y_mcu_height;
    } else if (par->image_color_space == MY_JPEG_COLOR_SPACE_YUV422_12) {
        par->output.height[0] = (par->output.height[0] >> 1) << 1;
        par->output.width[1]  = par->output.width[0];
        par->output.height[1] = par->output.height[0] >> 1;
        par->reg_mgr.uv_mcu_height = par->reg_mgr.y_mcu_height >> 1;
    } else if (par->image_color_space == MY_JPEG_COLOR_SPACE_YUV444) {
        par->output.width[1]  = par->output.width[0];
        par->output.height[1] = par->output.height[0];
        par->reg_mgr.uv_mcu_height = par->reg_mgr.y_mcu_height;
    } else {
        /* nothing to do */
        jpeg_err("output size is not support");
    }

    if ((par->output_yuv420sp == TD_TRUE) && (par->image_color_space != MY_JPEG_COLOR_SPACE_YUV400)) {
        par->output.width[0]  = par->output.width[0] - (par->output.width[0] % 2);   /* 2 is tolower even number */
        par->output.height[0] = par->output.height[0] - (par->output.height[0] % 2); /* 2 is tolower even number */
        par->output.width[1]  = par->output.width[0] >> 1;
        par->output.height[1] = par->output.height[0] >> 1;
        par->reg_mgr.uv_mcu_height = par->reg_mgr.y_mcu_height >> 1;
    }

    par->reg_mgr.y_mcu_height = par->output.height[0];
    par->output.size[0] = par->reg_mgr.y_mcu_height  * par->output.stride[0];
    par->output.size[1] = par->reg_mgr.uv_mcu_height * par->output.stride[1];
    return;
}

static td_void jpeg_parse_calc_output_stride(jpeg_decompress_par_ptr par)
{
    /* y stride 128 bytes align, for uv_addr need 128 algin */
    par->output.stride[0] = (par->output_width + 128 - 1) & (~(128 - 1));

    if (par->image_color_space == MY_JPEG_COLOR_SPACE_YUV400) {
        par->output.stride[1] = 0;
    } else if (par->image_color_space == MY_JPEG_COLOR_SPACE_YUV420) {
        par->output.stride[1] = par->output.stride[0];
    } else if (par->image_color_space == MY_JPEG_COLOR_SPACE_YUV422_21) {
        par->output.stride[1] = par->output.stride[0];
    } else if (par->image_color_space == MY_JPEG_COLOR_SPACE_YUV422_12) {
        par->output.stride[1] = (par->output_yuv420sp == TD_TRUE) ?
                                ((par->output.stride[0] << 1) >> 1) : (par->output.stride[0] << 1);
    } else if (par->image_color_space == MY_JPEG_COLOR_SPACE_YUV444) {
        par->output.stride[1] = (par->output_yuv420sp == TD_TRUE) ?
                                ((par->output.stride[0] << 1) >> 1) : (par->output.stride[0] << 1);
    } else {
        /* nothing to do */
    }

    if (par->output_color_space == MY_JPEG_COLOR_SPACE_RGB888) {
        par->output_stride = (par->output_width * 3 + 16 - 1) & (~(16 - 1)); /* 2 xrgb need 3 and 16 bytes align */
    } else if (par->output_color_space == MY_JPEG_COLOR_SPACE_ARGB8888) {
        par->output_stride = (par->output_width * 4 + 16 - 1) & (~(16 - 1)); /* 2 xrgb need 4 and 16 bytes align */
    } else {
        /* nothing to do */
    }
}

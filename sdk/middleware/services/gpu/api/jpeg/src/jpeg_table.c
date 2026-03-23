/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: jpeg table
 */

#include "jpeg_table.h"
#include "securec.h"

/*-------------------------------------- macro define -------------------------------------*/
#define LU          0
#define CH          1
#define MAX_TABLE   2

static td_void jpeg_swap(td_u32 *a, td_u32 *b)
{
    td_u32 swap_temp = *a;
    *a = *b;
    *b = swap_temp;
}


/*-------------------------------------- struct info --------------------------------------*/

/*-------------------------------------- global data --------------------------------------*/

static const td_u16 g_protocol_lu_quant_table[JPEG_DCT_SIZE2] = {
    16, 11,  10,  16, 24, 40, 51, 61, 12,  12,  14,  19,  26, 58, 60, 55,  14,  13,  16,  24, 40, 57,
    69, 56,  14,  17, 22, 29, 51, 87, 80,  62,  18,  22,  37, 56, 68, 109, 103, 77,  24,  35, 55, 64,
    81, 104, 113, 92, 49, 64, 78, 87, 103, 121, 120, 101, 72, 92, 95, 98,  112, 100, 103, 99
};

static const td_u16 g_protocol_ch_quant_table[JPEG_DCT_SIZE2] = {
    17, 18, 24, 47, 99, 99, 99, 99, 18, 21, 26, 66, 99, 99, 99, 99, 24, 26, 56, 99, 99, 99,
    99, 99, 47, 66, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99
};

static const my_jpeg_huff_table g_default_dc_table[JPEG_HUFF_DC_TABLES] = {
    {{ 0x00, 0x00, 0x01, 0x05, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
     { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B },
     0 },
    {{ 0x01, 0x00, 0x03, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00 },
     { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B },
     0 }
};

static const my_jpeg_huff_table g_default_ac_table[JPEG_HUFF_DC_TABLES] = {
    {{ 0x10, 0x00, 0x02, 0x01, 0x03, 0x03, 0x02, 0x04, 0x03, 0x05, 0x05, 0x04, 0x04, 0x00, 0x00, 0x01, 0x7D },
     { 0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12, 0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07, 0x22, 0x71,
       0x14, 0x32, 0x81, 0x91, 0xA1, 0x08, 0x23, 0x42, 0xB1, 0xC1, 0x15, 0x52, 0xD1, 0xF0, 0x24, 0x33, 0x62, 0x72,
       0x82, 0x09, 0x0A, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x34, 0x35, 0x36, 0x37,
       0x38, 0x39, 0x3A, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
       0x5A, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x83,
       0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0xA2, 0xA3,
       0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xC2, 0xC3,
       0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xE1, 0xE2,
       0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA },
     0 },
    {{ 0x11, 0x00, 0x02, 0x01, 0x02, 0x04, 0x04, 0x03, 0x04, 0x07, 0x05, 0x04, 0x04, 0x00, 0x01, 0x02, 0x77 },
     { 0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21, 0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71, 0x13, 0x22,
       0x32, 0x81, 0x08, 0x14, 0x42, 0x91, 0xA1, 0xB1, 0xC1, 0x09, 0x23, 0x33, 0x52, 0xF0, 0x15, 0x62, 0x72, 0xD1,
       0x0A, 0x16, 0x24, 0x34, 0xE1, 0x25, 0xF1, 0x17, 0x18, 0x19, 0x1A, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x35, 0x36,
       0x37, 0x38, 0x39, 0x3A, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
       0x59, 0x5A, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A,
       0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A,
       0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA,
       0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA,
       0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA },
     0 }
};

/*-------------------------------------- func define --------------------------------------*/

static td_s32 jpeg_table_quant_decompress(jpeg_decompress_par_ptr par);

static td_s32 jpeg_table_huff_decompress(jpeg_decompress_par_ptr par);
static td_s32 jpeg_huff_dc_table_decompress(jpeg_decompress_par_ptr par);
static td_s32 jpeg_set_dc_default_table(my_jpeg_huff_table *dc_table);
static td_s32 jpeg_set_dc_image_table(jpeg_decompress_par_ptr par, my_jpeg_huff_table *dc_table);
static td_u32 jpeg_huff_dc_dec(const my_jpeg_huff_table *huff_tbl, unsigned int bit[JPEG_HUFF_CODE_SIZE]);
static td_void jpeg_product_dc_table(jpeg_decompress_par_ptr par, const my_jpeg_huff_table *dc_table,
                                     td_u32 huff_code[], td_u32 max_index, td_u32 tab_num);

static td_s32 jpeg_huff_ac_table_decompress(jpeg_decompress_par_ptr par);
static td_s32 jpeg_set_ac_default_table(my_jpeg_huff_table *ac_table);
static td_s32 jpeg_set_ac_image_table(jpeg_decompress_par_ptr par, my_jpeg_huff_table *ac_table);
static td_void jpeg_product_ac_table(jpeg_decompress_par_ptr par, const my_jpeg_huff_table huff_table[], td_u32 length);
static td_u32 jpeg_huff_ac_dec(const my_jpeg_huff_table *huff_tbl);
static td_void jpeg_product_ac_symbol_table(jpeg_decompress_par_ptr par, const my_jpeg_huff_table huff_table[],
                                            td_u32 sum_syms, td_u32 tab_num);
static td_void jpeg_product_ac_min_table(jpeg_decompress_par_ptr par, const td_u32 min_tab[][16]);
static td_void jpeg_product_ac_base_table(jpeg_decompress_par_ptr par, const td_u32 base_tab[][16]);

/*-------------------------------------- func release -------------------------------------*/

td_s32 jpeg_table_decompress(jpeg_decompress_par_ptr par)
{
    td_s32 ret;

    ret = jpeg_table_quant_decompress(par);
    if (ret != EXT_SUCCESS) {
        jpeg_err("call jpeg_table_quant_decompress failure");
        return EXT_FAILURE;
    }

    ret = jpeg_table_huff_decompress(par);
    if (ret != EXT_SUCCESS) {
        jpeg_err("call jpeg_table_huff_decompress failure");
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

static td_s32 jpeg_table_quant_decompress(jpeg_decompress_par_ptr par)
{
    td_u32 i;
    td_u16 *quant_cr = TD_NULL;
    td_u16 *quant_cb = TD_NULL;
    td_u16 *quant_y = TD_NULL;
    td_s32 quant_tbl_no[JPEG_QUANT_TABLES] = {0};

    if ((par->num_components > JPEG_MAX_COMPONENTS) ||
        (par->num_components > JPEG_QUANT_TABLES)) {
        return EXT_FAILURE;
    }

    if (par->quant_table[0].is_has_quant_table == TD_FALSE) {
        quant_y  = (td_u16 *)g_protocol_lu_quant_table;
        quant_cb = (td_u16 *)g_protocol_ch_quant_table;
        quant_cr = (td_u16 *)g_protocol_ch_quant_table;
    } else {
        for (i = 0; i < par->num_components; i++) {
            quant_tbl_no[i] = ((par->component_info[i].quant_table_no < 0) ||
                               (par->component_info[i].quant_table_no >= JPEG_QUANT_TABLES)) ?
                               (0) : (par->component_info[i].quant_table_no);
        }
        if (par->quant_table[quant_tbl_no[0]].is_has_quant_table == TD_FALSE) {
            jpeg_err("not quant table");
            return EXT_FAILURE;
        }
        quant_y  = par->quant_table[quant_tbl_no[0]].quant_value;
        quant_cb = (par->quant_table[quant_tbl_no[1]].is_has_quant_table == TD_FALSE) ? /* 1 is cb table */
                   (quant_y) : (par->quant_table[quant_tbl_no[1]].quant_value);  /* 1 is cb table */
        quant_cr = (par->quant_table[quant_tbl_no[2]].is_has_quant_table == TD_FALSE) ?  /* 2 is cr table */
                   (quant_cb) : (par->quant_table[quant_tbl_no[2]].quant_value);  /* 2 is cr table */
    }

    for (i = 0; i < DRV_JPEG_MAX_QTB_SIZE; i++) {
        par->reg_mgr.quant_ytable[i]  = (td_u8)quant_y[i];
        par->reg_mgr.quant_cbtable[i] = (td_u8)quant_cb[i];
        par->reg_mgr.quant_crtable[i] = (td_u8)quant_cr[i];
    }

    return EXT_SUCCESS;
}

static td_s32 jpeg_table_huff_decompress(jpeg_decompress_par_ptr par)
{
    td_s32 ret;
    ret = jpeg_huff_dc_table_decompress(par);
    if (ret != EXT_SUCCESS) {
        jpeg_err("call jpeg_huff_dc_table_decompress failure");
        return EXT_FAILURE;
    }

    ret = jpeg_huff_ac_table_decompress(par);
    if (ret != EXT_SUCCESS) {
        jpeg_err("call jpeg_huff_ac_table_decompress failure");
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

static td_s32 jpeg_huff_dc_table_decompress(jpeg_decompress_par_ptr par)
{
    td_s32 ret;
    td_u32 max_idx[MAX_TABLE] = {0};
    td_u32 huffcode[MAX_TABLE][JPEG_HUFF_CODE_SIZE] = {{0}};
    my_jpeg_huff_table dc_table[MAX_TABLE] = {{{0}, {0}, 0 }};

    if ((par->dc_huff_table[0].is_has_huff_table == TD_FALSE) ||
        (par->dc_huff_table[2].is_has_huff_table == TD_TRUE)) {  /* 2 is cr table */
        ret = jpeg_set_dc_default_table(dc_table);
        if (ret != EXT_SUCCESS) {
            jpeg_err("call jpeg_set_dc_default_table failure");
            return EXT_FAILURE;
        }
    } else {
        ret = jpeg_set_dc_image_table(par, dc_table);
        if (ret != EXT_SUCCESS) {
            jpeg_err("call jpeg_set_dc_image_table failure");
            return EXT_FAILURE;
        }
    }

    max_idx[LU] = jpeg_huff_dc_dec(&dc_table[LU], huffcode[LU]);
    max_idx[CH] = jpeg_huff_dc_dec(&dc_table[CH], huffcode[CH]);

    jpeg_product_dc_table(par, &dc_table[LU], huffcode[LU], max_idx[LU], LU);
    jpeg_product_dc_table(par, &dc_table[CH], huffcode[CH], max_idx[CH], CH);

    return EXT_SUCCESS;
}

static td_s32 jpeg_set_dc_default_table(my_jpeg_huff_table *dc_table)
{
    errno_t ret;
    td_u8 *lu_dc_dafault_table = (td_u8 *)&(g_default_dc_table[0]);
    td_u8 *ch_dc_dafault_table = (td_u8 *)&(g_default_dc_table[1]);

    lu_dc_dafault_table += 1;
    ret = memcpy_s(&(dc_table[LU].bits[1]), sizeof(dc_table[LU].bits) - 1, lu_dc_dafault_table, 16); /* 16:index */
    if (ret != EOK) {
        jpeg_err("call memcpy_s failure");
        return EXT_FAILURE;
    }

    lu_dc_dafault_table += 16; /* 16 is lu value length */
    ret = memcpy_s(&(dc_table[LU].huff_value[0]), sizeof(dc_table[LU].huff_value),
                   lu_dc_dafault_table, DRV_JPEG_MAX_HTB_DC_SIZE);
    if (ret != EOK) {
        jpeg_err("call memcpy_s failure");
        return EXT_FAILURE;
    }

    ch_dc_dafault_table += 1;
    ret = memcpy_s(&(dc_table[CH].bits[1]), sizeof(dc_table[CH].bits) - 1, ch_dc_dafault_table, 16); /* 16:index */
    if (ret != EOK) {
        jpeg_err("call memcpy_s failure");
        return EXT_FAILURE;
    }

    ch_dc_dafault_table += 16; /* 16 is ch value length */
    ret = memcpy_s(&(dc_table[CH].huff_value[0]), sizeof(dc_table[CH].huff_value),
                   ch_dc_dafault_table, DRV_JPEG_MAX_HTB_DC_SIZE);
    if (ret != EOK) {
        jpeg_err("call memcpy_s failure");
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

static td_s32 jpeg_set_dc_image_table(jpeg_decompress_par_ptr par, my_jpeg_huff_table *dc_table)
{
    errno_t ret;
    ret = memcpy_s(&(dc_table[LU]), sizeof(dc_table[LU]), &(par->dc_huff_table[0]), sizeof(par->dc_huff_table[0]));
    if (ret != EOK) {
        jpeg_err("call memcpy_s failure");
        return EXT_FAILURE;
    }

    if (par->dc_huff_table[1].is_has_huff_table == TD_FALSE) {
        ret = memcpy_s(&(dc_table[CH]), sizeof(dc_table[CH]), &(par->dc_huff_table[0]), sizeof(par->dc_huff_table[0]));
        if (ret != EOK) {
            jpeg_err("call memcpy_s failure");
            return EXT_FAILURE;
        }
    } else {
        ret = memcpy_s(&(dc_table[CH]), sizeof(dc_table[CH]), &(par->dc_huff_table[1]), sizeof(par->dc_huff_table[1]));
        if (ret != EOK) {
            jpeg_err("call memcpy_s failure");
            return EXT_FAILURE;
        }
    }

    return EXT_SUCCESS;
}

static td_u32 jpeg_huff_dc_dec(const my_jpeg_huff_table *huff_tbl, unsigned int bit[JPEG_HUFF_CODE_SIZE])
{
    td_u32 idx;
    td_u32 cnt;
    td_u32 max_idx = 0;
    td_u32 loc = 0;
    td_u32 value = 0;

    for (idx = 1; idx < JPEG_HUFF_BITS_SIZE; idx++) {
        if (huff_tbl->bits[idx] == 0) {
            value <<= 1;
            continue;
        }
        max_idx = idx;
        for (cnt = huff_tbl->bits[idx]; cnt > 0 && loc < JPEG_HUFF_CODE_SIZE; cnt--) {
            bit[loc] = value;
            loc++;
            value++;
        }
        value <<= 1;
    }
    return max_idx;
}

static td_void jpeg_product_dc_table(jpeg_decompress_par_ptr par, const my_jpeg_huff_table *dc_table,
                                     td_u32 huff_code[], td_u32 max_index, td_u32 tab_num)
{
    td_u32 sum_syms = 0;
    td_u32 syms = 0;
    td_u32 temp = 0;
    td_u32 index = 0;
    td_u32 index1 = 0;
    td_u32 dc_hufflen[DRV_JPEG_MAX_HTB_DC_SIZE] = {0};
    td_u32 dc_sym[DRV_JPEG_MAX_HTB_DC_SIZE] = {0};

    if (max_index > 16) { /* 16:max index */
        return;
    }

    for (index = 0; index < max_index; index++) {
        syms = dc_table->bits[index + 1];
        sum_syms += syms;
        while ((syms-- != 0) && (temp < DRV_JPEG_MAX_HTB_DC_SIZE)) {
            dc_hufflen[temp] = index + 1;
            dc_sym[temp] = dc_table->huff_value[temp];
            temp++;
        }
    }

    if (sum_syms > DRV_JPEG_MAX_HTB_DC_SIZE) {
        return;
    }

    for (index = 0; index < sum_syms; index++) {
        for (index1 = index + 1; index1 < sum_syms; index1++) {
            if (dc_sym[index] > dc_sym[index1]) {
                jpeg_swap(&dc_sym[index], &dc_sym[index1]);
                jpeg_swap(&dc_hufflen[index], &dc_hufflen[index1]);
                jpeg_swap(&huff_code[index], &huff_code[index1]);
            }
        }
        if (dc_sym[index] >= DRV_JPEG_MAX_HTB_DC_SIZE) {
            return;
        }
    }

    for (index = 0; index < sum_syms; index++) {
        if (tab_num == 0) {
            /* 8:shift val */
            *(par->reg_mgr.dht_dc_lu_table + dc_sym[index]) = (td_u16)(((dc_hufflen[index] & 0xf) << 8) |
                                                                      (huff_code[index] & 0xff));
        } else {
            /* 8:shift val */
            *(par->reg_mgr.dht_dc_ch_table + dc_sym[index]) = (td_u16)(((dc_hufflen[index] & 0xf) << 8) |
                                                                      (huff_code[index] & 0xff));
        }
    }

    return;
}

static td_s32 jpeg_huff_ac_table_decompress(jpeg_decompress_par_ptr par)
{
    td_s32 ret;
    my_jpeg_huff_table ac_table[MAX_TABLE] = {{{0}, {0}, 0 }};

    if ((par->ac_huff_table[0].is_has_huff_table == TD_FALSE) ||
        (par->ac_huff_table[2].is_has_huff_table == TD_TRUE)) { /* 2:index */
        ret = jpeg_set_ac_default_table(ac_table);
        if (ret != EXT_SUCCESS) {
            jpeg_err("call jpeg_set_ac_default_table failure");
            return EXT_FAILURE;
        }
    } else {
        ret = jpeg_set_ac_image_table(par, ac_table);
        if (ret != EXT_SUCCESS) {
            jpeg_err("call jpeg_set_ac_image_table failure");
            return EXT_FAILURE;
        }
    }

    jpeg_product_ac_table(par, ac_table, MAX_TABLE);

    return EXT_SUCCESS;
}

static td_s32 jpeg_set_ac_default_table(my_jpeg_huff_table *ac_table)
{
    errno_t err_ret;
    td_u8 *lu_ac_default_table = (td_u8 *)&(g_default_ac_table[0]);
    td_u8 *ch_ac_default_table = (td_u8 *)&(g_default_ac_table[1]);

    lu_ac_default_table += 1;
    err_ret = memset_s(&(ac_table[LU].bits[0]), sizeof(ac_table[LU].bits), 0, sizeof(ac_table[LU].bits));
    if (err_ret != EOK) {
        return EXT_FAILURE;
    }

    /* 16:table size */
    err_ret = memcpy_s(&(ac_table[LU].bits[1]), sizeof(ac_table[LU].bits) - 1, lu_ac_default_table, 16);
    if (err_ret != EOK) {
        return EXT_FAILURE;
    }

    lu_ac_default_table += 16; /* 16:index */
    err_ret = memset_s(&(ac_table[LU].huff_value[0]), sizeof(ac_table[LU].huff_value),
                       0x0, sizeof(ac_table[LU].huff_value));
    if (err_ret != EOK) {
        return EXT_FAILURE;
    }

    /* 162:table size */
    err_ret = memcpy_s(&(ac_table[LU].huff_value[0]), sizeof(ac_table[LU].huff_value), lu_ac_default_table, 162);
    if (err_ret != EOK) {
        return EXT_FAILURE;
    }

    ch_ac_default_table += 1;
    err_ret = memset_s(&(ac_table[CH].bits[0]), sizeof(ac_table[CH].bits), 0, sizeof(ac_table[CH].bits));
    if (err_ret != EOK) {
        return EXT_FAILURE;
    }

    /* 16:table size */
    err_ret = memcpy_s(&(ac_table[CH].bits[1]), sizeof(ac_table[CH].bits) - 1, ch_ac_default_table, 16);
    if (err_ret != EOK) {
        return EXT_FAILURE;
    }

    ch_ac_default_table += 16; /* 16:index */
    err_ret = memset_s(&(ac_table[CH].huff_value[0]), sizeof(ac_table[CH].huff_value),
                       0, sizeof(ac_table[CH].huff_value));
    if (err_ret != EOK) {
        return EXT_FAILURE;
    }

    /* 162:table size */
    err_ret = memcpy_s(&(ac_table[CH].huff_value[0]), sizeof(ac_table[CH].huff_value), ch_ac_default_table, 162);
    if (err_ret != EOK) {
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

static td_s32 jpeg_set_ac_image_table(jpeg_decompress_par_ptr par, my_jpeg_huff_table *ac_table)
{
    errno_t err_ret;

    err_ret = memset_s(&(ac_table[LU].bits[0]), sizeof(ac_table[LU].bits), 0, sizeof(ac_table[LU].bits));
    if (err_ret != EOK) {
        return EXT_FAILURE;
    }

    err_ret = memset_s(&(ac_table[LU].huff_value[0]), sizeof(ac_table[LU].huff_value),
                       0, sizeof(ac_table[LU].huff_value));
    if (err_ret != EOK) {
        return EXT_FAILURE;
    }

    err_ret = memset_s(&(ac_table[CH].bits[0]), sizeof(ac_table[CH].bits), 0, sizeof(ac_table[CH].bits));
    if (err_ret != EOK) {
        return EXT_FAILURE;
    }

    err_ret = memset_s(&(ac_table[CH].huff_value[0]), sizeof(ac_table[CH].huff_value),
                       0, sizeof(ac_table[CH].huff_value));
    if (err_ret != EOK) {
        return EXT_FAILURE;
    }

    err_ret = memcpy_s(&(ac_table[LU]), sizeof(ac_table[LU]), &(par->ac_huff_table[0]), sizeof(par->ac_huff_table[0]));
    if (err_ret != EOK) {
        return EXT_FAILURE;
    }
    if (par->ac_huff_table[1].is_has_huff_table == TD_FALSE) {
        err_ret = memcpy_s(&(ac_table[CH]), sizeof(ac_table[CH]),
                           &(par->ac_huff_table[0]), sizeof(par->ac_huff_table[0]));
        if (err_ret != EOK) {
            return EXT_FAILURE;
        }
    } else {
        err_ret = memcpy_s(&(ac_table[CH]), sizeof(ac_table[CH]),
                           &(par->ac_huff_table[1]), sizeof(par->ac_huff_table[1]));
        if (err_ret != EOK) {
            return EXT_FAILURE;
        }
    }
    return EXT_SUCCESS;
}

static td_void jpeg_product_ac_table(jpeg_decompress_par_ptr par, const my_jpeg_huff_table huff_table[], td_u32 length)
{
    td_u32 tab_num, index, pre_index, sum_syms, syms;
    td_u32 min_tab[MAX_TABLE][16] = {{0}};  /* 16:table size */
    td_u32 base_tab[MAX_TABLE][16] = {{0}}; /* 16:table size */
    td_u32 max_idx[MAX_TABLE] = {0};

    max_idx[LU] = jpeg_huff_ac_dec(&huff_table[0]);
    max_idx[CH] = jpeg_huff_ac_dec(&huff_table[1]);

    for (tab_num = 0; tab_num < length; tab_num++) {
        sum_syms = 0;
        for (index = 0; index < 16; index++) { /* 16:index */
            syms = huff_table[tab_num].bits[index + 1];
            pre_index = (index != 0) ? (index - 1) : (0);
            if (index >= max_idx[tab_num]) {
                min_tab[tab_num][index] = ~0;
                continue;
            }
            min_tab[tab_num][index] = (min_tab[tab_num][pre_index] + huff_table[tab_num].bits[index]) << 1;
            if (syms != 0) {
                base_tab[tab_num][index] = sum_syms - min_tab[tab_num][index];
            }
            sum_syms += huff_table[tab_num].bits[index + 1];
        }

        if (sum_syms > DRV_JPEG_MAX_HTB_AC_SYMBOL_SIZE) {
            return;
        }

        jpeg_product_ac_symbol_table(par, huff_table, sum_syms, tab_num);
    }

    jpeg_product_ac_min_table(par, min_tab);
    jpeg_product_ac_base_table(par, base_tab);

    return;
}

static td_u32 jpeg_huff_ac_dec(const my_jpeg_huff_table *huff_tbl)
{
    td_u32 index = 0;
    td_u32 max_idx = 0;
    for (index = 1; index < 17; index++) { /* 17:max index */
        if (huff_tbl->bits[index] == 0) {
            continue;
        }
        max_idx = index;
    }
    return max_idx;
}

static td_void jpeg_product_ac_symbol_table(jpeg_decompress_par_ptr par, const my_jpeg_huff_table huff_table[],
                                            td_u32 sum_syms, td_u32 tab_num)
{
    td_u32 index = 0;
    for (index = 0; index < sum_syms; index++) {
        if (tab_num == 0) {
            par->reg_mgr.dht_ac_lu_symblo[index] = huff_table[LU].huff_value[index];
        } else {
            par->reg_mgr.dht_ac_ch_symblo[index] = huff_table[CH].huff_value[index];
        }
    }
    return;
}

static td_void jpeg_product_ac_min_table(jpeg_decompress_par_ptr par, const td_u32 min_tab[][16])
{
    td_u32 index = 0;
    for (index = 0; index < DRV_JPEG_MAX_HTB_AC_MIN_SIZE; index++) {
        par->reg_mgr.dht_ac_lu_mincode_even[index] = min_tab[LU][MAX_TABLE * index]     & 0xff;
        par->reg_mgr.dht_ac_lu_mincode_odd[index]  = min_tab[LU][MAX_TABLE * index + 1] & 0xff;
        par->reg_mgr.dht_ac_ch_mincode_even[index] = min_tab[CH][MAX_TABLE * index]     & 0xff;
        par->reg_mgr.dht_ac_ch_mincode_odd[index]  = min_tab[CH][MAX_TABLE * index + 1] & 0xff;
    }
    return;
}

static td_void jpeg_product_ac_base_table(jpeg_decompress_par_ptr par, const td_u32 base_tab[][16])
{
    td_u32 index = 0;
    for (index = 0; index < DRV_JPEG_MAX_HTB_AC_BASE_SIZE; index++) {
        par->reg_mgr.dht_ac_lu_base_even[index] = base_tab[LU][MAX_TABLE * index]     & 0xff;
        par->reg_mgr.dht_ac_lu_base_odd[index]  = base_tab[LU][MAX_TABLE * index + 1] & 0xff;
        par->reg_mgr.dht_ac_ch_base_even[index] = base_tab[CH][MAX_TABLE * index]     & 0xff;
        par->reg_mgr.dht_ac_ch_base_odd[index]  = base_tab[CH][MAX_TABLE * index + 1] & 0xff;
    }
    return;
}

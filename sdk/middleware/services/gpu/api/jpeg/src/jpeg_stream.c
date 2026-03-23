/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: jpeg stream
 */

#include "jpeg_stream.h"
#include "jpeg_mem.h"

/*-------------------------------------- macro define -------------------------------------*/

#define JPEG_STREAM_INPUT_SIZE  (128 * 1024)
#define JPEG_STREAM_1BYTE       1
#define JPEG_STREAM_2BYTE       2
#define JPEG_STREAM_BITDEPTH    8

/*-------------------------------------- struct info --------------------------------------*/

/*-------------------------------------- func define --------------------------------------*/

/*-------------------------------------- func release -------------------------------------*/

td_s32 jpeg_stream_init(jpeg_decompress_par_ptr par)
{
    td_s32 ret;
    jpeg_mem_mgr mem = { 0 };

    if (par->src.is_mem == TD_TRUE) {
        if ((par->src.phy_buf == 0) || (par->src.vir_buf == TD_NULL) || (par->src.size == 0)) {
            jpeg_err("input buf or buf size is error");
            return EXT_FAILURE;
        }
        par->reg_mgr.is_eoi = 0x1;
        par->reg_mgr.stream_phy_buf  = par->src.phy_buf;
        par->reg_mgr.stream_buf_size = par->src.size;
        return EXT_SUCCESS;
    } else {
        if (par->src.infile == TD_NULL) {
            jpeg_err("input file is error");
            return EXT_FAILURE;
        }
        par->reg_mgr.is_eoi = 0x0;
    }

    mem.size  = JPEG_STREAM_INPUT_SIZE;
    mem.align = JPEG_MEM_STREAM_ALIGN;
    ret = jpeg_mem_alloc_large(&mem);
    if (ret != EXT_SUCCESS) {
        jpeg_err("call jpeg_mem_alloc_large failure");
        return EXT_FAILURE;
    }

    par->src.vir_buf = mem.vir_buf;
    par->src.phy_buf = mem.phy_buf;
    par->src.size = mem.size;
    par->src.cur_offset = 0;
    par->src.size_in_buffer = 0;

    par->reg_mgr.stream_phy_buf  = par->src.phy_buf;
    par->reg_mgr.stream_buf_size = par->src.size;
    return EXT_SUCCESS;
}


td_void jpeg_stream_deinit(jpeg_decompress_par_ptr par)
{
    jpeg_mem_mgr mem = { 0 };

    if (par->src.is_mem == TD_TRUE) {
        return;
    }

    mem.vir_buf = (td_char *)par->src.vir_buf;
    mem.phy_buf = par->src.phy_buf;
    jpeg_mem_free_large(&mem);

    par->src.vir_buf = TD_NULL;
    par->src.phy_buf = 0;
    par->src.size = 0;
    par->src.cur_offset = 0;
    par->src.size_in_buffer = 0;

    return;
}

td_s32 jpeg_stream_read_one_byte(jpeg_decompress_par_ptr par)
{
    td_s32 code;
    td_char *buffer = TD_NULL;

    if (par->src.size_in_buffer == 0) {
        jpeg_stream_fill_buffer(par);
    }

    buffer = par->src.vir_buf + par->src.cur_offset;
    code = (td_u32)(*buffer) & 0xff; /* 0xff get low 8 bits */

    par->src.cur_offset += JPEG_STREAM_1BYTE;
    par->src.size_in_buffer -= JPEG_STREAM_1BYTE;

    return code;
}

td_s32 jpeg_stream_read_two_byte(jpeg_decompress_par_ptr par)
{
    td_s32 code1, code2;

    code1 = jpeg_stream_read_one_byte(par);
    if (code1 < 0) {
        return -1; /* -1 is illegal value */
    }

    code2 = jpeg_stream_read_one_byte(par);
    if (code2 < 0) {
        return -1; /* -1 is illegal value */
    }

    return (td_s32)(((td_u32)code1 << JPEG_STREAM_BITDEPTH) | (td_u32)code2);
}

td_void jpeg_stream_skip_bytes(jpeg_decompress_par_ptr par, td_u32 nbytes)
{
    td_u32 tmp_nbytes = nbytes;

    if (tmp_nbytes == 0) {
        return;
    }

    while (tmp_nbytes > par->src.size_in_buffer) {
        tmp_nbytes -= par->src.size_in_buffer;
        jpeg_stream_fill_buffer(par);
    }

    par->src.cur_offset += tmp_nbytes;
    par->src.size_in_buffer -= tmp_nbytes;

    return;
}

td_void jpeg_stream_fill_buffer(jpeg_decompress_par_ptr par)
{
    td_s32 nbytes;

    if (par->src.is_mem == TD_TRUE) {
        nbytes = 0;
    } else {
        nbytes = (td_s32)fread(par->src.vir_buf, JPEG_STREAM_1BYTE, JPEG_STREAM_INPUT_SIZE, par->src.infile);
    }

    if (nbytes <= 0) {
        par->src.vir_buf[0] = JPEG_MARK_TAG;
        par->src.vir_buf[JPEG_STREAM_1BYTE] = JPEG_MARK_EOI;
        nbytes = JPEG_STREAM_2BYTE;
    }

    if (nbytes < JPEG_STREAM_INPUT_SIZE) {
        par->reg_mgr.is_eoi = 0x1;
    }

    par->src.cur_offset = 0;
    par->src.size_in_buffer = (td_u32)nbytes;
    return;
}

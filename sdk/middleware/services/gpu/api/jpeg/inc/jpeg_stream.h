/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: jpeg stream
 */

#ifndef API_GRAPHICS_JPEG_STREAM_H
#define API_GRAPHICS_JPEG_STREAM_H

#include "jpeg_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*-------------------------------- macro define -------------------------------------------*/

/*-------------------------------- struct define ------------------------------------------*/

/*-------------------------------- func declares ------------------------------------------*/

td_s32  jpeg_stream_init(jpeg_decompress_par_ptr par);
td_void jpeg_stream_deinit(jpeg_decompress_par_ptr par);
td_s32  jpeg_stream_read_one_byte(jpeg_decompress_par_ptr par);
td_s32  jpeg_stream_read_two_byte(jpeg_decompress_par_ptr par);
td_void jpeg_stream_skip_bytes(jpeg_decompress_par_ptr par, td_u32 nbytes);
td_void jpeg_stream_fill_buffer(jpeg_decompress_par_ptr par);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* API_GRAPHICS_JPEG_STREAM_H */
/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: jpeg exif
 */

#ifndef API_GRAPHICS_JPEG_EXIF_H
#define API_GRAPHICS_JPEG_EXIF_H

#include "jpeg_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*-------------------------------- macro define -------------------------------------------*/

/*-------------------------------- struct define ------------------------------------------*/

/*-------------------------------- func declares ------------------------------------------*/

td_void jpeg_exif_init(jpeg_decompress_par_ptr par);
td_void jpeg_exif_deinit(jpeg_decompress_par_ptr par);
td_void jpeg_exif_parse(jpeg_decompress_par_ptr par);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* API_GRAPHICS_JPEG_EXIF_H */
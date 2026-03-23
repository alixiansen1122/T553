/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: jpeg csc
 */

#ifndef API_GRAPHICS_JPEG_CSC_H
#define API_GRAPHICS_JPEG_CSC_H

#include "jpeg_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*-------------------------------- macro define -------------------------------------------*/

/*-------------------------------- struct define ------------------------------------------*/

/*-------------------------------- func declares ------------------------------------------*/

td_s32 jpeg_csc_convert(const jpeg_decompress_par_ptr par);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* API_GRAPHICS_JPEG_CSC_H */

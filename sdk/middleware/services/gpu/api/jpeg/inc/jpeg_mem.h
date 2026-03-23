/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: jpeg mem
 */

#ifndef API_GRAPHICS_JPEG_API_MEM_H
#define API_GRAPHICS_JPEG_API_MEM_H

#include "td_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*-------------------------------- macro define -------------------------------------------*/

#define JPEG_MEM_STREAM_ALIGN 16
#define JPEG_MEM_OUTPUT_ALIGN 128

/*-------------------------------- struct define ------------------------------------------*/

typedef struct {
    td_char *vir_buf;
    td_u32 phy_buf;
    td_u32 size;
    td_u32 align;
} jpeg_mem_mgr, *jpeg_mem_ptr;

/*-------------------------------- func declares ------------------------------------------*/

td_s32  jpeg_mem_alloc_small(jpeg_mem_ptr mem);
td_void jpeg_mem_free_small(jpeg_mem_ptr mem);
td_s32  jpeg_mem_alloc_large(jpeg_mem_ptr mem);
td_void jpeg_mem_free_large(const jpeg_mem_ptr mem);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* API_GRAPHICS_JPEG_API_MEM_H */

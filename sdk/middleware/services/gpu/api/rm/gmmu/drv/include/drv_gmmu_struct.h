/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: drv gmmu struct
 */

#ifndef DRV_GRAPHICS_DRV_GMMU_STRUCT_H
#define DRV_GRAPHICS_DRV_GMMU_STRUCT_H

#include "td_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*-------------------------------- macro define -------------------------------------------*/

#define DRV_GMMU_MASK   0x0123456789ABCDEF

/*-------------------------------- struct define ------------------------------------------*/

typedef struct {
    td_u64 mask;
    td_u8 input_bpp;
    td_u32 input_width;
    td_u32 input_height;
    td_u8 offset;
    td_u32 output_size;
    td_u32 output_stride;
    td_u32 output_addr0;
    td_u32 output_addr1;
} drv_gmmu_mgr;

/*-------------------------------- func declares ------------------------------------------*/

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* DRV_GRAPHICS_DRV_GMMU_STRUCT_H */

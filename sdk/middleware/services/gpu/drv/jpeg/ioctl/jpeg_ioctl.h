/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: jpeg ioctl
 */

#ifndef DRV_GRAPHICS_JPEG_IOCTL_H
#define DRV_GRAPHICS_JPEG_IOCTL_H

#include "td_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*-------------------------------- macro define -------------------------------------------*/

/*-------------------------------- struct define ------------------------------------------*/

/*-------------------------------- func declares ------------------------------------------*/

td_s32 jpeg_ioctl_start_decompress(td_u32 cmd, td_void *arg, td_void *file);
td_s32 jpeg_ioctl_get_decompress_status(td_u32 cmd, td_void *arg, td_void *file);
td_s32 jpeg_ioctl_decomress_create(td_u32 cmd, td_void *arg, td_void *file);
td_s32 jpeg_ioctl_decompress_destroy(td_u32 cmd, td_void *arg, td_void *file);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* DRV_GRAPHICS_JPEG_IOCTL_H */

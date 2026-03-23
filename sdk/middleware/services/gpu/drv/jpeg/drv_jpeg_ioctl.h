/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: drv jpeg ioctl
 */

#ifndef DRV_JPEG_IOCTL_H
#define DRV_JPEG_IOCTL_H

#include "drv_jpeg_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*-------------------------------- macro define -------------------------------------------*/

#define JPEG_DEV_NAME  "/dev/jpeg"

#define DRV_JPEG_CMD_DECODE       0
#define DRV_JPEG_CMD_GETINTSTATUS 1
#define DRV_JPEG_CMD_CREATE       2
#define DRV_JPEG_CMD_DESTROY      3

td_s32 jpeg_open(const td_char *path_name, td_s32 flags, td_s32 mode);
td_s32 jpeg_ioctl(td_s32 module_idx, td_ulong ioctl_cmd, td_void *para);
td_s32 jpeg_close(td_s32 module_idx);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* DRV_JPEG_IOCTL_H */

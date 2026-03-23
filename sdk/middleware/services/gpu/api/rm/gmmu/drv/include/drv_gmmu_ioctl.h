/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: drv gmmu ioctl
 */

#ifndef DRV_GRAPHICS_DRV_GMMU_IOCTL_H
#define DRV_GRAPHICS_DRV_GMMU_IOCTL_H

#include <stdio.h>
#include "td_base.h"
#include "drv_gmmu_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*-------------------------------- macro define -------------------------------------------*/

#define GMMU_DEV_NAME  "/dev/gmmu"

#define DRV_GMMU_CMD_CONFIG  0

td_s32 gmmu_open(const td_char *path_name, td_s32 flags, td_s32 mode);
td_s32 gmmu_ioctl(td_s32 module_idx, td_ulong ioctl_cmd, td_void *para);
td_s32 gmmu_close(td_s32 module_idx);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* DRV_GRAPHICS_DRV_GMMU_IOCTL_H */

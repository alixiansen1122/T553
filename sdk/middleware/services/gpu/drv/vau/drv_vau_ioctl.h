/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description : vau ioctl head file
 */

#ifndef DRV_GRAPHICS_DRV_VAU_IOCTL_H
#define DRV_GRAPHICS_DRV_VAU_IOCTL_H

#include "td_base.h"
#include "drv_vau_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define VAU_DEV_NAME   "/dev/vau"

#define DRV_VAU_CMD_CREATE        0
#define DRV_VAU_CMD_DESTROY       1
#define DRV_VAU_CMD_FILL          2
#define DRV_VAU_CMD_BLIT          3
#define DRV_VAU_CMD_COMPOSE       4
#define DRV_VAU_CMD_SUBMIT        5
#define DRV_VAU_CMD_CANCEL        6
#define DRV_VAU_CMD_WAIT_FOR_DONE 7
#define DRV_VAU_CMD_SET_CUBIC_THRESHOLD 8

td_s32 vau_open(const td_char *path_name, td_s32 flags, td_s32 mode);
td_s32 vau_ioctl(td_s32 module_idx, td_ulong ioctl_cmd, td_void *para);
td_s32 vau_close(td_s32 module_idx);


#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif /* DRV_GRAPHICS_DRV_VAU_IOCTL_H */

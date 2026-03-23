/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description : dpu ioctl head file
 */

#ifndef DRV_GRAPHICS_DRV_DPU_IOCTL_H
#define DRV_GRAPHICS_DRV_DPU_IOCTL_H

#include "td_base.h"
#include "drv_dpu_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define DPU_DEV_NAME  "/dev/dpu"

#define DRV_DPU_CMD_CREATELAYER       0
#define DRV_DPU_CMD_DESTROYLAYER      1

#define DRV_DPU_CMD_PUTLAYERALPHA     2
#define DRV_DPU_CMD_GETLAYERALPHA     3

#define DRV_DPU_CMD_PUTLAYERPOS       4
#define DRV_DPU_CMD_GETLAYERPOS       5

#define DRV_DPU_CMD_PUTLAYERZORDER    6
#define DRV_DPU_CMD_GETLAYERZORDER    7

#define DRV_DPU_CMD_SHOWLAYER         8
#define DRV_DPU_CMD_HIDELAYER         9

#define DRV_DPU_CMD_PUTLAYERINFO      10
#define DRV_DPU_CMD_GETLAYERINFO      11

#define DRV_DPU_CMD_PUTYLAYERCOLORKYE 12
#define DRV_DPU_CMD_GETYLAYERCOLORKYE 13

#define DRV_DPU_CMD_REFRESHLAYERS     14
#define DRV_DPU_CMD_WAITLAYERVSYNC    15

#define DRV_DPU_CMD_PUTLAYERVBLANK    16
#define DRV_DPU_CMD_GETLAYERVBLANK    17

#define DRV_DPU_CMD_QUERYLAYERSTATE   18

td_s32 dpu_open(const td_char *path_name, td_s32 flags, td_s32 mode);
td_s32 dpu_ioctl(td_s32 module_idx, td_ulong ioctl_cmd, td_void *para);
td_s32 dpu_close(td_s32 module_idx);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif /* DRV_GRAPHICS_DRV_DPU_IOCTL_H */

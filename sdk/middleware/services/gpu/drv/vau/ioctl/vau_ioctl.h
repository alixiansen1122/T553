/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description : vau ioctl headfile
 */

#ifndef DRV_GRAPHICS_IOCTL_VAU_IOCTL_H
#define DRV_GRAPHICS_IOCTL_VAU_IOCTL_H

#include "td_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

td_s32 vau_ioctl_create(td_u32 cmd, td_void *arg, td_void *file);
td_s32 vau_ioctl_fill(td_u32 cmd, td_void *arg, td_void *file);
td_s32 vau_ioctl_blit(td_u32 cmd, td_void *arg, td_void *file);
td_s32 vau_ioctl_compose(td_u32 cmd, td_void *arg, td_void *file);
td_s32 vau_ioctl_submit(td_u32 cmd, td_void *arg, td_void *file);
td_s32 vau_ioctl_cancel(td_u32 cmd, td_void *arg, td_void *file);
td_s32 vau_ioctl_destroy(td_u32 cmd, td_void *arg, td_void *file);
td_s32 vau_ioctl_wait_for_done(td_u32 cmd, td_void *arg, td_void *file);
td_s32 vau_ioctl_set_cubic_threshold(td_u32 cmd, td_void *arg, td_void *file);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif /* DRV_GRAPHICS_IOCTL_VAU_IOCTL_H */

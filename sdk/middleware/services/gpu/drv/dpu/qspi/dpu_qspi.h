/*
 * Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved.
 * Description : dpu qspi header
 */

#ifndef DRV_GRAPHICS_QSPI_DPU_QSPI_H
#define DRV_GRAPHICS_QSPI_DPU_QSPI_H

#include "td_base.h"
#include "soc_osal.h"
#include "lcd_bus.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */


td_s32 dpu_qspi_init(td_void);
td_s32 dpu_qspi_ioctl(td_ulong ioctl_cmd, td_void *para);
td_bool dpu_qspi_check_is_qspi(td_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif /* DRV_GRAPHICS_QSPI_DPU_QSPI_H */
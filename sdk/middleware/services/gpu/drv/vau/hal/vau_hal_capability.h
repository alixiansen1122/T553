/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: capability
 */

#ifndef DRV_GRAPHIC_VAU_HAL_CAPABILITY_H
#define DRV_GRAPHIC_VAU_HAL_CAPABILITY_H

#include "drv_vau_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*-------------------------------- macro define -------------------------------------------*/

/*-------------------------------- struct define ------------------------------------------*/

/*-------------------------------- par define ---------------------------------------------*/

/*-------------------------------- func declares ------------------------------------------*/

td_s32 vau_hal_check_src1(const drv_vau_surface *surface, td_bool is_dma);
td_s32 vau_hal_check_src2(const drv_vau_surface *surface);
td_s32 vau_hal_check_dst(const drv_vau_surface *surface);
td_s32 vau_hal_check_blit_opt(const drv_vau_blit_opt *opt);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* DRV_GRAPHIC_VAU_HAL_CAPABILITY_H */

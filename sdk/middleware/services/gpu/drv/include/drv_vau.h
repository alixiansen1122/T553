/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description : drv vau file
 */

#ifndef DRV_INCLUDE_DRV_VAU_H
#define DRV_INCLUDE_DRV_VAU_H

#include "drv_vau_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

td_s32 drv_vau_suspend(td_void *private_data);
td_s32 drv_vau_resume(td_void *private_data);
td_bool drv_vau_get_suspend_flag(td_void);

td_s32 uapi_vau_module_init(td_void);
td_void uapi_vau_module_exit(td_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* DRV_INCLUDE_DRV_VAU_H */

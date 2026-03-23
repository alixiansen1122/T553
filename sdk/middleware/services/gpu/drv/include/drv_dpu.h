/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description : drv dpu file
 */

#ifndef DRV_INCLUDE_DRV_DPU_H
#define DRV_INCLUDE_DRV_DPU_H

#include "drv_dpu_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

td_s32 drv_dpu_suspend(td_void *private_data);
td_s32 drv_dpu_resume(td_void *private_data);
td_bool drv_dpu_get_suspend_flag(td_void);

td_s32 uapi_dpu_module_init(void);
td_void uapi_dpu_module_exit(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* DRV_INCLUDE_DRV_DPU_H */
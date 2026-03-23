/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: drv gmmu intf
 */

#ifndef DRV_GRAPHICS_DRV_GMMU_INTF_H
#define DRV_GRAPHICS_DRV_GMMU_INTF_H

#include "td_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*-------------------------------- macro define -------------------------------------------*/

typedef td_s32 (*pfn_gmmu_suspend)(td_void);
typedef td_s32 (*pfn_gmmu_resume)(td_void);

/*-------------------------------- struct define ------------------------------------------*/

typedef struct {
    pfn_gmmu_suspend suspend;
    pfn_gmmu_resume  resume;
} drv_gmmu_export_func;

/*-------------------------------- func declares ------------------------------------------*/

td_s32 drv_gmmu_module_init(td_void);
td_void drv_gmmu_module_exit(td_void);

td_s32 drv_gmmu_suspend(td_void *private_data);
td_s32 drv_gmmu_resume(td_void *private_data);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* DRV_GRAPHICS_DRV_GMMU_INTF_H */

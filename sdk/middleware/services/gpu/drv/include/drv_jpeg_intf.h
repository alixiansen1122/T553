/*
 * Copyright (c) CompanyNameMagic Co., Ltd. 2021-2021. All rights reserved.
 * Description: drv jpeg intf
 */

#ifndef DRV_INCLUDE_DRV_JPEG_INTF_H
#define DRV_INCLUDE_DRV_JPEG_INTF_H

#include "td_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*-------------------------------- macro define -------------------------------------------*/

typedef td_s32 (*pfn_jpeg_suspend)(td_void);
typedef td_s32 (*pfn_jpeg_resume)(td_void);

/*-------------------------------- struct define ------------------------------------------*/

typedef struct {
    pfn_jpeg_suspend suspend;
    pfn_jpeg_resume  resume;
} drv_jpeg_export_func;

/*-------------------------------- func declares ------------------------------------------*/

td_s32 uapi_jpeg_module_init(td_void);
td_void uapi_jpeg_module_exit(td_void);
td_s32 drv_jpeg_suspend(td_void *private_data);
td_s32 drv_jpeg_resume(td_void *private_data);
td_void uapi_jpeg_video_sub_reset(td_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* DRV_INCLUDE_DRV_JPEG_INTF_H */

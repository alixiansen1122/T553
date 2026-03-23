/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: jpeg dev
 */

#ifndef DRV_GRAPHICS_JPEG_DEV_H
#define DRV_GRAPHICS_JPEG_DEV_H

#include "td_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*-------------------------------- macro define -------------------------------------------*/

/*-------------------------------- struct define ------------------------------------------*/

/*-------------------------------- func declares ------------------------------------------*/

td_s32 jpeg_dev_open(td_void);
td_s32 jpeg_dev_close(td_void);
td_void jpeg_suspend(td_void);
td_void jpeg_resume(td_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* DRV_GRAPHICS_JPEG_DEV_H */

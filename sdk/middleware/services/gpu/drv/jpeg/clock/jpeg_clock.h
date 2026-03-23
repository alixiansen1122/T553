/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: jpeg clock
 */

#ifndef DRV_GRAPHICS_JPEG_CLOCK_H
#define DRV_GRAPHICS_JPEG_CLOCK_H

#include "td_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*-------------------------------- macro define -------------------------------------------*/

/*-------------------------------- struct define ------------------------------------------*/

/*-------------------------------- func declares ------------------------------------------*/

td_void jpeg_clock_open(td_void);
td_void jpeg_clock_close(td_void);
td_void jpeg_clock_reset(td_void);
td_void jpeg_clock_dereset(td_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* DRV_GRAPHICS_JPEG_CLOCK_H */

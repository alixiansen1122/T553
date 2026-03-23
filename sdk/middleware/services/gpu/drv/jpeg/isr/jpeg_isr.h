/*
 * Copyright (c) @CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: jpeg isr
 */

#ifndef DRV_GRAPHICS_JPEG_ISR_H
#define DRV_GRAPHICS_JPEG_ISR_H

#include "td_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

td_s32 jpeg_isr_init(td_void);
td_s32 jpeg_wait_all_finish(td_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* DRV_GRAPHICS_JPEG_ISR_H */

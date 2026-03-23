/*
 * Copyright (c) @CompanyNameMagicTag. 2022. All rights reserved.
 */

#ifndef LV_PORT_DISP_H
#define LV_PORT_DISP_H

#include <stdint.h>
#include "misc/lv_area.h"
#include "misc/lv_color.h"
#include "misc/lv_style.h"
#include "hal/lv_hal_disp.h"

#ifdef __cplusplus
extern "C" {
#endif

void LvPortDispInit(lv_coord_t width, lv_coord_t height);

void LvSetPxCb(struct _lv_disp_drv_t* dispDrv, uint8_t* buf, lv_coord_t bufW, lv_coord_t x, lv_coord_t y,
    lv_color_t color, lv_opa_t opa, lv_blend_mode_t blendMode);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_PORT_DISP_H */

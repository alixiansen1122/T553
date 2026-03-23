/*
 * Copyright (c) @CompanyNameMagicTag. 2022. All rights reserved.
 */

#ifndef LV_TOUCH_INJECTOR_H
#define LV_TOUCH_INJECTOR_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

void LvInitTouchInjector(void);
void LvTouchInjectorRead(lv_indev_drv_t* indevDrv, lv_indev_data_t* data);
void LvTouchInjectorPush(lv_point_t point, lv_indev_state_t state);
bool LvIsTouchInjectorEmpty(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
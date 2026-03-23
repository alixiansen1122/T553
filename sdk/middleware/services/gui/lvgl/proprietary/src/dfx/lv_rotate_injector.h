/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#ifndef LV_ROTATE_INJECTOR_H
#define LV_ROTATE_INJECTOR_H

#include "lvgl.h"
#include "lv_hal_indev.h"

#ifdef __cplusplus
extern "C" {
#endif

void LvInitRotateInjector(void);
void LvRotateInjectorRead(lv_indev_drv_t* indevDrv, lv_indev_data_t* data);
void LvRotateInjectorPush(int16_t rotate);
bool LvIsRotateInjectorEmpty(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
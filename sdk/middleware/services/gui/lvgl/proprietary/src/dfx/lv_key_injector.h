/*
 * Copyright (c) @CompanyNameMagicTag. 2022. All rights reserved.
 */

#ifndef LV_KEY_INJECTOR_H
#define LV_KEY_INJECTOR_H

#include <stdint.h>
#include "hal/lv_hal_indev.h"

#ifdef __cplusplus
extern "C" {
#endif

void LvInitKeyInjector(void);
void LvKeyInjectorRead(lv_indev_drv_t* indevDrv, lv_indev_data_t* data);
void LvKeyInjectorPush(uint32_t key, lv_indev_state_t state);
bool LvIsKeyInjectorEmpty(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
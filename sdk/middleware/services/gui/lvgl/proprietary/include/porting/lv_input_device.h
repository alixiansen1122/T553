/*
 * Copyright (c) @CompanyNameMagicTag. 2022. All rights reserved.
 */

#ifndef LV_INPUT_DEVICE_H
#define LV_INPUT_DEVICE_H

#include <stdbool.h>
#include "lv_hal_indev.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Call this func first to prepare input devices, before use any other funcs. */
bool LvSetUpInputDevices(void);

/* Open touch input device */
bool LvOpenTouchDev(void);

/* Touch device read function */
void LvTouchDevRead(lv_indev_drv_t* indevDrv, lv_indev_data_t* data);

/* Open key input device */
bool LvOpenKeyDev(void);

/* Key device read function */
void LvKeyDevRead(lv_indev_drv_t* indevDrv, lv_indev_data_t* data);

/* Open rotate input device */
bool LvOpenRotateDev(void);

/* Rotate device read function */
void LvRotateDevRead(lv_indev_drv_t* indevDrv, lv_indev_data_t* data);

/* Check whether touch event is empty */
bool LvIsTouchEventEmpty(void);

/* Check whether key event is empty */
bool LvIsKeyEventEmpty(void);

/* Check whether rotate event empty */
bool LvIsRotateEventEmpty(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
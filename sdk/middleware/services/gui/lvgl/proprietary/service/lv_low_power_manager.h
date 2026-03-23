/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#ifndef LV_LOW_POWER_MANAGER_H
#define LV_LOW_POWER_MANAGER_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

#if LV_USE_LOW_POWER_MANAGER

/**
 * @brief  Try to enter low power mode if satisfy certain conditions.
 */
void TryToEnterLowPower(void);

/**
 * @brief  Exit low power mode.
 */
void ExitLowPower(void);

#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
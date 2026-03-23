/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#ifndef LV_DEMO_RAINBOW_H
#define LV_DEMO_RAINBOW_H

#include "lv_conf.h"
#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

#if LV_USE_DEMO_RAINBOW

void LvDemoRainbow(lv_obj_t* par);
void LvRegisterDemoRainbow(void);

#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
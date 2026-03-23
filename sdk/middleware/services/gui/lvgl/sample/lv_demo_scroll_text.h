/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#ifndef LV_DEMO_SCROLL_TEXT_H
#define LV_DEMO_SCROLL_TEXT_H

#include "lv_conf.h"
#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

#if LV_USE_DEMO_SCROLL_TEXT

void LvDemoScrollText(lv_obj_t* par);
void LvRegisterDemoScrollText(void);

#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */
#ifndef LV_DEMO_RECT_H
#define LV_DEMO_RECT_H

#include "lv_conf.h"
#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

#if LV_USE_DEMO_RECT

void LvDemoRect(lv_obj_t* par);
void LvRegisterDemoRect(void);

#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
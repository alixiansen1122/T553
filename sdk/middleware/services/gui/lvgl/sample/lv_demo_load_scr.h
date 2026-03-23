/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#ifndef LV_DEMO_LOAD_SCR_H
#define LV_DEMO_LOAD_SCR_H

#include "lv_conf.h"
#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

#if LV_USE_DEMO_LOAD_SCR

void LvDemoLoadScr(lv_obj_t* par);
void LvRegisterDemoLoadScr(void);

#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
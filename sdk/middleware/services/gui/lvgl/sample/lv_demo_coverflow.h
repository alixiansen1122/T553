/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#ifndef LV_DEMO_COVERFLOW_H
#define LV_DEMO_COVERFLOW_H

#include "lv_conf.h"
#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

#if LV_USE_DEMO_COVERFLOW

void LvDemoCoverFlow(lv_obj_t* par);
void LvRegisterDemoCoverFlow(void);

#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#ifndef LV_DEMO_IMG_H
#define LV_DEMO_IMG_H

#include "lv_conf.h"
#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

#if LV_USE_DEMO_IMG

void LvDemoImg(lv_obj_t* par);
void LvRegisterDemoImg(void);

#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
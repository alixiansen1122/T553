/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#ifndef LV_DEMO_IMG_CACHE_MANAGER_H
#define LV_DEMO_IMG_CACHE_MANAGER_H

#include "lv_conf.h"
#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

#if LV_USE_DEMO_IMG_CACHE_MANAGER

void LvDemoImgCacheManager(lv_obj_t* par);
void LvRegisterDemoImgCacheManager(void);

#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
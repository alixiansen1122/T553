/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#ifndef LV_DEMO_IMG_EXT_H
#define LV_DEMO_IMG_EXT_H

#include "lv_conf.h"
#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

#if LV_USE_DEMO_IMG_EXT

void LvDemoImgExt(lv_obj_t* par);
void LvRegisterDemoImgExt(void);

#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
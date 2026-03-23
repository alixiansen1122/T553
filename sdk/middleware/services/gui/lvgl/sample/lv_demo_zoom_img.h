/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#ifndef LV_DEMO_ZOOM_IMG_H
#define LV_DEMO_ZOOM_IMG_H

#include "lv_conf.h"
#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

#if LV_USE_DEMO_ZOOM_IMG

void LvDemoZoomImg(lv_obj_t* par);
void LvRegisterDemoZoomImg(void);

#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
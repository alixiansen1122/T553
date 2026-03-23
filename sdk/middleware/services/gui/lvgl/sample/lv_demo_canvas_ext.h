/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#ifndef LV_DEMO_CANVAS_EXT_H
#define LV_DEMO_CANVAS_EXT_H

#include "lv_conf.h"
#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

#if LV_USE_DEMO_CANVAS_EXT

void LvDemoCanvasExt(lv_obj_t* par);
void LvRegisterDemoCanvasExt(void);

#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
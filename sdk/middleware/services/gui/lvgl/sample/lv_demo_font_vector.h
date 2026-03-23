/*
 * Copyright (c) @CompanyNameMagicTag. 2022. All rights reserved.
 */

#ifndef LV_DEMO_FONT_VECTOR_H
#define LV_DEMO_FONT_VECTOR_H

#include "lv_conf.h"
#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

#if LV_USE_DEMO_FONT_VECTOR

void LvDemoFontVector(lv_obj_t* par);
void LvRegisterDemoFontVector(void);

#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
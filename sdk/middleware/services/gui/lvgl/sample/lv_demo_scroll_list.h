/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#ifndef LV_DEMO_SCROLL_LIST_H
#define LV_DEMO_SCROLL_LIST_H

#include "lv_conf.h"
#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

#if LV_USE_DEMO_SCROLL_LIST

void LvDemoScrollList(lv_obj_t* par);
void LvRegisterDemoScrollList(void);

#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
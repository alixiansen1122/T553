/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#ifndef ACTIVITY_H
#define ACTIVITY_H

#include "lv_conf.h"
#include "lvgl.h"
#include "lv_cross_view.h"
#if LV_USE_DEMO_WATCH

void LvglCreateActivity(lv_obj_t* parent);
LvPageInfo* GetActivityPageInfo(void);
#endif
#endif // ACTIVITY_H

/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#ifndef WATCH_H
#define WATCH_H

#include "lv_conf.h"
#include "lvgl.h"
#include "lv_cross_view.h"
#if LV_USE_DEMO_WATCH

void LvglCreateClock(lv_obj_t* subMenu);
LvPageInfo* GetClockPageInfo(void);
#endif
#endif // WATCH_H
/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#ifndef CONTROL_H
#define CONTROL_H

#include "lv_conf.h"
#include "lvgl.h"
#include "lv_cross_view.h"
#if LV_USE_DEMO_WATCH

void LvglCreateControl(lv_obj_t* subMenu);
LvPageInfo* GetControlPageInfo(void);
#endif
#endif // CONTROL_H

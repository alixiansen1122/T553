/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#ifndef APP_H
#define APP_H

#include "lv_conf.h"
#include "lvgl.h"

#if LV_USE_DEMO_WATCH

lv_obj_t* LvglPageMenuStart(lv_obj_t* parent);
void SetBtnClickable(bool isClickable);

#endif
#endif // APP_H

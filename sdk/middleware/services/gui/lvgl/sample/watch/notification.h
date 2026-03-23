/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include "lv_conf.h"
#include "lvgl.h"
#include "lv_cross_view.h"
#if LV_USE_DEMO_WATCH

void LvglCreateNotification(lv_obj_t* subMenu);
LvPageInfo* GetNotificationPageInfo(void);
#endif
#endif // NOTIFICATION_H

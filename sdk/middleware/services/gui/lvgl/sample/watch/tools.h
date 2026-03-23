/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#ifndef TOOLS_H
#define TOOLS_H

#include "lv_conf.h"
#include "lvgl.h"
#include "lv_cross_view.h"
#if LV_USE_DEMO_WATCH

void LvglCreateTools(lv_obj_t* parent);
LvPageInfo* GetToolsPageInfo(void);
#endif
#endif // TOOLS_H

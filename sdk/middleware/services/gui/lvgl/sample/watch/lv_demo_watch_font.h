/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#ifndef LV_DEMO_WATCH_FONT_H
#define LV_DEMO_WATCH_FONT_H

#include "lv_conf.h"
#if LV_USE_FONT_VECTOR
#include "lv_font_loader_ext.h"
#endif
#include "src/font/lv_font.h"

#if LV_USE_DEMO_WATCH

void InitLvFontRes();
void DeinitLvFontRes();
lv_font_t* GetLvFont(uint8_t size);

#endif
#endif
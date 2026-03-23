/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */
#ifndef LV_PAGE_FLIP_EFFECT_H
#define LV_PAGE_FLIP_EFFECT_H

#include "lvgl.h"
#include "lv_cross_view.h"
#ifdef __cplusplus
extern "C" {
#endif

void LvPageFlipScrollEffectInit(void);
void LvPageFlipScrollEffectDeinit(void);
ScrollEffectFunc LvGetPageFlipScrollEffect(void);

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif // LV_PAGE_FLIP_EFFECT_H
/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */
#ifndef LV_TURN_PAGE_EFFECT_H
#define LV_TURN_PAGE_EFFECT_H

#include "lvgl.h"
#include "lv_cross_view.h"
#ifdef __cplusplus
extern "C" {
#endif

void LvTurnPageScrollEffectInit(void);
void LvTurnPageScrollEffectDeinit(void);
ScrollEffectFunc LvGetTurnPageScrollEffect(void);

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif // LV_TURN_PAGE_EFFECT_H
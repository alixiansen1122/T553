/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */
#ifndef LV_ZOOM_EFFECT_H
#define LV_ZOOM_EFFECT_H

#include "lvgl.h"
#include "lv_cross_view.h"
#ifdef __cplusplus
extern "C" {
#endif

ScrollEffectFunc LvGetZoomScrollEffect(void);

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif // LV_ZOOM_EFFECT_H
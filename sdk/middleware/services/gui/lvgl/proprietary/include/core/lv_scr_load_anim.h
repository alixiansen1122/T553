/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#ifndef LV_SCR_LOAD_ANIM_H
#define LV_SCR_LOAD_ANIM_H

#include "lvgl.h"
#include "lv_conf_ext.h"

#if LV_USE_SCR_LOAD_ANIM

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    LV_SCR_LOAD_ANIM_OVER_SCALE,
    LV_SCR_LOAD_ANIM_OUT_SCALE,
    LV_SCR_LOAD_ANIM_FLIP,
    LV_SCR_LOAD_ANIM_MAX,
} LvScrLoadAnimType;

/**
 * Switch screen with animation.
 * @param newScr pointer to the new screen to load.
 * @param animType type of the animation (see 'LvScrLoadAnimType' enum).
 * @param time time of the animation.
 * @param delay delay before the transition.
 * @param auto_del true: automatically delete the old screen.
 * @param userData When animType is LV_SCR_LOAD_ANIM_OVER_SCALE or LV_SCR_LOAD_ANIM_OUT_SCALE,
                   it should be pointer to a lv_area_t struct, and scr will scale from or scale to this area.
                   When animType is LV_SCR_LOAD_ANIM_FLIP, it is not used, and it should be NULL.
 */
void LvScrLoadAnimExt(lv_obj_t* newScr, LvScrLoadAnimType animType, uint32_t time,
    uint32_t delay, bool autoDel, void* userData);

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif
#endif /* LV_SCR_LOAD_ANIM_H */

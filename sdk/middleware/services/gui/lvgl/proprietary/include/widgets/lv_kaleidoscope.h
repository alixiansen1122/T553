/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#ifndef LV_KALEIDOSCOPE_H
#define LV_KALEIDOSCOPE_H

#include "lvgl.h"
#include "lv_conf.h"
#include "lv_kaleidoscope_utils.h"
#include "lv_obj_class.h"

#if LV_USE_KALEIDOSCOPE

#ifdef __cplusplus
extern "C" {
#endif

extern const lv_obj_class_t lv_kaleidoscope_class;

typedef struct {
    lv_obj_t base;
    LvKaleidoscopeInfo info;
    int16_t rotate;
    uint32_t resId;
} LvKaleidoscope;

/**
 * @brief  Create kaleidoscope obj.
           This obj will directly modify the act buffer.
           Its size must be as same as the disp's size.
 */
lv_obj_t* LvKaleidoscopeCreate(lv_obj_t* parent);

/**
 * @brief  Set src of the kaleidoscope obj. ResId is a valid num returned by LvLoadImg API.
 */
bool LvKaleidoscopeSetSrc(lv_obj_t* obj, uint32_t resId);

/**
 * @brief  Update the obj by a rotate value. Currently, only the sign of the rotate number matters.
 */
void LvKaleidoscopeRotate(lv_obj_t* obj, int16_t rotate);

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif
#endif /* LV_KALEIDOSCOPE_H */

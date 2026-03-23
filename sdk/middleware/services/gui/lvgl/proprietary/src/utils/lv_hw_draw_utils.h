/*
 * Copyright (c) @CompanyNameMagicTag. 2022. All rights reserved.
 */

#ifndef LV_HW_DRAW_UTILS_H
#define LV_HW_DRAW_UTILS_H

#include "lv_conf.h"
#include "misc/lv_area.h"
#include "lv_math_ext.h"

#if LV_USE_CANVAS_EXT

#ifdef __cplusplus
extern "C" {
#endif

#define LV_ARC_TO_BEZIER_90_DEG 0.551915024494f
#define LV_RAD_90_DEG (90 / LV_RADIAN_TO_ANGLE)

typedef struct {
    lv_point_t center;
    uint16_t radius;
    int16_t startAngle;
    int16_t endAngle;
} LvArcInfo;

bool LvObtainArcPath(const LvArcInfo* arc, uint32_t* cmdNum, uint8_t** cmd,
    uint32_t* dataNum, float** data);

void GetArcControlPoint(float* arcData, const lv_point_t* center, float mangicNum, int8_t neg);

bool LvObtainCirclePath(const LvArcInfo* arc, uint32_t* cmdNum, uint8_t** cmd,
    uint32_t* dataNum, float** data);

void LvObtain30DegSectorPath(const LvArcInfo* arc, uint8_t* cmd, float* data);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // LV_USE_CANVAS_EXT

#endif // LV_HW_DRAW_UTILS_H
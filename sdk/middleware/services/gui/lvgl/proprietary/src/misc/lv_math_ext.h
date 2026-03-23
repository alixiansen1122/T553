/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#ifndef LV_MATH_EXT_H
#define LV_MATH_EXT_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

static const float LV_PI = 3.1415926f;
static const float LV_RADIAN_TO_ANGLE = 57.295779513f;

bool LvFloatEqual(float a, float b);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
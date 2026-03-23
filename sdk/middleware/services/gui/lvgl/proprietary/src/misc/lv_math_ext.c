/*
 * Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
 */

#include "lv_math_ext.h"
#include "lv_math.h"

bool LvFloatEqual(float a, float b)
{
    if (LV_ABS(a - b) < 0.00001f) {
        return true;
    }
    return false;
}
/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Graphic Math Ext
 * Author: Hisi Graphic Team
 * Created: 2025-5
 */

#ifndef GRAPHIC_MATH_EXT_H
#define GRAPHIC_MATH_EXT_H

#include <algorithm>
#include <cmath>
#include "gfx_utils/heap_base.h"
#include "gfx_utils/graphic_math.h"

namespace OHOS {

static constexpr float MATH_PI = 3.14159265358979323846f;
static constexpr float BEZIER_EPSILON = 1e-2f;
static constexpr float ANT2_COEFF1 = -0.0464964749f;
static constexpr float ANT2_COEFF2 = 0.15931422f;
static constexpr float ANT2_COEFF3 = 0.327622764f;
static constexpr float ANT2_COEFF4 = 1.57079637f;
static constexpr float ANT2_COEFF5 = 3.14159274f;
static constexpr float CONST_COEFF_1 = 1.0f;
static constexpr float CONST_COEFF_2 = 2.0f;
static constexpr float CONST_COEFF_3 = 3.0f;
static constexpr float CONST_COEFF_6 = 6.0f;
static constexpr float RED2DEG_COEFF = 180.0f;
const static float DISTANCE_COEFF = 0.375f;

template <typename T>
inline T Lerp(const T &start, const T &end, float t)
{
    return static_cast<T>(start + (end - start) * t);
}

template<typename T>
uint32_t Bsearch(T* frames, float frameNo)
{
    int32_t low = 0;
    int32_t high = int32_t(frames->Size()) - 1;

    while (low <= high) {
        auto mid = low + (high - low) / 2;
        auto frame = frames->Begin()[mid];
        if (frameNo < frame->time) high = mid - 1;
        else low = mid + 1;
    }
    if (high < low) low = high;
    if (low < 0) low = 0;
    return low;
}

inline void Clamp(float& value, float min, float max)
{
    if (FloatLess(value, min)) {
        value = min;
    } else if (FloatMore(value, max)) {
        value = max;
    }
}

inline float ConstA(float aA1, float aA2)
{
    return CONST_COEFF_1 - CONST_COEFF_3 * aA2 + CONST_COEFF_3 * aA1;
}
inline float ConstB(float aA1, float aA2)
{
    return CONST_COEFF_3 * aA2 - CONST_COEFF_6 * aA1;
}
inline float ConstC(float aA1)
{
    return CONST_COEFF_3 * aA1;
}

inline float GetSlope(float t, float aA1, float aA2)
{
    return CONST_COEFF_3 * ConstA(aA1, aA2) * t * t + CONST_COEFF_2 * ConstB(aA1, aA2) * t + ConstC(aA1);
}

inline float CalcBezier(float t, float aA1, float aA2)
{
    return ((ConstA(aA1, aA2) * t + ConstB(aA1, aA2)) * t + ConstC(aA1)) * t;
}

inline float Rad2deg(float radian)
{
    return radian * (RED2DEG_COEFF / MATH_PI);
}

inline float Distance(const Vector2<float>& pt1, const Vector2<float>& pt2, bool fast = true)
{
    Vector2<float> diff = pt2 - pt1;
    if (fast) {
        diff.x_ = (diff.x_ < 0 ? -diff.x_ : diff.x_);
        diff.y_ = (diff.y_ < 0 ? -diff.y_ : diff.y_);
        return (diff.x_ > diff.y_) ? (diff.x_ + diff.y_ * DISTANCE_COEFF) : (diff.y_ + diff.x_ * DISTANCE_COEFF);
    }
    return sqrtf(diff.x_ * diff.x_ + diff.y_ * diff.y_);
}

inline float Atan2(float y, float x)
{
    if (y == 0.0f && x == 0.0f) return 0.0f;
    auto a = std::min(fabsf(x), fabsf(y)) / std::max(fabsf(x), fabsf(y));
    auto s = a * a;
    auto r = ((ANT2_COEFF1 * s + ANT2_COEFF2) * s - ANT2_COEFF3) * s * a + a;
    if (fabsf(y) > fabsf(x)) r = ANT2_COEFF4 - r;
    if (x < 0) r = ANT2_COEFF5 - r;
    if (y < 0) return -r;
    return r;
}

} // namespace OHOS
#endif // GRAPHIC_MATH_EXT_H
/**
 * @}
 */

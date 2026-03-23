/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Graphic Lottie Bezier
 * Author: Hisi Graphic Team
 * Created: 2025-5
 */

#ifndef LOTT_BEZIER_H
#define LOTT_BEZIER_H
#include "gfx_utils/heap_base.h"
#include "gfx_utils/graphic_math.h"

namespace OHOS {
const static int INITIAL_COUNT = 4; // cache to speed up
class LottBezier : public HeapBase {
public:
    LottBezier() = default;
    LottBezier(Vector2<float> start, Vector2<float> ctrl1, Vector2<float> ctrl2, Vector2<float> end);
    void Split(float t, LottBezier& leftBz);
    void Split(LottBezier& leftBz, LottBezier& rightBz);
    // length should be the actual bezier length if not use default value
    void Split(float startLength, float endLength, LottBezier& clipBz, float length = 0.0f);
    float Length(bool fast = true);
    // length should be the actual bezier length if not use default value
    float TAtLength(float subLength, float length = 0.0f, bool fast = true);
    Vector2<float> PointAt(float t) const;
    float Angle(float t) const;

    Vector2<float> start_{};
    Vector2<float> ctrl1_{};
    Vector2<float> ctrl2_{};
    Vector2<float> end_{};
    float length_{0.0f};
};
} // namespace OHOS
#endif // LOTT_BEZIER_H
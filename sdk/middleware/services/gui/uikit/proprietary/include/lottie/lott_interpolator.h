/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: LottInterpolator
 * Author: Hisi Graphic Team
 * Created: 2025-5
 */

#ifndef LOTT_INTERPOLATOR_H
#define LOTT_INTERPOLATOR_H

#include <math.h>
#include "gfx_utils/graphic_math.h"
#include "gfx_utils/heap_base.h"
#include "gfx_utils/vector.h"
#include "lottie/lott_math.h"

namespace OHOS {

static constexpr int32_t SPLINE_TABLE_SIZE =  11;
static constexpr float NEWTON_MIN_SLOPE =  0.02f;
static constexpr int32_t NEWTON_ITERATIONS =  4;
static constexpr float SUBDIVISION_PRECISION =  0.0000001f;
static constexpr int32_t SUBDIVISION_MAX_ITERATIONS =  10;

class LottInterpolator : public HeapBase {
public:
    LottInterpolator() {}
    float Progress(float time);
    void SetTangent(Vector2<float>& inTangent, Vector2<float>& outTangent);

    Vector2<float> inTangent;
    Vector2<float> outTangent;

private:
    static constexpr float SAMPLE_STEP_SIZE = 1.0f / float(SPLINE_TABLE_SIZE - 1);
    float samples[SPLINE_TABLE_SIZE];

    float GetTForX(float aX);
    float BinarySubdivide(float aX, float aA, float aB);
    float NewtonRaphsonIterate(float aX, float aGuessT);
};

class LottInterpolatorFactory : public HeapBase {
public:
    LottInterpolatorFactory() {}
    static LottInterpolatorFactory& GetInstance(void);
    LottInterpolator* GetInterpolator(Vector2<float>& in, Vector2<float>& out);
    void ClearAll();

    Graphic::Vector<LottInterpolator*> interpolators_;
};

}

#endif  // LOTT_INTERPOLATOR_H


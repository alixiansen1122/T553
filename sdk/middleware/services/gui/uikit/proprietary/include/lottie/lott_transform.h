/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: Lottie
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */

#ifndef LOTT_TRANSFORM_H
#define LOTT_TRANSFORM_H

#include "lottie/lott_property.h"
#include "display_vgu.h"
#include "gfx_utils/graphic_math.h"
#include "gfx_utils/heap_base.h"

namespace OHOS {
class LottTransform : public HeapBase {
public:
    LottTransform();
    ~LottTransform();

    struct LottSplitPosition {
        LottFloatProp x;
        LottFloatProp y;
    };

    LottSplitPosition* splitPosition_ = nullptr;
    LottPositionProp position_;
    LottFloatProp rotation_;
    LottFloatProp skewAngle_;
    LottFloatProp skewAxis_;
    LottVectorProp scale_;
    LottVectorProp anchor_;
    LottIntegerProp opa_;

    void Update(float frameNo, uint8_t& opa, Matrix3<float>& mat);
    Matrix3<float> GetTransformMatrix();
};
}
#endif // LOTT_TRANSFORM_H
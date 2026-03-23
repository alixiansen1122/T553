/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: Lottie
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */

#ifndef LOTT_RECT_H
#define LOTT_RECT_H

#include "lottie/lott_shape.h"
#include "lottie/lott_property.h"
#include "lottie/lott_shape_group.h"

namespace OHOS {
class LottRect : public LottShape {
public:
    LottRect();
    virtual ~LottRect();

    bool Update(float frameNo) override;
    bool GetPathData(Path& path) override;

    LottPositionProp position_;
    LottVectorProp size_;
    LottFloatProp radius_;

private:
    bool GetRectPathWithoutRadius(float x, float y, float width, float height);
};
}
#endif // LOTT_RECT_H
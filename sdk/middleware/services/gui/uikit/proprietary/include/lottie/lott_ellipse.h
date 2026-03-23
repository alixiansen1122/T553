/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: Lottie
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */

#ifndef LOTT_ELLIPSE
#define LOTT_ELLIPSE

#include "lottie/lott_shape.h"
#include "lottie/lott_property.h"

namespace OHOS {

class LottEllipse : public LottShape {
public:
    LottEllipse();
    virtual ~LottEllipse();

    bool Update(float frameNo) override;
    bool GetPathData(Path& path) override;

    LottPositionProp position_;
    LottVectorProp size_;
};
}
#endif // LOTT_ELLIPSE
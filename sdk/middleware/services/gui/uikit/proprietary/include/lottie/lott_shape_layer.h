/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: Lottie
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */

#ifndef LOTT_SHAPE_LAYER_H
#define LOTT_SHAPE_LAYER_H

#include "lottie/lott_layer.h"
#include "lottie/lott_shape_group.h"

namespace OHOS {

class LottShapeLayer : public LottLayer {
public:
    LottShapeLayer();
    virtual ~LottShapeLayer();

    void OnStop() override;
    bool UpdateLottLayer(float frameNo) override;
    void DrawLottLayer(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea,
        const Matrix3<float>& matrix, uint8_t opa) override;

    Graphic::Vector<LottShapeGroup*> shapeGroups_;
};
}
#endif // LOTT_SHAPE_LAYER_H
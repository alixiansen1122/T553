/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: Lottie
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */

#ifndef LOTT_SOLID_LAYER_H
#define LOTT_SOLID_LAYER_H

#include "lottie/lott_layer.h"
#include "common/graphic_hardware_types.h"

namespace OHOS {

class LottSolidLayer : public LottLayer {
public:
    LottSolidLayer();
    virtual ~LottSolidLayer();

    void DrawLottLayer(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea,
        const Matrix3<float>& matrix, uint8_t opa) override;

    uint32_t color_ = 0;
    Path path_ = {0};
};
}
#endif // LOTT_SOLID_LAYER_H
/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: Lottie
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */

#ifndef LOTT_PRECOMP_LAYER_H
#define LOTT_PRECOMP_LAYER_H

#include "lottie/lott_layer.h"
namespace OHOS {

class LottPrecompLayer : public LottLayer {
public:
    LottPrecompLayer();
    virtual ~LottPrecompLayer();

    bool UpdateLottLayer(float frameNo) override;
    void DrawLottLayer(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea,
        const Matrix3<float>& matrix, uint8_t opa) override;
    void OnStop() override;
    Graphic::Vector<LottLayer*> layers_;
};
}
#endif // LOTT_PRECOMP_LAYER_H
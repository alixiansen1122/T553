/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: Lottie
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */

#ifndef LOTT_IMAGE_LAYER_H
#define LOTT_IMAGE_LAYER_H

#include "lottie/lott_layer.h"
#include "gfx_utils/image_info.h"

namespace OHOS {

class LottImageLayer : public LottLayer {
public:
    LottImageLayer();
    virtual ~LottImageLayer();

    bool UpdateLottLayer(float frameNo) override;
    void DrawLottLayer(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea,
        const Matrix3<float>& matrix, uint8_t opa) override;

    uint32_t resId_ = 0;
    uint32_t offset_ = 0;
    std::string filePath_;
    ImageInfo* imgInfo_ = nullptr;
};
}
#endif // LOTT_IMAGE_LAYER_H
/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: Lottie
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */

#ifndef LOTT_COMP_H
#define LOTT_COMP_H

#include <string>
#include "gfx_utils/vector.h"
#include "gfx_utils/graphic_buffer.h"
#include "lottie/lott_layer.h"

namespace OHOS {
class LottComp : public HeapBase {
public:
    LottComp();
    virtual ~LottComp();
    void UpdateLottComp(float frameNo);
    void DrawLottComp(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea, const Matrix3<float>& mat, uint8_t opa);
    float GetFrameCnt();
    void OnStop();

    float width_ = 0.0f;
    float height_ = 0.0f;
    float frameRate_ = 0.0f;
    float inFrame_ = 0.0f;
    float outFrame_ = 0.0f;
    std::string version_;
    std::string name_;
    Graphic::Vector<LottLayer*> layers_;
};
}
#endif // LOTT_COMP_H
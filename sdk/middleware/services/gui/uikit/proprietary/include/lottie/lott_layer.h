/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: Lottie
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */

#ifndef LOTT_LAYER_H
#define LOTT_LAYER_H

#include <string>
#include "lottie/lott_paint.h"
#include "lottie/lott_property.h"
#include "lottie/lott_transform.h"
#include "engines/gfx/gfx_engine_manager.h"
#include "gfx_utils/rect.h"
#include "gfx_utils/graphic_buffer.h"

namespace OHOS {
enum class LottLayerType {
    PRECOMP_LAYER = 0,
    SOLID_LAYER,
    IMAGE_LAYER,
    NULL_LAYER,
    SHAPE_LAYER,
    TEXT_LAYER,
};

class LottLayer : public HeapBase {
public:
    LottLayer();
    virtual ~LottLayer();
    virtual void OnStop() {};
    virtual bool UpdateLottLayer(float frameNo);
    virtual void DrawLottLayer(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea,
        const Matrix3<float>& matrix, uint8_t opa) {};
    LottLayerType GetType();

    float width_ = 0.0f;
    float height_ = 0.0f;
    float inFrame_ = 0.0f;
    float outFrame_ = 0.0f;
    float startFrame_ = 0.0f;
    int16_t id_ = -1;
    int16_t pId_ = -1;
    LottLayer* parent_ = nullptr;

    uint8_t opa_ = LOTT_OPA_MAX;
    Matrix3<float> matrix_;

    std::string name_;
    BlendMode blendMode_ = BlendMode::BLEND_MODE;
    LottTransform* transform_ = nullptr;

protected:
    void ResetUpdateState();
    bool isUpdated_ = false;
    bool isValidFrame_ = false;
    LottLayerType type_ = LottLayerType::NULL_LAYER;
};
}
#endif // LOTT_LAYER_H
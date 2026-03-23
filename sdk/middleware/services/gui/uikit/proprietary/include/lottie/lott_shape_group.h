/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: Lottie
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */

#ifndef LOTT_SHAPE_GROUP_H
#define LOTT_SHAPE_GROUP_H

#include "lottie/lott_shape.h"
#include "lottie/lott_paint.h"
#include "lottie/lott_transform.h"
#include "lottie/lott_trim.h"
#include "common/graphic_hardware_types.h"
#include "gfx_utils/graphic_types.h"
#include "display_vgu.h"

namespace OHOS {

struct LottRenderContext {
    uint8_t opa;
    Matrix3<float> matrix;
    LottPaintValue paint;
    LottTrimValue trim;

    void DeepCopy(LottRenderContext& ctx)
    {
        opa = ctx.opa;
        matrix = ctx.matrix;
        paint.DeepCopy(ctx.paint);
        trim = ctx.trim;
    }
};

class LottShapeGroup : public LottShape {
public:
    LottShapeGroup();
    virtual ~LottShapeGroup();

    void OnStop() override;
    bool Update(float frameNo) override;
    void DrawLottShape(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea, LottRenderContext& ctx);
    void Prepare();

    Graphic::Vector<LottShape*> shapes_;
    LottTransform* transform_ = nullptr;
    Matrix3<float> matrix_;
    bool mergeable_ = false;
    LottPaint* paint_ = nullptr;
    LottTrim* trim_ = nullptr;

private:
    void DrawMerged(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea, LottRenderContext& ctx);
    void DrawSeparateShapes(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea, LottRenderContext& ctx);
    bool UpdatePathInner();
    Graphic::Vector<float> datas_;
    Graphic::Vector<uint8_t> cmds_;
};
}
#endif // LOTT_SHAPE_GROUP_H
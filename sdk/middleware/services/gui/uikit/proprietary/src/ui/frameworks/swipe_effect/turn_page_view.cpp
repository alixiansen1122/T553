/*
 * Copyright (c) CompanyNameMagicTag 2024. All rights reserved.
 * Description: TurnPageView
 * Author:
 * Create: 2024-6
 */

#include "turn_page_view.h"
#include "imgdecode/image_load.h"
#include "engines/gfx/lite_m_gfx_engine.h"

namespace OHOS {
void TurnPageView::SetTurnPageInfo(ImageInfo* leftCard, ImageInfo* rightCard, float angle)
{
    if (leftCard == nullptr || rightCard == nullptr) {
        leftInfo_ = nullptr;
        rightInfo_ = nullptr;
        return;
    }

    int16_t height = GetHeight();
    int16_t width = GetWidth();
    if (height != leftCard->header.height || width != leftCard->header.width
        || height != rightCard->header.height || width != rightCard->header.width) {
        GRAPHIC_LOGE("image shape not coninsides with the view rect!");
        leftInfo_ = nullptr;
        rightInfo_ = nullptr;
        return;
    }
    int16_t middleWidth = width / 2; // 2: half
    int16_t middleHeight = height / 2; // 2: half
    Rect origRect = GetOrigRect();
    leftInfo_ = leftCard;
    rightInfo_ = rightCard;
    Rect rect;
    rect.SetWidth(middleWidth);
    rect.SetHeight(height);

    // ll
    rect.SetPosition(origRect.GetX(), origRect.GetY());
    transll_.SetTransMapRect(rect);
    // lr
    rect.SetPosition(origRect.GetX() + middleWidth - 1, origRect.GetY());
    translr_.SetTransMapRect(rect);
    SetCamera({0, middleWidth}, translr_);
    // rl
    rect.SetPosition(origRect.GetX(), origRect.GetY());
    transrl_.SetTransMapRect(rect);
    SetCamera({middleWidth, middleHeight}, transrl_);
    // rr
    rect.SetPosition(origRect.GetX() + middleWidth, origRect.GetY());
    transrr_.SetTransMapRect(rect);

    RotateImg(angle);
    UpdateInvalidArea();
}

void TurnPageView::OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea)
{
    DrawPartialRect(gfxDstBuffer, invalidatedArea, leftInfo_, transll_);
    DrawPartialRect(gfxDstBuffer, invalidatedArea, rightInfo_, transrr_);
    if (usingLeft_) {
        DrawPartialRect(gfxDstBuffer, invalidatedArea, leftInfo_, translr_);
    } else {
        DrawPartialRect(gfxDstBuffer, invalidatedArea, rightInfo_, transrl_);
    }
}

void TurnPageView::DrawPartialRect(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea,
    ImageInfo *info, const TransformMap &transform)
{
    uint8_t opa = GetMixOpaScale();
    if (info == nullptr) {
        return;
    }
    Rect dstRect(invalidatedArea);
    if (!dstRect.Intersect(dstRect, transform.GetBoxRect())) {
        return;
    }
    BufferInfo src = {{0, 0, info->header.width - 1, info->header.height - 1}};
    src.width = info->header.width;
    src.height = info->header.height;
    src.stride = static_cast<int32_t>(ImageLoad::GetAlignedStride(src.width,
        info->header.compressMode, info->header.colorMode));
    src.compressMode = info->header.compressMode;
    src.mode = static_cast<ColorMode>(info->header.colorMode);
    src.virAddr = reinterpret_cast<void*>(const_cast<uint8_t*>(info->data));
    src.phyAddr = reinterpret_cast<void*>(const_cast<uint8_t*>(info->data));

    BlendOption opt;
    opt.mode = BLEND_SRC_OVER;
    opt.opacity = opa;
    src.rect = transform.GetTransMapRect();

    Matrix4<float> matrix = transform.GetTransformMatrix();
    opt.transMap.SetMatrix(matrix);
    // adjust src rect using dstRect for only translate matrix
    if (FloatEqual(matrix[0][0], 1.0f) && FloatEqual(matrix[0][1], 0.0f) &&
        FloatEqual(matrix[0][3], 0.0f) && FloatEqual(matrix[1][0], 0.0f) && // 3: index
        FloatEqual(matrix[1][1], 1.0f) && FloatEqual(matrix[1][3], 0.0f) && // 3: index
        FloatEqual(matrix[3][3], 1.0f)) { // 3: index
        src.rect.SetWidth(dstRect.GetWidth());
        src.rect.SetHeight(dstRect.GetHeight());
    }
    BaseGfxEngine::GetInstance()->Blit(gfxDstBuffer, {0, 0}, src, dstRect, opt);
}

void TurnPageView::RotateImg(float angle)
{
    const float maxAngle = 180;
    const float rightAngle = 90;
    TransformMap *transform = angle < rightAngle ? &translr_ : &transrl_;
    float w = GetWidth();
    float x = angle < rightAngle ? 0 : w / 2; // 2: half
    angle = angle < rightAngle ? angle : angle - maxAngle;
    usingLeft_ = (transform == &translr_);
    transform->Rotate(angle, Vector3<float>{x, 0, 0}, Vector3<float>{x, 1, 0});
}

void TurnPageView::UpdateInvalidArea()
{
    Rect joinRect = transll_.GetBoxRect();
    joinRect.Join(joinRect, transrr_.GetBoxRect());
    joinRect.Join(joinRect, translr_.GetBoxRect());
    joinRect.Join(joinRect, transrl_.GetBoxRect());
    InvalidateRect(joinRect);
}
} // OHOS

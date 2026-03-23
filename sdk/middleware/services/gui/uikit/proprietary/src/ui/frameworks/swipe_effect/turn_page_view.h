/*
 * Copyright (c) CompanyNameMagicTag 2024. All rights reserved.
 * Description: TurnPageView
 * Author:
 * Create: 2024-6
 */

#ifndef UI_TURN_PAGE_VIEW_H
#define UI_TURN_PAGE_VIEW_H
#include "securec.h"
#include "components/ui_view.h"

namespace OHOS {
class TurnPageView : public UIView {
public:
    TurnPageView()
    {
    }
    ~TurnPageView() override {}
    TurnPageView &operator=(const TurnPageView&) = delete;
    TurnPageView(const TurnPageView&) = delete;
    void SetTurnPageInfo(ImageInfo* leftCard, ImageInfo* rightCard, float angle);
    void OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea) override;

private:
    void DrawPartialRect(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea,
        ImageInfo *info, const TransformMap &transform);
    void SetCamera(Vector2<float> pos, TransformMap &transform)
    {
        constexpr int16_t CAMERA_ZPOS = -600;
        transform.SetCameraPosition(pos);
        transform.SetCameraDistance(CAMERA_ZPOS);
    }
    void RotateImg(float angle);
    void UpdateInvalidArea();
    ImageInfo *leftInfo_ = nullptr;
    ImageInfo *rightInfo_ = nullptr;
    bool usingLeft_ = false;
    TransformMap transll_;
    TransformMap translr_;
    TransformMap transrl_;
    TransformMap transrr_;
};
} // OHOS
#endif // UI_TURN_PAGE_VIEW_H
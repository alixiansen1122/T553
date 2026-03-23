/*
 * Copyright (c) @CompanyNameMagicTag. 2024. All rights reserved.
 */

#ifndef UI_TEST_HEXAGONS_LIST_H
#define UI_TEST_HEXAGONS_LIST_H

#include "ui_test.h"
#include "gfx_utils/list.h"
#include "components/ui_custom_hexagons_list.h"
#include "components/ui_button.h"
#include "components/ui_view.h"
#include "components/ui_transform_group.h"
#include "animator/animator.h"

namespace OHOS {
class UITestHexagonsList : public UITest, public UIView::OnClickListener, public UIView::OnLongPressListener {
public:
    UITestHexagonsList() {}
    ~UITestHexagonsList() override {}
    void SetUp() override;
    void TearDown() override;
    const UIView *GetTestView() override;
    bool OnClick(UIView& view, const ClickEvent& event) override;
    bool OnLongPress(UIView& view, const LongPressEvent& event) override;

private:
    void HandleClickEventForRectScreen(const UIView& view);
    void ConfigButtonAttr(UIView& view, int16_t x, int16_t y, int16_t width, int16_t height);
    void SetUpButtons(int16_t xEnd, int16_t yEnd, int16_t xMiddle, int16_t yMiddle);
    void SetUpChildren();
    UICustomHexagonsList* hexagonsList_ = nullptr;
    int16_t horMargin_ = 0;
    int16_t verMargin_ = 0;
    int16_t borderRadius_ = 0;
    UIViewGroup* container_ = nullptr;
    uint16_t imgSize_ = 0;
    uint16_t imgDistance_ = 0;
    float imgScaleFactor_ = 0;
    float globalScale_ = 0;
    bool isIncreaseImgSize_ = true;
    UIButton* modifyImgSizeButton_ = nullptr;
    bool isIncreaseImgDistance_ = true;
    UIButton* modifyImgDistanceButton_ = nullptr;
    bool isIncreaseScaleFactor_ = true;
    UIButton* modifyScaleFatorButton_ = nullptr;
    bool isIncreaseGlobalScale_ = true;
    UIButton* modifyGlobalScaleButton_ = nullptr;
    bool isIncreaseHorMargin_ = true;
    UIButton* modifyHorMarginButton_ = nullptr;
    bool isIncreaseVerMargin_ = true;
    UIButton* modifyVerMarginButton_ = nullptr;
    bool isIncreaseBorderRadius_ = true;
    UIButton* modifyBorderRadiusButton_ = nullptr;
    bool isImageMode_ = true;
    UIButton* modifyModeButton_ = nullptr;

    class TransformCallback : public AnimatorCallback {
    public:
        explicit TransformCallback(UITransformGroup* group, UIView* child) : transformGroup_(group),
            transformedChild_(child) {}
        ~TransformCallback() override {};
        void Callback(UIView* view) override
        {
            if (transformGroup_ == nullptr || transformedChild_ == nullptr) {
                return;
            }
            deg_ += 2.0f;
            if (FloatMore(deg_, 360.0f)) {
                deg_ = 0.0f;
            }
            TransformMap map;
            map.Rotate(deg_,
                Vector2<float>{transformedChild_->GetWidth() / 2.0f, transformedChild_->GetHeight() / 2.0f});
            transformGroup_->TransformChild(transformedChild_, map);
            transformGroup_->Invalidate();
        }
    private:
        UITransformGroup* transformGroup_;
        UIView* transformedChild_;
        float deg_ = 0.0f;
    };

    TransformCallback* callback_ = nullptr;
    Animator* animator_ = nullptr;
};
}
#endif // UI_TEST_HEXAGONS_LIST_H
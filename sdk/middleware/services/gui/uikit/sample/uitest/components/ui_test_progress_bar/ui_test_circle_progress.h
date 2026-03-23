/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: Lottie
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */

#ifndef UI_TEST_CIRCLE_PROGRESS_H
#define UI_TEST_CIRCLE_PROGRESS_H

#include "components/ui_circle_progress.h"
#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "ui_test.h"

namespace OHOS {
class UITestCircleProgress : public UITest, public UIView::OnClickListener {
public:
    UITestCircleProgress() {}
    ~UITestCircleProgress() {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;

    void SetUpButton(UILabelButton* btn, const char* title, int16_t x, int16_t y);
    void SetUpLabel(const char* title, int16_t x, int16_t y) const;
    void SetBtnEvent1(const UIView& view, uint16_t& progress, uint16_t& step, int16_t& min, int16_t& max);
    void SetBtnEvent2(const UIView& view, Point& center, Point& imgPos, int16_t& startAngle, int16_t& endAngle);
    void SetBtnEvent3(const UIView& view, int16_t& radius);
    void SetMoveBtnEvent(UIView& view);
    bool OnClick(UIView& view, const ClickEvent& event) override;

    void UIKitCircleProgressTestUICircleProgress001();
    void UIKitCircleProgressTestSetValue002();
    void UIKitCircleProgressTestSetImage003();
    void UIKitCircleProgressTestSetStyle004();
    void UIKitCircleProgressTestGetStyle005();
    void UIKitCircleProgressTestSetCapType006();
    void UIKitCircleProgressTestSetStep007();
    void UIKitCircleProgressTestSetRange008();
    void UIKitCircleProgressTestEnableBackground009();
    void UIKitCircleProgressTestSetCenter010();
    void UIKitCircleProgressTestSetRadius011();
    void UIKitCircleProgressTestSetLineWidth012();
    void UIKitCircleProgressTestSetImagePosition013();
    void UIKitCircleProgressTestSetLineColor014();
    void UIKitCircleProgressTestSetAngle015();

private:
    UIScrollView* container_ = nullptr;
    UIScrollView* scroll_ = nullptr;
    UICircleProgress* circleProgress_ = nullptr;
    UIViewGroup* uiViewGroupFrame_ = nullptr;

    UILabelButton* resetBtn_ = nullptr;
    UILabelButton* incProgressBtn_ = nullptr;
    UILabelButton* decProgressBtn_ = nullptr;
    UILabelButton* incMinProgressBtn_ = nullptr;
    UILabelButton* decMinProgressBtn_ = nullptr;
    UILabelButton* incMaxProgressBtn_ = nullptr;
    UILabelButton* decMaxProgressBtn_ = nullptr;
    UILabelButton* centerXBtn_ = nullptr;
    UILabelButton* centerYBtn_ = nullptr;
    UILabelButton* incRadiusBtn_ = nullptr;
    UILabelButton* decRadiusBtn_ = nullptr;
    UILabelButton* incWidthBtn_ = nullptr;
    UILabelButton* decWidthBtn_ = nullptr;
    UILabelButton* incStartAngleBtn_ = nullptr;
    UILabelButton* decStartAngleBtn_ = nullptr;
    UILabelButton* incEndAngleBtn_ = nullptr;
    UILabelButton* decEndAngleBtn_ = nullptr;
    UILabelButton* swapAngleBtn_ = nullptr;
    UILabelButton* stepBtn_ = nullptr;
    UILabelButton* imageBtn_ = nullptr;
    UILabelButton* noImageBtn_ = nullptr;
    UILabelButton* setStyleBtn_ = nullptr;
    UILabelButton* getStyleBtn_ = nullptr;
    UILabelButton* roundCapBtn_ = nullptr;
    UILabelButton* noneCapBtn_ = nullptr;
    UILabelButton* enableBgBtn_ = nullptr;
    UILabelButton* disableBgBtn_ = nullptr;
    UILabelButton* imgPosXBtn_ = nullptr;
    UILabelButton* imgPosYBtn_ = nullptr;
    UILabelButton* lineColorBtn_ = nullptr;
};
} // namespace OHOS
#endif // UI_TEST_CIRCLE_PROGRESS_H

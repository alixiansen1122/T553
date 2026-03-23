/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: Lottie
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */

#ifndef UI_TEST_UI_SWIPE_VIEW_H
#define UI_TEST_UI_SWIPE_VIEW_H

#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "components/ui_swipe_view.h"
#include "ui_test.h"

namespace OHOS {
class UITestUISwipeView : public UITest, UIView::OnClickListener {
public:
    UITestUISwipeView() {}
    ~UITestUISwipeView() {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;
    bool OnClick(UIView& view, const ClickEvent& event) override;

    void UIKitSwipeViewTestHorizontal001();
    void UIKitSwipeViewTestHorizontal002();
    void UIKitSwipeViewTestHorizontal003();
    void UIKitSwipeViewTestVer001();
    void UIKitSwipeViewTestVer002();
    void UIKitSwipeViewTestRemove001();
    void UIKitSwipeViewTestAlign001(UISwipeView::AlignMode alignMode);
    void UIKitSwipeViewTestSetCurrentPage();

private:
    void SetUpButton(UILabelButton* btn, const char* title);
    void SetLastPos(UIView* view);
    void InitBtn();
    UIScrollView* container_ = nullptr;
    UISwipeView* currentSwipe_ = nullptr;
    UILabelButton* addBtnInHead_ = nullptr;
    UILabelButton* addBtnInTail_ = nullptr;
    UILabelButton* addBtnInMid_ = nullptr;
    UILabelButton* removeHeadBtn_ = nullptr;
    UILabelButton* removeMidBtn_ = nullptr;
    UILabelButton* removeAllBtn_ = nullptr;
    UILabelButton* loopBtn_ = nullptr;
    UILabelButton* changePageBtn_ = nullptr;
    bool loop_ = false;
    int16_t lastX_ = 0;
    int16_t lastY_ = 0;
    int16_t btnNum_ = 0;
};
} // namespace OHOS
#endif // UI_TEST_UI_SWIPE_VIEW_H

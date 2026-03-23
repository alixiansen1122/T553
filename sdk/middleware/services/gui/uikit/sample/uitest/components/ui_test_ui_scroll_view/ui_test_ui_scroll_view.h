/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: Lottie
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */

#ifndef UI_TEST_UI_SCROLL_VIEW_H
#define UI_TEST_UI_SCROLL_VIEW_H

#include "components/ui_label.h"
#include "components/ui_scroll_view.h"
#include "ui_test.h"

namespace OHOS {
class UITestUIScrollView : public UITest {
public:
    UITestUIScrollView() {}
    ~UITestUIScrollView() {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;

    void UIKitUIScrollViewTestBar001();
    void UIKitUIScrollViewTestBar002();
    void UIKitUIScrollViewTestBar003();
    void UIKitUIScrollViewTestBar004();
    void UIKitUIScrollViewTestScrollableSet001();
    void UIKitUIScrollViewTestScrollableSet002();
    void UIKitUIScrollViewTestScrollBlankSet001();
    void UIKitUIScrollViewTestScrollListener001();

private:
    void SetLastPos(UIView* view);
    UIScrollView* container_ = nullptr;
    int16_t lastX_ = 0;
    int16_t lastY_ = 0;
};
} // namespace OHOS
#endif // UI_TEST_UI_SCROLL_VIEW_H

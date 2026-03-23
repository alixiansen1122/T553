/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: Lottie
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */

#ifndef UI_TEST_DRAW_LINE_H
#define UI_TEST_DRAW_LINE_H

#include "components/ui_canvas.h"
#include "components/ui_label.h"
#include "components/ui_scroll_view.h"
#include "components/ui_view_group.h"
#include "ui_test.h"

namespace OHOS {
class UITestDrawLine : public UITest {
public:
    UITestDrawLine() {}
    ~UITestDrawLine() {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;

    /**
     * @brief Test draw line Function
     */
    void UIKitDrawLineTestLine001();
    void UIKitDrawLineTestLine002();
    void UIKitDrawLineTestLine003();
    void UIKitDrawLineTestLine004();
    void UIKitDrawLineTestLine005();
    void UIKitDrawLineTestLine006();
    void UIKitDrawLineTestLine007();
    void UIKitDrawLineTestLine008();
    void UIKitDrawLineTestLine009();
    void UIKitDrawLineTestLine010();
    void UIKitDrawLineTestLine011();
    void UIKitDrawLineTestLine012();

private:
    UIViewGroup* CreateTestCaseGroup() const;
    UILabel* CreateTitleLabel() const;
    UICanvas* CreateCanvas() const;
    UIScrollView* container_ = nullptr;
};
} // namespace OHOS
#endif // UI_TEST_DRAW_LINE_H

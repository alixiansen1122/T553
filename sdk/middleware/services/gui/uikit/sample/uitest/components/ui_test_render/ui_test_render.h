/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: TestCase
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */

#ifndef UI_TEST_RENDER_H
#define UI_TEST_RENDER_H

#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "ui_test.h"

namespace OHOS {
constexpr char* UI_TEST_RENDER_ID = "Render";
constexpr char* UI_TEST_RENDER_UPDATA_BUTTON_ID_01 = "test_render_updata_buttin_01";

class UITestRender : public UITest, public UIView::OnClickListener {
public:
    UITestRender() : container_(nullptr) {}
    ~UITestRender() {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;
    bool OnClick(UIView& view, const ClickEvent& event) override;

    /**
     * @brief Test render Function
     */
    void UIKitRenderTestRender001();

    /**
     * @brief Test render measure Function
     */
    void UIKitRenderTestRenderMeasure001();

private:
    UIViewGroup* CreateTestCaseGroup() const;
    UILabel* CreateTitleLabel() const;
    UIScrollView* container_ = nullptr;
    UILabel* testLabel_ = nullptr;
    UILabelButton* labelButton_ = nullptr;
};
} // namespace OHOS
#endif // UI_TEST_RENDER_H

/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: TestCase
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */

#ifndef UI_TEST_ALIGNMENT1_H
#define UI_TEST_ALIGNMENT1_H

#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "ui_test.h"

namespace OHOS {
class UITestInternalAlignment : public UITest {
public:
    UITestInternalAlignment() : container_(nullptr) {}
    ~UITestInternalAlignment() {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;

    /**
     * @brief Test align Function
     */
    void UIKitAlignInternalTest001();
    void UIKitAlignInternalTop();
    void UIKitAlignInternalBottom();
    void UIKitAlignInternalMid();

private:
    UIScrollView* container_ = nullptr;
    UIViewGroup* viewGroup_ = nullptr;
    UILabel* label1_ = nullptr;
    UILabel* label2_ = nullptr;
    UILabel* label3_ = nullptr;
    UILabel* label4_ = nullptr;
    UILabel* label5_ = nullptr;
    UILabel* label6_ = nullptr;
    UILabel* label7_ = nullptr;
    UILabel* label8_ = nullptr;
    UILabel* label9_ = nullptr;
};
} // namespace OHOS
#endif // UI_TEST_ALIGNMENT1_H

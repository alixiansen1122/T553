/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: TestCase
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */

#ifndef UI_TEST_ALIGNMENT2_H
#define UI_TEST_ALIGNMENT2_H

#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "ui_test.h"

namespace OHOS {
class UITestExternalAlignment : public UITest {
public:
    UITestExternalAlignment() : container_(nullptr) {}
    ~UITestExternalAlignment() {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;

    /**
     * @brief Test align Function
     */
    void UIKitAlignExternalTest001();
    void UIKitAlignExternalTop();
    void UIKitAlignExternalBottom();
    void UIKitAlignExternalLeft();
    void UIKitAlignExternalRight();

private:
    UIScrollView* container_ = nullptr;
    UILabel* label_ = nullptr;
    UILabel* label1_ = nullptr;
    UILabel* label2_ = nullptr;
    UILabel* label3_ = nullptr;
    UILabel* label4_ = nullptr;
    UILabel* label5_ = nullptr;
    UILabel* label6_ = nullptr;
    UILabel* label7_ = nullptr;
    UILabel* label8_ = nullptr;
    UILabel* label9_ = nullptr;
    UILabel* label10_ = nullptr;
    UILabel* label11_ = nullptr;
    UILabel* label12_ = nullptr;
};
} // namespace OHOS
#endif // UI_TEST_ALIGNMENT2_H

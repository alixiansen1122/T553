/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: TestCase
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */

#ifndef UI_TEST_ALIGNMENT3_H
#define UI_TEST_ALIGNMENT3_H

#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "components/ui_image_view.h"
#include "layout/list_layout.h"
#include "ui_test.h"
#include "animator/animator.h"

namespace OHOS {
class UITestDynamicAlignment : public UITest, public AnimatorCallback {
public:
    UITestDynamicAlignment() : animator_(this, nullptr, 1000, true) // 1000: the animator duration time is 1000ms
    {}
    ~UITestDynamicAlignment() {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;
    void Callback(UIView* view) override;
    int16_t angleValue_ = 0;
    Vector2<float> scaleValue_ = {1.0f, 1.0f};
    float scaleStep_ = 0.01f;
    const Vector2<float> VIEW_CENTER = {75, 75};
    const Vector2<float> GROUP_CENTER = {200, 200};

    /**
     * @brief Test align Function
     */
    void UIKitAlignDynamicTest001();
    void UIKitAlignDynamicTop();
    void UIKitAlignDynamicMid();
    void UIKitAlignDynamicButtom();

private:
    Animator animator_;
    UIScrollView* container_ = nullptr;
    ListLayout* list_ = nullptr;
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
#endif // UI_TEST_ALIGNMENT3_H

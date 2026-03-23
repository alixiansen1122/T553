/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: Lottie
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */

#ifndef UI_TEST_OPACITY_H
#define UI_TEST_OPACITY_H

#include "components/root_view.h"
#include "components/text_adapter.h"
#include "components/ui_arc_label.h"
#include "components/ui_button.h"
#include "components/ui_image_animator.h"
#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_list.h"
#include "components/ui_scroll_view.h"
#include "ui_test.h"
namespace OHOS {
class UITestOpacity : public UITest {
public:
    UITestOpacity() {}
    ~UITestOpacity() {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;

    /**
     * @brief Test UILabel
     */
    void UIKitOpacityTestUILabel001();

    /**
     * @brief Test UILabel exception value
     */
    void UIKitOpacityTestUILabel002();

    /**
     * @brief Test UIButton
     */
    void UIKitOpacityTestUIButton001();

    /**
     * @brief Test UIButton image
     */
    void UIKitOpacityTestUIButton002();

    /**
     * @brief Test UILabelButton
     */
    void UIKitOpacityTestUILabelButton001();

    /**
     * @brief Test UIArcLabel
     */
    void UIKitOpacityTestUIArcLabel001();

    /**
     * @brief Test UIImageView
     */
    void UIKitOpacityTestUIImageView001();

    /**
     * @brief Test UIImageAnimator
     */
    void UIKitOpacityTestUIImageAnimator001();

    /**
     * @brief Test UIView
     */
    void UIKitOpacityTestUIView001();

    /**
     * @brief Test UIViewGroup
     */
    void UIKitOpacityTestUIViewGroup001();

    /**
     * @brief Test UIList
     */
    void UIKitOpacityTestUIList001();

    /**
     * @brief Test UIScrollView
     */
    void UIKitOpacityTestUIScrollView001();

    /**
     * @brief Test UISwipeView
     */
    void UIKitOpacityTestUISwipeView001();

private:
    UIViewGroup* CreateTestCaseGroup(std::string title) const;
    UILabel* CreateTestCaseUILabel(std::string title, uint8_t opaScale) const;
    UIButton* CreateTestCaseUIButton(uint8_t opaScale) const;
    UILabelButton* CreateTestCaseUILabelButton(std::string title, uint8_t opaScale) const;
    UIArcLabel* CreateTestCaseUIArcLabel(std::string title, uint8_t opaScale) const;
    UIImageView* CreateTestCaseUIImageView(std::string path, uint8_t opaScale) const;
    UIImageAnimatorView* CreateTestCaseUIImageAnimator(const ImageAnimatorInfo imageAnimatorInfo[],
                                                       uint8_t opaScale) const;
    UIView* CreateTestCaseUIView(uint8_t opaScale) const;
    UIList* CreateTestCaseUIList(uint8_t opaScale) const;

    UIScrollView* container_ = nullptr;
};
}
#endif // UI_TEST_OPACITY_H

/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: Lottie
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */
#ifndef UI_TEST_BUTTON_H
#define UI_TEST_BUTTON_H

#include "components/ui_button.h"
#include "components/ui_checkbox.h"
#include "components/ui_scroll_view.h"
#include "ui_test.h"

namespace OHOS {
class UITestButton : public UITest {
public:
    UITestButton() {}
    ~UITestButton() {}
    void SetUp() override;
    void TearDown() override;
    void SubTearDown();

    const UIView* GetTestView() override;

    /**
     * @brief Test Checkbox Function
     */
    void UIKitCheckBoxTest001();

    /**
     * @brief Test Checkbox's SetImage Function
     */
    void UIKitCheckBoxTest002() const;

    /**
     * @brief Test Radiobutton Function
     */
    void UIKitRadioButtonTest001();

    /**
     * @brief Test Radiobutton's SetImage Function
     */
    void UIKitRadioButtonTest002() const;

    /**
     * @brief Test Togglebutton Function
     */
    void UIKitToggleButtonTest001();

    /**
     * @brief Test Togglebutton's SetImage Function
     */
    void UIKitToggleButtonTest002();

    /**
     * @brief Test button Function
     */
    void UIKitButtonTest001();

private:
    void UIKit_Button_Test_002(UIScrollView* container, UIButton* button);
    UIViewGroup* CreateButtonGroup(int16_t posX, int16_t posY,
                                    UICheckBox::OnChangeListener** listener,
                                    UIViewType type = UI_CHECK_BOX,
                                    const char* name = "aa");
    static constexpr int16_t CHANGE_SIZE = 10;
    UIScrollView* container_ = nullptr;

    UICheckBox::OnChangeListener* checkBoxChangeListener_ = nullptr;
    UICheckBox::OnChangeListener* checkBoxChangeListener1_ = nullptr;
    UICheckBox::OnChangeListener* radioChangeListener_ = nullptr;
    UICheckBox::OnChangeListener* radioChangeListener1_ = nullptr;
    UICheckBox::OnChangeListener* toggleChangeListener_ = nullptr;
    UICheckBox::OnChangeListener* toggleChangeListener1_ = nullptr;

    UIView::OnClickListener* clickBigListener_ = nullptr;
    UIView::OnClickListener* clickLeftListener_ = nullptr;
    UIView::OnClickListener* clickRightListener_ = nullptr;
    UIView::OnClickListener* clickUpListener_ = nullptr;
    UIView::OnClickListener* clickDownListener_ = nullptr;
    UIView::OnClickListener* clickSmallListener_ = nullptr;
};
} // namespace OHOS
#endif // UI_TEST_BUTTON_H

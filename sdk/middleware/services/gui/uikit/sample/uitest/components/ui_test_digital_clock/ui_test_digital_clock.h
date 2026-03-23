/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: Lottie
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */

#ifndef UI_TEST_DIGITAL_CLOCK_H
#define UI_TEST_DIGITAL_CLOCK_H

#include "components/ui_scroll_view.h"
#include "font/ui_font.h"
#include "ui_test.h"

namespace OHOS {
class UITestDigitalClock : public UITest {
public:
    UITestDigitalClock() {}
    ~UITestDigitalClock() {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;

    void UIKitUIDigitalClockTestDisplay001();
    void UIKitUIDigitalClockTestDisplay002();
    void UIKitUIDigitalClockTestSetColor();
    void UIKitUIDigitalClockTestDisplayMode001();
    void UIKitUIDigitalClockTestDisplayMode002();
    void UIKitUIDigitalClockTestDisplayMode003();
    void UIKitUIDigitalClockTestDisplayMode004();
    void UIKitUIDigitalClockTestDisplayLeadingZero();
    void UIKitUIDigitalClockTestSetOpacity();

private:
    UIScrollView* container_ = nullptr;
    void InnerTestTitle(const char* title);
};
} // namespace OHOS
#endif // UI_TEST_DIGITAL_CLOCK_H

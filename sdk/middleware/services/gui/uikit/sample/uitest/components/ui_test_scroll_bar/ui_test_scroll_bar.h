/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: Lottie
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */

#ifndef UI_TEST_SCROLL_BAR_H
#define UI_TEST_SCROLL_BAR_H

#include "components/text_adapter.h"
#include "components/ui_image_view.h"
#include "components/ui_list.h"
#include "components/ui_scroll_view.h"
#include "ui_test.h"

namespace OHOS {
class UITestScrollBar : public UITest {
public:
    UITestScrollBar() {}
    ~UITestScrollBar() {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;

    void UIKitScrollBarTestScrollview001();
    void UIKitScrollBarTestList001();

private:
    UIScrollView* container_ = nullptr;
    UIScrollView* scrollView_ = nullptr;
    UIList* list_ = nullptr;
    UIImageView* foreImg1_ = nullptr;
    UIImageView* foreImg2_ = nullptr;
    List<const char*>* adapterData_ = nullptr;
    TextAdapter* adapter_ = nullptr;
}; // namespace OHOS
}
#endif // UI_TEST_SCROLL_BAR_H

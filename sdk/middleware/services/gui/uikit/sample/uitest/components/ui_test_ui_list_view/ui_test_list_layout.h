/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: Lottie
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */

#ifndef UI_TEST_UI_LIST_VIEW_H
#define UI_TEST_UI_LIST_VIEW_H

#include "components/ui_label.h"
#include "components/ui_scroll_view.h"
#include "ui_test.h"

namespace OHOS {
class UITestListLayout : public UITest {
public:
    UITestListLayout() {}
    ~UITestListLayout() {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;

    void ListVerticalAddTestAutoSize001();
    void ListVerticalRemoveTestAutoSize001();
    void ListVerticalRemoveTestAutoSize002();
    void ListHorAddTestAutoSize001();
    void ListHorRemoveTestAutoSize001();
    void ListHorRemoveTestAutoSize002();

private:
    void SetLastPos(UIView* view);
    UIScrollView* container_ = nullptr;
    UILabel* listDirect_ = nullptr;
};
} // namespace OHOS
#endif // UI_TEST_INPUT_EVENT_H

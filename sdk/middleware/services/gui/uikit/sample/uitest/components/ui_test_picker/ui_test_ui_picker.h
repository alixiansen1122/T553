/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: Lottie
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */

#ifndef UI_TEST_UI_PICKER_H
#define UI_TEST_UI_PICKER_H

#include "components/text_adapter.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_picker.h"
#include "components/ui_scroll_view.h"
#include "components/ui_time_picker.h"
#include "ui_test.h"

namespace OHOS {
class UITestUIPicker : public UITest, UIView::OnClickListener, UIPicker::SelectedListener,
    UITimePicker::SelectedListener {
public:
    UITestUIPicker() {}
    ~UITestUIPicker() {}
    void SetUp() override;
    void TearDown() override;
    bool OnClick(UIView& view, const ClickEvent& event) override;
    const UIView* GetTestView() override;
    void OnPickerStoped(UIPicker& picker) override;
    void OnTimePickerStoped(UITimePicker& picker) override;
    void CreatButtons();
    void UIKitPickerTestBase001();
    void UIKitPickerTestBase002();
    void UIKitPickerTestTimePicker001();

private:
    void SetLastPos(UIView* view);
    void SetUpButton(UILabelButton* btn, const char* title);
    UILabelButton* setLoopBtn_ = nullptr;
    UILabelButton* setLoopOffBtn_ = nullptr;
    UILabelButton* setSelectBtn_ = nullptr;
    UILabelButton* setLeftToRightBtn_ = nullptr;
    UILabelButton* setRightToLeftBtn_ = nullptr;
    UILabel* selectIndex_ = nullptr;
    UILabel* selectTime_ = nullptr;
    UIScrollView* container_ = nullptr;
    UIPicker* picker1_ = nullptr;
    UIPicker* picker3_ = nullptr;
    UITimePicker* picker2_ = nullptr;
    int16_t lastX_ = 0;
    int16_t lastY_ = 0;
};
} // namespace OHOS
#endif // UI_TEST_UI_PICKER_H

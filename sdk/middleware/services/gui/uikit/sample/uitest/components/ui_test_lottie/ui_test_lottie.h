/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: Lottie Test
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */

#ifndef UI_TEST_LOTTIE_H
#define UI_TEST_LOTTIE_H

#include "ui_test.h"
#include "lottie/ui_lott_view.h"
#include "components/ui_label_button.h"

namespace OHOS {
class UITestLottie : public UITest, public UIView::OnClickListener {
public:
    UITestLottie() {}
    ~UITestLottie() override {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;
    bool OnClick(UIView& view, const ClickEvent& event) override;

private:
    UIViewGroup* container_ = nullptr;
    UILottView* lottieView_ = nullptr;
    UILabelButton* startBtn_ = nullptr;
    UILabelButton* switchBtn_ = nullptr;
    UILabelButton* repeatBtn_ = nullptr;
    UILabelButton* transformBtn_ = nullptr;
    uint8_t fileIndex_ = 0;
};
}
#endif
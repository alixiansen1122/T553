
/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: Lottie
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */

#ifndef UI_TEST_FOCUS_MANAGER_H
#define UI_TEST_FOCUS_MANAGER_H

#include "graphic_config.h"
#if ENABLE_FOCUS_MANAGER
#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "dock/focus_manager.h"
#include "ui_test.h"

namespace OHOS {
class UITestFocusManager : public UITest {
public:
    UITestFocusManager() {}
    ~UITestFocusManager() {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;
    void InitListener(UIViewGroup* group);

    void UIKit_Focus_Manager_Test_001();
    void UIKit_Focus_Manager_Test_002(UIViewGroup* btnViewGroup);

private:
    UIView* CreateTestUILabel(UIViewGroup* parent, int16_t x, int16_t y,
        const char* text, bool focusable);
    UIViewGroup* CreateTestUIViewGroup(UIViewGroup* parent, bool focusable, bool interceptFocus);
    UILabelButton* SetUpButton(std::string title, int16_t x, int16_t y, UIViewGroup* viewGroup,
        UIView::OnClickListener* listener);
    UIScrollView* container_ = nullptr;
    UIView::OnFocusListener* testOnFocusListener_ = nullptr;
    UIView::OnClickListener* requestFocusByDirectionLeftListener_ = nullptr;
    UIView::OnClickListener* requestFocusByDirectionRightListener_ = nullptr;
    UIView::OnClickListener* requestFocusByDirectionUpListener_ = nullptr;
    UIView::OnClickListener* requestFocusByDirectionDownListener_ = nullptr;
    UIView::OnClickListener* setFocusableViewListener_ = nullptr;
    UIView::OnClickListener* setFocusableViewListener1_ = nullptr;
    UIView::OnClickListener* setGroupInterceptListener_ = nullptr;
    UIView::OnClickListener* setGroupInterceptListener1_ = nullptr;
    UIView::OnClickListener* resetFocusListener_ = nullptr;
    UIView::OnClickListener* clearFocusListener_ = nullptr;
};
} // namespace OHOS
#endif
#endif // UI_TEST_FOCUS_MANAGER_H
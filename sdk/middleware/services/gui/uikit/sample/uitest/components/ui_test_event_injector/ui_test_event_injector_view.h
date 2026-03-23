/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: Navtive Test Case
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */
#ifndef UI_TEST_EVENT_INJECTOR_VIEW_H
#define UI_TEST_EVENT_INJECTOR_VIEW_H

#include "graphic_config.h"
#include "components/root_view.h"

namespace OHOS {

class UITestEventInjectorView : public UIView, public RootView::OnKeyActListener {
public:
    UITestEventInjectorView() : label_(nullptr) {}
    virtual ~UITestEventInjectorView() {RootView::GetInstance()->ClearOnKeyActListener();}
    void InitListener() { RootView::GetInstance()->SetOnKeyActListener(this);}
    bool OnLongPressEvent(const LongPressEvent& event) override;
    bool OnDragEvent(const DragEvent& event) override;
    void SetSentence(std::string sentence) {sentence_ = sentence;}
    void SetLabel(UILabel* label) {label_ = label;}
    bool OnClickEvent(const ClickEvent& event) override;
    bool OnPressEvent(const PressEvent& event) override;
    bool OnReleaseEvent(const ReleaseEvent& event) override;
    bool OnCancelEvent(const CancelEvent& event) override;
    bool OnKeyAct(UIView& view, const KeyEvent& event) override;

private:
    UILabel* label_;
    std::string sentence_ = "click";
};
}

#endif // UI_TEST_EVENT_INJECTOR_VIEW_H
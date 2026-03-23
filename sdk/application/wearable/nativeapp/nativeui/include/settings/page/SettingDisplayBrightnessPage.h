/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingDisplayBrightnessPage
 * Create: 2025-06-01
 */
#ifndef SETTING_DISPLAY_BRIGHTNESS_PAGE_H
#define SETTING_DISPLAY_BRIGHTNESS_PAGE_H

#include "components/ui_view_group.h"
#include "components/ui_circle_progress.h"
#include "components/ui_scroll_view_nested.h"
#include "components/ui_label.h"
#include "components/ui_button.h"
#include "animator/animator.h"
#include "SlicePage.h"
#include "settings/SettingPresenter.h"
#include "settings/common/SettingCommon.h"

namespace OHOS {
class SettingDisplayBrightnessPage : public SlicePage<SettingPresenter>,
                                     public UIView::OnClickListener,
                                     public UIView::OnLongPressListener,
                                     public UIView::OnTouchListener,
                                     public UIView::OnDragListener,
                                     public AnimatorCallback {
public:
    using SlicePage<SettingPresenter>::OnStop;
    using AnimatorCallback::OnStop;
    void OnStart(void* data) override;
    void OnResume() override;
    void OnPause() override;
    void OnStop() override;
    bool OnClick(UIView &view, const ClickEvent &event) override;
    void Callback(UIView* view) override;
    bool OnLongPress(UIView& view, const LongPressEvent& event) override;
    bool OnRelease(UIView& view, const ReleaseEvent& event) override;
    bool OnDragEnd(UIView& view, const DragEvent& event) override;
protected:
    void InitProgressView();
    void InitButtonView();
private:
    UILabel* title_ = nullptr;
    UICircleProgress* progress_ = nullptr;
    UILabel* progressNum_ = nullptr;
    UIButton* add_ = nullptr;
    UIButton* sub_ = nullptr;
    Animator* animator_ = nullptr;
    UIButton* curLongPressButton_ = nullptr;
};
}
#endif
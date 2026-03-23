/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingDisplaySteadyOnPage
 * Create: 2025-06-01
 */
#ifndef SETTING_DISPLAY_STEADY_ON_PAGE_H
#define SETTING_DISPLAY_STEADY_ON_PAGE_H

#include "components/ui_scroll_view_nested.h"
#include "components/ui_label.h"
#include "components/ui_button.h"
#include "components/ui_picker.h"
#include "SlicePage.h"
#include "settings/SettingPresenter.h"
#include "settings/common/SettingCommon.h"

namespace OHOS {
class SettingDisplaySteadyOnPage : public SlicePage<SettingPresenter>,
                                   public UIView::OnClickListener {
public:
    void OnStart(void* data) override;
    void OnResume() override;
    void OnStop() override;
    bool OnClick(UIView &view, const ClickEvent &event) override;
private:
    UILabel* title_ = nullptr;
    UIPicker* picker_ = nullptr;
    UIButton* button_ = nullptr;
};
}
#endif
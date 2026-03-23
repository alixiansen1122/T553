/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingDesktopDialPage
 * Create: 2025-06-01
 */
#ifndef SETTING_DESKTOP_DIAL_PAGE_H
#define SETTING_DESKTOP_DIAL_PAGE_H

#include <vector>
#include "components/ui_label.h"
#include "components/ui_button.h"
#include "components/ui_scroll_view_nested.h"
#include "components/ui_coverflow_view.h"
#include "components/ui_roller_view.h"
#include "SlicePage.h"
#include "settings/SettingPresenter.h"
#include "settings/common/SettingCommon.h"

namespace OHOS {
class SettingDesktopDialPage : public SlicePage<SettingPresenter>,
                               public UIView::OnClickListener,
                               public UICoverflowView::OnCoverflowScrollListener,
                               public UIRollerView::OnScrollListener {
public:
    void OnStart(void* data) override;
    void OnResume() override;
    void OnPause() override;
    void OnStop() override;
    bool OnClick(UIView& view, const ClickEvent& event) override;
    bool OnScroll() override;
    void OnScroll(UIRollerView* roller, std::string currentItemName, float currentItemAngle) override;
protected:
    void InitDialView();
    void InitCtrlView();
private:
    UILabel* itemLabel_ = nullptr;
    UICoverflowView* coverflow_ = nullptr;
    UIRollerView* rollerView_ = nullptr;
    uint8_t itemCount_ = 0;
    uint8_t itemIndex_ = 0;
};
} // OHOS
#endif
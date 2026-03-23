/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingSystemOTAPage
 * Create: 2025-06-01
 */
#ifndef SETTING_SYSTEM_OTA_PAGE_H
#define SETTING_SYSTEM_OTA_PAGE_H

#include "components/ui_scroll_view_nested.h"
#include "components/ui_label.h"
#include "components/ui_button.h"
#include "components/ui_slider.h"
#include "components/ui_view_group.h"
#include "components/ui_circle_progress.h"
#include "animator/animator.h"
#include "SlicePage.h"
#include "settings/SettingPresenter.h"
#include "settings/common/SettingCommon.h"

namespace OHOS {
class SettingSystemOtaPage : public SlicePage<SettingPresenter>,
                             public UIView::OnClickListener,
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
protected:
    void InitPreGroupView();
    void InitMidCircleView();
    void InitMidMessageView();
private:
    Animator* upgradeAnimator_ = nullptr;
    UIViewGroup* preUpgradeGroup_ = nullptr;
    UIViewGroup* midUpgradeGroup_ = nullptr;
    UIButton* preUpgradeButton_ = nullptr;
    UILabel* preUpgradeLabel_ = nullptr;
    UICircleProgress* midUpgradeProress_ = nullptr;
    UILabel* midUpgradeLabel_ = nullptr;
    UILabel* midUpgradePercent_ = nullptr;
    UILabel* midUpgradeMessage_ = nullptr;
};
} // OHOS
#endif
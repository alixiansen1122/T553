/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingPowerPage
 * Create: 2025-06-01
 */
#ifndef SETTING_POWER_PAGE_H
#define SETTING_POWER_PAGE_H

#include "components/ui_scroll_view_nested.h"
#include "components/ui_label.h"
#include "components/ui_view_group.h"
#include "animator/animator.h"
#include "SlicePage.h"
#include "settings/SettingPresenter.h"
#include "settings/common/SettingCommon.h"

namespace OHOS {
class SettingPowerPage : public SlicePage<SettingPresenter>,
                         public AnimatorCallback {
public:
    using SlicePage<SettingPresenter>::OnStop;
    using AnimatorCallback::OnStop;
    void OnStart(void* data) override;
    void OnResume() override;
    void OnPause() override;
    void OnStop() override;
    void Callback(UIView* view) override;
protected:
    void InitPowerView();
    void InitBatteryStatusView();
    void InitNotify();
private:
    UIScrollView* scrollView_ = nullptr;
    UILabel* title_ = nullptr;
    UILabel* powerDesc_ = nullptr;
    UILabel* batteryPercentage_ = nullptr;
    UILabel* timeLabel_ = nullptr;
    UIViewGroup* batteryStatusGroup_ = nullptr;
    UILabel* batteryLabel_ = nullptr;
    UILabel* batteryLevel_ = nullptr;
    UILabel* detailsDesc_ = nullptr;
    Animator* timeAnimator_ = nullptr;
};
} // OHOS
#endif
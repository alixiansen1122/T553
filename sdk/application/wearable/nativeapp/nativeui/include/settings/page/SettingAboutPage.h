/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingAboutPage
 * Create: 2025-06-01
 */
#ifndef SETTING_ABOUT_PAGE_H
#define SETTING_ABOUT_PAGE_H

#include "components/ui_label.h"
#include "components/ui_scroll_view_nested.h"
#include "SlicePage.h"
#include "settings/SettingPresenter.h"
#include "settings/common/SettingCommon.h"

namespace OHOS {
class SettingAboutPage : public SlicePage<SettingPresenter> {
public:
    void OnStart(void* data) override;
    void OnResume() override;
    void OnStop() override;
protected:
    void InitTitle();
    void InitDeviceTypeView();
    void InitDeviceNameView();
private:
    UILabel* title_ = nullptr;
    UILabel* deviceNameTitle_ = nullptr;
    UILabel* deviceTypeTitle_ = nullptr;
    UILabel* deviceName_ = nullptr;
    UILabel* deviceType_ = nullptr;
};
} // OHOS
#endif
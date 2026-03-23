/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingSystemOperationPage
 * Create: 2025-06-01
 */
#ifndef SETTING_SYSTEM_OPERATION_PAGE_H
#define SETTING_SYSTEM_OPERATION_PAGE_H

#include <map>
#include <string>
#include "components/ui_scroll_view_nested.h"
#include "components/ui_label.h"
#include "components/ui_button.h"
#include "components/ui_slider.h"
#include "components/ui_view_group.h"
#include "SlicePage.h"
#include "settings/SettingPresenter.h"
#include "settings/common/SettingCommon.h"

namespace OHOS {
struct OperationData {
    ColorType sliderBgColor;
    uint32_t  sliderKnobImage;
};

class SettingSystemOperationPage : public SlicePage<SettingPresenter>,
                                   public UIView::OnClickListener,
                                   public UISlider::UISliderEventListener {
public:
    SettingSystemOperationPage();
    void OnStart(void* data) override;
    void OnResume() override;
    void OnStop() override;
    bool OnClick(UIView &view, const ClickEvent &event) override;
    void OnChange(int32_t value) override;
    void OnRelease(int32_t value) override;
protected:
    void InitSliderView();
    void InitButtonView();
    void RefreshSlider();
private:
    UISlider* slider_ = nullptr;
    UILabel* sliderLabel_ = nullptr;
    UIButton* button_ = nullptr;
    UILabel* buttonDescription_ = nullptr;
    std::string operationName_;
    std::map<std::string, OperationData> operationData_;
    bool sliderBegin_ = false;
};
} // OHOS
#endif
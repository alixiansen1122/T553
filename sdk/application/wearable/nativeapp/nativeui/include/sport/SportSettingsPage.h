/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SportSettingsPage
 * Create: 2025-06-06
 */

#ifndef SPORT_SETTINGS_PAGE_H
#define SPORT_SETTINGS_PAGE_H

#include "components/ui_label.h"
#include "components/ui_simple_list.h"
#include "components/ui_image_view.h"
#include "components/ui_scroll_view.h"
#include "SlicePage.h"
#include "ui_resource_sport.h"
#include "SportPresenter.h"
#include "SportModel.h"
#include "SportSettingsItemView.h"

namespace OHOS {
const char* SETTINGS_ITEM_TARGET = "settingsItemTarget";
const char* SETTINGS_ITEM_REMIND = "settingsItemTargetRemind";
const char* SETTINGS_ITEM_MISTOUCH = "settingsItemTargetMistouch";

class SportSettingsPage : public SlicePage<SportPresenter>,
    public UIView::OnClickListener,
    public UIView::OnDragListener {
public:
    SportSettingsPage();
    ~SportSettingsPage();
    void OnStart(void* data) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnClick(UIView& view, const ClickEvent& event) override;
private:
    void InitBackgroundView();
    void CreateUILabel(void);
    void CreateUIList(void);
    UIScrollView *group_{nullptr};
    UIImageView *background_{nullptr};
    UILabel *titleLabel_{nullptr};
    UISimpleList *settingList_{nullptr};
    SportSettingsItem settings_[SPORT_SET_TYPE_MAX] = {
        {SPORT_SET_TYPE_TARGET, IMAGE_SPORT_TARGET, "目标", SETTINGS_ITEM_TARGET},
        {SPORT_SET_TYPE_REMIND, IMAGE_SPORT_REMIND, "提醒", SETTINGS_ITEM_REMIND},
        {SPORT_SET_TYPE_MISTOUCH, IMAGE_SPORT_MISTOUCH, "防误触", SETTINGS_ITEM_MISTOUCH},
        };
};
}  // namespace OHOS
#endif  // SPORT_SETTINGS_PAGE_H
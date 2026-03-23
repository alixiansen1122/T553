/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SportRemindPage
 * Create: 2025-06-06
 */

#ifndef SPORT_REMIND_PAGE_H
#define SPORT_REMIND_PAGE_H

#include "components/ui_label.h"
#include "components/ui_simple_list.h"
#include "components/ui_image_view.h"
#include "components/ui_view_group.h"
#include "SlicePage.h"
#include "ui_resource_sport.h"
#include "SportPresenter.h"
#include "SportModel.h"
#include "SportRemindItemView.h"

namespace OHOS {
const char* SPORT_REMIND_ITEM_INTERVAL = "sportRemindItemInterval";
const char* SPORT_REMIND_ITEM_HEARTRATE = "sportRemindItemHeartRate";

class SportRemindPage : public SlicePage<SportPresenter>,
    public UIView::OnClickListener,
    public UIView::OnDragListener {
public:
    SportRemindPage();
    ~SportRemindPage();
    void OnStart(void* data) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnClick(UIView& view, const ClickEvent& event) override;
private:
    void InitBackgroundView();
    void CreateUILabel(void);
    void CreateUIList(void);
    UIViewGroup *group_{nullptr};
    UIImageView *background_{nullptr};
    UILabel *titleLabel_{nullptr};
    UISimpleList *remindList_{nullptr};
    SportRemindItem reminds_[SPORT_REMIND_MAX] = {
        {SPORT_REMIND_INTERVAL, IMAGE_SPORT_INTERVAL, "间隔提醒", SPORT_REMIND_ITEM_INTERVAL},
        {SPORT_REMIND_HEARTRATE, IMAGE_SPORT_HEARTRATE, "心率提醒", SPORT_REMIND_ITEM_HEARTRATE}};
};
}  // namespace OHOS
#endif  // SPORT_REMIND_PAGE_H
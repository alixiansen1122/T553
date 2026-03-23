/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SportRemindIntervalPage
 * Create: 2025-06-06
 */

#ifndef SPORT_REMIND_INTERVAL_PAGE_H
#define SPORT_REMIND_INTERVAL_PAGE_H

#include "components/ui_label.h"
#include "components/ui_simple_list.h"
#include "components/ui_image_view.h"
#include "components/ui_scroll_view.h"
#include "SlicePage.h"
#include "ui_resource_sport.h"
#include "SportPresenter.h"
#include "SportModel.h"
#include "SportRemindIntervalItemView.h"

namespace OHOS {
const char* SPORT_REMIND_INTERVAL_ITEM_DISTANCE = "sportRemindIntervalItemDistance";
const char* SPORT_REMIND_INTERVAL_ITEM_TIME = "sportRemindIntervalItemTime";
const char* SPORT_REMIND_INTERVAL_ITEM_NONE = "sportRemindIntervalItemNone";

class SportRemindIntervalPage : public SlicePage<SportPresenter>,
    public UIView::OnClickListener,
    public UIView::OnDragListener {
public:
    SportRemindIntervalPage();
    ~SportRemindIntervalPage();
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
    UISimpleList *remindIntervalList_{nullptr};
    SportRemindIntervalItem remindIntervals_[SPORT_REMIND_INTERVAL_MAX] = {
        {SPORT_REMIND_INTERVAL_DISTANCE, IMAGE_SPORT_DISTANCE, "距离", SPORT_REMIND_INTERVAL_ITEM_DISTANCE},
        {SPORT_REMIND_INTERVAL_TIME, IMAGE_SPORT_TIME, "时间", SPORT_REMIND_INTERVAL_ITEM_TIME},
        {SPORT_REMIND_INTERVAL_NONE, IMAGE_SPORT_NONE, "无提醒", SPORT_REMIND_INTERVAL_ITEM_NONE}};
};
}  // namespace OHOS
#endif  // SPORT_REMIND_INTERVAL_PAGE_H
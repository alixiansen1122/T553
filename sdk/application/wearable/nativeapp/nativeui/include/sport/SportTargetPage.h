/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SportTargetPage
 * Create: 2025-06-06
 */

#ifndef SPORT_TARGET_PAGE_H
#define SPORT_TARGET_PAGE_H

#include "components/ui_label.h"
#include "components/ui_simple_list.h"
#include "components/ui_image_view.h"
#include "components/ui_scroll_view.h"
#include "SlicePage.h"
#include "ui_resource_sport.h"
#include "SportPresenter.h"
#include "SportModel.h"
#include "SportTargetItemView.h"

namespace OHOS {
const char* SPORT_TARGET_ITEM_DISTANCE = "sportTargetItemDistance";
const char* SPORT_TARGET_ITEM_TIME = "sportTargetItemTime";
const char* SPORT_TARGET_ITEM_CALORIE = "sportTargetItemCalorie";

class SportTargetPage : public SlicePage<SportPresenter>,
    public UIView::OnClickListener,
    public UIView::OnDragListener {
public:
    SportTargetPage();
    ~SportTargetPage();
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
    UISimpleList *targetList_{nullptr};
    SportTargetItem targets_[SPORT_TARGET_MAX] = {
        {SPORT_TARGET_DISTANCE, IMAGE_SPORT_DISTANCE, "距离", SPORT_TARGET_ITEM_DISTANCE},
        {SPORT_TARGET_TIME, IMAGE_SPORT_TIME, "时间", SPORT_TARGET_ITEM_TIME},
        {SPORT_TARGET_CALORIE, IMAGE_SPORT_CALORIE, "热量", SPORT_TARGET_ITEM_CALORIE}};
};
}  // namespace OHOS
#endif  // SPORT_TARGET_PAGE_H
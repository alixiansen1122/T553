/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SportHeartRateRangePage
 * Create: 2025-06-06
 */

#ifndef SPORT_HEART_RATE_RANGE_PAGE_H
#define SPORT_HEART_RATE_RANGE_PAGE_H

#include "components/ui_label.h"
#include "components/ui_simple_list.h"
#include "components/ui_scroll_view.h"
#include "SlicePage.h"
#include "UiConfig.h"
#include "SportPresenter.h"
#include "SportModel.h"
#include "SportHeartRateRangeItemView.h"

namespace OHOS {
const char* SPORT_HEARTRATE_RANGE_ITEM_1 = "sportHeartRateRangeItem1";
const char* SPORT_HEARTRATE_RANGE_ITEM_2 = "sportHeartRateRangeItem2";
const char* SPORT_HEARTRATE_RANGE_ITEM_3 = "sportHeartRateRangeItem3";
const char* SPORT_HEARTRATE_RANGE_ITEM_4 = "sportHeartRateRangeItem4";

class SportHeartRateRangePage : public SlicePage<SportPresenter>,
    public UIView::OnClickListener,
    public UIView::OnDragListener {
public:
    SportHeartRateRangePage();
    ~SportHeartRateRangePage();
    void OnStart(void* data) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnClick(UIView& view, const ClickEvent& event) override;
private:
    void CreateUILabel(void);
    void CreateUIList(void);
    UIScrollView *group_{nullptr};
    UILabel *titleLabel_{nullptr};
    UISimpleList *heartRateRangeList_{nullptr};
    SportHeartRateRangeItem heartRateRanges_[SPORT_HEARTRATE_RANGE_MAX] = {
        {SPORT_HEARTRATE_RANGE_ITEM1, "热身燃脂", SPORT_HEARTRATE_RANGE_ITEM_1},
        {SPORT_HEARTRATE_RANGE_ITEM2, "有氧耐力", SPORT_HEARTRATE_RANGE_ITEM_2},
        {SPORT_HEARTRATE_RANGE_ITEM3, "无氧耐力", SPORT_HEARTRATE_RANGE_ITEM_3},
        {SPORT_HEARTRATE_RANGE_ITEM4, "极限训练", SPORT_HEARTRATE_RANGE_ITEM_4}};
};
}  // namespace OHOS
#endif  // SPORT_HEART_RATE_RANGE_PAGE_H
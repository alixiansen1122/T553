/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: WeatherPageDays
 * Create: 2025-03-15
 */

#ifndef WEATHER_PAGE_DAYS_H
#define WEATHER_PAGE_DAYS_H

#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_simple_list.h"
#include "components/ui_scroll_view_nested.h"
#include "components/ui_swipe_view.h"
#include "SlicePage.h"
#include "UiConfig.h"
#include "WeatherItemView.h"
#include "WeatherPresenter.h"

namespace OHOS {
class WeatherPageDays :  public SlicePage<WeatherPresenter>,
                        public UIView::OnClickListener,
                        public UIView::OnDragListener,
                        public UISwipeView::OnSwipeListener {
public:
    WeatherPageDays();
    ~WeatherPageDays() override;
    void OnStart(void* data) override;
    bool OnClick(UIView &view, const ClickEvent &event) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    void OnSwipe(UISwipeView &view) override;
    void InitScrollView();
    bool InitScrollData();
    void RefreshWeathersList(WeatherItem *item, uint8_t count);
private:
    UIViewGroup *group_{nullptr};
    UISimpleList *contentList_{nullptr};
    UILabel *title_{nullptr};
    UIImageView *bgImg_{nullptr};
    UIImageView *pageIdxImg_{nullptr};
};
}
#endif
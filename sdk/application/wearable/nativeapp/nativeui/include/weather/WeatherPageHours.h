/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: WeatherPageHours
 * Create: 2025-03-15
 */

#ifndef WEATHER_PAGE_HOURS_H
#define WEATHER_PAGE_HOURS_H

#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_swipe_view.h"
#include "ui_swipe_view.h"
#include "SlicePage.h"
#include "UiConfig.h"
#include "WeatherPresenter.h"
#include "WeatherView.h"
#include "WeatherPageHoursGroup.h"

namespace OHOS {
static constexpr int16_t GROUP_24H_COUNT = 3;

class WeatherPresenter;
class WeatherPageHours : public SlicePage<WeatherPresenter>,
                        public UIView::OnClickListener,
                        public UIView::OnDragListener,
                        public UISwipeView::OnSwipeListener {
public:
    WeatherPageHours();
    ~WeatherPageHours() override;
    void OnStart(void* data) override;
    bool OnClick(UIView &view, const ClickEvent &event) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    void OnSwipe(UISwipeView &view) override;
private:
    UISwipeView *weatherSwipeGroup_{nullptr};
    WeatherPageHoursGroup *hoursGroup_[GROUP_24H_COUNT]{nullptr};
};
}
#endif
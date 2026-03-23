/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: WeatherPageMonthRise
 * Create: 2025-03-15
 */

#ifndef WEATHER_PAGE_MONTHRISE_H
#define WEATHER_PAGE_MONTHRISE_H

#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_swipe_view.h"
#include "ui_swipe_view.h"
#include "SlicePage.h"
#include "UiConfig.h"
#include "WeatherPresenter.h"
#include "WeatherView.h"

namespace OHOS {
static constexpr int16_t MONTHRISE_TIME_LABEL_COUNT = 2;
static constexpr int16_t BUTTON_CHANGE_END = 6;

class WeatherPresenter;
class WeatherPageMonthRise : public SlicePage<WeatherPresenter>,
                        public UIView::OnClickListener,
                        public UIView::OnDragListener {
public:
    WeatherPageMonthRise();
    ~WeatherPageMonthRise() override;
    void OnStart(void* data) override;
    bool OnClick(UIView &view, const ClickEvent &event) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    void InitLabel();
    void InitLabelMonth();
    void InitImage();
    void InitButton();
private:
    UIViewGroup *group_{nullptr};
    UIImageView *bgImg_{nullptr};
    UIImageView *drawImg_{nullptr};
    UIImageView *monthImg_{nullptr};
    UILabel *labelTitle_{nullptr};
    UILabel *labelDay_{nullptr};
    UILabel *labelMonth_[MONTHRISE_TIME_LABEL_COUNT]{nullptr};
    UIButton *buttonChangeLeft_{nullptr};
    UIButton *buttonChangeRight_{nullptr};
    MonthStruct *month_ = nullptr;
    int index_ = 0;
};
}
#endif
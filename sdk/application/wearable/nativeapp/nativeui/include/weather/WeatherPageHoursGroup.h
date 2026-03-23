/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: WeatherPageHoursGroup
 * Create: 2025-03-15
 */

#ifndef WEATHER_PAGE_HOURS_GROUP_H
#define WEATHER_PAGE_HOURS_GROUP_H

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
static constexpr int16_t WEATHER_24H_PER_8H = 8;

class WeatherPresenter;
class WeatherPageHoursGroup : public UIViewGroup {
public:
    WeatherPageHoursGroup();
    ~WeatherPageHoursGroup() override;
    void WeatherInit(int index);
    void InitLabel(int index);
    void InitImage(int index);
private:
    std::vector<HourWeather> houWeather_;
    UILabel *labelTime_[WEATHER_24H_PER_8H]{nullptr};
    UILabel *labelTemp_[WEATHER_24H_PER_8H]{nullptr};
    UIImageView *imgBack_[WEATHER_24H_PER_8H]{nullptr};
    UIImageView *imgWeather_[WEATHER_24H_PER_8H]{nullptr};
    UILabel *topLabel_{nullptr};
    UIImageView *bgImg_{nullptr};
    UIImageView *pageIdxImg_{nullptr};
};
}
#endif
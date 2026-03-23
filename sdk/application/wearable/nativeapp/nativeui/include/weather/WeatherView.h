/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: WeatherView
 * Create: 2025-11-23
 */

#ifndef WEATHER_VIEW_H
#define WEATHER_VIEW_H

#include "View.h"

namespace OHOS {
enum WEATHER_PAGES {
    WEATHER_MAIN_PAGE = 1,  // 0 is invalid
    WEATHER_HOURS_PAGE = 2,
    WEATHER_DAYS_PAGE = 3,
    WEATHER_SUNRISE_PAGE = 4,
    WEATHER_MONTHRISE_PAGE = 5,
    WEATHER_LUNARPHASE_PAGE = 6,
};

class WeatherPresenter;
class WeatherView : public View<WeatherPresenter> {};
}
#endif  // WEATHER_VIEW_H

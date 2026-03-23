/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: WeatherPresenter
 * Create: 2025-11-23
 */

#ifndef WEATHER_PRESENTER_H
#define WEATHER_PRESENTER_H

#include "Presenter.h"
#include "components/ui_view.h"
#include "WeatherModel.h"

namespace OHOS {
class WeatherView;
class WeatherPresenter : public Presenter<WeatherView> {};
}

#endif  // WEATHER_PRESENTER_H
/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: WeatherPresenter
 * Create: 2025-03-23
 */

#include "NativeRegisterManager.h"
#include "weather/WeatherView.h"
#include "weather/WeatherPresenter.h"

namespace OHOS {
REGIST_MENU(VIEW_WEATHER, WeatherView, WeatherPresenter, WEATHER_ICON, WEATHER_ICON, "天气");
}

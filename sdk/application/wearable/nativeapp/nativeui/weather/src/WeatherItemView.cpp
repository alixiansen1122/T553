/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: WeatherItemView
 * Create: 2025-03-23
 */

#include "wearable_log.h"
#include "weather/WeatherModel.h"
#include "weather/WeatherItemView.h"

namespace OHOS {
static constexpr int16_t WEATHER_ICON_X = 155;
static constexpr int16_t WEATHER_LABEL_X = 10;
static constexpr int16_t WEATHER_LABEL2_X = 220;
static constexpr int16_t WEATHER_CHARTLINE_X = 20;
static constexpr int16_t WEATHER_CHARTLINE_Y = 78;
static constexpr int16_t WEATHER_CHARTLINE_WIDTH = 450;
static constexpr int16_t WEATHER_CHARTLINE_HEIGHT = 2;
static constexpr int16_t WEATHER_ITEM_LABEL_WIDTH = 155;
static constexpr int16_t WEATHER_ITEM_LABEL_HEIGHT = 80;
static constexpr int16_t WEATHER_ITEM_FONT_SIZE = 35;
static constexpr int16_t WEATHER_IMAGE_SIZE = 300;
static constexpr int16_t WEATHER_ICON_Y = 15;
static constexpr int16_t NUMS_50 = 50;

WeatherItemView::WeatherItemView()
{
    bgImg_.SetPosition(0, 0);
    bgImg_.Resize(WEATHER_IMAGE_SIZE, WEATHER_IMAGE_SIZE);
    WeatherModel::GetInstance().LoadImageOfDayNight(&bgImg_, IMAGE_WEATHER_ITEM1, IMAGE_WEATHER_ITEM2);
    labelDay_.SetPosition(WEATHER_LABEL_X, 0);
    labelDay_.Resize(WEATHER_ITEM_LABEL_WIDTH, WEATHER_ITEM_LABEL_HEIGHT);
    labelDay_.SetFont(DEFAULT_VECTOR_FONT_FILENAME, WEATHER_ITEM_FONT_SIZE);
    labelDay_.SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    labelDay_.SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    labelDay_.SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
    labelDay_.SetStyle(STYLE_TEXT_COLOR, Color::White().full);

    icon_.SetPosition(WEATHER_ICON_X, WEATHER_ICON_Y);
    icon_.SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);

    labelTmp_.SetPosition(WEATHER_LABEL2_X, 0);
    labelTmp_.Resize(WEATHER_ITEM_LABEL_WIDTH, WEATHER_ITEM_LABEL_HEIGHT);
    labelTmp_.SetFont(DEFAULT_VECTOR_FONT_FILENAME, WEATHER_ITEM_FONT_SIZE);
    labelTmp_.SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    labelTmp_.SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
    labelTmp_.SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    labelTmp_.SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);

    chartLine_.SetPosition(WEATHER_CHARTLINE_X, WEATHER_CHARTLINE_Y, WEATHER_CHARTLINE_WIDTH, WEATHER_CHARTLINE_HEIGHT);
    chartLine_.SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);

    Add(&bgImg_);
    Add(&labelDay_);
    Add(&icon_);
    Add(&labelTmp_);
    Add(&chartLine_);
}

WeatherItemView::~WeatherItemView()
{
    Remove(&bgImg_);
    Remove(&icon_);
    Remove(&labelDay_);
    Remove(&labelTmp_);
    Remove(&chartLine_);
}

void WeatherItemView::SetItemInfo(const WeatherItem itemInfo)
{
    labelDay_.SetText(itemInfo.weatherWeek);
    LOADIMG::LoadImageViewImg(&icon_, WEATHER_IMAGE, itemInfo.image);
    labelTmp_.SetText(itemInfo.weatherTemp);
    chartLine_.SetVisible(itemInfo.hasSecondMenu);
}
}

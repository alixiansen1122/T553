/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: WeatherPageHoursGroup
 * Create: 2025-03-15
 */

#include "NativeAbility.h"
#include "SlicePageFactory.h"
#include "common/image_cache_manager.h"
#include "components/ui_list_nested.h"
#include "components/ui_scroll_view_nested.h"
#include "ui_resource_weather.h"
#include "wearable_log.h"
#include "weather/WeatherModel.h"
#include "weather/WeatherPresenter.h"
#include "weather/WeatherView.h"
#include "weather/WeatherPageHoursGroup.h"

namespace OHOS {
static constexpr uint16_t WEATHER_TITLE_WIDTH = 51;
static constexpr uint16_t WEATHER_TITLE_HEIGHT = 26;
static constexpr uint16_t WEATHER_TOPLABEL_X = 156;
static constexpr uint16_t WEATHER_TOPLABEL_Y = 22;
static constexpr uint16_t WEATHER_TOPLABEL_WIDTH = 150;
static constexpr uint16_t WEATHER_TOPLABEL_HEIGHT = 60;
static constexpr uint16_t WEATHER_TOPLABEL_FRONT = 26;
static constexpr uint16_t WEATHER_LABELTIME_FRONT = 20;
static constexpr uint16_t WEATHER_LABELTIME_FRONT_TWO = 24;
static constexpr int16_t WEATHER_IMAGEPAGEKEY_X = 199;
static constexpr int16_t WEATHER_IMAGEPAGEKEY_Y = 429;
static constexpr int16_t WEATHER_IMAGEPAGEKEY_WIDTH = 56;
static constexpr int16_t WEATHER_IMAGEPAGEKEY_HEIGHT = 10;

WeatherPageHoursGroup::WeatherPageHoursGroup()
{
    WeatherInit(WeatherModel::GetInstance().index_);
}

WeatherPageHoursGroup::~WeatherPageHoursGroup()
{
    for (int i = 0; i < WEATHER_24H_PER_8H; ++i) {
        if (imgBack_[i] != nullptr) {
            delete imgBack_[i];
            imgBack_[i] = nullptr;
        }
    }

    for (int i = 0; i < WEATHER_24H_PER_8H; ++i) {
        if (labelTime_[i] != nullptr) {
            delete labelTime_[i];
            labelTime_[i] = nullptr;
        }
    }

    for (int i = 0; i < WEATHER_24H_PER_8H; ++i) {
        if (labelTemp_[i] != nullptr) {
            delete labelTemp_[i];
            labelTemp_[i] = nullptr;
        }
    }

    for (int i = 0; i < WEATHER_24H_PER_8H; ++i) {
        if (imgWeather_[i] != nullptr) {
            delete imgWeather_[i];
            imgWeather_[i] = nullptr;
        }
    }

    if (bgImg_ != nullptr) {
        delete bgImg_;
        bgImg_ = nullptr;
    }

    if (pageIdxImg_ != nullptr) {
        delete pageIdxImg_;
        pageIdxImg_ = nullptr;
    }

    if (topLabel_ != nullptr) {
        delete topLabel_;
        topLabel_ = nullptr;
    }
    ImageCacheManager::GetInstance().UnloadAllInMultiRes(WEATHER_IMAGE);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "~WeatherPageHoursGroup");
}

void WeatherPageHoursGroup::WeatherInit(int index)
{
    SetPosition(0, 0);
    Resize(HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    SetViewId("weather");
    SetDraggable(true);
    SetTouchable(true);
    houWeather_ = WeatherModel::GetInstance().GetHourWeather();

    bgImg_ = new UIImageView();
    if (bgImg_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "WeatherInit bgImg_ new fail");
        return;
    }
    bgImg_->SetPosition(0, 0);
    bgImg_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    WeatherModel::GetInstance().LoadImageOfDayNight(bgImg_, IMAGE_WEATHER_BG1, IMAGE_WEATHER_BG2);
    Add(bgImg_);

    pageIdxImg_ = new UIImageView();
    if (pageIdxImg_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "pageIdxImg_ new fail");
        return;
    }
    pageIdxImg_->SetPosition(WEATHER_IMAGEPAGEKEY_X, WEATHER_IMAGEPAGEKEY_Y, WEATHER_IMAGEPAGEKEY_WIDTH,
                             WEATHER_IMAGEPAGEKEY_HEIGHT);
    pageIdxImg_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    LOADIMG::LoadImageViewImg(pageIdxImg_, WEATHER_IMAGE, IMAGE_WEATHER_PAGE2);
    Add(pageIdxImg_);

    topLabel_ = new UILabel();
    if (topLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "WeatherInit new topLabel_ fail");
        return;
    }
    topLabel_->SetText("24小时预报");
    topLabel_->SetFont(BOLD_VECTOR_FONT_FILENAME, WEATHER_TOPLABEL_FRONT);
    topLabel_->SetPosition(WEATHER_TOPLABEL_X, WEATHER_TOPLABEL_Y, WEATHER_TOPLABEL_WIDTH, WEATHER_TOPLABEL_HEIGHT);
    Add(topLabel_);

    InitImage(index * WEATHER_24H_PER_8H);
    InitLabel(index * WEATHER_24H_PER_8H);
    return;
}

void WeatherPageHoursGroup::InitImage(int index)
{
    int weatherPosX[WEATHER_24H_PER_8H] = {67, 161, 257, 351, 67, 161, 257, 351};
    int weatherPosY[WEATHER_24H_PER_8H] = {125, 125, 125, 125, 289, 289, 289, 289};
    int backPosX[WEATHER_24H_PER_8H] = {47, 141, 235, 329, 47, 141, 235, 329};
    int backPosY[WEATHER_24H_PER_8H] = {71, 71, 71, 71, 235, 235, 235, 235};
    for (int i = 0; i < WEATHER_24H_PER_8H; i++) {
        imgBack_[i] = new UIImageView();
        if (imgBack_[i] == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "InitImage new imgBack_ fail");
            return;
        }
        imgBack_[i]->SetPosition(backPosX[i], backPosY[i]);
        WeatherModel::GetInstance().LoadImageOfDayNight(imgBack_[i], IMAGE_WEATHER_ITEM3, IMAGE_WEATHER_ITEM4);

        imgWeather_[i] = new UIImageView();
        if (imgWeather_[i] == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "InitImage new labelTime_ fail");
            return;
        }
        imgWeather_[i]->SetPosition(weatherPosX[i], weatherPosY[i]);
        int curWeatherCode_ = std::atoi(houWeather_[index + i].weather);
        LOADIMG::LoadImageViewImg(imgWeather_[i], WEATHER_IMAGE, smallWeatherImage[curWeatherCode_]);
        Add(imgBack_[i]);
        Add(imgWeather_[i]);
    }
}

void WeatherPageHoursGroup::InitLabel(int index)
{
    int timePosX[WEATHER_24H_PER_8H] = {67, 161, 255, 349, 67, 161, 255, 349};
    int timePosY[WEATHER_24H_PER_8H] = {91, 91, 91, 91, 255, 255, 255, 255};
    int tempPosX[WEATHER_24H_PER_8H] = {75, 169, 263, 356, 75, 169, 263, 356};
    int tempPosY[WEATHER_24H_PER_8H] = {175, 175, 175, 175, 339, 339, 339, 339};

    for (int i = 0; i < WEATHER_24H_PER_8H; i++) {
        labelTime_[i] = new UILabel();
        if (labelTime_[i] == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "InitLabel new labelTime_ fail");
            return;
        }
        std::string strHour = houWeather_[index + i].hour;
        labelTime_[i]->SetText(strHour.c_str());
        labelTime_[i]->SetFont(DEFAULT_VECTOR_FONT_FILENAME, WEATHER_LABELTIME_FRONT);
        labelTime_[i]->SetPosition(timePosX[i], timePosY[i], WEATHER_TITLE_WIDTH, WEATHER_TITLE_HEIGHT);
        Add(labelTime_[i]);

        labelTemp_[i] = new UILabel();
        if (labelTemp_[i] == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "InitLabel new labelTemp_ fail");
            return;
        }
        std::ostringstream oss;
        oss << houWeather_[index + i].temp << "°";
        std::string temp = oss.str();
        labelTemp_[i]->SetText(temp.c_str());
        labelTemp_[i]->SetFont(DEFAULT_VECTOR_FONT_FILENAME, WEATHER_LABELTIME_FRONT_TWO);
        labelTemp_[i]->SetPosition(tempPosX[i], tempPosY[i], WEATHER_TITLE_WIDTH, WEATHER_TITLE_HEIGHT);
        Add(labelTemp_[i]);
    }
}
}
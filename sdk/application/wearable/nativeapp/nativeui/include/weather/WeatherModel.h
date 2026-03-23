/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: WeatherModel
 * Create: 2025-03-23
 */

#ifndef WEATHER_MODEL_H
#define WEATHER_MODEL_H

#include <sys/time.h>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <iomanip>
#include <map>
#include <iostream>
#include <random>
#include <chrono>
#include <ctime>
#include <string>
#include "hal_tick.h"
#include "AppViewIDs.h"
#include "UiConfig.h"
#include "cJSON.h"
#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "common/image_cache_manager.h"
#include "main/LoadImg.h"
#include "ui_resource_weather.h"
#ifndef _WIN32
#include "time64.h"
#define WEATHER_PATH "/user/app/user/ace/data/com.vendor.Weather/"
#else
#define WEATHER_PATH APP_RES_PATH
#endif

namespace OHOS {
static constexpr int16_t MIN_TO_HOUR = 60;
static constexpr int16_t HOURLY_SYSTEM = 24;
static constexpr int16_t ALL_HOURS_WEATHER_COUNT = 24;
static constexpr int16_t WEATHER_HOUR = 12;
static constexpr int16_t START_YEAR = 1900;
static constexpr int16_t ARRAY_SIZE = 32;
static constexpr uint16_t WEATHER_ITEM_COUNT = 10;
static constexpr uint16_t ONE_WEEK_DYAS = 7;
static constexpr int16_t INDEX_0 = 0;
static constexpr int16_t INDEX_1 = 1;
static constexpr int16_t INDEX_2 = 2;
static constexpr int16_t INDEX_3 = 3;
static constexpr int16_t INDEX_4 = 4;
static constexpr int16_t INDEX_5 = 5;
static constexpr int16_t INDEX_6 = 6;
static constexpr int16_t INDEX_7 = 7;
static constexpr int16_t INDEX_8 = 8;
static constexpr int16_t INDEX_12 = 12;
static constexpr int16_t NUMBER_10 = 10;

const int bigWeatherImage[ARRAY_SIZE] = {
    IMAGE_WEATHER_WEATHER00, IMAGE_WEATHER_WEATHER01, IMAGE_WEATHER_WEATHER02, IMAGE_WEATHER_WEATHER03,
    IMAGE_WEATHER_WEATHER04, IMAGE_WEATHER_WEATHER05, IMAGE_WEATHER_WEATHER06, IMAGE_WEATHER_WEATHER07,
    IMAGE_WEATHER_WEATHER08, IMAGE_WEATHER_WEATHER09, IMAGE_WEATHER_WEATHER10, IMAGE_WEATHER_WEATHER11,
    IMAGE_WEATHER_WEATHER12, IMAGE_WEATHER_WEATHER13, IMAGE_WEATHER_WEATHER14, IMAGE_WEATHER_WEATHER15,
    IMAGE_WEATHER_WEATHER16, IMAGE_WEATHER_WEATHER17, IMAGE_WEATHER_WEATHER18, IMAGE_WEATHER_WEATHER19,
    IMAGE_WEATHER_WEATHER20, IMAGE_WEATHER_WEATHER21, IMAGE_WEATHER_WEATHER22, IMAGE_WEATHER_WEATHER23,
    IMAGE_WEATHER_WEATHER24, IMAGE_WEATHER_WEATHER25, IMAGE_WEATHER_WEATHER26, IMAGE_WEATHER_WEATHER27,
    IMAGE_WEATHER_WEATHER28, IMAGE_WEATHER_WEATHER29, IMAGE_WEATHER_WEATHER30, IMAGE_WEATHER_WEATHER31};

const int smallWeatherImage[ARRAY_SIZE] = {
    IMAGE_WEATHER_WEATHER32, IMAGE_WEATHER_WEATHER33, IMAGE_WEATHER_WEATHER34, IMAGE_WEATHER_WEATHER35,
    IMAGE_WEATHER_WEATHER36, IMAGE_WEATHER_WEATHER37, IMAGE_WEATHER_WEATHER38, IMAGE_WEATHER_WEATHER39,
    IMAGE_WEATHER_WEATHER40, IMAGE_WEATHER_WEATHER41, IMAGE_WEATHER_WEATHER42, IMAGE_WEATHER_WEATHER43,
    IMAGE_WEATHER_WEATHER44, IMAGE_WEATHER_WEATHER45, IMAGE_WEATHER_WEATHER46, IMAGE_WEATHER_WEATHER47,
    IMAGE_WEATHER_WEATHER48, IMAGE_WEATHER_WEATHER49, IMAGE_WEATHER_WEATHER50, IMAGE_WEATHER_WEATHER51,
    IMAGE_WEATHER_WEATHER52, IMAGE_WEATHER_WEATHER53, IMAGE_WEATHER_WEATHER54, IMAGE_WEATHER_WEATHER55,
    IMAGE_WEATHER_WEATHER56, IMAGE_WEATHER_WEATHER57, IMAGE_WEATHER_WEATHER58, IMAGE_WEATHER_WEATHER59,
    IMAGE_WEATHER_WEATHER60, IMAGE_WEATHER_WEATHER61, IMAGE_WEATHER_WEATHER62, IMAGE_WEATHER_WEATHER63};

typedef struct {
    const char *weatherWeek;
    int image;
    const char *weatherTemp;
    bool hasSecondMenu;
} WeatherItem;

typedef struct {
    const char *min_temp;
    const char *max_temp;
    const char *address;
    const char *weather_code;
    const char *temp;
    const char *updateTime;
    const char *humidity;
    std::string sunSet;
    std::string sunRise;
} CurWeather;

typedef struct {
    const char *hour;
    const char *temp;
    const char *weather;
} HourWeather;

typedef struct {
    const char *sunRise;
    const char *sunSet;
    const char *weekTime;
} SunStruct;

typedef struct {
    const char *monthRise;
    const char *monthSet;
    const char *weekTime;
} MonthStruct;

typedef struct {
    const char *phase;
    int image;
    const char *lightTime;
    const char *monthDay;
    const char *weekTime;
} LunarPhaseStruct;

class WeatherModel {
public:
    static WeatherModel &GetInstance(void);
    virtual ~WeatherModel();
    bool InitWeatherItem();
    void InitMainPageData();
    void InitHoursPageData();
    void InitDaysPageData();
    void UpdateTemp();
    void InitStringData();
    bool InitVecForecast(cJSON *root);
    bool InitVecObserve(cJSON *root);
    bool InitVecRise(cJSON *root);
    bool InitVecWeek(cJSON *root);
    bool InitVecAddress(cJSON *root);
    WeatherItem *GetWeatherlistItems();
    uint8_t GetWeatherListNum();
    CurWeather GetCurWeather();
    SunStruct *GetSunData();
    MonthStruct *GetMonthData();
    LunarPhaseStruct *GetLunarPhaseData();
    std::vector<HourWeather> GetHourWeather();
    void SetDayStatus(const std::string &timeRise, const std::string &timeSet);
    void ClearData();
    int index_ = 0;
    void LoadImageOfDayNight(UIImageView *view, uint32_t resDayId, uint32_t resNightId);
    void LoadTextColorOfDayNight(UILabel *view, uint32_t colorDay, uint32_t colorNight);

private:
    WeatherModel();
    WeatherModel(const WeatherModel &);
    WeatherModel &operator=(const WeatherModel &);
    CurWeather curWeather_{nullptr};
    std::vector<HourWeather> houWeather_;
    WeatherItem weather_[ONE_WEEK_DYAS] = {nullptr, 0, nullptr, false};
    std::string strTmp_[ONE_WEEK_DYAS];
    std::string minTemp_[ONE_WEEK_DYAS];
    std::string maxTemp_[ONE_WEEK_DYAS];
    std::string currentWeek_[ONE_WEEK_DYAS];
    std::string strRise_[ONE_WEEK_DYAS];
    std::string strSet_[ONE_WEEK_DYAS];
    int weatherValue_[ONE_WEEK_DYAS];
    SunStruct sunData_[ONE_WEEK_DYAS] = {nullptr, nullptr, nullptr};
    MonthStruct monthData_[ONE_WEEK_DYAS] = {nullptr, nullptr, nullptr};
    LunarPhaseStruct parseData_[ONE_WEEK_DYAS] = {nullptr, 0, nullptr, nullptr, nullptr};
    std::vector<std::tuple<std::string, std::string, int>> vec_forecast_1h_;
    std::vector<std::tuple<std::string, std::string, int>> vec_forecast_24h_;
    std::map<std::string, std::string> map_observe_;
    std::map<std::string, std::string> map_address_;
    int usedLen_{0};
    bool isDay_{0};
};
}

#endif // Weather_MODEL_H
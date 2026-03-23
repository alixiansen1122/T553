/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: WeatherModel
 * Create: 2025-03-23
 */

#include "ui_resource_weather.h"
#include "wearable_log.h"
#ifdef BUILD_IN_LINUX
#include "localtime.h"
#endif
#include "weather/WeatherPresenter.h"
#include "weather/WeatherModel.h"

namespace OHOS {
static constexpr uint16_t IMAGEANIMATOERVIEW_SIZE = 28;
static const char *WEATHER_FILE = WEATHER_PATH "weather.json";
const int g_imageInfo[IMAGEANIMATOERVIEW_SIZE] = {
    IMAGE_WEATHER_MOON1, IMAGE_WEATHER_MOON2, IMAGE_WEATHER_MOON3, IMAGE_WEATHER_MOON4,
    IMAGE_WEATHER_MOON5, IMAGE_WEATHER_MOON6, IMAGE_WEATHER_MOON7, IMAGE_WEATHER_MOON8,
};

WeatherModel::WeatherModel() {}

WeatherModel::~WeatherModel() {}

WeatherModel &WeatherModel::GetInstance()
{
    static WeatherModel instance;
    return instance;
}

uint8_t WeatherModel::GetWeatherListNum()
{
    return usedLen_;
}

WeatherItem *WeatherModel::GetWeatherlistItems()
{
    if (weather_[0].weatherWeek == nullptr) {
        weather_[INDEX_0] = {"今天", smallWeatherImage[0], "16°/26°", true};
        weather_[INDEX_1] = {"明天", smallWeatherImage[0], "18°/28°", false};
        weather_[INDEX_2] = {"周三", smallWeatherImage[0], "17°/27°", false};
        weather_[INDEX_3] = {"周四", smallWeatherImage[0], "15°/24°", false};
        weather_[INDEX_4] = {"周五", smallWeatherImage[0], "14°/22°", false};
        weather_[INDEX_5] = {"周六", smallWeatherImage[0], "16°/25°", false};
        weather_[INDEX_6] = {"周日", smallWeatherImage[0], "19°/29°", false};
        usedLen_ = ONE_WEEK_DYAS;
    }
    return weather_;
}

CurWeather WeatherModel::GetCurWeather()
{
    if (curWeather_.min_temp == nullptr) { // 如果更新天气失败则输出打桩数据
        curWeather_.min_temp = "10";
        curWeather_.max_temp = "25";
        curWeather_.address = "北京市";
        curWeather_.weather_code = "02";
        curWeather_.temp = "18";
        curWeather_.updateTime = "2023-11-15 14:30:00 20";
        curWeather_.humidity = "90";
        curWeather_.sunSet = "17:30";
        curWeather_.sunRise = "06:45";
    }
    return curWeather_;
}

SunStruct *WeatherModel::GetSunData()
{
    if (sunData_[0].sunRise == nullptr) { // 如果更新天气失败则输出打桩数据
        sunData_[INDEX_0] = {"05:20", "17:43", "今天"};
        sunData_[INDEX_1] = {"06:30", "17:47", "明天"};
        sunData_[INDEX_2] = {"05:20", "17:21", "周三"};
        sunData_[INDEX_3] = {"05:02", "17:24", "周四"};
        sunData_[INDEX_4] = {"06:25", "17:44", "周五"};
        sunData_[INDEX_5] = {"05:21", "17:33", "周六"};
        sunData_[INDEX_6] = {"06:20", "17:12", "周日"};
    }
    return sunData_;
}

MonthStruct *WeatherModel::GetMonthData()
{
    // 无月出数据 打桩数据输出
    monthData_[INDEX_0] = {"月出 18:42", "月落 06:43", "今天"};
    monthData_[INDEX_1] = {"月出 18:32", "月落 05:47", "明天"};
    monthData_[INDEX_2] = {"月出 19:42", "月落 05:21", "周三"};
    monthData_[INDEX_3] = {"月出 18:42", "月落 06:24", "周四"};
    monthData_[INDEX_4] = {"月出 18:45", "月落 05:44", "周五"};
    monthData_[INDEX_5] = {"月出 18:22", "月落 05:33", "周六"};
    monthData_[INDEX_6] = {"月出 18:52", "月落 05:12", "周日"};
    if (!currentWeek_[0].empty()) {
        for (int i = 0; i < ONE_WEEK_DYAS; i++) {
            monthData_[i].weekTime = currentWeek_[i].c_str();
        }
    }
    return monthData_;
}

LunarPhaseStruct *WeatherModel::GetLunarPhaseData()
{
    // 无月出数据 打桩数据输出
    parseData_[INDEX_0] = {"残月", g_imageInfo[INDEX_0], "月亮照亮66%", "月龄23天", "今天"};
    parseData_[INDEX_1] = {"亏凸月", g_imageInfo[INDEX_1], "月亮照亮23%", "月龄21天", "明天"};
    parseData_[INDEX_2] = {"盈凸月", g_imageInfo[INDEX_2], "月亮照亮51%", "月龄23天", "周三"};
    parseData_[INDEX_3] = {"娥眉月", g_imageInfo[INDEX_3], "月亮照亮11%", "月龄13天", "周四"};
    parseData_[INDEX_4] = {"下弦月", g_imageInfo[INDEX_4], "月亮照亮51%", "月龄25天", "周五"};
    parseData_[INDEX_5] = {"上弦月", g_imageInfo[INDEX_5], "月亮照亮80%", "月龄15天", "周六"};
    parseData_[INDEX_6] = {"新月", g_imageInfo[INDEX_6], "月亮照亮23%", "月龄18天", "周日"};
    if (!currentWeek_[0].empty()) {
        for (int i = 0; i < ONE_WEEK_DYAS; i++) {
            parseData_[i].weekTime = currentWeek_[i].c_str();
        }
    }
    return parseData_;
}

std::vector<HourWeather> WeatherModel::GetHourWeather()
{
    if (houWeather_.size() < ALL_HOURS_WEATHER_COUNT) { // 无天气文件或手机下发数据异常不够24个 输出打桩数据
        HourWeather weatherData[] = {
            {"00:00", "18", "04"}, {"01:00", "17", "06"}, {"02:00", "17", "06"}, {"03:00", "16", "06"},
            {"04:00", "16", "07"}, {"05:00", "15", "07"}, {"06:00", "16", "08"}, {"07:00", "17", "08"},
            {"08:00", "18", "04"}, {"09:00", "20", "04"}, {"10:00", "22", "04"}, {"11:00", "24", "04"},
            {"12:00", "25", "04"}, {"13:00", "26", "04"}, {"14:00", "26", "04"}, {"15:00", "25", "06"},
            {"16:00", "24", "06"}, {"17:00", "23", "06"}, {"18:00", "22", "06"}, {"19:00", "21", "07"},
            {"20:00", "20", "07"}, {"21:00", "19", "10"}, {"22:00", "18", "10"}, {"23:00", "18", "10"}};
        for (const auto &item : weatherData) {
            houWeather_.push_back(item);
        }
    }
    return houWeather_;
}

bool WeatherModel::InitWeatherItem()
{
    std::ifstream file(WEATHER_FILE);
    if (!file.is_open()) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "open fail");
        return false;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    int intsize = sizeof(content);
    cJSON *root = nullptr;
    // 解析JSON字符串
    if (intsize != 0) {
        root = cJSON_Parse(content.c_str());
    }
    if (root == NULL) {
        // 如果解析失败，输出错误信息
        const char *error_ptr;
        return false;
    }
    // 访问JSON数据
    bool result = false;
    result = InitVecObserve(root);
    if (!result) {
        return false;
    }
    result = InitVecForecast(root);
    if (!result) {
        return false;
    }
    result = InitVecWeek(root);
    if (!result) {
        return false;
    }
    result = InitVecAddress(root);
    if (!result) {
        return false;
    }
    InitStringData();
    result = InitVecRise(root);
    if (!result) {
        return false;
    }
    // 暂无月出、月相文件数据，后续增加相关字段后，需要自行加解析
    cJSON_Delete(root);

    InitMainPageData();
    InitHoursPageData();
    InitDaysPageData();
    return true;
}

void WeatherModel::InitMainPageData()
{
    curWeather_ = {0};
    static std::string sunset;
    static std::string sunrise;
    if (map_observe_.find("min_temp") != map_observe_.end()) {
        curWeather_.min_temp = map_observe_["min_temp"].c_str();
    }
    if (map_observe_.find("max_temp") != map_observe_.end()) {
        curWeather_.max_temp = map_observe_["max_temp"].c_str();
    }
    if (map_observe_.find("temp") != map_observe_.end()) {
        curWeather_.temp = map_observe_["temp"].c_str();
    }
    if (map_observe_.find("weather_code") != map_observe_.end()) {
        curWeather_.weather_code = map_observe_["weather_code"].c_str();
    }
    if (map_observe_.find("update_time") != map_observe_.end()) {
        curWeather_.updateTime = map_observe_["update_time"].c_str();
    }
    if (map_observe_.find("humidity") != map_observe_.end()) {
        curWeather_.humidity = map_observe_["humidity"].c_str();
    }
    if (map_address_.find("city") != map_address_.end()) {
        curWeather_.address = map_address_["city"].c_str();
    }
    if (map_address_.find("address") != map_address_.end()) {
        curWeather_.address = map_address_["address"].c_str();
    }
}

void WeatherModel::InitHoursPageData()
{
    houWeather_.clear();
    static std::string dayTime[ALL_HOURS_WEATHER_COUNT];
    static std::string dayTemp[ALL_HOURS_WEATHER_COUNT];
    static std::string dayWeather[ALL_HOURS_WEATHER_COUNT];

    for (const auto &item : vec_forecast_1h_) {
        const std::string name = std::get<0>(item);
        const std::string category = std::get<1>(item);
        const int id = std::get<INDEX_2>(item);
        HourWeather sample = {nullptr};
        if (name == "update_time") {
            dayTime[id] = category;
        } else if (name == "temp") {
            dayTemp[id] = category;
        } else if (name == "weather_code") {
            dayWeather[id] = category;
        }
    }
    for (int i = 0; i < ALL_HOURS_WEATHER_COUNT; i++) {
        HourWeather sample = {nullptr};
        sample.hour = dayTime[i].c_str();
        sample.temp = dayTemp[i].c_str();
        sample.weather = dayWeather[i].c_str();
        houWeather_.push_back(sample);
    }
}

void WeatherModel::InitDaysPageData()
{
    for (int i = 0; i < ONE_WEEK_DYAS; i++) {
        strTmp_[i] = maxTemp_[i] + "° / " + minTemp_[i] + "°";
    }
    for (int i = 0; i < ONE_WEEK_DYAS; i++) {
        int weatherIndex = weatherValue_[i];
        weather_[i] = {currentWeek_[i].c_str(), smallWeatherImage[weatherIndex], strTmp_[i].c_str(), true};
    }
    usedLen_ = ONE_WEEK_DYAS;
}

void WeatherModel::InitStringData()
{
    for (const auto &item : vec_forecast_24h_) {
        const std::string name = std::get<0>(item);
        const std::string value = std::get<1>(item);
        const int id = std::get<INDEX_2>(item);
        if (name == "current_week") {
            if (id == INDEX_0) {
                currentWeek_[id] = "今天";
            } else if (id == INDEX_1) {
                currentWeek_[id] = "明天";
            } else {
                currentWeek_[id] = value;
            }
        } else if (name == "min_temp") {
            minTemp_[id] = value;
        } else if (name == "max_temp") {
            maxTemp_[id] = value;
        } else if (name == "day_weather_code") {
            weatherValue_[id] = std::atoi(value.c_str());
        }
    }
}

bool WeatherModel::InitVecForecast(cJSON *root)
{
    vec_forecast_1h_.clear();
    cJSON *forecast_1h = cJSON_GetObjectItemCaseSensitive(root, "forecast_1h");
    if (!cJSON_IsObject(forecast_1h)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Error forecast_1h:");
        cJSON_Delete(root);
        return false;
    }
    for (int i = 0; i < ALL_HOURS_WEATHER_COUNT; i++) {
        cJSON *forecast_1h_num = cJSON_GetObjectItemCaseSensitive(forecast_1h, std::to_string(i).c_str());
        if (!cJSON_IsObject(forecast_1h_num)) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Error forecast_1h_num:%d", i);
            continue;
        }
        std::string str = cJSON_GetObjectItemCaseSensitive(forecast_1h_num, "update_time")->valuestring;
        std::string hour = str.substr(INDEX_12, INDEX_2);
        if (str.length() >= WEATHER_HOUR) {
            hour = str.substr(INDEX_8, INDEX_2) + ":00";
        } else {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Error forecast_1h hour size error");
            hour = "00:00";
        }
        vec_forecast_1h_.push_back(std::make_tuple("update_time", hour, i));
        vec_forecast_1h_.push_back(
            std::make_tuple("temp", cJSON_GetObjectItemCaseSensitive(forecast_1h_num, "degree")->valuestring, i));
        vec_forecast_1h_.push_back(std::make_tuple(
            "weather_code", cJSON_GetObjectItemCaseSensitive(forecast_1h_num, "weather_code")->valuestring, i));
    }
    // 由外层函数释放cjson嵌套结构体内存
    return true;
}

bool WeatherModel::InitVecObserve(cJSON *root)
{
    map_observe_.clear();
    // 获取 observe 对象
    cJSON *observe = cJSON_GetObjectItemCaseSensitive(root, "observe");
    if (!cJSON_IsObject(observe)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Error observe:");
        cJSON_Delete(root);
        return false;
    }
    map_observe_.insert(std::make_pair("weather", cJSON_GetObjectItemCaseSensitive(observe, "weather")->valuestring));
    map_observe_.insert(std::make_pair("temp", cJSON_GetObjectItemCaseSensitive(observe, "degree")->valuestring));
    map_observe_.insert(std::make_pair("humidity", cJSON_GetObjectItemCaseSensitive(observe, "humidity")->valuestring));
    map_observe_.insert(
        std::make_pair("min_temp", cJSON_GetObjectItemCaseSensitive(observe, "min_degree")->valuestring));
    map_observe_.insert(
        std::make_pair("max_temp", cJSON_GetObjectItemCaseSensitive(observe, "max_degree")->valuestring));
    map_observe_.insert(
        std::make_pair("update_time", cJSON_GetObjectItemCaseSensitive(observe, "update_time")->valuestring));
    map_observe_.insert(
        std::make_pair("weather_code", cJSON_GetObjectItemCaseSensitive(observe, "weather_code")->valuestring));
    // 由外层函数释放cjson嵌套结构体内存
    return true;
}

bool WeatherModel::InitVecRise(cJSON *root)
{
    // 获取 rise 对象
    cJSON *rise = cJSON_GetObjectItemCaseSensitive(root, "rise");
    if (!cJSON_IsObject(rise)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Error rise:");
        cJSON_Delete(root);
        return false;
    }
    for (int i = 0; i < ONE_WEEK_DYAS; i++) {
        cJSON *rise_num = cJSON_GetObjectItemCaseSensitive(rise, std::to_string(i).c_str());
        if (!cJSON_IsObject(rise_num)) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Error rise:%d", i);
            continue;  // 跳过当前循环，继续处理下一个
        }
        strRise_[i] = cJSON_GetObjectItemCaseSensitive(rise_num, "sunrise")->valuestring;
        strSet_[i] = cJSON_GetObjectItemCaseSensitive(rise_num, "sunset")->valuestring;
        sunData_[i].sunRise = strRise_[i].c_str();
        sunData_[i].sunSet = strSet_[i].c_str();
        sunData_[i].weekTime = currentWeek_[i].c_str();
    }
    if (sunData_[0].sunRise != nullptr) {
        SetDayStatus(sunData_[0].sunRise, sunData_[0].sunSet);
    } else {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "sunData_ is empty");
        cJSON_Delete(root);
        return false;
    }
    // 由外层函数释放cjson嵌套结构体内存
    return true;
}

bool WeatherModel::InitVecWeek(cJSON *root)
{
    vec_forecast_24h_.clear();
    cJSON *forecast_24h = cJSON_GetObjectItemCaseSensitive(root, "forecast_24h");
    if (!cJSON_IsObject(forecast_24h)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Error forecast_24h:");
        cJSON_Delete(root);
        return false;
    }
    // app写的数据包含了昨天，跳过第一个数据
    for (int i = 1; i <= ONE_WEEK_DYAS; i++) {
        cJSON *forecast_24h_num = cJSON_GetObjectItemCaseSensitive(forecast_24h, std::to_string(i).c_str());
        if (!cJSON_IsObject(forecast_24h_num)) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Error forecast_24h_num:%d", i);
            continue;
        }
        std::istringstream iss(cJSON_GetObjectItemCaseSensitive(forecast_24h_num, "time")->valuestring);
        char dash1;
        char dash2;
        int year = 0;
        int month = 0;
        int day = 0;
        const int start_year = START_YEAR;
        if ((iss >> year >> dash1 >> month >> dash2 >> day) && (dash1 == '-') && (dash2 == '-')) {
            std::tm timeinfo = {};
            timeinfo.tm_year = year - start_year;
            timeinfo.tm_mon = month - 1;
            timeinfo.tm_mday = day;
            timeinfo.tm_hour = 0;
            timeinfo.tm_min = 0;
            timeinfo.tm_sec = 0;
            timeinfo.tm_isdst = -1;
            std::time_t timestamp = mktime(&timeinfo);
            if (timestamp == -1) {
                cJSON_Delete(root);
                return false;
            }
            std::tm localTimeInfo;
            if (localtime_r(&timestamp, &localTimeInfo) == nullptr) {
                cJSON_Delete(root);
                return false;
            }
            const char *current_weeks[] = {"周日", "周一", "周二", "周三", "周四", "周五", "周六"};
            vec_forecast_24h_.push_back(std::make_tuple("current_week", current_weeks[localTimeInfo.tm_wday], i - 1));
            vec_forecast_24h_.push_back(std::make_tuple(
                "min_temp", cJSON_GetObjectItemCaseSensitive(forecast_24h_num, "min_degree")->valuestring, i - 1));
            vec_forecast_24h_.push_back(std::make_tuple(
                "max_temp", cJSON_GetObjectItemCaseSensitive(forecast_24h_num, "max_degree")->valuestring, i - 1));
            vec_forecast_24h_.push_back(std::make_tuple(
                "day_weather_code", cJSON_GetObjectItemCaseSensitive(forecast_24h_num, "day_weather_code")->valuestring,
                i - 1));
        } else {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Error Date");
        }
    }
    // 由外层函数释放cjson嵌套结构体内存
    return true;
}

bool WeatherModel::InitVecAddress(cJSON *root)
{
    map_address_.clear();
    cJSON *address = cJSON_GetObjectItemCaseSensitive(root, "address");
    if (!cJSON_IsObject(address)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Error address:");
        cJSON_Delete(root);
        return false;
    }
    map_address_.insert(std::make_pair("province", cJSON_GetObjectItemCaseSensitive(address, "province")->valuestring));
    map_address_.insert(std::make_pair("city", cJSON_GetObjectItemCaseSensitive(address, "city")->valuestring));
    map_address_.insert(std::make_pair("county", cJSON_GetObjectItemCaseSensitive(address, "county")->valuestring));
    // 由外层函数释放cjson嵌套结构体内存
    return true;
}

void WeatherModel::SetDayStatus(const std::string &timeRise, const std::string &timeSet)
{
    std::time_t now = std::time(nullptr);
    std::tm *local = std::localtime(&now);
    uint8_t realHour = static_cast<uint8_t>(local->tm_hour);
    uint8_t realMinute = static_cast<uint8_t>(local->tm_min);
    int currentTime = realHour * MIN_TO_HOUR + realMinute;
    int hours = (timeRise[INDEX_0] - '0') * NUMBER_10 + (timeRise[INDEX_1] - '0');
    int minutes = (timeRise[INDEX_3] - '0') * NUMBER_10 + (timeRise[INDEX_4] - '0');
    int rise = hours * MIN_TO_HOUR + minutes;
    hours = (timeSet[INDEX_0] - '0') * NUMBER_10 + (timeSet[INDEX_1] - '0');
    minutes = (timeSet[INDEX_3] - '0') * NUMBER_10 + (timeSet[INDEX_4] - '0');
    int set = hours * MIN_TO_HOUR + minutes;
    isDay_ = (currentTime >= rise && currentTime < set) ? true : false;
}

void WeatherModel::LoadImageOfDayNight(UIImageView *view, uint32_t resDayId, uint32_t resNightId)
{
    if (isDay_) {
        LOADIMG::LoadImageViewImg(view, WEATHER_IMAGE, resDayId);
    } else {
        LOADIMG::LoadImageViewImg(view, WEATHER_IMAGE, resNightId);
    }
}

void WeatherModel::LoadTextColorOfDayNight(UILabel *view, uint32_t colorDay, uint32_t colorNight)
{
    if (isDay_) {
        view->SetStyle(STYLE_TEXT_COLOR, colorDay);
    } else {
        view->SetStyle(STYLE_TEXT_COLOR, colorNight);
    }
}

void WeatherModel::ClearData()
{
    std::remove(WEATHER_FILE);
}
}

/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: WeatherMainPage
 * Create: 2025-03-23
 */

#include <iomanip>
#include <sstream>
#include <string>
#include "NativeAbility.h"
#include "SlicePageFactory.h"
#include "common/image_cache_manager.h"
#include "bts_br_gap.h"
#include "bts_spp.h"
#include "msg_center_device.h"
#include "components/ui_list_nested.h"
#include "components/ui_scroll_view_nested.h"
#include "ui_resource_weather.h"
#include "wearable_log.h"
#include "weather/WeatherModel.h"
#include "weather/WeatherPresenter.h"
#include "weather/WeatherMainPage.h"

static constexpr int16_t WEATHER_LABEL_SIZE_WIDTH_ONE = 250;
static constexpr int16_t WEATHER_LABEL_SIZE_HEIGHT_ONE = 60;
static constexpr int16_t WEATHER_LABEL_SIZE_WIDTH_FOUR = 210;
static constexpr int16_t WEATHER_LABEL_SIZE_HEIGHT_FOUR = 40;
static constexpr int16_t WEATHER_LABEL_SIZE_WIDTH_FIVE = 223;
static constexpr int16_t WEATHER_LABEL_SIZE_HEIGHT_FIVE = 46;
static constexpr int16_t WEATHER_LABEL_SIZE_WIDTH_SIX = 149;
static constexpr int16_t WEATHER_LABEL_SIZE_HEIGHT_SIX = 20;
static constexpr int16_t WEATHER_LABEL_FRONT_ONE = 40;
static constexpr int16_t WEATHER_LABEL_FRONT_TWO = 24;
static constexpr int16_t WEATHER_LABEL_FRONT_THREE = 36;
static constexpr int16_t WEATHER_LABEL_FRONT_FOUR = 120;
static constexpr int16_t WEATHER_LABEL_FRONT = 22;
static constexpr int16_t WEATHER_AROUND_X = 49;
static constexpr int16_t WEATHER_AROUND_Y = 323;
static constexpr int16_t WEATHER_AROUND_WIDTH = 198;
static constexpr int16_t WEATHER_AROUND_HEIGHT = 40;
static constexpr int16_t WEATHER_CODE_POSITION_X = 267;
static constexpr int16_t WEATHER_CODE_POSITION_Y = 268;
static constexpr int16_t WEATHER_CODE_POSITION_WIDTH = 72;
static constexpr int16_t WEATHER_CODE_POSITION_HEIGHT = 46;
static constexpr int16_t WEATHER_TEMP_POSITION_X = 210;
static constexpr int16_t WEATHER_TEMP_POSITION_Y = 130;
static constexpr int16_t WEATHER_TEMP_WIDTH = 190;
static constexpr int16_t WEATHER_TEMP_HEIGHT = 120;
static constexpr int16_t WEATHER_IMAGE_WIDTH = 78;
static constexpr int16_t WEATHER_IMAGE_HEIGHT = 115;
static constexpr int16_t WEATHER_IMAGEAIR_X = 49;
static constexpr int16_t WEATHER_IMAGEAIR_Y = 323;
static constexpr int16_t WEATHER_IMAGEAIR_DATA = 40;
static constexpr int16_t WEATHER_IMAGEHUMIDITY_X = 357;
static constexpr int16_t WEATHER_IMAGEHUMIDITY_Y = 323;
static constexpr int16_t WEATHER_LABLEAIR_DATA = 79;
static constexpr int16_t WEATHER_LABLEAIR_HEIGHT = 30;
static constexpr int16_t WEATHER_IMAGEHUMIDITY_DATA = 120;
static constexpr int16_t WEATHER_IMG_X = 71;
static constexpr int16_t WEATHER_IMAGEPAGEKEY_X = 199;
static constexpr int16_t WEATHER_IMAGEPAGEKEY_Y = 429;
static constexpr int16_t WEATHER_IMAGEPAGEKEY_WIDTH = 56;
static constexpr int16_t WEATHER_IMAGEPAGEKEY_HEIGHT = 10;
static constexpr int16_t WEATHER_LABELTITLE_POSITION_X = 63;
static constexpr int16_t WEATHER_LABELTITLE_POSITION_Y = 100;
static constexpr int16_t WEATHER_LABELTITLE_WIDTH = 400;
static constexpr int16_t WEATHER_ADDRESS_X = 143;
static constexpr int16_t WEATHER_ADDRESS_Y = 49;
static constexpr int16_t WEATHER_TYPE = 32;
static constexpr int16_t BUFFER_SIZE = 6;

const char *g_weatherNames[] = {"晴", "多云", "阴", "阵雨", "雷阵雨", "雷阵雨伴有冰雹", "雨夹雪",
                                "小雨", "中雨", "大雨", "暴雨", "大暴雨", "特大暴雨", "阵雪",
                                "小雪", "中雪", "大雪", "暴雪", "雾", "冻雨", "沙尘暴",
                                "小到中雨", "中到大雨", "大到暴雨", "小到中雪", "中到大雪",
                                "大到暴雪", "浮尘", "扬沙", "强沙尘暴", "霾", "无天气数据"};

namespace OHOS {
REGIST_SLICE_PAGE(VIEW_WEATHER, WEATHER_PAGES::WEATHER_MAIN_PAGE, WeatherMainPage, true);
static WeatherMainPage *g_pWeatherMainPage = nullptr;

WeatherMainPage::WeatherMainPage()
{
    g_pWeatherMainPage = this;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "WeatherMainPage::WeatherMainPage");
}

WeatherMainPage::~WeatherMainPage()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "WeatherMainPage::~WeatherMainPage");
    g_pWeatherMainPage = nullptr;
    if (group_ != nullptr) {
        group_->RemoveAll();
        delete group_;
        group_ = nullptr;
    }

    for (int i = 0; i < LABEL_COUNT; i++) {
        if (title_[i] != nullptr) {
            delete title_[i];
            title_[i] = nullptr;
        }
    }

    if (labelTitle_ != nullptr) {
        delete labelTitle_;
        labelTitle_ = nullptr;
    }

    if (addressImg_ != nullptr) {
        delete addressImg_;
        addressImg_ = nullptr;
    }

    if (weatherImg_ != nullptr) {
        delete weatherImg_;
        weatherImg_ = nullptr;
    }

    if (bgImg_ != nullptr) {
        delete bgImg_;
        bgImg_ = nullptr;
    }

    if (airQualityAndHumidityBgImg_ != nullptr) {
        delete airQualityAndHumidityBgImg_;
        airQualityAndHumidityBgImg_ = nullptr;
    }

    if (airQulityImgIcon_ != nullptr) {
        delete airQulityImgIcon_;
        airQulityImgIcon_ = nullptr;
    }

    if (humidityImgIcon_ != nullptr) {
        delete humidityImgIcon_;
        humidityImgIcon_ = nullptr;
    }

    if (pageIdxImg_ != nullptr) {
        delete pageIdxImg_;
        pageIdxImg_ = nullptr;
    }
    ImageCacheManager::GetInstance().UnloadAllInMultiRes(WEATHER_IMAGE);
}

WeatherMainPage *WeatherMainPage::GetInstance()
{
    return g_pWeatherMainPage;
}

void WeatherMainPage::OnStart(void *data)
{
    group_ = new UIViewGroup();
    if (group_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "WeatherMainPage::group_ fail");
        return;
    }
    group_->SetPosition(0, 0);
    group_->Resize(HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    group_->SetOnDragListener(this);
    group_->SetViewId("weather");
    group_->SetDraggable(true);
    group_->SetTouchable(true);
    flag_ = WeatherModel::GetInstance().InitWeatherItem();
    cur_ = WeatherModel::GetInstance().GetCurWeather();
    bgImg_ = new UIImageView();
    if (bgImg_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "WeatherMainPage::bgImg_ new image fail");
        return;
    }
    bgImg_->SetPosition(0, 0);
    bgImg_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    WeatherModel::GetInstance().LoadImageOfDayNight(bgImg_, IMAGE_WEATHER_BG1, IMAGE_WEATHER_BG2);

    group_->Add(bgImg_);
    InitWeatherImage1();
    InitWeatherImage2();
    InitWeatherLabel1();
    InitWeatherLabel2();

    labelTitle_ = new UILabel();
    if (labelTitle_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "WeatherMainPage::labelTitle_ new label fail");
        return;
    }
    if (!flag_) {
        labelTitle_->SetPosition(WEATHER_LABELTITLE_POSITION_X, WEATHER_LABELTITLE_POSITION_Y, WEATHER_LABELTITLE_WIDTH,
                                 WEATHER_LABEL_SIZE_HEIGHT_ONE);
        labelTitle_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, WEATHER_LABEL_FRONT);
        labelTitle_->SetText("天气未更新，请连接手机更新数据");
        labelTitle_->SetVisible(true);
        group_->Add(labelTitle_);
    }
    AddViewToPageContainer(group_);
    if (GetConnectStatus()) {
        errcode_t result;
        uint8_t value = 1; // 更新数据实际不需要使用value 手机侧不关心该字段
#ifdef _WIN32
        result = 0;
#else
        result = msg_center_send_data(MSGCENTER_CMD_DEVICE, MSGCENTER_TYPE_ID_SYNC_WEATHER_REQ, &value, sizeof(value));
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "WeatherMainPage msg_center_send_data result:%d ", result);
#endif
    }
}

void WeatherMainPage::ReloadPage()
{
    flag_ = WeatherModel::GetInstance().InitWeatherItem();
    cur_ = WeatherModel::GetInstance().GetCurWeather();
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "WeatherMainPage::flag_:%d", flag_);
    if (!flag_) {
        labelTitle_->SetVisible(true);
    } else {
        labelTitle_->SetVisible(false);
    }
    WeatherModel::GetInstance().LoadImageOfDayNight(bgImg_, IMAGE_WEATHER_BG1, IMAGE_WEATHER_BG2);
    WeatherModel::GetInstance().LoadImageOfDayNight(airQualityAndHumidityBgImg_,
                                                  IMAGE_WEATHER_ITEM5, IMAGE_WEATHER_ITEM6);

    curWeatherCode_ = std::atoi(cur_.weather_code);
    LOADIMG::LoadImageViewImg(weatherImg_, WEATHER_IMAGE, bigWeatherImage[curWeatherCode_]);

    for (int i = 0; i < LABEL_COUNT; i++) {
        std::ostringstream oss;
        if (i == LABEL_ADDRESS) {
            title_[i]->SetText(cur_.address);
            int controlOffset = 10;
            int cut = 2;
            int posX = (VERTICAL_RESOLUTION - addressImg_->GetWidth() -
                        title_[i]->GetTextWidth() - controlOffset) / cut;
            addressImg_->SetX(posX);
            posX = posX + addressImg_->GetWidth() + controlOffset;
            title_[i]->SetX(posX);
        } else if (i == LABEL_TEMP) {
            oss << cur_.max_temp << "° / " << cur_.min_temp << "°  " << g_weatherNames[curWeatherCode_];
            std::string weatherTemp = oss.str();
            title_[i]->SetText(weatherTemp.c_str());
        } else if (i == LABEL_AIR) {
            oss << "空气   --";
            std::string weatherAir = oss.str();
            title_[i]->SetText(weatherAir.c_str());
        } else if (i == LABEL_HUMIDITY) {
            oss << "湿度 " << cur_.humidity << "%";
            std::string weatherHumidity = oss.str();
            title_[i]->SetText(weatherHumidity.c_str());
        } else if (i == LABEL_UPDATETIME) {
            std::string updateTime = FormatTime(cur_.updateTime);
            oss << "上次更新时间 " << updateTime;
            updateTime = oss.str();
            title_[i]->SetText(updateTime.c_str());
        } else if (i == LABEL_CURRENT_TEMP) {
            oss << cur_.temp << "°";
            std::string weatherTemp = oss.str();
            title_[i]->SetText(weatherTemp.c_str());
        }
    }
}

void WeatherMainPage::InitWeatherImage1()
{
    addressImg_ = new UIImageView();
    if (addressImg_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "WeatherMainPage::addressImg_ new image fail");
        return;
    }
    addressImg_->SetPosition(WEATHER_ADDRESS_X, WEATHER_ADDRESS_Y);
    addressImg_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    curWeatherCode_ = std::atoi(cur_.weather_code);
    LOADIMG::LoadImageViewImg(addressImg_, WEATHER_IMAGE, IMAGE_WEATHER_LOCATE);

    weatherImg_ = new UIImageView();
    if (weatherImg_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "WeatherMainPage::weatherImg_ new image fail");
        return;
    }
    weatherImg_->SetPosition(WEATHER_IMG_X, WEATHER_TEMP_POSITION_Y);
    weatherImg_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    curWeatherCode_ = std::atoi(cur_.weather_code);
    if (curWeatherCode_ < 0 || curWeatherCode_ > WEATHER_TYPE) {
        curWeatherCode_ = 0;
    }
    LOADIMG::LoadImageViewImg(weatherImg_, WEATHER_IMAGE, bigWeatherImage[curWeatherCode_]);
    group_->Add(addressImg_);
    group_->Add(weatherImg_);
}

void WeatherMainPage::InitWeatherImage2()
{
    airQualityAndHumidityBgImg_ = new UIImageView();
    if (airQualityAndHumidityBgImg_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "WeatherMainPage::aroundImage new image fail");
        return;
    }
    airQualityAndHumidityBgImg_->SetPosition(WEATHER_AROUND_X, WEATHER_AROUND_Y);
    WeatherModel::GetInstance().LoadImageOfDayNight(airQualityAndHumidityBgImg_,
                                                  IMAGE_WEATHER_ITEM5, IMAGE_WEATHER_ITEM6);
    airQulityImgIcon_ = new UIImageView();
    if (airQulityImgIcon_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "WeatherMainPage::airQulityImgIcon_ new image fail");
        return;
    }
    airQulityImgIcon_->SetPosition(WEATHER_IMAGEAIR_X, WEATHER_IMAGEAIR_Y);
    airQulityImgIcon_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    LOADIMG::LoadImageViewImg(airQulityImgIcon_, WEATHER_IMAGE, IMAGE_WEATHER_AIR);
    humidityImgIcon_ = new UIImageView(); // humidityImgIcon_->humidityImgIconIcon
    if (humidityImgIcon_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "WeatherMainPage::humidityImgIcon_ new image fail");
        return;
    }
    humidityImgIcon_->SetPosition(WEATHER_IMAGEHUMIDITY_X, WEATHER_IMAGEHUMIDITY_Y);
    humidityImgIcon_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    LOADIMG::LoadImageViewImg(humidityImgIcon_, WEATHER_IMAGE, IMAGE_WEATHER_WET);
    pageIdxImg_ = new UIImageView();
    if (pageIdxImg_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "WeatherMainPage::pageIdxImg_ new image fail");
        return;
    }
    pageIdxImg_->SetPosition(WEATHER_IMAGEPAGEKEY_X, WEATHER_IMAGEPAGEKEY_Y);
    pageIdxImg_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    LOADIMG::LoadImageViewImg(pageIdxImg_, WEATHER_IMAGE, IMAGE_WEATHER_PAGE1);
    group_->Add(airQualityAndHumidityBgImg_);
    group_->Add(airQulityImgIcon_);
    group_->Add(humidityImgIcon_);
    group_->Add(pageIdxImg_);
}

void WeatherMainPage::InitWeatherLabel1()
{
    int posX[LABEL_COUNT] = {140, 113, 119, 232, 100, 210};
    int posY[LABEL_COUNT] = {34, 274, 332, 332, 376, 130};
    for (int i = 0; i < LABEL_HUMIDITY; i++) { // 从adress控件加载到humidity控件
        std::ostringstream oss;
        title_[i] = new UILabel();
        if (title_[i] == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "WeatherMainPage new label:%d fail", i);
            return;
        }
        if (i == LABEL_ADDRESS) {
            title_[i]->SetText(cur_.address);
            title_[i]->SetPosition(posX[i], posY[i], WEATHER_LABEL_SIZE_WIDTH_ONE, WEATHER_LABEL_SIZE_HEIGHT_ONE);
            title_[i]->SetLineBreakMode(UILabel::LINE_BREAK_ADAPT);
            title_[i]->SetFont(BOLD_VECTOR_FONT_FILENAME, WEATHER_LABEL_FRONT_ONE);
            int controlOffset = 10;
            int cut = 2;
            int posX = (VERTICAL_RESOLUTION - addressImg_->GetWidth() - title_[i]->GetTextWidth()
                        - controlOffset) / cut;
            addressImg_->SetX(posX);
            posX = posX + addressImg_->GetWidth() + controlOffset;
            title_[i]->SetX(posX);
        } else if (i == LABEL_TEMP) {
            oss << cur_.max_temp << "° / " << cur_.min_temp << "°  " << g_weatherNames[curWeatherCode_];
            std::string weatherTemp = oss.str();
            title_[i]->SetText(weatherTemp.c_str());
            title_[i]->SetPosition(posX[i], posY[i], WEATHER_LABEL_SIZE_WIDTH_FIVE, WEATHER_LABEL_SIZE_HEIGHT_FIVE);
            title_[i]->SetFont(DEFAULT_VECTOR_FONT_FILENAME, WEATHER_LABEL_FRONT_THREE);
        } else if (i == LABEL_AIR) {
            oss << "空气   --";
            std::string weatherAir = oss.str();
            title_[i]->SetText(weatherAir.c_str());
            title_[i]->SetLineBreakMode(UILabel::LINE_BREAK_ADAPT);
            title_[i]->SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
            title_[i]->SetPosition(posX[i], posY[i], WEATHER_LABLEAIR_DATA, WEATHER_LABLEAIR_HEIGHT);
            title_[i]->SetFont(DEFAULT_VECTOR_FONT_FILENAME, WEATHER_LABEL_FRONT_TWO);
        }
        title_[i]->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
        group_->Add(title_[i]);
    }
}

void WeatherMainPage::InitWeatherLabel2()
{
    int posX[LABEL_COUNT] = {140, 113, 119, 232, 100, 210};
    int posY[LABEL_COUNT] = {34, 274, 332, 332, 376, 130};
    for (int i = LABEL_HUMIDITY; i < LABEL_COUNT; i++) { // 从HUMIDITY控件开始到加载剩余label
        std::ostringstream oss;
        title_[i] = new UILabel();
        if (title_[i] == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "WeatherMainPage new label:%d fail", i);
            return;
        }
        if (i == LABEL_HUMIDITY) {
            title_[i]->SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
            title_[i]->SetPosition(posX[i], posY[i], WEATHER_IMAGEHUMIDITY_DATA, WEATHER_LABLEAIR_HEIGHT);
            title_[i]->SetFont(DEFAULT_VECTOR_FONT_FILENAME, WEATHER_LABEL_FRONT_TWO);
            oss << "湿度 " << cur_.humidity << "%";
            std::string weatherHumidity = oss.str();
            title_[i]->SetText(weatherHumidity.c_str());
        } else if (i == LABEL_UPDATETIME) {
            std::string updateTime = FormatTime(cur_.updateTime);
            oss << "上次更新时间 " << updateTime;
            updateTime = oss.str();
            title_[i]->SetText(updateTime.c_str());
            title_[i]->SetPosition(posX[i], posY[i], WEATHER_LABEL_SIZE_WIDTH_ONE, WEATHER_LABEL_SIZE_HEIGHT_FOUR);
            title_[i]->SetFont(DEFAULT_VECTOR_FONT_FILENAME, WEATHER_LABEL_FRONT_TWO);
        } else if (i == LABEL_CURRENT_TEMP) {
            oss << cur_.temp << "°";
            std::string weatherTemp = oss.str();
            title_[i]->SetText(weatherTemp.c_str());
            title_[i]->SetPosition(posX[i], posY[i], WEATHER_TEMP_WIDTH, WEATHER_TEMP_HEIGHT);
            title_[i]->SetFont(DEFAULT_VECTOR_FONT_FILENAME, WEATHER_LABEL_FRONT_FOUR);
        }
        title_[i]->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
        group_->Add(title_[i]);
    }
}

std::string WeatherMainPage::FormatTime(const std::string &inputStr)
{
    if (!flag_) {
        return "--";
    }
    tm inputTm = {};
    std::istringstream iss(inputStr);
    iss >> std::get_time(&inputTm, "%Y%m%d%H%M");
    time_t inputTime = mktime(&inputTm);
    time_t now = time(nullptr);
    tm *nowTm = localtime(&now);
    if (inputTm.tm_year == nowTm->tm_year && inputTm.tm_mon == nowTm->tm_mon && inputTm.tm_mday == nowTm->tm_mday) {
        char buffer[BUFFER_SIZE];
        strftime(buffer, sizeof(buffer), "%H:%M", &inputTm);
        return std::string(buffer);
    } else {
        char buffer[BUFFER_SIZE];
        strftime(buffer, sizeof(buffer), "%m/%d", &inputTm);
        return std::string(buffer);
    }
}

bool WeatherMainPage::OnDrag(UIView &view, const DragEvent &event)
{
    UNUSED(view);
    if ((event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) && (event.GetDeltaX() > X_DRAG_OFFSET) &&
        (event.GetDeltaX() > (DOUBLES * MATH_ABS(event.GetDeltaY())))) {
        NativeAbility::GetInstance().ChangeSliceToApplist();
    } else if (event.GetDragDirection() == DragEvent::DIRECTION_RIGHT_TO_LEFT) {
        NativeAbility::GetInstance().SwitchPageInSlice(WEATHER_PAGES::WEATHER_HOURS_PAGE,
            TransitionType::TRANSITION_INVALID, false);
    } else if (event.GetDragDirection() == DragEvent::DIRECTION_BOTTOM_TO_TOP) {
        NativeAbility::GetInstance().SwitchPageInSlice(WEATHER_PAGES::WEATHER_SUNRISE_PAGE,
            TransitionType::TRANSITION_INVALID, false);
    }
    return true;
}

bool WeatherMainPage::OnClick(UIView &view, const ClickEvent &event)
{
    UNUSED(event);
    return true;
}

uint8_t WeatherMainPage::GetConnectStatus()
{
    uint8_t conn_status = 0;
#ifdef _WIN32
    conn_status = 1;
#else
    if (msg_center_get_trans_channel() == DIAG_FRAME_FID_PHONE) {
        conn_status = is_spp_connected(0) == true ? 1 : 0;
    } else if (msg_center_get_trans_channel() == DIAG_FRAME_FID_BT) {
        conn_status = msg_center_get_ble_conn_state();
    }
#endif
    return conn_status;
}
}

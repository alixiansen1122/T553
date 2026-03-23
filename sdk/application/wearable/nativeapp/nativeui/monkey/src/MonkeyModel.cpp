/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: MonkeyModel
 * Create: 2025-04-24
 */
#include <algorithm>
#include <random>
#include "AppViewIDs.h"
#include "UiConfig.h"
#include "NativeAbility.h"
#include "flashlight/FlashLightView.h"
#include "phonemenu/PhoneMenuView.h"
#include "stopwatch/StopwatchView.h"
#include "sport/SportView.h"
#include "timers/TimerView.h"
#include "weather/WeatherView.h"
#include "blood/BloodView.h"
#include "heartrate/HeartRateView.h"
#include "camera/CameraView.h"
#include "findphone/FindPhoneView.h"
#include "sleep/SleepView.h"
#include "settings/common/SettingCommon.h"
#include "date/DateView.h"
#include "monkey/MonkeyModel.h"
#include "cmsis_os.h"

namespace OHOS {
static constexpr uint8_t TIME_PERIOL = 20;
static std::vector<SlicePageId> g_page;
static uint8_t g_curTime = 0;
MonkeyModel::MonkeyModel()
{
    g_page.clear();
    // 手电筒
    g_page.push_back({VIEW_FLASHLIGHT, FLASH_LIGHT_VIEW});
    g_page.push_back({VIEW_FLASHLIGHT, FLASH_LIGHT_PLAY_VIEW});
    g_page.push_back({VIEW_FLASHLIGHT, FLASH_LIGHT_SET_VIEW});
    g_page.push_back({VIEW_FLASHLIGHT, FLASH_LIGHT_COLOR_VIEW});
    g_page.push_back({VIEW_FLASHLIGHT, FLASH_LIGHT_BLINK_VIEW});
    g_page.push_back({VIEW_FLASHLIGHT, FLASH_LIGHT_RECOVER_VIEW});
    // 通话菜单
    g_page.push_back({VIEW_PHONE_MENU, PHONE_MENU_MAIN_PAGE});
    g_page.push_back({VIEW_PHONE_MENU, PHONE_MENU_CONTACTS_PAGE});
    g_page.push_back({VIEW_PHONE_MENU, PHONE_MENU_DIAL_PAGE});
    g_page.push_back({VIEW_PHONE_MENU, PHONE_MENU_DISCONNECT_PAGE});
    g_page.push_back({VIEW_PHONE_MENU, PHONE_MENU_CALLER_LOG_PAGE});
    g_page.push_back({VIEW_PHONE_MENU, PHONE_MENU_CALLING_PAGE});
    // 秒表
    g_page.push_back({VIEW_STOPWATCH, STOPWATCH_MAIN_PAGE});
    g_page.push_back({VIEW_STOPWATCH, STOPWATCH_RECORD_PAGE});
    // 运动
    g_page.push_back({VIEW_SPORT, SPORT_PAGE_MENU});
    g_page.push_back({VIEW_SPORT, SPORT_PAGE_PREPARE});
    g_page.push_back({VIEW_SPORT, SPORT_PAGE_COUNTDOWN});
    g_page.push_back({VIEW_SPORT, SPORT_PAGE_DATA});
    g_page.push_back({VIEW_SPORT, SPORT_PAGE_PAUSE});
    g_page.push_back({VIEW_SPORT, SPORT_PAGE_PROMPT});
    g_page.push_back({VIEW_SPORT, SPORT_PAGE_SETTINGS});
    g_page.push_back({VIEW_SPORT, SPORT_PAGE_TARGET});
    g_page.push_back({VIEW_SPORT, SPORT_PAGE_TARGET_SETTING});
    g_page.push_back({VIEW_SPORT, SPORT_PAGE_TARGET_PROMPT});
    g_page.push_back({VIEW_SPORT, SPORT_PAGE_REMIND});
    g_page.push_back({VIEW_SPORT, SPORT_PAGE_REMIND_INTERVAL});
    g_page.push_back({VIEW_SPORT, SPORT_PAGE_REMIND_HEARTRATE});
    g_page.push_back({VIEW_SPORT, SPORT_PAGE_HEARTRATE_RANGE});
    g_page.push_back({VIEW_SPORT, SPORT_PAGE_MISTOUCH});
    // 设置
    g_page.push_back({VIEW_SETTING, MAIN_PAGE});
    g_page.push_back({VIEW_SETTING, BLUETOOTH_PAGE});
    g_page.push_back({VIEW_SETTING, DISPLAY_PAGE});
    g_page.push_back({VIEW_EMPTY, 1});
    g_page.push_back({VIEW_SETTING, DESKTOP_PAGE});
    g_page.push_back({VIEW_SETTING, POWER_PAGE});
    g_page.push_back({VIEW_SETTING, CARD_PAGE});
    g_page.push_back({VIEW_SETTING, SYSTEM_PAGE});
    g_page.push_back({VIEW_SETTING, ABOUT_PAGE});
    g_page.push_back({VIEW_SETTING, DISPLAY_BRIGHTNESS_PAGE});
    g_page.push_back({VIEW_SETTING, DISPLAY_SCREEN_OFF_PAGE});
    g_page.push_back({VIEW_SETTING, DISPLAY_STEADY_ON_PAGE});
    g_page.push_back({VIEW_SETTING, DESKTOP_DIAL_PAGE});
    g_page.push_back({VIEW_EMPTY, 1});
    g_page.push_back({VIEW_SETTING, DESKTOP_DIAL_STYLE_PAGE});
    g_page.push_back({VIEW_SETTING, CARD_EFFECT_PAGE});
    g_page.push_back({VIEW_SETTING, CARD_MGR_PAGE});
    g_page.push_back({VIEW_SETTING, CARD_ADD_PAGE});
    g_page.push_back({VIEW_SETTING, SYSTEM_OPERATION_PAGE});
    g_page.push_back({VIEW_SETTING, SYSTEM_OTA_PAGE});
    g_page.push_back({VIEW_SETTING, CONNECT_NEW_PHONE_PAGE});
    g_page.push_back({VIEW_SETTING, CONNECT_OPEN_PAGE});
    g_page.push_back({VIEW_EMPTY, 1});
    g_page.push_back({VIEW_SETTING, CONNECT_BLUE_SUCCESS_PAGE});
    g_page.push_back({VIEW_SETTING, CONNECT_BLUE_FAIL_PAGE});
    g_page.push_back({VIEW_SETTING, RECONNECTION_PAGE});
    g_page.push_back({VIEW_SETTING, RECONNECT_SECOND_CONFIRM_PAGE});
    g_page.push_back({VIEW_SETTING, UNPAIR_SECOND_CONFIRM_PAGE});
    g_page.push_back({VIEW_SETTING, QRCODE_CONN_PAGE});
    g_page.push_back({VIEW_EMPTY, 1});
    // 计时器
    g_page.push_back({VIEW_TIMER, TIMER_MAIN_PAGE});
    g_page.push_back({VIEW_TIMER, TIMER_TIMING_PAGE});
    g_page.push_back({VIEW_TIMER, TIMER_SETTING_PAGE});
    // 天气
    g_page.push_back({VIEW_WEATHER, WEATHER_MAIN_PAGE});
    g_page.push_back({VIEW_WEATHER, WEATHER_HOURS_PAGE});
    g_page.push_back({VIEW_WEATHER, WEATHER_DAYS_PAGE});
    g_page.push_back({VIEW_WEATHER, WEATHER_SUNRISE_PAGE});
    g_page.push_back({VIEW_WEATHER, WEATHER_MONTHRISE_PAGE});
    g_page.push_back({VIEW_WEATHER, WEATHER_LUNARPHASE_PAGE});
    // 血氧
    g_page.push_back({VIEW_BLOOD, BLOOD_MAIN_PAGE});
    g_page.push_back({VIEW_BLOOD, BLOOD_READY_PAGE});
    g_page.push_back({VIEW_BLOOD, BLOOD_MEASUREMENT_PAGE});
    g_page.push_back({VIEW_BLOOD, BLOOD_RESULT_PAGE});
    g_page.push_back({VIEW_BLOOD, BLOOD_CHART_PAGE});
    g_page.push_back({VIEW_BLOOD, BLOOD_EXPLAIN_PAGE});
    // 心率
    g_page.push_back({VIEW_HEARTRATE, HEARTRATE_MAIN_PAGE});
    g_page.push_back({VIEW_HEARTRATE, HEARTRATE_READY_PAGE});
    g_page.push_back({VIEW_HEARTRATE, HEARTRATE_MEASUREMENT_PAGE});
    // 相机
    g_page.push_back({VIEW_CAMERA, CAMERA_MAIN_PAGE});
    // 寻找手机
    g_page.push_back({VIEW_FIND_PHONE, FIND_PHONE_MAIN_PAGE});
    // 睡眠
    g_page.push_back({VIEW_SLEEPING, SLEEPING_MAIN_PAGE});
    g_page.push_back({VIEW_SLEEPING, SLEEPING_DEATIL_PAGE});
    g_page.push_back({VIEW_SLEEPING, SLEEPING_TARGET_PAGE});
    // 日历
    g_page.push_back({VIEW_DATE, DATE_MAIN_PAGE});
    g_page.push_back({VIEW_DATE, DATE_MESSAGE_PAGE});
    g_page.push_back({VIEW_EMPTY, 1});
    timeCallback_ = new TimeAnimatorCallback(TIME_PERIOL);
    if (timeCallback_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new timeCallback_ fail");
        return;
    }
    timeAnimator_ = new Animator(timeCallback_, nullptr, 0, true);
    if (timeAnimator_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new timeAnimator_ fail");
        return;
    }
}

MonkeyModel::~MonkeyModel()
{
    if (timeAnimator_ != nullptr) {
        delete timeAnimator_;
        timeAnimator_ = nullptr;
    }
    if (timeCallback_ != nullptr) {
        delete timeCallback_;
        timeCallback_ = nullptr;
    }
}

MonkeyModel &MonkeyModel::GetInstance()
{
    static MonkeyModel instance;
    return instance;
}

void MonkeyModel::SetStart(bool start)
{
    if (start) {
        timeAnimator_->Start();
    } else {
        timeAnimator_->Stop();
    }
}

void MonkeyModel::TimeAnimatorCallback::Callback(UIView *view)
{
    int pageSize = g_page.size();
    static int monkeyId = 0;
    static int g_curTime = 0;
    g_curTime++;
    if (g_curTime > periol_) {
        monkeyId = (monkeyId + 1) % pageSize;
        std::cout << "sliceId = " << g_page[monkeyId].sliceId << " pageId = " << g_page[monkeyId].pageId << std::endl;
        NativeAbility::GetInstance().SwitchSlice(g_page[monkeyId].sliceId, g_page[monkeyId].pageId);
        g_curTime = 0;
    }
}
}

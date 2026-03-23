/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingDisplayModel
 * Create: 2025-06-01
 */

#include "kv_store.h"
#ifndef _WIN32
#include "screennotify/ui_screennotify.h"
#endif
#include "power_display_service.h"
#include "UiConfig.h"
#include "wearable_log.h"
#include "main/HealthModel.h"
#include "settings/common/SettingCommon.h"
#include "settings/model/SettingDisplayModel.h"

namespace OHOS {
static constexpr uint32_t SETTING_DISPLAY_VALUE_LEN = 32;
static constexpr uint8_t SETTING_DISPLAY_MODLE_BRI_PERCENT_MAX = 100;
static constexpr uint8_t SETTING_DISPLAY_MODEL_BRI_PERCENT_DEFAULT = 50;
static constexpr uint8_t SETTING_DISPLAY_MODLE_BRI_RANGE_MIN = 55;
static constexpr uint8_t SETTING_DISPLAY_MODLE_BRI_RANGE_STEP = 2;
static const char* SETTING_SCREEN_OFF_ALL_STR[] = {"5秒钟", "10秒钟", "15秒钟", "30秒钟", "1分钟", "5分钟"};
static const char* SETTING_STEADY_ON_ALL_STR[] = {"5分钟", "10分钟", "20分钟", "30分钟", "1小时", "2小时"};
enum SettingStrIndex {INDEX_0, INDEX_1, INDEX_2, INDEX_3, INDEX_4, INDEX_5, INDEX_6};

SettingDisplayModel::SettingDisplayModel()
{
    std::vector<SettingUnionItemData> tempUnionData = {{
        PlainTextItemData{"显示与亮度"},
        IconTextNavigationItemData{SettingPageId::DISPLAY_BRIGHTNESS_PAGE, IMAGE_DISPLAY_LIST_ADJUST_BRIGHTNESS,
            "调节亮度", nullptr, true, SETTING_LIST_DEFAULT_BG_COLOR, SETTING_LIST_DEFAULT_BG_COLOR_OPA},
        IconTextToggleItemData{IMAGE_DISPLAY_LIST_AUTO_ADJUST, "自动调节", false,
            SETTING_LIST_DEFAULT_BG_COLOR, SETTING_LIST_DEFAULT_BG_COLOR_OPA},
        IconTextNavigationItemData{SettingPageId::DISPLAY_SCREEN_OFF_PAGE, IMAGE_DISPLAY_LIST_SCREEN_OFF_TIME,
            "熄屏时间", nullptr, true, SETTING_LIST_DEFAULT_BG_COLOR, SETTING_LIST_DEFAULT_BG_COLOR_OPA},
        IconTextNavigationItemData{SettingPageId::DISPLAY_STEADY_ON_PAGE, IMAGE_DISPLAY_LIST_SCREEN_ON_TIME,
            "长亮时刻", nullptr, true, SETTING_LIST_DEFAULT_BG_COLOR, SETTING_LIST_DEFAULT_BG_COLOR_OPA},
    }};
    displayUnionData_.swap(tempUnionData);
}

std::vector<SettingUnionItemData>& SettingDisplayModel::GetDisplayUnionData()
{
    RefreshDisplayUnionDataForAutoAdjustBrightness();
    RefreshDisPlayUnionDataForScreenOffDataContext();
    RefreshDisplayUnionDataForSteadyOnDataContext();
    return displayUnionData_;
}

void SettingDisplayModel::RefreshDisplayUnionDataForAutoAdjustBrightness()
{
    auto toggleItemData = static_cast<IconTextToggleItemData*>(displayUnionData_[INDEX_2].GetUnionItemData());
    toggleItemData->selected = GetAutoAdjustBrightness();
}

void SettingDisplayModel::RefreshDisPlayUnionDataForScreenOffDataContext()
{
    auto screenOffItemData = static_cast<IconTextNavigationItemData*>(displayUnionData_[INDEX_3].GetUnionItemData());
    screenOffItemData->dataContent = GetScreenOffTimeShortStr(GetScreenOffTime());
}

void SettingDisplayModel::RefreshDisplayUnionDataForSteadyOnDataContext()
{
    auto steadyOnItemData = static_cast<IconTextNavigationItemData*>(displayUnionData_[INDEX_4].GetUnionItemData());
    steadyOnItemData->dataContent = GetSteadyOnTimeShortStr(GetSteadyOnTime());
}

void SettingDisplayModel::InitDisplayModel()
{
    const power_display_svr_api_t* svr = power_display_svr_get_api();
    if (svr == nullptr) {
        return;
    }
    // 设置亮度调节模式
    SetAutoAdjustBrightness(GetAutoAdjustBrightness());
    // 设置当前亮度
    SetBrightness(GetBrightness());
    // 设置熄屏时间
    SetScreenOffTime(GetScreenOffTime());
}

void SettingDisplayModel::EnableSteadyOn()
{
    const power_display_svr_api_t *srv = power_display_svr_get_api();
    if (srv == nullptr) {
        return;
    }
    if (OHOS::HealthModel::GetInstance()->GetBriScrStatus() == 0) {
#if !defined(_WIN32)
        // 设置应用永不返回
        set_back_to_home_interval(0xFFFFFFFF);
#endif
        if (srv->get_screen_state() != SCREEN_ON) {
            srv->turn_on_screen();
        }
        srv->set_screen_set_keepon_timeout((uint32_t)GetSteadyOnTime());
    }
}

void SettingDisplayModel::DisableSteadyOn()
{
    const power_display_svr_api_t *srv = power_display_svr_get_api();
    if (srv == nullptr) {
        return;
    }
    if (OHOS::HealthModel::GetInstance()->GetBriScrStatus() == 0) {
#if !defined(_WIN32)
        // 设置应用的返回时间和灭屏时间相同
        set_back_to_home_interval((uint32_t)GetScreenOffTime());
#endif
        srv->set_screen_auto_off_timeout((uint32_t)GetScreenOffTime());
        srv->set_screen_set_keepon_timeout(0);
    }
}

bool SettingDisplayModel::GetAutoAdjustBrightness()
{
    uint8_t brightness = 0;
    char temp[SETTING_DISPLAY_VALUE_LEN] = "";
    (void)UtilsGetValue("auto_adjust_brightness", temp, SETTING_DISPLAY_VALUE_LEN);
    if (strcmp(temp, "auto") == 0) {
        return true;
    } else if (strcmp(temp, "manual") == 0) {
        return false;
    }
    // 配置文件读取失败时, 取当前系统的设置
    const power_display_svr_api_t* svr = power_display_svr_get_api();
    if (svr == nullptr) {
        return false;
    }
    return svr->get_brightness_mode() == AUTO;
}

void SettingDisplayModel::SetAutoAdjustBrightness(bool isAuto)
{
    const power_display_svr_api_t* svr = power_display_svr_get_api();
    if (svr == nullptr) {
        return;
    }
    if (isAuto) {
        svr->set_brightness_mode(AUTO);
        // 自动调节没有传感器支持, 当前设置为默认亮度
        svr->set_brightness(TransBriPercentToValue(SETTING_DISPLAY_MODEL_BRI_PERCENT_DEFAULT));
        (void)UtilsSetValue("auto_adjust_brightness", "auto");
    } else { // 从自动调节转到非自动调节时, 设置存储的亮度值
        svr->set_brightness_mode(MANUAL);
        svr->set_brightness(TransBriPercentToValue(GetBrightness()));
        (void)UtilsSetValue("auto_adjust_brightness", "manual");
    }
}

uint8_t SettingDisplayModel::GetBrightness()
{
    uint32_t brightness = 0;
    char temp[SETTING_DISPLAY_VALUE_LEN] = "";
    (void)UtilsGetValue("brightness", temp, SETTING_DISPLAY_VALUE_LEN);
    if (sscanf_s(temp, "%hhu", &brightness) != 1) {
        // 配置文件读取失败时, 取最大亮度的50%
        return SETTING_DISPLAY_MODEL_BRI_PERCENT_DEFAULT;
    }
    if (brightness > SETTING_DISPLAY_MODLE_BRI_PERCENT_MAX) {
        brightness = SETTING_DISPLAY_MODLE_BRI_PERCENT_MAX;
    }
    return brightness;
}

void SettingDisplayModel::SetBrightness(uint8_t brightness)
{
    if (brightness > SETTING_DISPLAY_MODLE_BRI_PERCENT_MAX) {
        brightness = SETTING_DISPLAY_MODLE_BRI_PERCENT_MAX;
    }
    // 非自动调节时 应用传入亮度值, 否则仅保存亮度值
    if (!GetAutoAdjustBrightness()) {
        const power_display_svr_api_t* svr = power_display_svr_get_api();
        if (svr == nullptr) {
            return;
        }
        svr->set_brightness(TransBriPercentToValue(brightness));
    }
    char temp[SETTING_DISPLAY_VALUE_LEN] = "";
    if (sprintf_s(temp, sizeof(temp), "%u", brightness) > 0) {
        (void)UtilsSetValue("brightness", temp);
    }
}

ScreenOffTime SettingDisplayModel::GetScreenOffTime()
{
    ScreenOffTime screenOffTime = ScreenOffTime::SECONDS_5;
    char temp[SETTING_DISPLAY_VALUE_LEN] = "";
    (void)UtilsGetValue("screen_off_time", temp, SETTING_DISPLAY_VALUE_LEN);
    if (sscanf_s(temp, "%u", &screenOffTime) == 0) {
        return ScreenOffTime::SECONDS_5;
    }
    return screenOffTime;
}

void SettingDisplayModel::SetScreenOffTime(ScreenOffTime screenOffTime)
{
    const power_display_svr_api_t* svr = power_display_svr_get_api();
    if (svr == nullptr) {
        return;
    }
    // 设置灭屏时间
    errcode_t err = svr->set_screen_auto_off_timeout((uint32_t)screenOffTime);
#if !defined(_WIN32)
    // 设置应用返回的时间和灭屏时间同步
    set_back_to_home_interval((uint32_t)screenOffTime);
#endif
    // 保存灭屏时间到配置
    char temp[SETTING_DISPLAY_VALUE_LEN] = "";
    if (sprintf_s(temp, sizeof(temp), "%u", screenOffTime) > 0) {
        (void)UtilsSetValue("screen_off_time", temp);
    }
    return;
}

SteadyOnTime SettingDisplayModel::GetSteadyOnTime()
{
    SteadyOnTime steadyOnTime = SteadyOnTime::MINUTE_10;
    char temp[SETTING_DISPLAY_VALUE_LEN] = "";
    (void)UtilsGetValue("steady_on_time", temp, SETTING_DISPLAY_VALUE_LEN);
    if (sscanf_s(temp, "%u", &steadyOnTime) == 0) {
        return SteadyOnTime::MINUTE_5;
    }
    return steadyOnTime;
}

void SettingDisplayModel::SetSteadyOnTime(SteadyOnTime steadyOnTime)
{
    char temp[SETTING_DISPLAY_VALUE_LEN] = "";
    if (sprintf_s(temp, sizeof(temp), "%u", steadyOnTime) > 0) {
        (void)UtilsSetValue("steady_on_time", temp);
    }
}

const char** SettingDisplayModel::GetScreenOffTimeAllStr()
{
    return SETTING_SCREEN_OFF_ALL_STR;
}

const char** SettingDisplayModel::GetSteadyOnTimeAllStr()
{
    return SETTING_STEADY_ON_ALL_STR;
}

uint8_t SettingDisplayModel::GetScreenOffTimeAllStrSize()
{
    return sizeof(SETTING_SCREEN_OFF_ALL_STR)/sizeof(SETTING_SCREEN_OFF_ALL_STR[0]);
}

uint8_t SettingDisplayModel::GetSteadyOnTimeAllStrSize()
{
    return sizeof(SETTING_STEADY_ON_ALL_STR)/sizeof(SETTING_SCREEN_OFF_ALL_STR[0]);
}

uint8_t SettingDisplayModel::GetScreenOffTimeIndex()
{
    ScreenOffTime screenOffTime = GetScreenOffTime();
    switch (screenOffTime) {
        case ScreenOffTime::SECONDS_5:  return INDEX_0;
        case ScreenOffTime::SECONDS_10: return INDEX_1;
        case ScreenOffTime::SECONDS_15: return INDEX_2;
        case ScreenOffTime::SECONDS_30: return INDEX_3;
        case ScreenOffTime::MINUTE_1:   return INDEX_4;
        case ScreenOffTime::MINUTE_5:   return INDEX_5;
    }
}

uint8_t SettingDisplayModel::GetSteadyOnTimeIndex()
{
    SteadyOnTime steadyOnTime = GetSteadyOnTime();
    switch (steadyOnTime) {
        case SteadyOnTime::MINUTE_5:  return INDEX_0;
        case SteadyOnTime::MINUTE_10: return INDEX_1;
        case SteadyOnTime::MINUTE_20: return INDEX_2;
        case SteadyOnTime::MINUTE_30: return INDEX_3;
        case SteadyOnTime::HOUR_1:    return INDEX_4;
        case SteadyOnTime::HOUR_2:    return INDEX_5;
    }
}

void SettingDisplayModel::SetScreenOffTimeIndex(uint8_t index)
{
    ScreenOffTime screenOffTime;
    switch (index) {
        case INDEX_0: screenOffTime = ScreenOffTime::SECONDS_5;  break;
        case INDEX_1: screenOffTime = ScreenOffTime::SECONDS_10; break;
        case INDEX_2: screenOffTime = ScreenOffTime::SECONDS_15; break;
        case INDEX_3: screenOffTime = ScreenOffTime::SECONDS_30; break;
        case INDEX_4: screenOffTime = ScreenOffTime::MINUTE_1;   break;
        case INDEX_5: screenOffTime = ScreenOffTime::MINUTE_5;   break;
        default: return;
    }
    SetScreenOffTime(screenOffTime);
}

void SettingDisplayModel::SetSteadyOnTimeIndex(uint8_t index)
{
    SteadyOnTime steadyOnTime;
    switch (index) {
        case INDEX_0: steadyOnTime = SteadyOnTime::MINUTE_5; break;
        case INDEX_1: steadyOnTime = SteadyOnTime::MINUTE_10; break;
        case INDEX_2: steadyOnTime = SteadyOnTime::MINUTE_20; break;
        case INDEX_3: steadyOnTime = SteadyOnTime::MINUTE_30; break;
        case INDEX_4: steadyOnTime = SteadyOnTime::HOUR_1;    break;
        case INDEX_5: steadyOnTime = SteadyOnTime::HOUR_2;    break;
        default: return;
    }
    SetSteadyOnTime(steadyOnTime);
}

const char* SettingDisplayModel::GetScreenOffTimeShortStr(ScreenOffTime screenOffTime)
{
    switch (screenOffTime) {
        case ScreenOffTime::SECONDS_5:   return "5s";
        case ScreenOffTime::SECONDS_10:  return "10s";
        case ScreenOffTime::SECONDS_15:  return "15s";
        case ScreenOffTime::SECONDS_30:  return "30s";
        case ScreenOffTime::MINUTE_1:    return "1m";
        case ScreenOffTime::MINUTE_5:    return "5m";
    }
}

const char* SettingDisplayModel::GetSteadyOnTimeShortStr(SteadyOnTime steadyOnTime)
{
    switch (steadyOnTime) {
        case SteadyOnTime::MINUTE_5:  return "5m";
        case SteadyOnTime::MINUTE_10: return "10m";
        case SteadyOnTime::MINUTE_20: return "20m";
        case SteadyOnTime::MINUTE_30: return "30m";
        case SteadyOnTime::HOUR_1:    return "1h";
        case SteadyOnTime::HOUR_2:    return "2h";
    }
}

uint8_t SettingDisplayModel::TransBriValueToPercent(uint8_t value)
{
    return (value - SETTING_DISPLAY_MODLE_BRI_RANGE_MIN) / SETTING_DISPLAY_MODLE_BRI_RANGE_STEP;
}

uint8_t SettingDisplayModel::TransBriPercentToValue(uint8_t percent)
{
    return percent * SETTING_DISPLAY_MODLE_BRI_RANGE_STEP + SETTING_DISPLAY_MODLE_BRI_RANGE_MIN;
}
}
/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingDisplayModel
 * Create: 2025-06-01
 */
#ifndef SETTING_DISPLAY_MODEL_H
#define SETTING_DISPLAY_MODEL_H

#include <cstdint>
#include "settings/common/SettingUnionItemGroup.h"
#include "settings/common/SettingRefSingleton.h"

namespace OHOS {
// 息屏时间 (单位:ms)
enum class ScreenOffTime : uint32_t {
    SECONDS_5 = 5000u,
    SECONDS_10 = 10000u,
    SECONDS_15 = 15000u,
    SECONDS_30 = 30000u,
    MINUTE_1 = 60000u,
    MINUTE_5 = 300000u,
};

// 常亮时间 (单位:ms)
enum class SteadyOnTime : uint32_t {
    MINUTE_5 = 300000u,
    MINUTE_10 = 600000u,
    MINUTE_20 = 1200000u,
    MINUTE_30 = 1800000u,
    HOUR_1 = 3600000u,
    HOUR_2 = 7200000u,
};

class SettingDisplayModel : public SettingRefSingleton<SettingDisplayModel> {
public:
    SettingDisplayModel();
    std::vector<SettingUnionItemData>& GetDisplayUnionData();
    void RefreshDisplayUnionDataForAutoAdjustBrightness();
    void RefreshDisPlayUnionDataForScreenOffDataContext();
    void RefreshDisplayUnionDataForSteadyOnDataContext();
    void InitDisplayModel();
    void EnableSteadyOn();      // 打开常亮
    void DisableSteadyOn();     // 关闭常亮
    bool GetAutoAdjustBrightness();
    uint8_t GetBrightness();
    ScreenOffTime GetScreenOffTime();
    SteadyOnTime GetSteadyOnTime();
    void SetAutoAdjustBrightness(bool isAuto);
    void SetBrightness(uint8_t brightness);
    void SetScreenOffTime(ScreenOffTime blinkFrequency);
    void SetSteadyOnTime(SteadyOnTime blinkFrequency);
    const char** GetScreenOffTimeAllStr();
    const char** GetSteadyOnTimeAllStr();
    uint8_t GetScreenOffTimeAllStrSize();
    uint8_t GetSteadyOnTimeAllStrSize();
    uint8_t GetScreenOffTimeIndex();
    uint8_t GetSteadyOnTimeIndex();
    void SetScreenOffTimeIndex(uint8_t index);
    void SetSteadyOnTimeIndex(uint8_t index);
protected:
    const char* GetScreenOffTimeShortStr(ScreenOffTime screenOffTime);
    const char* GetSteadyOnTimeShortStr(SteadyOnTime steadyOnTime);
    uint8_t TransBriValueToPercent(uint8_t value);
    uint8_t TransBriPercentToValue(uint8_t percent);
private:
    std::vector<SettingUnionItemData> displayUnionData_;
};
} // OHOS
#endif
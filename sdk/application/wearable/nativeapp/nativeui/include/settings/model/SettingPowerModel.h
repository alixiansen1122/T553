/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingPowerModel
 * Create: 2025-06-01
 */
#ifndef SETTING_POWER_MODEL_H
#define SETTING_POWER_MODEL_H

#include <cstdint>
#if !defined(_WIN32)
#include "adc.h"
#include "tcxo.h"
#include "pinctrl.h"
#include "hal_adc.h"
#endif
#include "settings/common/SettingRefSingleton.h"

namespace OHOS {
#if !defined(_WIN32)
class SettingPowerModel : public SettingRefSingleton<SettingPowerModel> {
public:
    SettingPowerModel();
    ~SettingPowerModel();
    uint8_t GetBatteryPowerLevel();
    const char* GetPowerModelNotify();
protected:
    bool adc_context_init();
    bool adc_context_startup();
    void adc_context_shutdown();
    uint8_t calc_percentage(uint32_t voltage);
    uint32_t calc_voltage();
    uint32_t adc_trans_sytick_to_voltages(uint32_t sytick);
private:
    adc_channel_t channel_;        // ADC信道
    pin_t analog_pin_;             // ADC输入引脚
    pin_mode_t mode_;              // ADC管脚工作模式
    uint32_t samples_to_keep_;     // 总采样次数
    uint32_t samples_to_discard_;  // 丢弃的初始样本数
    uint32_t adc_max_range_mv_;    // ADC原始值上限(单位:毫伏)
    uint32_t adc_min_range_mv_;    // ADC原始值下限(单位:毫伏)
};
#else
class SettingPowerModel : public SettingRefSingleton<SettingPowerModel> {
public:
    uint8_t GetBatteryPowerLevel();
    const char* GetPowerModelNotify();
};
#endif
} // OHOS
#endif
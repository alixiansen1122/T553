/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingPowerModel
 * Create: 2025-06-01
 */
#if !defined(_WIN32)
#include "osal_interrupt.h"
#endif
#include "settings/common/SettingCommon.h"
#include "settings/model/SettingPowerModel.h"

namespace OHOS {
static constexpr const uint8_t percentage_scale = 100; // 百分比换算基数
static constexpr const uint8_t percentage_max = 100;
static constexpr const uint8_t percentage_min = 0;

static constexpr const uint32_t adc_reference_ratio_numerator = 450U;
static constexpr const uint32_t adc_reference_ratio_denominator = 1024U / 2U;
static constexpr const uint32_t millivolt_per_volt = 1000U;

static constexpr const uint32_t def_samples_to_keep = 10;
static constexpr const uint32_t def_samples_to_discard = 5;
static constexpr const uint32_t def_adc_max_range_mv = 380;
static constexpr const uint32_t def_adc_min_range_mv = 0;

#if !defined(_WIN32)
SettingPowerModel::SettingPowerModel()
{
    channel_ = ADC_CHANNEL_3;
    analog_pin_ = S_AGPIO_L27;
    mode_ = PIN_MODE_0;
    samples_to_keep_ = def_samples_to_keep;
    samples_to_discard_ = def_samples_to_discard;
    // 毫伏更容易计算电量百分比
    adc_max_range_mv_ = def_adc_max_range_mv;
    adc_min_range_mv_ = def_adc_min_range_mv;
}

SettingPowerModel::~SettingPowerModel()
{
    adc_context_shutdown();
}

uint8_t SettingPowerModel::GetBatteryPowerLevel()
{
    uint32_t voltage = 0;
    // 从adc获取电压值
    if (adc_context_startup()) {
        voltage = calc_voltage();
    }
    adc_context_shutdown();
    // 根据电压值计算电量百分比
    return calc_percentage(voltage);
}

const char* SettingPowerModel::GetPowerModelNotify()
{
    return "这是相对于新电池而言的电池容量。容量较低可能导致充电后，电池使用时间缩短。内建动态软硬件系统将协助克服随手表化学老化出现的性能影响。";
}

bool SettingPowerModel::adc_context_init()
{
    // 初始化pin模块
    uapi_pin_init();
    // 初始化高精度计数器
    errcode_t err = uapi_tcxo_init();
    if (err != ERRCODE_SUCC) {
        return false;
    }
    // 设置pin脚模式
    err = uapi_pin_set_mode(analog_pin_, mode_);
    if (err != ERRCODE_SUCC) {
        return false;
    }
    return true;
}

bool SettingPowerModel::adc_context_startup()
{
    errcode_t err = uapi_adc_init(ADC_CLOCK_500KHZ);  // 初始化adc
    if (err != ERRCODE_SUCC) {
        return false;
    }
    uapi_adc_power_en(AFE_SCAN_MODE_MAX_NUM, true);
    err = uapi_adc_open_channel(channel_);
    if (err != ERRCODE_SUCC) {
        return false;
    }
    return true;
}

void SettingPowerModel::adc_context_shutdown()
{
    errcode_t err = uapi_adc_close_channel(channel_);
    if (err != ERRCODE_SUCC) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "uapi_adc_close_channel error %u", err);
    }
    uapi_adc_power_en(AFE_SCAN_MODE_MAX_NUM, false);
    uapi_adc_deinit();
}

uint8_t SettingPowerModel::calc_percentage(uint32_t voltage)
{
    if (voltage >= adc_max_range_mv_) {
        return percentage_max;
    }
    if (voltage <= adc_min_range_mv_) {
        return percentage_min;
    }

    uint32_t voltage_range = adc_max_range_mv_ - adc_min_range_mv_;
    uint32_t rounding_term = voltage_range / 2;
    uint32_t voltage_offset = voltage - adc_min_range_mv_;
    return static_cast<uint8_t>((voltage_offset * percentage_scale + rounding_term) / voltage_range);
}

uint32_t SettingPowerModel::calc_voltage()
{
    // 丢弃初始帧数据
    for (uint32_t i = 0; i < samples_to_discard_; i++) {
        uapi_adc_manual_sample(ADC_CHANNEL_NONE);
        uapi_tcxo_delay_ms((uint64_t)3);  // 延迟3ms
    }

    uint32_t stick = 0;
    uint32_t stick_count = 0;
    // 采集样本数
    for (uint32_t i = 0; i < samples_to_keep_; i++) {
        uint32_t irq_sts = osal_irq_lock();
        uint32_t sample_result =
            (uint32_t)uapi_adc_manual_sample(ADC_CHANNEL_NONE);  // 每次读取CHANNEL_NONE? testsuit中是这样写的
        if (sample_result != 0) {
            stick += sample_result;
            stick_count++;
        }
        osal_irq_restore(irq_sts);
    }
    if (stick_count == 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "calc voltage error, no result");
        return 0;
    }
    return adc_trans_sytick_to_voltages(stick / stick_count);
}

uint32_t SettingPowerModel::adc_trans_sytick_to_voltages(uint32_t sytick)
{
    // 整数运算公式：(sytick * numerator + denominator/2) / denominator
    return (sytick * adc_reference_ratio_numerator + adc_reference_ratio_denominator) / adc_reference_ratio_denominator;
}
#else
uint8_t SettingPowerModel::GetBatteryPowerLevel()
{
    return percentage_max;
}

const char* SettingPowerModel::GetPowerModelNotify()
{
    return "这是相对于新电池而言的电池容量。容量较低可能导致充电后，电池使用时间缩短。内建动态软硬件系统将协助克服随手表化学老化出现的性能影响。";
}
#endif
}
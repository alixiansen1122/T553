/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * Description: Breath Ease
 * Author: wangle
 * Create: 2025-4-23
 */
#include <cstdio>
#include "device_proxy.h"
#include "wearable_log.h"
#include "common_def.h"
#include "cpp/audio_manager.h"
#include "uiservice/ui_service.h"
#include "ui_resource_string.h"
#include <sys/statfs.h>
#include <algorithm>
#include "key_id.h"
#include "common.h"
#include "nv.h"
#include "upg.h"
#include "upg_porting.h"

namespace OHOS {
static constexpr const char *MOUNT_POINT[] = {"/system", "/user", "/update", "/music"};
static constexpr const uint16_t language_protocol[] = {(uint8_t)(0x0u << 8) | LANGUAGE_ID_ZH,
                                                       (uint8_t)(0x1u << 8) | LANGUAGE_ID_ZH,
                                                       (uint8_t)(0x2u << 8) | LANGUAGE_ID_EN};
static constexpr const uint8_t language_mask_bits = 8;
static constexpr const uint16_t language_key_mask = 0xFF00;
static constexpr const uint16_t language_val_mask = 0x00FF;

static constexpr const uint8_t percentage_scale = 100;             // 百分比换算基数
static constexpr const uint8_t percentage_max = 100;
static constexpr const uint8_t percentage_min = 0;

static constexpr const uint32_t adc_reference_ratio_numerator = 450U;          // 18 * 1000
static constexpr const uint32_t adc_reference_ratio_denominator = 1024U / 2U;  // 40960 / (1000/18)/2U
static constexpr const uint32_t millivolt_per_volt = 1000U;

static constexpr const uint32_t def_samples_to_keep = 10;
static constexpr const uint32_t def_samples_to_discard = 5;
static constexpr const uint32_t def_adc_max_range_mv = 380;
static constexpr const uint32_t def_adc_min_range_mv = 0;

template <typename T>
T clamp(const T &value, const T &min_val, const T &max_val)
{
    return (value < min_val) ? (min_val) : (value > max_val ? max_val : value);
}

void device_proxy::get_storage_info(storage_info &info)
{
    memset_s(&info, sizeof(storage_info), 0, sizeof(storage_info));
    for (size_t i = 0; i < sizeof(MOUNT_POINT) / sizeof(MOUNT_POINT[0]); i++) {
        struct statfs tmpStat = {0};
        if (statfs(MOUNT_POINT[i], &tmpStat) != 0) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "无法获取%s的挂载点信息,%s", MOUNT_POINT[i], strerror(errno));
            continue;
        }

        uint32_t total = static_cast<uint32_t>(tmpStat.f_blocks * tmpStat.f_bsize);
        uint32_t used = static_cast<uint32_t>((tmpStat.f_blocks - tmpStat.f_bfree) * tmpStat.f_bsize);
        uint32_t free = static_cast<uint32_t>(tmpStat.f_bfree * tmpStat.f_bsize);

        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "挂载点%s信息, 总:%d(byte), 用:%d(byte), 剩:%d(byte)",
                      MOUNT_POINT[i], total, used, free);
        info.total_size += total;
        info.used_size += used;
        info.free_size += free;
    }
}
void device_proxy::get_language_id(uint8_t &id)
{
    sys_language_id_t lang_id = {0};
    uint16_t nvsize = 0;

    errcode_t ret = uapi_nv_read(NV_ID_SYS_LANGUAGE_ID, sizeof(sys_language_id_t), &nvsize, (uint8_t *)&lang_id);
    if (ret != ERRCODE_SUCC) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "nv get language id error, ret=%u", ret);
        return;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "language nvsize=%u id = %u", nvsize, lang_id.language_id);

    for (auto key : language_protocol) {
        if (lang_id.language_id == (uint8_t)(key & language_val_mask)) {
            id = (uint8_t)((key & language_key_mask) >> language_mask_bits);
            return;
        }
    }
    id = LANGUAGE_ID_ZH;
}
void device_proxy::set_language_id(uint8_t id)
{
    sys_language_id_t lang_id = {.language_id = LANGUAGE_ID_ZH};

    for (auto key : language_protocol) {
        if (id == (uint8_t)((key & language_key_mask) >> language_mask_bits)) {
            lang_id.language_id = (uint8_t)(key & 0xFF);
        }
    }

    errcode_t ret = uapi_nv_write(NV_ID_SYS_LANGUAGE_ID, (uint8_t *)&lang_id, sizeof(sys_language_id_t));
    if (ret != ERRCODE_SUCC) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "nv set language id error, ret=%u", ret);
        return;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "set language id = %u", lang_id.language_id);
    // 发送更新消息到uiservice
    SendMsgToUIService(UI_UPDATE_LANGUAGE, 0, nullptr, 0);
}
void device_proxy::get_product_info(std::string &info_string)
{
    sys_product_info_t info = {0};
    uint16_t nvsize = 0;
    info_string.clear();

    errcode_t ret = uapi_nv_read(NV_ID_SYS_PRODUCT_INFO, sizeof(sys_product_info_t), &nvsize, (uint8_t *)&info);
    if (ret != ERRCODE_SUCC) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "nv get product info error, ret=%u", ret);
        return;
    }

    if (nvsize < sizeof(sys_product_info_t) || info.size == 0) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "invalid product info size: nvsize=%u, info.size=%u", nvsize, info.size);
        return;
    }

    info_string.assign((const char *)info.name, info.size);

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "product info: nvsize=%u, strlen=%zu, data=%.*s", nvsize, info_string.size(),
                  static_cast<int>(info_string.size()), info_string.c_str());
}
void device_proxy::get_volume_info(uint8_t vol_type, std::vector<volume_info> &vol_info)
{
    std::vector<AudioStreamType> audio_types;

    if (vol_type == vol_type_all_idx || vol_type > vol_type_max_idx) {
        for (uint8_t i = 0; i < vol_type_max_idx; i++) {
            uint8_t max_perc = 0;
            trans_type_to_audio_stream_type(i, audio_types);
            for (auto types : audio_types) {  // 每个类型也由多个音频类型组成, 取声音最大的
                int32_t vol = Audio::AudioManager::GetInstance().GetVolume(types);
                int32_t max_vol = Audio::AudioManager::GetInstance().GetMaxVolume(types);
                int32_t min_vol = Audio::AudioManager::GetInstance().GetMinVolume(types);
                uint8_t perc = calc_vol_percentage(min_vol, max_vol, vol);
                max_perc = max_perc > perc ? max_perc : perc;
            }
            volume_info info;
            info.type = i;
            info.perc = max_perc;

            vol_info.push_back(info);
        }
    } else {
        uint8_t max_perc = 0;
        trans_type_to_audio_stream_type(vol_type, audio_types);
        for (auto types : audio_types) {  // 每个类型也由多个音频类型组成, 取声音最大的
            int32_t vol = Audio::AudioManager::GetInstance().GetVolume(types);
            int32_t max_vol = Audio::AudioManager::GetInstance().GetMaxVolume(types);
            int32_t min_vol = Audio::AudioManager::GetInstance().GetMinVolume(types);
            uint8_t perc = calc_vol_percentage(min_vol, max_vol, vol);
            max_perc = max_perc > perc ? max_perc : perc;
        }
        volume_info info;
        info.type = vol_type;
        info.perc = max_perc;

        vol_info.push_back(info);
    }
}
void device_proxy::set_volume_info(volume_info &vol_info)
{
    std::vector<AudioStreamType> audio_types;
    trans_type_to_audio_stream_type(vol_info.type, audio_types);
    for (auto types : audio_types) {
        int32_t max_vol = Audio::AudioManager::GetInstance().GetMaxVolume(types);
        int32_t min_vol = Audio::AudioManager::GetInstance().GetMinVolume(types);

        int32_t vol = calc_vol_volume(min_vol, max_vol, vol_info.perc);
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SetVolume %08X, %d", types, vol);
        Audio::AudioManager::GetInstance().SetVolume(types, vol);
    }
}
OTA_ERR device_proxy::ota_init(ota_info &info)
{
    // 检查OTA的当前状态
    upg_status status = uapi_upg_get_status();
    if (status == UPG_STATUS_UPDATING) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "OTAInit:The update module is being upgraded.");
        return OTA_ERR::OTA_ERR_STATUS_UPDATING;
    }
    // 清理并检查升级区空间大小
    delete_file_with_cleanup(upg_get_pkg_file_dir(), upg_get_pkg_file_path());
    uint32_t total_size = uapi_upg_get_storage_size();
    if (total_size == 0) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "OTAInit:OTA check storage size error, total_size is 0");
        return OTA_ERR::OTA_ERR_INVAILD_STORAGE;
    }
    if (info.package_size >= total_size) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "OTAInit:check size error, total size:%u, need size:%u", total_size,
                      info.package_size);
        return OTA_ERR::OTA_ERR_INSUFFICIENT_STORAGE_SPACE;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "OTAInit:check succ");
    return OTA_ERR::OTA_SUCC;
}
OTA_ERR device_proxy::ota_update(ota_info &info)
{
    uint32_t total_size = 0;
    do {
        FILE *fp = fopen(upg_get_pkg_file_path(), "rb");
        if (fp == nullptr) {
            break;
        }
        fseek(fp, 0L, SEEK_END);
        total_size = ftell(fp);
        fclose(fp);
    } while (false);
    if (total_size == 0) {
        return OTA_ERR::OTA_ERR_INVAILD_STORAGE;
    }
    if (total_size != info.package_size) {
        return OTA_ERR::OTA_ERR_PACKAGE_SIZE_ERROR;
    }    
    return OTA_ERR::OTA_SUCC;
}

void device_proxy::call_ui_ota_update()
{
    // 发送升级请求到uiservice
    SendMsgToUIService(UI_OTA_UPDATE, 0, nullptr, 0);
}

int device_proxy::delete_file_with_cleanup(const char *ppath, const char *path)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "path:%s, ppath:%s", path, ppath);
    // 1. 删除文件
    if (unlink(path) == -1) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "unlink failed:%d", errno);
        return 1;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "unlink succ");
    // 2. 打开父目录文件描述符
    int dir_fd = open(ppath, O_RDONLY);
    if (dir_fd == -1) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "open parent dir failed:%d", errno);
        return 1;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "open parent dir succ");
    // 3. 同步父目录元数据到磁盘
    if (fsync(dir_fd) == -1) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "fsync dir failed:%d", errno);
        close(dir_fd);
        return 1;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "fsync dir succ");
    close(dir_fd);
    return 0;
}

uint8_t device_proxy::calc_vol_percentage(int32_t min, int32_t max, int32_t curr)
{
    if (min >= max) {
        return 0;
    }
    const int32_t clamped_curr = clamp(curr, min, max);
    const int64_t range = static_cast<int64_t>(max) - min;

    const int64_t scaled_value = (static_cast<int64_t>(clamped_curr - min) * percentage_scale);
    return static_cast<uint8_t>(scaled_value / range);
}
int32_t device_proxy::calc_vol_volume(int32_t min, int32_t max, uint8_t percentage)
{
    if (min >= max) {
        return min;
    }
    uint8_t clamped_percentage = clamp(percentage, percentage_min, percentage_max);

    double range = static_cast<double>(max - min);
    double percentage_ratio = static_cast<double>(clamped_percentage) / 100.0;
    int32_t curr = static_cast<int32_t>(min + range * percentage_ratio);

    return static_cast<int32_t>(clamp(curr, min, max));
}
void device_proxy::trans_type_to_audio_stream_type(uint8_t type, std::vector<AudioStreamType> &audio_types)
{
    audio_types.clear();
    switch (type) {
        case vol_type_alarm:
            audio_types.push_back(AUDIO_STREAM_ALARM);         // 通用报警音
            audio_types.push_back(AUDIO_STREAM_ALARM_SYSTEM);  // 设备报警
            audio_types.push_back(AUDIO_STREAM_ALARM_CLOCK);   // 闹钟报警
            break;
        case vol_type_ring:
            audio_types.push_back(AUDIO_STREAM_RING);  // 来电铃声
            break;
        case vol_type_call:
            audio_types.push_back(AUDIO_STREAM_VOICE_CALL);         // 普通语音通话
            audio_types.push_back(AUDIO_STREAM_VOICE_CALL_VOIP);    // 网络语音通话（如微信通话）
            audio_types.push_back(AUDIO_STREAM_VOICE_CALL_BT_SCO);  // 蓝牙语音通话
            audio_types.push_back(AUDIO_STREAM_VOICE_CALL_VOLTE);   // 4G VOLTE 高清语音通话
            break;
        case vol_type_assistant:
            audio_types.push_back(AUDIO_STREAM_VOICE_ASSISTANT);  // 语音助手（如 Siri、Google Assistant）
            audio_types.push_back(AUDIO_STREAM_TTS);              // 语音合成（如文本转语音）
            break;
        case vol_type_notification:
            audio_types.push_back(AUDIO_STREAM_NOTIFICATION);  // 通用提示音
            audio_types.push_back(AUDIO_STREAM_NOTIFICATION_SYSTEM);  // 设备提示音（如蓝牙连接/断开、低电量提示）
            audio_types.push_back(AUDIO_STREAM_NOTIFICATION_PROMPT);  // 运动健康提示音（如运动完成提醒）
            break;
        case vol_type_music:
            audio_types.push_back(AUDIO_STREAM_MUSIC);  // 本地音乐播放。
            audio_types.push_back(AUDIO_STREAM_A2DP_MUSIC);  // 蓝牙传输的媒体音（如从手机传输到蓝牙耳机的音乐）。
            break;
        case vol_type_video:
            audio_types.push_back(AUDIO_STREAM_FITNESS_VIDEO);  // 健身视频的指导音频。
            break;
    }
}

uint8_t battery_power::get_battery_percentage()
{
    battery_power *inst = get_instance();
    if (inst == nullptr) {
        return 0;
    }
    uint32_t voltage = 0;
    // 从adc获取电压值
    if (inst->adc_context_startup()) {
        voltage = inst->calc_voltage();
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "calc_voltage:%u", voltage);
    }
    inst->adc_context_shutdown();
    // 根据电压值计算电量百分比
    return inst->calc_percentage(voltage);
}

battery_power *battery_power::get_instance()
{
    static battery_power *instance = nullptr;
    if (instance == nullptr) {
        battery_power *temp = new battery_power();
        if (!temp->adc_context_init()) {
            delete temp;
            return nullptr;
        }
        instance = temp;
    }
    return instance;
}

battery_power::battery_power()
{
    // 数据由海思提供
    channel = ADC_CHANNEL_3;
    analog_pin = S_AGPIO_L27;
    mode = PIN_MODE_0;
    samples_to_keep = def_samples_to_keep;
    samples_to_discard = def_samples_to_discard;
    // 由测试检测而来, 测试中得到的数据应该是0.3744V(没有标注单位,猜测得到,未必准确)
    // 因为要算百分比, 换算成毫伏更容易计算
    adc_max_range_mv = def_adc_max_range_mv;
    adc_min_range_mv = def_adc_min_range_mv;
}

battery_power::~battery_power()
{
    adc_context_shutdown();
}

bool battery_power::adc_context_init()
{
    // 初始化pin模块
    uapi_pin_init();
    // 初始化高精度计数器
    errcode_t err = uapi_tcxo_init();
    if (err != ERRCODE_SUCC) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "uapi_tcxo_init error %u", err);
        return false;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "uapi_tcxo_init succ");
    // 设置pin脚模式
    err = uapi_pin_set_mode(analog_pin, mode);
    if (err != ERRCODE_SUCC) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "uapi_pin_set_mode error %u", err);
        return false;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "uapi_pin_set_mode succ");
    return true;
}

bool battery_power::adc_context_startup()
{
    errcode_t err = uapi_adc_init(ADC_CLOCK_500KHZ);  // 初始化adc
    if (err != ERRCODE_SUCC) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "uapi_adc_init error %u", err);
        return false;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "uapi_adc_init succ");
    uapi_adc_power_en(AFE_SCAN_MODE_MAX_NUM, true);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "uapi_adc_power_en true");
    err = uapi_adc_open_channel(channel);
    if (err != ERRCODE_SUCC) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "uapi_adc_open_channel error %u", err);
        return false;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "uapi_adc_open_channel succ");
    return true;
}
void battery_power::adc_context_shutdown()
{
    errcode_t err = uapi_adc_close_channel(channel);
    if (err != ERRCODE_SUCC) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "uapi_adc_close_channel error %u", err);
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "uapi_adc_close_channel succ");
    uapi_adc_power_en(AFE_SCAN_MODE_MAX_NUM, false);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "uapi_adc_power_en false");
    uapi_adc_deinit();
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "uapi_adc_deinit");
}

uint8_t battery_power::calc_percentage(uint32_t voltage)
{
    if (voltage >= adc_max_range_mv) {
        return percentage_max;
    }
    if (voltage <= adc_min_range_mv) {
        return percentage_min;
    }

    uint32_t voltage_range = adc_max_range_mv - adc_min_range_mv;
    uint32_t rounding_term = voltage_range / 2;
    uint32_t voltage_offset = voltage - adc_min_range_mv;
    return static_cast<uint8_t>((voltage_offset * percentage_scale + rounding_term) / voltage_range);
}

uint32_t battery_power::calc_voltage()
{
    // 丢弃初始帧数据
    for (uint32_t i = 0; i < samples_to_discard; i++) {
        uapi_adc_manual_sample(ADC_CHANNEL_NONE);
        uapi_tcxo_delay_ms((uint64_t)3);  // 延迟3ms
    }

    uint32_t stick = 0;
    uint32_t stick_count = 0;
    // 采集样本数
    for (uint32_t i = 0; i < samples_to_keep; i++) {
        uint32_t irq_sts = osal_irq_lock();
        uint32_t sample_result =
            (uint32_t)uapi_adc_manual_sample(ADC_CHANNEL_NONE);  // 每次读取CHANNEL_NONE? testsuit中是这样写的
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "sample result = %u", sample_result);
        if (sample_result != 0) {
            stick += sample_result;
            stick_count++;
        }
        osal_irq_restore(irq_sts);
    }

    if (stick_count == 0) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "error, no result");
        return 0;
    }

    uint32_t voltage = adc_trans_sytick_to_voltages(stick / stick_count);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "adc trans voltages:%u", voltage);
    return voltage;
}

uint32_t battery_power::adc_trans_sytick_to_voltages(uint32_t sytick)
{
    // 整数运算公式：(sytick * numerator + denominator/2) / denominator
    return (sytick * adc_reference_ratio_numerator + adc_reference_ratio_denominator) / adc_reference_ratio_denominator;
}
}  // namespace OHOS
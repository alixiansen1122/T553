/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SportModel
 * Create: 2025-06-06
 */

#ifndef SPORT_MODEL_H
#define SPORT_MODEL_H

#include <vector>
#if defined(_WIN32)
#include "animator/animator.h"
#define SPORT_CONFIG_PATH
#else
#define SPORT_CONFIG_PATH "/user/data/"
#include "ohos_timer.h"
#include "graphic_timer.h"
#endif

namespace OHOS {
enum SportType : uint8_t {
    SPORT_TYPE_OUTDOOR_RUNNING = 0,
    SPORT_TYPE_INDOOR_RUNNING,
    SPORT_TYPE_CYCLING,
    SPORT_TYPE_CLIMBING,
    SPORT_TYPE_INDOOR_SWIMMING,
    SPORT_TYPE_OUTDOOR_SWIMMING,
    SPORT_TYPE_MAX,
};

enum SportSetType : uint8_t {
    SPORT_SET_TYPE_TARGET = 0,  // 设置项(目标)和提示类型共用
    SPORT_SET_TYPE_REMIND,      // 设置项(提醒)和提示类型共用
    SPORT_SET_TYPE_MISTOUCH,    // 设置项(防误触)
    SPORT_SET_TYPE_MAX = 3,     // 设置项最大值
    SPORT_SET_TYPE_HEARTRATE,   // 只用于心率提示类型
};

enum SportTarget : uint8_t {
    SPORT_TARGET_DISTANCE = 0,
    SPORT_TARGET_TIME,
    SPORT_TARGET_CALORIE,
    SPORT_TARGET_MAX,
};

enum SportRemind : uint8_t {
    SPORT_REMIND_INTERVAL = 0,
    SPORT_REMIND_HEARTRATE,
    SPORT_REMIND_MAX,
};

enum SportRemindInterval : uint8_t {
    SPORT_REMIND_INTERVAL_NONE = 0,
    SPORT_REMIND_INTERVAL_DISTANCE,
    SPORT_REMIND_INTERVAL_TIME,
    SPORT_REMIND_INTERVAL_MAX,
};

enum SportRemindHeartRate : uint8_t {
    SPORT_REMIND_HEARTRATE_ENABLE = 0,
    SPORT_REMIND_HEARTRATE_RANGE,
    SPORT_REMIND_HEARTRATE_MAX,
};

enum SportHeartRateRange : uint8_t {
    SPORT_HEARTRATE_RANGE_ITEM1 = 0,
    SPORT_HEARTRATE_RANGE_ITEM2,
    SPORT_HEARTRATE_RANGE_ITEM3,
    SPORT_HEARTRATE_RANGE_ITEM4,
    SPORT_HEARTRATE_RANGE_MAX,
};

enum GpsSignal : uint8_t {
    SPORT_GPS_SIGNAL0 = 0,
    SPORT_GPS_SIGNAL1,
    SPORT_GPS_SIGNAL2,
    SPORT_GPS_SIGNAL3,
};

enum SportAction : uint8_t {
    SPORT_ACTION_START = 1,
    SPORT_ACTION_PAUSE = 2,
    SPORT_ACTION_RESUME = 3,
    SPORT_ACTION_STOP = 4,
};

enum SportConfig : uint8_t {
    SPORT_CONFIG_INFO = 1,
    SPORT_CONFIG_RECORD = 2,
};

typedef struct sport_control {
    uint8_t action;
    uint8_t type;
} sport_control_t;

typedef struct sport_data {
    uint8_t type;          // 运动类型
    uint16_t heartRate;    // 心率
    uint16_t calorie;      // 卡路里
    uint16_t altitude;     // 高度
    uint16_t ascent;       // 海拔上升
    uint16_t descent;      // 海拔下降
    uint32_t distance;     // 运动距离
    uint32_t lap;          // 室内游泳趟数
    uint32_t startTime;    // 运动开始时间
    uint32_t endTime;      // 运动结束时间
    uint32_t elapsedTime;  // 运动总耗时
} sport_data_t;

typedef struct sport_info {
    SportSetType setType;                // 当前设置项类型(目标或提醒)
    SportSetType promptType;             // 当前提醒项类型
    bool isTargetSetted;                 // 是否已设置过目标
    SportTarget targetType;              // 目标类型
    SportTarget targetTypeTemp;          // 临时目标类型,用于恢复没有成功设置目标类型时
    uint16_t targetValue;                // 设置的目标值，在Picker中选择的值
    bool targetHalf;                     // 是否达到一半目标
    bool targetComplete;                 // 是否达成半目标
    bool isRemindSetted;                 // 是否已设置过提醒
    SportRemindInterval remindType;      // 提醒类型
    SportRemindInterval remindTypeTemp;  // 临时提醒类型,用于恢复没有成功设置提醒类型时
    uint16_t remindIndex;                // 提醒的索引值，在Picker中的索引值
    uint16_t remindValue;                // 设置的提醒值，在Picker中选择的值
    uint16_t remindTimes;                // 已经提醒的次数
    bool enableHeartRemind;              // 是否开启心率提醒
    bool isHeartReminded;                // 同区间的心率值是否已经提醒过
    SportHeartRateRange heartRange;      // 设置的心率提醒区间
    bool enableScreenLock;               // 是否开启屏幕锁定
    bool enableCrownLock;                // 是否开启表冠锁定
    uint32_t startTimeTemp;              // 用于暂停时再继续运动时计算运动耗时
    sport_data_t data;                   // 各项运动数据
} sport_info_t;

std::string GetCurrentTimeStr();
std::string ConvertDistance(uint32_t distance);
std::string FormatSportTime(uint32_t uiSportTime);
std::string CalculateSpeed(uint32_t distance, uint32_t time);
const char *GetHeartRateRangeDescription(uint8_t range);
uint16_t GetHeartRateRangeBounder(uint8_t index);

class SportModel {
public:
    enum class SportState : uint8_t { STOPPED, RUNNING, PAUSED };
    static SportModel &GetInstance(void);
    void SetSportType(SportType sportType);
    SportType GetSportType(void);
    void SetTargetValue(uint16_t index);
    void SetRemindValue(uint16_t index);
    void SetGpsSignal(GpsSignal gpsSignal);
    GpsSignal GetGpsSignal(void);
    uint16_t GetHeartRate(void);
    sport_info_t *GetSportInfo(SportType type);
    sport_info_t *GetCurrSportInfo();
    uint32_t GetElapsedTime();
    void StartSport(bool startedByWatch);
    void PauseAndReport(bool isReport);
    void ResumeAndReport(bool isReport);
    void StopAndReport(bool isReport);
    void AddRecord();
    void ClearRecord();
    std::vector<sport_data_t> &GetRecord();
    bool IsStartByWatch();
    void LoadConfig();
    void SaveConfig(uint8_t config);
    void ClearData();

private:
    SportModel();
    SportModel(const SportModel &);
    SportModel &operator=(const SportModel &);
    virtual ~SportModel();
    SportType curSport_;
    sport_info_t info_[SPORT_TYPE_MAX];
    GpsSignal gpsSignal_;
    bool isStartByWatch_{false};
    SportState state_;
    std::vector<sport_data_t> records_;
#if defined(_WIN32)
    class SportAnimatorCallback : public AnimatorCallback {
    public:
        SportAnimatorCallback() {}
        virtual ~SportAnimatorCallback() override {}
        void Callback(UIView *view) override;

    private:
    };
    SportAnimatorCallback *callback_{nullptr};
    Animator *animator_{nullptr};
#else
    GraphicTimer *generateDataHandle_{nullptr};
    void InitGenerateSportData();
    void StartGenerateSportData();
    void StopGenerateSportData();
#endif
};
}

#endif  // SPORT_MODEL_H
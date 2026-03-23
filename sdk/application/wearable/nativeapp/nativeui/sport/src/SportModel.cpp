/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SportModel
 * Create: 2025-06-06
 */

#include <fstream>
#include <string>
#include <sys/time.h>
#include "wearable_log.h"
#include "msg_center_device.h"
#ifdef BUILD_IN_LINUX
#include "localtime.h"
#endif
#include "sport/SportModel.h"

namespace OHOS {
static constexpr uint16_t NUM_TEN = 10;
static constexpr uint16_t NUM_HUNDRED = 100;
static constexpr uint16_t NUM_THOUSAND = 1000;
static constexpr uint16_t UNIT_SECS_PER_MIN = 60;
static constexpr uint16_t UNIT_SECS_PER_HOUR = 3600;
static const uint16_t COUNT_TIMES = 100;
static const uint16_t DISTANCE_RANGE = 200;
static const uint16_t HEARTRATE_MIN_VALUE = 116;
static const uint16_t HEARTRATE_MAX_VALUE = 195;
static const uint16_t CALORIE_RANGE = 8;
static const uint16_t ALTITUDE_RANGE = 4;
static const uint16_t ASCENT_RANGE = 7;
static const uint16_t DESCENT_RANGE = 6;
static const uint16_t g_targetDistanceValues[] = {1, 3, 5, 7, 10, 15, 20, 25, 30, 40};  // 距离Picker值集，单位为km
static const uint16_t g_targetTimeValues[] = {10, 20, 30, 60, 120, 180, 200};  // 时间Picker值集，单位为分钟
static const uint16_t g_targetCalorieValues[] = {100, 200, 300, 500, 600, 800, 1000};  // 热量Picker值集，单位为千卡
static const uint16_t g_targetLapValues[] = {1, 2, 4, 6, 8, 10, 12, 16, 20, 30};  // 室内游泳趟数Picker值集，单位为趟
static const char *g_remindHeartRateRange[] = {"116-136次/分", "136-156次/分", "156-176次/分", "176-195次/分"};
static const uint16_t g_heartRateRangeBounder[] = {116, 136, 156, 176, 195};
static const char *SPORT_INFO_FILE = SPORT_CONFIG_PATH "sport_info.dat";
static const char *SPORT_RECORD_FILE = SPORT_CONFIG_PATH "sport_record.dat";

std::string GetCurrentTimeStr()
{
    struct tm localTime;
    time_t seconds = time(nullptr);
    localtime_r(&seconds, &localTime);
    std::string currTime;
    if (localTime.tm_hour < NUM_TEN) {
        currTime += "0";
    }
    currTime += std::to_string(localTime.tm_hour) + ":";
    if (localTime.tm_min < NUM_TEN) {
        currTime += "0";
    }
    currTime += std::to_string(localTime.tm_min);
    return currTime;
}

std::string ConvertDistance(uint32_t distance)
{
    std::string distanceStr;
    uint32_t distanceIntPart = distance / NUM_THOUSAND;
    uint32_t distanceFractionalPart = (distance % NUM_THOUSAND) / NUM_TEN;
    if (distanceIntPart > NUM_HUNDRED) {
        distanceStr = std::to_string(distanceIntPart);
    } else if (distanceIntPart > NUM_TEN) {
        distanceStr = std::to_string(distanceIntPart) + "." + std::to_string(distanceFractionalPart / NUM_TEN);
    } else {
        if (distanceFractionalPart < NUM_TEN) {
            distanceStr = std::to_string(distanceIntPart) + ".0" + std::to_string(distanceFractionalPart);
        } else {
            distanceStr = std::to_string(distanceIntPart) + "." + std::to_string(distanceFractionalPart);
        }
    }
    return distanceStr;
}

std::string FormatSportTime(uint32_t uiSportTime)
{
    std::string strSportTime;
    std::string hours;
    std::string minute;
    std::string seconds;
    uint32_t hour = uiSportTime / UNIT_SECS_PER_HOUR;
    uiSportTime %= UNIT_SECS_PER_HOUR;
    uint32_t min = uiSportTime / UNIT_SECS_PER_MIN;
    uiSportTime %= UNIT_SECS_PER_MIN;
    uint32_t sec = uiSportTime;
    if (hour >= 0 && hour < NUM_TEN) {
        hours = "0" + std::to_string(hour);
    } else {
        hours = std::to_string(hour);
    }
    if (min >= 0 && min < NUM_TEN) {
        minute = "0" + std::to_string(min);
    } else {
        minute = std::to_string(min);
    }
    if (sec >= 0 && sec < NUM_TEN) {
        seconds = "0" + std::to_string(sec);
    } else {
        seconds = std::to_string(sec);
    }
    strSportTime = hours + ":" + minute + ":" + seconds;
    return strSportTime;
}

std::string CalculateSpeed(uint32_t distance, uint32_t time)
{
    if (distance == 0) {
        return "00'00\"";
    }
    std::string speedStr;
    uint64_t speed = time * NUM_THOUSAND / distance;
    uint64_t min = speed / UNIT_SECS_PER_MIN;
    uint64_t sec = speed % UNIT_SECS_PER_MIN;
    speedStr = std::to_string(min) + "'";
    if (sec >= 0 && sec < NUM_TEN) {
        speedStr += "0";
    }
    speedStr += std::to_string(sec) + "\"";
    return speedStr;
}

const char *GetHeartRateRangeDescription(uint8_t range)
{
    return g_remindHeartRateRange[range];
}

uint16_t GetHeartRateRangeBounder(uint8_t index)
{
    return g_heartRateRangeBounder[index];
}

#if defined(_WIN32)
void SportModel::SportAnimatorCallback::Callback(UIView *view)
{
    static uint64_t count = 0;
    static bool increase = true;
    if (count++ % COUNT_TIMES == 0) {
        uint32_t randNum = rand();
        uint32_t distance = randNum % DISTANCE_RANGE;
        uint16_t calorie = randNum % CALORIE_RANGE;
        uint16_t altitude = randNum % ALTITUDE_RANGE;
        uint16_t ascent = randNum % ASCENT_RANGE;
        uint16_t descent = randNum % DESCENT_RANGE;

        sport_info_t *sportInfo = SportModel::GetInstance().GetCurrSportInfo();
        sportInfo->data.distance += distance;
        if (sportInfo->data.heartRate < HEARTRATE_MIN_VALUE) {
            sportInfo->data.heartRate = HEARTRATE_MIN_VALUE;
            increase = true;
        }
        if (sportInfo->data.heartRate > HEARTRATE_MAX_VALUE) {
            sportInfo->data.heartRate = HEARTRATE_MAX_VALUE;
            increase = false;
        }
        if (increase) {
            sportInfo->data.heartRate++;
        } else {
            sportInfo->data.heartRate--;
        }
        sportInfo->data.calorie += calorie;
        sportInfo->data.altitude += altitude;
        sportInfo->data.ascent += ascent;
        sportInfo->data.descent += descent;
        sportInfo->data.lap++;
    }
}
#else
static void GenerateDataCallback(void *data)
{
    static bool increase = true;
    uint32_t randNum = rand();
    uint32_t distance = randNum % DISTANCE_RANGE;
    uint16_t calorie = randNum % CALORIE_RANGE;
    uint16_t altitude = randNum % ALTITUDE_RANGE;
    uint16_t ascent = randNum % ASCENT_RANGE;
    uint16_t descent = randNum % DESCENT_RANGE;

    sport_info_t *sportInfo = SportModel::GetInstance().GetCurrSportInfo();
    sportInfo->data.distance += distance;
    if (sportInfo->data.heartRate < HEARTRATE_MIN_VALUE) {
        sportInfo->data.heartRate = HEARTRATE_MIN_VALUE;
        increase = true;
    }
    if (sportInfo->data.heartRate > HEARTRATE_MAX_VALUE) {
        sportInfo->data.heartRate = HEARTRATE_MAX_VALUE;
        increase = false;
    }
    if (increase) {
        sportInfo->data.heartRate++;
    } else {
        sportInfo->data.heartRate--;
    }
    sportInfo->data.calorie += calorie;
    sportInfo->data.altitude += altitude;
    sportInfo->data.ascent += ascent;
    sportInfo->data.descent += descent;
    sportInfo->data.lap++;

    if (!SportModel::GetInstance().IsStartByWatch()) {
        msg_center_send_data(MSGCENTER_CMD_DEVICE, MSGCENTER_TYPE_ID_SPORT_REPORT_DATA, &sportInfo->data,
                             sizeof(sport_data_t));
    }
}

void SportModel::InitGenerateSportData()
{
    uint32_t tick = GetOSTick(2000);
    generateDataHandle_ = new GraphicTimer(tick, GenerateDataCallback, nullptr, true);
    if (generateDataHandle_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportModel::InitGenerateSportData new GraphicTimer failed!!");
        return;
    }
}

void SportModel::StartGenerateSportData()
{
    if (generateDataHandle_ != nullptr) {
        bool retTimer = generateDataHandle_->Start();
        if (!retTimer) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "StartGenerateSportData failed!!");
            if (generateDataHandle_ != nullptr) {
                delete generateDataHandle_;
                generateDataHandle_ = nullptr;
            }
            return;
        }
    }
}

void SportModel::StopGenerateSportData()
{
    if (generateDataHandle_ != nullptr) {
        generateDataHandle_->Stop();
    }
}
#endif

SportModel::SportModel()
{
    LoadConfig();
#if defined(_WIN32)
    callback_ = new SportAnimatorCallback();
    if (callback_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportModel::SportModel callback_ new fail");
        return;
    }

    animator_ = new Animator(callback_, nullptr, 0, true);
    if (animator_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportModel::SportModel new Animator animator_ fail");
        return;
    }
#else
    InitGenerateSportData();
#endif
}

SportModel::~SportModel() {}

SportModel &SportModel::GetInstance()
{
    static SportModel instance;
    return instance;
}

void SportModel::SetSportType(SportType sportType)
{
    curSport_ = sportType;
}

SportType SportModel::GetSportType(void)
{
    return curSport_;
}

void SportModel::SetTargetValue(uint16_t index)
{
    SportTarget target = info_[curSport_].targetType;
    if (target == SPORT_TARGET_DISTANCE) {
        if (curSport_ == SPORT_TYPE_INDOOR_SWIMMING) {
            info_[curSport_].targetValue = g_targetLapValues[index];
        } else {
            info_[curSport_].targetValue = g_targetDistanceValues[index];
        }
    } else if (target == SPORT_TARGET_TIME) {
        info_[curSport_].targetValue = g_targetTimeValues[index];
    } else if (target == SPORT_TARGET_CALORIE) {
        info_[curSport_].targetValue = g_targetCalorieValues[index];
    }
}

void SportModel::SetRemindValue(uint16_t index)
{
    SportRemindInterval remind = info_[curSport_].remindType;
    if (remind == SPORT_REMIND_INTERVAL_DISTANCE) {
        if (curSport_ == SPORT_TYPE_INDOOR_SWIMMING) {
            info_[curSport_].remindValue = g_targetLapValues[index];
        } else {
            info_[curSport_].remindValue = g_targetDistanceValues[index];
        }
    } else if (remind == SPORT_REMIND_INTERVAL_TIME) {
        info_[curSport_].remindValue = g_targetTimeValues[index];
    }
    info_[curSport_].remindIndex = index;
}

uint32_t SportModel::GetElapsedTime()
{
    if (state_ == SportState::RUNNING) {
        uint32_t now = time(nullptr);
        return info_[curSport_].data.elapsedTime + (now - info_[curSport_].startTimeTemp);
    }
    return info_[curSport_].data.elapsedTime;
}

void SportModel::SetGpsSignal(GpsSignal gpsSignal)
{
    gpsSignal_ = gpsSignal;
}

GpsSignal SportModel::GetGpsSignal(void)
{
    return gpsSignal_;
}

uint16_t SportModel::GetHeartRate(void)
{
    return info_[curSport_].data.heartRate;
}

sport_info_t *SportModel::GetSportInfo(SportType type)
{
    return &info_[type];
}

sport_info_t *SportModel::GetCurrSportInfo()
{
    return GetSportInfo(curSport_);
}

void SportModel::StartSport(bool startedByWatch)
{
    if (state_ == SportState::STOPPED) {
        state_ = SportState::RUNNING;
        isStartByWatch_ = startedByWatch;
        info_[curSport_].targetHalf = false;
        info_[curSport_].targetComplete = false;
        info_[curSport_].isHeartReminded = false;
        info_[curSport_].remindTimes = 0;
        (void)memset_s(&(info_[curSport_].data), sizeof(info_[curSport_].data), 0, sizeof(sport_data_t));
        info_[curSport_].data.startTime = time(nullptr);
        info_[curSport_].startTimeTemp = info_[curSport_].data.startTime;
#if defined(_WIN32)
        animator_->Start();
#else
        StartGenerateSportData();
#endif
    }
}

void SportModel::PauseAndReport(bool isReport)
{
    if (state_ == SportState::RUNNING) {
        state_ = SportState::PAUSED;
        info_[curSport_].data.elapsedTime += time(nullptr) - info_[curSport_].startTimeTemp;
#if defined(_WIN32)
        animator_->Stop();
#else
        StopGenerateSportData();
        if (isReport) {
            sport_control_t control = {0};
            control.action = static_cast<uint8_t>(SPORT_ACTION_PAUSE);
            control.type = static_cast<uint8_t>(curSport_);
            msg_center_send_data(MSGCENTER_CMD_DEVICE, MSGCENTER_TYPE_ID_SPORT_REPORT_ACTION, &control,
                                 sizeof(sport_control_t));
        }
#endif
    }
}

void SportModel::ResumeAndReport(bool isReport)
{
    if (state_ == SportState::PAUSED) {
        state_ = SportState::RUNNING;
        info_[curSport_].startTimeTemp = time(nullptr);
#if defined(_WIN32)
        animator_->Start();
#else
        StartGenerateSportData();
        if (isReport) {
            sport_control_t control = {0};
            control.action = static_cast<uint8_t>(SPORT_ACTION_RESUME);
            control.type = static_cast<uint8_t>(curSport_);
            msg_center_send_data(MSGCENTER_CMD_DEVICE, MSGCENTER_TYPE_ID_SPORT_REPORT_ACTION, &control,
                                 sizeof(sport_control_t));
        }
#endif
    }
}

void SportModel::StopAndReport(bool isReport)
{
    if (state_ != SportState::STOPPED) {
        state_ = SportState::STOPPED;
        info_[curSport_].data.type = static_cast<uint8_t>(curSport_);
        info_[curSport_].data.endTime = time(nullptr);
#if defined(_WIN32)
        animator_->Stop();
#else
        StopGenerateSportData();
        if (isReport) {
            sport_control_t control = {0};
            control.action = static_cast<uint8_t>(SPORT_ACTION_STOP);
            control.type = static_cast<uint8_t>(curSport_);
            msg_center_send_data(MSGCENTER_CMD_DEVICE, MSGCENTER_TYPE_ID_SPORT_REPORT_ACTION, &control,
                                 sizeof(sport_control_t));
        }
#endif
    }
}

bool SportModel::IsStartByWatch()
{
    return isStartByWatch_;
}

void SportModel::AddRecord()
{
    if (isStartByWatch_) {
        records_.push_back(info_[curSport_].data);
    }
    SaveConfig(SPORT_CONFIG_RECORD);
}

std::vector<sport_data_t> &SportModel::GetRecord()
{
    return records_;
}

void SportModel::ClearRecord()
{
    records_.clear();
    std::remove(SPORT_RECORD_FILE);
}

void SportModel::LoadConfig()
{
    std::ifstream infoFile(SPORT_INFO_FILE, std::ios::binary);
    if (infoFile) {
        for (uint8_t i = 0; i < SPORT_TYPE_MAX; i++) {
            infoFile.read(reinterpret_cast<char *>(&info_[i]), sizeof(sport_info_t));
        }
        infoFile.close();
    }
    std::ifstream recordFile(SPORT_RECORD_FILE, std::ios::binary);
    if (recordFile) {
        uint16_t num = 0;
        recordFile.read(reinterpret_cast<char *>(&num), sizeof(uint16_t));
        for (uint8_t i = 0; i < num; i++) {
            sport_data_t data;
            recordFile.read(reinterpret_cast<char *>(&data), sizeof(sport_data_t));
            records_.push_back(data);
        }
        recordFile.close();
    }
}

void SportModel::SaveConfig(uint8_t config)
{
    if (config == SPORT_CONFIG_INFO) {
        std::ofstream infoFile(SPORT_INFO_FILE, std::ios::binary);
        if (infoFile) {
            infoFile.write(reinterpret_cast<char *>(&info_), SPORT_TYPE_MAX * sizeof(sport_info_t));
            infoFile.close();
        }
    } else if (config == SPORT_CONFIG_RECORD) {
        std::ofstream recordFile(SPORT_RECORD_FILE, std::ios::binary);
        if (recordFile) {
            uint16_t num = records_.size();
            recordFile.write(reinterpret_cast<char *>(&num), sizeof(uint16_t));
            for (uint8_t i = 0; i < num; i++) {
                recordFile.write(reinterpret_cast<char *>(&records_[i]), sizeof(sport_data_t));
            }
            recordFile.close();
        }
    }
}

void SportModel::ClearData()
{
    std::remove(SPORT_INFO_FILE);
    std::remove(SPORT_RECORD_FILE);
}
}

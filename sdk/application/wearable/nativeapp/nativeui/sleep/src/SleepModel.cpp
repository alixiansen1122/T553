/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SleepModel
 * Created: 2025-06-05
 */

#include <sys/time.h>
#include <ctime>
#include <string>
#include <cstring>
#include "main/HealthModel.h"
#include "uiservice/ui_service.h"
#include "wearable_log.h"
#include "NativeAbility.h"
#include "ui_test_group.h"
#include "graphic_timer.h"
#include "ohos_timer.h"
#include "sleep/SleepModel.h"

namespace OHOS {
static constexpr uint64_t SLEEPMODEL_MINUTE_HOUR = 60;
static constexpr uint64_t SLEEPMODEL_HOUR = 23;
static constexpr uint64_t SLEEPMODEL_MINUTE = 59;
static constexpr uint64_t SLEEPMODEL_ERROR = 100;

SleepModel::SleepModel() {}

SleepModel::~SleepModel() {}

void SleepModel::LoadData()
{
    if (GetRecord() == false) {
        SetSleepDataVec();
        SetSoberingTime();
        SetRapidEyeMovementTime();
        SetLightSleepTime();
        SetDeepSleepTime();
    }
}

bool SleepModel::GetRecord()
{
    if (sleepDataVector_.empty() == true) {
        return false;
    }
    return true;
}

std::string SleepModel::GetTodayDate()
{
    time_t now = time(nullptr);
    struct tm *timeinfo = localtime(&now);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", timeinfo);
    return std::string(buffer);
}

uint8_t *SleepModel::SerializeSleepData(const SleepData *sleepData, size_t *outSize)
{
    if (sleepData == nullptr) {
        return nullptr;
    }
    size_t dataSize = sizeof(SleepData);
    *outSize = dataSize;
    uint8_t *buffer = new uint8_t[dataSize];
    uint8_t isSendValue = sleepData->isSended ? 1 : 0;
    memcpy_s(buffer, sizeof(isSendValue), &isSendValue, sizeof(isSendValue));
    memcpy_s(buffer + offsetof(SleepData, date), sizeof(sleepData->date), sleepData->date, sizeof(sleepData->date));
    memcpy_s(buffer + offsetof(SleepData, type), sizeof(sleepData->type), &sleepData->type, sizeof(sleepData->type));
    memcpy_s(buffer + offsetof(SleepData, typeSleepStart), sizeof(sleepData->typeSleepStart), sleepData->typeSleepStart,
             sizeof(sleepData->typeSleepStart));
    memcpy_s(buffer + offsetof(SleepData, typeSleepEnd), sizeof(sleepData->typeSleepEnd), sleepData->typeSleepEnd,
             sizeof(sleepData->typeSleepEnd));
    memcpy_s(buffer + offsetof(SleepData, sleepStart), sizeof(sleepData->sleepStart), sleepData->sleepStart,
             sizeof(sleepData->sleepStart));
    memcpy_s(buffer + offsetof(SleepData, sleepEnd), sizeof(sleepData->sleepEnd), sleepData->sleepEnd,
             sizeof(sleepData->sleepEnd));
    return buffer;
}

void SleepModel::SendData()
{
    // 发送数据
    size_t dataSize;
    for (int i = 0; i < sleepDataVector_.size(); i++) {
        if (sleepDataVector_[i].isSended == true) {
            continue;
        }
        uint8_t *serializedData = SerializeSleepData(&sleepDataVector_[i], &dataSize);
        errcode_t ret;
#ifdef _WIN32
        ret = 0;
#else
        ret = msg_center_send_data(MSGCENTER_CMD_DEVICE, MSGCENTER_TYPE_ID_SLEEP_DATA, serializedData, dataSize);
        delete[] serializedData;
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "send sleep data ret = %d", ret);
        if (ret == 0) {
            sleepDataVector_[i].isSended = true;
        }
#endif
    }
}

// sleepDataVector打桩数据，后续需要客户适配传感器自实现
void SleepModel::SetSleepDataVec()
{
#ifndef _WIN32
    std::string today = GetTodayDate();

    sleepDataVector_.push_back({false, today.c_str(), SLEEP_TYPE_DEEP, (today + " 00:00:00").c_str(),
                               (today + " 00:30:00").c_str(), (today + " 00:00:00").c_str(),
                               (today + " 07:50:00").c_str()});

    sleepDataVector_.push_back({false, today.c_str(), SLEEP_TYPE_EYEMOVE, (today + " 00:30:00").c_str(),
                               (today + " 03:00:00").c_str(), (today + " 00:00:00").c_str(),
                               (today + " 07:50:00").c_str()});

    sleepDataVector_.push_back({false, today.c_str(), SLEEP_TYPE_LIGHT, (today + " 03:00:00").c_str(),
                               (today + " 03:30:00").c_str(), (today + " 00:00:00").c_str(),
                               (today + " 07:50:00").c_str()});

    sleepDataVector_.push_back({false, today.c_str(), SLEEP_TYPE_SOBER, (today + " 03:30:00").c_str(),
                               (today + " 05:30:00").c_str(), (today + " 00:00:00").c_str(),
                               (today + " 07:50:00").c_str()});

    sleepDataVector_.push_back({false, today.c_str(), SLEEP_TYPE_LIGHT, (today + " 05:30:00").c_str(),
                               (today + " 06:00:00").c_str(), (today + " 00:00:00").c_str(),
                               (today + " 07:50:00").c_str()});

    sleepDataVector_.push_back({false, today.c_str(), SLEEP_TYPE_EYEMOVE, (today + " 06:00:00").c_str(),
                               (today + " 07:30:00").c_str(), (today + " 00:00:00").c_str(),
                               (today + " 07:50:00").c_str()});

    sleepDataVector_.push_back({false, today.c_str(), SLEEP_TYPE_DEEP, (today + " 07:30:00").c_str(),
                               (today + " 07:50:00").c_str(), (today + " 00:00:00").c_str(),
                               (today + " 07:50:00").c_str()});
#endif
}

SleepModel &SleepModel::GetInstance()
{
    static SleepModel instance;
    return instance;
}

std::tuple<uint32_t, uint32_t> SleepModel::GetSoberingTime()
{
    return MinutesToHours(soberingTime_);
}

std::tuple<uint32_t, uint32_t> SleepModel::GetRapidEyeMovementTime()
{
    return MinutesToHours(eyeMoveTime_);
}

std::tuple<uint32_t, uint32_t> SleepModel::GetLightSleepTime()
{
    return MinutesToHours(lightTime_);
}

std::tuple<uint32_t, uint32_t> SleepModel::GetDeepSleepTime()
{
    return MinutesToHours(deepTime_);
}

std::tuple<uint32_t, uint32_t> SleepModel::GetSleepTime()
{
    return MinutesToHours(GetTotalSleepTime());
}

std::tuple<uint32_t, uint32_t> SleepModel::MinutesToHours(uint32_t minutes)
{
    if (minutes == 0) {
        return std::make_tuple(0, 0);
    }
    int hours = minutes / SLEEPMODEL_MINUTE_HOUR;
    int remainingMinutes = minutes % SLEEPMODEL_MINUTE_HOUR;

    if (hours > SLEEPMODEL_HOUR || remainingMinutes > SLEEPMODEL_MINUTE) {
        return std::make_tuple(SLEEPMODEL_ERROR, 0);
    }

    return std::make_tuple(hours, remainingMinutes);
}

uint32_t SleepModel::GetSleepTimeHours()
{
    std::tuple<uint32_t, uint32_t> timeTuple = GetSleepTime();
    return std::get<0>(timeTuple);
}

uint32_t SleepModel::GetSleepTimeMinutes()
{
    std::tuple<uint32_t, uint32_t> timeTuple = GetSleepTime();
    return std::get<1>(timeTuple);
}

void SleepModel::SetTargetTime(uint32_t minute)
{
    sleepTarget_ = minute;
}

std::tuple<uint32_t, uint32_t> SleepModel::GetTargetTime()
{
    return MinutesToHours(sleepTarget_);
}

uint32_t SleepModel::TimeDiffInMinutes(const std::string &timeStr1, const std::string &timeStr2)
{
    std::tm tm1 = {};
    std::tm tm2 = {};

    std::istringstream iss1(timeStr1);
    std::istringstream iss2(timeStr2);

    iss1 >> std::get_time(&tm1, "%Y-%m-%d %H:%M:%S");
    if (iss1.fail()) {
        return -1;
    }

    iss2 >> std::get_time(&tm2, "%Y-%m-%d %H:%M:%S");
    if (iss2.fail()) {
        return -1;
    }

    std::time_t t1 = std::mktime(&tm1);
    std::time_t t2 = std::mktime(&tm2);
    if (t1 == -1 || t2 == -1) {
        return -1;
    }

    double diff_seconds = std::difftime(t2, t1);
    return static_cast<uint32_t>(diff_seconds / SLEEPMODEL_MINUTE_HOUR);
}

// 遍历vector中的结构体  获取对应类型的时间差
uint32_t SleepModel::GetDetailData(int type)
{
    uint32_t min = 0;
    for (size_t i = 0; i < sleepDataVector_.size(); ++i) {
        if (sleepDataVector_[i].type == type) {
            min += TimeDiffInMinutes(sleepDataVector_[i].typeSleepStart, sleepDataVector_[i].typeSleepEnd);
        }
    }
    return min;
}

// 获取总睡觉时间的时间差
uint32_t SleepModel::GetTotalSleepTime()
{
    uint32_t time = TimeDiffInMinutes(sleepDataVector_[0].sleepStart, sleepDataVector_[0].sleepEnd);
    return time;
}

void SleepModel::SetSoberingTime()
{
    soberingTime_ = GetDetailData(SLEEP_TYPE_SOBER);
}
void SleepModel::SetRapidEyeMovementTime()
{
    eyeMoveTime_ = GetDetailData(SLEEP_TYPE_EYEMOVE);
}
void SleepModel::SetLightSleepTime()
{
    lightTime_ = GetDetailData(SLEEP_TYPE_LIGHT);
}
void SleepModel::SetDeepSleepTime()
{
    deepTime_ = GetDetailData(SLEEP_TYPE_DEEP);
}
}

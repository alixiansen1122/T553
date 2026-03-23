/*
 * Copyright (c) CompanyNameMagicTag 2023. All rights reserved.
 * Description: DialModelTime
 * Author:
 * Create: 2023-12
 */

#include <cmath>
#include "sys/time.h"
#include "main/dial/RegisterDialModel.h"
#include "main/dial/DialModelTime.h"
#include "components/root_view.h"

#define REGIST_DIAL_MODULE_ANGLE_H(type, func) static OHOS::RegisterDialModel(REGIST_ANGLE_H)(type, func)
#define REGIST_DIAL_MODULE_ANGLE_M(type, func) static OHOS::RegisterDialModel(REGIST_ANGLE_M)(type, func)
#define REGIST_DIAL_MODULE_ANGLE_S(type, func) static OHOS::RegisterDialModel(REGIST_ANGLE_S)(type, func)
#define REGIST_DIAL_MODULE_ANGLE_MS(type, func) static OHOS::RegisterDialModel(REGIST_ANGLE_MS)(type, func)

#define REGIST_DIAL_MODULE_SECOND(type, func) static OHOS::RegisterDialModel(REGIST_SECOND)(type, func)
#define REGIST_DIAL_MODULE_MINUTE(type, func) static OHOS::RegisterDialModel(REGIST_MINUTE)(type, func)
#define REGIST_DIAL_MODULE_HOUR(type, func) static OHOS::RegisterDialModel(REGIST_HOUR)(type, func)
#define REGIST_DIAL_MODULE_DATE(type, func) static OHOS::RegisterDialModel(REGIST_DATE)(type, func)
#define REGIST_DIAL_MODULE_WEEK(type, func) static OHOS::RegisterDialModel(REGIST_WEEK)(type, func)
#define REGIST_DIAL_MODULE_MOUNTH(type, func) static OHOS::RegisterDialModel(REGIST_MOUNTH)(type, func)
#define REGIST_DIAL_MODULE_YEAR(type, func) static OHOS::RegisterDialModel(REGIST_YEAR)(type, func)
#define REGIST_DIAL_MODULE_GEAR_POSITIVE(type, func) static OHOS::RegisterDialModel(REGIST_GEAR_POSITIVE)(type, func)
#define REGIST_DIAL_MODULE_GEAR_NEGATIVE(type, func) static OHOS::RegisterDialModel(REGIST_GEAR_NEGATIVE)(type, func)

REGIST_DIAL_MODULE_ANGLE_H(OHOS::DialDataType::CLOCK_ANGLE, OHOS::DialModelTime::GetInstance());
REGIST_DIAL_MODULE_ANGLE_M(OHOS::DialDataType::MINUTE_ANGLE, OHOS::DialModelTime::GetInstance());
REGIST_DIAL_MODULE_ANGLE_S(OHOS::DialDataType::SECOND_ANGLE, OHOS::DialModelTime::GetInstance());
REGIST_DIAL_MODULE_ANGLE_MS(OHOS::DialDataType::MILLISECOND_ANGLE, OHOS::DialModelTime::GetInstance());

REGIST_DIAL_MODULE_SECOND(OHOS::DialDataType::SECOND_TIME, OHOS::DialModelTime::GetInstance());
REGIST_DIAL_MODULE_MINUTE(OHOS::DialDataType::MINUTE_TIME, OHOS::DialModelTime::GetInstance());
REGIST_DIAL_MODULE_HOUR(OHOS::DialDataType::CLOCK, OHOS::DialModelTime::GetInstance());
REGIST_DIAL_MODULE_DATE(OHOS::DialDataType::DATE, OHOS::DialModelTime::GetInstance());
REGIST_DIAL_MODULE_WEEK(OHOS::DialDataType::WEEKDAY_DATA, OHOS::DialModelTime::GetInstance());
REGIST_DIAL_MODULE_MOUNTH(OHOS::DialDataType::MONTH, OHOS::DialModelTime::GetInstance());
REGIST_DIAL_MODULE_YEAR(OHOS::DialDataType::SOLAR_YEAR, OHOS::DialModelTime::GetInstance());
REGIST_DIAL_MODULE_GEAR_POSITIVE(OHOS::DialDataType::SWIVEL_GEAR_POSITIVE, OHOS::DialModelTime::GetInstance());
REGIST_DIAL_MODULE_GEAR_NEGATIVE(OHOS::DialDataType::SWIVEL_GEAR_NEGATIVE, OHOS::DialModelTime::GetInstance());

static OHOS::RegisterDialModel REGIST_MOUNTH_80(OHOS::DialDataType::MONTH_DATA, OHOS::DialModelTime::GetInstance());

namespace OHOS {
static constexpr int16_t S_MS_US_TRANS = 1000;      // 1000: seconds, milliseconds, microsecond transform value
static constexpr uint8_t ONE_MINUTE_IN_SECOND = 60; // 60: 60 seconds per minute
static constexpr uint8_t ONE_HOUR_IN_MINUTE = 60;   // 60: 60 minutes per hour
static constexpr uint8_t HALF_DAY_IN_HOUR = 12;     // 60: 12 hours every half day

static uint16_t ConvertHandValueToAngle(int32_t handValue, uint8_t range, int32_t secondHandValue, uint16_t ratio)
{
    if ((range == 0) || (ratio == 0)) {
        return 0;
    }
    /*
     * Example: calculate the angle of hour hand
     * Assume that the time is 5: 30, then range is 12, radio is 60
     * angle is [(5 * 60  + 30) / (12 * 60)] * 360
     */
    int32_t degree = (handValue * ratio + secondHandValue);
    degree = CIRCLE_IN_DEGREE * degree / (static_cast<uint16_t>(range) * ratio);

    return static_cast<uint16_t>(degree % CIRCLE_IN_DEGREE);
}

static float ConvertHandSecondValueToAngle(int32_t handValue, uint16_t range)
{
    /*
     * Example: calculate the angle of second hand without millisecond handle
     * Assume that the time is 5:30:30, then range is 60
     * angle is (30 / 60) * 360
     */
    return (static_cast<float>(handValue * CIRCLE_IN_DEGREE) / range);
}

void DialModelTime::UpdateTime()
{
    struct timeval tv;
    gettimeofday(&tv, nullptr);
#ifdef _WIN32
    time_t time = tv.tv_sec;
    if (tblock_ == nullptr) {
        tblock_ = new struct tm();
        memset_s(tblock_, sizeof(struct tm), 0, sizeof(struct tm));
    }
    localtime_s(tblock_, &time);
#else
    tblock_ = localtime(&tv.tv_sec);
#endif
    int realMS = tblock_->tm_sec * S_MS_US_TRANS + tv.tv_usec / S_MS_US_TRANS;

    angleH_ = ConvertHandValueToAngle(tblock_->tm_hour, HALF_DAY_IN_HOUR, tblock_->tm_min, ONE_HOUR_IN_MINUTE);
    angleM_ = ConvertHandValueToAngle(tblock_->tm_min, ONE_HOUR_IN_MINUTE, tblock_->tm_sec, ONE_MINUTE_IN_SECOND);
    angleS_ = ConvertHandValueToAngle(tblock_->tm_sec, ONE_MINUTE_IN_SECOND, 0, 1);
    angleMS_ = ConvertHandSecondValueToAngle(realMS, S_MS_US_TRANS * ONE_MINUTE_IN_SECOND);

    if (tblock_ == nullptr) {
        return;
    }
    for (ListNode<TimeChangeListener*>* node = listeners_.Head(); node != listeners_.End(); node = node->next_) {
        TimeChangeListener *listener = node->data_;
        if (listener != nullptr) {
            listener->OnTimeUpdate(*tblock_);
        }
    }
    RootView::GetInstance()->Invalidate();
}

void DialModelTime::Register(TimeChangeListener* listener)
{
    if (listener == nullptr) {
        return;
    }
    for (ListNode<TimeChangeListener*>* node = listeners_.Head(); node != listeners_.End(); node = node->next_) {
        TimeChangeListener *tmp = node->data_;
        if (tmp == listener) {
            return;
        }
    }
    listeners_.PushBack(listener);
}

void DialModelTime::Unregister(TimeChangeListener* listener)
{
    if (listener == nullptr) {
        return;
    }
    for (ListNode<TimeChangeListener*>* node = listeners_.Head(); node != listeners_.End(); node = node->next_) {
        TimeChangeListener *tmp = node->data_;
        if (tmp == listener) {
            listeners_.Remove(node);
            return;
        }
    }
}

bool DialModelTime::GetDialFloatData(DialDataType& type, float& out)
{
    if (tblock_ == nullptr) {
        UpdateTime();
    }

    switch (type) {
        case DialDataType::CLOCK_ANGLE:
            out = static_cast<float>(angleH_);
            break;
        case DialDataType::MINUTE_ANGLE:
            out = static_cast<float>(angleM_);
            break;
        case DialDataType::SECOND_ANGLE:
            out = static_cast<float>(angleS_);
            break;
        case DialDataType::MILLISECOND_ANGLE:
            out = static_cast<float>(angleMS_);
            break;
        case DialDataType::SOLAR_YEAR:
            out = static_cast<float>(tblock_->tm_year + 1900); // 1900: since 1900
            break;
        case DialDataType::MONTH:
            out = static_cast<float>(tblock_->tm_mon);
            break;
        case DialDataType::WEEKDAY_DATA:
            out = static_cast<float>(tblock_->tm_wday);
            break;
        case DialDataType::DATE:
            out = static_cast<float>(tblock_->tm_mday);
            break;
        case DialDataType::CLOCK:
            out = static_cast<float>(tblock_->tm_hour);
            break;
        case DialDataType::MINUTE_TIME:
            out = static_cast<float>(tblock_->tm_min);
            break;
        case DialDataType::SECOND_TIME:
            out = static_cast<float>(tblock_->tm_sec);
            break;
        case DialDataType::SWIVEL_GEAR_POSITIVE: {
            float coeff = 6.0f;  // 6.0: coeff
            float accleratedAngle = angleMS_ * coeff - floor(angleMS_ * coeff / CIRCLE_IN_DEGREE) * CIRCLE_IN_DEGREE;
            out = static_cast<float>(accleratedAngle);
            break;
        }
        case DialDataType::SWIVEL_GEAR_NEGATIVE: {
            float coeff = 6.0f;  // 6.0: coeff
            float accleratedAngle = angleMS_ * coeff - floor(angleMS_ * coeff / CIRCLE_IN_DEGREE) * CIRCLE_IN_DEGREE;
            out = static_cast<float>(accleratedAngle * -1.0 + CIRCLE_IN_DEGREE);
            break;
        }
        case DialDataType::MONTH_DATA: {
            out = static_cast<float>(tblock_->tm_mon) + 1.0;
            break;
        }
        default:
            return false;
    }
    return true;
}

void TimeChangeListener::SetTimeInervals(TimeInterval *intervals, uint8_t num)
{
    if (intervals == nullptr || num == 0) {
        return;
    }
    if (intervals_ != nullptr) {
        UIFree(intervals_);
        intervals_ = nullptr;
    }
    num_ = 0;

    intervals_ = static_cast<TimeInterval*>(UIMalloc(sizeof(TimeInterval) * num));
    if (intervals_ == nullptr) {
        GRAPHIC_LOGE("failed to malloc!");
        return;
    }
    num_ = num;
    memcpy_s(intervals_, sizeof(TimeInterval) * num, intervals, sizeof(TimeInterval) * num);
}

bool TimeChangeListener::IsInTimeInterval(const struct tm &curTime)
{
    if (num_ == 0) { // 0-24
        return true;
    }
    for (uint8_t i = 0; i < num_; ++i) {
        if (intervals_[i].start <= curTime.tm_hour && intervals_[i].end > curTime.tm_hour) {
            return true;
        }
    }
    return false;
}

TimeChangeListener::TimeChangeListener()
{
    DialModelTime::GetInstance()->Register(this);
}

TimeChangeListener::~TimeChangeListener()
{
    if (intervals_ != nullptr) {
        UIFree(intervals_);
        intervals_ = nullptr;
    }
    num_ = 0;
    DialModelTime::GetInstance()->Unregister(this);
}
}

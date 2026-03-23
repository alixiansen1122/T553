/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: FlashLightModel
 * Create: 2025-04-24
 */

#ifdef BUILD_IN_LINUX
#include "localtime.h"
#endif
#include "flashlight/FlashLightModel.h"

namespace OHOS {
static constexpr uint32_t TIME_DISPLAY_NUM = 10;
FlashLightModel::FlashLightModel(FlashLightColorAttribute color, FlashLightBlinkFrequency blinkFrequency)
{
    color_ = color;
    blinkFrequency_ = blinkFrequency;
}

FlashLightModel::~FlashLightModel() {}

FlashLightModel &FlashLightModel::GetInstance()
{
    static FlashLightModel instance;
    return instance;
}

FlashLightColorAttribute FlashLightModel::GetViewColorId()
{
    return color_;
}

void FlashLightModel::SetViewColorId(FlashLightColorAttribute color)
{
    color_ = color;
}

FlashLightBlinkFrequency FlashLightModel::GetBlinkFrequency()
{
    return blinkFrequency_;
}

void FlashLightModel::SetBlinkFrequency(FlashLightBlinkFrequency blinkFrequency)
{
    blinkFrequency_ = blinkFrequency;
}

void FlashLightModel::SetDefault()
{
    blinkFrequency_ = FlashLightBlinkFrequency::NOT_BLINK;  // 不闪烁;
    color_ = FlashLightColorAttribute::WHITE_COLOR;         // 白色;
}

uint32_t FlashLightModel::GetSysCurrentTime(std::string &currentTime)
{
#ifdef _WIN32
    struct tm localTime;
    time_t timep;
    gettimeofday(&sysTime_, NULL);
    timep = (time_t)sysTime_.tv_sec;
    localtime_r(&timep, &localTime);
#else
    struct tm localTime;
    time64_t timep;
    gettimeofday64(&sysTime_, NULL);
    timep = (time64_t)sysTime_.tv_sec;
    localtime64_r(&sysTime_.tv_sec, &localTime);
#endif
    if (localTime.tm_hour < TIME_DISPLAY_NUM) {
        currentTime += "0";
    }
    currentTime += std::to_string(localTime.tm_hour) + ":";
    if (localTime.tm_min < TIME_DISPLAY_NUM) {
        currentTime += "0";
    }
    currentTime += std::to_string(localTime.tm_min);
    return 0;
}
}

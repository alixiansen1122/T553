/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: TimerModel
 * Create: 2025-06-09
 */

#include "wearable_log.h"
#include "timers/TimerModel.h"

namespace OHOS {
static constexpr uint16_t MS_ONE_SECONDS = 1000;
TimerModel::TimerModel() {}

TimerModel::~TimerModel() {}

TimerModel &TimerModel::GetInstance()
{
    static TimerModel instance;
    return instance;
}

uint64_t TimerModel::GetEclipseTime()
{
    return eclipseTime_;
}

void TimerModel::SetEclipseTime(bool flag, uint64_t value)
{
    if (flag) {
        eclipseTime_ = 0;
    } else {
        eclipseTime_ += value;
    }
}

void TimerModel::SetTotalTime(uint64_t time)
{
    totalTime_ = time;
}

uint64_t TimerModel::GetTotalTime()
{
    return totalTime_;
}

/* 定时器回调函数 */
void TimerTimerCallback(void *data)
{
    (void)data;
    uint64_t total_time = TimerModel::GetInstance().GetTotalTime();
    uint64_t start_time = TimerModel::GetInstance().GetStartTime();
    uint64_t eclipse_time = TimerModel::GetInstance().GetEclipseTime();
    struct timeval current_time;
    gettimeofday(&current_time, nullptr);
    uint64_t curr_time_ms = (uint64_t)current_time.tv_sec * MS_ONE_SECONDS + current_time.tv_usec / MS_ONE_SECONDS;
    int64_t remTime = (uint64_t)total_time * MS_ONE_SECONDS - (curr_time_ms - start_time) - eclipse_time;
    if (remTime <= 0) {
        TimerModel::GetInstance().SetExistTimer(false);
        TimerModel::GetInstance().DisableTimer();
    }
}

/* 开启定时器 */
void TimerModel::InitTimer()
{
    if (startCounting_ != nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "InitTimer startCounting_ != nullptr!!");
        return;
    }
    uint32_t tick = GetOSTick(MS_ONE_SECONDS);
    startCounting_ = new GraphicTimer(tick, TimerTimerCallback, nullptr, true);
    if (startCounting_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "InitTimer new GraphicTimer failed!!");
        return;
    }
    bool retTimer = startCounting_->Start();
    if (!retTimer) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "InitTimer Start failed!!");
        if (startCounting_ != nullptr) {
            delete startCounting_;
            startCounting_ = nullptr;
        }
        return;
    }
}

/* 关闭定时器 */
void TimerModel::DisableTimer()
{
    if (startCounting_ != nullptr) {
        startCounting_->Stop();
        delete startCounting_;
        startCounting_ = nullptr;
    }
}
}
/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: StopwatchModel
 * Create: 2025-06-06
 */

#include <sys/time.h>
#include "wearable_log.h"
#include "stopwatch/StopwatchModel.h"

namespace OHOS {
static constexpr uint16_t UNIT_SEC_TO_MSEC = 1000;
StopwatchModel::StopwatchModel()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "StopwatchModel::StopwatchModel");
}

StopwatchModel::~StopwatchModel()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "StopwatchModel::~StopwatchModel");
}

StopwatchModel &StopwatchModel::GetInstance()
{
    static StopwatchModel instance;
    return instance;
}

void StopwatchModel::SetStartTime()
{
#ifdef _WIN32
    gettimeofday(&startTime_, NULL);
#else
    gettimeofday64(&startTime_, NULL);
#endif
}

void StopwatchModel::SaveElapsedTime()
{
    savedElapsedTime_ += elapsedTime_;
    elapsedTime_ = 0;
}

uint64_t StopwatchModel::GetElapsedTime()
{
    return savedElapsedTime_ + elapsedTime_;
}

uint64_t StopwatchModel::UpdateElapsedTime()
{
#ifdef _WIN32
    struct timeval currTime = {0};
    gettimeofday(&currTime, NULL);
#else
    struct timeval64 currTime = {0};
    gettimeofday64(&currTime, NULL);
#endif
    elapsedTime_ = (currTime.tv_sec * UNIT_SEC_TO_MSEC + currTime.tv_usec / UNIT_SEC_TO_MSEC) -
                   (startTime_.tv_sec * UNIT_SEC_TO_MSEC + startTime_.tv_usec / UNIT_SEC_TO_MSEC);
    return savedElapsedTime_ + elapsedTime_;
}

void StopwatchModel::ResetCounter()
{
    elapsedTime_ = 0;
    savedElapsedTime_ = 0;
    records_.Clear();
}

bool StopwatchModel::GetRunningState()
{
    return isRunning_;
}

void StopwatchModel::SetRunningState(bool isRunning)
{
    isRunning_ = isRunning;
}

List<StopwatchItem> &StopwatchModel::GetRecordsList()
{
    return records_;
}

void StopwatchModel::AddRecordToList(const StopwatchItem &record)
{
    ListNode<StopwatchItem> *node = records_.Head();
    while (node != records_.End()) {
        StopwatchItem &itemData = node->data_;
        itemData.isLatest = false;
        node = node->next_;
    }
    records_.PushFront(record);
}

bool StopwatchModel::GetRecordListVisible()
{
    return isRecordListVisible_;
}

void StopwatchModel::SetRecordListVisible(bool isVisible)
{
    isRecordListVisible_ = isVisible;
}

bool StopwatchModel::GetTimeOverRangeState()
{
    return isTimeOverRange_;
}

void StopwatchModel::SetTimeOverRangeState(bool isTimeOverRange)
{
    isTimeOverRange_ = isTimeOverRange;
}
}

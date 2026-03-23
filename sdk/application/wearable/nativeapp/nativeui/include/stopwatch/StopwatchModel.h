/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: StopwatchModel
 * Create: 2025-06-06
 */

#ifndef STOPWATCH_MODEL_H
#define STOPWATCH_MODEL_H

#include "gfx_utils/list.h"
#ifndef _WIN32
#include "time64.h"
#endif
#include "StopwatchItemView.h"

namespace OHOS {
class StopwatchModel {
public:
    ~StopwatchModel();
    static StopwatchModel &GetInstance(void);

    void SetStartTime();
    void SaveElapsedTime();
    uint64_t GetElapsedTime();
    uint64_t UpdateElapsedTime();
    void ResetCounter();
    bool GetRunningState();
    void SetRunningState(bool isRunning);
    bool GetRecordListVisible();
    void SetRecordListVisible(bool isVisible);
    bool GetTimeOverRangeState();
    void SetTimeOverRangeState(bool isTimeOverRange);
    List<StopwatchItem> &GetRecordsList();
    void AddRecordToList(const StopwatchItem &record);
private:
    StopwatchModel();
    StopwatchModel(const StopwatchModel &);
    StopwatchModel &operator=(const StopwatchModel &);
#ifdef _WIN32
    struct timeval startTime_{0};
#else
    struct timeval64 startTime_{0};
#endif
    uint64_t elapsedTime_{0};
    uint64_t savedElapsedTime_{0};
    bool isTimeOverRange_{false};
    bool isRunning_{false};
    bool isRecordListVisible_{false};
    List<StopwatchItem> records_;
};
}

#endif  // STOPWATCH_MODEL_H
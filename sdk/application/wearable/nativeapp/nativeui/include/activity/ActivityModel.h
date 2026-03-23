/*
 * Copyright (c) CompanyNameMagicTag 2021-2021. All rights reserved.
 * Description: ActivityModel view
 * Author:
 * Create: 2022-03-02
 */

#ifndef ACTIVITY_MODEL_H
#define ACTIVITY_MODEL_H

#define NUM_7 7

#include <ctime>
#include <cstdio>
#include "ohos_types.h"
#include "rtcservice/RtcService.h"
#include "samgr_lite.h"

namespace OHOS {
typedef enum {
    MONDAY_STEP_COUNTER = 0,
    TUESDAY_STEP_COUNTER,
    WEDNESDAY_STEP_COUNTER,
    THURSDAY_STEP_COUNTER,
    FRIDAY_STEP_COUNTER,
    SATURDAY_STEP_COUNTER,
    SUNDAY_STEP_COUNTER,
} StepDataDistribute;

typedef struct {
    uint32 weekSteps[NUM_7];
} StepAllData;

class ActivityModel {
public:
    ~ActivityModel();
    void Init();
    static ActivityModel *GetInstance(void);
    uint32 GetStepCounter(void);
    void ReviseStepCounter(uint32 &counter);
    void GetStepData(const StepDataDistribute &type, uint32_t *value);
    void SetStepData(const uint32_t &value);
    void SaveStepToFile(struct tm *tm_data);
    int GetWeekToday(void);
    uint32 GetLastSavedValue(void);
    uint32_t ZeroHourStepHandle(void);
private:
    ActivityModel();
    bool InitStepData(void);
    uint32 stepCounter;
    uint32 lastSavedValue;
    int weekToday;
};
}

#endif
/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: HeartRateModel
 * Create: 2025-06
 */

#ifndef HREATE_RATE_ITEM_MODEL_H
#define HREATE_RATE_ITEM_MODEL_H

#include <string>
#include <list>
#include <sys/time.h>
#include "graphic_timer.h"

namespace OHOS {
static constexpr uint16_t HEART_ALL_POINTS = 360;

typedef struct {
    uint16_t heartRate;
    struct timeval time;
} heartRateMeasurementInfo;

typedef struct {
    /** X coordinate */
    uint16_t x;
    /** Y coordinate */
    uint16_t y;
} HeartPoint;

typedef struct {
    uint16_t perSec;
    uint16_t rest;
    uint16_t minX;
    uint16_t minValue;
    uint16_t maxX;
    uint16_t maxValue;
    uint16_t index;
    HeartPoint value[HEART_ALL_POINTS];
} HeartDatas;

class HeartRateModel {
public:
    static HeartRateModel &GetInstance(void);
    void SetHeartRate(heartRateMeasurementInfo value)
    {
        heartRate = value;
    };
    heartRateMeasurementInfo GetHeartRate()
    {
        return heartRate;
    };

private:
    HeartRateModel();
    HeartRateModel(const HeartRateModel &);
    HeartRateModel &operator=(const HeartRateModel &);
    virtual ~HeartRateModel();
    heartRateMeasurementInfo heartRate = {0, 0};
};
}
#endif  // HREATE_RATE_ITEM_MODEL_H
/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: BloodModel
 * Create: 2025-06
 */

#ifndef BLOOD_MODEL_H
#define BLOOD_MODEL_H

#include <string>
#include <list>
#include <sys/time.h>
#include "graphic_timer.h"

namespace OHOS {
typedef struct {
    uint16_t blood;
    struct timeval time;
} bloodMeasurementInfo;
class BloodModel {
public:
    static BloodModel &GetInstance(void);
    void SetBlood(bloodMeasurementInfo value)
    {
        blood = value;
    };
    bloodMeasurementInfo GetBlood()
    {
        return blood;
    };

private:
    BloodModel();
    BloodModel(const BloodModel &);
    BloodModel &operator=(const BloodModel &);
    virtual ~BloodModel();
    bloodMeasurementInfo blood = {0, 0};
};
}
#endif  // BLOOD_MODEL_H
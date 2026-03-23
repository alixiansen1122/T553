/*
 * Copyright (c) CompanyNameMagicTag 2023. All rights reserved.
 * Description: DialModelHeartRate
 * Author:
 * Create: 2023-12
 */

#ifndef DIAL_MODEL_HEART_RATE_H
#define DIAL_MODEL_HEART_RATE_H

#include "main/dial/DialBaseModel.h"

namespace OHOS {
class DialModelHeartRate : public DialBaseModel {
public:
    DialModelHeartRate() {}
    ~DialModelHeartRate() override {}
    static DialModelHeartRate* GetInstance()
    {
        static DialModelHeartRate instance;
        return &instance;
    }
    bool GetDialFloatData(DialDataType& type, float& out) override;
    void UpdateHeartRate(int16_t heartRate);

private:
    float heartRate_ = 67.0; // 67.0: default heart value as reference
};
}
#endif

/*
 * Copyright (c) CompanyNameMagicTag 2023. All rights reserved.
 * Description: DialModelPower
 * Author:
 * Create: 2023-12
 */

#ifndef DIAL_MODEL_POWER_H
#define DIAL_MODEL_POWER_H

#include "main/dial/DialBaseModel.h"

namespace OHOS {
class DialModelPower : public DialBaseModel {
public:
    DialModelPower() {}
    ~DialModelPower() override {}
    static DialModelPower* GetInstance()
    {
        static DialModelPower instance;
        return &instance;
    }
    bool GetDialFloatData(DialDataType& type, float& out) override;

private:
    int16_t power_ = 90; // 90: default power level
};
}
#endif

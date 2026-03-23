/*
 * Copyright (c) CompanyNameMagicTag 2023. All rights reserved.
 * Description: DialModelPower
 * Author:
 * Create: 2023-12
 */

#include "main/dial/RegisterDialModel.h"
#include "main/dial/DialModelPower.h"

REGIST_DIAL_MODULE(OHOS::DialDataType::BATTERY, OHOS::DialModelPower::GetInstance());

namespace OHOS {
static RegisterDialModel REGIST_BATTERY_LEVEL(DialDataType::BATTERY_LEVEL, DialModelPower::GetInstance());
static RegisterDialModel REGIST_BATTERY_ENUM(DialDataType::BATTERY_ENUM, DialModelPower::GetInstance());
bool DialModelPower::GetDialFloatData(DialDataType& type, float& out)
{
    switch (type) {
    case DialDataType::BATTERY: {
        out = power_;
        break;
    }
    case DialDataType::BATTERY_LEVEL: {
        out = static_cast<float>(power_) / 100.0; // 100.0: full power
        break;
    }
    case DialDataType::BATTERY_ENUM: {
        out = static_cast<float>(power_ / 10); // 10: num
        break;
    }
    default:
        (void)type;
    }
    return true;
}
}

/*
 * Copyright (c) CompanyNameMagicTag 2023. All rights reserved.
 * Description: DialModelHeartRate
 * Author:
 * Create: 2023-12
 */

#include "main/dial/RegisterDialModel.h"
#include "main/dial/DialModelHeartRate.h"

REGIST_DIAL_MODULE(OHOS::DialDataType::HEART_RATE, OHOS::DialModelHeartRate::GetInstance());

namespace OHOS {
bool DialModelHeartRate::GetDialFloatData(DialDataType& type, float& out)
{
    (void)type;
    out = heartRate_;
    return true;
}

void DialModelHeartRate::UpdateHeartRate(int16_t heartRate)
{
    heartRate_ = static_cast<float>(heartRate);
    if (onChangeListener_ != nullptr) {
        onChangeListener_->OnDialFloatDataUpdate(DialDataType::HEART_RATE, heartRate_);
    }
}
}

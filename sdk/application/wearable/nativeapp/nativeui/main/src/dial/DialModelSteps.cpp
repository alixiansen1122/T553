/*
 * Copyright (c) CompanyNameMagicTag 2023. All rights reserved.
 * Description: DialModelSteps
 * Author:
 * Create: 2023-12
 */

#include "main/dial/RegisterDialModel.h"
#include "main/dial/DialModelSteps.h"

REGIST_DIAL_MODULE(OHOS::DialDataType::STEP_COUNT, OHOS::DialModelSteps::GetInstance());

namespace OHOS {
bool DialModelSteps::GetDialFloatData(DialDataType& type, float& out)
{
    (void)type;
    out = steps_;
    return true;
}

void DialModelSteps::UpdateSteps(int16_t steps)
{
    steps_ = static_cast<float>(steps);
    if (onChangeListener_ != nullptr) {
        onChangeListener_->OnDialFloatDataUpdate(DialDataType::STEP_COUNT, steps_);
    }
}
}

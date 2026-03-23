/*
/*
 * Copyright (c) CompanyNameMagicTag 2025. All rights reserved.
 * Description: DialModelArcProgress
 * Author: Hisi Graphic
 * Create: 2025-08
 */

#include "main/dial/RegisterDialModel.h"
#include "main/dial/DialModelArcProgress.h"
#include "gfx_utils/graphic_math.h"

REGIST_DIAL_MODULE(OHOS::DialDataType::ARC_PROGRESS_TEST, OHOS::DialModelArcProgress::GetInstance());

namespace OHOS {
DialModelArcProgress* DialModelArcProgress::GetInstance()
{
    static DialModelArcProgress instance;
    return &instance;
}

bool DialModelArcProgress::GetDialFloatData(DialDataType& type, float& out)
{
    (void)type;
    out = data_;
    data_ = FloatMore(data_, 1.0) ? 0.1 : data_ + 0.01;
    return true;
}
}

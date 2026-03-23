/*
 * Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
 * Description: CompassModel.cpp
 * Author:
 * Create: 2022-04-02
 */

#include "compass/CompassModel.h"
#include "gfx_utils/graphic_math.h"

#ifdef __cplusplus
extern "C" {
#endif

using namespace OHOS;

#ifdef __cplusplus
}
#endif

namespace OHOS {
static constexpr int16_t DELTA_ANGLE = 1;

CompassModel *CompassModel::GetInstance(void)
{
    static CompassModel instance;
    return &instance;
}

void CompassModel::UpdateAngle()
{
    if (clockwise_) {
        angle_ += DELTA_ANGLE;
    } else {
        angle_ -= DELTA_ANGLE;
    }
    if (angle_ >= CIRCLE_IN_DEGREE) {
        angle_ = 360;
        clockwise_ = false;
    }
    if (angle_ <= 0) {
        angle_ = 0;
        clockwise_ = true;
    }
}

int16_t CompassModel::GetAngle(void)
{
    return angle_;
}

void CompassModel::ResetAngle(void)
{
    angle_ = 0;
}

bool CompassModel::OpenCompassSensor(void)
{
    // Add implemetion later
    return true;
}

bool CompassModel::CloseCompassSensor(void)
{
    // Add implemetion later
    return true;
}
}


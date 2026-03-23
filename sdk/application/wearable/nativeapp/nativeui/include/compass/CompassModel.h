/*
 * Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
 * Description: CompassModel.h
 * Author:
 * Create: 2022-04-02
 */

#ifndef COMPASS_MODEL_H
#define COMPASS_MODEL_H

#include <cstdint>

namespace OHOS {
class CompassModel {
public:
    static CompassModel *GetInstance(void);
    bool OpenCompassSensor(void);
    bool CloseCompassSensor(void);
    void UpdateAngle(void);
    void ResetAngle(void);
    int16_t GetAngle(void);

private:
    int16_t angle_{0};
    bool clockwise_{true};
};
}

#endif


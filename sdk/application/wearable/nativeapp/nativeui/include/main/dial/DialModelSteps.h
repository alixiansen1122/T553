/*
 * Copyright (c) CompanyNameMagicTag 2023. All rights reserved.
 * Description: DialModelSteps
 * Author:
 * Create: 2023-12
 */

#ifndef DIAL_MODEL_STEPS_H
#define DIAL_MODEL_STEPS_H

#include "main/dial/DialBaseModel.h"

namespace OHOS {
class DialModelSteps : public DialBaseModel {
public:
    DialModelSteps() {}
    ~DialModelSteps() override {}
    static DialModelSteps* GetInstance()
    {
        static DialModelSteps instance;
        return &instance;
    }
    bool GetDialFloatData(DialDataType& type, float& out) override;
    void UpdateSteps(int16_t steps);

private:
    float steps_ = 11520.0; // 11520.0: default step value as reference
};
}
#endif

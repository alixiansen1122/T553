/*
 * Copyright (c) CompanyNameMagicTag 2025. All rights reserved.
 * Description: DialModelArcProgress
 * Author: Hisi Graphic
 * Create: 2025-08
 */

#ifndef DIAL_MODEL_ARC_PROGRESS_H
#define DIAL_MODEL_ARC_PROGRESS_H

#include "main/dial/DialBaseModel.h"

namespace OHOS {
class DialModelArcProgress : public DialBaseModel {
public:
    static DialModelArcProgress* GetInstance();
    bool GetDialFloatData(DialDataType& type, float& out) override;
private:
    DialModelArcProgress() {}
    ~DialModelArcProgress() override {}

    DialModelArcProgress(const DialModelArcProgress &) = delete;
    const DialModelArcProgress &operator=(const DialModelArcProgress &) = delete;
    float data_ = 0.1; // 0.1: default heart value as reference
};
}
#endif
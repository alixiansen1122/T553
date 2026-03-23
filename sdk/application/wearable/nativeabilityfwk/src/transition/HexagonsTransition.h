/*
 * Copyright (c) CompanyNameMagicTag 2024. All rights reserved.
 * Description: HexagonsTransition
 * Author:
 * Create: 2024-04
 */

#ifndef HEXAGONS_TRASITION_H
#define HEXAGONS_TRASITION_H
#include "TransitionCallback.h"
#include "components/ui_custom_hexagons_list.h"

namespace OHOS {
class HexagonsTransition : public TransitionCallback {
public:
    HexagonsTransition(uint32_t time, bool enableCurrentSnapshot, bool enableTargetSnapshot) :
        TransitionCallback(time, enableCurrentSnapshot, enableTargetSnapshot) {}
    ~HexagonsTransition() override {}
    void OnTransitionStart(UIViewGroup* current, UIImageView* target) override;
    void TransitionAlg(uint32_t time) override;
    void OnTransitionEnd() override;
private:
    uint32_t durationBackup_ = 0;
    UICustomHexagonsList* current_ = nullptr;
    UIImageView* target_ = nullptr;
    UIImageView* focusView_ = nullptr;
    uint8_t focusIndex_ = 0;
    float scaleStart_ = 1.0f;
    float scaleEnd_ = 1.0f;
    uint8_t opaStart_ = 255;
    int16_t xDistance_ = 0;
    int16_t yDistance_ = 0;
    int16_t prevX_ = 0;
    int16_t prevY_ = 0;
    float thresholdTime_ = 0.0f;
    int16_t targetXStart_ = 0;
    int16_t targetYStart_ = 0;
    float targetScaleStart_ = 1.0f;
};
} // namespace OHOS
#endif
/*
 * Copyright (c) CompanyNameMagicTag 2024. All rights reserved.
 * Description: BackToHexagonsTransition
 * Author:
 * Create: 2024-04
 */

#ifndef HEXAGONS_TRASITION_H
#define HEXAGONS_TRASITION_H
#include "TransitionCallback.h"
#include "components/ui_custom_hexagons_list.h"

namespace OHOS {
class BackToHexagonsTransition : public TransitionCallback {
public:
    BackToHexagonsTransition(uint32_t time, bool enableCurrentSnapshot, bool enableTargetSnapshot) :
        TransitionCallback(time, enableCurrentSnapshot, enableTargetSnapshot) {}
    ~BackToHexagonsTransition() override {}
    void OnTransitionStart(UIImageView* current, UIViewGroup* target) override;
    void TransitionAlg(uint32_t time) override;
    void OnTransitionEnd() override;
private:
    UIImageView* current_ = nullptr;
    UICustomHexagonsList* target_ = nullptr;
    UIView* focusView_ = nullptr;
    uint8_t focusIndex_ = 0;
    float curScaleEnd_ = 1.0f;
    float tarScaleStart_ = 1.0f;
};
} // namespace OHOS
#endif
/*
 * Copyright (c) CompanyNameMagicTag 2024. All rights reserved.
 * Description: BackToWaterfallTransition
 * Author:
 * Create: 2024-08
 */

#ifndef WATERFALL_TRASITION_H
#define WATERFALL_TRASITION_H
#include "TransitionCallback.h"
#include "components/ui_waterfall_list.h"

namespace OHOS {
class BackToWaterfallTransition : public TransitionCallback {
public:
    BackToWaterfallTransition(uint32_t time, bool enableCurrentSnapshot, bool enableTargetSnapshot) :
        TransitionCallback(time, enableCurrentSnapshot, enableTargetSnapshot) {}
    ~BackToWaterfallTransition() override {}
    void OnTransitionStart(UIImageView* current, UIViewGroup* target) override;
    void TransitionAlg(uint32_t time) override;
    void OnTransitionEnd() override;
private:
    UIImageView* current_ = nullptr;
    UIWaterfallList* target_ = nullptr;
    UIView* focusView_ = nullptr;
    uint8_t focusIndex_ = 0;
    int16_t xEnd_ = 0;
    int16_t yEnd_ = 0;
    float curScaleEnd_ = 1.0f;
    float tarScaleStart_ = 1.0f;
};
} // namespace OHOS
#endif
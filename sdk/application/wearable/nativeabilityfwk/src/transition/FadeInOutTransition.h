/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: FadeInOutTransition
 * Create: 2025-04
 */

#ifndef FADE_IN_OUT_TRANSITION_H
#define FADE_IN_OUT_TRANSITION_H
#include "TransitionCallback.h"

namespace OHOS {
class FadeInOutTransition : public TransitionCallback {
public:
    FadeInOutTransition(uint32_t time, bool enableCurrentSnapshot, bool enableTargetSnapshot) :
        TransitionCallback(time, enableCurrentSnapshot, enableTargetSnapshot) {}
    ~FadeInOutTransition() override {}
    void OnTransitionStart(UIImageView* current, UIImageView* target) override;
    void TransitionAlg(uint32_t time) override;
    void OnTransitionEnd() override;
private:
    UIImageView* current_ = nullptr;
    UIImageView* target_ = nullptr;
};
} // namespace OHOS
#endif // FADE_IN_OUT_TRANSITION_H
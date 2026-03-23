/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: TransparentGradientTransition
 * Create: 2025-04
 */

#ifndef TRANSPARENT_GRADIENT_TRANSITION_H
#define TRANSPARENT_GRADIENT_TRANSITION_H

#include "TransitionCallback.h"

namespace OHOS {
    class TransparentGradientTransition : public TransitionCallback {
    public:
        TransparentGradientTransition(uint32_t time, bool enableCurrentSnapshot, bool enableTargetSnapshot) :
            TransitionCallback(time, enableCurrentSnapshot, enableTargetSnapshot) {}
        ~TransparentGradientTransition() override {}

        void OnTransitionStart(UIImageView* current, UIImageView* target) override;
        void TransitionAlg(uint32_t time) override;
        void OnTransitionEnd() override;
    private:

        UIImageView* current_ = nullptr;
        UIImageView* target_ = nullptr;
        uint8_t currentOpa_;
        uint8_t targetOpa_;
        int16_t transIn_;
        int16_t transOut_;
    };
} // namespace OHOS

#endif
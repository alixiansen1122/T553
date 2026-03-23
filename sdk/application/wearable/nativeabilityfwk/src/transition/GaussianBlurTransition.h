/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: GaussianBlurTransition
 * Create: 2025-04
 */

#ifndef GAUSSIAN_BLUR_TRANSITION_H
#define GAUSSIAN_BLUR_TRANSITION_H

#include "TransitionCallback.h"

namespace OHOS {
    class GaussianBlurTransition : public TransitionCallback {
    public:
        GaussianBlurTransition(uint32_t time, bool enableCurrentSnapshot, bool enableTargetSnapshot) :
            TransitionCallback(time, enableCurrentSnapshot, enableTargetSnapshot) {}
        ~GaussianBlurTransition() override {}

        void OnTransitionStart(UIViewGroup* current, UIViewGroup* target) override;
        void TransitionAlg(uint32_t time) override;
        void OnTransitionEnd() override;
    private:

        void AddBlurView(UIViewGroup* groupView);
        void SetViewBlur(UIViewGroup* groupView, float guassScale);
        void ClearBlurView(UIViewGroup* groupView);

        UIViewGroup* current_ = nullptr;
        UIViewGroup* target_ = nullptr;
        bool blurFlag_;
    };
} // namespace OHOS

#endif // GAUSSIAN_BLUR_TRANSITION_H
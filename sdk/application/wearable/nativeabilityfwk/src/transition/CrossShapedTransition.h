/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: CrossShapedTransition
 * Create: 2025-04
 */

#ifndef CROSS_SHAPED_TRANSITION_H
#define CROSS_SHAPED_TRANSITION_H

#include "TransitionCallback.h"

namespace OHOS {
    class CrossShapedTransition : public TransitionCallback {
    public:
        CrossShapedTransition(uint32_t time, bool enableCurrentSnapshot, bool enableTargetSnapshot) :
            TransitionCallback(time, enableCurrentSnapshot, enableTargetSnapshot) {}
        ~CrossShapedTransition() override {}

        void OnTransitionStart(UIImageView* current, UIImageView* target) override;
        void TransitionAlg(uint32_t time) override;
        void OnTransitionEnd() override;
    private:

        void CommonAlg(UIView* visualView, int16_t xOffset, bool visible);
        UIView* GetVisibleView(int16_t xOffset, bool &visible);

        UIImageView* currentView_ = nullptr;
        UIImageView* targetView_ = nullptr;
        UIImageView* cameraView_ = nullptr;

        int16_t middleWidth_;
        int16_t middleHeight_;
        int16_t screenWidth_;
        int16_t screenHeight_;
    };

} // namespace OHOS

#endif // CROSS_SHAPED_TRANSITION_H
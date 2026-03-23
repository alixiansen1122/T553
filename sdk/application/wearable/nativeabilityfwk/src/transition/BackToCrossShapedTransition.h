/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: BackToCrossShapedTransition
 * Create: 2025-04
 */

#ifndef BACK_TO_CROSS_SHAPED_TRANSITION_H
#define BACK_TO_CROSS_SHAPED_TRANSITION_H

#include "TransitionCallback.h"

namespace OHOS {
    class BackToCrossShapedTransition : public TransitionCallback {
    public:
        BackToCrossShapedTransition(uint32_t time, bool enableCurrentSnapshot, bool enableTargetSnapshot) :
            TransitionCallback(time, enableCurrentSnapshot, enableTargetSnapshot) {}
        ~BackToCrossShapedTransition() override {}

        void OnTransitionStart(UIImageView* current, UIImageView* target) override;
        void TransitionAlg(uint32_t time) override;
        void OnTransitionEnd() override;
    private:

        void CommonAlg(UIView* visualView, int16_t offSet);
        UIView* GetVisibleView(int16_t offSet);

        UIImageView* currentView_ = nullptr;
        UIImageView* targetView_ = nullptr;
        UIImageView* cameraView_ = nullptr;

        int16_t middleWidth_;
        int16_t middleHeight_;
        int16_t screenWidth_;
        int16_t screenHeight_;
    };

} // namespace OHOS

#endif // BACK_TO_CROSS_SHAPED_TRANSITION_H
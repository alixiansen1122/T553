/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: ZoomTransition
 * Create: 2025-04
 */

#ifndef ZOOM_TRANSITION_H
#define ZOOM_TRANSITION_H
#include "TransitionCallback.h"

namespace OHOS {
class ZoomTransition : public TransitionCallback {
public:
    ZoomTransition(uint32_t time, bool enableCurrentSnapshot, bool enableTargetSnapshot) :
        TransitionCallback(time, enableCurrentSnapshot, enableTargetSnapshot) {}
    ~ZoomTransition() override {}
    void OnTransitionStart(UIImageView* current, UIImageView* target) override;
    void TransitionAlg(uint32_t time) override;
private:
    UIImageView* current_ = nullptr;
    UIImageView* target_ = nullptr;
    float middleWidth_;
    float middleHeight_;
};
} // namespace OHOS
#endif // ZOOM_TRANSITION_H
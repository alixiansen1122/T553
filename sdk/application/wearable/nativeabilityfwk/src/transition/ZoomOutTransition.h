/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: ZoomOutTransition
 * Create: 2025-04
 */

#ifndef ZOOM_OUT_TRANSITION_H
#define ZOOM_OUT_TRANSITION_H
#include "TransitionCallback.h"

namespace OHOS {
class ZoomOutTransition : public TransitionCallback {
public:
    ZoomOutTransition(uint32_t time, bool enableCurrentSnapshot, bool enableTargetSnapshot) :
        TransitionCallback(time, enableCurrentSnapshot, enableTargetSnapshot) {}
    ~ZoomOutTransition() override {}
    void OnTransitionStart(UIImageView* current, UIImageView* target) override;
    void TransitionAlg(uint32_t time) override;
private:
    UIImageView* current_ = nullptr;
    UIImageView* target_ = nullptr;
    float middleWidth_;
    float middleHeight_;
};
} // namespace OHOS
#endif // ZOOM_OUT_TRANSITION_H
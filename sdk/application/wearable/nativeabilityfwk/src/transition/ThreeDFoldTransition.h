/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: ThreeDFoldTransition
 * Create: 2025-04
 */

#ifndef THREE_D_FOLD_TRANSITION_H
#define THREE_D_FOLD_TRANSITION_H
#include "TransitionCallback.h"

namespace OHOS {
class ThreeDFoldTransition : public TransitionCallback {
public:
    ThreeDFoldTransition(uint32_t time, bool enableCurrentSnapshot, bool enableTargetSnapshot) :
        TransitionCallback(time, enableCurrentSnapshot, enableTargetSnapshot) {}
    ~ThreeDFoldTransition() override {}
    void OnTransitionStart(UIImageView* current, UIImageView* target) override;
    void TransitionAlg(uint32_t time) override;
    void OnTransitionEnd() override;
private:
    UIImageView* current_ = nullptr;
    UIImageView* target_ = nullptr;
};
} // namespace OHOS
#endif // THREE_D_FOLD_TRANSITION_H
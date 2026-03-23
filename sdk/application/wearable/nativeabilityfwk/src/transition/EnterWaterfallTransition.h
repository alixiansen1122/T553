/*
 * Copyright (c) CompanyNameMagicTag 2024. All rights reserved.
 * Description: EnterWaterfallTransition
 * Author:
 * Create: 2024-08
 */

#ifndef ENTER_WATERFALL_TRANSITION_H
#define ENTER_WATERFALL_TRANSITION_H
#include "TransitionCallback.h"
#include "components/ui_waterfall_list.h"


namespace OHOS {
class EnterWaterfallTransition : public TransitionCallback {
public:
    EnterWaterfallTransition(uint32_t time, bool enableCurrentSnapshot, bool enableTargetSnapshot) :
        TransitionCallback(time, enableCurrentSnapshot, enableTargetSnapshot) {}
    ~EnterWaterfallTransition() override {}
    void OnTransitionStart(UIImageView* current, UIViewGroup* target) override;
    void TransitionAlg(uint32_t time) override;
    void OnTransitionEnd() override;
private:
    UIWaterfallList* target_ = nullptr;
    UIImageView* current_ = nullptr;
};
} // namespace OHOS
#endif
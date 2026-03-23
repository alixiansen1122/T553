/*
 * Copyright (c) CompanyNameMagicTag 2024. All rights reserved.
 * Description: EnterWaterfallTransition
 * Author:
 * Create: 2024-04
 */


#include "EnterWaterfallTransition.h"
#include "animator/easing_equation.h"
#include "components/root_view.h"
#include "TransitionManager.h"
#include "wearable_log.h"
#include "TransitionUtils.h"

namespace OHOS {
REGIST_TRANSITION(TransitionType::TRANSITION_ENTER_WATERFALL, EnterWaterfallTransition, 300, true, false);
static const float SCALE_INIT = 1.5f;
static const float SCALE_END = 1.0f;

void EnterWaterfallTransition::OnTransitionStart(UIImageView* current, UIViewGroup* target)
{
    current_ = current;
    target_ = dynamic_cast<UIWaterfallList*>(TransitionUtils::GetViewAndTransparentizePath(target, UI_WATERFALL_LIST));
    if ((target_ == nullptr) || (current_ == nullptr)) {
        return;
    }
}

void EnterWaterfallTransition::TransitionAlg(uint32_t time)
{
    if ((current_ == nullptr) || (target_ == nullptr)) {
        return;
    }
    current_->SetOpaScale(0); // 0, transparency
    float timeRatio = 2 * time > duration_ ? 1 : 2 * time * 1.0f / duration_;
    float curScale = timeRatio * (SCALE_END - SCALE_INIT) + SCALE_INIT;
    target_->Scale(curScale);
    target_->EnterMove(time, duration_);
}

void EnterWaterfallTransition::OnTransitionEnd()
{
    if (target_ != nullptr) {
        current_->SetOpaScale(0); // 0, transparency
        target_->Scale(SCALE_END);
        target_->EnterMove(duration_, duration_);
    }
}
}
/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: TransparentGradientTransition
 * Create: 2025-04
 */

#include "animator/easing_equation.h"
#include "components/root_view.h"
#include "TransitionManager.h"
#include "wearable_log.h"
#include "TransparentGradientTransition.h"


namespace OHOS {
    static const uint8_t FADE_OUT_OPACITY = 250;
    static const uint8_t FADE_IN_OPACITY = 30;
    static const uint8_t FADE_DELTA = 200;
    static const int16_t FADE_IN_TRANS = 5;
    static const int16_t FADE_OUT_TRANS = -5;
    static constexpr int16_t OPA_MAX = 255;

    REGIST_TRANSITION(TransitionType::TRANSITION_TRANSPARENT_GRADIENT, TransparentGradientTransition, 400, true, true);
    void TransparentGradientTransition::OnTransitionStart(UIImageView* current, UIImageView* target)
    {
        if ((current == nullptr) || (target == nullptr)) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "TransparentGradientTransition ImageView is empty!");
            return;
        }

        current_ = current;
        target_ = target;

        // currentView fade out
        currentOpa_ = FADE_OUT_OPACITY;
        transOut_ = 0;
        Vector2<int16_t> transTarget(transIn_, 0);
        current_->SetOpaScale(currentOpa_);

        // targeview fade in
        transIn_ = FADE_IN_TRANS;
        targetOpa_ = FADE_IN_OPACITY;
        target_->Translate(transTarget);
        target_->SetOpaScale(targetOpa_);
    }

    void TransparentGradientTransition::TransitionAlg(uint32_t time)
    {
        if ((current_ == nullptr) || (target_ == nullptr)) {
            return;
        }

        float scale = EasingEquation::QuadEaseIn(0, OPA_MAX, time, duration_) / static_cast<float>(OPA_MAX);
        uint8_t opa = uint8_t(scale * FADE_DELTA);
        currentOpa_ = FADE_OUT_OPACITY - opa;
        targetOpa_ = FADE_IN_OPACITY + opa;
        transIn_ = FADE_IN_TRANS - int16_t(scale*FADE_IN_TRANS);
        transOut_ = int16_t(scale*FADE_OUT_TRANS);

        Vector2<int16_t> transCurrent(transOut_, 0);
        Vector2<int16_t> transTarget(transIn_, 0);

        current_->Translate(transCurrent);
        current_->SetOpaScale(currentOpa_);

        target_->Translate(transTarget);
        target_->SetOpaScale(targetOpa_);
    }

    void TransparentGradientTransition::OnTransitionEnd()
    {
        if (current_ == nullptr || target_ == nullptr) {
            return;
        }

        target_->SetOpaScale(OPA_OPAQUE);
        current_->SetOpaScale(OPA_TRANSPARENT);
    }
}
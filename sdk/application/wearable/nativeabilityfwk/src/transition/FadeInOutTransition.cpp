/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: FadeInOutTransition
 * Create: 2025-04
 */

#include "animator/easing_equation.h"
#include "common/screen.h"
#include "TransitionManager.h"
#include "wearable_log.h"
#include "FadeInOutTransition.h"

namespace OHOS {
static constexpr int16_t DURA_RANGE = 400;
static constexpr int16_t FADE_IN_END = 300;
static constexpr int16_t FADE_IN_OPASTART = OPA_OPAQUE - 30;
static constexpr int16_t FADE_IN_OPAEND = 0;
static constexpr int16_t FADE_OUT_START = 100;
static constexpr int16_t FADE_OUT_OPASTART = 0;
static constexpr int16_t FADE_OUT_OPAEND = OPA_OPAQUE - 30;
static constexpr int16_t FADE_OPA_DELTA = 15;

REGIST_TRANSITION(TransitionType::TRANSITION_FADEINOUT, FadeInOutTransition, DURA_RANGE, true, true);

void FadeInOutTransition::OnTransitionStart(UIImageView* current, UIImageView* target)
{
    if ((current == nullptr) || (target == nullptr)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "FadeInOutTransition no current or target\n");
        return;
    }
    current_ = current;
    target_ = target;
    target_->SetVisible(false);
    current_->SetOpaScale(OPA_OPAQUE - FADE_OPA_DELTA);
    current_->Invalidate();
    target_->SetOpaScale(0);
}

void FadeInOutTransition::TransitionAlg(uint32_t time)
{
    if ((current_ == nullptr) || (target_ == nullptr)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "FadeInOutTransition no current or target\n");
        return;
    }
    if (time < FADE_IN_END) {
        float curFloatOpa = (FADE_IN_OPASTART - FADE_IN_OPAEND) * (FADE_IN_END - time) * 1.0f;
        uint8_t curOpa = static_cast<uint8_t> (curFloatOpa / FADE_IN_END);
        curOpa = curOpa + FADE_IN_OPAEND;
        current_->SetOpaScale(curOpa);
        current_->Invalidate();
    }
    if (time > FADE_OUT_START) {
        target_->SetVisible(true);
        float curFloatOpa = (FADE_OUT_OPAEND - FADE_OUT_OPASTART)  * (time - FADE_OUT_START) * 1.0f;
        uint8_t tarOpa = static_cast<uint8_t> (curFloatOpa / (DURA_RANGE - FADE_OUT_START));
        tarOpa = tarOpa + FADE_OUT_OPASTART;
        target_->SetOpaScale(tarOpa);
        target_->Invalidate();
    }
}

void FadeInOutTransition::OnTransitionEnd()
{
    if ((current_ == nullptr) || (target_ == nullptr)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "FadeInOutTransition no current or target\n");
        return;
    }
    current_->SetVisible(false);
    target_->SetVisible(true);
    target_->SetOpaScale(OPA_OPAQUE - FADE_OPA_DELTA);
    target_->Invalidate();
}
} // namespace OHOS

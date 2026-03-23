/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: ZoomTransition
 * Create: 2025-04
 */

#include "animator/easing_equation.h"
#include "common/screen.h"
#include "TransitionManager.h"
#include "wearable_log.h"
#include "ZoomTransition.h"

namespace OHOS {
static constexpr uint16_t DURATION = 160;
static constexpr uint16_t OPA_DURATION = 100;
static constexpr float NUM2 = 2.0f;
static constexpr float SCALE_BASE = 1;

REGIST_TRANSITION(TransitionType::TRANSITION_ZOOM, ZoomTransition, DURATION, true, true);

void ZoomTransition::OnTransitionStart(UIImageView* current, UIImageView* target)
{
    if (current == nullptr || target == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "ZoomTransition ImageView is empty!");
        return;
    }
    current_ = current;
    target_ = target;
    middleWidth_ = Screen::GetInstance().GetWidth() / NUM2;
    middleHeight_ = Screen::GetInstance().GetHeight() / NUM2;
}

void ZoomTransition::TransitionAlg(uint32_t time)
{
    float zoom_scale = EasingEquation::LinearEaseNone(0, DURATION, time, duration_) \
                                        / static_cast<float>(DURATION);
    float zoom_out_scale = SCALE_BASE + zoom_scale;
    float zoom_in_scale = zoom_scale;
    uint8_t opaScale = EasingEquation::CubicEaseOut(OPA_OPAQUE / NUM2, OPA_OPAQUE, time, OPA_DURATION);
    current_->Scale(Vector2<float>{zoom_out_scale, zoom_out_scale}, Vector2<float>{middleWidth_, middleHeight_});
    target_->SetOpaScale(opaScale);
    target_->Scale(Vector2<float>{zoom_in_scale, zoom_in_scale}, Vector2<float>{middleWidth_, middleHeight_});
}
} // namespace OHOS

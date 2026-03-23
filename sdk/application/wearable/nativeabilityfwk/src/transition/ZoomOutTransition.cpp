/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: ZoomOutTransition
 * Create: 2025-04
 */

#include "animator/easing_equation.h"
#include "common/screen.h"
#include "components/root_view.h"
#include "TransitionManager.h"
#include "wearable_log.h"
#include "ZoomOutTransition.h"

namespace OHOS {
static constexpr int16_t DURATION = 200;
static constexpr float HALF = 0.5f;
static constexpr float NUM2 = 2.0f;
static constexpr int16_t SCALE_BASE = 1;
REGIST_TRANSITION(TransitionType::TRANSITION_ZOOM_OUT, ZoomOutTransition, DURATION, true, true);

void ZoomOutTransition::OnTransitionStart(UIImageView* current, UIImageView* target)
{
    if (current == nullptr || target == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "ZoomOutTransition ImageView is empty!");
        return;
    }
    target_ = target;
    current_ = current;
    RootView::GetInstance()->Remove(target);
    RootView::GetInstance()->Insert(nullptr, target);
    middleWidth_ = Screen::GetInstance().GetWidth() / NUM2;
    middleHeight_ = Screen::GetInstance().GetHeight() / NUM2;
}

void ZoomOutTransition::TransitionAlg(uint32_t time)
{
    float zoom_scale = EasingEquation::LinearEaseNone(DURATION, 0, time, duration_) / static_cast<float>(DURATION);
    float zoom_in_scale = zoom_scale;
    current_->Scale(Vector2<float>{zoom_in_scale, zoom_in_scale}, Vector2<float>{middleWidth_, middleHeight_});
    float zoom_out_scale = SCALE_BASE + zoom_scale;
    target_->Scale(Vector2<float>{zoom_out_scale, zoom_out_scale}, Vector2<float>{middleWidth_, middleHeight_});
}
} // namespace OHOS

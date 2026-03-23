/*
 * Copyright (c) CompanyNameMagicTag 2024. All rights reserved.
 * Description: WaterfallTransition
 * Author:
 * Create: 2024-09
 */

#include "WaterfallTransition.h"
#include "animator/easing_equation.h"
#include "components/root_view.h"
#include "TransitionManager.h"
#include "wearable_log.h"
#include "TransitionUtils.h"

namespace OHOS {
REGIST_TRANSITION(TransitionType::TRANSITION_WATERFALL, WaterfallTransition, 300, false, true);
static constexpr int16_t RANGE = 1000;
static const float WATERFALL_SCALE_INIT = 1.0f;
static const float WATERFALL_SCALE_TRANSPARENT = 2.5f;
static const float TARGET_SCALE_END = 1.0f;

void WaterfallTransition::OnTransitionStart(UIViewGroup* current, UIImageView* target)
{
    current_ = dynamic_cast<UIWaterfallList*>(TransitionUtils::GetViewAndTransparentizePath(current, UI_WATERFALL_LIST));
    if (current_ == nullptr) {
        return;
    }
    target_ = target;
    int8_t index = current_->GetFocusedImgIndex();
    if (index <= 0) {
        focusIndex_ = 0;
    } else {
        focusIndex_ = static_cast<uint8_t>(index);
    }
    focusView_ = dynamic_cast<UIImageView*>(current_->GetViewByIndex(focusIndex_));
    if (focusView_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "WaterfallTransition no focus view\n");
        return;
    }
    opaStart_ = focusView_->GetOpaScale();
    scaleStart_ = current_->GetGlobalScale();
    Rect rect = focusView_->GetRect();
    float xScale = target_->GetWidth() * 1.0f / rect.GetWidth();
    float yScale = target_->GetHeight() * 1.0f / rect.GetHeight();
    scaleEnd_ = FloatLessEqual(xScale, yScale) ? xScale : yScale;

    targetXStart_ = rect.GetX();
    targetYStart_ = rect.GetY();
    targetScaleStart_ = 1.0f / scaleEnd_;

    uint32_t offsetTime = static_cast<uint32_t>((scaleStart_ - WATERFALL_SCALE_INIT) /
        (scaleEnd_ - WATERFALL_SCALE_INIT) * duration_);
    durationBackup_ = duration_;
    duration_ = duration_ - offsetTime;
    thresholdTime_ = static_cast<uint32_t>((WATERFALL_SCALE_TRANSPARENT - scaleStart_) / (scaleEnd_ - scaleStart_)
        * duration_);

    // put target image view in the background
    RootView::GetInstance()->Remove(target);
    RootView::GetInstance()->Insert(nullptr, target);
}

void WaterfallTransition::TransitionAlg(uint32_t time)
{
    if ((current_ == nullptr) || (target_== nullptr) || (focusView_ == nullptr)) {
        return;
    }
    float timeRatio = time * 1.0f / duration_;
    float opaRatio = time * 1.0f / thresholdTime_;
    opaRatio = FloatMore(opaRatio, 1.0f) ? 1.0f : opaRatio;
    float curScale = timeRatio * (scaleEnd_ - scaleStart_) + scaleStart_;
    uint8_t opa = opaRatio * (OPA_TRANSPARENT - opaStart_) + opaStart_;
    focusView_->SetOpaScale(opa);
    current_->Scale(curScale, focusIndex_);

    int16_t targetXPos = static_cast<int16_t>((1.0f - timeRatio) * targetXStart_);
    int16_t targetYPos = static_cast<int16_t>((1.0f - timeRatio) * targetYStart_);
    target_->SetPosition(targetXPos, targetYPos);
    float targetScale = targetScaleStart_ + timeRatio * (1.0f - targetScaleStart_);
    target_->Scale(Vector2<float>(targetScale, targetScale), Vector2<float>(0.0f, 0.0f));
    uint8_t targetOpa = timeRatio * (OPA_OPAQUE - OPA_TRANSPARENT) + OPA_TRANSPARENT;
    target_->SetOpaScale(targetOpa);
}

void WaterfallTransition::OnTransitionEnd()
{
    duration_ = durationBackup_;
}
} // namespace OHOS
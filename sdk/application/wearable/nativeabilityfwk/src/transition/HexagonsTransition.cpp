/*
 * Copyright (c) CompanyNameMagicTag 2024. All rights reserved.
 * Description: HexagonsTransition
 * Author:
 * Create: 2024-04
 */

#include "HexagonsTransition.h"
#include "animator/easing_equation.h"
#include "components/root_view.h"
#include "TransitionManager.h"
#include "wearable_log.h"
#include "TransitionUtils.h"

namespace OHOS {
REGIST_TRANSITION(TransitionType::TRANSITION_HEXAGONS, HexagonsTransition, 300, false, true);
static constexpr int16_t RANGE = 1000;
static const float HEXAGONS_SCALE_INIT = 1.0f;
static const float HEXAGONS_SCALE_TRANSPARENT = 2.5f;
static const float TARGET_SCALE_END = 1.0f;

void HexagonsTransition::OnTransitionStart(UIViewGroup* current, UIImageView* target)
{
    current_ = dynamic_cast<UICustomHexagonsList*>(TransitionUtils::GetViewAndTransparentizePath(current, UI_HEXAGONS_LIST));
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
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "HexagonsTransition no focus view\n");
        return;
    }
    Vector2<int16_t> delta = Vector2<int16_t>(current_->GetWidth() / 2, current_->GetHeight() / 2) -
        current_->GetCurrentPosition(focusIndex_);
    opaStart_ = focusView_->GetOpaScale();
    scaleStart_ = current_->GetGlobalScale();
    float xScale = target_->GetWidth() * 1.0f / focusView_->GetWidth();
    float yScale = target_->GetHeight() * 1.0f / focusView_->GetHeight();
    scaleEnd_ = FloatLessEqual(xScale, yScale) ? xScale : yScale;

    Rect rect = focusView_->GetRect();
    targetXStart_ = rect.GetX();
    targetYStart_ = rect.GetY();
    float targetXScale = rect.GetWidth() * 1.0f / target_->GetWidth();
    float targetYScale = rect.GetHeight() * 1.0f / target_->GetHeight();
    targetScaleStart_ = FloatMoreEqual(targetXScale, targetYScale) ? targetXScale : targetYScale;

    uint32_t offsetTime = static_cast<uint32_t>((scaleStart_ - HEXAGONS_SCALE_INIT) /
        (scaleEnd_ - HEXAGONS_SCALE_INIT) * duration_);
    durationBackup_ = duration_;
    duration_ = duration_ - offsetTime;
    thresholdTime_ = static_cast<uint32_t>((HEXAGONS_SCALE_TRANSPARENT - scaleStart_) / (scaleEnd_ - scaleStart_)
        * duration_);
    xDistance_ = delta.x_;
    yDistance_ = delta.y_;
    prevX_ = 0;
    prevY_ = 0;

    // put target image view in the background
    RootView::GetInstance()->Remove(target);
    RootView::GetInstance()->Insert(nullptr, target);
}

void HexagonsTransition::TransitionAlg(uint32_t time)
{
    if ((current_ == nullptr) || (target_== nullptr) || (focusView_ == nullptr)) {
        return;
    }
    float timeRatio = time * 1.0f / duration_;
    float opaRatio = time * 1.0f / thresholdTime_;
    opaRatio = FloatMore(opaRatio, 1.0f) ? 1.0f : opaRatio;
    int16_t curX = static_cast<int16_t>(timeRatio * xDistance_);
    int16_t curY = static_cast<int16_t>(timeRatio * yDistance_);
    current_->ScrollBy(curX - prevX_, curY - prevY_);
    prevX_ = curX;
    prevY_ = curY;

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

void HexagonsTransition::OnTransitionEnd()
{
    duration_ = durationBackup_;
}
} // namespace OHOS
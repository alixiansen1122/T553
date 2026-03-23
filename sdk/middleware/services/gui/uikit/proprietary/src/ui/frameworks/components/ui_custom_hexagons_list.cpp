/*
 * Copyright (c) @CompanyNameMagicTag. 2024. All rights reserved.
 */

#include "components/ui_custom_hexagons_list.h"
#include "animator/easing_equation.h"

namespace OHOS {
static const float TRANSPARENT_SCALE = 2.5f;
static const float ROTATE_SCALE_FACTOR = 0.02f;
static const float SCALE_SMALLEST_VALUE = 0.1f;
static const float ROTATE_ANIMATION_DURATION = 10;

UICustomHexagonsList::~UICustomHexagonsList()
{
    if (rotateAnimator_ != nullptr) {
        delete rotateAnimator_;
        rotateAnimator_ = nullptr;
    }
}

UICustomHexagonsList::RotateAnimatorCallback::RotateAnimatorCallback()
    : curTime_(0),
      totalTime_(0),
      xDistance_(0),
      yDistance_(0),
      prevX_(0),
      prevY_(0),
      startScale_(0.0f),
      endScale_(0.0f),
      centerIndex_(0)
{}

void UICustomHexagonsList::RotateAnimatorCallback::Callback(UIView* view)
{
    UICustomHexagonsList* hexView = dynamic_cast<UICustomHexagonsList*>(view);
    if (hexView == nullptr) {
        return;
    }
    curTime_++;
    if (curTime_ > totalTime_) {
        hexView->StopRotateAnimator();
        return;
    }
    float timeRatio = curTime_ * 1.0f / totalTime_;
    if (!FloatEqual(xDistance_, 0)) {
        if (!FloatEqual(prevX_, xDistance_)) {
            int16_t curX = static_cast<int16_t>(timeRatio * xDistance_);
            hexView->DragXInner(curX - prevX_);
            prevX_ = curX;
        }
    }
    if (!FloatEqual(yDistance_, 0)) {
        if (!FloatEqual(prevY_, yDistance_)) {
            int16_t curY = static_cast<int16_t>(timeRatio * yDistance_);
            hexView->DragYInner(curY - prevY_);
            prevY_ = curY;
        }
    }
    float curScale = timeRatio * (endScale_ - startScale_) + startScale_;
    if (FloatMoreEqual(curScale, 1.0f)) {
        uint8_t opa = static_cast<uint8_t>((1 - (curScale - 1.0f) / (TRANSPARENT_SCALE - 1.0f)) * OPA_OPAQUE);
        UIView* curview = hexView->GetViewByIndex(centerIndex_);
        if (curview != nullptr) {
            curview->SetOpaScale(opa);
        }
    }
    if ((FloatMore(startScale_, endScale_) && (curScale >= endScale_)) ||
        (FloatLess(startScale_, endScale_) || (curScale <= endScale_))) {
        hexView->Scale(curScale, centerIndex_);
    }
    if (FloatEqual(curScale, endScale_)) {
        hexView->StopRotateAnimator();
    }
}

void UICustomHexagonsList::StartRotateAnimator(uint8_t center, int16_t xDistance, int16_t yDistance,
    float startScale, float endScale)
{
    if (rotateAnimator_ == nullptr) {
        rotateAnimator_ = new Animator(&rotateCallback_, this, 0, true);
    }
    rotateCallback_.ResetCallback();
    rotateCallback_.SetCenterIndex(center);
    rotateCallback_.SetDuration(ROTATE_ANIMATION_DURATION);
    rotateCallback_.SetRotateRange(xDistance, yDistance, startScale, endScale);
    rotateAnimator_->Start();
}

void UICustomHexagonsList::StopRotateAnimator()
{
    if (rotateAnimator_ != nullptr) {
        rotateAnimator_->Stop();
        rotateCallback_.ResetCallback();
    }
}

#if ENABLE_ROTATE_INPUT
bool UICustomHexagonsList::OnRotateStartEvent(const RotateEvent& event)
{
    if (scrollAnimator_.GetState() != Animator::STOP) {
        UIAbstractScroll::StopAnimator();
    }
    if ((rotateAnimator_ != nullptr) && (rotateAnimator_->GetState() != Animator::STOP)) {
        StopRotateAnimator();
    }

    // Record rotate center index when scale is large than 1.0f
    if (FloatMoreEqual(GetGlobalScale(), 1.0f)) {
        int8_t focusIndex = GetFocusedImgIndex();
        if (focusIndex < 0) {
            GRAPHIC_LOGE("wrong focus index %d, force to be 0 !!", focusIndex);
            rotateCenterIndexInNormal_ = 0;
        } else {
            rotateCenterIndexInNormal_ = static_cast<uint8_t>(focusIndex);
        }
    }
    SetTouchable(false);
    SetDraggable(false);
    return UIView::OnRotateStartEvent(event);
}

bool UICustomHexagonsList::OnRotateEvent(const RotateEvent& event)
{
    float deltaScale = event.GetRotate() * ROTATE_SCALE_FACTOR * GetRotateFactor();
    float targetScale = globalScale_ + deltaScale;
    Vector2<int16_t> position = GetCurrentPosition(rotateCenterIndexInNormal_);
    Vector2<int16_t> deltaDistance;
    // Current scale > Normal(1.0f), scale and change opaque
    if (FloatMore(targetScale, 1.0f)) {
        UIView* view = GetViewByIndex(rotateCenterIndexInNormal_);
        uint8_t opa = static_cast<uint8_t>((1 - (targetScale - 1.0f) / (TRANSPARENT_SCALE - 1.0f)) * OPA_OPAQUE);
        if (view != nullptr) {
            view->SetOpaScale(opa);
        }
        Scale(targetScale, rotateCenterIndexInNormal_);
    // Current scale in [Equal-Threshold, Normal], only scale
    } else if (FloatMoreEqual(targetScale, scaleThresholdOfZeroFactor_)) {
        Scale(targetScale, rotateCenterIndexInNormal_);
    // Current scale in [Small, Equal-Threshold), scale and scroll
    } else if (FloatMore(targetScale, scaleInSmallState_)) {
        if (deltaScale < 0) {
            Vector2<int16_t> targetPos = GetPositionWithCondition(rotateCenterIndexInNormal_, 0, scaleInSmallState_);
            deltaDistance = targetPos - position;
            float deltaScaleToSmallState = scaleInSmallState_ - globalScale_;
            int16_t deltaX = static_cast<int16_t>(deltaDistance.x_ / deltaScaleToSmallState  * deltaScale);
            int16_t deltaY = static_cast<int16_t>(deltaDistance.y_ / deltaScaleToSmallState  * deltaScale);
            ScrollBy(deltaX, deltaY);
        } else {
            deltaDistance = Vector2<int16_t>(GetWidth() / 2, GetHeight() / 2) - position; // 2, center
            float deltaScaleToSmallState = scaleThresholdOfZeroFactor_ - globalScale_;
            int16_t deltaX = static_cast<int16_t>(deltaDistance.x_ / deltaScaleToSmallState  * deltaScale);
            int16_t deltaY = static_cast<int16_t>(deltaDistance.y_ / deltaScaleToSmallState  * deltaScale);
            ScrollBy(deltaX, deltaY);
        }
        Scale(targetScale, rotateCenterIndexInNormal_);
    // Current scale in [Smallest, Small), only scale
    } else if (FloatMore(targetScale, SCALE_SMALLEST_VALUE)) {
        Vector2<int16_t> centerImgPosition = GetCurrentPosition(0);
        Vector2<int16_t> targetPos(GetWidth() / 2, GetHeight() / 2); // 2, center
        deltaDistance = targetPos - centerImgPosition;
        ScrollBy(deltaDistance.x_, deltaDistance.y_);
        Scale(targetScale, 0);
    }
    return UIView::OnRotateEvent(event);
}

bool UICustomHexagonsList::OnRotateEndEvent(const RotateEvent& event)
{
    // Rotate end in (Normal, .), start animator to enter Normal State
    if (FloatMore(globalScale_, 1.0f)) {
        StartRotateAnimator(rotateCenterIndexInNormal_, 0, 0, globalScale_, 1.0f);
        SetReboundMode(ReboundMode::NEAREST_IMG_ON_SCR_CENTER);
    // Rotate end in (Equal-Threshold, Normal), start animator to enter Normal State
    } else if (FloatMore(globalScale_, scaleThresholdOfZeroFactor_)) {
        StartRotateAnimator(rotateCenterIndexInNormal_, 0, 0, globalScale_, 1.0f);
        SetReboundMode(ReboundMode::NEAREST_IMG_ON_SCR_CENTER);
    // Rotate end in (Small, Equal-Threshold), start animator to enter Small State
    } else if (FloatMore(globalScale_, scaleInSmallState_)) {
        Vector2<int16_t> targetPos = GetPositionWithCondition(rotateCenterIndexInNormal_, 0, scaleInSmallState_);
        Vector2<int16_t> curPos = GetCurrentPosition(rotateCenterIndexInNormal_);
        Vector2<int16_t> delta = targetPos - curPos;
        StartRotateAnimator(rotateCenterIndexInNormal_, delta.x_, delta.y_, globalScale_, scaleInSmallState_);
        SetReboundMode(ReboundMode::CENTER_IMG_ON_SCR_CENTER);
    // Rotate end in (., Small), start animator to enter Small State
    } else {
        StartRotateAnimator(0, 0, 0, globalScale_, scaleInSmallState_);
        SetReboundMode(ReboundMode::CENTER_IMG_ON_SCR_CENTER);
    }

    SetTouchable(true);
    SetDraggable(true);
    return UIView::OnRotateEndEvent(event);
}
#endif
}

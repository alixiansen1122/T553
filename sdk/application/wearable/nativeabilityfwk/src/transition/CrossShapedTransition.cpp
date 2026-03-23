/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: CrossShapedTransition
 * Create: 2025-04
 */

#include "applist/AppItemView.h"
#include "animator/easing_equation.h"
#include "common/screen.h"
#include "components/root_view.h"
#include "components/ui_view.h"
#include "TransitionManager.h"
#include "wearable_log.h"
#include "CrossShapedTransition.h"

namespace OHOS {
    static constexpr uint16_t DURATION = 200;
    static constexpr uint16_t NUM2 = 2;
    static constexpr uint16_t HALF_DURATION = DURATION / NUM2;
    static constexpr int16_t CAMERA_DISTANCE = 600;
    static constexpr int16_t EASE_RANGE = 100;
    static constexpr int16_t MIN_OFFSET = 10;

    REGIST_TRANSITION(TransitionType::TRANSITION_CROSS_SHAPED, CrossShapedTransition, DURATION, true, true);

    void CrossShapedTransition::OnTransitionStart(UIImageView *current, UIImageView *target)
    {
        if (current == nullptr || target == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "CrossShapedTransition ImageView is empty!");
            return;
        }

        currentView_ = current;
        targetView_ = target;

        currentView_->SetVisible(false);
        targetView_->SetVisible(false);

        if (cameraView_ == nullptr) {
            cameraView_ = new UIImageView();
        }

        screenWidth_ = Screen::GetInstance().GetWidth();
        screenHeight_ = Screen::GetInstance().GetHeight();

        middleWidth_ = screenWidth_ / NUM2;
        middleHeight_ = screenHeight_ / NUM2;

        cameraView_->SetPosition(0, 0, screenWidth_, screenHeight_);
        cameraView_->SetCameraPosition(Vector2<float>(middleWidth_, middleHeight_));
        cameraView_->SetCameraDistance(CAMERA_DISTANCE);
        RootView::GetInstance()->Add(cameraView_);
    }

    void CrossShapedTransition::TransitionAlg(uint32_t time)
    {
        int16_t xOffset = 0;
        if (time < HALF_DURATION) {
            xOffset = EasingEquation::QuadEaseIn(-MIN_OFFSET, -middleWidth_, time, HALF_DURATION);
        } else {
            xOffset = EasingEquation::QuadEaseIn(middleWidth_, MIN_OFFSET,
                                                 time - HALF_DURATION, DURATION - HALF_DURATION);
        }

        bool visible = false; // true: current; false: target
        UIView* tmpCard = GetVisibleView(xOffset, visible);
        CommonAlg(tmpCard, xOffset, visible);
    }

    UIView* CrossShapedTransition::GetVisibleView(int16_t xOffset, bool &visible)
    {
        UIView* view = nullptr;
        if (xOffset > 0) {
            if (xOffset < middleWidth_) {
                view = targetView_;
                visible = false;
                return view;
            }

            view = currentView_;
            visible = true;
        } else {
            if (abs(xOffset) < middleWidth_) {
                view = currentView_;
                visible = true;
                return view;
            }

            view = targetView_;
            visible = false;
        }

        return view;
    }

    void CrossShapedTransition::OnTransitionEnd()
    {
        if (cameraView_ != nullptr) {
            RootView::GetInstance()->Remove(cameraView_);
        }

        if (targetView_ != nullptr) {
            targetView_->SetVisible(true);
        }
    }

    void CrossShapedTransition::CommonAlg(UIView* visualView, int16_t xOffset, bool visible)
    {
        if (visualView == nullptr) {
            return;
        }

        float factor;
        if (abs(xOffset) < middleWidth_) {
            factor = EasingEquation::LinearEaseNone(0, EASE_RANGE, abs(xOffset), middleWidth_);
        } else {
            factor = EasingEquation::LinearEaseNone(0, EASE_RANGE, screenWidth_ - abs(xOffset), middleWidth_);
        }

        float scale = 1.0 - 0.2 * factor / EASE_RANGE; // 0.2: scale range
        float angle = 90 * factor / EASE_RANGE;        // 90: quarter cycle degrees
        if (visible) {
            angle = -angle;
        }

        Vector3<float> scaleCenter = {middleWidth_, middleHeight_, 0};
        Vector3<float> rotateStart = {middleWidth_, 0, 0};
        Vector3<float> rotateEnd = {middleWidth_, 2, 0};
        UIView* tmpCard = nullptr;

        cameraView_->SetSrc(static_cast<UIImageView *>(visualView)->GetImageInfo());
        cameraView_->Scale(Vector3<float>(scale, scale, 0), scaleCenter);
        cameraView_->Rotate(angle, rotateStart, rotateEnd);
    }
} // namespace OHOS
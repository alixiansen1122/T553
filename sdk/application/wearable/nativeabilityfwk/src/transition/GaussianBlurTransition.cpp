/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: GaussianBlurTransition
 * Create: 2025-04
 */

#include "animator/easing_equation.h"
#include "components/root_view.h"
#include "components/ui_view.h"
#include "TransitionManager.h"
#include "wearable_log.h"
#include "GaussianBlurTransition.h"

namespace OHOS {
    static constexpr int64_t VIEW_COLOR = 0xff505050;
    static constexpr uint8_t VIEW_OPACITY = 10;
    static constexpr uint64_t VIEW_BLUR_OPACITY = 150;
    static constexpr uint8_t BLUR_MIN = 5;
    static constexpr float BLUR_MAX = 31.0;
    static constexpr int16_t BREAK_TIME = 125;
    static constexpr int16_t DURITION = 200;
    static constexpr char* BLURVIEW_NAME = "blur";

    REGIST_TRANSITION(TransitionType::TRANSITION_GAUSSIAN_BLUR, GaussianBlurTransition, DURITION, false, false);

    void GaussianBlurTransition::AddBlurView(UIViewGroup* groupView)
    {
        if (groupView == nullptr) {
            return;
        }

        UIView* blurCurent = new UIView(BLURVIEW_NAME);
        if (blurCurent == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_AAFWK, "GaussianBlurTransition new view failed!");
            return;
        }

        Rect currRect = groupView->GetVisibleRect();
        blurCurent->SetPosition(currRect.GetX(), currRect.GetY(), currRect.GetWidth(), currRect.GetHeight());
        blurCurent->SetStyle(STYLE_BACKGROUND_COLOR, VIEW_COLOR);
        blurCurent->SetStyle(STYLE_BACKGROUND_OPA, VIEW_OPACITY);

        UIView* tailView = groupView->GetChildrenTail();
        if (tailView != nullptr) {
            groupView->Insert(tailView, blurCurent);
        }
    }

    void GaussianBlurTransition::SetViewBlur(UIViewGroup* groupView, float guassScale)
    {
        if (nullptr == groupView) {
            return;
        }

        UIView* tailView = groupView->GetChildrenTail();
        if (tailView == nullptr && tailView->GetViewId() != BLURVIEW_NAME) {
            return;
        }

        UIView* testView = RootView::GetInstance()->GetBlurView();
        RootView::GetInstance()->ClearBlurView(tailView);
        tailView->SetBackgroundBlur(guassScale);
        tailView->SetStyle(STYLE_BACKGROUND_OPA, VIEW_BLUR_OPACITY);
    }

    void GaussianBlurTransition::ClearBlurView(UIViewGroup* groupView)
    {
        if (groupView == nullptr) {
            return;
        }

        UIView* tailView = groupView->GetChildrenTail();
        if (tailView != nullptr && tailView->GetViewId() == BLURVIEW_NAME) {
            RootView::GetInstance()->ClearBlurView(tailView);
            groupView->Remove(tailView);
        }
    }

    void GaussianBlurTransition::OnTransitionStart(UIViewGroup* current, UIViewGroup* target)
    {
        if ((current == nullptr) || (target == nullptr)) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_AAFWK, "GaussianBlurTransition ImageView is empty!");
            return;
        }

        current_ = current;
        target_ = target;
        blurFlag_ = false;

        AddBlurView(current_);
        AddBlurView(target_);
        target_->SetVisible(false);
    }

    void GaussianBlurTransition::TransitionAlg(uint32_t time)
    {
        if (time <= BREAK_TIME) {
            float scale = EasingEquation::QuadEaseIn(BLUR_MIN, static_cast<int16_t>(BLUR_MAX), time,\
                                                     BREAK_TIME)/ BLUR_MAX;
            float guassScale = scale * BLUR_MAX;

            SetViewBlur(current_, guassScale);
        } else {
            if (!blurFlag_) {
                UIView* tailView = current_->GetChildrenTail();
                if (tailView != nullptr) {
                    tailView->ClearBackgroundBlur();
                }

                if (current_->IsVisible()) {
                    current_->SetVisible(false);
                }

                target_->SetVisible(true);
                SetViewBlur(target_, BLUR_MAX);
                blurFlag_ = true;
                return;
            }

            float scale = EasingEquation::QuadEaseIn(static_cast<int16_t>(BLUR_MAX), BLUR_MIN, time - BREAK_TIME,\
                                                     DURITION) / BLUR_MAX;
            float guassScale = scale * BLUR_MAX;
            SetViewBlur(target_, guassScale);
        }
    }

    void GaussianBlurTransition::OnTransitionEnd()
    {
        if (current_ == nullptr || target_ == nullptr) {
            return;
        }

        ClearBlurView(current_);
        ClearBlurView(target_);
    }
}
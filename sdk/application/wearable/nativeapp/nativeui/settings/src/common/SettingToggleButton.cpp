/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingToggleButton
 * Create: 2025-06-01
 */
#include "common/image.h"
#include "imgdecode/cache_manager.h"
#include "engines/gfx/gfx_engine_manager.h"
#include "settings/common/SettingToggleButton.h"

namespace OHOS {
constexpr uint8_t TOGGLE_BTN_UNSELECTED_OPA = 97;
constexpr uint8_t TOGGLE_HALF_NUM = 2;
#if DEFAULT_ANIMATION
constexpr int16_t DEFAULT_ANIMATOR_TIME = 150;
constexpr float BEZIER_CONTROL_POINT_X_1 = 0.2;
constexpr float BEZIER_CONTROL_POINT_X_2 = 0.2;
#endif

SettingToggleButton::SettingToggleButton()
{
    backgroundOpacity_ = TOGGLE_BTN_UNSELECTED_OPA;
    image_[UNSELECTED].SetSrc("");
    image_[SELECTED].SetSrc("");
#if DEFAULT_ANIMATION
    checkBoxAnimator_.SetTime(DEFAULT_ANIMATOR_TIME);
#endif
    Resize(width_, height_);
}

void SettingToggleButton::SetTrackPadding(int16_t padding)
{
    trackPadding_ = padding;
}

void SettingToggleButton::CalculateSize()
{
    width_ = GetWidth() - 1;   // UIView中保存的矩形宽高会比设置的宽高小1
    height_ = GetHeight() - 1;

    trackCorner_ = height_ / TOGGLE_HALF_NUM;
    thumbRadius_ = trackCorner_ - trackPadding_;

    Rect contentRect = GetContentRect();
    leftCenter_ = {static_cast<int16_t>(contentRect.GetX() + trackCorner_),
        static_cast<int16_t>(contentRect.GetY() + trackCorner_)};
    rightCenter_ = {static_cast<int16_t>(contentRect.GetX() + width_ - trackCorner_),
        static_cast<int16_t>(contentRect.GetY() + trackCorner_)};
#if DEFAULT_ANIMATION
    if (checkBoxAnimator_.GetState() != Animator::START) {
        currentCenter_ = (state_ == SELECTED) ? rightCenter_ : leftCenter_;
        backgroundOpacity_ = (state_ == SELECTED) ? OPA_OPAQUE : TOGGLE_BTN_UNSELECTED_OPA;
        bgColor_ = (state_ == SELECTED) ? selectedStateColor_ : Color::White();
    }
#else
    currentCenter_ = (state_ == SELECTED) ? rightCenter_ : leftCenter_;
    backgroundOpacity_ = (state_ == SELECTED) ? OPA_OPAQUE : TOGGLE_BTN_UNSELECTED_OPA;
    bgColor_ = (state_ == SELECTED) ? selectedStateColor_ : Color::White();
#endif
    rectMid_.SetRect(contentRect.GetX(), contentRect.GetY(), contentRect.GetX() + width_,
        contentRect.GetY() + (trackCorner_ << 1) + 1);
}

void SettingToggleButton::OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea)
{
    if ((image_[SELECTED].GetSrcType() != IMG_SRC_UNKNOWN) && (image_[UNSELECTED].GetSrcType() != IMG_SRC_UNKNOWN)) {
        UICheckBox::OnDraw(gfxDstBuffer, invalidatedArea);
    } else {
        CalculateSize();
        BaseGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, GetRect(), invalidatedArea, *style_, opaScale_);
        Rect contentRect = GetContentRect();
        Rect trunc = invalidatedArea;
        bool isIntersect = trunc.Intersect(trunc, contentRect);
        if (isIntersect == false) {
            return;
        }
        Style styleUnSelect = StyleDefault::GetBackgroundTransparentStyle();
        styleUnSelect.bgColor_ = bgColor_;
        styleUnSelect.bgOpa_ = backgroundOpacity_;
        styleUnSelect.borderRadius_ = trackCorner_;
        BaseGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, rectMid_, trunc, styleUnSelect, opaScale_);
        ArcInfo arcInfoLeft = {currentCenter_, {0}, thumbRadius_, 0, CIRCLE_IN_DEGREE, nullptr};
        styleUnSelect.lineColor_ = Color::White();
        styleUnSelect.lineWidth_ = thumbRadius_;
        BaseGfxEngine::GetInstance()->DrawArc(gfxDstBuffer, arcInfoLeft, trunc, styleUnSelect, OPA_OPAQUE,
            CapType::CAP_NONE);
    }
}

#if DEFAULT_ANIMATION
void SettingToggleButton::Callback(UIView* view)
{
    runTime_ = checkBoxAnimator_.GetRunTime();
    float x = static_cast<float>(runTime_) / checkBoxAnimator_.GetTime();
    float coefficient = Interpolation::GetBezierY(x, BEZIER_CONTROL_POINT_X_1, 0, BEZIER_CONTROL_POINT_X_2, 1);
    if (state_ == SELECTED) {
        currentCenter_.y = rightCenter_.y;
        currentCenter_.x = (rightCenter_.x - leftCenter_.x) * coefficient + leftCenter_.x;
        backgroundOpacity_ = TOGGLE_BTN_UNSELECTED_OPA + (OPA_OPAQUE - TOGGLE_BTN_UNSELECTED_OPA) * coefficient;
        bgColor_ = Color::GetMixColor(selectedStateColor_, Color::White(), OPA_OPAQUE * coefficient);
    } else {
        currentCenter_.y = leftCenter_.y;
        currentCenter_.x = rightCenter_.x - (rightCenter_.x - leftCenter_.x) * coefficient;
        backgroundOpacity_ = OPA_OPAQUE - (OPA_OPAQUE - TOGGLE_BTN_UNSELECTED_OPA) * coefficient;
        bgColor_ = Color::GetMixColor(selectedStateColor_, Color::White(), OPA_OPAQUE * (1 - coefficient));
    }
    Invalidate();
}

void SettingToggleButton::OnStop(UIView& view)
{
    if (state_ == SELECTED) {
        bgColor_ = selectedStateColor_;
    } else {
        bgColor_ = Color::White();
    }
    backgroundOpacity_ = (state_ == SELECTED) ? OPA_OPAQUE : TOGGLE_BTN_UNSELECTED_OPA;
    Invalidate();
}
#endif
} // namespace OHOS
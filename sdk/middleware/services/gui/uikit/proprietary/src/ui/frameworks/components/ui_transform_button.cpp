/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: UITransformButton
 * Author: Hisi Graphic Team
 * Created: 2025-11
 */

#include "ui_transform_button.h"
#include "animator/interpolation.h"
#include "common/image.h"
#include "draw/draw_image.h"
#include "engines/gfx/gfx_engine_manager.h"
#include "engines/gfx/lite_m_gfx_engine.h"
#include "gfx_utils/graphic_log.h"
#include "gfx_utils/style.h"
#include "imgdecode/cache_manager.h"
#include "themes/theme_manager.h"

namespace OHOS {
namespace {
constexpr float FULL_SCALE = 1.0f;
constexpr float SHRINK_SCALE = 0.8f;
constexpr uint32_t SHRINK_DURATION = 150;
constexpr uint32_t RECOVER_DURATION = 200;
constexpr int64_t MASK_OPA = 25;
constexpr float BEZIER_CONTROL = 0.2f;
} // namespace

UITransformButton::UITransformButton()
    : contentWidth_(0),
      contentHeight_(0),
      state_(TransformButtonState::RELEASED),
      styleState_(TransformButtonState::RELEASED),
      enableAnimation_(true),
      animator_(*this),
      buttonStyleAllocFlag_(false),
      shrinkScale_(SHRINK_SCALE),
      shrinkDuration_(SHRINK_DURATION),
      recoverDuration_(RECOVER_DURATION)
{
    touchable_ = true;
    SetupThemeStyles();
}

UITransformButton::~UITransformButton()
{
    if (buttonStyleAllocFlag_) {
        for (uint8_t i = 0; i < static_cast<uint8_t>(TransformButtonState::BTN_STATE_NUM); i++) {
            delete buttonStyles_[i];
            buttonStyles_[i] = nullptr;
        }
        buttonStyleAllocFlag_ = false;
    }
}

void UITransformButton::SetShrinkScale(float scale)
{
    shrinkScale_ = scale;
}

float UITransformButton::GetShrinkScale() const
{
    return shrinkScale_;
}

void UITransformButton::SetShrinkDuration(uint32_t dur)
{
    shrinkDuration_ = dur;
}

uint32_t UITransformButton::GetShrinkDuration() const
{
    return shrinkDuration_;
}

void UITransformButton::SetRecoverDuration(uint32_t dur)
{
    recoverDuration_ = dur;
}

uint32_t UITransformButton::GetRecoverDuration() const
{
    return recoverDuration_;
}

void UITransformButton::OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea)
{
    OpacityType opa = GetMixOpaScale();
    LiteMGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, GetOrigRect(), invalidatedArea,
        *buttonStyles_[static_cast<uint8_t>(state_)], opa, transMap_);
}

void UITransformButton::SetupThemeStyles()
{
    Theme* theme = ThemeManager::GetInstance().GetCurrent();

    if (theme == nullptr) {
        buttonStyles_[static_cast<uint8_t>(TransformButtonState::RELEASED)] = &(StyleDefault::GetButtonReleasedStyle());
        buttonStyles_[static_cast<uint8_t>(TransformButtonState::PRESSED)] = &(StyleDefault::GetButtonPressedStyle());
        buttonStyles_[static_cast<uint8_t>(TransformButtonState::INACTIVE)] = &(StyleDefault::GetButtonInactiveStyle());
    } else {
        buttonStyles_[static_cast<uint8_t>(TransformButtonState::RELEASED)] = &(theme->GetButtonStyle().released);
        buttonStyles_[static_cast<uint8_t>(TransformButtonState::PRESSED)] = &(theme->GetButtonStyle().pressed);
        buttonStyles_[static_cast<uint8_t>(TransformButtonState::INACTIVE)] = &(theme->GetButtonStyle().inactive);
    }
    style_ = buttonStyles_[static_cast<uint8_t>(TransformButtonState::RELEASED)];
}

int64_t UITransformButton::GetStyle(uint8_t key) const
{
    return GetStyleForState(key, styleState_);
}

void UITransformButton::SetStyle(uint8_t key, int64_t value)
{
    SetStyleForState(key, value, styleState_);
}

int64_t UITransformButton::GetStyleForState(uint8_t key, TransformButtonState state) const
{
    if (state < TransformButtonState::BTN_STATE_NUM) {
        return (buttonStyles_[static_cast<uint8_t>(state)])->GetStyle(key);
    }
    return 0;
}

void UITransformButton::SetStyleForState(uint8_t key, int64_t value, TransformButtonState state)
{
    if (state >= TransformButtonState::BTN_STATE_NUM) {
        return;
    }

    if (!buttonStyleAllocFlag_) {
        uint8_t i = 0;
        for (; i < static_cast<uint8_t>(TransformButtonState::BTN_STATE_NUM); i++) {
            Style styleSaved = *buttonStyles_[i];
            buttonStyles_[i] = new Style;
            if (buttonStyles_[i] == nullptr) {
                GRAPHIC_LOGE("new Style fail");
                break;
            }
            *(buttonStyles_[i]) = styleSaved;
        }
        if (i < static_cast<uint8_t>(TransformButtonState::BTN_STATE_NUM)) {
            for (uint8_t j = 0; j < i; ++j) {
                delete buttonStyles_[j];
                buttonStyles_[j] = nullptr;
            }
            SetupThemeStyles();
            return;
        }
        buttonStyleAllocFlag_ = true;
    }
    style_ = buttonStyles_[static_cast<uint8_t>(TransformButtonState::RELEASED)];
    int16_t width = GetWidth();
    int16_t height = GetHeight();
    int16_t x = GetX();
    int16_t y = GetY();
    buttonStyles_[static_cast<uint8_t>(state)]->SetStyle(key, value);
    Rect rect(x, y, x + width - 1, y + height -  1);
    UpdateRectInfo(key, rect);
}

bool UITransformButton::OnPressEvent(const PressEvent& event)
{
    SetState(TransformButtonState::PRESSED);
    Resize(contentWidth_, contentHeight_);
    Invalidate();
    if (enableAnimation_) {
        animator_.Start();
    }
    return UIView::OnPressEvent(event);
}

bool UITransformButton::OnReleaseEvent(const ReleaseEvent& event)
{
    SetState(TransformButtonState::RELEASED);
    Resize(contentWidth_, contentHeight_);
    Invalidate();
    if (enableAnimation_) {
        animator_.Start();
    }
    return UIView::OnReleaseEvent(event);
}

bool UITransformButton::OnCancelEvent(const CancelEvent& event)
{
    SetState(TransformButtonState::RELEASED);
    Resize(contentWidth_, contentHeight_);
    Invalidate();
    if (enableAnimation_) {
        animator_.Start();
    }
    return UIView::OnCancelEvent(event);
}

void UITransformButton::Disable()
{
    SetState(TransformButtonState::INACTIVE);
    touchable_ = false;
}

void UITransformButton::Enable()
{
    SetState(TransformButtonState::RELEASED);
    touchable_ = true;
}

void UITransformButton::SetState(TransformButtonState state)
{
    state_ = state;
    style_ = buttonStyles_[static_cast<uint8_t>(state_)];
    Invalidate();
}

bool UITransformButton::OnPreDraw(Rect& invalidatedArea) const
{
    Rect rect(GetRect());
    int16_t topRadius = buttonStyles_[static_cast<uint8_t>(styleState_)]->GetStyle(STYLE_TOP_BORDER_RADIUS);
    int16_t bottomRadius = buttonStyles_[static_cast<uint8_t>(styleState_)]->GetStyle(STYLE_BOTTOM_BORDER_RADIUS);
    if (topRadius == bottomRadius && topRadius == COORD_MAX) {
        return true;
    }

    if (topRadius != 0 || bottomRadius != 0) {
        rect = GetInnerRect(rect, topRadius, bottomRadius);
    }
    if (rect.IsContains(invalidatedArea)) {
        return true;
    }
    invalidatedArea.Intersect(invalidatedArea, rect);
    return false;
}

void UITransformButton::ButtonAnimator::Start()
{
    bool isReverse = (button_.state_ == UITransformButton::TransformButtonState::PRESSED);
    float targetScale = isReverse ? button_.shrinkScale_ : FULL_SCALE;
    if ((animator_.GetState() == Animator::STOP) && FloatEqual(targetScale, scale_)) {
        return;
    }

    if (isReverse) {
        animator_.SetTime(button_.shrinkDuration_);
    } else {
        animator_.SetTime(button_.recoverDuration_);
    }
    animator_.Start();
    /* reverse the animator direction */
    float x = isReverseAnimation_ ? (FULL_SCALE - scale_) : (scale_ - button_.shrinkScale_);
    float y = x / (FULL_SCALE - button_.shrinkScale_);
    x = Interpolation::GetBezierY(FULL_SCALE - y, 0, BEZIER_CONTROL, FULL_SCALE, BEZIER_CONTROL);
    animator_.SetRunTime(static_cast<uint32_t>(animator_.GetTime() * x));
    isReverseAnimation_ = isReverse;
}

static inline void ScaleButton(UITransformButton& button, float scale)
{
    Vector2<float> scaleValue_ = {scale, scale};
    Vector2<float> centrePoint(button.GetWidth() / 2.0f, button.GetHeight() / 2.0f);
    button.Scale(scaleValue_, centrePoint);
}

void UITransformButton::ButtonAnimator::Callback(UIView* view)
{
    float x = static_cast<float>(animator_.GetRunTime()) / animator_.GetTime();
    float offset = Interpolation::GetBezierY(x, BEZIER_CONTROL, 0, BEZIER_CONTROL, FULL_SCALE);
    float scale = (FULL_SCALE - button_.shrinkScale_) * offset;

    scale_ = isReverseAnimation_ ? (FULL_SCALE - scale) : (scale + button_.shrinkScale_);
    ScaleButton(button_, scale_);
}

void UITransformButton::ButtonAnimator::OnStop(UIView& view)
{
    if (isReverseAnimation_) {
        scale_ = button_.shrinkScale_;
        ScaleButton(button_, button_.shrinkScale_);
    } else {
        scale_ = FULL_SCALE;
        button_.ResetTransParameter();
    }
}
} // namespace OHOS

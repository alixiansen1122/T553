/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: UITransformButton
 * Author: Hisi Graphic Team
 * Created: 2025-11
 */

#ifndef UI_TRANSFORM_BUTTON_H
#define UI_TRANSFORM_BUTTON_H
#include "animator/animator.h"
#include "common/text.h"
#include "components/ui_transform_group.h"
#include "gfx_utils/graphic_log.h"

namespace OHOS {
class UITransformButton : public UITransformGroup {
public:
    UITransformButton();
    ~UITransformButton() override;
    /**
     * @brief Obtains the component type.
     *
     * @return Returns the component type, as defined in {@link UIViewType}.
     */
    UIViewType GetViewType() const override
    {
        return UI_TRANSFORM_BUTTON;
    }

    /**
     * Sets the scale factor for the shrink effect.
     * @param scale The scaling factor to apply during shrink.
     */
    void SetShrinkScale(float scale);

    /**
     * Returns the current shrink scale factor.
     * @return The current shrink scale factor.
     */
    float GetShrinkScale() const;

    /**
     * Sets the duration of the shrink effect.
     * @param dur The duration of the shrink effect in milliseconds.
     */
    void SetShrinkDuration(uint32_t dur);

    /**
     * Returns the current shrink duration.
     * @return The current shrink duration in milliseconds.
     */
    uint32_t GetShrinkDuration() const;

    /**
     * Sets the duration of the recover effect.
     * @param dur The duration of the recover effect in milliseconds.
     */
    void SetRecoverDuration(uint32_t dur);

    /**
     * Returns the current recover duration.
     * @return The current recover duration in milliseconds.
     */
    uint32_t GetRecoverDuration() const;

    /**
     * @fn virtual bool UITransformButton::OnPreDraw(Rect& invalidatedArea) override
     *
     * @brief Do something before draw, this function will be invoked mainly to check if this view need
     *        to cover invalidate area so render manager can decide which layer to draw firstly.
     * @param [in] invalidate area.
     * @returns True if need cover.
     */
    bool OnPreDraw(Rect& invalidatedArea) const override;

    /**
     * @fn  virtual void UITransformButton::OnDraw(const Rect& invalidatedArea) override;
     *
     * @brief   Executes the draw action
     *
     * @param   [in] invalidatedArea The rectangle to draw, with coordinates relative to this drawable..
     */
    void OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea) override;

    /**
     * @fn  virtual void UITransformButton::OnPressEvent(const PressEvent& event) override;
     *
     * @brief   Executes the press event action
     *
     * @param   [in] event   The press event, contain press position.
     *
     * @return Returns <b>true</b> if the event is consumed; returns <b>false</b> otherwise.
     */
    bool OnPressEvent(const PressEvent& event) override;

    /**
     * @fn  virtual void UITransformButton::OnReleaseEvent(const ReleaseEvent& event) override;
     *
     * @brief   Executes the press release event action
     *
     * @param   [in] event   The press release event.
     */
    bool OnReleaseEvent(const ReleaseEvent& event) override;

    /**
     * @fn  virtual void UITransformButton::OnCancelEvent(const CancelEvent& event) override;
     *
     * @brief   Executes the cancel event action
     *
     * @param   [in] event   The cancel event.
     */
    bool OnCancelEvent(const CancelEvent& event) override;

    /**
     * @brief Enumerates the states of this button.
     *
     * The button has three states.
     */
    enum class TransformButtonState : uint8_t {
        /* An enum constant representing the Button state released option */
        RELEASED = 0,
        /* An enum constant representing the Button state pressed option */
        PRESSED,
        /* An enum constant representing the Button state inactive option */
        INACTIVE,
        /* An enum constant representing the Button state Number option */
        BTN_STATE_NUM,
    };

    /**
     * @brief Obtains the width of this button.
     *
     * @return Returns the button width.
     */
    int16_t GetWidth() override
    {
        Style* style = buttonStyles_[static_cast<uint8_t>(state_)];
        return GetRelativeRect().GetWidth() - (style->paddingLeft_ + style->paddingRight_) -
               (style->borderWidth_ * 2); /* 2: left and right border */
    }

    /**
     * @brief Obtains the height of this button.
     *
     * @return Returns the button height.
     */
    int16_t GetHeight() override
    {
        Style* style = buttonStyles_[static_cast<uint8_t>(state_)];
        return GetRelativeRect().GetHeight() - (style->paddingTop_ + style->paddingBottom_) -
               (style->borderWidth_ * 2); /* 2: top and bottom border */
    }

    /**
     * @brief Sets the width for this button.
     *
     * @param width Indicates the width to set.
     */
    void SetWidth(int16_t width) override
    {
        contentWidth_ = width;
        UIView::SetWidth(width); /* 2: left and right border */
    }

    /**
     * @brief Sets the height for this button.
     *
     * @param height Indicates the height to set.
     */
    void SetHeight(int16_t height) override
    {
        contentHeight_ = height;
        UIView::SetHeight(height); /* 2: top and bottom border */
    }

    /**
     * @brief Obtains a rectangular area that contains coordinate information.
     *
     * @return Returns the rectangle area.
     */
    virtual Rect GetContentRect() override
    {
        Rect contentRect;
        Style* style = buttonStyles_[static_cast<uint8_t>(state_)];
        contentRect.SetX(GetOrigRect().GetX() + style->paddingLeft_ + style->borderWidth_);
        contentRect.SetY(GetOrigRect().GetY() + style->paddingTop_ + style->borderWidth_);
        contentRect.SetWidth(GetWidth());
        contentRect.SetHeight(GetHeight());
        return contentRect;
    }

    /**
     * @brief Obtains the value of a style.
     *
     * @param key Indicates the key of the style.
     * @return Returns the value of the style.
     */
    int64_t GetStyle(uint8_t key) const override;

    /**
     * @brief Sets a style.
     *
     * @param key Indicates the key of the style to set.
     * @param value Indicates the value matching the key.
     */
    void SetStyle(uint8_t key, int64_t value) override;

    /**
     * @brief Obtains the style of a button in a specific state.
     *
     * @param key Indicates the key of the style.
     * @param state Indicates the button state, as enumerated in {@link TransformButtonState}.
     * @return Returns the style of the button in the specific state.
     */
    int64_t GetStyleForState(uint8_t key, TransformButtonState state) const;

    /**
     * @brief Sets the style for a button in a specific state.
     *
     * @param key Indicates the key of the style to set.
     * @param value Indicates the value matching the key.
     * @param state Indicates the button state, as enumerated in {@link TransformButtonState}.
     */
    void SetStyleForState(uint8_t key, int64_t value, TransformButtonState state);

    /**
     * @brief Disables this button.
     */
    void Disable();

    /**
     * @brief Enables this button.
     */
    void Enable();

    /**
     * @brief Sets the state for a button. After the setting, calling {@link SetStyle}
     *        will change the style of this button, but not its state.
     *
     * @param state Indicates the button state, as enumerated in {@link TransformButtonState}.
     */
    void SetStateForStyle(TransformButtonState state)
    {
        styleState_ = state;
    }

    /**
    * Enables or disables the button animation feature.
    * @param enable True to enable the animation, false to disable it.
    */
    void EnableButtonAnimation(bool enable)
    {
        enableAnimation_ = enable;
    }

protected:
    int16_t contentWidth_;
    int16_t contentHeight_;
    void SetState(TransformButtonState state);

    TransformButtonState state_;
    TransformButtonState styleState_;
    Style* buttonStyles_[static_cast<uint8_t>(TransformButtonState::BTN_STATE_NUM)];

    bool enableAnimation_;
    friend class ButtonAnimator;
    class ButtonAnimator final : public AnimatorCallback {
    public:
        ButtonAnimator() = delete;
        ButtonAnimator(const ButtonAnimator&) = delete;
        ButtonAnimator& operator=(const ButtonAnimator&) = delete;
        ButtonAnimator(ButtonAnimator&&) = delete;
        ButtonAnimator& operator=(ButtonAnimator&&) = delete;
        explicit ButtonAnimator(UITransformButton& button) : animator_(this, nullptr, 0, false), button_(button) {}
        ~ButtonAnimator() override {}

        void Start();
        void Callback(UIView* view) override;
        void OnStop(UIView& view) override;

    private:
        Animator animator_;
        bool isReverseAnimation_ = false;
        float scale_ = 1.0f;
        UITransformButton& button_;
    } animator_;

    bool buttonStyleAllocFlag_;

private:
    /** Sets up the theme styles */
    void SetupThemeStyles();
    float shrinkScale_;
    uint32_t shrinkDuration_;
    uint32_t recoverDuration_;
}; // class UITransformButton
} // namespace OHOS
#endif // UI_TRANSFORM_BUTTON_H

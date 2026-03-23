/*
 * Copyright (c) @CompanyNameMagicTag. 2024. All rights reserved.
 */

/**
 * @addtogroup UI_Components
 * @{
 *
 * @brief Defines UI components such as buttons, texts, images, lists, and progress bars.
 */

/**
 * @file ui_kaleidoscope_view.h
 *
 * @brief Declares a UIKaleidoscopeView.
 */

#ifndef UI_KALEIDOSCOPE_VIEW_H
#define UI_KALEIDOSCOPE_VIEW_H

#include "components/ui_view.h"
#include "gfx_utils/image_info.h"
#include "components/kaleidoscope_utils.h"
#include "hals/display_dev.h"

namespace OHOS {
/**
 * @brief It provide kaleidoscope view.
 */
class UIKaleidoscopeView : public UIView {
public:
    UIKaleidoscopeView(const UIKaleidoscopeView &) = delete;
    UIKaleidoscopeView &operator=(const UIKaleidoscopeView &) = delete;

    /**
     * @brief A constructor used to create the <b>UIKaleidoscopeView</b> instance.
              This view will directly modify the buffer, and it is the same size as the screen.
     */
    UIKaleidoscopeView();

    /**
     * @brief A destructor used to delete the <b>UIKaleidoscopeView</b> instance.
     */
    ~UIKaleidoscopeView() override;

    /**
     * @brief Obtains the view type.
     * @return Returns <b>UI_KALEIDOSCOPE_VIEW</b>, as defined in {@link UIViewType}.
     */
    UIViewType GetViewType() const override
    {
        return UI_KALEIDOSCOPE_VIEW;
    }

    /**
    * @brief  Set src img for kaleidoscope.
    * @param  [in]  src    Pointer to a ImageInfo struct, which is the source img for kaleidoscope img.
                           The size of img must match the size of screen.
    * @return Return true if set successfully; and false otherwise.
    */
    bool SetSrc(const ImageInfo* src);

    /**
    * @brief  Update the kaleidoscope by a rotate value.
    * @param  [in]  rotate    Rotate value.
    */
    void Rotate(float rotate);

#if ENABLE_ROTATE_INPUT
    /**
    * @brief  Set rotate factor for rotate event.
    */
    void SetRotateEventFactor(float factor);

    /**
    * @brief  Handle Rotate Event. Update the kaleidoscope by a rotate event.
    * @param  [in]  event    Rotate event.
    */
    bool OnRotateEvent(const RotateEvent& event) override;
#endif

    /**
    * @brief  Set rotate value for rotate animator callback.
    * @param  [in]  rotate    Rotate value.
    */
    void SetRotateAnimatorValue(float rotate);

    /**
    * @brief  Start rotate animator
    */
    void StartRotateAnimator();

    /**
    * @brief  Stop rotate animator
    */
    void StopRotateAnimator();

    void OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea) override;

    void DumpImgInfo() override;

private:
    class RotateCallback : public AnimatorCallback {
    public:
        RotateCallback() {}
        ~RotateCallback() override {}
        void SetRotateValue(float rotate);
        void Callback(UIView* view) override;
    private:
        float rotate_ = 0.0f;
    };

    void BlitKaleidoscopeImg(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea);

#if ENABLE_ROTATE_INPUT
    float factor_ = 1.0f;
#endif
    float rotate_ = 0.0f;
    KaleidoscopeUtils kaleidoscope_;
    RotateCallback callback_;
    Animator* animator_ = nullptr;
    bool directFb_ = false;
};
}
#endif
/**
 * @}
 */

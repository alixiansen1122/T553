/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: Lottie
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */

#ifndef UI_LOTT_VIEW_H
#define UI_LOTT_VIEW_H

#include <string>
#include "lottie/lott_comp.h"
#include "animator/animator.h"
#include "gfx_utils/vector.h"
#include "gfx_utils/rect.h"
#include "gfx_utils/graphic_buffer.h"

namespace OHOS {

class UILottView : public UIView {
public:
    /**
     * @brief  Constructor of UILottView.
     */
    UILottView();

    /**
     * @brief  Destructor of UILottView.
     */
    virtual ~UILottView();

    /**
     * @brief Set lottie source file.
     * @param file lottie source file.
     * @return Returns <b>true</b> if set file successfully; returns <b> false</b> otherwise.
     */
    bool SetSrc(std::string file);

    /**
     * @brief  Start lottie animation.
     */
    void Start();

    /**
     * @brief  Pause lottie animation.
     */
    void Pause();

    /**
     * @brief  Resume lottie animation.
     */
    void Resume();

    /**
     * @brief  Stop lottie animation.
     */
    void Stop();

    /**
     * @brief Set whether lottie animation is played repeatedly.
     */
    void SetRepeat(bool repeat);

    /**
     * @brief Get whether lottie animation is played repeatedly.
     */
    bool IsRepeat() const;

    /**
     * @brief  Get lottie animation state.
     */
    uint8_t GetState() const;

    void OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea) override;

    UIViewType GetViewType() const override
    {
        return UI_LOTT_VIEW;
    }

private:

    class LottAnimCb : public AnimatorCallback {
    public:
        LottAnimCb() {}
        virtual ~LottAnimCb() {}
        void Callback(UIView* view) override;
        void OnStop(UIView& view) override;
        void SetEndValue(float value)
        {
            endValue_ = value;
        }

        void BindAnimator(Animator* anim)
        {
            anim_ = anim;
        }

        void Reset()
        {
            endValue_ = 0;
            anim_ = nullptr;
        }

    private:
        float endValue_ = 0;
        Animator* anim_ = nullptr;
    };

    friend class LottAnimCb;

    bool repeat_ = false;
    LottAnimCb lottAnimCb_;
    Animator animator_;
    LottComp* lottComp_ = nullptr;
    std::string filePath_;
};
}
#endif

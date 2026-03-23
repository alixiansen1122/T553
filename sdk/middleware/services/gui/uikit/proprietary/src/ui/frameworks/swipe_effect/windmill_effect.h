/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: WindmillEffect
 * Create: 2025-04
 */

#ifndef WINDMILL_EFFECT_H
#define WINDMILL_EFFECT_H
#include "components/ui_cross_view.h"

namespace OHOS {
class WindmillEffect : public CardSwipe {
public:
    WindmillEffect(void)
    {
        isNeedClip_ = true;
        width_ = Screen::GetInstance().GetWidth();
        height_ = Screen::GetInstance().GetHeight();
    }
    ~WindmillEffect() override {}
    void CardSwipeAlg(UICardPage* leftCard, UICardPage* rightCard, int16_t xOffset) override;
    void CardSwipeAlg(UIImageView* leftCard, UIImageView* rightCard, int16_t xOffset) override;
    void StopEffect() override;

private:
    int16_t width_;
    int16_t height_;
    UIView* preLeftCard_ = nullptr;
    UIView* preRightCard_ = nullptr;

    void CommonAlg(UIView* leftCard, UIView* rightCard, int16_t xOffset);
};

}
#endif

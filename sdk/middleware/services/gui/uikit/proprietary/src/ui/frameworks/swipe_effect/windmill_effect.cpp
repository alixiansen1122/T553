/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: WindmillEffect
 * Create: 2025-04
 */

#include "common/screen.h"
#include "engines/gfx/lite_m_gfx_engine.h"
#include "windmill_effect.h"

namespace OHOS {
static constexpr int16_t EASE_RANGE = 30;
static constexpr int16_t NUM_2 = 2;

void WindmillEffect::StopEffect()
{
    if (preLeftCard_ != nullptr) {
        preLeftCard_->ResetTransParameter();
        preLeftCard_ = nullptr;
    }

    if (preRightCard_ != nullptr) {
        preRightCard_->ResetTransParameter();
        preRightCard_ = nullptr;
    }

    CardSwipe::StopEffect();
}

void WindmillEffect::CommonAlg(UIView* leftCard, UIView* rightCard, int16_t xOffset)
{
    if (xOffset == 0) {
        leftCard->ResetTransParameter();
        return;
    }

    if (preLeftCard_ != leftCard && preRightCard_ != rightCard) {
        if (preLeftCard_ != nullptr) {
            preLeftCard_->ResetTransParameter();
        }
        if (preRightCard_ != nullptr) {
            preRightCard_->ResetTransParameter();
        }
        preLeftCard_ = leftCard;
        preRightCard_ = rightCard;
    }

    Vector2<float> scaleCenter;
    scaleCenter.x_ = width_ / NUM_2;
    scaleCenter.y_ = height_;

    float factor = 0;
    factor = EasingEquation::LinearEaseNone(0, EASE_RANGE, abs(xOffset), width_);
    if (xOffset > 0) {
        leftCard->Rotate(-EASE_RANGE + factor, scaleCenter);
        rightCard->Rotate(factor, scaleCenter);
    } else {
        leftCard->Rotate(-factor, scaleCenter);
        rightCard->Rotate(EASE_RANGE - factor, scaleCenter);
    }
    return;
}

void WindmillEffect::CardSwipeAlg(UICardPage* leftCard, UICardPage* rightCard, int16_t xOffset)
{
    CardSwipe::CardSwipeAlg(leftCard, rightCard, xOffset);
    CommonAlg(leftCard, rightCard, xOffset);
}

void WindmillEffect::CardSwipeAlg(UIImageView* leftCard, UIImageView* rightCard, int16_t xOffset)
{
    CardSwipe::CardSwipeAlg(leftCard, rightCard, xOffset);
    CommonAlg(leftCard, rightCard, xOffset);
}
}

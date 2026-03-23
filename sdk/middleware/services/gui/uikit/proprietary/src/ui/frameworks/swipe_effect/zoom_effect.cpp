/*
 * Copyright (c) CompanyNameMagicTag 2023. All rights reserved.
 * Description: UiCrossView
 * Author:
 * Create: 2023-10
 */

#include "securec.h"
#include "common/screen.h"
#include "zoom_effect.h"

namespace OHOS {
void ZoomEffect::StopEffect()
{
    if (preLeftCard != nullptr) {
        preLeftCard->ResetTransParameter();
        preLeftCard = nullptr;
    }

    if (preRightCard != nullptr) {
        preRightCard->ResetTransParameter();
        preRightCard = nullptr;
    }

    CardSwipe::StopEffect();
}

void ZoomEffect::CommonAlg(UIView* leftCard, UIView* rightCard, int16_t xOffset)
{
    if (xOffset == 0) {
        leftCard->ResetTransParameter();
        return;
    }

    if (preLeftCard != leftCard && preRightCard != rightCard) {
        if (preLeftCard != nullptr) {
            preLeftCard->ResetTransParameter();
        }
        if (preRightCard != nullptr) {
            preRightCard->ResetTransParameter();
        }
        preLeftCard = leftCard;
        preRightCard = rightCard;
    }

    float width = Screen::GetInstance().GetWidth();
    float scale = 0.5 * xOffset / width; // 0.5: base scale value
    Vector2<float> leftScaleCenter;
    Vector2<float> rightScaleCenter;

    leftScaleCenter.x_ = leftCard->GetX() + leftCard->GetWidth();
    leftScaleCenter.y_ = leftCard->GetY() + leftCard->GetHeight() / 2; // 2: middle
    rightScaleCenter.x_ = rightCard->GetX();
    rightScaleCenter.y_ = rightCard->GetY() + rightCard->GetHeight() / 2; // 2: middle

    if (xOffset > 0) {
        float scaleIn = 0.5 + scale; // 0.5: base scale value
        float scaleOut = 1 - scale;
        leftCard->Scale(Vector2<float>(scaleIn, scaleIn), leftScaleCenter);
        rightCard->Scale(Vector2<float>(scaleOut, scaleOut), rightScaleCenter);
    } else {
        float scaleIn = 0.5 - scale; // 0.5: base scale value (scale < 0)
        float scaleOut = 1 + scale;
        leftCard->Scale(Vector2<float>(scaleOut, scaleOut), leftScaleCenter);
        rightCard->Scale(Vector2<float>(scaleIn, scaleIn), rightScaleCenter);
    }
    return;
}

void ZoomEffect::CardSwipeAlg(UICardPage* leftCard, UICardPage* rightCard, int16_t xOffset)
{
    CardSwipe::CardSwipeAlg(leftCard, rightCard, xOffset);
    CommonAlg(leftCard, rightCard, xOffset);
}

void ZoomEffect::CardSwipeAlg(UIImageView* leftCard, UIImageView* rightCard, int16_t xOffset)
{
    CardSwipe::CardSwipeAlg(leftCard, rightCard, xOffset);
    CommonAlg(leftCard, rightCard, xOffset);
}
}

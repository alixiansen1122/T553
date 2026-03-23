/*
 * Copyright (c) CompanyNameMagicTag 2023. All rights reserved.
 * Description: UiCrossView
 * Author:
 * Create: 2023-10
 */

#include "common/screen.h"
#include "engines/gfx/lite_m_gfx_engine.h"
#include "card_flip_effect.h"

namespace OHOS {
static constexpr int16_t CAMERA_DISTANCE = 600;
static constexpr int16_t EASE_RANGE = 100;

void CardFlipEffect::StopEffect()
{
    if (preLeftCard != nullptr) {
        preLeftCard->SetVisible(true);
        preRightCard->SetVisible(true);
        preLeftCard = nullptr;
        preRightCard = nullptr;
        visibleCard_ = nullptr;
        container_->UIViewGroup::Remove(&img_);
    }

    CardSwipe::StopEffect();
}

void CardFlipEffect::CommonAlg(UIView* leftCard, UIView* rightCard, int16_t xOffset)
{
    int16_t width = Screen::GetInstance().GetWidth();
    int16_t height = Screen::GetInstance().GetHeight();
    int16_t middleWidth = width / 2;
    int16_t middleHeight = height / 2;

    if (preLeftCard == nullptr) {
        img_.SetPosition(0, 0, width, height);
        img_.SetCameraPosition(Vector2<float>(middleWidth, middleHeight));
        img_.SetCameraDistance(CAMERA_DISTANCE);
        container_->UIViewGroup::Add(&img_);
    }

    if (preLeftCard != leftCard && preRightCard != rightCard) {
        if (preLeftCard != nullptr) {
            preLeftCard->SetVisible(true);
        }
        if (preRightCard != nullptr) {
            preRightCard->SetVisible(true);
        }
        leftCard->SetVisible(false);
        rightCard->SetVisible(false);
        preLeftCard = leftCard;
        preRightCard = rightCard;
    }

    float factor;
    if (abs(xOffset) < middleWidth) {
        factor = EasingEquation::LinearEaseNone(0, EASE_RANGE, abs(xOffset), middleWidth);
    } else {
        factor = EasingEquation::LinearEaseNone(0, EASE_RANGE, width - abs(xOffset), middleWidth);
    }

    float scale = 1.0 - 0.2 * factor / EASE_RANGE; // 0.2: scale range
    float angle = 90 * factor / EASE_RANGE;        // 90: quarter cycle degrees
    Vector3<float> scaleCenter = {middleWidth, middleHeight, 0};
    Vector3<float> rotateStart = {middleWidth, 0, 0};
    Vector3<float> rotateEnd = {middleWidth, 1, 0};
    UIView* tmpCard = nullptr;

    if (xOffset > 0) {
        if (xOffset < middleWidth) {
            tmpCard = rightCard;
        } else {
            angle = -angle;
            tmpCard = leftCard;
        }
    } else {
        if (abs(xOffset) < middleWidth) {
            angle = -angle;
            tmpCard = leftCard;
        } else {
            tmpCard = rightCard;
        }
    }

    if (visibleCard_ != tmpCard) {
        visibleCard_ = tmpCard;
        if (visibleCard_->GetViewType() == UI_IMAGE_VIEW) {
            img_.SetSrc(static_cast<UIImageView *>(visibleCard_)->GetImageInfo());
        } else {
            ImageInfo info;
            if (img_.GetImageInfo() != nullptr) {
                info = *(img_.GetImageInfo());
                ImageCacheFree(info);
            }
            visibleCard_->SetVisible(true);
            visibleCard_->GetBitmap(info);
            img_.SetSrc(&info);
            visibleCard_->SetVisible(false);
        }
    }

    img_.Scale(Vector3<float>(scale, scale, 0), scaleCenter);
    img_.Rotate(angle, rotateStart, rotateEnd);
    return;
}

void CardFlipEffect::CardSwipeAlg(UICardPage* leftCard, UICardPage* rightCard, int16_t xOffset)
{
    CardSwipe::CardSwipeAlg(leftCard, rightCard, xOffset);
    CommonAlg(leftCard, rightCard, xOffset);
}

void CardFlipEffect::CardSwipeAlg(UIImageView* leftCard, UIImageView* rightCard, int16_t xOffset)
{
    CardSwipe::CardSwipeAlg(leftCard, rightCard, xOffset);
    CommonAlg(leftCard, rightCard, xOffset);
}
}

/*
 * Copyright (c) CompanyNameMagicTag 2023. All rights reserved.
 * Description: UiCrossView
 * Author:
 * Create: 2023-10
 */
#include "turn_page_effect.h"
#include "imgdecode/image_load.h"

namespace OHOS {
void TurnPageEffect::StopEffect()
{
    if (preLeftCard != nullptr) {
        RemoveFromContainer();

        preLeftCard->SetVisible(true);
        preRightCard->SetVisible(true);
        preLeftCard = nullptr;
        preRightCard = nullptr;

        FreeInfo();
    }

    CardSwipe::StopEffect();
}

void TurnPageEffect::CommonAlg(UIView* leftCard, UIView* rightCard, int16_t xOffset)
{
    if (preLeftCard != leftCard && preRightCard != rightCard) {
        if (preLeftCard != nullptr) {
            preLeftCard->SetVisible(true);
            preRightCard->SetVisible(true);
            FreeInfo();
        }
        if (leftCard->GetViewType() == UI_IMAGE_VIEW) {
            lInfo_ = *(static_cast<UIImageView *>(leftCard)->GetImageInfo());
            rInfo_ = *(static_cast<UIImageView *>(rightCard)->GetImageInfo());
        } else {
            FreeInfo();
            if (!AllocInfo(leftCard, rightCard)) {
                GRAPHIC_LOGE("get bitmap failed!");
                return;
            }
            lInfo_ = *leftInfo_;
            rInfo_ = *rightInfo_;
        }

        leftCard->SetVisible(false);
        rightCard->SetVisible(false);
        preLeftCard = leftCard;
        preRightCard = rightCard;
    }
    AddToContainer();
    turnPageView_.SetTurnPageInfo(&lInfo_, &rInfo_, ComputeAngle(xOffset));
}

void TurnPageEffect::CardSwipeAlg(UICardPage* leftCard, UICardPage* rightCard, int16_t xOffset)
{
    CardSwipe::CardSwipeAlg(leftCard, rightCard, xOffset);
    CommonAlg(leftCard, rightCard, xOffset);
}

void TurnPageEffect::CardSwipeAlg(UIImageView* leftCard, UIImageView* rightCard, int16_t xOffset)
{
    CardSwipe::CardSwipeAlg(leftCard, rightCard, xOffset);
    CommonAlg(leftCard, rightCard, xOffset);
}

void TurnPageEffect::FreeInfo()
{
    if (leftInfo_ != nullptr) {
        ImageCacheFree(*leftInfo_);
        delete leftInfo_;
        leftInfo_ = nullptr;
    }
    if (rightInfo_ != nullptr) {
        ImageCacheFree(*rightInfo_);
        delete rightInfo_;
        rightInfo_ = nullptr;
    }
}

bool TurnPageEffect::AllocInfo(UIView* leftCard, UIView* rightCard)
{
    if (leftCard == nullptr || rightCard == nullptr) {
        return false;
    }
    leftInfo_ = new ImageInfo;
    rightInfo_ = new ImageInfo;
    if (leftInfo_ == nullptr || rightInfo_ == nullptr) {
        delete leftInfo_;
        delete rightInfo_;
        return false;
    }
    memset_s(leftInfo_, sizeof(ImageInfo), 0, sizeof(ImageInfo));
    memset_s(rightInfo_, sizeof(ImageInfo), 0, sizeof(ImageInfo));
    leftCard->GetBitmap(*leftInfo_);
    rightCard->GetBitmap(*rightInfo_);
    return true;
}

float TurnPageEffect::ComputeAngle(int16_t xOffset)
{
    float w = Screen::GetInstance().GetWidth();
    float x = xOffset;
    const float maxAngle = 180;
    const float rightAngle = 90;
    x = x < 0 ? -x : w - x;
    float angle = x / w * maxAngle;
    if (xOffset == 0) {
        angle = lastPage_ ? 0 : maxAngle;
    }
    lastPage_ = (angle < rightAngle);
    return angle;
}
}

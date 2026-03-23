/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: PushPressEffect
 * Create: 2025-04
 */

#include "securec.h"
#include "common/screen.h"
#include "push_press_effect.h"

namespace OHOS {
static constexpr int16_t CAMERA_DISTANCE = 600;
static constexpr int16_t EASE_RANGE = 100;
static constexpr float PERCENT = 100.f;
static constexpr float SCALE_RANGE = 0.2f;
static constexpr float SCALE_MAX = 1.0f;
static constexpr int16_t NUM_2 = 2;

void PushPressEffect::ResetPreviousCards(void)
{
    if (preLeftCard_ != nullptr) {
        preLeftCard_->SetVisible(true);
        preRightCard_->SetVisible(true);
        preLeftCard_ = nullptr;
        preRightCard_ = nullptr;
        FreeInfo();
    }
}

void PushPressEffect::StopEffect()
{
    ResetPreviousCards();
    if (added_) {
        container_->UIViewGroup::Remove(&leftImg_);
        container_->UIViewGroup::Remove(&rightImg_);
        added_ = false;
    }
    CardSwipe::StopEffect();
}

bool PushPressEffect::UpdateCardVisibility(UIView* leftCard, UIView* rightCard)
{
    if (preLeftCard_ == leftCard || preRightCard_ == rightCard) {
        return true;
    }

    ResetPreviousCards();

    if (leftCard->GetViewType() == UI_IMAGE_VIEW) {
        lInfo_ = *(static_cast<UIImageView *>(leftCard)->GetImageInfo());
        rInfo_ = *(static_cast<UIImageView *>(rightCard)->GetImageInfo());
    } else {
        FreeInfo();
        if (!AllocInfo(leftCard, rightCard)) {
            GRAPHIC_LOGE("get bitmap failed!");
            return false;
        }
        lInfo_ = *leftInfo_;
        rInfo_ = *rightInfo_;
    }
    leftCard->SetVisible(false);
    rightCard->SetVisible(false);
    preLeftCard_ = leftCard;
    preRightCard_ = rightCard;
    return true;
}

void PushPressEffect::ResetAndAddImages(void)
{
    if (!added_) {
        leftImg_.SetAutoEnable(false);
        leftImg_.SetResizeMode(UIImageView::ImageResizeMode::FILL);
        leftImg_.SetCameraDistance(CAMERA_DISTANCE);
        container_->Add(&leftImg_);

        rightImg_.SetAutoEnable(false);
        rightImg_.SetResizeMode(UIImageView::ImageResizeMode::FILL);
        rightImg_.SetCameraDistance(CAMERA_DISTANCE);
        container_->Add(&rightImg_);
        added_ = true;
    }
    leftImg_.SetSrc(&lInfo_);
    rightImg_.SetSrc(&rInfo_);
}

void PushPressEffect::FreeInfo(void)
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

bool PushPressEffect::AllocInfo(UIView* leftCard, UIView* rightCard)
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

void PushPressEffect::CommonAlg(UIView* leftCard, UIView* rightCard, int16_t xOffset)
{
    if (xOffset == 0) {
        return;
    }
    static const int16_t middleHeight = height_ / NUM_2;
    if (preLeftCard_ != leftCard && preRightCard_ != rightCard) {
        if (!UpdateCardVisibility(leftCard, rightCard)) {
            return;
        }
        ResetAndAddImages();
    }

    const float scaleFactor = EasingEquation::LinearEaseNone(0, EASE_RANGE, abs(xOffset), width_) / PERCENT;
    const float outScale = SCALE_MAX - SCALE_RANGE * scaleFactor;
    const float intScale = SCALE_MAX - SCALE_RANGE * (SCALE_MAX - scaleFactor);

    int16_t cardWidth = 0;
    float leftScale = 0;
    float rightScale = 0;
    if (xOffset > 0) {
        cardWidth = xOffset;
        leftScale = intScale;
        rightScale = outScale;
    } else {
        cardWidth = width_ + xOffset;
        leftScale = outScale;
        rightScale = intScale;
    }

    Vector2<float> leftScaleCenter = {cardWidth / NUM_2, middleHeight};
    Vector2<float> rightScaleCenter = {(width_ - cardWidth) / NUM_2, middleHeight};

    leftImg_.SetPosition(0, 0, cardWidth, height_);
    leftImg_.Scale(Vector2<float>(SCALE_MAX, leftScale), leftScaleCenter);
    rightImg_.SetPosition(cardWidth, 0, width_ - cardWidth, height_);
    rightImg_.Scale(Vector2<float>(SCALE_MAX, rightScale), rightScaleCenter);
    return;
}

void PushPressEffect::CardSwipeAlg(UICardPage* leftCard, UICardPage* rightCard, int16_t xOffset)
{
    CardSwipe::CardSwipeAlg(leftCard, rightCard, xOffset);
    CommonAlg(leftCard, rightCard, xOffset);
}

void PushPressEffect::CardSwipeAlg(UIImageView* leftCard, UIImageView* rightCard, int16_t xOffset)
{
    CardSwipe::CardSwipeAlg(leftCard, rightCard, xOffset);
    CommonAlg(leftCard, rightCard, xOffset);
}

}

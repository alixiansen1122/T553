/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: PushPressEffect
 * Create: 2025-04
 */

#ifndef PUSH_PRESS_EFFECT_H
#define PUSH_PRESS_EFFECT_H
#include "common/screen.h"
#include "components/ui_cross_view.h"

namespace OHOS {
class PushPressEffect : public CardSwipe {
public:
    PushPressEffect(void)
    {
        isNeedClip_ = true;
        width_ = Screen::GetInstance().GetWidth();
        height_ = Screen::GetInstance().GetHeight();
    }
    ~PushPressEffect() override
    {
        FreeInfo();
    }
    void CardSwipeAlg(UICardPage* leftCard, UICardPage* rightCard, int16_t xOffset) override;
    void CardSwipeAlg(UIImageView* leftCard, UIImageView* rightCard, int16_t xOffset) override;
    void StopEffect() override;

private:
    int16_t width_;
    int16_t height_;
    UIImageView leftImg_;
    UIImageView rightImg_;
    bool added_ = false;
    UIView* preLeftCard_ = nullptr;
    UIView* preRightCard_ = nullptr;
    ImageInfo* leftInfo_ = nullptr;
    ImageInfo* rightInfo_ = nullptr;
    ImageInfo lInfo_ = {0};
    ImageInfo rInfo_ = {0};

    void CommonAlg(UIView* leftCard, UIView* rightCard, int16_t xOffset);
    void ResetAndAddImages(void);
    void ResetPreviousCards(void);
    bool UpdateCardVisibility(UIView* leftCard, UIView* rightCard);
    void FreeInfo(void);
    bool AllocInfo(UIView* leftCard, UIView* rightCard);
};
}
#endif

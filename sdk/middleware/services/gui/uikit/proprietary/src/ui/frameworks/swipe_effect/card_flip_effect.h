/*
 * Copyright (c) CompanyNameMagicTag 2023. All rights reserved.
 * Description: UiCrossView
 * Author:
 * Create: 2023-10
 */

#ifndef UI_CARD_FLIP_H
#define UI_CARD_FLIP_H
#include "components/ui_cross_view.h"

namespace OHOS {
class CardFlipEffect : public CardSwipe {
public:
    CardFlipEffect(void)
    {
        isNeedClip_ = true;
    }
    ~CardFlipEffect() override {}
    void CardSwipeAlg(UICardPage* leftCard, UICardPage* rightCard, int16_t xOffset) override;
    void CardSwipeAlg(UIImageView* leftCard, UIImageView* rightCard, int16_t xOffset) override;
    void StopEffect() override;

private:
    UIImageView img_;
    UIView* visibleCard_ = nullptr;
    UIView* preLeftCard = nullptr;
    UIView* preRightCard = nullptr;

    void CommonAlg(UIView* leftCard, UIView* rightCard, int16_t xOffset);
};
}
#endif

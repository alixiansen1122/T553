/*
 * Copyright (c) CompanyNameMagicTag 2023. All rights reserved.
 * Description: UiCrossView
 * Author:
 * Create: 2023-10
 */

#ifndef UI_ZOOM_H
#define UI_ZOOM_H
#include "components/ui_cross_view.h"

namespace OHOS {
class ZoomEffect : public CardSwipe {
public:
    ZoomEffect(void)
    {
        isNeedClip_ = true;
    }
    ~ZoomEffect() override {}
    void CardSwipeAlg(UICardPage* leftCard, UICardPage* rightCard, int16_t xOffset) override;
    void CardSwipeAlg(UIImageView* leftCard, UIImageView* rightCard, int16_t xOffset) override;
    void StopEffect() override;

private:
    UIView* preLeftCard = nullptr;
    UIView* preRightCard = nullptr;
    void CommonAlg(UIView* leftCard, UIView* rightCard, int16_t xOffset);
};
}
#endif

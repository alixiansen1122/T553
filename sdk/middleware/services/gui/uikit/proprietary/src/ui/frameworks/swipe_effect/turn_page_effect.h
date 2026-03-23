/*
 * Copyright (c) CompanyNameMagicTag 2023. All rights reserved.
 * Description: UiCrossView
 * Author:
 * Create: 2023-10
 */

#ifndef UI_TURN_PAGE_H
#define UI_TURN_PAGE_H
#include "common/screen.h"
#include "turn_page_view.h"
#include "components/ui_cross_view.h"

namespace OHOS {
class TurnPageEffect : public CardSwipe {
public:
    TurnPageEffect(void)
    {
        isNeedClip_ = true;
    }
    ~TurnPageEffect() override
    {
        FreeInfo();
    }
    void CardSwipeAlg(UICardPage* leftCard, UICardPage* rightCard, int16_t xOffset) override;
    void CardSwipeAlg(UIImageView* leftCard, UIImageView* rightCard, int16_t xOffset) override;
    void StopEffect() override;

private:
    TurnPageView turnPageView_;
    ImageInfo lInfo_ = {0};
    ImageInfo rInfo_ = {0};
    bool added_ = false;
    UIView* preLeftCard = nullptr;
    UIView* preRightCard = nullptr;
    ImageInfo *leftInfo_ = nullptr;
    ImageInfo *rightInfo_ = nullptr;
    bool lastPage_ = false; // false for left, true for right

    void CommonAlg(UIView* leftCard, UIView* rightCard, int16_t xOffset);
    void FreeInfo();
    bool AllocInfo(UIView* leftCard, UIView* rightCard);
    float ComputeAngle(int16_t xOffset);
    void AddToContainer()
    {
        if (!added_) {
            turnPageView_.SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
            container_->UIViewGroup::Add(&turnPageView_);
            added_ = true;
        }
    }

    void RemoveFromContainer()
    {
        if (added_) {
            container_->UIViewGroup::Remove(&turnPageView_);
            added_ = false;
        }
    }
};
}
#endif

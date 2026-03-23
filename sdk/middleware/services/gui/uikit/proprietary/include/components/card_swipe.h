/*
 * Copyright (c) CompanyNameMagicTag 2023. All rights reserved.
 * Description: CardSwipe
 * Author:
 * Create: 2023-09
 */

#ifndef UI_CARD_SWIPE_H
#define UI_CARD_SWIPE_H
#include "components/ui_card_page.h"
#include "components/ui_image_view.h"
#include "components/ui_view_group.h"

namespace OHOS {
class CardSwipe : public HeapBase {
public:
    /* *
     * @brief A destructor used to delete the <b>CardSwipe</b> instance.
     * @since 1.0
     * @version 1.0
     */
    virtual ~CardSwipe() {}

    /* *
     * @brief Swipe algorithm when drag card.
     * @param leftCard Left card.
     * @param rightCard Right card.
     * @param xOffset Drag distance, xOffset > 0 means left to right, xOffset < 0 means right to left.
     * @since 1.0
     * @version 1.0
     */
    virtual void CardSwipeAlg(UICardPage* leftCard, UICardPage* rightCard, int16_t xOffset)
    {
        if (!IsStarted()) {
            StartEffect();
        }
    }

    /* *
     * @brief Swipe algorithm when drag card.
     * @param leftCard Left card.
     * @param rightCard Right card.
     * @param xOffset Drag distance, xOffset > 0 means left to right, xOffset < 0 means right to left.
     * @since 1.0
     * @version 1.0
     */
    virtual void CardSwipeAlg(UIImageView* leftCard, UIImageView* rightCard, int16_t xOffset)
    {
        if (!IsStarted()) {
            StartEffect();
        }
    }

    /* *
     * @brief Begin card swipe effect
     * @since 1.0
     * @version 1.0
     */
    virtual void StartEffect()
    {
        isStarted_ = true;
    };

    /* *
     * @brief End card swipe effect
     * @since 1.0
     * @version 1.0
     */
    virtual void StopEffect()
    {
        isStarted_ = false;
    };

    /* *
     * @brief Return whether swipe effect is started.
     * @since 1.0
     * @version 1.0
     */
    bool IsStarted()
    {
        return isStarted_;
    }

    /* *
     * @brief Set container view (UICrossView) which is used to produce effect.
     * @since 1.0
     * @version 1.0
     */
    void SetContainer(UIViewGroup* view)
    {
        container_ = view;
    }

    /* *
     * @brief Enable clipping card page into a circle.
     * @since 1.0
     * @version 1.0
     */
    void EnableClip(bool enable)
    {
        isNeedClip_ = enable;
    }

    bool isNeedClip_ = false;

protected:
    UIViewGroup* container_ = nullptr;

private:
    bool isStarted_ = false;
};
}
#endif

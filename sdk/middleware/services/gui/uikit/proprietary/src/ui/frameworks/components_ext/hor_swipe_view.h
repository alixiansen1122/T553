/*
 * Copyright (c) CompanyNameMagicTag 2023. All rights reserved.
 * Description: HorSwipeView
 * Author:
 * Create: 2023-09
 */

#ifndef UI_HOR_SWIPE_VIEW_H
#define UI_HOR_SWIPE_VIEW_H
#include "components/ui_swipe_view.h"
#include "components/card_swipe.h"
#include "animator/animator.h"

namespace OHOS {
class HorSwipeView : public UISwipeView {
public:
    /* *
     * @brief A constructor used to create the <b>HorSwipeView</b> instance.
     * @since 1.0
     * @version 1.0
     */
    HorSwipeView() {}

    /* *
     * @brief A destructor used to delete the <b>HorSwipeView</b> instance.
     * @since 1.0
     * @version 1.0
     */
    ~HorSwipeView() override {}

    bool DragXInner(int16_t distance) override;
    bool OnDragEvent(const DragEvent& event) override;

    /* *
     * @brief It will call customer Swipe algorithm callback when drag card.
     * @since 1.0
     * @version 1.0
     */
    void SwipeCard(void);

    /* *
     * @brief Enable screen capture when swipe card(true) or nort(false).
     * @since 1.0
     * @version 1.0
     */
    void EnableScreenCap(bool enable)
    {
        enableScreenCap_ = enable;
    }

    /* *
     * @brief Register swipe callback used when drag card.
     * @since 1.0
     * @version 1.0
     */
    void RegisterSwipeCallback(CardSwipe* callback)
    {
        swipeCallback_ = callback;
    }

    /* *
     * @brief Return swipe callback.
     * @since 1.0
     * @version 1.0
     */
    CardSwipe* GetSwipeCallback()
    {
        return swipeCallback_;
    }

    /* *
     * @brief Set the start point for drag.
     * @since 1.0
     * @version 1.0
     */
    void SwipeStart(void);

private:
    bool enableScreenCap_ = false;
    CardSwipe* swipeCallback_ = nullptr;
    int16_t xStart_ = 0;
};
}
#endif

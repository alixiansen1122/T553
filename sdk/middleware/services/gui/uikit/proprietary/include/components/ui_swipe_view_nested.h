/*
 * Copyright (c) CompanyNameMagicTag 2024. All rights reserved.
 * Description: UISwipeViewNested
 * Author:
 * Create: 2024-12-09
 */

#ifndef GRAPHIC_LITE_UI_SWIPE_VIEW_NESTED_H
#define GRAPHIC_LITE_UI_SWIPE_VIEW_NESTED_H

#include "graphic_config.h"
#include "components/ui_swipe_view.h"

namespace OHOS {

class UISwipeViewNested : public UISwipeView {
public:
    explicit UISwipeViewNested(uint8_t direction = VERTICAL);

    ~UISwipeViewNested() override {}

    bool OnDragStartEvent(const DragEvent& event) override;

    bool OnDragEvent(const DragEvent& event) override;

    bool OnDragEndEvent(const DragEvent& event) override;

    /**
     * @brief Sets whether to intercept the drag event when the sliding is out of bounds.
     *        By default, the drag event is intercepted.
     *        Note that this takes effect only when the direction of the
     *        intercepting is the same as the direction of the widget itself.
     *        For example, if the widget is horizontally oriented, the intercepting of sliding left or right will work.
     *
     * @param direction Indicates the direction of the drag event. The value range is
     *                  DragEvent::DIRECTION_LEFT_TO_RIGHT, DragEvent::DIRECTION_RIGHT_TO_LEFT,
     *                  DragEvent::DIRECTION_TOP_TO_BOTTOM, DragEvent::DIRECTION_BOTTOM_TO_TOP.
     *
     * @param isIntercept Indicates whether to intercept an out-of-bounds sliding in this direction.
     *                    <b>true</b> indicates that the sliding is intercepted,
     *                    and <b>false</b> indicates that the sliding is not intercepted.
     *
     * @since 1.0
     * @version 1.0
     */
    void SetBoundaryIntercept(uint8_t direction, bool isIntercept);

    /**
     * @brief Indicates whether to intercept the drag event when the sliding is out of bounds.
     *
     * @param direction Indicates the direction of the drag event. The value range is
     *                  DragEvent::DIRECTION_LEFT_TO_RIGHT, DragEvent::DIRECTION_RIGHT_TO_LEFT,
     *                  DragEvent::DIRECTION_TOP_TO_BOTTOM, DragEvent::DIRECTION_BOTTOM_TO_TOP.
     *
     * @return <b>true</b> indicates that the sliding is intercepted,
     *         and <b>false</b> indicates that the sliding is not intercepted.
     *
     * @since 1.0
     * @version 1.0
     */
    bool IsBoundaryIntercept(uint8_t direction) const;

private:
    bool isConsumed_ = false;
    bool isIntercepts_[4] = {true, true, true, true}; // 4: the number of direction
}; // class UISwipeViewNested

} // namespace OHOS

#endif // GRAPHIC_LITE_UI_SWIPE_VIEW_NESTED_H

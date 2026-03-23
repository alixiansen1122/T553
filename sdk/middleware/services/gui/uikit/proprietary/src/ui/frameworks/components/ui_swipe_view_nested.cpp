/*
 * Copyright (c) CompanyNameMagicTag 2024. All rights reserved.
 * Description: UISwipeViewNested
 * Author:
 * Create: 2024-12-09
 */

#include "components/ui_swipe_view_nested.h"

namespace OHOS {
UISwipeViewNested::UISwipeViewNested(uint8_t direction) : UISwipeView(direction)
{
}

bool UISwipeViewNested::OnDragStartEvent(const DragEvent& event)
{
    if (reboundSize_ != 0) {
        for (uint8_t i = 0; i < sizeof(isIntercepts_) / sizeof(bool); ++i) {
            isIntercepts_[i] = true;
        }
    }

    uint8_t dragDirection;
    if (MATH_ABS(event.GetDeltaX()) >= MATH_ABS(event.GetDeltaY())) {
        dragDirection = HORIZONTAL;
    } else {
        dragDirection = VERTICAL;
    }
    isConsumed_ = (direction_ == dragDirection) && (childrenHead_ != nullptr);

    UIView::OnDragStartEvent(event);
    return isConsumed_;
}

bool UISwipeViewNested::OnDragEvent(const DragEvent& event)
{
    if (!isConsumed_) {
        UIView::OnDragEvent(event);
        return false;
    }

    UISwipeView::OnDragEvent(event);
    if (loop_) {
        return true;
    }
    bool outOfBounds[4] = {false, false, false, false}; // 4: the number of direction
    int16_t height = GetHeight();
    int16_t width = GetWidth();

    if (direction_ == VERTICAL) {
        outOfBounds[DragEvent::DIRECTION_TOP_TO_BOTTOM] = childrenHead_->GetY() >= blankSize_;
        outOfBounds[DragEvent::DIRECTION_BOTTOM_TO_TOP] =
            childrenTail_->GetRelativeRect().GetBottom() <= height - blankSize_;
    } else {
        outOfBounds[DragEvent::DIRECTION_LEFT_TO_RIGHT] = childrenHead_->GetX() >= blankSize_;
        outOfBounds[DragEvent::DIRECTION_RIGHT_TO_LEFT] =
            childrenTail_->GetRelativeRect().GetRight() <= width - blankSize_;
    }

    bool transfer = false;
    for (uint8_t i = 0; i < sizeof(outOfBounds) / sizeof(bool); ++i) {
        transfer = transfer || (outOfBounds[i] && !IsBoundaryIntercept(i));
    }
    isConsumed_ = !transfer;
    if (!isConsumed_) {
        UIView *view = GetParent();
        while (view != nullptr) {
            if (view->OnDragStartEvent(event)) {
                break;
            }
            view = view->GetParent();
        }
    }
    return isConsumed_;
}

bool UISwipeViewNested::OnDragEndEvent(const DragEvent& event)
{
    if (!isConsumed_) {
        UIView::OnDragEndEvent(event);
        return false;
    }
    UISwipeView::OnDragEndEvent(event);
    return true;
}

void UISwipeViewNested::SetBoundaryIntercept(uint8_t direction, bool isIntercept)
{
    if (direction > DragEvent::DIRECTION_BOTTOM_TO_TOP) {
        return;
    }
    if (reboundSize_ != 0) {
        GRAPHIC_LOGE("rebound size is not zero!");
        return;
    }
    uint8_t dragDirection = (direction == DragEvent::DIRECTION_LEFT_TO_RIGHT ||
        direction == DragEvent::DIRECTION_RIGHT_TO_LEFT) ? HORIZONTAL : VERTICAL;
    if (dragDirection != direction_) {
        GRAPHIC_LOGE("The interception direction does not match the widget direction.");
        return;
    }
    isIntercepts_[direction] = isIntercept;
}

bool UISwipeViewNested::IsBoundaryIntercept(uint8_t direction) const
{
    if (direction > DragEvent::DIRECTION_BOTTOM_TO_TOP) {
        return false;
    }
    return isIntercepts_[direction];
}
} // namespace OHOS

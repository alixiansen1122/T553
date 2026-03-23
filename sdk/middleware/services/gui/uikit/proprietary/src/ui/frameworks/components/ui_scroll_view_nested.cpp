/*
 * Copyright (c) CompanyNameMagicTag 2024. All rights reserved.
 * Description: UIScrollViewNested
 * Author:
 * Create: 2024-12-09
 */

#include "components/ui_scroll_view_nested.h"

namespace OHOS {
UIScrollViewNested::UIScrollViewNested(uint8_t direction) : UIScrollView()
{
    direction_ = direction;
}

bool UIScrollViewNested::OnDragStartEvent(const DragEvent& event)
{
    if (reboundSizeTop_ != 0) {
        isIntercepts_[DragEvent::DIRECTION_TOP_TO_BOTTOM] = true;
    }
    if (reboundSizeBottom_ != 0) {
        isIntercepts_[DragEvent::DIRECTION_BOTTOM_TO_TOP] = true;
    }
    if (reboundSizeLeft_ != 0) {
        isIntercepts_[DragEvent::DIRECTION_LEFT_TO_RIGHT] = true;
    }
    if (reboundSizeRight_ != 0) {
        isIntercepts_[DragEvent::DIRECTION_RIGHT_TO_LEFT] = true;
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

bool UIScrollViewNested::OnDragEvent(const DragEvent& event)
{
    if (!isConsumed_) {
        UIView::OnDragEvent(event);
        return false;
    }

    UIScrollView::OnDragEvent(event);
    Rect childRect = GetAllChildRelativeRect();
    int16_t scrollWidth = GetWidth();
    int16_t scrollHeight = GetHeight();
    bool outOfBounds[4] = {false, false, false, false}; // 4: the number of direction
    if (direction_ == VERTICAL) {
        outOfBounds[DragEvent::DIRECTION_TOP_TO_BOTTOM] = childRect.GetTop() >= scrollBlankSizeTop_;
        outOfBounds[DragEvent::DIRECTION_BOTTOM_TO_TOP] =
            childRect.GetBottom() <= scrollHeight - scrollBlankSizeBottom_;
    } else {
        outOfBounds[DragEvent::DIRECTION_LEFT_TO_RIGHT] = childRect.GetLeft() >= scrollBlankSizeLeft_;
        outOfBounds[DragEvent::DIRECTION_RIGHT_TO_LEFT] = childRect.GetRight() <= scrollWidth - scrollBlankSizeRight_;
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

bool UIScrollViewNested::OnDragEndEvent(const DragEvent& event)
{
    if (!isConsumed_) {
        UIView::OnDragEndEvent(event);
        return false;
    }
    UIScrollView::OnDragEndEvent(event);
    return true;
}

void UIScrollViewNested::SetBoundaryIntercept(uint8_t direction, bool isIntercept)
{
    switch (direction) {
        case DragEvent::DIRECTION_LEFT_TO_RIGHT:
            if (reboundSizeLeft_ != 0) {
                GRAPHIC_LOGE("reboundSizeLeft_ is not zero!");
                return;
            }
            break;
        case DragEvent::DIRECTION_RIGHT_TO_LEFT:
            if (reboundSizeRight_ != 0) {
                GRAPHIC_LOGE("reboundSizeRight_ is not zero!");
                return;
            }
            break;
        case DragEvent::DIRECTION_TOP_TO_BOTTOM:
            if (reboundSizeTop_ != 0) {
                GRAPHIC_LOGE("reboundSizeTop_ is not zero!");
                return;
            }
            break;
        case DragEvent::DIRECTION_BOTTOM_TO_TOP:
            if (reboundSizeBottom_ != 0) {
                GRAPHIC_LOGE("reboundSizeBottom_ is not zero!");
                return;
            }
            break;
        default:
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

bool UIScrollViewNested::IsBoundaryIntercept(uint8_t direction) const
{
    if (direction > DragEvent::DIRECTION_BOTTOM_TO_TOP) {
        return false;
    }
    return isIntercepts_[direction];
}
} // namespace OHOS

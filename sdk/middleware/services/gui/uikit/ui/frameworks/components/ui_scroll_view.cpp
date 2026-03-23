/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "components/ui_scroll_view.h"

#include "components/ui_abstract_scroll_bar.h"
#include "dock/focus_manager.h"
#include "dock/vibrator_manager.h"
#include "draw/draw_rect.h"
#include "gfx_utils/graphic_log.h"

namespace OHOS {
UIScrollView::UIScrollView()
    : scrollListener_(nullptr)
{
#if ENABLE_ROTATE_INPUT
    rotateFactor_ = DEFAULT_SCROLL_VIEW_ROTATE_FACTOR;
    lastRotateLen_ = 0;
#endif
#if ENABLE_VIBRATOR
    totalRotateLen_ = 0;
    lastVibratorRotateLen_ = 0;
    vibrationLenThreshold_ = DEFAULT_SCROLL_VIEW_VIBRATION_LEN;
#endif
#if ENABLE_FOCUS_MANAGER
    focusable_ = true;
#endif
    direction_ = HORIZONTAL_AND_VERTICAL;
}

bool UIScrollView::OnDragEvent(const DragEvent& event)
{
    if (scrollAnimator_.GetState() != Animator::STOP) {
        UIAbstractScroll::StopAnimator();
    }
    Drag(event);
    return UIView::OnDragEvent(event);
}

bool UIScrollView::OnDragEndEvent(const DragEvent& event)
{
    Point last = event.GetPreLastPoint();
    Point current = event.GetLastPoint();
    if ((last.x == current.x) && (last.y == current.y)) {
        last = current;
        current = event.GetCurrentPos();
    }

    if (!DragThrowAnimator(current, last, event.GetDragDirection())) {
        if (scrollListener_ && (scrollListener_->GetScrollState() == OnScrollListener::SCROLL_STATE_MOVE)) {
            scrollListener_->OnScrollEnd();
            scrollListener_->SetScrollState(OnScrollListener::SCROLL_STATE_STOP);
        }
    }
    return UIView::OnDragEndEvent(event);
}

void UIScrollView::Drag(const DragEvent& event)
{
    int16_t xDistance = event.GetDeltaX();
    int16_t yDistance = event.GetDeltaY();

    if ((direction_ == HORIZONTAL || direction_ == HORIZONTAL_AND_VERTICAL) && xDistance != 0) {
        DragXInner(xDistance);
    }
    if ((direction_ == VERTICAL || direction_ == HORIZONTAL_AND_VERTICAL) && yDistance != 0) {
        RefreshDelta(yDistance);
        DragYInner(yDistance);
    }
}

bool UIScrollView::OnPressEvent(const PressEvent& event)
{
    StopAnimator();
    return UIView::OnPressEvent(event);
}

#if ENABLE_ROTATE_INPUT
bool UIScrollView::OnRotateStartEvent(const RotateEvent& event)
{
    if (scrollAnimator_.GetState() != Animator::STOP) {
        UIAbstractScroll::StopAnimator();
    }
    return UIView::OnRotateStartEvent(event);
}

bool UIScrollView::OnRotateEvent(const RotateEvent& event)
{
    if (direction_ == HORIZONTAL_NOR_VERTICAL) {
        return UIView::OnRotateEvent(event);
    }
    lastRotateLen_ = static_cast<int16_t>(event.GetRotate() * rotateFactor_);
#if ENABLE_VIBRATOR
    bool lastIsEdge = false;
    Rect childRect = GetAllChildRelativeRect();
    if (direction_ == HORIZONTAL) {
        if (childRect.GetLeft() - scrollBlankSizeLeft_ >= 0 ||
            childRect.GetRight() + scrollBlankSizeRight_ <= GetWidth()) {
            lastIsEdge = true;
        }
    } else {
        if (childRect.GetTop() - scrollBlankSizeTop_ >= 0 ||
            childRect.GetBottom() + scrollBlankSizeBottom_ <= GetHeight()) {
            lastIsEdge = true;
        }
    }
#endif
    if (direction_ == HORIZONTAL) {
        DragXInner(lastRotateLen_);
    } else {
        DragYInner(lastRotateLen_);
    }
#if ENABLE_VIBRATOR
    totalRotateLen_ += lastRotateLen_;
    childRect = GetAllChildRelativeRect();
    bool isEdge = false;
    if (direction_ == HORIZONTAL) {
        if (childRect.GetLeft() - scrollBlankSizeLeft_ >= 0 ||
            childRect.GetRight() + scrollBlankSizeRight_ <= GetWidth()) {
            isEdge = true;
        }
    } else {
        if (childRect.GetTop() - scrollBlankSizeTop_ >= 0 ||
            childRect.GetBottom() + scrollBlankSizeBottom_ <= GetHeight()) {
            isEdge = true;
        }
    }
    VibratorFunc vibratorFunc = VibratorManager::GetInstance()->GetVibratorFunc();
    if (vibratorFunc != nullptr && !isEdge) {
        uint16_t rotateLen = MATH_ABS(totalRotateLen_ - lastVibratorRotateLen_);
        if (rotateLen > vibrationLenThreshold_) {
            uint16_t vibrationCnt = rotateLen / vibrationLenThreshold_;
            for (uint16_t i = 0; i < vibrationCnt; i++) {
                GRAPHIC_LOGI("UIScrollView::OnRotateEvent calls TYPE_ONE vibrator");
                vibratorFunc(VibratorType::VIBRATOR_TYPE_ONE);
            }
            lastVibratorRotateLen_ = totalRotateLen_;
        }
    }
    if (vibratorFunc != nullptr && (!lastIsEdge && isEdge)) {
        GRAPHIC_LOGI("UIScrollView::OnRotateEvent calls TYPE_THREE vibrator");
        vibratorFunc(VibratorType::VIBRATOR_TYPE_THREE);
    }
#endif
    return UIView::OnRotateEvent(event);
}

bool UIScrollView::OnRotateEndEvent(const RotateEvent& event)
{
    if (direction_ == HORIZONTAL_NOR_VERTICAL) {
        return UIView::OnRotateEvent(event);
    }
    return UIAbstractScroll::OnRotateEndEvent(event);
}
#endif

void UIScrollView::ScrollBy(int16_t xDistance, int16_t yDistance)
{
    if ((direction_ == HORIZONTAL || direction_ == HORIZONTAL_AND_VERTICAL) && xDistance != 0) {
        DragXInner(xDistance);
    }
    if ((direction_ == VERTICAL || direction_ == HORIZONTAL_AND_VERTICAL) && yDistance != 0) {
        DragYInner(yDistance);
    }
    if ((scrollListener_ != nullptr) && (scrollListener_->GetScrollState() == OnScrollListener::SCROLL_STATE_MOVE)) {
        scrollListener_->OnScrollEnd();
        scrollListener_->SetScrollState(OnScrollListener::SCROLL_STATE_STOP);
    }
}

bool UIScrollView::DragXInner(int16_t distance)
{
    Rect childRect = GetAllChildRelativeRect();
    int16_t reboundSizeLeft = reboundSizeLeft_;
    int16_t reboundSizeRight = reboundSizeRight_;
    if (scrollAnimator_.GetState() != Animator::STOP) {
        reboundSizeLeft = 0;
        reboundSizeRight = 0;
    }

    if (childRect.GetWidth() <= (GetWidth() - scrollBlankSizeLeft_ - scrollBlankSizeRight_) ||
        !(direction_ == HORIZONTAL || direction_ == HORIZONTAL_AND_VERTICAL)) {
        return false;
    }

    if (distance > 0) {
        if (childRect.GetLeft() > scrollBlankSizeLeft_ + reboundSizeLeft) {
            distance = 0;
        } else if ((childRect.GetLeft() + distance) > scrollBlankSizeLeft_ + reboundSizeLeft) {
            distance = scrollBlankSizeLeft_ - childRect.GetLeft() + reboundSizeLeft;
        }
    } else {
        int16_t childRight = childRect.GetRight();
        int16_t scrollWidth = GetWidth();
        if (childRight < scrollWidth - (scrollBlankSizeRight_ + reboundSizeRight)) {
            distance = 0;
        } else if (childRight + distance < scrollWidth - (scrollBlankSizeRight_ + reboundSizeRight)) {
            distance = scrollWidth - (scrollBlankSizeRight_ + reboundSizeRight) - childRight - 1;
        }
    }

    return MoveOffset(distance, 0);
}

bool UIScrollView::DragYInner(int16_t distance)
{
    Rect childRect = GetAllChildRelativeRect();
    int16_t reboundSizeTop = reboundSizeTop_;
    int16_t reboundSizeBottom = reboundSizeBottom_;
    if (scrollAnimator_.GetState() != Animator::STOP) {
        reboundSizeTop = 0;
        reboundSizeBottom = 0;
    }

    if (childRect.GetHeight() <= (GetHeight() - scrollBlankSizeTop_ - scrollBlankSizeBottom_) ||
        !(direction_ == VERTICAL || direction_ == HORIZONTAL_AND_VERTICAL)) {
        return false;
    }

    if (distance > 0) {
        if (childRect.GetTop() > scrollBlankSizeTop_ + reboundSizeTop) {
            distance = 0;
        } else if ((childRect.GetTop() + distance) > scrollBlankSizeTop_ + reboundSizeTop) {
            distance = scrollBlankSizeTop_ - childRect.GetTop() + reboundSizeTop;
        }
    } else {
        int16_t childBottom = childRect.GetBottom();
        int16_t scrollHeight = GetHeight();
        if (childBottom < scrollHeight - (scrollBlankSizeBottom_ + reboundSizeBottom)) {
                distance = 0;
        } else if (childBottom + distance < scrollHeight - (scrollBlankSizeBottom_ + reboundSizeBottom)) {
                distance = scrollHeight - (scrollBlankSizeBottom_ + reboundSizeBottom) - childBottom - 1;
        }
    }

    return MoveOffset(0, distance);
}

bool UIScrollView::MoveOffset(int16_t offsetX, int16_t offsetY)
{
    if ((offsetX != 0) || (offsetY != 0)) {
        if ((scrollListener_ != nullptr) &&
            (scrollListener_->GetScrollState() == OnScrollListener::SCROLL_STATE_STOP)) {
            scrollListener_->OnScrollStart();
            scrollListener_->SetScrollState(OnScrollListener::SCROLL_STATE_MOVE);
        }
        UIAbstractScroll::MoveChildByOffset(offsetX, offsetY);
        if (xScrollBarVisible_ || yScrollBarVisible_) {
            RefreshScrollBar();
        }
        Invalidate();
        return true;
    }
    return false;
}

void UIScrollView::RefreshScrollBar()
{
    Rect childrenRect = GetAllChildRelativeRect();
    /* calculate scrollBar's the proportion of foreground */
    if (yScrollBarVisible_) {
        int16_t totalLen = childrenRect.GetHeight() + scrollBlankSizeTop_ + scrollBlankSizeBottom_;
        int16_t len = GetHeight();
        yScrollBar_->SetForegroundProportion(static_cast<float>(len) / totalLen);
        /* calculate scrolling progress */
        yScrollBar_->SetScrollProgress(static_cast<float>(scrollBlankSizeTop_ - childrenRect.GetTop()) /
            (totalLen - len));
    }
    if (xScrollBarVisible_) {
        /* so do x-bar */
        int16_t totalLen = childrenRect.GetWidth() + scrollBlankSizeLeft_ + scrollBlankSizeRight_;
        int16_t len = GetWidth();
        xScrollBar_->SetForegroundProportion(static_cast<float>(len) / totalLen);
        xScrollBar_->SetScrollProgress(static_cast<float>(scrollBlankSizeLeft_ - childrenRect.GetLeft()) /
            (totalLen - len));
    }
    RefreshAnimator();
}

void UIScrollView::CalculateReboundDistance(int16_t& dragDistanceX, int16_t& dragDistanceY)
{
    Rect rect = GetAllChildRelativeRect();
    int16_t top = rect.GetTop();
    int16_t bottom = rect.GetBottom();
    int16_t scrollHeight = GetHeight();
    int16_t left = rect.GetLeft();
    int16_t right = rect.GetRight();
    int16_t scrollWidth = GetWidth();
    if (scrollBlankSizeTop_ < top) {
        dragDistanceY = scrollBlankSizeTop_ - top;
    } else if (bottom < (scrollHeight - scrollBlankSizeBottom_ - 1)) {
        dragDistanceY = scrollHeight - scrollBlankSizeBottom_ - bottom - 1;
    }

    if (scrollBlankSizeLeft_ < left) {
        dragDistanceX = scrollBlankSizeLeft_ - left;
    } else if (right < (scrollWidth - scrollBlankSizeRight_ - 1)) {
        dragDistanceX = scrollWidth - scrollBlankSizeRight_ - right - 1;
    }
}

void UIScrollView::StopAnimator()
{
    if ((scrollListener_ != nullptr) && (scrollListener_->GetScrollState() == OnScrollListener::SCROLL_STATE_MOVE)) {
        scrollListener_->OnScrollEnd();
        scrollListener_->SetScrollState(OnScrollListener::SCROLL_STATE_STOP);
    }
    UIAbstractScroll::StopAnimator();
}
} // namespace OHOS

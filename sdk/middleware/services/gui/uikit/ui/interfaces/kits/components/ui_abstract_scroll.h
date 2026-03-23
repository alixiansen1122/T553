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

/**
 * @addtogroup UI_Components
 * @{
 *
 * @brief Defines UI components such as buttons, texts, images, lists, and progress bars.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file ui_abstract_scroll.h
 *
 * @brief Declares the base class used to define the attributes of a scroll. The <b>UIList</b>, <b>UIScrollView</b>, and
 *        <b>UISwipeView</b> inherit from this class.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_UI_ABSTRACT_SCROLL_H
#define GRAPHIC_LITE_UI_ABSTRACT_SCROLL_H

#include "animator/animator.h"
#include "animator/easing_equation.h"
#include "components/ui_view_group.h"

namespace OHOS {
class BarEaseInOutAnimator;
class UIAbstractScrollBar;
/**
 * @brief Defines the attributes of a scroll, including the scroll direction, blank size of a scroll view, velocity and
 *        effects of a scroll animation.
 *
 * @since 1.0
 * @version 1.0
 */
class UIAbstractScroll : public UIViewGroup {
public:
    /**
     * @brief A constructor used to create a <b>UIAbstractScroll</b> instance.
     *
     * @since 1.0
     * @version 1.0
     */
    UIAbstractScroll();

    /**
     * @brief A destructor used to delete the <b>UIAbstractScroll</b> instance.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual ~UIAbstractScroll();

    /**
     * @brief Obtains the view type.
     * @return Returns the view type, as defined in {@link UIViewType}.
     * @since 1.0
     * @version 1.0
     */
    UIViewType GetViewType() const override
    {
        return UI_ABSTRACT_SCROLL;
    }

    enum class Direction {
        ALL,
        TOP,
        BOTTOM,
        LEFT,
        RIGHT,
    };

    /**
     * @brief Sets the blank size for this scroll view.
     *
     *
     * @param value Indicates the blank size to set. The default value is <b>0</b>. Taking a vertical scroll as an
     *              example, the value <b>0</b> indicates that the head node can only scroll downwards the top of the
     *              view and the tail node scroll upwards the bottom; the value <b>10</b> indicates that the head node
     *              can continue scrolling down by 10 pixels after it reaches the top of the view.
     * @param direction Indicates the direction of blank size to set. Default is ALL. Direction can be specified for UIList and UIScrollView.
     * @since 1.0
     * @version 1.0
     */
    void SetScrollBlankSize(uint16_t size, UIAbstractScroll::Direction direction = UIAbstractScroll::Direction::ALL)
    {
        switch (direction) {
            case UIAbstractScroll::Direction::ALL:
                scrollBlankSize_ = size;
                scrollBlankSizeTop_ = size;
                scrollBlankSizeBottom_ = size;
                scrollBlankSizeLeft_ = size;
                scrollBlankSizeRight_ = size;
                break;
            case UIAbstractScroll::Direction::TOP:
                scrollBlankSizeTop_ = size;
                break;
            case UIAbstractScroll::Direction::BOTTOM:
                scrollBlankSizeBottom_ = size;
                break;
            case UIAbstractScroll::Direction::LEFT:
                scrollBlankSizeLeft_ = size;
                break;
            case UIAbstractScroll::Direction::RIGHT:
                scrollBlankSizeRight_ = size;
                break;
        }
    }

    /**
     * @brief Sets the rebound size in a specific direction.
     * @param size Indicates the rebound size to set.
     * @param direction Indicates the direction of rebound size to set. Default is ALL. Direction can be specified for UIList and UIScrollView.
     */
    void SetReboundSize(uint16_t size, UIAbstractScroll::Direction direction = UIAbstractScroll::Direction::ALL)
    {
        switch (direction) {
            case UIAbstractScroll::Direction::ALL:
                reboundSize_ = size;
                reboundSizeTop_ = size;
                reboundSizeBottom_ = size;
                reboundSizeLeft_ = size;
                reboundSizeRight_ = size;
                break;
            case UIAbstractScroll::Direction::TOP:
                reboundSizeTop_ = size;
                break;
            case UIAbstractScroll::Direction::BOTTOM:
                reboundSizeBottom_ = size;
                break;
            case UIAbstractScroll::Direction::LEFT:
                reboundSizeLeft_ = size;
                break;
            case UIAbstractScroll::Direction::RIGHT:
                reboundSizeRight_ = size;
                break;
        }
    }

    /**
     * @brief Sets the maximum scroll distance after a finger lifts the screen.
     *
     * @param distance Indicates the maximum scroll distance to set. The default value is <b>0</b>, indicating that the
     *                 scroll distance is not limited.
     * @since 1.0
     * @version 1.0
     */
    void SetMaxScrollDistance(uint16_t distance)
    {
        maxScrollDistance_ = distance;
    }

    /**
     * @brief Obtains the maximum scroll distance after a finger lifts the screen.
     *
     * @return Returns the maximum scroll distance. The default value is <b>0</b>, indicating that the scroll distance
     * is not limited.
     * @since 1.0
     * @version 1.0
     */
    uint16_t GetMaxScrollDistance() const
    {
        return maxScrollDistance_;
    }

    /**
     * @brief Sets the easing function that specifies a scroll animation after a finger lifts the screen.
     *
     * @param func Indicates the easing function to set. The default function is {@link EasingEquation::CubicEaseOut}.
     *             For details, see {@link EasingEquation}.
     * @since 1.0
     * @version 1.0
     */
    void SetDragFunc(EasingFunc func)
    {
        easingFunc_ = func;
    }

    /**
     * @brief Sets whether to continue scrolling after a finger lifts the screen.
     *
     * @param throwDrag Specifies whether to continue scrolling after a finger lifts the screen. <b>true</b> indicates
     *                  the scroll continues, and <b>false</b> indicates the scroll stops immediately after a finger
     *                  lifts.
     * @since 1.0
     * @version 1.0
     */
    void SetThrowDrag(bool throwDrag)
    {
        throwDrag_ = throwDrag;
    }

    /**
     * @brief Sets whether to rebound with elastic mode.
     *
     * @param elasticEnable Specifies whether to rebound with elastic mode after a finger lifts the screen while
     *        left/right is off bound. <b>true</b> indicates will rebound with elastic mode. and <b>false</b> indicates
     *        not.
     * @since 1.0
     * @version 1.0
     */
    void SetElastic(bool isElastic)
    {
        isElastic_ = isElastic;
    }

    /**
     * @brief Sets elastic times.
     *
     * @param elasticTimes elastic times, default is <b>200</b>
     * @since 1.0
     * @version 1.0
     */
    void SetElasticTime(uint16_t elasticTime)
    {
        elasticTime_ = elasticTime;
    }

    /**
     * @brief Moves the position of all child views.
     *
     * @param offsetX Indicates the offset distance by which a child view is moved on the x-axis.
     * @param offsetY Indicates the offset distance by which a child view is moved on the y-axis.
     * @since 1.0
     * @version 1.0
     */
    void MoveChildByOffset(int16_t offsetX, int16_t offsetY) override;

    /**
     * @brief Sets the drag acceleration.
     *
     * @param value Indicates the drag acceleration to set. The default value is <b>10</b>. A larger drag acceleration
     *              indicates a higher inertial scroll velocity.
     * @since 1.0
     * @version 1.0
     */
    void SetDragACCLevel(uint8_t value)
    {
        if (value != 0) {
            dragAccCoefficient_ = value;
        }
    }

    /**
     * @brief Obtains the drag acceleration.
     *
     * @return Returns the drag acceleration.
     * @since 1.0
     * @version 1.0
     */
    uint8_t GetDragACCLevel() const
    {
        return dragAccCoefficient_;
    }

    /**
     * @brief Sets the compensation distance after a finger lifts the screen.
     *
     * @param value Indicates the compensation distance to set. The default value is <b>0</b>.
     * @since 1.0
     * @version 1.0
     */
    void SetSwipeACCLevel(uint8_t value)
    {
        swipeAccCoefficient_ = value;
    }

    /**
     * @brief Obtains the compensation distance after a finger lifts the screen.
     *
     * @return Returns the compensation distance.
     * @since 1.0
     * @version 1.0
     */
    uint8_t GetSwipeACCLevel() const
    {
        return swipeAccCoefficient_;
    }

#if ENABLE_ROTATE_INPUT
    /**
     * @brief Obtains the rotation factor.
     *
     * @return Returns the rotation factor.
     * @since 5.0
     * @version 3.0
     */
    float GetRotateFactor() const
    {
        return rotateFactor_;
    }

    /**
     * @brief Sets the rotation factor.
     *
     * @param factor Indicates the rotation factor to set.
     * @since 5.0
     * @version 3.0
     */
    void SetRotateFactor(float factor)
    {
        if (MATH_ABS(factor) > MAX_ROTATE_FACTOR) {
            rotateFactor_ = (factor > 0) ? MAX_ROTATE_FACTOR : -MAX_ROTATE_FACTOR;
            return;
        }
        rotateFactor_ = factor;
    }

    /**
     * @brief 设置触发惯性滑动的组件大小比例阈值.
     *
     * @param threshold 设置触发惯性滑动的比例阈值.
     * @since 6
     */
    void SetRotateThreshold(uint8_t threshold)
    {
        if (threshold == 0) {
            return;
        }
        threshold_ = threshold;
    }

    bool OnRotateEvent(const RotateEvent& event) override;

    bool OnRotateEndEvent(const RotateEvent& event) override;
#endif

    void SetXScrollBarVisible(bool visible);

    void SetYScrollBarVisible(bool visible);

    void SetScrollBarSide(uint8_t side)
    {
        scrollBarSide_ = side;
    }

    void SetScrollBarCenter(const Point& center)
    {
        scrollBarCenter_ = center;
        scrollBarCenterSetFlag_ = true;
    }

    /**
     * @brief Sets the list direction.
     *
     * @param direction Indicates the list direction, either {@link HORIZONTAL} or {@link VERTICAL}.
     * @since 1.0
     * @version 1.0
     */
    void SetDirection(uint8_t direction)
    {
        direction_ = direction;
    }

    /**
     * @brief Obtains the list direction.
     * @return Returns the list direction, either {@link HORIZONTAL} or {@link VERTICAL}.
     * @since 1.0
     * @version 1.0
     */
    uint8_t GetDirection() const
    {
        return direction_;
    }

    void OnPostDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea) override;

    /**
     * @brief Get state of scroll animator.
     * @since 1.0
     * @version 1.0
     */
    uint8_t GetScrollState()
    {
        return scrollAnimator_.GetState();
    }

    static constexpr uint8_t HORIZONTAL = 0;
    static constexpr uint8_t VERTICAL = 1;

protected:
    static constexpr uint8_t HORIZONTAL_AND_VERTICAL = 2;
    static constexpr uint8_t HORIZONTAL_NOR_VERTICAL = 3;
    /* calculate drag throw distance, last drag distance in one tick * DRAG_DISTANCE_COEFFICIENT */
    static constexpr uint8_t DRAG_DISTANCE_COEFFICIENT = 5;
    /* calculate drag throw times, drag distance / DRAG_TIMES_COEFFICIENT */
    static constexpr uint8_t DRAG_TIMES_COEFFICIENT = 18;
    /* the minimum duration of the swipe animator */
    static constexpr uint8_t MIN_DRAG_TIMES = 5;
    /* acceleration calculation coefficient */
    static constexpr uint8_t DRAG_ACC_FACTOR = 10;
    /* the maximum number of historical drag data */
    static constexpr uint8_t MAX_DELTA_SIZE = 3;

    static constexpr uint16_t SCROLL_BAR_WIDTH = 4;
    static constexpr uint8_t MAX_ROTATE_FACTOR = 128;

    class ListAnimatorCallback : public AnimatorCallback {
    public:
        ListAnimatorCallback()
            : curtTime_(0),
              dragTimes_(0),
              startValueX_(0),
              endValueX_(0),
              previousValueX_(0),
              startValueY_(0),
              endValueY_(0),
              previousValueY_(0)
        {
        }

        virtual ~ListAnimatorCallback() {}

        void SetDragTimes(uint16_t times)
        {
            dragTimes_ = times;
        }

        void SetDragStartValue(int16_t startValueX, int16_t startValueY)
        {
            startValueX_ = startValueX;
            previousValueX_ = startValueX;
            startValueY_ = startValueY;
            previousValueY_ = startValueY;
        }

        void SetDragEndValue(int16_t endValueX, int16_t endValueY)
        {
            endValueX_ = endValueX;
            endValueY_ = endValueY;
        }

        void ResetCallback()
        {
            curtTime_ = 0;
            dragTimes_ = 0;
            startValueX_ = 0;
            endValueX_ = 0;
            startValueY_ = 0;
            endValueY_ = 0;
        }

        virtual void Callback(UIView* view) override;

        uint16_t curtTime_;
        uint16_t dragTimes_;
        int16_t startValueX_;
        int16_t endValueX_;
        int16_t previousValueX_;
        int16_t startValueY_;
        int16_t endValueY_;
        int16_t previousValueY_;
    };

    bool DragThrowAnimator(Point currentPos,
                           Point lastPos,
                           uint8_t dragDirection,
                           bool dragBack = true);
    bool ElasticAnimator(uint16_t times);

    virtual void StopAnimator();

    virtual bool DragXInner(int16_t distance) = 0;

    virtual bool DragYInner(int16_t distance) = 0;

    void RefreshDelta(int16_t distance)
    {
        lastDelta_[deltaIndex_ % MAX_DELTA_SIZE] = distance;
        deltaIndex_++;
    }

    void CalculateDragDistance(Point currentPos,
                               Point lastPos,
                               uint8_t dragDirection,
                               int16_t& dragDistanceX,
                               int16_t& dragDistanceY);

    void StartAnimator(int16_t dragDistanceX, int16_t dragDistanceY);

    virtual void CalculateReboundDistance(int16_t& dragDistanceX, int16_t& dragDistanceY) {};

    int16_t GetMaxDelta() const;

    void RefreshAnimator();

    virtual void FixDistance(int16_t& distanceX, int16_t& distanceY) {}
    int16_t CalculateElasticDragLen(int16_t maxLen, int16_t currLen, int16_t dragLen);

    uint16_t scrollBlankSize_ = 0;
    uint16_t scrollBlankSizeTop_ = 0;
    uint16_t scrollBlankSizeBottom_ = 0;
    uint16_t scrollBlankSizeLeft_ = 0;
    uint16_t scrollBlankSizeRight_ = 0;
    uint16_t reboundSizeTop_ = 0;
    uint16_t reboundSizeBottom_ = 0;
    uint16_t reboundSizeLeft_ = 0;
    uint16_t reboundSizeRight_ = 0;
    uint16_t reboundSize_ = 0;
    int16_t accumulatedSize_ = 0;
    uint16_t elasticTime_ = 200;
    uint16_t maxScrollDistance_ = 0;
    int16_t lastDelta_[MAX_DELTA_SIZE] = {0};
    uint8_t dragAccCoefficient_ = DRAG_ACC_FACTOR;
    uint8_t swipeAccCoefficient_ = 0;
    uint8_t direction_ : 2;
    uint8_t deltaIndex_ : 2;
    uint8_t reserve_ : 4;
    bool throwDrag_ = false;
    bool isElastic_ = false;
    EasingFunc easingFunc_;
    ListAnimatorCallback animatorCallback_;
    Animator scrollAnimator_;
#if ENABLE_ROTATE_INPUT
    float rotateFactor_;
    int16_t threshold_;
    int16_t lastRotateLen_;
#endif
    bool yScrollBarVisible_ = false;
    UIAbstractScrollBar* yScrollBar_ = nullptr;
    bool xScrollBarVisible_ = false;
    UIAbstractScrollBar* xScrollBar_ = nullptr;
    uint8_t scrollBarSide_;
    Point scrollBarCenter_;
    bool scrollBarCenterSetFlag_;
    friend class BarEaseInOutAnimator;
    BarEaseInOutAnimator* barEaseInOutAnimator_ = nullptr;
};
} // namespace OHOS
#endif // GRAPHIC_LITE_UI_ABSTRACT_SCROLL_H
/**
 * @}
 */

/*
 * Copyright (c) CompanyNameMagicTag 2024. All rights reserved.
 * Description: UISlipFlow
 * Author:
 * Create: 2024-08
 */

#ifndef UI_SLIPFLOW_VIEW_H
#define UI_SLIPFLOW_VIEW_H
#include <string>
#include <map>
#include "gfx_utils/vector.h"
#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "components/ui_abstract_scroll.h"

namespace OHOS {
struct TempXoffsetInfo {
    int16_t tempMasterXoffset;
    int16_t tempSlaveXoffset;
    int16_t tempMasterYoffset;
    int16_t tempSlaveYoffset;
};

class UISlipflowView : public UIAbstractScroll {
public:
    explicit UISlipflowView();
    ~UISlipflowView() override;

    /**
     * @brief Adds a child view. All child views should have the same width and height.
     *
     * @param view Indicates the pointer to the child view to add.
     * @since 1.0
     * @version 1.0
     */
    void Add(UIView* view) override;

    /**
     * @brief remove a child view from slipflow.
     *
     * @param id Indicates the pointer to the child view to remove.
     */
    void Remove(UIView* view) override;

    /**
     * @brief Get page width form width of the child view.
     */
    uint16_t GetPageWidth() const
    {
        return pageWidth_;
    }

    /**
     * @brief Get page height form height of the child view.
     */
    uint16_t GetPageHeight() const
    {
        return pageHeight_;
    }

    /**
     * @brief Sets the index for the current tab.
     *
     * @param index Indicates the index of a view.
     * @param needAnimator Specifies whether a flip animation is needed. <b>false</b> (default value) indicates a flip
     * animation is not needed, and <b>true</b> indicates the opposite case.
     * @since 1.0
     * @version 1.0
     */
    void SetCurrentPage(uint16_t index, bool needAnimator = false);

    /**
     * @brief Obtains the current tab index.
     *
     * @return Returns the current tab index.
     * @since 1.0
     * @version 1.0
     */
    uint16_t GetCurrentPage() const
    {
        return curIndex_;
    }

    /**
     * @brief Obtains the current view.
     *
     * @return Returns the current view.
     * @since 1.0
     * @version 1.0
     */
    UIView* GetCurrentView() const;

    /**
     * @brief Obtains the component type.
     *
     * @return Returns the component type, as defined in {@link UIViewType}.
     * @since 1.0
     * @version 1.0
     */
    UIViewType GetViewType() const override
    {
        return UI_SLIP_FLOW;
    }

    /**
     * @brief Obtains a view based on its index.
     *
     * @param Indicates the index of a view.
     * @return Returns the view.
     * @since 1.0
     * @version 1.0
     */
    UIView* GetViewByIndex(uint16_t index);

    /**
     * @brief Indicates that the animation duration is 12 ticks.
     *
     * @since 1.0
     * @version 1.0
     */
    constexpr static uint16_t ANIMATOR_TIME = 12;

    /**
     * @brief Sets the time for the page being animated. The page will go beyond the blank during this time.
     *
     * @param time Indicates the time of the page being animated.
     * @since 1.0
     * @version 1.0
     */
    void SetAnimatorTime(uint16_t time);

    /**
     * @brief Set the movement coefficient of the current page and the page on the right compared to other pages.
     *
     * @param coefficient The moving speed of the current page and the page on the right is coefficient times
     * that of other pages.
     * @since 1.0
     * @version 1.0
     */
    void SetMoveCoefficient(uint16_t coefficient);

    /**
     * @fn void OnDragEvent(const DragEvent& event) override
     *
     * @brief realize drag event, Switch to specified view when drag
     *
     * @param event The drag event
     */
    bool OnDragEvent(const DragEvent& event) override;

    /**
     * @fn void OnDragStartEvent(const DragEvent& event) override
     *
     * @brief realize drag start event, Switch to specified view when drag start
     *
     * @param event The drag event start
     */

    bool OnDragStartEvent(const DragEvent& event) override;

    /**
     * @fn void OnDragEndEvent(const DragEvent& event) override
     *
     * @brief realize drag end event, Switch to specified view when drag end
     *
     * @param event The drag event end
     */
    bool OnDragEndEvent(const DragEvent& event) override;

    /**
     * @brief Represents a listener for swipe of the slipflow view.
     */
    class OnSlipflowScrollListener : public HeapBase {
    public:
        virtual bool OnScroll(int16_t distance) { return true; }
        virtual void OnScrollUpStart() {};
        virtual void OnScrollUpEnd() {};
        virtual bool OnRemove(UIView* view) { return true; }
        virtual void OnRefreshPageOpaScale(UIView* view, int16_t xOffset, int16_t yOffset) {}
        virtual ~OnSlipflowScrollListener() {}
    };

    /**
     * @brief Sets the listener that contains a callback to be invoked upon a swipe event.
     *
     * @param onSlipflowListener Indicates the listener to set.
     * @since 1.0
     * @version 1.0
     */
    void SetOnSlipflowListener(OnSlipflowScrollListener* onSlipflowListener)
    {
        slipflowListener_ = onSlipflowListener;
    }

    void SetDirection(uint8_t direction) = delete;
    uint8_t GetDirection() const = delete;
protected:
    void SortChild();
    bool DragXInner(int16_t distance) override;
    bool DragYInner(int16_t distance) override;
    void StopAnimator() override;
    void RefreshCurrentView(int16_t distance, uint8_t dragDirection);
    void SwitchToPage(int16_t dst, bool needAnimator = true);
    void MoveChildByXOffset(int16_t xOffset);
    void DeleteMoveChild(int16_t distance);
    void SwipUpReleaseOperation();
    void SwipUpDeleteOperation();
    void SwipUpRebound();
    void CalculateInvalidate();

private:
    UIView* GetViewByIndexInner(uint16_t index);
    void RemoveInner(UIView* view);
    void DeleteGroup(UIViewGroup* group);
    void AddTempXoffsetInfo();
    void ResetTempOffsetInfo();
    bool CalCurViewInfo();
    void RefreshChildIndex();
    void RefreshScreen();
    void RefreshPageOpaScale(UIView* view);
    int16_t GetPreViewOffsetToCenter();

    OnSlipflowScrollListener* slipflowListener_;
    Graphic::Vector<TempXoffsetInfo> pageTempOffsetInfo_;
    int16_t totalTempXoffset_{0};
    int16_t totalTempYoffset_{0};
    int16_t xMoveCoefficient_;
    int16_t yMoveCoefficient_;
    bool isSwipUpDelete_{false};

    uint16_t tickTime_;
    uint16_t curIndex_;
    UIView* curView_;
    UIView* deleteView_;
    uint16_t pageWidth_;
    uint16_t pageHeight_;
    uint16_t swipeMid_;
    int8_t dragDirection_;
    bool isSetPage_;
};
} // namespace OHOS
#endif // UI_SLIPFLOW_VIEW_H

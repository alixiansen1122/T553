/*
 * Copyright (c) CompanyNameMagicTag 2025. All rights reserved.
 * Description: UIFoldableView
 * Author:
 * Create: 2025-09
 */

#ifndef UI_FOLDABLE_VIEW_H
#define UI_FOLDABLE_VIEW_H

#include "components/ui_abstract_scroll.h"

namespace OHOS {
class UIFoldableView : public UIAbstractScroll {
public:
    explicit UIFoldableView();
    ~UIFoldableView() override;

    /**
     * @brief Adds a child view.
     *
     * @param view Indicates the pointer to the child view to add.
     *
     * @attention Only support Add UITransformGroup type.
     */
    void Add(UIView* view) override;

    /**
     * @brief Inserts a new child view behind the current one.
     *
     * @param prevView Indicates the pointer to the current child view, previous to the new child view to insert.
     * @param insertView Indicates the pointer to the new child view to insert.
     *
     * @attention Only support Insert UITransformGroup type.
     */
    void Insert(UIView* prevView, UIView* insertView) override;

    /**
     * @brief remove a child view from foldable.
     *
     * @param view Indicates the pointer to the child view to remove.
     */
    void Remove(UIView* view) override;

    /**
     * @brief Removes all child views.
     */
    void RemoveAll() override;

    /**
     * @brief Lays out all child views according to the preset arrangement mode.
     * @param needInvalidate Specifies whether to refresh the invalidated area after the layout is complete.
     *                       Value <b>true</b> means to refresh the invalidated area after the layout is complete,
     *                       and <b>false</b> means the opposite.
     */
    void LayoutChildren(bool needInvalidate = false) override;

    /**
     * @brief Move a child view behind the another one.
     *
     * @param prevView Indicates the pointer to the target child view, previous to the child view to move.
     * @param moveView Indicates the pointer to the child view to move.
     */
    void MoveChild(UIView* prevView, UIView* moveView);

    /**
     * @brief Set max page num.
     *
     * @param maxPageNum The max page num.
     */
    void SetMaxPageNum(uint8_t maxPageNum);

    /**
     * @brief Set page size.
     *
     * @param width The page width.
     * @param height The page height.
     */
    void SetPageSize(uint16_t width, uint16_t height);

    /**
     * @brief Get the page width.
     *
     * @return Returns the page width
     */
    uint16_t GetPageWidth() const
    {
        return pageWidth_;
    }

    /**
     * @brief Get the page height.
     *
     * @return Returns the page height
     */
    uint16_t GetPageHeight() const
    {
        return pageHeight_;
    }

    /**
     * @brief Set the gradient of the scaling coefficient decreases from the lower page to the upper page in fold mode
     *
     * @param scaleGradient The gradient of the scaling coefficient decreases from the lower page to the upper page
     */
    void SetFoldScaleGradient(float scaleGradient);

    /**
     * @brief Set the opacity change for the folded page in fold mode.
     *
     * @param expandPageOpacity The opacity of expand pages
     * @param minFoldPageOpacity The min opacity of fold pages
     * @param opacityGradient The gradient of the opacity decreases from the upper page to the lower page
     */
    void SetFoldOpacityChange(uint8_t expandPageOpacity, uint8_t minFoldPageOpacity, uint8_t opacityGradient);

    /**
     * @brief Set the blank space between expanded pages(applicable to fold and flat mode)
     *
     * @param margin The blank space between expanded pages
     */
    void SetExpandPagesMargin(uint16_t margin);

    /**
     * @brief Set the blank space between the top of the container and the top page in fold mode
     *
     * @param margin The blank space between the top of the container and the top page
     */
    void SetTopMarginInFoldMode(uint16_t margin);

    /**
     * @brief Set the blank space between the top of the container and the top page in flat mode
     *
     * @param margin The blank space between the top of the container and the top page
     */
    void SetTopMarginInFlatMode(uint16_t margin);

    /**
     * @brief Set the drag offset for the Y-axis in fold mode
     *
     * @param offset The drag offset for the Y-axis
     */
    void SetDragYOffInFoldMode(int16_t offset);

    /**
     * @brief Set the drag offset for the Y-axis in flat mode
     *
     * @param offset The drag offset for the Y-axis
     */
    void SetDragYOffInFlatMode(int16_t offset);

    /**
     * @brief Get the drag offset for the Y-axis in fold mode
     *
     * @return Returns the drag offset for the Y-axis
     */
    int16_t GetDragYOffInFoldMode()
    {
        return dragYOffFold_;
    }

    /**
     * @brief Get the drag offset for the Y-axis in flat mode
     *
     * @return Returns the drag offset for the Y-axis
     */
    int16_t GetDragYOffInFlatMode()
    {
        return dragYOffFlat_;
    }

    /**
     * @brief Get the fully expand drag offset for the Y-axis
     *
     * @return Returns the fully expand drag offset for the Y-axis
     */
    int16_t GetDragToEndYOff()
    {
        return dragToEndYOff_;
    }

    /**
     * @brief Get the Y-coordinate of the top page
     *
     * @return Returns the Y-coordinate of the top page
     */
    int16_t GetTopPageY()
    {
        if (childrenTail_ == nullptr) {
            return 0;
        }
        return childrenTail_->GetY();
    }

    /**
     * @brief Get the drag offset for the Y-axis
     *
     * @param currentPos The current point
     * @param offset The last point
     *
     * @return Returns the drag throw distance
     */
    int16_t CalculateDragThrowYDistance(Point currentPos, Point lastPos);

    /**
     * @brief realize drag start event, Switch to specified view when drag start
     *
     * @param event The drag event start
     * @return Returns <b>true</b> if the event is consumed; returns <b>false</b> otherwise.
     *         (If an event is consumed, it is not transferred to the parent view. If an event is not consumed,
     *         it is transferred to the parent view after local processing is complete.)
     *
     * @attention OnDragStartEvent and OnDragEndEvent must be used in pairs.
     */
    bool OnDragStartEvent(const DragEvent& event) override;

    /**
     * @brief realize drag event, Switch to specified view when drag
     *
     * @param event The drag event
     * @return Returns <b>true</b> if the event is consumed; returns <b>false</b> otherwise.
     *         (If an event is consumed, it is not transferred to the parent view. If an event is not consumed,
     *         it is transferred to the parent view after local processing is complete.)
     *
     * @attention Can only be called after OnDragStartEvent and before OnDragEndEvent.
     */
    bool OnDragEvent(const DragEvent& event) override;

    /**
     * @brief realize drag end event, Switch to specified view when drag end
     *
     * @param event The drag event end
     * @return Returns <b>true</b> if the event is consumed; returns <b>false</b> otherwise.
     *         (If an event is consumed, it is not transferred to the parent view. If an event is not consumed,
     *         it is transferred to the parent view after local processing is complete.)
     *
     * @attention OnDragStartEvent and OnDragEndEvent must be used in pairs.
     */
    bool OnDragEndEvent(const DragEvent& event) override;

    /**
     * @brief Sliding in gradually from the bottom or top
     *        if dragYOff less than dragToEndYOff, slid for top end of container to drag to end state;
     *        if dragYOff greater than 0, slid for bottom end of container to initial state;
     * @return Returns <b>true</b> if the animotor start success; returns <b>false</b> otherwise
     */
    bool StartEntranceAnimator();

    /**
     * @brief Slide distance gradually from cur postion
     *
     * @param distance Indicates the distance to slide
     * @return Returns <b>true</b> if the animotor start success; returns <b>false</b> otherwise
     */
    bool StartSlideAnimator(int16_t distance);

    /**
     * @brief Slide gradually from cur position to where target view is layout at the initial top position
     *
     * @param focusView Indicates the target view to layout at the initial top position
     * @return Returns <b>true</b> if the animotor start success; returns <b>false</b> otherwise
     */
    bool StartSlideAnimator(UIView *focusView);

    /**
     * @brief Move a child view behind the another one with animation.
     *
     * @param prevView Indicates the pointer to the target child view, previous to the child view to move.
     * @param moveView Indicates the pointer to the child view to move.
     */
    bool StartMoveChildAnimator(UIView* prevView, UIView* moveView, uint32_t time = EDIT_ANIM_TIME);

    /**
     * @brief remove a child view with animation.
     *
     * @param view Indicates the pointer to the child view to remove.
     */
    bool StartRemoveChildAnimator(UIView* view, uint32_t time = EDIT_ANIM_TIME);

    enum class LayoutMode {
        LAYOUT_MODE_FOLD,
        LAYOUT_MODE_FLAT,
        LAYOUT_MODE_MAX,
    };

    enum class SwitchAction {
        SWITCH_ADDTAIL_DELHEAD,
        SWITCH_ADDHEAD_DELTAIL,
        SWITCH_ACTION_MAX,
    };

    /**
     * @brief Switch layout mode of foldable view with animation.
     *
     * @param mode Indicates the layout mode of foldable view(fold or flat).
     */
    bool StartSwitchLayoutAnimator(LayoutMode mode, uint32_t time = EDIT_ANIM_TIME);

    /**
     * @brief Switch layout mode of foldable view with animation.
     *
     * @param mode Indicates the layout mode of foldable view(fold or flat).
     * @param action Indicates the additional action required to be performed for this switch.
     * @param view Indicates the object to be operate in additional action, it can be null if no additional action.
     */
    bool StartSwitchLayoutAnimator(LayoutMode mode, SwitchAction action, UIView *view, uint32_t time = EDIT_ANIM_TIME);

    /**
     * @brief Force stop running animator.
     */
    void ForceStopAnimator();

    /**
     * @brief Represents a listener for scroll of the foldable view.
     */
    class OnFoldableViewEventListener : public HeapBase {
    public:
        virtual ~OnFoldableViewEventListener() {};

        /**
        * @brief Notify the holder that the foldable view has been relayout in fold mode.
        */
        virtual void OnReLayoutInFoldMode() {};

        /**
        * @brief Notify the holder that the foldable view has been relayout in flat mode.
        */
        virtual void OnReLayoutInFlatMode() {};

        /**
        * @brief Notify the holder that the entrance animator stoped.
        */
        virtual void OnEntranceAnimatorStop() {};

        /**
        * @brief Notify the holder that the slide animator stoped.
        */
        virtual void OnSlideAnimatorStop() {};

        /**
        * @brief Notify the holder that the move child animator stoped.
        */
        virtual void OnMoveChildAnimatorStop() {};

        /**
        * @brief Notify the holder that the remove child animator stoped.
        */
        virtual void OnRemoveChildAnimatorStop() {};

        /**
        * @brief Notify the holder of the progress of the layout switch.
        * @param progress Indicates the progress(0 ~ 1) of the layout switch.
        */
        virtual void OnSwitchingLayout(LayoutMode targetMode, float progress) {};

        /**
        * @brief Notify the holder that the remove child animator stoped.
        */
        virtual void OnSwitchLayoutAnimatorStop() {};
    };

    /**
     * @brief Sets the listener that contains a callback to be invoked upon a scroll event.
     *
     * @param listener Indicates the listener to set.
     */
    void SetFoldableScrollListener(OnFoldableViewEventListener* listener)
    {
        eventListener_ = listener;
    }

    /**
     * @brief Switch layout mode of foldable view.
     *
     * @param mode Indicates the layout mode of foldable view(fold or flat).
     */
    void SwitchLayoutMode(LayoutMode mode);

    /**
     * @brief Get layout mode of foldable view.
     *
     * @return Returns the layout mode of foldable view
     */
    LayoutMode GetLayoutMode()
    {
        return layoutMode_;
    }

    /**
     * @brief Obtains the component type.
     *
     * @return Returns the component type, as defined in {@link UIViewType}.
     */
    UIViewType GetViewType() const override
    {
        return UI_FOLDABLE_VIEW;
    }

    void SetDirection(uint8_t direction) = delete;
    uint8_t GetDirection() const = delete;
    void SetAutoSize(bool state) = delete;
    UIView* GetChildrenTail() const = delete;
    UIView* GetChildrenHead() const = delete;

protected:
    void LayoutChildPages(bool firstLayout);
    bool DragYInner(int16_t distance) override;
    bool DragXInner(int16_t distance) override;
    void StopAnimator() override;
    int16_t CalculateDragThrowYDistanceInner(Point currentPos, Point lastPos);
    int16_t CalculateEntranceAnimationDistance();

    enum class EditAnimType {
        EDIT_ANIM_TYPE_ADD,
        EDIT_ANIM_TYPE_REMOVE,
        EDIT_ANIM_TYPE_MOVE,
        EDIT_ANIM_TYPE_SWITCH,
        EDIT_ANIM_TYPE_MAX,
    };

    enum class MoveDirection {
        MOVE_DIR_BOTTOM_TO_TOP,
        MOVE_DIR_TOP_TO_BOTTOM,
        MOVE_DIR_MAX,
    };

    class EditAnimatorCallback : public AnimatorCallback {
    public:
        EditAnimatorCallback()
            : prevView_(nullptr)
        {
        }

        virtual ~EditAnimatorCallback() {}

        void SetAnimTime(uint32_t animTime)
        {
            animTime_ = animTime;
        }

        void PrepareMoveChild(UIView* prevView, UIView* moveView,
            int16_t distance, float passiveMoveCoe, MoveDirection direction)
        {
            animType_ = EditAnimType::EDIT_ANIM_TYPE_MOVE;
            prevView_ = prevView;
            moveView_ = moveView;
            totalMoveDistance_ = distance;
            passiveMoveCoe_ = passiveMoveCoe;
            moveDirection_ = direction;
            curMoveDistance_ = 0;
        }

        void PrepareRemoveChild(UIView* view, int16_t distance)
        {
            animType_ = EditAnimType::EDIT_ANIM_TYPE_REMOVE;
            removeView_ = view;
            totalMoveDistance_ = distance;
            curMoveDistance_ = 0;
        }

        void PrepareSwitchLayout(LayoutMode mode, int16_t startDragY, SwitchAction action, UIView *view)
        {
            animType_ = EditAnimType::EDIT_ANIM_TYPE_SWITCH;
            layoutMode_ = mode;
            startDragY_ = startDragY;
            switchAction_ = action;
            operateView_ = view;
        }

        virtual void Callback(UIView* view) override;
        virtual void OnStop(UIView& view) override;
        void MoveChildGradually(UIView* view, uint32_t elapseTime);
        void RemoveChildGradually(UIView* view, uint32_t elapseTime);
        void SwitchLayoutModeGradually(UIView* view, uint32_t elapseTime);

        uint32_t animTime_ = EDIT_ANIM_TIME;
        uint32_t startTime_ = 0;
        EditAnimType animType_ = EditAnimType::EDIT_ANIM_TYPE_MAX;
        MoveDirection moveDirection_ = MoveDirection::MOVE_DIR_MAX;
        int16_t totalMoveDistance_ = 0;
        int16_t curMoveDistance_ = 0;
        float passiveMoveCoe_ = 0;
        UIView *addView_ = nullptr;
        UIView *removeView_ = nullptr;
        UIView *prevView_ = nullptr;
        UIView *insertView_ = nullptr;
        UIView *moveView_ = nullptr;
        LayoutMode layoutMode_ = LayoutMode::LAYOUT_MODE_MAX;
        int16_t startDragY_ = 0;
        SwitchAction switchAction_ = SwitchAction::SWITCH_ACTION_MAX;
        UIView *operateView_ = nullptr;
    };
    /* swipe acceleration calculation coefficient */
    static constexpr uint8_t SWIPE_ACC_FACTOR = 10;
    EditAnimatorCallback editAnimCallback_;
    Animator editAnimator_;

private:
    bool CheckRemoveChild(UIView* view);
    void RemoveInner(UIView* view);
    bool CheckMoveChild(UIView* prevView, UIView* moveView);
    void MoveChildInner(UIView* prevView, UIView* moveView);
    void MoveChildInFlatMode(UIView* prevView, UIView* moveView,
        int16_t distance, float passiveMoveCoe, MoveDirection direction);
    void MoveChildInFlatMode(UIView* view, int16_t distance);
    void MoveChildInFoldMode(UIView* view, int16_t distance);
    void SwithFlatToFoldLayout(int16_t dragY, int16_t topMar, int16_t distance, bool removeTail);
    void SwithFoldToFlatLayout(int16_t dragY, int16_t topMar);
    bool CheckDragLimit(int16_t distance);
    void CalculateTotalYoffset(int16_t distance);
    void LayoutOverFoldPages(int16_t dragY, int16_t topMar, UIView *headPage, UIView *tailPage);
    void LayoutFoldPageByExpandPage(UIView *headPage, UIView *tailPage, UIView *expandPage, int16_t dragY);
    void RecursionLayoutFoldPages(UIView *prePage, UIView *tailPage, float &scale, uint8_t &opacity);
    void RecursionLayoutExpandPages(UIView *prePage, UIView *tailPage);
    void LayoutChildPagesByFold(int16_t dragY, int16_t topMar, UIView *headPage, UIView *tailPage, uint16_t pageNum);
    void LayoutChildPagesByFlat(int16_t headPageY, UIView *headPage, UIView *tailPage);
    UIViewGroup *PackInputView(UIView* view);

    OnFoldableViewEventListener* eventListener_;
    uint8_t maxPageNum_;
    uint16_t pageWidth_;
    uint16_t pageHeight_;
    float scaleGradient_ = 0.0;
    uint8_t opacityGradient_ = 0;
    uint8_t expandPageOpacity_ = 0;
    uint8_t minFoldPageOpacity_ = 0;
    uint16_t foldPageMargin_ = 0;
    uint16_t foldTopMargin_ = 0;
    uint16_t flatTopMargin_ = 0;
    int16_t dragYOffFold_ = 0;
    int16_t dragYOffFlat_ = 0;
    int16_t dragToEndYOff_ = 0;
    int16_t startDragYoffset_ = 0;
    int8_t startDragDirection_ = 0;
    float pitchCoe_ = 0.0;
    uint16_t pitchExpandPage_ = 0;
    bool entranceSliding_ = false;
    bool wilfullySliding_ = false;
    bool disableCurDrag_ = false;
    LayoutMode layoutMode_ = LayoutMode::LAYOUT_MODE_FOLD;
    static constexpr int16_t EDIT_ANIM_TIME = 300;
};
} // namespace OHOS
#endif // UI_FOLDABLE_VIEW_H

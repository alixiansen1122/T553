/*
 * Copyright (c) CompanyNameMagicTag 2024. All rights reserved.
 * Description: VerSwipeView
 * Author:
 * Create: 2024-11
 */

#ifndef UI_VER_SWIPE_VIEW_H
#define UI_VER_SWIPE_VIEW_H
#include "components/ui_swipe_view.h"

namespace OHOS {
class VerSwipeView : public UISwipeView {
public:
    /**
     * @brief Represents a listener for moving children.
     *
     * This is an inner class of <b>VerSwipeView</b>.
     * It contains a callback function to be invoked when the swipe view moves children.
     *
     * @since 1.0
     * @version 1.0
     */
    class OnMoveChildListener : public HeapBase {
    public:
        OnMoveChildListener() {}
        virtual ~OnMoveChildListener() {}
        virtual void OnMoveChild(UISwipeView& view, int16_t xOffset, int16_t yOffset) = 0;
    };

    /* *
     * @brief A constructor used to create the <b>VerSwipeView</b> instance.
     * @since 1.0
     * @version 1.0
     */
    VerSwipeView() {}

    /* *
     * @brief A destructor used to delete the <b>VerSwipeView</b> instance.
     * @since 1.0
     * @version 1.0
     */
    ~VerSwipeView() override {}

    /**
     * @brief Obtains the listener set for moving child event.
     *
     * @return Returns the move child listener.
     * @since 1.0
     * @version 1.0
     */
    OnMoveChildListener*& GetOnMoveChildListener()
    {
        return onMoveChildListener_;
    }

    /**
     * @brief Sets the listener that contains a callback to be invoked upon moving child.
     *
     * @param onMoveChildListener Indicates the listener to set.
     * @since 1.0
     * @version 1.0
     */
    void SetOnMoveChildListener(OnMoveChildListener* onMoveChildListener)
    {
        onMoveChildListener_ = onMoveChildListener;
    }

protected:
    void MoveChildByOffset(int16_t xOffset, int16_t yOffset) override
    {
        UISwipeView::MoveChildByOffset(xOffset, yOffset);
        if (onMoveChildListener_ != nullptr) {
            onMoveChildListener_->OnMoveChild(*this, xOffset, yOffset);
        }
    }

private:
    OnMoveChildListener* onMoveChildListener_ = nullptr;
};
}
#endif

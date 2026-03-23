/*
 * Copyright (c) CompanyNameMagicTag 2024-2024. All rights reserved.
 * Description: BackDragListener
 * Author:
 * Create: 2024-11-13
 */

#ifndef BACK_DRAG_LISTENER_H
#define BACK_DRAG_LISTENER_H
#include <functional>
#include "animator/animator.h"
#include "components/ui_view_group.h"

namespace OHOS {
class BackDragListener : public UIView::OnDragListener {
public:
    static int16_t maxDragRegionX_;
    BackDragListener() = default;
    ~BackDragListener() override;
    bool OnDragStart(UIView& view, const DragEvent& event) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnDragEnd(UIView& view, const DragEvent& event) override;
    void ClearDragState();
    void SetParentView(UIViewGroup* view)
    {
        parentView_ = view;
    }
    void SetCurrentAndTargetView(UIView *cur, UIView *tar)
    {
        curView_ = cur;
        targetView_ = tar;
    }

    void SetDragToTargetAction(const std::function<void()> &action)
    {
        targetAction_ = action;
    }

    void SetDragToCurrentAction(const std::function<void()> &action)
    {
        curAction_ = action;
    }

    void SetAnimatorTime(uint16_t time)
    {
        time_ = time;
    }
    BackDragListener(const BackDragListener &) = delete;
    BackDragListener &operator=(const BackDragListener &) = delete;
    BackDragListener(BackDragListener &&) = delete;
    BackDragListener &operator=(BackDragListener &&) = delete;

private:
    class BackAnimatorCallback : public AnimatorCallback {
    public:
        BackAnimatorCallback(BackDragListener* listener, uint16_t duration)
            : dragListener_(listener), curtTime_(0), duration_(duration) {}
        void Callback(UIView* view) override;
        void OnStop(UIView& view) override
        {
            curtTime_ = 0;
        }

    private:
        BackDragListener* dragListener_;
        uint16_t curtTime_;
        uint16_t duration_;
    };

    bool StartAnimator();
    void StopDragging();
    Animator *backAnimator_ = nullptr;
    BackAnimatorCallback* backCallback_ = nullptr;
    UIView *targetView_ = nullptr;
    UIView *curView_ = nullptr;
    UIViewGroup* parentView_ = nullptr;
    int16_t left_ = 0;
    int16_t right_ = 0;
    std::function<void()> targetAction_;
    std::function<void()> curAction_;
    const static uint16_t DEFAULT_TIME = 200;
    uint16_t time_ = DEFAULT_TIME;
    bool goToTarget_ = false;
    int16_t animStartX_ = 0;
    int16_t animEndX_ = 0;
    bool isDragging_ = false;
    bool isTargetViewAdded_ = false;
}; // BackDragListener

} // namespace OHOS
#endif // BACK_DRAG_LISTENER_H

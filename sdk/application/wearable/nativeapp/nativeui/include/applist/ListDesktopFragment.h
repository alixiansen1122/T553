/*
 * Copyright (c) CompanyNameMagicTag 2025-2025. All rights reserved.
 * Description: ListDesktopFragment.h
 * Author:
 * Create: 2025-07-04
 */

#ifndef LIST_DESKTOP_FRAGMENT_H
#define LIST_DESKTOP_FRAGMENT_H

#include "UIDesktopFragment.h"
#include "ApplistAdapter.h"
#include "ChangeSliceListener.h"
#include "components/ui_list.h"
#include "drag_event.h"

namespace OHOS {
class ListDesktopFragment : public UIDesktopFragment {
public:
    bool OnClick(UIView &view, const ClickEvent &event) override;
    void OnItemSelected(int16 index, UIView *view) override;
    void AddAppItemToList(const AppItem &item) override;
    void ClearAppItemToList() override;
    void RefreshAppList() override;

protected:
    void OnCreateView(void* data) override;
    void OnDestroyView() override;

private:
    ApplistAdapter *listAdapter_{nullptr};
    UIList *contentList_{nullptr};
    UIView::OnDragListener *dragListener_{nullptr};
    AppItemView *itemViewSele{nullptr};
    AppItemView *preItemViewSele{nullptr};
};

class AppListOnDragListener : public UIView::OnDragListener {
public:
    AppListOnDragListener()
    {
        continueDrag_ = false;
    }
    ~AppListOnDragListener() override {}
    bool OnDragStart(UIView& view, const DragEvent& event) override
    {
        UNUSED(view);
        UNUSED(event);
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "Applist Ondragstart!!");
        continueDrag_ = true;
        return false;
    }

    bool OnDrag(UIView& view, const DragEvent& event) override
    {
        UNUSED(view);
        if (!continueDrag_) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AppList : Can not continue drag!!");
            return false;
        }
        if ((event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) &&
            (event.GetDeltaX() > X_DRAG_OFFSET) && (event.GetDeltaX() > (DOUBLES * MATH_ABS(event.GetDeltaY())))) {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "Applist Ondrag!!");
            NativeAbility::GetInstance().ChangeSlice(VIEW_MAIN_SAMPLE, TransitionType::TRANSITION_ZOOM);
        }

        return false;
    }

    bool OnDragEnd(UIView& view, const DragEvent& event) override
    {
        UNUSED(view);
        UNUSED(event);
        return false;
    }
private:
    bool continueDrag_{false};
};
}

#endif // LIST_DESKTOP_FRAGMENT_H

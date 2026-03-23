/*
 * Copyright (c) CompanyNameMagicTag 2025-2025. All rights reserved.
 * Description: ListDesktopFragment.cpp
 * Author:
 * Create: 2025-07-04
 */

#include "applist/ListDesktopFragment.h"
#include "applist/DesktopFactory.h"
#include "applist/ApplistModel.h"

namespace OHOS {
REGIST_DESKTOP_STYLE(APPLIST_STYLE, ListDesktopFragment);

static constexpr int16 NUM_2 = 2;
static constexpr int16 MAX_SCROLL_DIS = 1000;

void ListDesktopFragment::OnCreateView(void* data)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "ListDesktopFragment::OnCreateView");
    listAdapter_ = new ApplistAdapter();
    if (listAdapter_ == nullptr) {
        return;
    }
    listAdapter_->SetItemClickListener(this);

    contentList_ = new UIList(UIList::VERTICAL);
    if (contentList_ == nullptr) {
        return;
    }
    contentList_->SetPosition(0, 0);
    contentList_->Resize(HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    contentList_->SetScrollBlankSize(VERTICAL_RESOLUTION / NUM_2);
    contentList_->SetSelectPosition(VERTICAL_RESOLUTION / NUM_2); // middle position
    contentList_->SetDraggable(true);
    contentList_->SetThrowDrag(true);
    contentList_->EnableAutoAlign(true);
    contentList_->SetScrollStateListener(this);
    contentList_->SetMaxScrollDistance(MAX_SCROLL_DIS);
    contentList_->SetYScrollBarVisible(true);

    dragListener_ = new AppListOnDragListener();
    if (dragListener_ == nullptr) {
        return;
    }
    contentList_->SetOnDragListener(dragListener_);
    fragmentView_.Add(contentList_);
}


void ListDesktopFragment::OnDestroyView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "ListDesktopFragment::OnDestroyView");
    int16_t listHeadY = contentList_->GetChildrenHead()->GetY();
    int16_t listHeadIndex = contentList_->GetChildrenHead()->GetViewIndex();
    ApplistModel *model = &ApplistModel::GetInstance();
    model->SetHeadItemY(listHeadY);
    model->SetStartIndex(listHeadIndex);

    ClearAppItemToList();

    if (listAdapter_ != nullptr) {
        delete listAdapter_;
        listAdapter_ = nullptr;
    }
    if (contentList_ != nullptr) {
        delete contentList_;
        contentList_ = nullptr;
    }
    if (dragListener_ != nullptr) {
        delete dragListener_;
        dragListener_ = nullptr;
    }
}

bool ListDesktopFragment::OnClick(UIView &view, const ClickEvent &event)
{
    UNUSED(event);
    AppItemView *itemView = dynamic_cast<AppItemView *>(&view);
    if (itemView == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "itemView is nullptr");
        return false;
    }
    AppViewId viewId = itemView->GetViewId();
    char *uid = (char *)itemView->GetAppUid();

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "ListDesktopFragment::OnClick viewid = %d", viewId);
    return SwitchView(viewId, uid, TransitionType::TRANSITION_INVALID, APPLIST_STYLE);
}

void ListDesktopFragment::OnItemSelected(int16 index, UIView *view)
{
    UNUSED(index);
    if (view != nullptr) {
        itemViewSele = static_cast<AppItemView *>(view);
        itemViewSele->SelectedChange();

        if (preItemViewSele != nullptr && preItemViewSele != itemViewSele) {
            preItemViewSele->Reset();
        }
        preItemViewSele = itemViewSele;
    }
}

void ListDesktopFragment::AddAppItemToList(const AppItem &item)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "ListDesktopFragment::AddAppItemToList");
    listAdapter_->AddListItem(item);
}

void ListDesktopFragment::ClearAppItemToList()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "ListDesktopFragment::ClearAppItemToList");
    listAdapter_->ClearItem();
}

void ListDesktopFragment::RefreshAppList(void)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "ListDesktopFragment::RefreshAppList");
    uint16_t startIndex = 0;
    int16_t headY = 0;
    ApplistModel *model = &ApplistModel::GetInstance();
    if (model->GetStartIndex() >= model->GetAppListNum()) {
        startIndex = model->GetAppListNum() - 1;
        headY = VERTICAL_RESOLUTION / NUM_2;
    } else {
        startIndex = model->GetStartIndex();
        headY = model->GetHeadItemY();
        if (headY == 0) {
            headY = VERTICAL_RESOLUTION / NUM_2;
        }
    }
    contentList_->SetAdapter(listAdapter_);
    contentList_->ScrollTo(startIndex);
    contentList_->ScrollBy(headY);
    contentList_->RefreshList();
}
}

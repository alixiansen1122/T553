/*
 * Copyright (c) CompanyNameMagicTag 2025-2025. All rights reserved.
 * Description: WaterfallDesktopFragment.cpp
 * Author:
 * Create: 2025-07-04
 */

#include "applist/WaterfallDesktopFragment.h"
#include "applist/DesktopFactory.h"
#include "common/image_cache_manager.h"
#include "common/screen.h"

namespace OHOS {
REGIST_DESKTOP_STYLE(WATERFALL_STYLE, WaterfallDesktopFragment);

void WaterfallDesktopFragment::OnCreateView(void* data)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "WaterfallDesktopFragment::OnCreateView ");
    if (waterfallList_ == nullptr) {
        waterfallList_ = new UIWaterfallList();
    }
    waterfallList_->SetPosition(0, 0, fragmentView_.GetWidth(), fragmentView_.GetHeight());
    imgDistance_ = waterfallList_->GetWidth() * 0.25991; // 0.25991, empirical coefficient
    imgSize_ = waterfallList_->GetWidth() * 0.23348; // 0.23348, empirical coefficient
    waterfallList_->SetImageDistance(imgDistance_);
    waterfallList_->SetImageSizeInCenter(imgSize_);
    int16_t blankSize = waterfallList_->GetRelativeRect().GetWidth() > waterfallList_->GetRelativeRect().GetHeight() ?
        waterfallList_->GetRelativeRect().GetWidth() / 2 : waterfallList_->GetRelativeRect().GetHeight() / 2; // 2, half
    waterfallList_->SetThrowDrag(true);
    uint16_t layoutHeight = static_cast<uint16_t>(imgDistance_ * Sin(60));
    waterfallList_->SetReboundSize(layoutHeight); // 60: degree, row's layout height
    waterfallList_->SetScrollBlankSize(layoutHeight, UIAbstractScroll::Direction::TOP);
    waterfallList_->SetScrollBlankSize(Screen::GetInstance().GetHeight() / 2 - layoutHeight, // 2: half
        UIAbstractScroll::Direction::BOTTOM);
    waterfallList_->RequestFocus();
    waterfallList_->SetRotateFactor(10); // 10, empirical coefficient
    waterfallList_->SetOnRotateListener(this);
    waterfallList_->SetTouchable(false);
    waterfallList_->SetDraggable(false); // ensure that it can't be dragged during animation to avoid error display
    fragmentView_.Add(waterfallList_);
}

void WaterfallDesktopFragment::OnDestroyView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "WaterfallDesktopFragment::OnDestroyView");
    ClearAppItemToList();
    if (waterfallList_ != nullptr) {
        delete waterfallList_;
        waterfallList_ = nullptr;
    }
}

void WaterfallDesktopFragment::OnResumeView()
{
    if (waterfallList_ != nullptr) {
        waterfallList_->SetTouchable(true);
        waterfallList_->SetDraggable(true); // resume touchable after transition animation
    }
}

bool WaterfallDesktopFragment::OnClick(UIView &view, const ClickEvent &event)
{
    UNUSED(event);
    WaterfallItemView *itemView = dynamic_cast<WaterfallItemView *>(&view);
    UIWaterfallList *waterfallView = dynamic_cast<UIWaterfallList *>(itemView->GetParent());
    if (waterfallView == nullptr || itemView == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "waterfallView or itemView is nullptr");
        return false;
    }
    AppViewId viewId = itemView->GetViewId();
    char *uid = (char *)itemView->GetAppUid();
    waterfallView->SetDraggable(false);
    waterfallView->Scale(1.0f, itemView->GetViewIndex());

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "WaterfallDesktopFragment::OnClick viewid = %d", viewId);
    return SwitchView(viewId, uid, TransitionType::TRANSITION_WATERFALL, APPLIST_STYLE);
}

void WaterfallDesktopFragment::AddAppItemToList(const AppItem &item)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "WaterfallDesktopFragment::AddAppItemToList");
    WaterfallItemView *itemView = new WaterfallItemView();
    if (!itemView->SetItemInfo(item)) {
        delete itemView;
        return;
    }
    itemView->SetTouchable(true);
    itemView->SetOnClickListener(this);
    waterfallList_->Add(itemView);
    waterfallItemList_.PushBack(itemView);
}

void WaterfallDesktopFragment::ClearAppItemToList()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "WaterfallDesktopFragment::ClearAppItemToList");
    if (waterfallList_ != nullptr) {
        waterfallList_->RemoveAll();
    }
    ListNode<WaterfallItemView*>* viewNode = waterfallItemList_.Head();
    while (viewNode != waterfallItemList_.End()) {
        delete(viewNode->data_);
        viewNode = viewNode->next_;
    }
    waterfallItemList_.Clear();
}

void WaterfallDesktopFragment::RefreshAppList(void)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "WaterfallDesktopFragment::RefreshAppList");
    waterfallList_->LayoutChildren(true);
}

WaterfallItemView::~WaterfallItemView()
{
    if (itemInfo_.iconSmall == nullptr && itemInfo_.resPath != nullptr) {
        ImageCacheManager::GetInstance().UnloadOneInMultiRes(itemInfo_.resId, itemInfo_.resPath);
    }
}

bool WaterfallItemView::SetItemInfo(const AppItem &itemInfo)
{
    itemInfo_ = itemInfo;
    if (itemInfo.iconSmall == nullptr && itemInfo.resPath != nullptr) {
        ImageInfo* image = ImageCacheManager::GetInstance().LoadOneInMultiRes(itemInfo.resId, itemInfo.resPath);
        if (image == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Load App Icon path %s Image %x fail",
                itemInfo.resId, itemInfo.resPath);
            return false;
        }
        SetSrc(image);
    } else {
        SetSrc(itemInfo.iconSmall);
    }
    return true;
}

AppViewId WaterfallItemView::GetViewId()
{
    return static_cast<AppViewId>(itemInfo_.id);
}

const char *WaterfallItemView::GetAppUid()
{
    return itemInfo_.uid;
}
}

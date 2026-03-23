/*
 * Copyright (c) CompanyNameMagicTag 2025-2025. All rights reserved.
 * Description: HexagonsDesktopFragment.cpp
 * Author:
 * Create: 2025-07-04
 */

#include "applist/HexagonsDesktopFragment.h"
#include "applist/DesktopFactory.h"
#include "common/image_cache_manager.h"
#include "NativeAbility.h"

namespace OHOS {
REGIST_DESKTOP_STYLE(HEXAGONS_STYLE, HexagonsDesktopFragment);

void HexagonsDesktopFragment::OnCreateView(void* data)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "HexagonsDesktopFragment::OnCreateView ");
    if (hexagonsList_ == nullptr) {
        hexagonsList_ = new UICustomHexagonsList();
    }
    hexagonsList_->SetPosition(0, 0, fragmentView_.GetWidth(), fragmentView_.GetHeight()); // 2, two
    imgDistance_ = hexagonsList_->GetWidth() * 0.25991; // 0.25991, empirical coefficient
    imgSize_ = hexagonsList_->GetWidth() * 0.23348; // 0.23348, empirical coefficient
    hexagonsList_->SetImageDistance(imgDistance_);
    hexagonsList_->SetImageSizeInCenter(imgSize_);
    int16_t blankSize = hexagonsList_->GetRelativeRect().GetWidth() > hexagonsList_->GetRelativeRect().GetHeight() ?
        hexagonsList_->GetRelativeRect().GetWidth() / 2 : hexagonsList_->GetRelativeRect().GetHeight() / 2; // 2, half
    hexagonsList_->SetScrollBlankSize(blankSize);
    hexagonsList_->SetReboundSize(blankSize);
    hexagonsList_->SetThrowDrag(true);
    hexagonsList_->SetOnRotateListener(this);
    hexagonsList_->RequestFocus();
    fragmentView_.Add(hexagonsList_);
}

void HexagonsDesktopFragment::OnDestroyView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "HexagonsDesktopFragment::OnDestroyView");
    ClearAppItemToList();
    if (hexagonsList_ != nullptr) {
        delete hexagonsList_;
        hexagonsList_ = nullptr;
    }
}

bool HexagonsDesktopFragment::OnRotate(UIView& view, const RotateEvent& event)
{
    UICustomHexagonsList* hexagonsView = dynamic_cast<UICustomHexagonsList *>(&view);
    if (hexagonsView == nullptr) {
        return false;
    }
    int8_t index = hexagonsView->GetFocusedImgIndex();
    uint8_t focusIndex_ = 0;
    if (index >= 0) {
        focusIndex_ = static_cast<uint8_t>(index);
    }
    HexagonsItemView *itemView = dynamic_cast<HexagonsItemView *>(hexagonsView->GetViewByIndex(focusIndex_));
    AppViewId viewId = itemView->GetViewId();
    if (FloatMore(hexagonsView->GetGlobalScale(), 2.0f)) { // 2.0 threshold for beginning transition
        hexagonsView->StopRotateAnimator();
        NativeAbility::GetInstance().ChangeSlice(viewId, TransitionType::TRANSITION_HEXAGONS);
        hexagonsView->ClearFocus();
    }
    return true;
}

bool HexagonsDesktopFragment::OnClick(UIView &view, const ClickEvent &event)
{
    UNUSED(event);
    HexagonsItemView *itemView = dynamic_cast<HexagonsItemView *>(&view);
    UIHexagonsList *hexagonsView = dynamic_cast<UIHexagonsList *>(itemView->GetParent());
    if (hexagonsView == nullptr || itemView == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "hexagonsView or itemView is nullptr");
        return false;
    }
    AppViewId viewId = itemView->GetViewId();
    char *uid = (char *)itemView->GetAppUid();
    hexagonsView->SetDraggable(false);
    hexagonsView->Scale(1.0f, itemView->GetViewIndex()); // switch focus to click view

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "WaterfallDesktopFragment::OnClick viewid = %d", viewId);
    return SwitchView(viewId, uid, TransitionType::TRANSITION_HEXAGONS, HEXAGONS_STYLE);
}

void HexagonsDesktopFragment::AddAppItemToList(const AppItem &item)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "WaterfallDesktopFragment::AddAppItemToList");
    HexagonsItemView *itemView = new HexagonsItemView();
    if (!itemView->SetItemInfo(item)) {
        delete itemView;
        return;
    }
    itemView->SetTouchable(true);
    itemView->SetOnClickListener(this);
    hexagonsList_->Add(itemView);
    listHexagonsView_.PushBack(itemView);
}

void HexagonsDesktopFragment::ClearAppItemToList()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "HexagonsDesktopFragment::ClearAppItemToList");
    if (hexagonsList_ != nullptr) {
        hexagonsList_->RemoveAll();
    }
    ListNode<HexagonsItemView*>* viewNode = listHexagonsView_.Head();
    while (viewNode != listHexagonsView_.End()) {
        delete(viewNode->data_);
        viewNode = viewNode->next_;
    }
    listHexagonsView_.Clear();
}

void HexagonsDesktopFragment::RefreshAppList(void)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "WaterfallDesktopFragment::RefreshAppList");
    hexagonsList_->LayoutChildren(true);
}

HexagonsItemView::~HexagonsItemView()
{
    if (itemInfo_.iconSmall == nullptr && itemInfo_.resPath != nullptr) {
        ImageCacheManager::GetInstance().UnloadOneInMultiRes(itemInfo_.resId, itemInfo_.resPath);
    }
}

bool HexagonsItemView::SetItemInfo(const AppItem &itemInfo)
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

AppViewId HexagonsItemView::GetViewId()
{
    return static_cast<AppViewId>(itemInfo_.id);
}

const char *HexagonsItemView::GetAppUid()
{
    return itemInfo_.uid;
}
}

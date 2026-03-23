/*
 * Copyright (c) CompanyNameMagicTag 2025-2025. All rights reserved.
 * Description: FootballDesktopFragment.cpp
 * Author:
 * Create: 2025-07-04
 */

#include "applist/FootballDesktopFragment.h"
#include "applist/DesktopFactory.h"
#include "common/image_cache_manager.h"
#include "common/screen.h"
#include "hal_tick.h"

namespace OHOS {
REGIST_DESKTOP_STYLE(FOOTBALL_STYLE, FootballDesktopFragment);

void FootballDesktopFragment::OnCreateView(void* data)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "FootballDesktopFragment::OnCreateView ");
    if (footballList_ == nullptr) {
        footballList_ = new UIIcosahedronView();
    }
    footballList_->SetThrowDrag(true);
    if (footballDefaultImg_ == nullptr) {
        footballDefaultImg_ = ImageCacheManager::GetInstance().LoadSingleRes(PNG_APPLIST_NOTSETUSE_IMG);
        if (footballDefaultImg_ == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "load res faild");
            return;
        }
    }
    if (callback_ == nullptr) {
        callback_ = new FootballAnimatorCallback(footballList_, 30, 200); // 30: Initial Size 200: Final display size
    }
    footballList_->SetDefaultImage(footballDefaultImg_);
    footballList_->SetPosition(0, 0, fragmentView_.GetWidth(), fragmentView_.GetHeight());
    enterAnimator_ = callback_->GetAnimator();
    fragmentView_.Add(footballList_);
    if (callback_ != nullptr) {
        callback_->SetStartTime(HALTick::GetInstance().GetTime());
    }
}

void FootballDesktopFragment::OnDestroyView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "FootballDesktopFragment::OnDestroyView");
    ClearAppItemToList();
    if (footballList_ != nullptr) {
        delete footballList_;
        footballList_ = nullptr;
    }
    if (footballDefaultImg_ != nullptr) {
        ImageCacheManager::GetInstance().UnloadSingleRes(PNG_APPLIST_NOTSETUSE_IMG);
        footballDefaultImg_ = nullptr;
    }
    if (callback_ != nullptr) {
        delete callback_;
        callback_ = nullptr;
    }
    enterAnimator_ = nullptr;
}

void FootballDesktopFragment::OnResumeView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "FootballDesktopFragment::OnResumeView");
    if (enterAnimator_ != nullptr) {
        enterAnimator_->Start();
    }
}

bool FootballDesktopFragment::OnClick(UIView &view, const ClickEvent &event)
{
    UNUSED(event);
    FootballItemView *itemView = dynamic_cast<FootballItemView *>(&view);
    AppViewId viewId = itemView->GetViewId();
    char *uid = (char *)itemView->GetAppUid();

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "FootballDesktopFragment::OnClick viewid = %d", viewId);
    return SwitchView(viewId, uid, TransitionType::TRANSITION_INVALID, FOOTBALL_STYLE);
}

void FootballDesktopFragment::AddAppItemToList(const AppItem &item)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "FootballDesktopFragment::AddAppItemToList");
    FootballItemView *itemView = new FootballItemView();
    if (!itemView->SetItemInfo(item)) {
        delete itemView;
        return;
    }
    itemView->SetTouchable(true);
    itemView->SetOnClickListener(this);
    footballList_->Add(itemView);
    footballItemList_.PushBack(itemView);
}

void FootballDesktopFragment::ClearAppItemToList()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "FootballDesktopFragment::ClearAppItemToList");
    if (enterAnimator_ != nullptr) {
        enterAnimator_->Stop();
    }
    if (footballList_ != nullptr) {
        footballList_->RemoveAll();
    }
    ListNode<FootballItemView*>* viewNode = footballItemList_.Head();
    while (viewNode != footballItemList_.End()) {
        delete(viewNode->data_);
        viewNode = viewNode->next_;
    }
    footballItemList_.Clear();
}

void FootballDesktopFragment::RefreshAppList(void)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "FootballDesktopFragment::RefreshAppList");
    footballList_->RefreshIcosahedron();
}

void FootballAnimatorCallback::Callback(UIView *view)
{
    if (HALTick::GetInstance().GetElapseTime(startTime_) < 300) { // 300, empirical coefficient
        return;
    }
    if (!isStartAnim_) {
        animator_->Stop();
        animator_->Start();
        isStartAnim_ = true;
    }
    UIIcosahedronView* footView = dynamic_cast<UIIcosahedronView*>(view);
    int16_t el = EasingEquation::LinearEaseNone(startPos_, endPos_, animator_->GetRunTime(), animator_->GetTime());
    footView->SetSideLength(el);
    footView->RefreshIcosahedron();
    const Vector3<float> rotateStart(Screen::GetInstance().GetWidth() / 2.0, // 2.0: divisor
        Screen::GetInstance().GetHeight() / 2.0, 0); // 2.0: divisor
    const Vector3<float> rotateEnd(0, 0, 0);
    // 270 : stop angle
    int16_t angle = EasingEquation::LinearEaseNone(0, 270, animator_->GetRunTime(), animator_->GetTime());
    footView->RotatePlanes(angle, rotateStart, rotateEnd);
}

FootballItemView::~FootballItemView()
{
    if (itemInfo_.iconHexagon == nullptr && itemInfo_.hexagonPath != nullptr) {
        ImageCacheManager::GetInstance().UnloadOneInMultiRes(itemInfo_.hexagonId, itemInfo_.hexagonPath);
    }
}

bool FootballItemView::SetItemInfo(const AppItem &itemInfo)
{
    itemInfo_ = itemInfo;
    if (itemInfo.iconHexagon == nullptr && itemInfo.hexagonPath != nullptr) {
        ImageInfo* image =
            ImageCacheManager::GetInstance().LoadOneInMultiRes(itemInfo.hexagonId, itemInfo.hexagonPath);
        if (image == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Load App Icon path %s Image %x fail",
                itemInfo.hexagonId, itemInfo.hexagonPath);
            return false;
        }
        SetSrc(image);
    } else {
        SetSrc(itemInfo.iconHexagon);
    }
    return true;
}

AppViewId FootballItemView::GetViewId()
{
    return static_cast<AppViewId>(itemInfo_.id);
}

const char *FootballItemView::GetAppUid()
{
    return itemInfo_.uid;
}
}

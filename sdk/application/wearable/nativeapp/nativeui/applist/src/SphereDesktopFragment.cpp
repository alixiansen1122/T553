  /*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: SphereDesktopFragment
 * Author: Hisi Graphic Team
 * Created: 2025-09
 */

#include <cmath>
#include "applist/SphereDesktopFragment.h"
#include "applist/DesktopFactory.h"
#include "common/image_cache_manager.h"
#include "NativeAbility.h"
#include "hal_tick.h"

namespace OHOS {
REGIST_DESKTOP_STYLE(SPHERE_STYLE, SphereDesktopFragment);

void SphereDesktopFragment::OnCreateView(void* data)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SphereDesktopFragment::OnCreateView ");
    if (sphereView_ == nullptr) {
        sphereView_ = new UISphereView(1);
    }
    sphereView_->SetThrowDrag(true);

    if (callback_ == nullptr) {
        callback_ = new SphereAnimatorCallback(sphereView_, fragmentView_.GetWidth(), fragmentView_.GetHeight());
    }

    sphereView_->SetPosition(0, 0, fragmentView_.GetWidth(), fragmentView_.GetHeight());
    enterAnimator_ = callback_->GetAnimator();
    sphereView_->SetVisible(false);
    fragmentView_.Add(sphereView_);
    if (callback_ != nullptr) {
        callback_->SetStartTime(HALTick::GetInstance().GetTime());
    }
}

void SphereDesktopFragment::OnDestroyView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SphereDesktopFragment::OnDestroyView");
    ClearAppItemToList();
    if (sphereView_ != nullptr) {
        delete sphereView_;
        sphereView_ = nullptr;
    }
    if (callback_ != nullptr) {
        delete callback_;
        callback_ = nullptr;
    }
    enterAnimator_ = nullptr;
}

void SphereDesktopFragment::OnResumeView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SphereDesktopFragment::OnResumeView");
    if (enterAnimator_ != nullptr) {
        enterAnimator_->Start();
    }
}

bool SphereDesktopFragment::OnRotate(UIView& view, const RotateEvent& event)
{
    UISphereView *sphereView = dynamic_cast<UISphereView *>(&view);
    if (sphereView == nullptr) {
        return false;
    }
    sphereView->UpdateItemAngle(event.GetRotate());
    return true;
}

bool SphereDesktopFragment::OnClick(UIView &view, const ClickEvent &event)
{
    UNUSED(event);
    SphereItemView *itemView = dynamic_cast<SphereItemView *>(&view);
    UISphereView *sphereView = dynamic_cast<UISphereView *>(itemView->GetParent());
    if (sphereView == nullptr || itemView == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "sphereView or itemView is nullptr");
        return false;
    }
    AppViewId viewId = itemView->GetViewId();
    char *uid = (char *)itemView->GetAppUid();
    sphereView->SetDraggable(false);

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SphereDesktopFragment::OnClick viewid = %d", viewId);
    return SwitchView(viewId, uid, TransitionType::TRANSITION_FADEINOUT, SPHERE_STYLE);
}

void SphereDesktopFragment::AddAppItemToList(const AppItem &item)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SphereDesktopFragment::AddAppItemToList");
    SphereItemView *itemView = new SphereItemView();
    if (!itemView->SetItemInfo(item)) {
        delete itemView;
        return;
    }
    itemView->SetTouchable(true);
    itemView->SetOnClickListener(this);
    sphereView_->Add(itemView);
    listSphereView_.PushBack(itemView);
    sphereView_->SetNumberOfRow((listSphereView_.Size() + 2) / 3); // 控制在3排显示
}

void SphereDesktopFragment::ClearAppItemToList()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SphereDesktopFragment::ClearAppItemToList");
    if (enterAnimator_ != nullptr) {
        enterAnimator_->Stop();
    }
    if (sphereView_ != nullptr) {
        sphereView_->RemoveAll();
    }
    ListNode<SphereItemView*>* viewNode = listSphereView_.Head();
    while (viewNode != listSphereView_.End()) {
        delete(viewNode->data_);
        viewNode = viewNode->next_;
    }
    listSphereView_.Clear();
}

void SphereDesktopFragment::RefreshAppList(void)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SphereDesktopFragment::RefreshAppList");
    sphereView_->LayoutChildren(true);
}

void SphereAnimatorCallback::Callback(UIView *view)
{
    if (HALTick::GetInstance().GetElapseTime(startTime_) < 300) { // 300: 动画延迟显示时间，观感效果更好
        angle_=0;
        return;
    }
    if (!view->IsVisible()) {
        view->SetVisible(true);
    }
    if (!isStartAnim_) {
        animator_->Stop();
        animator_->Start();
        isStartAnim_ = true;
    }
    UISphereView* sphereView = dynamic_cast<UISphereView*>(view);
    const int16_t precision = 1000;// 1000: 精度
    int16_t el = EasingEquation::QuadEaseOut(0, precision, animator_->GetRunTime(), animator_->GetTime()); 
    int roateAngle = el * 360 / precision; // 360 : 一共旋转360度
    sphereView->UpdateItemAngle(roateAngle - angle_);
    angle_ = roateAngle;
}

SphereItemView::~SphereItemView()
{
    if (imageView_ != nullptr) {
        delete imageView_;
        imageView_ = nullptr;
    }
    if (itemInfo_.iconSmall == nullptr && itemInfo_.resPath != nullptr) {
        ImageCacheManager::GetInstance().UnloadOneInMultiRes(itemInfo_.resId, itemInfo_.resPath);
    }
}

bool SphereItemView::SetItemInfo(const AppItem &itemInfo)
{
    if (imageView_ == nullptr) {
        imageView_ = new UIImageView();
    }
    itemInfo_ = itemInfo;
    if (itemInfo.iconSmall == nullptr && itemInfo.resPath != nullptr) {
        ImageInfo* image = ImageCacheManager::GetInstance().LoadOneInMultiRes(itemInfo.resId, itemInfo.resPath);
        if (image == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Load App Icon path %s Image %x fail",
                itemInfo.resId, itemInfo.resPath);
            return false;
        }
        imageView_->SetSrc(image);
    } else {
        imageView_->SetSrc(itemInfo.iconSmall);
    }
    Add(imageView_);
    SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    Resize(imageView_->GetWidth(), imageView_->GetHeight());
    return true;
}

AppViewId SphereItemView::GetViewId()
{
    return static_cast<AppViewId>(itemInfo_.id);
}

const char *SphereItemView::GetAppUid()
{
    return itemInfo_.uid;
}
}

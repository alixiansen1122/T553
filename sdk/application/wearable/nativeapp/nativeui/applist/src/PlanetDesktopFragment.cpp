/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: PlanetDesktop
 * Created: 2025-07-29
 */

#include "applist/PlanetDesktopFragment.h"
#include "applist/DesktopFactory.h"
#include "applist/ApplistModel.h"

namespace OHOS {
REGIST_DESKTOP_STYLE(PLANET_STYLE, PlanetDesktopFragment);

static constexpr int16 LABEL_FONT_SIZE = 15;

void PlanetDesktopFragment::OnCreateView(void* data)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "PlanetDesktopFragment::OnCreateView ");
    imgSize_ = 100; // 100, size of item
    int16_t imageDistance = 5; // 5, image distance
    if (planetList_ == nullptr) {
        planetList_ = new UICustomPlanetList(imgSize_, imageDistance);
    }
    planetList_->SetPosition(0, 0, fragmentView_.GetWidth(), fragmentView_.GetHeight());
    planetList_->SetDraggable(true);
    planetList_->SetThrowDrag(true);
    planetList_->SetItemClickListener(this);
    planetList_->RequestFocus();
    fragmentView_.Add(planetList_);
}

void PlanetDesktopFragment::OnDestroyView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "PlanetDesktopFragment::OnDestroyView");
    ClearAppItemToList();
    if (planetList_ != nullptr) {
        delete planetList_;
        planetList_ = nullptr;
    }
}

bool PlanetDesktopFragment::OnClick(UIView &view, const ClickEvent &event)
{
    UNUSED(event);
    PlanetItemView *itemView = dynamic_cast<PlanetItemView *>(&view);
    AppViewId viewId = itemView->GetViewId();
    char *uid = (char *)itemView->GetAppUid();

    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "PlanetDesktopFragment::OnClick viewid = %d", viewId);
    return SwitchView(viewId, uid, TransitionType::TRANSITION_INVALID, PLANET_STYLE);
}

void PlanetDesktopFragment::AddAppItemToList(const AppItem &item)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "PlanetDesktopFragment::AddAppItemToList");
    PlanetItemView* itemView = new PlanetItemView();
    UILabel* label = new UILabel();
    itemView->SetItemInfo(item);
    itemView->SetSrc(item.iconSmall);
    itemView->SetTouchable(true);
    itemView->SetAutoEnable(false);
    itemView->SetResizeMode(UIImageView::ImageResizeMode::CONTAIN);
    itemView->SetOnClickListener(this);
    label->SetText(item.label);
    label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, LABEL_FONT_SIZE);
    label->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    label->SetVisible(false);
    label->SetHeight(30); // 30, label height
    label->SetWidth(100); // 100, label width
    planetList_->AddItemVIew(itemView);
    planetList_->AddLabel(label);
    pltItemList_.PushBack(itemView);
    pltLabelList_.PushBack(label);
}

void PlanetDesktopFragment::ClearAppItemToList()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "PlanetDesktopFragment::ClearAppItemToList");
    int16_t offsetX = 0;
    int16_t offsetY = 0;
    ApplistModel *model = &ApplistModel::GetInstance();

    planetList_->RestoreItemState();
    planetList_->GetLastOffset(offsetX, offsetY);
    model->SetLastOffset(offsetX, offsetY);
    if (planetList_ != nullptr) {
        planetList_->RemoveAll();
    }
    ListNode<PlanetItemView*>* viewNode = pltItemList_.Head();
    while (viewNode != pltItemList_.End()) {
        delete(viewNode->data_);
        viewNode = viewNode->next_;
    }
    pltItemList_.Clear();
    ListNode<UILabel*>* labelNode = pltLabelList_.Head();
    while (labelNode != pltLabelList_.End()) {
        delete(labelNode->data_);
        labelNode = labelNode->next_;
    }
    pltLabelList_.Clear();
}

void PlanetDesktopFragment::RefreshAppList(void)
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "PlanetDesktopFragment::RefreshAppList");
    int16_t offsetX = 0;
    int16_t offsetY = 0;
    ApplistModel *model = &ApplistModel::GetInstance();
    model->GetLastOffset(offsetX, offsetY);
    planetList_->SetLastOffset(offsetX, offsetY);
    planetList_->LayoutChildren(true);
    planetList_->AddClickListenerToAppItems();
}

void PlanetItemView::SetItemInfo(const AppItem &itemInfo)
{
    itemInfo_ = itemInfo;
}

AppViewId PlanetItemView::GetViewId()
{
    return static_cast<AppViewId>(itemInfo_.id);
}

const char *PlanetItemView::GetAppUid()
{
    return itemInfo_.uid;
}

const char *PlanetItemView::GetAppLabel()
{
    return itemInfo_.label;
}
}

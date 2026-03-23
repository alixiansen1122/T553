/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingCardPage
 * Create: 2025-06-01
 */
#include "components/ui_list_nested.h"
#include "components/ui_scroll_view_nested.h"
#include "SlicePageFactory.h"
#include "PageTransitionMgr.h"
#include "NativeAbility.h"
#include "settings/model/SettingMainModel.h"
#include "settings/model/SettingCardModel.h"
#include "settings/page/SettingCardPage.h"

namespace OHOS {
REGIST_SLICE_PAGE(VIEW_SETTING, SettingPageId::CARD_PAGE, SettingCardPage, false);

static constexpr int16_t SETTING_LIST_POSITION_X = 14;
static constexpr int16_t SETTING_LIST_WIDTH = 426;
static constexpr int16_t SETTING_SCROLLBLANKSIZE = 30;
static constexpr int16 MAX_SCR_DIS = 1000;
static constexpr int16 SNUM_2 = 2;
static SettingCardPage *g_pSettingCardPage{nullptr};

void SettingCardPage::OnStart(void* data)
{
    g_pSettingCardPage = this;
    content_ = new UISimpleList();
    if (content_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingCardPage::OnStart content_ new fail");
        return;
    }
    content_->SetPosition(SETTING_LIST_POSITION_X, 0, SETTING_LIST_WIDTH, HORIZONTAL_RESOLUTION);
    content_->SetScrollBlankSize(SETTING_SCROLLBLANKSIZE, UIAbstractScroll::Direction::BOTTOM);
    content_->SetSelectPosition(HORIZONTAL_RESOLUTION / SNUM_2);
    content_->SetDraggable(true);
    content_->SetThrowDrag(true);
    content_->EnableAutoAlign(true);
    content_->SetMaxScrollDistance(MAX_SCR_DIS);

    scroll_ = new UIScrollViewNested();
    if (scroll_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingCardPage::OnStart scroll_ new fail");
        return;
    }
    scroll_->Resize(HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    scroll_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    scroll_->SetStyle(STYLE_BACKGROUND_OPA, UN_OPACITY);
    scroll_->SetThrowDrag(true);
    scroll_->SetIntercept(true);
    scroll_->Add(content_);
    AddViewToPageContainer(scroll_);
}

void SettingCardPage::OnResume()
{
    content_->ClearAll();
    for(auto& it : SettingCardModel::GetInstance().GetCardUnionData()) {
        Contents cont;
        cont.createFunc = SettingMainModel::OnSimpleListCreate;
        cont.updateFunc = SettingCardPage::OnSimpleListUpdate;
        cont.type = it.GetUnionItemType();
        cont.data = &it;
        content_->AddContent(cont);
    }
    content_->RefreshList();
}

void SettingCardPage::OnStop()
{
    g_pSettingCardPage = nullptr;
    if (scroll_ != nullptr) {
        scroll_->RemoveAll();
        delete scroll_;
        scroll_ = nullptr;
    }
    if (content_ != nullptr) {
        delete content_;
        content_ = nullptr;
    }
}

bool SettingCardPage::OnClick(UIView& view, const ClickEvent& event)
{
    uintptr_t viewId = (uintptr_t)view.GetViewId();
    if (viewId == (uintptr_t)ICON_TEXT_NAVIGATION) {
        int16_t viewIndex = view.GetViewIndex();
        auto& cardUnionData = SettingCardModel::GetInstance().GetCardUnionData();
        if (viewIndex < 0 || viewIndex > cardUnionData.size()) {
            return true;
        }

        SettingUnionItemData& itemData = cardUnionData[viewIndex];
        if (itemData.GetUnionItemType() == ICON_TEXT_NAVIGATION) {
            IconTextNavigationItemData* data = static_cast<IconTextNavigationItemData*>(itemData.GetUnionItemData());
            NativeAbility::GetInstance().SwitchPageInSlice(data->pageId,
                TransitionType::TRANSITION_INVALID, true);
        }
        return true;
    }
    return false;
}

void SettingCardPage::OnSimpleListUpdate(UIView* view, void* data, uint8_t type)
{
    if (view == nullptr || data == nullptr) {
        return;
    }
    if (type == SettingUnionItemType::PLAIN_TEXT) {
        PlainTextItemView *itemGroup = dynamic_cast<PlainTextItemView*>(view);
        if (itemGroup == nullptr) {
            return;
        }
        PlainTextItemData* itemData = static_cast<PlainTextItemData*>(data);
        itemGroup->UpdateViewData(itemData);
    } else if(type == SettingUnionItemType::ICON_TEXT_TOGGLE) {
        IconTextToggleItemView *itemGroup = dynamic_cast<IconTextToggleItemView*>(view);
        if (itemGroup == nullptr) {
            return;
        }
        IconTextToggleItemData* itemData = static_cast<IconTextToggleItemData*>(data);
        itemGroup->UpdateViewData(itemData);
        itemGroup->SetOnClickListener(g_pSettingCardPage);
    }  else if(type == SettingUnionItemType::ICON_TEXT_NAVIGATION) {
        IconTextNavigationItemView *itemGroup = dynamic_cast<IconTextNavigationItemView*>(view);
        if (itemGroup == nullptr) {
            return;
        }
        IconTextNavigationItemData* itemData = static_cast<IconTextNavigationItemData*>(data);
        itemGroup->UpdateViewData(itemData);
        itemGroup->SetOnClickListener(g_pSettingCardPage);
    }
    return;
}
}
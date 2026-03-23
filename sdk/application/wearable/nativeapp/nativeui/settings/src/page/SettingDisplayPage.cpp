/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingDisplayPage
 * Create: 2025-06-01
 */
#include "components/ui_scroll_view_nested.h"
#include "SlicePageFactory.h"
#include "PageTransitionMgr.h"
#include "NativeAbility.h"
#include "settings/model/SettingMainModel.h"
#include "settings/model/SettingDisplayModel.h"
#include "settings/page/SettingDisplayPage.h"

namespace OHOS {
REGIST_SLICE_PAGE(VIEW_SETTING, SettingPageId::DISPLAY_PAGE, SettingDisplayPage, false);

static constexpr int16_t SETTING_LIST_POSITION_X = 14;
static constexpr int16_t SETTING_LIST_WIDTH = 426;
static constexpr int16_t SETTING_SCROLLBLANKSIZE = 96;
static constexpr int16 MAX_SCR_DIS = 1000;
static constexpr int16 SNUM_2 = 2;
static SettingDisplayPage *g_pSettingDisplayPage{nullptr};

void SettingDisplayPage::OnStart(void *data)
{
    g_pSettingDisplayPage = this;
    scroll_ = new UIScrollViewNested();
    if (scroll_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingDisplayPage::OnStart scroll_ new fail");
        return;
    }
    scroll_->Resize(HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    scroll_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    scroll_->SetStyle(STYLE_BACKGROUND_OPA, UN_OPACITY);
    scroll_->SetThrowDrag(true);
    scroll_->SetTouchable(true);
    scroll_->SetDraggable(true);
    scroll_->SetIntercept(true);

    content_ = new UISimpleList();
    if (content_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingDisplayPage::OnStart content_ new fail");
        return;
    }
    content_->SetPosition(SETTING_LIST_POSITION_X, 0, SETTING_LIST_WIDTH, HORIZONTAL_RESOLUTION);
    content_->SetScrollBlankSize(SETTING_SCROLLBLANKSIZE, UIAbstractScroll::Direction::BOTTOM);
    content_->SetSelectPosition(HORIZONTAL_RESOLUTION / SNUM_2);
    content_->SetDraggable(true);
    content_->SetThrowDrag(true);
    content_->SetMaxScrollDistance(MAX_SCR_DIS);
    scroll_->Add(content_);
    AddViewToPageContainer(scroll_);
}

void SettingDisplayPage::OnResume()
{
    content_->ClearAll();
    for(auto& it : SettingDisplayModel::GetInstance().GetDisplayUnionData()) {
        Contents cont;
        cont.createFunc = SettingMainModel::OnSimpleListCreate;
        cont.updateFunc = SettingDisplayPage::OnSimpleListUpdate;
        cont.type = it.GetUnionItemType();
        cont.data = &it;
        content_->AddContent(cont);
    }
    content_->RefreshList();
}

void SettingDisplayPage::OnStop()
{
    g_pSettingDisplayPage = nullptr;
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

bool SettingDisplayPage::OnClick(UIView& view, const ClickEvent& event)
{
    uintptr_t viewId = (uintptr_t)view.GetViewId();
    if (viewId == (uintptr_t)ICON_TEXT_TOGGLE) {
        int16_t viewIndex = view.GetViewIndex();
        auto& displayUnionData = SettingDisplayModel::GetInstance().GetDisplayUnionData();
        if (viewIndex < 0 || viewIndex > displayUnionData.size()) {
            return true;
        }

        SettingDisplayModel::GetInstance().SetAutoAdjustBrightness(
            !SettingDisplayModel::GetInstance().GetAutoAdjustBrightness());
        SettingDisplayModel::GetInstance().RefreshDisplayUnionDataForAutoAdjustBrightness();
        content_->RefreshList();
    } else if (viewId == (uintptr_t)ICON_TEXT_NAVIGATION) {
        int16_t viewIndex = view.GetViewIndex();
        auto& displayUnionData = SettingDisplayModel::GetInstance().GetDisplayUnionData();
        if (viewIndex < 0 || viewIndex > displayUnionData.size()) {
            return true;
        }

        SettingUnionItemData& itemData = displayUnionData[viewIndex];
        if (itemData.GetUnionItemType() == ICON_TEXT_NAVIGATION) {
            IconTextNavigationItemData* data = static_cast<IconTextNavigationItemData*>(itemData.GetUnionItemData());
            NativeAbility::GetInstance().SwitchPageInSlice(data->pageId,
                TransitionType::TRANSITION_INVALID, true);
        }
    }
    return true;
}

void SettingDisplayPage::OnSimpleListUpdate(UIView* view, void* data, uint8_t type)
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
        itemGroup->SetOnClickListener(g_pSettingDisplayPage);
    }  else if(type == SettingUnionItemType::ICON_TEXT_NAVIGATION) {
        IconTextNavigationItemView *itemGroup = dynamic_cast<IconTextNavigationItemView*>(view);
        if (itemGroup == nullptr) {
            return;
        }
        IconTextNavigationItemData* itemData = static_cast<IconTextNavigationItemData*>(data);
        itemGroup->UpdateViewData(itemData);
        itemGroup->SetOnClickListener(g_pSettingDisplayPage);
    }
    return;
}
}
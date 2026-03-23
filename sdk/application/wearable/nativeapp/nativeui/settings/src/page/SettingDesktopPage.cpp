/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingDesktopPage
 * Create: 2025-06-01
 */
#include "components/ui_list_nested.h"
#include "components/ui_scroll_view_nested.h"
#include "SlicePageFactory.h"
#include "PageTransitionMgr.h"
#include "NativeAbility.h"
#include "settings/model/SettingMainModel.h"
#include "settings/model/SettingDesktopModel.h"
#include "settings/page/SettingDesktopPage.h"

namespace OHOS {
REGIST_SLICE_PAGE(VIEW_SETTING, SettingPageId::DESKTOP_PAGE, SettingDesktopPage, false);

static constexpr int16_t SETTING_LIST_POSITION_X = 14;
static constexpr int16_t SETTING_LIST_WIDTH = 426;
static constexpr int16_t SETTING_SCROLLBLANKSIZE = 96;
static constexpr int16 MAX_SCR_DIS = 1000;
static constexpr int16 SNUM_2 = 2;
static SettingDesktopPage *g_pSettingDesktopPage{nullptr};

void SettingDesktopPage::OnStart(void *data)
{
    g_pSettingDesktopPage = this;
    content_ = new UISimpleList();
    if (content_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingDesktopPage::OnStart content_ new fail");
        return;
    }
    content_->SetPosition(SETTING_LIST_POSITION_X, 0, SETTING_LIST_WIDTH, HORIZONTAL_RESOLUTION);
    content_->SetScrollBlankSize(SETTING_SCROLLBLANKSIZE, UIAbstractScroll::Direction::BOTTOM);
    content_->SetSelectPosition(HORIZONTAL_RESOLUTION / SNUM_2);
    content_->SetDraggable(true);
    content_->SetThrowDrag(true);
    content_->SetMaxScrollDistance(MAX_SCR_DIS);

    scroll_ = new UIScrollViewNested();
    if (scroll_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingDesktopPage::OnStart scroll_ new fail");
        return;
    }
    scroll_->Resize(HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    scroll_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    scroll_->SetStyle(STYLE_BACKGROUND_OPA, UN_OPACITY);
    scroll_->SetThrowDrag(true);
    scroll_->SetIntercept(true);
    scroll_->SetOnDragListener(this);
    scroll_->Add(content_);
    AddViewToPageContainer(scroll_);
}

void SettingDesktopPage::OnResume()
{
    content_->ClearAll();
    for (SettingUnionItemData& it : SettingDesktopModel::GetInstance().GetDesktopUnionData()) {
        Contents cont;
        cont.createFunc = SettingMainModel::OnSimpleListCreate;
        cont.updateFunc = SettingDesktopPage::OnSimpleListUpdate;
        cont.type = it.GetUnionItemType();
        cont.data = &it;
        content_->AddContent(cont);
    }
    content_->RefreshList();
}

void SettingDesktopPage::OnStop()
{
    if (scroll_ != nullptr) {
        scroll_->RemoveAll();
        delete scroll_;
        scroll_ = nullptr;
    }
    if (content_ != nullptr) {
        delete content_;
        content_ = nullptr;
    }
    g_pSettingDesktopPage = nullptr;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SettingDesktopPage::~SettingDesktopPage");
}

bool SettingDesktopPage::OnClick(UIView& view, const ClickEvent& event)
{
    uintptr_t viewId = (uintptr_t)view.GetViewId();
    if (viewId == (uintptr_t)ICON_TEXT_NAVIGATION) {
        int16_t viewIndex = view.GetViewIndex();
        auto& desktopUnionData = SettingDesktopModel::GetInstance().GetDesktopUnionData();
        if (viewIndex < 0 || viewIndex > desktopUnionData.size()) {
            return true;
        }

        SettingUnionItemData& itemData = desktopUnionData[viewIndex];
        if (itemData.GetUnionItemType() == ICON_TEXT_NAVIGATION) {
            IconTextNavigationItemData* data = static_cast<IconTextNavigationItemData*>(itemData.GetUnionItemData());
            NativeAbility::GetInstance().SwitchPageInSlice(data->pageId,
                TransitionType::TRANSITION_INVALID, true);
        }
    }
    return true;
}

void SettingDesktopPage::OnSimpleListUpdate(UIView* view, void* data, uint8_t type)
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
        itemGroup->SetOnClickListener(g_pSettingDesktopPage);
    }  else if(type == SettingUnionItemType::ICON_TEXT_NAVIGATION) {
        IconTextNavigationItemView *itemGroup = dynamic_cast<IconTextNavigationItemView*>(view);
        if (itemGroup == nullptr) {
            return;
        }
        IconTextNavigationItemData* itemData = static_cast<IconTextNavigationItemData*>(data);
        itemGroup->UpdateViewData(itemData);
        itemGroup->SetOnClickListener(g_pSettingDesktopPage);
    }
    return;
}
}
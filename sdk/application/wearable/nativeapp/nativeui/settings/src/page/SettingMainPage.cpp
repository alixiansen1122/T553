/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingMainPage
 * Create: 2025-06-01
 */
#include "components/ui_list_nested.h"
#include "components/ui_scroll_view_nested.h"
#include "SlicePageFactory.h"
#include "PageTransitionMgr.h"
#include "NativeAbility.h"
#include "settings/common/SettingCommon.h"
#include "settings/model/SettingMainModel.h"
#include "settings/model/SettingDesktopModel.h"
#include "settings/page/SettingMainPage.h"

namespace OHOS {
REGIST_SLICE_PAGE(VIEW_SETTING, SettingPageId::MAIN_PAGE, SettingMainPage, true);

static constexpr int16_t SETTING_LIST_POSITION_X = 14;
static constexpr int16_t SETTING_LIST_WIDTH = 426;
static constexpr int16_t SETTING_SCROLLBLANKSIZE = 96;
static constexpr int16 MAX_SCR_DIS = 1000;
static constexpr int16 SNUM_2 = 2;
static SettingMainPage *g_pSettingMainPage = nullptr;

void SettingMainPage::OnStart(void *data)
{
    g_pSettingMainPage = this;
    simpleList_ = new UISimpleList();
    if (simpleList_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingMainPage::OnStart simpleList_ new fail");
        return;
    }
    simpleList_->SetPosition(SETTING_LIST_POSITION_X, 0, SETTING_LIST_WIDTH, HORIZONTAL_RESOLUTION);
    simpleList_->SetScrollBlankSize(SETTING_SCROLLBLANKSIZE, UIAbstractScroll::Direction::BOTTOM);
    simpleList_->SetSelectPosition(HORIZONTAL_RESOLUTION / SNUM_2);
    simpleList_->SetTouchable(true);
    simpleList_->SetDraggable(true);
    simpleList_->SetThrowDrag(true);
    simpleList_->SetMaxScrollDistance(MAX_SCR_DIS);
    scrollView_ = new UIScrollViewNested();
    if (scrollView_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingMainPage::OnStart scrollView_ new fail");
        return;
    }
    scrollView_->Resize(HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    scrollView_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    scrollView_->SetStyle(STYLE_BACKGROUND_OPA, UN_OPACITY);
    scrollView_->SetThrowDrag(true);
    scrollView_->Add(simpleList_);
    AddViewToPageContainer(scrollView_);
    if (PageTransitionMgr::GetInstance().GetTopSlideBackImage() == nullptr) { // 滑动跟手返回
        scrollView_->SetOnDragListener(this);
    }
}

void SettingMainPage::OnResume()
{
    simpleList_->ClearAll();
    for(auto& it : SettingMainModel::GetInstance().GetMainUnionData()) {
        Contents cont;
        cont.createFunc = SettingMainModel::OnSimpleListCreate;
        cont.updateFunc = SettingMainPage::OnSimpleListUpdate;
        cont.type = it.GetUnionItemType();
        cont.data = &it;
        simpleList_->AddContent(cont);
    }
    simpleList_->RefreshList();
}

void SettingMainPage::OnStop()
{
    g_pSettingMainPage = this;
    if (scrollView_ != nullptr) {
        scrollView_->RemoveAll();
        delete scrollView_;
        scrollView_ = nullptr;
    }
    if (simpleList_ != nullptr) {
        delete simpleList_;
        simpleList_ = nullptr;
    }
}

bool SettingMainPage::OnDrag(UIView &view, const DragEvent &event)
{
    UNUSED(view);
    if ((event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) &&
        (event.GetDeltaX() > X_DRAG_OFFSET) && (event.GetDeltaX() > (DOUBLES * MATH_ABS(event.GetDeltaY())))) {
        NativeAbility::GetInstance().ChangeSliceToApplist();
    }
    return false;
}

bool SettingMainPage::OnClick(UIView& view, const ClickEvent& event)
{
    uintptr_t simpleListViewId = (uintptr_t)view.GetViewId();
    if (simpleListViewId == (uintptr_t)ICON_TEXT_NAVIGATION) {
        int16_t viewIndex = view.GetViewIndex();
        auto& mainUnionData = SettingMainModel::GetInstance().GetMainUnionData();
        if (viewIndex < 0 || viewIndex > mainUnionData.size()) {
            return true;
        }

        SettingUnionItemData& itemData = mainUnionData[viewIndex];
        if (itemData.GetUnionItemType() == ICON_TEXT_NAVIGATION) {
            IconTextNavigationItemData* data = static_cast<IconTextNavigationItemData*>(itemData.GetUnionItemData());
            NativeAbility::GetInstance().SwitchPageInSlice(data->pageId,
                TransitionType::TRANSITION_INVALID, true);
        }
    }
    return true;
}

void SettingMainPage::OnSimpleListUpdate(UIView* view, void* data, uint8_t type)
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
        itemGroup->SetOnClickListener(g_pSettingMainPage);
    }  else if(type == SettingUnionItemType::ICON_TEXT_NAVIGATION) {
        IconTextNavigationItemView *itemGroup = dynamic_cast<IconTextNavigationItemView*>(view);
        if (itemGroup == nullptr) {
            return;
        }
        IconTextNavigationItemData* itemData = static_cast<IconTextNavigationItemData*>(data);
        itemGroup->UpdateViewData(itemData);
        itemGroup->SetOnClickListener(g_pSettingMainPage);
    }
    return;
}
}
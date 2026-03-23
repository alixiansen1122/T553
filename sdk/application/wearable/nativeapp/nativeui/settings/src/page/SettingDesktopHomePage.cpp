/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingDesktopHomePage
 * Create: 2025-06-01
 */
#include "components/ui_scroll_view_nested.h"
#include "SlicePageFactory.h"
#include "PageTransitionMgr.h"
#include "NativeAbility.h"
#include "settings/model/SettingDesktopModel.h"
#include "settings/page/SettingDesktopHomePage.h"

namespace OHOS {
REGIST_SLICE_PAGE(VIEW_SETTING, SettingPageId::DESKTOP_HOME_PAGE, SettingDesktopHomePage, false);

static constexpr int16_t SETTING_DESKTOP_HOME_LIST_Y = 130;
static constexpr int16_t SETTING_DESKTOP_HOME_LSIT_HEIGHT = 205;
static constexpr int16_t SETTING_DESKTOP_HOME_LIST_BLANK = 0;
static constexpr int16_t SETTING_DESKTOP_HOME_LIST_DISTANCE = 1000;
static constexpr int16_t SETTING_DESKTOP_HOME_LIST_SELECTPOS = HORIZONTAL_RESOLUTION / 4;

static constexpr int16_t SETTING_DESKTOP_HOME_TITLE_X = 100;
static constexpr int16_t SETTING_DESKTOP_HOME_TITLE_Y = 39;
static constexpr int16_t SETTING_DESKTOP_HOME_TITLE_WIDTH = 254;
static constexpr int16_t SETTING_DESKTOP_HOME_TITLE_HEIGHT = 53;
static constexpr int16_t SETTING_DESKTOP_HOME_TITLE_FSIZE = 38;
static const char* SETTING_DESKTOP_HOME_TITLE_TEXT = "桌面风格";

void SettingDesktopHomePage::OnStart(void* data)
{
    list_ = new UISimpleList();
    if (list_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingDesktopHomePage::OnStart list_ new fail");
        return;
    }
    list_->SetDirection(UIAbstractScroll::HORIZONTAL);
    list_->SetBoundaryIntercept(DragEvent::DIRECTION_LEFT_TO_RIGHT, false);
    list_->SetPosition(0, SETTING_DESKTOP_HOME_LIST_Y);
    list_->Resize(HORIZONTAL_RESOLUTION, SETTING_DESKTOP_HOME_LSIT_HEIGHT);
    list_->SetScrollBlankSize(SETTING_DESKTOP_HOME_LIST_BLANK);
    list_->SetSelectPosition(SETTING_DESKTOP_HOME_LIST_SELECTPOS);
    list_->SetDraggable(true);
    list_->SetThrowDrag(true);
    list_->EnableAutoAlign(true);
    list_->SetMaxScrollDistance(SETTING_DESKTOP_HOME_LIST_DISTANCE);
    list_->SetIntercept(false);

    title_ = new UILabel();
    if (title_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingDesktopHomePage::OnStart title_ new fail");
        return;
    }
    title_->SetPosition(SETTING_DESKTOP_HOME_TITLE_X, SETTING_DESKTOP_HOME_TITLE_Y);
    title_->Resize(SETTING_DESKTOP_HOME_TITLE_WIDTH, SETTING_DESKTOP_HOME_TITLE_HEIGHT);
    title_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    title_->SetFont(BOLD_VECTOR_FONT_FILENAME, SETTING_DESKTOP_HOME_TITLE_FSIZE);
    title_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    title_->SetText(SETTING_DESKTOP_HOME_TITLE_TEXT);

    AddViewToPageContainer(list_);
    AddViewToPageContainer(title_);
}

void SettingDesktopHomePage::OnResume()
{
    list_->ClearAll();
    for(auto& it : SettingDesktopModel::GetInstance().GetDesktopHomeData()) {
        it.contextPtr = this;
        Contents cont;
        cont.createFunc = SettingDesktopHomePage::OnSimpleListCreate;
        cont.updateFunc = SettingDesktopHomePage::OnSimpleListUpdate;
        cont.type = 0;
        cont.data = &it;
        list_->AddContent(cont);
    }
    list_->RefreshList();
}

void SettingDesktopHomePage::OnStop()
{
    if (list_ != nullptr) {
        delete list_;
        list_ = nullptr;
    }
    if (title_ != nullptr) {
        delete title_;
        title_ = nullptr;
    }
}

bool SettingDesktopHomePage::OnClick(UIView& view, const ClickEvent& event)
{
    int16_t index = view.GetViewIndex();
    int16_t currIndex = SettingDesktopModel::GetInstance().GetDesktopHomeDataSelect();
    auto& desktopHomeData = SettingDesktopModel::GetInstance().GetDesktopHomeData();
    if (index != currIndex && index >= 0 && (size_t)index < desktopHomeData.size()) {
        SettingDesktopModel::GetInstance().SetDesktopHomeDataSelect((DesktopStyle)desktopHomeData[index].style);
        list_->RefreshList();
        NativeAbility::GetInstance().ChangeSlice(VIEW_MAIN_SAMPLE);
    }
    return true;
}

UIView* SettingDesktopHomePage::OnSimpleListCreate(uint8_t type)
{
    UNUSED(type);
    return new SettingDesktopHomeItemGroup();
}

void SettingDesktopHomePage::OnSimpleListUpdate(UIView* view, void* data, uint8_t type)
{
    UNUSED(type);
    if (view == nullptr || data == nullptr) {
        return;
    }
    SettingDesktopHomeItemGroup* itemGroup = dynamic_cast<SettingDesktopHomeItemGroup*>(view);
    if (itemGroup == nullptr) {
        return;
    }
    SettingDesktopHomeItemData* itemData = static_cast<SettingDesktopHomeItemData*>(data);
    SettingDesktopHomePage* page = static_cast<SettingDesktopHomePage*>(itemData->contextPtr);
    itemGroup->UpdateItemGroup(itemData);
    itemGroup->SetOnClickListener(page);
}
}
/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SportSettingsPage
 * Create: 2025-06-06
 */

#include "UiConfig.h"
#include "main/LoadImg.h"
#include "wearable_log.h"
#include "AppViewIDs.h"
#include "NativeAbility.h"
#include "SlicePageFactory.h"
#include "sport/SportView.h"
#include "sport/SportSettingsPage.h"

namespace OHOS {
static constexpr uint16_t LABELS_X = 100;
static constexpr uint16_t LABELS_Y = 38;
static constexpr uint16_t LABELS_W = 253;
static constexpr uint16_t LABELS_H = 53;
static constexpr uint16_t LABELS_FONT = 38;
static constexpr uint16_t LIST_X = 20;
static constexpr uint16_t LIST_Y = 100;
static constexpr uint16_t LIST_W = 415;
static constexpr uint16_t LIST_H = 324;
static constexpr uint16_t LIST_ITEM_W = 415;
static constexpr uint16_t LIST_ITEM_H = 108;
static constexpr uint16_t REBOUND_SIZE = 60;
static constexpr uint16_t BLANK_SIZE = 60;

REGIST_SLICE_PAGE(VIEW_SPORT, SportPages::SPORT_PAGE_SETTINGS, SportSettingsPage, false);

static SportSettingsPage *g_SportSettingsPageInstance = nullptr;

static UIView *CreateViewCb(uint8_t type)
{
    SportSettingsItemView *item = new SportSettingsItemView();
    if (item == nullptr) {
        return nullptr;
    }
    item->SetPosition(0, 0, LIST_ITEM_W, LIST_ITEM_H);
    item->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    item->SetTouchable(true);
    item->SetOnClickListener(g_SportSettingsPageInstance);
    return item;
}

static void UpdateViewCb(UIView *view, void *data, uint8_t type)
{
    if (view == nullptr || data == nullptr) {
        return;
    }
    SportSettingsItemView *item = dynamic_cast<SportSettingsItemView *>(view);
    if (item == nullptr) {
        return;
    }
    SportSettingsItem *itemInfo = static_cast<SportSettingsItem *>(data);
    item->SetItemInfo(*itemInfo);
}

SportSettingsPage::SportSettingsPage()
{
    g_SportSettingsPageInstance = this;
}

SportSettingsPage::~SportSettingsPage()
{
    if (group_ != nullptr) {
        group_->RemoveAll();
        delete group_;
        group_ = nullptr;
    }
    if (background_ != nullptr) {
        delete background_;
        background_ = nullptr;
    }
    if (titleLabel_ != nullptr) {
        delete titleLabel_;
        titleLabel_ = nullptr;
    }
    if (settingList_ != nullptr) {
        delete settingList_;
        settingList_ = nullptr;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SportSettingsPage::~SportSettingsPage");
}

void SportSettingsPage::InitBackgroundView()
{
    background_ = new UIImageView();
    if (background_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportSettingsPage background_ new fail");
        return;
    }
    background_->SetPosition(0, 0);
    LOADIMG::LoadImageViewImg(background_, SPORT_IMAGE, IMAGE_SPORT_BG);
    group_->Add(background_);
}

void SportSettingsPage::CreateUILabel()
{
    titleLabel_ = new UILabel();
    if (titleLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportSettingsPage titleLabel_ new fail");
        return;
    }
    titleLabel_->SetPosition(LABELS_X, LABELS_Y, LABELS_W, LABELS_H);
    titleLabel_->SetText("设置");
    titleLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    titleLabel_->SetFont(BOLD_VECTOR_FONT_FILENAME, LABELS_FONT);
    group_->Add(titleLabel_);
}

void SportSettingsPage::CreateUIList()
{
    settingList_ = new UISimpleList();
    if (settingList_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportSettingsPage settingList_ new fail");
        return;
    }
    settingList_->SetPosition(LIST_X, LIST_Y, LIST_W, LIST_H);
    settingList_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    settingList_->SetBoundaryIntercept(DragEvent::DIRECTION_TOP_TO_BOTTOM, false);
    settingList_->SetBoundaryIntercept(DragEvent::DIRECTION_BOTTOM_TO_TOP, false);

    for (uint8_t i = 0; i < SPORT_SET_TYPE_MAX; ++i) {
        Contents content;
        content.createFunc = CreateViewCb;
        content.updateFunc = UpdateViewCb;
        content.type = 0;
        content.data = reinterpret_cast<void *>(&settings_[i]);
        settingList_->AddContent(content);
    }
    settingList_->RefreshList();
    group_->Add(settingList_);
}

void SportSettingsPage::OnStart(void *data)
{
    group_ = new UIScrollView();
    if (group_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportSettingsPage group_ new fail");
        return;
    }
    group_->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    group_->SetOnDragListener(this);
    group_->SetDraggable(true);
    group_->SetTouchable(true);
    group_->SetReboundSize(REBOUND_SIZE, UIAbstractScroll::Direction::BOTTOM);
    group_->SetScrollBlankSize(BLANK_SIZE, UIAbstractScroll::Direction::BOTTOM);

    InitBackgroundView();
    CreateUILabel();
    CreateUIList();
    AddViewToPageContainer(group_);
}

bool SportSettingsPage::OnDrag(UIView &view, const DragEvent &event)
{
    if ((event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) && (event.GetDeltaX() > X_DRAG_OFFSET) &&
        (event.GetDeltaX() > (DOUBLES * MATH_ABS(event.GetDeltaY())))) {
        NativeAbility::GetInstance().SwitchPageInSlice(SportPages::SPORT_PAGE_MENU,
            TransitionType::TRANSITION_INVALID, false);
    }
    return true;
}

bool SportSettingsPage::OnClick(UIView &view, const ClickEvent &event)
{
    if (strcmp(view.GetViewId(), SETTINGS_ITEM_TARGET) == 0) {
        NativeAbility::GetInstance().SwitchPageInSlice(SportPages::SPORT_PAGE_TARGET,
            TransitionType::TRANSITION_INVALID, false);
    } else if (strcmp(view.GetViewId(), SETTINGS_ITEM_REMIND) == 0) {
        NativeAbility::GetInstance().SwitchPageInSlice(SportPages::SPORT_PAGE_REMIND,
            TransitionType::TRANSITION_INVALID, false);
    } else if (strcmp(view.GetViewId(), SETTINGS_ITEM_MISTOUCH) == 0) {
        NativeAbility::GetInstance().SwitchPageInSlice(SportPages::SPORT_PAGE_MISTOUCH,
            TransitionType::TRANSITION_INVALID, false);
    }
    return true;
}
}
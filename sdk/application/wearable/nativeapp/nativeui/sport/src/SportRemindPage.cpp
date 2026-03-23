/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SportRemindPage
 * Create: 2025-06-06
 */

#include "UiConfig.h"
#include "main/LoadImg.h"
#include "wearable_log.h"
#include "AppViewIDs.h"
#include "NativeAbility.h"
#include "SlicePageFactory.h"
#include "sport/SportView.h"
#include "sport/SportRemindPage.h"

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
static constexpr uint16_t LIST_REBOUND_SIZE = 60;

REGIST_SLICE_PAGE(VIEW_SPORT, SportPages::SPORT_PAGE_REMIND, SportRemindPage, false);

static SportRemindPage *g_SportRemindPageInstance = nullptr;

static UIView *CreateViewCb(uint8_t type)
{
    SportRemindItemView *item = new SportRemindItemView();
    if (item == nullptr) {
        return nullptr;
    }
    item->SetPosition(0, 0, LIST_ITEM_W, LIST_ITEM_H);
    item->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    item->SetTouchable(true);
    item->SetOnClickListener(g_SportRemindPageInstance);
    return item;
}

static void UpdateViewCb(UIView *view, void *data, uint8_t type)
{
    if (view == nullptr || data == nullptr) {
        return;
    }
    SportRemindItemView *item = dynamic_cast<SportRemindItemView *>(view);
    if (item == nullptr) {
        return;
    }
    SportRemindItem *itemInfo = static_cast<SportRemindItem *>(data);
    item->SetItemInfo(*itemInfo);
}

SportRemindPage::SportRemindPage()
{
    g_SportRemindPageInstance = this;
}

SportRemindPage::~SportRemindPage()
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
    if (remindList_ != nullptr) {
        delete remindList_;
        remindList_ = nullptr;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SportRemindPage::~SportRemindPage");
}

void SportRemindPage::InitBackgroundView()
{
    background_ = new UIImageView();
    if (background_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportRemindPage background_ new fail");
        return;
    }
    background_->SetPosition(0, 0);
    LOADIMG::LoadImageViewImg(background_, SPORT_IMAGE, IMAGE_SPORT_BG);
    group_->Add(background_);
}

void SportRemindPage::CreateUILabel()
{
    titleLabel_ = new UILabel();
    if (titleLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportRemindPage titleLabel_ new fail");
        return;
    }
    titleLabel_->SetPosition(LABELS_X, LABELS_Y, LABELS_W, LABELS_H);
    titleLabel_->SetText("提醒");
    titleLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    titleLabel_->SetFont(BOLD_VECTOR_FONT_FILENAME, LABELS_FONT);
    group_->Add(titleLabel_);
}

void SportRemindPage::CreateUIList()
{
    remindList_ = new UISimpleList();
    if (remindList_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportRemindPage remindList_ new fail");
        return;
    }
    remindList_->SetPosition(LIST_X, LIST_Y, LIST_W, LIST_H);
    remindList_->SetThrowDrag(true);
    remindList_->SetDraggable(true);
    remindList_->SetReboundSize(LIST_REBOUND_SIZE);
    remindList_->EnableAutoAlign(true);
    remindList_->SetElastic(true);
    remindList_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);

    for (uint8_t i = 0; i < SPORT_REMIND_MAX; ++i) {
        Contents content;
        content.createFunc = CreateViewCb;
        content.updateFunc = UpdateViewCb;
        content.type = 0;
        content.data = reinterpret_cast<void *>(&reminds_[i]);
        remindList_->AddContent(content);
    }
    remindList_->RefreshList();
    group_->Add(remindList_);
}

void SportRemindPage::OnStart(void *data)
{
    group_ = new UIViewGroup();
    if (group_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportRemindPage group_ new fail");
        return;
    }
    group_->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    group_->SetOnDragListener(this);
    group_->SetDraggable(true);
    group_->SetTouchable(true);

    InitBackgroundView();
    CreateUILabel();
    CreateUIList();
    AddViewToPageContainer(group_);
}

bool SportRemindPage::OnDrag(UIView &view, const DragEvent &event)
{
    if ((event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) && (event.GetDeltaX() > X_DRAG_OFFSET) &&
        (event.GetDeltaX() > (DOUBLES * MATH_ABS(event.GetDeltaY())))) {
            NativeAbility::GetInstance().SwitchPageInSlice(SportPages::SPORT_PAGE_SETTINGS,
                TransitionType::TRANSITION_INVALID, false);
    }
    return true;
}

bool SportRemindPage::OnClick(UIView &view, const ClickEvent &event)
{
    if (strcmp(view.GetViewId(), SPORT_REMIND_ITEM_INTERVAL) == 0) {
        NativeAbility::GetInstance().SwitchPageInSlice(SportPages::SPORT_PAGE_REMIND_INTERVAL,
            TransitionType::TRANSITION_INVALID, false);
    } else if (strcmp(view.GetViewId(), SPORT_REMIND_ITEM_HEARTRATE) == 0) {
        NativeAbility::GetInstance().SwitchPageInSlice(SportPages::SPORT_PAGE_REMIND_HEARTRATE,
            TransitionType::TRANSITION_INVALID, false);
    }
    return true;
}
}
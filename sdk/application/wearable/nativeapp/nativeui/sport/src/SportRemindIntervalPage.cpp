/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SportRemindIntervalPage
 * Create: 2025-06-06
 */

#include "UiConfig.h"
#include "main/LoadImg.h"
#include "wearable_log.h"
#include "AppViewIDs.h"
#include "NativeAbility.h"
#include "SlicePageFactory.h"
#include "sport/SportView.h"
#include "sport/SportRemindIntervalPage.h"

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

REGIST_SLICE_PAGE(VIEW_SPORT, SportPages::SPORT_PAGE_REMIND_INTERVAL, SportRemindIntervalPage, false);

static SportRemindIntervalPage *g_SportRemindIntervalPageInstance = nullptr;

static UIView *CreateViewCb(uint8_t type)
{
    SportRemindIntervalItemView *item = new SportRemindIntervalItemView();
    if (item == nullptr) {
        return nullptr;
    }
    item->SetPosition(0, 0, LIST_ITEM_W, LIST_ITEM_H);
    item->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    item->SetTouchable(true);
    item->SetOnClickListener(g_SportRemindIntervalPageInstance);
    return item;
}

static void UpdateViewCb(UIView *view, void *data, uint8_t type)
{
    if (view == nullptr || data == nullptr) {
        return;
    }
    SportRemindIntervalItemView *item = dynamic_cast<SportRemindIntervalItemView *>(view);
    if (item == nullptr) {
        return;
    }
    SportRemindIntervalItem *itemInfo = static_cast<SportRemindIntervalItem *>(data);
    item->SetItemInfo(*itemInfo);
}

SportRemindIntervalPage::SportRemindIntervalPage()
{
    g_SportRemindIntervalPageInstance = this;
}

SportRemindIntervalPage::~SportRemindIntervalPage()
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
    if (remindIntervalList_ != nullptr) {
        delete remindIntervalList_;
        remindIntervalList_ = nullptr;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SportRemindIntervalPage::~SportRemindIntervalPage");
}

void SportRemindIntervalPage::InitBackgroundView()
{
    background_ = new UIImageView();
    if (background_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportRemindIntervalPage background_ new fail");
        return;
    }
    background_->SetPosition(0, 0);
    LOADIMG::LoadImageViewImg(background_, SPORT_IMAGE, IMAGE_SPORT_BG);
    group_->Add(background_);
}

void SportRemindIntervalPage::CreateUILabel()
{
    titleLabel_ = new UILabel();
    if (titleLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportRemindIntervalPage titleLabel_ new fail");
        return;
    }
    titleLabel_->SetPosition(LABELS_X, LABELS_Y, LABELS_W, LABELS_H);
    titleLabel_->SetText("间隔提醒");
    titleLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    titleLabel_->SetFont(BOLD_VECTOR_FONT_FILENAME, LABELS_FONT);
    group_->Add(titleLabel_);
}

void SportRemindIntervalPage::CreateUIList()
{
    remindIntervalList_ = new UISimpleList();
    if (remindIntervalList_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportRemindIntervalPage remindIntervalList_ new fail");
        return;
    }
    remindIntervalList_->SetPosition(LIST_X, LIST_Y, LIST_W, LIST_H);
    remindIntervalList_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    remindIntervalList_->SetBoundaryIntercept(DragEvent::DIRECTION_TOP_TO_BOTTOM, false);
    remindIntervalList_->SetBoundaryIntercept(DragEvent::DIRECTION_BOTTOM_TO_TOP, false);

    for (uint8_t i = 0; i < SPORT_REMIND_INTERVAL_MAX; ++i) {
        Contents content;
        content.createFunc = CreateViewCb;
        content.updateFunc = UpdateViewCb;
        content.type = 0;
        content.data = reinterpret_cast<void *>(&remindIntervals_[i]);
        remindIntervalList_->AddContent(content);
    }
    remindIntervalList_->RefreshList();
    group_->Add(remindIntervalList_);
}

void SportRemindIntervalPage::OnStart(void *data)
{
    group_ = new UIScrollView();
    if (group_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportRemindIntervalPage group_ new fail");
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

bool SportRemindIntervalPage::OnDrag(UIView &view, const DragEvent &event)
{
    if ((event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) && (event.GetDeltaX() > X_DRAG_OFFSET) &&
        (event.GetDeltaX() > (DOUBLES * MATH_ABS(event.GetDeltaY())))) {
        NativeAbility::GetInstance().SwitchPageInSlice(SportPages::SPORT_PAGE_REMIND,
            TransitionType::TRANSITION_INVALID, false);
    }
    return true;
}

bool SportRemindIntervalPage::OnClick(UIView &view, const ClickEvent &event)
{
    sport_info_t *sportInfo = SportModel::GetInstance().GetCurrSportInfo();
    if (strcmp(view.GetViewId(), SPORT_REMIND_INTERVAL_ITEM_DISTANCE) == 0) {
        sportInfo->setType = SPORT_SET_TYPE_REMIND;
        sportInfo->remindTypeTemp = sportInfo->remindType;
        sportInfo->remindType = SPORT_REMIND_INTERVAL_DISTANCE;
        NativeAbility::GetInstance().SwitchPageInSlice(SportPages::SPORT_PAGE_TARGET_SETTING,
            TransitionType::TRANSITION_INVALID, false);
    } else if (strcmp(view.GetViewId(), SPORT_REMIND_INTERVAL_ITEM_TIME) == 0) {
        sportInfo->setType = SPORT_SET_TYPE_REMIND;
        sportInfo->remindTypeTemp = sportInfo->remindType;
        sportInfo->remindType = SPORT_REMIND_INTERVAL_TIME;
        NativeAbility::GetInstance().SwitchPageInSlice(SportPages::SPORT_PAGE_TARGET_SETTING,
            TransitionType::TRANSITION_INVALID, false);
    } else if (strcmp(view.GetViewId(), SPORT_REMIND_INTERVAL_ITEM_NONE) == 0) {
        sportInfo->remindTypeTemp = sportInfo->remindType;
        sportInfo->remindType = SPORT_REMIND_INTERVAL_NONE;
        SportModel::GetInstance().SaveConfig(SPORT_CONFIG_INFO);
        NativeAbility::GetInstance().SwitchPageInSlice(SportPages::SPORT_PAGE_REMIND,
            TransitionType::TRANSITION_INVALID, false);
    }
    return true;
}
}
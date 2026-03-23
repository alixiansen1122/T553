/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SportRemindHeartRatePage
 * Create: 2025-06-06
 */

#include "UiConfig.h"
#include "wearable_log.h"
#include "AppViewIDs.h"
#include "NativeAbility.h"
#include "SlicePageFactory.h"
#include "sport/SportView.h"
#include "sport/SportRemindHeartRatePage.h"

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

REGIST_SLICE_PAGE(VIEW_SPORT, SportPages::SPORT_PAGE_REMIND_HEARTRATE, SportRemindHeartRatePage, false);

static SportRemindHeartRatePage *g_SportRemindHeartRatePageInstance = nullptr;

static UIView *CreateViewCb(uint8_t type)
{
    SportRemindHeartRateItemView *item = new SportRemindHeartRateItemView();
    if (item == nullptr) {
        return nullptr;
    }
    item->SetPosition(0, 0, LIST_ITEM_W, LIST_ITEM_H);
    item->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    item->SetTouchable(true);
    item->SetOnClickListener(g_SportRemindHeartRatePageInstance);
    return item;
}

static void UpdateViewCb(UIView *view, void *data, uint8_t type)
{
    if (view == nullptr || data == nullptr) {
        return;
    }
    SportRemindHeartRateItemView *item = dynamic_cast<SportRemindHeartRateItemView *>(view);
    if (item == nullptr) {
        return;
    }
    SportRemindHeartRateItem *itemInfo = static_cast<SportRemindHeartRateItem *>(data);
    item->SetItemInfo(*itemInfo);
}

SportRemindHeartRatePage::SportRemindHeartRatePage()
{
    g_SportRemindHeartRatePageInstance = this;
}

SportRemindHeartRatePage::~SportRemindHeartRatePage()
{
    if (group_ != nullptr) {
        group_->RemoveAll();
        delete group_;
        group_ = nullptr;
    }
    if (titleLabel_ != nullptr) {
        delete titleLabel_;
        titleLabel_ = nullptr;
    }
    if (remindHeartRateList_ != nullptr) {
        delete remindHeartRateList_;
        remindHeartRateList_ = nullptr;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SportRemindHeartRatePage::~SportRemindHeartRatePage");
}

void SportRemindHeartRatePage::CreateUILabel()
{
    titleLabel_ = new UILabel();
    if (titleLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportRemindHeartRatePage titleLabel_ new fail");
        return;
    }
    titleLabel_->SetPosition(LABELS_X, LABELS_Y, LABELS_W, LABELS_H);
    titleLabel_->SetText("心率提醒");
    titleLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    titleLabel_->SetFont(BOLD_VECTOR_FONT_FILENAME, LABELS_FONT);
    group_->Add(titleLabel_);
}

void SportRemindHeartRatePage::CreateUIList()
{
    remindHeartRateList_ = new UISimpleList();
    if (remindHeartRateList_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportRemindHeartRatePage remindHeartRateList_ new fail");
        return;
    }
    remindHeartRateList_->SetPosition(LIST_X, LIST_Y, LIST_W, LIST_H);
    remindHeartRateList_->SetThrowDrag(true);
    remindHeartRateList_->SetDraggable(true);
    remindHeartRateList_->SetReboundSize(LIST_REBOUND_SIZE);
    remindHeartRateList_->EnableAutoAlign(true);
    remindHeartRateList_->SetElastic(true);
    remindHeartRateList_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);

    for (uint8_t i = 0; i < SPORT_REMIND_HEARTRATE_MAX; ++i) {
        Contents content;
        content.createFunc = CreateViewCb;
        content.updateFunc = UpdateViewCb;
        content.type = 0;
        content.data = reinterpret_cast<void *>(&remindHeartRates_[i]);
        remindHeartRateList_->AddContent(content);
    }
    remindHeartRateList_->RefreshList();
    group_->Add(remindHeartRateList_);
}

void SportRemindHeartRatePage::OnStart(void *data)
{
    group_ = new UIViewGroup();
    if (group_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportRemindHeartRatePage group_ new fail");
        return;
    }
    group_->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    group_->SetOnDragListener(this);
    group_->SetDraggable(true);
    group_->SetTouchable(true);

    CreateUILabel();
    CreateUIList();
    AddViewToPageContainer(group_);
}

bool SportRemindHeartRatePage::OnDrag(UIView &view, const DragEvent &event)
{
    if ((event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) && (event.GetDeltaX() > X_DRAG_OFFSET) &&
        (event.GetDeltaX() > (DOUBLES * MATH_ABS(event.GetDeltaY())))) {
        NativeAbility::GetInstance().SwitchPageInSlice(SportPages::SPORT_PAGE_REMIND,
            TransitionType::TRANSITION_INVALID, false);
    }
    return true;
}

bool SportRemindHeartRatePage::OnClick(UIView &view, const ClickEvent &event)
{
    if (strcmp(view.GetViewId(), SPORT_REMIND_HEARTRATE_ITEM_ENABLE) == 0) {
        sport_info_t *sportInfo = SportModel::GetInstance().GetCurrSportInfo();
        sportInfo->enableHeartRemind = !sportInfo->enableHeartRemind;
        remindHeartRateList_->RefreshList();
        SportModel::GetInstance().SaveConfig(SPORT_CONFIG_INFO);
    } else if (strcmp(view.GetViewId(), SPORT_REMIND_HEARTRATE_ITEM_RANGE) == 0) {
        NativeAbility::GetInstance().SwitchPageInSlice(SportPages::SPORT_PAGE_HEARTRATE_RANGE,
            TransitionType::TRANSITION_INVALID, false);
    }
    return true;
}
}
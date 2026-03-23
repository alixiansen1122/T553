/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SportHeartRateRangePage
 * Create: 2025-06-06
 */

#include "wearable_log.h"
#include "UiConfig.h"
#include "AppViewIDs.h"
#include "NativeAbility.h"
#include "SlicePageFactory.h"
#include "sport/SportView.h"
#include "sport/SportPresenter.h"
#include "sport/SportHeartRateRangePage.h"

namespace OHOS {
static constexpr uint16_t LABELS_X = 100;
static constexpr uint16_t LABELS_Y = 38;
static constexpr uint16_t LABELS_W = 253;
static constexpr uint16_t LABELS_H = 53;
static constexpr uint16_t LABELS_FONT = 38;
static constexpr uint16_t LIST_X = 20;
static constexpr uint16_t LIST_Y = 100;
static constexpr uint16_t LIST_W = 415;
static constexpr uint16_t LIST_H = 485;
static constexpr uint16_t LIST_ITEM_W = 415;
static constexpr uint16_t LIST_ITEM_H = 108;
static constexpr uint16_t REBOUND_SIZE = 60;
static constexpr uint16_t TOP_BLANK_SIZE = 38;
static constexpr uint16_t BOTTOM_BLANK_SIZE = 60;

REGIST_SLICE_PAGE(VIEW_SPORT, SportPages::SPORT_PAGE_HEARTRATE_RANGE, SportHeartRateRangePage, false);

static SportHeartRateRangePage *g_SportHeartRateRangePageInstance = nullptr;

static UIView *CreateViewCb(uint8_t type)
{
    SportHeartRateRangeItemView *item = new SportHeartRateRangeItemView();
    if (item == nullptr) {
        return nullptr;
    }
    item->SetPosition(0, 0, LIST_ITEM_W, LIST_ITEM_H);
    item->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    item->SetTouchable(true);
    item->SetOnClickListener(g_SportHeartRateRangePageInstance);
    return item;
}

static void UpdateViewCb(UIView *view, void *data, uint8_t type)
{
    if (view == nullptr || data == nullptr) {
        return;
    }
    SportHeartRateRangeItemView *item = dynamic_cast<SportHeartRateRangeItemView *>(view);
    if (item == nullptr) {
        return;
    }
    SportHeartRateRangeItem *itemInfo = static_cast<SportHeartRateRangeItem *>(data);
    item->SetItemInfo(*itemInfo);
}

SportHeartRateRangePage::SportHeartRateRangePage()
{
    g_SportHeartRateRangePageInstance = this;
}

SportHeartRateRangePage::~SportHeartRateRangePage()
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
    if (heartRateRangeList_ != nullptr) {
        delete heartRateRangeList_;
        heartRateRangeList_ = nullptr;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SportHeartRateRangePage::~SportHeartRateRangePage");
}

void SportHeartRateRangePage::CreateUILabel()
{
    titleLabel_ = new UILabel();
    if (titleLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportHeartRateRangePage titleLabel_ new fail");
        return;
    }
    titleLabel_->SetPosition(LABELS_X, LABELS_Y, LABELS_W, LABELS_H);
    titleLabel_->SetText("心率范围");
    titleLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    titleLabel_->SetFont(BOLD_VECTOR_FONT_FILENAME, LABELS_FONT);
    group_->Add(titleLabel_);
}

void SportHeartRateRangePage::CreateUIList()
{
    heartRateRangeList_ = new UISimpleList();
    if (heartRateRangeList_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportHeartRateRangePage heartRateRangeList_ new fail");
        return;
    }
    heartRateRangeList_->SetPosition(LIST_X, LIST_Y, LIST_W, LIST_H);
    heartRateRangeList_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    heartRateRangeList_->SetBoundaryIntercept(DragEvent::DIRECTION_TOP_TO_BOTTOM, false);
    heartRateRangeList_->SetBoundaryIntercept(DragEvent::DIRECTION_BOTTOM_TO_TOP, false);

    for (uint8_t i = 0; i < SPORT_HEARTRATE_RANGE_MAX; ++i) {
        Contents content;
        content.createFunc = CreateViewCb;
        content.updateFunc = UpdateViewCb;
        content.type = 0;
        content.data = reinterpret_cast<void *>(&heartRateRanges_[i]);
        heartRateRangeList_->AddContent(content);
    }
    heartRateRangeList_->RefreshList();
    group_->Add(heartRateRangeList_);
}

void SportHeartRateRangePage::OnStart(void *data)
{
    group_ = new UIScrollView();
    if (group_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportHeartRateRangePage group_ new fail");
        return;
    }
    group_->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    group_->SetOnDragListener(this);
    group_->SetDraggable(true);
    group_->SetTouchable(true);
    group_->SetReboundSize(REBOUND_SIZE, UIAbstractScroll::Direction::BOTTOM);
    group_->SetScrollBlankSize(TOP_BLANK_SIZE, UIAbstractScroll::Direction::TOP);
    group_->SetScrollBlankSize(BOTTOM_BLANK_SIZE, UIAbstractScroll::Direction::BOTTOM);

    CreateUILabel();
    CreateUIList();
    AddViewToPageContainer(group_);
}

bool SportHeartRateRangePage::OnDrag(UIView &view, const DragEvent &event)
{
    if ((event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) && (event.GetDeltaX() > X_DRAG_OFFSET) &&
        (event.GetDeltaX() > (DOUBLES * MATH_ABS(event.GetDeltaY())))) {
        NativeAbility::GetInstance().SwitchPageInSlice(SportPages::SPORT_PAGE_REMIND_HEARTRATE,
            TransitionType::TRANSITION_INVALID, false);
    }
    return true;
}

bool SportHeartRateRangePage::OnClick(UIView &view, const ClickEvent &event)
{
    sport_info_t *sportInfo = SportModel::GetInstance().GetCurrSportInfo();
    if (strcmp(view.GetViewId(), SPORT_HEARTRATE_RANGE_ITEM_1) == 0) {
        sportInfo->heartRange = SPORT_HEARTRATE_RANGE_ITEM1;
    } else if (strcmp(view.GetViewId(), SPORT_HEARTRATE_RANGE_ITEM_2) == 0) {
        sportInfo->heartRange = SPORT_HEARTRATE_RANGE_ITEM2;
    } else if (strcmp(view.GetViewId(), SPORT_HEARTRATE_RANGE_ITEM_3) == 0) {
        sportInfo->heartRange = SPORT_HEARTRATE_RANGE_ITEM3;
    } else if (strcmp(view.GetViewId(), SPORT_HEARTRATE_RANGE_ITEM_4) == 0) {
        sportInfo->heartRange = SPORT_HEARTRATE_RANGE_ITEM4;
    }
    heartRateRangeList_->RefreshList();
    SportModel::GetInstance().SaveConfig(SPORT_CONFIG_INFO);
    NativeAbility::GetInstance().SwitchPageInSlice(SportPages::SPORT_PAGE_REMIND_HEARTRATE,
        TransitionType::TRANSITION_INVALID, false);
    return true;
}
}
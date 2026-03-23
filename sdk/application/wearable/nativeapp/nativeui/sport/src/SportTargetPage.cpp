/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SportTargetPage
 * Create: 2025-06-06
 */

#include "UiConfig.h"
#include "main/LoadImg.h"
#include "wearable_log.h"
#include "AppViewIDs.h"
#include "NativeAbility.h"
#include "SlicePageFactory.h"
#include "sport/SportView.h"
#include "sport/SportTargetPage.h"

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

REGIST_SLICE_PAGE(VIEW_SPORT, SportPages::SPORT_PAGE_TARGET, SportTargetPage, false);

static SportTargetPage *g_SportTargetPageInstance = nullptr;

static UIView *CreateViewCb(uint8_t type)
{
    SportTargetItemView *item = new SportTargetItemView();
    if (item == nullptr) {
        return nullptr;
    }
    item->SetPosition(0, 0, LIST_ITEM_W, LIST_ITEM_H);
    item->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    item->SetTouchable(true);
    item->SetOnClickListener(g_SportTargetPageInstance);
    return item;
}

static void UpdateViewCb(UIView *view, void *data, uint8_t type)
{
    if (view == nullptr || data == nullptr) {
        return;
    }
    SportTargetItemView *item = dynamic_cast<SportTargetItemView *>(view);
    if (item == nullptr) {
        return;
    }
    SportTargetItem *itemInfo = static_cast<SportTargetItem *>(data);
    item->SetItemInfo(*itemInfo);
}

SportTargetPage::SportTargetPage()
{
    g_SportTargetPageInstance = this;
}

SportTargetPage::~SportTargetPage()
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
    if (targetList_ != nullptr) {
        delete targetList_;
        targetList_ = nullptr;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SportTargetPage::~SportTargetPage");
}

void SportTargetPage::InitBackgroundView()
{
    background_ = new UIImageView();
    if (background_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportTargetPage background_ new fail");
        return;
    }
    background_->SetPosition(0, 0);
    LOADIMG::LoadImageViewImg(background_, SPORT_IMAGE, IMAGE_SPORT_BG);
    group_->Add(background_);
}

void SportTargetPage::CreateUILabel()
{
    titleLabel_ = new UILabel();
    if (titleLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportTargetPage titleLabel_ new fail");
        return;
    }
    titleLabel_->SetPosition(LABELS_X, LABELS_Y, LABELS_W, LABELS_H);
    titleLabel_->SetText("目标");
    titleLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    titleLabel_->SetFont(BOLD_VECTOR_FONT_FILENAME, LABELS_FONT);
    group_->Add(titleLabel_);
}

void SportTargetPage::CreateUIList()
{
    targetList_ = new UISimpleList();
    if (targetList_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportTargetPage targetList_ new fail");
        return;
    }
    targetList_->SetPosition(LIST_X, LIST_Y, LIST_W, LIST_H);
    targetList_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    targetList_->SetBoundaryIntercept(DragEvent::DIRECTION_TOP_TO_BOTTOM, false);
    targetList_->SetBoundaryIntercept(DragEvent::DIRECTION_BOTTOM_TO_TOP, false);

    for (uint8_t i = 0; i < SPORT_TARGET_MAX; ++i) {
        Contents content;
        content.createFunc = CreateViewCb;
        content.updateFunc = UpdateViewCb;
        content.type = 0;
        content.data = reinterpret_cast<void *>(&targets_[i]);
        targetList_->AddContent(content);
    }
    targetList_->RefreshList();
    group_->Add(targetList_);
}

void SportTargetPage::OnStart(void *data)
{
    group_ = new UIScrollView();
    if (group_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportTargetPage group_ new fail");
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

bool SportTargetPage::OnDrag(UIView &view, const DragEvent &event)
{
    if ((event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) && (event.GetDeltaX() > X_DRAG_OFFSET) &&
        (event.GetDeltaX() > (DOUBLES * MATH_ABS(event.GetDeltaY())))) {
        NativeAbility::GetInstance().SwitchPageInSlice(SportPages::SPORT_PAGE_SETTINGS,
            TransitionType::TRANSITION_INVALID, false);
    }
    return true;
}

bool SportTargetPage::OnClick(UIView &view, const ClickEvent &event)
{
    sport_info_t *sportInfo = SportModel::GetInstance().GetCurrSportInfo();
    sportInfo->setType = SPORT_SET_TYPE_TARGET;
    if (strcmp(view.GetViewId(), SPORT_TARGET_ITEM_DISTANCE) == 0) {
        sportInfo->targetTypeTemp = sportInfo->targetType;
        sportInfo->targetType = SPORT_TARGET_DISTANCE;
    } else if (strcmp(view.GetViewId(), SPORT_TARGET_ITEM_TIME) == 0) {
        sportInfo->targetTypeTemp = sportInfo->targetType;
        sportInfo->targetType = SPORT_TARGET_TIME;
    } else if (strcmp(view.GetViewId(), SPORT_TARGET_ITEM_CALORIE) == 0) {
        sportInfo->targetTypeTemp = sportInfo->targetType;
        sportInfo->targetType = SPORT_TARGET_CALORIE;
    }
    NativeAbility::GetInstance().SwitchPageInSlice(SportPages::SPORT_PAGE_TARGET_SETTING,
        TransitionType::TRANSITION_INVALID, false);
    return true;
}
}
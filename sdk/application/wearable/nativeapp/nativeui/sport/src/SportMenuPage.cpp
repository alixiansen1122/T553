/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SportMenuPage
 * Create: 2025-06-06
 */

#include "UiConfig.h"
#include "main/LoadImg.h"
#include "wearable_log.h"
#include "AppViewIDs.h"
#include "NativeAbility.h"
#include "SlicePageFactory.h"
#include "sport/SportView.h"
#include "sport/SportMenuPage.h"

namespace OHOS {
static constexpr uint16_t TITLI_LABEL_FONT = 38;
static constexpr uint16_t TITLI_LABEL_X = 100;
static constexpr uint16_t TITLI_LABEL_Y = 38;
static constexpr uint16_t TITLI_LABEL_W = 253;
static constexpr uint16_t TITLI_LABEL_H = 53;
static constexpr uint16_t LIST_X = 19;
static constexpr uint16_t LIST_Y = 108;
static constexpr uint16_t LIST_W = 415;
static constexpr uint16_t LIST_H = 780;
static constexpr uint16_t LIST_ITEM_W = 415;
static constexpr uint16_t LIST_ITEM_H = 129;
static constexpr uint16_t REBOUND_SIZE = 60;
static constexpr uint16_t BLANK_SIZE = 100;
static constexpr uint16_t SETTINGS_START_XPOS = 364;

REGIST_SLICE_PAGE(VIEW_SPORT, SportPages::SPORT_PAGE_MENU, SportMenuPage, true);

static SportMenuPage *g_SportMenuPageInstance = nullptr;

static UIView *CreateViewCb(uint8_t type)
{
    SportMenuItemView *item = new SportMenuItemView();
    if (item == nullptr) {
        return nullptr;
    }
    item->SetPosition(0, 0, LIST_ITEM_W, LIST_ITEM_H);
    item->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    item->SetTouchable(true);
    item->SetOnClickListener(g_SportMenuPageInstance);
    return item;
}

static void UpdateViewCb(UIView *view, void *data, uint8_t type)
{
    if (view == nullptr || data == nullptr) {
        return;
    }
    SportMenuItemView *item = dynamic_cast<SportMenuItemView *>(view);
    if (item == nullptr) {
        return;
    }
    SportMenuItem *itemInfo = static_cast<SportMenuItem *>(data);
    item->SetItemInfo(*itemInfo);
}

SportMenuPage::SportMenuPage()
{
    g_SportMenuPageInstance = this;
}

SportMenuPage::~SportMenuPage()
{
    bool ret = ImageCacheManager::GetInstance().UnloadAllInMultiRes(SPORT_IMAGE);
    if (ret == false) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "UnloadSportImage fail");
    }

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
    if (sportList_ != nullptr) {
        delete sportList_;
        sportList_ = nullptr;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SportMenuPage::~SportMenuPage");
}

void SportMenuPage::InitBackgroundView()
{
    background_ = new UIImageView();
    if (background_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportMenuPage background_ new fail");
        return;
    }
    background_->SetPosition(0, 0);
    LOADIMG::LoadImageViewImg(background_, SPORT_IMAGE, IMAGE_SPORT_BG);
    group_->Add(background_);
}

void SportMenuPage::CreateUILabel()
{
    titleLabel_ = new UILabel();
    if (titleLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportMenuPage titleLabel_ new fail");
        return;
    }

    titleLabel_->SetPosition(TITLI_LABEL_X, TITLI_LABEL_Y, TITLI_LABEL_W, TITLI_LABEL_H);
    titleLabel_->SetText("运动");
    titleLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    titleLabel_->SetFont(BOLD_VECTOR_FONT_FILENAME, TITLI_LABEL_FONT);
    group_->Add(titleLabel_);
}

void SportMenuPage::CreateUIList()
{
    sportList_ = new UISimpleList();
    if (sportList_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportMenuPage sportList_ new fail");
        return;
    }

    sportList_->SetPosition(LIST_X, LIST_Y, LIST_W, LIST_H);
    sportList_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    sportList_->SetBoundaryIntercept(DragEvent::DIRECTION_TOP_TO_BOTTOM, false);
    sportList_->SetBoundaryIntercept(DragEvent::DIRECTION_BOTTOM_TO_TOP, false);

    for (uint8_t i = 0; i < SPORT_TYPE_MAX; ++i) {
        Contents content;
        content.createFunc = CreateViewCb;
        content.updateFunc = UpdateViewCb;
        content.type = 0;
        content.data = reinterpret_cast<void *>(&sportMenu_[i]);
        sportList_->AddContent(content);
    }
    sportList_->RefreshList();
    group_->Add(sportList_);
}

void SportMenuPage::OnStart(void *data)
{
    bool ret = ImageCacheManager::GetInstance().LoadAllInMultiRes(SPORT_IMAGE);
    if (ret == false) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "LoadSportImage fail");
        return;
    }
    group_ = new UIScrollView();
    if (group_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SportMenuPage group_ new fail");
        return;
    }
    group_->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    group_->SetOnDragListener(this);
    group_->SetDraggable(true);
    group_->SetTouchable(true);
    group_->SetYScrollBarVisible(true);
    group_->SetReboundSize(REBOUND_SIZE, UIAbstractScroll::Direction::BOTTOM);
    group_->SetScrollBlankSize(BLANK_SIZE, UIAbstractScroll::Direction::BOTTOM);

    InitBackgroundView();
    CreateUILabel();
    CreateUIList();
    AddViewToPageContainer(group_);
}

bool SportMenuPage::OnDrag(UIView &view, const DragEvent &event)
{
    if ((event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) && (event.GetDeltaX() > X_DRAG_OFFSET) &&
        (event.GetDeltaX() > (DOUBLES * MATH_ABS(event.GetDeltaY())))) {
        NativeAbility::GetInstance().ChangeSliceToApplist();
    }
    return true;
}

bool SportMenuPage::OnClick(UIView &view, const ClickEvent &event)
{
    if (strcmp(view.GetViewId(), SPORT_ITEM_OUTDOOR_RUNNING) == 0) {
        SportModel::GetInstance().SetSportType(SPORT_TYPE_OUTDOOR_RUNNING);
    } else if (strcmp(view.GetViewId(), SPORT_ITEM_INDOOR_RUNNING) == 0) {
        SportModel::GetInstance().SetSportType(SPORT_TYPE_INDOOR_RUNNING);
    } else if (strcmp(view.GetViewId(), SPORT_ITEM_CYCLING) == 0) {
        SportModel::GetInstance().SetSportType(SPORT_TYPE_CYCLING);
    } else if (strcmp(view.GetViewId(), SPORT_ITEM_CLIMBING) == 0) {
        SportModel::GetInstance().SetSportType(SPORT_TYPE_CLIMBING);
    } else if (strcmp(view.GetViewId(), SPORT_ITEM_INDOOR_SWIMMING) == 0) {
        SportModel::GetInstance().SetSportType(SPORT_TYPE_INDOOR_SWIMMING);
    } else if (strcmp(view.GetViewId(), SPORT_ITEM_OUTDOOR_SWIMMING) == 0) {
        SportModel::GetInstance().SetSportType(SPORT_TYPE_OUTDOOR_SWIMMING);
    }

    bool isSettingClicked = event.GetCurrentPos().x > SETTINGS_START_XPOS ? true : false;
    if (isSettingClicked) {
        NativeAbility::GetInstance().SwitchPageInSlice(SportPages::SPORT_PAGE_SETTINGS,
            TransitionType::TRANSITION_INVALID, false);
    } else {
        NativeAbility::GetInstance().SwitchPageInSlice(SportPages::SPORT_PAGE_PREPARE,
            TransitionType::TRANSITION_INVALID, false);
    }
    return true;
}
}
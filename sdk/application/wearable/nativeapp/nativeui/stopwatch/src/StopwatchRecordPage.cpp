/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: StopwatchRecordPage
 * Create: 2025-06-06
 */

#include "UiConfig.h"
#include "main/LoadImg.h"
#include "wearable_log.h"
#include "AppViewIDs.h"
#include "NativeAbility.h"
#include "SlicePageFactory.h"
#include "stopwatch/StopwatchView.h"
#include "stopwatch/StopwatchRecordPage.h"

namespace OHOS {
static constexpr uint16_t LABEL_X = 100;
static constexpr uint16_t LABEL_Y = 38;
static constexpr uint16_t LABEL_W = 253;
static constexpr uint16_t LABEL_H = 53;
static constexpr uint16_t LABEL_FONT = 38;
static constexpr uint16_t LIST_X = 73;
static constexpr uint16_t LIST_Y = 100;
static constexpr uint16_t LIST_W = 312;
static constexpr uint16_t LIST_H = 364;
static constexpr uint16_t LIST_ITEM_WIDTH = 312;
static constexpr uint16_t LIST_ITEM_HEIGHT = 60;
static constexpr uint16_t LIST_REBOUND_SIZE = 60;
static constexpr uint16_t LIST_BLANK_SIZE = 80;

REGIST_SLICE_PAGE(VIEW_STOPWATCH, STOPWATCH_PAGES::STOPWATCH_RECORD_PAGE, StopwatchRecordPage, false);

static UIView *CreateViewCb(uint8_t type)
{
    StopwatchItemView *item = new StopwatchItemView();
    if (item == nullptr) {
        return nullptr;
    }
    item->SetPosition(0, 0, LIST_ITEM_WIDTH, LIST_ITEM_HEIGHT);
    item->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    item->SetTouchable(true);
    return item;
}

static void UpdateViewCb(UIView *view, void *data, uint8_t type)
{
    if (view == nullptr || data == nullptr) {
        return;
    }
    StopwatchItemView *item = dynamic_cast<StopwatchItemView *>(view);
    if (item == nullptr) {
        return;
    }
    StopwatchItem *itemInfo = static_cast<StopwatchItem *>(data);
    item->SetItemInfo(*itemInfo);
}

StopwatchRecordPage::~StopwatchRecordPage()
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
    if (recordList_ != nullptr) {
        delete recordList_;
        recordList_ = nullptr;
    }
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "StopwatchRecordPage::~StopwatchRecordPage");
}

void StopwatchRecordPage::InitBackgroundView()
{
    background_ = new UIImageView();
    if (background_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "StopwatchRecordPage background_ new fail");
        return;
    }
    background_->SetPosition(0, 0);
    LOADIMG::LoadImageViewImg(background_, STOPWATCH_IMAGE, IMAGE_STOPWATCH_BG);
    group_->Add(background_);
}

void StopwatchRecordPage::CreateUILabel()
{
    titleLabel_ = new UILabel();
    if (titleLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "StopwatchRecordPage titleLabel_ new fail");
        return;
    }
    titleLabel_->SetPosition(LABEL_X, LABEL_Y, LABEL_W, LABEL_H);
    titleLabel_->SetText("分段详情");
    titleLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    titleLabel_->SetFont(BOLD_VECTOR_FONT_FILENAME, LABEL_FONT);
    group_->Add(titleLabel_);
}

void StopwatchRecordPage::CreateUIList()
{
    recordList_ = new UISimpleList();
    if (recordList_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "StopwatchRecordPage recordList_ new fail");
        return;
    }
    recordList_->SetPosition(LIST_X, LIST_Y, LIST_W, LIST_H);
    recordList_->SetThrowDrag(true);
    recordList_->SetDraggable(true);
    recordList_->SetReboundSize(LIST_REBOUND_SIZE);
    recordList_->EnableAutoAlign(true);
    recordList_->SetElastic(true);
    recordList_->SetScrollBlankSize(LIST_BLANK_SIZE, UIAbstractScroll::Direction::BOTTOM);
    recordList_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);

    auto &records = StopwatchModel::GetInstance().GetRecordsList();
    ListNode<StopwatchItem> *node = records.Head();
    while (node != records.End()) {
        Contents content;
        content.createFunc = CreateViewCb;
        content.updateFunc = UpdateViewCb;
        content.type = 0;
        StopwatchItem &itemData = node->data_;
        content.data = reinterpret_cast<void *>(&itemData);
        recordList_->AddContent(content);
        node = node->next_;
    }
    recordList_->RefreshList();
    group_->Add(recordList_);
}

void StopwatchRecordPage::OnStart(void *data)
{
    group_ = new UIViewGroup();
    if (group_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "StopwatchRecordPage group_ new fail");
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

bool StopwatchRecordPage::OnDrag(UIView &view, const DragEvent &event)
{
    if ((event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) && (event.GetDeltaX() > X_DRAG_OFFSET) &&
        (event.GetDeltaX() > (DOUBLES * MATH_ABS(event.GetDeltaY())))) {
        NativeAbility::GetInstance().SwitchPageInSlice(STOPWATCH_PAGES::STOPWATCH_MAIN_PAGE,
            TransitionType::TRANSITION_INVALID, false);
    }
    return true;
}

bool StopwatchRecordPage::OnClick(UIView &view, const ClickEvent &event)
{
    return false;
}
}
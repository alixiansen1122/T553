/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: DateMessagePage
 * Create: 2025-03-15
 */

#include "NativeAbility.h"
#include "SlicePageFactory.h"
#include "UiConfig.h"
#include "common/image_cache_manager.h"
#include "components/ui_list_nested.h"
#include "components/ui_scroll_view_nested.h"
#include "ui_resource_date.h"
#include "date/DateModel.h"
#include "date/DatePresenter.h"
#include "date/DateView.h"
#include "date/DateMessagePage.h"

namespace OHOS {
static constexpr uint16_t DATE_MONTH_WIDTH = 221;
static constexpr uint16_t DATE_MESSAGE_WIDTH = 180;
static constexpr uint16_t DATE_MESSAGE_HEIGHT = 50;
static constexpr uint16_t DATE_MESSAGE_POSITON_X = 117;
static constexpr uint16_t DATE_MESSAGE_POSITON_Y = 30;
static constexpr uint16_t DATE_MESSAGE_POSITON_X1 = 137;
static constexpr uint16_t DATE_MESSAGE_FRONT_SIZE_ONE = 36;
static constexpr uint16_t DATE_MESSAGE_FRONT_SIZE_TWO = 32;
static constexpr uint16_t DATE_MESSAGE_TWO_POSITION_X = 31;
static constexpr uint16_t DATE_MESSAGE_TWO_POSITION_Y = 120;
static constexpr uint16_t DATE_MESSAGE_TWO_WIDTH = 392;
static constexpr uint16_t DATE_MESSAGE_TWO_HEIGHT = 120;
static constexpr uint16_t DATE_MESSAGE_MAX_LINE_HEIGHT = 140;
static constexpr uint16_t DATE_MESSAGE_ADDRESS_WIDTH = 392;
static constexpr uint16_t DATE_MESSAGE_ADDRESS_HEIGHT = 120;
static constexpr uint16_t DATE_MESSAGE_TIME_POS_Y = 400;
static constexpr uint16_t DATE_MESSAGE_BLANK_SIZE = 30;

REGIST_SLICE_PAGE(VIEW_DATE, DATE_PAGES::DATE_MESSAGE_PAGE, DateMessagePage, false);

DateMessagePage::DateMessagePage()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "DateMessagePage::DateMessagePage");
}

DateMessagePage::~DateMessagePage()
{
    if (group_ != nullptr) {
        group_->RemoveAll();
        delete group_;
        group_ = nullptr;
    }

    if (bgImage_ != nullptr) {
        delete bgImage_;
        bgImage_ = nullptr;
    }

    if (labelTitle_ != nullptr) {
        delete labelTitle_;
        labelTitle_ = nullptr;
    }

    if (labelMessage_ != nullptr) {
        delete labelMessage_;
        labelMessage_ = nullptr;
    }

    if (labelTime_ != nullptr) {
        delete labelTime_;
        labelTime_ = nullptr;
    }

    if (labelAddress_ != nullptr) {
        delete labelAddress_;
        labelAddress_ = nullptr;
    }

    if (DateModel::GetInstance().resetPage_ == PAGE_STATUS_0) {
        DateModel::GetInstance().selectButton_ = "";
    }
    ImageCacheManager::GetInstance().UnloadAllInMultiRes(DATE_IMAGE);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "DateMessagePage::~DateMessagePage");
}

void DateMessagePage::OnStart(void *data)
{
    DateModel::GetInstance().resetPage_ = PAGE_STATUS_0;
    group_ = new UIScrollView();
    group_->SetPosition(0, 0);
    group_->Resize(HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    group_->SetThrowDrag(true);
    group_->SetScrollBlankSize(DATE_MESSAGE_BLANK_SIZE);
    group_->SetOnDragListener(this);
    group_->SetDraggable(true);
    group_->SetTouchable(true);
    group_->SetStyle(STYLE_BACKGROUND_OPA, 0);
    bgImage_ = new UIImageView;
    if (bgImage_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "bgImage_ is nullptr");
        return;
    }
    bgImage_->SetPosition(0, 0);
    bgImage_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    ImageInfo *imgSrc = ImageCacheManager::GetInstance().LoadOneInMultiRes(IMAGE_DATE_BG2, DATE_IMAGE);
    if (imgSrc == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "imgSrc fail");
        return;
    }
    bgImage_->SetSrc(imgSrc);
    InitLabel();
    AddViewToPageContainer(bgImage_);
    AddViewToPageContainer(group_);
}

void DateMessagePage::InitLabel()
{
    labelTitle_ = new UILabel();
    if (labelTitle_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "title is nullptr");
        return;
    }
    labelTitle_->SetPosition(DATE_MESSAGE_POSITON_X, DATE_MESSAGE_POSITON_Y, DATE_MONTH_WIDTH, DATE_MESSAGE_HEIGHT);
    labelTitle_->SetStyle(STYLE_TEXT_COLOR, Color::Purple().full);
    labelTitle_->SetFont(BOLD_VECTOR_FONT_FILENAME, DATE_MESSAGE_FRONT_SIZE_ONE);
    labelTitle_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    labelTitle_->SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
    labelTitle_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    std::string strTitle = DateModel::GetInstance().titleName_;
    labelTitle_->SetText(strTitle.c_str());
    group_->Add(labelTitle_);

    labelMessage_ = new UILabel();
    if (labelMessage_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "labelMessage_ is nullptr");
        return;
    }
    labelMessage_->SetPosition(DATE_MESSAGE_TWO_POSITION_X, DATE_MESSAGE_TWO_POSITION_Y, DATE_MESSAGE_TWO_WIDTH,
                              DATE_MESSAGE_TWO_HEIGHT);
    labelMessage_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    labelMessage_->SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
    labelMessage_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    labelMessage_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    labelMessage_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, DATE_MESSAGE_FRONT_SIZE_ONE);
    labelMessage_->SetLineBreakMode(UILabel::LINE_BREAK_ELLIPSIS);
    labelMessage_->SetRollSpeed(0);
    group_->Add(labelMessage_);

    labelTime_ = new UILabel();
    if (labelTime_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "labelTime_ is nullptr");
        return;
    }
    labelTime_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    labelTime_->SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
    labelTime_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    labelTime_->SetStyle(STYLE_TEXT_COLOR, Color::Gray().full);
    labelTime_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, DATE_MESSAGE_FRONT_SIZE_TWO);
    group_->Add(labelTime_);

    labelAddress_ = new UILabel();
    if (labelAddress_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "labelTime_ is nullptr");
        return;
    }
    labelAddress_->SetPosition(DATE_MESSAGE_TWO_POSITION_X, DATE_MESSAGE_TWO_POSITION_Y, DATE_MESSAGE_ADDRESS_WIDTH,
                              DATE_MESSAGE_ADDRESS_HEIGHT);
    labelAddress_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    labelAddress_->SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
    labelAddress_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    labelAddress_->SetStyle(STYLE_TEXT_COLOR, Color::Gray().full);
    labelAddress_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, DATE_MESSAGE_FRONT_SIZE_TWO);
    group_->Add(labelAddress_);

    for (const auto &item : DateModel::GetInstance().listDate_) {
        std::string index = DateModel::GetInstance().messageIndex_;
        if (!strcmp(index.c_str(), item.index)) {
            labelTime_->SetText(item.title);
            labelMessage_->SetText(item.describetion);
            labelAddress_->SetText(item.address);
            break;
        }
    }
    int posTextHeight = labelMessage_->GetTextHeight();
    int posHeight = DATE_MESSAGE_TWO_POSITION_Y;
    labelMessage_->SetY(DATE_MESSAGE_TWO_POSITION_Y);
    labelMessage_->SetHeight(posTextHeight);
    int pos = 0;
    int offset = 10;
    int pageHeight;
    pos = DATE_MESSAGE_TWO_POSITION_Y + posTextHeight + offset;
    if (labelAddress_->GetText() != nullptr && (!strcmp(labelAddress_->GetText(), "no location"))) {
        labelAddress_->SetVisible(false);
        int pageH = 454;
        int half = 2;
        int posY = (pageH - posTextHeight) / half;
        labelMessage_->SetY(posY);
        labelTime_->SetPosition(DATE_MESSAGE_POSITON_X1, pos, DATE_MESSAGE_WIDTH, DATE_MESSAGE_HEIGHT);
        pageHeight = labelMessage_->GetY() + labelMessage_->GetHeight();
    } else {
        int addressHeight = labelAddress_->GetTextHeight();
        labelAddress_->SetY(pos);
        labelAddress_->SetHeight(labelAddress_->GetTextHeight());
        pos = labelAddress_->GetY() + labelAddress_->GetHeight() + offset;
        labelTime_->SetPosition(DATE_MESSAGE_POSITON_X1, pos, DATE_MESSAGE_WIDTH, DATE_MESSAGE_HEIGHT);
        pageHeight = labelAddress_->GetY() + labelAddress_->GetHeight();
    }
    if (pageHeight <= DATE_MESSAGE_TIME_POS_Y) {
        int timeOffsetY = 364;
        labelTime_->SetY(timeOffsetY);
    }
}

bool DateMessagePage::OnDrag(UIView &view, const DragEvent &event)
{
    UNUSED(view);
    if ((event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) && (event.GetDeltaX() > X_DRAG_OFFSET) &&
        (event.GetDeltaX() > (DOUBLES * MATH_ABS(event.GetDeltaY())))) {
        DateModel::GetInstance().resetPage_ = PAGE_STATUS_2;
        NativeAbility::GetInstance().SwitchPageInSlice(DATE_PAGES::DATE_MAIN_PAGE,
            TransitionType::TRANSITION_INVALID, false);
    }
    return true;
}

}
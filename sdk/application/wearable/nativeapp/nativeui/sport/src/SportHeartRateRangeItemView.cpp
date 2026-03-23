/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SportHeartRateRangeItemView
 * Create: 2025-06-06
 */

#include "wearable_log.h"
#include "UiConfig.h"
#include "main/LoadImg.h"
#include "ui_resource_sport.h"
#include "sport/SportModel.h"
#include "sport/SportHeartRateRangeItemView.h"

namespace OHOS {
static constexpr uint16_t BG_X = 0;
static constexpr uint16_t BG_Y = 6;
static constexpr uint16_t TITLE_X = 39;
static constexpr uint16_t TITLE_Y = 17;
static constexpr uint16_t TITLE_W = 200;
static constexpr uint16_t TITLE_H = 29;
static constexpr uint16_t TITLE_FONT = 24;
static constexpr uint16_t RANGE_X = 39;
static constexpr uint16_t RANGE_Y = 50;
static constexpr uint16_t RANGE_W = 264;
static constexpr uint16_t RANGE_H = 39;
static constexpr uint16_t RANGE_FONT = 32;
static constexpr uint16_t BUTTON_X = 350;
static constexpr uint16_t BUTTON_Y = 32;
static constexpr uint16_t BUTTON_W = 44;
static constexpr uint16_t BUTTON_H = 44;

SportHeartRateRangeItemView::SportHeartRateRangeItemView()
{
    bgImage_.SetPosition(BG_X, BG_Y);
    LOADIMG::LoadImageViewImg(&bgImage_, SPORT_IMAGE, IMAGE_SPORT_ITEMBG);

    title_.SetPosition(TITLE_X, TITLE_Y, TITLE_W, TITLE_H);
    title_.SetFont(DEFAULT_VECTOR_FONT_FILENAME, TITLE_FONT);
    title_.SetAlign(TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_CENTER);
    title_.SetStyle(STYLE_TEXT_COLOR, Color::Gray().full);
    title_.SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);

    range_.SetPosition(RANGE_X, RANGE_Y, RANGE_W, RANGE_H);
    range_.SetFont(DEFAULT_VECTOR_FONT_FILENAME, RANGE_FONT);
    range_.SetAlign(TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_CENTER);
    range_.SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);

    button_.SetPosition(BUTTON_X, BUTTON_Y, BUTTON_W, BUTTON_H);
    button_.SetImages(ImageCacheManager::GetInstance().LoadOneInMultiRes(IMAGE_SPORT_RADIO1, SPORT_IMAGE),
                      ImageCacheManager::GetInstance().LoadOneInMultiRes(IMAGE_SPORT_RADIO0, SPORT_IMAGE));

    Add(&bgImage_);
    Add(&title_);
    Add(&range_);
    Add(&button_);
}

SportHeartRateRangeItemView::~SportHeartRateRangeItemView()
{
    Remove(&bgImage_);
    Remove(&title_);
    Remove(&range_);
    Remove(&button_);
}

void SportHeartRateRangeItemView::SetItemInfo(const SportHeartRateRangeItem &itemInfo)
{
    title_.SetText(itemInfo.title);
    range_.SetText(GetHeartRateRangeDescription(itemInfo.itemId));
    sport_info_t *sportInfo = SportModel::GetInstance().GetCurrSportInfo();
    if (sportInfo->heartRange == itemInfo.itemId) {
        button_.SetState(UICheckBox::UICheckBoxState::SELECTED);
    } else {
        button_.SetState(UICheckBox::UICheckBoxState::UNSELECTED);
    }
    SetViewId(itemInfo.viewId);
}
}
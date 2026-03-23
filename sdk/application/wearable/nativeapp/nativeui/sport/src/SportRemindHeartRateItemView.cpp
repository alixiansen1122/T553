/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SportRemindHeartRateItemView
 * Create: 2025-06-06
 */

#include "main/LoadImg.h"
#include "ui_resource_sport.h"
#include "wearable_log.h"
#include "UiConfig.h"
#include "sport/SportModel.h"
#include "sport/SportRemindHeartRateItemView.h"

namespace OHOS {
static constexpr uint16_t BG_X = 0;
static constexpr uint16_t BG_Y = 6;
static constexpr uint16_t ICON_X = 8;
static constexpr uint16_t ICON_Y = 13;
static constexpr uint16_t LABEL_X = 100;
static constexpr uint16_t LABEL_Y = 17;
static constexpr uint16_t LABEL_W = 207;
static constexpr uint16_t LABEL_H = 39;
static constexpr uint16_t LABEL_FONT = 32;
static constexpr uint16_t HITS_X = 100;
static constexpr uint16_t HITS_Y = 60;
static constexpr uint16_t HITS_W = 176;
static constexpr uint16_t HITS_H = 29;
static constexpr uint16_t HITS_FONT = 24;
static constexpr uint16_t IMAGE_X = 370;
static constexpr uint16_t IMAGE_Y = 38;
static constexpr uint16_t BUTTON_X = 304;
static constexpr uint16_t BUTTON_Y = 33;
static constexpr uint16_t BUTTON_W = 81;
static constexpr uint16_t BUTTON_H = 46;

SportRemindHeartRateItemView::SportRemindHeartRateItemView()
{
    bgImage_.SetPosition(BG_X, BG_Y);
    LOADIMG::LoadImageViewImg(&bgImage_, SPORT_IMAGE, IMAGE_SPORT_ITEMBG);

    icon_.SetPosition(ICON_X, ICON_Y);

    label_.SetPosition(LABEL_X, LABEL_Y, LABEL_W, LABEL_H);
    label_.SetFont(DEFAULT_VECTOR_FONT_FILENAME, LABEL_FONT);
    label_.SetAlign(TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_CENTER);
    label_.SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);

    hits_.SetPosition(HITS_X, HITS_Y, HITS_W, HITS_H);
    hits_.SetFont(DEFAULT_VECTOR_FONT_FILENAME, HITS_FONT);
    hits_.SetAlign(TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_CENTER);
    hits_.SetStyle(STYLE_TEXT_COLOR, Color::Gray().full);
    hits_.SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);

    nextImage_.SetPosition(IMAGE_X, IMAGE_Y);
    LOADIMG::LoadImageViewImg(&nextImage_, SPORT_IMAGE, IMAGE_SPORT_NEXT);

    button_.SetPosition(BUTTON_X, BUTTON_Y, BUTTON_W, BUTTON_H);
    button_.SetImages(ImageCacheManager::GetInstance().LoadOneInMultiRes(IMAGE_SPORT_TOGGLE1, SPORT_IMAGE),
                      ImageCacheManager::GetInstance().LoadOneInMultiRes(IMAGE_SPORT_TOGGLE0, SPORT_IMAGE));

    Add(&bgImage_);
    Add(&icon_);
    Add(&label_);
    Add(&hits_);
    Add(&button_);
    Add(&nextImage_);
}

SportRemindHeartRateItemView::~SportRemindHeartRateItemView()
{
    Remove(&bgImage_);
    Remove(&icon_);
    Remove(&label_);
    Remove(&hits_);
    Remove(&button_);
    Remove(&nextImage_);
}

void SportRemindHeartRateItemView::SetItemInfo(const SportRemindHeartRateItem &itemInfo)
{
    LOADIMG::LoadImageViewImg(&icon_, SPORT_IMAGE, itemInfo.iconId);
    label_.SetText(itemInfo.label);
    sport_info_t *sportInfo = SportModel::GetInstance().GetCurrSportInfo();
    if (itemInfo.itemId == SPORT_REMIND_HEARTRATE_ENABLE) {
        button_.SetVisible(true);
        button_.SetState(sportInfo->enableHeartRemind);
        if (sportInfo->enableHeartRemind) {
            hits_.SetText("开启");
        } else {
            hits_.SetText("关闭");
        }
        nextImage_.SetVisible(false);
    } else if (itemInfo.itemId == SPORT_REMIND_HEARTRATE_RANGE) {
        button_.SetVisible(false);
        nextImage_.SetVisible(true);
        hits_.SetText(GetHeartRateRangeDescription(sportInfo->heartRange));
    }
    SetViewId(itemInfo.viewId);
}
}
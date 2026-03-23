/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SportSettingsItemView
 * Create: 2025-06-06
 */

#include "main/LoadImg.h"
#include "ui_resource_sport.h"
#include "wearable_log.h"
#include "UiConfig.h"
#include "sport/SportModel.h"
#include "sport/SportSettingsItemView.h"

namespace OHOS {
static constexpr uint16_t BG_X = 0;
static constexpr uint16_t BG_Y = 6;
static constexpr uint16_t ICON_X = 8;
static constexpr uint16_t ICON_Y = 13;
static constexpr uint16_t LABEL_X = 100;
static constexpr uint16_t LABEL_Y1 = 17;
static constexpr uint16_t LABEL_Y2 = 31;
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

SportSettingsItemView::SportSettingsItemView()
{
    bgImage_.SetPosition(BG_X, BG_Y);
    LOADIMG::LoadImageViewImg(&bgImage_, SPORT_IMAGE, IMAGE_SPORT_ITEMBG);

    icon_.SetPosition(ICON_X, ICON_Y);

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

    Add(&bgImage_);
    Add(&icon_);
    Add(&label_);
    Add(&hits_);
    Add(&nextImage_);
}

SportSettingsItemView::~SportSettingsItemView()
{
    Remove(&bgImage_);
    Remove(&icon_);
    Remove(&label_);
    Remove(&hits_);
    Remove(&nextImage_);
}

void SportSettingsItemView::SetItemInfo(const SportSettingsItem &itemInfo)
{
    LOADIMG::LoadImageViewImg(&icon_, SPORT_IMAGE, itemInfo.iconId);
    label_.SetText(itemInfo.label);
    SetViewId(itemInfo.viewId);
    sport_info_t *sportInfo = SportModel::GetInstance().GetCurrSportInfo();
    if (itemInfo.itemId == SPORT_SET_TYPE_TARGET) {
        label_.SetPosition(LABEL_X, LABEL_Y1, LABEL_W, LABEL_H);
        hits_.SetVisible(true);
        if (!sportInfo->isTargetSetted) {
            hits_.SetText("无目标");
        } else if (sportInfo->targetType == SPORT_TARGET_DISTANCE) {
            SportType sportType = SportModel::GetInstance().GetSportType();
            if (sportType == SPORT_TYPE_INDOOR_SWIMMING) {
                hits_.SetText((std::to_string(sportInfo->targetValue) + "趟").c_str());
            } else {
                hits_.SetText((std::to_string(sportInfo->targetValue) + "km").c_str());
            }
        } else if (sportInfo->targetType == SPORT_TARGET_TIME) {
            hits_.SetText((std::to_string(sportInfo->targetValue) + "分钟").c_str());
        } else if (sportInfo->targetType == SPORT_TARGET_CALORIE) {
            hits_.SetText((std::to_string(sportInfo->targetValue) + "千卡").c_str());
        }
    } else if (itemInfo.itemId == SPORT_SET_TYPE_REMIND) {
        label_.SetPosition(LABEL_X, LABEL_Y1, LABEL_W, LABEL_H);
        hits_.SetVisible(true);
        if (sportInfo->remindType == SPORT_REMIND_INTERVAL_DISTANCE) {
            SportType sportType = SportModel::GetInstance().GetSportType();
            if (sportType == SPORT_TYPE_INDOOR_SWIMMING) {
                hits_.SetText((std::to_string(sportInfo->remindValue) + "趟").c_str());
            } else {
                hits_.SetText((std::to_string(sportInfo->remindValue) + "km").c_str());
            }
        } else if (sportInfo->remindType == SPORT_REMIND_INTERVAL_TIME) {
            hits_.SetText((std::to_string(sportInfo->remindValue) + "分钟").c_str());
        } else if (sportInfo->remindType == SPORT_REMIND_INTERVAL_NONE || !sportInfo->isRemindSetted) {
            if (sportInfo->enableHeartRemind) {
                hits_.SetText(GetHeartRateRangeDescription(sportInfo->heartRange));
            } else {
                hits_.SetText("无提醒");
            }
        }
    } else if (itemInfo.itemId == SPORT_SET_TYPE_MISTOUCH) {
        label_.SetPosition(LABEL_X, LABEL_Y2, LABEL_W, LABEL_H);
        hits_.SetVisible(false);
    }
}
}
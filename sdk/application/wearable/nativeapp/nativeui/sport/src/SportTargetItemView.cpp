/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SportTargetItemView
 * Create: 2025-06-06
 */

#include "wearable_log.h"
#include "UiConfig.h"
#include "main/LoadImg.h"
#include "ui_resource_sport.h"
#include "sport/SportModel.h"
#include "sport/SportTargetItemView.h"

namespace OHOS {
static constexpr uint16_t BG_X = 0;
static constexpr uint16_t BG_Y = 6;
static constexpr uint16_t ICON_X = 8;
static constexpr uint16_t ICON_Y = 13;
static constexpr uint16_t LABEL_X = 100;
static constexpr uint16_t LABEL_Y = 31;
static constexpr uint16_t LABEL_W = 179;
static constexpr uint16_t LABEL_H = 37;
static constexpr uint16_t LABEL_FONT = 32;
static constexpr uint16_t IMAGE_X = 370;
static constexpr uint16_t IMAGE_Y = 38;

SportTargetItemView::SportTargetItemView()
{
    bgImage_.SetPosition(BG_X, BG_Y);
    LOADIMG::LoadImageViewImg(&bgImage_, SPORT_IMAGE, IMAGE_SPORT_ITEMBG);

    icon_.SetPosition(ICON_X, ICON_Y);

    label_.SetPosition(LABEL_X, LABEL_Y, LABEL_W, LABEL_H);
    label_.SetFont(DEFAULT_VECTOR_FONT_FILENAME, LABEL_FONT);
    label_.SetAlign(TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_CENTER);
    label_.SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);

    nextImage_.SetPosition(IMAGE_X, IMAGE_Y);
    LOADIMG::LoadImageViewImg(&nextImage_, SPORT_IMAGE, IMAGE_SPORT_NEXT);

    Add(&bgImage_);
    Add(&icon_);
    Add(&label_);
    Add(&nextImage_);
}

SportTargetItemView::~SportTargetItemView()
{
    Remove(&bgImage_);
    Remove(&icon_);
    Remove(&label_);
    Remove(&nextImage_);
}

void SportTargetItemView::SetItemInfo(const SportTargetItem &itemInfo)
{
    SportType sportType = SportModel::GetInstance().GetSportType();
    if (itemInfo.itemId == SPORT_TARGET_DISTANCE && sportType == SPORT_TYPE_INDOOR_SWIMMING) {
        LOADIMG::LoadImageViewImg(&icon_, SPORT_IMAGE, IMAGE_SPORT_LAP);
        label_.SetText("趟数");
    } else {
        LOADIMG::LoadImageViewImg(&icon_, SPORT_IMAGE, itemInfo.iconId);
        label_.SetText(itemInfo.label);
    }
    SetViewId(itemInfo.viewId);
}
}
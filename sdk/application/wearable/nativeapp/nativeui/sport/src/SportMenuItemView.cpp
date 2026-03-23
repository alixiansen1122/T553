/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SportMenuItemView
 * Create: 2025-06-06
 */

#include "wearable_log.h"
#include "UiConfig.h"
#include "ui_resource_sport.h"
#include "main/LoadImg.h"
#include "sport/SportMenuItemView.h"

namespace OHOS {
static constexpr uint16_t BG_X = 0;
static constexpr uint16_t BG_Y = 10;
static constexpr uint16_t ICON_X = 16;
static constexpr uint16_t ICON_Y = 25;
static constexpr uint16_t LABEL_X = 124;
static constexpr uint16_t LABEL_Y = 47;
static constexpr uint16_t LABEL_W = 166;
static constexpr uint16_t LABEL_H = 40;
static constexpr uint16_t LABEL_FONT = 32;
static constexpr uint16_t IMAGE_X = 344;
static constexpr uint16_t IMAGE_Y = 44;

SportMenuItemView::SportMenuItemView()
{
    bgImage_.SetPosition(BG_X, BG_Y);

    icon_.SetPosition(ICON_X, ICON_Y);

    label_.SetPosition(LABEL_X, LABEL_Y, LABEL_W, LABEL_H);
    label_.SetFont(DEFAULT_VECTOR_FONT_FILENAME, LABEL_FONT);
    label_.SetAlign(TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_CENTER);

    LOADIMG::LoadImageViewImg(&settingImage_, SPORT_IMAGE, IMAGE_SPORT_SETTING);
    settingImage_.SetPosition(IMAGE_X, IMAGE_Y);

    Add(&bgImage_);
    Add(&icon_);
    Add(&label_);
    Add(&settingImage_);
}

SportMenuItemView::~SportMenuItemView()
{
    Remove(&bgImage_);
    Remove(&icon_);
    Remove(&label_);
    Remove(&settingImage_);
}

void SportMenuItemView::SetItemInfo(const SportMenuItem &itemInfo)
{
    LOADIMG::LoadImageViewImg(&bgImage_, SPORT_IMAGE, itemInfo.bgId);
    LOADIMG::LoadImageViewImg(&icon_, SPORT_IMAGE, itemInfo.iconId);
    label_.SetText(itemInfo.label);
    SetViewId(itemInfo.viewId);
}
}
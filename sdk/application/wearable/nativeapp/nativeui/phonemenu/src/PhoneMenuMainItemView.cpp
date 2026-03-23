/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: PhoneMenuMainItemView
 * Create: 2025-07-12
 */

#include "UiConfig.h"
#include "main/LoadImg.h"
#include "phonemenu/PhoneMenuMainItemView.h"

namespace OHOS {
static constexpr uint16_t BG_GROUP_X = 0;
static constexpr uint16_t BG_GROUP_Y = 0;
static constexpr uint16_t BG_GROUP_W = 426;
static constexpr uint16_t BG_GROUP_H = 96;
static constexpr uint16_t BG_BACKGROUND_OPA = 80;
static constexpr uint16_t BG_BORDER_RADIUS = 48;
static constexpr uint16_t ICON_X = 8;
static constexpr uint16_t ICON_Y = 8;
static constexpr uint16_t LABEL_X = 101;
static constexpr uint16_t LABEL_Y = 22;
static constexpr uint16_t LABEL_W = 150;
static constexpr uint16_t LABEL_H = 53;
static constexpr uint16_t LABEL_FONT = 32;

PhoneMenuMainItemView::PhoneMenuMainItemView()
{
    bg_.SetPosition(BG_GROUP_X, BG_GROUP_Y, BG_GROUP_W, BG_GROUP_H);
    bg_.SetStyle(STYLE_BACKGROUND_OPA, BG_BACKGROUND_OPA);
    bg_.SetStyle(STYLE_BACKGROUND_COLOR, Color::Gray().full);
    bg_.SetStyle(STYLE_BORDER_RADIUS, BG_BORDER_RADIUS);

    icon_.SetPosition(ICON_X, ICON_Y);

    label_.SetPosition(LABEL_X, LABEL_Y, LABEL_W, LABEL_H);
    label_.SetFont(DEFAULT_VECTOR_FONT_FILENAME, LABEL_FONT);
    label_.SetAlign(TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_CENTER);

    Add(&bg_);
    Add(&icon_);
    Add(&label_);
}

PhoneMenuMainItemView::~PhoneMenuMainItemView()
{
    Remove(&icon_);
    Remove(&label_);
    Remove(&bg_);
}

void PhoneMenuMainItemView::SetItemInfo(const PhoneMenuItem &itemInfo)
{
    label_.SetText(itemInfo.label);
    LOADIMG::LoadImageViewImg(&icon_, PHONEMENU_IMAGE, itemInfo.iconResId);
    SetViewId(itemInfo.viewId);
}
}  // namespace OHOS
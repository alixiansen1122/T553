/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: PhoneMenuContactsItemView
 * Create: 2025-07-12
 */

#include "UiConfig.h"
#include "main/LoadImg.h"
#include "ui_resource_phonemenu.h"
#include "phonemenu/PhoneMenuContactsItemView.h"

namespace OHOS {
static constexpr uint16_t BG_GROUP_X = 0;
static constexpr uint16_t BG_GROUP_Y = 7;
static constexpr uint16_t BG_GROUP_W = 383;
static constexpr uint16_t BG_GROUP_H = 88;
static constexpr uint16_t BG_BACKGROUND_OPA = 48;
static constexpr uint16_t BG_BORDER_RADIUS = 45;
static constexpr uint16_t ICON_X = 33;
static constexpr uint16_t ICON_Y = 26;
static constexpr uint16_t LABEL_X = 89;
static constexpr uint16_t LABEL_Y = 30;
static constexpr uint16_t LABEL_W = 294;
static constexpr uint16_t LABEL_H = 35;
static constexpr uint16_t LABEL_FONT = 32;

PhoneMenuContactsItemView::PhoneMenuContactsItemView()
{
    bg_.SetPosition(BG_GROUP_X, BG_GROUP_Y, BG_GROUP_W, BG_GROUP_H);
    bg_.SetStyle(STYLE_BACKGROUND_OPA, BG_BACKGROUND_OPA);
    bg_.SetStyle(STYLE_BACKGROUND_COLOR, Color::Gray().full);
    bg_.SetStyle(STYLE_BORDER_RADIUS, BG_BORDER_RADIUS);

    icon_.SetPosition(ICON_X, ICON_Y);
    LOADIMG::LoadImageViewImg(&icon_, PHONEMENU_IMAGE, IMAGE_PHONEMENU_CONTACTS);

    label_.SetPosition(LABEL_X, LABEL_Y, LABEL_W, LABEL_H);
    label_.SetFont(DEFAULT_VECTOR_FONT_FILENAME, LABEL_FONT);
    label_.SetAlign(TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_CENTER);
    label_.SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);

    Add(&bg_);
    Add(&icon_);
    Add(&label_);
}

PhoneMenuContactsItemView::~PhoneMenuContactsItemView()
{
    Remove(&bg_);
    Remove(&icon_);
    Remove(&label_);
}

void PhoneMenuContactsItemView::SetItemInfo(const PhoneMenuContactsItem &itemInfo)
{
    itemInfo_ = itemInfo;
    label_.SetText(itemInfo.name.c_str());
}

const char *PhoneMenuContactsItemView::GetContactsName() const
{
    return itemInfo_.name.c_str();
}

const char *PhoneMenuContactsItemView::GetContactsCid() const
{
    return itemInfo_.cid.c_str();
}

const char *PhoneMenuContactsItemView::GetContactsPhoneNumber() const
{
    return itemInfo_.phoneNumber.c_str();
}
}
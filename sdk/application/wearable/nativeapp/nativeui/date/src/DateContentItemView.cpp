/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: DateItemView
 * Create: 2025-03-23
 */

#include "UiConfig.h"
#include "common/image_cache_manager.h"
#include "ui_resource_date.h"
#include "wearable_log.h"
#include "date/DateContentItemView.h"

namespace OHOS {
static constexpr int16_t DATE_ITEM_LABEL_POSITION_X = 43;
static constexpr int16_t DATE_ITEM_LABEL_POSITION_Y = 22;
static constexpr int16_t DATE_ITEM_LABEL_DES_POSITION_X = 23;
static constexpr int16_t DATE_ITEM_LABEL_DES_POSITION_Y = 60;
static constexpr int16_t DATE_ITEM_LABEL_ADDRESS_POSITION_X = 23;
static constexpr int16_t DATE_ITEM_LABEL_ADDRESS_POSITION_Y = 110;
static constexpr int16_t DATE_ITEM_LABEL_WIDTH = 200;
static constexpr int16_t DATE_ITEM_LABEL_HEIGHT = 40;
static constexpr int16_t DATE_ITEM_LABEL_DES_WIDTH = 314;
static constexpr int16_t DATE_ITEM_LABEL_DES_HEIGHT = 40;
static constexpr int16_t DATE_ITEM_LABEL_ADDRESS_WIDTH = 314;
static constexpr int16_t DATE_ITEM_LABEL_ADDRESS_HEIGHT = 40;
static constexpr int16_t DATE_ITEM_FONT_SIZE = 24;
static constexpr int16_t DATE_ITEM_FONT2_SIZE = 32;
static constexpr int16_t DATE_ITEM_WIDTH = 376;
static constexpr int16_t DATE_ITEM_HEIGHT = 170;
static constexpr int16_t DATE_ITEM_HEIGHT_BORDER = 10;
static constexpr int16_t DATE_ITEM_BORDER_RADIUS = 15;

DateContentItemView::DateContentItemView()
{
    labelTitle_.Resize(DATE_ITEM_LABEL_WIDTH, DATE_ITEM_LABEL_HEIGHT);
    labelTitle_.SetAlign(TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_LEFT);
    labelTitle_.SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
    labelTitle_.SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    labelTitle_.SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);

    labelDes_.Resize(DATE_ITEM_LABEL_DES_WIDTH, DATE_ITEM_LABEL_DES_POSITION_Y);
    labelDes_.SetAlign(TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_LEFT);
    labelDes_.SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
    labelDes_.SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    labelDes_.SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);

    labelAddress_.Resize(DATE_ITEM_LABEL_ADDRESS_WIDTH, DATE_ITEM_LABEL_ADDRESS_HEIGHT);
    labelAddress_.SetAlign(TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_LEFT);
    labelAddress_.SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
    labelAddress_.SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    labelAddress_.SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    signs_.SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    LOADIMG::LoadImageViewImg(&signs_, DATE_IMAGE, IMAGE_DATE_SIGNS);

    backGround_.Disable();
    backGround_.SetPosition(0, 0, DATE_ITEM_WIDTH, DATE_ITEM_HEIGHT - DATE_ITEM_HEIGHT_BORDER);
    backGround_.SetStyleForState(STYLE_BACKGROUND_COLOR, 0X36FFFFFF, UIButton::PRESSED);
    backGround_.SetStyleForState(STYLE_BACKGROUND_COLOR, 0X36FFFFFF, UIButton::INACTIVE);
    backGround_.SetStyleForState(STYLE_BACKGROUND_COLOR, 0X36FFFFFF, UIButton::RELEASED);
    backGround_.SetStyleForState(STYLE_BORDER_RADIUS, DATE_ITEM_BORDER_RADIUS, UIButton::PRESSED);
    backGround_.SetStyleForState(STYLE_BORDER_RADIUS, DATE_ITEM_BORDER_RADIUS, UIButton::INACTIVE);
    backGround_.SetStyleForState(STYLE_BORDER_RADIUS, DATE_ITEM_BORDER_RADIUS, UIButton::RELEASED);

    Add(&backGround_);
    Add(&labelTitle_);
    Add(&labelDes_);
    Add(&labelAddress_);
    Add(&signs_);
}

DateContentItemView::~DateContentItemView()
{
    Remove(&backGround_);
    Remove(&labelTitle_);
    Remove(&labelDes_);
    Remove(&labelAddress_);
    Remove(&signs_);
}

void DateContentItemView::SetItemInfo(const DateItem itemInfo)
{
    itemInfo_ = itemInfo;
    int offsetX1 = 95;
    int offsetY1 = 10;
    int offsetX2 = 30;
    int offsetY2 = 40;
    labelTitle_.SetText(itemInfo.title);
    labelDes_.SetText(itemInfo.describetion);

    if (!strcmp(itemInfo.address, "no location")) {
        labelAddress_.SetVisible(false);
        backGround_.SetHeight(DATE_ITEM_HEIGHT - offsetY2 - offsetY1);
        SetHeight(DATE_ITEM_HEIGHT - offsetY2);
    } else {
        labelAddress_.SetVisible(true);
        labelAddress_.SetText(itemInfo.address);
        backGround_.SetHeight(DATE_ITEM_HEIGHT - offsetY1);
        SetHeight(DATE_ITEM_HEIGHT);
    }

    if (itemInfo.visiable) {
        labelTitle_.SetPosition(DATE_ITEM_LABEL_POSITION_X + offsetX1, DATE_ITEM_LABEL_POSITION_Y + offsetY1);
        labelTitle_.SetStyle(STYLE_TEXT_COLOR, Color::White().full);
        labelTitle_.SetFont(DEFAULT_VECTOR_FONT_FILENAME, DATE_ITEM_FONT2_SIZE);
        labelDes_.SetPosition(DATE_ITEM_LABEL_DES_POSITION_X + offsetX2, DATE_ITEM_LABEL_DES_POSITION_Y + offsetY2);
        labelDes_.SetStyle(STYLE_TEXT_COLOR, Color::Gray().full);
        labelDes_.SetFont(DEFAULT_VECTOR_FONT_FILENAME, DATE_ITEM_FONT_SIZE);
        labelAddress_.SetVisible(false);
    } else {
        labelTitle_.SetPosition(DATE_ITEM_LABEL_POSITION_X, DATE_ITEM_LABEL_POSITION_Y);
        labelTitle_.SetStyle(STYLE_TEXT_COLOR, Color::Gray().full);
        labelTitle_.SetFont(DEFAULT_VECTOR_FONT_FILENAME, DATE_ITEM_FONT_SIZE);
        labelDes_.SetPosition(DATE_ITEM_LABEL_DES_POSITION_X, DATE_ITEM_LABEL_DES_POSITION_Y);
        labelDes_.SetStyle(STYLE_TEXT_COLOR, Color::White().full);
        labelDes_.SetFont(DEFAULT_VECTOR_FONT_FILENAME, DATE_ITEM_FONT2_SIZE);
        labelAddress_.SetPosition(DATE_ITEM_LABEL_DES_POSITION_X, DATE_ITEM_LABEL_ADDRESS_POSITION_Y);
        labelAddress_.SetStyle(STYLE_TEXT_COLOR, Color::Gray().full);
        labelAddress_.SetFont(DEFAULT_VECTOR_FONT_FILENAME, DATE_ITEM_FONT_SIZE);
        signs_.SetPosition(DATE_ITEM_LABEL_DES_POSITION_X, DATE_ITEM_FONT_SIZE);
    }
    signs_.SetVisible(!itemInfo.visiable);
}

const char *DateContentItemView ::GetViewId()
{
    return itemInfo_.index;
}
bool DateContentItemView::GetVisiable()
{
    return itemInfo_.visiable;
}

}
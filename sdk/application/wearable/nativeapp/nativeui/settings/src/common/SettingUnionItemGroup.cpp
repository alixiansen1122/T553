/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingUnionItemGroup
 * Create: 2025-06-01
 */
#include <cstdint>
#include "wearable_log.h"
#include "main/LoadImg.h"
#include "settings/common/SettingUnionItemGroup.h"

namespace OHOS {
static constexpr int16_t SETTING_PAGE_ITEM_WIDTH = 426;
static constexpr int16_t SETTING_PAGE_ITEM_HEIGHT = 96;
static constexpr int16_t SETTING_PAGE_ITEM_RADIUS = 48;
static constexpr int16_t SETTING_PAGE_ITEM_MARGIN_BOTTON = 15;
static constexpr int8_t SETTING_PAGE_ITEM_ICON_X = 6;
static constexpr int8_t SETTING_PAGE_ITEM_ICON_Y = 6;
static constexpr int8_t SETTING_PAGE_ITEM_ICON_WIDTH = 84;
static constexpr int8_t SETTING_PAGE_ITEM_ICON_HEIGHT = 84;
static constexpr int16_t SETTING_PAGE_ITEM_LABEL_FSIZE = 32;

static constexpr int16_t SETTING_PAGE_STATIC_LABEL_WIDTH = 260;
static constexpr int16_t SETTING_PAGE_STATIC_LABEL_HEIGHT = 53;
static constexpr int16_t SETTING_PAGE_STATIC_LABEL_X = 83;
static constexpr int16_t SETTING_PAGE_STATIC_LABEL_Y = 25;
static constexpr int16_t SETTING_PAGE_STATIC_LABEL_FSIZE = 38;

static constexpr int16_t SETTING_PAGE_TOGGLE_X = 312;
static constexpr int16_t SETTING_PAGE_TOGGLE_Y = 24;
static constexpr int16_t SETTING_PAGE_TOGGLE_WIDTH = 84;
static constexpr int16_t SETTING_PAGE_TOGGLE_HEIGHT = 48;
static constexpr int16_t SETTING_PAGE_TOGGLE_TRACK_PADDING = 4;

static constexpr int16_t SETTING_PAGE_INPAGE_TLABEL_X = 101;
static constexpr int16_t SETTING_PAGE_INPAGE_LABEL_Y = 29;
static constexpr int16_t SETTING_PAGE_INPAGE_LABEL_WIDTH = 38;
static constexpr int16_t SETTING_PAGE_INPAGE_DLABEL_X_OFFSET = 13; // TextLabel后偏移13个像素是DataLabel的起点
static constexpr int16_t SETTING_PAGE_INPAGE_DLABEL_OPA = 127;
static constexpr int16_t SETTING_PAGE_INPAGE_IMAGE_X = 380;
static constexpr int16_t SETTING_PAGE_INPAGE_IMAGE_Y = 33;
static constexpr int16_t SETTING_PAGE_INPAGE_IMAGE_WIDTH = 18;
static constexpr int16_t SETTING_PAGE_INPAGE_IMAGE_HEIGHT = 31;

SettingUnionItemType SettingUnionItemData::GetUnionItemType()
{
    return type_;
}

void* SettingUnionItemData::GetUnionItemData()
{
    return &unionItemData_;
}

SettingUnionItemData::SettingUnionItemData(const PlainTextItemData& itemData) {
    type_ = PLAIN_TEXT;
    unionItemData_.plainTextItemData = itemData;
}

SettingUnionItemData::SettingUnionItemData(const IconTextToggleItemData& itemData) {
    type_ = ICON_TEXT_TOGGLE;
    unionItemData_.iconTextToggleItemData = itemData;
}

SettingUnionItemData::SettingUnionItemData(const IconTextNavigationItemData& itemData) {
    type_ = ICON_TEXT_NAVIGATION;
    unionItemData_.iconTextNavigationItemData = itemData;
}

PlainTextItemView::PlainTextItemView()
{
    SetPosition(0, 0, SETTING_PAGE_ITEM_WIDTH, SETTING_PAGE_ITEM_HEIGHT);
    SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    SetStyle(STYLE_BACKGROUND_OPA, UN_OPACITY);
    SetTouchable(false);

    label_.SetPosition(SETTING_PAGE_STATIC_LABEL_X, SETTING_PAGE_STATIC_LABEL_Y);
    label_.Resize(SETTING_PAGE_STATIC_LABEL_WIDTH, SETTING_PAGE_STATIC_LABEL_HEIGHT);
    label_.SetFont(BOLD_VECTOR_FONT_FILENAME, SETTING_PAGE_STATIC_LABEL_FSIZE);
    label_.SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    label_.SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
    label_.SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    label_.SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    label_.SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    Add(&label_);
}

PlainTextItemView::~PlainTextItemView()
{
    Remove(&label_);
}

void PlainTextItemView::UpdateViewData(PlainTextItemData* data)
{
    if (data != nullptr) {
        label_.SetText(data->textContent);
        SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    }
}

IconTextToggleItemView::IconTextToggleItemView()
{
    SetPosition(0, 0, SETTING_PAGE_ITEM_WIDTH, SETTING_PAGE_ITEM_HEIGHT);
    SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    SetStyle(STYLE_BACKGROUND_OPA, UN_OPACITY);
    SetStyle(STYLE_BORDER_RADIUS, SETTING_PAGE_ITEM_RADIUS);
    SetStyle(STYLE_MARGIN_BOTTOM, SETTING_PAGE_ITEM_MARGIN_BOTTON);
    SetTouchable(true);

    icon_.SetPosition(SETTING_PAGE_ITEM_ICON_X, SETTING_PAGE_ITEM_ICON_Y);
    icon_.Resize(SETTING_PAGE_ITEM_ICON_WIDTH, SETTING_PAGE_ITEM_ICON_HEIGHT);
    icon_.SetStyle(STYLE_IMAGE_OPA, OPA_OPAQUE);
    icon_.SetAutoEnable(false);
    icon_.SetResizeMode(UIImageView::SCALE_DOWN);

    label_.SetPosition(SETTING_PAGE_INPAGE_TLABEL_X, SETTING_PAGE_INPAGE_LABEL_Y);
    label_.Resize(0, SETTING_PAGE_INPAGE_LABEL_WIDTH);
    label_.SetLineBreakMode(UILabel::LINE_BREAK_ADAPT);
    label_.SetFont(DEFAULT_VECTOR_FONT_FILENAME, SETTING_PAGE_ITEM_LABEL_FSIZE);
    label_.SetAlign(TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_CENTER);
    label_.SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
    label_.SetStyle(STYLE_TEXT_COLOR, Color::White().full);

    toggleButton_.SetPosition(SETTING_PAGE_TOGGLE_X, SETTING_PAGE_TOGGLE_Y);
    toggleButton_.Resize(SETTING_PAGE_TOGGLE_WIDTH, SETTING_PAGE_TOGGLE_HEIGHT);
    toggleButton_.SetTrackPadding(SETTING_PAGE_TOGGLE_TRACK_PADDING);
    toggleButton_.SetTouchable(false);

    Add(&icon_);
    Add(&label_);
    Add(&toggleButton_);
}

IconTextToggleItemView::~IconTextToggleItemView()
{
    Remove(&icon_);
    Remove(&label_);
    Remove(&toggleButton_);
}

void IconTextToggleItemView::UpdateViewData(IconTextToggleItemData* data)
{
    if (data != nullptr) {
        SetStyle(STYLE_BACKGROUND_COLOR, data->bgColor.full);
        SetStyle(STYLE_BACKGROUND_OPA, data->bgOpa);
        icon_.SetSrc(CacheImage(data->icon));
        label_.SetText(data->textContent);
        toggleButton_.SetState(data->selected);
    }
}

IconTextNavigationItemView::IconTextNavigationItemView()
{
    SetPosition(0, 0, SETTING_PAGE_ITEM_WIDTH, SETTING_PAGE_ITEM_HEIGHT);
    SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    SetStyle(STYLE_BACKGROUND_OPA, UN_OPACITY);
    SetStyle(STYLE_BORDER_RADIUS, SETTING_PAGE_ITEM_RADIUS);
    SetStyle(STYLE_MARGIN_BOTTOM, SETTING_PAGE_ITEM_MARGIN_BOTTON);
    SetTouchable(true);

    icon_.SetPosition(SETTING_PAGE_ITEM_ICON_X, SETTING_PAGE_ITEM_ICON_Y);
    icon_.Resize(SETTING_PAGE_ITEM_ICON_WIDTH, SETTING_PAGE_ITEM_ICON_HEIGHT);
    icon_.SetStyle(STYLE_IMAGE_OPA, OPA_OPAQUE);
    icon_.SetAutoEnable(false);
    icon_.SetResizeMode(UIImageView::SCALE_DOWN);

    textLabel_.SetPosition(SETTING_PAGE_INPAGE_TLABEL_X, SETTING_PAGE_INPAGE_LABEL_Y);
    textLabel_.Resize(0, SETTING_PAGE_INPAGE_LABEL_WIDTH);
    textLabel_.SetLineBreakMode(UILabel::LINE_BREAK_ADAPT);
    textLabel_.SetAlign(TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_CENTER);
    textLabel_.SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
    textLabel_.SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    textLabel_.SetFont(DEFAULT_VECTOR_FONT_FILENAME, SETTING_PAGE_ITEM_LABEL_FSIZE);

    dataLabel_.Resize(0, SETTING_PAGE_INPAGE_LABEL_WIDTH);
    dataLabel_.SetLineBreakMode(UILabel::LINE_BREAK_ADAPT);
    dataLabel_.SetAlign(TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_CENTER);
    dataLabel_.SetStyle(STYLE_TEXT_OPA, SETTING_PAGE_INPAGE_DLABEL_OPA);
    dataLabel_.SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    dataLabel_.SetFont(DEFAULT_VECTOR_FONT_FILENAME, SETTING_PAGE_ITEM_LABEL_FSIZE);

    nextImage_.SetPosition(SETTING_PAGE_INPAGE_IMAGE_X, SETTING_PAGE_INPAGE_IMAGE_Y);
    nextImage_.Resize(SETTING_PAGE_INPAGE_IMAGE_WIDTH, SETTING_PAGE_INPAGE_IMAGE_HEIGHT);
    LOADIMG::LoadImageViewImg(&nextImage_, SETTING_IMAGE, IMAGE_MAIN_LIST_ARROW);
    nextImage_.SetStyle(STYLE_IMAGE_OPA, OPA_OPAQUE);
    nextImage_.SetAutoEnable(false);
    nextImage_.SetResizeMode(UIImageView::SCALE_DOWN);

    Add(&icon_);
    Add(&textLabel_);
    Add(&dataLabel_);
    Add(&nextImage_);
}

IconTextNavigationItemView::~IconTextNavigationItemView()
{
    Remove(&icon_);
    Remove(&textLabel_);
    Remove(&dataLabel_);
    Remove(&nextImage_);
}

void IconTextNavigationItemView::UpdateViewData(IconTextNavigationItemData* data)
{
    if (data != nullptr) {
        SetStyle(STYLE_BACKGROUND_COLOR, data->bgColor.full);
        SetStyle(STYLE_BACKGROUND_OPA, data->bgOpa);
        icon_.SetSrc(CacheImage(data->icon));
        textLabel_.SetText(data->textContent);
        dataLabel_.SetVisible(false);
        if (data->dataContent != nullptr) {
            dataLabel_.SetPosition(
                SETTING_PAGE_INPAGE_TLABEL_X + textLabel_.GetTextWidth() + SETTING_PAGE_INPAGE_DLABEL_X_OFFSET,
                SETTING_PAGE_INPAGE_LABEL_Y);
            dataLabel_.SetText(data->dataContent);
            dataLabel_.SetVisible(true);
        }
        nextImage_.SetVisible(data->hasSecondMenu);
    }
}
}
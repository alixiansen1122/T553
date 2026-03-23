/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingDesktopHomeItemGroup
 * Create: 2025-06-01
 */
#include <cstdint>
#include "UiConfig.h"
#include "wearable_log.h"
#include "settings/common/SettingDesktopHomeItemGroup.h"

namespace OHOS {
static constexpr int16_t SETTING_HOME_ITEM_WIDTH = 153;
static constexpr int16_t SETTING_HOME_ITEM_HEIGHT = 205;
static constexpr int16_t SETTING_HOME_ITEM_MARGIN = 13;

static constexpr int16_t SETTING_HOME_BUTTON_HEIGHT = 153;
static constexpr int16_t SETTING_HOME_BUTTON_RADIUS = 0;
static constexpr int16_t SETTING_HOME_LABEL_X = 28;
static constexpr int16_t SETTING_HOME_LABEL_Y = 167;
static constexpr int16_t SETTING_HOME_LABEL_WIDTH = 98;
static constexpr int16_t SETTING_HOME_LABEL_HEIGHT = 38;
static constexpr int16_t SETTING_HOME_LABEL_FSIZE = 32;

SettingDesktopHomeItemGroup::SettingDesktopHomeItemGroup()
{
    SetPosition(0, 0, SETTING_HOME_ITEM_WIDTH, SETTING_HOME_ITEM_HEIGHT);
    SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    SetStyle(STYLE_BACKGROUND_OPA, UN_OPACITY);
    SetStyle(STYLE_MARGIN_LEFT, SETTING_HOME_ITEM_MARGIN);
    SetStyle(STYLE_MARGIN_RIGHT, SETTING_HOME_ITEM_MARGIN);
    SetTouchable(true);

    button_.SetPosition(0, 0, SETTING_HOME_ITEM_WIDTH, SETTING_HOME_BUTTON_HEIGHT);
    button_.SetStyle(STYLE_BORDER_RADIUS, SETTING_HOME_BUTTON_RADIUS);

    label_.SetPosition(SETTING_HOME_LABEL_X, SETTING_HOME_LABEL_Y);
    label_.Resize(SETTING_HOME_LABEL_WIDTH, SETTING_HOME_LABEL_HEIGHT);
    label_.SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    label_.SetFont(DEFAULT_VECTOR_FONT_FILENAME, SETTING_HOME_LABEL_FSIZE);
    label_.SetStyle(STYLE_TEXT_COLOR, Color::White().full);

    Add(&button_);
    Add(&label_);
}

SettingDesktopHomeItemGroup::~SettingDesktopHomeItemGroup()
{
    RemoveAll();
}

void SettingDesktopHomeItemGroup::UpdateItemGroup(SettingDesktopHomeItemData* itemData)
{
    SetViewId(itemData->label);
    label_.SetText(itemData->label);
    ImageInfo* select = ImageCacheManager::GetInstance().LoadOneInMultiRes(itemData->selectIcon, SETTING_IMAGE);
    ImageInfo* unselect = ImageCacheManager::GetInstance().LoadOneInMultiRes(itemData->unSelectIcon, SETTING_IMAGE);
    button_.SetImages(select, unselect);
    if (itemData->isSelect) {
        button_.SetState(UICheckBox::SELECTED, true);
    } else {
        button_.SetState(UICheckBox::UNSELECTED, true);
    }
}
}

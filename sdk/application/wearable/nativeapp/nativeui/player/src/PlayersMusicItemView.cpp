/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: PlayersMusicItemView
 * Create: 2025-03-23
 */

#include "UiConfig.h"
#include "common/image_cache_manager.h"
#include "ui_resource_date.h"
#include "wearable_log.h"
#include "player/PlayersMusicItemView.h"

namespace OHOS {
constexpr uint16_t STYLE_BORDER_WIDTH_VALUE = 4;
constexpr uint16_t STYLE_BORDER_RADIUS_VALUE = 12;
constexpr uint16_t LABEL_BUTTON_HEIGHT = 64;
static constexpr uint32 BUTTON_STYLE_BACKGROUND_COLOR_VALUE = 0xFF333333;
PlayersMusicItemView::PlayersMusicItemView()
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "PlayersMusicItemView");
    labelButton_.SetPosition(0, 0);
    labelButton_.Resize(HORIZONTAL_RESOLUTION, LABEL_BUTTON_HEIGHT);
    labelButton_.SetStyleForState(STYLE_BORDER_WIDTH, STYLE_BORDER_WIDTH_VALUE, UIButton::RELEASED);
    labelButton_.SetStyleForState(STYLE_BORDER_WIDTH, STYLE_BORDER_WIDTH_VALUE, UIButton::PRESSED);
    labelButton_.SetStyleForState(STYLE_BORDER_WIDTH, STYLE_BORDER_WIDTH_VALUE, UIButton::INACTIVE);
    labelButton_.SetStyleForState(STYLE_BORDER_OPA, 0, UIButton::RELEASED);
    labelButton_.SetStyleForState(STYLE_BORDER_OPA, 0, UIButton::PRESSED);
    labelButton_.SetStyleForState(STYLE_BORDER_OPA, 0, UIButton::INACTIVE);
    labelButton_.SetStyleForState(STYLE_BORDER_RADIUS, STYLE_BORDER_RADIUS_VALUE, UIButton::RELEASED);
    labelButton_.SetStyleForState(STYLE_BORDER_RADIUS, STYLE_BORDER_RADIUS_VALUE, UIButton::PRESSED);
    labelButton_.SetStyleForState(STYLE_BORDER_RADIUS, STYLE_BORDER_RADIUS_VALUE, UIButton::INACTIVE);
    labelButton_.SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::RELEASED);
    labelButton_.SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::PRESSED);
    labelButton_.SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::INACTIVE);
    labelButton_.SetFont(DEFAULT_VECTOR_FONT_FILENAME, 24); // 24: means font size
    labelButton_.SetAlign(TEXT_ALIGNMENT_CENTER);
    labelButton_.SetLabelPosition(24, 0); // 24: lable x-coordinate
    Add(&labelButton_);
}

PlayersMusicItemView::~PlayersMusicItemView()
{
    Remove(&labelButton_);
}

void PlayersMusicItemView::SetItemInfo(const PlayerCaseInfo itemInfo)
{
    itemInfo_ = itemInfo;
    std::string strSongName = itemInfo.sliceId;
    std::string pureName = strSongName.substr(0, strSongName.rfind("."));
    labelButton_.SetText(pureName.c_str());
    labelButton_.SetViewIndex(itemInfo.index);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "pureName.c_str(%s)", pureName.c_str());
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "pureName.index(%d)", itemInfo.index);
}

PlayerCaseInfo PlayersMusicItemView::GetItem()
{
    return itemInfo_;
}
}
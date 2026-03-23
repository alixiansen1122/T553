/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: FlashLightSetItemView
 * Create: 2025-02-15
 */

#include "wearable_log.h"
#include "UiConfig.h"
#include "screensetting/ScreenModels.h"
#include "main/LoadImg.h"
#include "flashlight/FlashLightModel.h"
#include "flashlight/FlashLightSetItemView.h"

namespace OHOS {
static constexpr uint16_t ITEM_X = 0;
static constexpr uint16_t ITEM_Y = 0;
static constexpr uint16_t ITEM_W = 340;
static constexpr uint16_t ITEM_H = 122;
static constexpr uint16_t ICON_X = 0;
static constexpr uint16_t ICON_Y = 10;
static constexpr uint16_t LABEL_X = 109;
static constexpr uint16_t LABEL_Y = 0;
static constexpr uint16_t LABEL_W = 172;
static constexpr uint16_t LABEL_H = 53;
static constexpr uint16_t LABEL_FONT = 38;
static constexpr uint16_t IMAGE_X = 315;
static constexpr uint16_t IMAGE_Y = 33;
static constexpr uint16_t ITEM_LABEL_X = 109;
static constexpr uint16_t ITEM_LABEL_Y = 52;
static constexpr uint16_t ITEM_LABEL_W = 172;
static constexpr uint16_t ITEM_LABEL_H = 46;
static constexpr uint16_t ITEM_LABEL_FONT = 32;

FlashLightSetItemView::FlashLightSetItemView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "FlashLightSetItemView");
    sIcon_.SetPosition(ICON_X, ICON_Y);
    sIcon_.SetStyle(STYLE_IMAGE_OPA, OPA_OPAQUE);

    sLabel_.SetPosition(LABEL_X, LABEL_Y);
    sLabel_.Resize(LABEL_W, LABEL_H);
    sLabel_.SetFont(DEFAULT_VECTOR_FONT_FILENAME, LABEL_FONT);
    sLabel_.SetAlign(TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_CENTER);
    sLabel_.SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
    sLabel_.SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    sLabel_.SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    sLabel_.SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);

    itemLabel_.SetPosition(ITEM_LABEL_X, ITEM_LABEL_Y);
    itemLabel_.Resize(ITEM_LABEL_W, ITEM_LABEL_H);
    itemLabel_.SetFont(DEFAULT_VECTOR_FONT_FILENAME, ITEM_LABEL_FONT);
    itemLabel_.SetAlign(TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_CENTER);
    itemLabel_.SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
    itemLabel_.SetStyle(STYLE_TEXT_COLOR, Color::Gray().full);
    itemLabel_.SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    itemLabel_.SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);

    sNextImage_.SetPosition(IMAGE_X, IMAGE_Y);
    sNextImage_.SetStyle(STYLE_IMAGE_OPA, OPA_OPAQUE);
    LOADIMG::LoadImageViewImg(&sNextImage_, FLASHLIGHT_IMAGE, IMAGE_FLASHLIGHT_RIGHTARROW);

    Add(&sIcon_);
    Add(&sLabel_);
    Add(&itemLabel_);
    Add(&sNextImage_);
}

FlashLightSetItemView::~FlashLightSetItemView()
{
    Remove(&sIcon_);
    Remove(&sLabel_);
    Remove(&itemLabel_);
    Remove(&sNextImage_);
    ImageCacheManager::GetInstance().UnloadSingleRes(FLASHLIGHT_IMAGE);
}

void FlashLightSetItemView::SetItemInfo(const FlashLightSetItem itemInfo)
{
    itemInfo_ = itemInfo;
    sLabel_.SetText(itemInfo.label.c_str());
    itemLabel_.SetText(itemInfo.itemLabel.c_str());
    SetViewId(itemInfo_.viewId.c_str());
    LOADIMG::LoadImageViewImg(&sIcon_, FLASHLIGHT_IMAGE, itemInfo.resId);
}
}
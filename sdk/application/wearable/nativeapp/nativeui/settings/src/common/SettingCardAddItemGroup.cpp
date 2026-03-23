/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingCardAddItemGroup
 * Create: 2025-06-01
 */
#include "settings/common/SettingCommon.h"
#include "settings/common/SettingCardAddItemGroup.h"

namespace OHOS {
static constexpr int16_t SETTING_CARD_ADD_ITEM_WIDTH = 157;
static constexpr int16_t SETTING_CARD_ADD_ITEM_HEIGHT = 397;
static constexpr int16_t SETTING_CARD_ADD_ITEM_MARGIN = 15;
static constexpr int16_t SETTING_CARD_ADD_ITEM_IMAGE_WIDTH = 157;
static constexpr int16_t SETTING_CARD_ADD_ITEM_IMAGE_HEIGHT = 157;
static constexpr int16_t SETTING_CARD_ITEM_BUTTON_X = 4;
static constexpr int16_t SETTING_CARD_ITEM_BUTTON_Y = 202;
static constexpr int16_t SETTING_CARD_ITEM_BUTTON_WIDTH = 150;
static constexpr int16_t SETTING_CARD_ITEM_BUTTON_HEIGHT = 70;

SettingCardAddItemGroup::SettingCardAddItemGroup()
{
    SetPosition(0, 0, SETTING_CARD_ADD_ITEM_WIDTH, SETTING_CARD_ADD_ITEM_HEIGHT);
    SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    SetStyle(STYLE_BACKGROUND_OPA, UN_OPACITY);
    SetStyle(STYLE_MARGIN_LEFT, SETTING_CARD_ADD_ITEM_MARGIN);
    SetStyle(STYLE_MARGIN_RIGHT, SETTING_CARD_ADD_ITEM_MARGIN);

    image_.SetAutoEnable(false);
    image_.SetPosition(0, 0, SETTING_CARD_ADD_ITEM_IMAGE_WIDTH, SETTING_CARD_ADD_ITEM_IMAGE_HEIGHT);
    image_.SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    image_.SetStyle(STYLE_BACKGROUND_OPA, UN_OPACITY);
    image_.SetResizeMode(UIImageView::CONTAIN);

    button_.SetPosition(SETTING_CARD_ITEM_BUTTON_X, SETTING_CARD_ITEM_BUTTON_Y);
    button_.Resize(SETTING_CARD_ITEM_BUTTON_WIDTH, SETTING_CARD_ITEM_BUTTON_HEIGHT);
    button_.SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Black().full, UIButton::PRESSED);
    button_.SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Black().full, UIButton::RELEASED);
    button_.SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Black().full, UIButton::INACTIVE);
    button_.SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::PRESSED);
    button_.SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::RELEASED);
    button_.SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::INACTIVE);

    Add(&image_);
    Add(&button_);
}

SettingCardAddItemGroup::~SettingCardAddItemGroup()
{
    if (imageData_ != nullptr && imageSrc_ != nullptr) {
        ImageCacheManager::GetInstance().UnloadSingleRes(imageSrc_);
        imageData_ = nullptr;
        imageSrc_ = nullptr;
    }
}

void SettingCardAddItemGroup::UpdateItemGroup(SettingCardItemData* itemData)
{
    if (imageData_ != nullptr && imageSrc_ != nullptr) {
        // 通过LoadSingleRes加载的图片不会自动释放
        ImageCacheManager::GetInstance().UnloadSingleRes(imageSrc_);
        imageData_ = nullptr;
        imageSrc_ = nullptr;
    }

    imageSrc_ = itemData->image;
    imageData_ = ImageCacheManager::GetInstance().LoadSingleRes(imageSrc_);
    image_.SetSrc(imageData_);
    if (itemData->isSelect) {
        ImageInfo* image = ImageCacheManager::GetInstance().LoadOneInMultiRes(IMAGE_CARD_ADD_ADDED, SETTING_IMAGE);
        button_.SetImageSrc(image, image);
        button_.SetTouchable(false);
    } else {
        ImageInfo* image = ImageCacheManager::GetInstance().LoadOneInMultiRes(IMAGE_CARD_ADD_ADD, SETTING_IMAGE);
        button_.SetImageSrc(image, image);
        button_.SetTouchable(true);
    }
}
}

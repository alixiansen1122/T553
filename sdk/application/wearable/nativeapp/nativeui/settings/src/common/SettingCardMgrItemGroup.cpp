/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingCardMgrItemGroup
 * Create: 2025-06-01
 */
#include "settings/common/SettingCommon.h"
#include "settings/common/SettingCardMgrItemGroup.h"

namespace OHOS {
static constexpr int16_t SETTING_CARD_MGR_ITEM_WIDTH = 454;
static constexpr int16_t SETTING_CARD_MGR_ITEM_HEIGHT = 245;

SettingCardMgrItemGroup::SettingCardMgrItemGroup()
{
    SetPosition(0, 0, SETTING_CARD_MGR_ITEM_WIDTH, SETTING_CARD_MGR_ITEM_HEIGHT);
    SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    SetStyle(STYLE_BACKGROUND_OPA, UN_OPACITY);

    image_.SetAutoEnable(false);
    image_.SetPosition(0, 0, SETTING_CARD_MGR_ITEM_WIDTH, SETTING_CARD_MGR_ITEM_HEIGHT);
    image_.SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    image_.SetStyle(STYLE_BACKGROUND_OPA, UN_OPACITY);
    image_.SetResizeMode(UIImageView::CONTAIN);
    Add(&image_);
}

SettingCardMgrItemGroup::~SettingCardMgrItemGroup()
{
    if (imageData_ != nullptr && imageSrc_ != nullptr) {
        ImageCacheManager::GetInstance().UnloadSingleRes(imageSrc_);
        imageData_ = nullptr;
        imageSrc_ = nullptr;
    }
}

void SettingCardMgrItemGroup::UpdateItemGroup(SettingCardItemData* itemData)
{
    if (imageData_ != nullptr && imageSrc_ != nullptr) {
        ImageCacheManager::GetInstance().UnloadSingleRes(imageSrc_);
        imageData_ = nullptr;
        imageSrc_ = nullptr;
    }
    imageSrc_ = itemData->image;
    imageData_ = ImageCacheManager::GetInstance().LoadSingleRes(imageSrc_);
    image_.SetSrc(imageData_);
}
}

/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingCardAddItemGroup
 * Create: 2025-06-01
 */
#ifndef SETTING_CARD_ADD_ITEM_GROUP_H
#define SETTING_CARD_ADD_ITEM_GROUP_H

#include "components/ui_view_group.h"
#include "components/ui_image_view.h"
#include "components/ui_button.h"
#include "UiConfig.h"
#include "settings/common/SettingCommon.h"
#include "settings/common/SettingCardMgrItemGroup.h"

namespace OHOS {
class SettingCardAddItemGroup : public ItemGroupImageCache,
                                public UIViewGroup {
public:
    explicit SettingCardAddItemGroup();
    virtual ~SettingCardAddItemGroup();
    void UpdateItemGroup(SettingCardItemData* itemData);
private:
    const char*   imageSrc_ = nullptr;
    ImageInfo*    imageData_ = nullptr;
    UIImageView   image_;
    UIButton      button_;
};
}
#endif
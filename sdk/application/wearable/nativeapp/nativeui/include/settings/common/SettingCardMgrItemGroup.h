/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingCardMgrItemGroup
 * Create: 2025-06-01
 */
#ifndef SETTING_CARD_MGR_ITEM_GROUP_H
#define SETTING_CARD_MGR_ITEM_GROUP_H

#include "components/ui_view_group.h"
#include "components/ui_image_view.h"
#include "UiConfig.h"

namespace OHOS {
struct SettingCardItemData {
    uint8_t index;
    uint8_t id;
    const char* image = nullptr;
    bool isSelect;
    void* contextPtr = nullptr;
};

class SettingCardMgrItemGroup : public UIViewGroup {
public:
    explicit SettingCardMgrItemGroup();
    virtual ~SettingCardMgrItemGroup() override;
    void UpdateItemGroup(SettingCardItemData* itemData);
private:
    const char*   imageSrc_ = nullptr;
    ImageInfo*    imageData_ = nullptr;
    UIImageView   image_;
};
}
#endif
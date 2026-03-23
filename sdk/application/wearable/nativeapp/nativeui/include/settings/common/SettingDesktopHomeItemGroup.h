/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingDesktopHomeItemGroup
 * Create: 2025-06-01
 */
#ifndef SETTING_DESKTOP_HOME_ITEM_GROUP_H
#define SETTING_DESKTOP_HOME_ITEM_GROUP_H

#include "components/ui_view_group.h"
#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "components/ui_radio_button.h"
#include "settings/common/SettingCommon.h"

namespace OHOS {
struct SettingDesktopHomeItemData {
    uint8_t style;         // 风格
    uint32_t selectIcon;   // 选中图标
    uint32_t unSelectIcon; // 未选中图标
    const char* label;     // 文本内容
    bool isSelect;         // 是否选中
    void* contextPtr;      // 上下文指针,内部使用
};

class SettingDesktopHomeItemGroup : public ItemGroupImageCache,
                                    public UIViewGroup {
public:
    explicit SettingDesktopHomeItemGroup();
    ~SettingDesktopHomeItemGroup() override;
    void UpdateItemGroup(SettingDesktopHomeItemData* itemData);
private:
    UIImageView   image_;
    UILabel       label_;
    UIRadioButton button_;
};
}
#endif
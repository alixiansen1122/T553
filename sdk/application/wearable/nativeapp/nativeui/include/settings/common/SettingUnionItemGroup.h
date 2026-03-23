/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingUnionItemGroup
 * Create: 2025-06-01
 */
#ifndef SETTING_UNION_ITEM_GROUP_H
#define SETTING_UNION_ITEM_GROUP_H

#include <vector>
#include <unordered_map>
#include "components/ui_label.h"
#include "components/ui_view_group.h"
#include "components/ui_image_view.h"
#include "UiConfig.h"
#include "AppViewIDs.h"
#include "settings/common/SettingCommon.h"
#include "settings/common/SettingToggleButton.h"

namespace OHOS {
enum SettingUnionItemType : uint16_t {
    PLAIN_TEXT,           // 纯文本类型的item
    ICON_TEXT_TOGGLE,     // 按钮切换类型item
    ICON_TEXT_NAVIGATION, // 页面切换类型item
};

struct PlainTextItemData {
    const char* textContent;  // 文本内容
};

struct IconTextToggleItemData {
    uint32_t icon;           // 图标
    const char* textContent; // 文本内容
    bool selected;           // 是否选择
    ColorType bgColor;       // item背景色
    uint8_t bgOpa;           // item背景透明度
};

struct IconTextNavigationItemData {
    SettingPageId pageId;    // 页面ID
    uint32_t icon;           // 图标
    const char* textContent; // 固定文本内容
    const char* dataContent; // 动态数据内容
    bool hasSecondMenu;      // 是否有二级页面
    ColorType bgColor;       // item背景色
    uint8_t bgOpa;           // item背景透明度
};

class SettingUnionItemData {
public:
    SettingUnionItemType GetUnionItemType();
    void* GetUnionItemData();
    
    // 新增加到union的data类型请在这里注册
    SettingUnionItemData(const PlainTextItemData& itemData);
    SettingUnionItemData(const IconTextToggleItemData& itemData);
    SettingUnionItemData(const IconTextNavigationItemData& itemData);

    // 新增加到union的data类型请在这里注册
    union {
        PlainTextItemData plainTextItemData;
        IconTextToggleItemData iconTextToggleItemData;
        IconTextNavigationItemData iconTextNavigationItemData;
    } unionItemData_;
private:
    SettingUnionItemType type_;
    void* contextPtr_ = nullptr; // 用于SimpleList的Create和Update静态函数中使用
};

class PlainTextItemView : public UIViewGroup, public ItemGroupImageCache {
public:
    explicit PlainTextItemView();
    ~PlainTextItemView() override;
    void UpdateViewData(PlainTextItemData* data);
private:
    UILabel label_;
};

class IconTextToggleItemView : public UIViewGroup, public ItemGroupImageCache {
public:
    explicit IconTextToggleItemView();
    ~IconTextToggleItemView() override;
    void UpdateViewData(IconTextToggleItemData* data);
private:
    UIImageView icon_;
    UILabel label_;
    SettingToggleButton toggleButton_;
};

class IconTextNavigationItemView : public UIViewGroup, public ItemGroupImageCache {
public:
    explicit IconTextNavigationItemView();
    ~IconTextNavigationItemView() override;
    void UpdateViewData(IconTextNavigationItemData* data);
private:
    UIImageView icon_;
    UILabel textLabel_;
    UILabel dataLabel_;
    UIImageView nextImage_;
};
}
#endif

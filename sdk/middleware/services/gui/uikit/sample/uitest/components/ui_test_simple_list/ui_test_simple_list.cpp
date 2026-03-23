/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: List with built-in adapter sample
 * Author: Hisi Team
 * Created: 2025-07
 */

#include "ui_test_simple_list.h"
#include "common/screen.h"
#include "components/ui_menu_item.h"
#include "common/image_cache_manager.h"
#include "graphic_config.h"
#include "ui_test_list_fps/image_text_adapter.h"
#include "ui_test_list_fps/ui_list_fps_res.h"

namespace OHOS {
static const int16_t LIST_WIDTH = 454;
static const int16_t LIST_HEIGHT = 454;
static const uint16_t ITEM_WIDTH = 400;
static const uint16_t ITEM_HEIGHT = 100;
static const uint16_t LIST_FONT_SIZE = 40;

static ItemNode g_itemNode[] = {
    {APPLIST_ALARM, "闹钟"},
    {BLUETOOTH_TITLE, "蓝牙"},
    {APPLIST_CALLER_IMAGE, "通话记录"},
    {APPLIST_COMPASS_IMAGE, "指南针"},
    {APPLIST_SETTING, "设置"},
    {APPLIST_SPO2, "血氧饱和度"},
    {APPLIST_VOCASSIST_IMAGE, "语音助手"},
    {APPLIST_ALARM, "长文本滚动测试ABCD"},
    {BLUETOOTH_TITLE, "蓝牙"},
    {APPLIST_CALLER_IMAGE, "通话记录"},
    {APPLIST_COMPASS_IMAGE, "指南针"},
    {APPLIST_SETTING, "设置"},
    {APPLIST_SPO2, "血氧饱和度"},
    {APPLIST_VOCASSIST_IMAGE, "语音助手"}
};

static UIView* CreateViewCb(uint8_t type)
{
    UIMenuItem* item = new UIMenuItem();
    if (item == nullptr) {
        return nullptr;
    }

    item->Resize(ITEM_WIDTH, ITEM_HEIGHT);
    item->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    item->SetImgResizeMode(UIImageView::ImageResizeMode::CENTER);
    item->SetFont(DEFAULT_VECTOR_FONT_FILENAME, LIST_FONT_SIZE);
    item->SetScaleAlignMode(UIMenuItem::ScaleAlignMode::ALIGN_ITEM_CENTER);
    item->SetLineBreakMode(UILabel::LineBreakMode::LINE_BREAK_MARQUEE);
    return item;
}

static void UpdateViewCb(UIView* view, void* data, uint8_t type)
{
    if (view == nullptr || data == nullptr) {
        return;
    }
    UIMenuItem* item = dynamic_cast<UIMenuItem*>(view);
    ItemNode* content = static_cast<ItemNode *>(data);
    if (content->imgInfo == nullptr) {
        content->imgInfo = ImageCacheManager::GetInstance().LoadOneInMultiRes(content->resId, LIST_FPS);
    }
    item->SetImageSrc(content->imgInfo);
    item->SetText(content->txt);
    item->SetAlign(TEXT_ALIGNMENT_CENTER);
}

void UITestSimpleList::SetUp()
{
    if (list_ == nullptr) {
        list_ = new UISimpleList();
    }
    list_->SetPosition(0, 0, LIST_WIDTH, LIST_HEIGHT);
    list_->SetThrowDrag(true);
    list_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    list_->SetScrollBlankSize(LIST_HEIGHT / 2); // 2: divisor

    Contents* content = nullptr;
    for (uint8_t i = 0; i < (sizeof(g_itemNode) / sizeof(ItemNode)); ++i) {
        content = new Contents();
        if (content == nullptr) {
            continue;
        }
        content->createFunc = CreateViewCb;
        content->updateFunc = UpdateViewCb;
        content->type = 1;
        content->data = reinterpret_cast<void *>(&g_itemNode[i]);
        data_.PushBack(content);
        list_->AddContent(*content);
    }
    list_->RefreshList();
}

void UITestSimpleList::TearDown()
{
    if (list_ != nullptr) {
        delete list_;
        list_ = nullptr;
    }

    while (!data_.IsEmpty()) {
        Contents* element = data_.Back();
        if (element != nullptr) {
            delete element;
        }
        data_.PopBack();
    }

    ImageCacheManager::GetInstance().UnloadAllInMultiRes(LIST_FPS);
    for (uint8_t i = 0; i < (sizeof(g_itemNode) / sizeof(ItemNode)); ++i) {
        g_itemNode[i].imgInfo = nullptr;
    }
}

UIView* UITestSimpleList::GetTestView()
{
    return list_;
}
} // namespace OHOS

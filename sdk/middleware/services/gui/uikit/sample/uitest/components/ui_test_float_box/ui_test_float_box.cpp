/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: UITestFloatBoxView
 * Author: Hisi Graphic Team
 * Created: 2025-8
 */

#include "ui_test_float_box.h"
#include "common/screen.h"
#include "font/ui_font.h"
#include "components/ui_float_box_view.h"
#include "common/image_cache_manager.h"

namespace OHOS {
namespace {
const int16_t BTN_W = 80;
const int16_t BTN_H = 50;

static constexpr uint8_t MAX_ITEM_CNT = 3;
static FloatItem g_floatItems[MAX_ITEM_CNT] = {
    {"SPORT", nullptr},
    {"SETTING", nullptr},
    {"SLEEP", nullptr},
};

static const char* g_floatItemsPath[MAX_ITEM_CNT] = {
    RES_PATH"SPORT_ICON.bin",
    RES_PATH"SETTING_ICON.bin",
    RES_PATH"SLEEP_ICON.bin",
};
} // namespace

void UITestFloatBox::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->SetThrowDrag(true);
        container_->SetHorizontalScrollState(false);
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());

        UIView* view = new UIView();
        view->SetPosition(0, 0, Screen::GetInstance().GetWidth(), 800); // 800: height
        view->SetStyle(STYLE_BACKGROUND_COLOR, Color::White().full);
        container_->Add(view);
    }

    UIScrollView* group = new UIScrollView();
    // 50: x, y; 100: screen width and height - 100
    group->SetPosition(50, 50, Screen::GetInstance().GetWidth() - 100, Screen::GetInstance().GetHeight() - 100);
    group->SetStyle(STYLE_BACKGROUND_COLOR, Color::Gray().full);
    group->SetIntercept(true);
    container_->Add(group);

    UIView* view = new UIView();
    view->SetPosition(0, 0, 800, 800); // 800: width and height
    group->Add(view);

    if (floatBox_ == nullptr) {
        floatBox_ = new UIFloatBoxView();
    }
    // 200: x, y; 150: screen width and height - 150
    floatBox_->SetPosition(200, 200, Screen::GetInstance().GetWidth() - 150, Screen::GetInstance().GetHeight() - 150);
#if ENABLE_ROTATE_INPUT
    floatBox_->RequestFocus();
#endif
    LoadItemImgs();
    floatBox_->AddItems(g_floatItems, 2); // 2: item cnt
    floatBox_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Yellow().full);
    floatBox_->SetItemSize(60); // 60: item size
    floatBox_->SetItemSpacing(10); // 10: item spacing
    floatBox_->SetDraggable(true);
    floatBox_->SetTouchable(true);
    floatBox_->SetIntercept(true);
    floatBox_->SetDragFactor(1.5f);
    floatBox_->SetAccFactor(6.0f);
    floatBox_->SetRotateFactor(5.0f);
    floatBox_->SetAlignAvgSpeed(2.0f);
    floatBox_->SetOnItemClickListener(this);
    group->Add(floatBox_);

    if (switchBtn_ == nullptr) {
        switchBtn_ = new UILabelButton();
    }
    switchBtn_->SetPosition((Screen::GetInstance().GetWidth() - BTN_W) / 2, // 2: divider
        Screen::GetInstance().GetHeight() - BTN_H, BTN_W, BTN_H); // 2: divider
    switchBtn_->SetOnClickListener(this);
    switchBtn_->SetText("Switch");
    container_->Add(switchBtn_);
}

void UITestFloatBox::LoadItemImgs()
{
    for (uint8_t i = 0; i < MAX_ITEM_CNT; i++) {
        if (g_floatItems[i].imgInfo == nullptr) {
            g_floatItems[i].imgInfo = ImageCacheManager::GetInstance().LoadSingleRes(g_floatItemsPath[i]);
        }
    }
}

bool UITestFloatBox::OnClick(UIView& view, const ClickEvent& event)
{
    if (&view == switchBtn_) {
        if (floatBox_ == nullptr) {
            return false;
        }
        uint16_t itemCnt = floatBox_->GetItemCnt();
        if (itemCnt == 2) { // 2: item cnt
            floatBox_->AddItems(g_floatItems, 3); // 3: item Cnt
            printf("Add 3 items\n");
        } else {
            floatBox_->AddItems(g_floatItems, 2); // 2: item Cnt
            printf("Add 2 items\n");
        }
    }
    return true;
}

void UITestFloatBox::TearDown()
{
#if ENABLE_ROTATE_INPUT
    floatBox_->ClearFocus();
#endif
    DeleteChildren(container_);
    container_ = nullptr;
    floatBox_ = nullptr;
    switchBtn_ = nullptr;

    for (uint8_t i = 0; i < MAX_ITEM_CNT; i++) {
        if (g_floatItems[i].imgInfo != nullptr) {
            ImageCacheManager::GetInstance().UnloadSingleRes(g_floatItemsPath[i]);
            g_floatItems[i].imgInfo = nullptr;
        }
    }
}

const UIView* UITestFloatBox::GetTestView()
{
    return container_;
}

void UITestFloatBox::OnItemClick(const std::string& name)
{
    printf("Click Item: %s\n", name.c_str());
}
}

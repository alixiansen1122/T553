/*
 * Copyright (c) 2022 CompanyNameMagicTag.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ui_test_list_ext.h"
#include "animator/animator_manager.h"
#include "common/screen.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_image_view.h"
#include "core/render_manager.h"
#include "graphic_config.h"
#include "securec.h"
#include "ui_test_list_fps/ui_list_fps_res.h"

namespace OHOS {
static const int16_t LIST_WIDTH = 454;
static const int16_t LIST_HEIGHT = 454;
static const int16_t SCROLL_STEP = 10;
static const uint16_t ITEM_WIDTH = 400;
static const uint16_t ITEM_HEIGHT = 100;
static const uint16_t IMAGE_WIDTH = 100;
static const uint16_t LIST_FONT_SIZE = 40;
static int16_t g_scrollStep = 10;
static int16_t g_scrollDistance = 0;
static int16_t g_scrollMax = 0;

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

void UITestListExt::SetUp()
{
    if (data_ == nullptr) {
        data_ = new List<ItemNode*>();
        if (data_ == nullptr) {
            return;
        }
        for (uint8_t i = 0; i < (sizeof(g_itemNode) / sizeof(ItemNode)); ++i) {
            data_->PushBack(&g_itemNode[i]);
        }
    }
}

void UITestListExt::TearDown()
{
    if (animator_ != nullptr) {
        animator_->Stop();
        delete animator_;
        animator_ = nullptr;
    }

    if (list_ != nullptr) {
        list_->SetLoopState(false);
        list_->SetAdapter(nullptr);
        list_->RefreshList();
        delete list_;
        list_ = nullptr;
    }

    if (data_ != nullptr) {
        data_->Clear();
        delete data_;
        data_ = nullptr;
    }

    if (adapter_ != nullptr) {
        delete adapter_;
        adapter_ = nullptr;
    }
}


bool UITestListExt::TestList::OnPressEvent(const PressEvent& event)
{
    if (animator_ != nullptr) {
        animator_->Stop();
    }
    return UIList::OnPressEvent(event);
}

bool UITestListExt::TestList::OnLongPressEvent(const LongPressEvent& event)
{
    if (animator_ != nullptr) {
        animator_->Start();
    }
    return true;
}

void UITestListExt::TestList::SetAnimator(Animator* animator)
{
    animator_ = animator;
}

void UITestListExt::ListExtFpsCallback::Callback(UIView* view)
{
    UIList* list = static_cast<UIList*>(view);
    if (list != nullptr) {
        if (g_scrollDistance <= 0) {
            g_scrollStep = -1 * g_scrollStep;
            g_scrollDistance = g_scrollMax;
        }
        list->ScrollBy(g_scrollStep);
        g_scrollDistance -= abs(g_scrollStep);
    }
}

UIView* UITestListExt::GetTestView()
{
    UIListScrollTestFPS();
    return list_;
}

void UITestListExt::UIListScrollTestFPS()
{
    g_scrollMax = (sizeof(g_itemNode) / sizeof(ItemNode)) * ITEM_HEIGHT - LIST_HEIGHT;
    g_scrollDistance = g_scrollMax;

    adapter_ = new MenuItemAdapter();
    if (adapter_ == nullptr) {
        TearDown();
        return;
    }
    adapter_->SetWidth(ITEM_WIDTH);
    adapter_->SetHeight(ITEM_HEIGHT);
    adapter_->SetImageWidth(IMAGE_WIDTH);
    adapter_->SetImageHeight(ITEM_HEIGHT);
    adapter_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, LIST_FONT_SIZE);
    adapter_->SetData(data_);
    if (list_ == nullptr) {
        list_ = new TestList();
    }
    list_->SetPosition(0, 0, LIST_WIDTH, LIST_HEIGHT);
    list_->SetAdapter(adapter_);
    list_->SetThrowDrag(true);
    list_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    list_->SetMaxScrollDistance(1000); // 1000: max scroll distance
    list_->SetSwipeACCLevel(200); // 200: compensation distance
    list_->SetDragACCLevel(5); // 5: drag acceleration
    list_->SetYScrollBarVisible(true);

    list_->EnableAutoAlign(true);
    list_->SetSelectPosition(LIST_HEIGHT / 2); // 2: divisor
    list_->EnableCrossDragBack(false);
    list_->SetElastic(true);
    list_->SetReboundSize(LIST_HEIGHT / 2); // 2: divisor
    list_->SetItemSpace(30); // 30: item space
    list_->SetAutoAlignTime(300); // 300: align time
    list_->SetScrollBlankSize(LIST_HEIGHT / 2); // 2: divisor
    list_->SetDragFunc(EasingEquation::QuadEaseOut);

    if (animator_ == nullptr) {
        animator_ = new Animator(&callback_, list_, DEFAULT_TASK_PERIOD, true);
    }
    list_->SetAnimator(animator_);
}
} // namespace OHOS

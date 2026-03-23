/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "ui_test_rotate_injector.h"

#if ENABLE_DEBUG && ENABLE_ROTATE_INPUT
#include "dfx/rotate_event_injector.h"
#include "dock/focus_manager.h"
namespace OHOS {
const static int16_t START_X = 100;
const static int16_t START_Y = 0;
const static int16_t GAP = 10;
const static int16_t WIDTH_200 = 200;
const static int16_t HEIGHT_150 = 150;
const static int16_t WIDTH_100 = 100;
const static int16_t HEIGHT_100 = 100;
const static int16_t ROTATE_FACTOR_2 = 2;
static int16_t g_xCoord = START_X;
static int16_t g_yCoord = START_Y;
static const char* g_pickerRange[] = {"A0", "B1", "C2", "D3", "E4", "F5", "G6", "H7", "I8", "J9", "K10", "L11"};

void UITestRotateInjector::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->SetThrowDrag(true);
        container_->SetHorizontalScrollState(false);
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    }
}

void UITestRotateInjector::TearDown()
{
    FocusManager::GetInstance()->ClearFocus();
    container_->Remove(picker_);
    delete picker_;
    picker_ = nullptr;
    DeleteChildren(container_);
    container_ = nullptr;
    scroll_ = nullptr;
    swipe_ = nullptr;

    g_xCoord = START_X;
    g_yCoord = START_Y;
}

const UIView* UITestRotateInjector::GetTestView()
{
    TestScrollView();
    TestSwipeView();
    TestPicker();
    return container_;
}

bool UITestRotateInjector::OnPress(UIView& view, const PressEvent& event)
{
    if (&view == scroll_) {
        scroll_->RequestFocus();
        return true;
    } else if (&view == swipe_) {
        swipe_->RequestFocus();
        return true;
    } else if (&view == picker_) {
        picker_->RequestFocus();
        return true;
    }
    return false;
}

void UITestRotateInjector::TestScrollView()
{
    if (container_ == nullptr) {
        return;
    }

    UILabel* label = new UILabel();
    label->SetText("UIScrollView Test");
    label->SetPosition(g_xCoord, g_yCoord, TEST_LABEL_WIDTH, TEST_LABEL_HEIGHT);
    container_->Add(label);
    g_yCoord += TEST_LABEL_HEIGHT + GAP;

    if (scroll_ == nullptr) {
        scroll_ = new UIScrollView();
    }

    scroll_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    scroll_->SetPosition(g_xCoord, g_yCoord, WIDTH_200, HEIGHT_150);
    scroll_->SetRotateFactor(ROTATE_FACTOR_2);
    UIButton* btn = new UIButton();
    btn->SetPosition(0, 0, 300, 300); // 300: button width and height
    scroll_->Add(btn);
    scroll_->SetThrowDrag(true);
    scroll_->SetOnTouchListener(this);
    scroll_->SetReboundSize(20); // 20: rebound size
    container_->Add(scroll_);
    g_yCoord += HEIGHT_150 + GAP;
}

void UITestRotateInjector::TestPicker()
{
    if (container_ == nullptr) {
        return;
    }

    UILabel* label = new UILabel();
    label->SetText("UIPicker Test");
    label->SetPosition(g_xCoord, g_yCoord, TEST_LABEL_WIDTH, TEST_LABEL_HEIGHT);
    container_->Add(label);
    g_yCoord += TEST_LABEL_HEIGHT + GAP;

    if (picker_ == nullptr) {
        picker_ = new UIPicker();
    }

    picker_->SetPosition(g_xCoord, g_yCoord, WIDTH_200, HEIGHT_150);
    picker_->SetItemHeight(50); // 50: height
    picker_->SetValues(g_pickerRange, sizeof(g_pickerRange) / sizeof(g_pickerRange[0]));
    picker_->SetOnTouchListener(this);
    picker_->SetRotateFactor(ROTATE_FACTOR_2);
    container_->Add(picker_);
    g_yCoord += HEIGHT_150 + GAP;
}

void UITestRotateInjector::TestSwipeView()
{
    if (container_ == nullptr) {
        return;
    }

    UILabel* label = new UILabel();
    label->SetText("UISwipeView Test");
    label->SetPosition(g_xCoord, g_yCoord, TEST_LABEL_WIDTH, TEST_LABEL_HEIGHT);
    container_->Add(label);
    g_yCoord += TEST_LABEL_HEIGHT + GAP;

    if (swipe_ == nullptr) {
        swipe_ = new UISwipeView(UISwipeView::HORIZONTAL);
    }

    swipe_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    swipe_->SetPosition(g_xCoord, g_yCoord, WIDTH_200, HEIGHT_150);
    swipe_->SetBlankSize(100); // 100: blank size
    UILabelButton* btn1 = new UILabelButton();
    UILabelButton* btn2 = new UILabelButton();
    UILabelButton* btn3 = new UILabelButton();
    UILabelButton* btn4 = new UILabelButton();
    UILabelButton* btn5 = new UILabelButton();
    btn1->SetPosition(0, 0, WIDTH_100, HEIGHT_100);
    btn2->SetPosition(0, 0, WIDTH_100, HEIGHT_100);
    btn3->SetPosition(0, 0, WIDTH_100, HEIGHT_100);
    btn4->SetPosition(0, 0, WIDTH_100, HEIGHT_100);
    btn5->SetPosition(0, 0, WIDTH_100, HEIGHT_100);
    btn1->SetText("Button 1");
    btn2->SetText("Button 2");
    btn3->SetText("Button 3");
    btn4->SetText("Button 4");
    btn5->SetText("Button 5");
    swipe_->Add(btn1);
    swipe_->Add(btn2);
    swipe_->Add(btn3);
    swipe_->Add(btn4);
    swipe_->Add(btn5);
    swipe_->SetOnTouchListener(this);
    swipe_->SetRotateFactor(ROTATE_FACTOR_2);
    container_->Add(swipe_);
    g_yCoord += HEIGHT_150 + GAP;
}
}
#endif
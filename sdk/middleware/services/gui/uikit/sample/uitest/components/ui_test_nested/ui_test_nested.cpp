/*
 * Copyright (c) CompanyNameMagicTag 2024. All rights reserved.
 * Description: UITestNested
 * Author:
 * Create: 2024-12-09
 */

#include "ui_test_nested.h"

namespace OHOS {
static const int16_t POS_X = 100;
static const int16_t POS_Y = 50;
static const int16_t SCROLL_WIDTH = 100;
static const int16_t SCROLL_HEIGHT = 250;
static const int16_t SWIPE_WIDTH = 250;
static const int16_t SWIPE_HEIGHT = 250;
static const int16_t LIST_WIDTH = 250;
static const int16_t LIST_HEIGHT = 250;
static const int16_t LABEL_X = 210;
static const int16_t LABEL_Y = 50;
static const int16_t LABEL_WIDTH = 140;
static const int16_t LABEL_HEIGHT = 100;
static const int16_t SCREEN_WIDTH = HORIZONTAL_RESOLUTION;
static const int16_t SCREEN_HEIGHT = VERTICAL_RESOLUTION;
static const int16_t DATA_NUM = 100;
static const int16_t STR_LEN = 20;
static char g_testData1[DATA_NUM][STR_LEN];
static char g_testData2[DATA_NUM][STR_LEN];
static const char STR[] = "预期测试现象：\n"
"1. list上下滑动时，整个界面不跟随移动。\n"
"2. 左右滑动时，list不上下移动。\n"
"3. 向右滑至边界时，不松手左滑，整个界面跟随平移。\n"
"4. 一开始就向左滑动时，展现出swipe view特性；向右滑动且没到边界时展现swipe view特性.\n";

void UITestNested::SetUp()
{
    list1_ = new UIListNested();
    list1_->Resize(LIST_WIDTH, LIST_HEIGHT);
    list1_->SetThrowDrag(true);
    for (int16_t i = 0; i < DATA_NUM; ++i) {
        sprintf_s(g_testData1[i], STR_LEN, "test %d", i);
        data1_.PushBack(g_testData1[i]);
    }
    adp1_.SetData(&data1_);
    list1_->SetAdapter(&adp1_);

    list2_ = new UIListNested();
    list2_->Resize(LIST_WIDTH, LIST_HEIGHT);
    list2_->SetThrowDrag(true);
    for (int16_t i = 0; i < DATA_NUM; ++i) {
        sprintf_s(g_testData2[i], STR_LEN, "Test %d", i);
        data2_.PushBack(g_testData2[i]);
    }
    adp2_.SetData(&data2_);
    list2_->SetAdapter(&adp2_);

    swipe_ = new UISwipeViewNested(UIAbstractScroll::HORIZONTAL);
    swipe_->Resize(SWIPE_WIDTH, SWIPE_HEIGHT);
    swipe_->SetBlankSize(0);
    swipe_->SetBoundaryIntercept(DragEvent::DIRECTION_LEFT_TO_RIGHT, false);
    swipe_->Add(list1_);
    swipe_->Add(list2_);
    list1_->SetDraggable(true);
    list2_->SetDraggable(true);

    scroll_ = new UIScrollViewNested(UIAbstractScroll::HORIZONTAL);
    scroll_->SetPosition(POS_X, POS_Y, SCROLL_WIDTH, SCROLL_HEIGHT);
    scroll_->Add(swipe_);

    label_ = new UILabel;
    label_->SetPosition(LABEL_X, LABEL_Y, LABEL_WIDTH, LABEL_HEIGHT);
    label_->SetText(STR);
    label_->SetLineBreakMode(UILabel::LineBreakMode::LINE_BREAK_WRAP);

    container_ = new UIViewGroup();
    container_->Resize(SCREEN_WIDTH, SCREEN_HEIGHT);
    container_->Add(scroll_);
    container_->Add(label_);
}

void UITestNested::TearDown()
{
    if (container_ != nullptr) {
        container_->RemoveAll();
        delete container_;
        container_ = nullptr;
    }

    if (scroll_ != nullptr) {
        scroll_->RemoveAll();
        delete scroll_;
        scroll_ = nullptr;
    }

    if (swipe_ != nullptr) {
        swipe_->RemoveAll();
        delete swipe_;
        swipe_ = nullptr;
    }

    if (list2_ != nullptr) {
        delete list2_;
        list2_ = nullptr;
    }

    if (list1_ != nullptr) {
        delete list1_;
        list1_ = nullptr;
    }

    if (label_ != nullptr) {
        delete label_;
        label_ = nullptr;
    }

    data1_.Clear();
    data2_.Clear();
}

const UIView* UITestNested::GetTestView()
{
    return container_;
}

} // namespace OHOS

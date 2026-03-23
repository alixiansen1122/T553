/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: Lottie
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */

#include "ui_test_ui_swipe_view.h"

#include <string>

#include "common/screen.h"
#include "components/ui_label.h"
#include "components/ui_swipe_view.h"

namespace OHOS {
namespace {
static int16_t g_buttonH = 80;
static int16_t g_buttonW = 400;
static int16_t g_blank = 20;
static int16_t g_swipeH = 200;
static int16_t g_swipeW = 400;
static int16_t g_swipeHorH = 110;
static int16_t g_deltaCoordinateY = 19;
static int16_t g_deltaCoordinateY2 = 37;
static uint16_t g_maxIndex = 3;
} // namespace

void UITestUISwipeView::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight() - BACK_BUTTON_HEIGHT);
        container_->SetThrowDrag(true);
        container_->SetHorizontalScrollState(false);
    }
}

void UITestUISwipeView::TearDown()
{
    DeleteChildren(container_);
    container_ = nullptr;
    addBtnInHead_ = nullptr;
    addBtnInTail_ = nullptr;
    addBtnInMid_ = nullptr;
    removeHeadBtn_ = nullptr;
    removeMidBtn_ = nullptr;
    removeAllBtn_ = nullptr;
    loopBtn_ = nullptr;
    changePageBtn_ = nullptr;
    lastX_ = 0;
    lastY_ = 0;
}

const UIView* UITestUISwipeView::GetTestView()
{
    UIKitSwipeViewTestHorizontal001();
    UIKitSwipeViewTestHorizontal002();
    UIKitSwipeViewTestHorizontal003();
    UIKitSwipeViewTestAlign001(UISwipeView::ALIGN_LEFT);
    UIKitSwipeViewTestAlign001(UISwipeView::ALIGN_CENTER);
    UIKitSwipeViewTestAlign001(UISwipeView::ALIGN_RIGHT);

    UIKitSwipeViewTestVer001();
    UIKitSwipeViewTestVer002();
    UIKitSwipeViewTestRemove001();

    UIKitSwipeViewTestSetCurrentPage();

    return container_;
}

void UITestUISwipeView::UIKitSwipeViewTestHorizontal001()
{
    if (container_ == nullptr) {
        return;
    }
    UILabel* empty = GetTitleLabel("");
    container_->Add(empty);
    positionY_ = TEXT_DISTANCE_TO_TOP_SIDE;
    empty->SetPosition(0, 0, Screen::GetInstance().GetWidth() / 2, positionY_);    // 2: half of screen

    UILabel* label = GetTitleLabel("    两个子元素循环水平滑动");
    container_->Add(label);
    positionX_ = TEXT_DISTANCE_TO_LEFT_SIDE;
    positionY_ += g_deltaCoordinateY2;
    label->SetPosition(positionX_, positionY_);
    positionY_ += g_deltaCoordinateY2;

    UISwipeView* swipe = new UISwipeView(UISwipeView::HORIZONTAL);
    swipe->SetIntercept(true);
    swipe->SetLoopState(true);
    swipe->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
    swipe->SetPosition(positionX_, positionY_, g_swipeW, g_swipeHorH);
    swipe->SetBlankSize(100); // 100: is blank size
    container_->Add(swipe);
    UILabelButton* button1 = new UILabelButton();
    button1->SetPosition(0, 0, g_buttonW, g_buttonH);
    button1->SetText("button1");
    swipe->Add(button1);
    UILabelButton* button2 = new UILabelButton();
    button2->SetPosition(0, 0, g_buttonW, g_buttonH);
    button2->SetText("button2");
    swipe->Add(button2);
    SetLastPos(swipe);
    positionY_ += g_swipeHorH;
}

void UITestUISwipeView::UIKitSwipeViewTestHorizontal002()
{
    if (container_ == nullptr) {
        return;
    }
    UILabel* label = GetTitleLabel("水平滑动");
    container_->Add(label);
    positionY_ += g_deltaCoordinateY;
    label->SetPosition(positionX_, positionY_);
    positionY_ += g_deltaCoordinateY2;

    UISwipeView* swipe = new UISwipeView(UISwipeView::HORIZONTAL);
    swipe->SetIntercept(true);
    swipe->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
    swipe->SetPosition(positionX_, positionY_, g_swipeW, g_swipeHorH);
    swipe->SetAnimatorTime(100); // 100: mean animator drag time(ms)
    container_->Add(swipe);
    UILabelButton* button1 = new UILabelButton();
    button1->SetPosition(0, 0, g_buttonW, g_buttonH);
    button1->SetText("button1");
    swipe->Add(button1);
    UILabelButton* button2 = new UILabelButton();
    button2->SetPosition(0, 0, g_buttonW, g_buttonH);
    button2->SetText("button2");
    swipe->Add(button2);
    UILabelButton* button3 = new UILabelButton();
    button3->SetPosition(0, 0, g_buttonW, g_buttonH);
    button3->SetText("button3");
    swipe->Add(button3);
    SetLastPos(swipe);
    positionY_ += g_swipeHorH;
}

void UITestUISwipeView::UIKitSwipeViewTestHorizontal003()
{
    if (container_ == nullptr) {
        return;
    }
    UILabel* label = GetTitleLabel("循环水平滑动");
    container_->Add(label);
    positionY_ += g_deltaCoordinateY;
    label->SetPosition(positionX_, positionY_);
    positionY_ += g_deltaCoordinateY2;

    UISwipeView* swipe = new UISwipeView(UISwipeView::HORIZONTAL);
    swipe->SetIntercept(true);
    swipe->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
    swipe->SetPosition(positionX_, positionY_, g_swipeW, g_swipeHorH);
    swipe->SetLoopState(true);
    swipe->SetAnimatorTime(100); // 100: mean animator drag time(ms)
    container_->Add(swipe);
    UILabelButton* button1 = new UILabelButton();
    button1->SetPosition(0, 0, g_buttonW, g_buttonH);
    button1->SetText("button1");
    swipe->Add(button1);
    UILabelButton* button2 = new UILabelButton();
    button2->SetPosition(0, 0, g_buttonW, g_buttonH);
    button2->SetText("button2");
    swipe->Add(button2);
    UILabelButton* button3 = new UILabelButton();
    button3->SetPosition(0, 0, g_buttonW, g_buttonH);
    button3->SetText("button3");
    swipe->Add(button3);
    SetLastPos(swipe);
    positionY_ += g_swipeHorH;
}

void UITestUISwipeView::UIKitSwipeViewTestVer001()
{
    if (container_ == nullptr) {
        return;
    }
    UILabel* label = GetTitleLabel("垂直滑动");
    container_->Add(label);
    positionY_ += g_deltaCoordinateY;
    label->SetPosition(positionX_, positionY_);
    positionY_ += g_deltaCoordinateY2;

    UISwipeView* swipe = new UISwipeView(UISwipeView::VERTICAL);
    swipe->SetIntercept(true);
    swipe->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
    swipe->SetPosition(positionX_, positionY_, g_swipeH, g_swipeW);
    container_->Add(swipe);
    UILabelButton* button1 = new UILabelButton();
    button1->SetPosition(0, 0, g_buttonH, g_buttonW);
    button1->SetText("button1");
    swipe->Add(button1);
    UILabelButton* button2 = new UILabelButton();
    button2->SetPosition(0, 0, g_buttonH, g_buttonW);
    button2->SetText("button2");
    swipe->Add(button2);
    SetLastPos(swipe);
    positionY_ += g_swipeW + g_deltaCoordinateY;
}

void UITestUISwipeView::UIKitSwipeViewTestVer002()
{
    if (container_ == nullptr) {
        return;
    }

    UILabel* label = GetTitleLabel("循环垂直滑动");
    container_->Add(label);
    label->SetPosition(positionX_, positionY_);
    positionY_ += g_deltaCoordinateY2;

    UISwipeView* swipe = new UISwipeView(UISwipeView::VERTICAL);
    swipe->SetIntercept(true);
    swipe->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
    swipe->SetPosition(positionX_, positionY_, g_swipeH, g_swipeW);
    swipe->SetLoopState(true);
    swipe->SetAnimatorTime(100); // 100: is animator drag time(ms)
    container_->Add(swipe);
    UILabelButton* button1 = new UILabelButton();
    button1->SetPosition(0, 0, g_buttonH, g_buttonW);
    button1->SetText("button1");
    swipe->Add(button1);
    UILabelButton* button2 = new UILabelButton();
    button2->SetPosition(0, 0, g_buttonH, g_buttonW);
    button2->SetText("button2");
    swipe->Add(button2);
    UILabelButton* button3 = new UILabelButton();
    button3->SetPosition(0, 0, g_buttonH, g_buttonW);
    button3->SetText("button3");
    swipe->Add(button3);
    SetLastPos(swipe);
    positionY_ += g_swipeW;
}

void UITestUISwipeView::UIKitSwipeViewTestRemove001()
{
    if (container_ == nullptr) {
        return;
    }
    UILabel* label = GetTitleLabel("增加和删除子节点");
    container_->Add(label);
    positionY_ += g_deltaCoordinateY;
    label->SetPosition(positionX_, positionY_);
    positionY_ += g_deltaCoordinateY2;

    UISwipeView* swipe = new UISwipeView(UISwipeView::HORIZONTAL);
    swipe->SetIntercept(true);
    swipe->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
    swipe->SetPosition(positionX_, positionY_, g_swipeH, g_swipeW);
    swipe->SetLoopState(true);
    swipe->SetAnimatorTime(100); // 100: mean animator drag time(ms)
    currentSwipe_ = swipe;
    container_->Add(swipe);
    InitBtn();
    positionX_ = positionX_ + swipe->GetWidth() + 20; // 20: is interval between button and swipe
    SetUpButton(addBtnInHead_, "增加至头部 ");
    SetUpButton(addBtnInTail_, "增加至尾部 ");
    SetUpButton(addBtnInMid_, "增加至头部后 ");
    SetUpButton(removeHeadBtn_, "删除头部节点 ");
    SetUpButton(removeMidBtn_, "删除中间节点 ");
    SetUpButton(removeAllBtn_, "删除全部节点 ");
    SetUpButton(loopBtn_, "设置循环 关 ");
    SetUpButton(changePageBtn_, "切换页面 ");
    SetLastPos(swipe);
}

void UITestUISwipeView::InitBtn()
{
    if (addBtnInHead_ == nullptr) {
        addBtnInHead_ = new UILabelButton();
    }
    if (addBtnInTail_ == nullptr) {
        addBtnInTail_ = new UILabelButton();
    }
    if (addBtnInMid_ == nullptr) {
        addBtnInMid_ = new UILabelButton();
    }
    if (removeHeadBtn_ == nullptr) {
        removeHeadBtn_ = new UILabelButton();
    }
    if (removeMidBtn_ == nullptr) {
        removeMidBtn_ = new UILabelButton();
    }
    if (removeAllBtn_ == nullptr) {
        removeAllBtn_ = new UILabelButton();
    }
    if (loopBtn_ == nullptr) {
        loopBtn_ = new UILabelButton();
    }
    if (changePageBtn_ == nullptr) {
        changePageBtn_ = new UILabelButton();
    }
}

void UITestUISwipeView::UIKitSwipeViewTestAlign001(UISwipeView::AlignMode alignMode)
{
    static uint8_t divNum = 4;
    if (container_ == nullptr) {
        return;
    }

    UILabel* label;
    if (alignMode == UISwipeView::ALIGN_LEFT) {
        label = GetTitleLabel("子组件左对齐");
    } else if (alignMode == UISwipeView::ALIGN_RIGHT) {
        label = GetTitleLabel("子组件右对齐");
    } else {
        label = GetTitleLabel("子组件Center对齐");
    }

    container_->Add(label);
    label->SetPosition(positionX_, lastY_ + g_blank);

    UISwipeView* swipe = new UISwipeView(UISwipeView::HORIZONTAL);
    swipe->SetIntercept(true);
    swipe->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
    positionY_ = label->GetY() + g_blank + g_deltaCoordinateY + g_swipeH;
    swipe->SetPosition(positionX_, label->GetY() + g_blank + g_deltaCoordinateY,
        g_swipeW - 100, g_swipeH);    // 100: offset
    swipe->SetLoopState(true);
    swipe->SetAnimatorTime(100); // 100: mean animator drag time(ms)
    swipe->SetAlignMode(alignMode);
    container_->Add(swipe);
    UILabelButton* button1 = new UILabelButton();
    button1->SetPosition(0, 0, g_buttonW / divNum, g_buttonH / 2); // 2: half
    button1->SetText("button1");
    swipe->Add(button1);
    UILabelButton* button2 = new UILabelButton();
    button2->SetPosition(0, 0, g_buttonW / divNum, g_buttonH / 2); // 2: half
    button2->SetText("button2");
    swipe->Add(button2);
    UILabelButton* button3 = new UILabelButton();
    button3->SetPosition(0, 0, g_buttonW / divNum, g_buttonH / 2); // 2: half
    button3->SetText("button3");
    swipe->Add(button3);

    UILabel* tmpLabel = nullptr;
    uint8_t tmpAlignMode = swipe->GetAlignMode();
    if (tmpAlignMode == alignMode) {
        tmpLabel = GetTitleLabel("The result of GetAlignMode is OK.");
    } else {
        tmpLabel = GetTitleLabel("The result of GetAlignMode is Error.");
    }
    tmpLabel->SetPosition(positionX_, 100, 250, 25); // 100: y, 250:width, 25:height
    swipe->Add(tmpLabel);

    SetLastPos(swipe);
}

void UITestUISwipeView::UIKitSwipeViewTestSetCurrentPage()
{
    if (container_ == nullptr) {
        return;
    }
    UILabel* label = GetTitleLabel("UISwipeView切换页面");
    container_->Add(label);
    positionY_ += g_deltaCoordinateY;
    label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, positionY_);
    positionY_ += g_deltaCoordinateY2;

    UISwipeView* swipe = new UISwipeView(UISwipeView::HORIZONTAL);
    swipe->SetIntercept(true);
    swipe->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
    swipe->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, positionY_, g_swipeW, g_swipeH);
    swipe->SetLoopState(loop_);
    swipe->SetAnimatorTime(1000); // 1000: mean animator drag time(ms)
    container_->Add(swipe);
    UIView* view1 = new UIView();
    view1->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
    view1->Resize(g_swipeW, g_swipeH);
    swipe->Add(view1);
    UIView* view2 = new UIView();
    view2->SetStyle(STYLE_BACKGROUND_COLOR, Color::White().full);
    view2->Resize(g_swipeW, g_swipeH);
    swipe->Add(view2);
    UIView* view3 = new UIView();
    view3->SetStyle(STYLE_BACKGROUND_COLOR, Color::Blue().full);
    view3->Resize(g_swipeW, g_swipeH);
    swipe->Add(view3);
    UIView* view4 = new UIView();
    view4->SetStyle(STYLE_BACKGROUND_COLOR, Color::Yellow().full);
    view4->Resize(g_swipeW, g_swipeH);
    swipe->Add(view4);
}

bool UITestUISwipeView::OnClick(UIView& view, const ClickEvent& event)
{
    if (currentSwipe_ == nullptr) {
        return true;
    }
    if (&view == addBtnInHead_) {
        UILabelButton* btn = new UILabelButton();
        btn->SetPosition(0, 0, g_buttonH, g_buttonW);
        btn->SetText(std::to_string(btnNum_).c_str());
        currentSwipe_->Insert(nullptr, btn);
    } else if (&view == addBtnInTail_) {
        UILabelButton* btn = new UILabelButton();
        btn->SetPosition(0, 0, g_buttonH, g_buttonW);
        btn->SetText(std::to_string(btnNum_).c_str());
        currentSwipe_->Add(btn);
    } else if (&view == addBtnInMid_) {
        UILabelButton* btn = new UILabelButton();
        btn->SetPosition(0, 0, g_buttonH, g_buttonW);
        btn->SetText(std::to_string(btnNum_).c_str());
        currentSwipe_->Insert(currentSwipe_->GetChildrenHead(), btn);
    } else if (&view == removeHeadBtn_) {
        currentSwipe_->Remove(currentSwipe_->GetChildrenHead());
    } else if (&view == removeMidBtn_) {
        UIView* view = currentSwipe_->GetViewByIndex(1);
        currentSwipe_->Remove(view);
    } else if (&view == removeAllBtn_) {
        currentSwipe_->RemoveAll();
    } else if (&view == loopBtn_) {
        loop_ = !loop_;
        currentSwipe_->SetLoopState(loop_);
        if (!loop_) {
            loopBtn_->SetText("设置循环 关 ");
        } else {
            loopBtn_->SetText("设置循环 开 ");
        }
    } else if (&view == changePageBtn_) {
        uint16_t currentIndex = currentSwipe_->GetCurrentPage();
        if (currentIndex < g_maxIndex) {
            currentSwipe_->SetCurrentPage(++currentIndex, true);
        } else {
            currentSwipe_->SetCurrentPage(0, true);
        }
    }
    currentSwipe_->Invalidate();
    btnNum_++;
    return true;
}

void UITestUISwipeView::SetUpButton(UILabelButton* btn, const char* title)
{
    if (btn == nullptr) {
        return;
    }
    container_->Add(btn);
    btn->SetPosition(positionX_, positionY_, BUTTON_WIDHT2, BUTTON_HEIGHT2);
    positionY_ += btn->GetHeight() + 10; // 10: increase y-coordinate
    btn->SetText(title);
    btn->SetFont(DEFAULT_VECTOR_FONT_FILENAME, BUTTON_LABEL_SIZE);
    btn->SetOnClickListener(this);
    btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::RELEASED);
    btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::PRESSED);
    btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::INACTIVE);
    btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::RELEASED);
    btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::PRESSED);
    btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::INACTIVE);
    container_->Invalidate();
}

void UITestUISwipeView::SetLastPos(UIView* view)
{
    if (view == nullptr) {
        return;
    }
    lastX_ = view->GetX();
    lastY_ = view->GetY() + view->GetHeight();
}
} // namespace OHOS

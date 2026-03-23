/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: Lottie
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */

#include "graphic_config.h"

#if ENABLE_DEBUG
#include "components/root_view.h"
#include "dfx/event_injector.h"
#include "engines/gfx/gfx_engine_manager.h"
#include "ui_test_event_injector.h"
#include <string>
#if ENABLE_WINDOW
#include "window/window.h"
#endif

#define POINT_YHX static_cast<int16_t>(scrollPoint.y + SCROLL_HEIGHT / RATIO_X)
#define POINT_YHY static_cast<int16_t>(scrollPoint.y + SCROLL_HEIGHT / RATIO_Y)
#define POINT_XWX static_cast<int16_t>(scrollPoint.x + SCROLL_WIDTH / RATIO_X)
#define POINT_XWY static_cast<int16_t>(scrollPoint.x + SCROLL_WIDTH / RATIO_Y)
namespace OHOS {
namespace {
const int16_t ITEM_H = 50;
const int16_t TEXT_H = 29;
const int16_t TEXT_W = 250;
const int16_t TEST_VIEW_H = 50;
const int16_t TEST_VIEW_W = 50;
const int16_t GAP = 5;
const int16_t TEST_VIEW_GAP = 80;
const int16_t TEST_BUTTON_W = 100;
const int16_t TEST_BUTTON_H = 30;
const int16_t LAYOUT_HEIGHT = 250;
const int16_t LAYOUT_WIDTH = 320;
const int16_t BLANK = 20;
const int16_t SCROLL_WIDTH = 250;
const int16_t SCROLL_HEIGHT = 250;
const int16_t SCROLL_BUTTON_W = 740;
const int16_t SCROLL_BUTTON_H = 360;
const int16_t DRAG_TIME_OFFSET = 20;
const int16_t RATIO_X = 3;
const int16_t RATIO_Y = 10;
const int16_t POINT_OFFSET = 4;
} // namespace

bool UITestEventInjectorView::OnLongPressEvent(const LongPressEvent& event)
{
    if (label_ != nullptr) {
        label_->SetText("long press!");
        label_->Invalidate();
    }
    return UIView::OnLongPressEvent(event);
}

bool UITestEventInjectorView::OnDragEvent(const DragEvent& event)
{
    if (label_ != nullptr) {
        label_->SetText("drag!");
        label_->Invalidate();
    }
    return UIView::OnDragEvent(event);
}

bool UITestEventInjectorView::OnClickEvent(const ClickEvent& event)
{
    if (label_ != nullptr) {
        label_->SetText(sentence_.c_str());
        label_->Invalidate();
    }
    return UIView::OnClickEvent(event);
}

bool UITestEventInjectorView::OnPressEvent(const PressEvent& event)
{
    if (label_ != nullptr) {
        label_->SetText("press!");
        label_->Invalidate();
    }
    return UIView::OnPressEvent(event);
}

bool UITestEventInjectorView::OnReleaseEvent(const ReleaseEvent& event)
{
    if (label_ != nullptr) {
        label_->SetText("release!");
        label_->Invalidate();
    }
    return UIView::OnReleaseEvent(event);
}

bool UITestEventInjectorView::OnCancelEvent(const CancelEvent& event)
{
    if (label_ != nullptr) {
        label_->SetText("cancel!");
        label_->Invalidate();
    }
    return UIView::OnCancelEvent(event);
}


bool UITestEventInjectorView::OnKeyAct(UIView& view, const KeyEvent& event)
{
    if (label_ == nullptr) {
        return false;
    }
    label_->SetText("key!");
    label_->Invalidate();
    return true;
}

void UITestEventInjector::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight() - BACK_BUTTON_HEIGHT);
        container_->SetHorizontalScrollState(false);
        container_->SetThrowDrag(true);
    }
    EventInjector::GetInstance()->RegisterEventInjector(EventDataType::POINT_TYPE);
    EventInjector::GetInstance()->RegisterEventInjector(EventDataType::KEY_TYPE);

    positionX_ = 20; // 20 : start x
    positionY_ = 20; // 20 : start y

#if ENABLE_WINDOW
    Window* window = RootView::GetInstance()->GetBoundWindow();
    if (window != nullptr) {
        EventInjector::GetInstance()->SetWindowId(window->GetWindowId());
    }
#endif
}

void UITestEventInjector::TearDown()
{
    DeleteChildren(container_);
    container_ = nullptr;
    layout_ = nullptr;

    if (EventInjector::GetInstance()->IsEventInjectorRegistered(EventDataType::POINT_TYPE)) {
        EventInjector::GetInstance()->UnregisterEventInjector(EventDataType::POINT_TYPE);
    }
    if (EventInjector::GetInstance()->IsEventInjectorRegistered(EventDataType::KEY_TYPE)) {
        EventInjector::GetInstance()->UnregisterEventInjector(EventDataType::KEY_TYPE);
    }
}

const UIView* UITestEventInjector::GetTestView()
{
    UIKitEventInjectorClickEvent001();
    UIKitEventInjectorDragEvent002();
    UIKitEventInjectorLongPressEvent003();
    UIKitEventInjectorKeyEvent004();

    SetUpScrollView();
    UIKitEventInjectorUptodown005();
    UIKitEventInjectorDowntoUp006();
    UIKitEventInjectorLefttoRight007();
    UIKitEventInjectorRighttoLeft008();
    UIKitEventInjectorULefttoLRight009();
    UIKitEventInjectorLRighttoULeft010();
    IncreaseDragTime();
    DecreaseDragTime();
    DragTimeDisplay();

    layout_->LayoutChildren();
    return container_;
}

void UITestEventInjector::UIKitEventInjectorClickEvent001()
{
    clickBtn_ = new UILabelButton();
    InnerTest("模拟点击事件 ", true, false, false, "click", clickBtn_, clickTestView_);
}

void UITestEventInjector::UIKitEventInjectorDragEvent002()
{
    dragBtn_ = new UILabelButton();
    InnerTest("模拟滑动事件 ", true, true, false, "drag", dragBtn_, dragTestView_);
}

void UITestEventInjector::UIKitEventInjectorLongPressEvent003()
{
    longPressBtn_ = new UILabelButton();
    InnerTest("模拟长按事件 ", true, true, true, "long press", longPressBtn_, longPressTestView_);
}

void UITestEventInjector::UIKitEventInjectorKeyEvent004()
{
    keyBtn_ = new UILabelButton();
    InnerTest("模拟按键输入 ", true, false, false, "key event", keyBtn_, KeyEventTestView_);
}

void UITestEventInjector::UIKitEventInjectorUptodown005()
{
    upToDownBtn_ = new UILabelButton();
    SetUpButton(upToDownBtn_, "up to down");
}

void UITestEventInjector::UIKitEventInjectorDowntoUp006()
{
    downToUpBtn_ = new UILabelButton();
    SetUpButton(downToUpBtn_, "down to up");
}

void UITestEventInjector::UIKitEventInjectorLefttoRight007()
{
    leftToRightBtn_ = new UILabelButton();
    SetUpButton(leftToRightBtn_, "left to right");
}

void UITestEventInjector::UIKitEventInjectorRighttoLeft008()
{
    rightToLeftBtn_ = new UILabelButton();
    SetUpButton(rightToLeftBtn_, "right to left");
}

void UITestEventInjector::UIKitEventInjectorULefttoLRight009()
{
    uLeftTolRightBtn_ = new UILabelButton();
    SetUpButton(uLeftTolRightBtn_, "uLeft to lRight");
}

void UITestEventInjector::UIKitEventInjectorLRighttoULeft010()
{
    lRightTouLeftBtn_ = new UILabelButton();
    SetUpButton(lRightTouLeftBtn_, "lRight to uleft");
}

void UITestEventInjector::IncreaseDragTime()
{
    increaseDragTimeBtn_ = new UILabelButton();
    SetUpButton(increaseDragTimeBtn_, "increase drag time");
}

void UITestEventInjector::DecreaseDragTime()
{
    decreaseDragTimeBtn_ = new UILabelButton();
    SetUpButton(decreaseDragTimeBtn_, "decrease drag time");
}

void UITestEventInjector::DragTimeDisplay()
{
    DragTimeDisplayBtn_ = new UILabelButton();
    std::string dragTimestr = "dragTime:" + std::to_string(dragTime_);
    SetUpButton(DragTimeDisplayBtn_, dragTimestr.c_str());
}

void UITestEventInjector::InnerTest(std::string title,
                                    bool touchable,
                                    bool draggable,
                                    bool dragParent,
                                    std::string btnTitle,
                                    UILabelButton* btn,
                                    UITestEventInjectorView*& testView)
{
    if ((container_ != nullptr) && (btn != nullptr)) {
        UILabel* label = new UILabel();
        container_->Add(label);
        label->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, positionY_, Screen::GetInstance().GetWidth(),
                           TITLE_LABEL_DEFAULT_HEIGHT);
        label->SetText(title.c_str());
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
        positionY_ += (TEXT_H + GAP);
        testView = new UITestEventInjectorView();
        testView->InitListener();
        container_->Add(testView);
        testView->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, positionY_, TEST_VIEW_W, TEST_VIEW_H);
        testView->SetStyle(STYLE_BACKGROUND_COLOR, Color::Blue().full);
        testView->SetTouchable(touchable);
        testView->SetDraggable(draggable);
        testView->SetDragParentInstead(dragParent);
        UILabel* label1 = new UILabel();
        container_->Add(label1);
        label1->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE + TEST_VIEW_GAP,
            positionY_ + 2 * GAP, TEXT_W, TEXT_H); // 2 : ratio
        label1->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
        testView->SetLabel(label1);

        container_->Add(btn);
        btn->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE + 3 * TEST_VIEW_GAP, positionY_ + GAP, // 3 : ratio
                         BUTTON_WIDHT2, BUTTON_HEIGHT2);
        btn->SetText(btnTitle.c_str());
        btn->SetFont(DEFAULT_VECTOR_FONT_FILENAME, BUTTON_LABEL_SIZE);
        btn->SetOnClickListener(this);
        btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::RELEASED);
        btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::PRESSED);
        btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::INACTIVE);
        btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::RELEASED);
        btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::PRESSED);
        btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::INACTIVE);

        positionY_ += ITEM_H;
    }
}

void UITestEventInjector::SetUpScrollView()
{
    if (container_ == nullptr) {
        return;
    }
    UILabel* label = GetTitleLabel("模拟Drag事件测试");
    container_->Add(label);
    label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, positionY_);
    SetLastPos(label);

    scrollTestView_ = new UIScrollView();
    scrollTestView_->SetIntercept(true);
    scrollTestView_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
    scrollTestView_->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, positionY_, SCROLL_WIDTH, SCROLL_HEIGHT);
    if (Screen::GetInstance().GetScreenShape() == ScreenShape::RECTANGLE) {
        scrollTestView_->SetXScrollBarVisible(true);
    }
    scrollTestView_->SetYScrollBarVisible(true);
    scrollTestView_->SetThrowDrag(true);
    container_->Add(scrollTestView_);
    UILabelButton* button1 = new UILabelButton();
    button1->SetText("button1");
    button1->SetPosition(0, 0, SCROLL_BUTTON_W, SCROLL_BUTTON_H);
    UILabelButton* button2 = new UILabelButton();
    button2->SetText("button2");
    button2->SetPosition(0, SCROLL_BUTTON_H, SCROLL_BUTTON_W, SCROLL_BUTTON_H);
    scrollTestView_->Add(button1);
    scrollTestView_->Add(button2);

    if (layout_ == nullptr) {
        layout_ = new GridLayout();
        // 40 : offset
        layout_->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE + 40, positionY_ + SCROLL_HEIGHT, LAYOUT_WIDTH, LAYOUT_HEIGHT);
        container_->Add(layout_);
        layout_->SetLayoutDirection(LAYOUT_VER);
        layout_->SetRows(5); // 5 : rows
        layout_->SetCols(2); // 2 : columns
    }
}

void UITestEventInjector::SetLastPos(UIView* view)
{
    if (view == nullptr) {
        return;
    }
    lastX_ = view->GetX();
    lastY_ = view->GetY() + view->GetHeight();
    positionY_ = lastY_ + 8 * GAP; /* 8:ratio */
}

void UITestEventInjector::SetUpButton(UILabelButton* btn, std::string title)
{
    if (btn == nullptr) {
        return;
    }
    layout_->Add(btn);
    btn->Resize(BUTTON_WIDHT3, BUTTON_HEIGHT3);
    btn->SetText(title.c_str());
    btn->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    btn->SetOnClickListener(this);
    btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::RELEASED);
    btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::PRESSED);
    btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::INACTIVE);
    btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::RELEASED);
    btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::PRESSED);
    btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::INACTIVE);
}

void UITestEventInjector::SetDragTimeDisplay(uint16_t dragTime)
{
    std::string dragTimestr = "dragTime:" + std::to_string(dragTime);
    DragTimeDisplayBtn_->SetText(dragTimestr.c_str());
    DragTimeDisplayBtn_->Invalidate();
}

void UITestEventInjector::ActionEvent(const UIView& view)
{
    Point point;
    Point scrollPoint;
    if (&view == clickBtn_) {
        point = {static_cast<int16_t>(clickTestView_->GetRect().GetX() + TEST_VIEW_W / POINT_OFFSET),
                 clickBtn_->GetRect().GetBottom()};
        EventInjector::GetInstance()->SetClickEvent(point);
    } else if (&view == dragBtn_) {
        point = {static_cast<int16_t>(dragTestView_->GetRect().GetX() + TEST_VIEW_W / POINT_OFFSET),
                 dragBtn_->GetRect().GetBottom()};
        Point endPoint = {static_cast<int16_t>(point.x + (RATIO_X * TEST_VIEW_W) / POINT_OFFSET), point.y};
        // 80:ms
        EventInjector::GetInstance()->SetDragEvent(point, endPoint, 80);
    } else if (&view == longPressBtn_) {
        point = {static_cast<int16_t>(longPressTestView_->GetRect().GetX() + TEST_VIEW_W / POINT_OFFSET),
                 longPressBtn_->GetRect().GetBottom()};
        EventInjector::GetInstance()->SetLongPressEvent(point);
    } else if (&view == keyBtn_) {
        // 26:key id
        uint16_t keyId = 26;
        EventInjector::GetInstance()->SetKeyEvent(keyId, InputDevice::STATE_PRESS);
    } else if (&view == upToDownBtn_) {
        scrollPoint = {scrollTestView_->GetRect().GetX(), scrollTestView_->GetRect().GetY()};
        Point startPoint = {POINT_XWX, POINT_YHY};
        Point endPoint = {POINT_XWX, POINT_YHX};
        EventInjector::GetInstance()->SetDragEvent(startPoint, endPoint, dragTime_);
    } else if (&view == downToUpBtn_) {
        scrollPoint = {scrollTestView_->GetRect().GetX(), scrollTestView_->GetRect().GetY()};
        Point startPoint = {POINT_XWX, POINT_YHX};
        Point endPoint = {POINT_XWX, POINT_YHY};
        EventInjector::GetInstance()->SetDragEvent(startPoint, endPoint, dragTime_);
    }
}

/* drag range:0 < x < 300, 62 < y < 362, click range:20 < x < 60, 392 < y < 432 */
bool UITestEventInjector::OnClick(UIView& view, const ClickEvent& event)
{
    Point point;
    Point scrollPoint;
    if (&view == leftToRightBtn_) {
        scrollPoint = {scrollTestView_->GetRect().GetX(), scrollTestView_->GetRect().GetY()};
        Point startPoint = {POINT_XWY, POINT_YHX};
        Point endPoint = {POINT_XWX,  POINT_YHX};
        EventInjector::GetInstance()->SetDragEvent(startPoint, endPoint, dragTime_);
    } else if (&view == rightToLeftBtn_) {
        scrollPoint = {scrollTestView_->GetRect().GetX(), scrollTestView_->GetRect().GetY()};
        Point startPoint = {POINT_XWX,  POINT_YHX};
        Point endPoint = {POINT_XWY, POINT_YHX};
        EventInjector::GetInstance()->SetDragEvent(startPoint, endPoint, dragTime_);
    } else if (&view == uLeftTolRightBtn_) {
        scrollPoint = {scrollTestView_->GetRect().GetX(), scrollTestView_->GetRect().GetY()};
        Point startPoint = {POINT_XWY, POINT_YHY};
        Point endPoint = {POINT_XWX, POINT_YHX};
        EventInjector::GetInstance()->SetDragEvent(startPoint, endPoint, dragTime_);
    } else if (&view == lRightTouLeftBtn_) {
        scrollPoint = {scrollTestView_->GetRect().GetX(), scrollTestView_->GetRect().GetY()};
        Point startPoint = {POINT_XWX, POINT_YHX};
        Point endPoint = {POINT_XWY, POINT_YHY};
        EventInjector::GetInstance()->SetDragEvent(startPoint, endPoint, dragTime_);
    } else if (&view == increaseDragTimeBtn_) {
        dragTime_ += DRAG_TIME_OFFSET;
        SetDragTimeDisplay(dragTime_);
    } else if (&view == decreaseDragTimeBtn_) {
        dragTime_ -= DRAG_TIME_OFFSET;
        SetDragTimeDisplay(dragTime_);
    }
    ActionEvent(view);
    return true;
}
} // namespace OHOS
#endif // ENABLE_DEBUG
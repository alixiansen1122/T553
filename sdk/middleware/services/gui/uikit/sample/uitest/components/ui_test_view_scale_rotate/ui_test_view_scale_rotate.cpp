/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: TestCase
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */

#include "ui_test_view_scale_rotate.h"
#include "common/screen.h"
#include "components/ui_label.h"
#include "core/render_manager.h"
#include "test_resource_config.h"

const uint16_t rotate_positionx_ = 150;
const uint16_t rotate_positiony_ = 50;
const uint16_t rotate_positionw_ = 300;
const uint16_t rotate_positionh_ = 200;
const uint16_t rotatex_ = 20;
const uint16_t rotatey_ = 10;
const uint16_t rotatew_ = 50;
const uint16_t rotateh_ = 150;
const uint16_t offsetX = 150;
const uint16_t offsetY = 150;
const uint16_t offsetb = 100;
namespace OHOS {
void UITestViewScaleRotate::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight() - BACK_BUTTON_HEIGHT);
    }

    if (list_ == nullptr) {
        list_ = new ListLayout(UISwipeView::VERTICAL);
        uint16_t width = Screen::GetInstance().GetWidth();
        uint16_t height = Screen::GetInstance().GetHeight();
        list_->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, 0, width, height + 1);
        container_->Add(list_);
    }
}

void UITestViewScaleRotate::TearDown()
{
    animator_.Stop();
    DeleteChildren(container_);
    container_ = nullptr;
    list_ = nullptr;
}

const UIView* UITestViewScaleRotate::GetTestView()
{
    UIKitViewScaleRotateTestLabel001();
    UIKitViewScaleRotateTestLabelButton002();
    UIKitViewScaleRotateTestCanvas003();
    UIKitViewScaleRotateTestUIBoxProgress004();
    UIKitViewScaleRotateTestUICircleProgress005();
    UIKitViewScaleRotateTestUIDigitalClock006();
    UIKitViewScaleRotateTestGroup007();

    animator_.Start();
    return container_;
}

void UITestViewScaleRotate::Callback(UIView* view)
{
    angleValue_++;

    if (scaleValue_.x_ < 0.2f) {
        scaleStep_ = 0.01f;
    } else if (scaleValue_.x_ > 1.5f) {
        scaleStep_ = -0.01f;
    }
    scaleValue_.x_ += scaleStep_;
    scaleValue_.y_ += scaleStep_;

    button1_->Rotate(angleValue_, VIEW_CENTER);
    button1_->Scale(scaleValue_, VIEW_CENTER);
    group1_->Rotate(angleValue_, GROUP_CENTER);
    group1_->Scale(scaleValue_, GROUP_CENTER);
    canvas_->Rotate(angleValue_, GROUP_CENTER);
    canvas_->Scale(scaleValue_, GROUP_CENTER);
    boxProgress_->Rotate(angleValue_, VIEW_CENTER);
    boxProgress_->Scale(scaleValue_, VIEW_CENTER);
    circleProgress_->Rotate(angleValue_, VIEW_CENTER);
    circleProgress_->Scale(scaleValue_, VIEW_CENTER);
    dClock_->Rotate(angleValue_, VIEW_CENTER);
    dClock_->Scale(scaleValue_, VIEW_CENTER);
    label_->Rotate(angleValue_, VIEW_CENTER);
    label_->Scale(scaleValue_, VIEW_CENTER);
}

void UITestViewScaleRotate::SetUpLabel(const char* title) const
{
    UILabel* label = new UILabel();
    label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE + offsetX, TEXT_DISTANCE_TO_TOP_SIDE,
        Screen::GetInstance().GetWidth(), TITLE_LABEL_DEFAULT_HEIGHT);
    label->SetText(title);
    label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    list_->Add(label);
}

void UITestViewScaleRotate::UIKitViewScaleRotateTestLabel001()
{
    UIViewGroup* group1 = new UIViewGroup();
    group1->SetPosition(rotate_positionx_, rotate_positiony_, offsetX, offsetY);
    list_->Add(group1);

    SetUpLabel("UILabel组件旋转 ");
    UIViewGroup* group = new UIViewGroup();
    group->SetPosition(rotate_positionx_, rotate_positiony_, groupWidht, groupHeight);
    label_ = new UILabel();
    label_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    label_->SetText("Test UILabel");
    label_->SetPosition(offsetb, offsetb, rotate_positionh_, rotatew_); // 100: x 100: y 200: width 50: height
    group->Add(label_);
    list_->Add(group);
}

void UITestViewScaleRotate::UIKitViewScaleRotateTestLabelButton002()
{
    SetUpLabel("UILabelButton组件旋转 ");
    UIViewGroup* group = new UIViewGroup();
    group->SetPosition(rotate_positionx_, rotate_positiony_, groupWidht, groupHeight);
    list_->Add(group);
    button1_ = new UILabelButton();
    button1_->SetPosition(offsetb, offsetb, rotate_positionh_, rotatew_); // 100: x 100: y 200: width 50: height
    button1_->SetText("Test UILabelButton");
    group->Add(button1_);
}

void UITestViewScaleRotate::UIKitViewScaleRotateTestCanvas003()
{
    SetUpLabel("UICanvas组件旋转 ");
    UIViewGroup* group = new UIViewGroup();
    group->SetPosition(rotate_positionx_, rotate_positiony_, groupWidht, groupHeight);
    list_->Add(group);
    canvas_ = new UICanvas();
    group->Add(canvas_);

    canvas_->SetPosition(0, 0, rotate_positionw_, rotate_positionh_);
    canvas_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Gray().full);

    Paint paint;
    paint.SetStrokeWidth(rotatex_); // 20: StrokeWidth
    canvas_->DrawLine({rotateh_, rotateh_}, {rotate_positionw_, rotate_positionw_}, paint);

    paint.SetStrokeColor(Color::Red());
    canvas_->DrawCurve({offsetb, rotatew_}, {rotateh_, rotate_positiony_},
        {rotateh_, rotate_positiony_}, {rotateh_, offsetb}, paint);
    canvas_->DrawRect({rotatey_, rotatey_}, rotatew_, rotatew_, paint); // 10: x 10: y 50: width 50: height
}

void UITestViewScaleRotate::UIKitViewScaleRotateTestUIBoxProgress004()
{
    SetUpLabel("UIBoxProgress组件旋转 ");
    UIViewGroup* group = new UIViewGroup();
    group->SetPosition(rotate_positionx_, rotate_positiony_, groupWidht, groupHeight);
    list_->Add(group);

    boxProgress_ = new UIBoxProgress();
    boxProgress_->EnableBackground(true);
    boxProgress_->SetPosition(offsetb, offsetb, rotate_positionh_, rotatew_); // 100: x 100:y 200:width 50: height
    boxProgress_->SetValue(80); // 80: value
    boxProgress_->SetBackgroundStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    boxProgress_->SetBackgroundStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
    group->Add(boxProgress_);
}

void UITestViewScaleRotate::UIKitViewScaleRotateTestUICircleProgress005()
{
    SetUpLabel("UICircleProgress组件旋转 ");
    UIViewGroup* group = new UIViewGroup();
    group->SetPosition(rotate_positionx_, rotate_positiony_, groupWidht, groupHeight);
    list_->Add(group);
    circleProgress_ = new UICircleProgress();
    circleProgress_->SetPosition(rotatex_, rotatey_, rotateh_, rotateh_); // 10: x 10: y 150: width 150: height
    circleProgress_->SetCenterPosition(75, 75);     // 75: position x 75: position y
    circleProgress_->SetRadius(rotatew_);                 // 50: radius
    circleProgress_->SetValue(rotatex_);                  // 20: value
    circleProgress_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    circleProgress_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Yellow().full);
    group->Add(circleProgress_);
}
void UITestViewScaleRotate::UIKitViewScaleRotateTestUIDigitalClock006()
{
    SetUpLabel("UIDigitalClock组件旋转 ");
    UIViewGroup* group = new UIViewGroup();
    group->SetPosition(rotate_positionx_, rotate_positiony_, groupWidht, groupHeight);
    list_->Add(group);
    dClock_ = new UIDigitalClock();
    dClock_->SetPosition(rotatex_, rotatey_, rotateh_, rotateh_);             // 10: x 10:y 150: width 150: height
    dClock_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 30); // 30: font size
    dClock_->SetTime24Hour(7, 25, 50);                  // 7: hour, 25: minute, 50: second
    group->Add(dClock_);
}

void UITestViewScaleRotate::UIKitViewScaleRotateTestGroup007()
{
    SetUpLabel("UIViewGroup组件旋转 ");
    UIViewGroup* group = new UIViewGroup();
    group->SetPosition(rotate_positionx_, rotate_positiony_, groupWidht, groupHeight);
    list_->Add(group);

    group1_ = new UIViewGroup();
    group1_->SetPosition(rotatex_, 0, rotate_positionw_, rotate_positionh_);
    group1_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    group1_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Yellow().full);
    group->Add(group1_);

    UISlider* slider = new UISlider();
    slider->SetPosition(rotatex_, rotatey_, rotate_positionh_, rotatew_); // 10: x 10: y 200: width 50: height
    slider->SetValue(rotatew_); // 50: value
    slider->SetDirection(UISlider::Direction::DIR_LEFT_TO_RIGHT);
    group1_->Add(slider);

    UILabelButton* button = new UILabelButton();
    button->SetPosition(rotatex_, offsetb, rotate_positionh_, rotatew_); // 10: x 100:y 200: width 50: height
    button->SetText("test ViewGroup");
    group1_->Add(button);
}
} // namespace OHOS
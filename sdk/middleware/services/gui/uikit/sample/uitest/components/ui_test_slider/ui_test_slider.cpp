/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "ui_test_slider.h"
#include "common/screen.h"
#include "dock/focus_manager.h"
namespace OHOS {
namespace {
const int16_t LABEL_HEIGHT = 29;
const int16_t BUTTON_WIDTH = 120;
const int16_t BUTTON_HEIGHT = 40;
const int16_t DELTA_X_COORDINATE = 8;
const int16_t DELTA_Y_COORDINATE = 12;
const int16_t DELTA_Y_COORDINATE_2 = 19;
static bool g_onChange = false;
static bool g_onRelease = false;
static bool g_onClick = false;
static bool g_onDrag = false;
static uint16_t g_progress = 20;
static uint16_t g_step = 0;
static uint16_t g_width = 50;
static uint16_t g_height = 250;
static int16_t g_min = 0;
static int16_t g_max = 100;
static int16_t g_knobWidth = 50;
} // namespace

void TestUISliderEventListener::OnChange(int32_t progress)
{
    value_ = progress;
    if (label_ == nullptr) {
        if (!MallocLabel()) {
            return;
        }
    }
    if (g_onChange) {
        label_->SetText("OnChange");
    }
}

void TestUISliderEventListener::OnRelease(int32_t progress)
{
    value_ = progress;
    if (label_ == nullptr) {
        if (!MallocLabel()) {
            return;
        }
    }
    if (g_onRelease) {
        label_->SetText("OnRelease");
    }
}

bool TestUISliderEventListener::OnClick(UIView& view, const ClickEvent& event)
{
    if (label_ == nullptr) {
        if (!MallocLabel()) {
            return false;
        }
    }
    if (g_onClick) {
        label_->SetText("OnClick");
    }
    return true;
}

bool TestUISliderEventListener::MallocLabel()
{
    label_ = new UILabel();
    if (label_ == nullptr) {
        return false;
    }
    viewGroup_->Add(label_);
    label_->SetPosition(0, 0, 100, 30); // 100: width; 30: height
    label_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    label_->SetStyle(STYLE_TEXT_COLOR, Color::Black().full);
    label_->SetStyle(STYLE_BACKGROUND_COLOR, Color::White().full);
    label_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    return true;
}

bool TestUISliderEventListener::OnDrag(UIView& view, const DragEvent& event)
{
    if (label_ == nullptr) {
        if (!MallocLabel()) {
            return false;
        }
    }

    if (g_onDrag) {
        label_->SetText("OnDrag");
    }
    return true;
}

void UITestSlider::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
        positionX_ = 0;
        positionY_ = 0;
        UIViewGroup* uiViewGroup = new UIViewGroup();
        uiViewGroup->SetPosition(positionX_, positionY_, 320, 390); // 320: width; 390: height
        container_->Add(uiViewGroup);
        UILabel* label = new UILabel();
        uiViewGroup->Add(label);
        label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, TEXT_DISTANCE_TO_TOP_SIDE, 100, 40); // 100: width; 40: height
        label->SetText("UISlider效果");
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
        uiViewGroupFrame_ = new UIViewGroup();
        uiViewGroup->Add(uiViewGroupFrame_);
        uiViewGroupFrame_->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, VIEW_DISTANCE_TO_TOP_SIDE,
            100, 300); // 100: width; 300: height
        uiViewGroupFrame_->SetStyle(STYLE_BORDER_COLOR, Color::White().full);
        uiViewGroupFrame_->SetStyle(STYLE_BORDER_OPA, HALF_OPA_OPAQUE);
        uiViewGroupFrame_->SetStyle(STYLE_BORDER_WIDTH, VIEW_STYLE_BORDER_WIDTH);
        uiViewGroupFrame_->SetStyle(STYLE_BORDER_RADIUS, VIEW_STYLE_BORDER_RADIUS);
        uiViewGroupFrame_->SetStyle(STYLE_BACKGROUND_OPA, 0);

        slider_ = new UISlider();
        FocusManager::GetInstance()->RequestFocus(slider_);

        slider_->SetPosition(10, 10, 40, 220); // 10: x; 10: y; 40: width; 220: height
        slider_->SetValidHeight(200);          // 200: valid height;
        slider_->SetValue(20);                 // 20: progress bar current value
        slider_->SetDirection(UISlider::Direction::DIR_BOTTOM_TO_TOP);
        listener_ = new TestUISliderEventListener(uiViewGroupFrame_);
        slider_->SetSliderEventListener(listener_);
        slider_->SetOnClickListener(listener_);
        slider_->SetOnDragListener(listener_);
        uiViewGroupFrame_->Add(slider_);
        slider_->LayoutCenterOfParent();
    }

    if (scroll_ == nullptr) {
        scroll_ = new UIScrollView();
        scroll_->SetIntercept(true);
        scroll_->SetPosition(160, 10); // 160: x-coordinate; 10: y-coordinate
        scroll_->Resize(280, Screen::GetInstance().GetHeight() - 70); // 280: width; 70: decrease height
        container_->Add(scroll_);
        positionY_ = 0;
    }
}

void UITestSlider::TearDown()
{
    FocusManager::GetInstance()->ClearFocus();
    DeleteChildren(container_);
    container_ = nullptr;
    slider_ = nullptr;
    scroll_ = nullptr;
    uiViewGroupFrame_ = nullptr;
    delete listener_;
    listener_ = nullptr;
}

const UIView* UITestSlider::GetTestView()
{
    UiKitSliderTestUiSlider001();
    UiKitSliderTestSetRange002();
    UiKitSliderTestSetValue003();
    UiKitSliderTestSetStyle005();
    UiKitSliderTestGetStyle006();
    UiKitSliderTestSetKnobWidth007();
    UiKitSliderTestSetStep008();
    UiKitSliderTestSetColor009();
    UiKitSliderTestSetDirection010();
    UiKitSliderTestSetValidSize011();
    UiKitSliderTestSetRadius012();
    UiKitSliderTestSetOnChangeListener013();
    UiKitSliderTestSetOnReleaseListener014();
    UiKitSliderTestSetOnClickCallback015();
    UiKitSliderTestSetOnDragCallback016();

    return container_;
}

void UITestSlider::SetUpLabel(const char* title, int16_t x, int16_t y) const
{
    UILabel* label = new UILabel();
    scroll_->Add(label);
    label->SetPosition(x, y, 288, LABEL_HEIGHT); // 288: width
    label->SetText(title);
    label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
}
void UITestSlider::SetUpButton(UILabelButton* btn, const char* title, int16_t x, int16_t y)
{
    if (btn == nullptr) {
        return;
    }
    scroll_->Add(btn);
    btn->SetPosition(x, y, BUTTON_WIDHT2, 50); // 50: button height
    btn->SetText(title);
    btn->SetFont(DEFAULT_VECTOR_FONT_FILENAME, BUTTON_LABEL_SIZE);
    btn->SetOnClickListener(this);
    btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::RELEASED);
    btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::PRESSED);
    btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::INACTIVE);
    btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::RELEASED);
    btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::PRESSED);
    btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::INACTIVE);
    scroll_->Invalidate();
}

void UITestSlider::UiKitSliderTestUiSlider001()
{
    resetBtn_ = new UILabelButton();
    positionX_ = 24; // 24: x-coordinate
    positionY_ = 0;
    SetUpLabel("reset slider:", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(resetBtn_, "reset", positionX_, positionY_);
}

void UITestSlider::UiKitSliderTestSetRange002()
{
    incMinProgressBtn_ = new UILabelButton();
    decMinProgressBtn_ = new UILabelButton();
    incMaxProgressBtn_ = new UILabelButton();
    decMaxProgressBtn_ = new UILabelButton();
    positionX_ = 24; // 24: x-coordinate
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE_2;
    SetUpLabel("set Slider range:", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(incMinProgressBtn_, "Min +", positionX_, positionY_);
    positionX_ += BUTTON_WIDTH + DELTA_Y_COORDINATE;
    SetUpButton(decMinProgressBtn_, "Min -", positionX_, positionY_);
    positionX_ = 24; // 24: x-coordinate
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE;
    SetUpButton(incMaxProgressBtn_, "MAX +", positionX_, positionY_);
    positionX_ += BUTTON_WIDTH + DELTA_Y_COORDINATE;
    SetUpButton(decMaxProgressBtn_, "MAX -", positionX_, positionY_);
}

void UITestSlider::UiKitSliderTestSetValue003()
{
    incProgressBtn_ = new UILabelButton();
    decProgressBtn_ = new UILabelButton();
    positionX_ = 24; // 24: x-coordinate
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE_2;
    SetUpLabel("set value：", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(incProgressBtn_, "val +", positionX_, positionY_);
    positionX_ += BUTTON_WIDTH + DELTA_Y_COORDINATE;
    SetUpButton(decProgressBtn_, "val -", positionX_, positionY_);
}

void UITestSlider::UiKitSliderTestSetStyle005()
{
    setStyleBtn_ = new UILabelButton();
    positionX_ = 24; // 24: x-coordinate
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE_2;
    SetUpLabel("set style1:", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(setStyleBtn_, "style1", positionX_, positionY_);
}

void UITestSlider::UiKitSliderTestGetStyle006()
{
    getStyleBtn_ = new UILabelButton();
    positionX_ = 24; // 24: x-coordinate
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE_2;
    SetUpLabel("set style2:", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(getStyleBtn_, "style2", positionX_, positionY_);
}

void UITestSlider::UiKitSliderTestSetKnobWidth007()
{
    incKnobWidthBtn_ = new UILabelButton();
    decKnobWidthBtn_ = new UILabelButton();
    positionX_ = 24; // 24: x-coordinate
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE_2;
    SetUpLabel("set knob width:", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(incKnobWidthBtn_, "width +", positionX_, positionY_);
    positionX_ += BUTTON_WIDTH + DELTA_Y_COORDINATE;
    SetUpButton(decKnobWidthBtn_, "width -", positionX_, positionY_);
}

void UITestSlider::UiKitSliderTestSetStep008()
{
    stepBtn_ = new UILabelButton();
    positionX_ = 24; // 24: x-coordinate
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE_2;
    SetUpLabel("set step length:", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(stepBtn_, "length +", positionX_, positionY_);
}

void UITestSlider::UiKitSliderTestSetColor009()
{
    colorBtn_ = new UILabelButton();
    positionX_ = 24; // 24: x-coordinate
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE_2;
    SetUpLabel("set color:", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(colorBtn_, "color", positionX_, positionY_);
}

void UITestSlider::UiKitSliderTestSetDirection010()
{
    dirL2RBtn_ = new UILabelButton();
    dirR2LBtn_ = new UILabelButton();
    dirT2BBtn_ = new UILabelButton();
    dirB2TBtn_ = new UILabelButton();
    positionX_ = 24; // 24: x-coordinate
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE_2;
    SetUpLabel("set direction:", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(dirL2RBtn_, "LTR", positionX_, positionY_);
    positionX_ += BUTTON_WIDTH + DELTA_Y_COORDINATE;
    SetUpButton(dirR2LBtn_, "RTL", positionX_, positionY_);
    positionX_ = 24; // 24: x-coordinate
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE;
    SetUpButton(dirT2BBtn_, "UTB", positionX_, positionY_);
    positionX_ += BUTTON_WIDTH + DELTA_Y_COORDINATE;
    SetUpButton(dirB2TBtn_, "BTU", positionX_, positionY_);
}

void UITestSlider::UiKitSliderTestSetValidSize011()
{
    widthBtn_ = new UILabelButton();
    heightBtn_ = new UILabelButton();
    positionX_ = 24; // 24: x-coordinate
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE_2;
    SetUpLabel("set box dimension:", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(heightBtn_, "box height", positionX_, positionY_);
    positionX_ += BUTTON_WIDTH + DELTA_Y_COORDINATE;
    SetUpButton(widthBtn_, "box width ", positionX_, positionY_);
}

void UITestSlider::UiKitSliderTestSetRadius012()
{
    radiusBtn_ = new UILabelButton();
    positionX_ = 24; // 24: x-coordinate
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE_2;
    SetUpLabel("set slider radius", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(radiusBtn_, "radius", positionX_, positionY_);
}

void UITestSlider::UiKitSliderTestSetOnChangeListener013()
{
    onChangeBtn_ = new UILabelButton();
    positionX_ = 24; // 24: x-coordinate
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE_2;
    SetUpLabel("set change listener： ", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(onChangeBtn_, "OnChange", positionX_, positionY_);
}

void UITestSlider::UiKitSliderTestSetOnReleaseListener014()
{
    onReleaseBtn_ = new UILabelButton();
    positionX_ = 24; // 24: x-coordinate
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE_2;
    SetUpLabel("set DragEnd listener:", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(onReleaseBtn_, "OnRelease", positionX_, positionY_);
}

void UITestSlider::UiKitSliderTestSetOnClickCallback015()
{
    onClickBtn_ = new UILabelButton();
    positionX_ = 24; // 24: x-coordinate
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE_2;
    SetUpLabel("set click listener:", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(onClickBtn_, "OnClick", positionX_, positionY_);
}

void UITestSlider::UiKitSliderTestSetOnDragCallback016()
{
    onDragBtn_ = new UILabelButton();
    positionX_ = 24; // 24: x-coordinate
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE_2;
    SetUpLabel("set Drag listener:", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(onDragBtn_, "OnDrag", positionX_, positionY_);
}

bool UITestSlider::OnClick(UIView& view, const ClickEvent& event)
{
    if (&view == resetBtn_) {
        slider_->LayoutCenterOfParent();
        g_progress = 20; // 20: progress
        g_step = 0;
        g_width = 50;     // 50: width
        g_height = 250;   // 250: height
        g_knobWidth = 50; // 50: knobWidth
        g_min = 0;
        g_max = 100; // 100: max
        slider_->SetDirection(UISlider::Direction::DIR_BOTTOM_TO_TOP);
        slider_->SetValidHeight(g_height);
        slider_->SetValidWidth(g_width);
        slider_->SetKnobWidth(50); // 50: knob width
        slider_->SetRange(g_max, g_min);
        slider_->SetValue(g_progress);
        slider_->SetStep(g_step);
        slider_->SetBackgroundStyle(StyleDefault::GetProgressBackgroundStyle());
        slider_->SetForegroundStyle(StyleDefault::GetProgressForegroundStyle());
        slider_->EnableBackground(true);
    } else if (&view == incProgressBtn_) {
        g_progress++;
        slider_->SetValue(g_progress);
    } else if (&view == decProgressBtn_) {
        g_progress--;
        slider_->SetValue(g_progress);
    } else if (&view == stepBtn_) {
        g_step++;
        slider_->SetStep(g_step);
    } else if (&view == widthBtn_) {
        g_width--;
        slider_->SetValidWidth(g_width);
    } else if (&view == heightBtn_) {
        g_height--;
        slider_->SetValidHeight(g_height);
    } else {
        ExpandClick1(view, event);
    }
    container_->Invalidate();
    return true;
}

bool UITestSlider::ExpandClick1(UIView& view, const ClickEvent& event)
{
    if (&view == incMinProgressBtn_) {
        g_min++;
        slider_->SetRange(slider_->GetRangeMax(), g_min);
    } else if (&view == decMinProgressBtn_) {
        g_min--;
        slider_->SetRange(slider_->GetRangeMax(), g_min);
    } else if (&view == incMaxProgressBtn_) {
        g_max++;
        slider_->SetRange(g_max, slider_->GetRangeMin());
    } else if (&view == decMaxProgressBtn_) {
        g_max--;
        slider_->SetRange(g_max, slider_->GetRangeMin());
    } else if (&view == dirL2RBtn_) {
        g_width = DEFAULT_HEIGHT;
        g_height = DEFAULT_WIDTH;
        slider_->SetValidHeight(g_height);
        slider_->SetValidWidth(g_width);
        slider_->Resize(g_width, g_height);
        slider_->SetDirection(UISlider::Direction::DIR_LEFT_TO_RIGHT);
        slider_->LayoutCenterOfParent();
    } else if (&view == dirR2LBtn_) {
        g_width = DEFAULT_HEIGHT;
        g_height = DEFAULT_WIDTH;
        slider_->SetValidHeight(g_height);
        slider_->SetValidWidth(g_width);
        slider_->Resize(g_width, g_height);
        slider_->SetDirection(UISlider::Direction::DIR_RIGHT_TO_LEFT);
        slider_->LayoutCenterOfParent();
    } else if (&view == dirT2BBtn_) {
        g_width = DEFAULT_WIDTH;
        g_height = DEFAULT_HEIGHT;
        slider_->SetValidHeight(g_height);
        slider_->SetValidWidth(g_width);
        slider_->Resize(g_width, g_height);
        slider_->SetDirection(UISlider::Direction::DIR_TOP_TO_BOTTOM);
        slider_->LayoutCenterOfParent();
    } else if (&view == dirB2TBtn_) {
        g_width = DEFAULT_WIDTH;
        g_height = DEFAULT_HEIGHT;
        slider_->SetValidHeight(g_height);
        slider_->SetValidWidth(g_width);
        slider_->Resize(g_width, g_height);
        slider_->SetDirection(UISlider::Direction::DIR_BOTTOM_TO_TOP);
        slider_->LayoutCenterOfParent();
    } else {
        ExpandClick2(view, event);
    }
    return true;
}

bool UITestSlider::ExpandClick2(UIView& view, const ClickEvent& event)
{
    if (&view == setStyleBtn_) {
        Style style = StyleDefault::GetDefaultStyle();
        style.bgColor_ = Color::Green();
        slider_->SetBackgroundStyle(style);
        style.bgColor_ = Color::Red();
        slider_->SetForegroundStyle(style);
        style.bgColor_ = Color::Gray();
        slider_->SetKnobStyle(style);
    } else if (&view == getStyleBtn_) {
        slider_->SetBackgroundStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
        slider_->SetForegroundStyle(STYLE_BACKGROUND_COLOR, Color::Yellow().full);
        slider_->SetKnobStyle(STYLE_BACKGROUND_COLOR, Color::Green().full);
    } else if (&view == incKnobWidthBtn_) {
        g_knobWidth++;
        slider_->SetKnobWidth(g_knobWidth);
    } else if (&view == decKnobWidthBtn_) {
        g_knobWidth--;
        slider_->SetKnobWidth(g_knobWidth);
    } else if (&view == colorBtn_) {
        slider_->SetSliderColor(Color::Silver(), Color::Blue());
        slider_->SetKnobColor(Color::White());
    } else if (&view == radiusBtn_) {
        slider_->SetSliderRadius(DEFAULT_RADIUS, DEFAULT_RADIUS);
        slider_->SetKnobRadius(DEFAULT_RADIUS);
    } else if (&view == onChangeBtn_) {
        g_onChange = true;
    } else if (&view == onReleaseBtn_) {
        g_onRelease = true;
    } else if (&view == onClickBtn_) {
        g_onClick = true;
    } else if (&view == onDragBtn_) {
        g_onDrag = true;
    }
    return true;
}
} // namespace OHOS

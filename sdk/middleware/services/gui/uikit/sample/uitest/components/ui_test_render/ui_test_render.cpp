/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: TestCase
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */

#include "ui_test_render.h"

#include "common/screen.h"

namespace OHOS {
    namespace {
    const uint16_t LABEL_HEIGHT = 60;
    const uint16_t DEFAULT_WIDITH = 200;
    const uint16_t DEFAULT_WIDITH1 = 550;
    const uint16_t DEFAULT_HEIGHT = 40;
    const uint16_t offsetX = 50;
    const uint16_t offsetY = 50;
    const uint16_t x_position1 = 20;
    const uint16_t x_position = 50;
    const uint16_t y_position = 80;
    const uint16_t w_position = 100;
    const uint16_t h_position = 50;
    const uint16_t fontSize = 15;
}
void UITestRender::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight() - BACK_BUTTON_HEIGHT);
    }
}

void UITestRender::TearDown()
{
    DeleteChildren(container_);
    container_ = nullptr;
}

const UIView* UITestRender::GetTestView()
{
    UIKitRenderTestRender001();
    UIKitRenderTestRenderMeasure001();
    return container_;
}

class TestRenderClickListener : public UIView::OnClickListener {
public:
    bool OnClick(UIView& view, const ClickEvent& event) override
    {
        static uint8_t r = 20;
        static uint8_t g = 120;
        static uint8_t b = 50;
        view.SetStyle(STYLE_BACKGROUND_COLOR, Color::GetColorFromRGB(r, g, b).full);
        view.Invalidate();

        r = r + 30; // 30: red add
        g = g + 30; // 30: green add
        b = b + 30; // 30: blue add
        return true;
    }
};

UIViewGroup* UITestRender::CreateTestCaseGroup() const
{
    UIViewGroup* group = new UIViewGroup();
    group->Resize(Screen::GetInstance().GetWidth(), DEFAULT_WIDITH); // 200: height
    return group;
}

UILabel* UITestRender::CreateTitleLabel() const
{
    UILabel* label = new UILabel();
    // 216: label width
    label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, TEXT_DISTANCE_TO_TOP_SIDE, 216, TITLE_LABEL_DEFAULT_HEIGHT);
    label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    return label;
}

void UITestRender::UIKitRenderTestRender001()
{
    if (container_ == nullptr) {
        return;
    }

    UILabel* label1 = CreateTitleLabel();
    container_->Add(label1);

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_Render_Test_Render_001");
    group->SetPosition(x_position1, h_position);
    
    UILabel* label = CreateTitleLabel();
    group->Add(label);
    label->SetText("UIKit绘制效果:");

    UIView* view = new UIView();
    view->Resize(DEFAULT_WIDITH, DEFAULT_HEIGHT);     // 200: width 50: height
    view->SetPosition(x_position, y_position); // 50: position x 50: position y
    view->SetStyle(STYLE_BACKGROUND_COLOR, Color::Yellow().full);
    group->Add(view);
    view->SetTouchable(true);
    view->SetOnClickListener(new TestRenderClickListener());

    UIView* view1 = new UIView();
    view1->Resize(w_position, h_position);      // 100: width 50: height
    view1->SetPosition(x_position, y_position + offsetY); // 50: position x 120: position y
    view1->SetStyle(STYLE_BACKGROUND_COLOR, Color::Green().full);
    view1->SetTouchable(true);
    view1->SetOnClickListener(new TestRenderClickListener());
    group->Add(view1);
    container_->Add(group);
}

void UITestRender::UIKitRenderTestRenderMeasure001()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_Render_Test_RenderMeasure_001");
    container_->Add(group);

    UILabel* label = CreateTitleLabel();
    group->Add(label);
    // 2: half of screen width
    label->Resize(Screen::GetInstance().GetWidth() / 2, TITLE_LABEL_DEFAULT_HEIGHT);
    label->SetText("UIKit绘制Measure效果:");

    testLabel_ = new UILabel();
    group->Add(testLabel_);
    testLabel_->SetPosition(x_position, offsetY);
    testLabel_->Resize(w_position + offsetX, w_position); // 150: width 80: height
    testLabel_->SetViewId("label_text");
    testLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    testLabel_->SetLineBreakMode(UILabel::LINE_BREAK_WRAP);
    testLabel_->SetText("示例文字,示例文字");

    labelButton_ = new UILabelButton();
    group->Add(labelButton_);
    labelButton_->Resize(BUTTON_WIDHT3, BUTTON_HEIGHT3);
    labelButton_->SetText("更新label");
    labelButton_->SetViewId(UI_TEST_RENDER_UPDATA_BUTTON_ID_01);
    labelButton_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    labelButton_->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::RELEASED);
    labelButton_->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::PRESSED);
    labelButton_->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::RELEASED);
    labelButton_->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_PRESS, UIButton::PRESSED);
    labelButton_->SetPosition(DEFAULT_WIDITH + offsetX, DEFAULT_WIDITH1);
    labelButton_->SetOnClickListener(this);
    labelButton_->LayoutBottomToSibling("label_text"); // 10: offset
    group->LayoutBottomToSibling("UIKit_Render_Test_Render_001", 10); // 10: offset
}

bool UITestRender::OnClick(UIView& view, const ClickEvent& event)
{
    if (&view == labelButton_) {
        testLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, fontSize); // 18: font size
    }
    return true;
}
} // namespace OHOS
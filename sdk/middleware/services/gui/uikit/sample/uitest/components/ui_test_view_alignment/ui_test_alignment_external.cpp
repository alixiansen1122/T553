/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: TestCase
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */

#include "ui_test_alignment_external.h"

#include "common/screen.h"
#include "components/ui_view.h"

static int16_t g_deltaY1 = 150;
static int16_t g_deltaY2 = 25;
static int16_t g_buttonx = 255;
static int16_t g_buttony = 255;
static int16_t g_buttonW = 216;
static int16_t g_buttonH = 200;
namespace OHOS {

void UITestExternalAlignment::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight() - BACK_BUTTON_HEIGHT);
    }
    label_ = new UILabel();
    label1_ = new UILabel();
    label2_ = new UILabel();
    label3_ = new UILabel();
    label4_ = new UILabel();
    label5_ = new UILabel();
    label6_ = new UILabel();
    label7_ = new UILabel();
    label8_ = new UILabel();
    label9_ = new UILabel();
    label10_ = new UILabel();
    label11_ = new UILabel();
    label12_ = new UILabel();
}

void UITestExternalAlignment::TearDown()
{
    DeleteChildren(container_);
    container_ = nullptr;
    label_ = nullptr;
    label1_ = nullptr;
    label2_ = nullptr;
    label3_ = nullptr;
    label4_ = nullptr;
    label5_ = nullptr;
    label6_ = nullptr;
    label7_ = nullptr;
    label8_ = nullptr;
    label9_ = nullptr;
    label10_ = nullptr;
    label11_ = nullptr;
    label12_ = nullptr;
}

const UIView* UITestExternalAlignment::GetTestView()
{
    UIKitAlignExternalTest001();
    return container_;
}

void UITestExternalAlignment::UIKitAlignExternalTop()
{
    label1_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    label1_->SetText("OUT_TOP_LEFT:");
    label1_->SetTouchable(true);
    label1_->Resize(g_buttonW, g_buttonH);
    label1_->SetStyle(STYLE_TEXT_COLOR, Color::Yellow().full);
    label1_->SetStyle(STYLE_BORDER_OPA, Color::Gray().full);
    label1_->AlignToReference(ALIGN_OUT_TOP_LEFT, label_, 0, g_deltaY1);
    container_->Add(label1_);

    label2_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    label2_->SetText("OUT_TOP_MID:");
    label2_->SetTouchable(true);
    label2_->Resize(g_buttonW, g_buttonH);
    label2_->AlignToReference(ALIGN_OUT_TOP_MID, label_, 0, g_deltaY1);
    label2_->SetStyle(STYLE_TEXT_COLOR, Color::Green().full);
    container_->Add(label2_);
    
    label3_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    label3_->SetText("OUT_TOP_RIGHT:");
    label3_->SetTouchable(true);
    label3_->Resize(g_buttonW, g_buttonH);
    label3_->AlignToReference(ALIGN_OUT_TOP_RIGHT, label_, 0, g_deltaY1);
    label3_->SetStyle(STYLE_TEXT_COLOR, Color::Blue().full);
    container_->Add(label3_);
}
void UITestExternalAlignment::UIKitAlignExternalBottom()
{
    label4_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    label4_->SetText("OUT_BOTTOM_LEFT:");
    label4_->SetTouchable(true);
    label4_->Resize(g_buttonW, g_buttonH);
    label4_->SetStyle(STYLE_TEXT_COLOR, Color::Yellow().full);
    label4_->AlignToReference(ALIGN_OUT_BOTTOM_LEFT, label_, 0, -g_deltaY1);
    container_->Add(label4_);

    label5_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    label5_->SetText("OUT_BOTTOM_MID:");
    label5_->SetTouchable(true);
    label5_->Resize(g_buttonW, g_buttonH);
    label5_->AlignToReference(ALIGN_OUT_BOTTOM_MID, label_, 0, -g_deltaY1);
    label5_->SetStyle(STYLE_TEXT_COLOR, Color::Green().full);
    container_->Add(label5_);
    
    label6_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    label6_->SetText("OUT_BOTTOM_RIGHT:");
    label6_->SetTouchable(true);
    label6_->Resize(g_buttonW, g_buttonH);
    label6_->AlignToReference(ALIGN_OUT_BOTTOM_RIGHT, label_, 0, -g_deltaY1);
    label6_->SetStyle(STYLE_TEXT_COLOR, Color::Blue().full);
    container_->Add(label6_);
}

void UITestExternalAlignment::UIKitAlignExternalLeft()
{
    label7_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    label7_->SetText("OUT_LEFT_TOP:");
    label7_->SetTouchable(true);
    label7_->Resize(g_buttonW, g_buttonH);
    label7_->SetStyle(STYLE_TEXT_COLOR, Color::Yellow().full);
    label7_->AlignToReference(ALIGN_OUT_LEFT_TOP, label_, 0, -g_deltaY2);
    container_->Add(label7_);

    label8_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    label8_->SetText("OUT_LEFT_MID:");
    label8_->SetTouchable(true);
    label8_->Resize(g_buttonW, g_buttonH);
    label8_->AlignToReference(ALIGN_OUT_LEFT_MID, label_, 0, 0);
    label8_->SetStyle(STYLE_TEXT_COLOR, Color::Green().full);
    container_->Add(label8_);
    
    label9_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    label9_->SetText("OUT_LEFT_BOTTOM:");
    label9_->SetTouchable(true);
    label9_->Resize(g_buttonW, g_buttonH);
    label9_->AlignToReference(ALIGN_OUT_LEFT_BOTTOM, label_, 0, g_deltaY2);
    label9_->SetStyle(STYLE_TEXT_COLOR, Color::Blue().full);
    container_->Add(label9_);
}

void UITestExternalAlignment::UIKitAlignExternalRight()
{
    label10_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    label10_->SetText("OUT_RIGHT_TOP:");
    label10_->SetTouchable(true);
    label10_->Resize(g_buttonW, g_buttonH);
    label10_->SetStyle(STYLE_TEXT_COLOR, Color::Yellow().full);
    label10_->AlignToReference(ALIGN_OUT_RIGHT_TOP, label_, 0, -g_deltaY2);
    container_->Add(label10_);

    label11_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    label11_->SetText("OUT_RIGHT_MID:");
    label11_->SetTouchable(true);
    label11_->Resize(g_buttonW, g_buttonH);
    label11_->AlignToReference(ALIGN_OUT_RIGHT_MID, label_, 0, 0);
    label11_->SetStyle(STYLE_TEXT_COLOR, Color::Green().full);
    container_->Add(label11_);
    
    label12_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    label12_->SetText("OUT_RIGHT_BOTTOM:");
    label12_->SetTouchable(true);
    label12_->Resize(g_buttonW, g_buttonH);
    label12_->AlignToReference(ALIGN_OUT_RIGHT_BOTTOM, label_, 0, g_deltaY2);
    label12_->SetStyle(STYLE_TEXT_COLOR, Color::Blue().full);
    container_->Add(label12_);
}

void UITestExternalAlignment::UIKitAlignExternalTest001()
{
    if (container_ == nullptr) {
        return;
    }
    
    label_->SetPosition(g_buttonx, g_buttony, g_buttonW, g_buttonH);
    label_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    label_->SetText("参考控件:");
    label_->SetStyle(STYLE_TEXT_COLOR, Color::Red().full);
    label_->SetStyle(STYLE_BORDER_OPA, Color::Gray().full);
    label_->SetTouchable(true);
    container_->Add(label_);

    UIKitAlignExternalTop();
    UIKitAlignExternalBottom();
    UIKitAlignExternalLeft();
    UIKitAlignExternalRight();
}
} // namespace OHOS
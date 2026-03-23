/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: TestCase
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */

#include "ui_test_alignment_internal.h"

#include "common/screen.h"
#include "components/ui_view.h"


static int16_t g_deltaY = 0;
static int16_t g_buttonW = 150;
static int16_t g_buttonH = 170;
static int16_t g_rx = 40;
static int16_t g_ry = 40;
static int16_t g_rw = 375;
static int16_t g_rh = 325;
static int16_t xofs = 10;
static int16_t yofs = 10;

namespace OHOS {

void UITestInternalAlignment::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight() - BACK_BUTTON_HEIGHT);
    }
    viewGroup_ = new UIViewGroup();
    label1_ = new UILabel();
    label2_ = new UILabel();
    label3_ = new UILabel();
    label4_ = new UILabel();
    label5_ = new UILabel();
    label6_ = new UILabel();
    label7_ = new UILabel();
    label8_ = new UILabel();
    label9_ = new UILabel();
}

void UITestInternalAlignment::TearDown()
{
    DeleteChildren(container_);
    container_ = nullptr;
    viewGroup_ = nullptr;
    label1_ = nullptr;
    label2_ = nullptr;
    label3_ = nullptr;
    label4_ = nullptr;
    label5_ = nullptr;
    label6_ = nullptr;
    label7_ = nullptr;
    label8_ = nullptr;
    label9_ = nullptr;
}

const UIView* UITestInternalAlignment::GetTestView()
{
    UIKitAlignInternalTest001();
    return container_;
}

void UITestInternalAlignment::UIKitAlignInternalTop()
{
    label1_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    label1_->SetText("TOP_LEFT:");
    label1_->SetTouchable(true);
    label1_->Resize(g_buttonW, g_buttonH);
    label1_->SetStyle(STYLE_TEXT_COLOR, Color::Yellow().full);
    label1_->AlignToReference(ALIGN_TOP_LEFT, viewGroup_, xofs, yofs);

    label2_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    label2_->SetText("TOP_MID:");
    label2_->SetTouchable(true);
    label2_->Resize(g_buttonW, g_buttonH);
    label2_->AlignToReference(ALIGN_TOP_MID, viewGroup_, xofs, yofs);
    label2_->SetStyle(STYLE_TEXT_COLOR, Color::Green().full);
    
    label3_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    label3_->SetText("TOP_RIGHT:");
    label3_->SetTouchable(true);
    label3_->Resize(g_buttonW, g_buttonH);
    label3_->AlignToReference(ALIGN_TOP_RIGHT, viewGroup_, xofs, yofs);
    label3_->SetStyle(STYLE_TEXT_COLOR, Color::Blue().full);
}

void UITestInternalAlignment::UIKitAlignInternalBottom()
{
    label7_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    label7_->SetText("BOTTOM_LEFT:");
    label7_->SetTouchable(true);
    label7_->Resize(g_buttonW, g_buttonH);
    label7_->SetStyle(STYLE_TEXT_COLOR, Color::Yellow().full);
    label7_->AlignToReference(ALIGN_BOTTOM_LEFT, viewGroup_, xofs, yofs);

    label8_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    label8_->SetText("BOTTOM_MID:");
    label8_->SetTouchable(true);
    label8_->Resize(g_buttonW, g_buttonH);
    label8_->AlignToReference(ALIGN_BOTTOM_MID, viewGroup_, xofs, yofs);
    label8_->SetStyle(STYLE_TEXT_COLOR, Color::Green().full);
    
    label9_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    label9_->SetText("BOTTOM_RIGHT:");
    label9_->SetTouchable(true);
    label9_->Resize(g_buttonW, g_buttonH);
    label9_->AlignToReference(ALIGN_BOTTOM_RIGHT, viewGroup_, xofs, yofs);
    label9_->SetStyle(STYLE_TEXT_COLOR, Color::Blue().full);
}
void UITestInternalAlignment::UIKitAlignInternalMid()
{
    label4_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    label4_->SetText("LEFT_MID:");
    label4_->SetTouchable(true);
    label4_->Resize(g_buttonW, g_buttonH);
    label4_->SetStyle(STYLE_TEXT_COLOR, Color::Yellow().full);
    label4_->AlignToReference(ALIGN_MID_LEFT, viewGroup_, xofs, yofs);

    label5_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    label5_->SetText("CENTER_IND:");
    label5_->SetTouchable(true);
    label5_->Resize(g_buttonW, g_buttonH);
    label5_->AlignToReference(ALIGN_MID_MID, viewGroup_, xofs, yofs);
    label5_->SetStyle(STYLE_TEXT_COLOR, Color::Green().full);
    
    label6_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    label6_->SetText("RIGHT_MID:");
    label6_->SetTouchable(true);
    label6_->Resize(g_buttonW, g_buttonH);
    label6_->AlignToReference(ALIGN_MID_RIGHT, viewGroup_, xofs, yofs);
    label6_->SetStyle(STYLE_TEXT_COLOR, Color::Blue().full);
}

void UITestInternalAlignment::UIKitAlignInternalTest001()
{
    if (container_ == nullptr) {
        return;
    }

    container_->Add(viewGroup_);
    viewGroup_->Add(label1_);
    viewGroup_->Add(label2_);
    viewGroup_->Add(label3_);
    viewGroup_->Add(label4_);
    viewGroup_->Add(label5_);
    viewGroup_->Add(label6_);
    viewGroup_->Add(label7_);
    viewGroup_->Add(label8_);
    viewGroup_->Add(label9_);
    
    viewGroup_->SetPosition(g_rx, g_ry, g_rw, g_rh);
    viewGroup_->SetStyle(STYLE_TEXT_COLOR, Color::Red().full);
    viewGroup_->SetStyle(STYLE_BORDER_COLOR, Color::White().full);
    viewGroup_->SetStyle(STYLE_BORDER_OPA, HALF_OPA_OPAQUE);
    viewGroup_->SetStyle(STYLE_BORDER_WIDTH, VIEW_STYLE_BORDER_WIDTH);
    viewGroup_->SetStyle(STYLE_BORDER_RADIUS, VIEW_STYLE_BORDER_RADIUS);
    viewGroup_->SetStyle(STYLE_BACKGROUND_OPA, 0);
    UIKitAlignInternalTop();
    UIKitAlignInternalBottom();
    UIKitAlignInternalMid();
}
} // namespace OHOS
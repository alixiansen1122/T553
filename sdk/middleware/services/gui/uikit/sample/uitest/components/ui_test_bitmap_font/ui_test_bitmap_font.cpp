/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: TestCase
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */

#include "ui_test_bitmap_font.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "common/ui_text_language.h"
namespace OHOS {
const static float START_X = 0.2;
const static float START_Y = 0.1;
const static float WIDTH = 0.2;
const static float HEIGHT = 0.2;
const static float GAP_X = 0.1;
const static float GAP_Y = 0.1;
static float g_xcoord = START_X;
static float g_ycoord = START_Y;
void UITestBitmapFont::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->SetThrowDrag(true);
        container_->SetHorizontalScrollState(false);
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
        positionX_ = 0;
        positionY_ = 0;
    }
}

void UITestBitmapFont::TearDown()
{
    DeleteChildren(container_);
    container_ = nullptr;
    g_xcoord = START_X;
    g_ycoord = START_Y;
}

const UIView* UITestBitmapFont::GetTestView()
{
    TestBitmapDynamicFont();
    TestBitmapStaticFont();
    return container_;
}

void UITestBitmapFont::TestBitmapDynamicFont()
{
    UILabel* label = new UILabel();
    container_->Add(label);
    label->SetPositionPercent(g_xcoord, g_ycoord, WIDTH, HEIGHT);
    label->SetText("Label Dynamic");
    label->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    label->SetStyle(STYLE_TEXT_COLOR, Color::Black().full);
    label->SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
    label->SetStyle(STYLE_BACKGROUND_COLOR, Color::White().full);
    label->SetStyle(STYLE_BACKGROUND_OPA, OPA_150);
    label->SetFontId(0);

    g_xcoord += WIDTH + GAP_X;

    UILabelButton* button = new UILabelButton();
    container_->Add(button);
    button->SetPositionPercent(g_xcoord, g_ycoord, WIDTH, HEIGHT);
    button->SetText("Button Dynamic");
    button->SetAlign(TEXT_ALIGNMENT_CENTER);
    button->SetStyle(STYLE_BACKGROUND_COLOR, Color::Orange().full);
    button->SetStyle(STYLE_BACKGROUND_OPA, OPA_150);
    button->SetFontId(0);

    g_xcoord = START_X;
    g_ycoord += HEIGHT + GAP_Y;
}

void UITestBitmapFont::TestBitmapStaticFont()
{
    UILabel* label = new UILabel();
    container_->Add(label);
    label->SetPositionPercent(g_xcoord, g_ycoord, WIDTH, HEIGHT);
    label->SetText(T_HEARTRATE_CHINA1);
    label->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    label->SetStyle(STYLE_TEXT_COLOR, Color::Black().full);
    label->SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
    label->SetStyle(STYLE_BACKGROUND_COLOR, Color::White().full);
    label->SetStyle(STYLE_BACKGROUND_OPA, OPA_150);
    label->SetFontId(0);

    g_xcoord += WIDTH + GAP_X;

    UILabelButton* button = new UILabelButton();
    container_->Add(button);
    button->SetPositionPercent(g_xcoord, g_ycoord, WIDTH, HEIGHT);
    button->SetText(T_HEARTRATE_CHINA1);
    button->SetAlign(TEXT_ALIGNMENT_CENTER);
    button->SetStyle(STYLE_BACKGROUND_COLOR, Color::Orange().full);
    button->SetStyle(STYLE_BACKGROUND_OPA, OPA_150);
    button->SetFontId(0);

    g_xcoord = START_X;
    g_ycoord += GAP_Y;
}
}
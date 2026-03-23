/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: Png Test
 * Author: Hisi Graphic Team
 * Created: 2025-9
 */

#include "ui_test_png.h"
#if ENABLE_JPEG_AND_PNG
namespace OHOS {
const static int16_t START_X = 100;
const static int16_t START_Y = 0;
const static int16_t GAP = 10;
const static int16_t IMG_WIDTH = 62;
const static int16_t IMG_HEIGHT = 62;
const static int16_t HEIGHT_100 = 100;
const static int16_t WIDTH_50 = 50;
const static int16_t HEIGHT_50 = 50;
static int16_t g_xCoord = START_X;
static int16_t g_yCoord = START_Y;

void UITestPNG::SetUp()
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

void UITestPNG::TearDown()
{
    DeleteChildren(container_);
    container_ = nullptr;
    g_xCoord = START_X;
    g_yCoord = START_Y;
}

const UIView* UITestPNG::GetTestView()
{
    SetBackgroundColor();
    TestPNG();
    TestFillPNG();
    TestContainPNG();
    return container_;
}

void UITestPNG::SetBackgroundColor()
{
    container_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Orange().full);
    container_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
}

void UITestPNG::CreateLabel(const char* text)
{
    UILabel* label = new UILabel();
    label->SetText(text);
    label->SetPosition(g_xCoord, g_yCoord, TEST_LABEL_WIDTH, TEST_LABEL_HEIGHT);
    container_->Add(label);
    g_yCoord += TEST_LABEL_HEIGHT + GAP;
}

void UITestPNG::TestPNG()
{
    CreateLabel("PNG full image");

    UIImageView* img = new UIImageView();
    img->SetAutoEnable(true);
    img->SetPosition(g_xCoord, g_yCoord, WIDTH_50, HEIGHT_50);
    img->SetSrc(RES_PATH"haha.png");
    img->SetStyle(STYLE_IMAGE_OPA, OPA_OPAQUE);
    container_->Add(img);
    g_yCoord += IMG_HEIGHT + GAP;
}

void UITestPNG::TestFillPNG()
{
    CreateLabel("PNG FILL 50x100");

    UIImageView* img = new UIImageView();
    img->SetAutoEnable(false);
    img->SetSrc(RES_PATH"haha.png");
    img->SetPosition(g_xCoord, g_yCoord, WIDTH_50, HEIGHT_100);
    img->SetResizeMode(UIImageView::FILL);
    img->SetStyle(STYLE_IMAGE_OPA, OPA_OPAQUE);
    container_->Add(img);
    g_yCoord += HEIGHT_100 + GAP;
}

void UITestPNG::TestContainPNG()
{
    CreateLabel("PNG CONTAIN 50x100");

    UIImageView* img = new UIImageView();
    img->SetAutoEnable(false);
    img->SetSrc(RES_PATH"haha.png");
    img->SetPosition(g_xCoord, g_yCoord, WIDTH_50, HEIGHT_100);
    img->SetResizeMode(UIImageView::CONTAIN);
    img->SetStyle(STYLE_IMAGE_OPA, OPA_OPAQUE);
    container_->Add(img);
    g_yCoord += HEIGHT_100 + GAP;
}
}
#endif // ENABLE_JPEG_AND_PNG
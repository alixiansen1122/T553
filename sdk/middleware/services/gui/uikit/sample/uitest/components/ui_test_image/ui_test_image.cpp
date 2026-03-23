/*
 * Copyright (c) 2022 CompanyNameMagicTag.
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

#include "ui_test_image.h"
namespace OHOS {
const static int16_t START_X = 100;
const static int16_t START_Y = 0;
const static int16_t GAP = 10;
const static int16_t IMG_WIDTH = 220;
const static int16_t IMG_HEIGHT = 220;
const static int16_t WIDTH_110 = 110;
const static int16_t HEIGHT_110 = 110;
const static int16_t WIDTH_50 = 50;
const static int16_t HEIGHT_50 = 50;
const static int16_t WIDTH_300 = 300;
const static int16_t HEIGHT_250 = 250;
const static int16_t WIDTH_150 = 150;
const static int16_t HEIGHT_100 = 100;
static int16_t g_xCoord = START_X;
static int16_t g_yCoord = START_Y;

void UITestImage::SetUp()
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

void UITestImage::TearDown()
{
    DeleteChildren(container_);
    container_ = nullptr;
    g_xCoord = START_X;
    g_yCoord = START_Y;
}

const UIView* UITestImage::GetTestView()
{
    SetBackgroundColor();
    TestUncmpImgWithTrans();
    TestUncmpImgWithCrop();
    TestUncmpImgWithAlpha();
    TestUncmpImgWithScale();
    TestUncmpImgWithComplex();
    TestAbypassImgWithTrans();
    TestAbypassImgWithCrop();
    TestAbypassImgWithAlpha();
    TestAbypassImgWithScale();
    TestAbypassImgWithComplex();
    TestHFBCImg8888();
    TestUncmpImg565();
    TestBypassImg565();
    TestUncmpImg888();
    TestBypassImg888();
    return container_;
}

void UITestImage::SetBackgroundColor()
{
    container_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Orange().full);
    container_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
}

void UITestImage::TestUncmpImgWithTrans()
{
    UILabel* label = new UILabel();
    label->SetText("ARGB8888 translate");
    label->SetPosition(g_xCoord, g_yCoord, TEST_LABEL_WIDTH, TEST_LABEL_HEIGHT);
    container_->Add(label);
    g_yCoord += TEST_LABEL_HEIGHT + GAP;

    UIImageView* img = new UIImageView();
    img->SetSrc(RES_PATH"A094_051_8888.bin");
    img->SetPosition(g_xCoord, g_yCoord, IMG_WIDTH, IMG_HEIGHT);
    img->SetStyle(STYLE_IMAGE_OPA, OPA_OPAQUE);
    container_->Add(img);
    g_yCoord += IMG_HEIGHT + GAP;
}

void UITestImage::TestUncmpImgWithCrop()
{
    UILabel* label = new UILabel();
    label->SetText("ARGB8888 crop");
    label->SetPosition(g_xCoord, g_yCoord, TEST_LABEL_WIDTH, TEST_LABEL_HEIGHT);
    container_->Add(label);
    g_yCoord += TEST_LABEL_HEIGHT + GAP;

    UIImageView* img = new UIImageView();
    img->SetAutoEnable(false);
    img->SetSrc(RES_PATH"A094_051_8888.bin");
    img->SetPosition(g_xCoord, g_yCoord, WIDTH_110, HEIGHT_110);
    img->SetStyle(STYLE_IMAGE_OPA, OPA_OPAQUE);
    container_->Add(img);
    g_yCoord += HEIGHT_110 + GAP;
}

void UITestImage::TestUncmpImgWithAlpha()
{
    UILabel* label = new UILabel();
    label->SetText("ARGB8888 OPA 50");
    label->SetPosition(g_xCoord, g_yCoord, TEST_LABEL_WIDTH, TEST_LABEL_HEIGHT);
    container_->Add(label);
    g_yCoord += TEST_LABEL_HEIGHT + GAP;

    UIImageView* img = new UIImageView();
    img->SetSrc(RES_PATH"A094_051_8888.bin");
    img->SetPosition(g_xCoord, g_yCoord, IMG_WIDTH, IMG_HEIGHT);
    img->SetStyle(STYLE_IMAGE_OPA, OPA_50);
    container_->Add(img);
    g_yCoord += IMG_HEIGHT + GAP;
}

void UITestImage::TestUncmpImgWithScale()
{
    UILabel* label = new UILabel();
    label->SetText("ARGB8888 FILL");
    label->SetPosition(g_xCoord, g_yCoord, TEST_LABEL_WIDTH, TEST_LABEL_HEIGHT);
    container_->Add(label);
    g_yCoord += TEST_LABEL_HEIGHT + GAP;

    UIImageView* img = new UIImageView();
    img->SetAutoEnable(false);
    img->SetPosition(g_xCoord, g_yCoord, WIDTH_50, HEIGHT_50);
    img->SetStyle(STYLE_IMAGE_OPA, OPA_OPAQUE);
    img->SetSrc(RES_PATH"A094_051_8888.bin");
    img->SetResizeMode(UIImageView::FILL);
    container_->Add(img);
    g_yCoord += HEIGHT_50 + GAP;

    UILabel* label1 = new UILabel();
    label1->SetText("ARGB8888 CONTAIN");
    label1->SetPosition(g_xCoord, g_yCoord, TEST_LABEL_WIDTH, TEST_LABEL_HEIGHT);
    container_->Add(label1);
    g_yCoord += TEST_LABEL_HEIGHT + GAP;

    UIImageView* img1 = new UIImageView();
    img1->SetAutoEnable(false);
    img1->SetPosition(g_xCoord, g_yCoord, WIDTH_300, HEIGHT_250);
    img1->SetStyle(STYLE_IMAGE_OPA, OPA_OPAQUE);
    img1->SetSrc(RES_PATH"A094_051_8888.bin");
    img1->SetResizeMode(UIImageView::CONTAIN);
    container_->Add(img1);
    g_yCoord += HEIGHT_250 + GAP;
}

void UITestImage::TestUncmpImgWithComplex()
{
    UILabel* label = new UILabel();
    label->SetText("ARGB8888 COMPLEX");
    label->SetPosition(g_xCoord, g_yCoord, TEST_LABEL_WIDTH, TEST_LABEL_HEIGHT);
    container_->Add(label);
    g_yCoord += TEST_LABEL_HEIGHT + GAP;

    UIImageView* img = new UIImageView();
    img->SetAutoEnable(false);
    img->SetPosition(g_xCoord, g_yCoord, WIDTH_110, HEIGHT_110);
    img->SetStyle(STYLE_IMAGE_OPA, OPA_50);
    img->SetSrc(RES_PATH"A094_051_8888.bin");
    img->SetResizeMode(UIImageView::FILL);
    container_->Add(img);

    UIView* view = new UIView();
    view->SetPosition(g_xCoord, g_yCoord, WIDTH_110, 55); // 55: width
    view->SetStyle(STYLE_BACKGROUND_COLOR, Color::White().full);
    view->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    container_->Add(view);
    g_yCoord += HEIGHT_110 + GAP;
}

void UITestImage::TestAbypassImgWithTrans()
{
    UILabel* label = new UILabel();
    label->SetText("HFBC_BYPASS ARGB8888 translate");
    label->SetPosition(g_xCoord, g_yCoord, TEST_LABEL_WIDTH, TEST_LABEL_HEIGHT);
    container_->Add(label);
    g_yCoord += TEST_LABEL_HEIGHT + GAP;

    UIImageView* img = new UIImageView();
    img->SetSrc(RES_PATH"A094_051_8888_abypass.bin");
    img->SetPosition(g_xCoord, g_yCoord, IMG_WIDTH, IMG_HEIGHT);
    img->SetStyle(STYLE_IMAGE_OPA, OPA_OPAQUE);
    container_->Add(img);
    g_yCoord += IMG_HEIGHT + GAP;
}

void UITestImage::TestAbypassImgWithCrop()
{
    UILabel* label = new UILabel();
    label->SetText("HFBC_BYPASS ARGB8888 crop");
    label->SetPosition(g_xCoord, g_yCoord, TEST_LABEL_WIDTH, TEST_LABEL_HEIGHT);
    container_->Add(label);
    g_yCoord += TEST_LABEL_HEIGHT + GAP;

    UIImageView* img = new UIImageView();
    img->SetAutoEnable(false);
    img->SetSrc(RES_PATH"A094_051_8888_abypass.bin");
    img->SetPosition(g_xCoord, g_yCoord, WIDTH_110, HEIGHT_110);
    img->SetStyle(STYLE_IMAGE_OPA, OPA_OPAQUE);
    container_->Add(img);
    g_yCoord += HEIGHT_110 + GAP;
}

void UITestImage::TestAbypassImgWithAlpha()
{
    UILabel* label = new UILabel();
    label->SetText("HFBC_BYPASS ARGB8888 OPA 50");
    label->SetPosition(g_xCoord, g_yCoord, TEST_LABEL_WIDTH, TEST_LABEL_HEIGHT);
    container_->Add(label);
    g_yCoord += TEST_LABEL_HEIGHT + GAP;

    UIImageView* img = new UIImageView();
    img->SetSrc(RES_PATH"A094_051_8888_abypass.bin");
    img->SetPosition(g_xCoord, g_yCoord, IMG_WIDTH, IMG_HEIGHT);
    img->SetStyle(STYLE_IMAGE_OPA, OPA_50);
    container_->Add(img);
    g_yCoord += IMG_HEIGHT + GAP;
}

void UITestImage::TestAbypassImgWithScale()
{
    UILabel* label = new UILabel();
    label->SetText("HFBC_BYPASS ARGB8888 FILL");
    label->SetPosition(g_xCoord, g_yCoord, TEST_LABEL_WIDTH, TEST_LABEL_HEIGHT);
    container_->Add(label);
    g_yCoord += TEST_LABEL_HEIGHT + GAP;

    UILabel* label1 = new UILabel();
    label1->SetText("HFBC_BYPASS ARGB8888 CONTAIN");
    label1->SetPosition(g_xCoord, g_yCoord, TEST_LABEL_WIDTH, TEST_LABEL_HEIGHT);
    container_->Add(label1);
    g_yCoord += TEST_LABEL_HEIGHT + GAP;

    UIImageView* img1 = new UIImageView();
    img1->SetAutoEnable(false);
    img1->SetPosition(g_xCoord, g_yCoord, WIDTH_300, HEIGHT_250);
    img1->SetStyle(STYLE_IMAGE_OPA, OPA_OPAQUE);
    img1->SetSrc(RES_PATH"A094_051_8888_abypass.bin");
    img1->SetResizeMode(UIImageView::CONTAIN);
    container_->Add(img1);
    g_yCoord += HEIGHT_250 + GAP;
}

void UITestImage::TestAbypassImgWithComplex()
{
    UILabel* label = new UILabel();
    label->SetText("HFBC_BYPASS ARGB8888 COMPLEX");
    label->SetPosition(g_xCoord, g_yCoord, TEST_LABEL_WIDTH, TEST_LABEL_HEIGHT);
    container_->Add(label);
    g_yCoord += TEST_LABEL_HEIGHT + GAP;

    UIImageView* img = new UIImageView();
    img->SetAutoEnable(false);
    img->SetPosition(g_xCoord, g_yCoord, WIDTH_110, HEIGHT_110);
    img->SetStyle(STYLE_IMAGE_OPA, OPA_50);
    img->SetSrc(RES_PATH"A094_051_8888_abypass.bin");
    img->SetResizeMode(UIImageView::FILL);
    container_->Add(img);

    UIView* view = new UIView();
    view->SetPosition(g_xCoord, g_yCoord, WIDTH_110, 55); // 55: height
    view->SetStyle(STYLE_BACKGROUND_COLOR, Color::White().full);
    view->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    container_->Add(view);
    g_yCoord += HEIGHT_110 + GAP;
}

void UITestImage::TestHFBCImg8888()
{
    UILabel* label = new UILabel();
    label->SetText("HFBC ARGB8888 translate");
    label->SetPosition(g_xCoord, g_yCoord, TEST_LABEL_WIDTH, TEST_LABEL_HEIGHT);
    container_->Add(label);
    g_yCoord += TEST_LABEL_HEIGHT + GAP;

    UIImageView* img = new UIImageView();
    img->SetSrc(RES_PATH"A094_051_8888_hfbc.bin");
    img->SetPosition(g_xCoord, g_yCoord, IMG_WIDTH, IMG_HEIGHT);
    img->SetStyle(STYLE_IMAGE_OPA, OPA_OPAQUE);
    container_->Add(img);
    g_yCoord += IMG_HEIGHT + GAP;

    UILabel* label1 = new UILabel();
    label1->SetText("HFBC ARGB8888 CONTAIN OPA 150");
    label1->SetPosition(g_xCoord, g_yCoord, TEST_LABEL_WIDTH, TEST_LABEL_HEIGHT);
    container_->Add(label1);
    g_yCoord += TEST_LABEL_HEIGHT + GAP;

    UIImageView* img1 = new UIImageView();
    img1->SetAutoEnable(false);
    img1->SetSrc(RES_PATH"A094_051_8888_hfbc.bin");
    img1->SetPosition(g_xCoord, g_yCoord, WIDTH_150, HEIGHT_100);
    img1->SetResizeMode(UIImageView::CONTAIN);
    img1->SetStyle(STYLE_IMAGE_OPA, OPA_150);
    container_->Add(img1);
    g_yCoord += HEIGHT_100 + GAP;
}

void UITestImage::TestUncmpImg565()
{
    UILabel* label = new UILabel();
    label->SetText("RGB565 translate");
    label->SetPosition(g_xCoord, g_yCoord, TEST_LABEL_WIDTH, TEST_LABEL_HEIGHT);
    container_->Add(label);
    g_yCoord += TEST_LABEL_HEIGHT + GAP;

    UIImageView* img = new UIImageView();
    img->SetSrc(RES_PATH"A094_051_565.bin");
    img->SetPosition(g_xCoord, g_yCoord, IMG_WIDTH, IMG_HEIGHT);
    img->SetStyle(STYLE_IMAGE_OPA, OPA_OPAQUE);
    container_->Add(img);
    g_yCoord += IMG_HEIGHT + GAP;

    UILabel* label1 = new UILabel();
    label1->SetText("RGB565 CONTAIN OPA 150");
    label1->SetPosition(g_xCoord, g_yCoord, TEST_LABEL_WIDTH, TEST_LABEL_HEIGHT);
    container_->Add(label1);
    g_yCoord += TEST_LABEL_HEIGHT + GAP;

    UIImageView* img1 = new UIImageView();
    img1->SetAutoEnable(false);
    img1->SetSrc(RES_PATH"A094_051_565.bin");
    img1->SetPosition(g_xCoord, g_yCoord, WIDTH_150, HEIGHT_100);
    img1->SetResizeMode(UIImageView::CONTAIN);
    img1->SetStyle(STYLE_IMAGE_OPA, OPA_150);
    container_->Add(img1);
    g_yCoord += HEIGHT_100 + GAP;
}

void UITestImage::TestBypassImg565()
{
    UILabel* label = new UILabel();
    label->SetText("HFBC_BYPASS RGB565 translate");
    label->SetPosition(g_xCoord, g_yCoord, TEST_LABEL_WIDTH, TEST_LABEL_HEIGHT);
    container_->Add(label);
    g_yCoord += TEST_LABEL_HEIGHT + GAP;

    UIImageView* img = new UIImageView();
    img->SetSrc(RES_PATH"A094_051_565_abypass.bin");
    img->SetPosition(g_xCoord, g_yCoord, IMG_WIDTH, IMG_HEIGHT);
    img->SetStyle(STYLE_IMAGE_OPA, OPA_OPAQUE);
    container_->Add(img);
    g_yCoord += IMG_HEIGHT + GAP;

    UILabel* label1 = new UILabel();
    label1->SetText("HFBC_BYPASS RGB565 CONTAIN OPA 150");
    label1->SetPosition(g_xCoord, g_yCoord, TEST_LABEL_WIDTH, TEST_LABEL_HEIGHT);
    container_->Add(label1);
    g_yCoord += TEST_LABEL_HEIGHT + GAP;

    UIImageView* img1 = new UIImageView();
    img1->SetAutoEnable(false);
    img1->SetSrc(RES_PATH"A094_051_565_abypass.bin");
    img1->SetPosition(g_xCoord, g_yCoord, WIDTH_150, HEIGHT_100);
    img1->SetResizeMode(UIImageView::CONTAIN);
    img1->SetStyle(STYLE_IMAGE_OPA, OPA_150);
    container_->Add(img1);
    g_yCoord += HEIGHT_100 + GAP;
}

void UITestImage::TestUncmpImg888()
{
    UILabel* label = new UILabel();
    label->SetText("RGB888 translate");
    label->SetPosition(g_xCoord, g_yCoord, TEST_LABEL_WIDTH, TEST_LABEL_HEIGHT);
    container_->Add(label);
    g_yCoord += TEST_LABEL_HEIGHT + GAP;

    UIImageView* img = new UIImageView();
    img->SetSrc(RES_PATH"A094_051_888.bin");
    img->SetPosition(g_xCoord, g_yCoord, IMG_WIDTH, IMG_HEIGHT);
    img->SetStyle(STYLE_IMAGE_OPA, OPA_OPAQUE);
    container_->Add(img);
    g_yCoord += IMG_HEIGHT + GAP;

    UILabel* label1 = new UILabel();
    label1->SetText("RGB888 CONTAIN OPA 150");
    label1->SetPosition(g_xCoord, g_yCoord, TEST_LABEL_WIDTH, TEST_LABEL_HEIGHT);
    container_->Add(label1);
    g_yCoord += TEST_LABEL_HEIGHT + GAP;

    UIImageView* img1 = new UIImageView();
    img1->SetAutoEnable(false);
    img1->SetSrc(RES_PATH"A094_051_888.bin");
    img1->SetPosition(g_xCoord, g_yCoord, WIDTH_150, HEIGHT_100);
    img1->SetResizeMode(UIImageView::CONTAIN);
    img1->SetStyle(STYLE_IMAGE_OPA, OPA_150);
    container_->Add(img1);
    g_yCoord += HEIGHT_100 + GAP;
}

void UITestImage::TestBypassImg888()
{
    UILabel* label = new UILabel();
    label->SetText("HFBC_BYPASS RGB888 translate");
    label->SetPosition(g_xCoord, g_yCoord, TEST_LABEL_WIDTH, TEST_LABEL_HEIGHT);
    container_->Add(label);
    g_yCoord += TEST_LABEL_HEIGHT + GAP;

    UIImageView* img = new UIImageView();
    img->SetSrc(RES_PATH"A094_051_888_abypass.bin");
    img->SetPosition(g_xCoord, g_yCoord, IMG_WIDTH, IMG_HEIGHT);
    img->SetStyle(STYLE_IMAGE_OPA, OPA_OPAQUE);
    container_->Add(img);
    g_yCoord += IMG_HEIGHT + GAP;

    UILabel* label1 = new UILabel();
    label1->SetText("HFBC_BYPASS RGB888 CONTAIN OPA 150");
    label1->SetPosition(g_xCoord, g_yCoord, TEST_LABEL_WIDTH, TEST_LABEL_HEIGHT);
    container_->Add(label1);
    g_yCoord += TEST_LABEL_HEIGHT + GAP;

    UIImageView* img1 = new UIImageView();
    img1->SetAutoEnable(false);
    img1->SetSrc(RES_PATH"A094_051_888_abypass.bin");
    img1->SetPosition(g_xCoord, g_yCoord, WIDTH_150, HEIGHT_100);
    img1->SetResizeMode(UIImageView::CONTAIN);
    img1->SetStyle(STYLE_IMAGE_OPA, OPA_150);
    container_->Add(img1);
    g_yCoord += HEIGHT_100 + GAP;
}
}
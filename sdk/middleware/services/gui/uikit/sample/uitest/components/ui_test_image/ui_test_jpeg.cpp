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

#include "ui_test_jpeg.h"
#if CAPABILITY_HW_JPEG_DECODE
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

void UITestJPEG::SetUp()
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

void UITestJPEG::TearDown()
{
    DeleteChildren(container_);
    container_ = nullptr;
    g_xCoord = START_X;
    g_yCoord = START_Y;
}

const UIView* UITestJPEG::GetTestView()
{
    SetBackgroundColor();
    TestJPEG();
    TestFillJPEG();
    TestContainJPEG();
    return container_;
}

void UITestJPEG::SetBackgroundColor()
{
    container_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Orange().full);
    container_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
}

void UITestJPEG::CreateLabel(const char* text)
{
    UILabel* label = new UILabel();
    label->SetText(text);
    label->SetPosition(g_xCoord, g_yCoord, TEST_LABEL_WIDTH, TEST_LABEL_HEIGHT);
    container_->Add(label);
    g_yCoord += TEST_LABEL_HEIGHT + GAP;
}

void UITestJPEG::TestJPEG()
{
    CreateLabel("JPEG full image");

    UIImageView* img = new UIImageView();
    img->SetAutoEnable(true);
    img->SetPosition(g_xCoord, g_yCoord, WIDTH_50, HEIGHT_50);
    img->SetSrc(RES_PATH"lele.jpg");
    img->SetStyle(STYLE_IMAGE_OPA, OPA_OPAQUE);
    container_->Add(img);
    g_yCoord += IMG_HEIGHT + GAP;
}

void UITestJPEG::TestFillJPEG()
{
    CreateLabel("JPEG FILL 50x100");

    UIImageView* img = new UIImageView();
    img->SetAutoEnable(false);
    img->SetSrc(RES_PATH"lele.jpg");
    img->SetPosition(g_xCoord, g_yCoord, WIDTH_50, HEIGHT_100);
    img->SetResizeMode(UIImageView::FILL);
    img->SetStyle(STYLE_IMAGE_OPA, OPA_OPAQUE);
    container_->Add(img);
    g_yCoord += HEIGHT_100 + GAP;
}

void UITestJPEG::TestContainJPEG()
{
    CreateLabel("JPEG CONTAIN 50x100");

    UIImageView* img = new UIImageView();
    img->SetAutoEnable(false);
    img->SetSrc(RES_PATH"lele.jpg");
    img->SetPosition(g_xCoord, g_yCoord, WIDTH_50, HEIGHT_100);
    img->SetResizeMode(UIImageView::CONTAIN);
    img->SetStyle(STYLE_IMAGE_OPA, OPA_OPAQUE);
    container_->Add(img);
    g_yCoord += HEIGHT_100 + GAP;
}
}
#endif
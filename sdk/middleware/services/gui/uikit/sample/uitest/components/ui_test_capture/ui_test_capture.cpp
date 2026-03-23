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

#include "ui_test_capture.h"
#include "common/screen.h"
#include "components/root_view.h"
#include "components/ui_image_view.h"
#include "securec.h"

namespace OHOS {
const static int16_t COOR_50 = 50;
const static int16_t COOR_120 = 120;
const static int16_t COOR_100 = 100;
const static int16_t COOR_190 = 190;
const static int16_t COOR_250 = 250;
const static int16_t WIDTH_100 = 100;
const static int16_t HEIGHT_50 = 50;

class ViewBitmapListener : public UIView::OnClickListener {
public:
    ViewBitmapListener(UIScrollView* scroll, UIImageView* img) : scroll_(scroll), img_(img) {}
    ~ViewBitmapListener() override
    {
        if (info_.data != nullptr) {
            ImageCacheFree(info_);
        }
    }

    bool OnClick(UIView& view, const ClickEvent& event) override
    {
        if (isClicked_) {
            GRAPHIC_LOGE("only capture once.");
            return false;
        }
        isClicked_ = true;
        if (!view.GetBitmap(info_)) {
            return false;
        }
        img_->SetSrc(&info_);
        scroll_->Add(img_);
        img_->Invalidate();
        return true;
    }
private:
    bool isClicked_ = false;
    UIScrollView* scroll_;
    UIImageView* img_;
    ImageInfo info_ = {{0}};
};

class ScreenBitmapListener : public UIView::OnClickListener {
public:
    ScreenBitmapListener(UIScrollView* scroll, UIImageView* img) : scroll_(scroll), img_(img) {}
    ~ScreenBitmapListener() override
    {
        if (info_.data != nullptr) {
            ImageCacheFree(info_);
        }
    }

    bool OnClick(UIView& view, const ClickEvent& event) override
    {
        if (isClicked_) {
            GRAPHIC_LOGE("only capture once.");
            return false;
        }
        isClicked_ = true;
        if (!Screen::GetInstance().GetScreenBitmapByRect(info_, nullptr)) {
            return false;
        }
        img_->SetSrc(&info_);
        scroll_->Add(img_);
        img_->Invalidate();
        return true;
    }
private:
    bool isClicked_ = false;
    UIScrollView* scroll_;
    UIImageView* img_;
    ImageInfo info_ = {{0}};
};

class RectBitmapListener : public UIView::OnClickListener {
public:
    RectBitmapListener(UIScrollView* scroll, UIImageView* img, const Rect& rect) : scroll_(scroll),
        img_(img), rect_(rect) {}
    ~RectBitmapListener() override
    {
        if (info_.data != nullptr) {
            ImageCacheFree(info_);
        }
    }

    bool OnClick(UIView& view, const ClickEvent& event) override
    {
        if (isClicked_) {
            GRAPHIC_LOGE("only capture once.");
            return false;
        }
        isClicked_ = true;
        if (!Screen::GetInstance().GetScreenBitmapByRect(info_, &rect_)) {
            return false;
        }
        img_->SetSrc(&info_);
        scroll_->Add(img_);
        GRAPHIC_LOGI("Capture Rect Info: x=%d, y=%d, w=%d, h=%d\n", rect_.GetX(), rect_.GetY(),
            rect_.GetWidth(), rect_.GetHeight());
        img_->Invalidate();
        return true;
    }
private:
    bool isClicked_ = false;
    UIScrollView* scroll_;
    UIImageView* img_;
    ImageInfo info_ = {{0}};
    Rect rect_;
};

void UITestCapture::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->SetThrowDrag(true);
        container_->SetHorizontalScrollState(false);
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
        container_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Yellow().full);
        container_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    }
}

void UITestCapture::TearDown()
{
    DeleteChildren(container_);
    viewBitmapBtn_ = nullptr;
    screenBitmapBtn_ = nullptr;
    rectBitmapBtn_ = nullptr;
    if (viewBitmapListener_ != nullptr) {
        delete viewBitmapListener_;
        viewBitmapListener_ = nullptr;
    }
    if (screenBitmapListener_ != nullptr) {
        delete screenBitmapListener_;
        screenBitmapListener_ = nullptr;
    }
    if (rectBitmapListener_ != nullptr) {
        delete rectBitmapListener_;
        rectBitmapListener_ = nullptr;
    }
    container_ = nullptr;
}

void UITestCapture::TestViewBitmap()
{
    if (container_ == nullptr) {
        return;
    }
    viewBitmapBtn_ = new UILabelButton();
    viewBitmapBtn_->SetPosition(COOR_100, COOR_50, WIDTH_100, HEIGHT_50);
    viewBitmapBtn_->SetText("viewCap");
    viewBitmapBtn_->SetViewId("viewCap");

    UIImageView* cap = new UIImageView();
    cap->SetAutoEnable(false);
    cap->SetPosition(COOR_250, COOR_50, WIDTH_100, HEIGHT_50);
    viewBitmapListener_ = new ViewBitmapListener(container_, cap);
    viewBitmapBtn_->SetOnClickListener(viewBitmapListener_);
    container_->Add(viewBitmapBtn_);
}

void UITestCapture::TestRectBitmap()
{
    if (container_ == nullptr) {
        return;
    }
    rectBitmapBtn_ = new UILabelButton();
    rectBitmapBtn_->SetPosition(COOR_100, COOR_120, WIDTH_100, HEIGHT_50);
    rectBitmapBtn_->SetText("rectCap");
    rectBitmapBtn_->SetViewId("rectCap");

    UIImageView* cap = new UIImageView();
    cap->SetAutoEnable(false);
    cap->SetPosition(COOR_250, COOR_120, WIDTH_100, HEIGHT_50);
    Rect rect = {COOR_100, COOR_120, COOR_100 + WIDTH_100 - 1, COOR_120 + HEIGHT_50 - 1};

    rectBitmapListener_ = new RectBitmapListener(container_, cap, rect);
    rectBitmapBtn_->SetOnClickListener(rectBitmapListener_);
    container_->Add(rectBitmapBtn_);
}

void UITestCapture::TestScreenBitmap()
{
    if (container_ == nullptr) {
        return;
    }
    screenBitmapBtn_ = new UILabelButton();
    screenBitmapBtn_->SetPosition(COOR_100, COOR_190, WIDTH_100, HEIGHT_50);
    screenBitmapBtn_->SetText("screenCap");
    screenBitmapBtn_->SetViewId("screenCap");

    UIImageView* cap = new UIImageView();
    cap->SetAutoEnable(false);
    uint16_t width = Screen::GetInstance().GetWidth();
    uint16_t height = Screen::GetInstance().GetHeight();
    cap->SetPosition(0, COOR_250, width, height);

    screenBitmapListener_ = new ScreenBitmapListener(container_, cap);
    screenBitmapBtn_->SetOnClickListener(screenBitmapListener_);
    container_->Add(screenBitmapBtn_);
}

const UIView* UITestCapture::GetTestView()
{
    TestViewBitmap();
    TestRectBitmap();
    TestScreenBitmap();
    return container_;
}
}
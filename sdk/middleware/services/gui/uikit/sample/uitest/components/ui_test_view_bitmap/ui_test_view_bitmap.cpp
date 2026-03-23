/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: TestCase
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */

#include "ui_test_view_bitmap.h"

#include "securec.h"

#include "common/screen.h"
#include "components/ui_label_button.h"
#include "test_resource_config.h"

const uint16_t bitmap_positionX_ = 100;
const uint16_t bitmap_positionY_ = 100;
const uint16_t offsetY = 100;
namespace OHOS {
class ViewBitmapListener : public UIView::OnClickListener {
public:
    ViewBitmapListener(UIViewGroup* container, UIImageView* img) : container_(container), img_(img)
    {
        memset_s(&info_, sizeof(ImageInfo), 0, sizeof(ImageInfo));
        if ((img != nullptr) && (container != nullptr)) {
            container->Add(img);
            img->SetVisible(false);
        }
    }
    virtual ~ViewBitmapListener()
    {
        if (info_.data != nullptr) {
            ImageCacheFree(info_);
        }
    }

    bool OnClick(UIView& view, const ClickEvent& event) override
    {
        if (img_ == nullptr) {
            return false;
        }
        if (info_.data != nullptr) {
            ImageCacheFree(info_);
        }
        view.GetBitmap(info_);
        img_->SetVisible(true);
        img_->SetSrc(&info_);
        img_->Invalidate();
        return false;
    }

    UIViewGroup* container_;
    UIImageView* img_;
    ImageInfo info_;
};

class ScreenBitmapListener : public UIView::OnClickListener {
public:
    ScreenBitmapListener(UIViewGroup* container, UIImageView* img) : container_(container), img_(img)
    {
        memset_s(&info_, sizeof(ImageInfo), 0, sizeof(ImageInfo));
        if ((img != nullptr) && (container != nullptr)) {
            container->Add(img);
            img->SetVisible(false);
        }
    }
    virtual ~ScreenBitmapListener()
    {
        if (info_.data != nullptr) {
            ImageCacheFree(info_);
        }
    }
    bool OnClick(UIView& view, const ClickEvent& event) override
    {
        if (img_ == nullptr) {
            return false;
        }
        if (info_.data != nullptr) {
            ImageCacheFree(info_);
        }
        if (!Screen::GetInstance().GetCurrentScreenBitmap(info_)) {
            return false;
        }
        img_->SetVisible(true);
        img_->SetSrc(&info_);
        img_->Invalidate();
        return false;
    }

    UIViewGroup* container_;
    UIImageView* img_;
    ImageInfo info_;
};

void UITestViewBitmap::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight() - BACK_BUTTON_HEIGHT);
        container_->SetHorizontalScrollState(false);
    }
    if (viewBitmap_ == nullptr) {
        viewBitmap_ = new UIImageView();
        // 500 : x pos of image; 50: y pos of image; 960 width of image; 480 : height of image.
        viewBitmap_->SetPosition(500, 50, 960, 480);
    }
    if (screenBitmap_ == nullptr) {
        screenBitmap_ = new UIImageView();
        // 0 : x pos of image; 210: y pos of image; 960 width of image; 480 : height of image.
        screenBitmap_->SetPosition(0, 210, 960, 480);
    }
    if (viewBitmapListener_ == nullptr) {
        viewBitmapListener_ = new ViewBitmapListener(container_, viewBitmap_);
    }
    if (screenBitmapListener_ == nullptr) {
        screenBitmapListener_ = new ScreenBitmapListener(container_, screenBitmap_);
    }
}

void UITestViewBitmap::TearDown()
{
    if (viewBitmapListener_ != nullptr) {
        delete viewBitmapListener_;
        viewBitmapListener_ = nullptr;
    }
    if (screenBitmapListener_ != nullptr) {
        delete screenBitmapListener_;
        screenBitmapListener_ = nullptr;
    }
    DeleteChildren(container_);
    container_ = nullptr;

    viewBitmap_ = nullptr;
    screenBitmap_ = nullptr;
}

const UIView* UITestViewBitmap::GetTestView()
{
    UIKitBitmapTestGetViewBitmap001();
    UIKitBitmapTestGetScreenBitmap001();
    return container_;
}

void UITestViewBitmap::UIKitBitmapTestGetViewBitmap001()
{
    UILabelButton* btn = new UILabelButton();
    container_->Add(btn);
    // 100 : x pos of button; 50: y pos of  button.
    btn->SetPosition(bitmap_positionX_, bitmap_positionY_, BUTTON_WIDHT3, BUTTON_HEIGHT3);
    btn->SetText("测试组件截屏");
    btn->SetFont(DEFAULT_VECTOR_FONT_FILENAME, BUTTON_LABEL_SIZE);
    btn->SetOnClickListener(viewBitmapListener_);
    btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::RELEASED);
    btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::PRESSED);
    btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::INACTIVE);
    btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::RELEASED);
    btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::PRESSED);
    btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::INACTIVE);
}

void UITestViewBitmap::UIKitBitmapTestGetScreenBitmap001()
{
    UILabelButton* btn = new UILabelButton();
    container_->Add(btn);
    // 100 : x pos of button; 150: y pos of  button.
    btn->SetPosition(bitmap_positionX_, bitmap_positionY_ + offsetY, BUTTON_WIDHT3, BUTTON_HEIGHT3);
    btn->SetText("测试全屏截屏");
    btn->SetFont(DEFAULT_VECTOR_FONT_FILENAME, BUTTON_LABEL_SIZE);
    btn->SetOnClickListener(screenBitmapListener_);
    btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::RELEASED);
    btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::PRESSED);
    btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::INACTIVE);
    btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::RELEASED);
    btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::PRESSED);
    btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::INACTIVE);
}
} // namespace OHOS

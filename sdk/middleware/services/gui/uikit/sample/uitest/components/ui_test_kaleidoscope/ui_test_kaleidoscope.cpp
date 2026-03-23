/*
 * Copyright (c) @CompanyNameMagicTag. 2024. All rights reserved.
 */

#include "ui_test_kaleidoscope.h"
#include "common/image_cache_manager.h"

namespace OHOS {

void UITestKaleidoscope::KaleidoscopeAnimator::Callback(UIView* view)
{
    if (view == nullptr) {
        return;
    }

    kaleidoscopeUtils_->Rotate(0.02f); // 0.02: rotate by 0.02
    view->Invalidate();
}

void UITestKaleidoscope::SetUp()
{
    container_ = new UIViewGroup();
    container_->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    container_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    container_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);

    imgInfo_ = ImageCacheManager::GetInstance().LoadSingleRes(RES_PATH"rabbit454.bin");
    if (imgInfo_ == nullptr) {
        delete container_;
        container_ = nullptr;
        return;
    }

    InitKaleidoscopeView();

    if (switchBtn_ == nullptr) {
        switchBtn_ = new UILabelButton();
    }
    switchBtn_->Resize(100, 100); // 100: size
    container_->Add(switchBtn_);
    switchBtn_->LayoutCenterOfParent();
    switchBtn_->SetOpaScale(OPA_TRANSPARENT);
    switchBtn_->SetOnClickListener(this);
}

void UITestKaleidoscope::InitKaleidoscopeImg()
{
    if (kaleidoscopeUtils_ == nullptr) {
        kaleidoscopeUtils_ = new KaleidoscopeUtils();
    }
    kaleidoscopeUtils_->Init(imgInfo_, nullptr);
    ImageInfoExt* infoExt = kaleidoscopeUtils_->GetKaleidoscopeInfo();

    if (img_ == nullptr) {
        img_ = new UIImageView();
    }
    img_->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    img_->SetAutoEnable(false);
    img_->SetResizeMode(UIImageView::ImageResizeMode::FILL);
    img_->SetSrc(&infoExt->info);
    container_->Add(img_);

    if (utilsAnimator_ == nullptr) {
        utilsAnimator_ = new KaleidoscopeAnimator(kaleidoscopeUtils_, img_);
    }
    utilsAnimator_->Start();
}

void UITestKaleidoscope::InitKaleidoscopeView()
{
    if (kaleidoscope_ == nullptr) {
        kaleidoscope_ = new UIKaleidoscopeView();
    }

    if (kaleidoscope_->SetSrc(imgInfo_) == false) {
        return;
    }

    container_->Add(kaleidoscope_);
    kaleidoscope_->RequestFocus();
    kaleidoscope_->SetRotateEventFactor(0.2f); // 0.2: rotate event factor
    kaleidoscope_->SetRotateAnimatorValue(0.02f); // 0.02: rotate animator value
    kaleidoscope_->StartRotateAnimator();
}

void UITestKaleidoscope::TearDown()
{
    if (utilsAnimator_ != nullptr) {
        utilsAnimator_->Stop();
        delete utilsAnimator_;
        utilsAnimator_ = nullptr;
    }

    if (kaleidoscope_ != nullptr) {
        kaleidoscope_->StopRotateAnimator();
        kaleidoscope_->ClearFocus();
        kaleidoscope_ = nullptr;
    }

    DeleteChildren(container_);
    container_ = nullptr;
    imgInfo_ = nullptr;
    img_ = nullptr;
    switchBtn_ = nullptr;

    if (kaleidoscopeUtils_ != nullptr) {
        delete kaleidoscopeUtils_;
        kaleidoscopeUtils_ = nullptr;
    }
    ImageCacheManager::GetInstance().UnloadSingleRes(RES_PATH"rabbit454.bin");
}

bool UITestKaleidoscope::OnClick(UIView& view, const ClickEvent& event)
{
    if (&view == switchBtn_) {
        if (kaleidoscope_ != nullptr) {
            container_->Remove(kaleidoscope_);
            delete kaleidoscope_;
            kaleidoscope_ = nullptr;

            InitKaleidoscopeImg();
            printf("Switch to Kaleidoscope View!");
        } else {
            utilsAnimator_->Stop();
            container_->Remove(img_);
            delete kaleidoscopeUtils_;
            delete img_;
            delete utilsAnimator_;
            kaleidoscopeUtils_ = nullptr;
            img_ = nullptr;
            utilsAnimator_ = nullptr;

            InitKaleidoscopeView();
            printf("Switch to Kaleidoscope Img!");
        }
    }
    return true;
}

const UIView* UITestKaleidoscope::GetTestView()
{
    return container_;
}
}

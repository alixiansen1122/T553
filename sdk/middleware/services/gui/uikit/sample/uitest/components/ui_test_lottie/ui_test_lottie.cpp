/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: Lottie Test
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */

#include "ui_test_lottie.h"
#include "common/image_cache_manager.h"

namespace OHOS {
constexpr uint8_t FILE_MAX = 8;
static char* g_filePath[FILE_MAX] = {
    RES_PATH"2016_lottie.bin",
    RES_PATH"singing-contract.bin",
    RES_PATH"girl-cycling-in-autumn.bin",
    RES_PATH"loading.bin",
    RES_PATH"ripple_loading_animation.bin",
    RES_PATH"starts_transparent.bin",
    RES_PATH"fingerprint_success.bin",
    RES_PATH"intelia_logo_animation.bin",
};

void UITestLottie::SetUp()
{
    fileIndex_ = 0;
    if (container_ == nullptr) {
        container_ = new UIViewGroup();
    }
    container_->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    container_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    container_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);

    if (lottieView_ == nullptr) {
        lottieView_ = new UILottView();
        lottieView_->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
        lottieView_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
        lottieView_->SetStyle(STYLE_BACKGROUND_COLOR, Color::White().full);
        if (lottieView_->SetSrc(g_filePath[fileIndex_])) {
            lottieView_->Start();
        }
        fileIndex_++;
    }
    container_->Add(lottieView_);

    startBtn_ = new UILabelButton();
    startBtn_->SetPosition(87, 380, 70, 50); // 87, 380, 70, 50: coordinate
    startBtn_->SetOnClickListener(this);
    startBtn_->SetText("Start");
    container_->Add(startBtn_);

    switchBtn_ = new UILabelButton();
    switchBtn_->SetPosition(157, 380, 70, 50); // 157, 380, 70, 50: coordinate
    switchBtn_->SetOnClickListener(this);
    switchBtn_->SetText("Switch");
    container_->Add(switchBtn_);

    repeatBtn_ = new UILabelButton();
    repeatBtn_->SetPosition(227, 380, 70, 50); // 227, 380, 70, 50: coordinate
    repeatBtn_->SetOnClickListener(this);
    repeatBtn_->SetText("Repeat");
    container_->Add(repeatBtn_);

    transformBtn_ = new UILabelButton();
    transformBtn_->SetPosition(297, 380, 80, 50); // 297, 380, 80, 50: coordinate
    transformBtn_->SetOnClickListener(this);
    transformBtn_->SetText("Transform");
    container_->Add(transformBtn_);
}

void UITestLottie::TearDown()
{
    DeleteChildren(container_);
    lottieView_ = nullptr;
    startBtn_ = nullptr;
    switchBtn_ = nullptr;
    repeatBtn_ = nullptr;
    transformBtn_ = nullptr;
    container_ = nullptr;
}

bool UITestLottie::OnClick(UIView& view, const ClickEvent& event)
{
    if (lottieView_ == nullptr) {
        return false;
    }
    if (&view == startBtn_) {
        lottieView_->Start();
    } else if (&view == switchBtn_) {
        lottieView_->SetSrc(g_filePath[fileIndex_]);
        fileIndex_++;
        if (fileIndex_ == FILE_MAX) {
            fileIndex_ = 0;
        }
        lottieView_->Start();
    } else if (&view == repeatBtn_) {
        bool repeat = !lottieView_->IsRepeat();
        lottieView_->SetRepeat(repeat);
        printf("SetRepeat to %d", repeat);
    } else if (&view == transformBtn_) {
        if (lottieView_->IsTransInvalid()) {
            Vector2<float> pivot = {lottieView_->GetWidth() / 2.0f, lottieView_->GetHeight() / 2.0f}; // 2: divider
            lottieView_->Rotate(90.0f, pivot);
            lottieView_->Scale(Vector2<float>{0.5f, 0.5f}, pivot);
            printf("Rotate 90 degree and scale by [0.5, 0.5]\n");
        } else {
            Matrix4<float> identity;
            lottieView_->GetTransformMap().SetMatrix(identity);
            lottieView_->Invalidate();
            printf("Reset transformation!\n");
        }
    }
    return true;
}

const UIView* UITestLottie::GetTestView()
{
    return container_;
}
}
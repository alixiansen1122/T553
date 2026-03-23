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

#include "ui_test_image_fps.h"
#include "common/screen.h"
#include "common/image_cache_manager.h"
#include "core/render_manager.h"
#include "graphic_config.h"
#include "securec.h"

namespace OHOS {
constexpr uint16_t X_POS = 50;
constexpr uint16_t Y_POS = 50;
constexpr uint16_t IMAGE_W = 220;
constexpr uint16_t IMAGE_H = 220;
constexpr uint16_t LABEL_W = 250;
constexpr uint16_t LABEL_H = 50;
constexpr uint16_t BUTTON_W = 100;
constexpr uint16_t BUTTON_H = 50;
constexpr uint16_t FONT_SIZE = 20;
constexpr uint16_t BUF_LEN = 15;
constexpr uint16_t IMAGE_NUM = 3;

static char* g_imagePath[IMAGE_NUM] = {
    RES_PATH"A094_051_8888.bin",
    RES_PATH"A094_051_888.bin",
    RES_PATH"A094_051_565.bin",
};

static ImageAnimatorInfo g_binImageAnimatorInfo[IMAGE_NUM] = {{{0}}};

void UITestImageFPS::LoadImg(void)
{
    ImageAnimatorInfo baseinfo = {nullptr, {X_POS, Y_POS}, IMAGE_W, IMAGE_H, IMG_SRC_IMAGE_INFO};
    for (int i = 0; i < IMAGE_NUM; i++) {
        ImageInfo* img = ImageCacheManager::GetInstance().LoadSingleRes(g_imagePath[i]);
        if (img == nullptr) {
            GRAPHIC_LOGE("Load image failed: %s", g_imagePath[i]);
            ReleaseImg();
            return;
        }
        g_binImageAnimatorInfo[i] = baseinfo;
        g_binImageAnimatorInfo[i].imageInfo = img;
    }
}

void UITestImageFPS::ReleaseImg(void)
{
    for (int i = 0; i < IMAGE_NUM; i++) {
        if (g_binImageAnimatorInfo[i].imageInfo != nullptr) {
            ImageCacheManager::GetInstance().UnloadSingleRes(g_imagePath[i]);
            g_binImageAnimatorInfo[i].imageInfo = nullptr;
        }
    }
}
void UITestImageFPS::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        if (container_ == nullptr) {
            return;
        }
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());

        LoadImg();
        imageAnimator_ = new UIImageAnimatorView();
        imageAnimator_->SetPosition(X_POS, Y_POS, IMAGE_W, IMAGE_H);
        imageAnimator_->SetImageAnimatorSrc(g_binImageAnimatorInfo, IMAGE_NUM, 50); // 50: update time Interval
        imageAnimator_->Start();
        container_->Add(imageAnimator_);
    }
    if (titleLabel_ == nullptr) {
        titleLabel_ = new UILabel();
        if (titleLabel_ == nullptr) {
            return;
        }
        titleLabel_->SetPosition(imageAnimator_->GetX() + imageAnimator_->GetWidth(), Y_POS, LABEL_W, LABEL_H);
        SetUpLabel(titleLabel_, "image-animator fps test");
    }
    if (typeLabel_ == nullptr) {
        typeLabel_ = new UILabel();
        if (typeLabel_ == nullptr) {
            return;
        }
        typeLabel_->SetPosition(imageAnimator_->GetX() + imageAnimator_->GetWidth(),
            titleLabel_->GetY() + LABEL_H, LABEL_W, LABEL_H);
        SetUpLabel(typeLabel_, "bin");
    }
    if (fpsLabel_ == nullptr) {
        fpsLabel_ = new UILabel();
        if (fpsLabel_ == nullptr) {
            return;
        }
        fpsLabel_->SetPosition(imageAnimator_->GetX() + imageAnimator_->GetWidth(),
            typeLabel_->GetY() + LABEL_H, LABEL_W, LABEL_H);
        SetUpLabel(fpsLabel_, "fps: ");
    }

    RenderManager::GetInstance().RegisterFPSChangedListener(this);
}

void UITestImageFPS::OnFPSChanged(float newFPS)
{
    if (fpsBuf_ == nullptr) {
        fpsBuf_ = static_cast<char*>(UIMalloc(BUF_LEN));
        if (fpsBuf_ == nullptr) {
            return;
        }
    }
    if (snprintf_s(fpsBuf_, BUF_LEN, BUF_LEN - 1, "fps: %.2f", newFPS) == -1) {
        return;
    }
    if (fpsLabel_ != nullptr) {
        fpsLabel_->SetText(fpsBuf_);
        fpsLabel_->Invalidate();
    }
}

void UITestImageFPS::SetUpLabel(UILabel* label, const char* title)
{
    if ((label == nullptr) || (container_ == nullptr)) {
        return;
    }
    label->SetStyle(STYLE_TEXT_COLOR, Color::Black().full);
    label->SetStyle(STYLE_BACKGROUND_COLOR, Color::White().full);
    label->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_SIZE);
    label->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    label->SetText(title);
    container_->Add(label);
    label->Invalidate();
}

void UITestImageFPS::TearDown()
{
    RenderManager::GetInstance().RegisterFPSChangedListener(nullptr);
    DeleteChildren(container_);
    if (fpsBuf_ != nullptr) {
        UIFree(reinterpret_cast<void*>(fpsBuf_));
        fpsBuf_ = nullptr;
    }
    container_ = nullptr;
    imageAnimator_ = nullptr;
    fpsLabel_ = nullptr;
    titleLabel_ = nullptr;
    typeLabel_ = nullptr;
    ReleaseImg();
}

UIView* UITestImageFPS::GetTestView()
{
    return container_;
}
} // namespace OHOS

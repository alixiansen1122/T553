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

#include "ui_test_texture_mapper.h"
#include "common/screen.h"
#include "graphic_config.h"

namespace OHOS {
namespace {
constexpr uint16_t DURATION = 3000;
constexpr uint16_t BUTTON_X = 350;
constexpr uint16_t BUTTON_OFFSET = 50;
constexpr uint16_t BUTTON_WIDTH = 60;
constexpr uint16_t BUTTON_HEIGHT = 40;
constexpr uint16_t PIVOT_X = 47;
constexpr uint16_t PIVOT_Y = 47;
} // namespace

void UITestTextureMapper::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
        container_->SetStyle(STYLE_BACKGROUND_OPA, 200); // 200: opa
        container_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Yellow().full);

        textureMapper_ = new UITextureMapper();
        textureMapper_->SetPosition(150, 150, 400, 400); // 150: x, y; 400: width, height
        textureMapper_->SetSrc(RES_PATH"blue_argb8888.bin");
        TestNegativeScale();

        container_->Add(textureMapper_);
    }

    uint16_t buttonY = 50; // 50: button y axis start position
    startBtn_ = new UILabelButton();
    SetUpButton(startBtn_, BUTTON_X, buttonY, "start");

    buttonY += BUTTON_OFFSET;
    resetBtn_ = new UILabelButton();
    SetUpButton(resetBtn_, BUTTON_X, buttonY, "reset");

    buttonY += BUTTON_OFFSET;
    cancelBtn_ = new UILabelButton();
    SetUpButton(cancelBtn_, BUTTON_X, buttonY, "cancel");

    buttonY += BUTTON_OFFSET;
    ani1Btn_ = new UILabelButton();
    SetUpButton(ani1Btn_, BUTTON_X, buttonY, "Ani 1");

    buttonY += BUTTON_OFFSET;
    ani2Btn_ = new UILabelButton();
    SetUpButton(ani2Btn_, BUTTON_X, buttonY, "Ani 2");

    buttonY += BUTTON_OFFSET;
    ani3Btn_ = new UILabelButton();
    SetUpButton(ani3Btn_, BUTTON_X, buttonY, "Ani 3");

    buttonY += BUTTON_OFFSET;
    ani4Btn_ = new UILabelButton();
    SetUpButton(ani4Btn_, BUTTON_X, buttonY, "Ani 4");
}

void UITestTextureMapper::TearDown()
{
    DeleteChildren(container_);
    container_ = nullptr;
    textureMapper_ = nullptr;
}

const UIView* UITestTextureMapper::GetTestView()
{
    return container_;
}

void UITestTextureMapper::SetUpButton(UILabelButton* btn, uint16_t x, uint16_t y, const char* title)
{
    if (btn == nullptr) {
        return;
    }
    container_->Add(btn);
    btn->SetPosition(x, y, BUTTON_WIDTH, BUTTON_HEIGHT); // 60: width, 50: height
    btn->SetText(title);
    btn->SetOnClickListener(this);
}

void UITestTextureMapper::TestNegativeScale()
{
    textureMapper_->Reset();
    textureMapper_->SetScaleStart(-1.7); // -1.7: scale start
    textureMapper_->SetScaleEnd(0.5); // 0.5: scale end
    textureMapper_->SetRotateStart(0);
    textureMapper_->SetRotateEnd(0);
    textureMapper_->SetPivot(0, 0);
    textureMapper_->SetDurationTime(DURATION);
    textureMapper_->SetEasingFunc(EasingEquation::LinearEaseNone);
}

void UITestTextureMapper::TestPositionScale()
{
    textureMapper_->Reset();
    textureMapper_->SetScaleStart(2); // 2: scale end
    textureMapper_->SetScaleEnd(1);
    textureMapper_->SetRotateStart(0);
    textureMapper_->SetRotateEnd(0);
    textureMapper_->SetPivot(0, 0);
    textureMapper_->SetDurationTime(DURATION);
    textureMapper_->SetEasingFunc(EasingEquation::CircEaseOut);
}

void UITestTextureMapper::TestRotateWithPivot()
{
    textureMapper_->Reset();
    textureMapper_->SetScaleStart(1);
    textureMapper_->SetScaleEnd(1);
    textureMapper_->SetRotateStart(270); // 270: start angle
    textureMapper_->SetRotateEnd(90); // 90: end angle
    textureMapper_->SetPivot(0, 30); // 30: Pivot Y
    textureMapper_->SetDurationTime(DURATION);
    textureMapper_->SetEasingFunc(EasingEquation::LinearEaseNone);
}

void UITestTextureMapper::TestComplex()
{
    textureMapper_->Reset();
    textureMapper_->SetScaleStart(1.5); // 1.5: scale start
    textureMapper_->SetScaleEnd(0.5); // 0.5: scale end
    textureMapper_->SetRotateStart(-270); // -270: start angle
    textureMapper_->SetRotateEnd(0);
    textureMapper_->SetPivot(PIVOT_X, PIVOT_Y);
    textureMapper_->SetDurationTime(DURATION);
    textureMapper_->SetEasingFunc(EasingEquation::CircEaseInOut);
}

bool UITestTextureMapper::OnClick(UIView& view, const ClickEvent& event)
{
    if (&view == startBtn_) {
        textureMapper_->Start();
    } else if (&view == resetBtn_) {
        textureMapper_->Reset();
    } else if (&view == cancelBtn_) {
        textureMapper_->Cancel();
    } else if (&view == ani1Btn_) {
        TestNegativeScale();
    } else if (&view == ani2Btn_) {
        TestPositionScale();
    } else if (&view == ani3Btn_) {
        TestRotateWithPivot();
    } else if (&view == ani4Btn_) {
        TestComplex();
    }
    return true;
}
} // namespace OHOS

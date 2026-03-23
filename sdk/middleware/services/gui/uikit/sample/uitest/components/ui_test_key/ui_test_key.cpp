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

#include "ui_test_key.h"

namespace OHOS {
namespace {
static const int16_t LABEL_X = 100;
static const int16_t LABEL_H = 100;
static const int16_t LABEL_W = 200;
static const int16_t LABEL_ID_Y = 50;
static const int16_t LABEL_STATE_Y = 150;
static const int16_t LABEL_STYLE_Y = 250;
} // namespace

void UITestKey::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
        container_->SetHorizontalScrollState(false);
        container_->SetThrowDrag(true);
    }

    if (id_ == nullptr) {
        id_ = new UILabel();
        id_->SetPosition(LABEL_X, LABEL_ID_Y, LABEL_W, LABEL_H);
        id_->SetText("KeyId");
        id_->SetViewId("keyId");
        container_->Add(id_);
    }

    if (state_ == nullptr) {
        state_ = new UILabel();
        state_->SetPosition(LABEL_X, LABEL_STATE_Y, LABEL_W, LABEL_H);
        state_->SetText("KeyState");
        state_->SetViewId("keyState");
        container_->Add(state_);
    }

    if (style_ == nullptr) {
        style_ = new UILabel();
        style_->SetPosition(LABEL_X, LABEL_STYLE_Y, LABEL_W, LABEL_H);
        style_->SetText("style");
        style_->SetViewId("style");
        style_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Gray().full);
        style_->SetStyle(STYLE_BACKGROUND_OPA, 255); // 255: opa
        style_->SetStyle(STYLE_BORDER_RADIUS, 20); // 20: radius
        style_->SetStyle(STYLE_BORDER_COLOR, Color::Yellow().full);
        style_->SetStyle(STYLE_BORDER_OPA, 255); // 255: opa
        style_->SetStyle(STYLE_BORDER_WIDTH, 10); // 10: width
        container_->Add(style_);
    }

    if (keyActListener_ == nullptr) {
        keyActListener_ = new TestKeyInputListener();
        if (keyActListener_ != nullptr) {
            RootView::GetInstance()->SetOnKeyActListener(keyActListener_);
        }
    }

    TestGaussianBlur();
}

class BlurListener : public UIView::OnClickListener {
public:
    BlurListener() {}

    ~BlurListener() override {}

    bool OnClick(UIView& view, const ClickEvent& event) override
    {
        count++;
        if (count % 2 == 1) { // 2: mod
            view.SetBackgroundBlur(10); // 10: blurRadius
        } else {
            view.ClearBackgroundBlur();
            count = 0;
        }
        return true;
    }
private:
    uint8_t count = 0;
};

void UITestKey::TestGaussianBlur()
{
    if (blur_ == nullptr) {
        blur_ = new UILabelButton();
        blur_->SetText("Blur");
        blur_->SetPosition(300, 150, 100, 50); // 300: x, 150: y, 100: width, 50: height
        blur_->SetFullScreenBlur(true);
        blurListener_ = new BlurListener();
        blur_->SetOnClickListener(blurListener_);
        container_->Add(blur_);
    }
}

void UITestKey::TearDown()
{
    RootView::GetInstance()->ClearOnKeyActListener();
    if (keyActListener_ != nullptr) {
        delete keyActListener_;
        keyActListener_ = nullptr;
    }
    blur_->ClearBackgroundBlur();
    DeleteChildren(container_);
    container_ = nullptr;
    id_ = nullptr;
    state_ = nullptr;
    style_ = nullptr;
    blur_ = nullptr;
    if (blurListener_ != nullptr) {
        delete blurListener_;
        blurListener_ = nullptr;
    }
}

const UIView* UITestKey::GetTestView()
{
    return container_;
}
} // namespace OHOS
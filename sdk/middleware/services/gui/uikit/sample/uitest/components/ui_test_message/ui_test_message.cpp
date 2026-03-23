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

#include "ui_test_message.h"
#include "animator/easing_equation.h"

namespace OHOS {
const int8_t TEXT_ARRAY_LEN = 24;
const float MIN_BLUR_VALUE = 0.0f;
const float MAX_BLUR_VALUE = 32.0f;
const float BLUR_MODIFY_STEP = 0.4f;

void UITestMessage::SetUp()
{
    container_ = new UIViewGroup();
    container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());

    UIImageView* img = new UIImageView();
    img->SetSrc(RES_PATH"earth.bin");
    img->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    img->SetStyle(STYLE_IMAGE_OPA, OPA_OPAQUE);
    container_->Add(img);

    under_ = new UILabel;
    under_->SetPosition(158, 101, 140, 34); // 158: x, 101: y, 140: w, 34: h
    under_->SetText("under");
    under_->SetStyle(STYLE_BACKGROUND_COLOR, fgColors_[0]);
    under_->SetStyle(STYLE_TEXT_COLOR, Color::Red().full);
    under_->SetStyle(STYLE_TOP_BORDER_RADIUS, 20); // 20: top radius
    under_->SetStyle(STYLE_BOTTOM_BORDER_RADIUS, 50); // 50: bottom radius
    under_->SetStyle(STYLE_BACKGROUND_OPA, 200); // 200: Opacity for background.
    under_->SetStyle(STYLE_BORDER_WIDTH, 10); // 10: border width
    under_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 35); // 35: Font size.
    under_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    container_->Add(under_);

    msgLabel_ = new UILabel();
    msgLabel_->SetPosition(23, 80, 405, 201); // 23, 80, 405, 201: Left, right, width, height.
    msgLabel_->SetText("Message: comes, please wait.................");
    msgLabel_->SetMarqueeBlankNum(blankSapce_);
    msgLabel_->SetLineBreakMode(UILabel::LINE_BREAK_MARQUEE);
    msgLabel_->SetStyle(STYLE_BACKGROUND_COLOR, 0xff505050);
    msgLabel_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    msgLabel_->SetStyle(STYLE_BORDER_RADIUS, 20); // 20: Radius for label boarder.
    msgLabel_->SetStyle(STYLE_BACKGROUND_OPA, 150); // 150: Opacity for background.
    msgLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 35); // 35: Font size.
    msgLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    container_->Add(msgLabel_);

    SetupBtn(70, 35, "fg color", fgColor_); // 70: x, 35: y
    SetupBtn(170, 35, "add blank", addBlank_); // 170: x, 35: y
    SetupBtn(270, 35, "switch mode", labelMode_); // 270: x, 35: y
    SetupBtn(70, 360, "cache", cacheEnable_); // 70: x, 360: y
    SetupBtn(70, 300, "Add blur", add_); // 70: x, 300: y
    SetupBtn(194, 300, "Reduce blur", reduce_); // 194: x, 300: y
    SetupBtn(318, 300, "Clear blur", clear_); // 318: x, 300: y

    valueLabel_ = new UILabel();
    valueLabel_->SetPosition(194, 360, 100, 40); // 194, 360, 100, 40: Left, right, width, height.
    valueLabel_->SetText("BLUR: 0.00");
    valueLabel_->SetLineBreakMode(UILabel::LINE_BREAK_STRETCH);
    valueLabel_->SetStyle(STYLE_TEXT_COLOR, Color::Red().full);
    valueLabel_->SetStyle(STYLE_BORDER_OPA, 0); // 20: Radius for label boarder.
    valueLabel_->SetStyle(STYLE_BACKGROUND_OPA, 0); // 150: Opacity for background.
    valueLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 30); // 30: Font size.
    valueLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    container_->Add(valueLabel_);
}

void UITestMessage::TearDown()
{
    msgLabel_->ClearBackgroundBlur();
    UIViewGroup::RemoveAndDeleteAllRecursively(container_);
    msgLabel_ = nullptr;
    add_ = nullptr;
    reduce_ = nullptr;
    clear_ = nullptr;
    valueLabel_ = nullptr;
    fgColor_ = nullptr;
    cacheEnable_ = nullptr;
    under_ = nullptr;
    addBlank_ = nullptr;
    labelMode_ = nullptr;
}

const UIView* UITestMessage::GetTestView()
{
    return container_;
}

bool UITestMessage::OnClick(UIView& view, const ClickEvent& event)
{
    if (&view == add_) {
        blurVaule_ += BLUR_MODIFY_STEP;
    } else if (&view == reduce_) {
        blurVaule_ -= BLUR_MODIFY_STEP;
    } else if (&view == clear_) {
        blurVaule_ = MIN_BLUR_VALUE;
    } else if (&view == fgColor_) {
        uint32_t color = fgColors_[fgIndex_];
        under_->SetStyle(STYLE_BACKGROUND_COLOR, color);
        under_->SetWidth(140 + fgIndex_ * 30); // 140: w, 30: width variation value
        fgIndex_ = (fgIndex_ + 1) % 2; // 2: num
        under_->Invalidate();
        return true;
    } else if (&view == cacheEnable_) {
        msgLabel_->EnableGaussianBlurCache(!msgLabel_->IsGaussianBlurCacheEnable());
    } else if (&view == addBlank_) {
        blankSapce_ += 10;  // 10: blankspace num
        msgLabel_->SetMarqueeBlankNum(blankSapce_);
    } else if (&view == labelMode_) {
        if (msgLabel_->GetLineBreakMode() == UILabel::LINE_BREAK_MARQUEE) {
            msgLabel_->SetLineBreakMode(UILabel::LINE_BREAK_TRUNCATE);
        } else {
            msgLabel_->SetLineBreakMode(UILabel::LINE_BREAK_MARQUEE);
        }
    }
    if (FloatMore(blurVaule_, MAX_BLUR_VALUE)) {
        blurVaule_ = MAX_BLUR_VALUE;
    } else if (FloatLess(blurVaule_, MIN_BLUR_VALUE)) {
        blurVaule_ = MIN_BLUR_VALUE;
    }
    msgLabel_->SetBackgroundBlur(blurVaule_);
    char blurText[TEXT_ARRAY_LEN] = {0};
    std::string enable = msgLabel_->IsGaussianBlurCacheEnable() ? "CACHE" : "BLUR";
    if (snprintf_s(blurText, TEXT_ARRAY_LEN, TEXT_ARRAY_LEN, "%s: %4.1f", enable.c_str(), blurVaule_) == -1) {
        GRAPHIC_LOGE("sprintf error");
        return false;
    }
    valueLabel_->SetText(blurText);
    return true;
}

void UITestMessage::SetupBtn(int16_t x, int16_t y, const std::string &title, UILabelButton *&btn)
{
    btn = new UILabelButton();
    btn->SetPosition(x, y, 100, 40); // 100: w, 40: h
    btn->SetText(title.c_str());
    btn->SetOnClickListener(this);
    container_->Add(btn);
}
}
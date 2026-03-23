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

#include "ui_test_emoji.h"
#include "font/ui_font.h"

namespace OHOS {

void UITestEmoji::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->SetThrowDrag(true);
        container_->SetHorizontalScrollState(false);
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
        x_ = START_X;
        y_ = START_Y;
    }
}

void UITestEmoji::TearDown()
{
    DeleteChildren(container_);
    container_ = nullptr;
}

const UIView *UITestEmoji::GetTestView()
{
    TestLabel();
    TestArcLabel();
    return container_;
}

void UITestEmoji::TestArcLabelDisplay(const std::string &title, const int16_t startAngle, const int16_t endAngle,
    const UIArcLabel::TextOrientation orientation, UITextLanguageAlignment alignment)
{
    if (container_ != nullptr) {
        UIArcLabel* label = new UIArcLabel();
        label->SetPosition(x_, y_);
        label->SetArcTextCenter(CENTER_X, CENTER_Y + y_ + GAP);
        label->SetArcTextRadius(RADIUS);
        label->SetArcTextAngle(startAngle, endAngle);
        label->SetArcTextOrientation(orientation);
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 30); // 30: font size
        label->SetText(title.c_str());
        label->SetStyle(STYLE_LETTER_SPACE, 10); // 10: space
        label->SetStyle(STYLE_TEXT_COLOR, Color::Red().full);
        label->SetAlign(alignment);
        container_->Add(label);
        y_ += (RADIUS * 2) + GAP; // 2: diameter
    }
}

void UITestEmoji::TestLabelDisplay(const std::string &title, UILabel::LineBreakMode mode, UITextLanguageDirect direct)
{
    if (container_ == nullptr) {
        return;
    }

    UILabel *label = new UILabel;
    label->SetDirect(direct);
    label->SetPosition(x_, y_, WIDTH, HEIGHT);
    label->SetLineBreakMode(static_cast<uint8_t>(mode));
    label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 30);  // 30: font size
    label->SetText(title.c_str());

    container_->Add(label);
    y_ += HEIGHT + GAP;
}

void UITestEmoji::TestArcLabel()
{
    std::string str = "\U0001F600\u4e52\U0001F609\u4e53\U0001F60f";
    int16_t startAngle = 0;
    int16_t endAngle = 270;
    UIArcLabel::TextOrientation orientation[] = {
        UIArcLabel::TextOrientation::INSIDE,
        UIArcLabel::TextOrientation::OUTSIDE
    };
    UITextLanguageAlignment alignment[] = {TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_RIGHT, TEXT_ALIGNMENT_CENTER};
    constexpr int16_t lenOri = sizeof(orientation) / sizeof(orientation[0]);
    constexpr int16_t lenAli = sizeof(alignment) / sizeof(alignment[0]);
    for (int16_t i = 0; i < lenOri; ++i) {
        for (int16_t j = 0; j < lenAli; ++j) {
            TestArcLabelDisplay(str, startAngle, endAngle, orientation[i], alignment[j]);
        }
    }
}

void UITestEmoji::TestLabel()
{
    using LineBreakMode = UILabel::LineBreakMode;
    struct {
        std::string str;
        LineBreakMode mode;
    } testCase[] = {
        {"", LineBreakMode::LINE_BREAK_ADAPT},
        {"xxxxxxx\U0001F610\u4e52\U0001F600\u4e53\U0001F603yyyyyyyyy", LineBreakMode::LINE_BREAK_ADAPT},
        {"xxxxxxx\U0001F600\u4e52\U0001F605\u4e53\U0001F602yyyyyyyyy", LineBreakMode::LINE_BREAK_STRETCH},
        {"xxxxxxx\U0001F603\u4e52\U0001F604\u4e53\U0001F609yyyyyyyyy", LineBreakMode::LINE_BREAK_WRAP},
        {"xxxxxxx\U0001F608\u4e52\U0001F613\u4e53\U0001F60ayyyyyyyyy", LineBreakMode::LINE_BREAK_ELLIPSIS},
        {"xxxxxxx\U0001F601\u4e52\U0001F616\u4e53\U0001F60dyyyyyyyyy", LineBreakMode::LINE_BREAK_TRUNCATE},
        {"xxxxxxx\U0001F607\u4e52\U0001F614\u4e53\U0001F60byyyyyyyyy", LineBreakMode::LINE_BREAK_MARQUEE},
        {"xxxxxxx\U0001F602\u4e52\U0001F615\u4e53\U0001F60cyyyyyyyyy", LineBreakMode::LINE_BREAK_OSCILLATION},
        {"xxxxxxx\U0001F601\u4e52\U0001F616\u4e53\U0001F60dyyyyyyyyy", LineBreakMode::LINE_BREAK_CLIP},
    };
    constexpr int len = sizeof(testCase) / sizeof(testCase[0]);
    for (int i = 0; i < len; ++i) {
        TestLabelDisplay(testCase[i].str, testCase[i].mode);
        TestLabelDisplay(testCase[i].str, testCase[i].mode, TEXT_DIRECT_RTL);
    }
}
}

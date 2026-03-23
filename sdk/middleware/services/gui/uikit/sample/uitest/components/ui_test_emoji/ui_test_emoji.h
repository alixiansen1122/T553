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

#ifndef UI_TEST_EMOJI_H
#define UI_TEST_EMOJI_H

#include "ui_test.h"
#include "common/screen.h"
#include "components/ui_label.h"
#include "components/ui_arc_label.h"
#include "components/ui_scroll_view.h"

namespace OHOS {
class UITestEmoji : public UITest {
public:
    UITestEmoji() {}
    ~UITestEmoji() override {}
    void SetUp() override;
    void TearDown() override;
    const UIView *GetTestView() override;

private:
    constexpr static int16_t GAP = 40;
    constexpr static int16_t CENTER_X = 150;
    constexpr static int16_t CENTER_Y = 150;
    constexpr static int16_t RADIUS = 100;
    constexpr static int16_t WIDTH = 180;
    constexpr static int16_t HEIGHT = 80;
    constexpr static int16_t START_X = 100;
    constexpr static int16_t START_Y = 100;
    UIScrollView *container_ = nullptr;
    int16_t x_ = START_X;
    int16_t y_ = START_Y;

    void TestArcLabelDisplay(const std::string &title, const int16_t startAngle, const int16_t endAngle,
        const UIArcLabel::TextOrientation orientation, UITextLanguageAlignment alignment);
    void TestLabelDisplay(const std::string &title,
        UILabel::LineBreakMode mode, UITextLanguageDirect direct = TEXT_DIRECT_LTR);
    void TestArcLabel();
    void TestLabel();
};
}
#endif // UI_TEST_EMOJI_H

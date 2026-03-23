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

#ifndef UI_TEST_ARC_LABEL_H
#define UI_TEST_ARC_LABEL_H

#include "components/ui_arc_label.h"
#include "components/ui_label.h"
#include "components/ui_scroll_view.h"
#include "ui_test.h"

namespace OHOS {
class UITestArcLabel : public UITest {
public:
    UITestArcLabel() {}
    ~UITestArcLabel() override {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;

private:
    UIScrollView* container_ = nullptr;
    int16_t x_ = 0;
    int16_t y_ = 0;

    void TestDegree();
    void TestOrientation();
    void TestAlignment();
    void TestArcLabelDisplay(const char* title, const int16_t startAngle, const int16_t endAngle,
        const UIArcLabel::TextOrientation orientation, UITextLanguageAlignment alignment);
};
}
#endif
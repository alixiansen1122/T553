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

#ifndef UI_TEST_MULTI_FONT_H
#define UI_TEST_MULTI_FONT_H

#include "ui_test.h"
#include "components/ui_scroll_view.h"
#if ENABLE_ICU && ENABLE_MULTI_FONT && ENABLE_SHAPING
namespace OHOS {
class UITestMultiFont : public UITest {
public:
    UITestMultiFont() {}
    ~UITestMultiFont() override {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;

private:
    void RegisterFont();
    void TestItalian();
    void TestChineseAndItalian();
    void TestArabic();
    void TestChineseAndArabic();
    void TestArabicAndChinese();

    UIScrollView* container_ = nullptr;
    uint16_t labelX_ = 0;
    uint16_t labelY_ = 0;
};
}
#endif
#endif
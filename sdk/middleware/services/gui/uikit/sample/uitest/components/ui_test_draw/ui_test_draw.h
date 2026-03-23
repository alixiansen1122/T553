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

#ifndef UI_TEST_DRAW_H
#define UI_TEST_DRAW_H

#include "ui_test.h"
#include "components/ui_canvas.h"
#include "components/ui_scroll_view.h"

namespace OHOS {
class UITestDraw : public UITest {
public:
    UITestDraw() {}
    ~UITestDraw() override {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;

    void TestDraw();
private:
    void SetUpCanvas(UIView* canvas);
    void DrawLine(UICanvas* canvas, Paint& paint);
    void DrawPathTriangle(UICanvas* canvas, Paint& paint);
    UIScrollView* container_ = nullptr;
};
}
#endif
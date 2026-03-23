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

#ifndef UI_TEST_CAPTURE_H
#define UI_TEST_CAPTURE_H

#include "ui_test.h"
#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"

namespace OHOS {
class UITestCapture : public UITest {
public:
    UITestCapture() {}
    ~UITestCapture() override {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;
    void TestDraw();

private:
    UIScrollView* container_ = nullptr;
    UIView::OnClickListener* viewBitmapListener_ = nullptr;
    UIView::OnClickListener* screenBitmapListener_ = nullptr;
    UIView::OnClickListener* rectBitmapListener_ = nullptr;
    UILabelButton* viewBitmapBtn_ = nullptr;
    UILabelButton* screenBitmapBtn_ = nullptr;
    UILabelButton* rectBitmapBtn_ = nullptr;

    void TestViewBitmap();
    void TestScreenBitmap();
    void TestRectBitmap();
};
}
#endif // UI_TEST_CAPTURE_H
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

#ifndef UI_TEST_JEPG_H
#define UI_TEST_JEPG_H

#include "ui_test.h"
#include "common/screen.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "core/render_manager.h"
#include "components/ui_image_view.h"
#include "components/ui_scroll_view.h"
#include "graphic_config.h"

#if CAPABILITY_HW_JPEG_DECODE
namespace OHOS {
class UITestJPEG : public UITest {
public:
    UITestJPEG() {}
    ~UITestJPEG() override {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;
private:
    void SetBackgroundColor();
    void TestJPEG();
    void TestFillJPEG();
    void TestContainJPEG();
    void CreateLabel(const char* text);
    UIScrollView* container_ = nullptr;
};
}
#endif
#endif
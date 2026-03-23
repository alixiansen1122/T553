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

#ifndef UI_TEST_KEYBOARD_H
#define UI_TEST_KEYBOARD_H

#include "components/ui_edit_text.h"
#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "graphic_config.h"
#include "ui_test.h"

namespace OHOS {
#if ENABLE_SOFT_KEYBOARD
class UITestKeyboard : public UITest, public UIView::OnClickListener {
public:
    UITestKeyboard() {}
    ~UITestKeyboard() override {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;

    void UIKitUIEditTextTestDisplay001();
    bool OnClick(UIView& view, const ClickEvent& event) override;
private:
    UIEditText* SetupEditText(const char* value, const char* placeholder, const char* viewId);

    UIScrollView* container_ = nullptr;
    UIEditText* editText2_ = nullptr;
    UILabelButton* showsBtn_ = nullptr;
};
#endif
} // namespace OHOS
#endif // UI_TEST_KEYBOARD_H
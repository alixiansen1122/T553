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

#ifndef UI_TEST_TEXT_FPS_H
#define UI_TEST_TEXT_FPS_H

#include "components/ui_list.h"
#include "components/ui_label.h"
#include "components/ui_scroll_view.h"
#include "components/text_adapter.h"
#include "ui_test.h"
#include "animator/animator.h"

namespace OHOS {

class UITestTextFPS : public UITest {
public:
    UITestTextFPS();
    ~UITestTextFPS() override {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;

private:
    class ScrollTextCallback : public AnimatorCallback {
    public:
        ~ScrollTextCallback() override {}
        void Callback(UIView* view) override;
    };
    class TextList : public UIList {
    public:
        void SetAnimator(Animator* animator);
        bool OnPressEvent(const PressEvent& event) override;
        bool OnLongPressEvent(const LongPressEvent& event) override;
    private:
        Animator* animator_ = nullptr;
    };

    ScrollTextCallback callback_;
    Animator* animator_ = nullptr;
    TextList* list_ = nullptr;
    TextAdapter* adapter_ = nullptr;
    List<const char*>* adapterData_ = nullptr;
};
} // namespace OHOS
#endif // UI_TEST_KEY_H

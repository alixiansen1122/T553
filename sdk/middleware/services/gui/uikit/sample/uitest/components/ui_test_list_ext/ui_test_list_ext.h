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


#ifndef UI_TEST_LIST_EXT_H
#define UI_TEST_LIST_EXT_H

#include "ui_test_list_ext.h"
#include "components/ui_list_ext.h"
#include "menu_item_adapter.h"
#include "ui_test.h"
#include "gfx_utils/sys_info.h"

namespace OHOS {
class UITestListExt : public UITest {
public:
    UITestListExt() : data_(nullptr), animator_(nullptr), list_(nullptr), adapter_(nullptr) {}
    ~UITestListExt() override {}
    void SetUp() override;
    void TearDown() override;
    UIView* GetTestView() override;
    void UIListScrollTestFPS();

protected:
    class ListExtFpsCallback : public AnimatorCallback {
    public:
        ListExtFpsCallback() {}

        ~ListExtFpsCallback() override {}

        void Callback(UIView* view) override;
    };
    class TestList : public UIListExt {
    public:
        void SetAnimator(Animator* animator);
        bool OnPressEvent(const PressEvent& event) override;
        bool OnLongPressEvent(const LongPressEvent& event) override;
    private:
        Animator* animator_ = nullptr;
    };
    Animator* animator_;
    ListExtFpsCallback callback_;
    TestList* list_;

private:
    List<ItemNode*>* data_;
    MenuItemAdapter* adapter_;
};
} // namespace OHOS
#endif // UI_TEST_LIST_FPS_H

/*
 * Copyright (c) 2024 CompanyNameMagicTag.
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

#ifndef UI_TEST_DRAW_RECT_H
#define UI_TEST_DRAW_RECT_H

#include "ui_test.h"
#include "components/ui_scroll_view.h"
#include "animator/animator_manager.h"

namespace OHOS {
class RectAnimatorCallback : public AnimatorCallback {
public:
    explicit RectAnimatorCallback(UIView* view)
        : animator_(new Animator(this, view, 1000, true)) {}  // 1000:duration of animator_, in milliseconds.

    ~RectAnimatorCallback() override
    {
        if (animator_ != nullptr) {
            delete animator_;
            animator_ = nullptr;
        }
    }

    void Callback(UIView* view) override;

    Animator* GetAnimator() const
    {
        return animator_;
    }

protected:
    int16_t curIndex_ = 0;
    uint32_t lastRunTime_ = 0;
    Animator* animator_;
};

class UITestDrawRect : public UITest {
public:
    UITestDrawRect() {}
    ~UITestDrawRect() override {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;

private:
    UIView* rect_ = nullptr;
    UIScrollView* container_ = nullptr;
    RectAnimatorCallback* callback_ = nullptr;
    Animator* rectAnimator_ = nullptr;
};
}
#endif
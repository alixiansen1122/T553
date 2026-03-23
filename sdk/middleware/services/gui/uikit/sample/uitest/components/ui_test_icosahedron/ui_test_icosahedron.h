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

#ifndef UI_TEST_ICOSAHEDRON_H
#define UI_TEST_ICOSAHEDRON_H

#include "ui_test.h"
#include "components/ui_icosahedron_view.h"
#include "components/ui_image_view.h"
#include "animator/animator_manager.h"

namespace OHOS {
class EntryAnimatorCallback : public AnimatorCallback {
public:
    explicit EntryAnimatorCallback(UIView* view, int16_t startPos, int16_t endPos)
        : startPos_(startPos),
          endPos_(endPos),
          animator_(new Animator(this, view, 1000, false)) {}  // 1000:duration of animator_, in milliseconds.

    ~EntryAnimatorCallback() override
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
    int16_t startPos_;
    int16_t endPos_;
    Animator* animator_;
};

class UITestIcosahedron : public UITest, public UIView::OnClickListener {
public:
    UITestIcosahedron() {}
    ~UITestIcosahedron() override {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;
    bool OnClick(UIView& view, const ClickEvent& event) override;

private:
    UIIcosahedronView* container_ = nullptr;
    ImageInfo* imgInfo_ = nullptr;
    ImageInfo* imgInverseInfo_ = nullptr;
    EntryAnimatorCallback* callback_ = nullptr;
    Animator* enterAnimator_ = nullptr;
};
}
#endif // UI_TEST_ICOSAHEDRON_H
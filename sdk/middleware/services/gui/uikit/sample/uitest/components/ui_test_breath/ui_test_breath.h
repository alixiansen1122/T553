/*
 * Copyright (c) 2023 CompanyNameMagicTag.
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

#ifndef UI_TEST_BREATH_H
#define UI_TEST_BREATH_H

#include "ui_test.h"
#include "components/ui_image_view.h"
#include "components/ui_label_button.h"
#include "components/ui_label.h"
#include "components/root_view.h"
#include "hal_tick.h"

namespace OHOS {
class BreathAnimatorCallback : public AnimatorCallback {
public:
    explicit BreathAnimatorCallback(UIImageView *img) : img_(img) {}
    ~BreathAnimatorCallback() override {}
    void Callback(UIView *view) override;
    void SetTextView(UILabel *view)
    {
        breath = view;
    }

    uint32_t startTimestamp = 0;

private:
    UIImageView* img_ = nullptr;
    UILabel* breath = nullptr;
    bool isBreathIn = false;
    int16_t angle = 0;
    int16_t step = -1;
    float scale = 0;
    uint16_t curTime = 0;
    uint16_t durationTime = 3000; // 3000: means 3000ms
    int16_t startVal = durationTime;
    int16_t endVal = durationTime / 2; // 2: half
};

class UITestBreath : public UITest, public UIView::OnClickListener {
public:
    UITestBreath() {}
    ~UITestBreath() override {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;
    bool OnClick(UIView& view, const ClickEvent& event) override;

private:
    void CaseInit();
    BreathAnimatorCallback *callBack_ = nullptr;
    Animator *ballAnimator_ = nullptr;
    UIImageView* img_ = nullptr;
    UILabelButton* start_ = nullptr;
    UILabel* breath_ = nullptr;
    UIViewGroup* container_ = nullptr;
    bool animateStart_ = false;
};
}
#endif

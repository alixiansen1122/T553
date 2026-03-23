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

#ifndef UI_TEST_RUNNING_BALL_H
#define UI_TEST_RUNNING_BALL_H

#include "ui_test.h"
#include "components/ui_scroll_view.h"
#include "components/ui_canvas_ext.h"
#include "components/ui_label_button.h"
#include "components/root_view.h"
#include "hal_tick.h"

namespace OHOS {
#if ENABLE_VGU_ENGINE && CAPABILITY_HWDRAW_PATH
class BallAnimatorCallback : public AnimatorCallback {
public:
    explicit BallAnimatorCallback(UICanvasExt *canvas) : canvas_(canvas) {}
    ~BallAnimatorCallback() override {}
    void Callback(UIView *view) override;
    void NotifyStart();

private:
    void UpdateBall(uint16_t curTime);

    UICanvasExt *canvas_ = nullptr;
    bool runningFlag_ = false;
    int16_t startPos_ = 0;
    int16_t endPos_ = 0;
    uint16_t curTime_ = 0;
    uint16_t durationTime_ = 2000; // 2000: means 2000ms
    uint32_t startTimestamp_ = 0;
};

class UITestRunningBall : public UITest, public UIView::OnClickListener {
public:
    UITestRunningBall() {}
    ~UITestRunningBall() override {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;
    bool OnClick(UIView& view, const ClickEvent& event) override;

private:
    void CaseInit();
    BallAnimatorCallback *callBack_ = nullptr;
    Animator *ballAnimator_ = nullptr;
    UICanvasExt* canvas_ = nullptr;
    UILabelButton* restart_ = nullptr;
    UIViewGroup* container_ = nullptr;
};
#endif
}
#endif

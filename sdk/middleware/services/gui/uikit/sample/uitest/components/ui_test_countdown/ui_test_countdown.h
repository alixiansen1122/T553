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

#ifndef UI_TEST_COUNTDOWN_H
#define UI_TEST_COUNTDOWN_H

#include "ui_test.h"
#include "components/ui_canvas.h"
#include "components/ui_scroll_view.h"
#include "components/ui_canvas_ext.h"
#include "components/ui_label_button.h"
#include "hal_tick.h"

namespace OHOS {
#if ENABLE_VGU_ENGINE && CAPABILITY_HWDRAW_PATH
enum class countStage {
    STAGE_GRADUAL_CIRCLE = 0,
    STAGE_COUNTING_DOWN,
    STAGE_FADE_OUT,
    STAGE_NONE
};

class CountdownAnimatorCallback : public AnimatorCallback {
public:
    explicit CountdownAnimatorCallback(UICanvasExt *canvas) : canvas_(canvas) {}
    ~CountdownAnimatorCallback() override {}
    void Callback(UIView *view) override;
    void NotifyStart();
    void SetInfo(UILabel* label, int counts);

private:
    void CountingDown();
    void UpdatePath(int16_t start, int16_t end, bool needDrawCircle);
    void UpdateLable(uint32_t curTime);
    void CircleFadeOut();
    void GradualCircle();
    UICanvasExt *canvas_ = nullptr;
    UILabel* label_ = nullptr;
    bool updateText_ = false;
    bool firstArc_ = false;
    countStage stage_ = countStage::STAGE_NONE;
    int16_t endAngle_ = 0;
    int16_t easeDegree_ = 0;
    uint16_t idleTime_ = 0;
    int16_t counts_ = 0;
    uint32_t lastTime_ = 0;
    uint32_t degreePerSecond_ = 0;
    uint32_t surplusDegree = 0;
};

class UITestCountdown : public UITest, public UIView::OnClickListener {
public:
    UITestCountdown() {}
    ~UITestCountdown() override {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;
    bool OnClick(UIView& view, const ClickEvent& event) override;

private:
    void InitCountdown(UICanvasExt* canvas);

    CountdownAnimatorCallback *callBack_ = nullptr;
    Animator *countdownAnimator_ = nullptr;
    UICanvasExt* canvas_ = nullptr;
    UILabelButton* restart_ = nullptr;
    UIViewGroup* container_ = nullptr;
    UILabel* label_ = nullptr;
};
#endif
}
#endif

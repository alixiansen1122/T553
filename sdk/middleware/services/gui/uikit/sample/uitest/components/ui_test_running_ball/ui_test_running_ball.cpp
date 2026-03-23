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

#include "ui_test_running_ball.h"

namespace OHOS {
#if ENABLE_VGU_ENGINE && CAPABILITY_HWDRAW_PATH
const static int16_t BUTTON_WIDTH = 50;
const static int16_t BUTTON_HEIGHT = 50;
const static int16_t CENTER_X = 50;
const static int16_t CENTER_Y = 100;
const static int16_t RADIUS = 30;
const static int16_t INTERVAL_STEP = 80;

void BallAnimatorCallback::Callback(UIView *view)
{
    if (!runningFlag_) {
        return;
    }
    canvas_->Clear();

    uint32_t temp = HALTick::GetInstance().GetTime();
    curTime_ = temp - startTimestamp_;
    if (curTime_ >= durationTime_) {
        runningFlag_ = false;
        curTime_ = durationTime_;
    }
    UpdateBall(curTime_);
}

void BallAnimatorCallback::UpdateBall(uint16_t curTime)
{
    PaintExt paint;
    paint.SetStyle(Paint::PaintStyle::FILL_STYLE);
    paint.SetOpacity(OPA_OPAQUE);
    paint.SetAntialiased(true);

    int16_t centerY = CENTER_Y;
    int16_t centerX = EasingEquation::SineEaseIn(startPos_, endPos_, curTime, durationTime_);
    paint.SetFillColor(Color::Blue());
    canvas_->DrawCircle({ centerX, centerY }, RADIUS, paint);

    centerY += INTERVAL_STEP;
    centerX = EasingEquation::QuadEaseInOut(startPos_, endPos_, curTime, durationTime_);
    paint.SetFillColor(Color::Red());
    canvas_->DrawCircle({ centerX, centerY }, RADIUS, paint);

    centerY += INTERVAL_STEP;
    centerX = EasingEquation::QuintEaseIn(startPos_, endPos_, curTime, durationTime_);
    paint.SetFillColor(Color::Yellow());
    canvas_->DrawCircle({ centerX, centerY }, RADIUS, paint);

    centerY += INTERVAL_STEP;
    paint.SetFillColor(Color::Green());
    centerX = EasingEquation::CubicEaseIn(startPos_, endPos_, curTime, durationTime_);
    canvas_->DrawCircle({ centerX, centerY }, RADIUS, paint);
}

void BallAnimatorCallback::NotifyStart()
{
    curTime_ = 0;
    runningFlag_ = true;
    startPos_ = CENTER_X;
    endPos_ = Screen::GetInstance().GetWidth() - BUTTON_WIDTH;
    startTimestamp_ = HALTick::GetInstance().GetTime();
}

void UITestRunningBall::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIViewGroup();
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    }

    if (canvas_ == nullptr) {
        canvas_ = new UICanvasExt();
    }
    canvas_->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    canvas_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    container_->Add(canvas_);

    if (restart_ == nullptr) {
        restart_ = new UILabelButton();
    }
    restart_->SetPosition(180, 410, 100, 40); // 180, 410, 100, 40: left, right, width, height
    restart_->SetText("Start");
    restart_->SetOnClickListener(this);
    container_->Add(restart_);
}

void UITestRunningBall::TearDown()
{
    if (ballAnimator_ != nullptr) {
        ballAnimator_->Stop();
        delete ballAnimator_;
        ballAnimator_ = nullptr;
    }
    if (callBack_ != nullptr) {
        delete callBack_;
        callBack_ = nullptr;
    }

    DeleteChildren(container_);
    container_ = nullptr;
    canvas_ = nullptr;
    restart_ = nullptr;
}

bool UITestRunningBall::OnClick(UIView& view, const ClickEvent& event)
{
    callBack_->NotifyStart();
    return true;
}

const UIView* UITestRunningBall::GetTestView()
{
    CaseInit();
    return container_;
}

void UITestRunningBall::CaseInit()
{
    PaintExt paint;
    paint.SetStyle(Paint::PaintStyle::FILL_STYLE);
    paint.SetOpacity(OPA_OPAQUE);
    paint.SetAntialiased(true);

    int16_t centerY = CENTER_Y;
    paint.SetFillColor(Color::Blue());
    canvas_->DrawCircle({ CENTER_X, centerY }, RADIUS, paint);

    centerY += INTERVAL_STEP;
    paint.SetFillColor(Color::Red());
    canvas_->DrawCircle({ CENTER_X, centerY }, RADIUS, paint);

    centerY += INTERVAL_STEP;
    paint.SetFillColor(Color::Yellow());
    canvas_->DrawCircle({ CENTER_X, centerY }, RADIUS, paint);

    centerY += INTERVAL_STEP;
    paint.SetFillColor(Color::Green());
    canvas_->DrawCircle({ CENTER_X, centerY }, RADIUS, paint);

    callBack_ = new BallAnimatorCallback(canvas_);
    if (callBack_ == nullptr) {
        GRAPHIC_LOGE("Tiger callBack new fail");
        return;
    }
    ballAnimator_ = new Animator(callBack_, canvas_, 0, true);
    if (ballAnimator_ == nullptr) {
        delete callBack_;
        callBack_ = nullptr;
        GRAPHIC_LOGE("tigerAnimator new fail");
        return;
    }
    ballAnimator_->Start();
}
#endif
}

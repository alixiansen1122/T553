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

#include "ui_test_countdown.h"
#include "gfx_utils/graphic_math.h"

namespace OHOS {
#if ENABLE_VGU_ENGINE && CAPABILITY_HWDRAW_PATH
const static int16_t CENTER_X = 227;
const static int16_t CENTER_Y = 200;
const static int16_t LABEL_X = 140;
const static int16_t LABEL_Y = 138;
const static int16_t RADIUS = 100;
const static int16_t STROKE_WIDTH = 30;
const static int16_t CIRCLE_BASE_WIDTH = 4;
const static uint32_t LIGHT_GREEN = 0xff007700;
static const uint16_t LABEL_FONT_SIZE = 100;
static const uint16_t ANIMATE_TIME = 500;
static const uint16_t SECOND_TO_MINISECOND = 1000;
static const uint16_t CIRCLE_FADE_OUT = 200;
static const uint16_t DEGREES_PER_CIRCLE = 360;

void CountdownAnimatorCallback::Callback(UIView *view)
{
    switch (stage_) {
        case countStage::STAGE_GRADUAL_CIRCLE: {
            GradualCircle();
            break;
        }
        case countStage::STAGE_COUNTING_DOWN: {
            CountingDown();
            break;
        }
        case countStage::STAGE_FADE_OUT: {
            CircleFadeOut();
            break;
        }
        default:
            break;
    }
}

void CountdownAnimatorCallback::CircleFadeOut()
{
    uint32_t diffTime = HALTick::GetInstance().GetTime() - lastTime_;
    if (diffTime < idleTime_) {
        return;
    }
    canvas_->Clear();

    /* Calculate stroke width */
    uint32_t curTime = diffTime - idleTime_;
    uint16_t width;
    if (curTime >= CIRCLE_FADE_OUT) {
        curTime = CIRCLE_FADE_OUT;
        stage_ = countStage::STAGE_NONE;
    }
    width = EasingEquation::QuadEaseInOut(STROKE_WIDTH, CIRCLE_BASE_WIDTH, curTime, CIRCLE_FADE_OUT);

    /* Draw circle */
    Point center = { CENTER_X, CENTER_Y };
    ColorType lightGreen;
    lightGreen.full = LIGHT_GREEN;
    PaintExt paint;
    paint.SetStyle(Paint::PaintStyle::STROKE_STYLE);
    paint.SetCapType(CAP_ROUND);
    paint.SetAntialiased(true);
    paint.SetStrokeColor(lightGreen);
    paint.SetStrokeWidth(width);
    canvas_->DrawCircle(center, RADIUS, paint);

    /* Update font size and transparent for label */
    uint16_t fontSize = LABEL_FONT_SIZE * width / STROKE_WIDTH;
    uint16_t opa = OPA_OPAQUE * width / STROKE_WIDTH;
    if (stage_ == countStage::STAGE_NONE) {
        opa = OPA_TRANSPARENT;
    }
    label_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, fontSize);
    label_->SetStyle(STYLE_TEXT_OPA, opa);
}

void CountdownAnimatorCallback::UpdateLable(uint32_t curTime)
{
    char buf[2] = {0}; // 2: buffer length
    uint16_t fontSize;
    uint16_t opa;

    if (curTime < ANIMATE_TIME / 2) { // 2: half time
        fontSize = LABEL_FONT_SIZE - LABEL_FONT_SIZE * curTime * 2 / ANIMATE_TIME; // 2: half time
        opa = OPA_OPAQUE * fontSize / LABEL_FONT_SIZE;
        if (firstArc_) {
            fontSize = fontSize / 2; // 2: half font size
        }
    } else {
        fontSize = LABEL_FONT_SIZE * curTime * 2 / ANIMATE_TIME - LABEL_FONT_SIZE; // 2: half time
        opa = OPA_OPAQUE * fontSize / LABEL_FONT_SIZE;
        if (updateText_) {
            if (sprintf_s(buf, sizeof(buf), "%u", (endAngle_ + easeDegree_) / degreePerSecond_) < 0) {
                return;
            }
            label_->SetText(buf);
            updateText_ = false;
        }
    }

    label_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, fontSize);
    label_->SetStyle(STYLE_TEXT_OPA, opa);
}

void CountdownAnimatorCallback::UpdatePath(int16_t start, int16_t end, bool needDrawCircle)
{
    Point center = { CENTER_X, CENTER_Y };
    ColorType lightGreen;
    lightGreen.full = LIGHT_GREEN;
    PaintExt paint;
    paint.SetStyle(Paint::PaintStyle::STROKE_STYLE);
    paint.SetStrokeWidth(STROKE_WIDTH);
    paint.SetCapType(CAP_ROUND);
    paint.SetAntialiased(true);
    paint.SetStrokeColor(lightGreen);

    canvas_->Clear();
    if (needDrawCircle) {
        canvas_->DrawCircle(center, RADIUS, paint);
    }

    if (start != end) {
        paint.SetStrokeColor(Color::Green());
        if (end - start == DEGREES_PER_CIRCLE) {
            canvas_->DrawCircle(center, RADIUS, paint);
        } else {
            canvas_->DrawArc(center, RADIUS, static_cast<float>(start), static_cast<float>(end), paint);
        }
    }
}

void CountdownAnimatorCallback::CountingDown()
{
    uint32_t diffTime = HALTick::GetInstance().GetTime() - lastTime_;
    if (diffTime >= SECOND_TO_MINISECOND) {
        lastTime_ = HALTick::GetInstance().GetTime();
        if (endAngle_ == DEGREES_PER_CIRCLE) {
            firstArc_ = true;
        } else {
            firstArc_ = false;
        }
        UpdatePath(0, endAngle_, true);
        UpdateLable(0);
        if (endAngle_ == 0) {
            stage_ = countStage::STAGE_FADE_OUT;
            idleTime_ = SECOND_TO_MINISECOND - CIRCLE_FADE_OUT;
            return;
        }
        easeDegree_ = degreePerSecond_;
        endAngle_ -= degreePerSecond_;
        if (endAngle_ <= static_cast<int16_t>(surplusDegree)) {
            endAngle_ = 0;
            easeDegree_ += surplusDegree;
        }
        updateText_ = true;
    } else if (diffTime >= idleTime_) {
        uint32_t curTime = diffTime - idleTime_;
        int16_t arcCurr = EasingEquation::QuadEaseInOut(0, easeDegree_, curTime, ANIMATE_TIME);
        UpdatePath(0, endAngle_ + easeDegree_ - arcCurr, true);
        UpdateLable(curTime);
    }
}

void CountdownAnimatorCallback::GradualCircle()
{
    uint32_t diffTime = HALTick::GetInstance().GetTime() - lastTime_;
    if (diffTime >= SECOND_TO_MINISECOND) {
        stage_ = countStage::STAGE_COUNTING_DOWN;
        idleTime_ = SECOND_TO_MINISECOND - ANIMATE_TIME;
    } else if (diffTime >= idleTime_) {
        uint32_t curTime = diffTime - idleTime_;
        int16_t arcCurr = EasingEquation::QuadEaseInOut(0, DEGREES_PER_CIRCLE,
            curTime, SECOND_TO_MINISECOND - idleTime_);
        UpdatePath(180, arcCurr + 180, false); // 180: first circle start from 180 degree
    }
}

void CountdownAnimatorCallback::SetInfo(UILabel* label, int counts)
{
    counts_ = counts;
    label_ = label;
}

void CountdownAnimatorCallback::NotifyStart()
{
    stage_ = countStage::STAGE_GRADUAL_CIRCLE;
    endAngle_ = DEGREES_PER_CIRCLE;
    idleTime_ = 200; // 200: idle time for first circle animate
    degreePerSecond_ = DEGREES_PER_CIRCLE / counts_;
    surplusDegree = DEGREES_PER_CIRCLE - degreePerSecond_ * counts_;
    lastTime_ = HALTick::GetInstance().GetTime();

    label_->SetText("Ready");
    label_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, LABEL_FONT_SIZE / 2); // 2: half font size
    label_->SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
}

void UITestCountdown::SetUp()
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

    if (label_ == nullptr) {
        label_ = new UILabel();
    }
    label_->SetPosition(LABEL_X, LABEL_Y, 180, 120); // 180, 120: width, height
    label_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    container_->Add(label_);

    if (restart_ == nullptr) {
        restart_ = new UILabelButton();
    }
    restart_->SetPosition(167, 350, 120, 60); // 167, 350, 120, 60: left, right, width, height
    restart_->SetText("Start");
    restart_->SetOnClickListener(this);
    container_->Add(restart_);
}

void UITestCountdown::TearDown()
{
    if (countdownAnimator_ != nullptr) {
        countdownAnimator_->Stop();
        delete countdownAnimator_;
        countdownAnimator_ = nullptr;
    }

    if (callBack_ != nullptr) {
        delete callBack_;
        callBack_ = nullptr;
    }

    DeleteChildren(container_);
    container_ = nullptr;
    canvas_ = nullptr;
    restart_ = nullptr;
    label_ = nullptr;
}

bool UITestCountdown::OnClick(UIView& view, const ClickEvent& event)
{
    callBack_->NotifyStart();
    return true;
}

const UIView* UITestCountdown::GetTestView()
{
    InitCountdown(canvas_);
    return container_;
}

void UITestCountdown::InitCountdown(UICanvasExt* canvas)
{
    PaintExt paint;
    ColorType lightGreen;
    lightGreen.full = LIGHT_GREEN;
    paint.SetStyle(Paint::PaintStyle::STROKE_STYLE);
    paint.SetStrokeWidth(CIRCLE_BASE_WIDTH);
    paint.SetAntialiased(true);
    paint.SetStrokeColor(lightGreen);
    canvas->DrawCircle({ CENTER_X, CENTER_Y }, RADIUS, paint);

    callBack_ = new CountdownAnimatorCallback(canvas);
    if (callBack_ == nullptr) {
        GRAPHIC_LOGE("CallBack new fail");
        return;
    }
    countdownAnimator_ = new Animator(callBack_, canvas, 0, true);
    if (countdownAnimator_ == nullptr) {
        delete callBack_;
        callBack_ = nullptr;
        GRAPHIC_LOGE("Animator new fail");
        return;
    }
    countdownAnimator_->Start();
    callBack_->SetInfo(label_, 5); // 5: countdown numbers
}
#endif
}

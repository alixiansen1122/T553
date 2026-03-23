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

#include "ui_test_draw_rect.h"
#include "hal_tick.h"

namespace OHOS {
const static uint16_t RECT_WIDTH = 200;
const static uint16_t RECT_HEIGHT = 200;

struct RectTest {
    uint8_t borderOpa;
    int16_t borderWidth;
    int16_t radius;
};

static RectTest g_rectTestList[32] = { // 32: testcase size
    {OPA_OPAQUE, 2, 7}, // 2, 7: borderWidth and radius
    {OPA_OPAQUE, 2, 10}, // 2, 10: borderWidth and radius
    {OPA_OPAQUE, 2, 20}, // 2, 20: borderWidth and radius
    {OPA_OPAQUE, 2, 30}, // 2, 30: borderWidth and radius
    {OPA_OPAQUE, 2, 40}, // 2, 40: borderWidth and radius
    {OPA_OPAQUE, 2, 50}, // 2, 50: borderWidth and radius
    {OPA_OPAQUE, 1, 50}, // 1, 50: borderWidth and radius
    {OPA_OPAQUE, 3, 50}, // 3, 50: borderWidth and radius
    {OPA_OPAQUE, 4, 50}, // 4, 50: borderWidth and radius
    {OPA_OPAQUE, 48, 50}, // 48, 50: borderWidth and radius
    {OPA_OPAQUE, 49, 50}, // 49, 50: borderWidth and radius
    {OPA_OPAQUE, 50, 50}, // 50, 50: borderWidth and radius
    {OPA_OPAQUE, 50, 100}, // 50, 100: borderWidth and radius
    {OPA_OPAQUE, 0, 7}, // 0, 7: borderWidth and radius
    {OPA_OPAQUE, 0, 50}, // 0, 50: borderWidth and radius
    {OPA_OPAQUE, 0, 100}, // 0, 100: borderWidth and radius
    {OPA_50, 2, 7}, // 2, 7: borderWidth and radius
    {OPA_50, 2, 10}, // 2, 10: borderWidth and radius
    {OPA_50, 2, 20}, // 2, 20: borderWidth and radius
    {OPA_50, 2, 30}, // 2, 30: borderWidth and radius
    {OPA_50, 2, 40}, // 2, 40: borderWidth and radius
    {OPA_50, 2, 50}, // 2, 50: borderWidth and radius
    {OPA_50, 1, 50}, // 1, 50: borderWidth and radius
    {OPA_50, 3, 50}, // 3, 50: borderWidth and radius
    {OPA_50, 4, 50}, // 4, 50: borderWidth and radius
    {OPA_50, 48, 50}, // 48, 50: borderWidth and radius
    {OPA_50, 49, 50}, // 49, 50: borderWidth and radius
    {OPA_50, 50, 50}, // 50, 50: borderWidth and radius
    {OPA_50, 50, 100}, // 50, 100: borderWidth and radius
    {OPA_50, 0, 7}, // 0, 7: borderWidth and radius
    {OPA_50, 0, 50}, // 0, 50: borderWidth and radius
    {OPA_50, 0, 100}, // 0, 100: borderWidth and radius
};

void RectAnimatorCallback::Callback(UIView *view)
{
    if (lastRunTime_ == 0) {
        lastRunTime_ = HALTick::GetInstance().GetTime();
    }
    if (HALTick::GetInstance().GetElapseTime(lastRunTime_) > 3000) { // 3000: 3s, time elaps
        view->Invalidate();
        view->SetStyle(STYLE_BORDER_OPA, g_rectTestList[curIndex_].borderOpa);
        view->SetStyle(STYLE_BORDER_WIDTH, g_rectTestList[curIndex_].borderWidth);
        view->SetStyle(STYLE_BORDER_RADIUS, g_rectTestList[curIndex_].radius);
        curIndex_++;
        printf("Test Rect[%u]: borderOpa = %d, borderWidth = %u, radius = %d\n",
            curIndex_, g_rectTestList[curIndex_].borderOpa,
            g_rectTestList[curIndex_].borderWidth, g_rectTestList[curIndex_].radius);
        if (curIndex_ == 32) { // 32: testcase size
            curIndex_ = 0;
        }
        view->Invalidate();
        lastRunTime_ = HALTick::GetInstance().GetTime();
    }
}

void UITestDrawRect::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->SetThrowDrag(true);
        container_->SetHorizontalScrollState(false);
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    }

    if (rect_ == nullptr) {
        rect_ = new UIView();
    }
    rect_->SetPosition(100, 100);  // 100 : x y position
    rect_->Resize(RECT_WIDTH, RECT_HEIGHT);
    rect_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    rect_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Blue().full);
    rect_->SetStyle(STYLE_BORDER_COLOR, Color::Red().full);
    container_->Add(rect_);

    if (callback_ == nullptr) {
        callback_ = new RectAnimatorCallback(rect_);
    }
    rectAnimator_ = callback_->GetAnimator();
}

void UITestDrawRect::TearDown()
{
    DeleteChildren(container_);
    container_ = nullptr;
    rect_ = nullptr;
    if (callback_ != nullptr) {
        delete callback_;
        callback_ = nullptr;
    }
}

const UIView* UITestDrawRect::GetTestView()
{
    rectAnimator_->Start();
    return container_;
}
}

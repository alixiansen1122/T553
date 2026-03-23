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

#include "ui_test_sweepclock.h"
#include "components/root_view.h"

namespace OHOS {
void UITestSweepClock::SetUp()
{
    if (clockView == nullptr) {
        clockView = new MainClockView();
        if (clockView != nullptr) {
            clockView->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
            clockView->SetStyle(STYLE_BACKGROUND_COLOR, Color::Yellow().full);
            clockView->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
            clockView->InitView();
        }
    }
}

void UITestSweepClock::TearDown()
{
    if (clockView != nullptr) {
        delete clockView;
        clockView = nullptr;
    }
}

const UIView *UITestSweepClock::GetTestView()
{
    return clockView;
}
}

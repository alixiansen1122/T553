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

#include "ui_test_compass.h"
#include "components/root_view.h"

namespace OHOS {
void UITestCompass::SetUp()
{
    if (compassView == nullptr) {
        compassView = new CompassControlView();
        if (compassView != nullptr) {
            compassView->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
            compassView->SetStyle(STYLE_BACKGROUND_COLOR, Color::Yellow().full);
            compassView->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
            compassView->InitView();
        }
    }
}

void UITestCompass::TearDown()
{
    if (compassView != nullptr) {
        delete compassView;
        compassView = nullptr;
    }
}

const UIView *UITestCompass::GetTestView()
{
    return compassView;
}
}

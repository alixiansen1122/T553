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

#include "ui_test_tiger/ui_test_tiger.h"
#include "components/root_view.h"

namespace OHOS {
void UITestTiger::SetUp()
{
    if (tigerView == nullptr) {
        tigerView = new TigerView();
        tigerView->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
        tigerView->SetThrowDrag(true);
        tigerView->SetStyle(STYLE_BACKGROUND_COLOR, 0xFFC8C8C8); // R,G,B are all set to 200.
        tigerView->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
        tigerView->InitView();
    }
}

void UITestTiger::TearDown()
{
    delete tigerView;
    tigerView = nullptr;
}

const UIView *UITestTiger::GetTestView()
{
    return tigerView;
}
}

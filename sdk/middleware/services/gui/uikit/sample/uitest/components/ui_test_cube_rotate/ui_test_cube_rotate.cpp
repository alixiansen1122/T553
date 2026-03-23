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

#include "ui_test_cube_rotate/ui_test_cube_rotate.h"
#include "components/root_view.h"

namespace OHOS {
void UITestCubeRotate::SetUp()
{
    if (cubeView == nullptr) {
        cubeView = new CubeRotateView();
        cubeView->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
        cubeView->SetStyle(STYLE_BACKGROUND_COLOR, 0xFFC8C8C8); // R,G,B are all set to 200.
        cubeView->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
        cubeView->InitView();
    }
}

void UITestCubeRotate::TearDown()
{
    delete cubeView;
    cubeView = nullptr;
}

const UIView *UITestCubeRotate::GetTestView()
{
    return cubeView;
}
}

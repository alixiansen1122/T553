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

#ifndef UI_TEST_CUBE_ROTATE_H
#define UI_TEST_CUBE_ROTATE_H

#include "ui_test.h"
#include "cube_rotate_view.h"

namespace OHOS {
class UITestCubeRotate : public UITest {
public:
    UITestCubeRotate() {}
    ~UITestCubeRotate() override {}
    void SetUp() override;
    void TearDown() override;
    const UIView *GetTestView() override;

private:
    CubeRotateView* cubeView = nullptr;
};
}
#endif // UI_TEST_CUBE_ROTATE_H
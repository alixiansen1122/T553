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

#ifndef UI_TEST_IMAGE_FPS_H
#define UI_TEST_IMAGE_FPS_H

#include "components/ui_image_animator.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "layout/grid_layout.h"
#include "ui_test.h"
#include "gfx_utils/sys_info.h"

namespace OHOS {
class UITestImageFPS : public UITest, public SysInfo::OnFPSChangedListener {
public:
    UITestImageFPS() : container_(nullptr), imageAnimator_(nullptr), fpsLabel_(nullptr),
                       fpsBuf_(nullptr), typeLabel_(nullptr), titleLabel_(nullptr) {}

    ~UITestImageFPS() override {}
    void SetUp() override;
    void TearDown() override;
    UIView* GetTestView() override;
    void OnFPSChanged(float newFPS) override;
    UIScrollView* container_;

private:
    void SetUpButton(UILabelButton* btn, const char* title);
    void SetUpLabel(UILabel* label, const char* title);
    void LoadImg(void);
    void ReleaseImg(void);
    UILabel* titleLabel_;
    UILabel* typeLabel_;
    UILabel* fpsLabel_;
    char* fpsBuf_;
    UIImageAnimatorView* imageAnimator_;
};
} // namespace OHOS
#endif // UI_TEST_IMAGE_FPS_H

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

#ifndef UI_TEST_SVG_H
#define UI_TEST_SVG_H

#include "graphic_config.h"
#include "ui_test.h"
#include "components/ui_canvas_ext.h"
#include "components/ui_label_button.h"
#include "components/ui_view_group.h"

namespace OHOS {
class UITestSVG : public UITest, public UIView::OnClickListener {
public:
    UITestSVG() {}
    ~UITestSVG() override {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;

    virtual bool OnClick(UIView& view, const ClickEvent& event) override
    {
        if (canvas_ != nullptr) {
            Matrix3<float> mat3(1, 0, 0, 0, 1, 0, 0, 100, 1); // 100: Y trans by 100
            canvas_->SetSvgMatrix(mat3, index_);
        }
        return true;
    }
private:
    UIViewGroup* group_ = nullptr;
    UICanvasExt* canvas_ = nullptr;
    UILabelButton* btn_ = nullptr;
    int16_t index_ = 0;
};
}
#endif // UI_TEST_SVG_H
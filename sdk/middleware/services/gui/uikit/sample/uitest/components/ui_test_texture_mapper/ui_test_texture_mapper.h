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

#ifndef UI_TEST_TEXTURE_MAPPER_H
#define UI_TEST_TEXTURE_MAPPER_H

#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "components/ui_texture_mapper.h"
#include "ui_test.h"

namespace OHOS {
class UITestTextureMapper : public UITest, public UIView::OnClickListener {
public:
    UITestTextureMapper() {}
    ~UITestTextureMapper() override {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;
    bool OnClick(UIView& view, const ClickEvent& event) override;

private:
    UIScrollView* container_ = nullptr;
    UITextureMapper* textureMapper_ = nullptr;

    UILabelButton* startBtn_ = nullptr;
    UILabelButton* resetBtn_ = nullptr;
    UILabelButton* cancelBtn_ = nullptr;
    UILabelButton* ani1Btn_ = nullptr;
    UILabelButton* ani2Btn_ = nullptr;
    UILabelButton* ani3Btn_ = nullptr;
    UILabelButton* ani4Btn_ = nullptr;

    void SetUpButton(UILabelButton* btn, uint16_t x, uint16_t y, const char* title);
    void TestNegativeScale();
    void TestPositionScale();
    void TestRotateWithPivot();
    void TestComplex();
};
} // namespace OHOS
#endif // UI_TEST_TEXTURE_MAPPER_H

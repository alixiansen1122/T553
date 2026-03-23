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

#ifndef UI_TEST_MESSAGE_H
#define UI_TEST_MESSAGE_H

#include "ui_test.h"
#include "common/screen.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_image_view.h"
#include "hal_tick.h"

namespace OHOS {
class UITestMessage : public UITest, public UIView::OnClickListener {
public:
    UITestMessage() {}
    ~UITestMessage() override {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;
    bool OnClick(UIView& view, const ClickEvent& event) override;

private:
    void SetupBtn(int16_t x, int16_t y, const std::string &title, UILabelButton *&btn);
    UIViewGroup* container_ = nullptr;
    UILabel* msgLabel_ = nullptr;
    UILabel* under_ = nullptr;
    UILabelButton* fgColor_ = nullptr;
    UILabelButton* addBlank_ = nullptr;
    UILabelButton* labelMode_ = nullptr;
    UILabelButton* cacheEnable_ = nullptr;
    UILabelButton* add_ = nullptr;
    UILabelButton* reduce_ = nullptr;
    UILabelButton* clear_ = nullptr;
    UILabel* valueLabel_ = nullptr;
    float blurVaule_ = 0.0f;
    uint32_t fgColors_[2] = {Color::Green().full, Color::Yellow().full}; // 2: num
    uint8_t fgIndex_ = 0;
    uint8_t blankSapce_ = 10;
};
}
#endif
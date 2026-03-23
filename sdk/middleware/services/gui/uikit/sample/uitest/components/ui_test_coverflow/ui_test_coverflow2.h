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

#ifndef UI_TEST_COVERFLOW_VIEW_2_H
#define UI_TEST_COVERFLOW_VIEW_2_H

#include "components/ui_coverflow_view2.h"
#include "components/ui_scroll_view.h"
#include "components/ui_label.h"
#include "ui_test.h"

namespace OHOS {
class UITestCoverflow2 : public UITest,
    public UICoverFlowView2::OnCoverflowScrollListener, public UIView::OnClickListener {
public:
    UITestCoverflow2() {}
    ~UITestCoverflow2() override {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;
    /**
     * @brief Test display UICoverflow
     */
    void UITestCoverflow2001();

protected:
    bool OnScroll(uint16_t distance) override;
    bool OnClick(UIView& view, const ClickEvent& event) override;

private:
    void CreateAddButton();
    void CreateTitleLabel();

    UIScrollView* container_ = nullptr;
    UICoverFlowView2* coverflow_ = nullptr;
    UILabel* label_ = nullptr;
    UIImageView* addImage_ = nullptr;
    uint16_t lastIndex_ = 0;
};
} // namespace OHOS
#endif // UI_TEST_COVERFLOW_VIEW_2_H

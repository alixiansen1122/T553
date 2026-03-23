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

#ifndef UI_TEST_COVERFLOW_VIEW_H
#define UI_TEST_COVERFLOW_VIEW_H

#include "components/ui_coverflow_view.h"
#include "components/ui_scroll_view.h"
#include "components/ui_label_button.h"
#include "animator/animator.h"
#include "ui_test.h"

namespace OHOS {
class UITestCoverflow : public UITest,
    public UICoverflowView::OnCoverflowScrollListener, public UIView::OnClickListener {
public:
    UITestCoverflow() {}
    ~UITestCoverflow() override {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;
    /**
     * @brief Test display UICoverflow
     */
    void UITestCoverflowView001();

    bool OnScroll() override;

    bool OnClick(UIView& view, const ClickEvent& event) override;
private:
    void InitButtonGroup();
    void CreateLabelButton(UILabelButton** btn, const char* tile, int16_t x, int16_t y);

    UIScrollView* container_ = nullptr;
    UICoverflowView* coverflow_ = nullptr;
    UILabel* label_ = nullptr;
    uint16_t lastIndex_ = 0;
    UIButton* showBtn_ = nullptr;
    bool isShowBtnGrp_ = false;
    UIViewGroup* btnGrp_ = nullptr;
    uint8_t loadNum_ = 0;
    UILabelButton* addBtn_ = nullptr;
    UILabelButton* removeBtn_ = nullptr;
    UILabelButton* imgWidthAddBtn_ = nullptr;
    UILabelButton* imgWidthSubBtn_ = nullptr;
    UILabelButton* imgPaddingAddBtn_ = nullptr;
    UILabelButton* imgPaddingSubBtn_ = nullptr;
    UILabelButton* maxAngleAddBtn_ = nullptr;
    UILabelButton* maxAngleSubBtn_ = nullptr;
    UILabelButton* loopBtn_ = nullptr;
    uint16_t imgWidth_ = 200;  // 200: default value
    uint16_t imgPadding_ = 50;  // 50: default value
    uint8_t mirrorOpa_ = 30;    // 30: default value
    float rotateAngle_ = 70.0f; // 70: default value
};
} // namespace OHOS
#endif // UI_TEST_QRCODE_H

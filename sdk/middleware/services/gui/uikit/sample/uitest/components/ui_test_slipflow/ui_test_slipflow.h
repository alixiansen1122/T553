/*
 * Copyright (c) CompanyNameMagicTag 2024. All rights reserved.
 * Description: UITestSlip
 * Author:
 * Create: 2024-08
 */

#ifndef UI_TEST_SLIPFLOW_VIEW_H
#define UI_TEST_SLIPFLOW_VIEW_H

#include "components/ui_slipflow_view.h"
#include "components/ui_scroll_view.h"
#include "components/ui_label_button.h"
#include "animator/animator.h"
#include "ui_test.h"

namespace OHOS {
class UITestSlipflow : public UITest,
    public UISlipflowView::OnSlipflowScrollListener {
public:
    UITestSlipflow() {}
    ~UITestSlipflow() override {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;
    /**
     * @brief Test display UISlipflow
     */
    void UITestSlipflowView();
    void UITestSlipflowViewInitFlow();

    bool OnScroll(int16_t distance) override;
    void OnScrollUpStart() override;
    void OnScrollUpEnd() override;
    void OnRefreshPageOpaScale(UIView* view, int16_t xOffset, int16_t yOffset) override;

    bool OnRemove(UIView* view) override;
private:
    void InitButtonGroup();
    void CreateLabelButton(UILabelButton** btn, const char* tile, int16_t x, int16_t y);

    UIScrollView* container_ = nullptr;
    UISlipflowView* slipflow_ = nullptr;
    UILabel* label_ = nullptr;
    UILabel* slipUpNoticelabel_ = nullptr;
    uint16_t lastIndex_ = 0;
    uint8_t loadNum_ = 0;
    uint16_t imgWidth_ = 200;  // 200: default value
    uint16_t imgPadding_ = 50;  // 50: default value
    uint8_t mirrorOpa_ = 30;    // 30: default value
    float rotateAngle_ = 70.0f; // 70: default value
};
} // namespace OHOS
#endif // UI_TEST_SLIPFLOW_VIEW_H

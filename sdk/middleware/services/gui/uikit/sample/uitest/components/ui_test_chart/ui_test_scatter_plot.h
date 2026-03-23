/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: UITestScatterPlot
 * Author: Hisi Graphic Team
 * Created: 2025-9
 */

#ifndef UI_TEST_SCATTER_PLOT_H
#define UI_TEST_SCATTER_PLOT_H

#include "components/ui_chart.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "components/ui_scatter_plot.h"
#include "ui_test.h"

namespace OHOS {
class UITestScatterPlot : public UITest, public UIView::OnClickListener {
public:
    UITestScatterPlot() {}
    ~UITestScatterPlot() override {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;

    void SetUpButton(UILabelButton* btn, const char* title);
    bool OnClick(UIView& view, const ClickEvent& event) override;

    void TestAddDataSerial();
    void TestReverse();
    void TestGradientBottom();
    void TestAddPoints();

private:
    void InnerDeleteChildren(UIView* view) const;
    void SetLastPos(UIView* view);
    static constexpr uint8_t DATA_NUM = 3;
    UIScrollView* container_ = nullptr;
    UIScatterPlot* chart_ = nullptr;
    UIScatterPlotDataSerial *dataSerial_[DATA_NUM] = {0};
    uint8_t curDataIndex_ = 0;

    UILabelButton* addDataSerialBtn_ = nullptr;
    UILabelButton* deleteDataSerialBtn_ = nullptr;
    UILabelButton* clearDataSerialBtn_ = nullptr;
    UILabelButton* reverseBtn_ = nullptr;
    UILabelButton* topPointBtn_ = nullptr;
    UILabelButton* bottomPointBtn_ = nullptr;
    UILabelButton* headPointBtn_ = nullptr;
    UILabelButton* addPointsBtn_ = nullptr;

    int16_t lastX_ = 0;
    int16_t lastY_ = 0;
};
} // namespace OHOS
#endif // UI_TEST_SCATTER_PLOT_H
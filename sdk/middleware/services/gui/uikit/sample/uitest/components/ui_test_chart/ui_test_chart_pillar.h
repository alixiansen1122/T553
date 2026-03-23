/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#ifndef UI_TEST_CHART_PILLAR_H
#define UI_TEST_CHART_PILLAR_H

#include "components/ui_chart_pillar_ext.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "ui_test.h"

namespace OHOS {
class UITestChartPillar : public UITest, public UIView::OnClickListener {
public:
    UITestChartPillar() : dataSerial_() {}
    ~UITestChartPillar() override {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;

    void SetUpButton(UILabelButton* btn, const char* title);

    bool OnClick(UIView& view, const ClickEvent& event) override;

    void UiKitChartPillarTestAddDataSerial001();
    void UiKitChartPillarTestEnableReverse002();
    void UiKitChartPillarTestSetAxisLineColor003();
    void UiKitChartPillarTestSetAxisLineVisible004();
    void UiKitChartPillarTestSwitchCap005();
    void UiKitChartPillarTestSwitchPillarType006();

private:
    void SetLastPos(UIView* view);
    void InnerDeleteChildren(UIView* view) const;
    static constexpr uint8_t DATA_NUM = 3;
    UIChartDataSerial *dataSerial_[DATA_NUM];
    uint8_t curDataIndex_ = 0;
    bool reverseState_ = false;

    UIScrollView* container_ = nullptr;
    UIChartPillarExt* chart_ = nullptr;
    UILabelButton* addDataSerialBtn_ = nullptr;
    UILabelButton* deleteDataSerialBtn_ = nullptr;
    UILabelButton* clearDataSerialBtn_ = nullptr;
    UILabelButton* reverseBtn_ = nullptr;
    UILabelButton* setAxisColorBtn_ = nullptr;
    UILabelButton* setAxisVisibleBtn_ = nullptr;
    UILabelButton* switchCapBtn_ = nullptr;
    UILabelButton* switchPillarTypeBtn_ = nullptr;

    int16_t lastX_ = 0;
    int16_t lastY_ = 0;
};
} // namespace OHOS
#endif // UI_TEST_CHART_PILLAR_H

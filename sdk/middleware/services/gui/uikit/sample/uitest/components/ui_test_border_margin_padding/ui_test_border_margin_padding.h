/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: Lottie
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */
#ifndef UI_TEST_BORDER_MARGIN_PADDING_H
#define UI_TEST_BORDER_MARGIN_PADDING_H

#include "ui_test_border_listener.h"
#include "components/text_adapter.h"
#include "components/ui_chart.h"
#include "components/ui_label.h"
#include "components/ui_picker.h"
#include "components/ui_scroll_view.h"
#include "components/ui_toggle_button.h"
#include "layout/grid_layout.h"
#include "layout/list_layout.h"
#include "ui_test.h"

namespace OHOS {
class UITestBorderMarginPadding : public UITest {
public:
    UITestBorderMarginPadding()
    {
        style_ = StyleDefault::GetDefaultStyle();
        style_.bgOpa_ = OPA_OPAQUE;
        style_.bgColor_ = Color::Gray();
    }
    virtual ~UITestBorderMarginPadding() {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;
    void ReloadTest();
    void InitUIToggleButton();
    void InitAdapterData();
    void UIKitUITestBorderMarginPaddingTest001();
    void UIKitUITestBorderMarginPaddingTest002();
    void UIKitUITestBorderMarginPaddingTest003();
    void UIKitUITestBorderMarginPaddingTest004();
    void UIKitUITestBorderMarginPaddingTest005();
    void UIKitUITestBorderMarginPaddingTest006();
    void UIKitUITestBorderMarginPaddingTest007();
    void UIKitUITestBorderMarginPaddingTest008();
    void UIKitUITestBorderMarginPaddingTest009();
    void UIKitUITestBorderMarginPaddingTest010();
    void UIKitUITestBorderMarginPaddingTest011();
    void UIKitUITestBorderMarginPaddingTest012();
    void UIKitUITestBorderMarginPaddingTest013();
    void UIKitUITestBorderMarginPaddingTest014();
    void UIKitUITestBorderMarginPaddingTest015();
    void UIKitUITestBorderMarginPaddingTest016();
    void UIKitUITestBorderMarginPaddingTest017();
    void UIKitUITestBorderMarginPaddingTest018();
    Style style_;

private:
    const int16_t BUTTON_GROUP_WIDTH = 200;
    UIScrollView* scroll_ = nullptr;
    GridLayout* layoutButton_ = nullptr;
    ListLayout* listScroll_ = nullptr;
    UIViewGroup* container_ = nullptr;
    MarginListener* marginListener_ = nullptr;
    BorderListener* borderListener_ = nullptr;
    PaddingListener* paddingListener_ = nullptr;
    UIChartPolyline* chart_ = nullptr;
    UIChartDataSerial* dataSerial_ = nullptr;
    TextAdapter* adapter_ = nullptr;
    List<const char*>* adapterData_ = nullptr;
    UIPicker* picker_ = nullptr;

    void AddTitle(const char* text);
};
} // namespace OHOS
#endif // UI_TEST_BORDER_MARGIN_PADDING_H

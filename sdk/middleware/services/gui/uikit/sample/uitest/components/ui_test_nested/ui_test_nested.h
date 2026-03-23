/*
 * Copyright (c) CompanyNameMagicTag 2024. All rights reserved.
 * Description: UITestNested
 * Author:
 * Create: 2024-12-09
 */

#ifndef UI_TEST_NESTED_H
#define UI_TEST_NESTED_H
#include "ui_test.h"
#include "common/screen.h"
#include "components/ui_label.h"
#include "components/ui_list_nested.h"
#include "components/ui_swipe_view_nested.h"
#include "components/ui_scroll_view_nested.h"
#include "components/text_adapter.h"

namespace OHOS {
class UITestNested : public UITest {
public:
    UITestNested() {}
    ~UITestNested() override {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;

private:
    UIListNested *list1_ = nullptr;
    UIListNested *list2_ = nullptr;
    UISwipeViewNested *swipe_ = nullptr;
    UIScrollViewNested *scroll_ = nullptr;
    UIViewGroup *container_ = nullptr;
    UILabel *label_ = nullptr;
    List<const char*> data1_;
    List<const char*> data2_;
    TextAdapter adp1_;
    TextAdapter adp2_;
}; // class UITestNested
} // namespace OHOS

#endif // UI_TEST_NESTED_H

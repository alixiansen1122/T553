/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: List with built-in adapter sample
 * Author: Hisi Team
 * Created: 2025-07
 */
#ifndef UI_TEST_SIMPLE_LIST_H
#define UI_TEST_SIMPLE_LIST_H

#include "components/ui_simple_list.h"
#include "ui_test.h"

namespace OHOS {
class UITestSimpleList : public UITest, UIView::OnClickListener, ListScrollListener {
public:
    UITestSimpleList() : list_(nullptr) {}
    ~UITestSimpleList() override {}
    void SetUp() override;
    void TearDown() override;
    UIView* GetTestView() override;

private:
    UISimpleList* list_;
    List<Contents*> data_;
};
} // namespace OHOS
#endif // UI_TEST_LIST_FPS_H


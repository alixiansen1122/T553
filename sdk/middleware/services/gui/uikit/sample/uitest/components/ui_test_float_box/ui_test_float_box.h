/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: UITestFloatBoxView
 * Author: Hisi Graphic Team
 * Created: 2025-8
 */

#ifndef UI_TEST_FLOAT_BOX_H
#define UI_TEST_FLOAT_BOX_H

#include "ui_test.h"
#include "components/ui_float_box_view.h"
#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "components/ui_view_group.h"

namespace OHOS {
class UITestFloatBox : public UITest, public UIView::OnClickListener, public UIFloatBoxView::OnItemClickListener {
public:
    UITestFloatBox() {}
    ~UITestFloatBox() override {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;
    void OnItemClick(const std::string& name) override;
    bool OnClick(UIView& view, const ClickEvent& event) override;

private:
    void LoadItemImgs();
    UIScrollView* container_ = nullptr;
    UIFloatBoxView* floatBox_ = nullptr;
    UILabelButton* switchBtn_ = nullptr;
};
}
#endif
/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: Lottie
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */

#ifndef UI_TEST_VIEW_GROUP_H
#define UI_TEST_VIEW_GROUP_H

#include <cstdlib>
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "components/ui_view_group.h"
#include "ui_test.h"

namespace OHOS {
class UITestViewGroup : public UITest, public UIView::OnClickListener {
public:
    UITestViewGroup() : container_(nullptr) {}
    ~UITestViewGroup() {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;
    bool OnClick(UIView& view, const ClickEvent& event) override;

    /**
     * @brief Test Add/Rmove Function
     */
    void UIKitViewGroupTestAddRemove001();

    /**
     * @brief Test add child multi time
     */
    void UIKitViewGroupTestAddError001();

    /**
     * @brief Test insert child multi time
     */
    void UIKitViewGroupTestInsertError001();

private:
    UIViewGroup* CreateTestCaseGroup() const;
    UILabel* CreateTitleLabel() const;
    UILabelButton* CreateButton(std::string text, int16_t width, int16_t height) const;
    void AddView();
    void RemoveView();
    void RemoveAndAddView();
    void AddMultiParent();
    void InsertMultiParent();

    UIScrollView* container_ = nullptr;
    UILabelButton* addBtn_ = nullptr;
    UILabelButton* removeBtn_ = nullptr;
    UILabelButton* removeAddBtn_ = nullptr;
    UILabelButton* addTwiceBtn_ = nullptr;
    UILabelButton* addMultiParentBtn_ = nullptr;
    UILabelButton* addSelfBtn_ = nullptr;
    UILabelButton* insertTwiceBtn_ = nullptr;
    UILabelButton* insertSelfBtn_ = nullptr;
    UILabelButton* insertMultiParentBtn_ = nullptr;
};
} // namespace OHOS
#endif // UI_TEST_VIEW_GROUP_H

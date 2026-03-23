/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: TestCase
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */

#ifndef UI_TEST_VIEW_BOUNDS_H
#define UI_TEST_VIEW_BOUNDS_H

#include "graphic_config.h"

#if ENABLE_DEBUG
#include "components/ui_scroll_view.h"
#include "components/ui_toggle_button.h"
#include "components/ui_label.h"
#include "ui_test.h"

namespace OHOS {
class UITestViewBounds : public UITest {
public:
    UITestViewBounds() {}
    ~UITestViewBounds();
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;

    /**
     * @brief test show or hide view bounds
     *
     */
    void UIKitViewBoundsSetState001();

    /**
     * @brief test view bounds with margin
     *
     */
    void UIKitViewBoundsMargin001();

    /**
     * @brief test view bounds with padding
     *
     */
    void UIKitViewBoundsPadding001();

    /**
     * @brief test view bounds with border
     *
     */
    void UIKitViewBoundsBorder001();

    /**
     * @brief test view bounds with margin/padding/border
     *
     */
    void UIKitViewBounds001();

    /**
     * @brief test small view which width or height small than 20px
     *
     */
    void UIKitViewBoundsSmallView001();

    /**
     * @brief test view which is invisible
     *
     */
    void UIKitViewBoundsInvisible001();

    /**
     * @brief test child view override OnPostDraw
     *
     */
    void UIKitViewBoundsPostDraw001();

private:
    UILabel* CreateTitleLabel() const;
    UIScrollView* container_ = nullptr;
    UICheckBox::OnChangeListener* viewBoundsChangeListener_ = nullptr;
};
} // namespace OHOS
#endif // ENABLE_DEBUG
#endif // UI_TEST_VIEW_BOUNDS_H

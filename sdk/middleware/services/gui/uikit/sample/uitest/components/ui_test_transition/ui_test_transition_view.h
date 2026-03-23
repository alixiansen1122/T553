/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: UITestTransitionView
 * Create: 2025-04
 */

#ifndef UI_TEST_TRANSITION_VIEW_H
#define UI_TEST_TRANSITION_VIEW_H

#include "common/screen.h"
#include "components/ui_image_view.h"
#include "components/ui_scroll_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_list.h"
#include "components/ui_view.h"
#include "core/render_manager.h"
#include "ui_test.h"
#include "ui_test_transition_adapter.h"
#include "ui_test_transition_itemview.h"

namespace OHOS {
    class UITestTransitionView : public UITest, UIView::OnClickListener, ListScrollListener,
            public UIView::OnDragListener {
    public:
        UITestTransitionView() {}
        ~UITestTransitionView() override;
        void SetUp() override;
        void TearDown() override;
        const UIView* GetTestView() override;
        bool OnClick(UIView& view, const ClickEvent& event) override;
        void OnItemSelected(int16_t index, UIView *view) override;
    private:
        UIScrollView *scrollView_{nullptr};
        UIList *contentList_{nullptr};
        UILabel titleText_;
        UITestTransitionAdapter *listAdapter_{nullptr};
        UITestTransitionItemView *itemViewSelected_{nullptr};
        UITestTransitionItemView *preItemViewSelected_{nullptr};
    };
}
#endif // UI_TEST_TRANSITION_VIEW_H
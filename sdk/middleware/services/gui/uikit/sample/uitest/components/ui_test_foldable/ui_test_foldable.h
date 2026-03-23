/*
 * Copyright (c) CompanyNameMagicTag 2025. All rights reserved.
 * Description: UITestFoldable
 * Author:
 * Create: 2025-09
 */

#ifndef UI_TEST_FOLDABLE_H
#define UI_TEST_FOLDABLE_H

#include "components/ui_foldable_view.h"
#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "ui_test.h"

namespace OHOS {
class UITestFoldable : public UITest,
    public UIFoldableView::OnFoldableViewEventListener, public UIView::OnDragListener,
    public UIView::OnClickListener, public UIView::OnLongPressListener {
public:
    UITestFoldable() {}
    ~UITestFoldable() override {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;
    /**
     * @brief Test display UIFoldable
     */
    void UITestFoldableView();

    void OnReLayoutInFoldMode() override;
    void OnReLayoutInFlatMode() override;
    void OnEntranceAnimatorStop() override;
    void OnSlideAnimatorStop() override;
    void OnMoveChildAnimatorStop() override;
    void OnRemoveChildAnimatorStop() override;
    void OnSwitchingLayout(UIFoldableView::LayoutMode targetMode, float progress) override;
    void OnSwitchLayoutAnimatorStop() override;

    bool OnDragStart(UIView& view, const DragEvent& event) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnDragEnd(UIView& view, const DragEvent& event) override;
    bool OnClick(UIView& view, const ClickEvent& event) override;
    bool OnLongPress(UIView& view, const LongPressEvent& event) override;
private:
    void InitDesktopPage();
    void InitApplistPage();
    void InitFlodableView();
    void InitFlodableViewCards();
    void InitAddPageCard();
    void InitEditCard();
    void InitEditMiniButtons();
    void DeinitEditMiniButtons();
    void InitEditFinishButton();
    bool OnClickInFoldMode(UIView& view, const ClickEvent& event);
    bool OnClickInFlatMode(UIView& view, const ClickEvent& event);
    void PinPageOnTop(int16_t id);
    void CancelPinPageOnTop(int16_t id);
    void DeletePage(int16_t id);
    void AddPage(int16_t id);

    static constexpr uint8_t HORIZONTAL = 0;
    static constexpr uint8_t VERTICAL = 1;

    UIViewGroup *container_ = nullptr;
    UIImageView *desktop_ = nullptr;
    UIFoldableView *foldableView_ = nullptr;
    UIViewGroup *lastFocusView_ = nullptr;
    UILabel *focusLabel_ = nullptr;
    UILabel *applistLabel_ = nullptr;
    UILabel *titleLabel_ = nullptr;
    UILabelButton *editFinishButton_ = nullptr;
    UIViewGroup *addCardPage_ = nullptr;
    UIViewGroup *editPage_ = nullptr;

    uint8_t direction_ = VERTICAL;
    uint8_t startDragDirection_ = 0;
    int16_t startDragYoffset_ = 0;
    int16_t curDragYoffset_ = 0;
    bool dragFoldableView_ = false;
    bool topSlidingOut_ = false;
    bool maySlidingIn_ = false;
    bool slidingIn_ = false;
    bool bottomSlidingOut_ = false;
    UIFoldableView::LayoutMode layoutMode_ = UIFoldableView::LayoutMode::LAYOUT_MODE_FOLD;
};
} // namespace OHOS
#endif // UI_TEST_FOLDABLE_H

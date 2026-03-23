/*
 * Copyright (c) @CompanyNameMagicTag. 2024. All rights reserved.
 */

#ifndef UI_TEST_ROLLER_VIEW_H
#define UI_TEST_ROLLER_VIEW_H

#include "ui_test.h"
#include "components/ui_roller_view.h"
#include "components/ui_image_view.h"
#include "components/ui_label_button.h"

namespace OHOS {
class UITestRollerView : public UITest, public UIView::OnClickListener, UIRollerView::OnModeChangeListener {
public:
    UITestRollerView() {}
    ~UITestRollerView() override {}
    void SetUp() override;
    void TearDown() override;;
    const UIView* GetTestView() override;
    bool OnClick(UIView& view, const ClickEvent& event) override;
    void OnModeChange(UIRollerView* view, RollerViewMode targetMode) override;

private:
    class ChildStateListener : public UIRollerView::OnChildStateListener {
    public:
        void SetRollerView(UIRollerView* roller)
        {
            roller_ = roller;
        }

        bool StateChange(UIView* view, float angle) override;
    private:
        UIRollerView* roller_ = nullptr;
    };

    void CreateButton(UILabelButton*& button, const char* text, int16_t x, int16_t y);
    void HideOrShowBtn();
    void CreateNoMirrorRollerView();
    UIViewGroup* container_ = nullptr;
    UIRollerView* rollerView_ = nullptr;
    UIRollerView* noMirrorRollerView_ = nullptr;
    UIRollerView* curRollerView_ = nullptr;
    UILabelButton* removeCurrentBtn_ = nullptr;
    UILabelButton* addBackBtn_ = nullptr;
    UILabelButton* clearBtn_ = nullptr;
    UILabelButton* alignBtn_ = nullptr;
    UILabelButton* switchAnimBtn_ = nullptr;
    UILabelButton* switchBtn_ = nullptr;
    UILabelButton* offMirrorBtn_ = nullptr;
    ChildStateListener* stateChangeListener_ = nullptr;
    bool isBtnVisible_ = false;
};
}
#endif
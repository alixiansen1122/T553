/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: TestCase
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */

#include "ui_test_view_bounds.h"

#if ENABLE_DEBUG
#include "common/screen.h"
#include "components/root_view.h"
#include "components/ui_button.h"
#include "components/ui_checkbox.h"
#include "components/ui_digital_clock.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_radio_button.h"
#include "components/ui_toggle_button.h"
#include "font/ui_font.h"
#include "test_resource_config.h"

#include "dfx/ui_view_bounds.h"

const uint16_t offsetx_ = 13;
const uint16_t offsety_ = 30;
const uint16_t bound_positionY_ = 50;
const uint16_t bound_positionW_ = 400;
const uint16_t bound_positionH_ = 50;
const uint16_t screenH_ = 150;
const uint16_t width = 40;
const uint16_t height = 40;
const uint16_t offset = 5;
namespace OHOS {
UITestViewBounds::~UITestViewBounds()
{
    if (viewBoundsChangeListener_ != nullptr) {
        delete viewBoundsChangeListener_;
        viewBoundsChangeListener_ = nullptr;
    }
}
void UITestViewBounds::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight() - BACK_BUTTON_HEIGHT);
        container_->SetHorizontalScrollState(false);
        container_->SetThrowDrag(true);
    }
}

void UITestViewBounds::TearDown()
{
    DeleteChildren(container_);
    container_ = nullptr;
}

const UIView* UITestViewBounds::GetTestView()
{
    UIKitViewBoundsSetState001();
    UIKitViewBoundsMargin001();
    UIKitViewBoundsPadding001();
    UIKitViewBoundsBorder001();
    UIKitViewBounds001();
    UIKitViewBoundsInvisible001();
    UIKitViewBoundsSmallView001();
    return container_;
}

class ViewBoundsChangeListener : public UIToggleButton::OnChangeListener {
public:
    ViewBoundsChangeListener() {}
    ~ViewBoundsChangeListener() {}

    bool OnChange(UICheckBox::UICheckBoxState state) override
    {
        switch (state) {
            case UICheckBox::SELECTED:
                UIViewBounds::GetInstance()->SetShowState(true);
                break;

            case UICheckBox::UNSELECTED:
                UIViewBounds::GetInstance()->SetShowState(false);
                break;
            default:
                break;
        }
        return true;
    }
};

UILabel* UITestViewBounds::CreateTitleLabel() const
{
    UILabel* label = new UILabel();
    label->SetHeight(TITLE_LABEL_DEFAULT_HEIGHT);
    label->SetLineBreakMode(UILabel::LINE_BREAK_STRETCH);
    label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE + offsetx_, TEXT_DISTANCE_TO_TOP_SIDE + offsety_);
    label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    return label;
}

void UITestViewBounds::UIKitViewBoundsSetState001()
{
    if (container_ == nullptr) {
        return;
    }
    UIViewGroup* group1 = new UIViewGroup();
    container_->Add(group1);
    UILabel* title1 = CreateTitleLabel();
    group1->Add(title1);

    UIViewGroup* group = new UIViewGroup();
    container_->Add(group);
    group->SetViewId("UIKitViewBoundsSetState001");
    group->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE + offsetx_, bound_positionY_, bound_positionW_, bound_positionH_);
    UILabel* viewBoundsLabel = new UILabel();
    group->Add(viewBoundsLabel);
    viewBoundsLabel->SetHeight(30); // 30: height
    viewBoundsLabel->LayoutCenterOfParent();
    viewBoundsLabel->LayoutLeftOfParent(offset); // 5: offset
    viewBoundsLabel->SetLineBreakMode(UILabel::LINE_BREAK_STRETCH);
    viewBoundsLabel->SetText("Show View Bounds: ");

    UIToggleButton* toggle = new UIToggleButton();
    group->Add(toggle);
    toggle->SetWidth(width);  // 50: width
    toggle->SetHeight(width); // 50: height
    toggle->LayoutCenterOfParent();
    toggle->LayoutRightOfParent(offset); // 5: offset
    if (viewBoundsChangeListener_ != nullptr) {
        delete viewBoundsChangeListener_;
    }
    viewBoundsChangeListener_ = new ViewBoundsChangeListener();
    toggle->SetOnChangeListener(viewBoundsChangeListener_);
    if (UIViewBounds::GetInstance()->GetShowState()) {
        toggle->SetState(true);
    } else {
        toggle->SetState(false);
    }
}

void UITestViewBounds::UIKitViewBoundsMargin001()
{
    if (container_ == nullptr) {
        return;
    }
    UIViewGroup* group = new UIViewGroup();
    container_->Add(group);
    group->Resize(Screen::GetInstance().GetWidth(), screenH_); // 150: height
    group->SetViewId("UIKitViewBoundsMargin001");
    group->LayoutBottomToSibling("UIKitViewBoundsSetState001");

    UILabel* title = CreateTitleLabel();
    group->Add(title);
    title->SetText("Test View bounds with margin: ");
    title->SetViewId("margin_title");

    UIView* view1 = new UIView();
    group->Add(view1);
    view1->Resize(width, width);                            // 50: view size
    view1->LayoutBottomToSibling("margin_title", 10); // 10: offset
    view1->AlignLeftToSibling("margin_title");
    view1->SetStyle(STYLE_BACKGROUND_COLOR, Color::Gray().full);
    view1->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    view1->SetStyle(STYLE_MARGIN_LEFT, 50); // 50: margin size
}

void UITestViewBounds::UIKitViewBoundsPadding001()
{
    if (container_ == nullptr) {
        return;
    }
    UIViewGroup* group = new UIViewGroup();
    container_->Add(group);
    group->Resize(Screen::GetInstance().GetWidth(), screenH_); // 150: height
    group->SetViewId("UIKitViewBoundsPadding001");
    group->LayoutBottomToSibling("UIKitViewBoundsMargin001");

    UILabel* title = CreateTitleLabel();
    group->Add(title);
    title->SetText("Test View bounds with padding: ");
    title->SetViewId("padding_title");

    UIView* view1 = new UIView();
    group->Add(view1);
    view1->Resize(width, width);                             // 50: view width and height
    view1->LayoutBottomToSibling("padding_title", 10); // 10: offset
    view1->AlignLeftToSibling("padding_title");
    view1->SetStyle(STYLE_BACKGROUND_COLOR, Color::Gray().full);
    view1->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    view1->SetStyle(STYLE_PADDING_LEFT, 50); // 50: padding size
}

void UITestViewBounds::UIKitViewBoundsBorder001()
{
    if (container_ == nullptr) {
        return;
    }
    UIViewGroup* group = new UIViewGroup();
    container_->Add(group);
    group->Resize(Screen::GetInstance().GetWidth(), screenH_); // 150: height
    group->SetViewId("UIKitViewBoundsBorder001");
    group->LayoutBottomToSibling("UIKitViewBoundsPadding001");

    UILabel* title = CreateTitleLabel();
    group->Add(title);
    title->SetText("Test View bounds with border:");
    title->SetViewId("border_title");

    UIView* view1 = new UIView();
    group->Add(view1);
    view1->Resize(width, width);                            // 50: view size
    view1->LayoutBottomToSibling("border_title", 10); // 10: offset
    view1->AlignLeftToSibling("border_title");
    view1->SetStyle(STYLE_BACKGROUND_COLOR, Color::Gray().full);
    view1->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    view1->SetStyle(STYLE_BORDER_COLOR, Color::Red().full);
    view1->SetStyle(STYLE_BORDER_OPA, OPA_OPAQUE);
    view1->SetStyle(STYLE_BORDER_WIDTH, 20); // 20: border size
}

void UITestViewBounds::UIKitViewBounds001()
{
    if (container_ == nullptr) {
        return;
    }
    UIViewGroup* group = new UIViewGroup();
    container_->Add(group);
    group->Resize(Screen::GetInstance().GetWidth(), screenH_); // 150: height
    group->SetViewId("UIKitViewBounds001");
    group->LayoutBottomToSibling("UIKitViewBoundsBorder001");

    UILabel* title = CreateTitleLabel();
    group->Add(title);
    title->SetText("Test View bounds with margin/padding/border:");
    title->SetViewId("margin_padding_border_title");

    UIView* view1 = new UIView();
    group->Add(view1);
    view1->Resize(width, width);                                           // 50: view size
    view1->LayoutBottomToSibling("margin_padding_border_title", 10); // 10: offset
    view1->AlignLeftToSibling("margin_padding_border_title");
    view1->SetStyle(STYLE_BACKGROUND_COLOR, Color::Gray().full);
    view1->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    view1->SetStyle(STYLE_BORDER_COLOR, Color::Yellow().full);
    view1->SetStyle(STYLE_BORDER_OPA, OPA_OPAQUE);
    view1->SetStyle(STYLE_BORDER_WIDTH, 20); // 20: border size
    view1->SetStyle(STYLE_MARGIN_LEFT, 20);  // 20: margin size
    view1->SetStyle(STYLE_PADDING_LEFT, 50); // 50: padding size
}

void UITestViewBounds::UIKitViewBoundsInvisible001()
{
    if (container_ == nullptr) {
        return;
    }
    UIViewGroup* group = new UIViewGroup();
    container_->Add(group);
    group->Resize(Screen::GetInstance().GetWidth(), screenH_); // 150: height
    group->SetViewId("UIKitViewBoundsInvisible001");
    group->LayoutBottomToSibling("UIKitViewBounds001");

    UILabel* title = CreateTitleLabel();
    group->Add(title);
    title->SetText("Test View bounds when view is invisible: ");
    title->SetViewId("invisible_title");

    UIView* view1 = new UIView();
    group->Add(view1);
    view1->Resize(width, width);                               // 50: view size
    view1->LayoutBottomToSibling("invisible_title", 10); // 10: offset
    view1->AlignLeftToSibling("invisible_title");
    view1->SetStyle(STYLE_BACKGROUND_COLOR, Color::Gray().full);
    view1->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    view1->SetStyle(STYLE_MARGIN_LEFT, 20); // 20: margin size
    view1->SetVisible(false);
}

void UITestViewBounds::UIKitViewBoundsSmallView001()
{
    if (container_ == nullptr) {
        return;
    }
    UIViewGroup* group = new UIViewGroup();
    container_->Add(group);
    group->Resize(Screen::GetInstance().GetWidth(), screenH_); // 150: height
    group->SetViewId("UIKitViewBoundsSmallView001");
    group->LayoutBottomToSibling("UIKitViewBoundsInvisible001");

    UILabel* title = CreateTitleLabel();
    group->Add(title);
    title->SetText("Test View bounds when view is small: ");
    title->SetViewId("small_title");

    UIView* view1 = new UIView();
    group->Add(view1);
    view1->Resize(10, 10);                           // 10: view size
    view1->LayoutBottomToSibling("small_title", 10); // 10: offset
    view1->AlignLeftToSibling("small_title");
    view1->SetStyle(STYLE_BACKGROUND_COLOR, Color::Gray().full);
    view1->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
}

void UITestViewBounds::UIKitViewBoundsPostDraw001()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = new UIViewGroup();
    container_->Add(group);
    group->Resize(Screen::GetInstance().GetWidth(), screenH_); // 150: height
    group->SetViewId("UIKitViewBoundsPostDraw001");
    group->LayoutBottomToSibling("UIKitViewBoundsSmallView001");

    UILabel* title = CreateTitleLabel();
    group->Add(title);
    title->SetText("Test View bounds when child view override OnPostDraw: ");
    title->SetViewId("postdraw_title");

    UIView* view1 = new UIView();
    group->Add(view1);
    view1->Resize(10, 10);                           // 10: view size
    view1->LayoutBottomToSibling("small_title", 10); // 10: offset
    view1->AlignLeftToSibling("small_title");
    view1->SetStyle(STYLE_BACKGROUND_COLOR, Color::Gray().full);
    view1->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
}
} // namespace OHOS
#endif

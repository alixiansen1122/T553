/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: Lottie
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */

#include "ui_test_view_group.h"

#include "common/screen.h"

namespace OHOS {
void UITestViewGroup::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight() - BACK_BUTTON_HEIGHT);
    }
}

void UITestViewGroup::TearDown()
{
    DeleteChildren(container_);
    container_ = nullptr;
}

const UIView* UITestViewGroup::GetTestView()
{
    UIKitViewGroupTestAddRemove001();
    UIKitViewGroupTestAddError001();
    UIKitViewGroupTestInsertError001();
    return container_;
}

UIViewGroup* UITestViewGroup::CreateTestCaseGroup() const
{
    UIViewGroup* group = new UIViewGroup();
    group->Resize(Screen::GetInstance().GetWidth(), 200); // 200: height
    return group;
}

UILabel* UITestViewGroup::CreateTitleLabel() const
{
    UILabel* label = new UILabel();
    // 300: label width
    label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, TEXT_DISTANCE_TO_TOP_SIDE, 300, TITLE_LABEL_DEFAULT_HEIGHT);
    label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);

    return label;
}

UILabelButton* UITestViewGroup::CreateButton(std::string text, int16_t width, int16_t height) const
{
    UILabelButton* button = new UILabelButton();
    button->Resize(width, height);
    button->SetText(text.c_str());
    button->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    button->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::RELEASED);
    button->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::PRESSED);
    button->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::RELEASED);
    button->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_PRESS, UIButton::PRESSED);

    return button;
}

void UITestViewGroup::UIKitViewGroupTestAddRemove001()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->Resize(Screen::GetInstance().GetWidth(), 250);   // 250:height
    group->SetViewId("UIKitViewGroupTestAddRemove001");
    group->SetPosition(0, 0);

    UILabel* label = CreateTitleLabel();
    group->Add(label);
    label->SetText("UIViewGroup 添加/删除组件:");
    label->SetViewId("label_text");
    label->LayoutLeftOfParent(80);  // 80: offset
    label->LayoutTopOfParent(50);  // 50: offset

    removeBtn_ = CreateButton("删除View", 150, 40); // 150: width 40:height
    group->Add(removeBtn_);
    removeBtn_->SetOnClickListener(this);
    removeBtn_->SetViewId("removeBtn");
    removeBtn_->LayoutBottomOfParent();
    removeBtn_->LayoutLeftOfParent(50); // 50: offset
    
    addBtn_ = CreateButton("添加View", 150, 40); // 150: width 40:height
    group->Add(addBtn_);
    addBtn_->SetOnClickListener(this);
    addBtn_->SetViewId("addBtn");
    addBtn_->LayoutBottomOfParent();
    addBtn_->LayoutLeftOfParent(50);     // 50: offset
    addBtn_->LayoutTopToSibling("removeBtn", 10); // 10: offset

    removeAddBtn_ = CreateButton("删除再添加View", 160, 40); // 160: width 40:height
    group->Add(removeAddBtn_);
    removeAddBtn_->SetOnClickListener(this);
    removeAddBtn_->SetViewId("removeAddBtn");
    removeAddBtn_->LayoutBottomOfParent();
    removeAddBtn_->LayoutRightToSibling("removeBtn", 0); // 10: offset

    container_->Add(group);
}

void UITestViewGroup::UIKitViewGroupTestAddError001()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKitViewGroupTestAddError001");
    container_->Add(group);

    UILabel* label = CreateTitleLabel();
    group->Add(label);
    // 2: half of screen width
    label->Resize(Screen::GetInstance().GetWidth() / 2, TITLE_LABEL_DEFAULT_HEIGHT);
    label->SetText("测试重复添加子组件问题:");

    addTwiceBtn_ = CreateButton("重复add", 150, 40); // 150: width 40:height
    group->Add(addTwiceBtn_);
    addTwiceBtn_->SetViewId("addTwiceBtn");
    addTwiceBtn_->SetOnClickListener(this);
    addTwiceBtn_->LayoutCenterOfParent();
    addTwiceBtn_->LayoutLeftOfParent(30); // 30: offset

    addMultiParentBtn_ = CreateButton("add到不同父节点", 200, 40); // 200: width 40:height
    group->Add(addMultiParentBtn_);
    addMultiParentBtn_->SetViewId("addMultiParentBtn");
    addMultiParentBtn_->SetOnClickListener(this);
    addMultiParentBtn_->LayoutCenterOfParent();
    addMultiParentBtn_->LayoutRightToSibling("addTwiceBtn", 10); // 10: offset

    addSelfBtn_ = CreateButton("add到自己", 150, 40); // 150: width 40:height
    group->Add(addSelfBtn_);
    addSelfBtn_->SetViewId("addSelfBtn");
    addSelfBtn_->SetOnClickListener(this);
    addSelfBtn_->LayoutCenterOfParent();
    addSelfBtn_->LayoutBottomToSibling("addTwiceBtn", 10); // 10: offset

    group->LayoutBottomToSibling("UIKitViewGroupTestAddRemove001", 10); // 10: offset
}

void UITestViewGroup::UIKitViewGroupTestInsertError001()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKitViewGroupTestInsertError001");
    container_->Add(group);

    UILabel* label = CreateTitleLabel();
    group->Add(label);
    // 2: half of screen width
    label->Resize(Screen::GetInstance().GetWidth() / 2, TITLE_LABEL_DEFAULT_HEIGHT);
    label->SetText("测试Insert子组件问题:");

    insertTwiceBtn_ = CreateButton("重复insert", 150, 40); // 150: width 40:height
    group->Add(insertTwiceBtn_);
    insertTwiceBtn_->SetViewId("insertTwiceBtn");
    insertTwiceBtn_->SetOnClickListener(this);
    insertTwiceBtn_->LayoutCenterOfParent();
    insertTwiceBtn_->LayoutLeftOfParent(30); // 30: offset

    insertMultiParentBtn_ = CreateButton("insert到不同父节点", 200, 40); // 200: width 40:height
    group->Add(insertMultiParentBtn_);
    insertMultiParentBtn_->SetViewId("insertMultiParentBtn");
    insertMultiParentBtn_->SetOnClickListener(this);
    insertMultiParentBtn_->LayoutCenterOfParent();
    insertMultiParentBtn_->LayoutRightToSibling("insertTwiceBtn", 10); // 10: offset

    insertSelfBtn_ = CreateButton("insert到自己", 150, 40); // 150: width 40:height
    group->Add(insertSelfBtn_);
    insertSelfBtn_->SetViewId("addSelfBtn");
    insertSelfBtn_->SetOnClickListener(this);
    insertSelfBtn_->LayoutCenterOfParent();
    insertSelfBtn_->LayoutBottomToSibling("insertTwiceBtn", 10); // 10: offset

    group->LayoutBottomToSibling("UIKitViewGroupTestAddError001", 10); // 10: offset
}

bool UITestViewGroup::OnClick(UIView& view, const ClickEvent& event)
{
    if (&view == addBtn_) {
        AddView();
    } else if (&view == removeBtn_) {
        RemoveView();
    } else if (&view == removeAddBtn_) {
        RemoveAndAddView();
    } else if (&view == addTwiceBtn_) {
        UIView* view = new UIView();
        container_->Add(view);
        container_->Add(view);
    } else if (&view == addMultiParentBtn_) {
        AddMultiParent();
    } else if (&view == addSelfBtn_) {
        container_->Add(container_);
    } else if (&view == insertTwiceBtn_) {
        UIView* view = new UIView();
        container_->Insert(nullptr, view);
        container_->Insert(nullptr, view);
    } else if (&view == insertMultiParentBtn_) {
        InsertMultiParent();
    } else if (&view == insertSelfBtn_) {
        container_->Insert(nullptr, container_);
    }
    return true;
}

void UITestViewGroup::AddView()
{
    UIView* view = new UIView();
    view->Resize(200, 50);  // 200 : width , 50 height
    view->SetPosition(90, 90);  // 90 : Coordinates of the top left corner of the circle
    ColorType color = Color::GetColorFromRGB(std::rand() % 256, std::rand() % 256, std::rand() % 256);
    view->SetStyle(STYLE_BACKGROUND_COLOR, color.full);
    view->SetViewId("id_view1");
    UIViewGroup* vg = static_cast<UIViewGroup*>(container_->GetChildById("UIKitViewGroupTestAddRemove001"));
    if (vg != nullptr) {
        vg->Add(view);
        vg->Invalidate();
    }
}

void UITestViewGroup::RemoveView()
{
    UIViewGroup* vg = static_cast<UIViewGroup*>(container_->GetChildById("UIKitViewGroupTestAddRemove001"));
    UIView* view1 = container_->GetChildById("id_view1");
    if ((vg != nullptr) && (view1 != nullptr)) {
        vg->Remove(view1);
        vg->Invalidate();
    }
}

void UITestViewGroup::RemoveAndAddView()
{
    UIViewGroup* vg = static_cast<UIViewGroup*>(container_->GetChildById("UIKitViewGroupTestAddRemove001"));
    UIView* view1 = container_->GetChildById("id_view1");
    if ((vg != nullptr) && (view1 != nullptr)) {
        vg->Remove(view1);
        vg->Add(view1);
        vg->Invalidate();
    }
}

void UITestViewGroup::AddMultiParent()
{
    UIView* view = new UIView();
    UIViewGroup* vg = static_cast<UIViewGroup*>(container_->GetChildById("UIKitViewGroupTestAddError001"));
    if ((vg != nullptr) && (view != nullptr)) {
        vg->Add(view);
    }
    container_->Add(view);
}

void UITestViewGroup::InsertMultiParent()
{
    UIView* view = new UIView();
    UIViewGroup* vg = static_cast<UIViewGroup*>(container_->GetChildById("UIKitViewGroupTestInsertError001"));
    if ((vg != nullptr) && (view != nullptr)) {
        vg->Insert(nullptr, view);
    }
    container_->Insert(nullptr, view);
}
} // namespace OHOS

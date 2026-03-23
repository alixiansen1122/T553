/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "test_case_list_adapter.h"
#include "common/screen.h"
#include "components/ui_button.h"
#include "components/ui_label_button.h"
#include "gfx_utils/list.h"
#include "ui_test.h"
#include "ui_test_app.h"
#include "ui_test_group.h"

namespace OHOS {
namespace {
const uint16_t TESTCASE_BUTTON_HEIGHT = 100;
const uint16_t STYLE_BORDER_WIDTH_VALUE = 4;
const uint16_t BACK_BUTTON_SIZE = 50;
} // namespace

uint16_t TestCaseListAdapter::GetCount()
{
    return UITestGroup::GetInstance()->GetTestCase().Size();
}

class TransparentBackButton : public UIButton {
public:
    TransparentBackButton() : UIButton()
    {
        opaScale_ = OPA_TRANSPARENT;
    }

    bool OnClickEvent(const ClickEvent &event) override
    {
        RootView::GetInstance()->RemoveAll();
        UITest* uiTest = UITestGroup::GetInstance()->GetTestCase(GetViewId());
        if (uiTest != nullptr) {
            uiTest->TearDown();
        }
        UITestApp::GetInstance()->Init();
        return true;
    }
};

class UiTestClickListener : public UIView::OnClickListener {
public:
    UiTestClickListener() {}
    ~UiTestClickListener() override
    {
        if (backBtn_ != nullptr) {
            delete backBtn_;
            backBtn_ = nullptr;
        }
    }
    UiTestClickListener(const UiTestClickListener &) = delete;
    UiTestClickListener &operator=(const UiTestClickListener &) = delete;

    bool OnClick(UIView& view, const ClickEvent& event) override
    {
        UIViewGroup* rootView = RootView::GetInstance();
        UITest* uiTest = UITestGroup::GetInstance()->GetTestCase(view.GetViewId());
        if (uiTest == nullptr) {
            return false;
        }
        uiTest->SetUp();
        rootView->RemoveAll();
        UIView* tempView = const_cast<UIView*>(uiTest->GetTestView());
        if (tempView != nullptr) {
            tempView->SetTouchable(true);
            tempView->SetViewId(view.GetViewId());
            tempView->SetPosition(tempView->GetX(), tempView->GetY());
            rootView->Add(tempView);
        }
        if (backBtn_ == nullptr) {
            backBtn_ = new TransparentBackButton();
            backBtn_->SetPosition(0, (Screen::GetInstance().GetHeight() - BACK_BUTTON_SIZE) / 2, // 2, half size
                BACK_BUTTON_SIZE, BACK_BUTTON_SIZE);
        }
        backBtn_->SetViewId(view.GetViewId());
        rootView->Add(backBtn_);
        rootView->Invalidate();
        return true;
    }
private:
    UIButton* backBtn_{nullptr};
};

UIView* TestCaseListAdapter::GetView(UIView* inView, int16_t index)
{
    List<TestCaseInfo> testCaseList = UITestGroup::GetInstance()->GetTestCase();
    if (testCaseList.IsEmpty()) {
        return nullptr;
    }
    if ((index > testCaseList.Size() - 1) || (index < 0)) {
        return nullptr;
    }
    UILabelButton* item = nullptr;
    if (inView == nullptr) {
        item = new UILabelButton();
        item->SetPosition(0, 0);
        item->SetStyleForState(STYLE_BORDER_WIDTH, STYLE_BORDER_WIDTH_VALUE, UIButton::RELEASED);
        item->SetStyleForState(STYLE_BORDER_WIDTH, STYLE_BORDER_WIDTH_VALUE, UIButton::PRESSED);
        item->SetStyleForState(STYLE_BORDER_WIDTH, STYLE_BORDER_WIDTH_VALUE, UIButton::INACTIVE);
        item->SetStyleForState(STYLE_BORDER_OPA, 0, UIButton::RELEASED);
        item->SetStyleForState(STYLE_BORDER_OPA, 0, UIButton::PRESSED);
        item->SetStyleForState(STYLE_BORDER_OPA, 0, UIButton::INACTIVE);
        item->Resize(Screen::GetInstance().GetWidth() - TEXT_DISTANCE_TO_LEFT_SIDE, TESTCASE_BUTTON_HEIGHT);
    } else {
        item = static_cast<UILabelButton*>(inView);
    }

    if (listener_ == nullptr) {
        listener_ = new UiTestClickListener();
    }
    ListNode<TestCaseInfo>* node = testCaseList.Begin();
    for (uint16_t i = 0; i < index; i++) {
        node = node->next_;
    }
    item->SetOnClickListener(listener_);
    item->SetText(node->data_.sliceId);
    item->SetViewId(node->data_.sliceId);
    item->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 40); // 40: means font size
    item->SetViewIndex(index);
    item->SetAlign(TEXT_ALIGNMENT_LEFT);
#ifndef QSPI_DISPLAY
    item->SetLabelPosition(150, 0); // 150: label x-coordinate
#endif
    item->SetStyleForState(STYLE_BORDER_RADIUS, 0, UIButton::RELEASED);
    item->SetStyleForState(STYLE_BORDER_RADIUS, 0, UIButton::PRESSED);
    item->SetStyleForState(STYLE_BORDER_RADIUS, 0, UIButton::INACTIVE);
    item->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::RELEASED);
    item->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::PRESSED);
    item->SetStyleForState(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT, UIButton::INACTIVE);
    return item;
}

int16_t TestCaseListAdapter::GetItemWidthWithMargin(int16_t index)
{
    // 2: two borders on both sides
    return Screen::GetInstance().GetWidth() - TEXT_DISTANCE_TO_LEFT_SIDE + STYLE_BORDER_WIDTH_VALUE * 2;
}

int16_t TestCaseListAdapter::GetItemHeightWithMargin(int16_t index)
{
    return TESTCASE_BUTTON_HEIGHT + STYLE_BORDER_WIDTH_VALUE * 2; // 2: two borders on both sides
}
} // namespace OHOS
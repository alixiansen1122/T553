/*
 * Copyright (c) 2022 CompanyNameMagicTag.
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

#include "ui_test_keyboard.h"

namespace OHOS {
constexpr int16_t EDIT_VIEW_OFFSET = 20;

#if ENABLE_SOFT_KEYBOARD
void UITestKeyboard::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
        container_->SetHorizontalScrollState(false);
        container_->SetThrowDrag(true);
    }
}

void UITestKeyboard::TearDown()
{
    DeleteChildren(container_);
    container_ = nullptr;
}

UIEditText* UITestKeyboard::SetupEditText(const char* value, const char* placeholder, const char* viewId)
{
    UIEditText* editText = new UIEditText();
    editText->SetText(value);
    editText->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 26); // 26: font size
    editText->SetStyle(STYLE_BACKGROUND_COLOR, 0xFFFFFFFF);
    editText->Resize(200, 80);                           // 200: width 80: height
    editText->SetPlaceholder(placeholder);
    editText->SetViewId(viewId);
    return editText;
}

bool UITestKeyboard::OnClick(UIView& view, const ClickEvent& event)
{
    if (editText2_ != nullptr) {
        if (editText2_->GetInputType() == InputType::TEXT_TYPE) {
            editText2_->SetInputType(InputType::PASSWORD_TYPE);
        } else {
            editText2_->SetInputType(InputType::TEXT_TYPE);
        }
    }

    return true;
}

void UITestKeyboard::UIKitUIEditTextTestDisplay001()
{
    if (container_ == nullptr) {
        return;
    }

    UIEditText* editText1 = SetupEditText("", "Account:", "editText1");
    container_->Add(editText1);
    editText1->SetPosition(20, 80); // 20:position x; 80: position x;
    editText1->SetPlaceholderColor(Color::Blue());
    editText1->SetCursorColor(Color::Red());

    editText2_ = SetupEditText("", "Password:", "editText2");
    container_->Add(editText2_);
    editText2_->SetMaxLength(8); // 8: max length
    editText2_->SetInputType(InputType::PASSWORD_TYPE);
    editText2_->LayoutBottomToSibling("editText1", EDIT_VIEW_OFFSET);
    editText2_->AlignLeftToSibling("editText1");

    showsBtn_ = new UILabelButton();
    container_->Add(showsBtn_);
    showsBtn_->SetOnClickListener(this);
    showsBtn_->Resize(100, 60); // 100: width 60: height
    showsBtn_->LayoutRightToSibling("editText2", 2); // 2:edit view width offset
    showsBtn_->AlignTopToSibling("editText2");
    showsBtn_->SetText("显示");
    showsBtn_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 26); // 26: font size
}

const UIView* UITestKeyboard::GetTestView()
{
    UIKitUIEditTextTestDisplay001();
    return container_;
}
#endif
} // namespace OHOS
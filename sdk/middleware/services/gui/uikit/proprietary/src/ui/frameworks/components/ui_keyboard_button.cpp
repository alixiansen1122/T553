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

#include "input_method/ui_keyboard_button.h"
#include "input_method/ui_input_method.h"
#include "input_method/ui_soft_keyboard.h"

#if ENABLE_SOFT_KEYBOARD
namespace OHOS {
namespace {
constexpr uint32_t KEY_FONT_SIZE = 20;
constexpr uint32_t KEY_RADIUS = 5;
constexpr uint32_t KEY_COLOR = 0xFFE8EAEE;
}

UIKeyboardButton::UIKeyboardButton()
{
    SetAlign(UITextLanguageAlignment::TEXT_ALIGNMENT_CENTER);
    SetStyleForState(STYLE_BACKGROUND_COLOR, KEY_COLOR, UIButton::ButtonState::RELEASED);
    SetStyleForState(STYLE_BACKGROUND_COLOR, KEY_COLOR, UIButton::ButtonState::PRESSED);
    SetStyleForState(STYLE_BACKGROUND_OPA, 0XFF, UIButton::ButtonState::RELEASED);
    SetStyleForState(STYLE_BACKGROUND_OPA, 0XFF, UIButton::ButtonState::PRESSED);
    SetStyleForState(STYLE_BORDER_RADIUS, KEY_RADIUS, UIButton::ButtonState::RELEASED);
    SetStyleForState(STYLE_BORDER_RADIUS, KEY_RADIUS, UIButton::ButtonState::PRESSED);
    SetFont(DEFAULT_VECTOR_FONT_FILENAME, KEY_FONT_SIZE);
    SetTextColor(Color::Black());
    SetTouchable(true);
    SetDraggable(true);
    for (int i = 0; i < MAX_KEY_VALUE_NUM; ++i) {
        keyCode_[i] = UI_KEYCODE::UI_KEY_BUTT;
    }
}

UIKeyboardButton::~UIKeyboardButton() {}

void UIKeyboardButton::SetButtonMask(UI_KEYCODE *keys, uint8_t num)
{
    if (keys == nullptr || num == 0) {
        return;
    }

    keyNum_ = 0;
    for (uint8_t i = 0; i < num; i++) {
        if (static_cast<int>(keys[i]) == 0) {
            break;
        }
        keyCode_[i] = keys[i];
        keyNum_++;
    }
}

void UIKeyboardButton::GetFuncKeyText(UI_KEYCODE key, std::string &tmpText)
{
    switch (key) {
        case UI_KEYCODE::UI_KEY_ENTER:
            tmpText += "Send";
            break;
        case UI_KEYCODE::UI_KEY_SOFT_KEYBOARD_PINYIN:
            tmpText += "中";
            break;
        case UI_KEYCODE::UI_KEY_DEL:
            tmpText += "Del";
            break;
        case UI_KEYCODE::UI_KEY_SOFT_KEYBOARD:
            tmpText += "IME";
            break;
        case UI_KEYCODE::UI_KEY_SOFT_KEYBOARD_EN:
            tmpText += "a";
            break;
        case UI_KEYCODE::UI_KEY_SOFT_KEYBOARD_CAP:
            tmpText += "A";
            break;
        case UI_KEYCODE::UI_KEY_SOFT_KEYBOARD_NUMBER:
            tmpText += "1";
            break;
        case UI_KEYCODE::UI_KEY_SOFT_KEYBOARD_SYMBOL:
            tmpText += "@";
            break;
        default:
            break;
    }
}

void UIKeyboardButton::SetButtonText()
{
    std::string tmpText;
    for (uint8_t i = 0; i < keyNum_; i++) {
        UI_KEYCODE key = keyCode_[i];
        if (key >= UI_KEYCODE::UI_KEY_ENTER) {
            GetFuncKeyText(key, tmpText);
        } else if (key == UI_KEYCODE::UI_KEY_SPACE) {
            tmpText += "Space";
        } else {
            tmpText.push_back(static_cast<char>(key));
        }
    }
    SetText(tmpText.c_str());
}

void UIKeyboardButton::Clear()
{
    keyNum_ = 0;
    SetText("");
}

bool UIKeyboardButton::OnClickEvent(const ClickEvent& event)
{
    if (keyNum_ == 1) {
        UIPinyinSoftKeyboard *parent = dynamic_cast<UIPinyinSoftKeyboard*>(GetParent());
        if (parent != nullptr) {
            UIInputMethod::GetInstance()->InputPinyin(keyCode_[0]);
            return true;
        }
        UIInputMethod::GetInstance()->Input(keyCode_[0]);
    }
    return true;
}

bool UIKeyboardButton::OnPressEvent(const PressEvent& event)
{
    if (keyNum_ > 1) {
        UIPinyinSoftKeyboard *parent = dynamic_cast<UIPinyinSoftKeyboard*>(GetParent());
        if (parent != nullptr) {
            UIInputMethod::GetInstance()->HideHanziPageList();
        }
        UIInputMethod::GetInstance()->ShowOptionBox(this, keyCode_, keyNum_);
    }
    return true;
}

bool UIKeyboardButton::OnReleaseEvent(const ReleaseEvent& event)
{
    if (keyNum_ > 1) {
        UIPinyinSoftKeyboard *parent = dynamic_cast<UIPinyinSoftKeyboard*>(GetParent());
        uint8_t keyIndex = UIInputMethod::GetInstance()->HideOptionBox();
        if (parent != nullptr) {
            UIInputMethod::GetInstance()->ShowHanziPageList();
            UIInputMethod::GetInstance()->InputPinyin(keyCode_[keyIndex]);
            return true;
        }
        UIInputMethod::GetInstance()->Input(keyCode_[keyIndex]);
    }
    return true;
}

bool UIKeyboardButton::OnDragEndEvent(const DragEvent& event)
{
    if (keyNum_ > 1) {
        UIPinyinSoftKeyboard *parent = dynamic_cast<UIPinyinSoftKeyboard*>(GetParent());
        uint8_t keyIndex = UIInputMethod::GetInstance()->HideOptionBox();
        if (parent != nullptr) {
            UIInputMethod::GetInstance()->ShowHanziPageList();
            UIInputMethod::GetInstance()->InputPinyin(keyCode_[keyIndex]);
            return true;
        }
        UIInputMethod::GetInstance()->Input(keyCode_[keyIndex]);
    }
    return true;
}
} // namespace OHOS
#endif // ENABLE_SOFT_KEYBOARD
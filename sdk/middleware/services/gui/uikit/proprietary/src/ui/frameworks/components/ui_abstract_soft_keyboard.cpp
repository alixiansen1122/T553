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

#include "input_method/ui_abstract_soft_keyboard.h"
#include "common/screen.h"
#include "font/ui_font.h"

#if ENABLE_SOFT_KEYBOARD
namespace OHOS {
constexpr uint32_t DEFAULE_ALLOC_KEYBOARD_BUT_NUM = 9;
uint16_t UIAbstractSoftKeyboard::usePoolCount_ = 0;
Graphic::Vector<UIKeyboardButton*> UIAbstractSoftKeyboard::keyBtnAllocPool_;
void UIAbstractSoftKeyboard::AllocKeyboardButton()
{
    UIKeyboardButton* keyBtnArr = new UIKeyboardButton[DEFAULE_ALLOC_KEYBOARD_BUT_NUM]{};
    for (uint32_t i = 0; i < DEFAULE_ALLOC_KEYBOARD_BUT_NUM; i++) {
        keyBtnAllocPool_.PushBack(&keyBtnArr[i]);
    }
}

void UIAbstractSoftKeyboard::ClearKeyBtnPool()
{
    for (uint8_t i = 0; i < keyBtnAllocPool_.Size();) {
        UIKeyboardButton* keyBtn = keyBtnAllocPool_[i];
        /* beacause of new [DEFAULE_ALLOC_KEYBOARD_BUT_NUM] objects at a time.so use delete []; */
        delete [] keyBtn;
        i += DEFAULE_ALLOC_KEYBOARD_BUT_NUM;
    }
    keyBtnAllocPool_.Clear();
}

UIAbstractSoftKeyboard::UIAbstractSoftKeyboard()
{
    SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    SetStyle(STYLE_BACKGROUND_OPA, 0x00);
    SetStyle(STYLE_BORDER_RADIUS, 10); // 10: border radius
    SetTouchable(false);
    SetDraggable(false);
    ++usePoolCount_;
}

UIAbstractSoftKeyboard::~UIAbstractSoftKeyboard()
{
    --usePoolCount_;
    for (int i = 0; i < curUseKeyBtnIndex_; i++) {
        UIKeyboardButton* keyBtn = keyBtnAllocPool_[i];
        if (keyBtn == nullptr) {
            continue;
        }
        UIViewGroup *parent = static_cast<UIViewGroup *>(keyBtn->GetParent());
        if (parent != nullptr && (parent == this)) {
            parent->Remove(keyBtn);
        }
    }

    if (usePoolCount_ == 0) {
        ClearKeyBtnPool();
    }
}

void UIAbstractSoftKeyboard::InitSoftKeyboard()
{
    curUseKeyBtnIndex_ = 0;
    CreateKeyButtons();
}

void UIAbstractSoftKeyboard::SetupKeyButton(SoftKey &key)
{
    if (curUseKeyBtnIndex_ >= keyBtnAllocPool_.Size()) {
        AllocKeyboardButton();
    }
    UIKeyboardButton* keyBtn = keyBtnAllocPool_[curUseKeyBtnIndex_++];
    if (keyBtn == nullptr) {
        return;
    }

    keyBtn->SetPosition(key.rect.x, key.rect.y, key.rect.width, key.rect.height);
    keyBtn->SetButtonMask(key.keys, MAX_KEY_VALUE_NUM);
    keyBtn->SetButtonText();
    UIViewGroup *parent = static_cast<UIViewGroup *>(keyBtn->GetParent());
    if (parent != nullptr) {
        parent->Remove(keyBtn);
    }
    this->Add(keyBtn);
}
} // namespace OHOS
#endif // ENABLE_SOFT_KEYBOARD

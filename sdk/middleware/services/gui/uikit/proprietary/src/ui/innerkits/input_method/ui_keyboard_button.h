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

#ifndef GRAPHIC_LITE_UI_KEYBOARD_BUTTON_H
#define GRAPHIC_LITE_UI_KEYBOARD_BUTTON_H

#include "components/ui_label_button.h"
#include "input_method/ui_keyboard_codetable.h"

#if ENABLE_SOFT_KEYBOARD
namespace OHOS {
/**
 * @brief Provides the functions related to UIKeyboardButton.
 */
class UIKeyboardButton : public UILabelButton {
public:
    UIKeyboardButton();
    ~UIKeyboardButton() override;
    void SetButtonMask(UI_KEYCODE *keys, uint8_t num);
    void SetButtonText();
    void Clear();

    bool OnClickEvent(const ClickEvent& event) override;
    bool OnPressEvent(const PressEvent& event) override;
    bool OnReleaseEvent(const ReleaseEvent& event) override;
    bool OnDragEndEvent(const DragEvent& event) override;

    static void GetFuncKeyText(UI_KEYCODE key, std::string &tmpText);
    UIViewType GetViewType() const override
    {
        return UI_KEYBOARD_BUTTON;
    }

private:
    UI_KEYCODE keyCode_[MAX_KEY_VALUE_NUM];
    uint8_t keyNum_ = 0;
};
} // namespace OHOS
#endif // ENABLE_SOFT_KEYBOARD
#endif // GRAPHIC_LITE_UI_KEYBOARD_BUTTON_H
